#pragma once
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/gl.h>
#include <FL/Fl_Widget.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include "GlWindow.h"
#include "diceSim.h"
#include <time.h>
#include <vector>
using namespace std;

// Put function prototypes here
void xInput_callback(Fl_Widget* widget, void*);
void yInput_callback(Fl_Widget* widget, void*);
void zInput_callback(Fl_Widget* widget, void*);
void numSim_callback(Fl_Widget* widget, void*);
DWORD WINAPI updateGUI(LPVOID lpParameter);
DWORD WINAPI simFunction(LPVOID lpParameter);
string parseString(float val, int digits);
void exit_cb(Fl_Widget *, void*);
void save_cb(Fl_Widget* widget, void*);
void sdig1_cb(void);
void sdig2_cb(void);
void sdig3_cb(void);
void sdig4_cb(void);
void reset_cb(void);
void run_sim_cb(Fl_Widget* widget, void*);
void reset_sim_cb(Fl_Widget* widget, void*);
void reset_graphics_cb(void);
void help_cb(void);

// STruct which holds the data for new threads
struct thread_data
{
	int m_id;
	thread_data(int id) : m_id(id) {}
};

// Define GUI components globally for cleanup as well as for access purposes
GlWindow		*GLWINDOW;		// The Graphics window, displays 3D model of die and center of gravity location
Fl_Window		*window;		// The main window, acts as a container for all other components
Fl_Input		*xInput;		// displays the numeric x-value of the center of gravity
Fl_Input		*yInput;		// displays the numeric y-value of the center of gravity
Fl_Input		*zInput;		// displays the numeric z-value of the center of gravity
Fl_Input		*nSim;			// displays the number of simulations which will be run with these settings
Fl_Text_Buffer	*oBuffer;		// buffer for the output box, holds the text to display
Fl_Text_Display *output;		// Displays the output from each simulation
Fl_Button		*submit;		// Button which starts the simulation
Fl_Button		*reset;			// Button which resets the simulation
Fl_Button		*results;		// Button which outputs simulation results to a file
Fl_Menu_Bar		*windowMenu;	// Menu bar, holds the menu items outlined below
Fl_Box			*zBox;			// Displays whether Graphics window is currently adjusting the y-axis or z-axis
Fl_Box			*xMM;			// Holds label for x-input units (mm)
Fl_Box			*yMM;			// Holds label for y-input units (mm)
Fl_Box			*zMM;			// Holds label for z-input units (mm)
HANDLE			thread;			// Thread which checks and links updates between the main window and the GL window
HANDLE			simThread;		// Thread which handles running the simulation
diceSim			sim;			// The physics simulation object

// Contents of the menu bar, numbers refer to each item's index in the array
Fl_Menu_Item menuContents[] = {
	{ "File", 0, 0, 0, FL_SUBMENU },                                              // 0
	{ "&Save Results...", FL_COMMAND + 's', (Fl_Callback *)save_cb, 0 },          // 1
	{ "&Reset Settings...", FL_COMMAND + 'r', (Fl_Callback *)reset_cb, 0 },       // 2
	{ "&Close Program...", FL_COMMAND + 'c', (Fl_Callback *)exit_cb },            // 3
	{ 0 },                                                                        // 4 
	{ "Simulation",0, 0, 0, FL_SUBMENU },                                         // 5
	{ "Significant Digits", 0, 0, 0, FL_SUBMENU },                                // 6
	{ "1 Digit", 0, (Fl_Callback *)sdig1_cb, 0, FL_MENU_TOGGLE },                 // 7
	{ "2 Digits", 0, (Fl_Callback *)sdig2_cb, 0, FL_MENU_TOGGLE },                // 8
	{ "3 Digits", 0, (Fl_Callback *)sdig3_cb, 0, FL_MENU_TOGGLE },                // 9
	{ "4 Digits", 0, (Fl_Callback *)sdig4_cb, 0, FL_MENU_TOGGLE },                // 10
	{ 0 },                                                                        // 11
	{ "S&tart Sim...", FL_COMMAND + 't', (Fl_Callback *)run_sim_cb, 0 },          // 12
	{ "R&eset Sim...", FL_COMMAND + 'e', (Fl_Callback *)reset_sim_cb, 0 },        // 13
	{ 0 },                                                                        // 14
	{ "Graphics", 0, 0, 0, FL_SUBMENU },                                          // 15
	{ "Reset &View...", FL_COMMAND + 'v', (Fl_Callback *)reset_graphics_cb, 0 },  // 16
	{ 0 },                                                                        // 17
	{ "Help", 0, (Fl_Callback *)help_cb, 0, 0 },                                  // 18
	{ 0 }																	      // 19
};

// settings components
int		sigDigits; // Holds the number of significant digits to use (1~4)
int		numSim;	   // Holds the number of simulations to run at a given time
float	xVal;	   // Holds the xValue of the center of gravity (in mm)
float	yVal;	   // Holds the yValue of the center of gravity (in mm)
float	zVal;	   // Holds the zValue of the center of gravity (in mm)
bool	zActive;   // Holds whether the y-axis or z-axis is being controlled by graphics controls

				   // GUI Helper variables
bool	running = true;		// Used to help shut down threads when the program ends
bool	simStart = false;	// Used to signal the simThread to start working
bool	bufferInit = false; // Used to signify that oBuffer has been initialized