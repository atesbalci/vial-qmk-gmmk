/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
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

#include QMK_KEYBOARD_H

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
enum custom_layers {
    _BL,
    _FL,
};

enum custom_keycodes {
    PERKEY_TOG = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  /* Keymap _BL: Base Layer (Default Layer)
   */
[_BL] = LAYOUT(
  KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_GRV,
  KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,
  KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT,             KC_PGUP,
  KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_PGDN,
  KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  MO(1),    KC_LEFT,  KC_DOWN,  KC_RGHT),

  /* Keymap _FL: Function Layer
   */
[_FL] = LAYOUT(
   KC_GRV,    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,   KC_F10,   KC_F11,   KC_F12,   _______,  KC_INS,
  _______,  _______,  RGB_VAI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_PSCR,  KC_SCRL,  KC_PAUS,   _______,  _______,
  _______,  _______,  RGB_VAD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MUTE,             KC_HOME,
  PERKEY_TOG, RGB_HUI, RGB_HUD, RGB_SPD,  RGB_SPI,  _______,  _______,  _______,  RGB_RMOD, RGB_MOD,  _______,  KC_MPLY,             KC_VOLU,  KC_END,
  _______,  _______,  _______,                                QK_BOOT,                                _______,  _______,  KC_MPRV,   KC_VOLD,  KC_MNXT)
};

typedef union {
    uint32_t raw;
    struct {
        bool per_key_active;
        bool initialized;
    };
} user_config_t;

const uint8_t escKeyIndex = 0;
const uint8_t capsKeyIndex = 30;
const uint8_t capsBlinkInterval = 250;

const char numberKeys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const uint8_t numberKeyCount = 10;

const char alphabetKeys[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                             31, 32, 33, 34, 35, 36, 37, 38, 39,
                             45, 46, 47, 48, 49, 50, 51};
const uint8_t alphabetKeyCount = 26;

const char miscCharKeys[] = {11, 12, 13, 14, 15, 26, 27, 28, 29, 40, 41, 42, 52, 53, 54, 61};
const uint8_t miscCharKeyCount = 16;

const char modKeys[] = {44, 55, 58, 59, 60, 62, 63};
const uint8_t modKeyCount = 7;

const char arrowKeys[] = {56, 64, 65, 66, 43, 57};
const uint8_t arrowKeyCount = 6;

uint16_t key_timer;
user_config_t user_config;

void setKeySetColor(const char *keys, uint16_t keyCount, uint16_t r, uint16_t g, uint16_t b) {
    for (uint16_t i = 0; i < keyCount; i++) {
        rgb_matrix_set_color(keys[i], r, g, b);
    }
}

void keyboard_post_init_user(void) {
    key_timer = timer_read();
    user_config.raw = eeconfig_read_user();
    if (!user_config.initialized) {
        rgb_matrix_mode(RGB_MATRIX_BAND_SAT);
        user_config.per_key_active = true;
        user_config.initialized = true;
        eeconfig_update_user(user_config.raw);
    }
}

float getBreathingEffectMultiplier(void) {    
    float value = (((float) ((timer_elapsed(key_timer) % (capsBlinkInterval * 2)) - capsBlinkInterval)) / ((float) capsBlinkInterval));
    value = value < 0 ? -value : value;
    return value;
}

void multiplyColor(uint8_t* color, float multiplier) {
    for (uint8_t i = 0; i < 3; i++) color[i] = (uint8_t) (color[i] * multiplier);
}

void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (user_config.per_key_active) {
        setKeySetColor(numberKeys, numberKeyCount, RGB_YELLOW);
        setKeySetColor(miscCharKeys, miscCharKeyCount, RGB_MAGENTA);
        setKeySetColor(modKeys, modKeyCount, RGB_GREEN);
        setKeySetColor(arrowKeys, arrowKeyCount, RGB_CYAN);
        rgb_matrix_set_color(escKeyIndex, RGB_RED);
    }

    if (g_led_config.flags[capsKeyIndex] & LED_FLAG_KEYLIGHT) {
        if (host_keyboard_led_state().caps_lock) {
            float value = getBreathingEffectMultiplier();
            uint8_t capsLockColor[] = {RGB_YELLOW};
            multiplyColor(capsLockColor, value);
            rgb_matrix_set_color(capsKeyIndex, capsLockColor[0], capsLockColor[1], capsLockColor[2]);

            uint8_t alphabetColor[] = {RGB_CYAN};
            multiplyColor(alphabetColor, value);
            setKeySetColor(alphabetKeys, alphabetKeyCount, alphabetColor[0], alphabetColor[1], alphabetColor[2]);
        } else {
            rgb_matrix_set_color(capsKeyIndex, RGB_OFF);
        }
    }

    uint8_t layer = get_highest_layer(layer_state);
    if (layer > 0) {
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];
                int keyCode = keymap_key_to_keycode(layer, (keypos_t){col,row});

                if (keyCode > KC_TRNS) {
                    rgb_matrix_set_color(index, RGB_RED);
                }
            }
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case PERKEY_TOG:
            if (!record->event.pressed) {
                user_config.per_key_active = !user_config.per_key_active;
                eeconfig_update_user(user_config.raw);
            }
            return false;
        default:
            return true;
    }
}
