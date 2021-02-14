#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include "resources.h"

class AudioResource : public Resource {
    REGISTER_SAUCER_OBJECT(AudioResource,Resource)

    protected:
        int         sample_rate;
        int         bit_depth;
        int         samples_per_channel;
        float       length_seconds;
        int         num_channels;
        uint32_t    buffer_size;    
        void*       buffer_data;

    protected:
        AudioResource( const std::vector<uint8_t>& data );
        ~AudioResource();

    public:

        int         get_sample_rate() const ;
        int         get_bit_depth() const ;
        int         get_samples_per_channel() const ;
        float       get_length_seconds() const ;
        int         get_num_channels() const ;
        uint32_t    get_buffer_size() const ;
        void*       get_buffer_data() const ;

        static void bind_methods();

};

class WavAudioResource : public AudioResource {
    REGISTER_SAUCER_OBJECT(WavAudioResource,AudioResource)
    public:
        WavAudioResource( const std::vector<uint8_t>& data );
        ~WavAudioResource();

};

#endif