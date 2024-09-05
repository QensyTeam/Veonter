#include <kernel/drv/ata_pio.h>
#include <kernel/kernel.h>
#include <kernel/disk_manager.h>
#include <bits.h>
#include <kernel/drv/serial_port.h>
#include <stdint.h>

//volatile bool ide_primary_fired = false;
//volatile bool ide_secondary_fired = false;

void ata_ide_select_drive(uint8_t bus, bool slave) {
	if(bus == PRIMARY) {
		outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, (0xA0 | ((uint8_t)slave << 4)));
    } else {
		outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, (0xA0 | ((uint8_t)slave << 4)));
    }
}

__attribute__((always_inline))
inline void ata_ide_400ns_delay(uint16_t io) {
	inb(io + ATA_REG_ALTSTATUS);
	inb(io + ATA_REG_ALTSTATUS);
	inb(io + ATA_REG_ALTSTATUS);
	inb(io + ATA_REG_ALTSTATUS);
}

void ata_set_params(uint8_t drive, uint16_t* io, uint8_t* real_drive) {
	uint8_t _io = drive >> 1; // PRIMARY / SECONDARY
	uint8_t _drv = drive & 1; // MASTER / SLAVE

	if(_io == PRIMARY) {
		*io = ATA_PRIMARY_IO;
    } else if(_io == SECONDARY) {
		*io = ATA_SECONDARY_IO;
    }

	*real_drive = _drv;
}

void ata_ide_soft_reset(size_t io) {
	outb(io + ATA_REG_CONTROL, 0x04);
	ata_ide_400ns_delay(io);
	outb(io + ATA_REG_CONTROL, 0);
}

// Returns true if okay, returns false on error.
bool ide_poll(uint16_t io) {
	while(1) {
		uint8_t status = inb(io + ATA_REG_STATUS);
        /*qemu_log("Status: %x (BSY: %d; DRQ: %d; ERR: %d; DF: %d)", status,
                status & ATA_SR_BSY,
                status & ATA_SR_DRQ,
                status & ATA_SR_ERR,
                status & ATA_SR_DF);*/

		if((!(status & ATA_SR_BSY)) && (status & ATA_SR_DRQ)) {
			return true;
        } else if ((status & ATA_SR_ERR) || (status & ATA_SR_DF)) {
            return false;
        }
	}
}

bool ide_poll_drq(uint16_t io) {
	while(1) {
		uint8_t status = inb(io + ATA_REG_STATUS);
        if(status & ATA_SR_DRQ) {
			return true;
        } else if ((status & ATA_SR_ERR) || (status & ATA_SR_DF)) {
            return false;
        }
	}
}

bool ide_poll_bsy(uint16_t io) {
	while(1) {
		uint8_t status = inb(io + ATA_REG_STATUS);
        /*qemu_log("Status: %x (BSY: %d; DRQ: %d; ERR: %d; DF: %d)", status,
                status & ATA_SR_BSY,
                status & ATA_SR_DRQ,
                status & ATA_SR_ERR,
                status & ATA_SR_DF);*/

		if(!(status & ATA_SR_BSY)) {
			return true;
        } else if ((status & ATA_SR_ERR) || (status & ATA_SR_DF)) {
            return false;
        }
	}
}

uint8_t ata_pio_read_sector_24lba(disk_t disk, uint8_t *buf, uint64_t lba) {
    ata_drive_t* drive = disk.priv_data;
    // Only 28-bit LBA supported!
    lba &= 0x00FFFFFF;

    uint16_t io = 0;
    uint8_t rdv = 0;

    ata_set_params(drive->drive_id, &io, &rdv);

    // For 24-bit LBA
    uint8_t cmd = (rdv == MASTER ? 0xE0 : 0xF0);
    uint8_t slavebit = (rdv == MASTER ? 0x00 : 0x01);

    outb(io + ATA_REG_HDDEVSEL, cmd | (slavebit << 4));
    //outb(io + 1, 0x00);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
    outb(io + ATA_REG_SECCOUNT0, 1);

    outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);

    ide_poll(io);

    uint16_t* buf16 = (uint16_t*)buf;

    for(int i = 0; i < 256; i++) {
        uint16_t data = inw(io);
        *(buf16 + i) = data;
    }

    ata_ide_400ns_delay(io);

    return 1;
}

uint8_t ata_pio_write_raw_sector_24lba(disk_t disk, const uint8_t *buf, uint64_t lba) {
    ata_drive_t* drive = disk.priv_data;
    // Only 24-bit LBA supported!
    lba &= 0x00FFFFFF;

    uint16_t io = 0;
    uint8_t rdv = 0;

    ata_set_params(drive->drive_id, &io, &rdv);

    // For 24-bit LBA
    uint8_t cmd = (rdv == MASTER ? 0xE0 : 0xF0);
    uint8_t slavebit = (rdv == MASTER ? 0x00 : 0x01);

    outb(io + ATA_REG_HDDEVSEL, cmd | (slavebit << 4));
    outb(io + 1, 0x00);
    outb(io + ATA_REG_SECCOUNT0, 1);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
    outb(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT);

    ide_poll_drq(io);

    for(int i = 0; i < 256; i++) {
        outw(io, *(uint16_t*)(buf + (i * 2)));
    }
    
    ata_ide_400ns_delay(io);

    outb(io + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);

    ide_poll_bsy(io);
    
    return 1;
}

uint8_t ata_pio_read_sector(disk_t disk, uint8_t *buf, uint64_t lba) {
    ata_drive_t* drive = disk.priv_data;
    // Only 28-bit LBA supported!
    //lba &= 0x00FFFFFF;

    uint16_t io = 0;
    uint8_t rdv = 0;

    ata_set_params(drive->drive_id, &io, &rdv);

    // For 24-bit LBA
    //uint8_t cmd = (rdv == MASTER ? 0xE0 : 0xF0);
    // For 48-bit LBA
    uint8_t cmd = (rdv == MASTER ? 0x40 : 0x50);
    uint8_t slavebit = (rdv == MASTER ? 0x00 : 0x01);

    outb(io + ATA_REG_HDDEVSEL, cmd | (slavebit << 4));
    //outb(io + 1, 0x00);
    outb(io + ATA_REG_SECCOUNT0, 0);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba) >> 24));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 32));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 48));
    outb(io + ATA_REG_SECCOUNT0, 1);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));

    outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);

    ide_poll(io);

    uint16_t* buf16 = (uint16_t*)buf;

    for(int i = 0; i < 256; i++) {
        uint16_t data = inw(io);
        *(buf16 + i) = data;
    }

    ata_ide_400ns_delay(io);

    return 1;
}

uint8_t ata_pio_write_raw_sector(disk_t disk, const uint8_t *buf, uint64_t lba) {
    ata_drive_t* drive = disk.priv_data;
    // Only 28-bit LBA supported!
    //lba &= 0x00FFFFFF;

    uint16_t io = 0;
    uint8_t rdv = 0;

    ata_set_params(drive->drive_id, &io, &rdv);

    // For 24-bit LBA
    //uint8_t cmd = (rdv == MASTER ? 0xE0 : 0xF0);
    // For 48-bit LBA
    uint8_t cmd = (rdv == MASTER ? 0x40 : 0x50);
    uint8_t slavebit = (rdv == MASTER ? 0x00 : 0x01);

    outb(io + ATA_REG_HDDEVSEL, cmd | (slavebit << 4));
    //outb(io + 1, 0x00);
    outb(io + ATA_REG_SECCOUNT0, 0);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba) >> 24));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 32));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 48));
    outb(io + ATA_REG_SECCOUNT0, 1);
    outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
    outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
    outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));

    outb(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT);

    //ide_poll(io);
    //ide_poll_irq(io);

    ide_poll_drq(io);

    for(int i = 0; i < 256; i++) {
        outw(io, *(uint16_t*)(buf + (i * 2)));
    }
    
    ata_ide_400ns_delay(io);

    outb(io + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);

    ide_poll_bsy(io);
    
    return 1;
}

void ata_pio_write_sectors(disk_t disk, uint8_t *buf, uint32_t lba, size_t sectors) {
    ata_drive_t* drive = disk.priv_data;

    for(size_t i = 0; i < sectors; i++) {
        //qemu_log("ATA WRITE: LBA: %d", lba + i);
        if(drive->is_lba48_supported) {
            ata_pio_write_raw_sector(disk, buf + (i * drive->block_size), lba + i);
        } else {
            ata_pio_write_raw_sector_24lba(disk, buf + (i * (drive->block_size)), lba + i);
        }
    }
}

void ata_pio_read_sectors(disk_t disk, uint8_t *buf, uint32_t lba, uint32_t numsects) {
    ata_drive_t* drive = disk.priv_data;
    uint8_t* rbuf = buf;

    for(size_t i = 0; i < numsects; i++) {
        if(drive->is_lba48_supported) {
            ata_pio_read_sector(disk, rbuf, lba + i);
        } else {
            ata_pio_read_sector_24lba(disk, rbuf, lba + i);
        }
        rbuf += drive->block_size;
    }
}

void ata_read(disk_t disk, uint64_t location, uint32_t length, void* buf) {
    ata_drive_t* drive = disk.priv_data;
    qemu_log("Read: Disk: %d", drive->drive_id);

	size_t start_sector = location / drive->block_size;
	size_t end_sector = (location + length - 1) / drive->block_size;
	size_t sector_count = end_sector - start_sector + 1;
	size_t real_length = sector_count * drive->block_size;

	uint8_t* real_buf = kmalloc(real_length);

    ata_pio_read_sectors(disk, real_buf, start_sector, sector_count);

    memcpy(buf, real_buf + (location % drive->block_size), length);

	kfree(real_buf);
}

void ata_write(disk_t disk, uint64_t location, uint32_t length, const void* buf) {
    ata_drive_t* drive = disk.priv_data;

    qemu_log("Write: Disk: %d", drive->drive_id);
	
	size_t start_sector = location / drive->block_size;
    size_t end_sector = (location + length - 1) / drive->block_size;
    size_t sector_count = end_sector - start_sector + 1;
    
    uint8_t* temp_buf = kmalloc(sector_count * drive->block_size);

	ata_pio_read_sectors(disk, temp_buf, start_sector, sector_count);
   
    size_t start_offset = location % drive->block_size;
    memcpy(temp_buf + start_offset, buf, length);

	ata_pio_write_sectors(disk, temp_buf, start_sector, sector_count);
    
    kfree(temp_buf);
}

uint64_t ata_get_capacity(disk_t disk) {
    ata_drive_t* drive = disk.priv_data;
    
    return drive->capacity * drive->block_size;
}

void ata_attach(__attribute__((unused)) disk_t disk) {
    // Nothing.
}

void ata_detach(__attribute__((unused)) disk_t disk) {
    // Nothing.
}

bool ata_ide_identify(uint8_t bus, uint8_t drive) {
	uint16_t io = (bus == PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

	uint8_t drive_num = (bus << 1) | drive;

	ata_ide_soft_reset(io);
	ata_ide_select_drive(bus, drive);

	outb(io + ATA_REG_SECCOUNT0, 0);
	outb(io + ATA_REG_LBA0, 0);
	outb(io + ATA_REG_LBA1, 0);
	outb(io + ATA_REG_LBA2, 0);
	
	outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	
	size_t timeout = 0xffff;
	uint8_t status = inb(io + ATA_REG_STATUS);

    uint16_t *ide_buf = calloc(256, 2);

    if(status) {
		uint8_t seccount = inb(io + ATA_REG_SECCOUNT0);
		uint8_t lba_l = inb(io + ATA_REG_LBA0);
		uint8_t lba_m = inb(io + ATA_REG_LBA1);
		uint8_t lba_h = inb(io + ATA_REG_LBA2);

        // VirtualBox sets these values (means error)
	    if(seccount == 0x7F && lba_l == 0x7F && lba_m == 0x7F && lba_h == 0x7F) {
            kfree(ide_buf);
            return false;
		}

		/* Now, poll until BSY is clear. */
		while((status & ATA_SR_BSY) != 0){
			if(status & ATA_SR_ERR) {
                kfree(ide_buf);
                return false;
			}

			if(!timeout) {
                kfree(ide_buf);
                return false;
			} else {
				timeout--;
			}

			status = inb(io + ATA_REG_STATUS);
		}

		timeout = 0xffff;

		while(!(status & ATA_SR_DRQ)) {
			if(status & ATA_SR_ERR) {
                kfree(ide_buf);
                return false;
			}

			if(!timeout) {
                kfree(ide_buf);
                return false;
			}

			timeout--;

			status = inb(io + ATA_REG_STATUS);
		}
		
		for(int i = 0; i < 256; i++) {
			*(uint16_t *)(ide_buf + i) = inw(io + ATA_REG_DATA);
		}

        ata_drive_t* drive_info = calloc(1, sizeof(ata_drive_t));

        drive_info->is_medium_inserted = true;

		// Dump model and firmware version
		uint16_t* fwver = (uint16_t *)drive_info->fwversion;
		uint16_t* model_name = (uint16_t *)drive_info->model_name;
		uint16_t* serial = (uint16_t *)drive_info->serial_number;

		memcpy(serial, ide_buf + 10, 20);
		memcpy(fwver, ide_buf + 23, 8);
		memcpy(model_name, ide_buf + 27, 40);

		for(int i = 0; i < 10; i++) {
			serial[i] = bit_flip_short(serial[i]);
		}

		for(int i = 0; i < 4; i++) {
			fwver[i] = bit_flip_short(fwver[i]);
		}

		for(int i = 0; i < 20; i++) {
			model_name[i] = bit_flip_short(model_name[i]);
		}

		// Zero-terminate the strings
		((uint8_t*)serial)[19] = 0;
		((uint8_t*)fwver)[7] = 0;
		((uint8_t*)model_name)[39] = 0;

        size_t capacity = (ide_buf[101] << 16) | ide_buf[100];  // 32-bit sector value

        bool is_lba48_supported = (ide_buf[83] & (1 << 10)) != 0;

        qemu_log("LBA48: %d", is_lba48_supported);

		drive_info->drive_id = drive_num;
		drive_info->block_size = 512;
		drive_info->capacity = capacity;
        drive_info->is_dma = (ide_buf[49] & 0x200) != 0;
        drive_info->is_lba48_supported = is_lba48_supported;

        check();
        printf("Found drive %d:%d\n", bus, drive);
        printf("\nModel: %s\nSerial: %s\nFirmware version: %s\n", (char*)model_name, (char*)serial, (char*)fwver);

        // REGISTER HERE

        diskmgr_add_disk(drive_info, ata_attach, ata_read, ata_write, ata_get_capacity, ata_detach);
	} else {
        kfree(ide_buf);
		return false;
	}

    kfree(ide_buf);
	return true;
}

void ide_primary_irq(__attribute__((unused)) registers_t regs) {
	inb(ATA_PRIMARY_IO + ATA_REG_STATUS);
    //qemu_log("ATA PRIMARY IRQ");
    //ide_primary_fired = true;
}

void ide_secondary_irq(__attribute__((unused)) registers_t regs) {
	inb(ATA_SECONDARY_IO + ATA_REG_STATUS);
    //qemu_log("ATA SECONDARY IRQ");
    //ide_secondary_fired = true;
}

void ata_init() {
    //install_irq_handler(ATA_PRIMARY_IRQ, ide_primary_irq);
	//install_irq_handler(ATA_SECONDARY_IRQ, ide_secondary_irq);

    ata_ide_identify(PRIMARY, MASTER);
	ata_ide_identify(PRIMARY, SLAVE);
	ata_ide_identify(SECONDARY, MASTER);
	ata_ide_identify(SECONDARY, SLAVE);
}
