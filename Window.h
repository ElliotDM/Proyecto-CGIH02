#pragma once
#include <stdio.h>
#include <glew.h>
#include <glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();

	GLboolean getResorte() { return resorte; }
	GLboolean getAnimacion() { return animacion; }

	GLfloat getFlipper1() { return angulo_flipper1; }
	GLvoid setFlipper1(float a) { angulo_flipper1 = a; }

	GLfloat getFlipper2() { return angulo_flipper2; }
	GLvoid setFlipper2(float a) { angulo_flipper2 = a; }

	GLfloat getFlipper3() { return angulo_flipper3; }
	GLvoid setFlipper3(float a) { angulo_flipper3 = a; }


	bool getShouldClose()
	{
		return glfwWindowShouldClose(mainWindow);
	}
	bool *getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLboolean rButton = false;
	bool mouseFirstMoved;

	GLboolean resorte = false;
	GLboolean animacion = false;

	GLfloat angulo_flipper1 = 0.0;
	GLfloat angulo_flipper2 = 0.0;
	GLfloat angulo_flipper3 = 0.0;

	static void ManejaTeclado(GLFWwindow *window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow *window, double xPos, double yPos);
	static void ManejaClick(GLFWwindow* window, int button, int action, int mods);
};
