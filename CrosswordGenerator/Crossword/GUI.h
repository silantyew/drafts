#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include "DefaultMap.h"

namespace GUI
{
	int showWindow(bool (*createCrosswordMethod)());
	void throwError(const char* message);
	unsigned getMapWidth();
	unsigned getMapHeight();
	char getMapMaskAt(unsigned x, unsigned y);
	void setLetterAt(unsigned x, unsigned y, char letter);
};