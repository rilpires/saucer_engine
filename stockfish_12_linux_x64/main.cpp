#include <iostream>
#include <fstream>

#include <cstdlib>
#include <unistd.h>
#include <string.h>

using namespace std;

int main(){

    int pipes [2][2]; // pipes[0] is parent to child , pipes[1] is child to parent


    pipe(pipes[0]);
    pipe(pipes[1]);

    int fork_result = fork();
    if( fork_result>0 ){
        // Parent
        const int BUFF_SIZE = 2048;
        char buf[BUFF_SIZE] = "";
        size_t n,n2;

        close(pipes[0][0]);
        close(pipes[1][1]);

        for( int i = 0 ; i < 5 ; i++ ){ usleep(1000 * 100 ); cout << '.'; } cout<<endl;

        n=read( pipes[1][0] , buf , BUFF_SIZE );
        cout << "n= " << n << endl;
        cout <<"Parent leu:\n" << buf << endl;

        for( int i = 0 ; i < 5 ; i++ ){ usleep(1000 * 100 ); cout << '.'; } cout<<endl;

        cout << "Parent mandando isready" << endl;
        write( pipes[0][1] , "uci\n" , 32 );

        for( int i = 0 ; i < 5 ; i++ ){ usleep(1000 * 100 ); cout << '.'; } cout<<endl;

        n2 = read( pipes[1][0] , buf , BUFF_SIZE );
        cout << "n2= " << n2 << endl;
        cout << "Parent leu:\n" << std::string(buf).substr(0,n2) << endl;
        for( int i = 0 ; i < 5 ; i++ ){ usleep(1000 * 100 ); cout << '.'; } cout<<endl;

        n2 = read( pipes[1][0] , buf , BUFF_SIZE );
        cout << "n2= " << n2 << endl;
        cout << "Parent leu:\n" << std::string(buf).substr(0,n2) << endl;


    } else if ( fork_result == 0) {
        // Child
        char* argv[] = { "./stockfish_20090216_x64" , 0 };
        dup2( pipes[0][0] , STDIN_FILENO );
        dup2( pipes[1][1] , STDOUT_FILENO );
        close( pipes[0][0] );
        close( pipes[0][1] );
        close( pipes[1][0] );
        close( pipes[1][1] );
        execv( "./stockfish_20090216_x64" , argv );
        

    } else {
        // Error!
    }
}
