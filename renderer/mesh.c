//
//  mesh.c
//  gameboi
//
//  Created by Ryan Terry on 9/7/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#include "mesh.h"


struct mesh create_mesh(vertex* vertices, GLuint vertex_count)
{
    struct mesh _mesh;
    _mesh.vertex_count = vertex_count;
    
    glGenVertexArrays(1, &_mesh.vertex_array);
    glBindVertexArray(_mesh.vertex_array);
    
    glGenBuffers(1, &_mesh.position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _mesh.position_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(*vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
    
    return _mesh;
}

void destroy_mesh(struct mesh* _mesh)
{
    glDeleteVertexArrays(1, &_mesh->vertex_array);
}

void draw_mesh(struct mesh* _mesh, GLenum render_mode)
{
    glBindVertexArray(_mesh->vertex_array);
    
    glDrawArrays(render_mode, 0, _mesh->vertex_count);
    
    glBindVertexArray(0);
}
