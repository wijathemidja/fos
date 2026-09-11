#include <uefi.h>
#include <stdbool.h>
#include <stdint.h>

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
    gop->SetMode(gop, selectMode);
    if (EFI_ERROR(status)) {
        printf("Can't set mode\n");
        return 1;
    }
    volatile uint64_t* framebuffer = (volatile uint64_t*)(gop->Mode->FrameBufferBase);
    const uint64_t framebuffer_size = gop->Mode->FrameBufferSize;
    const efi_gop_pixel_format_t pixel_format = gop->Mode->Information->PixelFormat;
    const uint32_t pitch = gop->Mode->Information->PixelsPerScanLine;
    while (true) {};
    return 0;
}


