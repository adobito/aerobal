/*
 * lcd.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Anthony
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "gpio.h"

/****************************************************/
/*	Constant Definitions							*/
/****************************************************/
//////////////////////////////////////////
// API Layer 0
#define LCD_CMD_FSET_2L_8B 0x38 // Function Set (8-bit interface, 2 lines, 5*7 Pixels
#define LCD_CMD_FSET_1L_8B 0x30 // Function set (8-bit interface, 1 line, 5*7 Pixels)
#define LCD_CMD_FSET_2L_4B 0x28 // Function set (4-bit interface, 2 lines, 5*7 Pixels)
#define LCD_CMD_FSET_1L_4B 0x20 // Function set (4-bit interface, 1 line, 5*7 Pixels)
#define LCD_CMD_FSET_ENTRY_INCREMENT_NOSHIFT 0x06 // Entry set: increment automatically, no display shift
#define LCD_CMD_SCROLL_RIGHT 0x1E //Scroll display one character right (all lines)
#define LCD_CMD_SCROLL_LEFT 0x18 //Scroll display one character left (all lines)
#define LCD_CMD_HOME 0x02 //Home (move cursor to top/left character position)
#define LCD_CMD_CURSOR_MOVE_LEFT 0x10 //Move cursor one character left
#define LCD_CMD_CURSOR_MOVE_RIGHT 0x14 //Move cursor one character right
#define LCD_CMD_CURSOR_UNDERLINE_STATIC 0x0E //Turn on visible underline cursor
#define LCD_CMD_CURSOR_BLOCK_DYNAMIC 0x0F //Turn on visible blinking-block cursor
#define LCD_CMD_CURSOR_HIDE 0x0C //Make cursor invisible
#define LCD_CMD_DISPLAY_BLANK 0x08 //Blank the display (without clearing)
#define LCD_CMD_DISPLAY_RESTORE 0x0C // Restore the display (with cursor hidden)
#define LCD_CMD_CLEAR 0x01 //Clear Screen
#define LCD_CMD_POSITION_UP 0x80 //Move the cursor home.
#define LCD_CMD_POSITION_DOWN 0xA8 //Move the cursor to start of second line.
#define LCD_CMD_POSITION_LINE_OFFSET 0x28 //Offset to move one line down in same position.

//////////////////////////////////////////
// API Layer 4
#define LCD_LINE_UP 0x11
#define LCD_LINE_DOWN 0x12

/****************************************************/
/*	Variable Definitions							*/
/****************************************************/
uint32_t _lcdInited ;
uint32_t _lcdPortControl ;
uint32_t _lcdPortDataUpper ;
uint32_t _lcdPortDataLower ;


/****************************************************/
/*	Function Definitions							*/
/****************************************************/
//////////////////////////////////////////
// API Layer 0
void lcdWriteCommand(uint32_t hexCommand);
void lcdWriteData(uint32_t hexData);

//////////////////////////////////////////
// API Layer 1
void lcdClear();
//void lcdCursorUp();
//void lcdCursorDown();
void lcdCursorHide();
void lcdCursorHome();
void lcdCursorHomeDown();
void lcdCursorMoveLeft();
void lcdCursorMoveRight();
void lcdDisplayHide();
void lcdDisplayRestore();
void lcdScrollLeft();
void lcdScrollRight();

//////////////////////////////////////////
// API Layer 2
void lcdInit(uint32_t __lcdPortControl,
		uint32_t __lcdPortDataUpper,
		uint32_t __lcdPortDataLower);

//////////////////////////////////////////
// API Layer 3
void lcdWriteLetter(char letter);
void lcdWriteLine(char* line);
void lcdWriteLineUp(char* line);
void lcdWriteLineDown(char* line);
void lcdWriteLines(char* line1, char* line2);

//////////////////////////////////////////
// API Layer 4
void lcdWriteString(char *letter);
void lcdClearLine(uint32_t line);
void lcdWriteNumber(double number);
void lcdWriteNumberWithBounds(double number, int integerDigits, int rationalDigits);

#endif /* LCD_H_ */
