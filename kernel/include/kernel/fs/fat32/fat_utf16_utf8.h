#pragma once

void utf16_to_utf8(const unsigned short* utf16, int utf16_length, unsigned char* utf8);
void utf8_to_utf16(const char* utf8_str, unsigned short* utf16_str);