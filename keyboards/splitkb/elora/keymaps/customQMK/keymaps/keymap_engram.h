#pragma once

const uint16_t PROGMEM layout_engram[][MATRIX_ROWS][MATRIX_COLS] = {
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
      KC_LSFT , KC_G ,  KC_X   ,  KC_J  ,   KC_K , KC_SCLN, KC_LBRC, OSL(_LEFTY_OS_MO),    MO(_FUNCTIONS), KC_RBRC, KC_SLSH ,  KC_R , KC_M, KC_F,KC_P, KC_ENT,
                            TD(TD_HM_PU), KC_LGUI, KC_LALT, KC_SPC , MO(_TEMP1)  ,    MO(_NUMPAD)   , KC_SPC ,QK_REP, KC_APP, TD(TD_EN_PD),

      KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0   ,                            KC_0   , KC_0   , KC_0   , KC_0   ,    KC_0
    )
};