#pragma once

#define IS_ASCII(x)  (((unsigned char)(x) > 0)  && ((unsigned char)(x) < 128))
#define IS_CHAR(x)   (((unsigned char)(x) > 32) && ((unsigned char)(x) < 128))
#define IS_LETTER(x) (((unsigned char)(x) > 96) && ((unsigned char)(x) < 123))

enum
{
	ESC = 128,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	LSHF,
	LCTL,
	LALT,
	RSHF,
	RCTL,
	RALT,

	CAPL,
	NUML,
	SCRL
};