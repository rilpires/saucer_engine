#include "resources/audiofile.h"
#include "lua_engine.h"
#include "audio.h"

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>
#include <AL/efx.h>

// WAV file decoding:
#include "AudioFile.h" 

// OGG file decoding:
#include "ogg/ogg.h"
#include "vorbis/vorbisfile.h"


// AudioResource ================================================
AudioResource::AudioResource( ) {
}
AudioResource::~AudioResource(){
}
int         AudioResource::get_sample_rate() const {
    return sample_rate;
}
int         AudioResource::get_bit_depth() const {
    return bit_depth;
}
int         AudioResource::get_samples_per_channel() const {
    return samples_per_channel;
}
float       AudioResource::get_length_seconds() const {
    return length_seconds;
}
int         AudioResource::get_num_channels() const {
    return num_channels;
}
unsigned int    AudioResource::get_al_buffer_format() const{
    if( num_channels == 1 && bit_depth == 8 )   return AL_FORMAT_MONO8;
    else 
    if( num_channels == 1 && bit_depth == 16 )  return AL_FORMAT_MONO16;
    else 
    if( num_channels == 2 && bit_depth == 8 )   return AL_FORMAT_STEREO8;
    else 
    if( num_channels == 2 && bit_depth == 16 )  return AL_FORMAT_STEREO16;
    else{
        saucer_err("call the cops");
        return 0;
    }
}
int         AudioResource::get_sample_size() const{
    return num_channels * (bit_depth/8);
}
void        AudioResource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_sample_rate );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_bit_depth );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_samples_per_channel );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_length_seconds );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_num_channels );
}

// WavAudioResource ================================================
WavAudioResource::WavAudioResource( const std::vector<uint8_t>& mem_data ){
    AL_CALL( alGenBuffers(1,&al_buffer) );

    AudioFile<float> audio_file;
    std::vector<uint8_t> mem_data_cpy{ mem_data.begin() , mem_data.end() };
    audio_file.load( mem_data_cpy );
    
    sample_rate         = audio_file.getSampleRate();
    bit_depth           = audio_file.getBitDepth();
    samples_per_channel = audio_file.getNumSamplesPerChannel();
    length_seconds      = audio_file.getLengthInSeconds();
    num_channels        = audio_file.getNumChannels();
    buffer_size         = num_channels*samples_per_channel*(bit_depth/8);
    buffer_data         = static_cast<void*>( new char[buffer_size] );
    
    if( bit_depth == 8 && num_channels == 1 ) {
        for( int i = 0 ; i < samples_per_channel ; i++ ){
            ((char*)buffer_data)[i] = 127*audio_file.samples[0][i];
        }
    } else if( bit_depth == 8 && num_channels == 2 ) {
        for( int i = 0 ; i < samples_per_channel ; i++ ){
            ((char*)buffer_data)[2*i+0] = 127*audio_file.samples[0][i];
            ((char*)buffer_data)[2*i+1] = 127*audio_file.samples[1][i];
        }
    } else if( bit_depth == 16 && num_channels == 1 ) {
        for( int i = 0 ; i < samples_per_channel ; i++ ){
            ((short*)buffer_data)[i] = (~0b1000000000000000)*audio_file.samples[0][i];
        }
    } else if( bit_depth == 16 && num_channels == 2 ) {
        for( int i = 0 ; i < samples_per_channel ; i++ ){
            ((short*)buffer_data)[2*i+0] = (~0b1000000000000000)*audio_file.samples[0][i];
            ((short*)buffer_data)[2*i+1] = (~0b1000000000000000)*audio_file.samples[1][i];
        }
    }
    
    else {
        saucer_err( "bit_depth==",bit_depth," && num_channels==" , num_channels )
    }

    AL_CALL( alBufferData( al_buffer , get_al_buffer_format() , buffer_data , buffer_size  , sample_rate ) );

}
WavAudioResource::~WavAudioResource(){
    AL_CALL( alDeleteBuffers(1,&al_buffer) );
    delete[] ((char*)buffer_data);
}
uint32_t        WavAudioResource::get_buffer_size() const {
    return buffer_size;
}
void*           WavAudioResource::get_buffer_data() const {
    return buffer_data;
}
unsigned int    WavAudioResource::get_al_buffer() const {
    return al_buffer;
}

// AudioStreamResource ================================================

AudioStreamResource::AudioStreamResource(){
    raw_offset = 0;
}
AudioStreamResource::~AudioStreamResource(){
    
}

// OggAudioResource ================================================

OggAudioResource::OggAudioResource(  const std::vector<uint8_t>& data  ){
    ogg_file = new OggVorbis_File();
    raw_data = data;
    
    ov_callbacks cbs;
    cbs.read_func = ogg_read_callback;
    cbs.seek_func = ogg_seek_callback;
    cbs.tell_func = ogg_tell_callback;
    cbs.close_func = nullptr;
    ov_open_callbacks( this , ogg_file , nullptr , 0 , cbs );
    
    vi = ov_info( ogg_file , -1 );
    num_channels = vi->channels;
    sample_rate = vi->rate;
    bit_depth = 16;
    length_seconds = ov_time_total( ogg_file , -1 );
    samples_per_channel = ov_pcm_total( ogg_file , -1 ) / num_channels;
}

OggAudioResource::~OggAudioResource(){
    delete ogg_file;
}

bool    OggAudioResource::fill_data( std::vector<uint8_t>& buffer , uint64_t pcm_offset  ){
    size_t emitted_now = 0;
    bool stream_ended = false;
    ov_pcm_seek(ogg_file,pcm_offset);
    while( !stream_ended && emitted_now < buffer.capacity() ){
        int bitstream;
        long read_result = ov_read( ogg_file, 
                                    (char*)&buffer[emitted_now], /* buffer pointer */
                                    buffer.capacity() - emitted_now , /* length, how many is available to emit*/
                                    0 /*Little endian*/ , 
                                    2 /*1 for 8-bit depth, 2 for 16-bit depth*/, 
                                    1 /* signed */ , 
                                    &bitstream ); /* logical position (?) */
        emitted_now += read_result;

        switch(read_result){
            case OV_HOLE:     saucer_err( "OV_HOLE found in update of buffer " ); break;
            case OV_EBADLINK: saucer_err( "OV_EBADLINK found in update of buffer " ); break;
            case OV_EINVAL:   saucer_err( "OV_EINVAL found in update of buffer " ); break;
            case 0 : stream_ended = true; break;
        }
    }
    buffer.resize( emitted_now );
    buffer.shrink_to_fit();
    return stream_ended;
}
size_t      OggAudioResource::ogg_read_callback(void* destination, size_t size, size_t nmemb, void *datasource){
    OggAudioResource* ogg_resource = static_cast<OggAudioResource*>(datasource);
    
    ALsizei length = size * nmemb;

    if( ogg_resource->raw_offset + length > ogg_resource->raw_data.size() ){
        length = ogg_resource->raw_data.size() - ogg_resource->raw_offset;
    }

    std::memcpy( destination , &(ogg_resource->raw_data[ogg_resource->raw_offset]) , length );
    ogg_resource->raw_offset += length;
    return length;
}
int32_t     OggAudioResource::ogg_seek_callback(void *datasource, int64_t offset, int whence){
    OggAudioResource* ogg_resource = static_cast<OggAudioResource*>(datasource);
    switch(whence){
        case SEEK_SET:
            ogg_resource->raw_offset = offset;
            break;
        case SEEK_CUR:
            ogg_resource->raw_offset += offset;
            break;
        case SEEK_END:
            ogg_resource->raw_offset = ogg_resource->raw_data.size() - offset;
            break;
    }
    if( ogg_resource->raw_offset < 0 ){
        ogg_resource->raw_offset = 0;
        return -1;
    }
    else if( ogg_resource->raw_offset > ogg_resource->raw_data.size() ){
        ogg_resource->raw_offset = ogg_resource->raw_data.size();
        return -1;
    }
    
    return 0;
}
long int    OggAudioResource::ogg_tell_callback(void *datasource){
    OggAudioResource* ogg_resource = static_cast<OggAudioResource*>(datasource);
    return ogg_resource->raw_offset;
}

