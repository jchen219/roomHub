
#include "clock.h"

// change_time_info.hour = 0;
// change_time_info.min = 0;
// change_time_info.sec = 0;
// change_time_info.cursor_position = 0;
// change_time_info.cursor_position = 4;
void init_clock(){
 setTime(1,1,1,1,1,1970);
}

void get_date( int *day_, int *weekday_, int *month_, int *year_){
  time_t current_time = now();
  *day_ = day(current_time);
  *weekday_ = weekday(current_time);
  *month_ = month(current_time);
  *year_ = year(current_time);
}

void get_time( int *hour_, int *min_, int *sec_){
  time_t current_time = now();
  *hour_ = hour(current_time);
  *min_ = minute(current_time);
  *sec_ = second(current_time);
}

void set_time( int *hour, int *min, int *sec){
  time_t current_time = now();
  setTime( *hour, *min, *sec, day(current_time), month(current_time), year(current_time) );
}

void set_date(int *day, int *month, int *year){
  time_t current_time = now();
  setTime( hour(current_time), minute(current_time), second(current_time), *day, *month, *year );
}
