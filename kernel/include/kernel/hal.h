#ifndef _KERNEL_HAL_H
#define _KERNEL_HAL_H

#include <stdint.h>
#include "multiboot.h"

int init_hal(multiboot_info_t *multiboot_info);

#endif