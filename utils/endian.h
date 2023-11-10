#pragma once

#define ENDIAN_SWAP_32(v)                                                      \
    ((v >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8) | (v << 24))
