#include "mc_companion.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FIELD_SIZE(type, field) (sizeof(((type*)0)->field))

static uint8_t            rx_buffer[MESHCORE_COMPANION_MAX_FRAME_SIZE] = {0};
static uint16_t           rx_position                                  = 0;
static companion_packet_t packet_buffer                                = {0};

static void mc_companion_handle_rx_command(companion_command_t command, uint8_t* data, uint16_t data_length,
                                           mc_companion_receive_callback callback) {
    packet_buffer.type    = COMPANION_PACKET_TYPE_COMMAND;
    packet_buffer.command = command;
    switch (command) {
        case COMPANION_CMD_APP_START: {
            size_t min_argument_length = FIELD_SIZE(companion_cmd_app_start_args_t, reserved);
            size_t max_argument_length = sizeof(companion_cmd_app_start_args_t) - sizeof('\0');
            if (data_length < min_argument_length || data_length > max_argument_length) return;  // Invalid data length
            memcpy(packet_buffer.args, data, data_length);
            packet_buffer.args[data_length] = '\0';  // NULL terminate the application name string
            break;
        }
        case COMPANION_CMD_GET_CONTACTS: {
            size_t argument_length = sizeof(companion_cmd_get_contacts_args_t);
            if (data_length == argument_length) {
                memcpy(packet_buffer.args, data, argument_length);
            } else {
                memset(packet_buffer.args, 0, argument_length);
            }
            packet_buffer.args[data_length] = '\0';  // NULL terminate the application name string
            break;
        }
        case COMPANION_CMD_SYNC_NEXT_MESSAGE: {
            // No arguments
            break;
        }
        case COMPANION_CMD_DEVICE_QUERY: {
            size_t argument_length = sizeof(companion_cmd_device_query_args_t);
            if (data_length != argument_length) return;  // Invalid data length
            memcpy(packet_buffer.args, data, data_length);
            break;
        }
        case COMPANION_CMD_GET_CHANNEL: {
            size_t argument_length = sizeof(companion_cmd_get_channel_args_t);
            if (data_length != argument_length) return;  // Invalid data length
            memcpy(packet_buffer.args, data, data_length);
            break;
        }
        case COMPANION_CMD_SET_FLOOD_SCOPE: {
            size_t argument_length = sizeof(companion_cmd_flood_scope_args_t);
            if (data_length != argument_length) return;  // Invalid data length
            memcpy(packet_buffer.args, data, data_length);
            break;
        }
        default:
            packet_buffer.type  = COMPANION_PACKET_TYPE_ERROR,
            packet_buffer.error = COMPANION_ERROR_CODE_UNSUPPORTED_CMD;
            break;  // Unsupported command
    }

    callback(&packet_buffer);
}

static void mc_companion_handle_serial_frame(bool is_server, mc_companion_receive_callback callback) {
    uint16_t length = (rx_buffer[1] | (rx_buffer[2] << 8));
    uint8_t* data   = &rx_buffer[3];

    if (is_server) {
        // Received packet is a command
        if (length < 1) {
            // Packet is too short
            return;
        }
        companion_command_t command = (companion_command_t)data[0];
        printf("Handle command %u\r\n", data[0]);
        mc_companion_handle_rx_command(command, &data[1], length - 1, callback);
    }
}

void mc_companion_read_serial_frame(bool is_server, uint8_t* received_data, size_t received_data_length,
                                    mc_companion_receive_callback callback) {
    printf("RX: ");
    for (size_t i = 0; i < received_data_length; i++) {
        printf("%02X", received_data[i]);
    }
    printf("\r\n");
    while (received_data_length > 0) {
        if (rx_position == 0) {
            // Ready to receive a frame, search for start byte
            char start_byte = is_server ? '<' : '>';
            while (received_data_length > 0) {
                if (*received_data == start_byte) {
                    // Found start byte
                    rx_buffer[rx_position] = *received_data;
                    rx_position++;
                    received_data = &received_data[1];
                    received_data_length--;
                    break;
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
            uint16_t expected_length = (rx_buffer[1] | (rx_buffer[2] << 8)) + 3;  // 3 = header length
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
                mc_companion_handle_serial_frame(is_server, callback);
                rx_position = 0;
            }
        }
    }
}

void mc_companion_write_serial_frame(bool is_server, companion_packet_t* packet, size_t output_buffer_size,
                                     uint8_t* out_framed_data, size_t* out_framed_data_length) {

    uint8_t  packet_type   = 0;
    uint16_t packet_length = 0;
    uint8_t* packet_data   = packet->args;

    switch (packet->type) {
        case COMPANION_PACKET_TYPE_COMMAND:
            break;
        case COMPANION_PACKET_TYPE_RESPONSE:
            packet_type = (uint8_t)packet->response;
            switch (packet->response) {
                case COMPANION_RESPONSE_CODE_CONTACTS_START:
                    packet_length = sizeof(companion_resp_contacts_start_t);
                    break;
                case COMPANION_RESPONSE_CODE_END_OF_CONTACTS:
                    packet_length = sizeof(companion_resp_end_of_contacts_t);
                    break;
                case COMPANION_RESPONSE_CODE_SELF_INFO:
                    packet_length = sizeof(companion_resp_self_info_args_t) -
                                    FIELD_SIZE(companion_resp_self_info_args_t, node_name) +
                                    strlen(packet->response_self_info_args.node_name);
                    break;
                case COMPANION_RESPONSE_CODE_NO_MORE_MESSAGES:
                    packet_length = 0;
                    break;
                case COMPANION_RESPONSE_CODE_DEVICE_INFO:
                    packet_length = sizeof(companion_resp_device_info_args_t);
                    break;
                default:
                    packet_length = 0;
                    break;
            }
            break;
        case COMPANION_PACKET_TYPE_PUSH:
            break;
        case COMPANION_PACKET_TYPE_ERROR:
            packet_type   = COMPANION_RESPONSE_CODE_ERR;
            packet_length = 1;
            break;
        case COMPANION_PACKET_TYPE_OK:
            packet_type   = COMPANION_RESPONSE_CODE_OK;
            packet_length = 0;
            break;
        case COMPANION_PACKET_TYPE_NONE:
        default:
            break;
    }

    packet_length++;

    // Frame the data
    uint16_t position         = 0;
    out_framed_data[position] = is_server ? '>' : '<';
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