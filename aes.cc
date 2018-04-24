#include <stdint.h>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "aes.h"
using namespace std;

const uint8_t S_TABLE[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B,
    0xFE, 0xD7, 0xAB, 0x76, 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0, 0xB7, 0xFD, 0x93, 0x26,
    0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2,
    0xEB, 0x27, 0xB2, 0x75, 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84, 0x53, 0xD1, 0x00, 0xED,
    0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F,
    0x50, 0x3C, 0x9F, 0xA8, 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2, 0xCD, 0x0C, 0x13, 0xEC,
    0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14,
    0xDE, 0x5E, 0x0B, 0xDB, 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, 0xE7, 0xC8, 0x37, 0x6D,
    0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F,
    0x4B, 0xBD, 0x8B, 0x8A, 0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, 0xE1, 0xF8, 0x98, 0x11,
    0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F,
    0xB0, 0x54, 0xBB, 0x16};

const uint8_t INV_S_TABLE[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E,
    0x81, 0xF3, 0xD7, 0xFB, 0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB, 0x54, 0x7B, 0x94, 0x32,
    0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49,
    0x6D, 0x8B, 0xD1, 0x25, 0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92, 0x6C, 0x70, 0x48, 0x50,
    0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05,
    0xB8, 0xB3, 0x45, 0x06, 0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
    0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B, 0x3A, 0x91, 0x11, 0x41,
    0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8,
    0x1C, 0x75, 0xDF, 0x6E, 0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
    0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B, 0xFC, 0x56, 0x3E, 0x4B,
    0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59,
    0x27, 0x80, 0xEC, 0x5F, 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
    0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF, 0xA0, 0xE0, 0x3B, 0x4D,
    0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63,
    0x55, 0x21, 0x0C, 0x7D};

const uint8_t ROUND_CONSTANT[256] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c,
    0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a,
    0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6,
    0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72,
    0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10,
    0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e,
    0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5,
    0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02,
    0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
    0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d,
    0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb,
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c,
    0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a,
    0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d};

const uint8_t AES::mult_2[256] = {
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16,
    0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e,
    0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46,
    0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
    0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76,
    0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e,
    0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa4, 0xa6,
    0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
    0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6,
    0xd8, 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee,
    0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe, 0x1b, 0x19, 0x1f, 0x1d,
    0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,
    0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d,
    0x23, 0x21, 0x27, 0x25, 0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55,
    0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45, 0x7b, 0x79, 0x7f, 0x7d,
    0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65,
    0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d,
    0x83, 0x81, 0x87, 0x85, 0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5,
    0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5, 0xdb, 0xd9, 0xdf, 0xdd,
    0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5,
    0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed,
    0xe3, 0xe1, 0xe7, 0xe5};

const uint8_t AES::mult_3[256] = {
    0x00, 0x03, 0x06, 0x05, 0x0c, 0x0f, 0x0a, 0x09, 0x18, 0x1b, 0x1e, 0x1d,
    0x14, 0x17, 0x12, 0x11, 0x30, 0x33, 0x36, 0x35, 0x3c, 0x3f, 0x3a, 0x39,
    0x28, 0x2b, 0x2e, 0x2d, 0x24, 0x27, 0x22, 0x21, 0x60, 0x63, 0x66, 0x65,
    0x6c, 0x6f, 0x6a, 0x69, 0x78, 0x7b, 0x7e, 0x7d, 0x74, 0x77, 0x72, 0x71,
    0x50, 0x53, 0x56, 0x55, 0x5c, 0x5f, 0x5a, 0x59, 0x48, 0x4b, 0x4e, 0x4d,
    0x44, 0x47, 0x42, 0x41, 0xc0, 0xc3, 0xc6, 0xc5, 0xcc, 0xcf, 0xca, 0xc9,
    0xd8, 0xdb, 0xde, 0xdd, 0xd4, 0xd7, 0xd2, 0xd1, 0xf0, 0xf3, 0xf6, 0xf5,
    0xfc, 0xff, 0xfa, 0xf9, 0xe8, 0xeb, 0xee, 0xed, 0xe4, 0xe7, 0xe2, 0xe1,
    0xa0, 0xa3, 0xa6, 0xa5, 0xac, 0xaf, 0xaa, 0xa9, 0xb8, 0xbb, 0xbe, 0xbd,
    0xb4, 0xb7, 0xb2, 0xb1, 0x90, 0x93, 0x96, 0x95, 0x9c, 0x9f, 0x9a, 0x99,
    0x88, 0x8b, 0x8e, 0x8d, 0x84, 0x87, 0x82, 0x81, 0x9b, 0x98, 0x9d, 0x9e,
    0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8f, 0x8c, 0x89, 0x8a,
    0xab, 0xa8, 0xad, 0xae, 0xa7, 0xa4, 0xa1, 0xa2, 0xb3, 0xb0, 0xb5, 0xb6,
    0xbf, 0xbc, 0xb9, 0xba, 0xfb, 0xf8, 0xfd, 0xfe, 0xf7, 0xf4, 0xf1, 0xf2,
    0xe3, 0xe0, 0xe5, 0xe6, 0xef, 0xec, 0xe9, 0xea, 0xcb, 0xc8, 0xcd, 0xce,
    0xc7, 0xc4, 0xc1, 0xc2, 0xd3, 0xd0, 0xd5, 0xd6, 0xdf, 0xdc, 0xd9, 0xda,
    0x5b, 0x58, 0x5d, 0x5e, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46,
    0x4f, 0x4c, 0x49, 0x4a, 0x6b, 0x68, 0x6d, 0x6e, 0x67, 0x64, 0x61, 0x62,
    0x73, 0x70, 0x75, 0x76, 0x7f, 0x7c, 0x79, 0x7a, 0x3b, 0x38, 0x3d, 0x3e,
    0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2f, 0x2c, 0x29, 0x2a,
    0x0b, 0x08, 0x0d, 0x0e, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16,
    0x1f, 0x1c, 0x19, 0x1a};

const uint8_t AES::mult_9[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53,
    0x6c, 0x65, 0x7e, 0x77, 0x90, 0x99, 0x82, 0x8b, 0xb4, 0xbd, 0xa6, 0xaf,
    0xd8, 0xd1, 0xca, 0xc3, 0xfc, 0xf5, 0xee, 0xe7, 0x3b, 0x32, 0x29, 0x20,
    0x1f, 0x16, 0x0d, 0x04, 0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0xab, 0xa2, 0xb9, 0xb0, 0x8f, 0x86, 0x9d, 0x94, 0xe3, 0xea, 0xf1, 0xf8,
    0xc7, 0xce, 0xd5, 0xdc, 0x76, 0x7f, 0x64, 0x6d, 0x52, 0x5b, 0x40, 0x49,
    0x3e, 0x37, 0x2c, 0x25, 0x1a, 0x13, 0x08, 0x01, 0xe6, 0xef, 0xf4, 0xfd,
    0xc2, 0xcb, 0xd0, 0xd9, 0xae, 0xa7, 0xbc, 0xb5, 0x8a, 0x83, 0x98, 0x91,
    0x4d, 0x44, 0x5f, 0x56, 0x69, 0x60, 0x7b, 0x72, 0x05, 0x0c, 0x17, 0x1e,
    0x21, 0x28, 0x33, 0x3a, 0xdd, 0xd4, 0xcf, 0xc6, 0xf9, 0xf0, 0xeb, 0xe2,
    0x95, 0x9c, 0x87, 0x8e, 0xb1, 0xb8, 0xa3, 0xaa, 0xec, 0xe5, 0xfe, 0xf7,
    0xc8, 0xc1, 0xda, 0xd3, 0xa4, 0xad, 0xb6, 0xbf, 0x80, 0x89, 0x92, 0x9b,
    0x7c, 0x75, 0x6e, 0x67, 0x58, 0x51, 0x4a, 0x43, 0x34, 0x3d, 0x26, 0x2f,
    0x10, 0x19, 0x02, 0x0b, 0xd7, 0xde, 0xc5, 0xcc, 0xf3, 0xfa, 0xe1, 0xe8,
    0x9f, 0x96, 0x8d, 0x84, 0xbb, 0xb2, 0xa9, 0xa0, 0x47, 0x4e, 0x55, 0x5c,
    0x63, 0x6a, 0x71, 0x78, 0x0f, 0x06, 0x1d, 0x14, 0x2b, 0x22, 0x39, 0x30,
    0x9a, 0x93, 0x88, 0x81, 0xbe, 0xb7, 0xac, 0xa5, 0xd2, 0xdb, 0xc0, 0xc9,
    0xf6, 0xff, 0xe4, 0xed, 0x0a, 0x03, 0x18, 0x11, 0x2e, 0x27, 0x3c, 0x35,
    0x42, 0x4b, 0x50, 0x59, 0x66, 0x6f, 0x74, 0x7d, 0xa1, 0xa8, 0xb3, 0xba,
    0x85, 0x8c, 0x97, 0x9e, 0xe9, 0xe0, 0xfb, 0xf2, 0xcd, 0xc4, 0xdf, 0xd6,
    0x31, 0x38, 0x23, 0x2a, 0x15, 0x1c, 0x07, 0x0e, 0x79, 0x70, 0x6b, 0x62,
    0x5d, 0x54, 0x4f, 0x46};

const uint8_t AES::mult_11[256] = {
    0x00, 0x0b, 0x16, 0x1d, 0x2c, 0x27, 0x3a, 0x31, 0x58, 0x53, 0x4e, 0x45,
    0x74, 0x7f, 0x62, 0x69, 0xb0, 0xbb, 0xa6, 0xad, 0x9c, 0x97, 0x8a, 0x81,
    0xe8, 0xe3, 0xfe, 0xf5, 0xc4, 0xcf, 0xd2, 0xd9, 0x7b, 0x70, 0x6d, 0x66,
    0x57, 0x5c, 0x41, 0x4a, 0x23, 0x28, 0x35, 0x3e, 0x0f, 0x04, 0x19, 0x12,
    0xcb, 0xc0, 0xdd, 0xd6, 0xe7, 0xec, 0xf1, 0xfa, 0x93, 0x98, 0x85, 0x8e,
    0xbf, 0xb4, 0xa9, 0xa2, 0xf6, 0xfd, 0xe0, 0xeb, 0xda, 0xd1, 0xcc, 0xc7,
    0xae, 0xa5, 0xb8, 0xb3, 0x82, 0x89, 0x94, 0x9f, 0x46, 0x4d, 0x50, 0x5b,
    0x6a, 0x61, 0x7c, 0x77, 0x1e, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2f,
    0x8d, 0x86, 0x9b, 0x90, 0xa1, 0xaa, 0xb7, 0xbc, 0xd5, 0xde, 0xc3, 0xc8,
    0xf9, 0xf2, 0xef, 0xe4, 0x3d, 0x36, 0x2b, 0x20, 0x11, 0x1a, 0x07, 0x0c,
    0x65, 0x6e, 0x73, 0x78, 0x49, 0x42, 0x5f, 0x54, 0xf7, 0xfc, 0xe1, 0xea,
    0xdb, 0xd0, 0xcd, 0xc6, 0xaf, 0xa4, 0xb9, 0xb2, 0x83, 0x88, 0x95, 0x9e,
    0x47, 0x4c, 0x51, 0x5a, 0x6b, 0x60, 0x7d, 0x76, 0x1f, 0x14, 0x09, 0x02,
    0x33, 0x38, 0x25, 0x2e, 0x8c, 0x87, 0x9a, 0x91, 0xa0, 0xab, 0xb6, 0xbd,
    0xd4, 0xdf, 0xc2, 0xc9, 0xf8, 0xf3, 0xee, 0xe5, 0x3c, 0x37, 0x2a, 0x21,
    0x10, 0x1b, 0x06, 0x0d, 0x64, 0x6f, 0x72, 0x79, 0x48, 0x43, 0x5e, 0x55,
    0x01, 0x0a, 0x17, 0x1c, 0x2d, 0x26, 0x3b, 0x30, 0x59, 0x52, 0x4f, 0x44,
    0x75, 0x7e, 0x63, 0x68, 0xb1, 0xba, 0xa7, 0xac, 0x9d, 0x96, 0x8b, 0x80,
    0xe9, 0xe2, 0xff, 0xf4, 0xc5, 0xce, 0xd3, 0xd8, 0x7a, 0x71, 0x6c, 0x67,
    0x56, 0x5d, 0x40, 0x4b, 0x22, 0x29, 0x34, 0x3f, 0x0e, 0x05, 0x18, 0x13,
    0xca, 0xc1, 0xdc, 0xd7, 0xe6, 0xed, 0xf0, 0xfb, 0x92, 0x99, 0x84, 0x8f,
    0xbe, 0xb5, 0xa8, 0xa3};

const uint8_t AES::mult_13[256] = {
    0x00, 0x0d, 0x1a, 0x17, 0x34, 0x39, 0x2e, 0x23, 0x68, 0x65, 0x72, 0x7f,
    0x5c, 0x51, 0x46, 0x4b, 0xd0, 0xdd, 0xca, 0xc7, 0xe4, 0xe9, 0xfe, 0xf3,
    0xb8, 0xb5, 0xa2, 0xaf, 0x8c, 0x81, 0x96, 0x9b, 0xbb, 0xb6, 0xa1, 0xac,
    0x8f, 0x82, 0x95, 0x98, 0xd3, 0xde, 0xc9, 0xc4, 0xe7, 0xea, 0xfd, 0xf0,
    0x6b, 0x66, 0x71, 0x7c, 0x5f, 0x52, 0x45, 0x48, 0x03, 0x0e, 0x19, 0x14,
    0x37, 0x3a, 0x2d, 0x20, 0x6d, 0x60, 0x77, 0x7a, 0x59, 0x54, 0x43, 0x4e,
    0x05, 0x08, 0x1f, 0x12, 0x31, 0x3c, 0x2b, 0x26, 0xbd, 0xb0, 0xa7, 0xaa,
    0x89, 0x84, 0x93, 0x9e, 0xd5, 0xd8, 0xcf, 0xc2, 0xe1, 0xec, 0xfb, 0xf6,
    0xd6, 0xdb, 0xcc, 0xc1, 0xe2, 0xef, 0xf8, 0xf5, 0xbe, 0xb3, 0xa4, 0xa9,
    0x8a, 0x87, 0x90, 0x9d, 0x06, 0x0b, 0x1c, 0x11, 0x32, 0x3f, 0x28, 0x25,
    0x6e, 0x63, 0x74, 0x79, 0x5a, 0x57, 0x40, 0x4d, 0xda, 0xd7, 0xc0, 0xcd,
    0xee, 0xe3, 0xf4, 0xf9, 0xb2, 0xbf, 0xa8, 0xa5, 0x86, 0x8b, 0x9c, 0x91,
    0x0a, 0x07, 0x10, 0x1d, 0x3e, 0x33, 0x24, 0x29, 0x62, 0x6f, 0x78, 0x75,
    0x56, 0x5b, 0x4c, 0x41, 0x61, 0x6c, 0x7b, 0x76, 0x55, 0x58, 0x4f, 0x42,
    0x09, 0x04, 0x13, 0x1e, 0x3d, 0x30, 0x27, 0x2a, 0xb1, 0xbc, 0xab, 0xa6,
    0x85, 0x88, 0x9f, 0x92, 0xd9, 0xd4, 0xc3, 0xce, 0xed, 0xe0, 0xf7, 0xfa,
    0xb7, 0xba, 0xad, 0xa0, 0x83, 0x8e, 0x99, 0x94, 0xdf, 0xd2, 0xc5, 0xc8,
    0xeb, 0xe6, 0xf1, 0xfc, 0x67, 0x6a, 0x7d, 0x70, 0x53, 0x5e, 0x49, 0x44,
    0x0f, 0x02, 0x15, 0x18, 0x3b, 0x36, 0x21, 0x2c, 0x0c, 0x01, 0x16, 0x1b,
    0x38, 0x35, 0x22, 0x2f, 0x64, 0x69, 0x7e, 0x73, 0x50, 0x5d, 0x4a, 0x47,
    0xdc, 0xd1, 0xc6, 0xcb, 0xe8, 0xe5, 0xf2, 0xff, 0xb4, 0xb9, 0xae, 0xa3,
    0x80, 0x8d, 0x9a, 0x97};

const uint8_t AES::mult_14[256] = {
    0x00, 0x0e, 0x1c, 0x12, 0x38, 0x36, 0x24, 0x2a, 0x70, 0x7e, 0x6c, 0x62,
    0x48, 0x46, 0x54, 0x5a, 0xe0, 0xee, 0xfc, 0xf2, 0xd8, 0xd6, 0xc4, 0xca,
    0x90, 0x9e, 0x8c, 0x82, 0xa8, 0xa6, 0xb4, 0xba, 0xdb, 0xd5, 0xc7, 0xc9,
    0xe3, 0xed, 0xff, 0xf1, 0xab, 0xa5, 0xb7, 0xb9, 0x93, 0x9d, 0x8f, 0x81,
    0x3b, 0x35, 0x27, 0x29, 0x03, 0x0d, 0x1f, 0x11, 0x4b, 0x45, 0x57, 0x59,
    0x73, 0x7d, 0x6f, 0x61, 0xad, 0xa3, 0xb1, 0xbf, 0x95, 0x9b, 0x89, 0x87,
    0xdd, 0xd3, 0xc1, 0xcf, 0xe5, 0xeb, 0xf9, 0xf7, 0x4d, 0x43, 0x51, 0x5f,
    0x75, 0x7b, 0x69, 0x67, 0x3d, 0x33, 0x21, 0x2f, 0x05, 0x0b, 0x19, 0x17,
    0x76, 0x78, 0x6a, 0x64, 0x4e, 0x40, 0x52, 0x5c, 0x06, 0x08, 0x1a, 0x14,
    0x3e, 0x30, 0x22, 0x2c, 0x96, 0x98, 0x8a, 0x84, 0xae, 0xa0, 0xb2, 0xbc,
    0xe6, 0xe8, 0xfa, 0xf4, 0xde, 0xd0, 0xc2, 0xcc, 0x41, 0x4f, 0x5d, 0x53,
    0x79, 0x77, 0x65, 0x6b, 0x31, 0x3f, 0x2d, 0x23, 0x09, 0x07, 0x15, 0x1b,
    0xa1, 0xaf, 0xbd, 0xb3, 0x99, 0x97, 0x85, 0x8b, 0xd1, 0xdf, 0xcd, 0xc3,
    0xe9, 0xe7, 0xf5, 0xfb, 0x9a, 0x94, 0x86, 0x88, 0xa2, 0xac, 0xbe, 0xb0,
    0xea, 0xe4, 0xf6, 0xf8, 0xd2, 0xdc, 0xce, 0xc0, 0x7a, 0x74, 0x66, 0x68,
    0x42, 0x4c, 0x5e, 0x50, 0x0a, 0x04, 0x16, 0x18, 0x32, 0x3c, 0x2e, 0x20,
    0xec, 0xe2, 0xf0, 0xfe, 0xd4, 0xda, 0xc8, 0xc6, 0x9c, 0x92, 0x80, 0x8e,
    0xa4, 0xaa, 0xb8, 0xb6, 0x0c, 0x02, 0x10, 0x1e, 0x34, 0x3a, 0x28, 0x26,
    0x7c, 0x72, 0x60, 0x6e, 0x44, 0x4a, 0x58, 0x56, 0x37, 0x39, 0x2b, 0x25,
    0x0f, 0x01, 0x13, 0x1d, 0x47, 0x49, 0x5b, 0x55, 0x7f, 0x71, 0x63, 0x6d,
    0xd7, 0xd9, 0xcb, 0xc5, 0xef, 0xe1, 0xf3, 0xfd, 0xa7, 0xa9, 0xbb, 0xb5,
    0x9f, 0x91, 0x83, 0x8d};

// Convert a string into a vector of bytes
vector<uint8_t> string_to_byte_vector(string input) {
  vector<uint8_t> byte_vector_;
  for (int i = 0; i < input.size(); i++) {
    uint8_t char_byte_ = (uint8_t)input[i];
    byte_vector_.emplace_back(char_byte_);
  }
  return byte_vector_;
}

// Generate a string of hex values from a byte vector
string hex_string(const vector<uint8_t> &bytes) {
  stringstream hex_bytes_;
  for (int i = 0; i < bytes.size(); i++) {
    hex_bytes_ << hex << (int)bytes[i];
  }
  return hex_bytes_.str();
}

// Big endian pass in
EasyWord::EasyWord(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  set_byte(0, byte0);
  set_byte(1, byte1);
  set_byte(2, byte2);
  set_byte(3, byte3);
}

// index 0 refers to the leftmost byte, index 3 is rightmost
uint8_t EasyWord::get_byte(int index) {
  assert(index >= 0 && index <= 3);
  // haha this is pretty much cheating at c++
  uint8_t *byte_ptr = reinterpret_cast<uint8_t *>(&word_);
  return byte_ptr[3 - index];
}

void EasyWord::set_byte(int index, uint8_t _val) {
  assert(index >= 0 && index <= 3);
  // haha this is pretty much cheating at c++
  uint8_t *byte_ptr = reinterpret_cast<uint8_t *>(&word_);
  byte_ptr[3 - index] = _val;
}

KeyMaster::KeyMaster(const vector<uint8_t> &_key)
    : key_Nb(4),
      key_Nk(_key.size() / key_Nb),
      key_Nr(_key.size() == 16 ? 10 : 14) {
  key_schedule_ = new EasyWord[(key_Nb * (key_Nr + 1)) + 4];
  actual_key_spot_ = key_schedule_;
  key_schedule_posterior_ = &key_schedule_[(key_Nb * (key_Nr + 1)) - 1];
  actual_key_end_ = key_schedule_posterior_;

  // detect key size and generate appropriate key schedule
  if (key_Nr == 10) {
    generate_128_bit_key_schedule(_key);
  } else {
    generate_256_bit_key_schedule(_key);
  }
}

void KeyMaster::reset() {
  key_schedule_ = actual_key_spot_;
  key_schedule_posterior_ = actual_key_end_;
}

void KeyMaster::print_key_schedule() {
  cout << hex << *key_schedule_posterior_ << endl;
  for (int i = (key_Nr + 1) * key_Nb - 1; i >= 0; i--) {
    cout << hex << key_schedule_[i];
  }
  cout << endl;
}

// get the next word in the key schedule
uint32_t KeyMaster::get_next_word() {
  uint32_t next_word = key_schedule_[0];
  key_schedule_ = &key_schedule_[1];
  return next_word;
}

// get the last word in the key schedule
uint32_t KeyMaster::get_last_word() {
  uint32_t last_word = *key_schedule_posterior_;
  key_schedule_posterior_ = key_schedule_posterior_ - 1;
  return last_word;
}

// create a key schedule using a 128 bit key
void KeyMaster::generate_128_bit_key_schedule(const vector<uint8_t> &_key) {
  // add inital four words to key schedule
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      key_schedule_[i].set_byte(j, _key[i * 4 + j]);
    }
  }
  // add remaining blocks of four words to key schedule
  for (int i = 1; i <= key_Nr; i++) {
    add_four_words(i * 4);
  }
}

// create a key schedule using a 256 bit key
void KeyMaster::generate_256_bit_key_schedule(const vector<uint8_t> &_key) {
  // add initial eight words to key schedule
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 4; j++) {
      key_schedule_[i].set_byte(j, _key[i * 4 + j]);
    }
  }
  // add remaining blocks of eight words to key schedule
  for (int i = 1; i <= key_Nr / 2; i++) {
    add_eight_words(i * 8);
  }
}

// adds four new words to the key_schedule using the previous four
void KeyMaster::add_four_words(int _ks_idx) {
  // run magic function to generate magic word
  assert(key_Nk > 0);
  uint32_t magic_word =
      schedule_core(key_schedule_[_ks_idx - 1], (int)_ks_idx / key_Nk);
  // add first value
  key_schedule_[_ks_idx] = key_schedule_[_ks_idx - 4] ^ magic_word;
  // add second value
  key_schedule_[_ks_idx + 1] =
      key_schedule_[_ks_idx] ^ key_schedule_[_ks_idx - 3];
  // add third value
  key_schedule_[_ks_idx + 2] =
      key_schedule_[_ks_idx + 1] ^ key_schedule_[_ks_idx - 2];
  // add fourth value
  key_schedule_[_ks_idx + 3] =
      key_schedule_[_ks_idx + 2] ^ key_schedule_[_ks_idx - 1];
}

// adds eight new words to the key_schedule using the previous eight
void KeyMaster::add_eight_words(int _ks_idx) {
  // run magic function to generate magic word
  uint32_t magic_word =
      schedule_core(key_schedule_[_ks_idx - 1], (int)_ks_idx / key_Nk);
  // add first value
  key_schedule_[_ks_idx] = key_schedule_[_ks_idx - 8] ^ magic_word;
  // add second value
  key_schedule_[_ks_idx + 1] =
      key_schedule_[_ks_idx] ^ key_schedule_[_ks_idx - 7];
  // add third value
  key_schedule_[_ks_idx + 2] =
      key_schedule_[_ks_idx + 1] ^ key_schedule_[_ks_idx - 6];
  // add fourth value
  key_schedule_[_ks_idx + 3] =
      key_schedule_[_ks_idx + 2] ^ key_schedule_[_ks_idx - 5];
  // perform s-box and add fifth value
  key_schedule_[_ks_idx + 4] =
      sub_word(key_schedule_[_ks_idx + 3]) ^ key_schedule_[_ks_idx - 4];
  // add sixth value
  key_schedule_[_ks_idx + 5] =
      key_schedule_[_ks_idx + 4] ^ key_schedule_[_ks_idx - 3];
  // add seventh value
  key_schedule_[_ks_idx + 6] =
      key_schedule_[_ks_idx + 5] ^ key_schedule_[_ks_idx - 2];
  // add eighth value
  key_schedule_[_ks_idx + 7] =
      key_schedule_[_ks_idx + 6] ^ key_schedule_[_ks_idx - 1];
}

// magic function to calculate the first word per round
uint32_t KeyMaster::schedule_core(EasyWord _word, int _round) {
  _word = rotate_word(_word);
  _word = sub_word(_word);
  // xor leftmost byte with precomputed ROUND_CONSTANT value
  uint32_t round_const = (ROUND_CONSTANT[_round] << 24);
  _word = _word ^ round_const;
  return _word;
}

// move leftmost byte to become the rightmost byte
uint32_t KeyMaster::rotate_word(EasyWord _word) {
  uint8_t first_byte = _word.get_byte(0);
  _word.set_byte(0, _word.get_byte(1));
  _word.set_byte(1, _word.get_byte(2));
  _word.set_byte(2, _word.get_byte(3));
  _word.set_byte(3, first_byte);
  return _word;
}

// perform simple s-box substitution
uint32_t KeyMaster::sub_word(EasyWord _word) {
  for (int i = 0; i < 4; i++) {
    _word.set_byte(i, S_TABLE[_word.get_byte(i)]);
  }
  return _word;
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.14-19, 5.1)
//   implementation (p.15,    5.1)
vector<uint8_t> AES::encrypt_this(vector<uint8_t> &_vectortext) {
  vector<uint8_t> _outputtext;

  // if _plaintext size is not evenly divisible by 16
  //                                  (128b block size / 8b ints)
  // pad end with 0s
  uint8_t to_pad = 16 - (_vectortext.size() % 16);
  for (uint8_t i = 0; i < to_pad; i++) {
    _vectortext.push_back(0);
  }
  // for each 128b block (16 * 8b)
  for (uint32_t i = 0; i < _vectortext.size(); i += 16) {
    master_.reset();
    // get (16 * 8b) block
    vector<uint8_t> block;
    for (uint32_t j = 0; j < 16; j++) {
      block.push_back(_vectortext[i + j]);
    }
    // put block in state
    input_to_state(block);

    add_round_key();

    for (int j = 0; j < master_.get_num_rounds() - 1; j++) {
      sub_bytes();
      shift_rows();
      mix_columns();
      add_round_key();
    }

    sub_bytes();
    shift_rows();
    add_round_key();

    vector<uint8_t> block_output = state_to_output();
    for (uint32_t j = 0; j < block_output.size(); j++) {
      _outputtext.emplace_back(block_output[j]);
    }
  }
  return _outputtext;
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.20-24, 5.3)
//   implementation (p.21,    5.3)
vector<uint8_t> AES::decrypt_this(vector<uint8_t> &_vectortext) {
  vector<uint8_t> _outputtext;

  // for each 128b block (16 * 8b)
  for (uint32_t i = 0; i < _vectortext.size(); i += 16) {
    // get (16 * 8b) block
    master_.reset();
    vector<uint8_t> block;
    for (uint8_t j = 0; j < 16; j++) {
      block.push_back(_vectortext[i + j]);
    }
    // put block in state
    input_to_state(block);

    add_round_key_reverse();

    for (int j = 0; j < master_.get_num_rounds() - 1; j++) {
      inv_shift_rows();
      inv_sub_bytes();
      add_round_key_reverse();
      inv_mix_columns();
    }

    inv_shift_rows();
    inv_sub_bytes();
    add_round_key_reverse();

    vector<uint8_t> block_output = state_to_output();
    for (uint8_t j = 0; j < block_output.size(); j++) {
      _outputtext.emplace_back(block_output[j]);
      // cout << block_output[j];
    }
  }
  return _outputtext;
}

void AES::input_to_state(const vector<uint8_t> &_input) {
  for (uint8_t r = 0; r < 4; r++) {
    for (uint8_t c = 0; c < 4; c++) {
      state_[r][c] = 0;
      state_[r][c] = _input[r + 4 * c];
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.18-19,   5.1.4)
//   explanation    (p.23,      5.3.4)
void AES::add_round_key() {
  for (int word = 0; word < 4; word++) {
    EasyWord next_word = master_.get_next_word();
    for (int byte = 0; byte < 4; byte++) {
      state_[byte][word] = state_[byte][word] ^ next_word.get_byte(byte);
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.18-19,   5.1.4)
//   explanation    (p.23,      5.3.4)
void AES::add_round_key_reverse() {
  for (int word = 0; word < 4; word++) {
    EasyWord last_word = master_.get_last_word();
    for (int byte = 0; byte < 4; byte++) {
      state_[byte][3 - word] =
          state_[byte][3 - word] ^ last_word.get_byte(byte);
    }
  }
}

vector<uint8_t> AES::multiply_column(function<uint8_t(uint8_t)> matrix[4][4],
                                     int col_index) {
  vector<uint8_t> new_col(4, 0);
  for (int j = 0; j < 4; j++) {
    // go through and multiply each val in column by matrix
    for (int k = 0; k < 4; k++) {
      new_col[j] ^= matrix[j][k](state_[k][col_index]);
    }
  }
  return new_col;
}

void AES::matrix_multiply(function<uint8_t(uint8_t)> matrix[4][4]) {
  vector<uint8_t> new_col(4);
  for (int i = 0; i < 4; i++) {
    // for every row value, we have to compute the matrix mult
    new_col = multiply_column(matrix, i);
    for (int k = 0; k < 4; k++) {
      state_[k][i] = new_col[k];
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.17,    5.1.2)
void AES::shift_rows() {
  uint8_t new_state[4][4] = {
      {state_[0][0], state_[0][1], state_[0][2], state_[0][3]},
      {state_[1][1], state_[1][2], state_[1][3], state_[1][0]},
      {state_[2][2], state_[2][3], state_[2][0], state_[2][1]},
      {state_[3][3], state_[3][0], state_[3][1], state_[3][2]}};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      state_[i][j] = new_state[i][j];
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.21-22,   5.3.1)
void AES::inv_shift_rows() {
  uint8_t new_state[4][4] = {
      {state_[0][0], state_[0][1], state_[0][2], state_[0][3]},
      {state_[1][3], state_[1][0], state_[1][1], state_[1][2]},
      {state_[2][2], state_[2][3], state_[2][0], state_[2][1]},
      {state_[3][1], state_[3][2], state_[3][3], state_[3][0]}};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      state_[i][j] = new_state[i][j];
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.17-18,   5.1.3)
void AES::mix_columns() {
  function<uint8_t(uint8_t)> multiply_1 = [&](uint8_t x) { return x; };
  function<uint8_t(uint8_t)> multiply_2 = [&](uint8_t x) {
    return mult_2[(int)x];
  };
  function<uint8_t(uint8_t)> multiply_3 = [&](uint8_t x) {
    return mult_3[(int)x];
  };
  // given in whitepaper
  function<uint8_t(uint8_t)> mult_by_mat[4][4] = {
      {multiply_2, multiply_3, multiply_1, multiply_1},
      {multiply_1, multiply_2, multiply_3, multiply_1},
      {multiply_1, multiply_1, multiply_2, multiply_3},
      {multiply_3, multiply_1, multiply_1, multiply_2}};
  matrix_multiply(mult_by_mat);
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.23,   5.3.3)
void AES::inv_mix_columns() {
  function<uint8_t(uint8_t)> multiply_9 = [&](uint8_t x) { return mult_9[x]; };
  function<uint8_t(uint8_t)> multiply_14 = [&](uint8_t x) {
    return mult_14[x];
  };
  function<uint8_t(uint8_t)> multiply_11 = [&](uint8_t x) {
    return mult_11[x];
  };
  function<uint8_t(uint8_t)> multiply_13 = [&](uint8_t x) {
    return mult_13[x];
  };
  function<uint8_t(uint8_t)> mult_by_mat[4][4] = {
      {multiply_14, multiply_11, multiply_13, multiply_9},
      {multiply_9, multiply_14, multiply_11, multiply_13},
      {multiply_13, multiply_9, multiply_14, multiply_11},
      {multiply_11, multiply_13, multiply_9, multiply_14}};
  matrix_multiply(mult_by_mat);
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.15-16,   5.1.1)
void AES::sub_bytes() {
  // use the value within the state to index into S-box
  // then, assign current cell in state to value in S-box
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int index = static_cast<int>(state_[i][j]);
      state_[i][j] = S_TABLE[index];
    }
  }
}

// see https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
//   explanation    (p.22,   5.3.2)
void AES::inv_sub_bytes() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int index = static_cast<int>(state_[i][j]);
      state_[i][j] = INV_S_TABLE[index];
    }
  }
}

vector<uint8_t> AES::state_to_output() {
  vector<uint8_t> output(16);
  for (uint8_t r = 0; r < 4; r++) {
    for (uint8_t c = 0; c < 4; c++) {
      output[r + 4 * c] = state_[r][c];
    }
  }

  return output;
}
