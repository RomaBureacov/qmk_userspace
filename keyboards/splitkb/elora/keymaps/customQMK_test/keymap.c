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

#include "keymaps/config.h"

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
  if (IS_LAYER_ON(_NUMPAD)) setLED((unsigned int[]){9, 10, 11}, 255, 255, 0, 3); // yellow on bottom of right
  if (IS_LAYER_ON(_FUNCTIONS))  setLED((unsigned int[]){6, 7, 8}, 255, 255, 0, 3); // yellow on top of right
  if (IS_LAYER_ON(_GAMING_EXTRAS)) setLED((unsigned int[]){0, 1, 2, 3}, 100, 0, 255, 4); // purple on outer rim of left
  if (IS_LAYER_ON(_TEMP1)) setLED((unsigned int[]){5}, 255, 255, 0, 1); // yellow on LED 5
  return true;
}


/* KEYMAPS */

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

#include "components/component_engram_default.c"


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

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