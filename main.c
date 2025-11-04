#include <stdio.h>
#include "meshcore/packet.h"
#include "meshcore/payload/advert.h"

unsigned char packet_bin[] = {0x11, 0x00, 0x7e, 0x76, 0x62, 0x67, 0x6f, 0x7f, 0x08, 0x50, 0xa8, 0xa3, 0x55, 0xba, 0xaf,
                              0xbf, 0xc1, 0xeb, 0x7b, 0x41, 0x74, 0xc3, 0x40, 0x44, 0x2d, 0x7d, 0x71, 0x61, 0xc9, 0x47,
                              0x4a, 0x2c, 0x94, 0x00, 0x6c, 0xe7, 0xcf, 0x68, 0x2e, 0x58, 0x40, 0x8d, 0xd8, 0xfc, 0xc5,
                              0x19, 0x06, 0xec, 0xa9, 0x8e, 0xbf, 0x94, 0xa0, 0x37, 0x88, 0x6b, 0xda, 0xde, 0x7e, 0xcd,
                              0x09, 0xfd, 0x92, 0xb8, 0x39, 0x49, 0x1d, 0xf3, 0x80, 0x9c, 0x94, 0x54, 0xf5, 0x28, 0x6d,
                              0x1d, 0x33, 0x70, 0xac, 0x31, 0xa3, 0x45, 0x93, 0xd5, 0x69, 0xe9, 0xa0, 0x42, 0xa3, 0xb4,
                              0x1f, 0xd3, 0x31, 0xdf, 0xfb, 0x7e, 0x18, 0x59, 0x9c, 0xe1, 0xe6, 0x09, 0x92, 0xa0, 0x76,
                              0xd5, 0x02, 0x38, 0xc5, 0xb8, 0xf8, 0x57, 0x57, 0x37, 0x53, 0x54, 0x52, 0x2f, 0x50, 0x75,
                              0x67, 0x65, 0x74, 0x4d, 0x65, 0x73, 0x68, 0x20, 0x43, 0x6f, 0x75, 0x67, 0x61, 0x72};
unsigned int  packet_bin_len = 134;

uint8_t encoded_packet[256] = {0};

const char* type_to_string(meshcore_payload_type_t type) {
    switch (type) {
        case MESHCORE_PAYLOAD_TYPE_REQ:
            return "Request";
        case MESHCORE_PAYLOAD_TYPE_RESPONSE:
            return "Response";
        case MESHCORE_PAYLOAD_TYPE_TXT_MSG:
            return "Plain text message";
        case MESHCORE_PAYLOAD_TYPE_ACK:
            return "Acknowledgement";
        case MESHCORE_PAYLOAD_TYPE_ADVERT:
            return "Node advertisement";
        case MESHCORE_PAYLOAD_TYPE_GRP_TXT:
            return "Group text message (unverified)";
        case MESHCORE_PAYLOAD_TYPE_GRP_DATA:
            return "Group data message (unverified)";
        case MESHCORE_PAYLOAD_TYPE_ANON_REQ:
            return "Anonymous request";
        case MESHCORE_PAYLOAD_TYPE_PATH:
            return "Returned path";
        case MESHCORE_PAYLOAD_TYPE_TRACE:
            return "Trace";
        case MESHCORE_PAYLOAD_TYPE_MULTIPART:
            return "Multipart";
        case MESHCORE_PAYLOAD_TYPE_RAW_CUSTOM:
            return "Custom raw";
        default:
            return "UNKNOWN";
    }
}

const char* route_to_string(meshcore_route_type_t route) {
    switch (route) {
        case MESHCORE_ROUTE_TYPE_TRANSPORT_FLOOD:
            return "Transport flood";
        case MESHCORE_ROUTE_TYPE_FLOOD:
            return "Flood";
        case MESHCORE_ROUTE_TYPE_DIRECT:
            return "Direct";
        case MESHCORE_ROUTE_TYPE_TRANSPORT_DIRECT:
            return "Transport direct";
        default:
            return "Unknown";
    }
}

const char* role_to_string(meshcore_device_role_t role) {
    switch (role) {
        case MESHCORE_DEVICE_ROLE_CHAT_NODE:
            return "Chat Node";
        case MESHCORE_DEVICE_ROLE_REPEATER:
            return "Repeater";
        case MESHCORE_DEVICE_ROLE_ROOM_SERVER:
            return "Room Server";
        case MESHCORE_DEVICE_ROLE_SENSOR:
            return "Sensor";
        default:
            return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    printf("Input packet binary data [%zu]:\n", packet_bin_len);
    for (unsigned int i = 0; i < packet_bin_len; i++) {
        printf("%02X", packet_bin[i]);
    }
    printf("\n");

    meshcore_message_t message;
    if (meshcore_deserialize(packet_bin, packet_bin_len, &message) >= 0) {
        printf("Decoded message:\n");
        printf("Type: %s [%d]\n", type_to_string(message.type), message.type);
        printf("Route: %s [%d]\n", route_to_string(message.route), message.route);
        printf("Version: %d\n", message.version);
        printf("Path Length: %d\n", message.path_length);
        if (message.path_length > 0) {
            printf("Path: ");
            for (unsigned int i = 0; i < message.path_length; i++) {
                printf("0x%02x, ", message.path[i]);
            }
            printf("\n");
        }
        printf("Payload Length: %d\n", message.payload_length);
        if (message.payload_length > 0) {
            printf("Payload [%d]: ", message.payload_length);
            for (unsigned int i = 0; i < message.payload_length; i++) {
                printf("%02X", message.payload[i]);
            }
            printf("\n");
        }

        if (message.type == MESHCORE_PAYLOAD_TYPE_ADVERT) {
            meshcore_advert_t advert;
            if (meshcore_advert_deserialize(message.payload, message.payload_length, &advert) >= 0) {
                printf("Decoded node advertisement:\n");
                printf("Public Key: ");
                for (unsigned int i = 0; i < MESHCORE_PUB_KEY_SIZE; i++) {
                    printf("%02X", advert.pub_key[i]);
                }
                printf("\n");
                printf("Timestamp: %u\n", advert.timestamp);
                printf("Signature: ");
                for (unsigned int i = 0; i < MESHCORE_SIGNATURE_SIZE; i++) {
                    printf("%02X", advert.signature[i]);
                }
                printf("\n");
                printf("Role: %s\n", role_to_string(advert.role));
                if (advert.position_valid) {
                    printf("Position: lat=%d, lon=%d\n", advert.position_lat, advert.position_lon);
                } else {
                    printf("Position: (not available)\n");
                }
                if (advert.extra1_valid) {
                    printf("Extra1: %u\n", advert.extra1);
                } else {
                    printf("Extra1: (not available)\n");
                }
                if (advert.extra2_valid) {
                    printf("Extra2: %u\n", advert.extra2);
                } else {
                    printf("Extra2: (not available)\n");
                }
                if (advert.name_valid) {
                    printf("Name: %s\n", advert.name);
                } else {
                    printf("Name: (not available)\n");
                }

                if (meshcore_advert_serialize(&advert, message.payload, &message.payload_length) < 0) {
                    printf("Failed to serialize node advertisement payload.\n");
                    return -1;
                }

            } else {
                printf("Failed to decode node advertisement payload.\n");
                return -1;
            }
        }
    } else {
        printf("Failed to decode message.\n");
        return -1;
    }

    uint8_t encoded_packet_len = 0;
    if (meshcore_serialize(&message, encoded_packet, &encoded_packet_len) >= 0) {
        printf("Serialized packet binary data [%zu]: ", encoded_packet_len);
        for (unsigned int i = 0; i < encoded_packet_len; i++) {
            printf("%02X", encoded_packet[i]);
        }
        printf("\n");
    } else {
        printf("Failed to serialize message.\n");
        return -1;
    }
    return 0;
}