#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "mc_companion.h"
#include "mc_companion_serial_interface.h"

#define FIELD_SIZE(type, field) (sizeof(((type*)0)->field))

static int                         serial_port                                  = -1;
static companion_response_packet_t tx_packet                                    = {0};
static uint8_t                     tx_buffer[MESHCORE_COMPANION_MAX_FRAME_SIZE] = {0};

static void transmit(uint8_t* data, size_t length) {
    write(serial_port, data, length);
}

void packet_callback(companion_command_packet_t* packet, mc_companion_command_parser_error_t error) {
    size_t tx_length = 0;
    memset(&tx_packet, 0, sizeof(tx_packet));

    if (error != COMPANION_COMMAND_PARSER_ERROR_NONE) {
        // Invalid command or arguments
        printf("Invalid %s\r\n", (error == COMPANION_COMMAND_PARSER_ERROR_INVALID_COMMAND) ? "command" : "arguments");
        tx_packet.response = COMPANION_RESPONSE_CODE_ERR;
        tx_packet.response_err_args.error_code =
            (error == COMPANION_COMMAND_PARSER_ERROR_INVALID_COMMAND) ? COMPANION_ERROR_CODE_UNSUPPORTED_CMD : COMPANION_ERROR_CODE_ILLEGAL_ARG;
        mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
        transmit(tx_buffer, tx_length);
        return;
    }

    switch (packet->command) {
        case COMPANION_CMD_APP_START:
            printf("Received app start command. Application name is '%s'\r\n", packet->command_app_start_args.app_name);
            tx_packet.response                                    = COMPANION_RESPONSE_CODE_SELF_INFO;
            tx_packet.response_self_info_args.adv_type            = COMPANION_ADV_TYPE_CHAT;
            tx_packet.response_self_info_args.configured_tx_power = 22;
            tx_packet.response_self_info_args.maximum_tx_power    = 22;
            memset(tx_packet.response_self_info_args.public_key, 0, MESHCORE_COMPANION_PUBLIC_KEY_SIZE);
            tx_packet.response_self_info_args.position_latitude   = 0;
            tx_packet.response_self_info_args.position_longitude  = 0;
            tx_packet.response_self_info_args.multi_acks          = 0;
            tx_packet.response_self_info_args.advert_loc_policy   = 0;
            tx_packet.response_self_info_args.telemetry_mode      = 0;
            tx_packet.response_self_info_args.manual_add_contacts = 0;
            tx_packet.response_self_info_args.frequency           = 868000;
            tx_packet.response_self_info_args.bandwidth           = 62500;
            tx_packet.response_self_info_args.spreading_factor    = 8;
            tx_packet.response_self_info_args.coding_rate         = 8;
            snprintf(tx_packet.response_self_info_args.node_name, FIELD_SIZE(companion_resp_self_info_args_t, node_name), "Roadrunner");
            mc_companion_write_serial_response(&tx_packet,
                                               sizeof(companion_resp_self_info_args_t) - FIELD_SIZE(companion_resp_self_info_args_t, node_name) +
                                                   strlen(tx_packet.response_self_info_args.node_name),
                                               sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_GET_CONTACTS:
            companion_contact_t contacts[] = {
                {
                    .public_key            = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                              0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20},
                    .type                  = 1,
                    .flags                 = 0,
                    .out_path_len          = 3,
                    .out_path              = {1, 2, 3},
                    .name                  = "Alice",
                    .last_advert_timestamp = 1625158800,
                    .gps_latitude          = 52345678,
                    .gps_longitude         = 13456789,
                    .last_modified         = 1625158800,
                },
                {
                    .public_key            = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
                                              0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40},
                    .type                  = 1,
                    .flags                 = 0,
                    .out_path_len          = 2,
                    .out_path              = {4, 5},
                    .name                  = "Bob",
                    .last_advert_timestamp = 1625158800,
                    .gps_latitude          = 52345678,
                    .gps_longitude         = 13456789,
                    .last_modified         = 1625158800,
                },
            };

            printf("Received get contacts command, sending %u contacts\r\n", sizeof(contacts) / sizeof(companion_contact_t));

            tx_packet.response                           = COMPANION_RESPONSE_CODE_CONTACTS_START;
            tx_packet.response_contacts_start_args.count = sizeof(contacts) / sizeof(companion_contact_t);
            mc_companion_write_serial_response(&tx_packet, sizeof(companion_resp_contacts_start_t), sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);

            tx_packet.response = COMPANION_RESPONSE_CODE_CONTACT;
            for (size_t i = 0; i < sizeof(contacts) / sizeof(companion_contact_t); i++) {
                memcpy(&tx_packet.response_contact_args, &contacts[i], sizeof(companion_contact_t));
                mc_companion_write_serial_response(&tx_packet, sizeof(companion_contact_t), sizeof(tx_buffer), tx_buffer, &tx_length);
                transmit(tx_buffer, tx_length);
            }

            tx_packet.response                            = COMPANION_RESPONSE_CODE_END_OF_CONTACTS;
            tx_packet.response_end_of_contacts_args.since = 0;
            mc_companion_write_serial_response(&tx_packet, sizeof(companion_resp_end_of_contacts_t), sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_SEND_SELF_ADVERT:
            printf("Received send self advert command, should send advertisement\r\n");
            tx_packet.response = COMPANION_RESPONSE_CODE_OK;
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_SYNC_NEXT_MESSAGE:
            printf("Received sync next message command\r\n");
            tx_packet.response = COMPANION_RESPONSE_CODE_NO_MORE_MESSAGES;
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_DEVICE_QUERY:
            printf("Received device query command. Target app version is %u\r\n", packet->command_device_query_args.app_target_version);
            // Respond with device info
            tx_packet.response                                        = COMPANION_RESPONSE_CODE_DEVICE_INFO;
            tx_packet.response_device_info_args.firmware_version_code = 8;
            tx_packet.response_device_info_args.max_contacts          = 100 / 2;
            tx_packet.response_device_info_args.max_group_channels    = 40;
            tx_packet.response_device_info_args.ble_pin[0]            = 0;
            tx_packet.response_device_info_args.ble_pin[1]            = 0;
            tx_packet.response_device_info_args.ble_pin[2]            = 0;
            tx_packet.response_device_info_args.ble_pin[3]            = 0;
            snprintf(tx_packet.response_device_info_args.firmware_build_date, FIELD_SIZE(companion_resp_device_info_args_t, firmware_build_date), "5 Jan 2026");
            snprintf(tx_packet.response_device_info_args.board_manufacturer_name, FIELD_SIZE(companion_resp_device_info_args_t, board_manufacturer_name),
                     "Acme Corporation");
            snprintf(tx_packet.response_device_info_args.firmware_version, FIELD_SIZE(companion_resp_device_info_args_t, firmware_version), "v1.11.0");
            mc_companion_write_serial_response(&tx_packet, sizeof(companion_resp_device_info_args_t), sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_GET_CHANNEL:
            printf("Received get channel command for channel ID %u\r\n", packet->command_get_channel_args.channel_idx);
            tx_packet.response                     = COMPANION_RESPONSE_CODE_ERR;
            tx_packet.response_err_args.error_code = COMPANION_ERROR_CODE_NOT_FOUND;
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_GET_CUSTOM_VARS:
            printf("Received get custom vars command\r\n");
            tx_packet.response = COMPANION_RESPONSE_CODE_CUSTOM_VARS;
            snprintf(tx_packet.response_custom_vars_args.data, FIELD_SIZE(companion_resp_custom_vars_args_t, data), "");
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        case COMPANION_CMD_SET_FLOOD_SCOPE:
            printf("Received set flood scope command\r\n");
            tx_packet.response = COMPANION_RESPONSE_CODE_OK;
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
        default:
            printf("Received unhandled command: %u\r\n", packet->command);
            tx_packet.response                     = COMPANION_RESPONSE_CODE_ERR;
            tx_packet.response_err_args.error_code = COMPANION_ERROR_CODE_UNSUPPORTED_CMD;
            mc_companion_write_serial_response(&tx_packet, 0, sizeof(tx_buffer), tx_buffer, &tx_length);
            transmit(tx_buffer, tx_length);
            break;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s port baudrate\r\n", argv[0]);
        return 1;
    }
    printf("Meshcore compantion radio protocol server\r\n");

    char* port     = argv[1];
    int   baudrate = atoi(argv[2]);

    printf("Listening on port %s @ %d baud\r\n", port, baudrate);

    serial_port = open(port, O_RDWR);

    // Check for errors
    if (serial_port < 0) {
        printf("Failed to open serial port (%i): %s\n", errno, strerror(errno));
        return 1;
    }

    printf("Opened serial port\r\n");

    tcflush(serial_port, TCIOFLUSH);  // Flush any existing data

    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Failed to read attributes (%i): %s\n", errno, strerror(errno));
        return 1;
    }

    // 8 bits per byte, one stop bit, no parity, allow reading, disable modem-specific signals
    tty.c_cflag = CS8 | CREAD | CLOCAL;

    // Disable canonical mode
    tty.c_lflag = 0;

    // Read returns after receiving at least one byte
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN]  = 1;

    // Set baudrate
    switch (baudrate) {
        case 4800:
            cfsetospeed(&tty, B4800);
            break;
        case 9600:
            cfsetospeed(&tty, B9600);
            break;
        case 19200:
            cfsetospeed(&tty, B19200);
            break;
        case 38400:
            cfsetospeed(&tty, B38400);
            break;
        case 115200:
            cfsetospeed(&tty, B115200);
            break;
        default:
            fprintf(stderr, "warning: baud rate %u is not supported, using 115200.\n", baudrate);
            cfsetospeed(&tty, B115200);
            break;
    }
    cfsetispeed(&tty, cfgetospeed(&tty));

    cfmakeraw(&tty);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Failed to write attributes (%i): %s\n", errno, strerror(errno));
        return 1;
    }

    while (1) {
        uint8_t read_buffer[MESHCORE_COMPANION_MAX_FRAME_SIZE] = {0};
        int     num_read                                       = read(serial_port, &read_buffer, sizeof(read_buffer));
        if (num_read < 1) {
            printf("Failed to read from serial port, exiting\r\n");
            break;
        }

        mc_companion_read_serial_command(read_buffer, num_read, packet_callback);
    }
}
