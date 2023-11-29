#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	// Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fall� inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	// Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Pinball of Pain", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	// Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	// MANEJAR TECLADO y MOUSE
	createCallbacks();

	// permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fall� inicializaci�n de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); // HABILITAR BUFFER DE PROFUNDIDAD
	//  Asignar valores de la ventana y coordenadas

// Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	// Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
	glfwSetMouseButtonCallback(mainWindow, ManejaClick);
	glfwSetScrollCallback(mainWindow, ManejaScroll);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	/* Camaras */
	// Camara isometrica
	if (key == GLFW_KEY_I)
	{
		theWindow->camaraIsometrica = true;
		theWindow->camaraAvatar = false;
		theWindow->camaraTopDown = false;
	}

	// Camara ligada al avatar
	if (key == GLFW_KEY_O)
	{
		theWindow->camaraIsometrica = false;
		theWindow->camaraAvatar = true;
		theWindow->camaraTopDown = false;
	}

	// Camara Top Down
	if (key == GLFW_KEY_P)
	{
		theWindow->camaraIsometrica = false;
		theWindow->camaraAvatar = false;
		theWindow->camaraTopDown = true;
	}

	/* Control de animaciones */

	// Insertar moneda
	if (key == GLFW_KEY_M)
	{
		theWindow->moneda = true;
	}

	// Resetea la animacion
	if (key == GLFW_KEY_R)
	{
		theWindow->reset = true;
	}

	// Elegir ruta
	if (key == GLFW_KEY_4)
	{
		theWindow->ruta = true;
	}

	if (key == GLFW_KEY_5)
	{
		theWindow->ruta = false;
	}

	/* Flippers */
	if (key == GLFW_KEY_Z and action == GLFW_PRESS)
	{
		theWindow->angulo_flipper1 = 60.0;
	}
	else if (key == GLFW_KEY_Z and action == GLFW_RELEASE)
	{
		theWindow->angulo_flipper1 = 0.0;
	}

	if (key == GLFW_KEY_X and action == GLFW_PRESS)
	{
		theWindow->angulo_flipper2 = 60.0;
		theWindow->flipper = true;
	}
	else if (key == GLFW_KEY_X and action == GLFW_RELEASE)
	{
		theWindow->angulo_flipper2 = 0.0;
		theWindow->flipper = false;
	}

	/* Luces */

	//Encender y apagar lampara principal
	if (key == GLFW_KEY_B)
	{
		theWindow->lampara = true;
	}
	else if (key == GLFW_KEY_N)
	{
		theWindow->lampara = false;
	}
	//Encender y apagar luz de los flippers
	if (key == GLFW_KEY_T)
	{
		theWindow->lightFlippers = true;
	}
	else if (key == GLFW_KEY_Y)
	{
		theWindow->lightFlippers = false;
	}

	//Para manipular el primer objeto
	if (key == GLFW_KEY_8)
	{
		theWindow-> Object1 = true;
		theWindow-> Object2 = false;
		theWindow-> Object3 = false;
	}
	//Para manipular al segundo objeto
	if (key == GLFW_KEY_9)
	{
		theWindow-> Object1 = false;
		theWindow-> Object2 = true;
		theWindow-> Object3 = false;
	}
	//Para manipular al tercer objeto
	if (key == GLFW_KEY_0)
	{
		theWindow-> Object1 = false;
		theWindow-> Object2 = false;
		theWindow-> Object3 = true;
	}
	if (theWindow -> getObject1()) {
		if (key == GLFW_KEY_G)
		{
			theWindow->hierarchicalObject = true;
		}
		else if (key == GLFW_KEY_H)
		{
			theWindow->hierarchicalObject = false;
		}
	}
	else if (theWindow-> getObject2()) {
		if (key == GLFW_KEY_G)
		{
			theWindow->hierarchicalObject2 = true;
		}
		else if (key == GLFW_KEY_H)
		{
			theWindow->hierarchicalObject2 = false;
		}
	}
	else if (theWindow-> getObject3()) {
		if (key == GLFW_KEY_G)
		{
			theWindow->hierarchicalObject3 = true;
		}
		else if (key == GLFW_KEY_H)
		{
			theWindow->hierarchicalObject3 = false;
		}
	}

	/* Avatar */
	if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D)
	{
		if (theWindow->pataDer > 40.0)
		{
			theWindow->pataDer -= 5.0;
			theWindow->pataIzq += 5.0;
		}
		else
		{
			theWindow->pataDer += 5.0;
			theWindow->pataIzq -= 5.0;
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			// printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			// printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void Window::ManejaClick(GLFWwindow* window, int button, int action, int mods)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		theWindow->rightButton = true;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		theWindow->rightButton = false;

	if (theWindow->rightButton)
		theWindow->resorte = true;
	else
		theWindow->resorte = false;
}

void Window::ManejaScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (yoffset > 0)
		theWindow->scroll = true;
	else
		theWindow->scroll = false;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}