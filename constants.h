#ifndef constants
#define constants

#define TFT_CS   10
#define TFT_RST  9 //for Arduino reset
#define TFT_DC   8 //DC power
#define TFT_SCLK 13 //Arduino Uno SPI SCK
#define TFT_MOSI 11 //Arduino Uno SPI MOSI
#define TFT_MISO 12 //Arduino Uno SPI MISO

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

#define MAX_NUM_OPTIONS 5 //Screen only fits about 5 rows comfortably
#define TOP_OFFSET 5
#define LEFT_OFFSET 5
#define MENU_GRID_X 3
#define MENU_ROW_0 0 
#define MENU_ROW_1 ST7735_TFTHEIGHT_18/(MAX_NUM_OPTIONS+2) - TOP_OFFSET
#define MENU_ROW_2 (ST7735_TFTHEIGHT_18/(MAX_NUM_OPTIONS+2))*2 - TOP_OFFSET
#define MENU_ROW_3 (ST7735_TFTHEIGHT_18/(MAX_NUM_OPTIONS+2))*3 - TOP_OFFSET
#define MENU_ROW_4 (ST7735_TFTHEIGHT_18/(MAX_NUM_OPTIONS+2))*4 - TOP_OFFSET
#define MENU_ROW_5 (ST7735_TFTHEIGHT_18/(MAX_NUM_OPTIONS+2))*5 - TOP_OFFSET//If a more dense menu is wanted, increase the amount of MENU_ROW_X definitions
#define MENU_COL_0 0 + 10
#define MENU_COL_1 ST7735_TFTWIDTH/MENU_GRID_X + 10
#define MENU_COL_2 ST7735_TFTWIDTH/MENU_GRID_X + 10

bool enable_alarm;
bool alarm_status;

enum menu_enum {
  homescreen=0,
  options,
  time,
  change_time,
  sync_time,
  alarm_clock,
  change_alarm_tone,
  change_alarm_time,
  add_alarm,
  delete_alarm,
  modify_alarm,
  blinds,
  enable_disable,
  calender,
  change_date,
  none
};

typedef struct menu_item{
  const char *title;
  bool endpoint; //layer ordering starts at 0
  uint8_t parent;
  uint8_t children[5]; //Maximum 5 options on the screen at once, so therefore maximum 5 children
} menu_item_t;

const char menu_root[] PROGMEM = " ROOT ";
const char menu_options[] PROGMEM = " OPTIONS ";
const char menu_time[] PROGMEM = " TIME ";
const char menu_change_time[] PROGMEM = " Change Time ";
const char menu_sync_time[] PROGMEM = " Sync Time ";
const char menu_alarm_clock[] PROGMEM = " Alarm Clock ";
const char menu_change_alarm_tone[] PROGMEM = " Change Alarm Tone ";
const char menu_change_alarm_time[] PROGMEM = " Change Alarms ";
const char menu_add_alarm[] PROGMEM = " Add An Alarm ";
const char menu_delete_alarm[] PROGMEM = " Delete An Alarm ";
const char menu_modify_alarm[] PROGMEM = " Change An Alarm ";
const char menu_blinds[] PROGMEM = " Blinds ";
const char menu_enable_disable[] PROGMEM = " Enable/Disable ";
const char menu_calender[] PROGMEM = " Calender ";
const char menu_change_date[] PROGMEM = " Change Date ";
const char menu_none[] PROGMEM = " BACK ";

const menu_item_t MENU[] PROGMEM = {
  {menu_root, false, homescreen, {options}},
  //ALL THE CATEGORIES OF MENU OPTIONS
  {menu_options, false, homescreen, {time,blinds,calender}},
  //MENU OPTIONS TO CHANGE AND CONTROL TIME/ALARM CLOCK
  {menu_time, false, options, {change_time,sync_time,alarm_clock}},
  {menu_change_time, true, time},
  {menu_sync_time, true, time},
  {menu_alarm_clock, false, time, {change_alarm_tone,change_alarm_time}},
  {menu_change_alarm_tone, true, alarm_clock},
  {menu_change_alarm_time, false, alarm_clock, {add_alarm, delete_alarm, modify_alarm}},
  {menu_add_alarm, true, change_alarm_time},
  {menu_delete_alarm, true, change_alarm_time},
  {menu_modify_alarm, true, change_alarm_time},
  //MENU OPTIONS FOR CONTROLLING THE AUTOMATED BLINDS OPENER SERVICE
  {menu_blinds, false, options, {enable_disable}},
  {menu_enable_disable, true, blinds},
  //MENU OPTIONS FOR CHANGING THE CALENDER
  {menu_calender, false, options, {change_date}},
  {menu_change_date, true, calender},
  {menu_none, false}
};

typedef struct display_option {
  menu_enum index;
  char title[20];
} display_option_t;

display_option_t current_options[MAX_NUM_OPTIONS + 1]; // plus 1 to fit the 'BACK' option

typedef enum month_enum{
  jan,
  feb,
  mar,
  apr,
  may,
  jun,
  jul,
  aug,
  sep,
  oct,
  nov,
  dec
} MONTH_ENUM;

const char month_0[] PROGMEM = "January";   // "String 0" etc are strings to store - change to suit.
const char month_1[] PROGMEM = "February";
const char month_2[] PROGMEM = "March";
const char month_3[] PROGMEM = "April";
const char month_4[] PROGMEM = "May";
const char month_5[] PROGMEM = "June";
const char month_6[] PROGMEM = "July";
const char month_7[] PROGMEM = "August";
const char month_8[] PROGMEM = "September";
const char month_9[] PROGMEM = "October";
const char month_10[] PROGMEM = "November";
const char month_11[] PROGMEM = "December";

const char* const MONTH_TABLE[] PROGMEM = 
{   
  month_0,
  month_1,
  month_2,
  month_3,
  month_4,
  month_5,
  month_6,
  month_7,
  month_8,
  month_9,
  month_10,
  month_11
};

#endif
