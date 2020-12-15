#version 150 core
#extension GL_ARB_explicit_attrib_location: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;

uniform sampler2D tex;
uniform vec2    viewport_size;
uniform mat4    model_transf;
uniform mat4    camera_transf;

out vec2 uv;

void main(){
    gl_Position = vec4( position , 1.0 );
    gl_Position.xy *= textureSize(tex,0)/viewport_size;
    gl_Position *= model_transf;
    gl_Position *= camera_transf;
    gl_Position.xy /= ( viewport_size/2 );
    uv = in_uv;
};