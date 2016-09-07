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


struct sfRenderWindow;

struct gb_dotmatrix
{
    struct sfRenderWindow* window;
};

void lcd_initialize(struct gb_dotmatrix* lcd);
void lcd_teardown(struct gb_dotmatrix* lcd);

void lcd_tick(struct gb_dotmatrix* lcd);
void lcd_update_framebuffer(struct gb_dotmatrix* lcd);
void lcd_drawFrame(struct gb_dotmatrix* lcd);

#endif /* dotmatrix_h */
