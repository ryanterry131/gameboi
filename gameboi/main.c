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
#include <GLFW/glfw3.h> // for glfw


int main(int argc, const char * argv[])
{
    glfwInit();
    
    if(!gb_system_boot())
    {
        printf("Failed to boot! Aborting...\n");
        return 1;
    }
    
    while(!glfwWindowShouldClose(gameboy->lcd->renderWindow) && !gameboy->stopped)
    {
        gb_system_loop();
    }
    
    printf("Shutting down...\n");
    gb_system_shutdown();
    
    glfwTerminate();
    
    return 0;
}
