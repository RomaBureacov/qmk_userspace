#pragma once

#include "action_layer.h"
#include "caps_word.h"
#include "config.h"
#include "keycodes.h"
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
    _TEMP1, // no ideas yet
    _GAMING_CONTROLS,
    _GAMING_EXTRAS,
    
    // always accessible
    _NUMPAD,
    _FUNCTIONS,
};

enum custom_keycodes {
  TG_TD = SAFE_RANGE, // toggle tap-dance lefty
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

};

// Aliases for readability
#define QWERTY   DF(_QWERTY)
#define ENGRAM   DF(_ENGRAM)
#define GM_CT    DF(_GAMING_CONTROLS)
#define GM_EX    DF(_GAMING_EXTRAS)