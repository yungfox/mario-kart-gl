#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, w, z;
} Vec4f;

typedef struct {
    Vec2f position;
    Vec2f uv;
    Vec4f color;
} Vertex;

#define VERTICES_COUNT 4

char* read_file_as_string(const char* file_path) {
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

    fread(buffer, file_size, 1, f);
    if (ferror(f))
        goto error;
    
    if (f)
        fclose(f);

    return buffer;

error:
    if (f)
        fclose(f);
    return NULL;
}

int main(void) {
    const char* itembox_frag_file_path = "./shaders/itembox.frag";
    const char* itembox_vert_file_path = "./shaders/itembox.vert";
    char* itembox_frag_src = read_file_as_string(itembox_frag_file_path);
    char* itembox_vert_src = read_file_as_string(itembox_vert_file_path);

    SDL_Window* window = SDL_CreateWindow("mario kart GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    int major;
    int minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    printf("GL version %d.%d\n", major, minor);

    if (SDL_GL_CreateContext(window) == NULL) {
        fprintf(stderr, "could not create context for window: %s\n", SDL_GetError());
        return 1;
    }

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        fprintf(stderr, "could not initialize glew: %s\n", glewGetErrorString(glew_err));
        return 1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    Vertex vertex_buffer[VERTICES_COUNT] = {
        (Vertex) {
            .position = (Vec2f){-0.5, 0},
            .uv = (Vec2f){-1, 0},
            .color = (Vec4f){1, 0, 0, 1},
        },
        (Vertex) {
            .position = (Vec2f){0, 0.5},
            .uv = (Vec2f){0, 1},
            .color = (Vec4f){0, 0, 1, 1},
        },
        (Vertex) {
            .position = (Vec2f){0, -0.5},
            .uv = (Vec2f){0, -1},
            .color = (Vec4f){1, 1, 0, 1},
        },
        (Vertex) {
            .position = (Vec2f){0.5, 0},
            .uv = (Vec2f){1, 0},
            .color = (Vec4f){0, 1, 0, 1},
        },
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), &vertex_buffer, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (const char**)&itembox_frag_src, NULL);
    glCompileShader(frag_shader);

    GLint frag_shader_compiled = 0;
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_shader_compiled);

    if (!frag_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(frag_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile shader: %.*s\n", message_size, message);
        return 1;
    }
    
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, (const char**)&itembox_vert_src, NULL);
    glCompileShader(vert_shader);

    GLint vert_shader_compiled = 0;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &vert_shader_compiled);

    if (!vert_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(vert_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile shader: %.*s\n", message_size, message);
        return 1;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, frag_shader);
    glAttachShader(program, vert_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetProgramInfoLog(program, sizeof(message), &message_size, message);
        fprintf(stderr, "could not link shader: %.*s\n", message_size, message);
        return 1;
    }

    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);
    free(itembox_frag_src);
    free(itembox_vert_src);

    glUseProgram(program);

    const char* texture_file_name = "images/questionmark.png";
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

    GLint image_uniform_location = glGetUniformLocation(program, "image");
    glUniform1i(image_uniform_location, 0);

    bool quit = false;
    while (!quit) {
        const Uint32 start = SDL_GetTicks();
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTICES_COUNT);

        SDL_GL_SwapWindow(window);

        const Uint32 duration = SDL_GetTicks() - start;
        const Uint32 delta_time_ms = 1000 / 60;
        if (duration < delta_time_ms) {
            SDL_Delay(delta_time_ms - duration);
        }
    }

    return 0;
}
