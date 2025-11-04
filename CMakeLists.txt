cmake_minimum_required(VERSION 3.5)
project(meshcore_c)

list(APPEND sources
    meshcore/packet.c
    meshcore/payload/request.c
    meshcore/payload/ack.c
    meshcore/payload/advert.c
    main.c)

add_executable(meshcore_c ${sources})

target_include_directories(
    meshcore_c PUBLIC
    .
    meshcore
    meshcore/payload
)
