//
//  rom.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include <stdlib.h>
#include <sys/stat.h>

#include "system.h"
#include "dataio.h"
#include "rom.h"


BOOL gb_rom_load(const char* path)
{
    FILE* rom = fopen(path, "rb");
    
    if(!rom)
    {
        printf("Invalid ROM!\n");
        return FALSE;
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
        read32(&header->entryCode, gameboy->current_rom->rawBytes, 0x100);
        readBytes(header->nintendoLogo, gameboy->current_rom->rawBytes, 0x104, 0x30);
        readChars(header->gameTitle, gameboy->current_rom->rawBytes, 0x134, 0x10);
        read16(&header->newLicensee, gameboy->current_rom->rawBytes, 0x144);
        read8(&header->sgbFunction, gameboy->current_rom->rawBytes, 0x146);
        read8(&header->cartridgeType, gameboy->current_rom->rawBytes, 0x147);
        read8(&header->ROMsize, gameboy->current_rom->rawBytes, 0x148);
        read8(&header->RAMsize, gameboy->current_rom->rawBytes, 0x149);
        read8(&header->international, gameboy->current_rom->rawBytes, 0x14A);
        read8(&header->oldLicensee, gameboy->current_rom->rawBytes, 0x14B);
        read8(&header->versionCode, gameboy->current_rom->rawBytes, 0x14C);
        read8(&header->headerChecksum, gameboy->current_rom->rawBytes, 0x14D);
        read16(&header->globalChecksum, gameboy->current_rom->rawBytes, 0x14E);
    }
    
    gb_rom_print_header();
    
    if(!gb_rom_validate_checksum())
    {
        printf("ERROR: Invalid checksum in ROM header! Aborting...\n");
        return FALSE;
    }
    
    printf("Checksum OK!\n");
    
    return TRUE;
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

BOOL gb_rom_validate_checksum()
{
    int checksum = 0;
    for(int i = 0x134; i <= 0x14C; i++)
    {
        checksum = checksum - gameboy->current_rom->rawBytes[i] - 1;
    }
    
    return ((checksum & 0xFF) == gameboy->current_rom->header.headerChecksum);
}
