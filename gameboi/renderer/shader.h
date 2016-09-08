//
//  shader.h
//  gameboi
//
//  Created by Ryan Terry on 9/7/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include <GLFW/glfw3.h> // for gl


struct shader
{
    GLuint shader_id;
    const GLchar** shader_source;
    GLint source_length;
};

void initialize_shaders();
void teardown_shaders();

void bind_lcd_shaders();
struct shader create_shader(GLuint program, GLenum shader_type, const GLchar** shader_source);

#endif /* shader_h */
