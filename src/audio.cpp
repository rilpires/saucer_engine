#include "audio.h"
#include <math.h>
#include "engine.h"
#include "lua_engine.h"
#include "resources/audiofile.h"
#include "editor.h"

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>
#include <AL/efx.h>

#include <cstring>

// AudioEngine =============================================================================
AudioEngine::AudioEngine( const char* device_name ){
    device = alcOpenDevice( device_name );
    
    if( !device ){
        if(device_name) saucer_err( "No audio device found with name:" , device_name )
        else            saucer_err( "No default audio device found" )
    } else saucer_print( "Loaded audio device:" , alcGetString( device , ALC_DEVICE_SPECIFIER ) )
    context = alcCreateContext( (ALCdevice*)device , nullptr );
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

// AudioEmitter =============================================================================
AudioEmitter::AudioEmitter(){
    AL_CALL( alGenSources(1,&source) );
    positional = false;
    looping = false;
    gain = 0.0f;
    AL_CALL( alSourcei( source , AL_LOOPING , looping ) );

    // Do we care about these?
    AL_CALL( alSourcei( source , AL_PITCH , 1 ) );
    AL_CALL( alSourcef( source , AL_GAIN , gain ) );
    AL_CALL( alSource3f( source , AL_POSITION , 0 , 0 , 0 ) );
    AL_CALL( alSource3f( source , AL_VELOCITY , 0 , 0 , 0 ) );
}
AudioEmitter::~AudioEmitter(){
    AL_CALL( alDeleteSources(1,&source) );
}
bool                    AudioEmitter::get_positional() const{
    return positional;
}
void                    AudioEmitter::set_positional( bool new_val ){
    positional = new_val;
}
float                   AudioEmitter::get_gain() const{
    return gain;
}
void                    AudioEmitter::set_gain( float new_val ){
    if( new_val < 0.0f ) new_val = 0.0f;
    if( new_val > 1.0f ) new_val = 1.0f;
    gain = new_val;
    AL_CALL( alSourcef( source , AL_GAIN , gain ) );
}
bool                    AudioEmitter::get_looping() const{
    return looping;
}
bool                    AudioEmitter::is_playing() const {
    return get_state() == AudioEmitter::STATE::PLAYING;
}
int                     AudioEmitter::get_state() const {
    int al_state;
    AL_CALL( alGetSourcei( source , AL_SOURCE_STATE , &al_state ) );
    switch(al_state){
        case AL_INITIAL: return INITIAL;
        case AL_PLAYING: return PLAYING;
        case AL_PAUSED: return PAUSED;
        case AL_STOPPED: return STOPPED;
    }
    saucer_warn("Unexpected OpenAL behavior");
}
void                    AudioEmitter::play(){
    set_gain( float(random()%100)/100.0f );
    AL_CALL( alSourceStop(source) );
    AL_CALL( alSourcePlay(source) );
}
void                    AudioEmitter::pause(){
    AL_CALL( alSourcePause(source) );
}
void                    AudioEmitter::bind_methods(){
    REGISTER_LUA_CONSTANT( AudioState , INITIAL , AudioEmitter::INITIAL );
    REGISTER_LUA_CONSTANT( AudioState , PLAYING , AudioEmitter::PLAYING );
    REGISTER_LUA_CONSTANT( AudioState , PAUSED , AudioEmitter::PAUSED );
    REGISTER_LUA_CONSTANT( AudioState , STOPPED , AudioEmitter::STOPPED );
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , get_positional )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_positional )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , get_looping )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_looping )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , is_playing )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , get_gain )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , set_gain )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , get_state )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , play )
    REGISTER_LUA_MEMBER_FUNCTION( AudioEmitter , pause )

}
void                    AudioEmitter::push_editor_items(){
    #ifdef SAUCER_EDITOR
    switch(get_state()){
        case STATE::INITIAL: 
            ImGui::Text("Current state: INITIAL"); break;
        case STATE::PLAYING: 
            ImGui::Text("Current state: PLAYING"); break;
        case STATE::PAUSED: 
            ImGui::Text("Current state: PAUSED"); break;
        case STATE::STOPPED: 
            ImGui::Text("Current state: STOPPED"); break;
        default: 
            ImGui::Text("Current state: ???"); break;;
    }
    #endif
}
YamlNode                AudioEmitter::to_yaml_node() const {
    YamlNode ret;
    ret["looping"] = looping;
    ret["positional"] = positional; 
    return ret;
}
void                    AudioEmitter::from_yaml_node( YamlNode yaml_node ) {
    set_positional( yaml_node["positional"].as<decltype(positional)>() );
    set_looping(    yaml_node["looping"].as<decltype(looping)>() );
}

// SamplePlayer ======================================================================
SamplePlayer::SamplePlayer(){
    audio_resource = nullptr;
}
SamplePlayer::~SamplePlayer(){

}
WavAudioResource*       SamplePlayer::get_audio_resource() const {
    return audio_resource;
}
void                    SamplePlayer::set_audio_resource(WavAudioResource* new_res){
    if( audio_resource == new_res ) return;
    audio_resource = new_res;
    if( audio_resource ){
        AL_CALL( alSourcei( source , AL_BUFFER , audio_resource->get_al_buffer() ) );
    } else {
        AL_CALL( alSourcei( source , AL_BUFFER , 0 ) );
    }
}
void                    SamplePlayer::set_looping( bool new_val ){
    looping = new_val;
    AL_CALL( alSourcei( source , AL_LOOPING , looping ) );
}
void                    SamplePlayer::bind_methods(){
    REGISTER_COMPONENT_HELPERS( SamplePlayer,"sample_player");
    REGISTER_LUA_MEMBER_FUNCTION( SamplePlayer , get_audio_resource )
    REGISTER_LUA_MEMBER_FUNCTION( SamplePlayer , set_audio_resource )
    REGISTER_LUA_MEMBER_FUNCTION( SamplePlayer , set_looping);
}
void                    SamplePlayer::push_editor_items(){
#ifdef SAUCER_EDITOR
    AudioEmitter::push_editor_items();
    PROPERTY_RESOURCE(this,audio_resource, WavAudioResource );
    PROPERTY_BOOL(this,looping);
    if(ImGui::SmallButton("Play")){if(get_audio_resource())this->play();}
#endif
}
YamlNode                SamplePlayer::to_yaml_node() const {
    YamlNode ret = AudioEmitter::to_yaml_node();
    if(audio_resource) ret["audio_resource"] = audio_resource->get_path();
    return ret;
}
void                    SamplePlayer::from_yaml_node( YamlNode yaml_node ) {
    AudioEmitter::from_yaml_node(yaml_node);
    if( yaml_node["audio_resource"].IsDefined() )
        set_audio_resource( ResourceManager::get_resource<WavAudioResource>(yaml_node["audio_resource"].as<std::string>()) );
}

// StreamPlayer =====================================================================
StreamPlayer::StreamPlayer(){
    audio_resource = nullptr;
    pcm_offset = 0;
    AL_CALL( alGenBuffers( NUM_BUFFERS , buffer_ids ) );
    for( auto b : buffer_ids ) unqueued_buffers.insert(b);
    pause();
}
StreamPlayer::~StreamPlayer(){
}
AudioStreamResource*    StreamPlayer::get_audio_resource() const {
    return audio_resource;
}
void                    StreamPlayer::set_audio_resource( AudioStreamResource* new_res ){
    pcm_offset = 0;
    if( audio_resource ) reset_buffers();
    audio_resource = new_res;
}
void                    StreamPlayer::set_looping( bool new_val ){
    looping = new_val;
}
void                    StreamPlayer::seek_seconds_position( float seconds ){
    if( audio_resource ){
        reset_buffers();
        float ratio = seconds / audio_resource->get_length_seconds();
        float fracpart,intpart;
        fracpart = modf(ratio,&intpart);
        seconds = fracpart * audio_resource->get_length_seconds();
        if( seconds < 0 )
            seconds = audio_resource->get_length_seconds() + seconds;
        pcm_offset = audio_resource->get_sample_rate() * seconds;
    }
}
void                    StreamPlayer::reset_buffers(){
    int state = get_state();
    AL_CALL( alSourceStop( source ) );
    AL_CALL( alSourcei( source , AL_BUFFER , 0 ) );
    for( auto b : buffer_ids ) unqueued_buffers.insert(b);
    last_buffers.clear();
    AL_CALL(alSourceRewind(source));
    if( state == PLAYING ){
        play();
    }
}
void                    StreamPlayer::update_stream(){
    if( audio_resource == nullptr ) return;
    ALint buffers_processed = 0;
    AL_CALL( alGetSourcei( source,AL_BUFFERS_PROCESSED, &buffers_processed) );
    for( int i = 0 ; i <  buffers_processed ; i++ ){
        ALuint buffer_id;
        AL_CALL( alSourceUnqueueBuffers( source , 1 , &buffer_id) );
        unqueued_buffers.insert(buffer_id);
        if( last_buffers.find(buffer_id) != last_buffers.end() ){
            last_buffers.erase(buffer_id);
            if(!looping){
                pause();
                reset_buffers();
            }
        }
    }
    if( last_buffers.size() == 0 && unqueued_buffers.size() > 0 ){
        ALuint next_buffer = *unqueued_buffers.begin();
        std::vector<uint8_t> buffer_data(BUFFER_SIZE,0);
        bool end = audio_resource->fill_data( buffer_data , pcm_offset );
        if( buffer_data.size() > 0 ){
            AL_CALL( alBufferData(next_buffer , audio_resource->get_al_buffer_format() , &buffer_data[0] , buffer_data.size() , audio_resource->get_sample_rate() ) );
            AL_CALL( alSourceQueueBuffers(source , 1 , &next_buffer) );
            pcm_offset += buffer_data.size() / audio_resource->get_sample_size();
            unqueued_buffers.erase(next_buffer);
            if( get_state() == STOPPED ){ // Streamer never stops, only pause!
                play();
            }
        }
        if( end ){
            pcm_offset = 0;
            last_buffers.insert(next_buffer);
        }
    }
}
void                    StreamPlayer::bind_methods(){
    REGISTER_COMPONENT_HELPERS(StreamPlayer,"stream_player");

    REGISTER_LUA_MEMBER_FUNCTION(StreamPlayer,set_looping);
    REGISTER_LUA_MEMBER_FUNCTION(StreamPlayer,get_audio_resource )
    REGISTER_LUA_MEMBER_FUNCTION(StreamPlayer,set_audio_resource);
    REGISTER_LUA_MEMBER_FUNCTION(StreamPlayer,seek_seconds_position);
}
void                    StreamPlayer::push_editor_items(){
#ifdef SAUCER_EDITOR
    AudioEmitter::push_editor_items();
    PROPERTY_RESOURCE(this,audio_resource, AudioStreamResource );
    PROPERTY_BOOL(this,looping);
    if(ImGui::SmallButton("Play") && get_audio_resource())  this->play();
    if(ImGui::SmallButton("Pause") && get_audio_resource()) this->pause();
    if(ImGui::SmallButton("Rewind") && get_audio_resource()) this->seek_seconds_position(0);
#endif
}
YamlNode                StreamPlayer::to_yaml_node() const {
    YamlNode ret = AudioEmitter::to_yaml_node();
    if(audio_resource) ret["audio_resource"] = audio_resource->get_path();
    return ret;
}
void                    StreamPlayer::from_yaml_node( YamlNode yaml_node ) {
    AudioEmitter::from_yaml_node(yaml_node);
    if( yaml_node["audio_resource"].IsDefined() )
        set_audio_resource( ResourceManager::get_resource<AudioStreamResource>(yaml_node["audio_resource"].as<std::string>()) );
}

// ===================================================================================
// ===================================================================================