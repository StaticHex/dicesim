#pragma once
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <FL/fl.h>
#include <FL/gl.h>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/GLU.h>
using namespace std;

class GlWindow : public Fl_Gl_Window {
	void draw();
	int	handle(int);
public:
	int			windowWidth;	// Holds the width of the window
	int			windowHeight;	// Holds the height of the window
	float		COG_X;			// Holds the X-position for the center of gravity
	float		COG_Y;			// Holds the Y-position for the center of gravity
	float		COG_Z;			// Holds the Z-posiiton for the center of gravity
	boolean		zMove;			// Holds whether controls affect the Y or Z axis
	glm::vec3	_eye;			// Vector which holds the location of the eye
	glm::vec3	_center;		// Vector which holds the location of the center
	glm::vec3	_up;			// Vector which holds the direction of up
	glm::vec3	_look;			// Vector which holds the direction we're looking (orthogonal to up and right)
	glm::vec3	_right;			// Vector which holds the direction of right (orthogonal to up and look)
	GlWindow(int X, int Y, int W, int H, const char* L) : Fl_Gl_Window(X, Y, W, H, L) { 
		// Set the center of gravity
		COG_X = COG_Y = COG_Z = 0;

		// Start by controlling the Y-axis
		zMove = false;

		// Store the window's height and width (used to keep mouse controls in bounds)
		windowWidth = W; 
		windowHeight = H; 

		// Set the rotation speed and pan speed
		rot_speed = 0.035f;
		pan_speed = 0.1f;

		// set initial values for the last position and velocity of the cursor
		lPos = glm::vec2(0.0f, 0.0f);
		dV = glm::vec2(0.0f, 0.0f);

		// Set initial positions for camera vectors
		_eye = glm::vec3(0.0f, 0.0f, 3.0f);
		_center = glm::vec3(0, 0, 0);
		_up = glm::vec3(0.0f, 1.0f, 0.0f);
		_look = glm::vec3(0.0f, 0.0f, -1.0f);
		_right = glm::vec3(1.0, 0.0f, 0.0f);

		// set up OpenGL to render
		mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	};
	void drawScene(float x, float y, float z);
	void resetView();
	void loadGSettings();
	void saveGSettings();
private:
	glm::vec2	lPos;		// last Position, holds the cursor's previous x and y position
	glm::vec2	dV;			// delta Vector, stores the change in the mouse cursor's position between this update and the last
	float		rot_speed;	// Constant used to help set the speed of rotation for the die (helps smooth the rotation out)
	float		pan_speed;	// Constant used to help set the speed of panning for the die (helps to smooth movement out)
	void rotateEye(glm::vec3 AOR, float angle);
	void setLook(glm::vec3 ePos, glm::vec3 cPos, glm::vec3 uPos);
	void drawXAxis(float x, float y, float z);
	void drawYAxis(float x, float y, float z);
	void drawZAxis(float x, float y, float z);
	void drawCOG(float x, float y, float z);
	void drawDice(float x, float y, float z);
	void drawXLabel(float x, float y, float z);
	void drawYLabel(float x, float y, float z);
	void drawZLabel(float x, float y, float z);
};
