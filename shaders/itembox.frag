#version 330 core

uniform sampler2D image;

in vec4 out_color;
in vec2 out_uv;

out vec4 frag_color;

void main(void) {
    vec2 scaled = (out_uv + 0.5) / 1.5;
    vec2 pos = vec2(scaled.x + 0.16666, -scaled.y + 0.8);
    vec4 t = vec4(texture(image, pos).rgb, 1);
    frag_color = mix(out_color, t, 0.25);
}
