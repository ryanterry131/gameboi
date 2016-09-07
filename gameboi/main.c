//
//  main.c
//  gameboi
//
//  Created by Ryan Terry on 8/25/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "system.h"
#include "dotmatrix.h"

#include <stdio.h> // for printf
#include <SFML/Graphics/RenderWindow.h> // for sfRenderWindow

int main(int argc, const char * argv[])
{    
    if(!gb_system_boot())
    {
        printf("Failed to boot! Aborting...\n");
        return 1;
    }
    
    while(sfRenderWindow_isOpen(gameboy->lcd->window) && !gameboy->stopped)
    {
        gb_system_loop();
    }
    
    printf("Shutting down...\n");
    gb_system_shutdown();
    
    return 0;
}
