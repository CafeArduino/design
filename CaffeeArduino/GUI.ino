#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


#define LCD_BLACK   0x0000
#define LCD_BLUE    0x001F
#define LCD_RED     0xF800
#define LCD_GREEN   0x07E0
#define LCD_CYAN    0x07FF
#define LCD_MAGENTA 0xF81F
#define LCD_YELLOW  0xFFE0
#define LCD_WHITE   0xFFFF



/////////////
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);




void setup_gui() {
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
    identifier=0x9341;
  tft.begin(identifier);
  screen_clear();
  screen_string_println("\n  Cafe Arduino!\n     Booting!");
}

/*void screen_string_print(String s) {
  tft.print(s);
}

void screen_hex_print(uint32_t h){
  tft.print(h, HEX);
};

void screen_integer_print(uint32_t h){
  tft.print(h);
};
*/
void screen_string_println(String s) {
   tft.println(s);  
}
/*
void screen_hex_println(uint32_t h){
  tft.println(h, HEX);
};

void screen_integer_println(uint32_t h){
  tft.println(h);
};
*/
void screen_clear(){
  tft.fillScreen(LCD_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(LCD_RED);
  tft.setTextSize(3);
}
/////////////


// class Screen 

void Screen::print(String s){
  screen_string_println(s);
}

void Screen::clear(){
  screen_clear();  
}



