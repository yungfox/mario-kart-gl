#version 330 core

uniform sampler2D image;

in vec4 out_color;
in vec2 out_uv;

out vec4 frag_color;

void main(void) {
    vec2 scaled = (out_uv + 0.5) / 1.5;
    vec2 pos = vec2(scaled.x + 0.16666, -scaled.y + 0.8);
    vec4 t = texture(image, pos);
    frag_color = mix(out_color + 0.125, t, 0.3333);
}
