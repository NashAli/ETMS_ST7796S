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
int QUICK = 400;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(1));
  tft.init(LANDSCAPE);      //  landscape mode
  SetBanner();
  SayHello();
}

void loop() {
  ScreenTest();
  //                            *******************       Text
  SetBanner();
  TextTest();
  delay(WAIT_TIME);
  tft.clrDisplay();
  tft.drawChar(21, 100, 100);
  delay(WAIT_TIME);
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
void ScreenTest() {
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
void TextTest() {
  tft.setFgBgColour(YELLOW, BLACK);
  tft.setFont(SmallFont);
  tft.printString(" !\"#$%&'()*+,-./0123456789:;<=>?", 20, 70, RED);
  tft.printString("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", 20, 82, BLUE);
  tft.printString("`abcdefghijklmnopqrstuvwxyz{|}~ ", CENTER, 94, YELLOW);
  tft.setFont(Arial);
  tft.printString(" !\"#$%&'()*+,-./0123456789:;<=>?", CENTER, 170, RED);
  tft.printString("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", CENTER, 192, BLUE);
  tft.printString("`abcdefghijklmnopqrstuvwxyz{|}~ ", CENTER, 214, YELLOW);

}
void SayHello() {
  tft.setFont(SmallFont);
  tft.printString("HELLO OUT THERE!", 20, 70, RED);
  delay(WAIT_TIME);
}
