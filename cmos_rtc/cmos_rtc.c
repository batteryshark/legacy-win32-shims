#include <time.h>
#include <string.h>

#include "cmos_rtc.h"

#define CMOS_DATA_SZ 0x80
#define CMOS_REGISTER_MASK 0x7F
#define RTC_REG_A_24_HOUR_RATE 0x26
#define RTC_REG_B_24_HOUR_BCD 0x02
#define RTC_REG_D_VALID_RAM_TIME 0x80

static unsigned char bin_to_bcd(int value)
{
    return (unsigned char)(((value / 10) << 4) | (value % 10));
}

static unsigned char cmos_data[CMOS_DATA_SZ] = {0x00};
static unsigned char selected_register = 0;
static int rtc_initialized = 0;

static int read_local_time(struct tm *local_time)
{
    time_t raw_time = time(NULL);
    struct tm *snapshot;

    if (raw_time == (time_t)-1) {
        return 0;
    }

    snapshot = localtime(&raw_time);
    if (snapshot == NULL) {
        return 0;
    }

    *local_time = *snapshot;
    return 1;
}

static void refresh_rtc_registers(void)
{
    struct tm now;
    int full_year;

    if (!read_local_time(&now)) {
        return;
    }

    full_year = now.tm_year + 1900;

    cmos_data[RTC_SECONDS] = bin_to_bcd(now.tm_sec);
    cmos_data[RTC_MINUTES] = bin_to_bcd(now.tm_min);
    cmos_data[RTC_HOURS] = bin_to_bcd(now.tm_hour);
    cmos_data[RTC_DAY_OF_WEEK] = bin_to_bcd(now.tm_wday + 1);
    cmos_data[RTC_DAY_OF_MONTH] = bin_to_bcd(now.tm_mday);
    cmos_data[RTC_MONTH] = bin_to_bcd(now.tm_mon + 1);
    cmos_data[RTC_YEAR] = bin_to_bcd(full_year % 100);
    cmos_data[RTC_CENTURY] = bin_to_bcd(full_year / 100);

    cmos_data[RTC_REG_A] = RTC_REG_A_24_HOUR_RATE;
    cmos_data[RTC_REG_B] = RTC_REG_B_24_HOUR_BCD;
    cmos_data[RTC_REG_C] = 0x00;
    cmos_data[RTC_REG_D] = RTC_REG_D_VALID_RAM_TIME;
}

void rtc_init(void)
{
    memset(cmos_data, 0, sizeof(cmos_data));
    selected_register = 0;
    rtc_initialized = 1;
    refresh_rtc_registers();
}

void rtc_shutdown(void)
{
    rtc_initialized = 0;
}

void rtc_set_register(unsigned char reg)
{
    selected_register = (unsigned char)(reg & CMOS_REGISTER_MASK);
}

unsigned char rtc_get_register_value(void)
{
    if (!rtc_initialized) {
        rtc_init();
    } else {
        refresh_rtc_registers();
    }

    return cmos_data[selected_register];
}
