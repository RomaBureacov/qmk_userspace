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

#pragma once

// Myriad boilerplate
#define MYRIAD_ENABLE

// Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed
#define DEBOUNCE 5

// /// Vial-specific settings

// // Increase the EEPROM size for layout options
// #define VIA_EEPROM_LAYOUT_OPTIONS_SIZE 2

// #define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #define RGB_MATRIX_KEYPRESSES

// // Default is 2, needed because keymap has 7 layers
// #define DYNAMIC_KEYMAP_LAYER_COUNT 8

// #define VIAL_KEYBOARD_UID {0xB3, 0x8D, 0x94, 0xDA, 0xB3, 0xD7, 0xDC, 0x3D}

// #define VIAL_UNLOCK_COMBO_ROWS { 3, 9 }
// #define VIAL_UNLOCK_COMBO_COLS { 2, 5 }

// #undef VIAL_TAP_DANCE_ENABLE

// audio features
#define AUDIO_PWM_DRIVER PWMD3
#define AUDIO_PWM_CHANNEL RP2040_PWM_CHANNEL_B

// #define AUDIO_ENABLE_TONE_MULTIPLEXING
// #define AUDIO_TONE_MULTIPLEXING_RATE_DEFAULT 10

#define MUSIC_MAP

#ifdef AUDIO_ENABLE
#define STARTUP_SONG SONG(GUARDIAN)
#define AUDIO_CLICKY
#define AUDIO_CLICKY_FREQ_DEFAULT 165
#define AUDIO_CLICKY_FREQ_RANDOMNESS 0.15f
#endif

// Caps word
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// Tap Dance configuration
#define TAPPING_TERM 175
// #define TAPPING_TERM_PER_KEY

// RGB configuration
// additional configuration options https://docs.qmk.fm/features/rgb_matrix#additional-configh-options
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_TRANSPORT_MIRROR // for reactive effects
// #define RGB_MATRIX_TIMEOUT 300000 // sleep in 5 minutes of inactivity
#define RGB_MATRIX_DEFAULT_SPD 60
// TODO: idle lighting?