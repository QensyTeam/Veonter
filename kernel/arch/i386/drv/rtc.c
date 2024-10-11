#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <kernel/sys/ports.h>

#define CENTURY 20

bool rtc_update_in_progress() {
    outb(0x70, 0x0A);
    return (inb(0x71) & 0x80) != 0;
}

uint8_t get_RTC_register(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

time_t rtc_time_provider() {
    while (rtc_update_in_progress()) {}

	uint8_t cmos_second = get_RTC_register(0x00);
    uint8_t cmos_minute = get_RTC_register(0x02);
    uint8_t cmos_hour = get_RTC_register(0x04);
    uint8_t cmos_day = get_RTC_register(0x07);
    uint8_t cmos_month = get_RTC_register(0x08);
    uint16_t cmos_year = get_RTC_register(0x09);

    uint8_t registerB = get_RTC_register(0x0B);

    if(!(registerB & 0x04)) {
        cmos_second = (cmos_second & 0x0F) + ((cmos_second / 16) * 10);
        cmos_minute = (cmos_minute & 0x0F) + ((cmos_minute / 16) * 10);
        cmos_hour = ((cmos_hour & 0x0F) + (((cmos_hour & 0x70) / 16) * 10) ) | (cmos_hour & 0x80);
        cmos_day = (cmos_day & 0x0F) + ((cmos_day / 16) * 10);
        cmos_month = (cmos_month & 0x0F) + ((cmos_month / 16) * 10);
        cmos_year = (cmos_year & 0x0F) + ((cmos_year / 16) * 10);
    }

    if (!(registerB & 0x02) && (cmos_hour & 0x80)) {
        cmos_hour = ((cmos_hour & 0x7F) + 12) % 24;
    }

    cmos_year += CENTURY * 100;

    return (time_t){
        .year = cmos_year,
        .month = cmos_month,
        .day = cmos_day,
        .hour = cmos_hour,
        .minute = cmos_minute,
        .second = cmos_second
    };
}
