/* Copyright 2023 splitkb.com <support@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// TODO: upload this keymap to github https://docs.qmk.fm/newbs_external_userspace

#include <stdint.h>
#include "action_layer.h"
#include "caps_word.h"
#include "config.h"
#include "keycodes.h"
#include "keymap_us.h"
#include "process_tap_dance.h"
#include "quantum.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"
#include "song_list.h"
// #include "vial.h"
#include QMK_KEYBOARD_H



enum layers {
    _QWERTY = 0,
    _ENGRAM,
    
    _LEFTY_TD,
    _LEFTY_TD_ENGRAM,
    _LEFTY_OS_MO,
    _LEFTY_OS_MO_ENGRAM,
    _GAMING_CONTROLS,
    _GAMING_EXTRAS,
    
    // always accessible
    _NUMPAD,
    _FUNCTIONS,
    _SYMBOL,
};

enum custom_keycodes {
  TG_TD = SAFE_RANGE, // toggle tap-dance lefty
};

enum {
  COMBO_BRACKET,
  COMBO_PAREN,
  COMBO_ANGLE,
  COMBO_CURLY,
};

const uint16_t PROGMEM combo_bracket[] =      {KC_LBRC, KC_AMPR, COMBO_END};
const uint16_t PROGMEM combo_parenthesis[] =  {KC_LPRN, KC_PIPE, COMBO_END};
const uint16_t PROGMEM combo_angle[] =        {KC_LT, KC_EQL, COMBO_END};
const uint16_t PROGMEM combo_curly[] =        {KC_LCBR, KC_QUES, COMBO_END};

combo_t key_combos[] = {
  [COMBO_BRACKET] = COMBO(combo_bracket, KC_RBRC),
  [COMBO_PAREN] = COMBO(combo_parenthesis, KC_RPRN),
  [COMBO_ANGLE] = COMBO(combo_angle, KC_GT),
  [COMBO_CURLY] = COMBO(combo_curly, KC_RCBR),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch(keycode) {
    case TG_TD:
      if (record->event.pressed) { // on key down
        if (IS_LAYER_ON_STATE(default_layer_state, _QWERTY)) layer_on(_LEFTY_TD);
        else if (IS_LAYER_ON_STATE(default_layer_state, _ENGRAM)) layer_on(_LEFTY_TD_ENGRAM);
      } else; // on key up
      break;
  }

  return true;
};

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
  switch(combo_index) {
    case COMBO_BRACKET:
    case COMBO_PAREN:
    case COMBO_ANGLE:
    case COMBO_CURLY:
      if (!layer_state_is(_SYMBOL))
        return false;
      break;
  }

  return true;
};

/* LAYER CHANGE CODE */

/* RGB lighting */
/*
 * LED layout (https://github.com/qmk/qmk_firmware/blob/79d3fb40a9cbbe13ea8d459f8fdcc805d3003555/keyboards/splitkb/elora/rev1/keyboard.json)
 *          (front)                     (back)
 *  ______,---.,---.______      __________  __________
 * | 3  4  5  || 11 10 9  |    ||2  1<-0  || 6->7  8| |
 * | 2  1  0  || 6  7  8  |    |V3->4  5  || 11 10<9V |
 *  ==========  ==========  OR  ======.___/\___,======
 *          (back)                      (front)
*/

bool game_mode_on = false;

float time_to_dive[][2] = SONG(HELLDIVER_INTRO); 
float dive_complete[][2] = SONG(HELLDIVER_OUTRO);

// for condition of caps word (won't work inside the layer_state_set_user as expected as it doesn't trigger a layer change)
void caps_word_set_user(bool active) {
  if (active) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS);
    rgb_matrix_sethsv_noeeprom(25, 255, 255); // orange
  } else layer_state_set_user(layer_state); // call back on the layer rgb matrix handler to figure out where it left off
}

layer_state_t layer_state_set_user(layer_state_t state) {
  if (IS_LAYER_ON_STATE(state, _GAMING_CONTROLS) && !game_mode_on) {
    PLAY_SONG(time_to_dive);
    game_mode_on = true;
  } else if (IS_LAYER_OFF_STATE(state, _GAMING_CONTROLS) && game_mode_on) {
    PLAY_SONG(dive_complete);
    game_mode_on = false;
  }

  if (host_keyboard_led_state().caps_lock) { // caps lock/word take precedence in RGB matrix mode
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS);
    rgb_matrix_sethsv_noeeprom(0, 255, 255); // red
  } else if (!is_caps_word_on()) { // rest is decided by the layer order, assuming caps word is not active (caps word takes precedence)
    switch (get_highest_layer(state | default_layer_state)) { // note: layer state and default layer states are separate entities
      case _QWERTY:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CYCLE_OUT_IN_DUAL);
        break;
      case _ENGRAM:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_RAINBOW_PINWHEELS);
        break;
      case _LEFTY_TD:
      case _LEFTY_OS_MO: 
      case _LEFTY_TD_ENGRAM:
      case _LEFTY_OS_MO_ENGRAM:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE);
        rgb_matrix_sethsv_noeeprom(165, 0xFF, 0xFF); // teal
        break;
      // case _NUMPAD:
      // case _FUNCTIONS:
      case _GAMING_CONTROLS:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CYCLE_UP_DOWN);
        break;
      // case _GAMING_EXTRAS:
      default:
        break;
    }
  }

  return state;
}

// set specified LEDs to rgb color
void setLED(unsigned int LED_indices[], unsigned int red, unsigned int green, unsigned int blue, unsigned int array_size) {
  red %= 256;
  blue %= 256;
  green %= 256;
  for (int index = 0; index < array_size; index++) {
    if (LED_indices[index] >= 12) return;
    rgb_matrix_set_color(LED_indices[index], red, green, blue);
  }
}

bool rgb_matrix_indicators_user() {
  if (IS_LAYER_ON(_NUMPAD))         setLED((unsigned int[]){9, 10, 11}, 255, 255, 0, 3); // yellow on bottom of right
  if (IS_LAYER_ON(_FUNCTIONS))      setLED((unsigned int[]){6, 7, 8}, 255, 255, 0, 3); // yellow on top of right
  if (IS_LAYER_ON(_GAMING_EXTRAS))  setLED((unsigned int[]){0, 1, 2, 3}, 100, 0, 255, 4); // purple on outer rim of left
  if (IS_LAYER_ON(_SYMBOL))          setLED((unsigned int[]){5}, 255, 255, 0, 1); // yellow on LED 5
  return true;
}


/* KEYMAPS */

// Aliases for readability
#define QWERTY   DF(_QWERTY)
#define ENGRAM   DF(_ENGRAM)
#define GM_CT    DF(_GAMING_CONTROLS)
#define GM_EX    DF(_GAMING_EXTRAS)

// Tap dance
enum {
  TD_BS_DL,
  TD_1_6, TD_2_7, TD_3_8, TD_4_9, TD_5_0,

  // Lefty qwerty
  TD_Q_Y, TD_W_U, TD_E_I, TD_R_O, TD_T_P,
  TD_A_H, TD_S_J, TD_D_K, TD_F_L, TD_G_g,
  TD_Z_N, TD_X_M, TD_C_c, TD_V_v, TD_B_b,

                                  TD_LB_RB,

  // Lefty engram
  TD_B_L, TD_Y_D, TD_O_W, TD_U_V, TD_z_Z,
  TD_C_H, TD_I_T, TD_E_S, TD_A_N, TD_q_Q,
  TD_G_R, /* TD_X_M, */ TD_J_F, TD_K_P, TD_CLN_APS,

  // extras
  TD_HM_PU, TD_EN_PD, // page up/down, home/end

  TD_VD_VU, // volume

  TD_EQL, // PEQL and EQL

  // symbol layer
  TD_PAREN,
  TD_CRLBR,
  TD_SQRBR,
  TD_ANGBR,
  TD_SLS,
  TD_TERN,
  
};

tap_dance_action_t tap_dance_actions[] = {
  [TD_BS_DL] = ACTION_TAP_DANCE_DOUBLE(KC_BSPC, KC_DEL),
  [TD_1_6] = ACTION_TAP_DANCE_DOUBLE(KC_1, KC_6),
  [TD_2_7] = ACTION_TAP_DANCE_DOUBLE(KC_2, KC_7),
  [TD_3_8] = ACTION_TAP_DANCE_DOUBLE(KC_3, KC_8),
  [TD_4_9] = ACTION_TAP_DANCE_DOUBLE(KC_4, KC_9),
  [TD_5_0] = ACTION_TAP_DANCE_DOUBLE(KC_5, KC_0),

  // Lefty tap dance for qwerty
  [TD_Q_Y] = ACTION_TAP_DANCE_DOUBLE(KC_Q, KC_Y),
  [TD_W_U] = ACTION_TAP_DANCE_DOUBLE(KC_W, KC_U),
  [TD_E_I] = ACTION_TAP_DANCE_DOUBLE(KC_E, KC_I), 
  [TD_R_O] = ACTION_TAP_DANCE_DOUBLE(KC_R, KC_O),
  [TD_T_P] = ACTION_TAP_DANCE_DOUBLE(KC_T, KC_P),

  [TD_A_H] = ACTION_TAP_DANCE_DOUBLE(KC_A, KC_H),
  [TD_S_J] = ACTION_TAP_DANCE_DOUBLE(KC_S, KC_J),
  [TD_D_K] = ACTION_TAP_DANCE_DOUBLE(KC_D, KC_K),
  [TD_F_L] = ACTION_TAP_DANCE_DOUBLE(KC_F, KC_L),
  [TD_G_g] = ACTION_TAP_DANCE_DOUBLE(KC_G, KC_SCLN),
  
  [TD_Z_N] = ACTION_TAP_DANCE_DOUBLE(KC_Z, KC_N),
  [TD_X_M] = ACTION_TAP_DANCE_DOUBLE(KC_X, KC_M),
  [TD_C_c] = ACTION_TAP_DANCE_DOUBLE(KC_C, KC_COMM),
  [TD_V_v] = ACTION_TAP_DANCE_DOUBLE(KC_V, KC_DOT),
  [TD_B_b] = ACTION_TAP_DANCE_DOUBLE(KC_B, KC_SLSH),


  // Lefty tap dance for engram
  [TD_B_L] = ACTION_TAP_DANCE_DOUBLE(KC_B, KC_L),
  [TD_Y_D] = ACTION_TAP_DANCE_DOUBLE(KC_Y, KC_D),
  [TD_O_W] = ACTION_TAP_DANCE_DOUBLE(KC_O, KC_W), 
  [TD_U_V] = ACTION_TAP_DANCE_DOUBLE(KC_U, KC_V),
  [TD_z_Z] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_Z),

  [TD_C_H] = ACTION_TAP_DANCE_DOUBLE(KC_C, KC_H),
  [TD_I_T] = ACTION_TAP_DANCE_DOUBLE(KC_I, KC_T),
  [TD_E_S] = ACTION_TAP_DANCE_DOUBLE(KC_E, KC_S),
  [TD_A_N] = ACTION_TAP_DANCE_DOUBLE(KC_A, KC_N),
  [TD_q_Q] = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_Q),
  
  [TD_G_R] = ACTION_TAP_DANCE_DOUBLE(KC_G, KC_R),
  // [TD_X_M] = ACTION_TAP_DANCE_DOUBLE(KC_X, KC_M),
  [TD_J_F] = ACTION_TAP_DANCE_DOUBLE(KC_J, KC_F),
  [TD_K_P] = ACTION_TAP_DANCE_DOUBLE(KC_K, KC_P),
  [TD_CLN_APS] = ACTION_TAP_DANCE_DOUBLE(KC_SCLN, KC_QUOT),


  [TD_LB_RB] = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_RBRC),

  // extra
  [TD_HM_PU] = ACTION_TAP_DANCE_DOUBLE(KC_HOME, KC_PGUP),
  [TD_EN_PD] = ACTION_TAP_DANCE_DOUBLE(KC_END, KC_PGDN),

  [TD_VD_VU] = ACTION_TAP_DANCE_DOUBLE(KC_VOLD, KC_VOLU),

  [TD_EQL] = ACTION_TAP_DANCE_DOUBLE(KC_PEQL, KC_EQL),

};

// Swap hands
// https://docs.qmk.fm/features/swap_hands
// const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
//   {{11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}},
//   {{11, 1}, {10, 1}, {9, 1}, {8, 1}, {7, 1}, {6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}},
//   {{11, 2}, {10, 2}, {9, 2}, {8, 2}, {7, 2}, {6, 2}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}},
// };

// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * Standard, default layout
 * Emulation of a standard full keyboard
 * 
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |      |  0   |  |  0   |      |   6  |   7  |   8  |   9  |   0  | Bksp   |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |      |  0   |  |  0   |      |   Y  |   U  |   I  |   O  |   P  |  | \   |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | LCtrl  |   A  |   S  |   D  |   F  |   G  |      |  0   |  |  0   |      |   H  |   J  |   K  |   L  | ;  : |  ' "   |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  | OSL  |  |  MO  |  ] } |   N  |   M  | ,  < | . >  | /  ? | Enter  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Home/| LGUI | LAlt | Space|  MO  |  |  MO  | Space| Rept | Menu | End/ |
 *                        | PGUP |      |      |      |      |  |NUMPAD|      |      |      | PGDN |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | 0    | 0    | 0     | 0    |      | 0    |                 | 0    | 0    | 0    | 0     |      | 0    |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_QWERTY] = LAYOUT_myr(
      KC_ESC  , KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,               KC_0   ,       KC_0       ,          KC_6 ,  KC_7 ,  KC_8 ,   KC_9 ,  KC_0 , KC_BSPC,
      KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,               KC_0   ,       KC_0       ,          KC_Y ,  KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSLS,
      KC_LCTL,  KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,               KC_0   ,       KC_0       ,          KC_H ,  KC_J ,  KC_K ,   KC_L ,KC_SCLN, KC_QUOT,
      KC_LSFT , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC,OSL(_LEFTY_OS_MO),MO(_FUNCTIONS), KC_RBRC, KC_N ,  KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_ENT,
                            TD(TD_HM_PU), KC_LGUI, KC_LALT, KC_SPC , MO(_SYMBOL)  ,    MO(_NUMPAD)   , KC_SPC ,QK_REP, KC_APP, TD(TD_EN_PD),

      KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0   ,                            KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0
    ),

/*
 * Base Layer: Engram
 *
 * alternative default layout
 * https://engram.dev/
 * 
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |      |  0   |  |  0   |      |   6  |   7  |   8  |   9  |   0  | Bksp   |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  Tab   |   B  |   Y  |   O  |   U  | , <  |      |  0   |  |  0   |      | | \  |   L  |   D  |   W  |   V  |    Z   |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | LCtrl  |   C  |   I  |   E  |   A  | . >  |      |  0   |  |  0   |      | ' "  |   H  |   T  |   S  |   N  |    Q   |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | LShift |   G  |   X  |   J  |   K  | ; :  | [ {  | OSL  |  |  MO  |  ] } | / ?  |   R  |   M  |   F  |   P  | Enter  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Home/| LGUI | LAlt | Space|  MO  |  |  MO  | Space| Rept | Menu | End/ |
 *                        | PGUP |      |      |      |      |  |NUMPAD|      |      |      | PGDN |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * | 0    | 0    | 0     | 0    |      | 0    |                 | 0    | 0    | 0    | 0     |      | 0    |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
    [_ENGRAM] = LAYOUT_myr(
      KC_ESC  , KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,               KC_0   ,       KC_0       ,          KC_6 ,  KC_7 ,  KC_8 ,   KC_9 ,  KC_0 , KC_BSPC,
      KC_TAB  , KC_B ,  KC_Y   ,  KC_O  ,   KC_U ,  KC_DOT,               KC_0   ,       KC_0       ,        KC_BSLS,  KC_L ,  KC_D ,   KC_W ,  KC_V , KC_Z,
      KC_LCTL , KC_C ,  KC_I   ,  KC_E  ,   KC_A , KC_COMM,               KC_0   ,       KC_0       ,        KC_QUOT,  KC_H ,  KC_T ,   KC_S ,  KC_N , KC_Q,
      KC_LSFT , KC_G ,  KC_X   ,  KC_J  ,   KC_K , KC_SCLN, KC_LBRC, OSL(_LEFTY_OS_MO_ENGRAM),    MO(_FUNCTIONS), KC_RBRC, KC_SLSH ,  KC_R , KC_M, KC_F,KC_P, KC_ENT,
                            TD(TD_HM_PU), KC_LGUI, KC_LALT, KC_SPC , MO(_SYMBOL)  ,    MO(_NUMPAD)   , KC_SPC ,QK_REP, KC_APP, TD(TD_EN_PD),

      KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0   ,                            KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0
    ),

// TODO: maybe it might be a good idea to define a left-handed numberpad?
/* 
 * Layer: numpad
 *
 * Idea: extend the numbers into two rows across where the right hand would naturally be
 * For numpad actions
 *
 * Note: KC_PEQL is not handled by most, if not all, applications
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |      |      |      |      |      |      |      |  |      |      | CAPSL| NUML | SCRLL|  (   |  )   | Delete |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |      |      |      |      |      |      |      |  |      |      |  ^   |  NP5 |  NP6 |  NP7 | NP8  | NP9    |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |      |      |      |      |      |      |      |  |      |      |  NP. |  NP0 |  NP1 |  NP2 | NP3  | NP4    |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |  TRNS  |      |      |      |      |      |      |      |  |      |      | =/=+ |  NP- |  NP+ |  NP/ | NP*  | NPENT  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      | TRNS | TRNS |      |      |  | TRNS | Left |  Up  | Down |Right |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,----------------------------.      ,------.                 ,----------------------------.      ,------.
 * |      |      |       |      |      |      |                 |      |      |       |      |      |      |
 * `----------------------------'      `------'                 `----------------------------'      '------'
 */
     [_NUMPAD] = LAYOUT_myr(
      _______, _______, _______, _______, _______, _______,          _______, _______,          KC_CAPS  , KC_NUM , KC_SCRL, KC_LPRN, KC_RPRN, KC_DEL,
      KC_TRNS, _______, _______, _______, _______, _______,          _______, _______,          KC_CIRC  , KC_P5  , KC_P6  , KC_P7  , KC_P8  , KC_P9,
      KC_TRNS, _______, _______, _______, _______, _______,          _______, _______,          KC_PDOT  , KC_P0  , KC_P1  , KC_P2  , KC_P3  , KC_P4,
      KC_TRNS, _______, _______, _______, _______, _______, _______, _______, _______, _______,TD(TD_EQL), KC_PMNS, KC_PPLS, KC_PSLS, KC_PAST, KC_PENT,
                                 _______, KC_TRNS, KC_TRNS, _______, _______, KC_TRNS, KC_LEFT, KC_UP  , KC_DOWN, KC_RIGHT,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

 /*
 * Layer: functions alt 01
 * 
 * Idea: mimic the numpad alt 01
 * 
 * Layers to house all the functions and settings (almost like a "configuration menu" for the keyboard)
 * 
 * Double-tap lefty activated through here
 * 
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        | DF QW| DF EN|      |      |      |      |      |  |      |      | INSR |  F10 |  F11 |  F12 | VOL+-| PL/PS  |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      | SCLK |  F5  |  F6  |  F7  | F8   | F9     |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      | NMLK | MUST |  F1  |  F2  | F3   | F4     |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      | TG   |  | TRNS | PSCR | CPLK | MUSNX| CKTG | CKRST| CKDN | CKUP   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  | RGB  | RGB  |      |      | RGBT |
 *                        |      |      |      |      |      |  | M P  | M B  |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_FUNCTIONS] = LAYOUT_myr(
      _______, _______, _______, _______, _______, _______,             _______   , _______,          KC_INS , KC_F10 , KC_F11 , KC_F12  , TD(TD_VD_VU), KC_MPLY,
      _______, _______, _______, _______, _______, _______,             _______   , _______,          KC_SCRL, KC_F5  , KC_F6  , KC_F7   , KC_F8  , KC_F9  ,
      _______, _______, _______, _______, _______, _______,             _______   , _______,          KC_NUM , MU_TOGG, KC_F1  , KC_F2   , KC_F3  , KC_F4 ,
      _______, _______, _______, _______, _______, _______, _______,    TG_TD     , KC_TRNS, KC_PSCR, KC_CAPS, MU_NEXT, CK_TOGG, CK_RST  , CK_DOWN, CK_UP ,
                                 _______, _______, _______, _______,    _______   , RGB_M_P, RGB_M_B, _______, _______, RGB_TOG,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Layer: lefty
 *
 * For one-handed typing on left board
 * Tap for main key, double-tap for the alternate key
 */

/* QWERTY
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |Bksp/Del|  1/6 |  2/7 |  3/8 |  4/9 |  5/0 |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |  q/y |  w/u |  e/i |  r/o |  t/p |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |  a/h |  s/j |  d/k |  f/l |  g/' |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |  TRNS  |  z/n |  x/m |  c/, |  v/. |  b// |[{/}] |  TG  |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      | TRNS | TRNS | TRNS |Enter |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */ 
    // tap-dance lefty qwerty
    [_LEFTY_TD] = LAYOUT_myr(
      TD(TD_BS_DL), TD(TD_1_6), TD(TD_2_7), TD(TD_3_8), TD(TD_4_9), TD(TD_5_0),                     _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_Q_Y), TD(TD_W_U), TD(TD_E_I), TD(TD_R_O), TD(TD_T_P),                     _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_A_H), TD(TD_S_J), TD(TD_D_K), TD(TD_F_L), TD(TD_G_g),                     _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_Z_N), TD(TD_X_M), TD(TD_C_c), TD(TD_V_v), TD(TD_B_b), TD(TD_LB_RB), TG(_LEFTY_TD), _______, _______, _______, _______, _______, _______, _______, _______,
                                            _______   , KC_TRNS   , KC_TRNS   , KC_TRNS     , KC_ENT       , _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),
    // one-shot/MO lefty qwerty
    [_LEFTY_OS_MO] = LAYOUT_myr(
      TD(TD_BS_DL), KC_6 ,  KC_7 ,  KC_8 ,   KC_9 ,  KC_0 ,          _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_Y ,  KC_U ,  KC_I ,   KC_O ,  KC_P ,          _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_H ,  KC_J ,  KC_K ,   KC_L ,KC_SCLN,          _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_N ,  KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_RBRC, TG(_LEFTY_OS_MO), _______, _______, _______, _______, _______, _______, _______, _______,
                                  _______, KC_TRNS,KC_TRNS, KC_TRNS, KC_ENT , _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/* 
 * Engram lefty
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |Bksp/Del|  1/6 |  2/7 |  3/8 |  4/9 |  5/0 |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |  b/l |  y/d |  o/w |  u/v |  ,/z |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |  TRNS  |  c/h |  i/t |  e/s |  a/n |  ./q |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |  TRNS  |  g/r |  x/m |  j/f |  k/p |  ;/' |[{/}] |  TG  |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      | TRNS | TRNS | TRNS |Enter |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */

    // tap-dance lefty, engram
    [_LEFTY_TD_ENGRAM] = LAYOUT_myr(
      TD(TD_BS_DL), TD(TD_1_6), TD(TD_2_7), TD(TD_3_8), TD(TD_4_9), TD(TD_5_0)    ,                _______             , _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_B_L), TD(TD_Y_D), TD(TD_O_W), TD(TD_U_V), TD(TD_z_Z)    ,                _______             , _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_C_H), TD(TD_I_T), TD(TD_E_S), TD(TD_A_N), TD(TD_q_Q)    ,                _______             , _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS     , TD(TD_G_R), TD(TD_X_M), TD(TD_J_F), TD(TD_K_P), TD(TD_CLN_APS), TD(TD_LB_RB) , TG(_LEFTY_TD_ENGRAM), _______, _______, _______, _______, _______, _______, _______, _______,
                                            _______   , KC_TRNS   , KC_TRNS       , KC_TRNS      , KC_ENT              , _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),
    // one-shot/MO lefty engram
    [_LEFTY_OS_MO_ENGRAM] = LAYOUT_myr(
      TD(TD_BS_DL), KC_6 ,  KC_7 ,  KC_8 ,   KC_9 ,  KC_0 ,          _______                , _______,        _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_L ,  KC_D ,  KC_W ,   KC_V ,  KC_Z ,          _______                , _______,        _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_H ,  KC_T ,  KC_S ,   KC_N ,  KC_Q ,          _______                , _______,        _______, _______, _______, _______, _______, _______,
      KC_TRNS     , KC_R ,  KC_M ,  KC_F ,   KC_P ,KC_QUOT, KC_RBRC, TG(_LEFTY_OS_MO_ENGRAM), _______, _______, _______, _______, _______, _______, _______, _______,
                                  _______, KC_TRNS,KC_TRNS, KC_TRNS, KC_ENT                 , _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Layer: symbol pad
 *                        /- Combo col -\
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |  ` ~   |  *   |  #   |  [   |  &   |      |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |  +   |  @   |  (   |  |   |      |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | RCTRL  |  -   |  !   |  <   | = +  |  ; : |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | RSHFT  |  /   |  %   |  {   |  ?   |  :   | PRSCR| TG GM|  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | LEFT | DOWN |  UP  | RGHT | TRNS |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_SYMBOL] = LAYOUT_myr(
      KC_GRV , KC_ASTR, KC_HASH, KC_LBRC, KC_AMPR, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
      _______, KC_PLUS, KC_AT  , KC_LPRN, KC_PIPE, _______,          _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_RCTL, KC_MINS, KC_EXLM, KC_LT  , KC_EQL , KC_SCLN,          _______, _______,          _______, _______, _______, _______, _______, _______,
      KC_RSFT, KC_SLSH, KC_PERC, KC_LCBR, KC_QUES, KC_COLN, KC_PSCR, TG(GM_CT), _______, _______, _______, _______, _______, _______, _______, _______,
                                 KC_LEFT, KC_DOWN, KC_UP  , KC_RIGHT, KC_TRNS, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

// TODO: consider making game-specific controls, or making it more abstract
//        further: played helldivers, these control schemes basically do not work, e placement is poor and no good
//                 push-to-talk setup
/*
 * Layer: main gaming controls
 * 
 * enter key essentially acts as a disable for the gaming layer to allow for typing in-game
 * (layouts prefer enter key for text chat over enter)
 * T key is intended for push-to-talk
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * | TRNS   |  4   |  5   |  6   |  7   |  8   |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | TRNS   |  3   |  Q   |  E   |  R   |  9   |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * | TRNS   |  2   |  A   |  W   |  D   |  0   |      |      |  |      |      |      | LFT  | UP   | RGT  |      |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * | TRNS   |  1   |  Z   |  S   |  C   |  V   |  R   |  TG  |  |      |      |      |      | DN   |      |      | TG     |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |  T   |  E   | TRNS | TRNS |  MO  |  |      | LFT  |  UP  |  DN  | RGT  |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_GAMING_CONTROLS] = LAYOUT_myr(
      KC_TRNS, KC_4   , KC_5   , KC_6   , KC_7   , KC_8   ,          _______  , _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS, KC_3   , KC_Q   , KC_E   , KC_R   , KC_9   ,          _______  , _______,          _______, _______, _______, _______, _______, _______,
      KC_TRNS, KC_2   , KC_A   , KC_W   , KC_D   , KC_0   ,          _______  , _______,          _______, KC_LEFT, KC_UP  ,KC_RIGHT, _______, _______,
      KC_TRNS, KC_1   , KC_Z   , KC_S   , KC_C   , KC_V   , KC_R   , TG(GM_CT), _______, _______, _______, _______, KC_DOWN, _______, _______, TG(GM_CT),
                                 KC_T   , KC_E   , KC_TRNS, KC_TRNS, MO(GM_EX), _______, KC_LEFT, KC_UP  , KC_DOWN,KC_RIGHT,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),

/*
 * Layers: extra gaming controls
 *
 * ,-------------------------------------------.      ,------.  ,------.      ,-------------------------------------------.
 * |        |  F4  |  F5  |  F6  |  F7  |  F8  |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |  F3  |  F   |  G   |  T   |  F9  |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |------|  |------|      |------+------+------+------+------+--------|
 * |        |  F2  |  LFT |  UP  |  RGT |  F10 |      |      |  |      |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+------+------|  |------|------+------+------+------+------+------+--------|
 * |        |  F1  |  X   |  DN  | B    |  F11 |  F12 |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      | TRNS |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 *
 * ,-----------------------------.      ,------.                ,---------------------------.      ,------.
 * |        |      |      |      |      |      |                |      |      |      |      |      |      |
 * `-----------------------------'      `------'                `---------------------------'      '------'
 */
    [_GAMING_EXTRAS] = LAYOUT_myr(
      _______, KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  ,          _______, _______,          _______, _______, _______, _______, _______, _______,
      _______, KC_F3  , KC_F   , KC_G   , KC_T   , KC_F9  ,          _______, _______,          _______, _______, _______, _______, _______, _______,
      _______, KC_F2  , KC_LEFT, KC_UP  , KC_RIGHT, KC_F10 ,         _______, _______,          _______, _______, _______, _______, _______, _______,
      _______, KC_F1  , KC_X   , KC_DOWN, KC_B   , KC_F11 , KC_F12 , _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, KC_TRNS, _______, _______, _______, _______, _______,

      _______, _______, _______, _______,          _______,                   _______, _______, _______, _______,          _______
    ),
};

/* The default OLED and rotary encoder code can be found at the bottom of qmk_firmware/keyboards/splitkb/elora/rev1/rev1.c
 * These default settings can be overriden by your own settings in your keymap.c
 * DO NOT edit the rev1.c file; instead override the weakly defined default functions by your own.
 */

/* DELETE THIS LINE TO UNCOMMENT (1/2)
#ifdef OLED_ENABLE
bool oled_task_user(void) {
  // Your code goes here
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
  // Your code goes here
}
#endif
DELETE THIS LINE TO UNCOMMENT (2/2) */

// Vial-specific encoder code

// #if defined(ENCODER_MAP_ENABLE)
// bool encoder_update_user(uint8_t index, bool clockwise) {
//   return false;
// }

// const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
//     [0] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [1] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [2] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [3] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [4] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [5] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     },
//     [6] = {
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_LEFT, KC_RIGHT),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_PGUP, KC_PGDN),
//       ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
//     }
// };
// #endif

// Music map
const uint8_t music_map[MATRIX_ROWS][MATRIX_COLS] = LAYOUT_myr(
      0, 30, 31, 32, 33, 34,          0, 0,           35, 36, 37, 38, 39, 0,
      0, 20, 21, 22, 23, 24,          0, 0,           25, 26, 27, 28, 29, 0,
      0, 10, 11, 12, 13, 14,          0, 0,           15, 16, 17, 18, 19, 0,
      0, 0,  1,  2,  3,  4,     0, 0,       0, 0,     5,  6,  7,  8,  9,  0,
                         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,

      0, 0, 0, 0,          0,                   0, 0, 0, 0,          0
);