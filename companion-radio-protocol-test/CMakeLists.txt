cmake_minimum_required(VERSION 3.5)
project(companion_protocol)

list(APPEND server_sources
    ../server.c
    ../companion-radio-protocol/mc_companion_serial_interface.c
    ../companion-radio-protocol/mc_companion_command_parser.c
)

add_executable(companion_server ${server_sources})

target_include_directories(
    companion_server PUBLIC
    ..
    ../companion-radio-protocol
)

list(APPEND client_sources
    ../client.c)

add_executable(companion_client ${client_sources})

target_include_directories(
    companion_client PUBLIC
    ..
    ../companion-radio-protocol
)
