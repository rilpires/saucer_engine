#include "audio.h"
#include <math.h>
#include "lua_engine.h"

#ifdef DEBUG
#define AL_CALL(x)\
    x ;                                                                                 \
    {                                                                                   \
        auto al_error = alGetError();                                                   \
        if( al_error != AL_NO_ERROR ){                                                  \
            saucer_err( "[OpenAL Error] Error code " , al_error );                      \
            switch(al_error){                                                           \
                case AL_INVALID_NAME:       saucer_err( "AL_INVALID_NAME" )      break; \
                case AL_INVALID_ENUM:       saucer_err( "AL_INVALID_ENUM" )      break; \
                case AL_INVALID_VALUE:      saucer_err( "AL_INVALID_VALUE" )     break; \
                case AL_INVALID_OPERATION:  saucer_err( "AL_INVALID_OPERATION" ) break; \
                case AL_OUT_OF_MEMORY:      saucer_err( "AL_OUT_OF_MEMORY" )     break; \
            }                                                                                           \
        }                                                                                               \
    }
#define ALC_CALL(device,x)\
    x ;                                                                                     \
    {                                                                                       \
        auto alc_error = alcGetError(device);                                               \
        if( alc_error != ALC_NO_ERROR ){                                                    \
            saucer_err( "[OpenAL Context Error] Context error code " , alc_error );         \
            switch(alc_error){                                                              \
                case ALC_INVALID_DEVICE:    saucer_err( "ALC_INVALID_DEVICE" )     break;   \
                case ALC_INVALID_CONTEXT:   saucer_err( "ALC_INVALID_CONTEXT" )    break;   \
                case ALC_INVALID_ENUM:      saucer_err( "ALC_INVALID_ENUM" )       break;   \
                case ALC_INVALID_VALUE:     saucer_err( "ALC_INVALID_VALUE" )      break;   \
                case ALC_OUT_OF_MEMORY:     saucer_err( "ALC_OUT_OF_MEMORY" )      break;   \
            }                                                                               \
        }                                                                                   \
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
        if(device_name) saucer_err( "No audio device found with name:" , device_name )
        else            saucer_err( "No default audio device found" )
        exit(1);
    } else saucer_print( "Loaded audio device:" , alcGetString( device , ALC_DEVICE_SPECIFIER ) )
    context = alcCreateContext( device , nullptr );
    if( !context ){
        saucer_err( "No context for audio could be created." )
        exit(1);
    }
    ALCboolean made_current = alcMakeContextCurrent( context );
    if( !made_current ){
        saucer_err( "Couldn't make created context as current." )
        exit(1);
    }
    // From now on we can use AL_CALL and ALC_CALL

    int major_version , minor_version;
    ALC_CALL( device , alcGetIntegerv( device , ALC_MAJOR_VERSION , 1 , &major_version ) );
    ALC_CALL( device , alcGetIntegerv( device , ALC_MINOR_VERSION , 1 , &minor_version ) );
    saucer_print( "Successfully initialized OpenAL version " , major_version , "." , minor_version ) 
    
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
        else saucer_err("call the cops");

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
YamlNode        AudioEmitter::to_yaml_node() const {
    YamlNode ret;
    if(audio_resource) ret["audio_resource"] = audio_resource->get_path();
    ret["looping"] = looping;
    ret["positional"] = positional; 
    return ret;
}
void            AudioEmitter::from_yaml_node( YamlNode yaml_node ) {
    if( yaml_node["audio_resource"].IsDefined() )
        set_audio_resource( (AudioResource*)ResourceManager::get_resource(yaml_node["audio_resource"].as<std::string>()) );
    set_positional( yaml_node["positional"].as<decltype(positional)>() );
    set_looping(    yaml_node["looping"].as<decltype(looping)>() );
}