#pragma once

#include "common.h"

typedef enum input_key input_key;

b8 input_get_key(input_key key);

typedef enum {
	INPUT_KEY_SPACE = 32,
	INPUT_KEY_APOSTRAPHE = 39,
	INPUT_KEY_COMMA = 44,
	INPUT_KEY_MINUS = 45,
	INPUT_KEY_EQUAL = 61,
	INPUT_KEY_SEMICOLON = 59,
	INPUT_KEY_PERIOD = 46,
	INPUT_KEY_SLASH = 47,

	INPUT_KEY_LBRACKET = 91,
	INPUT_KEY_RBRACKET = 93,
	INPUT_KEY_BACKSLASH = 92,
	INPUT_KEY_TILDE = 96,

	INPUT_KEY_ESCAPE = 256,
	INPUT_KEY_ENTER,
	INPUT_KEY_TAB,
	INPUT_KEY_BACKSPACE,
	INPUT_KEY_INSERT,
	INPUT_KEY_DELETE,
	INPUT_KEY_RIGHT,
	INPUT_KEY_LEFT,
	INPUT_KEY_DOWN,
	INPUT_KEY_UP,
	INPUT_KEY_PGUP,
	INPUT_KEY_PGDOWN,
	INPUT_KEY_HOME,
	INPUT_KEY_END,

	INPUT_KEY_PRNTSCRN = 283,
	INPUT_KEY_PAUSE,

	INPUT_KEY_0 = 48,
	INPUT_KEY_1,
	INPUT_KEY_2,
	INPUT_KEY_3,
	INPUT_KEY_4,
	INPUT_KEY_5,
	INPUT_KEY_6,
	INPUT_KEY_7,
	INPUT_KEY_8,
	INPUT_KEY_9,
	INPUT_KEY_A = 65,
	INPUT_KEY_B,
	INPUT_KEY_C,
	INPUT_KEY_D,
	INPUT_KEY_E,
	INPUT_KEY_F,
	INPUT_KEY_G,
	INPUT_KEY_H,
	INPUT_KEY_I,
	INPUT_KEY_J,
	INPUT_KEY_K,
	INPUT_KEY_L,
	INPUT_KEY_M,
	INPUT_KEY_N,
	INPUT_KEY_O,
	INPUT_KEY_P,
	INPUT_KEY_Q,
	INPUT_KEY_R,
	INPUT_KEY_S,
	INPUT_KEY_T,
	INPUT_KEY_U,
	INPUT_KEY_V,
	INPUT_KEY_W,
	INPUT_KEY_X,
	INPUT_KEY_Y,
	INPUT_KEY_Z,

	INPUT_KEY_LSHIFT = 340,
	INPUT_KEY_LCONTROL,
	INPUT_KEY_LALT,
	INPUT_KEY_RSHIFT = 344,
	INPUT_KEY_RCONTROL,
	INPUT_KEY_RALT
} input_key;