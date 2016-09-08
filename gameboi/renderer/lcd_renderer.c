//
//  lcd_renderer.c
//  gameboi
//
//  Created by Ryan Terry on 9/7/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "shader.h"
#include "mesh.h"
#include "lcd_renderer.h"

static struct mesh lcd_mesh;

void renderer_initialize()
{
    initialize_shaders();
    
    vertex vertices[4] =
    {
        {-1.0F, -1.0F, 0.0F},
        {1.0F, -1.0F, 0.0F},
        {-1.0F, 1.0F, 0.0F},
        {1.0F, 1.0F, 0.0F}
    };
    lcd_mesh = create_mesh(vertices, sizeof(vertices)/sizeof(vertex));
}

void renderer_teardown()
{
    teardown_shaders();
    destroy_mesh(&lcd_mesh);
}

void renderer_render()
{
    bind_lcd_shaders();
    draw_mesh(&lcd_mesh, GL_TRIANGLE_STRIP);
}