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

struct player
{
	uint32_t x, y;
	int8_t movement;
	uint32_t score;
};

typedef struct player Player;

static uint8_t running = 1;
static Player p1, p2;

static void main_menu();
static void game_loop();
static void draw_window();
static void init_players();
static void draw_players();
static void draw_player(Player* p);
static uint8_t process_key(uint8_t pressed, uint8_t released);

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

	char title[] = "PONG - MAIN MENU";
	char subtitle[] = "PRESS ENTER TO START";
	uint32_t title_len = sizeof(title) / sizeof(title[0]);
	uint32_t subtitle_len = sizeof(subtitle) / sizeof(subtitle[0]);

	asm_cursor((asm_winwidth() / 8 - title_len) / 2, asm_winheight() / (2 * 16) - 1);
	puts(title);
	asm_cursor((asm_winwidth() / 8 - subtitle_len) / 2, asm_winheight() / (2 * 16) + 1);
	puts(subtitle);

	while (getchar() != '\n') {}
}

static void
game_loop()
{
	asm_clear();
	draw_window();
	init_players();

	uint64_t ref_tick;
	uint8_t pressed, released;

	while (running) {
		if ((ref_tick = asm_getticks()) % TICK_RATE == 0) {
			pressed = getchar();
			released = asm_kreleased();
			if (pressed || released) {
				process_key(pressed, released);
				draw_players();
			}
		}
	}
}

static void
draw_window()
{
	asm_draw(0, 0, BORDER, asm_winheight());
	asm_draw(BORDER, 0, asm_winwidth() - BORDER, BORDER);
	asm_draw(asm_winwidth() - BORDER, 0, BORDER, asm_winheight());
	asm_draw(BORDER, asm_winheight() - BORDER, asm_winwidth() - BORDER, BORDER);
}

static void
init_players()
{
	p1.x = 20;
	p2.x = asm_winwidth() - BAR_WIDTH - p1.x;
	p1.y = p2.y = (asm_winheight() - BAR_HEIGHT) / 2;
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
		if (p->y + BAR_HEIGHT >= asm_winheight() - MOV_DIFF)
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

static uint8_t
process_key(uint8_t pressed, uint8_t released)
{
	if (pressed == 'q' || released == 'q') {
		running = 0;
		return 0;
	}

	if (pressed == 'w')
		p1.movement = -1;
	else if (pressed == 's')
		p1.movement = 1;

	if (released == 'w' || released == 's')
		p1.movement = 0;

	if (pressed == 'i')
		p2.movement = -1;
	else if (pressed == 'k')
		p2.movement = 1;

	if (released == 'i' || released == 'k')
		p2.movement = 0;

	return p1.movement || p2.movement;
}
