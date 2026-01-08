// SPDX-FileCopyrightText: 2026 Nicolai Electronics
// SPDX-FileCopyrightText: 2025 Scott Powell / rippleradios.com
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MESHCORE_COMPANION_MAX_FRAME_SIZE   172
#define MESHCORE_COMPANION_MAX_PAYLOAD_SIZE (MESHCORE_COMPANION_MAX_FRAME_SIZE - sizeof(uint8_t) - sizeof(uint16_t))  // Frame size minus start byte and length
#define MESHCORE_COMPANION_PUBLIC_KEY_SIZE  32

typedef enum {
    COMPANION_CMD_APP_START               = 1,
    COMPANION_CMD_SEND_TXT_MSG            = 2,
    COMPANION_CMD_SEND_CHANNEL_TXT_MSG    = 3,
    COMPANION_CMD_GET_CONTACTS            = 4,  // with optional 'since' (for efficient sync)
    COMPANION_CMD_GET_DEVICE_TIME         = 5,
    COMPANION_CMD_SET_DEVICE_TIME         = 6,
    COMPANION_CMD_SEND_SELF_ADVERT        = 7,
    COMPANION_CMD_SET_ADVERT_NAME         = 8,
    COMPANION_CMD_ADD_UPDATE_CONTACT      = 9,
    COMPANION_CMD_SYNC_NEXT_MESSAGE       = 10,
    COMPANION_CMD_SET_RADIO_PARAMS        = 11,
    COMPANION_CMD_SET_RADIO_TX_POWER      = 12,
    COMPANION_CMD_RESET_PATH              = 13,
    COMPANION_CMD_SET_ADVERT_LATLON       = 14,
    COMPANION_CMD_REMOVE_CONTACT          = 15,
    COMPANION_CMD_SHARE_CONTACT           = 16,
    COMPANION_CMD_EXPORT_CONTACT          = 17,
    COMPANION_CMD_IMPORT_CONTACT          = 18,
    COMPANION_CMD_REBOOT                  = 19,
    COMPANION_CMD_GET_BATT_AND_STORAGE    = 20,
    COMPANION_CMD_SET_TUNING_PARAMS       = 21,
    COMPANION_CMD_DEVICE_QUERY            = 22,
    COMPANION_CMD_EXPORT_PRIVATE_KEY      = 23,
    COMPANION_CMD_IMPORT_PRIVATE_KEY      = 24,
    COMPANION_CMD_SEND_RAW_DATA           = 25,
    COMPANION_CMD_SEND_LOGIN              = 26,
    COMPANION_CMD_SEND_STATUS_REQ         = 27,
    COMPANION_CMD_HAS_CONNECTION          = 28,
    COMPANION_CMD_LOGOUT                  = 29,
    COMPANION_CMD_GET_CONTACT_BY_KEY      = 30,
    COMPANION_CMD_GET_CHANNEL             = 31,
    COMPANION_CMD_SET_CHANNEL             = 32,
    COMPANION_CMD_SIGN_START              = 33,
    COMPANION_CMD_SIGN_DATA               = 34,
    COMPANION_CMD_SIGN_FINISH             = 35,
    COMPANION_CMD_SEND_TRACE_PATH         = 36,
    COMPANION_CMD_SET_DEVICE_PIN          = 37,
    COMPANION_CMD_SET_OTHER_PARAMS        = 38,
    COMPANION_CMD_SEND_TELEMETRY_REQ      = 39,  // can deprecate this
    COMPANION_CMD_GET_CUSTOM_VARS         = 40,
    COMPANION_CMD_SET_CUSTOM_VAR          = 41,
    COMPANION_CMD_GET_ADVERT_PATH         = 42,
    COMPANION_CMD_GET_TUNING_PARAMS       = 43,
    // NOTE: CMD range 44..49 parked, potentially for WiFi operations
    COMPANION_CMD_SEND_BINARY_REQ         = 50,
    COMPANION_CMD_FACTORY_RESET           = 51,
    COMPANION_CMD_SEND_PATH_DISCOVERY_REQ = 52,
    COMPANION_CMD_SET_FLOOD_SCOPE         = 54,  // v8+
    COMPANION_CMD_SEND_CONTROL_DATA       = 55,  // v8+
    COMPANION_CMD_GET_STATS               = 56,  // v8+, second byte is stats type
} companion_command_t;

// For COMPANION_CMD_GET_STATS
typedef enum {
    COMPANION_STATS_TYPE_CORE    = 0,
    COMPANION_STATS_TYPE_RADIO   = 1,
    COMPANION_STATS_TYPE_PACKETS = 2,
} companion_stats_t;

typedef enum {
    COMPANION_RESPONSE_CODE_OK                  = 0,
    COMPANION_RESPONSE_CODE_ERR                 = 1,
    COMPANION_RESPONSE_CODE_CONTACTS_START      = 2,   // first reply to CMD_GET_CONTACTS
    COMPANION_RESPONSE_CODE_CONTACT             = 3,   // multiple of these (after CMD_GET_CONTACTS)
    COMPANION_RESPONSE_CODE_END_OF_CONTACTS     = 4,   // last reply to CMD_GET_CONTACTS
    COMPANION_RESPONSE_CODE_SELF_INFO           = 5,   // reply to CMD_APP_START
    COMPANION_RESPONSE_CODE_SENT                = 6,   // reply to CMD_SEND_TXT_MSG
    COMPANION_RESPONSE_CODE_CONTACT_MSG_RECV    = 7,   // a reply to CMD_SYNC_NEXT_MESSAGE (ver < 3)
    COMPANION_RESPONSE_CODE_CHANNEL_MSG_RECV    = 8,   // a reply to CMD_SYNC_NEXT_MESSAGE (ver < 3)
    COMPANION_RESPONSE_CODE_CURR_TIME           = 9,   // a reply to CMD_GET_DEVICE_TIME
    COMPANION_RESPONSE_CODE_NO_MORE_MESSAGES    = 10,  // a reply to CMD_SYNC_NEXT_MESSAGE
    COMPANION_RESPONSE_CODE_EXPORT_CONTACT      = 11,
    COMPANION_RESPONSE_CODE_BATT_AND_STORAGE    = 12,  // a reply to a CMD_GET_BATT_AND_STORAGE
    COMPANION_RESPONSE_CODE_DEVICE_INFO         = 13,  // a reply to CMD_DEVICE_QEURY
    COMPANION_RESPONSE_CODE_PRIVATE_KEY         = 14,  // a reply to CMD_EXPORT_PRIVATE_KEY
    COMPANION_RESPONSE_CODE_DISABLED            = 15,
    COMPANION_RESPONSE_CODE_CONTACT_MSG_RECV_V3 = 16,  // a reply to CMD_SYNC_NEXT_MESSAGE (ver >= 3)
    COMPANION_RESPONSE_CODE_CHANNEL_MSG_RECV_V3 = 17,  // a reply to CMD_SYNC_NEXT_MESSAGE (ver >= 3)
    COMPANION_RESPONSE_CODE_CHANNEL_INFO        = 18,  // a reply to CMD_GET_CHANNEL
    COMPANION_RESPONSE_CODE_SIGN_START          = 19,
    COMPANION_RESPONSE_CODE_SIGNATURE           = 20,
    COMPANION_RESPONSE_CODE_CUSTOM_VARS         = 21,
    COMPANION_RESPONSE_CODE_ADVERT_PATH         = 22,
    COMPANION_RESPONSE_CODE_TUNING_PARAMS       = 23,
    COMPANION_RESPONSE_CODE_STATS               = 24,  // v8+, second byte is stats type
} companion_response_code_t;

typedef enum {
    COMPANION_PUSH_CODE_ADVERT                  = 0x80,
    COMPANION_PUSH_CODE_PATH_UPDATED            = 0x81,
    COMPANION_PUSH_CODE_SEND_CONFIRMED          = 0x82,
    COMPANION_PUSH_CODE_MSG_WAITING             = 0x83,
    COMPANION_PUSH_CODE_RAW_DATA                = 0x84,
    COMPANION_PUSH_CODE_LOGIN_SUCCESS           = 0x85,
    COMPANION_PUSH_CODE_LOGIN_FAIL              = 0x86,
    COMPANION_PUSH_CODE_STATUS_RESPONSE         = 0x87,
    COMPANION_PUSH_CODE_LOG_RX_DATA             = 0x88,
    COMPANION_PUSH_CODE_TRACE_DATA              = 0x89,
    COMPANION_PUSH_CODE_NEW_ADVERT              = 0x8A,
    COMPANION_PUSH_CODE_TELEMETRY_RESPONSE      = 0x8B,
    COMPANION_PUSH_CODE_BINARY_RESPONSE         = 0x8C,
    COMPANION_PUSH_CODE_PATH_DISCOVERY_RESPONSE = 0x8D,
    COMPANION_PUSH_CODE_CONTROL_DATA            = 0x8E,  // v8+
} companion_push_code_t;

typedef enum {
    COMPANION_ERROR_CODE_UNSUPPORTED_CMD = 1,
    COMPANION_ERROR_CODE_NOT_FOUND       = 2,
    COMPANION_ERROR_CODE_TABLE_FULL      = 3,
    COMPANION_ERROR_CODE_BAD_STATE       = 4,
    COMPANION_ERROR_CODE_FILE_IO_ERROR   = 5,
    COMPANION_ERROR_CODE_ILLEGAL_ARG     = 6,
} companion_error_code_t;

#define COMPANION_ADV_TYPE_NONE     0
#define COMPANION_ADV_TYPE_CHAT     1
#define COMPANION_ADV_TYPE_REPEATER 2
#define COMPANION_ADV_TYPE_ROOM     3
#define COMPANION_ADV_TYPE_SENSOR   4

// Command argument structures

typedef struct {
    uint8_t reserved[7];
    uint8_t app_name[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - sizeof(uint8_t) * 7];
} __attribute__((packed)) companion_cmd_app_start_args_t;

typedef struct {
    uint8_t  txt_type;
    uint8_t  attempt;
    uint32_t msg_timestamp;
    uint8_t  pub_key_prefix[6];
    uint8_t  text[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - sizeof(uint8_t) * 8 - sizeof(uint32_t)];
} __attribute__((packed)) companion_cmd_send_txt_msg_args_t;

typedef struct {
    uint8_t  txt_type;
    uint8_t  channel_idx;
    uint32_t msg_timestamp;
    uint8_t  text[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - sizeof(uint8_t) * 8 - sizeof(uint32_t)];
} __attribute__((packed)) companion_cmd_send_channel_txt_msg_args_t;

typedef struct {
    uint32_t since;
} __attribute__((packed)) companion_cmd_get_contacts_args_t;

typedef struct {
    uint32_t timestamp;
} __attribute__((packed)) companion_cmd_set_device_time_args_t;

typedef struct {
    char advert_name[32];
} __attribute__((packed)) companion_cmd_set_advert_name_args_t;

typedef struct {
    uint8_t  pub_key[32];
    uint8_t  type;
    uint8_t  flags;
    int8_t   out_path_len;
    uint8_t  out_path[64];
    char     name[32];
    uint32_t last_advert_timestamp;
    int32_t  gps_latitude;   // Optional
    int32_t  gps_longitude;  // Optional
    uint32_t last_modified;  // Optional
} __attribute__((packed)) companion_cmd_add_update_contact_args_t;

typedef struct {
    uint32_t frequency;
    uint32_t bandwidth;
    uint8_t  spreading_factor;
    uint8_t  coding_rate;
} __attribute__((packed)) companion_cmd_set_radio_params_args_t;

typedef struct {
    uint8_t tx_power;
} __attribute__((packed)) companion_cmd_set_radio_tx_power_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_reset_path_args_t;

typedef struct {
    int32_t latitude;
    int32_t longitude;
    int32_t altitude;  // Optional
} __attribute__((packed)) companion_cmd_set_advert_latlon_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_remove_contact_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_share_contact_args_t;

typedef struct {
    uint8_t pub_key[32];  // Optional (if not provided export self)
} __attribute__((packed)) companion_cmd_export_contact_args_t;

typedef struct {
    uint8_t advert_packet[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE];
} __attribute__((packed)) companion_cmd_import_contact_args_t;

typedef struct {
    char magic[6];  // should be "reboot"
} __attribute__((packed)) companion_cmd_reboot_args_t;

typedef struct {
    uint32_t rx;
    uint32_t af;
} __attribute__((packed)) companion_cmd_set_tuning_params_args_t;

typedef struct {
    uint8_t app_target_version;
} __attribute__((packed)) companion_cmd_device_query_args_t;

typedef struct {
    uint8_t private_key[64];
} __attribute__((packed)) companion_cmd_import_private_key_args_t;

typedef struct {
    int8_t  path_length;
    uint8_t path_and_packet[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - sizeof(int8_t)];  // path[path_length] + raw data
} __attribute__((packed)) companion_cmd_send_raw_data_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_login_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_status_req_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_has_connection_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_logout_args_t;

typedef struct {
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_get_contact_by_key_args_t;

typedef struct {
    uint8_t channel_idx;
} __attribute__((packed)) companion_cmd_get_channel_args_t;

typedef struct {
    uint8_t channel_idx;
    uint8_t channel_name[32];
    uint8_t channel_secret[16];
} __attribute__((packed)) companion_cmd_set_channel_args_t;

typedef struct {
    uint8_t data[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE];
} __attribute__((packed)) companion_cmd_sign_data_args_t;

typedef struct {
    uint32_t tag;
    uint32_t auth;
    uint8_t  flags;
    uint8_t  path[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - sizeof(uint32_t) * 2 - sizeof(uint8_t)];
} __attribute__((packed)) companion_cmd_send_trace_path_args_t;

typedef struct {
    uint8_t ble_pin[4];
} __attribute__((packed)) companion_cmd_set_device_pin_args_t;

typedef struct {
    uint8_t manual_add_contacts;
    uint8_t flags;                   // Optional
    uint8_t advert_location_policy;  // Optional
    uint8_t multi_acks;              // Optional
} __attribute__((packed)) companion_cmd_set_other_params_args_t;

typedef struct {
    uint8_t reserved[3];
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_send_telemetry_req_args_t;

typedef struct {
    char setting_value[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE];  // setting name and value separated by :
} __attribute__((packed)) companion_cmd_set_custom_var_args_t;

typedef struct {
    uint8_t reserved;
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_get_advert_path_args_t;

typedef struct {
    uint8_t pub_key[32];
    uint8_t req_data[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - 32];
} __attribute__((packed)) companion_cmd_send_binary_req_args_t;

typedef struct {
    char magic[5];  // should be "reset"
} __attribute__((packed)) companion_cmd_factory_reset_args_t;

typedef struct {
    uint8_t reserved;
    uint8_t pub_key[32];
} __attribute__((packed)) companion_cmd_send_path_discovery_req_args_t;

typedef struct {
    uint8_t reserved;
    uint8_t key[16];
} __attribute__((packed)) companion_cmd_flood_scope_args_t;

typedef struct {
    uint8_t data[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE];
} __attribute__((packed)) companion_cmd_send_control_data_args_t;

// Response argument structures

typedef struct {
    uint32_t count;
} __attribute__((packed)) companion_resp_contacts_start_t;

typedef struct {
    uint32_t since;
} __attribute__((packed)) companion_resp_end_of_contacts_t;

typedef struct {
    uint8_t  adv_type;
    uint8_t  configured_tx_power;
    uint8_t  maximum_tx_power;
    uint8_t  public_key[MESHCORE_COMPANION_PUBLIC_KEY_SIZE];
    int32_t  position_latitude;
    int32_t  position_longitude;
    uint8_t  multi_acks;
    uint8_t  advert_loc_policy;
    uint8_t  telemetry_mode;
    uint8_t  manual_add_contacts;
    uint32_t frequency;
    uint32_t bandwidth;
    uint8_t  spreading_factor;
    uint8_t  coding_rate;
    char     node_name[MESHCORE_COMPANION_MAX_PAYLOAD_SIZE - MESHCORE_COMPANION_PUBLIC_KEY_SIZE - 25];
} __attribute__((packed)) companion_resp_self_info_args_t;

typedef struct {
    uint8_t firmware_version_code;
    uint8_t max_contacts;  // Divide by 2
    uint8_t max_group_channels;
    uint8_t ble_pin[4];
    char    firmware_build_date[12];
    char    board_manufacturer_name[40];
    char    firmware_version[20];
} __attribute__((packed)) companion_resp_device_info_args_t;

// Packet structure

typedef enum {
    COMPANION_PACKET_TYPE_NONE     = 0,
    COMPANION_PACKET_TYPE_COMMAND  = 1,
    COMPANION_PACKET_TYPE_RESPONSE = 2,
    COMPANION_PACKET_TYPE_PUSH     = 3,
    COMPANION_PACKET_TYPE_ERROR    = 4,
    COMPANION_PACKET_TYPE_OK       = 5,
} companion_packet_type_t;

typedef struct {
    companion_packet_type_t type;
    union {
        companion_command_t       command;
        companion_response_code_t response;
        companion_push_code_t     push;
        companion_error_code_t    error;
    };
    union {
        uint8_t                                      args[0];
        // Commands
        companion_cmd_app_start_args_t               command_app_start_args;
        companion_cmd_send_txt_msg_args_t            command_send_txt_msg_args;
        companion_cmd_send_channel_txt_msg_args_t    command_send_channel_txt_msg_args;
        companion_cmd_get_contacts_args_t            command_get_contacts_args;
        companion_cmd_set_device_time_args_t         command_set_device_time_args;
        companion_cmd_set_advert_name_args_t         command_set_advert_name_args;
        companion_cmd_add_update_contact_args_t      command_add_update_contact_args;
        companion_cmd_set_radio_params_args_t        command_set_radio_params_args;
        companion_cmd_set_radio_tx_power_args_t      command_set_radio_tx_power_args;
        companion_cmd_reset_path_args_t              command_reset_path_args;
        companion_cmd_set_advert_latlon_args_t       command_set_advert_latlon_args;
        companion_cmd_remove_contact_args_t          command_remove_contact_args;
        companion_cmd_share_contact_args_t           command_share_contact_args;
        companion_cmd_export_contact_args_t          command_export_contact_args;
        companion_cmd_import_contact_args_t          command_import_contact_args;
        companion_cmd_reboot_args_t                  command_reboot_args;
        companion_cmd_set_tuning_params_args_t       command_set_tuning_params_args;
        companion_cmd_device_query_args_t            command_device_query_args;
        companion_cmd_import_private_key_args_t      command_import_private_key_args;
        companion_cmd_send_raw_data_args_t           command_send_raw_data_args;
        companion_cmd_login_args_t                   command_login_args;
        companion_cmd_status_req_args_t              command_status_req_args;
        companion_cmd_has_connection_args_t          command_has_connection_args;
        companion_cmd_logout_args_t                  command_logout_args;
        companion_cmd_get_contact_by_key_args_t      command_get_contact_by_key_args;
        companion_cmd_get_channel_args_t             command_get_channel_args;
        companion_cmd_set_channel_args_t             command_set_channel_args;
        companion_cmd_sign_data_args_t               command_sign_data_args;
        companion_cmd_send_trace_path_args_t         command_send_trace_path_args;
        companion_cmd_set_device_pin_args_t          command_set_device_pin_args;
        companion_cmd_set_other_params_args_t        command_set_other_params_args;
        companion_cmd_send_telemetry_req_args_t      command_send_telemetry_req_args;
        companion_cmd_set_custom_var_args_t          command_set_custom_var_args;
        companion_cmd_get_advert_path_args_t         command_get_advert_path_args;
        companion_cmd_send_binary_req_args_t         command_send_binary_req_args;
        companion_cmd_factory_reset_args_t           command_factory_reset_args;
        companion_cmd_send_path_discovery_req_args_t command_send_path_discovery_req_args;
        companion_cmd_flood_scope_args_t             command_flood_scope_args;
        companion_cmd_send_control_data_args_t       command_send_control_data_args;
        // Responses
        companion_resp_contacts_start_t              response_contacts_start_args;
        companion_resp_end_of_contacts_t             response_end_of_contacts_args;
        companion_resp_self_info_args_t              response_self_info_args;
        companion_resp_device_info_args_t            response_device_info_args;
    };
} companion_packet_t;

// Callback

typedef void (*mc_companion_receive_callback)(companion_packet_t* packet);
