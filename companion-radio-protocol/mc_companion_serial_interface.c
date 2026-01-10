#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mc_companion.h"
#include "mc_companion_command_parser.h"

#define FIELD_SIZE(type, field) (sizeof(((type*)0)->field))

static uint8_t                    rx_buffer[MESHCORE_COMPANION_MAX_FRAME_SIZE] = {0};
static uint16_t                   rx_position                                  = 0;
static companion_command_packet_t command_packet_buffer                        = {0};

static void mc_companion_handle_serial_command_frame(mc_companion_server_callback callback) {
    uint16_t length = sizeof(char) + sizeof(uint16_t) + (rx_buffer[1] | (rx_buffer[2] << 8));

    // Received packet is a command
    if (length < 1) {
        // Packet is too short
        return;
    }

    mc_companion_command_parser_error_t error = mc_companion_parse_command(&rx_buffer[3], length - 3, &command_packet_buffer);
    callback(&command_packet_buffer, error);
}

void mc_companion_read_serial_command(uint8_t* received_data, size_t received_data_length, mc_companion_server_callback server_callback) {
    while (received_data_length > 0) {
        if (rx_position == 0) {
            // Ready to receive a frame, search for start byte
            char start_byte = '<';
            while (received_data_length > 0) {
                if (*received_data == start_byte) {
                    // Found start byte
                    rx_buffer[rx_position] = *received_data;
                    rx_position++;
                    received_data = &received_data[1];
                    received_data_length--;
                    break;
                } else {
                    // printf("RX discarding byte: %02X\r\n", *received_data);
                }
                received_data = &received_data[1];
                received_data_length--;
            }
            continue;
        } else if (rx_position == 1 || rx_position == 2) {
            // Started receiving, store length bytes
            rx_buffer[rx_position] = *received_data;
            received_data          = &received_data[1];
            rx_position++;
            received_data_length--;
            continue;
        } else {
            // Receiving data
            uint16_t expected_length = sizeof(char) + sizeof(uint16_t) + (rx_buffer[1] | (rx_buffer[2] << 8));
            if (expected_length > sizeof(rx_buffer)) {
                // Invalid packet length, reset
                rx_position = 0;
                continue;
            }
            while (received_data_length > 0 && rx_position < expected_length) {
                rx_buffer[rx_position] = *received_data;
                rx_position++;
                received_data = &received_data[1];
                received_data_length--;
            }

            if (expected_length == rx_position) {
                // Received a full frame
                mc_companion_handle_serial_command_frame(server_callback);
                rx_position = 0;
            }
        }
    }
}

void mc_companion_write_serial_response(companion_response_packet_t* packet, uint16_t args_length, size_t output_buffer_size, uint8_t* out_framed_data,
                                        size_t* out_framed_data_length) {

    uint16_t packet_length = args_length;
    uint8_t  packet_type   = packet->response;
    uint8_t* packet_data   = packet->args;

    packet_length++;

    // Frame the data
    uint16_t position         = 0;
    out_framed_data[position] = '>';
    position++;
    out_framed_data[position] = (packet_length >> 0) & 0xFF;
    position++;
    out_framed_data[position] = (packet_length >> 8) & 0xFF;
    position++;
    out_framed_data[position] = packet_type;
    position++;
    if (packet_length > 0) {
        memcpy(&out_framed_data[position], packet_data, packet_length);
    }
    position += packet_length;

    *out_framed_data_length = position - 1;
}
