#pragma once

#include "stdint.h"

uint8_t getCRC(const uint8_t * message, uint8_t length);

uint8_t crc7_be(uint8_t crc, const uint8_t *buffer, size_t len);