/**
Adafruit 1.8" SPI display GUI menu library
Author: Jeffery Chen
**/

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <avr/pgmspace.h>
#include <Time.h>
#include <TimeAlarms.h>

#include "constants.h"
#include "clock.h"

#define NUM_OF_MENU_ITEMS (sizeof(menu)/sizeof(menu_item_t))

char str_buffer[30];    // make sure this is large enough for the largest string it must hold
change_time_t change_time_info = {0,0,0,0,4};
change_date_t change_date_info = {1,1,2015,0,4};

int8_t current_depth;   //keeps track of what level of menu we're currently on
menu_enum current_screen;
uint8_t current_highlight;
uint8_t currently_displayed_num; //number of currently displayed options
uint8_t background_colour = ST7735_BLACK;
bool at_endpoint = false;

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void clear_screen(){
  tft.fillScreen(background_colour);
}

void init_screen(int tab_colour){
  Serial.begin(9600);
  Serial.print("initializing menu...\n");
    
  //initializes a ST7735S chip of a certain tab colour
  tft.initR(tab_colour);
  tft.setRotation(3); //turn the screen horizontal    
  clear_screen();
  
  tft.setCursor(0,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextWrap(true);
  tft.print("WELCOME....");
  
  Serial.println("Done!\nWelcome!");
  delay(500);
  tft.fillScreen(ST7735_BLACK);
}

uint8_t read_str_pgm(char * sbuffer, const char * const * str_loc){
  if( sbuffer && str_loc )
  {
    strcpy_P(sbuffer, (char*)pgm_read_word(str_loc)); 
    return 1;
  }
  else
    return 0;
}

uint8_t read_menu_info(char *sbuffer, bool *endpoint,  uint8_t *parent, uint8_t *children, const struct menu_item *mitem){
  if( sbuffer && endpoint && mitem ){
    strcpy_P(sbuffer, (char*) pgm_read_word(&(mitem->title)));
    *endpoint = pgm_read_byte(&(mitem->endpoint));
    *parent = pgm_read_byte(&(mitem->parent));
    for( int i=0; i<MAX_NUM_OPTIONS; i++ ){
      children[i] = pgm_read_byte(&(mitem->children[i]));
    }
    return 1;
  }
  else  
    return 0;
}

void print_menu_item( uint8_t x, uint8_t y, char* msg){
  uint8_t cursor_x = 0;
  uint8_t cursor_y = 0;
  
  if( x >= MENU_GRID_X )x = 0;
  if( y >= (MAX_NUM_OPTIONS + 1) )y = 0;
  
  switch(y){
    case 0:
      cursor_y = MENU_ROW_0;
      break;
    case 1:
      cursor_y = MENU_ROW_1;
      break;  
    case 2:
      cursor_y = MENU_ROW_2;
      break;  
    case 3:
      cursor_y = MENU_ROW_3;
      break;  
    case 4:
      cursor_y = MENU_ROW_4;
      break;  
    case 5:
      cursor_y = MENU_ROW_5;
      break;     
  }
  
  switch(x){
    case 0:
      cursor_x = MENU_COL_0;
      break;
    case 1:
      cursor_x = MENU_COL_1;
      break;  
    case 2:
      cursor_x = MENU_COL_2;
      break;
  }
  
  tft.setTextSize(1);
  tft.setCursor(cursor_x,cursor_y);
  if( current_highlight == y )
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
  else
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
  tft.print(msg);
}

void print_clock(int *hour, int *min, int *sec){
  tft.setTextSize(3);
  tft.setCursor(7,20);
  
  tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    
  memset( str_buffer, 0, sizeof(str_buffer) );
  sprintf( str_buffer, "%02d:%02d:%02d", *hour, *min, *sec);
  tft.print( str_buffer );
}

void print_date(int *day, int *weekday, int *month, int *year){
  char short_buff[4];
  
  tft.setTextSize(2);
  tft.setCursor(0,50);
  
  tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
  memset( str_buffer, 0, sizeof(str_buffer) );
  
  strcpy( short_buff, monthShortStr(*month));
  
  sprintf( str_buffer, "%s%s%d/%d", dayShortStr((uint8_t)*weekday), short_buff, *day, *year );
  tft.print( str_buffer );
}

void parse_menu( menu_enum menu_option ){
  uint8_t i, child, parent, pholder;
  bool endpoint, eholder;
  uint8_t children[MAX_NUM_OPTIONS], holder[MAX_NUM_OPTIONS];
  
  currently_displayed_num = 0;
  
  if( menu_option < none && menu_option){
    read_menu_info( str_buffer, &endpoint, &parent, &children[0], &MENU[menu_option] );
    for( i=0; i<MAX_NUM_OPTIONS; i++ ){  
      child = children[i];
      if( child ){
        read_menu_info( &current_options[i].title[0], &eholder, &pholder, holder, &MENU[child] );
        current_options[i].index = (menu_enum)child;
        currently_displayed_num++;
      }
      else
        break;
    }
    
    read_menu_info( &current_options[i].title[0], &eholder, &pholder, holder, &MENU[none] );
    current_options[i].index = (menu_enum) parent;
    currently_displayed_num++;
      
    if( !endpoint )
      at_endpoint = false;
    else
      at_endpoint = true;
  }
}

void display_menu(){
  for( int i=0; i < currently_displayed_num; i++ ){
    print_menu_item( 0, i, current_options[i].title );
  }
}

void display_homescreen(){
  int hour, min, sec, day, weekday, month, year;
  
  get_time( &hour, &min, &sec);
  get_date( &day, &weekday, &month, &year);
  
  print_clock( &hour, &min, &sec);
  print_date( &day, &weekday, &month, &year);
  
  current_highlight = 5;
  current_options[current_highlight].index = options;
  strcpy_P(current_options[current_highlight].title, (char*) pgm_read_word(&(MENU[options].title)));
  print_menu_item( 1, current_highlight, current_options[current_highlight].title );
}

void display_default_endpoint(){
  display_menu();
}

void change_time_handler( int action ){
  if( change_time_info.cursor_position == 0 ){
    if( action == BUTTON_DOWN )
      change_time_info.hour--;
    else if( action == BUTTON_UP )
      change_time_info.hour++;
  }
  else if( change_time_info.cursor_position == 1 ){
    if( action == BUTTON_DOWN )
      change_time_info.min--;
    else if( action == BUTTON_UP )
      change_time_info.min++;
  }
  else if( change_time_info.cursor_position == 2 ){
    if( action == BUTTON_DOWN )
      change_time_info.sec--;
    else if( action == BUTTON_UP )
      change_time_info.sec++;
  }
  
  if( change_time_info.hour > max_hour )  change_time_info.hour = 0;
  if( change_time_info.min > max_min )  change_time_info.min = 0;
  if( change_time_info.sec > max_sec )  change_time_info.sec = 0;
  if( change_time_info.hour < 0 )  change_time_info.hour = max_hour;
  if( change_time_info.min < 0 )  change_time_info.min = max_min;
  if( change_time_info.sec < 0 )  change_time_info.sec = max_sec;
  
  if( change_time_info.cursor_position == 3 ){
    if( action == BUTTON_SELECT ){
      set_time( (int *)&change_time_info.hour, (int *)&change_time_info.min, (int *)&change_time_info.sec );
      change_time_info.cursor_position = 0;
      change_menu();
    }
  }
  else if( change_time_info.cursor_position == 4 ){
    if( action == BUTTON_SELECT ){
      change_time_info.cursor_position = 0;
      change_menu();
    }
  }
  
  if( action == BUTTON_LEFT ){
    change_time_info.cursor_position--;
    if( change_time_info.cursor_position < 0 )
      change_time_info.cursor_position = change_time_info.max_cursor_position;
  }
  else if( action == BUTTON_RIGHT ){
    change_time_info.cursor_position++;
    if( change_time_info.cursor_position > change_time_info.max_cursor_position )
      change_time_info.cursor_position = 0;
  }
}

int current_max_day(){
  if( change_date_info.month == 1 || change_date_info.month == 3 || change_date_info.month == 5 || change_date_info.month == 7 || change_date_info.month == 8 || change_date_info.month == 10 || change_date_info.month == 12) return 31;
  else if( change_date_info.month == 4 || change_date_info.month == 6 || change_date_info.month == 9 || change_date_info.month == 11)  return 30;
  else if( LEAP_YEAR(change_date_info.year-1970) && change_date_info.month == 2) return 29;
  else if( !LEAP_YEAR(change_date_info.year-1970) && change_date_info.month == 2) return 28; 
  return -1;
}

void change_date_handler( int action ){
  if( change_date_info.cursor_position == 0 ){
    if( action == BUTTON_LEFT )
      change_date_info.month--;
    else if( action == BUTTON_RIGHT )
      change_date_info.month++;
  }
  else if( change_date_info.cursor_position == 1 ){
    if( action == BUTTON_LEFT )
      change_date_info.day--;
    else if( action == BUTTON_RIGHT )
      change_date_info.day++;
  }
  else if( change_date_info.cursor_position == 2 ){
    if( action == BUTTON_LEFT )
      change_date_info.year--;
    else if( action == BUTTON_RIGHT )
      change_date_info.year++;
  }
  
  if( change_date_info.month > max_month )  change_date_info.month = 1;
  else if( change_date_info.month < 1 )  change_date_info.month = max_month;
  if( change_date_info.year > max_year )  change_date_info.year = min_year;
  else if( change_date_info.year < min_year )  change_date_info.year = max_year;
  if( change_date_info.day > current_max_day() ) change_date_info.day = 1;
  else if( change_date_info.day < 1 )  change_date_info.day = current_max_day();
  
  if( change_date_info.cursor_position == 3 ){
    if( action == BUTTON_SELECT ){
      set_date( (int *)&change_date_info.day, (int *)&change_date_info.month, (int *)&change_date_info.year );
      change_date_info.cursor_position = 0;
      change_menu();
    }
  }
  else if( change_date_info.cursor_position == 4 ){
    if( action == BUTTON_SELECT ){
      change_date_info.cursor_position = 0;
      change_menu();
    }
  }
  
  if( action == BUTTON_UP ){
    change_date_info.cursor_position--;
    if( change_date_info.cursor_position < 0 )
      change_date_info.cursor_position = change_date_info.max_cursor_position;
  }
  else if( action == BUTTON_DOWN ){
    change_date_info.cursor_position++;
    if( change_date_info.cursor_position > change_date_info.max_cursor_position )
      change_date_info.cursor_position = 0;
  }
}

void display_change_time_endpoint(){
  tft.setTextSize(3);
  
  tft.drawTriangle(26, 20, 11, 35, 41, 35, ST7735_WHITE);
  tft.drawTriangle(26, 80, 11, 65, 41, 65, ST7735_WHITE);
  tft.drawTriangle(80, 20, 65, 35, 95, 35, ST7735_WHITE);
  tft.drawTriangle(80, 80, 65, 65, 95, 65, ST7735_WHITE);
  tft.drawTriangle(135, 20, 120, 35, 150, 35, ST7735_WHITE);
  tft.drawTriangle(135, 80, 120, 65, 150, 65, ST7735_WHITE);
  current_highlight = 0;
      
  if( change_time_info.cursor_position == 0 ){
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
    sprintf(str_buffer, "%02d", change_time_info.hour);
    tft.setCursor(10,40);
    tft.print(str_buffer);
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    sprintf(str_buffer, ":%02d:%02d", change_time_info.min, change_time_info.sec);
    tft.setCursor(46,40);
    tft.print(str_buffer);
  }
  else if( change_time_info.cursor_position == 1 ){
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    sprintf(str_buffer, "%02d:", change_time_info.hour);
    tft.setCursor(10,40);
    tft.print(str_buffer);
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
    sprintf(str_buffer, "%02d", change_time_info.min);
    tft.setCursor(63,40);
    tft.print(str_buffer);
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    sprintf(str_buffer, ":%02d", change_time_info.sec);
    tft.setCursor(100,40);
    tft.print(str_buffer);
  }
  else if( change_time_info.cursor_position == 2 ){
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    sprintf(str_buffer, "%02d:%02d:", change_time_info.hour, change_time_info.min);
    tft.setCursor(10,40);
    tft.print(str_buffer);
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
    sprintf(str_buffer, "%02d", change_time_info.sec);
    tft.setCursor(116,40);
    tft.print(str_buffer);
  }
  else{
    sprintf(str_buffer, "%02d:%02d:%02d", change_time_info.hour, change_time_info.min, change_time_info.sec);
    tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
    tft.setCursor(10,40);
    tft.print(str_buffer);
    if( change_time_info.cursor_position == 3){
      current_highlight = 4;
    }
    else if( change_time_info.cursor_position == 4 ){
      current_highlight = 5;
    }
  }
  
  current_options[4].index = options;
  current_options[5].index = options;
  strcpy(current_options[4].title, "SET");
  strcpy(current_options[5].title, "CANCEL");
  print_menu_item( 1, 4, current_options[4].title );
  print_menu_item( 1, 5, current_options[5].title );
}

void display_change_date_endpoint(){
  tft.setTextSize(2);
  current_highlight = 0;
  
  tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
  tft.drawTriangle(1, 15, 21, 5, 21, 25, ST7735_WHITE);
  tft.drawTriangle(158, 15, 138, 5, 138, 25, ST7735_WHITE);
  tft.setCursor(28,9);
  if( change_date_info.cursor_position == 0 )
      tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
  sprintf(str_buffer, "%-9s", monthStr(change_date_info.month));
  tft.print(str_buffer);
  
  tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
  tft.drawTriangle(11, 40, 31, 30, 31, 50, ST7735_WHITE);
  tft.drawTriangle(150, 40, 130, 30, 130, 50, ST7735_WHITE);
  tft.setCursor(40,34);
  if( change_date_info.cursor_position == 1 )
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
  sprintf(str_buffer, "%02d", change_date_info.day);
  tft.print(str_buffer);
  
  tft.setTextColor(ST7735_WHITE,ST7735_BLACK);
  tft.drawTriangle(11, 65, 31, 55, 31, 75, ST7735_WHITE);
  tft.drawTriangle(150, 65, 130, 55, 130, 75, ST7735_WHITE);
  tft.setCursor(40,59);
  if( change_date_info.cursor_position == 2 )
    tft.setTextColor(ST7735_BLACK,ST7735_WHITE);
  sprintf(str_buffer, "%04d", change_date_info.year);
  tft.print(change_date_info.year);
  
  if( change_date_info.cursor_position == 3 )
    current_highlight = 4;
  else if( change_date_info.cursor_position == 4 )
    current_highlight = 5;
  
  current_options[4].index = options;
  current_options[5].index = options;
  strcpy(current_options[4].title, "SET");
  strcpy(current_options[5].title, "CANCEL");
  print_menu_item( 1, 4, current_options[4].title );
  print_menu_item( 1, 5, current_options[5].title );
}

void display_endpoint(){
  switch ( current_screen ) {
    case change_time:
      display_change_time_endpoint();
      break;
//    case sync_time:
//    
//      break;
//    case change_alarm_tone:
//    
//      break;
//    case change_alarm_time:
//    
//      break;
//    case enable_disable:
//    
//      break;
    case change_date:
      display_change_date_endpoint();
      break;
    default:
      display_default_endpoint();
      break;
  }
}

void display_screen(){
  if( current_screen != homescreen ){
      if( !at_endpoint )
        display_menu();
      else
        display_endpoint();
  } 
  else
    display_homescreen();
}

uint8_t readButton(void) {
  float a = analogRead(3);
  
  a *= 5.0;
  a /= 1024.0;
  
  Serial.print("Button read analog = ");
  Serial.println(a);
  if (a < 0.2) return BUTTON_LEFT;
  if (a < 1.0) return BUTTON_DOWN;
  if (a < 1.5) return BUTTON_SELECT;
  if (a < 2.0) return BUTTON_RIGHT;
  if (a < 3.2) return BUTTON_UP;
  else return BUTTON_NONE;
}

void change_menu(){
  current_screen = current_options[current_highlight].index;
  clear_screen();
  current_highlight = 0;
  at_endpoint = false;
  memset(current_options, 0, sizeof(current_options));
  parse_menu(current_screen);
}

void parse_action( int action ){
  if( !at_endpoint ){
    switch( action ) {
      case BUTTON_DOWN:
         if( current_highlight < (currently_displayed_num - 1)){      
          current_highlight++;
         }
        break;
      case BUTTON_UP:
        if( current_highlight > 0 ){
          current_highlight--;
        }
        break;
      case BUTTON_RIGHT: //Single column row menu for now
          break;
      case BUTTON_LEFT:
          break;
      case BUTTON_SELECT:
          change_menu();
        break;      
      default:
        break;    
    }
  }
  else{
    switch( current_screen ){
      case change_time:
        change_time_handler( action );
        break;
      case change_date:
        change_date_handler( action );
        break;
      default:
        change_menu();
        break;
    } 
  }
}
void setup(){
  init_screen(INITR_BLACKTAB);
  init_clock();
  
  current_depth = currently_displayed_num = 0;
  current_highlight = MENU_ROW_0;
  current_screen = homescreen;
  memset( current_options, 0, sizeof(current_options));
   
  parse_menu(current_screen);
  display_screen();
}

void loop(){
  int button;
  while(1){
    display_screen();
    if( button = readButton() ){
      parse_action( button );
      delay(50); //to prevent over stepping the navigation
    }
  }
}

