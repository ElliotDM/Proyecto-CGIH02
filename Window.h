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

	GLboolean getCamaraJugador() { return camaraJugador; }
	GLboolean getCamaraAvatar() { return camaraAvatar; }

	GLboolean getScroll() { return scroll; }
	GLboolean getRetroceder() { return retroceder; }

	GLboolean getMoneda() { return moneda; }
	GLvoid setMoneda(bool m) { moneda = m; }

	GLboolean getReset() { return reset; }
	GLvoid setReset(bool m) { reset = m; }

	GLboolean getResorte() { return resorte; }

	GLfloat getFlipper1() { return angulo_flipper1; }
	GLfloat getFlipper2() { return angulo_flipper2; }

	// Funciones para el avatar
	GLfloat getAvatarX() { return avatarX; }
	GLfloat getAvatarY() { return avatarY; }
	GLfloat getAvatarZ() { return avatarZ; }

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

	GLboolean camaraJugador = true;
	GLboolean camaraAvatar = false;

	GLboolean rightButton = false;
	GLboolean scroll = false;

	GLboolean moneda = false;
	GLboolean reset = false;
	GLboolean resorte = false;

	//Banderas para las luces
	GLboolean lampara = true;
	GLboolean lightFlippers = true;
	GLboolean hierarchicalObject = true;

	GLfloat angulo_flipper1 = 0.0;
	GLfloat angulo_flipper2 = 0.0;

	GLfloat avatarX = 22.75;
	GLfloat avatarY = 48;
	GLfloat avatarZ = 24;
	GLboolean retroceder = false;

	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void ManejaClick(GLFWwindow* window, int button, int action, int mods);
	static void ManejaScroll(GLFWwindow* window, double xoffset, double yoffset);
};