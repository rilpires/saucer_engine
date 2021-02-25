#include "resources.h"
#include "core.h"

#include <fstream>
#include <sstream>
#include <zlib.h>

std::unordered_map<std::string,SaucerId> ResourceManager::id_by_path;
std::unordered_map<std::string,ResourceManager::ContentTableEntry> ResourceManager::toc_entries;
std::ifstream ResourceManager::package_stream;

Resource::Resource(){
    dirty=false;
}
Resource::~Resource(){
}
std::string Resource::get_path() const {
    return path;
}
void        Resource::flag_as_dirty(){
    ;
}
void        Resource::reload(){}

void        Resource::bind_methods(){
    REGISTER_LUA_MEMBER_FUNCTION(Resource,get_path);
}

ResourceManager::ContentTableEntry::ContentTableEntry( std::string p_path , uint64_t p_offset , uint64_t p_compressed_size ): offset(p_offset), compressed_size(p_compressed_size) {
    if( p_path.size() > 255 ){
        saucer_err("Can't have a resource with path size > 255: " , p_path );
        exit(1);
    }
    for( size_t i = 0 ; i < p_path.size() ; i++ )path[i]=p_path[i];
    path[p_path.size()] = '\0';
}
ResourceManager::ContentTableEntry::ContentTableEntry() {
}
std::string             ResourceManager::ContentTableEntry::get_path(){
    return std::string(path);
}
const uint64_t          ResourceManager::ContentTableEntry::get_offset(){
    return offset;
}
const uint64_t          ResourceManager::ContentTableEntry::get_compressed_size(){
    return compressed_size;
}
Resource*               ResourceManager::load_resource(std::string filepath){
    Resource* ret = NULL;
    std::vector<uint8_t> data = get_data(filepath);
    if( data.size()==0 ) return nullptr;
    size_t extension_pos = filepath.find_last_of('.');
    std::string extension = "";
    if( extension_pos != std::string::npos ) extension = filepath.substr( extension_pos );
    else saucer_err("Resources should have a extension hint (i.e. \"res/player.png\")");
    try
    {
        if( extension == ".png" ){
            ret = new TextureResource( data );
        }
        else if (extension == ".wav"){
            ret = new WavAudioResource( data );
        }
        else if (extension == ".lua"){
            ret = new LuaScriptResource( data );
        }
        else if (extension == ".ttf"){
            ret = new FontResource( data );
        }
        else if (extension == ".node"){
            ret = new NodeTemplateResource( data );
        }
        else if (extension == ".config"){
            ret = new ProjectConfig( data );
        }
        else if (extension == ".ogg"){
            ret = new OggAudioResource( data );
        }
        else if (extension == ".glsl"){
            ret = new ShaderResource( data );
        } else saucer_err( "What is this? Unknown file format. Couldn't load resource for: " , filepath );
    }

    catch(const std::exception& e)
    {
        ret = nullptr;
        saucer_err("Error loading resource " , filepath , ": " , e.what() );
        saucer_err("Data size: " , float(data.size())/1000.0f , "KB"  );
    }
    if( ret ){
        ret->path = filepath;
        id_by_path.insert(std::pair<std::string,SaucerId>(ret->path ,ret->get_saucer_id()));
    }
    return ret;
}
void                    ResourceManager::fetch_package(std::string package_filepath){
    if( package_stream.is_open() ){
        saucer_warn("Data package is opened already");
        return;
    }
    uint64_t toc_size;
    package_stream.open(package_filepath);
    
    if( package_stream.good() ){
        package_stream.unsetf(std::ios::skipws);
        package_stream.read( (char*)&toc_size ,8);
    } else {
        saucer_warn("Couldn't find package data \"" , package_filepath , "\"" );
        toc_size = 0;
    }

    for( int toc_index = 0 ; toc_index < toc_size ; toc_index++ ){
        ContentTableEntry entry;
        package_stream.read( (char*)&(entry) , sizeof(ResourceManager::ContentTableEntry) );
        toc_entries.insert( std::pair<std::string,ContentTableEntry&>(entry.get_path() , entry) );
    }

}
std::vector<uint8_t>    ResourceManager::get_data( std::string filepath ){
    auto toc_find = toc_entries.find(filepath);
    std::vector<uint8_t> ret;
    
    // Using data from package file if available and is not editor
    if( toc_find != toc_entries.end()  &&  !Engine::is_editor() ){
        if( !package_stream.good() ){
            saucer_err("Package stream should be opened but it isn't? Oopsie unexpected behavior ahead.");
            return ret;
        }
        ContentTableEntry& toc_entry = toc_find->second;
        package_stream.seekg(   8 /*TOC size entry*/ 
                                + toc_entries.size()*sizeof(ContentTableEntry) /*Entire TOC content*/
                                + toc_entry.get_offset() /*Resource offset*/ );
        
        std::vector<uint8_t> compressed_data;
        compressed_data.resize( toc_entry.get_compressed_size() );
        package_stream.read( (char*)&(compressed_data[0]) , compressed_data.size() );

        const int buffer_size = 1<<16;
        uint8_t inflated_buffer[buffer_size]; // 32KB

        z_stream inflate_stream;
        inflate_stream.next_in = &(compressed_data[0]);
        inflate_stream.avail_in = compressed_data.size();
        inflate_stream.next_out = inflated_buffer;
        inflate_stream.avail_out = buffer_size;
        inflate_stream.zalloc = Z_NULL;
        inflate_stream.zfree = Z_NULL;
        inflate_stream.opaque = Z_NULL;
        
        inflateInit(&inflate_stream);
        auto inflate_ret = inflate(&inflate_stream,Z_FINISH);
        ret.resize(inflate_stream.total_out);
        memcpy( &(ret[0]) , inflated_buffer , ret.size() );
        
        while(inflate_ret != Z_STREAM_END){
            if( inflate_ret == Z_BUF_ERROR && inflate_stream.avail_in == 0 && inflate_stream.avail_out>0 ) break; // zlib has almost an entire page(https://zlib.net/zlib_how.html) dedicated to this case. I didn't read it all but I trust for now that this won't cause any problem. 
            SAUCER_ASSERT(inflate_stream.avail_in>0 , "No more compressed data available?");
            inflate_stream.next_out = inflated_buffer;
            inflate_stream.avail_out = buffer_size;
            inflate_ret = inflate(&inflate_stream,Z_FINISH);
            size_t old_size = ret.size();
            ret.resize( inflate_stream.total_out );
            memcpy( &(ret[old_size]) , inflated_buffer , ret.size() - old_size );
        }
        inflateEnd(&inflate_stream);

    } 
    
    // Load from physical file system
    else {
        std::ifstream f{ filepath.c_str() , std::ios::binary };
        if( !f.good() ){
            saucer_err("Can't open file: \"" , filepath ,  "\"");
            return ret;
        }
        f.unsetf(std::ios::skipws);
        std::istream_iterator<uint8_t> begin{f},end;
        ret = std::vector<uint8_t>( begin , end );
    }

    return ret;
}
template<> 
Resource*               ResourceManager::get_resource( std::string p_resource_path){
    auto it = id_by_path.find(p_resource_path);
    if( it == id_by_path.end() )
        return load_resource(p_resource_path);
    else{
        Resource* ret = static_cast<Resource*>(SaucerObject::from_saucer_id(it->second));
        #ifdef DEBUG
        if( ret->dirty ) ret->reload();
        #endif
        return ret;
    }
}
void                    ResourceManager::set_resource(std::string resource_name , Resource* r ){
    if( r == NULL )return;
    if( id_by_path.find(resource_name)!= id_by_path.end()){
        if( id_by_path[resource_name]!= r->get_saucer_id() ){
            saucer_err( "Resource name " , resource_name , " is already used by another resource" );
            return;
        } else return; // Nothing to do
    }
    r->path = resource_name;
    id_by_path[resource_name] = r->get_saucer_id();
}
void                    ResourceManager::free_resource(Resource* p_resource){
    id_by_path.erase( p_resource->get_path());
    delete p_resource;
}
void                    ResourceManager::dirty_every_resource(){
    for( auto& it : id_by_path ){
        Resource* res = static_cast<Resource*>( SaucerObject::from_saucer_id(it.second) );
        res->flag_as_dirty();
    }
}
void                    ResourceManager::bind_methods(){
    REGISTER_LUA_GLOBAL_FUNCTION( "load" , ResourceManager::get_resource<Resource> );
}
const decltype(ResourceManager::id_by_path)::iterator ResourceManager::begin(){
    return id_by_path.begin();
}
const decltype(ResourceManager::id_by_path)::iterator ResourceManager::end()  {
    return id_by_path.end();
}   
