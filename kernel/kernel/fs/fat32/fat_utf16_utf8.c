#include <kernel/fs/fat32/fat_utf16_utf8.h>
#include <stddef.h>

void utf16_to_utf8(const unsigned short* utf16, int utf16_length, unsigned char* utf8) {
    int i = 0;
    int j = 0;

    while (i < utf16_length) {
        unsigned int codepoint;

        if (utf16[i] < 0xD800 || utf16[i] > 0xDFFF) {
            // Простой BMP символ (не суррогатная пара)
            codepoint = utf16[i];
            i++;
        } else {
            // Суррогатная пара
            unsigned short high = utf16[i];
            unsigned short low = utf16[i + 1];
            codepoint = 0x10000 + ((high - 0xD800) << 10) + (low - 0xDC00);
            i += 2;
        }

        // Преобразование кодовой точки в UTF-8
        if (codepoint < 0x80) {
            utf8[j++] = (unsigned char)codepoint;
        } else if (codepoint < 0x800) {
            utf8[j++] = 0xC0 | ((codepoint >> 6) & 0x1F);
            utf8[j++] = 0x80 | (codepoint & 0x3F);
        } else if (codepoint < 0x10000) {
            utf8[j++] = 0xE0 | ((codepoint >> 12) & 0x0F);
            utf8[j++] = 0x80 | ((codepoint >> 6) & 0x3F);
            utf8[j++] = 0x80 | (codepoint & 0x3F);
        } else {
            utf8[j++] = 0xF0 | ((codepoint >> 18) & 0x07);
            utf8[j++] = 0x80 | ((codepoint >> 12) & 0x3F);
            utf8[j++] = 0x80 | ((codepoint >> 6) & 0x3F);
            utf8[j++] = 0x80 | (codepoint & 0x3F);
        }
    }
}

void utf8_to_utf16(const char* utf8_str, unsigned short* utf16_str) {
    if (utf8_str == NULL || utf16_str == NULL) {
        // Handle invalid input parameters
        return;
    }

    unsigned short* utf16_ptr = utf16_str;

    while (*utf8_str != '\0') {
        unsigned int code_point = 0;
        int bytes = 0;

        if ((*utf8_str & 0x80) == 0) {
            // Single-byte character
            code_point = *utf8_str;
            bytes = 1;
        } else if ((*utf8_str & 0xE0) == 0xC0) {
            // Two-byte character
            code_point = *utf8_str & 0x1F;
            bytes = 2;
        } else if ((*utf8_str & 0xF0) == 0xE0) {
            // Three-byte character
            code_point = *utf8_str & 0x0F;
            bytes = 3;
        } else if ((*utf8_str & 0xF8) == 0xF0) {
            // Four-byte character
            code_point = *utf8_str & 0x07;
            bytes = 4;
        } else {
            // Invalid UTF-8 sequence
            *utf16_ptr = 0; // Null-terminate the output string
            return;
        }

        for (int i = 1; i < bytes; ++i) {
            if ((utf8_str[i] & 0xC0) != 0x80) {
                // Invalid continuation byte
                *utf16_ptr = 0; // Null-terminate the output string
                return;
            }
            code_point = (code_point << 6) | (utf8_str[i] & 0x3F);
        }

        utf8_str += bytes;

        if (code_point <= 0xFFFF) {
            *utf16_ptr++ = (unsigned short)code_point;
        } else {
            code_point -= 0x10000;
            *utf16_ptr++ = 0xD800 | ((code_point >> 10) & 0x3FF);
            *utf16_ptr++ = 0xDC00 | (code_point & 0x3FF);
        }
    }

    // Null-terminate the output string
    *utf16_ptr = 0;
}
