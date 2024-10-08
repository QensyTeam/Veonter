#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    1
#define ATA_IDENT_HEADS        3
#define ATA_IDENT_SECTORS      6
#define ATA_IDENT_SERIAL       10
#define ATA_IDENT_MODEL        27
#define ATA_IDENT_CAPABILITIES 49
#define ATA_IDENT_FIELDVALID   53
#define ATA_IDENT_MAX_LBA      60
#define ATA_IDENT_COMMANDSETS  82
#define ATA_IDENT_MAX_LBA_EXT  100


#define ATA_DMA_PRIMARY_CMD 0x00
#define ATA_DMA_PRIMARY_STATUS 0x02
#define ATA_DMA_PRIMARY_PRDT 0x04

#define ATA_DMA_SECONDARY_CMD 0x08
#define ATA_DMA_SECONDARY_STATUS 0x0A
#define ATA_DMA_SECONDARY_PRDT 0x0C

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

typedef enum {
    MASTER = 0,
    SLAVE = 1
} ata_bus_t;

typedef enum {
    PRIMARY = 0,
    SECONDARY = 1
} ata_channel_t;

#define      ATA_READ      0x00
#define      ATA_WRITE     0x13

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

#define DRIVE(bus, drive) ((bus) << 1 | (drive))
#define ATA_PORT(bus) ((bus) == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO)

typedef struct {
	uint8_t drive_id;
    unsigned long long capacity;

    bool is_packet;
    bool is_medium_inserted;
    bool is_lba48_supported;

    uint16_t block_size;

    bool is_dma;

	char model_name[64];
	char fwversion[64];
	char serial_number[64];
} ata_drive_t;

void ata_init();
