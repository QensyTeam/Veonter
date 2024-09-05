#include <time.h>
#include <stdlib.h>

static time_t global_time = {};
static time_t (*time_provider)() = NULL;

time_t get_time() {
    if(time_provider) {
        global_time = time_provider();
    }

    return global_time;
}

void set_time_provider(time_t (*prov)()) {
    time_provider = prov;
}
