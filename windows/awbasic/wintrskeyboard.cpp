//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <windows.h>            
#include "resource.h"   
#include "basic.h" 

/*********************************************************
 * WinTrsKeyboard.cpp
 *
 * This file contains Windows specific code for simulating 
 * scanning TRS-80 keyboard codes.
 *
 *********************************************************/

static unsigned int last_char=0;
static unsigned int last_oem_code=0;
static unsigned char m_is_key_down[256];
static unsigned char m_key_oem_code[256];


#define TRS80_KEY_SHIFT	1
#define TRS80_KEY_CTRL	2
#define TRS80_KEY_LAST_SPECIAL 2

#define TRS80_KEY_BREAK	3
#define TRS80_KEY_CLEAR	31
#define TRS80_KEY_UP	91
#define TRS80_KEY_DOWN	10
#define TRS80_KEY_LEFT	8
#define TRS80_KEY_RIGHT	9

//**************************************************
//
// ASCII chars except for TRS80_KEY_ which are mapped in to ASCII
//
void WinSetCharDown(unsigned char c, unsigned char oem_code)
{
	if (c >	TRS80_KEY_LAST_SPECIAL)		// Shift, control arn't chars
	{
		last_char = c;    
		last_oem_code = oem_code;
	}

	m_is_key_down[c] = 1;
	m_key_oem_code[c] = oem_code;

	//TRACE("key down %x %x\n", c, oem_code);
}

//**************************************************

void WinOnKeyDown(unsigned int vk, unsigned int nRepCnt, unsigned int nFlags) 
{
	if (vk == VK_SHIFT)
		WinSetCharDown(TRS80_KEY_SHIFT, nFlags & 0xFF); //?? If VK_SHIFT can have multiple oem codes, there is a bug here

	else if (vk == VK_CONTROL)
		WinSetCharDown(TRS80_KEY_CTRL, nFlags & 0xFF);
	
	else if (vk==0x1b || vk==VK_PAUSE)	// ESC or pause/break
		WinSetCharDown(TRS80_KEY_BREAK, nFlags & 0xFF);
	
	else if (vk == VK_LEFT || vk == 8)	// bs or left
		WinSetCharDown(TRS80_KEY_LEFT, nFlags & 0xFF);
	
	else if (vk == VK_RIGHT )
		WinSetCharDown(TRS80_KEY_RIGHT, nFlags & 0xFF);
	
	else if (vk == VK_UP)
		WinSetCharDown(TRS80_KEY_UP, nFlags & 0xFF);
	
	else if (vk == VK_DOWN)
		WinSetCharDown(TRS80_KEY_DOWN, nFlags & 0xFF);
	
	else if (vk == VK_HOME)
		WinSetCharDown(TRS80_KEY_CLEAR, nFlags & 0xFF);

}

//**************************************************

void WinOnChar(unsigned int c, unsigned int nRepCnt, unsigned int nFlags) 
{
	WinSetCharDown(c, nFlags & 0xFF);	// Bug in Win docs, scan code is in LSB
}

//**************************************************
//
// WM_KEYUP messages, so there are lots of them
//
void WinSetCharUp(unsigned char oem_code)
{
	// TRACE("key up %x %x\n", c, oem_code);

	for (short i=0; i < 256; i++)
		if (m_is_key_down[i] &&  m_key_oem_code[i]==oem_code)
		{
			m_is_key_down[i] = 0;
			break;
		}

	if (i >= 256)
	{
		//TRACE("CWinTrs80::SetCharU Error:  Can't find oem_code %x\n", oem_code);
	}

}

//**************************************************

void WinOnKeyUp(unsigned int vk, unsigned int nRepCnt, unsigned int nFlags) 
{
	WinSetCharUp(nFlags&0xFF);	
}

//**************************************************

char WinInkey(IoConsole* io)
{
	char c;

	c = last_char;			// TRS-80 BASIC returns the last key pressed from inkey$, regardless of whether it is still pressed
	last_char = NULL;
	return c;
}

//**************************************************

unsigned char* WinPeekKeyboardStatus()
{
	return m_is_key_down;
}

//**************************************************


#define KB0(x) ((c[x]&1)<<0)
#define KB1(x) ((c[x]&1)<<1)
#define KB2(x) ((c[x]&1)<<2)
#define KB3(x) ((c[x]&1)<<3)
#define KB4(x) ((c[x]&1)<<4)
#define KB5(x) ((c[x]&1)<<5)
#define KB6(x) ((c[x]&1)<<6)
#define KB7(x) ((c[x]&1)<<7)

unsigned char WinTrsPeekKeyboard(struct IoConsole* io, unsigned short Addr)
{
	unsigned char* c;
	//
	// Get a 256 byte byte array.  One entry for each ASCII character, plus
	// special TRS80_KEY_ characters, such as TRS80_KEY_SHIFT.  A value of 1
	// indicates key is pressed, 0 means key is not pressed.
	//
	// I considered using GetKeyboardState() here, but this windows function returns virtual key states,
	// and there are no defined VKs for period, comma key, etc.  So used WM_CHAR to capture ascii keydowns, and then
	// use the scan codes to find the keyups (there appears to be no WM_CHAR key up equivlent, and WM_KEYUP uses
	// VK and scancodes, so these must be mapped back to the ascii key downs.).
	//
	c = WinPeekKeyboardStatus();

	switch (Addr)
	{
	case 0x3801:
		return KB0('@')|KB1('A')|KB2('B')|KB3('C')|KB4('D')|KB5('E')|KB6('F')|KB7('G') |
					    KB1('a')|KB2('b')|KB3('c')|KB4('d')|KB5('e')|KB6('f')|KB7('g');

	case 0x3802:
		return KB0('H')|KB1('I')|KB2('J')|KB3('K')|KB4('L')|KB5('M')|KB6('N')|KB7('O') |
		       KB0('h')|KB1('i')|KB2('j')|KB3('k')|KB4('l')|KB5('m')|KB6('n')|KB7('o');

	case 0x3804:
		return KB0('P')|KB1('Q')|KB2('R')|KB3('S')|KB4('T')|KB5('U')|KB6('V')|KB7('W') |
		       KB0('p')|KB1('q')|KB2('r')|KB3('s')|KB4('t')|KB5('u')|KB6('v')|KB7('w');

	case 0x3808:
		return KB0('X')|KB1('Y')|KB2('Z') | KB0('x')|KB1('y')|KB2('z');
	
	case 0x3810:
		return KB0('0')|KB1('1')|KB2('2')|KB3('3')|KB4('4')|KB5('5')|KB6('6')|KB7('7') |
			            KB1('!')|KB2('"')|KB3('#')|KB4('$')|KB5('%')|KB6('&')|KB7('\'');

	case 0x3820:
		return KB0('8')|KB1('9')|KB2(':')|KB3(';')|KB4(',')|KB5('-')|KB6('.')|KB7('/') |
			   KB0('(')|KB1(')')|KB2('*')|KB3('+')|KB4('<')|KB5('=')|KB6('>')|KB7('?');
	
	case 0x3840:
		return KB0(0xD)|KB1(TRS80_KEY_CLEAR)|KB2(TRS80_KEY_BREAK)|KB3(TRS80_KEY_UP)|KB4(TRS80_KEY_DOWN)|KB5(TRS80_KEY_LEFT)|KB6(TRS80_KEY_RIGHT)|KB7(' ');
	
	case 0x3880:

		// This is set up to emulate the wierd TRS80 keyboard behavior of shift-for-lowercase
		// to change, swap the A,B.. and a,b,..

		unsigned char yes_shift, no_shift;
		yes_shift = // set to 1 if any char that needs shift key is pressed
			c['A']|c['B']|c['C']|c['D']|c['E']|c['F']|c['G']|
 			c['H']|c['I']|c['J']|c['K']|c['L']|c['M']|c['N']|c['O']|
			c['P']|c['Q']|c['R']|c['S']|c['T']|c['U']|c['V']|c['W']|
			c['X']|c['Y']|c['Z']|			
			c['!']|c['"']|c['#']|c['$']|c['%']|c['&']|c['\'']|
			c['(']|c[']']|c['*']|c['+']|c['<']|c['=']|c['>']|c['?'];

		no_shift = 
			c['a']|c['b']|c['c']|c['d']|c['e']|c['f']|c['g']|
 			c['h']|c['i']|c['j']|c['k']|c['l']|c['m']|c['n']|c['o']|
			c['p']|c['q']|c['r']|c['s']|c['t']|c['u']|c['v']|c['w']|
			c['x']|c['y']|c['z']|
			c['@']|c[':']|c[';']|c[',']|c['-']|c['.']|c['/'];

			// is_shift is set by any key that requires shift be pressed
			// on the trs80 keyboard.
			// no_shift is set by any key that never has shift pressed to generate
			// on the trs80 keyboard.
			// KB0(TRS80_KEY_SHIFT) is set if the shift key is pressed on the host keyboard.
			
			return ( (KB0(TRS80_KEY_SHIFT)&(~no_shift))) | yes_shift | KB4(TRS80_KEY_CTRL);

	default:
		return 0;
	}
}


//*************************************************