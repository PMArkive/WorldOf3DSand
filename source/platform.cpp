#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <algorithm>

#include <3ds.h>

#include "platform.h"
#include "util.h"

static unsigned char asciiData[128][8] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x3E, 0x41, 0x55, 0x41, 0x55, 0x49, 0x3E },
		{ 0x00, 0x3E, 0x7F, 0x6B, 0x7F, 0x6B, 0x77, 0x3E },
		{ 0x00, 0x22, 0x77, 0x7F, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x7F, 0x2A, 0x08, 0x1C },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x08, 0x1C },
		{ 0x00, 0x00, 0x1C, 0x3E, 0x3E, 0x3E, 0x1C, 0x00 },
		{ 0xFF, 0xFF, 0xE3, 0xC1, 0xC1, 0xC1, 0xE3, 0xFF },
		{ 0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00 },
		{ 0xFF, 0xFF, 0xE3, 0xDD, 0xDD, 0xDD, 0xE3, 0xFF },
		{ 0x00, 0x0F, 0x03, 0x05, 0x39, 0x48, 0x48, 0x30 },
		{ 0x00, 0x08, 0x3E, 0x08, 0x1C, 0x22, 0x22, 0x1C },
		{ 0x00, 0x18, 0x14, 0x10, 0x10, 0x30, 0x70, 0x60 },
		{ 0x00, 0x0F, 0x19, 0x11, 0x13, 0x37, 0x76, 0x60 },
		{ 0x00, 0x08, 0x2A, 0x1C, 0x77, 0x1C, 0x2A, 0x08 },
		{ 0x00, 0x60, 0x78, 0x7E, 0x7F, 0x7E, 0x78, 0x60 },
		{ 0x00, 0x03, 0x0F, 0x3F, 0x7F, 0x3F, 0x0F, 0x03 },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x08, 0x2A, 0x1C, 0x08 },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x66 },
		{ 0x00, 0x3F, 0x65, 0x65, 0x3D, 0x05, 0x05, 0x05 },
		{ 0x00, 0x0C, 0x32, 0x48, 0x24, 0x12, 0x4C, 0x30 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x08, 0x2A, 0x1C, 0x3E },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x1C, 0x1C, 0x1C },
		{ 0x00, 0x1C, 0x1C, 0x1C, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x0C, 0x7E, 0x7F, 0x7E, 0x0C, 0x08 },
		{ 0x00, 0x08, 0x18, 0x3F, 0x7F, 0x3F, 0x18, 0x08 },
		{ 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x7F, 0x7F },
		{ 0x00, 0x00, 0x14, 0x22, 0x7F, 0x22, 0x14, 0x00 },
		{ 0x00, 0x08, 0x1C, 0x1C, 0x3E, 0x3E, 0x7F, 0x7F },
		{ 0x00, 0x7F, 0x7F, 0x3E, 0x3E, 0x1C, 0x1C, 0x08 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18 },
		{ 0x00, 0x36, 0x36, 0x14, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36 },
		{ 0x00, 0x08, 0x1E, 0x20, 0x1C, 0x02, 0x3C, 0x08 },
		{ 0x00, 0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06 },
		{ 0x00, 0x3C, 0x66, 0x3C, 0x28, 0x65, 0x66, 0x3F },
		{ 0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00 },
		{ 0x00, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06 },
		{ 0x00, 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60 },
		{ 0x00, 0x00, 0x36, 0x1C, 0x7F, 0x1C, 0x36, 0x00 },
		{ 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x60 },
		{ 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60 },
		{ 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00 },
		{ 0x00, 0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C },
		{ 0x00, 0x18, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E },
		{ 0x00, 0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E },
		{ 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C },
		{ 0x00, 0x0C, 0x1C, 0x2C, 0x4C, 0x7E, 0x0C, 0x0C },
		{ 0x00, 0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C },
		{ 0x00, 0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C },
		{ 0x00, 0x7E, 0x66, 0x0C, 0x0C, 0x18, 0x18, 0x18 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C },
		{ 0x00, 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C },
		{ 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00 },
		{ 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30 },
		{ 0x00, 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00 },
		{ 0x00, 0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x18, 0x00, 0x18 },
		{ 0x00, 0x38, 0x44, 0x5C, 0x58, 0x42, 0x3C, 0x00 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C },
		{ 0x00, 0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C },
		{ 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C },
		{ 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E },
		{ 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x60, 0x60, 0x6E, 0x66, 0x3C },
		{ 0x00, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C },
		{ 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x6C, 0x6C, 0x38 },
		{ 0x00, 0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E },
		{ 0x00, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63 },
		{ 0x00, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C },
		{ 0x00, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x06 },
		{ 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C },
		{ 0x00, 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x18 },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18 },
		{ 0x00, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63 },
		{ 0x00, 0x63, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x63 },
		{ 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18 },
		{ 0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E },
		{ 0x00, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E },
		{ 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00 },
		{ 0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78 },
		{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F },
		{ 0x00, 0x0C, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E },
		{ 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C },
		{ 0x00, 0x06, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C },
		{ 0x00, 0x1C, 0x36, 0x30, 0x30, 0x7C, 0x30, 0x30 },
		{ 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C },
		{ 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66 },
		{ 0x00, 0x00, 0x18, 0x00, 0x18, 0x18, 0x18, 0x3C },
		{ 0x00, 0x0C, 0x00, 0x0C, 0x0C, 0x6C, 0x6C, 0x38 },
		{ 0x00, 0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
		{ 0x00, 0x00, 0x00, 0x63, 0x77, 0x7F, 0x6B, 0x6B },
		{ 0x00, 0x00, 0x00, 0x7C, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C },
		{ 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60 },
		{ 0x00, 0x00, 0x3C, 0x6C, 0x6C, 0x3C, 0x0D, 0x0F },
		{ 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x60, 0x60 },
		{ 0x00, 0x00, 0x00, 0x3E, 0x40, 0x3C, 0x02, 0x7C },
		{ 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x18 },
		{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E },
		{ 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x3C, 0x18 },
		{ 0x00, 0x00, 0x00, 0x63, 0x6B, 0x6B, 0x6B, 0x3E },
		{ 0x00, 0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66 },
		{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x3E, 0x06, 0x3C },
		{ 0x00, 0x00, 0x00, 0x3C, 0x0C, 0x18, 0x30, 0x3C },
		{ 0x00, 0x0E, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0E },
		{ 0x00, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18 },
		{ 0x00, 0x70, 0x18, 0x18, 0x0C, 0x18, 0x18, 0x70 },
		{ 0x00, 0x00, 0x00, 0x3A, 0x6C, 0x00, 0x00, 0x00 },
		{ 0x00, 0x08, 0x1C, 0x36, 0x63, 0x41, 0x41, 0x7F }
};

PAD_KEY buttonMap[13] = {
		KEY_A,
		KEY_B,
		KEY_X,
		KEY_Y,
		KEY_L,
		KEY_R,
		KEY_START,
		KEY_SELECT,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_TOUCH
};

u8* fb = NULL;
u16 fbWidth = 0;
u16 fbHeight = 0;

bool screen_begin_draw() {
	if(fb != NULL) {
		return false;
	}

	fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &fbWidth, &fbHeight);
	return true;
}

bool screen_begin_draw_info() {
	if(fb != NULL) {
		return false;
	}

	fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fbWidth, &fbHeight);
	return true;
}

bool screen_end_draw() {
	if(fb == NULL) {
		return false;
	}

	fb = NULL;
	fbWidth = 0;
	fbHeight = 0;
	return true;
}

void screen_swap_buffers() {
	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();
	platform_delay(1); // Seems to help with flickering.
}

int screen_get_width() {
	// Use fbHeight since the framebuffer is rotated 90 degrees to the left.
	if(fb != NULL) {
		return fbHeight;
	}

	int width = 0;
	screen_begin_draw();
	width = fbHeight;
	screen_end_draw();
	return width;
}

int screen_get_height() {
	// Use fbWidth since the framebuffer is rotated 90 degrees to the left.
	if(fb != NULL) {
		return fbWidth;
	}

	int height = 0;
	screen_begin_draw();
	height = fbWidth;
	screen_end_draw();
	return height;
}

void screen_take_screenshot() {
	u32 imageSize = 400 * 480 * 3;
	u8* temp = (u8*) malloc(0x36 + imageSize);
	memset(temp, 0, 0x36 + imageSize);

	*(u16*) &temp[0x0] = 0x4D42;
	*(u32*) &temp[0x2] = 0x36 + imageSize;
	*(u32*) &temp[0xA] = 0x36;
	*(u32*) &temp[0xE] = 0x28;
	*(u32*) &temp[0x12] = 400;
	*(u32*) &temp[0x16] = 480;
	*(u32*) &temp[0x1A] = 0x00180001;
	*(u32*) &temp[0x22] = imageSize;

	u8* framebuf = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	for(int y = 0; y < 240; y++) {
		for(int x = 0; x < 400; x++) {
			int si = ((239 - y) + (x * 240)) * 3;
			int di = 0x36 + (x + ((479 - y) * 400)) * 3;
			temp[di++] = framebuf[si++];
			temp[di++] = framebuf[si++];
			temp[di++] = framebuf[si++];
		}
	}

	framebuf = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	for(int y = 0; y < 240; y++) {
		for(int x = 0; x < 320; x++) {
			int si = ((239 - y) + (x * 240)) * 3;
			int di = 0x36 + ((x+40) + ((239 - y) * 400)) * 3;
			temp[di++] = framebuf[si++];
			temp[di++] = framebuf[si++];
			temp[di++] = framebuf[si++];
		}
	}

	char file[256];
	snprintf(file, 256, "sdmc:/wo3ds_screenshot_%08d.bmp", (int) (svcGetSystemTick() / 446872));
	int fd = open(file, O_WRONLY | O_CREAT | O_SYNC);
	write(fd, temp, 0x36 + imageSize);
	close(fd);
	free(temp);
}

int screen_get_index(int x, int y) {
	int height = screen_get_height();
	// Reverse the y coordinate when finding the index.
	// This is done as the framebuffer is rotated 90 degrees to the left.
	return ((height - y) + x * height) * 3;
}

void screen_draw(int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return;
	}

	int idx = screen_get_index(x, y);
	fb[idx + 0] = b;
	fb[idx + 1] = g;
	fb[idx + 2] = r;
}

void screen_draw_c(int x, int y, Color color) {
	screen_draw(x, y, color.r, color.g, color.b);
}

void screen_fill(int x, int y, int width, int height, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return;
	}

	int swidth = screen_get_width();
	int sheight = screen_get_height();
	if(x + width < 0 || y + height < 0 || x >= swidth || y >= sheight) {
		return;
	}

	if(x < 0) {
		width += x;
		x = 0;
	}

	if(y < 0) {
		height += y;
		y = 0;
	}

	if(x + width >= swidth){
		width = swidth - x;
	}

	if(y + height >= sheight){
		height = sheight - y;
	}

	u8 colorLine[height * 3];
	for(int ly = 0; ly < height; ly++) {
		colorLine[ly * 3 + 0] = b;
		colorLine[ly * 3 + 1] = g;
		colorLine[ly * 3 + 2] = r;
	}

	u8* fbAddr = fb + screen_get_index(x, y) - (height * 3);
	for(int dx = 0; dx < width; dx++) {
		memcpy(fbAddr, colorLine, (size_t) (height * 3));
		fbAddr += sheight * 3;
	}
}

void screen_fill_c(int x, int y, int width, int height, Color color) {
	screen_fill(x, y, width, height, color.r, color.g, color.b);
}

int screen_get_str_width(const char* str) {
	return strlen(str) * 8;
}

int screen_get_str_height(const char* str) {
	return 8;
}

void screen_draw_char(char c, int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return;
	}

	unsigned char* data = asciiData[(int) c];
	for(int cy = 0; cy < 8; cy++) {
		unsigned char l = data[cy];
		for(int cx = 0; cx < 8; cx++) {
			if((0b10000000 >> cx) & l) {
				screen_draw(x + cx, y + cy, r, g, b);
			}
		}
	}
}

void screen_draw_string(const char* string, int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return;
	}

	int len = (int) strlen(string);
	int cx = x;
	int cy = y;
	for(int i = 0; i < len; i++) {
		char c = string[i];
		if(c == '\n') {
			cx = x;
			cy += 8;
		}

		screen_draw_char(c, cx, cy, r, g, b);
		cx += 8;
	}
}

void screen_draw_string_c(const char* string, int x, int y, Color color) {
	screen_draw_string(string, x, y, color.r, color.g, color.b);
}

void screen_clear(u8 r, u8 g, u8 b) {
	bool end = screen_begin_draw();
	screen_fill(0, 0, screen_get_width(), screen_get_height(), r, g, b);
	if(end) {
		screen_end_draw();
	}
}

void screen_clear_c(Color color) {
	screen_clear(color.r, color.g, color.b);
}

void screen_clear_info(u8 r, u8 g, u8 b) {
	bool end = screen_begin_draw_info();
	screen_fill(0, 0, screen_get_width(), screen_get_height(), r, g, b);
	if(end) {
		screen_end_draw();
	}
}

void screen_clear_info_c(Color color) {
	screen_clear_info(color.r, color.g, color.b);
}

void input_poll() {
	hidScanInput();
}

bool input_is_released(Button button) {
	return (hidKeysUp() & buttonMap[button]) != 0;
}

bool input_is_pressed(Button button) {
	return (hidKeysDown() & buttonMap[button]) != 0;
}

bool input_is_held(Button button) {
	return (hidKeysHeld() & buttonMap[button]) != 0;
}

Touch input_get_touch() {
	touchPosition pos;
	hidTouchRead(&pos);
	return {pos.px, pos.py};
}

void platform_init() {
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInit();
	fsInit();
	sdmcInit();
}

void platform_cleanup() {
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
}

bool platform_is_running() {
	return aptMainLoop();
}

u64 platform_get_time() {
	return osGetTime();
}

void platform_delay(int ms) {
	svcSleepThread(ms * 1000000);
}

void platform_print(const char* str) {
	svcOutputDebugString(str, strlen(str));
}

void platform_printf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	char* str = vsdprintf(format, args);
	va_end(args);
	platform_print(str);
}

char* platform_get_path(const char* path) {
	return sdprintf("sdmc:/%s", path);
}
