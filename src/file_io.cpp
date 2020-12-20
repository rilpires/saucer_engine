#include "file_io.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file_as_string( const char* filepath ){
    std::ifstream ifs( filepath , std::ifstream::in );
    if( !ifs ){
        std::cerr << "Couldn't open file " << filepath << std::endl;
    } else {
        std::ostringstream sstr;
        sstr << ifs.rdbuf();
        return sstr.str();
    }
    return "";
} 
