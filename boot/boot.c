#include <uefi.h>
#include <stdint.h>
#include "graphics.h"

#define SSFN_IMPLEMENTATION
#include "ssfn.h"

typedef struct {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Header;

enum Elf64_Ident {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_OSABI,
    EI_ABIVERSION,
    EI_PAD,
};

typedef struct {
    bool valid;
    char* error;
} bool_error;

bool_error valid_kernel_elf(Elf64_Header *header) {
    if (!header) return (bool_error){.valid = false, .error = "Null file"};
    if (header->e_ident[EI_MAG0] != 0x7F) return (bool_error){.valid = false, .error = "Magic number wrong MAG0"};
    if (header->e_ident[EI_MAG1] != 0x45) return (bool_error){.valid = false, .error = "Magic number wrong MAG1"};
    if (header->e_ident[EI_MAG2] != 0x4C) return (bool_error){.valid = false, .error = "Magic number wrong MAG2"};
    if (header->e_ident[EI_MAG3] != 0x46) return (bool_error){.valid = false, .error = "Magic number wrong MAG3"};
    if (header->e_ident[EI_CLASS] != 2) return (bool_error){.valid = false, .error = "Wrong EI_CLASS"};
    if (header->e_ident[EI_DATA] != 1) return (bool_error){.valid = false, .error = "Wrong EI_DATA"};
    if (header->e_ident[EI_VERSION] != 1) return (bool_error){.valid = false, .error = "Wrong EI_VERSION"};
    if (header->e_ident[EI_OSABI] != 0x00) return (bool_error){.valid = false, .error = "Wrong OS ABI"};
    return (bool_error){.valid = true, .error = nullptr};
}


int main() {
    printf("fos.\n");
    printf("Launched fos bootloader.\n");
    printf("Version 0.0.4\n");
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
    const int selectMode = area;
    printf("Selected target mode found\n");
    status = gop->SetMode(gop, selectMode);
    if (EFI_ERROR(status)) {
        printf("Can't set mode\n");
        return 1;
    }
    volatile uint32_t* const framebuffer = (volatile uint32_t*)(gop->Mode->FrameBufferBase);
    const uint64_t framebuffer_size = gop->Mode->FrameBufferSize;
    const int pitch = (int)gop->Mode->Information->PixelsPerScanLine;
    const int width = (int)gop->Mode->Information->HorizontalResolution;
    const int height = (int)gop->Mode->Information->VerticalResolution;
    framebuffer_t framebufferInstance = {.framebuffer = framebuffer, .framebuffer_size = framebuffer_size, .pitch = pitch, .width = width, .height = height};
    fbptr = &framebufferInstance;
    // now make a virtual framebuffer for text rendering which then goes to the actual framebuffer
    volatile uint32_t* const virtualframebuffer = malloc(framebuffer_size);
    memset((void*)virtualframebuffer, 0, framebuffer_size);
    framebuffer_t virtualframebuffer_Instance = {.framebuffer = virtualframebuffer, .framebuffer_size = framebuffer_size, .pitch = pitch, .width = width, .height = height};
    vfbptr = &virtualframebuffer_Instance;
    FILE *font_file = fopen("jetbrains_reg.sfn", "r");
    if (font_file == NULL) {
        printf("Can't open font file\n");
        printf("fopen failed. errno: %d \n", errno);
        return 1;
    }
    fseek(font_file, 0, SEEK_END);
    size_t font_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);
    unsigned char* chosen_font = malloc(font_size);
    fread(chosen_font, font_size, 1, font_file);
    fclose(font_file);
    unsigned int currentLine = 0;
    ssfn_load(&ctx, chosen_font);
    ssfn_select(&ctx, SSFN_FAMILY_ANY, nullptr, SSFN_STYLE_REGULAR, SIZE);
    ssfn_buffer.ptr = (unsigned char*)vfbptr->framebuffer;
    ssfn_buffer.h = vfbptr->height;
    ssfn_buffer.w = vfbptr->width;
    ssfn_buffer.p = vfbptr->pitch*4;
    print("fos");
    print("fos framebuffer has been initialised!");
    print("Version 0.0.4");
    // memory map setup
    efi_memory_descriptor_t* memory_map = nullptr;
    uint64_t memory_map_size = 0;
    uint64_t mapKey = 0;
    uint64_t mem_des_size = 0;
    uint32_t mem_des_ver = 0;
    status = BS->GetMemoryMap(&memory_map_size, memory_map, &mapKey, &mem_des_size, &mem_des_ver);
    if (EFI_ERROR(status)) {
        print("Can't get memory map (this is expected, buffer is too small)\n");
        print("Reallocating mem");
        memory_map_size += mem_des_size * 4; // padding for the reallocation of memory map affecting memory map's own size
        efi_memory_descriptor_t* tmp = realloc(memory_map, memory_map_size);
        if (tmp == NULL) {
            print("Can't reallocate memory map");
        } else {
            memory_map = tmp;
        }
        status = BS->GetMemoryMap(&memory_map_size, memory_map, &mapKey, &mem_des_size, &mem_des_ver);
        if (EFI_ERROR(status)) {
            print("Can't get memory map\n");
            return 1;
        }
        print("Successfully reallocated");
    }
    print("Found memory map!\n");
    print("Time to load kernel.");
    // open file stream
    FILE *kernel_file = fopen("fos.elf", "r");
    // check for open failure
    if (kernel_file == NULL) {
        print("Can't open kernel file");
        printf("fopen failed. errno: %d \n", errno);
        return 1;
    }

    // find size of kernel file
    fseek(kernel_file, 0, SEEK_END);
    size_t kernel_file_size = ftell(kernel_file);
    // make buffer for kernel file to be loaded into ram
    Elf64_Header kernel_header;
    // load file into buffer
    unsigned char* kernel_buffer = malloc(kernel_file_size);
    fseek(kernel_file, 0, SEEK_SET);
    fread(kernel_buffer, sizeof(unsigned char), kernel_file_size, kernel_file);
    fseek(kernel_file, 0, SEEK_SET);
    memcpy(&kernel_header, kernel_buffer, sizeof(Elf64_Header));
    print("Successfully read kernel file");
    print("Now need to read ELF header.");
    bool_error check_elf = valid_kernel_elf(&kernel_header);
    if (check_elf.valid == false) {
        print(check_elf.error);
        return 1;
    }
    print("Successfully read ELF header & valid elf.");
    while (true) {};
    return 0;
}





