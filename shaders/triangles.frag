#version 330 core

in vec4 out_color;

out vec4 frag_color;

void main(void) {
    frag_color = vec4(out_color.rgb, 0.5);
}
