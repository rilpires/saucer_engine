#include "core.h"

void open_project( std::string config_path="res/project.config"  );
void pack_resources();

int main( int argc , char** argv ){
    
    for( int i = 0 ; i < argc ; i++ )
        saucer_print( "argv[" , i , "] = " , argv[i] );  

    if( argc>1 ){
        if( strcmp(argv[1],"pack")==0 ){
            pack_resources();
            return 0;
        }
        else open_project( argv[1] );
    }
    open_project();

}

void open_project( std::string config_path ){
    Engine::initialize( config_path );
    Engine::set_window_title("SaucerEngine");

    while( !Engine::should_close() )
        Engine::update();
    Engine::close();
    LuaEngine::finish();
}

#include <zlib.h>
void pack_resources(){

    std::ifstream ifs , resource_file ;
    std::string resource_path;
    uint64_t offset = 0;

    std::vector<std::vector<uint8_t> >  resource_compressed_datas;
    std::vector<std::string>            resource_paths;
    std::vector<uint64_t>               resource_offsets;

    ifs.open("toc");
    while( std::getline(ifs,resource_path) ){
        
        resource_file.open(resource_path , std::ios::binary );
        resource_file.unsetf (std::ios::skipws);
        if( !resource_file.good() ){
            saucer_err("Can't find " , resource_path , ". Did you run toc_builder.py recently?");
            exit(1);
        }

        std::istream_iterator<uint8_t> begin(resource_file),end;
        std::vector<uint8_t> resource_data(begin,end);
        std::vector<uint8_t> compressed_data;
        compressed_data.resize( resource_data.size() + 128 /*safety!*/ );
        resource_file.close();

        z_stream compressing_stream;
        compressing_stream.avail_in = (unsigned int)resource_data.size();
        compressing_stream.next_in = &(resource_data[0]);
        compressing_stream.avail_out = compressed_data.capacity();
        compressing_stream.next_out = &(compressed_data[0]);
        compressing_stream.zalloc = Z_NULL;
        compressing_stream.zfree = Z_NULL;
        compressing_stream.opaque = Z_NULL;
        

        deflateInit( &compressing_stream , Z_DEFAULT_COMPRESSION );
        deflate( &compressing_stream , Z_SYNC_FLUSH );
        deflateEnd( &compressing_stream );
        compressed_data.resize(compressing_stream.total_out);
        compressed_data.shrink_to_fit();
        
        saucer_print("Compressed \"" , resource_path , "\" ratio: " , 100.0f * float(compressed_data.size())/resource_data.size() , "%" );


        resource_compressed_datas.push_back(compressed_data);
        resource_offsets.push_back(offset);
        resource_paths.push_back(resource_path);

        offset += compressed_data.size();
    }
    ifs.close();

    
    std::vector<ResourceManager::ContentTableEntry> toc_entries;
    for( int i = 0 ; i < resource_paths.size() ; i++ ){
        std::string res_path = resource_paths[i];
        uint64_t res_offset = resource_offsets[i];
        std::vector<uint8_t>& res_data = resource_compressed_datas[i];
        toc_entries.push_back( ResourceManager::ContentTableEntry(res_path , res_offset , res_data.size()) );
    }

    uint64_t amount_of_entries = toc_entries.size();
    std::vector<uint8_t> package_data;

    // toc_size (8bytes)
    for( size_t i = 0 ; i < sizeof(uint64_t) ; i++ ){
        package_data.push_back( ((uint8_t*)&amount_of_entries)[i] );
    }
    // array of ContentTableEntry
    for( int i = 0 ; i < resource_paths.size() ; i++ ){
        auto entry = toc_entries[i];
        for( size_t i = 0 ; i < sizeof(entry) ; i++ ){
            package_data.push_back( ((uint8_t*)&entry)[i] );
        }
    }
    // Compressed datas (offsets is relative from here)
    for( int i = 0 ; i < resource_paths.size() ; i++ ){
        auto entry = toc_entries[i];
        auto compressed_data = resource_compressed_datas[i];
        package_data.insert( package_data.end() , compressed_data.begin() , compressed_data.end() );
    }


    std::ofstream output_file;
    output_file.open("package.data", std::ios::binary );
    output_file.unsetf( std::ios::skipws );
    output_file.write( (const char*)&(package_data[0]) , package_data.size() );
    output_file.close();
}