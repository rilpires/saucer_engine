#include "resources/audiofile.h"
#include "lua_engine.h"
#include "AudioFile.h" // .wav files

AudioResource::AudioResource( std::string filepath ) : Resource(filepath){
}
AudioResource::~AudioResource(){
    delete[] ((char*)buffer_data);
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
uint32_t    AudioResource::get_buffer_size() const {
    return buffer_size;
}
void*       AudioResource::get_buffer_data() const {
    return buffer_data;
}
void        AudioResource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_sample_rate );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_bit_depth );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_samples_per_channel );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_length_seconds );
    REGISTER_LUA_MEMBER_FUNCTION( AudioResource , get_num_channels );
}

WavAudioResource::WavAudioResource( std::string filepath ) : AudioResource(filepath) {
    AudioFile<float> audio_file;
    audio_file.load( filepath );
    
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
        std::cerr << "bit_depth=="<<bit_depth<<" && num_channels==" << num_channels << std::endl;
    }

}
WavAudioResource::~WavAudioResource(){

}
