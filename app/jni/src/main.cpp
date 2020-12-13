//
// Created by USER on 10/12/2020.
//

#include <iostream>
#include <SDL.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <thread>
#define ASIO_STANDLALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using namespace std;

std::vector<char> vBuffer( 24 * 1024 );
std::vector<std::string> vect_message;
std::string message_assembled = "";

void GrabSomeData( asio::ip::tcp::socket& socket ){

    socket.async_read_some( asio::buffer( vBuffer.data() , vBuffer.size() ) ,
            [&]( std::error_code ec , std::size_t lenght ){
                if( !ec ){

                    std::string recieved = std::string(reinterpret_cast<char*>(vBuffer.data()));
                    vect_message.push_back(recieved);

                    GrabSomeData(socket);

                }
            }
    );

}


int SDL_main( int argc , char* argv[] ){

    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;

    SDL_bool done = SDL_FALSE;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        __android_log_print(ANDROID_LOG_VERBOSE, "SDL", "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    ///Window size: 2560x1550 , Device size: 2560x1600
    win = SDL_CreateWindow("Hello World!", 0, 0, 2560, 1550,
                           SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (win == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "SDL", "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    asio::error_code ec;

    asio::io_context context;

    asio::io_context::work idleWork(context);

    thread thrContext = thread( [&](){context.run();} );

    asio::ip::tcp::endpoint endpoint( asio::ip::make_address( "192.168.25.4" , ec ), 80 );

    asio::ip::tcp::socket socket(context);

    socket.connect( endpoint , ec );

    if( !ec ){
        __android_log_print(ANDROID_LOG_DEBUG, "ASIO", "ASIO CONNECTION: %s\n", "Could connect");
    }else{
        __android_log_print(ANDROID_LOG_ERROR, "ASIO", "ASIO CONNECTION: %s\n", ec.message().c_str());
    }

    if( socket.is_open() ){

        GrabSomeData(socket);

        std::string sRequest =
                "GET /webapp/json_get_data.php HTTP/1.1\r\n"
                "Host: 192.168.25.4\r\n"
                "Connection: close\r\n\r\n";

        socket.write_some( asio::buffer(sRequest.data() , sRequest.size() ) , ec );

        ///Though it is ALWAYS bad practice to add hardcoded tie delays, the code above it is supposed to be called, the the program continues
        ///along until it needs the data that this fetches, by then there would've been enought time to get what it needs, if not, it would've
        ///been added some error catching to deal with it, but for simplicity sake the delay remains.

        SDL_Delay(10000);

        for( size_t I = 0 ; I < vect_message.size() ; I++ ){
            message_assembled += vect_message[I];
        }

        ///TODO : Get the JSON parser working, currently it has a linking problem

        __android_log_print(ANDROID_LOG_DEBUG, "ASIO", "ASIO MESSAGE: %s\n", message_assembled.c_str());

    }

    while (!done) {

        SDL_SetRenderDrawColor( ren , 0,255,0, 255 );
        SDL_RenderClear(ren);



        SDL_RenderPresent(ren);
    }

    return 0;
}

