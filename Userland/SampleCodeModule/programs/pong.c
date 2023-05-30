#include <libasm.h>
#include <pong.h>
#include <stdlib.h>
#include <syscalls.h>

// dimensions
#define BAR_WIDTH 10
#define BAR_HEIGHT 80
#define BALL_SIZE 10
#define BALL_SPEED 15
#define MOV_DIFF 20
#define BORDER 4

// colors
#define BG 0x151f42
#define FG 0xf5ebbc

// index for player movement state
#define P1_UP 0
#define P1_DOWN 1
#define P2_UP 2
#define P2_DOWN 3

typedef enum
{
	QUIT = 0,
	RUNNING,
	RELOAD
} State;

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

typedef struct
{
	uint32_t x, y;
	uint32_t speed_x, speed_y;
} Ball;

static State state;
static Window window;
static Player p1, p2;
static Ball ball;
static uint8_t buttons[4] = { 0 };
static char buff[2];

static void intro_message();
static void game_loop();
static void process_key(uint8_t key, uint8_t state);
static void draw_window();
static void init_players();
static void init_ball();
static void update_players();
static void update_ball();
static void draw_players();
static void draw_player(Player* p);
static void draw_ball();

uint32_t
start_game()
{
	// hide cursor
	asm_show_cursor(0);
	asm_setcolor(FG, BG);

	// initialize the window
	window.width = asm_winwidth();
	window.height = asm_winheight();
	window.font_width = asm_fontwidth();
	window.font_height = asm_fontheight();

	// run until QUIT is set
	do {
		game_loop();
	} while (state == RELOAD);

	// show the cursor again
	asm_show_cursor(1);
	return 0;
}

static void
intro_message()
{
	char title[] = "PONG (THE GAME)";
	char subtitle[] = "PRESS ENTER TO START";
	uint32_t title_len = sizeof(title) / sizeof(title[0]);
	uint32_t subtitle_len = sizeof(subtitle) / sizeof(subtitle[0]);

	asm_cursor((window.width / window.font_width - title_len) / 2, window.height / (2 * window.font_height) - 4);
	puts(title);
	asm_cursor((window.width / window.font_width - subtitle_len) / 2, window.height / (2 * window.font_height) - 3);
	puts(subtitle);

	uint8_t c_status;
	while (!(getchar(&c_status) == '\n' && c_status == PRESSED)) {}

	asm_setcolor(BG, BG);
	asm_draw((window.width - (subtitle_len + 1) * window.font_width) / 2,
	         window.height / 2 - 4 * window.font_height,
	         subtitle_len * window.font_height,
	         2 * window.font_height);
}

static void
game_loop()
{
	asm_clear();
	draw_window();
	init_players();
	init_ball();
	intro_message();

	state = RUNNING;
	uint8_t c, c_status;

	for (int i = 0; i < 4; i++)
		buttons[i] = RELEASED;

	while (state == RUNNING) {
		c = getchar(&c_status);
		process_key(c, c_status);
		if (asm_ticked()) {
			update_players();
			update_ball();
			draw_players();
			draw_ball();
		}
	}
}

static void
process_key(uint8_t key, uint8_t status)
{
	switch (key) {
		case 'w': {
			buttons[P1_UP] = status;
		} break;

		case 's': {
			buttons[P1_DOWN] = status;
		} break;

		case 'i': {
			buttons[P2_UP] = status;
		} break;

		case 'k': {
			buttons[P2_DOWN] = status;
		} break;

		case 'q': {
			state = QUIT;
		} break;
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

// PLAYERS LOGIC
static void
init_players()
{
	p1.x = 20;
	p2.x = window.width - BAR_WIDTH - p1.x;
	p1.y = p2.y = (window.height - BAR_HEIGHT) / 2;
	p1.movement = p2.movement = 0;
	if (state != RELOAD)
		p1.score = p2.score = 0;
	asm_draw(p1.x, p1.y, BAR_WIDTH, BAR_HEIGHT);
	asm_draw(p2.x, p2.y, BAR_WIDTH, BAR_HEIGHT);
	draw_players();
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
draw_players()
{
	asm_cursor(0, 0);
	uint_to_base(p1.score, buff, 10);
	puts("Player 1: ");
	puts(buff);
	draw_player(&p1);

	static char msg[] = "Player 2: ";
	static uint32_t size = sizeof(msg) / sizeof(msg[0]);
	asm_cursor(window.width / window.font_width - size, 0);
	uint_to_base(p2.score, buff, 10);
	puts("Player 2: ");
	puts(buff);
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

// BALL LOGIC
static void
init_ball()
{
	ball.x = (window.width - BALL_SIZE) / 2;
	ball.y = (window.height - BALL_SIZE) / 2;
	/*
	 * TODO: randomizar el disparo inicial
	 */
	ball.speed_x = ball.speed_y = BALL_SPEED;
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE);
}

static void
update_ball()
{
	// rebota en los limites horizontales
	if (ball.y + ball.speed_y <= BORDER || ball.y + ball.speed_y >= window.height - BORDER)
		ball.speed_y *= -1;

	// dentro o fuera
	if (ball.x + ball.speed_x >= p1.x && ball.x + ball.speed_x + BALL_SIZE <= p2.x + BAR_WIDTH) {
		// rebota en algún jugador
		if ((ball.x + ball.speed_x <= p1.x + BAR_WIDTH && ball.y + ball.speed_y >= p1.y &&
		     ball.y + ball.speed_y <= p1.y + BAR_HEIGHT) ||
		    (ball.x + ball.speed_x + BALL_SIZE >= p2.x && ball.y + ball.speed_y >= p2.y &&
		     ball.y + ball.speed_y <= p2.y + BAR_HEIGHT))
			/*
			 * TODO: el angulo de salida deberia depender de la parte de la barra con la que impacta la bola
			 */
			ball.speed_x *= -1;
	} else {
		// se fue la pelota. hay que sumar puntos y reiniciar
		state = RELOAD;
		if (ball.x + ball.speed_x < p1.x)
			p2.score++;
		if (ball.x + ball.speed_x + BALL_SIZE > p2.x + BAR_WIDTH)
			p1.score++;
	}
}

static void
draw_ball()
{
	asm_setcolor(BG, BG);
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE);
	asm_setcolor(FG, BG);
	ball.x += ball.speed_x;
	ball.y += ball.speed_y;
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE);
}
