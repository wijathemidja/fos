#pragma once
#include <stdint.h>
#include <uefi.h>
#define SSFN_IMPLEMENTATION
#include <ssfn.h>

#define SIZE 24
// color "type (takes red green and blue values one byte each)
typedef struct {
    unsigned char r,g,b;
} color_t;

typedef struct {
    volatile uint32_t* framebuffer;
    const uint64_t framebuffer_size;
    const int pitch;
    const int width;
    const int height;
} framebuffer_t;

typedef struct {
    ssfn_t* ctx;
    ssfn_buf_t* buf;
} prstr_t;

framebuffer_t* fbptr = nullptr;
framebuffer_t* vfbptr = nullptr;
const color_t white = {.r = 255,.g = 255,.b = 255};
const color_t baby_blue = {.r = 153, .g = 204, .b = 255};
const color_t black = {.r = 0, .g = 0, .b = 0};

inline void print_pixel(const uint16_t x, const uint16_t y, const color_t color) {
    framebuffer_t fb = *fbptr;
    uint64_t where = x + y*fb.pitch;
    if (where*4 >= fb.framebuffer_size) {
        where = fb.framebuffer_size - 1; // stop memory overflow out of framebuffer
    }
    uint32_t pixel_col = color.r << 16 | color.g << 8 | color.b;
    fb.framebuffer[where] = pixel_col;
}
// print_rectangle doesnt recalculate where for each pixel so its much more efficient
inline void print_rectangle(const uint16_t x1, const uint16_t x2, const uint16_t y1, const uint16_t y2, const color_t color) {
    framebuffer_t fb = *fbptr;
    uint64_t where = x1 + y1*fb.pitch;
    if (where*4 >= fb.framebuffer_size) {
        where = fb.framebuffer_size - 1;
    }
    uint32_t pixel_col = color.r << 16 | color.g << 8 | color.b;
    for (int i = 0; i <= y2 - y1; i++) {
        for (int j = 0; j <= x2-x1; j++) {
            fb.framebuffer[where+j] = pixel_col;
        }
        where += fb.pitch;
    }
}


inline void print_string(prstr_t ssfn, char* string){
    ssfn_t* ctx = ssfn.ctx;
    ssfn_buf_t* buf = ssfn.buf;
    int bytes_left = ssfn_render(ctx, buf, string);
    if (bytes_left > 0) print_string(ssfn, string + bytes_left);
}

ssfn_t ctx = { 0 };
ssfn_buf_t ssfn_buffer = {
    .ptr = nullptr,
    .w = 0,
    .h = 0,
    .p = 0,
    .x = 0,
    .y = SIZE,
    .fg = 0,
};
inline unsigned int print_ln(color_t color, uint8_t opacity, char* string, uint8_t currentLine) {
    uint32_t ssfn_color = opacity << 24 | color.r << 16 | color.g << 8 | color.b;
    ssfn_buffer.x = 0;
    ssfn_buffer.y = SIZE * (currentLine+1);
    ssfn_buffer.fg = ssfn_color;
    prstr_t ssfn = {.ctx = &ctx, .buf = &ssfn_buffer};
    print_string(ssfn, string);
    memcpy((void*)fbptr->framebuffer, (void*)vfbptr->framebuffer, fbptr->framebuffer_size);
    return ++currentLine;
}

#define print_shortcut(string, currentLine) (print_ln(white, 0xFF, string, currentLine))
#define print(string) \
    currentLine = print_shortcut(string, currentLine)

inline uint8_t clear_screen() {
    const framebuffer_t fb = *fbptr;
    print_rectangle(0, fb.height, 0, fb.width, black);
    return 0;
}

