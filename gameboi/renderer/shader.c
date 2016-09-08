//
//  shader.c
//  gameboi
//
//  Created by Ryan Terry on 9/7/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "shader.h"

#include <stdio.h>
#include <string.h>

#include "shader_sources.h"

static struct shader vertex_shader, fragment_shader;
static GLuint shader_program;

void initialize_shaders()
{
    shader_program = glCreateProgram();
    
    vertex_shader = create_shader(shader_program, GL_VERTEX_SHADER, &m_vertex_shader);
    fragment_shader = create_shader(shader_program, GL_FRAGMENT_SHADER, &m_fragment_shader);
    
    glLinkProgram(shader_program);
    glValidateProgram(shader_program);
}

void teardown_shaders()
{
    {
        glDetachShader(shader_program, vertex_shader.shader_id);
        glDetachShader(shader_program, fragment_shader.shader_id);
        glDeleteShader(vertex_shader.shader_id);
        glDeleteShader(fragment_shader.shader_id);
    }
    glDeleteShader(shader_program);
}

void bind_lcd_shaders()
{
    glUseProgram(shader_program);
}

struct shader create_shader(GLuint program, GLenum shader_type, const GLchar** shader_source)
{
    struct shader _shader = {glCreateShader(shader_type), shader_source, (GLint) strlen(*shader_source)};
    
    glShaderSource(_shader.shader_id, 1, _shader.shader_source, &_shader.source_length);
    
    glCompileShader(_shader.shader_id);
    GLint success;
    GLchar error[1024];
    glGetShaderiv(_shader.shader_id, GL_COMPILE_STATUS, &success);
    
    if(success == GL_FALSE)
    {
        glGetShaderInfoLog(_shader.shader_id, sizeof(error), NULL, error);
        printf("Error compiling shader:\n%s\n", error);
    }
    
    glAttachShader(program, _shader.shader_id);
    
    return _shader;
}
