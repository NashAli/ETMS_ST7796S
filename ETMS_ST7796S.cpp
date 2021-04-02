/*
  ETMS_ST7796S.cpp
 	  Created:      03:06 PM 10/05/2020
  	Last Updated: 12:01 PM 11/16/2020
  	MIT License

  	Copyright (c) 2020 Zulfikar Naushad(Nash) Ali

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and the associated documentation files (the "Software"), to deal
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
  
  NOTES:
	Uses the Arduino DUE ONLY! (has not been tested with any other product) 
  	This program tests the new ETMS library for the generic 3.95 parallel i/f ST7796S
  	tft colour display controller in a 16-bit HW config. See docs.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
 
  This will include the Header File so that the Source File has access
  to the function definitions in the ETMS_ST7796S library.
*/

#include "ETMS_ST7796S.h" 
/*
  The #include of Arduino.h gives this library access to the standard
	Arduino types and constants (HIGH, digitalWrite, etc.). It's 
  unneccesary for sketches but required for libraries as they're not
	.ino (Arduino) files. 
*/
#include "Fonts.c"
#include "Arduino.h"
#define libver	100

// Pin shield control macros, this is like digitalWrite

#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

// configure masks for the control pins

#define ReadBitMask 0x100000;   //  PA20
#define WriteBitMask 0x80;      //  PC7
#define RegSelBitMask 0x40;     //  PC6
#define ChipSelBitMask 0x100;   //  PC8
#define ResetBitMask  0x200;    //  PC9


// configure macros for the 16 bit data bus @ pins 22 - 37

#define AMASK         ((1<<7)|(3<<14))          //PA7, PA14-PA15
#define BMASK         (1<<26)                   //PB26
#define CMASK         (31<<1)                   //PC1-PC5
#define DMASK         ((15<<0)|(1<<6)|(3<<9))   //PD0-PD3, PD6, PD9-PD10

#define write16(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; PIOD->PIO_CODR = DMASK; PIOA->PIO_SODR = (((x)&(1<<6))<<1)|(((x)&(3<<9))<<5); \
                       PIOB->PIO_SODR = (((x)&(1<<8))<<18); PIOC->PIO_SODR = (((x)&(1<<0))<<5); PIOC->PIO_SODR = (((x)&(1<<1))<<3); PIOC->PIO_SODR = (((x)&(1<<2))<<1); \
                       PIOC->PIO_SODR = (((x)&(1<<3))>>1); PIOC->PIO_SODR = (((x)&(1<<4))>>3); PIOD->PIO_SODR = (((x)&(1<<7))<<2)|(((x)&(1<<5))<<5)|(((x)&(15<<11))>>11)|(((x)&(1<<15))>>9);}

#define read16(dst) { dst=(0|((PIOC->PIO_PDSR & (1<<5))>>5)|((PIOC->PIO_PDSR & (1<<4))>>3)|((PIOC->PIO_PDSR & (1<<3))>>1)|((PIOC->PIO_PDSR & (1<<2))<<1)|((PIOC->PIO_PDSR & (1<<1))<<3)|((PIOD->PIO_PDSR & (1<<10))>>5)\
                      |((PIOA->PIO_PDSR & (1<<7))>>1)|((PIOD->PIO_PDSR & (1<<9))>>2)|((PIOB->PIO_PDSR & (1<<26))>>18)|((PIOA->PIO_PDSR & (3<<14))>>5)|((PIOD->PIO_PDSR & (15<<0))<<11)|((PIOD->PIO_PDSR & (1<<6))<<9));}

#define write8(x)   write16(x & 0xFF)
#define read8(dst)  {read16(dst);uint8_t dst &= 0x0FF;}

#define _tft_width 480
#define _tft_height 320

//#define fontbyte(x) pgm_read_byte(&cfont.font[x])
#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) cfont.font[x]  

#define pgm_read_word(data) *data
#define pgm_read_byte(data) *data



/*
  Source Code of the ETMS_ST7796S class for all constructors and functions that are part of this class. 
  Constructor to set ports configuration. 
*/

ETMS_ST7796S::ETMS_ST7796S() {
  SetOutputs();
}

void ETMS_ST7796S::SetOutputs(){
  PIOA->PIO_PER = 0x14C080;
  PIOA->PIO_OER = 0x14C080;
  PIOB->PIO_PER = 0x4000000;
  PIOB->PIO_OER = 0x4000000;
  PIOC->PIO_PER = 0x803FE;
  PIOC->PIO_OER = 0x803FE;
  PIOD->PIO_PER = 0x064F;
  PIOD->PIO_OER = 0x064F;
  PIOC->PIO_SODR = 0x3C0;        //  set the wr,reset,cd,cs pins high - inactive. port pins PC6 - PC9.
  PIOA->PIO_SODR = 0x100000;     //  set the rd pin high - inactive. port pin PA20.
}

void ETMS_ST7796S::Strobe_Write_Pin(){
  PIOC->PIO_CODR = WriteBitMask;    //  - Due port - PC07 mask
  PIOC->PIO_SODR = WriteBitMask;
}

void ETMS_ST7796S::Strobe_Read_Pin(){
  PIOA->PIO_CODR = ReadBitMask;    //  - Due port - PA20 mask
  PIOA->PIO_SODR = ReadBitMask;
}

void ETMS_ST7796S::Writ_Bus8(uint8_t val) {
  write8(val);                  //  put 8-bit data/command on bus.
  Strobe_Write_Pin();
}

void ETMS_ST7796S::Writ_Bus16(uint16_t val) {
  write16(val);                 //  put 16-bit data on bus.
  Strobe_Write_Pin();
}	
/*
uint8_t ETMS_ST7796S::Read_Bus8(){
  PIOC->PIO_CODR |= ChipSelBitMask;   //  LCD_CS=0 pin D43 Due port A20
  Strobe_Read_Pin();
  read8(uint8_t dst);
  PIOC->PIO_SODR |= ChipSelBitMask;
}

uint16_t ETMS_ST7796S::Read_Bus16(){
  PIOC->PIO_CODR |= ChipSelBitMask;   //  LCD_CS=0 pin D43 Due port A20
  Strobe_Read_Pin();
  read16(uint16_t dst);
  PIOC->PIO_SODR |= ChipSelBitMask;
}
*/
void ETMS_ST7796S::Lcd_Dummy_Read() {
  PIOC->PIO_CODR |= ChipSelBitMask;   //  LCD_CS=0 pin D43 Due port A20
  Strobe_Read_Pin();
  PIOC->PIO_SODR |= ChipSelBitMask;
}

void ETMS_ST7796S::Lcd_Write_Com(uint8_t com8) {  
  PIOC->PIO_CODR |= ChipSelBitMask;  //  LCD_CS low
  PIOC->PIO_CODR |= RegSelBitMask;   //  LCD_CD=0 pin D38 Due port C6 cd=0 command
  Writ_Bus8(com8); 
  PIOC->PIO_SODR |= ChipSelBitMask;  // CS high
}

void ETMS_ST7796S::Lcd_Write_Data8(uint8_t dat8) {
  PIOC->PIO_CODR |= ChipSelBitMask;      //  LCD_CS low
  PIOC->PIO_SODR |= RegSelBitMask;       //  LCD_CD=1 pin D38 Due port PC6 cd=1 data
  Writ_Bus8(dat8);                      //  write to bus
  PIOC->PIO_SODR |= ChipSelBitMask;      //  LCD_CS high
}

void ETMS_ST7796S::Lcd_Write_Data16(uint16_t dat16) {   
  PIOC->PIO_CODR |= ChipSelBitMask;      //  LCD_CS LOW
  PIOC->PIO_SODR |= RegSelBitMask;       //  LCD_CD=1 pin D38 Due port PC6 cd=1 data
  Writ_Bus16(dat16);                    //  write to bus D0-D15
  PIOC->PIO_SODR |= ChipSelBitMask;      //  LCD_CS HIGH
          
}

void ETMS_ST7796S::Convert_Float(char *buf, double num, int width, byte prec){
	char format[10];
	
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}

void ETMS_ST7796S::Lcd_Write_Com_Data(uint8_t com8,uint8_t dat8) {
  Lcd_Write_Com(com8);
  Lcd_Write_Data8(dat8);
}

void ETMS_ST7796S::init(boolean orientation) {
  PIOC->PIO_CODR |= ResetBitMask;  //display RESET low
  delay(20);
  PIOC->PIO_SODR |= ResetBitMask;  //display RESET high
  delay(120);
  //	the chip initialization - sw reset.
  Lcd_Write_Com(SWRESET);
  delay(50);
  Lcd_Write_Com(SLPOUT);                 //  sleep off
  delay(120);
  
  Lcd_Write_Com_Data(MADCTL, MADCTL_MV |MADCTL_MX|MADCTL_MY | MADCTL_RGB);   //  Memory Data access control default values top-left to bottom-right - 0x68 - UTFT 0x48 other
  Lcd_Write_Com_Data(COLMOD, RGB565);                               //  16-bit bus 16-bit 565colour  result - 1 transfer per pixel. 153.6 kWORD per screen
  Lcd_Write_Com_Data(IFMODE, 0x80);                                 //  interface control was 0x00
  
  Lcd_Write_Com_Data(CSCON, 0xC3);        //  set Command control on
  Lcd_Write_Com_Data(CSCON, 0x96);
 
  Lcd_Write_Com_Data(DFC, 0x20);
  Lcd_Write_Data8(0x02);  
  
  Lcd_Write_Com(BPC);                     //  blanking porch control
  Lcd_Write_Data8(0x02);
  Lcd_Write_Data8(0x03);
  Lcd_Write_Data8(0x00);
  Lcd_Write_Data8(0x04);
  
  Lcd_Write_Com_Data(FRMCTR1, 0x80);
  Lcd_Write_Data8(0x10);  

  Lcd_Write_Com_Data(DIC, 0x00);          //  display inversion control
  Lcd_Write_Com_Data(EM, 0xC6);           //  Entry mode
  Lcd_Write_Com_Data(VCMPCTL, 0x24);
  //Lcd_Write_Com_Data(0xE4, 0x31);       //  unknown command ??
  
  Lcd_Write_Com(DOCA);
  Lcd_Write_Data8(0x40);
  Lcd_Write_Data8(0x8A);
  Lcd_Write_Data8(0x00);
  Lcd_Write_Data8(0x00);
  Lcd_Write_Data8(0x29);
  Lcd_Write_Data8(0x19);
  Lcd_Write_Data8(0xA5);
  Lcd_Write_Data8(0x33);
  
  Lcd_Write_Com(PWR1);
  Lcd_Write_Data8(0x80);
  Lcd_Write_Data8(0x51);

  //Lcd_Write_Com_Data(PWR2, 0x19);

  Lcd_Write_Com_Data(PWR3,0xA7);
  
  Lcd_Write_Com(PGC);	//	positive gamma
  Lcd_Write_Data8(0xF0);
  Lcd_Write_Data8(0x09);
  Lcd_Write_Data8(0x13);
  Lcd_Write_Data8(0x12);
  Lcd_Write_Data8(0x12);
  Lcd_Write_Data8(0x2B);
  Lcd_Write_Data8(0x3C);
  Lcd_Write_Data8(0x44);
  Lcd_Write_Data8(0x4B);
  Lcd_Write_Data8(0x1B);
  Lcd_Write_Data8(0x18);
  Lcd_Write_Data8(0x17);
  Lcd_Write_Data8(0x1D);
  Lcd_Write_Data8(0x21);

  Lcd_Write_Com(NGC);	//	negative gamma
  Lcd_Write_Data8(0xF0);
  Lcd_Write_Data8(0x09);
  Lcd_Write_Data8(0x13);
  Lcd_Write_Data8(0x0C);
  Lcd_Write_Data8(0x0D);
  Lcd_Write_Data8(0x27);
  Lcd_Write_Data8(0x3B);
  Lcd_Write_Data8(0x44);
  Lcd_Write_Data8(0x4D);
  Lcd_Write_Data8(0x0B);
  Lcd_Write_Data8(0x17);
  Lcd_Write_Data8(0x17);
  Lcd_Write_Data8(0x1D);
  Lcd_Write_Data8(0x21);

  Lcd_Write_Com_Data(CSCON, 0x3C);   //  set command control off
  Lcd_Write_Com_Data(CSCON, 0x69);
  delay(120);             //  wait a sec (actually 120mSec)
  Lcd_Write_Com(NORON);   //  normal ON
  Lcd_Write_Com(SLPOUT);	//	sleep off
  Lcd_Write_Com(DISPON);	//	display on

  orient = orientation;
  cfont.font = 0;
}

void ETMS_ST7796S::displayOn() {
	Lcd_Write_Com(SLPOUT);
	Lcd_Write_Com(DISPON);
}

void ETMS_ST7796S::displayOff() {
	Lcd_Write_Com(SLPIN);
	Lcd_Write_Com(DISPOFF);
}

void ETMS_ST7796S::setBrightness(uint8_t br) {
	Lcd_Write_Com_Data(WRDISBV, br);
}

uint8_t ETMS_ST7796S::getID(){

}
uint16_t ETMS_ST7796S::getHeight(){
return 320;
}
uint16_t ETMS_ST7796S::getDisplayYSize(){
return 320;
}
uint16_t ETMS_ST7796S::getWidth(){
return 480;
}
uint16_t ETMS_ST7796S::getDisplayXSize(){
return 480;
}
/*
  First convert RGB888 to 16-bit colour and store to system variable "_colour565"
  this variable allows for repeated writes to the frame buffer of the ST7796S.
*/
uint16_t ETMS_ST7796S::to565Colour(uint8_t red, uint8_t green, uint8_t blue){
  uint16_t b = (blue >> 3) & 0x1f;
  uint16_t g = ((green >> 2) & 0x3f) << 5;
  uint16_t r = ((red >> 3) & 0x1f) << 11;
  _colour565 = (r | g | b);
  return _colour565;
}

// write a 16bit 565 colour pixel to frame buffer, if no parameters then uses last colour selected.
void ETMS_ST7796S::setPixel(){
  Lcd_Write_Data16(_colour565);      // rrrrrggg gggbbbbb
}
void ETMS_ST7796S::setPixel(uint16_t colour){
  Lcd_Write_Data16(colour);      // rrrrrggg gggbbbbb
}
void ETMS_ST7796S::setPixel(uint8_t red, uint8_t green, uint8_t blue){
  uint16_t pcolour565 = to565Colour(red,green,blue);
  Lcd_Write_Data16(pcolour565);      // rrrrrggg gggbbbbb
}

/*
  invert display - needs clarifying.
*/
void ETMS_ST7796S::invertDisplay(uint8_t inv) {
	Lcd_Write_Com_Data(DIC,inv);
	Lcd_Write_Com_Data(DIC,inv);
}
void ETMS_ST7796S::setTransparency(bool t) {
	_TRANSPARENT = t;
}
/*
  This is the same as the setxy routine found in most libraries. Sets the display address window boundaries. Normally
  portrait or landscape would affect this function, but only landscape is supported by this library (V1.0) for now.
*/
void ETMS_ST7796S::setAddressWindow(uint16_t xstartaddress, uint16_t ystartaddress, uint16_t w, uint16_t h) {
  	
  	uint16_t x2 = xstartaddress + w;		//garranties x2 > xstartaddress
  	uint16_t y2 = ystartaddress + h;
  
  	Lcd_Write_Com(CASET); 
	Lcd_Write_Data8(xstartaddress>>8);
	Lcd_Write_Data8(xstartaddress);
	Lcd_Write_Data8(x2>>8);
	Lcd_Write_Data8(x2);
	Lcd_Write_Com(RASET); 
	Lcd_Write_Data8(ystartaddress>>8);
	Lcd_Write_Data8(ystartaddress);
	Lcd_Write_Data8(y2>>8);
	Lcd_Write_Data8(y2);
	Lcd_Write_Com(RAMWR); 
}
//	for compatability...
void ETMS_ST7796S::setXY(uint16_t xstartaddress, uint16_t ystartaddress, uint16_t w, uint16_t h){
	setAddressWindow( xstartaddress, ystartaddress, w, h);
}
void ETMS_ST7796S::clrXY()
{
	if (orient==LANDSCAPE)
		setAddressWindow(0,0,_tft_height-1,_tft_width-1);
	else
		setAddressWindow(0,0,_tft_width-1,_tft_height-1);
}

void ETMS_ST7796S::fillWindow(uint16_t colour, int length= 153600) {
	_colour565 = colour;
  Lcd_Write_Com(RAMWRC);
for (int i = 0; i <= length; i++) {
   Lcd_Write_Data16(_colour565);
  }
}
void ETMS_ST7796S::fastFill(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height){
	int length = width * height;
  	Lcd_Write_Com(RAMWRC);
for (int i = 0; i <= length; i++) {
   	Lcd_Write_Data16(_colour565);
  }
}
void ETMS_ST7796S::fillScreen(){
  	int len = _tft_height*_tft_width;
 	setAddressWindow(0, 0, _tft_width-1, _tft_height-1);
	fillWindow(_colour565, len);
}
void ETMS_ST7796S::fillScreen(uint16_t colour){
	int len = _tft_height*_tft_width;
	setAddressWindow(0, 0, _tft_width-1, _tft_height-1);
	fillWindow(colour, len);
}
void ETMS_ST7796S::fillScreen(uint8_t red, uint8_t green, uint8_t blue){
	_colour565 = to565Colour(red,green,blue);
	fillScreen(_colour565);
}
void ETMS_ST7796S::clrDisplay() {
  	fillScreen(BLACK);
}
void ETMS_ST7796S::clrScr(){
	clrDisplay();
}

void ETMS_ST7796S::drawPixel(uint16_t xpos, uint16_t ypos) {
	// out of bounds check
	if ((xpos < 0) || (ypos < 0) || (xpos >= _tft_width) || (ypos >= _tft_height))
    return;
	// all good then draw a pixel with RGB565 here
	setAddressWindow(xpos, ypos, xpos, ypos);
	setPixel(_colour565);
}
void ETMS_ST7796S::drawPixel(uint16_t xpos, uint16_t ypos, uint16_t fcolour) {
	_colour565 = fcolour;
	drawPixel(xpos,ypos);
}
void ETMS_ST7796S::drawPixel(uint16_t xpos, uint16_t ypos, uint8_t red, uint8_t green, uint8_t blue) {
	_colour565 = to565Colour(red,green,blue);    //    to565Colour uses private variable - '_colour565'
	drawPixel(xpos,ypos);
}

/*
set foreground and background colours
*/
void ETMS_ST7796S::setColour(uint16_t colour){
  _colour565 = colour;
}
void ETMS_ST7796S::setColour(uint8_t red, uint8_t green, uint8_t blue){
 _colour565 = to565Colour(red, green, blue);
}

void ETMS_ST7796S::setBackColour(uint16_t colour){
  _backcolour565 = colour;
  _TRANSPARENT = false;
}
void ETMS_ST7796S::setBackColour(uint8_t red, uint8_t green, uint8_t blue){
  _backcolour565 = to565Colour(red, green, blue);
 _TRANSPARENT = false;
}
void ETMS_ST7796S::setFgBgColour(uint16_t fcolour,uint16_t bcolour){
  _colour565 = fcolour;
  _backcolour565 = bcolour;
  _TRANSPARENT = false;
}
/*
  Basic Graphic Primitives  **************************************************************
*/
//	Draw a line..
void ETMS_ST7796S::drawLine(uint16_t xstartaddress, uint16_t ystartaddress, uint16_t x2, uint16_t y2){
uint16_t steep = abs(y2 - ystartaddress) > abs(x2 - xstartaddress);
  	if (steep) 
	{
    	swap(uint16_t, xstartaddress, ystartaddress);
    	swap(uint16_t, x2, y2);
	}
	if (xstartaddress > x2) 
	{
    	swap(uint16_t, xstartaddress, x2);
    	swap(uint16_t, ystartaddress, y2);
  	}
	
  	int16_t dx, dy;
  	dx = x2 - xstartaddress;
  	dy = abs(y2 - ystartaddress);

	int16_t err = dx / 2;
	int16_t ystep;

	if (ystartaddress < y2) 
	{
    	ystep = 1;
  	} 
	else 
	{
    	ystep = -1;
	}

	for (; xstartaddress<=x2; xstartaddress++) 
	{
    	if (steep) 
		{
      		drawPixel(ystartaddress, xstartaddress);
    	} 
		else 
		{
      		drawPixel(xstartaddress, ystartaddress);
    	}
    	err -= dy;
    	if (err < 0) 
		{
			ystartaddress += ystep;
			err += dx;
    	}
  	}
}
void ETMS_ST7796S::drawLine(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t colour){
  _colour565 = colour;
  drawLine(xs, ys, xd, yd);
}
void ETMS_ST7796S::drawLine(uint16_t xs, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue){
  _colour565 = to565Colour( red, green, blue);
  drawLine(xs, y1, x2, y2);
}
//	Horizontal Line (FastLine method)
void ETMS_ST7796S::drawHLine(uint16_t xs,uint16_t ys ,uint16_t length) {
	//	draw line here,  do loop to add pixel to the x line
	 for (int i = 0; i <= length; i++) {
    drawPixel(xs+i, ys, _colour565);
  }
}
void ETMS_ST7796S::drawHLine(uint16_t xs,uint16_t ys ,uint16_t length, uint16_t colour) {
	_colour565 = colour;
	drawHLine(xs,ys,length);
}
void ETMS_ST7796S::drawHLine(uint16_t xs,uint16_t ys ,uint16_t length, uint8_t red, uint8_t green, uint8_t blue) {
	_colour565 = to565Colour(red,green,blue);
	drawHLine(xs,ys,length);
}
//  Vertical Line (FastLine method)
void ETMS_ST7796S::drawVLine(uint16_t xs, uint16_t ys, uint16_t length) {
	//	draw line here, do loop
	for (int i = 0; i <= length; i++) {
    drawPixel(xs, ys+i, _colour565);
  }
}
void ETMS_ST7796S::drawVLine(uint16_t xs, uint16_t ys, uint16_t length, uint16_t colour) {
  	_colour565 = colour;
	drawVLine(xs,ys,length);
	
}
void ETMS_ST7796S::drawVLine(uint16_t xs, uint16_t ys, uint16_t length, uint8_t red, uint8_t green, uint8_t blue) {
	_colour565 = to565Colour(red,green,blue);
	drawVLine(xs,ys,length);
}
/*
  Advanced Graphics.
  note: world coordinate system used - TOP-LEFT is origin (0,0).
  Let's start with the rectangles;open,filled,rounded.
  Then the Circles and Triangles.
  Asperations on fancy guages elements next...
*/
void ETMS_ST7796S::drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height){
	//xs = _tft_width - xs;
	drawHLine( xs, ys, width);
	drawHLine( xs, ys+height, width);
	drawVLine( xs, ys, height);
	drawVLine( xs+width, ys, height);
}
void ETMS_ST7796S::drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour){
	_colour565 = colour;
	drawRect( xs, ys, width, height);
}
void ETMS_ST7796S::drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue){
  	_colour565 = to565Colour( red, green, blue);
	drawRect( xs, ys, width, height);
}

void ETMS_ST7796S::fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height){
for (int j = 0; j <= height; j++){
    for (int i = 0; i <= width; i++) {
    drawPixel(xs+i, ys+j, _colour565);
    }
  }
}
void ETMS_ST7796S::fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour){
	_colour565 = colour;
	fillRect( xs, ys, width, height);
}
void ETMS_ST7796S::fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue){
	_colour565 = to565Colour(red,green,blue);
	fillRect( xs, ys, width, height);
}

void ETMS_ST7796S::drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height){
if (xs>width)
	{
		swap(uint16_t, xs, width);
	}
	if (ys>height)
	{
		swap(uint16_t, ys, height);
	}
	if ((width-xs)>4 && (height-ys)>4)
	{
		drawPixel(xs+1,ys+1,_colour565);
		drawPixel(width-1,ys+1,_colour565);
		drawPixel(xs+1,height-1,_colour565);
		drawPixel(width-1,height-1,_colour565);
		drawHLine(xs+2, ys, width-xs-4,_colour565);
		drawHLine(xs+2, height, width-xs-4,_colour565);
		drawVLine(xs, ys+2, height-ys-4,_colour565);
		drawVLine(width, ys+2, height-ys-4,_colour565);
	}
}
void ETMS_ST7796S::drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour){
	_colour565 = colour;
	drawRoundRect( xs,  ys,  width,  height);
}
void ETMS_ST7796S::drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue){
	_colour565 = to565Colour(red,green,blue);
  drawRoundRect(xs, ys, width, height, _colour565);
}

void ETMS_ST7796S::fillRoundRect(uint16_t xs, uint16_t y1, uint16_t width, uint16_t height){
	uint16_t x2 = xs + width;
	uint16_t y2 = y1 + height;

	if ((x2-xs)>4 && (y2-y1)>4)
	{
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				drawHLine(xs+2, y1+i, x2-xs-4);
				drawHLine(xs+2, y2-i, x2-xs-4);
				break;
			case 1:
				drawHLine(xs+1, y1+i, x2-xs-2);
				drawHLine(xs+1, y2-i, x2-xs-2);
				break;
			default:
				drawHLine(xs, y1+i, x2-xs);
				drawHLine(xs, y2-i, x2-xs);
			}
		}
	}

}
void ETMS_ST7796S::fillRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour){
  	_colour565 = colour;
	fillRoundRect(xs,ys,width,height);
}
void ETMS_ST7796S::fillRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue){
  	_colour565 = to565Colour(red,green,blue);
  	fillRoundRect(xs,ys,width,height);
}

//  Circles

void ETMS_ST7796S::drawCircle(uint16_t xs, uint16_t ys, uint16_t radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int xs = 0;
	int y1 = radius;
	
	setAddressWindow(xs, ys + radius, xs, ys + radius);
	Lcd_Write_Data16(_colour565);
	setAddressWindow(xs, ys - radius, xs, ys - radius);
	Lcd_Write_Data16(_colour565);
	setAddressWindow(xs + radius, ys, xs + radius, ys);
	Lcd_Write_Data16(_colour565);
	setAddressWindow(xs - radius, ys, xs - radius, ys);
	Lcd_Write_Data16(_colour565);
	
	while(xs < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		xs++;
		ddF_x += 2;
		f += ddF_x;    
		setAddressWindow(xs + xs, ys + y1, xs + xs, ys + y1);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs - xs, ys + y1, xs - xs, ys + y1);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs + xs, ys - y1, xs + xs, ys - y1);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs - xs, ys - y1, xs - xs, ys - y1);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs + y1, ys + xs, xs + y1, ys + xs);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs - y1, ys + xs, xs - y1, ys + xs);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs + y1, ys - xs, xs + y1, ys - xs);
		Lcd_Write_Data16(_colour565);
		setAddressWindow(xs - y1, ys - xs, xs - y1, ys - xs);
		Lcd_Write_Data16(_colour565);
	}
	clrXY();
}
void ETMS_ST7796S::drawCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t colour){
	_colour565 = colour;
	drawCircle( xs, ys, radius);
}
void ETMS_ST7796S::drawCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t lw, uint8_t red, uint8_t green, uint8_t blue){
	_colour565 = to565Colour(red,green,blue);
	drawCircle( xs, ys, radius);
}

void ETMS_ST7796S::fillCircle(uint16_t xs, uint16_t ys, uint16_t radius){

  for(long y1=-radius; y1<=0; y1++) 
		for(long xs=-radius; xs<=0; xs++)
			if(xs*xs+y1*y1 <= radius*radius) 
			{
				drawHLine(xs+xs, ys+y1, 2*(-xs));
				drawHLine(xs+xs, ys-y1, 2*(-xs));
				break;
			}
}
void ETMS_ST7796S::fillCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t colour){
_colour565 = colour;
  fillCircle(xs, ys, radius);
}
void ETMS_ST7796S::fillCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint8_t red, uint8_t green, uint8_t blue){
  _colour565 = to565Colour(red,green,blue);
  fillCircle(xs, ys, radius);
}
//	Arc
void ETMS_ST7796S::drawArc(uint16_t xpos, uint16_t ypos, uint16_t r, uint16_t start, uint16_t stop, uint16_t colour){


}

//  Triangles

void ETMS_ST7796S::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2){
	drawLine(x0, y0, x1, y1);
	drawLine(x1, y1, x2, y2);
  	drawLine(x2, y2, x0, y0);
}
void ETMS_ST7796S::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint16_t colour){
  	_colour565 = colour;
	drawTriangle(x0,y0,x1,y1,x2,y2);
}
void ETMS_ST7796S::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue){
  	_colour565 = to565Colour(red,green,blue);
	drawTriangle(x0, y0,  x1,  y1, x2,  y2, _colour565);
}
// tbd
void ETMS_ST7796S::fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  uint16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(uint16_t,y0, y1);
    swap(uint16_t,x0, x1);
  }
  if (y1 > y2) {
    swap(uint16_t,y2, y1);
    swap(uint16_t,x2, x1);
  }
  if (y0 > y1) {
    swap(uint16_t,y0, y1);
    swap(uint16_t,x0, x1);
  }

  //startWrite();
  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    drawHLine(a, y0, b - a + 1, _colour565);
    //endWrite();
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap(uint16_t,a, b);
    drawHLine(a, y, b - a + 1, _colour565);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (uint32_t)dx12 * (y - y1);
  sb = (uint32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap(uint16_t, a, b);
    drawHLine(a, y, b - a + 1, _colour565);
  }
  clrXY();
}
void ETMS_ST7796S::fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
	_colour565 = colour;
	fillTriangle(x0,y0,x1,y1,x2,y2);
}
void ETMS_ST7796S::fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue) {
	_colour565 = to565Colour(red,green,blue);
	fillTriangle(x0,y0,x1,y1,x2,y2);
}

/*
  Everything Text...

  set cursor - 0,0 is top left!
*/
void ETMS_ST7796S::setCursor(uint16_t xpos, uint16_t ypos){
  _CurrentW = cfont.x_size-1;   //  start with these values for the initial font set.
  _CurrentH = cfont.y_size-1;
  _CurrentX = xpos;
  _CurrentY = ypos;
  setAddressWindow(_CurrentX, _CurrentY, _CurrentW, _CurrentH);
}
/*
  print a character from the currently selected font array
*/

void ETMS_ST7796S::drawChar(byte c, uint16_t xpos, uint16_t ypos){
	byte i,ch;
	word j;
	word temp; 
	//xpos = _tft_width - xpos;
	if (!_TRANSPARENT)
	{
		if (orient==PORTRAIT)	//	is portrait orientaton
		{
			setAddressWindow(xpos,ypos,cfont.x_size-1,cfont.y_size-1);
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++)
			{
				ch=pgm_read_byte(&cfont.font[temp]);
				for(i=0;i<8;i++)
				{   
					if((ch&(1<<(7-i)))!=0)   
					{
						setPixel(_colour565);
					} 
					else
					{
						setPixel(_backcolour565);
					}   
				}
				temp++;
			}
		}
		else	//	is landscape orientation and NOT transparent!
		{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<cfont.y_size;j++) 
			{
				//setAddressWindow(xpos,ypos+(j/(cfont.x_size/8)),xpos+cfont.x_size-1,ypos+(j/(cfont.x_size/8)));
				
				for (int zz=0; zz<(cfont.x_size/8); zz++)
				{
					ch=pgm_read_byte(&cfont.font[temp+zz]);
					for(i=0;i<8;i++)	
					{
						setAddressWindow(xpos+i+(zz*8),ypos+j,xpos+i+(zz*8)+1,ypos+j+1);
						if((ch&(1<<(7-i)))!=0)   
						{
							setPixel(_colour565);
						} 
						else
						{
							//setAddressWindow(xpos+i+(zz*8),ypos+j,xpos+i+(zz*8)+1,ypos+j+1);
							setPixel(_backcolour565);
						}   
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
		clrXY();
	}
	else		//	is transparent and LANDSCAPE
	{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++) 
		{
			for (int zz=0; zz<(cfont.x_size/8); zz++)
			{
				ch=pgm_read_byte(&cfont.font[temp+zz]); 
				for(i=0;i<8;i++)
				{   
					if((ch&(1<<(7-i)))!=0)   
					{
						setAddressWindow(xpos+i+(zz*8),ypos+j,xpos+i+(zz*8)+1,ypos+j+1);
						setPixel(_colour565);
					} 
					
				}
			}
			temp+=(cfont.x_size/8);
		}
	}
	clrXY();
}
void ETMS_ST7796S::drawChar(uint8_t c, uint16_t xpos, uint16_t ypos, uint16_t colour){
  _colour565 = colour;
 drawChar(c,xpos,ypos);
}
void ETMS_ST7796S::drawChar(uint8_t c, uint16_t xpos, uint16_t ypos, uint8_t red, uint8_t green, uint8_t blue){
_colour565 = to565Colour(red,green,blue);  //  converts RGB888 and loads to _colour565
drawChar(c,xpos,ypos);
} 

void ETMS_ST7796S::rotateChar(byte c, int x, int y, int pos, int deg){
	byte i,j,ch;
	word temp; 
	int newx,newy;
	double radian;
	radian=deg*0.0175;  

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
		for (int zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=pgm_read_byte(&cfont.font[temp+zz]); 
			for(i=0;i<8;i++)
			{   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				setAddressWindow(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0)   
				{
					setPixel(_colour565);
				} 
				else  
				{
					if (!_TRANSPARENT)
						setPixel(_backcolour565);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
	clrXY();
}
// This is to print numbers
void ETMS_ST7796S::print(char *st, int x, int y, int deg){
	int i;
	int stl = strlen(st);

	if (orient==PORTRAIT)
	{
	if (x==RIGHT){
		x=(disp_y_size+1)-(stl*cfont.x_size);
		}
	if (x==CENTER){
		x=((disp_y_size+1)-(stl*cfont.x_size))/2;
		}
	}
	else			//			*****			is landscape orientation
	{
	if (x==RIGHT){
		 x=((_tft_width/2)-(stl*cfont.x_size));
		 }
	if (x==CENTER){
		 x=((_tft_width/2)-(stl*cfont.x_size)/2);
		 }
	}

	for (i=0; i<stl; i++)
		if (deg==0)
			drawChar(*st++, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
}
void ETMS_ST7796S::print(String st, int x, int y, int deg)
{
	char buf[st.length()+1];
	st.toCharArray(buf, st.length()+1);
	print(buf, x, y, deg);
}

void ETMS_ST7796S::printString(String x, uint16_t xpos, uint16_t ypos){
 	uint16_t csp = cfont.x_size;
	//  drawChar in a loop for the length of string x
	for (int i = 0; i <= x.length()-1; i++) {drawChar( x[i], xpos + (i * csp), ypos);}
}
void ETMS_ST7796S::printString(String x, uint16_t xpos, uint16_t ypos, uint16_t colour){
  	_colour565 = colour;
 	printString(x,xpos,ypos);
}
void ETMS_ST7796S::printString(String x, uint16_t xpos, uint16_t ypos, uint8_t red, uint8_t green, uint8_t blue){
	_colour565 = to565Colour(red,green,blue);
	printString(x,xpos,ypos);
}
//	Numbers
void ETMS_ST7796S::printNumI(long num, uint16_t xpos, uint16_t ypos, int length, char filler)
{
	char buf[25];
	char st[27];
	boolean neg=false;
	int c=0, f=0;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	print(st,xpos,ypos);
}

void ETMS_ST7796S::printNumF(double num, uint16_t xpos, uint16_t ypos, byte dec, char divider, int length, char filler)
{
	char st[27];
	boolean neg=false;

	if (dec<1)
		dec=1;
	else if (dec>20)
		dec=20;

	if (num<0)
		neg = true;

	Convert_Float(st, num, length, dec);

	if (divider != '.')
	{
		for (int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ')
	{
		if (neg)
		{
			st[0]='-';
			for (int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else
		{
			for (int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	print(st,xpos,ypos);
}
// Images
void ETMS_ST7796S::drawBitMap(int xpos, int ypos, int sx, int sy, bitmapdatatype data, int scale=1)
{
	unsigned int col;
	int tx, ty, tc, tsx, tsy;

	if (scale==1)
	{
		if (orient==PORTRAIT)
		{
			setXY(xpos, ypos, xpos+sx-1, ypos+sy-1);
			for (tc=0; tc<(sx*sy); tc++)
			{
				col=pgm_read_word(&data[tc]);
				setPixel(col);
				//LCD_Write_DATA(col>>8,col & 0xff);
			}
		}
		else	// is landscape
		{
			for (ty=0; ty<sy; ty++)
			{
				setXY(xpos, ypos+ty, xpos+sx-1, ypos+ty);
				for (tx=sx-1; tx>=0; tx--)
				{
					col=pgm_read_word(&data[(ty*sx)+tx]);
					setPixel(col);
					//LCD_Write_DATA(col>>8,col & 0xff);
				}
			}
		}
	}
	else
	{
		if (orient==PORTRAIT)
		{
			for (ty=0; ty<sy; ty++)
			{
				setXY(xpos, ypos+(ty*scale), xpos+((sx*scale)-1), ypos+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							setPixel(col);
							//LCD_Write_DATA(col>>8,col & 0xff);
					}
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				for (tsy=0; tsy<scale; tsy++)
				{
					setXY(xpos, ypos+(ty*scale)+tsy, xpos+((sx*scale)-1), ypos+(ty*scale)+tsy);
					for (tx=sx-1; tx>=0; tx--)
					{
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
						setPixel(col);
						//	LCD_Write_DATA(col>>8,col & 0xff);
					}
				}
			}
		}
	}
	clrXY();
}
void ETMS_ST7796S::drawBitMap(int xpos, int ypos, int sx, int sy, bitmapdatatype data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		drawBitMap(xpos, ypos, sx, sy, data);
	else
	{
		for (ty=0; ty<sy; ty++)
			for (tx=0; tx<sx; tx++)
			{
				col=pgm_read_word(&data[(ty*sx)+tx]);
				newx=xpos+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=ypos+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));
				setXY(newx, newy, newx, newy);
				setPixel(col);
				//LCD_Write_DATA(col>>8,col & 0xff);
			}
	}
	clrXY();
}

void ETMS_ST7796S::setFont(uint8_t* font){
  	cfont.font = font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
}

uint8_t* ETMS_ST7796S::getFont(){
	return cfont.font;
}

uint8_t ETMS_ST7796S::getFontXsize(){
	return cfont.x_size;
}

uint8_t ETMS_ST7796S::getFontYsize(){
	return cfont.y_size;
}



