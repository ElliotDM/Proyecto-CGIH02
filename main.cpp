#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <thread>
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
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"


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
bool resorte;
bool espera;
float zResorte;
float zManija;

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
bool wm1_inicio;
float wm1_izq;
float wm1_der;
float wm1_arc;
float wm1_arc_der_x;
float wm1_arc_der_z;
float wm1_arc_izq_x;
float wm1_arc_izq_z;
int wm1_cont1;
int wm1_cont2;

// Sierra
float rotSierra;

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture monedaTexture;
Texture gabineteTexture;
Texture canicaTexture;
Texture resorteTexture;
Texture wingmouldTexture;
Texture sierraTexture;

Model Gabinete_M;
Model Moneda_M;
Model Canica_M;
Model Resorte_M;
Model Manija_M;
Model ManijaRes_M;
Model Cristal_M;
Model Flipper_M;
Model WingMould_C;
Model WingMould_L;
Model WingMould_R;
Model Sierra_M;
Model Huevo_M;

Skybox skybox;

// Materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
// luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
// luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
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

	camera = Camera(glm::vec3(10.0f, 80.0f, 40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	monedaTexture = Texture("Textures/moneda.png");
	monedaTexture.LoadTextureA();
	gabineteTexture = Texture("Textures/prueba.png");
	gabineteTexture.LoadTextureA();
	canicaTexture = Texture("Textures/canica.png");
	canicaTexture.LoadTextureA();
	resorteTexture = Texture("Textures/resorte.png");
	resorteTexture.LoadTextureA();
	wingmouldTexture = Texture("Textures/wingmould.png");
	wingmouldTexture.LoadTextureA();
	sierraTexture = Texture("Textures/saw.png");
	sierraTexture.LoadTextureA();
	
	Gabinete_M = Model();
	Gabinete_M.LoadModel("Models/gabinete.obj");
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

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//contador de luces spotlight
	unsigned int spotLightCount = 0;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

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

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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

		// información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		/* Animaciones */

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
					if (wm1_inicio){}
					else
					{
						wm1_inicio = true;
					}

					// Sigue una trayectoria de un circulo
					if (t_curva <= 0.4)
					{
						t_curva += 0.001;

						movx_canica = -10.47 + (glm::sqrt(845.92) * glm::cos(t_curva));
						movy_canica -= 0.005 * deltaTime * 1.5;
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
						movy_canica -= 0.005 * deltaTime * 1.5;
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
							movy_canica += 0.01 * deltaTime * 1.5;
						}
						else if (movy_canica >= 48.35)
						{
							movy_canica -= 0.02 * deltaTime * 1.5;
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
							movy_canica -= 0.01 * deltaTime * 1.5;
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
			{
				wm1_cont2++;
			}

			if (wm1_cont2 == 1)
			{
				wm1_cont2 = 0;
				wm1_inicio = false;
			}
			else
			{
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

		// Gabinete
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -1.0f, -10.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gabinete_M.RenderModel();

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
		model = glm::rotate(model, -mainWindow.getFlipper1() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
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

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(9.7f, 49.0f, 4.6f));
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
		model = glm::translate(model, glm::vec3(5.2f, 48.9f, 8.2f));
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
		model = glm::translate(model, glm::vec3(4.4f, 48.9f, 2.3f));
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
		model = glm::translate(model, glm::vec3(-1.7f, 49.0f, 5.4f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(20.2f, 48.8f, 6.5f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
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

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
