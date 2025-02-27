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
    const char* itembox_frag_file_path = "../shaders/itembox.frag";
    const char* itembox_vert_file_path = "../shaders/itembox.vert";

    char* itembox_frag_src = read_file_as_string(itembox_frag_file_path);
    if (itembox_frag_src == NULL)
        return 1;

    char* itembox_vert_src = read_file_as_string(itembox_vert_file_path);
    if (itembox_vert_src == NULL)
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

    GLuint vertex_vao;
    glGenVertexArrays(1, &vertex_vao);
    glBindVertexArray(vertex_vao);

    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);

    const float size = 0.7;
    float alpha = 0.6666;

    Vertex vertex_buffer[] = {
        /* top back face */
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
        
        /* bottom back face */
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, -size},
            .color = (Vec4f){0, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, -size},
            .color = (Vec4f){1, 1, 0, alpha},
        },
        
        /* top left face */
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .color = (Vec4f){1, 0.5, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, -size},
            .color = (Vec4f){1, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, size},
            .color = (Vec4f){1, 0, 0, alpha},
        },
        
        /* bottom left face */
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, -size},
            .color = (Vec4f){1, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, size},
            .color = (Vec4f){1, 0, 0, alpha},
        },
        
        /* top right face */
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .color = (Vec4f){1, 0.5, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, -size},
            .color = (Vec4f){0, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, size},
            .color = (Vec4f){0, 1, 1, alpha},
        },
        
        /* bottom right face */
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, -size},
            .color = (Vec4f){0, 1, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, size},
            .color = (Vec4f){0, 1, 1, alpha},
        },
        
        /* top front face */
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .color = (Vec4f){1, 0.5, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, size},
            .color = (Vec4f){0, 1, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, size},
            .color = (Vec4f){1, 0, 0, alpha},
        },
        
        /* bottom front face */
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, size},
            .color = (Vec4f){0, 1, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, size},
            .color = (Vec4f){1, 0, 0, alpha},
        },
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), &vertex_buffer, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    alpha = 0.0;
    const float uv_size = 1;

    Vertex uv_buffer[] = {
        /* top face */
        (Vertex) {
            .position = (Vec3f){0, 1, 0},
            .uv = (Vec2f){-uv_size, 0},
            .color = (Vec4f){1, 0.5, 0, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, 0},
            .uv = (Vec2f){0, uv_size},
            .color = (Vec4f){0, 0.75, 0.75, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, 0},
            .uv = (Vec2f){0, -uv_size},
            .color = (Vec4f){0.75, 0.75, 0, alpha},
        },

        /* bottom face */
        (Vertex) {
            .position = (Vec3f){0, -1, 0},
            .uv = (Vec2f){uv_size, 0},
            .color = (Vec4f){0, 0, 1, alpha},
        },
        (Vertex) {
            .position = (Vec3f){size, 0, 0},
            .uv = (Vec2f){0, uv_size},
            .color = (Vec4f){0, 0.75, 0.75, alpha},
        },
        (Vertex) {
            .position = (Vec3f){-size, 0, 0},
            .uv = (Vec2f){0, -uv_size},
            .color = (Vec4f){0.75, 0.75, 0, alpha},
        },
    };

    GLuint uv_vao;
    glGenVertexArrays(1, &uv_vao);
    glBindVertexArray(uv_vao);

    GLuint uv_vbo;
    glGenBuffers(1, &uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer), &uv_buffer, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    const int VERTEX_BUFFER_COUNT = sizeof(vertex_buffer) / sizeof(vertex_buffer[0]);
    const int UV_BUFFER_COUNT = sizeof(uv_buffer) / sizeof(uv_buffer[0]);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (const char**)&itembox_frag_src, NULL);
    glCompileShader(frag_shader);

    GLint frag_shader_compiled = 0;
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_shader_compiled);

    if (!frag_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(frag_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile fragment shader: %.*s\n", message_size, message);
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
        fprintf(stderr, "could not compile vertex shader: %.*s\n", message_size, message);
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
        fprintf(stderr, "could not link program: %.*s\n", message_size, message);
        return 1;
    }

    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);
    free(itembox_frag_src);
    free(itembox_vert_src);

    glUseProgram(program);

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

    GLint image_uniform_location = glGetUniformLocation(program, "image");
    GLint time_uniform_location = glGetUniformLocation(program, "time");
    GLint resolution_uniform_location = glGetUniformLocation(program, "resolution");
    glUniform1i(image_uniform_location, 0);
    glUniform2f(resolution_uniform_location, WIDTH, HEIGHT);
    glViewport(0, 0, WIDTH, HEIGHT);

    while (RGFW_window_shouldClose(window) == 0) {
        while (RGFW_window_checkEvent(window)) {
            switch (window->event.type) {
                case RGFW_windowResized: {
                    int width = window->r.w;
                    int height = window->r.h;
                    glUniform2f(resolution_uniform_location, width, height);
                    glViewport(0, 0, width, height);
                } break;
                default:
                    break;
            }
        }

        u64 time = RGFW_getTimeNS() / 1e6;
        glUniform1f(time_uniform_location, time);
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vertex_vao);
        glDrawArrays(GL_TRIANGLES, 0, VERTEX_BUFFER_COUNT);

        glBindVertexArray(uv_vao);
        glDrawArrays(GL_TRIANGLES, 0, UV_BUFFER_COUNT);
                        
        RGFW_window_swapBuffers(window);
        RGFW_window_checkFPS(window, 60);
    }

    RGFW_window_close(window);
    return 0;
}
