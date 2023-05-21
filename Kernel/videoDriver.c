#include <font.h>
#include <videoDriver.h>

struct vbe_mode_info_structure
{
	uint16_t attributes;   // deprecated, only bit 7 should be
	                       // of interest to you, and it indicates the mode
	                       // supports a linear frame buffer.
	uint8_t window_a;      // deprecated
	uint8_t window_b;      // deprecated
	uint16_t granularity;  // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;  // deprecated; used to switch banks from protected
	                        // mode without returning to real mode
	uint16_t pitch;         // number of bytes per horizontal line
	uint16_t width;         // width in pixels
	uint16_t height;        // height in pixels
	uint8_t w_char;         // unused...
	uint8_t y_char;         //
	uint8_t planes;         //
	uint8_t bpp;            // bits per pixel in this mode
	uint8_t banks;          // deprecated; total number of banks in this mode
	uint8_t memory_model;   //
	uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but
	                        // may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;  // physical address of the linear frame buffer; write
	                       // here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;  // size of memory in the framebuffer but not
	                               // being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure* VBEModeInfo;
VBEModeInfo vbe_mode_info = (VBEModeInfo)0x5C00;

struct font_info
{
	uint32_t fg, bg;
};

typedef struct font_info FontInfo;
FontInfo fontInfo = {
	.bg = 0x0,
	.fg = WHITE,
};

void
vd_put_pixel(uint32_t hex_color, uint32_t x, uint32_t y)
{
	vd_put_pixel_rgb((hex_color & RED) >> 16, (hex_color & GREEN) >> 8, hex_color & BLUE, x, y);
}

void
vd_put_pixel_rgb(uint8_t r, uint8_t g, uint32_t b, uint32_t x, uint32_t y)
{
	uint8_t* screen = (uint8_t*)(uint64_t)vbe_mode_info->framebuffer;
	int offset = y * vbe_mode_info->pitch + x * vbe_mode_info->bpp / 8;
	screen[offset] = b;
	screen[offset + 1] = g;
	screen[offset + 2] = r;
}

void
vd_put_char(char c, uint32_t x, uint32_t y)
{
	uint32_t aux_x = x, aux_y = y;
	uint8_t* c_bitmap = fnt_get_char_bitmap(c);
	char is_foreground;

	for (int i = 0; i < CHAR_HEIGHT; i++) {
		for (int j = 0; j < CHAR_WIDTH; j++) {
			is_foreground = (1 << (CHAR_WIDTH - j)) & c_bitmap[i];
			if (is_foreground)
				vd_put_pixel(fontInfo.fg, aux_x, aux_y);
			else
				vd_put_pixel(fontInfo.bg, aux_x, aux_y);
			aux_x++;
		}
		aux_x = x;
		aux_y++;
	}
}

void
vd_set_color(uint32_t fg, uint32_t bg)
{
	fontInfo.fg = fg;
	fontInfo.bg = bg;
}

void
vd_clear()
{
	vd_clear_bg(fontInfo.bg);
}

void
vd_clear_bg(uint32_t bg)
{
	for (int i = 0; i < vbe_mode_info->height; i++)
		for (int j = 0; j < vbe_mode_info->width; j++)
			vd_put_pixel(bg, j, i);
}
