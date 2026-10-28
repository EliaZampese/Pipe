cmake_minimum_required(VERSION 4.2)
project(Pipe C)

set(CMAKE_C_STANDARD 11)

add_executable(Pipe main.c
        1.c
        2.c
        3.c
        NamedPipe1/server.c
        NamedPipe1/client.c
        NamePipe2/server.c
        NamePipe2/client.c
        NamePipe3/chat.c)
