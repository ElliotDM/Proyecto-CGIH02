#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <cstdlib>

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

//Para el audio
#include <irrklang\irrKlang.h>
using namespace irrklang;

// Variables para las camaras
float zoomY;
float zoomZ;
float posX;
float posY;
float posZ;
float anguloCamara;
bool enableMouse;

// Variables para el contador de dia y noche
bool day;
int counterHour;
int counterDay;
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
bool snd_moneda;

// Resorte
bool resorte;
bool espera;
float zResorte;
float zManija;
bool snd_resorte;

// Canica
float movx_canica;
float movy_canica;
float movz_canica;
float rot_canica;
float t_curva;
bool canica_init;
bool canica_animacion;
bool disparo;
bool salida;

// Primer recorrido
bool choca;
bool curva;
bool choca1;

// Segundo recorrido
bool rebote;
bool flipper;
bool caida;
bool bajada;
bool subida;
bool regreso;
float u;
float v;
float radio;

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

bool wm2_inicio;
float wm2_izq;
float wm2_der;
float wm2_arc;
float wm2_arc_der_x;
float wm2_arc_der_z;
float wm2_arc_izq_x;
float wm2_arc_izq_z;
int wm2_cont1;
int wm2_cont2;

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
Model IndicatorOld_M;
Model IndicatorCoiled_M;
Model IndicatorChannelled_M;
Model WatcherKnightBody_M;
Model WatcherKnightLeftArm_M;
Model WatcherKnightRightArm_M;
Model WatcherKnightLeftLeg_M;
Model WatcherKnightRightLeg_M;

Skybox skybox;

// Materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLightDay;
DirectionalLight mainLightNight;

// luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights1[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
PointLight pointLights3[MAX_POINT_LIGHTS];
PointLight pointLights4[MAX_POINT_LIGHTS];
PointLight pointLights5[MAX_POINT_LIGHTS];
PointLight pointLights6[MAX_POINT_LIGHTS];
PointLight pointLights7[MAX_POINT_LIGHTS];

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


void getEquationOfCircle(float x1, float y1, float x2, float y2, float x3, float y3) {
	u = (x1 + x2 + x3) / 3;
	v = (y1 + y2 + y3) / 3;

	float d1 = sqrt((u - x1) * (u - x1) + (v - y1) * (v - y1));
	float d2 = sqrt((u - x2) * (u - x2) + (v - y2) * (v - y2));
	float d3 = sqrt((u - x3) * (u - x3) + (v - y3) * (v - y3));
	radio = (d1 + d2 + d3) / 3;
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

	IndicatorOld_M = Model();
	IndicatorOld_M.LoadModel("Models/IndicatorOld.obj");

	//Caballero Vigia
	WatcherKnightBody_M = Model();
	WatcherKnightBody_M.LoadModel("Models/WatcherKnightBody.obj");
	WatcherKnightLeftArm_M = Model();
	WatcherKnightLeftArm_M.LoadModel("Models/WatcherKnightLeftArm.obj");
	WatcherKnightRightArm_M = Model();
	WatcherKnightRightArm_M.LoadModel("Models/WatcherKnightRightArm.obj");
	WatcherKnightLeftLeg_M = Model();
	WatcherKnightLeftLeg_M.LoadModel("Models/WatcherKnightLeftLeg.obj");
	WatcherKnightRightLeg_M = Model();
	WatcherKnightRightLeg_M.LoadModel("Models/WatcherKnightRightLeg.obj");

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

	//Pointlight de los flippers
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		8.0f, 48.0f, 21.0f,
		1.0f, 0.2f, 0.01f);
	pointLightCount++;

	// PointLight del objeto jeraquico 1
	pointLights[1] = PointLight(0.90f, 0.89f, 0.88f,
		0.0f, 1.0f,
		15.2f, 50.9f, -23.5,
		1.0f, 0.2f, 0.01f);
	pointLightCount++;

	// PointLight del objeto jeraquico 2
	pointLights[2] = PointLight(0.392f, 0.419f, 0.462f,
		0.0f, 1.0f,
		16.2f, 50.7f, -18.5,
		1.0f, 0.2f, 0.01f);
	pointLightCount++;

	// PointLight del objeto jeraquico 3
	pointLights[3] = PointLight(0.749f, 0.764f, 0.788f,
		0.0f, 1.0f,
		10.7f, 50.8f, -20.4,
		1.0f, 0.2f, 0.01f);
	pointLightCount++;

	pointLights1[0] = pointLights[0];
	pointLights1[1] = pointLights[1];
	pointLights1[2] = pointLights[3];
	pointLights1[3] = pointLights[2];

	pointLights2[0] = pointLights[0];
	pointLights2[1] = pointLights[2];
	pointLights2[2] = pointLights[3];
	pointLights2[3] = pointLights[1];

	pointLights3[0] = pointLights[0];
	pointLights3[1] = pointLights[3];
	pointLights3[2] = pointLights[1];
	pointLights3[3] = pointLights[2];

	pointLights4[0] = pointLights[1];
	pointLights4[1] = pointLights[2];
	pointLights4[2] = pointLights[3];
	pointLights4[3] = pointLights[0];

	pointLights5[0] = pointLights[1];
	pointLights5[1] = pointLights[3];
	pointLights5[2] = pointLights[2];
	pointLights5[3] = pointLights[0];

	pointLights6[0] = pointLights[2];
	pointLights6[1] = pointLights[3];
	pointLights6[2] = pointLights[1];
	pointLights6[3] = pointLights[0];

	pointLights7[0] = pointLights[3];
	pointLights7[1] = pointLights[2];
	pointLights7[2] = pointLights[1];
	pointLights7[3] = pointLights[0];

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

	//Inicializacion de variables para las camaras
	zoomY = 75.0;
	zoomZ = 60.0;
	posX = 8.6f;
	posY = 52.5f;
	posZ = -25.5f;
	anguloCamara = 0.0f;

	// Inicializacion de variables para animacion

	// Offsets
	moneda_offset = 0.03f;
	canica_offset = 0.05f;
	rot_offset = 2.0f;

	// Moneda
	mov_moneda = 33.0f;
	rot_moneda = 0.0f;
	animacion = true;
	snd_moneda = true;

	// Resorte
	resorte = false;
	espera = false;
	zResorte = 0.3;
	zManija = 50.0;
	snd_resorte = true;

	// Canica 
	movx_canica = 18.5f;
	movy_canica = 48.0f;
	movz_canica = 24.0f;
	rot_canica = 0.0f;
	t_curva = 0.0f;
	canica_init = false;
	canica_animacion = false;
	disparo = true;
	salida = false;

	// Primer recorrido
	choca = false;
	curva = false;
	choca1 = false;

	// Segundo recorrido
	rebote = false;
	flipper = false;
	caida = false;
	bajada = false;
	subida = false;
	regreso = false;

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

	wm2_inicio = false;
	wm2_izq = -0.5;
	wm2_der = 0.5;
	wm2_arc = 0.0;
	wm2_arc_der_x = 0.0;
	wm2_arc_der_z = 0.0;
	wm2_arc_izq_x = 0.0;
	wm2_arc_izq_z = 0.0;
	wm2_cont1 = 0;
	wm2_cont2 = 0;

	// Sierra
	rotSierra = 0.0;

	// Inicializacion de variables para el contador de dia y noche
	day = true;
	counterHour = 0;
	counterDay = 0;
	firts_Light = true;

	// Inicializacion del motor de sonido
	ISoundEngine* SoundEngine = createIrrKlangDevice();

	if (!SoundEngine)
		return 0; //Error en el audio

	irrklang::ISound* snd = SoundEngine->play2D("PathofPain.mp3", true);

	if (snd)
		snd->setVolume(0.5);

	// Audio en 3D de las sierras
	irrklang::vec3df saw_pos1(-3.0f, 49.3f, 5.5f);
	irrklang::vec3df saw_pos2(-2.5f, 49.2f, 3.5f);
	irrklang::vec3df saw_pos3(6.5f, 50.2f, -21.0f);
	irrklang::vec3df saw_pos4(6.5f, 49.9f, -14.0f);
	irrklang::vec3df saw_pos5(7.0f, 49.8f, -9.0f);
	irrklang::vec3df saw_pos6(15.0f, 49.6f, -5.0f);
	irrklang::vec3df saw_pos7(18.5f, 49.4f, 2.0f);
	
	irrklang::ISound* saw_snd1 = SoundEngine->play3D("saw1.mp3", saw_pos1, true, false, true);
	irrklang::ISound* saw_snd2 = SoundEngine->play3D("saw2.mp3", saw_pos2, true, false, true);
	irrklang::ISound* saw_snd3 = SoundEngine->play3D("saw1.mp3", saw_pos3, true, false, true);
	irrklang::ISound* saw_snd4 = SoundEngine->play3D("saw2.mp3", saw_pos4, true, false, true);
	irrklang::ISound* saw_snd5 = SoundEngine->play3D("saw1.mp3", saw_pos5, true, false, true);
	irrklang::ISound* saw_snd6 = SoundEngine->play3D("saw2.mp3", saw_pos6, true, false, true);
	irrklang::ISound* saw_snd7 = SoundEngine->play3D("saw2.mp3", saw_pos7, true, false, true);

	float minDistance = 2.0f;
	float volume = 6.0f;

	if (saw_snd1)
	{
		saw_snd1->setMinDistance(minDistance); // a loud sound
		saw_snd1->setIsPaused(false); // unpause the sound
		saw_snd1->setVolume(volume);
	}

	if (saw_snd2)
	{
		saw_snd2->setMinDistance(minDistance); // a loud sound
		saw_snd2->setIsPaused(false); // unpause the sound
		saw_snd2->setVolume(volume);
	}

	if (saw_snd3)
	{
		saw_snd3->setMinDistance(minDistance); // a loud sound
		saw_snd3->setIsPaused(false); // unpause the sound
		saw_snd3->setVolume(volume);
	}

	if (saw_snd4)
	{
		saw_snd4->setMinDistance(minDistance); // a loud sound
		saw_snd4->setIsPaused(false); // unpause the sound
		saw_snd4->setVolume(volume);
	}

	if (saw_snd5)
	{
		saw_snd5->setMinDistance(minDistance); // a loud sound
		saw_snd5->setIsPaused(false); // unpause the sound
		saw_snd5->setVolume(volume);
	}

	if (saw_snd6)
	{
		saw_snd6->setMinDistance(minDistance); // a loud sound
		saw_snd6->setIsPaused(false); // unpause the sound
		saw_snd6->setVolume(volume);
	}

	if (saw_snd7)
	{
		saw_snd7->setMinDistance(minDistance); // a loud sound
		saw_snd7->setIsPaused(false); // unpause the sound
		saw_snd7->setVolume(volume);
	}

	//Loop mientras no se cierre la ventana
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
			camera = Camera(glm::vec3(posX, posY, posZ), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 0.3f, 0.5f);
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
		//Caso 1
		if (mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		//Caso 2
		if (mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}
		//Caso 3
		if (mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights1, pointLightCount-1);
		}
		//Caso 4
		if (mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights1, pointLightCount-2);
		}
		//Caso 5
		if (mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
		//Caso 6
		if (mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights2, pointLightCount-2);
		}
		//Caso 7
		if (mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights3, pointLightCount-2);
		}
		//Caso 8
		if (mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights3, pointLightCount-3);
		}
		//Caso 9
		if (!mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights4, pointLightCount-1);
		}
		//Caso 10
		if (!mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights4, pointLightCount-2);
		}
		//Caso 11
		if (!mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights5, pointLightCount-2);
		}
		//Caso 12
		if (!mainWindow.getlightFlippers() and mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights5, pointLightCount-3);
		}
		//Caso 13
		if (!mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights6, pointLightCount-2);
		}
		//Caso 14
		if (!mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights6, pointLightCount-3);
		}
		//Caso 15
		if (!mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights7, pointLightCount-3);
		}
		//Caso 16
		if (!mainWindow.getlightFlippers() and !mainWindow.gethierarchicalObject() and !mainWindow.gethierarchicalObject2() and !mainWindow.gethierarchicalObject3())
		{
			shaderList[0].SetPointLights(pointLights7, pointLightCount-4);
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
			// Offsets
			moneda_offset = 0.03f;
			canica_offset = 0.05f;
			rot_offset = 2.0f;

			// Moneda
			mov_moneda = 33.0f;
			rot_moneda = 0.0f;
			animacion = true;
			snd_moneda = true;

			// Resorte
			resorte = false;
			espera = false;
			zResorte = 0.3;
			zManija = 50.0;
			snd_resorte = true;

			// Canica 
			movx_canica = 18.5f;
			movy_canica = 48.0f;
			movz_canica = 24.0f;
			rot_canica = 0.0f;
			t_curva = 0.0f;
			canica_init = false;
			canica_animacion = false;
			disparo = true;
			salida = false;

			// Primer recorrido
			choca = false;
			curva = false;
			choca1 = false;

			// Segundo recorrido
			rebote = false;
			flipper = false;
			caida = false;
			bajada = false;
			subida = false;
			regreso = false;

			mainWindow.setReset(false);
			mainWindow.setMoneda(false);
		}
		else
		{
			// Moneda
			// Inserta moneda y comienza la animacion
			if (mainWindow.getMoneda() && animacion)
			{
				if (snd_moneda)
				{
					SoundEngine->play2D("coin.mp3", false);
				}

				snd_moneda = false;

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
					canica_offset = 0.01;
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
					movz_canica += canica_offset * deltaTime;
				}
				// Si el usuario mantiene pulsado el click derecho
				// mantener el resorte comprimido
				else if (mainWindow.getResorte())
				{
				}
				// Regresa el resorte a su posicion inicial
				else if (!(mainWindow.getResorte()) && zResorte < 0.3) {
					if (snd_resorte)
					{
						SoundEngine->play2D("spring.mp3", false);
					}

					snd_resorte = false;

					zResorte = 0.3;
					zManija = 50.0;
					canica_offset = 0.4f;
					espera = true;
				}
			}
			// Canica (recorrido)
			else if (canica_animacion)
			{
				// La canica sale disparada
				if (disparo)
				{
					if (movz_canica >= -22.5)
					{
						movz_canica -= canica_offset * deltaTime;
						movy_canica = -0.053 * movz_canica + 49.267;
						rot_canica += rot_offset * deltaTime;
					}
					else
					{
						disparo = false;
						salida = true;
					}
				}
				// Curva de salida
				else if (salida)
				{
					// Sigue una trayectoria de un circulo
					if (t_curva <= 3.0)
					{
						t_curva += 0.05 * deltaTime;
						movx_canica = 19.5 + (3 * glm::cos(t_curva));
						movz_canica = -22.5 - (3 * glm::sin(t_curva));
						movy_canica = -0.053 * movz_canica + 49.267;
					}
					else
					{
						t_curva = -0.4f;
						canica_offset = 0.2f;
						salida = false;
						choca = true;
					}
				}
				// Recorridos
				if (mainWindow.getRuta())
				{
					// Canica choca con obstaculo y rebota
					if (choca)
					{
						// Activa animacion del objeto jerarquico
						if (wm1_inicio) {}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							SoundEngine->play2D("wingmould.mp3", false);
							wm1_inicio = true;
						}

						// Cae rapidamente
						if (movz_canica < -19.5)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							choca = false;
							rebote = true;
							t_curva = -0.47f;
						}
					}
					// Canica vuelve a chocar con obstaculo y rebota
					else if (rebote)
					{
						// Activa animacion del objeto jerarquico
						if (wm2_inicio) {}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							SoundEngine->play2D("wingmould.mp3", false);
							wm2_inicio = true;
						}

						// Sigue una trayectoria de un circulo
						if (t_curva >= -2.96)
						{
							t_curva -= 0.07 * deltaTime;
							movx_canica = 14.83 + (glm::sqrt(3.47) * glm::cos(t_curva));
							movz_canica = -18.67 + (glm::sqrt(3.47) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						// Cae rapidamente
						else if (movz_canica <= -13.7)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							canica_offset = 0.1;
							rebote = false;
							flipper = true;
						}
					}
					// Canica se desliza por el flipper
					else if (flipper)
					{
						// Si se acciona el flipper se cambia la ruta
						if (mainWindow.getAction())
						{
							getEquationOfCircle(movx_canica, movz_canica, 9.75, -16.5, 6.5, -15.5);
							SoundEngine->play2D("hit.mp3", false);
							canica_offset = 0.2;
							t_curva = 0.0;
							flipper = false;
							subida = true;
						}
						// Sino continua bajando
						if (movx_canica >= 11.2)
						{
							movx_canica -= canica_offset * deltaTime;
							movz_canica = -0.92 * movx_canica - 1.74;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							t_curva = 0.315;
							canica_offset = 0.2;
							flipper = false;
							caida = true;
						}
					}
					// Canica es golpeada por el flipper
					else if (subida)
					{
						if (t_curva >= -3.8)
						{
							t_curva -= 0.1 * deltaTime;
							movx_canica = u + (glm::sqrt(radio) * glm::cos(t_curva));
							movz_canica = v + (glm::sqrt(radio) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = -3.07;
							subida = false;
							regreso = true;
						}				
					}
					// Canica cae y continua su camino por el tablero
					else if (regreso)
					{
						if (t_curva <= 0)
						{
							t_curva += 0.07 * deltaTime;
							movx_canica = 8.1 + (glm::sqrt(2.57) * glm::cos(t_curva));
							movz_canica = -15.4 + (glm::sqrt(2.57) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else if (movz_canica <= 3.2)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = -3.07;
							subida = false;
							regreso = true;
						}				
					}
					// Canica cae y continua su camino por el tablero
					else if (regreso)
					{
						if (t_curva <= 0)
						{
							t_curva += 0.07 * deltaTime;
							movx_canica = 8.1 + (glm::sqrt(2.57) * glm::cos(t_curva));
							movz_canica = -15.4 + (glm::sqrt(2.57) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else if (movz_canica <= 3.2)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = 3.7;
							bajada = true;
							regreso = false;
						}
					}
					// Si no se activa el flipper la canica sigue su camino
					else if (caida)
					{
						if (t_curva <= 0.403)
						{
							t_curva += 0.004 * deltaTime;
							movx_canica = -35.15 + (glm::sqrt(2376.33) * glm::cos(t_curva));
							movz_canica = -27.1 + (glm::sqrt(2376.33) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else if (movz_canica <= 3.2)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = 3.7;
							bajada = true;
							caida = false;
						}
					}
					else if (bajada)
					{
						if (t_curva <= 6.18)
						{
							t_curva += 0.08 * deltaTime;
							movx_canica = 11.72 + (glm::sqrt(5.25) * glm::cos(t_curva));
							movz_canica = 4.28 + (glm::sqrt(5.25) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else if (movz_canica <= 13.5)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = -0.46f;
							canica_offset = 0.3f;
							choca1 = true;
							bajada = false;
						}
					}
				}
				else
				{
					// Canica choca con obstaculo y rebota
					if (choca)
					{
						// Activa animacion del objeto jerarquico
						if (wm1_inicio) {}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							SoundEngine->play2D("wingmould.mp3", false);
							wm1_inicio = true;
						}

						// Sigue una trayectoria de un circulo
						if (t_curva <= 0.4)
						{
							t_curva += 0.008 * deltaTime;
							movx_canica = -10.47 + (glm::sqrt(845.92) * glm::cos(t_curva));
							movz_canica = -12.05 + (glm::sqrt(845.92) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
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
							t_curva -= 0.004 * deltaTime;
							movx_canica = 73.12 + (glm::sqrt(3503.15) * glm::cos(t_curva));
							movz_canica = 15.88 + (glm::sqrt(3503.15) * glm::sin(t_curva));
							movy_canica = -0.053 * movz_canica + 49.267;
						}
						else
						{
							SoundEngine->play2D("hit.mp3", false);
							t_curva = -0.46f;
							canica_offset = 0.3f;
							curva = false;
							choca1 = true;
						}
					}
				}

				// Canica choca y cae entre los flippers
				if (choca1)
				{
					if (t_curva >= -3.4)
					{
						t_curva -= 0.08 * deltaTime;
						movx_canica = 11.11 + (glm::sqrt(10.32) * glm::cos(t_curva));
						movz_canica = 14.21 + (glm::sqrt(10.32) * glm::sin(t_curva));
						movy_canica = -0.053 * movz_canica + 49.267;
					}
					else
					{
						if (movz_canica <= 26.0)
						{
							movz_canica += canica_offset * deltaTime;
							movy_canica = -0.053 * movz_canica + 49.267;
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
				// Offsets
				moneda_offset = 0.03f;
				canica_offset = 0.05f;
				rot_offset = 2.0f;

				// Moneda
				mov_moneda = 33.0f;
				rot_moneda = 0.0f;
				animacion = true;
				snd_moneda = true;

				// Resorte
				resorte = false;
				espera = false;
				zResorte = 0.3;
				zManija = 50.0;
				snd_resorte = true;

				// Canica 
				movx_canica = 18.5f;
				movy_canica = 48.0f;
				movz_canica = 24.0f;
				rot_canica = 0.0f;
				t_curva = 0.0f;
				canica_init = false;
				canica_animacion = false;
				disparo = true;
				salida = false;

				// Primer recorrido
				choca = false;
				curva = false;
				choca1 = false;

				// Segundo recorrido
				rebote = false;
				flipper = false;
				caida = false;
				bajada = false;
				subida = false;
				regreso = false;
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

		if (wm2_inicio) {
			// Caparazon se separa
			if (wm2_der < 1.2) {
				wm2_der += 0.05;
				wm2_izq -= 0.05;
			}
			// Caparazon rota
			else if (wm2_arc_izq_z < 30)
			{
				wm2_arc_der_z += 5;
				wm2_arc_izq_z += 5;
			}
			// Interior rota 
			else if (wm2_arc < 40)
			{
				wm2_arc_izq_z += 1;
				wm2_arc += 5;
			}
			// Caparazon derecho rota
			else if (wm2_arc_der_z < 40)
			{
				wm2_arc_der_z += 5;
			}
			// Comienza a vibrar durante 2 segundos
			else if ((int)now % 2 == 0)
			{
				wm2_cont1++;
			}
			else
			{
				wm2_cont1 = 0;
			}

			if (wm2_cont1 == 1)
				wm2_cont2++;

			if (wm2_cont2 == 1)
			{
				wm2_cont2 = 0;
				wm2_inicio = false;
			}
			else
			{
				//La luz se apagara independientemente del estado
				//shaderList[0].SetPointLights(pointLights1, pointLightCount - 1);

				if (wm2_arc_der_x <= 30.0)
				{
					wm2_arc_der_x += 10.0;
					wm2_arc_izq_x -= 10.0;
				}
				else if (wm2_arc_izq_x <= 0.0)
				{
					wm2_arc_der_x -= 10.0;
					wm2_arc_izq_x += 10.0;
				}
			}
		}
		// Regresa a su estado inicial
		else
		{
			wm2_izq = -0.5;
			wm2_der = 0.5;
			wm2_arc = 0.0;
			wm2_arc_der_x = 0.0;
			wm2_arc_der_z = 0.0;
			wm2_arc_izq_x = 0.0;
			wm2_arc_izq_z = 0.0;
			wm2_cont1 = 0;
			wm2_cont2 = 0;
		}

		/* Modelos */

		//Aguijon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, 52.0f, -10.0f));
		model = glm::rotate(model, -70 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nail_M.RenderModel();

		// Soñadores
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 49.0f, 26.0f));
		model = glm::rotate(model, 3 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dreamers_M.RenderModel();

		// Soñadores
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(16.0f, 49.0f, 26.0f));
		model = glm::rotate(model, 3 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dreamers_M.RenderModel();

		// Moneda
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(9.25f, 47.5f, mov_moneda));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
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
		model = glm::rotate(model, wm2_arc * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_C.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(wm2_izq, 0.3f, -0.2f));
		model = glm::rotate(model, wm2_arc_der_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, wm2_arc_der_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WingMould_L.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(wm2_der, 0.3f, -0.2f));
		model = glm::rotate(model, wm2_arc_izq_x * toRadians, glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -wm2_arc_izq_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
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
		model = glm::translate(model, glm::vec3(9.7f, 49.1f, 4.4f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.2f, 49.0f, 8.1f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.4f, 49.3f, 2.2f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Huevo_M.RenderModel();

		// Sierra
		rotSierra += 36.0;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 49.3f, 5.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.5f, 49.2f, 3.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.5f, 50.2f, -21.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.5f, 49.9f, -14.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

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

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(18.5f, 49.4f, 2.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, rotSierra * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sierra_M.RenderModel();

		/* Avatar */

		if (mainWindow.getCamaraAvatar())
		{
			posX = camera.getCameraPosition().x;
			posY = camera.getCameraPosition().y;
			posZ = camera.getCameraPosition().z;
			anguloCamara = camera.getYaw() - 90;
		}

		//Avatar -> Caballero Vigia
		//Cuerpo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.5f, 50.6f, 20.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		modelaux = model;
		//model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightBody_M.RenderModel();

		//Brazo derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightLeftArm_M.RenderModel();

		//Brazo izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 0.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightRightArm_M.RenderModel();

		//Pierna derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, -4.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightLeftLeg_M.RenderModel();

		//Pierna Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, -4.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightRightLeg_M.RenderModel();

		//Cuerpo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(posX, posY - 1.3f, posZ));
		model = glm::rotate(model, -anguloCamara * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightBody_M.RenderModel();

		//Brazo derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightLeftArm_M.RenderModel();

		//Brazo izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 0.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightRightArm_M.RenderModel();

		//Pierna derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, -4.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightLeftLeg_M.RenderModel();

		//Pierna Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, -4.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WatcherKnightRightLeg_M.RenderModel();

		/* Gabinete */
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -1.0f, -10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gabinete_M.RenderModel();

		//Indicadores
		model = modelaux;
		model = glm::translate(model, glm::vec3(8.5f, 50.6f, -4.0f));
		model = glm::rotate(model, -5.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 3.5f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(7.8f, 50.25f, 2.0f));
		model = glm::rotate(model, -8.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 3.5f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(6.8f, 49.9f, 8.0f));
		model = glm::rotate(model, -10.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 3.5f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-7.5f, 50.03f, 5.5f));
		model = glm::rotate(model, -160.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -3.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -2.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-7.5f, 48.68f, 28.5f));
		model = glm::rotate(model, 50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 2.5f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 2.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 48.75f, 27.5f));
		model = glm::rotate(model, -50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 2.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -2.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.17f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		IndicatorOld_M.RenderModel();

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

	if (snd)
		snd->drop();

	if (saw_snd1)
		saw_snd1->drop();

	if (saw_snd2)
		saw_snd2->drop();

	if (saw_snd3)
		saw_snd3->drop();

	if (saw_snd4)
		saw_snd4->drop();

	if (saw_snd5)
		saw_snd5->drop();

	if (saw_snd6)
		saw_snd6->drop();

	if (saw_snd7)
		saw_snd7->drop();

	SoundEngine->drop();

	return 0;
}