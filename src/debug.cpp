#include "debug.h"

namespace saucer_debug {

std::vector<std::ostream*>& extern_console_streams(){ 
    static std::vector<std::ostream*> ret ;
    return ret;
};

std::string saucer_path_format( const char* file ){
    std::string ret = file;
    for( int i = 0 ; i < ret.size() ; i++ ) if(ret[i]=='\\') ret[i] = '/';
    std::string dev_folder_name = "saucer_engine/";
    std::size_t folder_pos = ret.rfind(dev_folder_name);
    if( folder_pos != std::string::npos )
        ret = ret.substr( folder_pos + dev_folder_name.size() );
    return ret;
}

}