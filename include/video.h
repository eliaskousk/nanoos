//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com        GNU GPL-V2              //
//////////////////////////////////////////////////////////
// Video headers                                        //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef VIDEO_H
#define VIDEO_H

#include "low-io.h"
#include "mutex.h"
//The different colours that can be set. Only the first 7 are relevent for background
typedef enum Colours {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
			 WHITE, DARKGRAY,BRIGHTBLUE, BRIGHTGREEN, BRIGHTCYAN, PINK, BRIGHTMAGENTA, YELLOW, BRIGHTWHITE
}TextColour;

class video	//Video class 
{
	public:
		video() ;
		~video() ;
		void clear() ;
		void putchar(char c) ;
		void write(char *cp) ;
		int GetX();
		int GetY();
		void gotoxy(unsigned x, unsigned y) ;
		void SetColour(TextColour Text, TextColour Back, bool blink);
		unsigned char GetBackColour();
		unsigned char GetTextColour();
		void SetBackColour(Colours col);
		void SetTextColour(Colours col);
		
	private:
		void scrollup() ;
		void setcursor(unsigned x, unsigned y);

		unsigned short *videomem ;		//pointer to video memory
		unsigned short crtc_mem;
		static unsigned int ypos ;				//offset, used like a y cord
		static unsigned int xpos ;				//position, used like x cord
		unsigned int scrHeight ;
		unsigned int scrWidth ;
		unsigned char colour ;			//Text colours
		unsigned char text ;
		unsigned char back ;
		mutex vmt;
};


#endif

