//
//  dotmatrix.c
//  gameboi
//
//  Created by Ryan Terry on 9/6/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "dotmatrix.h"

#include <stdio.h> // for printf
#include <stdlib.h> // for malloc

void lcd_initialize(struct gb_dotmatrix* lcd)
{
    lcd->renderWindow = glfwCreateWindow(160, 144, "Gameboi", NULL, NULL);
    glfwMakeContextCurrent(lcd->renderWindow);
    // make the screen render white by default
    glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
}

void lcd_teardown(struct gb_dotmatrix* lcd)
{
    //free(lcd->renderWindow); // glfw deallocs windows for you
    free(lcd);
}

void lcd_tick(struct gb_dotmatrix* lcd)
{
}

void lcd_drawFrame(struct gb_dotmatrix* lcd)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(lcd->renderWindow);
    glfwPollEvents();
}

void lcd_update_framebuffer(struct gb_dotmatrix* lcd)
{
    
}
