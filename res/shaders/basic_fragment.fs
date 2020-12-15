#version 150 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 uv;

uniform sampler2D tex;

out vec4 outColor;

void main(){
    outColor.rgba = texture(tex,uv);
};
