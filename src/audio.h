#ifndef AUDIO_H
#define AUDIO_H

#include <vector>

#include "component.h"
#include "resources/audiofile.h"

struct ALCcontext;
struct ALCdevice;

// AudioEngine =====================================================================
class AudioEngine {
    friend class Engine;
    
    private:
        ALCcontext*   context;
        ALCdevice*   device;
    public:
        AudioEngine( const char* device_name = nullptr );

        std::vector<std::string>     get_available_devices() const;
    
    private:
        ~AudioEngine();

};

// AudioEmitter =====================================================================
class AudioEmitter : public Component {
    REGISTER_AS_COMPONENT(AudioEmitter);
    
    enum STATE {
        INITIAL,
        PLAYING,
        PAUSED,
        STOPPED
    };

    friend class AudioEngine;
    protected:
        unsigned int            source;
        float                   gain; // [ 0 - 1 ]
        bool                    positional; // Not implemented
        bool                    looping;

    public:
        AudioEmitter();
        ~AudioEmitter();
        
        bool                    get_positional() const;
        void                    set_positional( bool new_val );
        float                   get_gain() const;
        void                    set_gain( float new_val );
        bool                    get_looping() const;
        virtual void            set_looping( bool new_val ){};
        bool                    is_playing() const ;
        int                     get_state() const ;
        void                    play();
        void                    pause();

        static void             bind_methods();
        void                    push_editor_items();
        YamlNode                to_yaml_node() const override ;
        void                    from_yaml_node( YamlNode ) override ;

};

// SamplePlayer =====================================================================
class SamplePlayer : public AudioEmitter {
    REGISTER_AS_INHERITED_COMPONENT(SamplePlayer,AudioEmitter);
    private:
        WavAudioResource*       audio_resource;
        
    public:
        SamplePlayer();
        ~SamplePlayer();    
        
        WavAudioResource*       get_audio_resource() const ;
        void                    set_audio_resource( WavAudioResource* new_res );
        void                    set_looping( bool new_val );
        
        static void             bind_methods();
        void                    push_editor_items();
        YamlNode                to_yaml_node() const override ;
        void                    from_yaml_node( YamlNode ) override ;
};
// StreamPlayer =====================================================================
class StreamPlayer : public AudioEmitter {
    REGISTER_AS_INHERITED_COMPONENT(StreamPlayer,AudioEmitter);

    private:
        uint64_t                pcm_offset;              
        AudioStreamResource*    audio_resource;

        static const size_t     NUM_BUFFERS = 2;
        static const size_t     BUFFER_SIZE = (1<<16); 
        uint32_t                buffer_ids[StreamPlayer::NUM_BUFFERS];
        std::set<uint32_t>      unqueued_buffers;
        std::set<uint32_t>      last_buffers;

    public:
        StreamPlayer();
        ~StreamPlayer();

        AudioStreamResource*    get_audio_resource() const ;
        void                    set_audio_resource( AudioStreamResource* new_res );
        void                    set_looping( bool new_val );
        void                    seek_seconds_position( float seconds );

        void                    reset_buffers();
        void                    update_stream();

        static void             bind_methods();
        void                    push_editor_items();
        YamlNode                to_yaml_node() const override ;
        void                    from_yaml_node( YamlNode ) override ;
};



#endif