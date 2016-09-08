static const GLchar* m_vertex_shader =
"#version 120\n"
"attribute vec3 position;\n"
"void main(){\n"
"   gl_Position = vec4(position, 1.0);\n"
"}";

static const GLchar* m_fragment_shader =
"#version 120\n"
"void main(){\n"
"   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
"}";