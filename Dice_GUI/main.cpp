#include "mainInclude.h"

int main(int argc, char **argv) {
	// First, load settings
	ifstream settings("./assets/settings.txt");
	// If this is the first time running the program, create the settings files
	if (!settings) {
		sigDigits	= 2;
		numSim		= 10;
		xVal		= 0.0f;
		yVal		= 0.0f;
		zVal		= 0.0f;
		zActive		= false;

	// If settings already exist, read them in
	} else {
		string header;
		string colon;
		string value;
		while (settings >> header >> colon >> value) {
			if (header.compare("SIGDIGIT") == 0) {
				sigDigits = atoi(value.c_str());
			} 
			else if (header.compare("NUMTIMES") == 0) {
				numSim = atoi(value.c_str());
			}
			else if (header.compare("XPOS") == 0) {
				xVal = atoi(value.c_str());
			}
			else if (header.compare("YPOS") == 0) {
				yVal = atoi(value.c_str());
			}
			else if (header.compare("ZPOS") == 0) {
				zVal = atoi(value.c_str());
			}
			else if (header.compare("ZACTIVE") == 0) {
				zActive = atoi(value.c_str());
			}
		}
		settings.close();
	}

	// Hide the console
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	// Create labels for input boxes 
	char* xTitle = "X Pos: ";
	char* yTitle = "Y Pos: ";
	char* zTitle = "Z Pos: ";
	char* nTitle = "Num Rolls: ";

	// Variables used position and size window components
	int wWidth		= 400;													// main window width
	int wHeight		= 500;													// main window height
	int tbOffset	= 20;													// Offset from left edge of window for title-box component
	int tbWidth		= wWidth - tbOffset * 2;								// Width of title-box component
	int tbHeight	= 30;													// Height of title-box component
	int tfLength	= 100;													// Default length for text field components
	int tfHeight	= 25;													// Default height for text field components
	int fontWidth	= 7;													// Character width of font used
	int xTxtY		= tbOffset * 2 + tbHeight;								// X-input text box's y position
	int yTxtY		= xTxtY + tfHeight + tbOffset;							// Y-input text box's y position
	int zTxtY		= yTxtY + tfHeight + tbOffset;							// Z-input text box's y position
	int nTxtY		= zTxtY + tfHeight + tbOffset;							// Num Sim text box's y position
	int oTxtY		= nTxtY + tfHeight + tbOffset + 5;						// Output Box's y position
	int sBtnY		= oTxtY + tfHeight + tbOffset;							// Save button's y position
	int oWidth		= wWidth - tbOffset * 2;								// Output box's width
	int oHeight		= wHeight - oTxtY - tbOffset - tfHeight;				// Output box's height
	int rBtnX		= tbOffset + tfLength + 10;								// Reset button's x position
	int reBtnX		= rBtnX + tfLength + 10;								// Save results button's x position
	int glX			= tbOffset * 2 + tfLength + strlen(nTitle)*fontWidth;	// GL window's x position
	int glY			= tbOffset * 2 + tbHeight;								// GL window's y position
	int glWidth		= wWidth - glX - tbOffset;								// GL window's width
	int glHeight	= glWidth;												// GL window's height
	int gBoxW		= 100;													// Width of the z-active box
	int gBoxH		= 20;													// Height of the z-active box


	// Set up the main window
	window = new Fl_Window(wWidth, wHeight);
	window->callback(exit_cb);
	window->resizable(false);
	
	//Set up our menu bar
	windowMenu = new Fl_Menu_Bar(0, 0, wWidth+1, tbHeight);
	menuContents[sigDigits+6].check();
	menuContents[1].deactivate();
	menuContents[13].deactivate();
	windowMenu->clear();
	windowMenu->copy(menuContents);

	// Create the text fields and labels for positioning the center of gravity
	xInput = new Fl_Input(tbOffset + strlen(xTitle)*fontWidth, xTxtY, tfLength, tfHeight, xTitle);
	xInput->callback(xInput_callback);
	xInput->value(parseString(xVal, sigDigits).c_str());
	xMM = new Fl_Box(tbOffset + strlen(xTitle)*fontWidth + xInput->w(), xTxtY, 40, tfHeight, "mm");
	xMM->box(FL_FLAT_BOX);
	xMM->labelsize(14);
	yInput = new Fl_Input(tbOffset + strlen(yTitle)*fontWidth, yTxtY, tfLength, tfHeight, yTitle);
	yInput->callback(yInput_callback);
	yInput->value(parseString(yVal, sigDigits).c_str());
	yMM = new Fl_Box(tbOffset + strlen(yTitle)*fontWidth + yInput->w(), yTxtY, 40, tfHeight, "mm");
	yMM->box(FL_FLAT_BOX);
	yMM->labelsize(14);
	zInput = new Fl_Input(tbOffset + strlen(zTitle)*fontWidth, zTxtY, tfLength, tfHeight, zTitle);
	zInput->callback(zInput_callback);
	zInput->value(parseString(zVal, sigDigits).c_str());
	zMM = new Fl_Box(tbOffset + strlen(zTitle)*fontWidth + zInput->w(), zTxtY, 40, tfHeight, "mm");
	zMM->box(FL_FLAT_BOX);
	zMM->labelsize(14);

	// Create our text field for the number of simulations to run
	nSim = new Fl_Input(tbOffset + strlen(nTitle)*fontWidth, nTxtY, tfLength, tfHeight, nTitle);
	nSim->callback(numSim_callback);
	nSim->value(to_string(numSim).c_str());

	// Create our simulation output field
	output = new Fl_Text_Display(tbOffset, oTxtY, oWidth, oHeight);
	output->label("Simulation Results");
	oBuffer = new Fl_Text_Buffer();
	bufferInit = true;
	output->buffer(oBuffer);
	output->textcolor(FL_DARK_GREEN);
	output->textfont(FL_COURIER);
	
	// Create our submit, reset, and save buttons
	submit = new Fl_Button(tbOffset, sBtnY + oHeight / 1.2f, tfLength, tfHeight, "Start Sim");
	submit->callback(run_sim_cb);
	reset = new Fl_Button(rBtnX, sBtnY + oHeight / 1.2f, tfLength, tfHeight, "Reset Sim");
	reset->callback(reset_sim_cb);
	reset->deactivate();
	results = new Fl_Button(reBtnX, sBtnY + oHeight / 1.2f, tfLength, tfHeight, "Save Results");
	results->callback(save_cb);
	results->deactivate();

	// Create our status box for z-axis/y-axis controls
	zBox = new Fl_Box(glX + glWidth - gBoxW, ((glY + tbHeight) / 2.0f) - (gBoxH / 2.0f), gBoxW, gBoxH, "Y-ACTIVE");
	zBox->box(FL_UP_BOX);
	zBox->labelsize(12);
	zBox->labelfont(FL_BOLD);
	
	// Create and set up the graphics Window
	GLWINDOW = new GlWindow(glX, glY, glWidth, glHeight, "");
	GLWINDOW->COG_X = xVal;
	GLWINDOW->COG_Y = yVal;
	GLWINDOW->COG_Z = zVal;
	GLWINDOW->zMove = zActive;
	GLWINDOW->loadGSettings();
	GLWINDOW->border(0);
	GLWINDOW->redraw();
	GLWINDOW->end();
	window->label("Dice Roll Simulator v1.1");
	window->end();

	// Make the main window and graphics window visible, give focus to graphics window
	window->show(argc, argv);
	GLWINDOW->show(argc, argv);
	GLWINDOW->take_focus();

	// Create threads for running the simuation and updating the GUI
	thread = CreateThread(NULL, 0, updateGUI, new thread_data(0), 0, 0);
	simThread = CreateThread(NULL, 0, simFunction, new thread_data(1), 0, 0);
	return Fl::run();
}

/*********************************************************************
* Callback function for the X-input text field                       *
* - Updates the center of gravity and sets xVal whenever the value   *
*   of the text field is updated                                     *
**********************************************************************/
void xInput_callback(Fl_Widget* widget, void*) {
	xVal = atof(((Fl_Input*)widget)->value());
	xVal = (abs(xVal) <= 9.0) ? xVal : (xVal / abs(xVal)*9.0);
	((Fl_Input*)widget)->value(parseString(xVal, sigDigits).c_str());
	GLWINDOW->COG_X = xVal;
	GLWINDOW->redraw();
}

/*********************************************************************
* Callback function for the Y-input text field                       *
* - Updates the center of gravity and sets yVal whenever the value   *
*   of the text field is updated                                     *
**********************************************************************/
void yInput_callback(Fl_Widget* widget, void*) {
	yVal = atof(((Fl_Input*)widget)->value());
	yVal = (abs(yVal) <= 9.0) ? yVal : (yVal / abs(yVal)*9.0);
	((Fl_Input*)widget)->value(parseString(yVal, sigDigits).c_str());
	GLWINDOW->COG_Y = yVal;
	GLWINDOW->redraw();
}

/*********************************************************************
* Callback function for the Z-input text field                       *
* - Updates the center of gravity and sets zVal whenever the value   *
*   of the text field is updated                                     *
**********************************************************************/
void zInput_callback(Fl_Widget* widget, void*) {
	zVal = atof(((Fl_Input*)widget)->value());
	zVal = (abs(zVal) <= 9.0) ? zVal : (zVal / abs(zVal)*9.0);
	((Fl_Input*)widget)->value(parseString(zVal, sigDigits).c_str());
	GLWINDOW->COG_Z = zVal;
	GLWINDOW->redraw();
}

/*********************************************************************
* Callback function for the Num. Sim. text field                     *
* - Set the number of simulations to run and updates numSim any time *
*   the value of the text field is updated                           *
**********************************************************************/
void numSim_callback(Fl_Widget* widget, void*) {
	numSim = atoi(((Fl_Input*)widget)->value());
	((Fl_Input*)widget)->value(to_string(numSim).c_str());
	if (numSim <= 0) {
		numSim = 1;
		((Fl_Input*)widget)->value("1");
	}
	if (numSim > 1000000000) {
		numSim = 1000000000;
		((Fl_Input*)widget)->value("1000000000");
	}
}


/*********************************************************************
* Thread Function: Update GUI                                        *
* - Acts as a link between the main window's input boxes and         *
*   GLWINDOW                                                         *
* - Responsible for updating X-input, Y-input, Z-input, Z-Active,    *
*   and the output window whenever changes are detected in either    *
*   the main window or in GLWINDOW                                   *
*********************************************************************/
DWORD WINAPI updateGUI(LPVOID lpParameter) {

	// These are used to store values between thread iterations; so we aren't updating the GUI constantly
	static float lCOG_X		= 0.0;		// Previous value of center of gravity's X-position
	static float lCOG_Y		= 0.0;		// Previous value of center of gravity's Y-position
	static float lCOG_Z		= 0.0;		// Previous value of center of gravity'z Z-position
	static string lOutText	= "";		// Previous value of the ouptut box's text
	static bool lZMove		= false;	// Previous value of the Z-active box

	while (running) {

		// Update COG coordinates and Coordinate boxes accordingly on update
		if (lCOG_X != GLWINDOW->COG_X) {
			xInput->value(parseString(GLWINDOW->COG_X, sigDigits).c_str());
			xVal = GLWINDOW->COG_X;
			lCOG_X = GLWINDOW->COG_X;
		}
		if (lCOG_Y != GLWINDOW->COG_Y) {
			yInput->value(parseString(GLWINDOW->COG_Y, sigDigits).c_str());
			yVal = GLWINDOW->COG_Y;
			lCOG_Y = GLWINDOW->COG_Y;
		}
		if (lCOG_Z != GLWINDOW->COG_Z) {
			zInput->value(parseString(GLWINDOW->COG_Z, sigDigits).c_str());
			zVal = GLWINDOW->COG_Y;
			lCOG_Z = GLWINDOW->COG_Z;
		}
		// Update z-active box
		if (lZMove != GLWINDOW->zMove) {
			if (GLWINDOW->zMove) {
				zBox->label("Z-ACTIVE");
			}
			else {
				zBox->label("Y-ACTIVE");
			}
			lZMove = GLWINDOW->zMove;
		}
	}

	return 0;
}

/*********************************************************************
* Thread Function: Sim Function                                      *
* - Handles the running of the actual physics simulation             *
**********************************************************************/
DWORD WINAPI simFunction(LPVOID lpParameter) {
	string outputTxt;
	while (running) {
		srand(time(0));
		if (simStart) {
			// Clear buffer
			oBuffer->remove(0, oBuffer->length());

			for (int i = 0; i < numSim; i++) {
				// Create physics sim object
				sim = diceSim((xVal / 9.0f) * 2.5f, (yVal / 9.0f) * 2.5f, (zVal / 9.0f) * 2.5f);
				// Update sim 
				sim.updateSim();
				// check if sim is complete
				int updatecounter = 0;
				while (!sim.simFinished()) {
					// call update function
					sim.updateSim();
				}
				// - If complete output "Simulation i+1 complete, dice rolled a: #
				outputTxt = "Simulation " + to_string(i + 1) + " complete, rolled a: " + sim.getFaceUp(true) + "\n";
				output->insert(outputTxt.c_str());
				output->show_insert_position();
				Fl::flush();
				// clean up memory
				sim.cleanup();
			}
			// Disable simStart so we don't run forever
			simStart = false;
			GLWINDOW->take_focus();

			menuContents[1].activate();
			menuContents[13].activate();
			reset->activate();
			results->activate();
			windowMenu->clear();
			windowMenu->copy(menuContents);
			Fl::flush();
		}
	}
	return 0;
}

/*********************************************************************
* Parse Float as String                                              *
* - Helper function which serves to round and format the values      *
*   displayed in X-input, Y-input and Z-input according to the set   *
*   number of significant digits                                     *
*********************************************************************/
string parseString(float val, int digits) {
	// if positive
	if (val >= 0) {
		return to_string(roundf(val * pow(10, sigDigits)) / pow(10, sigDigits)).substr(0, digits + 2);
	}
	// if negative
	else {
		return to_string(roundf(val * pow(10, sigDigits)) / pow(10, sigDigits)).substr(0, digits + 3);
	}
}

/*********************************************************************
 * Exit Callback Function                                            *
 * - Used by the close menu item and by the main window's close      *
 *   button                                                          *
*********************************************************************/
void exit_cb(Fl_Widget *, void*) {
	// Stop our threads
	running = false;

	// Double check with the user first
	if (fl_ask("Do you really want to exit?")) {
		// Save settings
		ofstream writeSettings("./assets/settings.txt");
		string line = "SIGDIGIT : " + to_string(sigDigits) + "\n" +
			"NUMTIMES : " + to_string(numSim) + "\n" +
			"XPOS : " + to_string(xVal) + "\n" +
			"YPOS : " + to_string(yVal) + "\n" +
			"ZPOS : " + to_string(zVal) + "\n" +
			"ZACTIVE : " + to_string(GLWINDOW->zMove);
		writeSettings.write(line.c_str(), strlen(line.c_str()));
		writeSettings.close();
		GLWINDOW->saveGSettings();
		WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
		WaitForSingleObject(simThread, INFINITE);
		CloseHandle(simThread);

		// Clean up memory like a good programmer
		delete zBox;
		delete xInput;
		delete yInput;
		delete zInput;
		delete xMM;
		delete yMM;
		delete zMM;
		delete output;
		delete nSim;
		delete submit;
		delete reset;
		delete results;
		delete windowMenu;
		delete GLWINDOW;
		delete window;
		delete oBuffer;

		// Close the program
		exit(0);
	}
}

/*********************************************************************
* Save Callback Function                                             *
* - Used by the save results menu item and by the save results       *
*   button in the main window                                        *
*********************************************************************/
void save_cb(Fl_Widget* widget, void*) {
	boolean overwrite = false;
	// Loop until we get a valid choice
	while (!overwrite) {
		// Choose a file
		char *fName = fl_file_chooser("Save File As?", "*", "dice_sim_results.txt");
		// Check to see if user canceled save
		if (fName != NULL) {
			ifstream exists(fName);
			// Check to see if file exists before writing
			if (exists) {
				// Confirm overwrite if it exists

				if (fl_ask("This file already exists, do you want to overwrite?")) {
					// If user is OK with overwriting, output to file
					oBuffer->outputfile(fName, 0, oBuffer->length());
					overwrite = true;
				}
			}
			// If file didn't exist, then there's nothing to confirm, so output to file
			else {
				oBuffer->outputfile(fName, 0, oBuffer->length());
				overwrite = true;
			}
		}
		// if save WAS cancelled, we're done!
		else {
			overwrite = true;
		}
	}
}

/*********************************************************************
* Significant Digit 1 Callback Function                              *
* - Used by the sigdig1 menu item                                    *
* - checks and unchecks the appropriate menu items                   *
* - sets sigDigits to the appropriate value                          *
* - updates x-input, y-input, and z-input accordingly                *
*********************************************************************/
void sdig1_cb(void) {
	menuContents[7].check();
	menuContents[8].uncheck();
	menuContents[9].uncheck();
	menuContents[10].uncheck();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	sigDigits = 1;
	xInput->value(parseString(xVal, sigDigits).c_str());
	yInput->value(parseString(yVal, sigDigits).c_str());
	zInput->value(parseString(zVal, sigDigits).c_str());
}

/*********************************************************************
* Significant Digit 2 Callback Function                              *
* - Used by the sigdig2 menu item                                    *
* - checks and unchecks the appropriate menu items                   *
* - sets sigDigits to the appropriate value                          *
* - updates x-input, y-input, and z-input accordingly                *
*********************************************************************/
void sdig2_cb(void) {
	menuContents[7].uncheck();
	menuContents[8].check();
	menuContents[9].uncheck();
	menuContents[10].uncheck();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	sigDigits = 2;
	xInput->value(parseString(xVal, sigDigits).c_str());
	yInput->value(parseString(yVal, sigDigits).c_str());
	zInput->value(parseString(zVal, sigDigits).c_str());
}

/*********************************************************************
* Significant Digit 3 Callback Function                              *
* - Used by the sigdig3 menu item                                    *
* - checks and unchecks the appropriate menu items                   *
* - sets sigDigits to the appropriate value                          *
* - updates x-input, y-input, and z-input accordingly                *
*********************************************************************/
void sdig3_cb(void) {
	menuContents[7].uncheck();
	menuContents[8].uncheck();
	menuContents[9].check();
	menuContents[10].uncheck();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	sigDigits = 3;
	xInput->value(parseString(xVal, sigDigits).c_str());
	yInput->value(parseString(yVal, sigDigits).c_str());
	zInput->value(parseString(zVal, sigDigits).c_str());
}

/*********************************************************************
* Significant Digit 4 Callback Function                              *
* - Used by the sigdig4 menu item                                    *
* - checks and unchecks the appropriate menu items                   *
* - sets sigDigits to the appropriate value                          *
* - updates x-input, y-input, and z-input accordingly                *
*********************************************************************/
void sdig4_cb(void) {
	menuContents[7].uncheck();
	menuContents[8].uncheck();
	menuContents[9].uncheck();
	menuContents[10].check();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	sigDigits = 4;
	xInput->value(parseString(xVal, sigDigits).c_str());
	yInput->value(parseString(yVal, sigDigits).c_str());
	zInput->value(parseString(zVal, sigDigits).c_str());
}

/*********************************************************************
* Reset Settings Callback Function                                   *
* - Used by the reset settings menu item                             *
* - Resets main window settings to default value, does not reset     *
*   graphics settings                                                *
*********************************************************************/
void reset_cb(void) {
	sigDigits = 2;
	numSim = 10;
	xVal = 0.0f;
	yVal = 0.0f;
	zVal = 0.0f;
	zActive = false;
	menuContents[7].uncheck();
	menuContents[8].check();
	menuContents[9].uncheck();
	menuContents[10].uncheck();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	xInput->value(parseString(xVal, sigDigits).c_str());
	yInput->value(parseString(yVal, sigDigits).c_str());
	zInput->value(parseString(zVal, sigDigits).c_str());
	nSim->value(to_string(numSim).c_str());
	GLWINDOW->zMove = zActive;
	GLWINDOW->COG_X = xVal;
	GLWINDOW->COG_Y = yVal;
	GLWINDOW->COG_Z = zVal;
	GLWINDOW->take_focus();
	GLWINDOW->redraw();
}

/*********************************************************************
* Run Simulation Callback Function                                   *
* - Used by the start Sim menu function as well as the start Sim     *
*   button in the main window                                        *
* - Deactivates the Start sim button and menu items and then         *
*   activates the handle for the thread which controls running the   *
*   simulation                                                       *
*********************************************************************/
void run_sim_cb(Fl_Widget* widget, void*) {
	// TODO: Much of this will be implemented after linking the physics engine
	menuContents[12].deactivate();
	submit->deactivate();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	simStart = true;
}

/*********************************************************************
* Reset Simulation Callback Function                                 *
* - Used by the reset simulation menu item as well as the reset      *
*   simulation menu button on the main window                        *
* - Deactivates the reset simulation button and menu item as well as *
*   the save results button and menu item                            *
* - Re-activates the start sim button and menu item                  *
* - Clears output buffer                                             *
*********************************************************************/
void reset_sim_cb(Fl_Widget* widget, void*) {
	if (!fl_ask("Do you want to keep current settings loaded?")) {
		reset_cb();
		reset_graphics_cb();
	}
	// Clear buffer
	oBuffer->remove(0, oBuffer->length());

	// Deactivate reset menu item and button as well as save menu item
	// and button
	// Activate save menu item and button
	menuContents[1].deactivate();
	menuContents[13].deactivate();
	menuContents[12].activate();
	windowMenu->clear();
	windowMenu->copy(menuContents);
	reset->deactivate();
	results->deactivate();
	submit->activate();
}

/*********************************************************************
* Reset Graphics Callback Function                                   *
* - Used by the reset graphics menu option                           *
* - Resets GLWINDOW to default view i.e. facing front                *
* - does NOT reset center of gravity position                        *
*********************************************************************/
void reset_graphics_cb(void) {
	GLWINDOW->resetView();
	GLWINDOW->zMove = false;
}

/*********************************************************************
* Help Callback Function                                             *
* - Used by the help menu option                                     *
* - Brings up the user manual                                        *
*********************************************************************/
void help_cb(void) {
	system("notepad.exe ./assets/README.txt");
}
