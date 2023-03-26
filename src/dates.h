#ifndef DATES_H
#define DATES_H

#include <time.h>

#define N_DATES         22

// Use 'date -d"YYYY-MM-DD hh:mm:ss" "+%s"' to populate the dates array.
// add 3600 for CET and 7200 for CEST
#ifdef __XC8
__eeprom time_t dates[N_DATES] = {
#else
time_t dates[N_DATES] = {
#endif
    1577664000,
    1580515200,
    1580688000,
};
#ifdef __XC8
__eeprom char *descs[N_DATES] = {
#else
char *descs[N_DATES] = {
#endif
    "Data",
    "Altra data",
    "Altra data ancora con descrizione lunga",
};

#endif
