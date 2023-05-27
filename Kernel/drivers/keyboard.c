#include "text.h"

#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <video.h>

static const uint8_t scancodes[][2] = {
	{ 0, 0 },       { KC_ESC, KC_ESC }, { '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' },  { '5', '%' },
	{ '6', '^' },   { '7', '&' },       { '8', '*' }, { '9', '(' }, { '0', ')' }, { '-', '_' },  { '=', '+' },
	{ '\b', '\b' }, { '\t', '\t' },     { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' }, { 'r', 'R' },  { 't', 'T' },
	{ 'y', 'Y' },   { 'u', 'U' },       { 'i', 'I' }, { 'o', 'O' }, { 'p', 'P' }, { '[', '{' },  { ']', '}' },
	{ '\n', '\n' }, { 0, 0 },           { 'a', 'A' }, { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' },  { 'g', 'G' },
	{ 'h', 'H' },   { 'j', 'J' },       { 'k', 'K' }, { 'l', 'L' }, { ';', ':' }, { '\'', '"' }, { '`', '~' },
	{ 0, 0 },       { '\\', '|' },      { 'z', 'Z' }, { 'x', 'X' }, { 'c', 'C' }, { 'v', 'V' },  { 'b', 'B' },
	{ 'n', 'N' },   { 'm', 'M' },       { ',', '<' }, { '.', '>' }, { '/', '?' }, { 0, 0 },      { 0, 0 },
	{ 0, 0 },       { ' ', ' ' },
};
static const uint32_t keys = sizeof(scancodes) / sizeof(scancodes[0]);

static uint8_t shift = 0, caps_lock = 0;
static uint8_t buffer_pressed[BUFFER_MAX];
static uint8_t buffer_released[BUFFER_MAX];
static uint32_t buffer_pressed_size = 0;
static uint32_t buffer_released_size = 0;

static uint8_t get_scancode(uint8_t key);
static void put_buffer(uint8_t code, uint8_t* buff, uint32_t* buff_size);
static uint8_t get_buffer(uint8_t* buff, uint32_t* size);

int
keyboard_handler()
{
	int key;
	while (asm_kbd_active()) {
		key = asm_kbd_getkey();

		if (key == KC_L_SHIFT || key == KC_R_SHIFT)
			shift = 1;
		else if (key == KC_L_SHIFT_RELEASE || key == KC_R_SHIFT_REREASE)
			shift = 0;
		else if (key == KC_CAPS_LOCK)
			caps_lock = !caps_lock;

		uint8_t code;
		if (key & 0x80) {
			code = get_scancode(key - 128);
			if (key >= 0 && key - 128 < keys && code != 0)
				put_buffer(code, buffer_released, &buffer_released_size);
		} else {
			code = get_scancode(key);
			if (key >= 0 && key < keys && code != 0)
				put_buffer(code, buffer_pressed, &buffer_pressed_size);
		}
	}
	return 0;
}

char
kb_getpressed()
{
	return get_buffer(buffer_pressed, &buffer_pressed_size);
}

char
kb_getreleased()
{
	return get_buffer(buffer_released, &buffer_released_size);
}

static uint8_t
get_scancode(uint8_t key)
{
	uint8_t c;
	if (caps_lock && !shift) {
		c = scancodes[key][0];
		if (c >= 'A' && c <= 'z')
			c = scancodes[key][1];
	} else {
		c = scancodes[key][shift];
	}
	return c;
}

static void
put_buffer(uint8_t code, uint8_t* buff, uint32_t* size)
{
	if (*size < BUFFER_MAX)
		buff[(*size)++] = code;
}

static uint8_t
get_buffer(uint8_t* buff, uint32_t* size)
{
	if (*size <= 0)
		return 0;

	// agarramos el primero agregado (como una queue)
	uint8_t key = buff[0];
	for (int i = 1; i < *size; i++)
		buff[i - 1] = buff[i];
	(*size)--;
	return key;
}
