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
#define KC_L_SHIFT 42
#define KC_R_SHIFT 54
#define KC_L_SHIFT_RELEASE KC_L_SHIFT + 128
#define KC_R_SHIFT_REREASE KC_R_SHIFT + 128
#define KC_CAPS_LOCK 58
#define KC_L_CTRL 29
#define KC_L_CTRL_RELEASE KC_L_CTRL + 128

#define BUFFER_MAX 10
#define REGISTER_CAPTURE 9
#define RELEASED 0
#define PRESSED 1

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
static uint8_t buffer_states[BUFFER_MAX];
static uint32_t buffer_size = 0;

// cola de pids esperando input
static Queue waiting_pids;

static uint8_t get_scancode(uint8_t key);
static void put_buffer(uint8_t code, uint8_t state);
static int read_callback(int pid, int fd, char* buf, size_t size);

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

		uint8_t code, state;

		// aunque el caracter haya sido soltado, quiero guardar su ASCII
		state = (key & 0x80 ? RELEASED : PRESSED);

		// TODO: eliminar implementacion de released
		if (state == RELEASED)
			return 0;

		key -= (key & 0x80 ? 0x80 : 0);
		code = get_scancode(key);

		// handle para el snapshot de registros
		if (control && (code == 'r' || code == 'R'))
			return REGISTER_CAPTURE;
		else if (key >= 0 && key < keys && code != 0)
			put_buffer(code, state);
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
	memcpy(buffer_states, buffer_states + count, buffer_size - count);
	buffer_size -= count;
	return count;
}

char
kb_getchar(uint8_t* state)
{
	if (buffer_size <= 0)
		return 0;

	// agarramos el primero agregado (como una queue)
	uint8_t key = buffer_chars[0];
	*state = buffer_states[0];

	// movemos los valores restantes una posición adelante
	for (int i = 1; i < buffer_size; i++) {
		buffer_chars[i - 1] = buffer_chars[i];
		buffer_states[i - 1] = buffer_states[i];
	}

	buffer_size--;
	return key;
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
put_buffer(uint8_t code, uint8_t state)
{
	if (buffer_size < BUFFER_MAX) {
		buffer_chars[buffer_size] = code;
		buffer_states[buffer_size++] = state;
		queue_unblock_all(waiting_pids);
	}
}

static int
read_callback(int pid, int fd, char* buf, size_t size)
{
	// TODO: chequear si esta en foreground
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
