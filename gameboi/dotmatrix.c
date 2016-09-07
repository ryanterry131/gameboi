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

#include <SFML/Graphics.h> // for sfRenderWindow

void lcd_initialize(struct gb_dotmatrix* lcd)
{
    sfVideoMode videoMode = {LCD_WIDTH, LCD_HEIGHT, 2};
    sfContextSettings settings = {1, 1, 1, 1, 0, 0};
    lcd->window = sfRenderWindow_create(videoMode, "Gameboi Emulator", 0, &settings);
}

void lcd_teardown(struct gb_dotmatrix* lcd)
{
    free(lcd->window);
    free(lcd);
}

void lcd_tick(struct gb_dotmatrix* lcd)
{
    sfEvent event;
    while(sfRenderWindow_pollEvent(lcd->window, &event))
    {
        if(event.type == sfEvtClosed)
        {
            // If we clicked the close button, close this window.
            printf("Gameboi has been quit!\n");
            sfRenderWindow_close(lcd->window);
        }
    }
}

void lcd_drawFrame(struct gb_dotmatrix* lcd)
{
    sfColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    sfRenderWindow_clear(lcd->window, white);
    
    lcd_update_framebuffer(lcd);
    
    sfRenderWindow_display(lcd->window);
}

void lcd_update_framebuffer(struct gb_dotmatrix* lcd)
{
    
}