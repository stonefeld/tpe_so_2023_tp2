#include <libasm.h>
#include <pong.h>
#include <stdlib.h>
#include <syscalls.h>

#define BAR_WIDTH 10
#define BAR_HEIGHT 80
#define BORDER 4
#define MOV_DIFF 20
#define TICK_RATE 1
#define BG 0x151f42
#define FG 0xf5ebbc

#define P1_UP 0
#define P1_DOWN 1
#define P2_UP 2
#define P2_DOWN 3

typedef struct
{
	uint32_t width, height;
	uint32_t font_width, font_height;
} Window;

typedef struct
{
	uint32_t x, y;
	int8_t movement;
	uint32_t score;
} Player;

static uint8_t running;
static Window window;
static Player p1, p2;
static uint8_t buttons[4] = { 0 };

static void main_menu();
static void game_loop();
static void draw_window();
static void init_players();
static void update_players();
static void draw_players();
static void draw_player(Player* p);
static void process_key(uint8_t key, uint8_t state);

uint32_t
start_game()
{
	main_menu();
	game_loop();
	return 0;
}

static void
main_menu()
{
	asm_setcolor(0xf5ebbc, 0x151f42);
	asm_clear();

	window.width = asm_winwidth();
	window.height = asm_winheight();
	window.font_width = asm_fontwidth();
	window.font_height = asm_fontheight();

	char title[] = "PONG - MAIN MENU";
	char subtitle[] = "PRESS ENTER TO START";
	uint32_t title_len = sizeof(title) / sizeof(title[0]);
	uint32_t subtitle_len = sizeof(subtitle) / sizeof(subtitle[0]);

	asm_cursor((window.width / 8 - title_len) / 2, window.height / (2 * 16) - 1);
	puts(title);
	asm_cursor((window.width / 8 - subtitle_len) / 2, window.height / (2 * 16) + 1);
	puts(subtitle);

	uint8_t state;

	while (!(getchar(&state) == '\n' && state == PRESSED)) {}
}

static void
game_loop()
{
	asm_clear();
	draw_window();
	init_players();

	running = 1;
	uint8_t c, state;

	while (running) {
		c = getchar(&state);
		process_key(c, state);
		if (asm_ticked()) {
			update_players();
			draw_players();
		}
	}
}

static void
draw_window()
{
	asm_draw(0, 0, BORDER, window.height);
	asm_draw(BORDER, 0, window.width - BORDER, BORDER);
	asm_draw(window.width - BORDER, 0, BORDER, window.height);
	asm_draw(BORDER, window.height - BORDER, window.width - BORDER, BORDER);
}

static void
update_players()
{
	if (buttons[P1_UP] == PRESSED && buttons[P1_DOWN] == RELEASED)
		p1.movement = -1;
	if (buttons[P1_UP] == RELEASED && buttons[P1_DOWN] == PRESSED)
		p1.movement = 1;
	if (buttons[P1_UP] == RELEASED && buttons[P1_DOWN] == RELEASED)
		p1.movement = 0;
	if (buttons[P2_UP] == PRESSED && buttons[P2_DOWN] == RELEASED)
		p2.movement = -1;
	if (buttons[P2_UP] == RELEASED && buttons[P2_DOWN] == PRESSED)
		p2.movement = 1;
	if (buttons[P2_UP] == RELEASED && buttons[P2_DOWN] == RELEASED)
		p2.movement = 0;
}

static void
init_players()
{
	p1.x = 20;
	p2.x = window.width - BAR_WIDTH - p1.x;
	p1.y = p2.y = (window.height - BAR_HEIGHT) / 2;
	p1.movement = p2.movement = 0;
	asm_draw(p1.x, p1.y, BAR_WIDTH, BAR_HEIGHT);
	asm_draw(p2.x, p2.y, BAR_WIDTH, BAR_HEIGHT);
}

static void
draw_players()
{
	draw_player(&p1);
	draw_player(&p2);
}

static void
draw_player(Player* p)
{
	if (p->movement == 1) {
		if (p->y + BAR_HEIGHT >= window.height - MOV_DIFF)
			return;
		asm_setcolor(BG, BG);
		asm_draw(p->x, p->y, BAR_WIDTH, MOV_DIFF);
		asm_setcolor(FG, BG);
		asm_draw(p->x, p->y + BAR_HEIGHT, BAR_WIDTH, MOV_DIFF);
	} else if (p->movement == -1) {
		if (p->y < MOV_DIFF)
			return;
		asm_setcolor(BG, BG);
		asm_draw(p->x, p->y + BAR_HEIGHT - MOV_DIFF, BAR_WIDTH, MOV_DIFF);
		asm_setcolor(FG, BG);
		asm_draw(p->x, p->y - MOV_DIFF, BAR_WIDTH, MOV_DIFF);
	}
	p->y += MOV_DIFF * p->movement;
}

static void
process_key(uint8_t key, uint8_t state)
{
	switch (key) {
		case 'w': {
			buttons[P1_UP] = state;
		} break;

		case 's': {
			buttons[P1_DOWN] = state;
		} break;

		case 'i': {
			buttons[P2_UP] = state;
		} break;

		case 'k': {
			buttons[P2_DOWN] = state;
		} break;

		case 'q': {
			running = 0;
		} break;
	}
}
