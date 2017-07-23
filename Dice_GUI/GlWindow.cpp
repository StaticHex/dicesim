#include "GlWindow.h"

/*********************************************************************
* Draw Function Override                                             *
* - Overrides default GlWindow draw function and draws the center of *
*   gravity, our die, and the three axes                             *
**********************************************************************/
void GlWindow::draw() {
	// Set up our viewport
	if (!valid()) {
		glViewport(0, 0, GlWindow::windowWidth, GlWindow::windowHeight);
		glClearColor(.1f, .1f, .1f, 1);
		glEnable(GL_DEPTH_TEST);
		valid();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the color and depth buffer

	// View Transformations
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->setLook(this->_eye, this->_center, this->_up);

	// draw our scene (cube, axes, center of gravity, etc.)
	GlWindow::drawScene(COG_X, COG_Y, COG_Z);
}

/*********************************************************************
* GlWindow Handle Override                                           *
* - Overrides default GlWindow handle, determines how to handle user *
*   input (basic event handling)                                     *
**********************************************************************/
int GlWindow::handle(int event) {
	switch (event) {
	case FL_PUSH:
		return 1;
	case FL_ENTER:
		this->take_focus();
		return 1;
	case FL_DRAG:
		// Right mouse button
		if (Fl::event_state() == FL_BUTTON3 || Fl::event_state() == FL_BUTTON2) {
			// Holds the current Cursor position
			glm::vec2	cPos = glm::vec2((Fl::event_x() / (Fl::w() / 2.0f))*this->windowWidth, (Fl::event_y() / (Fl::h() / 2.0))*this->windowHeight);
			dV = cPos - lPos;						// use th current position and the last position to calculate cursor velocity 
			float		dVL = glm::length(dV);		// Holds the length of the delta vector
			int			maxX = this->windowWidth;	// Holds the maximum allowed X position
			int			maxY = this->windowHeight;	// Holds the maximum allowed Y position
			if (cPos.x >= 0 && (cPos.x <= maxX) && cPos.y >= 0 && (cPos.y <= maxY)) {
				// Rotate the die and then redraw to update view
				this->rotateEye(_up, rot_speed * (dV.x / dVL) );
				this->rotateEye(_right, rot_speed * (dV.y / dVL));
				this->setLook(_eye, _center, _up);
				this->redraw();
			}
			lPos = cPos;
		}
		// Left mouse button
		else if (Fl::event_state() == FL_BUTTON1) {
			// Holds the current Cursor position
			glm::vec2 cPos		= glm::vec2((Fl::event_x() / (Fl::w() / 2.0f))*this->windowWidth, (Fl::event_y() / (Fl::h() / 2.0))*this->windowHeight);
						dV		= cPos - lPos;			// use th current position and the last position to calculate cursor velocity 
			float		newX	= this->COG_X - dV.x;	// Calculate the new X-position for the center of gravity using the delta vector
			float		newY;							// holds the new Y-position for the center of gravity
			float		newZ;							// Holds the new Z-position for the center of gravity
			// Update the center of gravity's X-position
			this->COG_X = (abs(newX) <= 9.0) ? newX : (newX / abs(newX)*9.0);
			// If currently set to update Z-axis...
			if (zMove) {
				newZ = this->COG_Z - dV.y; // Calculate the new Z-position for the center of gravity using the delta vector
				this->COG_Z = (abs(newZ) <= 9.0) ? newZ : (newZ / abs(newZ)*9.0); // Update the center of gravity's Z-position
			}
			// If currently set to update Y-axis...
			else {
				newY = this->COG_Y - dV.y; // Calculate the new Y-position for the center of gravity using the delta vector
				this->COG_Y = (abs(newY) <= 9.0) ? newY : (newY / abs(newY)*9.0); // Update the center of gravity's Y-position
			}
			this->redraw();
			lPos = cPos;
		}
		return 1;
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
	case FL_KEYBOARD:
		// Z --- Change whether controls affect Z axis or Y axis
		if (Fl::event_key() == 'z') {
			zMove = !zMove;
		}
		// W --- Move Z/Y axis up
		if (Fl::event_key() == 'w') {
			if (zMove) {
				float newZ = COG_Z + pan_speed;
				COG_Z = (abs(newZ) <= 9.0) ? newZ : (newZ / abs(newZ)*9.0);
			}
			else {
				float newY = COG_Y + pan_speed;
				COG_Y = (abs(newY) <= 9.0) ? newY : (newY / abs(newY)*9.0);
			}
			redraw();
		}
		// A --- Move X axis Left
		if (Fl::event_key() == 'a') {
			float newX = COG_X + pan_speed;
			COG_X = (abs(newX) <= 9.0) ? newX : (newX / abs(newX)*9.0);
			redraw();
		}
		// S --- Move Z/Y axis down
		if (Fl::event_key() == 's') {
			if (zMove) {
				float newZ = COG_Z - pan_speed;
				COG_Z = (abs(newZ) <= 9.0) ? newZ : (newZ / abs(newZ)*9.0);
			}
			else {
				float newY = COG_Y - pan_speed;
				COG_Y = (abs(newY) <= 9.0) ? newY : (newY / abs(newY)*9.0);
			}
			redraw();
		}
		// D --- Move X axis right
		if (Fl::event_key() == 'd') {
			float newX = COG_X - pan_speed;
			COG_X = (abs(newX) <= 9.0) ? newX : (newX / abs(newX)*9.0);
			redraw();
		}
		// UP -- Rotate die up
		if (Fl::event_key() == FL_Up) {
			rotateEye(_right, pan_speed);
			redraw();
		}
		// DOWN -- Rotate die down
		if (Fl::event_key() == FL_Down) {
			rotateEye(_right, -pan_speed);
			redraw();
		}
		// LEFT -- Rotate die left
		if (Fl::event_key() == FL_Left) {
			rotateEye(_up, pan_speed);
			redraw();
		}
		// RIGHT -- Rotate die right
		if (Fl::event_key() == FL_Right) {
			rotateEye(_up, -pan_speed);
			redraw();
		}
		return 1;
	default:
		// pass other events to the base class...
		return Fl_Gl_Window::handle(event);
	}
	return 0;
}

/*********************************************************************
* Draw Scene Function                                                *
* - Uses center of gravity positon and then calls on helper          *
*   functions which update GLWINDOW's contents                       *
**********************************************************************/
void GlWindow::drawScene(float x, float y, float z) {
	// Translate coordinates into cube coordinates
	float cX = x / 9.0f;
	float cY = y / 9.0f;
	float cZ = z / 9.0f;

	// Draw the center of gravity
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawCOG(cX, cY, cZ);
	glDisable(GL_BLEND);

	// Draw the axes
	drawXAxis(cX, cY, cZ);
	drawYAxis(cX, cY, cZ);
	drawZAxis(cX, cY, cZ);

	// Label our axes
	drawXLabel(cX, cY, cZ);
	drawYLabel(cX, cY, cZ);
	drawZLabel(cX, cY, cZ);
	glEnable(GL_BLEND);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw the dice
	drawDice(cX, cY, cZ);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

/*********************************************************************
* Set Look Function                                                  *
* - Uses the eye, the center and the up vectors for the camera to    *
*   set where the camera is looking                                  *
* - Helper function which is used by the GlWindow's handle           *
**********************************************************************/
void GlWindow::setLook(glm::vec3 ePos, glm::vec3 cPos, glm::vec3 uPos) {
	gluLookAt(ePos.x, ePos.y, ePos.z, cPos.x, cPos.y, cPos.z, uPos.x, uPos.y, uPos.z);
}

/*********************************************************************
* Rotate Eye Function                                                *
* - Uses the camera's right, up, look, and eye vectors to create a   *
*   rotation matrix and then rotate the eye around the axis of       *
*   rotation by the given angle                                      *
* - Helper function which is used by the GlWindow's handle           *
**********************************************************************/
void GlWindow::rotateEye(glm::vec3 AOR, float angle) {
	glm::mat4x4 V = { _right.x, _up.x, _look.x, _eye.x,
					  _right.y, _up.y, _look.y, _eye.y,
					  _right.z, _up.z, _look.z, _eye.z,
					   0.0f   ,  0.0f,  0.0f  ,  1.0f };

	V = glm::rotate(V, angle, AOR);
	_right = glm::vec3(V[0][0], V[1][0], V[2][0]);
	_up = glm::vec3(V[0][1], V[1][1], V[2][1]);
	_look = glm::vec3(V[0][2], V[1][2], V[2][2]);
	_eye = glm::vec3(V[0][3], V[1][3], V[2][3]);
}

/*********************************************************************
* Draw Center of Gravity Function                                    *
* - Responsible for positioning and drawing the sphere representing  *
*   the location of the center of gravity                            *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawCOG(float x, float y, float z) {
	// Position our COG
	glTranslatef(-x, y, z);

	// Set color to cyan
	glColor4f(0.0f, 1.0f, 1.0f, 0.5f);

	// Draw our center of gravity
	glBegin(GL_LINES);
	gluSphere(gluNewQuadric(), .15, 100, 100);
	glEnd();
}

/*********************************************************************
* Draw X-Axis Function                                               *
* - Responsible for positioning and drawing the line representing    *
*   the X-axis                                                       *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawXAxis(float x, float y, float z) {
	// Position x-axis 
	glTranslatef(x, 0.0f, 0.0f);

	// Set color to yellow
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	// Set line width
	glLineWidth(1.0);

	// draw x-axis
	glBegin(GL_LINES);
	glVertex3f(1.5f, 0.0f, 0.0f);
	glVertex3f(-1.5f, 0.0f, 0.0f);
	glEnd();

	// Translate back to our original position after drawing
	glTranslatef(-x, 0.0f, 0.0f);
}

/*********************************************************************
* Draw Y-Axis Function                                               *
* - Responsible for positioning and drawing the line representing    *
*   the Y-axis                                                       *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawYAxis(float x, float y, float z) {
	// Position y-axis 
	glTranslatef(0.0f, -y, 0.0f);

	// set color to cyan
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	// set line width
	glLineWidth(1.0);

	// draw y-axis
	glBegin(GL_LINES);
	glVertex3f(0.0f, 1.5f, 0.0f);
	glVertex3f(0.0f, -1.5f, 0.0f);
	glEnd();

	// Translate back to our original position after drawing
	glTranslatef(0.0f, y, 0.0f);
}

/*********************************************************************
* Draw Z-Axis Function                                               *
* - Responsible for positioning and drawing the line representing    *
*   the Z-axis                                                       *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawZAxis(float x, float y, float z) {
	// Position z-axis 
	glTranslatef(0.0f, 0.0f, -z);

	// set color to purple
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	// set line width
	glLineWidth(1.0);

	// draw z-axis
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 1.5f);
	glVertex3f(0.0f, 0.0f, -1.5f);
	glEnd();

	// Translate back to our original position after drawing
	glTranslatef(0.0f, 0.0f, z);
}

/*********************************************************************
* Draw X-Axis Label Function                                         *
* - Responsible for positioning and drawing the Character "X" which  *
*   serves as the label for the X-axis                               *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawXLabel(float x, float y, float z) {
	// Position Label
	glTranslatef(x, 0, 0);

	// Set color to yellow
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw first line in X
	glBegin(GL_LINES);
	glVertex3f(-1.9f, 0.20f, 0.0f);
	glVertex3f(-1.7f, -0.20f, 0.0f);
	glEnd();

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Set color to yellow
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 2nd line in X
	glBegin(GL_LINES);
	glVertex3f(-1.7f, 0.20f, 0.0f);
	glVertex3f(-1.9f, -0.20f, 0.0f);
	glEnd();
	glTranslatef(-x, 0, 0);
}

/*********************************************************************
* Draw Y-Axis Label Function                                         *
* - Responsible for positioning and drawing the Character "Y" which  *
*   serves as the label for the Y-axis                               *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawYLabel(float x, float y, float z) {
	// Position Label
	glTranslatef(0.0f, -y, 0.0f);

	// Set color to cyan
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 1st line in Y
	glBegin(GL_LINES);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glVertex3f(-0.1f, 2.1f, 0.0f);
	glEnd();

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Set color to cyan
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 2nd line in Y
	glBegin(GL_LINES);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glVertex3f(0.1f, 2.1f, 0.0f);
	glEnd();

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Set color to cyan
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 3rd line in Y
	glBegin(GL_LINES);
	glVertex3f(0.0f, 1.7f, 0.0f);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glEnd();
	glTranslatef(0.0f, y, 0.0f);
}

/*********************************************************************
* Draw Z-Axis Label Function                                         *
* - Responsible for positioning and drawing the Character "Z" which  *
*   serves as the label for the Z-axis                               *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawZLabel(float x, float y, float z) {
	// Position Label
	glTranslatef(0.0f, 0.0f, -z);

	// Set color to magenta
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 1st line in Z
	glBegin(GL_LINES);
	glVertex3f(0.0f, -0.2f, 1.7f);
	glVertex3f(0.0f, -0.2f, 1.9f);
	glEnd();

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Set color to magenta
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 2nd line in Z
	glBegin(GL_LINES);
	glVertex3f(0.0f, -0.2f, 1.9f);
	glVertex3f(0.0f,  0.2f, 1.7f);
	glEnd();

	glTranslatef(0.0f, 0.0f, 0.0f);

	// Set color to magenta
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	// Set line width
	glLineWidth(2.0);

	// draw 3rd line in Z
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.2f, 1.7f);
	glVertex3f(0.0f, 0.2f, 1.9f);
	glEnd();

	// reset draw position
	glTranslatef(0.0f, 0.0f, z);
}

/*********************************************************************
* Draw Dice Function                                                 *
* - Responsible for drawing and texturing the dice model in the      *
*   GLWINDOW                                                         *
* - Helper function for the drawScene function                       *
**********************************************************************/
void GlWindow::drawDice(float x, float y, float z) {
	// Load textures for the cube (apparently we have to do this separately because the alias for it is messed up)
	Fl_JPEG_Image one_tex("./assets/textures/one_side.jpg");
	Fl_JPEG_Image two_tex("./assets/textures/two_side.jpg");
	Fl_JPEG_Image three_tex("./assets/textures/three_side.jpg");
	Fl_JPEG_Image four_tex("./assets/textures/four_side.jpg");
	Fl_JPEG_Image five_tex("./assets/textures/five_side.jpg");
	Fl_JPEG_Image six_tex("./assets/textures/six_side.jpg");


	// Move position back to the origin
	glTranslatef(x, -y, -z);

	// Start drawing our die
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, five_tex.w(), five_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, five_tex.data()[0]);

	glBegin(GL_QUADS);
	// front (5-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 1.0); // Top Right Of The Texture and Quad
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 1.0); // Top Left Of The Texture and Quad
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0); // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0); // Bottom Right Of The Texture and Quad
	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, two_tex.w(), two_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, two_tex.data()[0]);

	glBegin(GL_QUADS);
	// back (2-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1, 1, -1);
	glTexCoord2f(0.0, 0.0); glVertex3f(1, 1, -1);
	glTexCoord2f(0.0, 1.0); glVertex3f(1, -1, -1);
	glTexCoord2f(1.0, 1.0); glVertex3f(-1, -1, -1);
	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, one_tex.w(), one_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, one_tex.data()[0]);

	glBegin(GL_QUADS);
	// top (1-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1, 1, -1);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1, 1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(1, 1, 1);
	glTexCoord2f(1.0, 1.0); glVertex3f(1, 1, -1);
	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, six_tex.w(), six_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, six_tex.data()[0]);

	glBegin(GL_QUADS);
	// bottom (6-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1, -1, -1);
	glTexCoord2f(0.0, 0.0); glVertex3f(1, -1, -1);
	glTexCoord2f(0.0, 1.0); glVertex3f(1, -1, 1);
	glTexCoord2f(1.0, 1.0); glVertex3f(-1, -1, 1);
	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, three_tex.w(), three_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, three_tex.data()[0]);

	glBegin(GL_QUADS);
	// left (3-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1, 1, -1);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1, -1);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1, -1, 1);
	glTexCoord2f(1.0, 1.0); glVertex3f(-1, 1, 1);
	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, four_tex.w(), four_tex.h(), 0, GL_RGB, GL_UNSIGNED_BYTE, four_tex.data()[0]);

	glBegin(GL_QUADS);
	// right (4-side)
	glColor4f(1, 1, 1, .4);
	glTexCoord2f(1.0, 0.0); glVertex3f(1, 1, 1);
	glTexCoord2f(0.0, 0.0); glVertex3f(1, -1, 1);
	glTexCoord2f(0.0, 1.0); glVertex3f(1, -1, -1);
	glTexCoord2f(1.0, 1.0); glVertex3f(1, 1, -1);
	glEnd();
}

/*********************************************************************
* Reset View Function                                                *
* - Responsible for resetting the camera vectors to their original   *
*   default positions                                                *
* - Restores view to front facing                                    *
**********************************************************************/
void GlWindow::resetView() {
	_eye = glm::vec3(0.0f, 0.0f, 3.0f);
	_center = glm::vec3(0.0f, 0.0f, 0.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_look = glm::vec3(0.0f, 0.0f, -1.0f);
	_right = glm::vec3(1.0, 0.0f, 0.0f);
	this->setLook(_eye, _center, _up);
	this->redraw();
}

/*********************************************************************
* Load Graphics Settings Function                                    *
* - Checks for the existence of a settings file and loads previous   *
*   settings from file to allow user input to persist after closing  *
*   the program                                                      *
* - If settings file is not found, this function will create a       *
*   settings file and initalize it with default values               *
**********************************************************************/
void GlWindow::loadGSettings() {
	ifstream settings("./assets/gsettings.txt");
	if (!settings) {
		_eye = glm::vec3(0.0f, 0.0f, 3.0f);
		_center = glm::vec3(0, 0, 0);
		_up = glm::vec3(0.0f, 1.0f, 0.0f);
		_look = glm::vec3(0.0f, 0.0f, -1.0f);
		_right = glm::vec3(1.0, 0.0f, 0.0f);
	}
	// If settings already exist, read them in
	else {
		string header;
		string colon;
		string xVal;
		string yVal;
		string zVal;
		while (settings >> header >> colon >> xVal >> yVal >> zVal) {
			if (header.compare("EYE") == 0) {
				_eye = glm::vec3(atof(xVal.c_str()), atof(yVal.c_str()), atof(zVal.c_str()));
			}
			else if (header.compare("CENTER") == 0) {
				_center = glm::vec3(atof(xVal.c_str()), atof(yVal.c_str()), atof(zVal.c_str()));
			}
			else if (header.compare("UP") == 0) {
				_up = glm::vec3(atof(xVal.c_str()), atof(yVal.c_str()), atof(zVal.c_str()));
			}
			else if (header.compare("LOOK") == 0) {
				_look = glm::vec3(atof(xVal.c_str()), atof(yVal.c_str()), atof(zVal.c_str()));
			}
			else if (header.compare("RIGHT") == 0) {
				_right = glm::vec3(atof(xVal.c_str()), atof(yVal.c_str()), atof(zVal.c_str()));
			}
		}
		settings.close();
	}
}

/*********************************************************************
* Save Graphics Settings Function                                    *
* - Outputs current settings to file                                 *
* - Works in tandem with Load Settings to help data persist after    *
*   program closes                                                   *
**********************************************************************/
void GlWindow::saveGSettings() {
	ofstream writeSettings("./assets/gsettings.txt");
	string line = "EYE : " + to_string(_eye.x) + " " + to_string(_eye.y) + " " + to_string(_eye.z) + "\n" +
		"CENTER : " + to_string(_center.x) + " " + to_string(_center.y) + " " + to_string(_center.z) + "\n" +
		"UP : " + to_string(_up.x) + " " + to_string(_up.y) + " " + to_string(_up.z) + "\n" +
		"LOOK : " + to_string(_look.x) + " " + to_string(_look.y) + " " + to_string(_look.z) + "\n" +
		"RIGHT : " + to_string(_right.x) + " " + to_string(_right.y) + " " + to_string(_right.z);
	writeSettings.write(line.c_str(), strlen(line.c_str()));
	writeSettings.close();
}