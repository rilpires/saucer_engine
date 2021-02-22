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

    protected:
        AudioResource();
        ~AudioResource();

    public:
        int             get_sample_rate() const ;
        int             get_bit_depth() const ;
        int             get_samples_per_channel() const ;
        float           get_length_seconds() const ;
        int             get_num_channels() const ;
        int             get_sample_size() const; // Size of each sample. Mono 8bit -> 1B ; Stereo 8bit -> 2B; Mono 16bit -> 2B; Stereo 16B -> 4B
        
        unsigned int    get_al_buffer_format() const;

        static void bind_methods();
};

class WavAudioResource : public AudioResource {
    REGISTER_SAUCER_OBJECT(WavAudioResource,AudioResource)
    
    protected:
        uint32_t        buffer_size;    
        void*           buffer_data;
        unsigned int    al_buffer;
    public:
        WavAudioResource( const std::vector<uint8_t>& data );
        ~WavAudioResource();

        uint32_t        get_buffer_size() const ;
        void*           get_buffer_data() const ;
        unsigned int    get_al_buffer() const;
        
};

class AudioStreamResource : public AudioResource {
    REGISTER_SAUCER_OBJECT(AudioStreamResource,AudioResource)

    protected:
        uint64_t raw_offset;
    protected:
        std::vector<uint8_t>    raw_data;

    protected:
        AudioStreamResource();
        ~AudioStreamResource();

    public:
        // Should return TRUE when reached end of stream
        virtual bool fill_data( std::vector<uint8_t>& buffer , uint64_t pcm_offset){};

};

struct OggVorbis_File;
struct vorbis_info;
class OggAudioResource : public AudioStreamResource {
    REGISTER_SAUCER_OBJECT(OggAudioResource,AudioStreamResource)
    private:
        
        OggVorbis_File*     ogg_file;
        vorbis_info*        vi;
        
        static size_t       ogg_read_callback(void *ptr, size_t size, size_t nmemb, void *datasource);
        static int32_t      ogg_seek_callback(void *datasource, int64_t offset, int whence);
        static long int     ogg_tell_callback(void *datasource);
    
    public:
        OggAudioResource( const std::vector<uint8_t>& data );
        ~OggAudioResource();

        bool fill_data( std::vector<uint8_t>& buffer , uint64_t pcm_offset );


};


#endif