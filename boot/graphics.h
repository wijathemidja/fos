#pragma once
#include <stdint.h>
#include <uefi.h>

// use ssfn
#define SSFN_IMPLEMENTATION
#include <ssfn.h>

// font size
#define SIZE 24

// ssfn init
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
// color "type (takes red green and blue values one byte each)
typedef struct {
    unsigned char r,g,b;
} color_t;

// all values important to a framebuffer
typedef struct {
    volatile uint32_t* framebuffer;
    const uint64_t framebuffer_size;
    const int pitch; // this is in pixels NOT bytes
    const int width;
    const int height;
} framebuffer_t;

// initialised the global physical fb and virtual fb pointers [could be dangerous?]
framebuffer_t* fbptr = nullptr;
framebuffer_t* vfbptr = nullptr;

// just some built in colors, not that useful as of now
const color_t white = {.r = 255,.g = 255,.b = 255};
const color_t baby_blue = {.r = 153, .g = 204, .b = 255};
const color_t black = {.r = 0, .g = 0, .b = 0};

// prints pixel in given location (not that useful)
inline void print_pixel(const uint16_t x, const uint16_t y, const color_t color) {
    framebuffer_t fb = *fbptr;
    uint64_t where = x + y*fb.pitch;
    if (where*4 >= fb.framebuffer_size) {
        where = fb.framebuffer_size - 1; // stop memory overflow out of framebuffer (not really - should fix this)
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

// loops thorugh ssfn render until a string is fully rendered
inline void print_string(ssfn_buf_t buf, char* string){
    int bytes_left = ssfn_render(&ctx, &buf, string);
    if (bytes_left > 0) print_string(buf, string + bytes_left);
}

// calls print_string with info about current line to print without manually putting in info into the ssfn buffer or context
inline unsigned int print_ln(color_t color, uint8_t opacity, char* string, uint8_t currentLine) {
    uint32_t ssfn_color = opacity << 24 | color.r << 16 | color.g << 8 | color.b;
    ssfn_buffer.x = 0;
    ssfn_buffer.y = SIZE * (currentLine+1);
    ssfn_buffer.fg = ssfn_color;
    print_string(ssfn_buffer, string);
    memcpy((void*)fbptr->framebuffer, (void*)vfbptr->framebuffer, fbptr->framebuffer_size);
    return ++currentLine;
}

// this just has quick use values for color. opacity
#define print_shortcut(string, currentLine) (print_ln(white, 0xFF, string, currentLine))
// shorthand for assigning currentLine to return value of print_ln
#define print(string) \
    currentLine = print_shortcut(string, currentLine)

// prints a black rectangle the size of the fb
inline uint8_t clear_screen() {
    const framebuffer_t fb = *fbptr;
    print_rectangle(0, fb.height, 0, fb.width, black);
    return 0;
}

