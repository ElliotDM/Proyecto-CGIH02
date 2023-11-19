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

	// Funciones para el cambio de camara
	GLboolean getCamaraIsometrica() { return camaraIsometrica; }
	GLboolean getCamaraAvatar() { return camaraAvatar; }
	GLboolean getCamaraTopDown() { return camaraTopDown; }

	// Funciones para el control del mouse
	GLboolean getScroll() { return scroll; }

	// Funciones para las animaciones
	GLboolean getReset() { return reset; }
	GLvoid setReset(bool m) { reset = m; }

	GLboolean getMoneda() { return moneda; }
	GLvoid setMoneda(bool m) { moneda = m; }

	GLboolean getResorte() { return resorte; }

	// Funciones para los flippers
	GLfloat getFlipper1() { return angulo_flipper1; }
	GLfloat getFlipper2() { return angulo_flipper2; }

	// Funciones para las luces
	GLboolean getLampara() { return lampara; }
	GLboolean getlightFlippers() { return lightFlippers; }
	GLboolean gethierarchicalObject() { return hierarchicalObject; }


	bool getShouldClose()
	{
		return glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	// Banderas para las camaras
	GLboolean camaraIsometrica = true;
	GLboolean camaraAvatar = false;
	GLboolean camaraTopDown = false;

	// Banderas de control del mouse
	GLboolean rightButton = false;
	GLboolean scroll = false;

	// Banderas de control para la animacion
	GLboolean moneda = false;
	GLboolean reset = false;
	GLboolean resorte = false;

	// Banderas para las luces
	GLboolean lampara = true;
	GLboolean lightFlippers = true;
	GLboolean hierarchicalObject = true;

	// Banderas de control para los flippers
	GLfloat angulo_flipper1 = 0.0;
	GLfloat angulo_flipper2 = 0.0;

	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void ManejaClick(GLFWwindow* window, int button, int action, int mods);
	static void ManejaScroll(GLFWwindow* window, double xoffset, double yoffset);
};