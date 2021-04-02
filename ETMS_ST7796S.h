/*
 	Created:      06:23 PM 10/04/2020
  	Last Updated: 12:05 PM 11/16/2020
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
  tft colour display controller.

	ETMS_ST7796S.h - library setup
 
	The #ifndef statement checks to see if the ETMS_ST7796S.h
	file isn't already defined. This is to stop double declarations
	of any identifiers within the library. It is paired with a
	#endif at the bottom of the header and this setup is known as 
	an 'Include Guard'. 
	The following def handles the ARM pcb.

*/

#ifndef __SAM3X8E__
	#define __SAM3X8E__
	#include "hardware/arm/HW_SAM3X8E.h"
#endif

#ifndef _etms_st7796s_h_                                  
#define _etms_st7796s_h_

// st7796s registers
#define NOP		0x00		//	no operation
#define	SWRESET	0x01		//	software reset
#define RDDID	0x04		//	read display ID
#define	RDDSI	0x05
#define	RDDST	0x09
#define RDDPM	0x0A		//	read display power mode
#define	RDDMADCTL	0x0B
#define	RDDCOLMOD	0x0C
#define RDDIM	0x0D		//	read display image mode
#define	RDDSM	0x0E		//	read display signal mode
#define RDDSDR	0x0F		//	read display self-diagnostic result

#define	SLPIN	0x10		//	sleep in
#define SLPOUT	0x11		//	sleep out
#define PTLON	0x12		//	partial display on
#define	NORON	0x13		//	normal display on
#define INVOFF	0x20		//	display inversion off
#define	INVON	0x21		//	display inversion on
#define DISPOFF	0X28		//	display off
#define	DISPON	0x29		//	display on

#define CASET	0x2A		//	column address set
#define RASET	0x2B		//	row address set
#define	RAMWR	0x2C		//	ram write
#define	RAMRD	0x2E		//	ram read

#define PTLAR	0x30		//	partial area
#define	VSCRDEF	0x33		//	vertical scroll definition
#define	MADCTL	0x36		//	Memory data access control
#define	VSCRSADD	0x37	//	vertical scroll start address of RAM
#define	IDMOFF	0x38		//	idle mode off
#define IDMON	0x39		//	idle mode on
#define	COLMOD	0x3A		//	interface pixel format register - colour data input format - RGB888 - RGB565.
#define RAMWRC	0x3C		//	write memory continue
#define RAMRDC	0x3E		//	read memory continue

#define WRDISBV	0x51		//	write display brightness
#define RDDISBV	0x52		//	read display brightness
#define WRCTRLD	0x53		//	write ctrl display
#define RDCTRLD	0x54		//	read ctrl display
#define WRCABC	0x55		//	write adaptive brightness control value
#define RDCABC	0x56		//	read adaptive brightness control value
#define WRCABCMB	0x5E	//	write CABC minimun brightness
#define RDCABCMB	0x5F	//	read CABC minimun brightness

#define RDFCHKSUM	0xAA	//	read first checksm
#define RDCFCS	0xAF		//	read checksum continue

#define	IFMODE	0xB0		//	interface mode control
#define FRMCTR1	0xB1		//	frame rate control - normal mode / full colours
#define FRMCTR2	0xB2		//	frame rate control - in idle mode / 8 colours
#define FRMCTR3	0xB3		//	frame rate control - partial mode / full colours
#define	DIC		0xB4		//	display inversion control
#define BPC		0xB5		//	blanking porch control
#define DFC		0xB6		//	display function control
#define	EM		0xB7		//	entry mode set
#define	PWR1	0xC0		//	power control 1
#define	PWR2	0xC1		//	power control 2
#define	PWR3	0xC2		//	power control 3
#define VCMPCTL	0xC5		//	vcomp control
#define	VCMOFFSET	0xC6	//	vcomp offset register

#define NVMADW	0xD0		//	nvm address/data write
#define NVMBPROG	0xD1	//	nvm byte program
#define NVMSTATRD	0xD2	//	nvm status read
#define RDID4	0xD3		//	read ID4
#define RDID1	0xDA		//	read ID1
#define RDID2	0xDB		//	read ID2
#define RDID3	0xDC		//	read ID3

#define PGC		0xE0		//	positive gamma
#define	NGC		0xE1		//	negative gamma
#define DGC1	0xE2		//	digital gamma 1
#define DGC2	0xE3		//	digital gamma 2
#define DOCA	0xE8		//	display output conrtol adjust
#define	CSCON	0xF0		//	command set control
#define SPIRDCTRL	0xFB	//	spi read control
#define PROMACT	0xFE

//	masks
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_DISL 0x04

#define RGB888	0x07	//	24bit -  16M
#define RGB666	0x06	//	18bit -  262K
#define RGB565	0x05	//	16bit -  65K

#define BUS16BIT	0x50	//	16 bit bus
#define BUS18BIT	0x60	//	18 bit bus

#define ON	true
#define OFF	false
#define PORTRAIT 0
#define LANDSCAPE 1
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998
#define TOP	2
#define MIDDLE	164
#define BOTTOM	304

//  standard colour defines ****************************************************
//  565 colour defines
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618

#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49

#define bitmapdatatype unsigned short*

/*
	The #include of Arduino.h gives this library access to the standard
	Arduino types and constants (HIGH, digitalWrite, etc.). It's 
	unneccesary for sketches but required for libraries as they're not
	.ino (Arduino) files.
*/
#include "Arduino.h"


struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
};

class ETMS_ST7796S{

	public:

		ETMS_ST7796S();
		
		//	Below are the functions for the class. These are the functions available in the library for the user to call.		
		void init(boolean orientation = LANDSCAPE);
		void displayOn();
		void displayOff();
		void setBrightness(uint8_t br);
		uint8_t getID();
		uint16_t getHeight();
		uint16_t getDisplayYSize();
		uint16_t getWidth();
		uint16_t getDisplayXSize();
		uint16_t to565Colour(uint8_t red, uint8_t green, uint8_t blue); // convert RGB888
		void setPixel();
		void setPixel(uint16_t colour);
		void setPixel(uint8_t red, uint8_t green, uint8_t blue);
		void setCursor(uint16_t x, uint16_t y);
		void invertDisplay(uint8_t inv);
		void setTransparency(bool t);
		void setAddressWindow(uint16_t xorg, uint16_t yorg, uint16_t xdest, uint16_t ydest);
		void setXY(uint16_t xorg, uint16_t yorg, uint16_t xdest, uint16_t ydest);
		void clrXY();		//	resets the address window to maximum.
		void fillWindow(uint8_t red, uint8_t green, uint8_t blue, int length);
		void fillWindow(uint16_t colour, int length);
		void fastFill(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
		void clrDisplay();											//	writes 0s to the entire frame buffer.
		void clrScr();
		void fillScreen();
		void fillScreen(uint16_t colour);
		void fillScreen(uint8_t red, uint8_t green, uint8_t blue);

		void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
		void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue);

		void drawPixel(uint16_t xpos, uint16_t ypos);
		void drawPixel(uint16_t xpos, uint16_t ypos, uint16_t colour);
		void drawPixel(uint16_t xp, uint16_t yp, uint8_t red, uint8_t green, uint8_t blue);

		void drawHLine(uint16_t xs,uint16_t ys ,uint16_t length);
		void drawHLine(uint16_t xs,uint16_t ys ,uint16_t length, uint16_t colour);
		void drawHLine(uint16_t xs,uint16_t ys ,uint16_t length, uint8_t red, uint8_t green, uint8_t blue);
		
		void drawVLine(uint16_t xs,uint16_t ys ,uint16_t length);
		void drawVLine(uint16_t xs,uint16_t ys ,uint16_t length, uint16_t colour);
		void drawVLine(uint16_t xs,uint16_t ys ,uint16_t length, uint8_t red, uint8_t green, uint8_t blue);
		
		void drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
		void drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour);
		void drawRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue);
		
		void fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
		void fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour);
		void fillRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue);

		void drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
		void drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour);
		void drawRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue);
		
		void fillRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
		void fillRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint16_t colour);
		void fillRoundRect(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, uint8_t red, uint8_t green, uint8_t blue);
		
		void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
		void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue);
		
		void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2);
		void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint16_t colour);
		void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint8_t red, uint8_t green, uint8_t blue);

		void drawCircle(uint16_t xs, uint16_t ys, uint16_t radius);
		void drawCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t colour);
		void drawCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t lw, uint16_t colour);
		void drawCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t lw, uint8_t red, uint8_t green, uint8_t blue);

		void fillCircle(uint16_t xs, uint16_t ys, uint16_t radius);
		void fillCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint16_t colour);
		void fillCircle(uint16_t xs, uint16_t ys, uint16_t radius, uint8_t red, uint8_t green, uint8_t blue);

		void drawArc(uint16_t x, uint16_t y, uint16_t r, uint16_t start, uint16_t stop, uint16_t colour);

		void drawChar(byte c, uint16_t xpos, uint16_t ypos);
		void drawChar(uint8_t c, uint16_t xpos, uint16_t ypos, uint16_t colour);
		void drawChar(uint8_t c, uint16_t xpos, uint16_t ypos, uint8_t red, uint8_t green, uint8_t blue);

		void rotateChar(byte c, int x, int y, int pos, int deg);
		void print(char *st, int x, int y, int deg=0);
		void print(String st, int x, int y, int deg=0);

		void printString(String x, uint16_t xpos, uint16_t ypos);
		void printString(String x, uint16_t xpos, uint16_t ypos, uint16_t colour);
		void printString(String x, uint16_t xpos, uint16_t ypos, uint8_t red, uint8_t green, uint8_t blue);
		
		void printString(String x, String xpos, uint16_t ypos);

		void printNumI(long num, uint16_t x, uint16_t y, int length=0, char filler=' ');
		void printNumF(double num, uint16_t x, uint16_t y, byte dec=5, char divider='.', int length=0, char filler=' ');

		void drawBitMap(int x, int y, int sx, int sy, bitmapdatatype data, int scale=1);
		void drawBitMap(int x, int y, int sx, int sy, bitmapdatatype data, int deg, int rox, int roy);

		void setColour(uint16_t colour);
		void setColour(uint8_t red, uint8_t green, uint8_t blue);
		
		void setBackColour(uint16_t colour);
		void setBackColour(uint8_t red, uint8_t green, uint8_t blue);
		void setFgBgColour(uint16_t fcolour,uint16_t bcolour);
		void setFont(uint8_t* font);

		uint8_t* getFont();
		uint8_t getFontXsize();
		uint8_t getFontYsize();

	private:
		void SetOutputs();
		void Strobe_Write_Pin();
		void Strobe_Read_Pin();
		void Writ_Bus8(uint8_t val);
		void Writ_Bus16(uint16_t val);
		uint8_t Read_Bus8();
		uint16_t Read_Bus16();
		void Lcd_Dummy_Read();
		void Lcd_Write_Com(uint8_t com8);
		void Lcd_Write_Data8(uint8_t dat8);
		void Lcd_Write_Data16(uint16_t dat16);
		void Lcd_Write_Com_Data(uint8_t com8,uint8_t dat8);
		void Convert_Float(char *buf, double num, int width, byte prec);
		byte orient, fch, fcl, bch, bcl;
		int disp_x_size, disp_y_size;
		_current_font cfont;
		boolean  _TRANSPARENT = true;
		uint16_t _CurrentX, _CurrentY, _CurrentW, _CurrentH, _xStart, _yStart,  _colour565, _backcolour565;
};

//	The end wrapping of the #ifndef Include Guard
#endif