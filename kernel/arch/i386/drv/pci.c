#include <stdint.h>
#include <kernel/sys/ports.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/drv/serial_port.h>
#include <kernel/drv/pci.h>
#include <vector.h>
#include <veo.h>

vector_t* pci_devices = NULL;

uint16_t pci_read_confspc_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    uint32_t addr;
    uint32_t slot32 = slot;
    uint32_t func32 = function;
    
    addr = (uint32_t)(((uint32_t)bus << 16) | (slot32 << 11) |
           (func32 << 8) | (offset & 0xfc) | ((uint32_t)0x80000000)); //yes, this line is copied from osdev

    outl(PCI_ADDRESS_PORT, addr);

    return inl(PCI_DATA_PORT) >> ((offset & 2) * 8);
}

__attribute__((always_inline))
inline uint8_t pci_get_class(uint8_t bus, uint8_t slot, uint8_t function) {
    return (uint8_t) (pci_read_confspc_word(bus, slot, function, 10) >> 8);
}

__attribute__((always_inline))
inline uint8_t pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t function) {
    return (uint8_t) pci_read_confspc_word(bus, slot, function, 10);
}

__attribute__((always_inline))
inline uint8_t pci_get_hdr_type(uint8_t bus, uint8_t slot, uint8_t function) {
    return (uint8_t) pci_read_confspc_word(bus, slot, function, 7);
}

__attribute__((always_inline))
inline uint16_t pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t function) {
    return pci_read_confspc_word(bus, slot, function, 0);
}

__attribute__((always_inline))
inline uint16_t pci_get_device(uint8_t bus, uint8_t slot, uint8_t function) {
    return pci_read_confspc_word(bus, slot, function, 2);
}

void pci_init() {
    pci_devices = vector_new();
}

finite_pointer_t pci_find_devices_by_ven_dev(uint16_t vendor, uint16_t device) {
    finite_pointer_t result = {};
    size_t found = 0;

    for(size_t i = 0; i < pci_devices->size; i++) {
        pci_device_t* dev = (void*)vector_get(pci_devices, i).element;

        if(dev->vendor_id == vendor && dev->device_id == device) {
            found++;
        }
    }

    if(found == 0) {
        return result;
    }

    pci_device_t* devs = calloc(found, sizeof(pci_device_t));
    size_t current_index = 0;

    for(size_t i = 0; i < pci_devices->size; i++) {
        pci_device_t* dev = (void*)vector_get(pci_devices, i).element;

        if(dev->vendor_id == vendor && dev->device_id == device) {
            devs[current_index++] = *dev;
        }
    }

    result.data = devs;
    result.size = found;
    
    return result;
}

void pci_scan_everything() {
    vector_erase_all(pci_devices);

    for (uint32_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint32_t func = 0;
            uint16_t hdrtype = 0, clid = 0, sclid = 0, device = 0;

            uint16_t vendor = pci_get_vendor(bus, slot, func);

            if (vendor != 0xFFFF) {
                clid = pci_get_class(bus, slot, func);
                sclid = pci_get_subclass(bus, slot, func);
                hdrtype = pci_get_hdr_type(bus, slot, func);
                device = pci_get_device(bus, slot, func);

                pci_device_t* dev = calloc(1, sizeof(pci_device_t));
                dev->klass = clid;
                dev->subclass = sclid;
                dev->bus = bus;
                dev->slot = slot;
                dev->func = func;
                dev->hdrtype = hdrtype | 0x80;
                dev->vendor_id = vendor;
                dev->device_id = device;

                qemu_log("%d.%d [%d:%d:%d] %x:%x", dev->klass, dev->subclass, dev->bus, dev->slot, dev->func, dev->vendor_id, dev->device_id);
                vector_push_back(pci_devices, (size_t)dev);
            }

            if ((hdrtype & 0x80) == 0) {
                for (func = 1; func < 8; func++) {
                    vendor = pci_get_vendor(bus, slot, func);

                    if (vendor != 0xFFFF) {
                        clid = pci_get_class(bus, slot, func);
                        sclid = pci_get_subclass(bus, slot, func);
                        device = pci_get_device(bus, slot, func);

                        pci_device_t* dev = calloc(1, sizeof(pci_device_t));
                        dev->klass = clid;
                        dev->subclass = sclid;
                        dev->bus = bus;
                        dev->slot = slot;
                        dev->hdrtype = hdrtype;
                        dev->func = func;
                        dev->vendor_id = vendor;
                        dev->device_id = device;

                        qemu_log("%d.%d [%d:%d:%d] %x:%x", dev->klass, dev->subclass, dev->bus, dev->slot, dev->func, dev->vendor_id, dev->device_id);
                        vector_push_back(pci_devices, (size_t)dev);
                    }
                }
            }
        }
    }
}
