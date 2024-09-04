#pragma once

#include <stdint.h>

#define PCI_ADDRESS_PORT 0xCF8  
#define PCI_DATA_PORT 0xCFC

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t klass;
    uint16_t subclass;
    uint16_t bus;
    uint16_t slot;
    uint16_t func;
    uint16_t hdrtype;
} pci_device_t;

void pci_init();
void pci_scan_everything();