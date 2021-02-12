#ifndef AUDIO_H
#define AUDIO_H

#include <vector>

#include "component.h"
#include "resources/audiofile.h"

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>
#include <AL/efx.h>


class AudioEngine {
    friend class Engine;
    
    private:
        ALCcontext*     context;
        ALCdevice*      device;
    public:
        AudioEngine( const char* device_name = nullptr );

        std::vector<std::string>     get_available_devices() const;
    
    private:
        ~AudioEngine();
};

class AudioEmitter : public Component {
    REGISTER_AS_COMPONENT(AudioEmitter);
    
    friend class AudioEngine;
    private:
        ALuint              source;
        ALuint              buffer;
        AudioResource*      audio_resource;
        bool                positional; // Not implemented
        bool                looping;
    public:
        AudioEmitter();
        ~AudioEmitter();
        
        AudioResource*      get_audio_resource() const ;
        void                set_audio_resource(AudioResource* new_res);
        bool                get_positional() const;
        void                set_positional( bool new_val );
        bool                get_looping() const;
        void                set_looping( bool new_val );
        bool                is_playing() const ;
        bool                is_paused() const ;
        void                play();
        void                pause();

        static void         bind_methods();
        void                push_editor_items();
        YamlNode            to_yaml_node() const override ;
        void                from_yaml_node( YamlNode ) override ;

};

#endif