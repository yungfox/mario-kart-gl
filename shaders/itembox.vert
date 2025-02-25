#version 330 core

uniform float time;
uniform vec2 resolution;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

out vec4 out_color;
out vec2 out_uv;

mat4 mat4_translate(vec3 dir) {
    mat4 result = mat4(1.0);
    result[3] = vec4(dir, 1.0);
    return result;
}

mat4 mat4_scale(vec3 s) {
    mat4 result = mat4(1.0);
    result[0][0] = s.x;
    result[1][1] = s.y;
    result[2][2] = s.z;
    return result;
}

mat4 mat4_rotate_y(float angle) {
    mat4 result = mat4(1.0);
    result[0][0] = cos(angle);
    result[2][0] = sin(angle);
    result[0][2] = -sin(angle);
    result[2][2] = cos(angle);
    return result;
}

mat4 mat4_rotate_z(float angle)
{
    mat4 result = mat4(1.0);
    result[0][0] = cos(angle);
    result[0][1] = sin(angle);
    result[1][0] = -sin(angle);
    result[1][1] = cos(angle);
    return result;
}

mat4 mat4_perspective(float fovy, float aspect, float near, float far) {
    float tan_half_fovy = tan(fovy * 0.5);
    mat4 result = mat4(0.0);
    result[0][0] = 1.0 / (aspect * tan_half_fovy);
    result[1][1] = 1.0 / tan_half_fovy;
    result[2][2] = far / (near - far);
    result[3][2] = -1.0;
    result[2][3] = -(far * near) / (far - near);
    return result;
}

void main(void) {
    float aspect = resolution.x / resolution.y;
    float t = time * 0.001953125; // 1/512
    float u = t * 0.25;

    mat4 camera = (
        mat4_translate(vec3(0.0, 0.0, -500.0)) *
        mat4_rotate_y(t) *
        mat4_rotate_z(u) *
        mat4_translate(vec3(0.125) * 1.0) *
        mat4_scale(vec3(100.0, 100.0, 100.0))
    );

    vec4 camera_pos = camera * vec4(position, 1);

    gl_Position = (
        mat4_perspective(0.75, aspect, 1.0, 500.0) *
        camera_pos
    );

    out_uv = uv;
    out_color = color;
}
