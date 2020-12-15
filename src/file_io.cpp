#include "file_io.h"

#include <fstream>
#include <iostream>

std::string read_file_as_string( const char* filepath ){
    std::ifstream ifs( filepath , std::ifstream::in );
    if( !ifs ){
        std::cerr << "Couldn't open file " << filepath << std::endl;
    } else {
        ifs.seekg(0,ifs.end);
        int length = ifs.tellg();
        ifs.seekg(0,ifs.beg);
        char* buffer = new char[length];
        ifs.read( buffer , length );
        std::string ret(buffer);
        delete[] buffer;
        ifs.close();
        return ret;
    }
    return "";
} 
