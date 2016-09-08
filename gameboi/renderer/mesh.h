//
//  mesh.h
//  gameboi
//
//  Created by Ryan Terry on 9/7/16.
//  Copyright © 2016 Byteandahalf. All rights reserved.
//

#ifndef mesh_h
#define mesh_h

#include <OpenGL/gl3.h> // for gl

typedef struct
{
    GLfloat _x, _y, _z;
} vertex;

struct mesh
{
    GLuint vertex_array;
    GLuint position_buffer;
    GLuint vertex_count;
};

struct mesh create_mesh(vertex* vertices, GLuint vertex_count);
void destroy_mesh(struct mesh* _mesh);
void draw_mesh(struct mesh* _mesh, GLenum render_mode);

#endif /* mesh_h */
