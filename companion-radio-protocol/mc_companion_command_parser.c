#include "mc_companion_command_parser.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mc_companion.h"

#define FIELD_SIZE(type, field) (sizeof(((type*)0)->field))

typedef struct {
    companion_command_t command;
    size_t              min_argument_length;
    size_t              max_argument_length;
} companion_command_definition_t;

companion_command_definition_t companion_command_definitions[] = {
    {COMPANION_CMD_APP_START, FIELD_SIZE(companion_cmd_app_start_args_t, reserved), sizeof(companion_cmd_app_start_args_t) - sizeof('\0')},
    {COMPANION_CMD_SEND_TXT_MSG, sizeof(companion_cmd_send_txt_msg_args_t) - FIELD_SIZE(companion_cmd_send_txt_msg_args_t, text),
     sizeof(companion_cmd_send_txt_msg_args_t)},
    {COMPANION_CMD_SEND_CHANNEL_TXT_MSG, sizeof(companion_cmd_send_channel_txt_msg_args_t) - FIELD_SIZE(companion_cmd_send_channel_txt_msg_args_t, text),
     sizeof(companion_cmd_send_channel_txt_msg_args_t)},
    {COMPANION_CMD_GET_CONTACTS, 0, sizeof(companion_cmd_get_contacts_args_t)},
    {COMPANION_CMD_GET_DEVICE_TIME, 0, 0},
    {COMPANION_CMD_SET_DEVICE_TIME, sizeof(companion_cmd_set_device_time_args_t), sizeof(companion_cmd_set_device_time_args_t)},
    {COMPANION_CMD_SEND_SELF_ADVERT, 0, 0},
    {COMPANION_CMD_SET_ADVERT_NAME, 1, sizeof(companion_cmd_set_advert_name_args_t)},
    {COMPANION_CMD_ADD_UPDATE_CONTACT,
     sizeof(companion_contact_t) - FIELD_SIZE(companion_contact_t, gps_latitude) - FIELD_SIZE(companion_contact_t, gps_longitude) -
         FIELD_SIZE(companion_contact_t, last_modified),
     sizeof(companion_contact_t)},
    {COMPANION_CMD_SYNC_NEXT_MESSAGE, 0, 0},
    {COMPANION_CMD_SET_RADIO_PARAMS, sizeof(companion_cmd_set_radio_params_args_t), sizeof(companion_cmd_set_radio_params_args_t)},
    {COMPANION_CMD_SET_RADIO_TX_POWER, sizeof(companion_cmd_set_radio_tx_power_args_t), sizeof(companion_cmd_set_radio_tx_power_args_t)},
    {COMPANION_CMD_RESET_PATH, sizeof(companion_cmd_reset_path_args_t), sizeof(companion_cmd_reset_path_args_t)},
    {COMPANION_CMD_SET_ADVERT_LATLON, sizeof(companion_cmd_set_advert_latlon_args_t) - FIELD_SIZE(companion_cmd_set_advert_latlon_args_t, altitude),
     sizeof(companion_cmd_set_advert_latlon_args_t)},
    {COMPANION_CMD_REMOVE_CONTACT, sizeof(companion_cmd_remove_contact_args_t), sizeof(companion_cmd_remove_contact_args_t)},
    {COMPANION_CMD_SHARE_CONTACT, sizeof(companion_cmd_share_contact_args_t), sizeof(companion_cmd_share_contact_args_t)},
    {COMPANION_CMD_EXPORT_CONTACT, 0, sizeof(companion_cmd_export_contact_args_t)},
    {COMPANION_CMD_IMPORT_CONTACT, 0, sizeof(companion_cmd_import_contact_args_t)},
    {COMPANION_CMD_REBOOT, sizeof(companion_cmd_reboot_args_t), sizeof(companion_cmd_reboot_args_t)},
    {COMPANION_CMD_GET_BATT_AND_STORAGE, 0, 0},
    {COMPANION_CMD_SET_TUNING_PARAMS, sizeof(companion_cmd_set_tuning_params_args_t), sizeof(companion_cmd_set_tuning_params_args_t)},
    {COMPANION_CMD_DEVICE_QUERY, sizeof(companion_cmd_device_query_args_t), sizeof(companion_cmd_device_query_args_t)},
    {COMPANION_CMD_EXPORT_PRIVATE_KEY, 0, 0},
    {COMPANION_CMD_IMPORT_PRIVATE_KEY, sizeof(companion_cmd_import_private_key_args_t), sizeof(companion_cmd_import_private_key_args_t)},
    {COMPANION_CMD_SEND_RAW_DATA, sizeof(companion_cmd_send_raw_data_args_t), sizeof(companion_cmd_send_raw_data_args_t)},
    {COMPANION_CMD_SEND_LOGIN, sizeof(companion_cmd_login_args_t), sizeof(companion_cmd_login_args_t)},
    {COMPANION_CMD_SEND_STATUS_REQ, sizeof(companion_cmd_status_req_args_t), sizeof(companion_cmd_status_req_args_t)},
    {COMPANION_CMD_HAS_CONNECTION, sizeof(companion_cmd_has_connection_args_t), sizeof(companion_cmd_has_connection_args_t)},
    {COMPANION_CMD_LOGOUT, sizeof(companion_cmd_logout_args_t), sizeof(companion_cmd_logout_args_t)},
    {COMPANION_CMD_GET_CONTACT_BY_KEY, sizeof(companion_cmd_get_contact_by_key_args_t), sizeof(companion_cmd_get_contact_by_key_args_t)},
    {COMPANION_CMD_GET_CHANNEL, sizeof(companion_cmd_get_channel_args_t), sizeof(companion_cmd_get_channel_args_t)},
    {COMPANION_CMD_SET_CHANNEL, sizeof(companion_cmd_set_channel_args_t), sizeof(companion_cmd_set_channel_args_t)},
    {COMPANION_CMD_SIGN_START, 0, 0},
    {COMPANION_CMD_SIGN_DATA, 1, sizeof(companion_cmd_sign_data_args_t)},
    {COMPANION_CMD_SIGN_FINISH, 0, 0},
    {COMPANION_CMD_SEND_TRACE_PATH, sizeof(companion_cmd_send_trace_path_args_t) - FIELD_SIZE(companion_cmd_send_trace_path_args_t, path),
     sizeof(companion_cmd_send_trace_path_args_t)},
    {COMPANION_CMD_SET_DEVICE_PIN, sizeof(companion_cmd_set_device_pin_args_t), sizeof(companion_cmd_set_device_pin_args_t)},
    {COMPANION_CMD_SET_OTHER_PARAMS,
     sizeof(companion_cmd_set_other_params_args_t) - FIELD_SIZE(companion_cmd_set_other_params_args_t, flags) -
         FIELD_SIZE(companion_cmd_set_other_params_args_t, advert_location_policy) - FIELD_SIZE(companion_cmd_set_other_params_args_t, multi_acks),
     sizeof(companion_cmd_set_other_params_args_t)},
    {COMPANION_CMD_SEND_TELEMETRY_REQ, sizeof(companion_cmd_send_telemetry_req_args_t), sizeof(companion_cmd_send_telemetry_req_args_t)},
    {COMPANION_CMD_GET_CUSTOM_VARS, 0, 0},
    {COMPANION_CMD_SET_CUSTOM_VAR, 0, sizeof(companion_cmd_set_custom_var_args_t)},
    {COMPANION_CMD_GET_ADVERT_PATH, sizeof(companion_cmd_get_advert_path_args_t), sizeof(companion_cmd_get_advert_path_args_t)},
    {COMPANION_CMD_GET_TUNING_PARAMS, 0, 0},
    {COMPANION_CMD_SEND_BINARY_REQ, FIELD_SIZE(companion_cmd_send_binary_req_args_t, pub_key) + 1, sizeof(companion_cmd_send_binary_req_args_t)},
    {COMPANION_CMD_FACTORY_RESET, sizeof(companion_cmd_factory_reset_args_t), sizeof(companion_cmd_factory_reset_args_t)},
    {COMPANION_CMD_SEND_PATH_DISCOVERY_REQ, sizeof(companion_cmd_send_path_discovery_req_args_t), sizeof(companion_cmd_send_path_discovery_req_args_t)},
    {COMPANION_CMD_SET_FLOOD_SCOPE, FIELD_SIZE(companion_cmd_flood_scope_args_t, reserved), sizeof(companion_cmd_flood_scope_args_t)},
    {COMPANION_CMD_SEND_CONTROL_DATA, 1, sizeof(companion_cmd_send_control_data_args_t)},
    {COMPANION_CMD_GET_STATS, 0, 0},
};

mc_companion_command_parser_error_t mc_companion_parse_command(uint8_t* data, uint16_t data_length, companion_command_packet_t* out_packet) {
    if (data_length < 1) {
        return COMPANION_COMMAND_PARSER_ERROR_INVALID_COMMAND;  // No command byte
    }
    memset(out_packet, 0, sizeof(companion_command_packet_t));
    out_packet->command  = (companion_command_t)data[0];
    data                 = &data[1];
    data_length         -= 1;
    for (size_t i = 0; i < sizeof(companion_command_definitions) / sizeof(companion_command_definition_t); i++) {
        if (companion_command_definitions[i].command == out_packet->command) {
            size_t min_length = companion_command_definitions[i].min_argument_length;
            size_t max_length = companion_command_definitions[i].max_argument_length;
            if (data_length < min_length || data_length > max_length) {
                printf("Data length: %u, expected min %zu, max %zu\r\n", data_length, min_length, max_length);
                return COMPANION_COMMAND_PARSER_ERROR_INVALID_ARGUMENTS;  // Invalid argument length
            }
            memcpy(out_packet->args, data, data_length);
            return COMPANION_COMMAND_PARSER_ERROR_NONE;
        }
    }
    return COMPANION_COMMAND_PARSER_ERROR_INVALID_COMMAND;  // Command not found
}