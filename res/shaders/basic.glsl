#vertex
#version 130
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_modulate; // Currently unused

uniform sampler2D tex;

uniform vec2    viewport_size;
uniform bool    ignore_camera;
uniform mat4    view_transf;
uniform vec4    uniform_modulate;
uniform mat4    model_transf;
uniform float   time;

out vec2 uv;
out vec4 modulate;

void main(){
    gl_Position = vec4( position , 1.0 );
    gl_Position *= model_transf;
    gl_Position *= view_transf;
    gl_Position.xy /= ( viewport_size.xy/2 );
    gl_Position.yz *= -1;
    uv = in_uv;
    modulate = uniform_modulate;
};


#fragment
#version 130
#extension GL_ARB_explicit_uniform_location : enable

in vec2 uv;
in vec4 modulate;

uniform bool tex_is_alpha_mask;
uniform float   time;
uniform sampler2D tex;

out vec4 outColor;

void main(){
    vec4 temp_modulate = modulate;

    if( tex_is_alpha_mask ){
        outColor.rgba = vec4(temp_modulate.rgb, temp_modulate.a * texture(tex,uv).r);
    }else{
        outColor.rgba = temp_modulate * texture(tex,uv);
    }

};
