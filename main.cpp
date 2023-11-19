﻿#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

// Variables para las camaras
float zoomY;
float zoomZ;
float posX;
float posY;
float posZ;
bool enableMouse;

// Variables para el contador de dia y noche
int counterHour;
int counterDay;
bool day;
bool firts_Light;

// Variables y banderas para animacion

// Offsets
float moneda_offset;
float canica_offset;
float rot_offset;

// Moneda
float mov_moneda;
float rot_moneda;
bool animacion;

// Resorte
float zResorte;
float zManija;
bool resorte;
bool espera;

// Canica
float movx_canica;
float movy_canica;
float movz_canica;
float rot_canica;
float t_curva;
bool canica_init;
bool canica_animacion;
bool subida;
bool salida;
bool choca;
bool curva;
bool choca1;

// Wingmoulds
float wm1_izq;
float wm1_der;
float wm1_arc;
float wm1_arc_der_x;
float wm1_arc_der_z;
float wm1_arc_izq_x;
float wm1_arc_izq_z;
int wm1_cont1;
int wm1_cont2;
bool wm1_inicio;

// Sierra
float rotSierra;

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture gabineteTexture;
Texture cristalTexture;
Texture monedaTexture;
Texture canicaTexture;
Texture resorteTexture;
Texture wingmouldTexture;
Texture sierraTexture;

Model Gabinete_M;
Model Cristal_M;
Model Moneda_M;
Model Canica_M;
Model Resorte_M;
Model Manija_M;
Model ManijaRes_M;
Model Flipper_M;
Model WingMould_C;
Model WingMould_L;
Model WingMould_R;
Model Sierra_M;
Model Huevo_M;
Model Nail_M;
Model Dreamers_M;

Skybox skybox;

// Materiales
Material Material_brillante;
Material Material_opaco;
Material Material_metalico;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLightDay;
DirectionalLight mainLightNight;

// luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights1[MAX_POINT_LIGHTS];

// luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funcion de calculo de normales por promedio de vertices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	// Se cargan las texturas de los elementos del pinball
	gabineteTexture = Texture("Textures/prueba.png");
	gabineteTexture.LoadTextureA();
	cristalTexture = Texture("Textures/Glass.tga");
	cristalTexture.LoadTextureA();

	monedaTexture = Texture("Textures/moneda.png");
	monedaTexture.LoadTextureA();
	canicaTexture = Texture("Textures/canica.png");
	canicaTexture.LoadTextureA();
	resorteTexture = Texture("Textures/resorte.png");
	resorteTexture.LoadTextureA();

	wingmouldTexture = Texture("Textures/wingmould.png");
	wingmouldTexture.LoadTextureA();

	sierraTexture = Texture("Textures/saw.png");
	sierraTexture.LoadTextureA();

	// Se cargan los modelos de los elementos del pinball
	Gabinete_M = Model();
	Gabinete_M.LoadModel("Models/gabinete.obj");
	Cristal_M = Model();
	Cristal_M.LoadModel("Models/cristal.obj");
	Manija_M = Model();
	Manija_M.LoadModel("Models/manija.obj");
	ManijaRes_M = Model();
	ManijaRes_M.LoadModel("Models/manija_res.obj");

	Moneda_M = Model();
	Moneda_M.LoadModel("Models/moneda.obj");
	Canica_M = Model();
	Canica_M.LoadModel("Models/canica.obj");
	Resorte_M = Model();
	Resorte_M.LoadModel("Models/resorte.obj");
	Flipper_M = Model();
	Flipper_M.LoadModel("Models/flipper.obj");

	WingMould_C = Model();
	WingMould_C.LoadModel("Models/wingmould_C.obj");
	WingMould_L = Model();
	WingMould_L.LoadModel("Models/wingmould_L.obj");
	WingMould_R = Model();
	WingMould_R.LoadModel("Models/wingmould_R.obj");

	Sierra_M = Model();
	Sierra_M.LoadModel("Models/sierra.obj");
	Huevo_M = Model();
	Huevo_M.LoadModel("Models/huevo.obj");

	Nail_M = Model();
	Nail_M.LoadModel("Models/nail.obj");
	Dreamers_M = Model();
	Dreamers_M.LoadModel("Models/dreamers.obj");

	std::vector<std::string> skyboxFacesDay;
	skyboxFacesDay.push_back("Textures/Skybox/day_lf.png");
	skyboxFacesDay.push_back("Textures/Skybox/day_rt.png");
	skyboxFacesDay.push_back("Textures/Skybox/day_dn.png");
	skyboxFacesDay.push_back("Textures/Skybox/day_up.png");
	skyboxFacesDay.push_back("Textures/Skybox/day_bk.png");
	skyboxFacesDay.push_back("Textures/Skybox/day_ft.png");

	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/night_rt.png");
	skyboxFacesNight.push_back("Textures/Skybox/night_lf.png");
	skyboxFacesNight.push_back("Textures/Skybox/night_dn.png");
	skyboxFacesNight.push_back("Textures/Skybox/night_up.png");
	skyboxFacesNight.push_back("Textures/Skybox/night_bk.png");
	skyboxFacesNight.push_back("Textures/Skybox/night_ft.png");

	skybox = Skybox(skyboxFacesDay);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	Material_metalico = Material(10.0f, 300);

	//luz direccional
	mainLightDay = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.7f, 0.7f,
		0.0f, 0.0f, -1.0f);

	mainLightNight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.2f,
		0.0f, 0.0f, -1.0f);

	//Se carga la primera luz direccional
	shaderList[0].SetDirectionalLight(&mainLightDay);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Pointlight del objeto jeraquico
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		15.5f, 50.9f, -23.5,
		1.0f, 0.2f, 0.1f);
	pointLightCount++;

	// PointLight de los flippers
	pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		8.0f, 48.0f, 21.0f,
		1.0f, 0.02f, 0.01f);
	pointLightCount++;

	pointLights1[0] = pointLights[1];
	pointLights1[1] = pointLights[0];

	//contador de luces spotlight
	unsigned int spotLightCount = 0;

	//SpotLight del Pinball
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		10.0f, 200.0f, -10.0f,
		0.0f, -1.0f, 0.0f,
		0.5f, 0.01f, 0.0f,
		15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//Inicializacion de variables para zoom
	zoomY = 75.0;
	zoomZ = 60.0;
	enableMouse = false;

	// Inicializacion de variables para animacion

	// Offsets
	moneda_offset = 0.03f;
	canica_offset = 0.05f;
	rot_offset = 2.0f;

	// Moneda
	mov_moneda = 33.0f;
	rot_moneda = 0.0f;
	animacion = true;

	// Resorte
	resorte = false;
	espera = false;
	zResorte = 0.3;
	zManija = 50.0;

	// Canica 
	movx_canica = 18.5f;
	movy_canica = 48.0f;
	movz_canica = 24.0f;
	rot_canica = 0.0f;
	t_curva = 0.0f;
	canica_init = false;
	canica_animacion = false;
	subida = true;
	salida = false;
	choca = false;
	curva = false;
	choca1 = false;

	// Wingmoulds
	wm1_inicio = false;
	wm1_izq = -0.5;
	wm1_der = 0.5;
	wm1_arc = 0.0;
	wm1_arc_der_x = 0.0;
	wm1_arc_der_z = 0.0;
	wm1_arc_izq_x = 0.0;
	wm1_arc_izq_z = 0.0;
	wm1_cont1 = 0;
	wm1_cont2 = 0;

	// Sierra
	rotSierra = 0.0;

	// Inicializacion de variables para el contador de dia y noche
	day = true;
	counterHour = 0;
	counterDay = 0;
	firts_Light = true;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();

		// Zoom para camara del jugador
		if (mainWindow.getScroll())
		{
			if (zoomY > 65.0f)
			{
				zoomY -= 0.1 * deltaTime;
				zoomZ -= 0.2 * deltaTime;
			}
			else
			{
			}
		}
		else
		{
			if (zoomY < 75.0f)
			{
				zoomY += 0.1 * deltaTime;
				zoomZ += 0.2 * deltaTime;
			}
			else
			{
			}
		}

		// Condicionales para el cambio de camara

		// Camara isomatrica
		if (mainWindow.getCamaraIsometrica())
		{
			camera = Camera(glm::vec3(8.0f, zoomY, zoomZ), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f, 0.3f, 0.5f);
			enableMouse = false;
		}

		// Camara ligada al avatar

		if (mainWindow.getCamaraAvatar() && !(enableMouse))
		{
			camera = Camera(glm::vec3(22.75, 49.0, 25.0), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f, 0.3f, 0.5f);
			enableMouse = true;
		}

		if (enableMouse)
		{
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		// Camara Top Down
		if (mainWindow.getCamaraTopDown())
		{
			camera = Camera(glm::vec3(8.0f, 110.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, -90.0f, 0.3f, 0.5f);
			enableMouse = false;
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		// informacion en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Informacion al shader de fuentes de iluminacion

		// Condiciones para el cambio entre dia y noche
		// el skybox y la luz direccional cambian cada 20 segundos

		//Se carga la primera luz direccional de dia
		if (firts_Light)
		{
			shaderList[0].SetDirectionalLight(&mainLightDay);
		}

		//Se empieza el conteo para realizar el cambio entre dia y noche
		if ((int)now % 2 == 0)
			counterHour++;
		else
			counterHour = 0;

		if (counterHour == 1)
			counterDay++;

		if (counterDay == 10)
		{
			counterDay = 0;

			if (day)
			{
				shaderList[0].SetDirectionalLight(&mainLightDay);
				skybox = Skybox(skyboxFacesDay);
				day = false;
				firts_Light = false;
			}
			else
			{
				shaderList[0].SetDirectionalLight(&mainLightNight);
				skybox = Skybox(skyboxFacesNight);
				day = true;
			}
		}

		//Para prender y apagar las pointLights
		//shaderList[0].SetPointLights(pointLights, pointLightCount);

		if (mainWindow.getlightFlippers() == false and mainWindow.gethierarchicalObject() == false)
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount - 2);
		}
		else if (mainWindow.getlightFlippers() == false and mainWindow.gethierarchicalObject() == true)
		{
			shaderList[0].SetPointLights(pointLights1, pointLightCount - 1);
		}
		else if (mainWindow.getlightFlippers() == true and mainWindow.gethierarchicalObject() == false)
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
		else if (mainWindow.getlightFlippers() == true and mainWindow.gethierarchicalObject() == true)
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}

		//Solo hay una spotligh, la cual ilumina al pinball
		if (mainWindow.getLampara())
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		/* Animaciones */

		// Reinicio forzado de la animacion
		if (mainWindow.getReset())
		{
			// Moneda
			mov_moneda = 33.0f;
			rot_moneda = 0.0f;
			animacion = true;

			// Resorte
			resorte = false;
			espera = false;
			zResorte = 0.3;

			// Canica 
			movx_canica = 18.5f;
			movy_canica = 48.0f;
			movz_canica = 24.0f;
			rot_canica = 0.0f;
			t_curva = 0.0f;
			canica_init = false;
			canica_animacion = false;
			subida = true;
			salida = false;
			choca = false;
			curva = false;
			choca1 = false;
			mainWindow.setReset(false);
			mainWindow.setMoneda(false);
		}
		else
		{
			// Moneda
			// Inserta moneda y comienza la animacion
			if (mainWindow.getMoneda() && animacion)
			{
				if (mov_moneda > 28.0)
				{
					mov_moneda -= moneda_offset * deltaTime;
					rot_moneda += rot_offset * deltaTime;
				}
				else {
					canica_init = true;
					animacion = false;
					mainWindow.setMoneda(false);
				}
			}
			// Canica (posicionamiento)
			else if (canica_init)
			{
				// La canica rueda y se coloca en el resorte
				if (movx_canica <= 22.75)
				{
					movx_canica += canica_offset * deltaTime;
					rot_canica += rot_offset * deltaTime;
				}
				else
				{
					resorte = true;
					canica_init = false;
				}
			}
			// Resorte
			else if (resorte)
			{
				// Espera a que el usuario compacte el resorte
				if (espera)
				{
					resorte = false;
					espera = false;
					canica_animacion = true;
				}
				// Comprimir el resorte
				else if (mainWindow.getResorte() && zResorte >= 0.1)
				{
					zResorte -= 0.0005;
					zManija += 0.005;
					movz_canica += (canica_offset / 2) * deltaTime;
				}
				// Si el usuario mantiene pulsado el click derecho
				// mantener el resorte comprimido
				else if (mainWindow.getResorte())
				{
				}
				// Regresa el resorte a su posicion inicial
				else if (!(mainWindow.getResorte()) && zResorte < 0.3) {
					zResorte = 0.3;
					zManija = 50.0;
					espera = true;
				}
			}
			// Canica (recorrido)
			else if (canica_animacion)
			{
				// La canica sale disparada
				if (subida)
				{
					if (movz_canica >= -22.5)
					{
						movz_canica -= canica_offset * deltaTime * 4;
						movy_canica += 0.01 * deltaTime;
						rot_canica += rot_offset * deltaTime;
					}
					else
					{
						subida = false;
						salida = true;
					}
				}
				// Curva de salida
				else if (salida)
				{
					// Sigue una trayectoria de un circulo
					if (t_curva <= 3.0)
					{
						if (movy_canica <= 50.6)
						{
							movy_canica += 0.0001;
						}

						t_curva += 0.01;
						movx_canica = 19.5 + (3 * glm::cos(t_curva));
						movz_canica = -22.5 - (3 * glm::sin(t_curva));
					}
					else
					{
						t_curva = -0.4f;
						salida = false;
						choca = true;

						// Aumentar puntuacion (cambiar textura)
					}
				}
				// Canica choca con obstaculo y rebota
				else if (choca)
				{
					// Activa animacion del objeto jerarquico
					if (wm1_inicio) {}
					else
					{
						wm1_inicio = true;
					}

					// Sigue una trayectoria de un circulo
					if (t_curva <= 0.4)
					{
						t_curva += 0.001;

						movx_canica = -10.47 + (glm::sqrt(845.92) * glm::cos(t_curva));
						movy_canica -= 0.005 * deltaTime;
						movz_canica = -12.05 + (glm::sqrt(845.92) * glm::sin(t_curva));
					}
					else
					{
						t_curva = -2.85f;
						choca = false;
						curva = true;
					}
				}
				// Canica continua su camino por el tablero
				else if (curva)
				{
					if (t_curva >= -3.1)
					{
						t_curva -= 0.0005;
						movx_canica = 73.12 + (glm::sqrt(3503.15) * glm::cos(t_curva));
						movy_canica -= 0.005 * deltaTime;
						movz_canica = 15.88 + (glm::sqrt(3503.15) * glm::sin(t_curva));
					}
					else
					{
						t_curva = -0.46f;
						curva = false;
						choca1 = true;
					}
				}
				// Canica choca y cae entre los flippers
				else if (choca1)
				{
					if (t_curva >= -3.4)
					{
						if (movx_canica >= 11.11)
						{
							movy_canica += 0.005 * deltaTime;
						}
						else if (movy_canica >= 48.35)
						{
							movy_canica -= 0.005 * deltaTime;
						}

						t_curva -= 0.01;
						movx_canica = 11.11 + (glm::sqrt(10.32) * glm::cos(t_curva));
						movz_canica = 14.21 + (glm::sqrt(10.32) * glm::sin(t_curva));
					}
					else
					{
						if (movz_canica <= 26.0)
						{
							movz_canica += canica_offset * deltaTime * 4;
							movy_canica -= 0.01 * deltaTime;
						}
						else
						{
							choca1 = false;
							canica_animacion = false;
						}

					}
				}
			}
			// La animacion cominza de nuevo
			else
			{
				animacion = true;

				// Moneda
				mov_moneda = 33.0f;
				rot_moneda = 0.0f;
				animacion = true;

				// Resorte
				resorte = false;
				espera = false;
				zResorte = 0.3;

				// Canica 
				movx_canica = 18.5f;
				movy_canica = 48.0f;
				movz_canica = 24.0f;
				rot_canica = 0.0f;
				t_curva = 0.0f;
				canica_init = false;
				subida = true;
				salida = false;
				choca = false;
				curva = false;
				choca1 = false;
			}
		}

		// Animacion del objeto jerarquico
		if (wm1_inicio) {
			// Caparazon se separa
			if (wm1_der < 1.2) {
				wm1_der += 0.05;
				wm1_izq -= 0.05;
			}
			// Caparazon rota
			else if (wm1_arc_izq_z < 30)
			{
				wm1_arc_der_z += 5;
				wm1_arc_izq_z += 5;
			}
			// Interior rota 
			else if (wm1_arc < 40)
			{
				wm1_arc_izq_z += 1;
				wm1_arc += 5;
			}
			// Caparazon derecho rota
			else if (wm1_arc_der_z < 40)
			{
				wm1_arc_der_z += 5;
			}
			// Comienza a vibrar durante 2 segundos
			else if ((int)now % 2 == 0)
			{
				wm1_cont1++;
			}
			else
			{
				wm1_cont1 = 0;
			}

			if (wm1_cont1 == 1)
				wm1_cont2++;

			if (wm1_cont2 == 1)
			{
				wm1_cont2 = 0;
				wm1_inicio = false;
			}
			else
			{
				//La luz se apagara independientemente del estado
				shaderList[0].SetPointLights(pointLights1, pointLightCount - 1);
				if (wm1_arc_der_x <= 30.0)
				{
					wm1_arc_der_x += 10.0;
					wm1_arc_izq_x -= 10.0;
				}
				else if (wm1_arc_izq_x <= 0.0)
				{
					wm1_arc_der_x -= 10.0;
					wm1_arc_izq_x += 10.0;
				}
			}
		}
		// Regresa a su estado inicial
		else
		{
			wm1_izq = -0.5;
			wm1_der = 0.5;
			wm1_arc = 0.0;
			wm1_arc_der_x = 0.0;
			wm1_arc_der_z = 0.0;
			wm1_arc_izq_x = 0.0;
			wm1_arc_izq_z = 0.0;
			wm1_cont1 = 0;
			wm1_cont2 = 0;
		}

		/* Modelos */

		//Aguijon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, 52.0f, -10.0f));
		model = glm::rotate(model, -70 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nail_M.RenderModel();

		//soñadores
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 49.0f, 26.0f));
		model = glm::rotate(model, 3 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dreamers_M.RenderModel();

		//soñadores
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(16.0f, 49.0f, 26.0f));
		model = glm::rotate(model, 3 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dreamers_M.RenderModel();

		// Moneda
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(13.0f, 39.5f, mov_moneda));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rot_moneda * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Moneda_M.RenderModel();

		// Resorte
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(22.75f, 48.0f, 27.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, zResorte));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Resorte_M.RenderModel();

		// Resorte manija
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(22.8f, 47.2f, 32.4f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, zManija));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ManijaRes_M.RenderModel();

		// Manija
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(22.8f, 47.1f, 32.7f - zResorte));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Manija_M.RenderModel();

		// Canica
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(movx_canica, movy_canica, movz_canica));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rot_canica * toRadians, glm::vec3(1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Canica_M.RenderModel();
		Material_metalico.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// Flipper 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.5f, 48.0f, 21.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::rotate(model, 10 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mainWindow.getFlipper1() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel();

		// Flipper 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(12.5f, 48.0f, 21.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::rotate(model, -100 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -mainWindow.getFlipper2() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel();

		// Flipper 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.7f, 50.1f, -13.2f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -mainWindow.getFlipper2() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel();

		/* Obstaculos */

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(15.2f, 50.9f, -23.5f));
		modelaux = model;
		model = glm::rotate(model, wm1_arc * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_C.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(wm1_izq, 0.3f, -0.2f));
		model = glm::rotate(model, wm1_arc_der_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, wm1_arc_der_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_L.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(wm1_der, 0.3f, -0.2f));
		model = glm::rotate(model, wm1_arc_izq_x * toRadians, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -wm1_arc_izq_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_R.RenderModel();

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(16.2f, 50.7f, -18.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_C.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.5f, 0.3f, -0.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_L.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.5f, 0.3f, -0.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_R.RenderModel();

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.7f, 50.8f, -20.4f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_C.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.5f, 0.3f, -0.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_L.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.5f, 0.3f, -0.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_R.RenderModel();

		// Huevo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(9.7f, 49.2f, 4.4f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.2f, 49.1f, 8.1f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.4f, 49.4f, 2.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		// Sierra
		rotSierra += 36.0;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(7.0f, 49.8f, -9.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(15.0f, 49.6f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		/* Avatar */
		// TODO: cambiar canica por avatar
		//		 programar animacion de mov del avatar

		posX = camera.getCameraPosition().x;
		posY = camera.getCameraPosition().y;
		posZ = camera.getCameraPosition().z;

		if (mainWindow.getCamaraAvatar())
		{
			if (camera.getCameraDirection().z < 0.0) 
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(posX, posY - 0.5, posZ - 0.5));
				model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Canica_M.RenderModel();
			}
			else
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(posX, posY - 0.5, posZ));
				model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Canica_M.RenderModel();
			}
		}

		/* Gabinete */
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -1.0f, -10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gabinete_M.RenderModel();

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//Cristal
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 55.0f, 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cristal_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}