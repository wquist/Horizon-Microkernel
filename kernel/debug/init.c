#include "init.h"
#include <arch.h>
#include <util/compare.h>
#include <memory.h>

static size_t cursor_x = 0, cursor_y = 0;
static console_char_t* buffer = CONSOLE_ADDR;

static void scroll();

//! Initialize all formats of debugging.
/*! Any debug_* or d* function can be called after this. */
void debug_init()
{
	/* Set the entire screen to spaces instead of 0,
	   since 0 might have extra color info or something
	 */
	size_t len = CONSOLE_WIDTH * CONSOLE_HEIGHT;
	for (size_t i = 0; i != len; ++i)
		buffer[i] = console_format(' ');
}

//! Redirect stdio's character stream directory to video memory.
int putchar(char c)
{
	switch (c)
	{
		case '\b':
			if (cursor_x) --cursor_x;
			break;
		case '\t':
			cursor_x = (cursor_x + 4) & -4;
			break;
		case '\r':
			cursor_x = 0;
			break;
		case '\n':
			cursor_x = 0; ++cursor_y;
			break;
		default: break;
	}

	if (c >= ' ')
	{
		console_char_t* loc = &(buffer[cursor_y * CONSOLE_WIDTH + cursor_x]);
		*loc = console_format(c);

		++cursor_x;
	}

	if (cursor_x >= CONSOLE_WIDTH)
	{
		cursor_x = 0;
		++cursor_y;
	}

	if (cursor_y >= CONSOLE_HEIGHT)
		scroll();
}

// Copy each line of the buffer to the previous line, and clear the last line.
void scroll()
{
	size_t i;
	size_t last_row   = CONSOLE_WIDTH * (CONSOLE_HEIGHT-1);
	size_t last_index = CONSOLE_WIDTH *  CONSOLE_HEIGHT;

	for (i = 0; i != last_row;   ++i) buffer[i] = buffer[i + CONSOLE_WIDTH];
	for (;      i != last_index; ++i) buffer[i] = console_format(' ');

	--cursor_y;
}
