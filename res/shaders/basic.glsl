
#vertex
#version 150 core
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
    if(!ignore_camera) gl_Position *= view_transf;
    gl_Position.xy /= ( viewport_size.xy/2 );
    gl_Position.yz *= -1;
    uv = in_uv;
    // modulate = vec4(in_modulate.x/255.0f,
    //                 in_modulate.y/255.0f,
    //                 in_modulate.z/255.0f,
    //                 in_modulate.w/255.0f);
    // modulate = in_modulate;
    modulate = uniform_modulate;
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
    bool aesthetics_90s_wave = false;

    if( aesthetics_90s_wave ){
        temp_modulate.r *=  0.9 + 0.1*sin(time*100);

        vec2    tex_size = textureSize(tex,0);
        float   dist_in_pixels = 1;
        float   r_angle = 0;
        float   g_angle = 3.1415;
        vec2    r_uv_vec = (vec2(cos(r_angle),sin(r_angle)) / tex_size) * dist_in_pixels ;
        vec2    g_uv_vec = (vec2(cos(g_angle),sin(g_angle)) / tex_size) * dist_in_pixels ;
            
        if(!tex_is_alpha_mask){
            
            float r = texture(tex,uv + r_uv_vec ).r;
            float g = texture(tex,uv + g_uv_vec ).g;
            float b = texture(tex,uv ).b;
            outColor.rgba = vec4( temp_modulate.r * r,
                                temp_modulate.g * g,
                                temp_modulate.b * b,
                                temp_modulate.a * texture(tex,uv).a );

        }else{
            float r = texture(tex,uv + r_uv_vec ).r;
            float g = texture(tex,uv + g_uv_vec ).r;
            float a = texture(tex,uv).r;
            outColor.rgba = vec4(   temp_modulate.r * r,
                                    temp_modulate.g * g,
                                    temp_modulate.b,
                                    temp_modulate.a * a );
        }
    }
    else{
        if(!tex_is_alpha_mask){
            outColor.rgba = temp_modulate * texture(tex,uv);
        }else{
            outColor.rgba = vec4(temp_modulate.rgb, temp_modulate.a * texture(tex,uv).r);
        }
    }

};
