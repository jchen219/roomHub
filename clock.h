#ifndef time
#define time
#include <Time.h>

#define max_sec 59
#define max_hour 23
#define max_min 59
#define max_alarms 5
#define max_year 3000
#define min_year 1970
#define max_month 12

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

typedef struct change_time_s{
  int8_t hour;
  int8_t min;
  int8_t sec;
  int8_t cursor_position;
  int8_t max_cursor_position;
}change_time_t;

typedef struct change_date_s{
  int8_t month;
  int8_t day;
  int year;
  int8_t cursor_position;
  int8_t max_cursor_position;
}change_date_t;

void init_clock();
void get_date( int *day_, int *weekday_, int *month_, int *year_);
void get_time( int *hour_, int *min_, int *sec_);
void set_time( int *hour, int *min, int *sec);
void set_date( int *day, int *month, int *year);

#endif
