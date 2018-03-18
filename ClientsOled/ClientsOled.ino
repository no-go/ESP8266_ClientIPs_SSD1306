#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include "SSD1306Spi.h"
#include "font.h"

extern "C" { 
#include<user_interface.h>
}

const char *ssid = "ESP rocky";
const char *password = "verySecret";

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;  // i2c SCL
static const uint8_t D2   = 4;  // i2c SDA
static const uint8_t D3   = 0;  // this with reset to GND: flash
static const uint8_t D4   = 2;  // wifi led
static const uint8_t D5   = 14; // pwm
static const uint8_t D6   = 12; // pwm
static const uint8_t D7   = 13;
static const uint8_t D8   = 15; // pwm
static const uint8_t D9   = 3;  // RX
static const uint8_t D10  = 1;  // TX
static const uint8_t D11  = 9;  // does only work without elf2image  --flash_mode dio
static const uint8_t D12  = 10; // does only work without elf2image  --flash_mode dio

// Initialize the OLED display using SPI
// GPIO 14 D5 -> CLK
// GPIO 13 D7 -> MOSI (DOUT)
// GPIO 16 D0 -> RES
// GPIO  4 D2 -> DC
// GPIO 15 D8 -> CS
SSD1306Spi display(D0, D2, D8);

int wifiLed = D4;

int hours,minutes,seconds;
struct ip_addr *IPaddress;
IPAddress address;

void setup() {
  pinMode(D3, INPUT_PULLUP);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH); // LOW = on
  
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();

  display.init();
  display.flipScreenVertically();
  display.setFont(DejaVu_Sans_Condensed_8);
}

void loop() {
  
  delay(1000);
  seconds++;
  if (seconds >= 60) {
    minutes++;
    seconds = seconds%60;
  }
  if (minutes >= 60) {
    hours++;
    minutes = minutes%60;
  }
  if (hours >= 24) {
    hours = hours%24;
  }

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  if (hours<10) {
    display.drawString(81, 0, "0");
    display.drawString(89, 0, String(hours));
  } else {
    display.drawString(89, 0, String(hours));
  }
  display.drawString(94, 0, ":");
  if (minutes<10) {
    display.drawString(101, 0, "0");
    display.drawString(109, 0, String(minutes));
  } else {
    display.drawString(109, 0, String(minutes));
  }
  display.drawString(114, 0, ":");
  if (seconds<10) {
    display.drawString(120, 0, "0");
    display.drawString(128, 0, String(seconds));
  } else {
    display.drawString(128, 0, String(seconds));
  }

  display.drawLine(0, 10, 128, 10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, String(ssid));

  struct station_info *stat_info = wifi_softap_get_station_info();
  int i = 0;
  while (stat_info != NULL) {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;
    display.drawStringMaxWidth(
       0, 13+i, 128,
       String( address&0x000000FF) + String(".") +
       String((address&0x0000FF00)>>8) + String(".") + 
       String((address&0x00FF0000)>>16) + String(".") + 
       String((address&0xFF000000)>>24) 
    );
    stat_info = STAILQ_NEXT(stat_info, next);
    i+=8;
  }
  
  display.display();
  
  if (i>0 || digitalRead(D3) == LOW) {
    digitalWrite(wifiLed, LOW); // LOW = on
  } else {
    digitalWrite(wifiLed, HIGH); // LOW = on
  }
}

