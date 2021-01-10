
#vertex
#version 150 core
#extension GL_ARB_explicit_attrib_location: enable
#extension GL_ARB_explicit_uniform_location: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;

uniform sampler2D tex;
uniform vec4    pixel_size; // ( viewport_size , object_size )
uniform vec4    uv_div; // ( topleft , bottomright )

uniform bool    ignore_camera;
uniform mat4    camera_transf;
uniform mat4    model_transf;
uniform vec4    in_modulate;
uniform float   time;

out vec2 uv;
out vec4 modulate;

void main(){
    vec2 tex_size = textureSize(tex,0);
    vec2 viewport_size_pixels = pixel_size.xy;
    vec2 size_in_pixels = pixel_size.zw;

    gl_Position = vec4( position , 1.0 );
    gl_Position.xy *= (size_in_pixels/viewport_size_pixels);
    gl_Position *= model_transf;
    if(!ignore_camera) gl_Position *= camera_transf;
    gl_Position.xy /= ( pixel_size.xy/2 );
    gl_Position.y *= -1;
    
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
