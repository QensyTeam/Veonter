#pragma once

#include <time.h>

time_t get_time();
void set_time_provider(time_t (*prov)());
