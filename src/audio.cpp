#include "audio.h"
#include <math.h>
#include "lua_engine.h"

#ifdef DEBUG
#define AL_CALL(x)\
    x ;                                                                                                 \
    {                                                                                                   \
        auto al_error = alGetError();                                                                   \
        if( al_error != AL_NO_ERROR ){                                                                  \
            std::cerr << "[OpenAL Error] Error code " << al_error << std::endl                          \
            << "on " << __FILE__ << ":" << __LINE__ << "; " << #x << std::endl;                         \
            switch(al_error){                                                                           \
                case AL_INVALID_NAME:       std::cerr << "AL_INVALID_NAME" << std::endl;      break;    \
                case AL_INVALID_ENUM:       std::cerr << "AL_INVALID_ENUM" << std::endl;      break;    \
                case AL_INVALID_VALUE:      std::cerr << "AL_INVALID_VALUE" << std::endl;     break;    \
                case AL_INVALID_OPERATION:  std::cerr << "AL_INVALID_OPERATION" << std::endl; break;    \
                case AL_OUT_OF_MEMORY:      std::cerr << "AL_OUT_OF_MEMORY" << std::endl;     break;    \
            }                                                                                           \
        }                                                                                               \
    }
#define ALC_CALL(device,x)\
    x ;                                                                                                 \
    {                                                                                                   \
        auto alc_error = alcGetError(device);                                                           \
        if( alc_error != ALC_NO_ERROR ){                                                                \
            std::cerr << "[OpenAL Context Error] Context error code " << alc_error << std::endl         \
            << "on " << __FILE__ << ":" << __LINE__ << "; " << #x << std::endl;                         \
            switch(alc_error){                                                                          \
                case ALC_INVALID_DEVICE:    std::cerr << "ALC_INVALID_DEVICE" << std::endl;     break;  \
                case ALC_INVALID_CONTEXT:   std::cerr << "ALC_INVALID_CONTEXT" << std::endl;    break;  \
                case ALC_INVALID_ENUM:      std::cerr << "ALC_INVALID_ENUM" << std::endl;       break;  \
                case ALC_INVALID_VALUE:     std::cerr << "ALC_INVALID_VALUE" << std::endl;      break;  \
                case ALC_OUT_OF_MEMORY:     std::cerr << "ALC_OUT_OF_MEMORY" << std::endl;      break;  \
            }                                                                                           \
        }                                                                                               \
    }
#else
#define AL_CALL(x)\
    x;
#define ALC_CALL(device,x)\
    x;                                                                 
#endif


AudioEngine::AudioEngine( const char* device_name ){
    device = alcOpenDevice( device_name );
    if( !device ){
        if(device_name) std:: cerr << "No audio device found with name:" << device_name;
        else            std:: cerr << "No default audio device found";
        exit(1);
    } else std::cout << "Loaded audio device:" << alcGetString( device , ALC_DEVICE_SPECIFIER ) << std::endl;
    context = alcCreateContext( device , nullptr );
    if( !context ){
        std::cerr << "No context for audio could be created." << std::endl;
        exit(1);
    }
    ALCboolean made_current = alcMakeContextCurrent( context );
    if( !made_current ){
        std::cerr << "Couldn't make created context as current." << std::endl;
        exit(1);
    }
    // From now on we can use AL_CALL and ALC_CALL

    int major_version , minor_version;
    ALC_CALL( device , alcGetIntegerv( device , ALC_MAJOR_VERSION , 1 , &major_version ) );
    ALC_CALL( device , alcGetIntegerv( device , ALC_MINOR_VERSION , 1 , &minor_version ) );
    std::cout << "Successfully initialized OpenAL version " << major_version << "." << minor_version << std::endl; 
    
}
AudioEngine::~AudioEngine(){
    ALC_CALL( device , alcMakeContextCurrent( nullptr ) );
    ALC_CALL( device , alcDestroyContext(context) );
    ALC_CALL( device , alcCloseDevice( device ) );
}
std::vector<std::string>     AudioEngine::get_available_devices() const{
    std::vector<std::string> ret;    
    return ret; 
}

// =============================================================================

std::unordered_multimap<SaucerId,AudioEmitter*> AudioEmitter::component_from_node;

AudioEmitter::AudioEmitter(){
    AL_CALL( alGenBuffers(1,&buffer) );
    AL_CALL( alGenSources(1,&source) );
    audio_resource = nullptr;
    positional = false;
    looping = false;
    AL_CALL( alSourcei( source , AL_LOOPING , looping ) );
    AL_CALL( alSourcei( source , AL_BUFFER , buffer ) );

    // Do we care about these?
    AL_CALL( alSourcei( source , AL_PITCH , 1 ) );
    AL_CALL( alSourcef( source , AL_GAIN , 1.0f ) );
    AL_CALL( alSource3f( source , AL_POSITION , 0 , 0 , 0 ) );
    AL_CALL( alSource3f( source , AL_VELOCITY , 0 , 0 , 0 ) );
}
AudioEmitter::~AudioEmitter(){
    AL_CALL( alDeleteBuffers(1,&buffer) );
    AL_CALL( alDeleteSources(1,&source) );
}
AudioResource*      AudioEmitter::get_audio_resource() const {
    return audio_resource;
}
void                AudioEmitter::set_audio_resource(AudioResource* new_res){
    if( audio_resource ){
    }
    audio_resource = new_res;
    if( new_res ){
        audio_resource = new_res;
        int bit_depth = new_res->get_bit_depth();
        int num_channels = new_res->get_num_channels();
        int format;
        
        if( num_channels == 1 && bit_depth == 8 )   format = AL_FORMAT_MONO8;
        else 
        if( num_channels == 1 && bit_depth == 16 )  format = AL_FORMAT_MONO16;
        else 
        if( num_channels == 2 && bit_depth == 8 )   format = AL_FORMAT_STEREO8;
        else 
        if( num_channels == 2 && bit_depth == 16 )  format = AL_FORMAT_STEREO16;

        AL_CALL( alSourcei( source , AL_BUFFER , 0 ) );
        AL_CALL( alBufferData( buffer , format , audio_resource->get_buffer_data() , audio_resource->get_buffer_size()  , audio_resource->get_sample_rate() ) );
        AL_CALL( alSourcei( source , AL_BUFFER , buffer ) );
    }
}
bool                AudioEmitter::is_positional() const{
    return positional;
}
void                AudioEmitter::set_positional( bool new_val ){
    positional = new_val;
}
bool                AudioEmitter::is_looping() const{
    return looping;
}
void                AudioEmitter::set_looping( bool new_val ){
    looping = new_val;
    AL_CALL( alSourcei( source , AL_LOOPING , looping ) );
}
bool                AudioEmitter::is_playing() const {
    int ret;
    AL_CALL( alGetSourcei( buffer , AL_SOURCE_STATE , &ret ) );
    return ret == AL_PLAYING;
}
bool                AudioEmitter::is_paused() const {
    int ret;
    AL_CALL( alGetSourcei( buffer , AL_SOURCE_STATE , &ret ) );
    return ret == AL_PAUSED;
}
void                AudioEmitter::play(){
    AL_CALL( alSourcePlay(source) );
}
void                AudioEmitter::pause(){
    AL_CALL( alSourcePause(source) );
}
void         AudioEmitter::bind_methods(){

    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , get_audio_resource )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_audio_resource )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , is_positional )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_positional )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , is_looping )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_looping )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , is_playing )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , is_paused )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , play )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , pause )

}
