#pragma once
#include <stddef.h>
#include <stdint.h>
// color "type (takes red green and blue values one byte each)
typedef struct {
    unsigned char r,g,b;
} color_t;

typedef struct {
    volatile uint32_t* framebuffer;
    const uint64_t framebuffer_size;
    const uint64_t pitch;
    const uint64_t width;
    const uint64_t height;
} framebuffer_t;
// the below code was generated using AI
uint8_t font[96][16];

// the below code was NOT generated with AI
unsigned int currentLine = 0;
framebuffer_t* fbptr = NULL;
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

// print_char basically prints an 8*16 rectangle and only fills in pixels if the corresponding coordinate in the font for said character has a pixel
inline void print_char(const uint16_t x1, const uint16_t y1, const color_t color, const unsigned char ascii_character) {
    framebuffer_t fb = *fbptr;
    const unsigned char character = ascii_character - 32;
    uint64_t where = x1 + y1*fb.pitch;
    if (where*4 >= fb.framebuffer_size) {
        where = fb.framebuffer_size - 1;
    }
    uint32_t pixel_col = color.r << 16 | color.g << 8 | color.b;
    for (int i = 0; i <= 16; i++) {
        uint8_t fontline = font[character][i];
        for (int j = 0; j <= 8; j++) {
            if ((fontline >> j) & 1){
                fb.framebuffer[where+(8-j)] = pixel_col;
            }
        }
        where += fb.pitch;
    }
}

inline void print_string(const uint16_t x, const uint16_t y,const color_t color, char* string) {
    framebuffer_t fb = *fbptr;
    uint64_t where = x + y*fb.pitch;
    if (where*4 >= fb.framebuffer_size) {
        where = fb.framebuffer_size - 1;
    }
    uint32_t pixel_col = color.r << 16 | color.g << 8 | color.b;
    while (*string != '\0'){
        if (*string == '\n') {
            where += 17*fb.pitch - 16;
            *string = ' ';
        }
        const unsigned char ascii_character = *string;
        const unsigned char character = ascii_character - 32;
        for (int j = 0; j <= 16; j++) {
            uint8_t fontline = font[character][j];
            for (int k = 0; k <= 8; k++) {
                if ((fontline >> k) & 1){
                    fb.framebuffer[where+(8-k)] = pixel_col;
                }
            }
            where += fb.pitch;
        }
        where += 8 - 17*fb.pitch;
        string++;
    }

}

inline void println(const color_t color, char* string) {
    print_string(0, currentLine*16, color, string);
    currentLine++;
}

inline void clear_screen() {
    const framebuffer_t fb = *fbptr;
    print_rectangle(0, fb.height, 0, fb.width, black);
    currentLine = 0;
}