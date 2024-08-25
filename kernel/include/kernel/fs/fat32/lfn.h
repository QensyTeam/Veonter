#pragma once

#include <stdint.h>

typedef struct {
	uint8_t attr_number;
	uint16_t first_name_chunk[5];
	uint8_t attribute;
	uint8_t reserved;
	uint8_t checksum;
	uint16_t second_name_chunk[6];
	uint16_t reserved2;
	uint16_t third_name_chunk[2];
} __attribute__((packed)) LFN_t;

void LFN2SFN(const char *lfn, char *sfn);
uint8_t lfn_checksum(const char sfn[11]); 
