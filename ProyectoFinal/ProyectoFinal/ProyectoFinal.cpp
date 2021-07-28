#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 25.0f, 150.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 11.5f;
float spotAngle = -30.0f;


// Light attributes
bool active;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
//Posiciones iniciales para realizar los ajustes necesarios de las animaciones asociados 
//a los objetos de Basquet y Waka por animacion de Keyframe.
float posYTopo1 = 0.0f, posYTopo2 = 0.0f, posYTopo3 = 0.0f, posYTopo4 = 0.0f, posZBalon = 0.0f, posYBalon = 0.0f;

#define MAX_FRAMES 18
int i_max_steps = 190;
int i_curr_steps = 0;

typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posYTopo1;		//Variable para posicion en Y del Topo 1
	float posYTopo2;		//Variable para posicion en Y del Topo 2
	float posYTopo3;		//Variable para posicion en Y del Topo 3
	float posYTopo4;		//Variable para posicion en Y del Topo 4

	float posZBalon;		//Variable para posicion en Z del Balon de Basquet
	float posYBalon;		//Variable para posicion en Y del Balon de Basquet

	float incYTopo1;		//Variable para incremento en Y del Topo 1
	float incYTopo2;		//Variable para incremento en Y del Topo 1
	float incYTopo3;		//Variable para incremento en Y del Topo 1
	float incYTopo4;		//Variable para incremento en Y del Topo 1

	float incZBalon;		//Variable para incremento en Z del Balon de Basquet
	float incYBalon;		//Variable para incremento en Y del Balon de Basquet

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 18;			//Definimos un numero exacto de frames que ocuparemos para animación
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	//Pointlight de focos del estacionamiento
	glm::vec3(80,85,31),
	glm::vec3(80,85,-31),

	glm::vec3(-80,85,31),
	glm::vec3(-80,85,-31),

	glm::vec3(80,85,120),
	glm::vec3(80,85,180),

	glm::vec3(-80,85,120),
	glm::vec3(-80,85,180),
		
	glm::vec3(105,85,-145),
	glm::vec3(105,85,-210),

	glm::vec3(-220,85,-145),
	glm::vec3(-220,85,-210),

	//Pointlight de focos exteriores del Arcade
	glm::vec3(58,35,-95),
	glm::vec3(-49,35,-95),


	//Pointlight de focos interiores del arcade
	glm::vec3(-38,39,-146),
	glm::vec3(-38,39,-216),
	glm::vec3(53,40,-129),
	glm::vec3(46,39,-216),
	glm::vec3(-108,39,-216),

	//Spotlight de focos del estacionamiento (CONCEPTO)
	glm::vec3(80,85,31),
	glm::vec3(0,-1,0)

};

//glm::vec3 LightP1;
glm::vec3 Light = glm::vec3(0);		//Configuracion inicial para las luces PointLight del estacionamiento
glm::vec3 LightA = glm::vec3(0.7);	//Configuracion inicial para las luces DirectionalLight del ambiente
glm::vec3 LightExt = glm::vec3(0);	//Configuracion inicial para las luces PointLight del exterior del arcade
glm::vec3 LightInt = glm::vec3(0);	//Configuracion inicial para las luces PointLight del interior del arcade
glm::vec3 LightC = glm::vec3(0);	//Configuracion inicial para las luces SpotLight de la camara

//Animación de abrir las puertas de cristal
float rot1 = 0.0;
float rot2 = 0.0;

//Animacion de columpio
float rot3 = 0.0;

//Animacion de escritorio
float despX1 = 0.0;
float despX2 = 0.0;
float despX3 = 0.0;
float despX4 = 0.0;

//Abrir todas las puertas (Activacion= T)
bool animacion1 = false;
bool abrir = false;
bool cerrar = false;

//Activar la animacion del columpio (Activacion = Y / Desactivacion = H)
bool animacion2 = false;
bool giro1 = true;
bool giro2 = false;

//Activar la animacion del escritorio (Activacion = U / Desactivacion = J)
bool animacion3 = false;
bool des1 = true;
bool des2 = false;


void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);
	
	KeyFrame[FrameIndex].posYTopo1 = posYTopo1;
	KeyFrame[FrameIndex].posYTopo2 = posYTopo2;
	KeyFrame[FrameIndex].posYTopo3 = posYTopo3;
	KeyFrame[FrameIndex].posYTopo4 = posYTopo4;

	KeyFrame[FrameIndex].posZBalon = posZBalon;
	KeyFrame[FrameIndex].posYBalon = posYBalon;
	
	//Impresiones de pantalla para conocer la posicion de los objetos al crear los keyframes
	//printf("Topo1: %f\n Topo2: %f\n  Topo3: %f\n  Topo4: %f\n", posYTopo1, posYTopo2, posYTopo3, posYTopo4);
	//printf("Posicion Y: %f \n Posicion Z: %f", posYBalon, posZBalon);
	FrameIndex++;
}

void resetElements(void)
{
	posYTopo1 = KeyFrame[0].posYTopo1;
	posYTopo2 = KeyFrame[0].posYTopo2;
	posYTopo3 = KeyFrame[0].posYTopo3;
	posYTopo4 = KeyFrame[0].posYTopo4;

	posZBalon = KeyFrame[0].posZBalon;
	posYBalon = KeyFrame[0].posYBalon;
}

void interpolation(void)
{

	KeyFrame[playIndex].incYTopo1 = (KeyFrame[playIndex + 1].posYTopo1 - KeyFrame[playIndex].posYTopo1) / i_max_steps;
	KeyFrame[playIndex].incYTopo2 = (KeyFrame[playIndex + 1].posYTopo2 - KeyFrame[playIndex].posYTopo2) / i_max_steps;
	KeyFrame[playIndex].incYTopo3 = (KeyFrame[playIndex + 1].posYTopo3 - KeyFrame[playIndex].posYTopo3) / i_max_steps;
	KeyFrame[playIndex].incYTopo4 = (KeyFrame[playIndex + 1].posYTopo4 - KeyFrame[playIndex].posYTopo4) / i_max_steps;

	KeyFrame[playIndex].incZBalon = (KeyFrame[playIndex + 1].posZBalon - KeyFrame[playIndex].posZBalon) / i_max_steps;
	KeyFrame[playIndex].incYBalon = (KeyFrame[playIndex + 1].posYBalon - KeyFrame[playIndex].posYBalon) / i_max_steps;
	
}




int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");

	//Importacion de modelos OBJ 
	Model Entorno((char*)"Models/Entorno/EntornoFinal.obj");
	Model Maquinita((char*)"Models/Maquinita/AsterFireArcade.obj");
	Model Basquet((char*)"Models/Basquet/Basquet.obj");
	Model Balon((char*)"Models/Basquet/Balon.obj");
	Model Refrescos((char*)"Models/Refrescos/CokeMachine.obj");
	Model Escritorio((char*)"Models/Escritorio/Desk.obj");
	Model Escritorio1((char*)"Models/Escritorio/Desk1.obj");
	Model BoteBasura((char*)"Models/AdornosExterior/BoteBasura/BoteBasura.obj");
	Model LamparaExterior((char*)"Models/AdornosExterior/LamparaExterior/LamparaExterior.obj");	
	Model Silla((char*)"Models/AdornosExterior/Silla/Silla.obj");
	Model BoteBasura2((char*)"Models/BoteBasura/BoteBasura.obj");
	Model Archivero((char*)"Models/Archivero/Archivero.obj");
	Model Moto((char*)"Models/Moto/Moto.obj");
	Model PistaBaile((char*)"Models/PistaBaile/PistaBaile.obj");
	Model Waka((char*)"Models/Waka/Waka.obj");
	Model Topo((char*)"Models/Waka/Topo.obj");
	Model Parque((char*)"Models/Parque/Parque.obj");
	Model Parque1((char*)"Models/Parque/Parque1.obj");
	Model Estante((char*)"Models/Estante/Estante.obj");
	Model Luces1((char*)"Models/Luces1/Luces1.obj");
	Model Luces2((char*)"Models/Luces2/Luces2.obj");
	Model Puerta((char*)"Models/Puerta/Puerta.obj");
	Model Puerta1((char*)"Models/Puerta/Puerta1.obj");
	Model Puerta2((char*)"Models/Puerta/Puerta2.obj");
	Model PuertaCristal1((char*)"Models/PuertaCristal/PuertaCristal1.obj");
	Model PuertaCristal2((char*)"Models/PuertaCristal/PuertaCristal2.obj");
	
	//Inicialización de KeyFrames
	
	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].posYTopo1 = 0;
		KeyFrame[i].posYTopo2 = 0;
		KeyFrame[i].posYTopo3 = 0;
		KeyFrame[i].posYTopo4 = 0;
		KeyFrame[i].posZBalon = 0;
		KeyFrame[i].posYBalon = 0;
		KeyFrame[i].incYTopo1 = 0;
		KeyFrame[i].incYTopo2 = 0;
		KeyFrame[i].incYTopo3 = 0;
		KeyFrame[i].incYTopo4 = 0;
		KeyFrame[i].incZBalon = 0;
		KeyFrame[i].incYBalon = 0;
	}

	//Implementacion de las animaciones por Keyframe

	//Animacion del Whack a Mole (Pegale al topo)
	KeyFrame[0].posYTopo1 = 0;
	KeyFrame[0].posYTopo2 = 0;
	KeyFrame[0].posYTopo3 = 0;
	KeyFrame[0].posYTopo4 = 0;
	KeyFrame[0].posZBalon = 0;
	KeyFrame[0].posYBalon = 0;

	KeyFrame[1].posYTopo1 = 0.3;
	KeyFrame[1].posYTopo2 = 0;
	KeyFrame[1].posYTopo3 = 0;
	KeyFrame[1].posYTopo4 = 0;
	KeyFrame[1].posZBalon = 0;
	KeyFrame[1].posYBalon = 0;

	KeyFrame[2].posYTopo1 = 0;
	KeyFrame[2].posYTopo2 = 0;
	KeyFrame[2].posYTopo3 = 0;
	KeyFrame[2].posYTopo4 = 0;
	KeyFrame[1].posZBalon = 0;
	KeyFrame[1].posYBalon = 0;

	KeyFrame[3].posYTopo1 = 0;
	KeyFrame[3].posYTopo2 = 0;
	KeyFrame[3].posYTopo3 = 0.3;
	KeyFrame[3].posYTopo4 = 0;
	KeyFrame[3].posZBalon = 0;
	KeyFrame[3].posYBalon = 0;

	KeyFrame[4].posYTopo1 = 0;
	KeyFrame[4].posYTopo2 = 0;
	KeyFrame[4].posYTopo3 = 0;
	KeyFrame[4].posYTopo4 = 0;
	KeyFrame[4].posZBalon = 0;
	KeyFrame[4].posYBalon = 0;

	KeyFrame[5].posYTopo1 = 0;
	KeyFrame[5].posYTopo2 = 0.3;
	KeyFrame[5].posYTopo3 = 0.3;
	KeyFrame[5].posYTopo4 = 0;
	KeyFrame[5].posZBalon = 0;
	KeyFrame[5].posYBalon = 0;

	KeyFrame[6].posYTopo1 = 0;
	KeyFrame[6].posYTopo2 = 0;
	KeyFrame[6].posYTopo3 = 0;
	KeyFrame[6].posYTopo4 = 0;
	KeyFrame[6].posZBalon = 0;
	KeyFrame[6].posYBalon = 0;

	KeyFrame[7].posYTopo1 = 0.3;
	KeyFrame[7].posYTopo2 = 0.3;
	KeyFrame[7].posYTopo3 = 0.3;
	KeyFrame[7].posYTopo4 = 0.3;
	KeyFrame[7].posZBalon = 0;
	KeyFrame[7].posYBalon = 0;

	KeyFrame[8].posYTopo1 = 0;
	KeyFrame[8].posYTopo2 = 0;
	KeyFrame[8].posYTopo3 = 0;
	KeyFrame[8].posYTopo4 = 0;
	KeyFrame[8].posZBalon = 0;
	KeyFrame[8].posYBalon = 0;


	KeyFrame[9].posYTopo1 = 0.3;
	KeyFrame[9].posYTopo2 = 0;
	KeyFrame[9].posYTopo3 = 0;
	KeyFrame[9].posYTopo4 = 0.3;
	KeyFrame[9].posZBalon = 0;
	KeyFrame[9].posYBalon = 0;

	KeyFrame[10].posYTopo1 = 0;
	KeyFrame[10].posYTopo2 = 0;
	KeyFrame[10].posYTopo3 = 0;
	KeyFrame[10].posYTopo4 = 0;
	KeyFrame[10].posZBalon = 0;
	KeyFrame[10].posYBalon = 0;

	//Animacion de lanzamiento del balon de Basquetball
	KeyFrame[11].posYTopo1 = 0;
	KeyFrame[11].posYTopo2 = 0;
	KeyFrame[11].posYTopo3 = 0;
	KeyFrame[11].posYTopo4 = 0;
	KeyFrame[11].posZBalon = 0;
	KeyFrame[11].posYBalon = 0;

	KeyFrame[12].posYTopo1 = 0;
	KeyFrame[12].posYTopo2 = 0;
	KeyFrame[12].posYTopo3 = 0;
	KeyFrame[12].posYTopo4 = 0;
	KeyFrame[12].posZBalon = 0;
	KeyFrame[12].posYBalon = 1;

	KeyFrame[13].posYTopo1 = 0;
	KeyFrame[13].posYTopo2 = 0;
	KeyFrame[13].posYTopo3 = 0;
	KeyFrame[13].posYTopo4 = 0;
	KeyFrame[13].posZBalon = -1;
	KeyFrame[13].posYBalon = 1.55;

	KeyFrame[14].posYTopo1 = 0;
	KeyFrame[14].posYTopo2 = 0;
	KeyFrame[14].posYTopo3 = 0;
	KeyFrame[14].posYTopo4 = 0;
	KeyFrame[14].posZBalon = -2;
	KeyFrame[14].posYBalon = 1.2;

	KeyFrame[15].posYTopo1 = 0;
	KeyFrame[15].posYTopo2 = 0;
	KeyFrame[15].posYTopo3 = 0;
	KeyFrame[15].posYTopo4 = 0;
	KeyFrame[15].posZBalon = -2;
	KeyFrame[15].posYBalon = 0.5;

	KeyFrame[16].posYTopo1 = 0;
	KeyFrame[16].posYTopo2 = 0;
	KeyFrame[16].posYTopo3 = 0;
	KeyFrame[16].posYTopo4 = 0;
	KeyFrame[16].posZBalon = -1;
	KeyFrame[16].posYBalon = 0.25;

	KeyFrame[17].posYTopo1 = 0;
	KeyFrame[17].posYTopo2 = 0;
	KeyFrame[17].posYTopo3 = 0;
	KeyFrame[17].posYTopo4 = 0;
	KeyFrame[17].posZBalon = 0;
	KeyFrame[17].posYBalon = 0;


	//// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);


	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/front.tga");
	faces.push_back("SkyBox/back.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), LightA.x, LightA.y, LightA.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), LightA.x, LightA.y, LightA.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.1f, 0.1f, 0.1f);

		//Iluminacion destinada al estacionamiento
		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f); //Rojo
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f); //Amarillo
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 1.0f, 1.0f); //Azul claro
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// Point light 5
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].position"), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].quadratic"), 0.032f);

		// Point light 6
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].position"), pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].quadratic"), 0.032f);

		// Point light 7
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].position"), pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].quadratic"), 0.032f);

		// Point light 8
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].position"), pointLightPositions[7].x, pointLightPositions[7].y, pointLightPositions[7].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].quadratic"), 0.032f);

		// Point light 9
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].position"), pointLightPositions[8].x, pointLightPositions[8].y, pointLightPositions[8].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].quadratic"), 0.032f);

		// Point light 10
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].position"), pointLightPositions[9].x, pointLightPositions[9].y, pointLightPositions[9].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].quadratic"), 0.032f);

		// Point light 11
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].position"), pointLightPositions[10].x, pointLightPositions[10].y, pointLightPositions[10].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].quadratic"), 0.032f);

		// Point light 12
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].position"), pointLightPositions[11].x, pointLightPositions[11].y, pointLightPositions[11].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].ambient"), Light.x, Light.y, Light.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].quadratic"), 0.032f);

		//Iluminacion destinada al exterior del arcade
		// Point light 13
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].position"), pointLightPositions[12].x, pointLightPositions[12].y, pointLightPositions[12].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].ambient"), LightExt.x, LightExt.y, LightExt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].quadratic"), 0.32f);

		// Point light 14
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].position"), pointLightPositions[13].x, pointLightPositions[13].y, pointLightPositions[13].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].ambient"), LightExt.x, LightExt.y, LightExt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].quadratic"), 0.32f);


		//Iluminacion destinada al interior del arcade
		// Point light 15
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].position"), pointLightPositions[14].x, pointLightPositions[14].y, pointLightPositions[14].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].ambient"), LightInt.x, LightInt.y, LightInt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].quadratic"), 0.32f);

		// Point light 16
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].position"), pointLightPositions[15].x, pointLightPositions[15].y, pointLightPositions[15].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].ambient"), LightInt.x, LightInt.y, LightInt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].quadratic"), 0.32f);

		// Point light 17
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].position"), pointLightPositions[16].x, pointLightPositions[16].y, pointLightPositions[16].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].ambient"), LightInt.x, LightInt.y, LightInt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].quadratic"), 0.32f);

		// Point light 18
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].position"), pointLightPositions[17].x, pointLightPositions[17].y, pointLightPositions[17].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].ambient"), LightInt.x, LightInt.y, LightInt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].quadratic"), 0.32f);

		// Point light 19
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].position"), pointLightPositions[18].x, pointLightPositions[18].y, pointLightPositions[18].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].ambient"), LightInt.x, LightInt.y, LightInt.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].quadratic"), 0.32f);

		//Iluminacion destinada a la camara
		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		//Esta configuracion es una variante que tenia en mente para la iluminacion de las luces del estacionamiento, pero solo es un concepto 
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), pointLightPositions[19].x, pointLightPositions[19].y, pointLightPositions[19].z);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), pointLightPositions[20].x, pointLightPositions[20].y, pointLightPositions[20].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), LightC.x, LightC.y, LightC.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.05f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.32f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(range)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(spotAngle)));


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//Carga y dibujado de los objetps implementados en el proyecto 
		glBindVertexArray(VAO);
		glm::mat4 model(1);

		//Entorno
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Entorno.Draw(lightingShader);

		//Luces exterior del arcade
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Luces1.Draw(lightingShader);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Luces2.Draw(lightingShader);

		//Maquinita
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Maquinita.Draw(lightingShader);

		//Maquina de refrescos
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Refrescos.Draw(lightingShader);

		//Botes de basura
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BoteBasura.Draw(lightingShader);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		BoteBasura2.Draw(lightingShader);

		//Sillas exteriores
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//Lamparas exteriores (Estacionamiento)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LamparaExterior.Draw(lightingShader);

		//Archivero
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Archivero.Draw(lightingShader);

		//Maquinita de simulacion de conduccion 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Moto.Draw(lightingShader);

		//Pista de baile
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PistaBaile.Draw(lightingShader);

		//Estante
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Estante.Draw(lightingShader);

		//Marco de las puertas de madera
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta.Draw(lightingShader);

		//Whack a Mole (Pegale al Topo)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Waka.Draw(lightingShader);

		//Escritorio
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio.Draw(lightingShader);

		//Parque
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Parque.Draw(lightingShader);
		
		//Juego basquetball
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Basquet.Draw(lightingShader);

		//Puertas de cristal
		model = glm::translate(model, glm::vec3(-1.2, 1.6, -12.35));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaCristal1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(2, 1.6, -12.35));
		model = glm::rotate(model, glm::radians(rot2), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaCristal2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(10.3, 1.6, -25.5));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(rot2), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaCristal1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(10.3, 1.6, -28.7));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaCristal2.Draw(lightingShader);

		//Puertas de madera
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-9.45, 1.5, -27.2));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-13.6, 1.5, -22.1));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta2.Draw(lightingShader);

		//Columpio
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(23.9, 4.8, -28.3));
		model = glm::rotate(model, glm::radians(rot3), glm::vec3(1.0f, 0.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Parque1.Draw(lightingShader);

		//Cajones de escritorio
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-10.25, 1.0, -23.25));
		model = glm::translate(model, glm::vec3(-(despX1), 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-10.25, 0.7, -23.25));
		model = glm::translate(model, glm::vec3(-(despX2), 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-10.25, 0.4, -23.25));
		model = glm::translate(model, glm::vec3(-(despX3), 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-10.25, 0.1, -23.25));
		model = glm::translate(model, glm::vec3(-(despX4), 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio1.Draw(lightingShader);

		//Topos del juego Whack a Mole (Pegale al Topo)
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-8.44, 1.25, -24.35));
		model = glm::translate(model, glm::vec3(0, posYTopo1, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Topo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-8.44, 1.25, -24.85));
		model = glm::translate(model, glm::vec3(0, posYTopo2, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Topo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-7.92, 1.25, -24.85));
		model = glm::translate(model, glm::vec3(0, posYTopo3, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Topo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-7.92, 1.25, -24.35));
		model = glm::translate(model, glm::vec3(0, posYTopo4, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Topo.Draw(lightingShader);

		//Balon de Basquetball
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-5.4, 1.05, -29.45));
		model = glm::translate(model, glm::vec3(0,posYBalon,posZBalon));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Balon.Draw(lightingShader);

		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Primeramente me ayudo en la ubicacion de las luces PointLight y SpotLight
		//Draw the light object (using light's vertex attributes)
		//glBindVertexArray(lightVAO);
		//for (GLuint i = 0; i < 21; i++)
		//{
		//	model = glm::mat4(1);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}

void animacion()
{
	//Animacion de las puertas
	if (animacion1) 
	{
		if (abrir and rot1 < 90 and rot2 > -90)
		{
			rot1 += 1.0f;
			rot2 -= 1.0f;

			if (rot1 > 90 and rot2 > -90)
			{
				animacion1 = false;
				abrir = false;
			}
		}
		
	}

	//Animacion del columpio
	if (animacion2)
	{
		if (giro1)
		{
			rot3 += 1.0f;
			if (rot3 > 60)
			{
				giro1 = false;
				giro2 = true;
			}

		}

		if (giro2) 
		{
			rot3 -= 1.0f;
			if (rot3 < -60) 
			{
				giro2 = false;
				giro1 = true;
			}
		}
	
	}

	//Animacion de los cajones del escritorio
	if (animacion3) 
	{
		if (des1)
		{
			despX1 += 0.001f;
			despX2 += 0.003f;
			despX3 += 0.007f;
			despX4 += 0.01f;

			if (despX1 > 0.1 and despX2 > 0.2 and despX3 > 0.3 and despX4 > 0.4)
			{
				des1 = false;
				des2 = true;
			}
		}

		if (des2)
		{
			despX1 -= 0.001f;
			despX2 -= 0.003f;
			despX3 -= 0.007f;
			despX4 -= 0.01f;

			if (despX1 < 0 and despX2 < 0 and despX3 < 0 and despX4 < 0)
			{
				des2 = false;
				des1 = true;
			}
		}
	}

	//Movimiento destinado a las animaciones del Whak a Mole y de la pelota de Basquetball
	if (play)
	{
		if (i_curr_steps >= i_max_steps) 
		{
			playIndex++;
			if (playIndex>FrameIndex - 2)	
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Dibujado de la animacion
			posYTopo1 += KeyFrame[playIndex].incYTopo1;
			posYTopo2 += KeyFrame[playIndex].incYTopo2;
			posYTopo3 += KeyFrame[playIndex].incYTopo3;
			posYTopo4 += KeyFrame[playIndex].incYTopo4;

			posZBalon += KeyFrame[playIndex].incZBalon;
			posYBalon += KeyFrame[playIndex].incYBalon;

	
			i_curr_steps++;
		}
	
	}

}



// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	//Realiza la animacion por Keyframe del Whack a Mole y del balon de Basquetball
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	//Realiza el guardado de los frames
	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}

	}

	//Cierra la ventana de ejecucion 
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	//Activacion y desactivacion de la iluminacion del entorno
	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light = glm::vec3(10.0f, 10.0f, 10.0f);
			LightExt = glm::vec3(20.0f, 20.0f, 20.0f);
			LightInt = glm::vec3(30.0f, 30.0f, 30.0f);
			LightC = glm::vec3(500.0f, 500.0f, 500.0f);
		}


		else 
		{
			Light = glm::vec3(0.0f, 0.0f, 0.0f);
			LightExt = glm::vec3(0.0f, 0.0f, 0.0f);
			LightInt = glm::vec3(0.0f, 0.0f, 0.0f);
			LightC = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	//Activacion del modo dia y noche en el entorno
	if (keys[GLFW_KEY_N])
	{
		active = !active;
		if (active)
			LightA = glm::vec3(0.0f, 0.0f, 0.0f);
		else
			LightA = glm::vec3(0.7f, 0.7f, 0.7f);
	}

	//Activamos la animacion de las puertas
	if (keys[GLFW_KEY_T])
	{
		animacion1 = true;
		abrir = true; 
	}

	//Activamos animacion del columpio
	if (keys[GLFW_KEY_Y])
	{
		animacion2 = true;
	}

	//Desactivamos animacion del columpio
	if (keys[GLFW_KEY_H])
	{
		animacion2 = false;
	}

	//Activamos animacion de los cajones del escritorio
	if (keys[GLFW_KEY_U])
	{
		animacion3 = true;
	}

	//Desactivamos animacion de los cajones del escritorio
	if (keys[GLFW_KEY_J])
	{
		animacion3 = false;
	}

}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	//Permite ajustar el rango de la Spotlight
	if (keys[GLFW_KEY_1])
	{
		range += 0.1;
		printf("El rango es %f\n", range);

	}

	//Permite ajustar el rango de la Spotlight
	if (keys[GLFW_KEY_2])
	{
		range -= 0.1;
		printf("El rango es %f\n", range);
			
	}

	//Permite ajustar el angulo de la Spotlight
	if (keys[GLFW_KEY_3])
	{
		spotAngle += 0.1;
		printf("El spotangle es %f\n", spotAngle);
	}

	//Permite ajustar el angulo de la Spotlight
	if (keys[GLFW_KEY_4])
	{
		spotAngle -= 0.1;
		printf("El spotangle es %f\n", spotAngle);
	}



	//Permite ajustar el movimiento de los topos para la animacion por keyframe
	if (keys[GLFW_KEY_Z])
	{
		if (posYTopo1 < 0.25)
		{
			posYTopo1 += 0.05f;
		}
	}

	if (keys[GLFW_KEY_X])
	{
		if (posYTopo1 > 0)
		{
			posYTopo1 -= 0.05f;
		}
	}

	if (keys[GLFW_KEY_C])
	{
		if (posYTopo2 < 0.25)
		{
			posYTopo2 += 0.05f;
		}
	}

	if (keys[GLFW_KEY_V])
	{
		if (posYTopo2 > 0)
		{
			posYTopo2 -= 0.05f;
		}
	}

	if (keys[GLFW_KEY_5])
	{
		if (posYTopo3 < 0.25)
		{
			posYTopo3 += 0.05f;
		}
	}

	if (keys[GLFW_KEY_6])
	{
		if (posYTopo3 > 0)
		{
			posYTopo3 -= 0.05f;
		}
	}

	if (keys[GLFW_KEY_7])
	{
		if (posYTopo4 < 0.25)
		{
			posYTopo4 += 0.05f;
		}
	}

	if (keys[GLFW_KEY_8])
	{
		if (posYTopo4 > 0)
		{
			posYTopo4 -= 0.05f;
		}
	}

	//Permite ajustar el movimiento del balon de basquetball para la animacion por keyframe
	if (keys[GLFW_KEY_O])
	{
		if (posZBalon > -2)
		{
			posZBalon -= 0.05f;
		}

	}
	if (keys[GLFW_KEY_P])
	{
		if (posZBalon <= 0)
		{
			posZBalon += 0.05f;
		}

	}

	if (keys[GLFW_KEY_9])
	{
		if (posYBalon < 1.5)
		{
			posYBalon += 0.05f;
		}
	}

	if (keys[GLFW_KEY_0])
	{
		if (posYBalon >= 0)
		{
			posYBalon -= 0.05f;
		}

	}



	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}