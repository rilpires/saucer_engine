
#vertex
#version 150 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_modulate;

uniform sampler2D tex;

uniform vec2    viewport_size;
uniform bool    ignore_camera;
uniform mat4    view_transf;
uniform mat4    model_transf;
uniform float   time;

out vec2 uv;
out vec4 modulate;

void main(){
    gl_Position = vec4( position , 1.0 );
    gl_Position *= model_transf;
    if(!ignore_camera) gl_Position *= view_transf;
    gl_Position.xy /= ( viewport_size.xy/2 );
    gl_Position.y *= -1;
    uv = in_uv;
    // modulate = vec4(in_modulate.x/255.0f,
    //                 in_modulate.y/255.0f,
    //                 in_modulate.z/255.0f,
    //                 in_modulate.w/255.0f);
    modulate = in_modulate;
};


#fragment
#version 150 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 uv;
in vec4 modulate;

uniform bool tex_is_alpha_mask;
uniform float   time;
uniform sampler2D tex;

out vec4 outColor;

void main(){
    vec4 temp_modulate = modulate;
    bool aesthetics_90s_wave = true;


    if( !tex_is_alpha_mask ){
        if( aesthetics_90s_wave ){
            // AESTHETICS 90's WAVE
            temp_modulate.r *=  0.9 + 0.1*sin(time*100);
            float dist = 0.011;
            float r = texture(tex,uv + vec2(  dist, dist) ).r;
            float g = texture(tex,uv + vec2( -dist, dist) ).g;
            float b = texture(tex,uv + vec2(     0, dist) ).b;
            outColor.rgba = vec4( temp_modulate.r * r,
                                temp_modulate.g * g,
                                temp_modulate.b * b,
                                temp_modulate.a * texture(tex,uv).a );
        } else {
            outColor.rgba = temp_modulate * texture(tex,uv);
        }
    }
    else {
        if( aesthetics_90s_wave ){
            // AESTHETICS 90's WAVE
            temp_modulate.b *=  0.8 + 0.2*sin(time*80);
            float dist = 0.005;
            float r = texture(tex,uv + vec2(  dist, dist) ).r;
            float g = texture(tex,uv + vec2( -dist, dist) ).r;
            float b = texture(tex,uv + vec2(     0, dist) ).r;
            outColor.rgba = vec4(   temp_modulate.r * r,
                                    temp_modulate.g * g,
                                    temp_modulate.b * b,
                                    temp_modulate.a * texture(tex,uv).r );
        } else {
            outColor.rgba = vec4(temp_modulate.rgb, temp_modulate.a * texture(tex,uv).r);
        }
    }
};
