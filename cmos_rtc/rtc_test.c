#include <stdio.h>
#include <time.h>

#include "cmos_rtc.h"

#define BCD2BIN(n) ((((n) >> 4) * 10) + ((n) & 0x0F))

static unsigned char read_register(unsigned char reg)
{
    rtc_set_register(reg);
    return rtc_get_register_value();
}

static int is_valid_bcd(unsigned char value)
{
    return (value & 0x0F) <= 9 && ((value >> 4) & 0x0F) <= 9;
}

static int expect_range(const char *name, int value, int min, int max)
{
    if (value < min || value > max) {
        fprintf(stderr, "%s out of range: %d (expected %d-%d)\n",
                name,
                value,
                min,
                max);
        return 0;
    }

    return 1;
}

static int expect_bcd(const char *name, unsigned char value)
{
    if (!is_valid_bcd(value)) {
        fprintf(stderr, "%s is not valid BCD: 0x%02X\n", name, value);
        return 0;
    }

    return 1;
}

int main(void)
{
    unsigned char century_bcd;
    unsigned char year_bcd;
    unsigned char month_bcd;
    unsigned char day_bcd;
    unsigned char hours_bcd;
    unsigned char minutes_bcd;
    unsigned char seconds_bcd;
    int century;
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
    int ok = 1;

    rtc_init();

    century_bcd = read_register(RTC_CENTURY);
    year_bcd = read_register(RTC_YEAR);
    month_bcd = read_register(RTC_MONTH);
    day_bcd = read_register(RTC_DAY_OF_MONTH);
    hours_bcd = read_register(RTC_HOURS);
    minutes_bcd = read_register(RTC_MINUTES);
    seconds_bcd = read_register(RTC_SECONDS);

    ok &= expect_bcd("century", century_bcd);
    ok &= expect_bcd("year", year_bcd);
    ok &= expect_bcd("month", month_bcd);
    ok &= expect_bcd("day", day_bcd);
    ok &= expect_bcd("hours", hours_bcd);
    ok &= expect_bcd("minutes", minutes_bcd);
    ok &= expect_bcd("seconds", seconds_bcd);

    century = BCD2BIN(century_bcd);
    year = BCD2BIN(year_bcd);
    month = BCD2BIN(month_bcd);
    day = BCD2BIN(day_bcd);
    hours = BCD2BIN(hours_bcd);
    minutes = BCD2BIN(minutes_bcd);
    seconds = BCD2BIN(seconds_bcd);

    ok &= expect_range("century", century, 19, 99);
    ok &= expect_range("year", year, 0, 99);
    ok &= expect_range("month", month, 1, 12);
    ok &= expect_range("day", day, 1, 31);
    ok &= expect_range("hours", hours, 0, 23);
    ok &= expect_range("minutes", minutes, 0, 59);
    ok &= expect_range("seconds", seconds, 0, 59);

    printf("RTC: %02d%02d-%02d-%02d %02d:%02d:%02d\n",
           century,
           year,
           month,
           day,
           hours,
           minutes,
           seconds);

    rtc_shutdown();

    if (!ok) {
        return 1;
    }

    printf("Test OK\n");
    return 0;
}
