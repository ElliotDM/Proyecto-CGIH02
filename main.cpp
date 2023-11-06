#define STB_IMAGE_IMPLEMENTATION

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
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"


// Variables y banderas para animacion

// Resorte
float zResorte;

// Wingmoulds
bool inicio;
float pos_wm1_l;
float pos_wm1_r;
float arc_wm1;
float arc_wm1_l;
float arc_wm1_r;

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

	camera = Camera(glm::vec3(10.0f, 70.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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

	// Resorte
	zResorte = 0.3;

	// Wingmoulds
	inicio = false;
	pos_wm1_l = -0.5;
	pos_wm1_r = 0.5;
	arc_wm1 = 0.0;
	arc_wm1_l = 0.0;
	arc_wm1_r = 0.0;

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

		// Animacion del resorte
		// Comprimir el resorte
		if (mainWindow.getResorte() && zResorte >= 0.1)
		{
			zResorte -= 0.0005;
		}
		// Si el usuario mantiene pulsado el click derecho
		// mantener el resorte comprimido
		else if (mainWindow.getResorte())
		{
		}
		// Regresa el resorte a su posicion inicial
		else if (!(mainWindow.getResorte()) && zResorte <= 0.3) {
			zResorte += 0.1;
		}
		else
		{
			zResorte = 0.3;
		}

		// Resorte
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(22.75f, 48.0f, 27.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, zResorte));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Resorte_M.RenderModel();

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
		model = glm::translate(model, glm::vec3(6.0f, 50.3f, -15.3f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::rotate(model, 10 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mainWindow.getFlipper3() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel();

		// Animacion del objeto jerarquico (Previa)
		if (mainWindow.getAnimacion())
		{
			inicio = true;
		}

		if (inicio) {
			// 1
			if (pos_wm1_r < 1.2) {
				pos_wm1_r += 0.05;
				pos_wm1_l -= 0.05;
			}
			// 2
			else if (arc_wm1_r < 30)
			{
				arc_wm1_l += 5;
				arc_wm1_r += 5;
			}
			// 3
			else if (arc_wm1 < 40)
			{
				arc_wm1_r += 1;
				arc_wm1 += 5;
			}
			// 4
			else if (arc_wm1_l < 40)
			{
				arc_wm1_l += 5;
			}
			else
			{
				inicio = false;
			}
		}
		else
		{
			pos_wm1_l = -0.5;
			pos_wm1_r = 0.5;
			arc_wm1 = 0.0;
			arc_wm1_l = 0.0;
			arc_wm1_r = 0.0;
		}

		// Obstaculos

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(16.2f, 50.7f, -18.5f));
		modelaux = model;
		model = glm::rotate(model, arc_wm1 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_C.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(pos_wm1_l, 0.3f, -0.2f));
		model = glm::rotate(model, arc_wm1_l * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_L.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(pos_wm1_r, 0.3f, -0.2f));
		model = glm::rotate(model, -arc_wm1_r * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_R.RenderModel();

		// Wingmould (Objeto jerarquico)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(15.2f, 50.9f, -23.7f));
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
		model = glm::translate(model, glm::vec3(-1.7f, 49.0f, 5.4f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		// Sierra
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(7.0f, 49.8f, -9.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		// Canica
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(22.75f, 48.2f, 27.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Canica_M.RenderModel();

		// Moneda
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.65f, 8.0f, -1.8f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Moneda_M.RenderModel();

		// Gabinete
		// TODO mover gabinete, modelo jerarquico
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -1.0f, -10.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gabinete_M.RenderModel();
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
