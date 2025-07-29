#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#include <windows.h>
#endif

#include "include/glad/glad.h"

#define RGFW_IMPLEMENTATION
#define RGFW_NO_IOKIT
#include "include/RGFW.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, w;
} Vec3f;

typedef struct {
    float x, y, w, z;
} Vec4f;

typedef struct {
    Vec3f position;
    Vec2f uv;
    Vec4f color;
} Vertex;

#define WIDTH 600
#define HEIGHT 600

char* read_file_as_string(const char* file_path) {
    int error = 0;
    FILE* f = fopen(file_path, "r");
    if (f == NULL)
        goto error;

    long previous_position = ftell(f);
    if (previous_position < 0)
        goto error;

    if (fseek(f, 0, SEEK_END) < 0)
        goto error;

    long file_size = ftell(f);
    if (file_size < 0)
        goto error;

    if (fseek(f, previous_position, SEEK_SET) < 0)
        goto error;

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "malloc failed! go buy more RAM fool LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOL\n");
        exit(1);
    }
    memset((void*)buffer, 0, sizeof(buffer[0]) * file_size + 1);

    fread(buffer, file_size, 1, f);
    if (ferror(f))
        goto error;
    
    if (f)
        fclose(f);

    return buffer;

error:
    error = errno;
    if (f)
        fclose(f);
    fprintf(stderr, "error reading file %s: %s\n", file_path, strerror(error));
    return NULL;
}

int main(void) {
    const char* triangles_frag_file_path = "../shaders/triangles.frag";
    const char* triangles_vert_file_path = "../shaders/triangles.vert";
    const char* texture_frag_file_path = "../shaders/texture.frag";
    const char* texture_vert_file_path = "../shaders/texture.vert";

    char* triangles_frag_src = read_file_as_string(triangles_frag_file_path);
    if (triangles_frag_src == NULL)
        return 1;

    char* triangles_vert_src = read_file_as_string(triangles_vert_file_path);
    if (triangles_vert_src == NULL)
        return 1;

    char* texture_frag_src = read_file_as_string(texture_frag_file_path);
    if (texture_frag_src == NULL)
        return 1;

    char* texture_vert_src = read_file_as_string(texture_vert_file_path);
    if (texture_vert_src == NULL)
        return 1;

    RGFW_setGLVersion(RGFW_glCore, 3, 3);

	RGFW_window* window = RGFW_createWindow("mario kart GL", RGFW_RECT(WIDTH, HEIGHT, WIDTH, HEIGHT), RGFW_windowAllowDND | RGFW_windowCenter);
    if (window == NULL) {
        fprintf(stderr, "failed to create RGFW window\n");
        return 1;
    }
    RGFW_window_makeCurrent(window);

    if (gladLoadGL() == 0) {
        fprintf(stderr, "failed to initialize GLAD\n");
        return 1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLuint vertex_vao;
    glGenVertexArrays(1, &vertex_vao);
    glBindVertexArray(vertex_vao);

    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);

    const float size = 0.7f;
    float alpha = 0.5f;

    Vertex vertex_buffer[] = {
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .color = (Vec4f){1, 0.5, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, -size},
            .color = (Vec4f){0, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, -size},
            .color = (Vec4f){1, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, size},
            .color = (Vec4f){1, 0, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, size},
            .color = (Vec4f){0, 1, 1, alpha},
        },
    };

    GLuint vertex_indices[] = {
        /* top back triangle */
        0, 1, 2,
        /* bottom back triangle */
        3, 2, 1,
        /* top left triangle */
        0, 2, 4,
        /* bottom left triangle */
        3, 4, 2,
        /* top right triangle */
        0, 5, 1,
        /* bottom right triangle */
        3, 1, 5,
        /* top front triangle */
        0, 4, 5,
        /* bottom front triangle */
        3, 5, 4,
        /* top back triangle */
        0, 2, 1,
        /* bottom back triangle */
        3, 1, 2,
        /* top left triangle */
        0, 4, 2,
        /* bottom left triangle */
        3, 2, 4,
        /* top right triangle */
        0, 1, 5,
        /* bottom right triangle */
        3, 5, 1,
        /* top front triangle */
        0, 5, 4,
        /* bottom front triangle */
        3, 4, 5,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), &vertex_buffer, GL_DYNAMIC_DRAW);

    GLuint vertex_ibo;
    glGenBuffers(1, &vertex_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), &vertex_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    alpha = 0.0f;
    const float uv_size = 0.75f;

    Vertex uv_buffer[] = {
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .uv = (Vec2f){-uv_size, 0},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, 0},
            .uv = (Vec2f){0, -uv_size},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, 0},
            .uv = (Vec2f){0, uv_size},
        },
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .uv = (Vec2f){uv_size, 0},
        },
    };

    GLuint uv_indices[] = {
        /* top triangle */
        0, 1, 2,
        /* bottom triangle */
        3, 2, 1,
        /* top triangle */
        0, 2, 1,
        /* bottom triangle */
        3, 1, 2,
    };

    GLuint uv_vao;
    glGenVertexArrays(1, &uv_vao);
    glBindVertexArray(uv_vao);

    GLuint uv_vbo;
    glGenBuffers(1, &uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer), &uv_buffer, GL_DYNAMIC_DRAW);

    GLuint uv_ibo;
    glGenBuffers(1, &uv_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uv_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uv_indices), &uv_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    const int VERTEX_INDICES_COUNT = sizeof(vertex_indices) / sizeof(vertex_indices[0]);
    const int UV_INDICES_COUNT = sizeof(uv_indices) / sizeof(uv_indices[0]);

    GLuint triangles_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(triangles_frag_shader, 1, (const char**)&triangles_frag_src, NULL);
    glCompileShader(triangles_frag_shader);

    GLint triangles_frag_shader_compiled = 0;
    glGetShaderiv(triangles_frag_shader, GL_COMPILE_STATUS, &triangles_frag_shader_compiled);

    if (!triangles_frag_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(triangles_frag_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile fragment shader: %.*s\n", message_size, message);
        return 1;
    }
    
    GLuint triangles_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(triangles_vert_shader, 1, (const char**)&triangles_vert_src, NULL);
    glCompileShader(triangles_vert_shader);

    GLint triangles_vert_shader_compiled = 0;
    glGetShaderiv(triangles_vert_shader, GL_COMPILE_STATUS, &triangles_vert_shader_compiled);

    if (!triangles_vert_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(triangles_vert_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile vertex shader: %.*s\n", message_size, message);
        return 1;
    }

    GLuint triangles_program = glCreateProgram();

    glAttachShader(triangles_program, triangles_frag_shader);
    glAttachShader(triangles_program, triangles_vert_shader);
    glLinkProgram(triangles_program);

    GLint linked = 0;
    glGetProgramiv(triangles_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetProgramInfoLog(triangles_program, sizeof(message), &message_size, message);
        fprintf(stderr, "could not link triangles_program: %.*s\n", message_size, message);
        return 1;
    }

    glDeleteShader(triangles_frag_shader);
    glDeleteShader(triangles_vert_shader);
    free(triangles_frag_src);
    free(triangles_vert_src);

    GLuint texture_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(texture_frag_shader, 1, (const char**)&texture_frag_src, NULL);
    glCompileShader(texture_frag_shader);

    GLint texture_frag_shader_compiled = 0;
    glGetShaderiv(texture_frag_shader, GL_COMPILE_STATUS, &texture_frag_shader_compiled);

    if (!texture_frag_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(texture_frag_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile fragment shader: %.*s\n", message_size, message);
        return 1;
    }
    
    GLuint texture_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(texture_vert_shader, 1, (const char**)&texture_vert_src, NULL);
    glCompileShader(texture_vert_shader);

    GLint texture_vert_shader_compiled = 0;
    glGetShaderiv(texture_vert_shader, GL_COMPILE_STATUS, &texture_vert_shader_compiled);

    if (!texture_vert_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(texture_vert_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile vertex shader: %.*s\n", message_size, message);
        return 1;
    }

    GLuint texture_program = glCreateProgram();

    glAttachShader(texture_program, texture_frag_shader);
    glAttachShader(texture_program, texture_vert_shader);
    glLinkProgram(texture_program);

    linked = 0;
    glGetProgramiv(texture_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetProgramInfoLog(texture_program, sizeof(message), &message_size, message);
        fprintf(stderr, "could not link texture_program: %.*s\n", message_size, message);
        return 1;
    }

    glDeleteShader(texture_frag_shader);
    glDeleteShader(texture_vert_shader);
    free(texture_frag_src);
    free(texture_vert_src);

    const char* texture_file_name = "../images/questionmark.png";
    int texture_width, texture_height, n;
    unsigned char* texture_data = stbi_load(texture_file_name, &texture_width, &texture_height, &n, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    GLint triangles_time_uniform_location = glGetUniformLocation(triangles_program, "time");
    GLint triangles_resolution_uniform_location = glGetUniformLocation(triangles_program, "resolution");
    
    GLint texture_image_uniform_location = glGetUniformLocation(texture_program, "image");
    GLint texture_time_uniform_location = glGetUniformLocation(texture_program, "time");
    GLint texture_resolution_uniform_location = glGetUniformLocation(texture_program, "resolution");

    glClearColor(0, 0, 0, 0);
    
    glUniform2f(triangles_resolution_uniform_location, WIDTH, HEIGHT);
    glUniform2f(texture_resolution_uniform_location, WIDTH, HEIGHT);
    glUniform1i(texture_image_uniform_location, 0);
    glViewport(0, 0, WIDTH, HEIGHT);

    int width = WIDTH;
    int height = HEIGHT;

    while (RGFW_window_shouldClose(window) == 0) {
        while (RGFW_window_checkEvent(window)) {
            switch (window->event.type) {
                case RGFW_windowResized: {
                    width = window->r.w;
                    height = window->r.h;
                    glUniform2f(triangles_resolution_uniform_location, (float)width, (float)height);
                    glUniform2f(texture_resolution_uniform_location, (float)width, (float)height);
                    glViewport(0, 0, width, height);
                } break;
                default:
                break;
            }
        }

        u64 time = RGFW_getTimeNS() / (u64)1e6;
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(texture_program);
        glUniform1i(texture_image_uniform_location, 0);
        glUniform2f(texture_resolution_uniform_location, (float)width, (float)height);
        glUniform1f(texture_time_uniform_location, (float)time);
        glBindVertexArray(uv_vao);
        glDrawElements(GL_TRIANGLES, UV_INDICES_COUNT, GL_UNSIGNED_INT, 0);
        
        glUseProgram(triangles_program);
        glUniform2f(triangles_resolution_uniform_location, (float)width, (float)height);
        glUniform1f(triangles_time_uniform_location, (float)time);
        glBindVertexArray(vertex_vao);
        glDrawElements(GL_TRIANGLES, VERTEX_INDICES_COUNT, GL_UNSIGNED_INT, 0);
        
        RGFW_window_swapBuffers(window);
        RGFW_window_checkFPS(window, 60);
    }

    RGFW_window_close(window);
    return 0;
}
