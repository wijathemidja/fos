#include <uefi.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    unsigned char r,g,b;
} color_t;

void print_pixel(volatile uint32_t* framebuffer, const uint64_t framebuffer_size, const uint64_t pitch, const uint16_t x, const uint16_t y, const struct color_t color) {
    uint64_t where = x + y*pitch;
    if (where*4 >= framebuffer_size) {
        where = framebuffer_size - 1; // stop memory overflow out of framebuffer
    }
    uint32_t pixel_col = color.r << 16 | color.g << 8 | color.b;
    framebuffer[where] = pixel_col;
}

int main() {
    printf("fos.\n");
    printf("Launched fos bootloader.\n");
    printf("Version 0.0.1\n");
    efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    efi_gop_t* gop;
    efi_status_t status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        printf("Can't find GOP\n");
        return 1;
    }
    int area = 0;
    printf("Found GOP\n");
    efi_gop_mode_info_t *info;
    uint64_t SizeOfInfo;
    uint32_t maxMode = gop->Mode->MaxMode;
    for (int i =0; i < maxMode; i ++) {
        status = gop->QueryMode(gop, i, &SizeOfInfo, &info);
        if (EFI_ERROR(status)) {
            printf("Can't get mode info\n");
        } else {
            printf("Successfully got mode %d info \n",i);
            printf("%dx%d \n \n",info->HorizontalResolution,info->VerticalResolution);
            if (info->HorizontalResolution * 9 == info->VerticalResolution * 16) {
                if (i > area) {
                    printf("Replaced resolution with %dx%d\n",info->HorizontalResolution,info->VerticalResolution);
                    area = i;
                }

            }
        }
    }
    int selectMode = area;
    printf("Selected target mode found\n");
    status = gop->SetMode(gop, selectMode);
    if (EFI_ERROR(status)) {
        printf("Can't set mode\n");
        return 1;
    }
    volatile uint32_t* framebuffer = (volatile uint32_t*)(gop->Mode->FrameBufferBase);
    const uint64_t framebuffer_size = gop->Mode->FrameBufferSize;
    const uint32_t pitch = gop->Mode->Information->PixelsPerScanLine;
    while (true) {};
    return 0;
}





