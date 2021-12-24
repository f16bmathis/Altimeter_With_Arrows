/* Removed Baro Knob and added UP and DN arrows. Simpler to adjust
 * the baro while flying. The touch points for the arrows are set as
 * large squares around the arrows, to make it easier to touch. Also
 * on line 19 & 20 are changeable for either one of the touch screens by elegoo or Hiletgo.
 * Copyright January 20, 2020.  Ken Burrell.
 * Edited by Brian Mathis 21 Dec 2021
 */

#include <Arduino.h>
#define USE_ADAFRUIT_SHIELD_PINOUT 1
#include <Adafruit_GFX.h>                                      //added Adafruit GFX with BIO Whatever also
#include <MCUFRIEND_kbv.h>                                     //added mcufriend.kbv
MCUFRIEND_kbv tft;

#include <TouchScreen.h>                                       //added Adafruit Touchscreen library
#define MINPRESSURE 200
#define MAXPRESSURE 1000
const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x9341
//const int TS_LEFT=116,TS_RT=912,TS_TOP=85,TS_BOT=903;        // For HiLetgo 2.4" TFT screen
const int TS_LEFT=912,TS_RT=116,TS_TOP=85,TS_BOT=903;          // For Elegoo TFT Screen
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button enter_btn;

// Assign human-readable names to some common 16-bit color values:

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#undef FLIP_180

int pixel_x, pixel_y, pixel_z;     //Touch_getXY() updates global vars

bool Touch_getXY(void)

{

    TSPoint p = ts.getPoint();

    pinMode(YP, OUTPUT);      //restore shared pins

    pinMode(XM, OUTPUT);

    digitalWrite(YP, HIGH);   //because TFT control pins

    digitalWrite(XM, HIGH);

    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);

    if (pressed) {

#ifdef FLIP_180                           // These next "if" lines affect the Touch of the screen

        pixel_x = map(p.x, TS_RT, TS_LEFT, 0, tft.width());

        pixel_y = map(p.y, TS_BOT, TS_TOP, 0, tft.height());

#else

        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());

        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());

#endif

        pixel_z = p.z;

    }

    return pressed;

}

float B = 29.92 ;

int delta = 25;

int clockCenterX=120;

int clockCenterY=110+delta;

#include <Wire.h>

#include <Adafruit_Sensor.h>                              //added Adafruit BMP280 library and all

#include <Adafruit_BMP280.h>

#include <SPI.h>                                                        

#define BMP_CS 19                                         // I'm using the BMP pressure chip

Adafruit_BMP280 bmp(BMP_CS) ;; // hardware SPI                          

void setup() {

   Serial.begin(9600);

   if (!bmp.begin(0x76)) {

     Serial.println("BMP init failed!");

       while (1);

  }

  uint16_t identifier = tft.readID();

  if(identifier == 0x9341) {

    Serial.println(F("Found ILI9341 LCD driver"));     // Use Serial Monitor to see if the screen was found

    tft.begin(identifier);
    tft.fillScreen(BLACK);
    tft.setRotation(0);                               // Flips Rotation 180 from 0 to 2, Landscape / portrate

    } else {

    return;

    }

  }

void drawDisplay(float B)

{

    tft.fillScreen(BLACK);

// Set up fixed text

    tft.setTextColor(GREEN,BLACK);  tft.setTextSize(3);
    tft.setCursor(2, 300); tft.println("DEG F");                             // For Temp reading

// Draw Clockface
  // first clear clock

  tft.fillCircle(clockCenterX, clockCenterY, 100, BLACK);

 for (int i=0; i<2; i++)
  {
    tft.drawCircle(clockCenterX, clockCenterY, 100-i, GREEN);
  }
 for (int i=0; i<3; i++)
{
    tft.drawCircle(clockCenterX, clockCenterY, i, GREEN);
  }
  drawMarks();
  drawNumbs();

  tft.setTextColor(WHITE); tft.setTextSize(3);      // Color and Size of Baro Readout
  tft.setCursor(80, 225+delta); tft.println(B,2);   // L to R position & up & down position of Baro Readout
 
}

void drawMarks()                                    // For the compass rose

{

  float x1, y1, x2, y2, h, phi;

  tft.setTextColor(GREEN); tft.setTextSize(1);

  for (int i=0; i<10; i++)

  {

    h   = i*36.0;
    phi = radians(h);
    x1= 99.0*sin(phi);
    y1=-99.0*cos(phi);
    x2= 89.0*sin(phi);
    y2=-89.0*cos(phi);

     tft.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY, GREEN);

  }

   for (int i=0; i<50; i++)

  {

    h   = i*7.20;

    phi = radians(h);

    x1= 99.0*sin(phi); float y1=-99.0*cos(phi);
    x2= 94.0*sin(phi); float y2=-94.0*cos(phi);

    tft.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY, GREEN);

  }

}

void drawNumbs()                              // For the Circle

{
  float x3, y3, h, phi;
  int j,k;
  tft.setTextColor(GREEN); tft.setTextSize(1);
  for (int i=0; i<10; i++)
  {
    h=i*36.0;
    phi = radians(h);
   j = 85.0*sin(phi)+clockCenterX;
    k =-85.0*cos(phi)+clockCenterY;
    tft.setCursor(j,k );
    tft.println(i);
  }
}

void drawit(float elev, int type)

{
  float x1, y1, x2, y2, x3, y3, x4, y4, h, phi, chi, omg;
  float tdel, p1, p2, p3, p4, x5, y5;
  if (type == 100 ) {
    h = ((int) elev % 1000 ) * 0.36;
    tdel = 8.0;
    p1 = 85.0; p2 = 10.0; p3 = 35.0;
  } else if (type == 1000) {
    h = elev * 0.036;
    tdel = 12.0;
    p1 = 70.0; p2 = 10.0; p3 = 25.0;
  } else if (type == 10000) {
    h = elev * 0.0036;
    tdel = 6.0;
    p1 = 94.0; p2 = 5.0; p3 = 96.0; p4 = 84.0;
  }

  phi = radians(h);  chi = radians(h+tdel);  omg = radians(h-tdel);
  x1= p1*sin(phi)+clockCenterX;  y1= -p1*cos(phi)+clockCenterY;
  x2= p2*sin(phi)+clockCenterX;  y2= -p2*cos(phi)+clockCenterY;
  x3= p3*sin(chi)+clockCenterX;  y3= -p3*cos(chi)+clockCenterY;
  x4= p3*sin(omg)+clockCenterX;  y4= -p3*cos(omg)+clockCenterY;
  if ( type == 10000 )
  {

    x5= p4*sin(phi)+clockCenterX;  y5=-p4*cos(phi)+clockCenterY;

    tft.drawLine(x1, y1, x2, y2, WHITE);
    tft.fillTriangle(x3, y3, x5, y5, x4, y4, WHITE);                        // Use this for up and down buttons
    tft.fillTriangle(10, 230, 60, 230, 35, 280, BLUE);                      // Push button triangle UP. What a pain!
    tft.fillTriangle(180, 280, 230, 280, 205, 230, BLUE);                   // Push button triangle DOWN. Yup, more pain
  } else {

   tft.drawLine(x1, y1, x3, y3, WHITE);            // 100's needle
   tft.drawLine(x3, y3, x2, y2, WHITE);            // 100's needle
   tft.drawLine(x2, y2, x4, y4, WHITE);            // 1000's needle
   tft.drawLine(x4, y4, x1, y1, WHITE);            // 1000's needle

  }

}

#define BUTTON_UP_X 180
#define BUTTON_UP_Y 280
#define BUTTON_UP_W 60
#define BUTTON_UP_H 60

#define BUTTON_DOWN_X 5
#define BUTTON_DOWN_Y 230
#define BUTTON_DOWN_W 60
#define BUTTON_DOWN_H 60

//Variables

float H0 = 0.0, OldB;

void loop(void)

{

     float QNH = 33.8639 * B ;                                // Adjusts for sensor?
     float T = bmp.readTemperature()* 9/5 + 32;               //TEMP CONVERTED TO F (* 9/5 + 32)
     float P = 0.01* bmp.readPressure();
     float H = round(3.28*bmp.readAltitude(QNH)) ;
     float PA = H + 27.0 * (1013.00 - QNH);
     float DA = round((1.2376 * PA) + ( 118.8 * T ) - 1782.0) ;

     if ( B != OldB || H < (H0 - 1.0) || H > (H0 + 1.0) ) {     

      if ( B != OldB ) {            // If "B" is not equal to OldB...

       drawDisplay(B);

      } else {

     // first clear hands of clock and re-draw clock numbers, then hands

        tft.fillCircle(clockCenterX, clockCenterY, 88, TFT_BLACK);

        for (int i=0; i<3; i++)

        {

           tft.drawCircle(clockCenterX, clockCenterY, i, TFT_GREEN);

        }

        drawNumbs();

      }

      drawit(H, 100);      // 100's arrow
      drawit(H, 1000);     // 1000's arrow
      drawit(H, 10000);    // 10,000 arrow & line

      H0 = H;

     }

     tft.setTextColor(YELLOW,BLACK); tft.setTextSize(3);       //FONT SIZE 1,2...
     tft.setCursor(85, 150); tft.println(H, 0);                // POSITION AWAY FROM EDGE & BELOW INFO & ALTITUDE DIGITAL READOUT
     tft.setCursor(110, 300); tft.println(T, 1);               // Temperature Readout position

     OldB = B;

     bool down = Touch_getXY();  // See 6 lines down, "down && UP", this is needed for the if (... 

                                  // This touch screen is (X) 320 by (Y) 240
                                                          
     bool UP = ( (pixel_x >= 140 && pixel_x <= 240) &&    // Approx where X for UP arrow is, between 160 & 240

                (  pixel_y >= 220 && pixel_y <= 310) );   // Approx where Y for UP arrow is, between 220 & 280

     bool DN = ( (pixel_x >= 5 && pixel_x <= 100) &&       // Approx where X for DN arrow is

                (  pixel_y >= 200 && pixel_y <= 310) );   // Approx where Y for DN arrow is

     delay(100);

if ( down  && UP )                                // if UP arrow position...
{
       Serial.println("Holy Smokes... UP");       // Will print this in serial monitor if pressed is working
       B = B + .01;                               // Takes the value of "B" and adds .01 to it.
    }

if ( down  && DN )                                // if DN arrow position...
{
       Serial.println("Holy Smokes... Hit DN");   // Will print this in serial monitor if pressed is working
       B = B - .01;                               // Takes the value of "B" and subtracts .01 from it
    }

          pinMode(YP, OUTPUT);      //restore shared pins (I don't know what this does)
          pinMode(XM, OUTPUT);
          digitalWrite(YP, HIGH);   //because TFT control pins  (or this)
          digitalWrite(XM, HIGH);

       }
© 2021 GitHub, Inc.
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
API
Training
Blog
About
