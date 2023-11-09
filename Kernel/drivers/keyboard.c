#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <naiveConsole.h>
#include <process.h>
#include <queue.h>
#include <scheduler.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <text.h>
#include <video.h>

#define KC_L_SHIFT         42
#define KC_R_SHIFT         54
#define KC_L_SHIFT_RELEASE KC_L_SHIFT + 128
#define KC_R_SHIFT_REREASE KC_R_SHIFT + 128
#define KC_CAPS_LOCK       58
#define KC_L_CTRL          29
#define KC_L_CTRL_RELEASE  KC_L_CTRL + 128

#define BUFFER_MAX 32

// define los caracteres para cada código. al presionar shift se utiliza la segunda columna
static const uint8_t scancodes[][2] = {
	{ 0, 0 },       { 0, 0 },       { '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' },  { '5', '%' },
	{ '6', '^' },   { '7', '&' },   { '8', '*' }, { '9', '(' }, { '0', ')' }, { '-', '_' },  { '=', '+' },
	{ '\b', '\b' }, { '\t', '\t' }, { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' }, { 'r', 'R' },  { 't', 'T' },
	{ 'y', 'Y' },   { 'u', 'U' },   { 'i', 'I' }, { 'o', 'O' }, { 'p', 'P' }, { '[', '{' },  { ']', '}' },
	{ '\n', '\n' }, { 0, 0 },       { 'a', 'A' }, { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' },  { 'g', 'G' },
	{ 'h', 'H' },   { 'j', 'J' },   { 'k', 'K' }, { 'l', 'L' }, { ';', ':' }, { '\'', '"' }, { '`', '~' },
	{ 0, 0 },       { '\\', '|' },  { 'z', 'Z' }, { 'x', 'X' }, { 'c', 'C' }, { 'v', 'V' },  { 'b', 'B' },
	{ 'n', 'N' },   { 'm', 'M' },   { ',', '<' }, { '.', '>' }, { '/', '?' }, { 0, 0 },      { 0, 0 },
	{ 0, 0 },       { ' ', ' ' },
};
static const uint32_t keys = sizeof(scancodes) / sizeof(scancodes[0]);

// caracteres de control
static uint8_t shift = 0, caps_lock = 0, control = 0;

// buffers de almacenamiento para el caracter y el estado del mismo (PRESSED o RELEASED)
static uint8_t buffer_chars[BUFFER_MAX];
static uint32_t buffer_size = 0;

// cola de pids esperando input
static Queue waiting_pids;

static uint8_t get_scancode(uint8_t key);
static void put_buffer(uint8_t code);
static int read_callback(int pid, int fd, char* buf, uint32_t size);

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
		if (key == KC_CAPS_LOCK)
			caps_lock = !caps_lock;
		if (key == KC_L_CTRL)
			control = 1;
		else if (key == KC_L_CTRL_RELEASE)
			control = 0;

		if (key & 0x80)
			continue;

		uint8_t code;
		code = get_scancode(key);

		if (key >= 0 && key < keys && code != 0) {
			put_buffer(code);
		}
	}
	return 0;
}

void
kb_init()
{
	waiting_pids = queue_create();
}

int
kb_read_chars(char* buf, uint32_t size)
{
	uint32_t count = buffer_size;
	if (count > size)
		count = size;
	if (count == 0)
		return 0;

	memcpy(buf, buffer_chars, count);
	memcpy(buffer_chars, buffer_chars + count, buffer_size - count);
	buffer_size -= count;
	return count;
}

int
kb_map_fd(int pid, int fd)
{
	return proc_map_fd(pid, fd, read_callback, NULL, NULL, NULL);
}

static uint8_t
get_scancode(uint8_t key)
{
	uint8_t c;
	if (caps_lock && !shift) {
		c = scancodes[key][0];
		if (c >= 'a' && c <= 'z')
			c = scancodes[key][1];
	} else {
		c = scancodes[key][shift];
	}
	return c;
}

static void
put_buffer(uint8_t code)
{
	if (buffer_size < BUFFER_MAX) {
		buffer_chars[buffer_size++] = code;
		queue_unblock_all(waiting_pids);
	}
}

static int
read_callback(int pid, int fd, char* buf, uint32_t size)
{
	if (proc_is_fg(pid) != 1)
		return -1;
	if (size == 0)
		return 0;
	if (size > BUFFER_MAX)
		size = BUFFER_MAX;

	int count;
	while ((count = kb_read_chars(buf, size)) == 0) {
		queue_add(waiting_pids, pid);
		sch_block(pid);
		sch_yield();
	}

	return count;
}
