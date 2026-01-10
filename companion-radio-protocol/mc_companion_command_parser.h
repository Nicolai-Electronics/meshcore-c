#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "mc_companion.h"

typedef enum {
    COMPANION_COMMAND_PARSER_ERROR_NONE              = 0,
    COMPANION_COMMAND_PARSER_ERROR_INVALID_COMMAND   = 1,
    COMPANION_COMMAND_PARSER_ERROR_INVALID_ARGUMENTS = 2,
} mc_companion_command_parser_error_t;

mc_companion_command_parser_error_t mc_companion_parse_command(uint8_t* data, uint16_t data_length, companion_command_packet_t* out_packet);

// Callback

typedef void (*mc_companion_server_callback)(companion_command_packet_t* command, mc_companion_command_parser_error_t error);
