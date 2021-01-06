
#vertex
#version 150 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;

uniform sampler2D tex;
uniform vec2    viewport_size;

uniform vec4    uv_div; // ( topleft , bottomright )

uniform mat4    camera_transf;
uniform mat4    model_transf;
uniform vec4    in_modulate;

out vec2 uv;
out vec4 modulate;

void main(){
    vec2 tex_size = textureSize(tex,0);
    vec2 size_in_pixels = tex_size * vec2( uv_div.z - uv_div.x , uv_div.w - uv_div.y );

    gl_Position = vec4( position , 1.0 );
    gl_Position.xy *= (size_in_pixels/viewport_size);
    gl_Position *= model_transf;
    gl_Position *= camera_transf;
    gl_Position.xy /= ( viewport_size/2 );
    
    uv = in_uv;
    uv.x *= (uv_div.z - uv_div.x);
    uv.y *= (uv_div.w - uv_div.y);
    uv.xy += uv_div.xy;
    
    modulate = in_modulate;
};


#fragment
#version 150 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 uv;
in vec4 modulate;

uniform sampler2D tex;

out vec4 outColor;

void main(){
    outColor.rgba = texture(tex,uv) * modulate;
};
