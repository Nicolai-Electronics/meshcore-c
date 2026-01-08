// SPDX-FileCopyrightText: 2026 Nicolai Electronics
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mc_companion.h"

void mc_companion_read_serial_frame(bool is_server, uint8_t* framed_data, size_t framed_data_length, mc_companion_receive_callback callback);
void mc_companion_write_serial_frame(bool is_server, companion_packet_t* packet, uint16_t args_length, size_t output_buffer_size, uint8_t* out_framed_data,
                                     size_t* out_framed_data_length);