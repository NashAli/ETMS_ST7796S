/*
  Created:      3:06 PM 10/05/2020
  Last Updated: 7:26 AM 11/19/2020
  MIT License

  Copyright (c) 2020 Zulfikar Naushad(Nash) Ali

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  Uses the Arduino DUE ONLY!
  NOTES:
  This program tests the new ETMS library for the generic 3.95 parallel i/f ST7796S
  tft colour display controller in 16-bit mode.
*/



#include <ETMS_ST7796S.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t Arial[];
ETMS_ST7796S tft;
int WAIT_TIME = 2000;
int QUICK = 500;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(1));
  tft.init(LANDSCAPE);      //  landscape mode
  SetBanner();
  tft.setFont(SmallFont);
  tft.printString("HELLO OUT THERE!", 20, 20, RED);
  delay(WAIT_TIME);
}

void loop() {
  // put your main code here, to run repeatedly:
  ColourTest();
  RectTest();
  CircleTest();
  LinesTest();

  //            **********************      Draw some random outline circles
  delay(WAIT_TIME);
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawCircle(32 + random(416), 45 + random(226), random(40));
  }
  //            ************************      Draw some random filled circles
  delay(WAIT_TIME);
  SetBanner();
  for (int i = 0; i < 175; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillCircle(32 + random(416), 45 + random(226), random(20));
  }
  //                          ************************      Draw some random triangles
  delay(WAIT_TIME);
  SetBanner();

  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawTriangle(random(416), random(216), random(416), random(216), random(416), random(216));
  }
  //                    ******************************      Draw some random outline rectangles
  delay(WAIT_TIME);
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawRect(random(300), random(160), random(300), random(160));
  }
  //                        ***********************************     random filled rectangles
  delay(WAIT_TIME);
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillRect(random(200), random(80), random(200), random(80));
  }
  CirclesFilled();
  RoundRectanglesFilled();
  TextTest();
}

void SetBanner() {
  tft.clrDisplay();
  tft.setColour(BLUE);
  tft.fillRect( 0, 0, 479, 13);
  tft.setColour(GREY);
  tft.fillRect( 0, 306, 479, 319);
  tft.setFgBgColour(WHITE, BLACK);
  tft.setColour(BLUE);
  tft.drawRect(0, 14, 479, 305);
}

void CirclesFilled() {
  //            ************************      Draw some random filled circles

  SetBanner();
  for (int i = 0; i < 175; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillCircle(32 + random(416), 45 + random(226), random(20));
  }
  delay(WAIT_TIME);
}


void RoundRectanglesFilled() {
  //                            ***********************       Draw some random rounded filled rectangles
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillRoundRect(random(260), random(160), random(260), random(160));
  }
  delay(WAIT_TIME);
}
void TextTest() {
  //                            *******************       Text
  SetBanner();
  tft.setFgBgColour(YELLOW, BLACK);
  tft.setFont(SmallFont);
  tft.printString(" !\"#$%&'()*+,-./0123456789:;<=>?", 20, 70, RED);
  tft.printString("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", 20, 82, BLUE);
  tft.printString("`abcdefghijklmnopqrstuvwxyz{|}~ ", CENTER, 94, YELLOW);
  tft.setFont(Arial);
  tft.printString(" !\"#$%&'()*+,-./0123456789:;<=>?", CENTER, 170, RED);
  tft.printString("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", CENTER, 192, BLUE);
  tft.printString("`abcdefghijklmnopqrstuvwxyz{|}~ ", CENTER, 214, YELLOW);
  delay(WAIT_TIME);

}
void ColourTest() {
  tft.fillScreen(WHITE); delay(QUICK);
  tft.fillScreen(LTGREY); delay(QUICK);
  tft.fillScreen(GREY); delay(QUICK);
  tft.fillScreen(DKGREY); delay(QUICK);
  tft.fillScreen(BLACK); delay(QUICK);
  tft.fillScreen(RED); delay(QUICK);
  tft.fillScreen(GREEN); delay(QUICK);
  tft.fillScreen(BLUE); delay(QUICK);
  tft.fillScreen(YELLOW); delay(QUICK);
  tft.fillScreen(MAGENTA); delay(QUICK);
  tft.fillScreen(CYAN); delay(QUICK);
  delay(WAIT_TIME);
}
void RectTest() {
  //                    ******************************      Draw some filled rectangles
  int buf[478];
  int x, x2;
  int y, y2;
  int r;
  SetBanner();
  for (int i = 1; i < 6; i++)
  {
    switch (i)
    {
      case 1:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 2:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 3:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 4:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 5:
        tft.setColour(random(255), random(255), random(255));
        break;
    }
    tft.fillRect(4 + (i * 15), 4 + (i * 20), 50 + (i * 20), 30 + (i * 20));
  }

  delay(WAIT_TIME);
}
void CircleTest() {
  SetBanner();

  //                    *******************       Draw some filled circles
  for (int i = 1; i < 6; i++)
  {
    switch (i)
    {
      case 1:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 2:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 3:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 4:
        tft.setColour(random(255), random(255), random(255));
        break;
      case 5:
        tft.setColour(random(255), random(255), random(255));
        break;
    }
    tft.fillCircle(180 + (i * 20), 100 + (i * 20), 30);
  }

}
void LinesTest() {
  //                    **************************      Draw some lines in a pattern
  delay(WAIT_TIME);
  SetBanner();
  tft.setColour (RED);
  for (int i = 15; i < 304; i += 5)
  {
    tft.drawLine(1, i, (i * 1.6) - 10, 304);
  }
  tft.setColour (YELLOW);
  for (int i = 304; i > 15; i -= 5)
  {
    tft.drawLine(478, i, (i * 1.6) - 11, 15);
  }
  tft.setColour (GREEN);
  for (int i = 304; i > 15; i -= 5)
  {
    tft.drawLine(1, i, 491 - (i * 1.6), 15);
  }
  tft.setColour (BLUE);
  for (int i = 15; i < 304; i += 5)
  {
    tft.drawLine(478, i, 490 - (i * 1.6), 304);
  }
}
