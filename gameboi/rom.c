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


bool gb_rom_load(const char* path)
{
    FILE* rom = fopen(path, "rb");
    
    if(!rom)
    {
        printf("Invalid ROM!\n");
        return false;
    }
    
    gameboy->current_rom = (struct gb_rom*) malloc(sizeof(struct gb_rom));
    
    gameboy->current_rom->path = (char*) path;
    struct stat s;
    stat(path, &s);
    gameboy->current_rom->rom_size = s.st_size;
    
    gameboy->current_rom->rawBytes = (byte*) malloc(gameboy->current_rom->rom_size);
    fread(gameboy->current_rom->rawBytes, gameboy->current_rom->rom_size, 1, rom);
    
    {
        // setup rom header
        struct rom_header* header = &gameboy->current_rom->header;
        gb_rom_read32(&header->entryCode, gameboy->current_rom->rawBytes, 0x100);
        gb_rom_readBytes(header->nintendoLogo, gameboy->current_rom->rawBytes, 0x104, 0x30);
        gb_rom_readChars(header->gameTitle, gameboy->current_rom->rawBytes, 0x134, 0x10);
        gb_rom_read16(&header->newLicensee, gameboy->current_rom->rawBytes, 0x144);
        gb_rom_read8(&header->sgbFunction, gameboy->current_rom->rawBytes, 0x146);
        gb_rom_read8(&header->cartridgeType, gameboy->current_rom->rawBytes, 0x147);
        gb_rom_read8(&header->ROMsize, gameboy->current_rom->rawBytes, 0x148);
        gb_rom_read8(&header->RAMsize, gameboy->current_rom->rawBytes, 0x149);
        gb_rom_read8(&header->international, gameboy->current_rom->rawBytes, 0x14A);
        gb_rom_read8(&header->oldLicensee, gameboy->current_rom->rawBytes, 0x14B);
        gb_rom_read8(&header->versionCode, gameboy->current_rom->rawBytes, 0x14C);
        gb_rom_read8(&header->headerChecksum, gameboy->current_rom->rawBytes, 0x14D);
        gb_rom_read16(&header->globalChecksum, gameboy->current_rom->rawBytes, 0x14E);
    }
    
    gb_rom_print_header();
    
    if(!gb_rom_validate_checksum())
    {
        printf("ERROR: Invalid checksum in ROM header! Aborting...\n");
        return false;
    }
    
    printf("Checksum OK!\n");
    
    return true;
}

void gb_rom_print_header()
{
    printf("Entry Code:%#08x\n", gameboy->current_rom->header.entryCode);
    printf("Logo:");
    for(int i = 0; i <= 0x30; i++)
    {
        printf("%02X", gameboy->current_rom->header.nintendoLogo[i]);
        if(i == 0x30)
        {
            // flush
            printf("\n");
        }
    }
    printf("Title:%s\n", gameboy->current_rom->header.gameTitle);
    printf("New Licensee:%#04x\n", gameboy->current_rom->header.newLicensee);
    printf("Use SGB:%#01x\n", gameboy->current_rom->header.sgbFunction);
    printf("Cartridge Type:%#01x\n", gameboy->current_rom->header.cartridgeType);
    printf("ROM size:%#01x\n", gameboy->current_rom->header.ROMsize);
    printf("RAM size:%#01x\n", gameboy->current_rom->header.RAMsize);
    printf("International:%#01x\n", gameboy->current_rom->header.international);
    printf("Old Licensee:%#01x\n", gameboy->current_rom->header.oldLicensee);
    printf("Version Code:%#01x\n", gameboy->current_rom->header.versionCode);
    printf("Header Checksum:%#01x\n", gameboy->current_rom->header.headerChecksum);
    printf("Global Checksum:%#02x\n", gameboy->current_rom->header.globalChecksum);
}

bool gb_rom_validate_checksum()
{
    int checksum = 0;
    for(int i = 0x134; i <= 0x14C; i++)
    {
        checksum = checksum - gameboy->current_rom->rawBytes[i] - 1;
    }
    
    return ((checksum & 0xFF) == gameboy->current_rom->header.headerChecksum);
}

void gb_rom_read8(u8* destination, const byte* src, int offset)
{
    *destination = src[offset];
}
void gb_rom_read16(u16* destination, const byte* src, int offset)
{
    memcpy(destination, src + offset, sizeof(*destination));
}
void gb_rom_read32(u32* destination, const byte* src, int offset)
{
    memcpy(destination, src + offset, sizeof(*destination));
}
void gb_rom_readChars(char* destination, const byte* src, int offset, int bytes)
{
    memcpy(destination, src + offset, bytes);
}
void gb_rom_readBytes(byte* destination, const byte* src, int offset, int bytes)
{
    memcpy(destination, src + offset, bytes);
}
