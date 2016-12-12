//
//  dotmatrix.c
//  gameboi
//
//  Created by Ryan Terry on 9/6/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "renderer/lcd_renderer.h"
#include "dotmatrix.h"

#include <stdio.h> // for printf
#include <stdlib.h> // for malloc

void lcd_initialize(struct gb_dotmatrix* lcd)
{
    lcd->renderWindow = glfwCreateWindow(LCD_WIDTH, LCD_HEIGHT, "Gameboi", NULL, NULL);
    glfwMakeContextCurrent(lcd->renderWindow);
    
    renderer_initialize();
}

void lcd_teardown(struct gb_dotmatrix* lcd)
{
    renderer_teardown();
    //free(lcd->renderWindow); // glfw deallocs windows for you
    free(lcd);
}

void lcd_tick(struct gb_dotmatrix* lcd)
{
    glfwPollEvents();
}

void lcd_refresh(struct gb_dotmatrix* lcd)
{
    // displays the rendered screen drawn into the back buffer
    renderer_render();
    glfwSwapBuffers(lcd->renderWindow);
}
