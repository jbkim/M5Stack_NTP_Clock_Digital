/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 It uses the time of compile/upload to set the time
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo...

 Based on clock sketch by Gilchrist 6/2/2014 1.0

A few colour codes:
code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink
 */

/* 
  1. Based on the code TFT_CLOCK_Digital of M5Stack example.
  2. Add NTP (Network Time Protocol)
  3. Need to install NTPClient library https://github.com/taranais/NTPClient
*/

#include <M5Stack.h>
#include <WiFi.h>
#include <NTPClient.h> 
#include <WiFiUdp.h>

#define TFT_GREY 0x5AEB

// Replace with your network credentials
// const char* ssid     = "REPLACE_WITH_YOUR_SSID";
// const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* ssid     = "Ateam";
const char* password = "Ateam20140701";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x
uint8_t hh, mm, ss;
// uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

void setup(void) {
  Serial.begin(115200);
  M5.begin();
  // M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT 0 = 0
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  timeClient.setTimeOffset(3600*9);  // GMT+9
}

void loop() {

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }  

  // The formattedDate comes with the following format:
  // 2019-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");\
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);

  M5.Lcd.setTextSize(2);
  M5.Lcd.drawString(dayStamp.c_str(), 200, 0, 1);
  M5.Lcd.setTextSize(1);

  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);

  // convert string 
  const char * c = timeStamp.c_str();

  hh = conv2d(c);
  mm = conv2d(c+3);
  ss = conv2d(c+6);  

  // Update digital time
  int xpos = 0;
  int ypos = 85; // Top left corner ot clock text, about half way down
  int ysecs = ypos + 24;

  if (omm != mm) { // Redraw hours and minutes time every minute
    omm = mm;
    // Draw hours and minutes
    if (hh < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add hours leading zero for 24 hr clock
    xpos += M5.Lcd.drawNumber(hh, xpos, ypos, 8);             // Draw hours
    xcolon = xpos; // Save colon coord for later to flash on/off later
    xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
    if (mm < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
    xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8);             // Draw minutes
    xsecs = xpos; // Sae seconds 'x' position for later display updates
  }
  if (oss != ss) { // Redraw seconds time every second
    oss = ss;
    xpos = xsecs;

    if (ss % 2) { // Flash the colons on/off
      M5.Lcd.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
      M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
      xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); // Seconds colon
      M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
    }
    else {
      M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
      xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); // Seconds colon
    }

    //Draw seconds
    if (ss < 10) xpos += M5.Lcd.drawChar('0', xpos, ysecs, 6); // Add leading zero
    M5.Lcd.drawNumber(ss, xpos, ysecs, 6);                     // Draw seconds
  }
}

// Function to extract numbers from compile time string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}



