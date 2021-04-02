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
int WAIT_TIME = 5000;
int QUICK = 400;
int buf[478];
int x, x2;
int y, y2;
int r;
void setup() {

  // put your setup code here, to run once:
  randomSeed(analogRead(1));
  tft.init(LANDSCAPE);      //  landscape mode
  tft.setFont(SmallFont);
  tft.setTransparency(ON);
  TestScreen();
}

void loop() {
  //                            *******************       Text

  DrawTrigStuff();
  MovingSineWave();
  LinesTest();
  RandomCircles();
  CirclesFilled();
  RectFilled();
  RandomTriangles();
  RoundRectFilled();
  RandomLines();
  RandomDots();
  SayHello();
  TestGraphics();
  TestText();
}

void SetBanner() {
  tft.clrDisplay();
  tft.setFont(SmallFont);
  tft.setTransparency(ON);
  tft.setColour(BLUE);
  tft.fillRect( 0, 0, 479, 13);
  tft.setColour(GREY);
  tft.fillRect( 0, 306, 479, 319);
  tft.drawRect(0, 14, 479, 305);
  tft.printString("ETMS - CapSoft Ltd. MiUAIG BIOS (c) 2020", 100, 1, RED);
  tft.printString("Main Information Page - MiUAIG BIOS (c) 2020-2021", 40, 307, BLUE);
}

void TestScreen() {
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
}

void TestText() {
  tft.clrDisplay();
  tft.setFont(SmallFont);
  tft.setTransparency(ON);
  tft.printString(" !\"#$%&'()*+,-./0123456789:;<=>?", 10, 70, RED);
  tft.printString("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", 10, 82, BLUE);
  tft.printString("`abcdefghijklmnopqrstuvwxyz{|}~ ", 10, 94, GREEN);
  tft.setFont(Arial);
  tft.printString(" !\"#$%&'()*+,-./", 10, 140, YELLOW);
  tft.printString("0123456789:;<=>?", 10, 160, YELLOW);
  tft.printString("@ABCDEFGHIJKLM", 10, 180, MAGENTA);
  tft.printString("NOPQRSTUVWXYZ[\\]^_", 10, 200, MAGENTA);
  tft.printString("`abcdefghijklm", 10, 220, CYAN);
  tft.printString("nopqrstuvwxyz{|}~ ", 10, 220, CYAN);
  delay(WAIT_TIME);
}
void SayHello() {
  tft.clrDisplay();
  tft.setFont(SmallFont);
  tft.printString("0000000000111111111122222222223333333333444444444455555555556", 0, 0, WHITE);
  tft.printString("0123456789012345678901234567890123456789012345678901234567890", 0, 12, WHITE);
  tft.printString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 0, 36, RED);
  tft.printString("HELLO OUT THERE!", 1, 48, RED);
  tft.drawRect(0, 0, 479, 319, GREEN);
  delay(WAIT_TIME);
  delay(WAIT_TIME);
}
void DrawTrigStuff() {
  // Draw crosshairs
  SetBanner();
  tft.setColour(BLUE);
  tft.setBackColour(BLACK);
  tft.drawLine(239, 15, 239, 304, GREEN);
  tft.drawLine(1, 159, 478, 159);
  for (int i = 9; i < 470; i += 10)
    tft.drawLine(i, 157, i, 161);
  for (int i = 19; i < 220; i += 10)
    tft.drawLine(237, i, 241, i);

  // Draw sin-, cos- and tan-lines
  tft.setColour(RED);
  tft.printString("Sin", 5, 15, RED);
  for (int i = 1; i < 478; i++)
  {
    tft.drawPixel(i, 159 + (sin(((i * 1.13) * 3.14) / 180) * 95));
  }

  tft.setColour(GREEN);
  tft.printString("Cos", 5, 27, GREEN);
  for (int i = 1; i < 478; i++)
  {
    tft.drawPixel(i, 159 + (cos(((i * 1.13) * 3.14) / 180) * 95));
  }

  tft.setColour(BLUE);
  tft.printString("Tan", 5, 39, BLUE);
  for (int i = 1; i < 478; i++)
  {
    tft.drawPixel(i, 159 + (tan(((i * 1.13) * 3.14) / 180)));
  }
  delay(WAIT_TIME);
}
void TestGraphics() {
  SetBanner();
  tft.drawHLine(20, 20, 100, WHITE);
  tft.drawVLine(20, 20, 100, WHITE);
  tft.drawLine(20, 20, 200, 200, MAGENTA);
  tft.drawCircle(200, 200, 20, MAGENTA);
  delay(WAIT_TIME);
  SetBanner();
  tft.drawCircle(tft.getWidth() / 2, tft.getHeight() / 2, 8, WHITE);
  tft.drawCircle(tft.getWidth() / 2, tft.getHeight() / 2, 9, WHITE);
  tft.drawCircle(tft.getWidth() / 2, tft.getHeight() / 2, 10, WHITE);
  tft.drawCircle(tft.getWidth() / 2, tft.getHeight() / 2, 14, WHITE);
  delay(WAIT_TIME);
  SetBanner();
  tft.drawCircle(150, 150, 20, BLUE);
  tft.drawCircle(30, 50, 20, GREEN);
  tft.drawCircle(70, 50, 20, GREEN);
  tft.drawCircle(110, 50, 20, GREEN);
  delay(WAIT_TIME);
  SetBanner();
  tft.drawCircle(200, 200, 50, RED);
  tft.fillCircle(100, 100, 50, RED);
  tft.drawRect(80, 60, 45, 45, CYAN);
  tft.fillRect(20, 20, 20, 20, RED);
  delay(WAIT_TIME);
  SetBanner();
  tft.drawRoundRect(80, 60, 100, 100, BLUE);
  tft.fillRoundRect(100, 120, 40, 60, YELLOW);
  delay(WAIT_TIME);
}
void RandomCircles() {
  SetBanner();
  for (int i = 0; i < 80; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawCircle(32 + random(416), 45 + random(226), random(40));
  }
  delay(WAIT_TIME);
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
//                          ************************      Draw some random triangles
void RandomTriangles() {
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawTriangle(random(416), random(216), random(416), random(216), random(416), random(216));
  }
  delay(WAIT_TIME);
}
//                            ***********************       Draw some random rounded filled rectangles
void RoundRectFilled() {
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillRoundRect(random(260), random(160) + 20, random(260), random(160));
  }
  delay(WAIT_TIME);
}
//                            ***********************       Draw some random filled rectangles
void RectFilled() {
  SetBanner();
  for (int i = 0; i < 20; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.fillRect(random(260), random(160), random(260), random(160));
  }
  delay(WAIT_TIME);
}
void LinesTest() {
  //                    **************************      Draw some lines in a pattern
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
  delay(WAIT_TIME);
}

void RandomLines() {
  SetBanner();
  for (int i = 0; i < 100; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    x = 2 + random(476);
    y = 16 + random(289);
    x2 = 2 + random(476);
    y2 = 16 + random(289);
    tft.drawLine(x, y, x2, y2);
  }
  delay(WAIT_TIME);
}
void RandomDots() {
  SetBanner();
  for (int i = 0; i < 20000; i++)
  {
    tft.setColour(random(255), random(255), random(255));
    tft.drawPixel(2 + random(476), 16 + random(289));
  }
  delay(WAIT_TIME);
}
void MovingSineWave() {
  SetBanner();
  // Draw a moving sinewave
  int x = 1;
  for (int i = 1; i < (478 * 15); i++)
  {
    x++;
    if (x == 479)
      x = 1;
    if (i > 479)
    {
      if ((x == 239) || (buf[x - 1] == 159))
        tft.setColour(0, 0, 255);
      else
        tft.setColour(0, 0, 0);
      tft.drawPixel(x, buf[x - 1]);
    }
    tft.setColour(0, 255, 255);
    y = 159 + (sin(((i * 0.7) * 3.14) / 180) * (90 - (i / 100)));
    tft.drawPixel(x, y);
    buf[x - 1] = y;
  }

  delay(2000);
}
