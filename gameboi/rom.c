//
//  rom.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "system.h"
#include "rom.h"

#include <stdlib.h> // for malloc
#include <stdio.h> // for printf and FILE
#include <string.h> // for memcpy
#include <sys/stat.h> // for stat


bool rom_load(struct gb_rom** rom_ptr, const char* path)
{
    FILE* rom_file = fopen(path, "rb");
    
    if(!rom_file)
    {
        printf("Invalid ROM!\n");
        return false;
    }
    
    *rom_ptr = (struct gb_rom*) malloc(sizeof(struct gb_rom));
    
    (*rom_ptr)->path = (char*) path;
    struct stat s;
    stat(path, &s);
    (*rom_ptr)->rom_size = s.st_size;
    
    gameboy->current_rom->rawBytes = (byte*) malloc(gameboy->current_rom->rom_size);
    fread(gameboy->current_rom->rawBytes, gameboy->current_rom->rom_size, 1, rom_file);
    
    {
        // setup rom header
        struct rom_header* header = &((*rom_ptr)->header);
        rom_read32(*rom_ptr, &header->entryCode, 0x100);
        rom_readBytes(*rom_ptr, header->nintendoLogo, 0x104, 0x30);
        rom_readChars(*rom_ptr, header->gameTitle, 0x134, 0x10);
        rom_read16(*rom_ptr, &header->newLicensee, 0x144);
        rom_read8(*rom_ptr, &header->sgbFunction, 0x146);
        rom_read8(*rom_ptr, &header->cartridgeType, 0x147);
        rom_read8(*rom_ptr, &header->ROMsize, 0x148);
        rom_read8(*rom_ptr, &header->RAMsize, 0x149);
        rom_read8(*rom_ptr, &header->international, 0x14A);
        rom_read8(*rom_ptr, &header->oldLicensee, 0x14B);
        rom_read8(*rom_ptr, &header->versionCode, 0x14C);
        rom_read8(*rom_ptr, &header->headerChecksum, 0x14D);
        rom_read16(*rom_ptr, &header->globalChecksum, 0x14E);
    }
    
    rom_print_header(*rom_ptr);

    return true;
}

void rom_close(struct gb_rom* rom)
{
    free(rom->rawBytes);
    free(rom);
}

void rom_print_header(struct gb_rom* rom)
{
    printf("Entry Code:%#08x\n", rom->header.entryCode);
    printf("Logo:");
    for(int i = 0; i <= 0x30; i++)
    {
        printf("%02X", rom->header.nintendoLogo[i]);
        if(i == 0x30)
        {
            // flush
            printf("\n");
        }
    }
    printf("Title:%s\n", rom->header.gameTitle);
    printf("New Licensee:%#04x\n", rom->header.newLicensee);
    printf("Use SGB:%#01x\n", rom->header.sgbFunction);
    printf("Cartridge Type:%#01x\n", rom->header.cartridgeType);
    printf("ROM size:%#01x\n", rom->header.ROMsize);
    printf("RAM size:%#01x\n", rom->header.RAMsize);
    printf("International:%#01x\n", rom->header.international);
    printf("Old Licensee:%#01x\n", rom->header.oldLicensee);
    printf("Version Code:%#01x\n", rom->header.versionCode);
    printf("Header Checksum:%#01x\n", rom->header.headerChecksum);
    printf("Global Checksum:%#02x\n", rom->header.globalChecksum);
}

bool rom_validate_checksum(struct gb_rom* rom)
{
    int checksum = 0;
    for(int i = 0x134; i <= 0x14C; i++)
    {
        checksum = checksum - rom->rawBytes[i] - 1;
    }
    
    return ((checksum & 0xFF) == rom->header.headerChecksum);
}

void rom_read8(struct gb_rom* rom, u8* destination, int offset)
{
    *destination = rom->rawBytes[offset];
}
void rom_read16(struct gb_rom* rom, u16* destination, int offset)
{
    memcpy(destination, rom->rawBytes + offset, sizeof(*destination));
}
void rom_read32(struct gb_rom* rom, u32* destination, int offset)
{
    memcpy(destination, rom->rawBytes + offset, sizeof(*destination));
}
void rom_readChars(struct gb_rom* rom, char* destination, int offset, int bytes)
{
    memcpy(destination, rom->rawBytes + offset, bytes);
}
void rom_readBytes(struct gb_rom* rom, byte* destination, int offset, int bytes)
{
    memcpy(destination, rom->rawBytes + offset, bytes);
}
