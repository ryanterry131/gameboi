//
//  dotmatrix.h
//  gameboi
//
//  Created by Ryan Terry on 9/6/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef dotmatrix_h
#define dotmatrix_h

#define LCD_WIDTH       160
#define LCD_HEIGHT      144
#define LCD_SCALE       1

#include <GLFW/glfw3.h> // for glfw

struct gb_dotmatrix
{
    // The main GLFW window for rendering
    GLFWwindow* renderWindow;
};

void lcd_initialize(struct gb_dotmatrix* lcd);
void lcd_teardown(struct gb_dotmatrix* lcd);

void lcd_tick(struct gb_dotmatrix* lcd);
void lcd_display(struct gb_dotmatrix* lcd);
void lcd_draw(struct gb_dotmatrix* lcd);

#endif /* dotmatrix_h */
