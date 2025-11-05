//para cargar imagen
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

//Para recursos
#include "Recursos.h"

const float toRadians = 3.14159265f / 180.0f;
using std::vector;
const float PI = 3.14159265f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

int pointLightCount = 0;
int spotLightCount = 0;

//Cambio Textura Humo

float tiempoAcumuladoHumo = 0.0f;
float intervaloCambioHumo = 5.0f;
int frameX = 0; // Columna actual (0-7)
int frameY = 0; // Fila actual (0-7)
float toffsetnumerocambiau = 0.0;
float toffsetnumerocambiav = 0.0;

//Animación Silla
bool agarrarSilla;

float angulosol = 90.0f;

float anguloBrazos = 0.0f;
float anguloPiernas = 0.0f;
float anguloBrazos2 = 0.0f;
float anguloPiernas2 = 0.0f;
float anguloRotacion = 0.0f;
float posicionAnimacion[3] = { 0.0f, 0.0f, 0.0f };
int faseAnimacion = 0;

float anguloMachamp = 0.0f;
float anguloMachamp2 = 0.0f;
float movimientoMachamp = 0.0f;

float anguloCharmander = 0.0f;
float movimientoCharmanderX = 0.0f;
float movimientoCharmanderZ = 0.0f;
int orientacionCharmander = 0;

bool animacionEnCurso = false;
bool puedeAnimar = false;
// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

void inputKeyframes(bool* keys);

//============ VARIABLES PARA KEYFRAMES CÍCLICOS ============
float movGlobo_x = 0.0f, movGlobo_y = 0.0f, movGlobo_z = 0.0f;
float giroGlobo = 0.0f;

#define MAX_FRAMES 100
int i_max_steps = 50;  // Pasos de interpolación entre frames
int i_curr_steps = 0;

typedef struct _frame
{
    float movGlobo_x;
    float movGlobo_y;
    float movGlobo_z;
    float movGlobo_xInc;  // Incrementos para interpolación
    float movGlobo_yInc;
    float movGlobo_zInc;
    float giroGlobo;
    float giroGloboInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = true;  // Siempre activa
int playIndex = 0;
//============ FUNCIÓN PARA NORMALIZAR ÁNGULOS ============
float normalizarAngulo(float angulo)
{
	while (angulo > 360.0f)
		angulo -= 360.0f;
	while (angulo < 0.0f)
		angulo += 360.0f;
	return angulo;
}

//============ FUNCIÓN PARA CALCULAR DIFERENCIA ANGULAR MÁS CORTA ============
float diferenciaAngular(float anguloActual, float anguloDestino)
{
	// Normalizar ambos ángulos
	anguloActual = normalizarAngulo(anguloActual);
	anguloDestino = normalizarAngulo(anguloDestino);

	// Calcular diferencia
	float diff = anguloDestino - anguloActual;

	// Ajustar para tomar el camino más corto
	if (diff > 180.0f)
		diff -= 360.0f;
	else if (diff < -180.0f)
		diff += 360.0f;

	return diff;
}

//============ FUNCIÓN DE INTERPOLACIÓN MODIFICADA ============
void interpolation(void)
{
	// Calcular siguiente frame (con ciclo)
	int nextFrame = (playIndex + 1) % FrameIndex;

	// Interpolación normal para posiciones
	KeyFrame[playIndex].movGlobo_xInc = (KeyFrame[nextFrame].movGlobo_x - KeyFrame[playIndex].movGlobo_x) / i_max_steps;
	KeyFrame[playIndex].movGlobo_yInc = (KeyFrame[nextFrame].movGlobo_y - KeyFrame[playIndex].movGlobo_y) / i_max_steps;
	KeyFrame[playIndex].movGlobo_zInc = (KeyFrame[nextFrame].movGlobo_z - KeyFrame[playIndex].movGlobo_z) / i_max_steps;

	// Interpolación angular con camino más corto
	float diffAngular = diferenciaAngular(KeyFrame[playIndex].giroGlobo, KeyFrame[nextFrame].giroGlobo);
	KeyFrame[playIndex].giroGloboInc = diffAngular / i_max_steps;
}

//============ FUNCIÓN DE ANIMACIÓN MODIFICADA ============
void animate(void)
{
	if (play && FrameIndex > 1)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;

			if (playIndex >= FrameIndex)
			{
				playIndex = 0;
			}

			i_curr_steps = 0;
			interpolation();
		}
		else
		{
			movGlobo_x += KeyFrame[playIndex].movGlobo_xInc;
			movGlobo_y += KeyFrame[playIndex].movGlobo_yInc;
			movGlobo_z += KeyFrame[playIndex].movGlobo_zInc;
			giroGlobo += KeyFrame[playIndex].giroGloboInc;

			// Normalizar el ángulo después de actualizar
			giroGlobo = normalizarAngulo(giroGlobo);

			i_curr_steps++;
		}
	}
}

//============ CARGAR KEYFRAMES DESDE ARCHIVO ============
void loadKeyframesFromFile(void)
{
    std::ifstream file("keyframes.txt");
    if (file.is_open())
    {
        file >> FrameIndex;
        for (int i = 0; i < FrameIndex; i++)
        {
            file >> KeyFrame[i].movGlobo_x
                 >> KeyFrame[i].movGlobo_y
                 >> KeyFrame[i].movGlobo_z
                 >> KeyFrame[i].giroGlobo;
        }
        file.close();
        printf("Keyframes cargados: %d frames\n", FrameIndex);
        
        // Inicializar la animación
        if (FrameIndex > 1)
        {
            interpolation();
        }
    }
    else
    {
        printf("Error: No se encontró keyframes.txt\n");
        FrameIndex = 0;
        play = false;
    }
}

//funci�n de calculo de normales por promedio de v�rtices 
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


/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {
	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	// ===== PAREDES LATERALES (usarán el aro circular de la textura) =====
	for (n = 0; n <= res; n++) {
		float angle = n * dt;
		if (n != res) {
			x = R * cos(angle);
			z = R * sin(angle);
		}
		//caso para terminar el círculo
		else {
			x = R * cos(0);
			z = R * sin(0);
		}

		// Calcular coordenadas de textura para el aro circular
		// El aro circular está entre radio 0.5 y 1.0 en la textura
		float texU_outer = 0.5f + 0.5f * cos(angle);  // Borde exterior del aro
		float texV_outer = 0.5f + 0.5f * sin(angle);
		float texU_inner = 0.5f + 0.25f * cos(angle); // Borde interior del aro  
		float texV_inner = 0.5f + 0.25f * sin(angle);

		// Calcular normales para las paredes (radiales)
		float nx = cos(angle);
		float nz = sin(angle);

		// Vértice INFERIOR de la pared - usa borde EXTERIOR del aro (parte de abajo)
		vertices.push_back(x);        // posición x
		vertices.push_back(-0.5f);    // posición y
		vertices.push_back(z);        // posición z
		vertices.push_back(texU_outer); // coordenada u (borde exterior del aro)
		vertices.push_back(texV_outer); // coordenada v
		vertices.push_back(nx);       // normal x
		vertices.push_back(0.0f);     // normal y
		vertices.push_back(nz);       // normal z

		// Vértice SUPERIOR de la pared - usa borde INTERIOR del aro (parte de arriba)
		vertices.push_back(x);        // posición x
		vertices.push_back(0.5f);     // posición y
		vertices.push_back(z);        // posición z
		vertices.push_back(texU_inner); // coordenada u (borde interior del aro)
		vertices.push_back(texV_inner); // coordenada v
		vertices.push_back(nx);       // normal x
		vertices.push_back(0.0f);     // normal y
		vertices.push_back(nz);       // normal z
	}

	// ===== TAPA INFERIOR (círculo completo 1:1) =====
	int baseInferior = vertices.size() / 8; // Índice base para la tapa inferior

	// Centro de la tapa inferior
	vertices.push_back(0.0f);     // x
	vertices.push_back(-0.5f);    // y
	vertices.push_back(0.0f);     // z
	vertices.push_back(0.5f);     // u (centro de la textura)
	vertices.push_back(0.5f);     // v (centro de la textura)
	vertices.push_back(0.0f);     // normal x
	vertices.push_back(-1.0f);    // normal y (apunta hacia abajo)
	vertices.push_back(0.0f);     // normal z

	// Vértices del borde de la tapa inferior
	for (n = 0; n <= res; n++) {
		float angle = n * dt;
		x = R * cos(angle);
		z = R * sin(angle);

		// Coordenadas de textura: círculo completo centrado
		float texU = 0.5f + 0.5f * cos(angle);
		float texV = 0.5f + 0.5f * sin(angle);

		vertices.push_back(x);    // x
		vertices.push_back(-0.5f); // y
		vertices.push_back(z);    // z
		vertices.push_back(texU); // u (círculo completo)
		vertices.push_back(texV); // v (círculo completo)
		vertices.push_back(0.0f); // normal x
		vertices.push_back(-1.0f); // normal y (apunta hacia abajo)
		vertices.push_back(0.0f); // normal z
	}

	// ===== TAPA SUPERIOR (círculo completo 1:1) =====
	int baseSuperior = vertices.size() / 8; // Índice base para la tapa superior

	// Centro de la tapa superior
	vertices.push_back(0.0f);     // x
	vertices.push_back(0.5f);     // y
	vertices.push_back(0.0f);     // z
	vertices.push_back(0.5f);     // u (centro de la textura)
	vertices.push_back(0.5f);     // v (centro de la textura)
	vertices.push_back(0.0f);     // normal x
	vertices.push_back(1.0f);     // normal y (apunta hacia arriba)
	vertices.push_back(0.0f);     // normal z

	// Vértices del borde de la tapa superior
	for (n = 0; n <= res; n++) {
		float angle = n * dt;
		x = R * cos(angle);
		z = R * sin(angle);

		// Coordenadas de textura: círculo completo centrado
		float texU = 0.5f + 0.5f * cos(angle);
		float texV = 0.5f + 0.5f * sin(angle);

		vertices.push_back(x);    // x
		vertices.push_back(0.5f); // y
		vertices.push_back(z);    // z
		vertices.push_back(texU); // u (círculo completo)
		vertices.push_back(texV); // v (círculo completo)
		vertices.push_back(0.0f); // normal x
		vertices.push_back(1.0f); // normal y (apunta hacia arriba)
		vertices.push_back(0.0f); // normal z
	}

	// ===== GENERAR ÍNDICES =====

	// Índices para las PAREDES LATERALES (usando triangle strips)
	for (n = 0; n < res; n++) {
		int base = n * 2;

		// Primer triángulo (sentido antihorario)
		indices.push_back(base);
		indices.push_back(base + 2);
		indices.push_back(base + 1);

		// Segundo triángulo
		indices.push_back(base + 1);
		indices.push_back(base + 2);
		indices.push_back(base + 3);
	}

	// Índices para la TAPA INFERIOR (triangle fan)
	int centroInferior = baseInferior;
	int primerVerticeInferior = baseInferior + 1;
	for (n = 0; n < res; n++) {
		indices.push_back(centroInferior);
		indices.push_back(primerVerticeInferior + n);
		indices.push_back(primerVerticeInferior + ((n + 1) % res));
	}

	// Índices para la TAPA SUPERIOR (triangle fan)
	int centroSuperior = baseSuperior;
	int primerVerticeSuperior = baseSuperior + 1;
	for (n = 0; n < res; n++) {
		indices.push_back(centroSuperior);
		indices.push_back(primerVerticeSuperior + ((n + 1) % res)); // Sentido invertido para normal correcta
		indices.push_back(primerVerticeSuperior + n);
	}

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(cilindro);
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

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
		
	unsigned int Suelo3DIndices[] = {
		0, 1, 2,
		0, 2, 3,
		3, 2, 4,
		3, 4, 5,
		6, 7, 4,
		6, 4, 2,
		7, 8, 9,
		8, 7, 10,
		10, 7, 6,
		11, 12, 13,
		11, 14, 12,
		10, 6, 12,
		10, 12, 14,
		15, 8, 16,
		17, 18, 8,
		10, 16, 8,
		19, 20, 8,
		19, 8, 21,
		21, 22, 23,
		22, 24, 25,
		26, 27, 28,
		26, 24, 27,
		27, 29, 30,
		27, 24, 29,
		29, 22, 31,
		29, 24, 22,
		32, 33, 18,
		34, 35, 36,
		34, 36, 17,
		37, 16, 38,
		37, 15, 16,
		39, 40, 41,
		39, 42, 40,
		43, 44, 37,
		43, 37, 45,
		42, 46, 45,
		42, 47, 46,
		37, 40, 45,
		37, 38, 40,
		38, 48, 40,
		40, 42, 45,
		15, 17, 8,
		8, 22, 21,
		22, 18, 31,
		22, 8, 18,
		18, 17, 32,
		32, 17, 36,
		49, 50, 51,
		49, 52, 50,
		52, 53, 50,
		52, 54, 53,
		55, 53, 56,
		55, 50, 53,
		56, 57, 58,
		58, 59, 56,
		59, 55, 56,
		60, 61, 62,
		60, 62, 63,
		59, 62, 55,
		59, 63, 62,
		64, 65, 58,
		66, 58, 67,
		59, 58, 65,
		68, 58, 69,
		68, 70, 58,
		70, 71, 72,
		72, 73, 74,
		75, 76, 77,
		75, 77, 74,
		77, 78, 79,
		77, 79, 74,
		79, 80, 72,
		79, 72, 74,
		81, 67, 82,
		81, 83, 84,
		85, 84, 86,
		85, 66, 84,
		87, 88, 65,
		87, 65, 64,
		89, 90, 91,
		89, 91, 92,
		93, 87, 94,
		93, 95, 87,
		92, 95, 96,
		92, 96, 97,
		87, 95, 91,
		87, 91, 88,
		88, 91, 98,
		91, 95, 92,
		64, 58, 66,
		58, 70, 72,
		72, 80, 67,
		72, 67, 58,
		67, 81, 66,
		81, 84, 66,
		99, 100, 101,
		102, 103, 104,
		105, 106, 107,
		108, 109, 110,
		111, 112, 113,
		114, 115, 116,
		117, 118, 119,
		120, 121, 122,
		123, 124, 125,
		125, 126, 123,
		127, 128, 129,
		130, 131, 132,
		133, 134, 135,
		134, 136, 135,
		137, 138, 139,
		140, 141, 142,
		143, 144, 145,
		145, 144, 146,
		147, 148, 149,
		150, 151, 152,
		153, 154, 155,
		156, 157, 158,
		159, 160, 161,
		160, 162, 161,
		163, 164, 165,
		163, 166, 164,
		167, 168, 169,
		170, 171, 172,
		173, 174, 175,
		176, 177, 174,
		178, 179, 180,
		181, 182, 183,
		184, 185, 186,
		187, 188, 189,
		190, 191, 192,
		193, 194, 195,
		196, 197, 194,
		198, 199, 200,
		201, 202, 203,
		204, 205, 206,
		207, 208, 209,
		210, 211, 212,
		213, 214, 215,
		216, 217, 218,
		219, 220, 221,
		222, 223, 224,
		225, 226, 227,
		228, 229, 230,
		231, 232, 233,
		234, 235, 236,
		237, 238, 239,
		237, 239, 240,
		241, 242, 243,
		244, 245, 246,
		247, 248, 249,
		250, 251, 252,
		250, 253, 251,
		254, 255, 256,
		254, 256, 257,
		258, 259, 260,
		261, 262, 263,
		264, 265, 266,
		267, 268, 269,
		270, 271, 272,
		273, 274, 275,
		273, 276, 274,
		277, 278, 279,
		280, 281, 282,
		283, 284, 285,
		283, 286, 287,
		288, 289, 290,
		291, 292, 293,
		294, 295, 296,
		294, 297, 298,
		299, 300, 301,
		299, 302, 300,
		303, 304, 305,
		303, 306, 304,
		307, 308, 309,
		307, 310, 308,
		311, 312, 313,
		314, 315, 316,
		317, 318, 319,
		320, 321, 322,
		323, 324, 325,
		326, 327, 328,
		329, 330, 331,
		332, 333, 330,
		334, 335, 336,
		337, 338, 339,
		340, 341, 342,
		343, 344, 345,
		346, 347, 348,
		349, 350, 351,
		352, 353, 354,
		355, 356, 357,
		358, 359, 360,
		361, 362, 363,
		364, 365, 366,
		367, 368, 369,
		370, 371, 372,
		373, 32, 36
	};

	GLfloat Suelo3DVertices[] = {
		-70.5083f, -8.71033f, -53.8823f, 2.12505f, 1.59145f, 0.0f, 1.0f, 0.0f,
		-66.8547f, -8.71032f, -52.8837f, 1.61299f, 1.60634f, 0.0f, 1.0f, 0.0f,
		-65.6454f, -8.71032f, -13.6248f, 1.59496f, 2.19175f, 0.0f, 1.0f, 0.0f,
		-72.4452f, -8.71033f, 96.7844f, 2.15393f, 3.83812f, 0.0f, 1.0f, 0.0f,
		-45.6085f, -8.71032f, 92.3999f, 1.29618f, 3.77274f, 0.0f, 1.0f, 0.0f,
		-46.2548f, -8.71032f, 97.9802f, 1.30581f, 3.85595f, 0.0f, 1.0f, 0.0f,
		-30.4191f, -8.71032f, -12.8276f, 1.06968f, 2.20364f, 0.0f, 1.0f, 0.0f,
		-32.035f, -8.71032f, 92.3999f, 1.09377f, 3.77274f, 0.0f, 1.0f, 0.0f,
		-23.6324f, -8.71032f, 96.3858f, 0.968479f, 3.83217f, 0.0f, 1.0f, 0.0f,
		-32.035f, -8.71032f, 95.9872f, 1.09377f, 3.82623f, 0.0f, 1.0f, 0.0f,
		-25.2483f, -8.71032f, -12.429f, 0.992574f, 2.20958f, 0.0f, 1.0f, 0.0f,
		-33.6509f, -8.71032f, -61.8541f, 1.11787f, 1.47258f, 0.0f, 1.0f, 0.0f,
		-29.4496f, -8.71032f, -55.0781f, 1.05522f, 1.57362f, 0.0f, 1.0f, 0.0f,
		-33.3277f, -8.71032f, -55.0781f, 1.11305f, 1.57362f, 0.0f, 1.0f, 0.0f,
		-24.2788f, -8.71032f, -61.8541f, 0.978117f, 1.47258f, 0.0f, 1.0f, 0.0f,
		12.2401f, -8.71031f, -12.0304f, 0.433564f, 2.21552f, 0.0f, 1.0f, 0.0f,
		-5.21138f, -8.71032f, -14.0234f, 0.693793f, 2.1858f, 0.0f, 1.0f, 0.0f,
		26.4599f, -8.71031f, -12.429f, 0.221526f, 2.20958f, 0.0f, 1.0f, 0.0f,
		76.5523f, -8.71031f, 2.31877f, -0.525428f, 2.42949f, 0.0f, 1.0f, 0.0f,
		98.8514f, -8.71031f, 100.372f, -0.857942f, 3.89161f, 0.0f, 1.0f, 0.0f,
		-23.6324f, -8.71032f, 104.358f, 0.968479f, 3.95105f, 0.0f, 1.0f, 0.0f,
		99.1746f, -8.71031f, 75.6592f, -0.862761f, 3.52311f, 0.0f, 1.0f, 0.0f,
		104.022f, -8.71031f, 46.1636f, -0.935047f, 3.08328f, 0.0f, 1.0f, 0.0f,
		104.022f, -8.71031f, 74.862f, -0.935047f, 3.51122f, 0.0f, 1.0f, 0.0f,
		104.345f, -8.71031f, -105.699f, -0.939866f, 0.818784f, 0.0f, 1.0f, 0.0f,
		106.285f, -8.71031f, 46.1636f, -0.968781f, 3.08328f, 0.0f, 1.0f, 0.0f,
		52.9604f, -8.71031f, -106.097f, -0.173637f, 0.812839f, 0.0f, 1.0f, 0.0f,
		80.7536f, -8.71031f, -68.2315f, -0.588075f, 1.37748f, 0.0f, 1.0f, 0.0f,
		51.9909f, -8.71031f, -69.8259f, -0.15918f, 1.3537f, 0.0f, 1.0f, 0.0f,
		85.9244f, -8.71031f, -51.8894f, -0.66518f, 1.62116f, 0.0f, 1.0f, 0.0f,
		79.784f, -8.71031f, -51.8894f, -0.573618f, 1.62116f, 0.0f, 1.0f, 0.0f,
		86.2476f, -8.71031f, 3.91312f, -0.669999f, 2.45327f, 0.0f, 1.0f, 0.0f,
		70.4119f, -8.71031f, -34.3514f, -0.433865f, 1.88268f, 0.0f, 1.0f, 0.0f,
		77.1986f, -8.71031f, -33.5543f, -0.535066f, 1.89457f, 0.0f, 1.0f, 0.0f,
		26.7831f, -8.71031f, -55.4767f, 0.216707f, 1.56767f, 0.0f, 1.0f, 0.0f,
		61.363f, -8.71031f, -55.4767f, -0.298932f, 1.56767f, 0.0f, 1.0f, 0.0f,
		60.3934f, -8.71031f, -42.3232f, -0.284475f, 1.76381f, 0.0f, 1.0f, 0.0f,
		12.2401f, -8.71031f, -25.1839f, 0.433564f, 2.01938f, 0.0f, 1.0f, 0.0f,
		-5.53456f, -8.71032f, -23.9881f, 0.698612f, 2.03722f, 0.0f, 1.0f, 0.0f,
		-15.553f, -8.71031f, -71.8188f, 0.848003f, 1.32399f, 0.0f, 1.0f, 0.0f,
		-9.41268f, -8.71032f, -29.1698f, 0.756441f, 1.95995f, 0.0f, 1.0f, 0.0f,
		-15.553f, -8.71032f, -29.1698f, 0.848003f, 1.95995f, 0.0f, 1.0f, 0.0f,
		-8.44315f, -8.71031f, -73.0146f, 0.741984f, 1.30616f, 0.0f, 1.0f, 0.0f,
		18.7037f, -8.71031f, -71.4202f, 0.337183f, 1.32993f, 0.0f, 1.0f, 0.0f,
		17.7341f, -8.71031f, -25.1839f, 0.35164f, 2.01938f, 0.0f, 1.0f, 0.0f,
		12.8865f, -8.71031f, -72.616f, 0.423926f, 1.3121f, 0.0f, 1.0f, 0.0f,
		12.8865f, -8.71031f, -75.4061f, 0.423926f, 1.27049f, 0.0f, 1.0f, 0.0f,
		-8.44315f, -8.71031f, -76.2033f, 0.741984f, 1.25861f, 0.0f, 1.0f, 0.0f,
		-9.41268f, -8.71032f, -23.9881f, 0.756441f, 2.03722f, 0.0f, 1.0f, 0.0f,
		-65.0426f, -4.2923f, -52.4718f, 2.04355f, 1.61248f, 0.0f, -1.0f, 0.0f,
		-61.547f, -4.2923f, -14.2777f, 1.53384f, 2.18201f, 0.0f, -1.0f, 0.0f,
		-62.7098f, -4.2923f, -51.5244f, 1.55118f, 1.62661f, 0.0f, -1.0f, 0.0f,
		-66.905f, -4.2923f, 90.4724f, 2.07132f, 3.744f, 0.0f, -1.0f, 0.0f,
		-42.2807f, -4.2923f, 86.3127f, 1.24655f, 3.68197f, 0.0f, -1.0f, 0.0f,
		-42.9022f, -4.2923f, 91.6069f, 1.25582f, 3.76091f, 0.0f, -1.0f, 0.0f,
		-27.6756f, -4.2923f, -13.5214f, 1.02877f, 2.19329f, 0.0f, -1.0f, 0.0f,
		-29.2293f, -4.2923f, 86.3127f, 1.05194f, 3.68197f, 0.0f, -1.0f, 0.0f,
		-29.2293f, -4.2923f, 89.7161f, 1.05194f, 3.73272f, 0.0f, -1.0f, 0.0f,
		-21.1499f, -4.2923f, 90.0943f, 0.931461f, 3.73836f, 0.0f, -1.0f, 0.0f,
		-22.7036f, -4.2923f, -13.1432f, 0.954629f, 2.19893f, 0.0f, -1.0f, 0.0f,
		-30.7831f, -4.2923f, -60.035f, 1.07511f, 1.4997f, 0.0f, -1.0f, 0.0f,
		-30.4723f, -4.2923f, -53.6063f, 1.07047f, 1.59556f, 0.0f, -1.0f, 0.0f,
		-26.7433f, -4.2923f, -53.6063f, 1.01487f, 1.59556f, 0.0f, -1.0f, 0.0f,
		-21.7714f, -4.2923f, -60.035f, 0.940728f, 1.4997f, 0.0f, -1.0f, 0.0f,
		13.343f, -4.2923f, -12.7651f, 0.417119f, 2.20457f, 0.0f, -1.0f, 0.0f,
		-3.43734f, -4.2923f, -14.6559f, 0.667339f, 2.17637f, 0.0f, -1.0f, 0.0f,
		27.0158f, -4.2923f, -13.1432f, 0.213236f, 2.19893f, 0.0f, -1.0f, 0.0f,
		75.1816f, -4.29229f, 0.84865f, -0.504989f, 2.40757f, 0.0f, -1.0f, 0.0f,
		96.6231f, -4.29229f, 93.8758f, -0.824714f, 3.79475f, 0.0f, -1.0f, 0.0f,
		-21.1499f, -4.2923f, 97.6574f, 0.931461f, 3.85114f, 0.0f, -1.0f, 0.0f,
		96.9338f, -4.2923f, 70.43f, -0.829348f, 3.44513f, 0.0f, -1.0f, 0.0f,
		101.595f, -4.29229f, 69.6737f, -0.898854f, 3.43385f, 0.0f, -1.0f, 0.0f,
		101.595f, -4.29229f, 42.4462f, -0.898853f, 3.02785f, 0.0f, -1.0f, 0.0f,
		103.77f, -4.29229f, 42.4462f, -0.931289f, 3.02785f, 0.0f, -1.0f, 0.0f,
		101.906f, -4.29229f, -101.633f, -0.903487f, 0.879419f, 0.0f, -1.0f, 0.0f,
		52.4971f, -4.2923f, -102.011f, -0.166728f, 0.87378f, 0.0f, -1.0f, 0.0f,
		51.5648f, -4.2923f, -67.5982f, -0.152827f, 1.38692f, 0.0f, -1.0f, 0.0f,
		79.2213f, -4.29229f, -66.0856f, -0.565227f, 1.40948f, 0.0f, -1.0f, 0.0f,
		78.289f, -4.29229f, -50.581f, -0.551326f, 1.64068f, 0.0f, -1.0f, 0.0f,
		84.1932f, -4.29229f, -50.581f, -0.639366f, 1.64068f, 0.0f, -1.0f, 0.0f,
		84.504f, -4.29229f, 2.36128f, -0.644f, 2.43012f, 0.0f, -1.0f, 0.0f,
		69.2774f, -4.29229f, -33.942f, -0.416948f, 1.88879f, 0.0f, -1.0f, 0.0f,
		75.8031f, -4.29229f, -33.1857f, -0.514256f, 1.90006f, 0.0f, -1.0f, 0.0f,
		69.8989f, -4.29229f, -40.3707f, -0.426215f, 1.79293f, 0.0f, -1.0f, 0.0f,
		59.6442f, -4.29229f, -41.5052f, -0.273303f, 1.77601f, 0.0f, -1.0f, 0.0f,
		27.3266f, -4.2923f, -53.9844f, 0.208602f, 1.58993f, 0.0f, -1.0f, 0.0f,
		60.5765f, -4.29229f, -53.9845f, -0.287204f, 1.58992f, 0.0f, -1.0f, 0.0f,
		13.343f, -4.2923f, -25.2443f, 0.417119f, 2.01848f, 0.0f, -1.0f, 0.0f,
		-3.74809f, -4.2923f, -24.1099f, 0.671973f, 2.0354f, 0.0f, -1.0f, 0.0f,
		-13.3812f, -4.2923f, -69.489f, 0.815618f, 1.35873f, 0.0f, -1.0f, 0.0f,
		-13.3812f, -4.2923f, -29.0259f, 0.815618f, 1.96209f, 0.0f, -1.0f, 0.0f,
		-7.47705f, -4.2923f, -29.0259f, 0.727577f, 1.96209f, 0.0f, -1.0f, 0.0f,
		-6.54481f, -4.2923f, -70.6235f, 0.713676f, 1.34181f, 0.0f, -1.0f, 0.0f,
		19.5579f, -4.2923f, -69.1108f, 0.324445f, 1.36437f, 0.0f, -1.0f, 0.0f,
		18.6257f, -4.2923f, -25.2443f, 0.338346f, 2.01848f, 0.0f, -1.0f, 0.0f,
		13.9645f, -4.2923f, -70.2453f, 0.407852f, 1.34745f, 0.0f, -1.0f, 0.0f,
		13.9645f, -4.2923f, -72.8924f, 0.407852f, 1.30798f, 0.0f, -1.0f, 0.0f,
		-6.54481f, -4.2923f, -73.6487f, 0.713676f, 1.2967f, 0.0f, -1.0f, 0.0f,
		-7.47705f, -4.2923f, -24.1099f, 0.727577f, 2.0354f, 0.0f, -1.0f, 0.0f,
		-66.8547f, -8.71032f, -52.8837f, 1.61299f, 1.60634f, -0.3754f, 0.0678f, 0.9244f,
		-65.0426f, -4.2923f, -52.4718f, 2.04355f, 1.61248f, -0.3754f, 0.0678f, 0.9244f,
		-62.7098f, -4.2923f, -51.5244f, 1.55118f, 1.62661f, -0.3754f, 0.0678f, 0.9244f,
		-70.5083f, -8.71033f, -53.8823f, 2.12505f, 1.59145f, -0.2636f, 0.0182f, 0.9645f,
		-65.0426f, -4.2923f, -52.4718f, 2.04355f, 1.61248f, -0.2636f, 0.0182f, 0.9645f,
		-66.8547f, -8.71032f, -52.8837f, 1.61299f, 1.60634f, -0.2636f, 0.0182f, 0.9645f,
		-70.5083f, -8.71033f, -53.8823f, 2.12505f, 1.59145f, 0.6291f, -0.7773f, 0.0081f,
		-72.4452f, -8.71033f, 96.7844f, 2.15393f, 3.83812f, 0.6291f, -0.7773f, 0.0081f,
		-66.905f, -4.2923f, 90.4724f, 2.07132f, 3.744f, 0.6291f, -0.7773f, 0.0081f,
		-65.0426f, -4.2923f, -52.4718f, 2.04355f, 1.61248f, 0.6273f, -0.7787f, 0.0082f,
		-70.5083f, -8.71033f, -53.8823f, 2.12505f, 1.59145f, 0.6273f, -0.7787f, 0.0082f,
		-66.905f, -4.2923f, 90.4724f, 2.07132f, 3.744f, 0.6273f, -0.7787f, 0.0082f,
		-72.4452f, -8.71033f, 96.7844f, 2.15393f, 3.83812f, 0.0255f, -0.8294f, -0.5581f,
		-46.2548f, -8.71032f, 97.9802f, 1.30581f, 3.85595f, 0.0255f, -0.8294f, -0.5581f,
		-66.905f, -4.2923f, 90.4724f, 2.07132f, 3.744f, 0.0255f, -0.8294f, -0.5581f,
		-66.905f, -4.2923f, 90.4724f, 2.07132f, 3.744f, 0.0265f, -0.828f, -0.5601f,
		-46.2548f, -8.71032f, 97.9802f, 1.30581f, 3.85595f, 0.0265f, -0.828f, -0.5601f,
		-42.9022f, -4.2923f, 91.6069f, 1.25582f, 3.76091f, 0.0265f, -0.828f, -0.5601f,
		-21.1499f, -4.2923f, 97.6574f, 0.931461f, 3.85114f, -0.0181f, -0.8316f, -0.5551f,
		-23.6324f, -8.71032f, 104.358f, 0.968479f, 3.95105f, -0.0181f, -0.8316f, -0.5551f,
		98.8514f, -8.71031f, 100.372f, -0.857942f, 3.89161f, -0.0181f, -0.8316f, -0.5551f,
		96.6231f, -4.29229f, 93.8758f, -0.824714f, 3.79475f, -0.0179f, -0.8296f, -0.5581f,
		-21.1499f, -4.2923f, 97.6574f, 0.931461f, 3.85114f, -0.0179f, -0.8296f, -0.5581f,
		98.8514f, -8.71031f, 100.372f, -0.857942f, 3.89161f, -0.0179f, -0.8296f, -0.5581f,
		-23.6324f, -8.71032f, 96.3858f, 0.968479f, 3.83217f, 0.8718f, -0.4899f, 0.0f,
		-23.6324f, -8.71032f, 104.358f, 0.968479f, 3.95105f, 0.8718f, -0.4899f, 0.0f,
		-21.1499f, -4.2923f, 97.6574f, 0.931461f, 3.85114f, 0.8718f, -0.4899f, 0.0f,
		-21.1499f, -4.2923f, 90.0943f, 0.931461f, 3.73836f, 0.8718f, -0.4899f, 0.0f,
		-23.6324f, -8.71032f, 96.3858f, 0.968479f, 3.83217f, 0.0269f, -0.823f, -0.5673f,
		-21.1499f, -4.2923f, 90.0943f, 0.931461f, 3.73836f, 0.0269f, -0.823f, -0.5673f,
		-32.035f, -8.71032f, 95.9872f, 1.09377f, 3.82623f, 0.0269f, -0.823f, -0.5673f,
		-21.1499f, -4.2923f, 90.0943f, 0.931461f, 3.73836f, 0.0266f, -0.8228f, -0.5678f,
		-29.2293f, -4.2923f, 89.7161f, 1.05194f, 3.73272f, 0.0266f, -0.8228f, -0.5678f,
		-32.035f, -8.71032f, 95.9872f, 1.09377f, 3.82623f, 0.0266f, -0.8228f, -0.5678f,
		-32.035f, -8.71032f, 95.9872f, 1.09377f, 3.82623f, 0.8442f, -0.5361f, 0.0f,
		-29.2293f, -4.2923f, 89.7161f, 1.05194f, 3.73272f, 0.8442f, -0.5361f, 0.0f,
		-32.035f, -8.71032f, 92.3999f, 1.09377f, 3.77274f, 0.8442f, -0.5361f, 0.0f,
		-29.2293f, -4.2923f, 86.3127f, 1.05194f, 3.68197f, 0.8442f, -0.5361f, 0.0f,
		-46.2548f, -8.71032f, 97.9802f, 1.30581f, 3.85595f, -0.8564f, 0.5068f, -0.0992f,
		-45.6085f, -8.71032f, 92.3999f, 1.29618f, 3.77274f, -0.8564f, 0.5068f, -0.0992f,
		-42.9022f, -4.2923f, 91.6069f, 1.25582f, 3.76091f, -0.8564f, 0.5068f, -0.0992f,
		-42.9022f, -4.2923f, 91.6069f, 1.25582f, 3.76091f, -0.8563f, 0.5065f, -0.1005f,
		-45.6085f, -8.71032f, 92.3999f, 1.29618f, 3.77274f, -0.8563f, 0.5065f, -0.1005f,
		-42.2807f, -4.2923f, 86.3127f, 1.24655f, 3.68197f, -0.8563f, 0.5065f, -0.1005f,
		-45.6085f, -8.71032f, 92.3999f, 1.29618f, 3.77274f, 0.0f, -0.8093f, -0.5874f,
		-32.035f, -8.71032f, 92.3999f, 1.09377f, 3.77274f, 0.0f, -0.8093f, -0.5874f,
		-42.2807f, -4.2923f, 86.3127f, 1.24655f, 3.68197f, 0.0f, -0.8093f, -0.5874f,
		-29.2293f, -4.2923f, 86.3127f, 1.05194f, 3.68197f, 0.0f, -0.8093f, -0.5874f,
		96.9338f, -4.2923f, 70.43f, -0.829348f, 3.44513f, -0.8858f, -0.464f, -0.0117f,
		96.6231f, -4.29229f, 93.8758f, -0.824714f, 3.79475f, -0.8858f, -0.464f, -0.0117f,
		98.8514f, -8.71031f, 100.372f, -0.857942f, 3.89161f, -0.8858f, -0.464f, -0.0117f,
		98.8514f, -8.71031f, 100.372f, -0.857942f, 3.89161f, -0.8862f, -0.4632f, -0.0116f,
		99.1746f, -8.71031f, 75.6592f, -0.862761f, 3.52311f, -0.8862f, -0.4632f, -0.0116f,
		96.9338f, -4.2923f, 70.43f, -0.829348f, 3.44513f, -0.8862f, -0.4632f, -0.0116f,
		96.9338f, -4.2923f, 70.43f, -0.829348f, 3.44513f, -0.1001f, -0.7808f, -0.6168f,
		99.1746f, -8.71031f, 75.6592f, -0.862761f, 3.52311f, -0.1001f, -0.7808f, -0.6168f,
		101.595f, -4.29229f, 69.6737f, -0.898854f, 3.43385f, -0.1001f, -0.7808f, -0.6168f,
		99.1746f, -8.71031f, 75.6592f, -0.862761f, 3.52311f, -0.1015f, -0.7804f, -0.617f,
		104.022f, -8.71031f, 74.862f, -0.935047f, 3.51122f, -0.1015f, -0.7804f, -0.617f,
		101.595f, -4.29229f, 69.6737f, -0.898854f, 3.43385f, -0.1015f, -0.7804f, -0.617f,
		101.595f, -4.29229f, 69.6737f, -0.898854f, 3.43385f, -0.8764f, -0.4815f, 0.0f,
		104.022f, -8.71031f, 74.862f, -0.935047f, 3.51122f, -0.8764f, -0.4815f, 0.0f,
		101.595f, -4.29229f, 42.4462f, -0.898853f, 3.02785f, -0.8764f, -0.4815f, 0.0f,
		104.022f, -8.71031f, 46.1636f, -0.935047f, 3.08328f, -0.8764f, -0.4815f, 0.0f,
		104.022f, -8.71031f, 46.1636f, -0.935047f, 3.08328f, 0.0f, -0.6438f, -0.7652f,
		103.77f, -4.29229f, 42.4462f, -0.931289f, 3.02785f, 0.0f, -0.6438f, -0.7652f,
		101.595f, -4.29229f, 42.4462f, -0.898853f, 3.02785f, 0.0f, -0.6438f, -0.7652f,
		106.285f, -8.71031f, 46.1636f, -0.968781f, 3.08328f, 0.0f, -0.6438f, -0.7652f,
		106.285f, -8.71031f, 46.1636f, -0.968781f, 3.08328f, -0.872f, -0.4894f, 0.0113f,
		101.906f, -4.29229f, -101.633f, -0.903487f, 0.879419f, -0.872f, -0.4894f, 0.0112f,
		103.77f, -4.29229f, 42.4462f, -0.931289f, 3.02785f, -0.872f, -0.4894f, 0.0112f,
		106.285f, -8.71031f, 46.1636f, -0.968781f, 3.08328f, -0.8721f, -0.4892f, 0.0112f,
		104.345f, -8.71031f, -105.699f, -0.939866f, 0.818784f, -0.8721f, -0.4892f, 0.0113f,
		101.906f, -4.29229f, -101.633f, -0.903487f, 0.879419f, -0.8721f, -0.4892f, 0.0113f,
		52.9604f, -8.71031f, -106.097f, -0.173637f, 0.812839f, -0.0057f, -0.6789f, 0.7342f,
		101.906f, -4.29229f, -101.633f, -0.903487f, 0.879419f, -0.0038f, -0.6783f, 0.7348f,
		104.345f, -8.71031f, -105.699f, -0.939866f, 0.818784f, -0.0077f, -0.6798f, 0.7334f,
		52.9604f, -8.71031f, -106.097f, -0.173637f, 0.812839f, -0.0056f, -0.6794f, 0.7338f,
		52.4971f, -4.2923f, -102.011f, -0.166728f, 0.87378f, -0.0056f, -0.6794f, 0.7338f,
		51.9909f, -8.71031f, -69.8259f, -0.15918f, 1.3537f, 0.9964f, 0.0809f, 0.0268f,
		52.4971f, -4.2923f, -102.011f, -0.166728f, 0.87378f, 0.9963f, 0.0811f, 0.0268f,
		52.9604f, -8.71031f, -106.097f, -0.173637f, 0.812839f, 0.9964f, 0.0807f, 0.0268f,
		51.9909f, -8.71031f, -69.8259f, -0.15918f, 1.3537f, 0.9962f, 0.0825f, 0.027f,
		51.5648f, -4.2923f, -67.5982f, -0.152827f, 1.38692f, 0.9962f, 0.0825f, 0.027f,
		52.4971f, -4.2923f, -102.011f, -0.166728f, 0.87378f, 0.9962f, 0.0825f, 0.027f,
		79.784f, -8.71031f, -51.8894f, -0.573618f, 1.62116f, 0.9511f, 0.3037f, 0.0569f,
		79.2213f, -4.29229f, -66.0856f, -0.565227f, 1.40948f, 0.9511f, 0.3037f, 0.0569f,
		80.7536f, -8.71031f, -68.2315f, -0.588075f, 1.37748f, 0.9511f, 0.3037f, 0.0569f,
		79.784f, -8.71031f, -51.8894f, -0.573618f, 1.62116f, 0.9507f, 0.3048f, 0.0572f,
		78.289f, -4.29229f, -50.581f, -0.551326f, 1.64068f, 0.9507f, 0.3048f, 0.0572f,
		79.2213f, -4.29229f, -66.0856f, -0.565227f, 1.40948f, 0.9507f, 0.3048f, 0.0572f,
		51.5648f, -4.2923f, -67.5982f, -0.152827f, 1.38692f, 0.05f, 0.4535f, -0.8899f,
		51.9909f, -8.71031f, -69.8259f, -0.15918f, 1.3537f, 0.0487f, 0.4534f, -0.89f,
		79.2213f, -4.29229f, -66.0856f, -0.565227f, 1.40948f, 0.0487f, 0.4534f, -0.89f,
		79.784f, -8.71031f, -51.8894f, -0.573618f, 1.62116f, -0.001f, 0.284f, -0.9588f,
		84.1932f, -4.29229f, -50.581f, -0.639366f, 1.64068f, 0.0f, 0.2839f, -0.9588f,
		78.289f, -4.29229f, -50.581f, -0.551326f, 1.64068f, 0.0011f, 0.2839f, -0.9589f,
		79.784f, -8.71031f, -51.8894f, -0.573618f, 1.62116f, -0.0012f, 0.284f, -0.9588f,
		85.9244f, -8.71031f, -51.8894f, -0.66518f, 1.62116f, 0.0f, 0.2839f, -0.9588f,
		85.9244f, -8.71031f, -51.8894f, -0.66518f, 1.62116f, 0.9305f, 0.3662f, -0.0055f,
		84.504f, -4.29229f, 2.36128f, -0.644f, 2.43012f, 0.9305f, 0.3662f, -0.0055f,
		84.1932f, -4.29229f, -50.581f, -0.639366f, 1.64068f, 0.9305f, 0.3662f, -0.0055f,
		85.9244f, -8.71031f, -51.8894f, -0.66518f, 1.62116f, 0.9308f, 0.3655f, -0.0054f,
		86.2476f, -8.71031f, 3.91312f, -0.669999f, 2.45327f, 0.9308f, 0.3655f, -0.0054f,
		84.504f, -4.29229f, 2.36128f, -0.644f, 2.43012f, 0.9308f, 0.3655f, -0.0054f,
		76.5523f, -8.71031f, 2.31877f, -0.525428f, 2.42949f, -0.1553f, 0.2703f, 0.9502f,
		84.504f, -4.29229f, 2.36128f, -0.644f, 2.43012f, -0.1553f, 0.2703f, 0.9502f,
		86.2476f, -8.71031f, 3.91312f, -0.669999f, 2.45327f, -0.1554f, 0.2703f, 0.9501f,
		76.5523f, -8.71031f, 2.31877f, -0.525428f, 2.42949f, -0.1551f, 0.2702f, 0.9502f,
		75.1816f, -4.29229f, 0.84865f, -0.504989f, 2.40757f, -0.155f, 0.2702f, 0.9502f,
		84.504f, -4.29229f, 2.36128f, -0.644f, 2.43012f, -0.1552f, 0.2702f, 0.9502f,
		77.1986f, -8.71031f, -33.5543f, -0.535066f, 1.89457f, -0.9538f, -0.2998f, -0.0174f,
		75.8031f, -4.29229f, -33.1857f, -0.514256f, 1.90006f, -0.9538f, -0.2998f, -0.0174f,
		75.1816f, -4.29229f, 0.84865f, -0.504989f, 2.40757f, -0.9538f, -0.2998f, -0.0174f,
		77.1986f, -8.71031f, -33.5543f, -0.535066f, 1.89457f, -0.9536f, -0.3006f, -0.0172f,
		75.1816f, -4.29229f, 0.84865f, -0.504989f, 2.40757f, -0.9532f, -0.3017f, -0.0181f,
		76.5523f, -8.71031f, 2.31877f, -0.525428f, 2.42949f, -0.9532f, -0.3018f, -0.0182f,
		70.4119f, -8.71031f, -34.3514f, -0.433865f, 1.88268f, -0.1155f, -0.1211f, 0.9859f,
		69.2774f, -4.29229f, -33.942f, -0.416948f, 1.88879f, -0.1133f, -0.1211f, 0.9862f,
		75.8031f, -4.29229f, -33.1857f, -0.514256f, 1.90006f, -0.1144f, -0.1197f, 0.9862f,
		70.4119f, -8.71031f, -34.3514f, -0.433865f, 1.88268f, -0.1158f, -0.1189f, 0.9861f,
		75.8031f, -4.29229f, -33.1857f, -0.514256f, 1.90006f, -0.1158f, -0.1189f, 0.9861f,
		77.1986f, -8.71031f, -33.5543f, -0.535066f, 1.89457f, -0.1158f, -0.1189f, 0.9861f,
		70.4119f, -8.71031f, -34.3514f, -0.433865f, 1.88268f, -0.9664f, -0.2395f, -0.0934f,
		69.8989f, -4.29229f, -40.3707f, -0.426215f, 1.79293f, -0.9666f, -0.2388f, -0.0929f,
		69.2774f, -4.29229f, -33.942f, -0.416948f, 1.88879f, -0.9664f, -0.2395f, -0.0934f,
		60.3934f, -8.71031f, -42.3232f, -0.284475f, 1.76381f, -0.1086f, -0.1969f, 0.9744f,
		69.8989f, -4.29229f, -40.3707f, -0.426215f, 1.79293f, -0.1098f, -0.197f, 0.9742f,
		71.0583f, -8.71031f, -41.1274f, -0.443504f, 1.78164f, -0.1079f, -0.1973f, 0.9744f,
		60.3934f, -8.71031f, -42.3232f, -0.284475f, 1.76381f, -0.1081f, -0.1978f, 0.9743f,
		59.6442f, -4.29229f, -41.5052f, -0.273303f, 1.77601f, -0.1078f, -0.1986f, 0.9741f,
		69.8989f, -4.29229f, -40.3707f, -0.426215f, 1.79293f, -0.1089f, -0.1978f, 0.9742f,
		60.3934f, -8.71031f, -42.3232f, -0.284475f, 1.76381f, -0.9854f, -0.1539f, -0.073f,
		61.363f, -8.71031f, -55.4767f, -0.298932f, 1.56767f, -0.986f, -0.1509f, -0.0712f,
		60.5765f, -4.29229f, -53.9845f, -0.287204f, 1.58992f, -0.9858f, -0.1505f, -0.0741f,
		60.3934f, -8.71031f, -42.3232f, -0.284475f, 1.76381f, -0.9854f, -0.1535f, -0.0736f,
		60.5765f, -4.29229f, -53.9845f, -0.287204f, 1.58992f, -0.9856f, -0.1521f, -0.0737f,
		59.6442f, -4.29229f, -41.5052f, -0.273303f, 1.77601f, -0.9853f, -0.1538f, -0.075f,
		26.7831f, -8.71031f, -55.4767f, 0.216707f, 1.56767f, 0.0f, -0.32f, 0.9474f,
		27.3266f, -4.2923f, -53.9844f, 0.208602f, 1.58993f, 0.0f, -0.32f, 0.9474f,
		60.5765f, -4.29229f, -53.9845f, -0.287204f, 1.58992f, 0.0f, -0.32f, 0.9474f,
		61.363f, -8.71031f, -55.4767f, -0.298932f, 1.56767f, -0.0005f, -0.3187f, 0.9479f,
		26.4599f, -8.71031f, -12.429f, 0.221526f, 2.20958f, 0.9923f, -0.1236f, 0.0075f,
		27.0158f, -4.2923f, -13.1432f, 0.213236f, 2.19893f, 0.9923f, -0.1236f, 0.0075f,
		27.3266f, -4.2923f, -53.9844f, 0.208602f, 1.58993f, 0.9923f, -0.1236f, 0.0075f,
		26.4599f, -8.71031f, -12.429f, 0.221526f, 2.20958f, 0.9922f, -0.1241f, 0.0074f,
		27.3266f, -4.2923f, -53.9844f, 0.208602f, 1.58993f, 0.9922f, -0.1247f, 0.0079f,
		26.7831f, -8.71031f, -55.4767f, 0.216707f, 1.56767f, 0.9922f, -0.1245f, 0.007f,
		26.4599f, -8.71031f, -12.429f, 0.221526f, 2.20958f, 0.0277f, 0.1572f, 0.9872f,
		12.2401f, -8.71031f, -12.0304f, 0.433564f, 2.21552f, 0.0277f, 0.1572f, 0.9872f,
		13.343f, -4.2923f, -12.7651f, 0.417119f, 2.20457f, 0.0274f, 0.1574f, 0.9872f,
		12.2401f, -8.71031f, -25.1839f, 0.433564f, 2.01938f, -0.9702f, 0.2422f, 0.0f,
		13.343f, -4.2923f, -12.7651f, 0.417119f, 2.20457f, -0.9702f, 0.2422f, 0.0f,
		12.2401f, -8.71031f, -12.0304f, 0.433564f, 2.21552f, -0.9702f, 0.2422f, 0.0f,
		13.343f, -4.2923f, -25.2443f, 0.417119f, 2.01848f, -0.9702f, 0.2422f, 0.0f,
		17.7341f, -8.71031f, -25.1839f, 0.35164f, 2.01938f, 0.0f, -0.0137f, -0.9999f,
		18.6257f, -4.2923f, -25.2443f, 0.338346f, 2.01848f, 0.0f, -0.0137f, -0.9999f,
		13.343f, -4.2923f, -25.2443f, 0.417119f, 2.01848f, 0.0f, -0.0137f, -0.9999f,
		12.2401f, -8.71031f, -25.1839f, 0.433564f, 2.01938f, 0.0f, -0.0137f, -0.9999f,
		17.7341f, -8.71031f, -25.1839f, 0.35164f, 2.01938f, -0.9798f, 0.1987f, -0.0205f,
		18.7037f, -8.71031f, -71.4202f, 0.337183f, 1.32993f, -0.9797f, 0.1996f, -0.0191f,
		19.5579f, -4.2923f, -69.1108f, 0.324445f, 1.36437f, -0.9794f, 0.2006f, -0.0215f,
		17.7341f, -8.71031f, -25.1839f, 0.35164f, 2.01938f, -0.9801f, 0.1975f, -0.0193f,
		19.5579f, -4.2923f, -69.1108f, 0.324445f, 1.36437f, -0.9798f, 0.1989f, -0.0208f,
		18.6257f, -4.2923f, -25.2443f, 0.338346f, 2.01848f, -0.9801f, 0.1973f, -0.0223f,
		26.4599f, -8.71031f, -12.429f, 0.221526f, 2.20958f, 0.0276f, 0.1561f, 0.9874f,
		13.343f, -4.2923f, -12.7651f, 0.417119f, 2.20457f, 0.0273f, 0.1565f, 0.9873f,
		27.0158f, -4.2923f, -13.1432f, 0.213236f, 2.19893f, 0.027f, 0.1561f, 0.9874f,
		18.7037f, -8.71031f, -71.4202f, 0.337183f, 1.32993f, -0.1831f, -0.4281f, 0.885f,
		12.8865f, -8.71031f, -72.616f, 0.423926f, 1.3121f, -0.1827f, -0.4306f, 0.8839f,
		13.9645f, -4.2923f, -70.2453f, 0.407852f, 1.34745f, -0.1804f, -0.4306f, 0.8843f,
		18.7037f, -8.71031f, -71.4202f, 0.337183f, 1.32993f, -0.1796f, -0.4282f, 0.8856f,
		13.9645f, -4.2923f, -70.2453f, 0.407852f, 1.34745f, -0.1796f, -0.4282f, 0.8856f,
		19.5579f, -4.2923f, -69.1108f, 0.324445f, 1.36437f, -0.1796f, -0.4282f, 0.8856f,
		12.8865f, -8.71031f, -72.616f, 0.423926f, 1.3121f, -0.9718f, 0.2357f, 0.0004f,
		13.9645f, -4.2923f, -72.8924f, 0.407852f, 1.30798f, -0.9715f, 0.237f, 0.0f,
		13.9645f, -4.2923f, -70.2453f, 0.407852f, 1.34745f, -0.9719f, 0.2356f, 0.0f,
		12.8865f, -8.71031f, -75.4061f, 0.423926f, 1.27049f, -0.9715f, 0.237f, 0.0f,
		12.8865f, -8.71031f, -75.4061f, 0.423926f, 1.27049f, -0.0322f, -0.4883f, 0.8721f,
		-6.54481f, -4.2923f, -73.6487f, 0.713676f, 1.2967f, -0.0322f, -0.4883f, 0.8721f,
		13.9645f, -4.2923f, -72.8924f, 0.407852f, 1.30798f, -0.0322f, -0.4883f, 0.8721f,
		12.8865f, -8.71031f, -75.4061f, 0.423926f, 1.27049f, -0.0326f, -0.4898f, 0.8712f,
		-8.44315f, -8.71031f, -76.2033f, 0.741984f, 1.25861f, -0.0326f, -0.4898f, 0.8712f,
		-6.54481f, -4.2923f, -73.6487f, 0.713676f, 1.2967f, -0.0326f, -0.4898f, 0.8712f,
		-8.44315f, -8.71031f, -73.0146f, 0.741984f, 1.30616f, 0.9188f, -0.3948f, 0.0f,
		-6.54481f, -4.2923f, -73.6487f, 0.713676f, 1.2967f, 0.9188f, -0.3947f, -0.0005f,
		-8.44315f, -8.71031f, -76.2033f, 0.741984f, 1.25861f, 0.9188f, -0.3948f, 0.0f,
		-6.54481f, -4.2923f, -70.6235f, 0.713676f, 1.34181f, 0.9188f, -0.3948f, 0.0f,
		-6.54481f, -4.2923f, -73.6487f, 0.713676f, 1.2967f, 0.9188f, -0.3946f, -0.0007f,
		-8.44315f, -8.71031f, -73.0146f, 0.741984f, 1.30616f, 0.1401f, -0.5172f, 0.8443f,
		-13.3812f, -4.2923f, -69.489f, 0.815618f, 1.35873f, 0.1401f, -0.5172f, 0.8443f,
		-6.54481f, -4.2923f, -70.6235f, 0.713676f, 1.34181f, 0.1401f, -0.5172f, 0.8443f,
		-8.44315f, -8.71031f, -73.0146f, 0.741984f, 1.30616f, 0.1412f, -0.5162f, 0.8447f,
		-15.553f, -8.71031f, -71.8188f, 0.848003f, 1.32399f, 0.1421f, -0.5155f, 0.845f,
		-13.3812f, -4.2923f, -69.489f, 0.815618f, 1.35873f, 0.1413f, -0.5163f, 0.8447f,
		-15.553f, -8.71032f, -29.1698f, 0.848003f, 1.95995f, 0.8974f, -0.4412f, 0.0f,
		-13.3812f, -4.2923f, -69.489f, 0.815618f, 1.35873f, 0.8973f, -0.4413f, 0.0009f,
		-15.553f, -8.71031f, -71.8188f, 0.848003f, 1.32399f, 0.8974f, -0.4412f, 0.0f,
		-13.3812f, -4.2923f, -29.0259f, 0.815618f, 1.96209f, 0.8974f, -0.4412f, 0.0f,
		-13.3812f, -4.2923f, -69.489f, 0.815618f, 1.35873f, 0.8973f, -0.4413f, 0.001f,
		-9.41268f, -8.71032f, -29.1698f, 0.756441f, 1.95995f, 0.0f, 0.0325f, -0.9995f,
		-13.3812f, -4.2923f, -29.0259f, 0.815618f, 1.96209f, 0.0f, 0.0325f, -0.9995f,
		-15.553f, -8.71032f, -29.1698f, 0.848003f, 1.95995f, 0.0f, 0.0325f, -0.9995f,
		-7.47705f, -4.2923f, -29.0259f, 0.727577f, 1.96209f, 0.0f, 0.0325f, -0.9995f,
		-9.41268f, -8.71032f, -23.9881f, 0.756441f, 2.03722f, 0.9159f, -0.4013f, 0.0f,
		-7.47705f, -4.2923f, -29.0259f, 0.727577f, 1.96209f, 0.9159f, -0.4013f, 0.0f,
		-9.41268f, -8.71032f, -29.1698f, 0.756441f, 1.95995f, 0.9159f, -0.4013f, 0.0f,
		-7.47705f, -4.2923f, -24.1099f, 0.727577f, 2.0354f, 0.9159f, -0.4013f, 0.0f,
		-5.53456f, -8.71032f, -23.9881f, 0.698612f, 2.03722f, 0.0f, -0.0275f, -0.9996f,
		-7.47705f, -4.2923f, -24.1099f, 0.727577f, 2.0354f, 0.0f, -0.0275f, -0.9996f,
		-9.41268f, -8.71032f, -23.9881f, 0.756441f, 2.03722f, 0.0f, -0.0275f, -0.9996f,
		-3.74809f, -4.2923f, -24.1099f, 0.671973f, 2.0354f, 0.0f, -0.0275f, -0.9996f,
		-5.53456f, -8.71032f, -23.9881f, 0.698612f, 2.03722f, 0.9264f, -0.3754f, -0.0304f,
		-3.43734f, -4.2923f, -14.6559f, 0.667339f, 2.17637f, 0.9264f, -0.3754f, -0.0304f,
		-3.74809f, -4.2923f, -24.1099f, 0.671973f, 2.0354f, 0.9264f, -0.3754f, -0.0304f,
		-5.53456f, -8.71032f, -23.9881f, 0.698612f, 2.03722f, 0.9261f, -0.3762f, -0.03f,
		-5.21138f, -8.71032f, -14.0234f, 0.693793f, 2.1858f, 0.9261f, -0.3762f, -0.03f,
		-3.43734f, -4.2923f, -14.6559f, 0.667339f, 2.17637f, 0.9261f, -0.3762f, -0.03f,
		-5.21138f, -8.71032f, -14.0234f, 0.693793f, 2.1858f, 0.0785f, 0.1124f, 0.9906f,
		-25.2483f, -8.71032f, -12.429f, 0.992574f, 2.20958f, 0.0806f, 0.1141f, 0.9902f,
		-22.7036f, -4.2923f, -13.1432f, 0.954629f, 2.19893f, 0.0773f, 0.1157f, 0.9903f,
		-5.21138f, -8.71032f, -14.0234f, 0.693793f, 2.1858f, 0.0792f, 0.1101f, 0.9908f,
		-22.7036f, -4.2923f, -13.1432f, 0.954629f, 2.19893f, 0.0778f, 0.1124f, 0.9906f,
		-3.43734f, -4.2923f, -14.6559f, 0.667339f, 2.17637f, 0.0764f, 0.1108f, 0.9909f,
		-25.2483f, -8.71032f, -12.429f, 0.992574f, 2.20958f, -0.8669f, 0.4981f, -0.0188f,
		-24.2788f, -8.71032f, -61.8541f, 0.978117f, 1.47258f, -0.8667f, 0.4986f, -0.0159f,
		-21.7714f, -4.2923f, -60.035f, 0.940728f, 1.4997f, -0.8664f, 0.4991f, -0.0181f,
		-25.2483f, -8.71032f, -12.429f, 0.992574f, 2.20958f, -0.8667f, 0.4986f, -0.016f,
		-21.7714f, -4.2923f, -60.035f, 0.940728f, 1.4997f, -0.8674f, 0.4972f, -0.0172f,
		-22.7036f, -4.2923f, -13.1432f, 0.954629f, 2.19893f, -0.8677f, 0.4967f, -0.0183f,
		-24.2788f, -8.71032f, -61.8541f, 0.978117f, 1.47258f, 0.0002f, -0.3799f, 0.925f,
		-30.7831f, -4.2923f, -60.035f, 1.07511f, 1.4997f, 0.0f, -0.3807f, 0.9247f,
		-21.7714f, -4.2923f, -60.035f, 0.940728f, 1.4997f, 0.0006f, -0.3799f, 0.925f,
		-24.2788f, -8.71032f, -61.8541f, 0.978117f, 1.47258f, 0.0002f, -0.3797f, 0.9251f,
		-33.6509f, -8.71032f, -61.8541f, 1.11787f, 1.47258f, 0.0f, -0.3807f, 0.9247f,
		-33.3277f, -8.71032f, -55.0781f, 1.11305f, 1.57362f, 0.8456f, -0.5323f, -0.0403f,
		-30.7831f, -4.2923f, -60.035f, 1.07511f, 1.4997f, 0.8456f, -0.5323f, -0.0403f,
		-33.6509f, -8.71032f, -61.8541f, 1.11787f, 1.47258f, 0.8456f, -0.5323f, -0.0403f,
		-33.3277f, -8.71032f, -55.0781f, 1.11305f, 1.57362f, 0.8453f, -0.5327f, -0.0409f,
		-30.4723f, -4.2923f, -53.6063f, 1.07047f, 1.59556f, 0.8453f, -0.5327f, -0.0409f,
		-30.7831f, -4.2923f, -60.035f, 1.07511f, 1.4997f, 0.8453f, -0.5327f, -0.0409f,
		-30.4723f, -4.2923f, -53.6063f, 1.07047f, 1.59556f, -0.3407f, 0.5115f, -0.7889f,
		-33.3277f, -8.71032f, -55.0781f, 1.11305f, 1.57362f, -0.3464f, 0.5034f, -0.7916f,
		-29.4496f, -8.71032f, -55.0781f, 1.05522f, 1.57362f, -0.3412f, 0.5108f, -0.7891f,
		-29.4496f, -8.71032f, -55.0781f, 1.05522f, 1.57362f, 0.7641f, -0.2383f, 0.5995f,
		-26.7433f, -4.2923f, -53.6063f, 1.01487f, 1.59556f, 0.3614f, -0.7773f, 0.515f,
		-24.2788f, -8.71032f, -61.8541f, 0.978117f, 1.47258f, 0.7485f, -0.6437f, 0.1595f,
		-26.7433f, -4.2923f, -53.6063f, 1.01487f, 1.59556f, -0.3544f, 0.4913f, -0.7956f,
		-30.4723f, -4.2923f, -53.6063f, 1.07047f, 1.59556f, -0.3417f, 0.51f, -0.7894f,
		-29.4496f, -8.71032f, -55.0781f, 1.05522f, 1.57362f, -0.3526f, 0.4941f, -0.7947f,
		-30.4191f, -8.71032f, -12.8276f, 1.06968f, 2.20364f, 0.0352f, 0.1704f, 0.9848f,
		-65.6454f, -8.71032f, -13.6248f, 1.59496f, 2.19175f, -0.023f, 0.1669f, 0.9857f,
		-61.547f, -4.2923f, -14.2777f, 1.53384f, 2.18201f, -0.0217f, 0.1658f, 0.9859f,
		-29.4496f, -8.71032f, -55.0781f, 1.05522f, 1.57362f, 0.8399f, -0.5375f, 0.0751f,
		-30.4191f, -8.71032f, -12.8276f, 1.06968f, 2.20364f, 0.841f, -0.5357f, 0.0756f,
		-26.7433f, -4.2923f, -53.6063f, 1.01487f, 1.59556f, 0.8401f, -0.5373f, 0.0752f,
		-30.4191f, -8.71032f, -12.8276f, 1.06968f, 2.20364f, 0.8409f, -0.5358f, 0.0756f,
		-27.6756f, -4.2923f, -13.5214f, 1.02877f, 2.19329f, 0.8405f, -0.5365f, 0.0753f,
		-26.7433f, -4.2923f, -53.6063f, 1.01487f, 1.59556f, 0.8407f, -0.5362f, 0.0754f,
		-30.4191f, -8.71032f, -12.8276f, 1.06968f, 2.20364f, -0.0501f, 0.1185f, 0.9917f,
		-61.547f, -4.2923f, -14.2777f, 1.53384f, 2.18201f, -0.022f, 0.1675f, 0.9856f,
		-27.6756f, -4.2923f, -13.5214f, 1.02877f, 2.19329f, -0.0212f, 0.168f, 0.9856f,
		-65.6454f, -8.71032f, -13.6248f, 1.59496f, 2.19175f, -0.7323f, 0.6805f, 0.0232f,
		-66.8547f, -8.71032f, -52.8837f, 1.61299f, 1.60634f, -0.7326f, 0.6804f, 0.0216f,
		-62.7098f, -4.2923f, -51.5244f, 1.55118f, 1.62661f, -0.7327f, 0.6802f, 0.0235f,
		-65.6454f, -8.71032f, -13.6248f, 1.59496f, 2.19175f, -0.7318f, 0.6811f, 0.0213f,
		-62.7098f, -4.2923f, -51.5244f, 1.55118f, 1.62661f, -0.7319f, 0.681f, 0.0229f,
		-61.547f, -4.2923f, -14.2777f, 1.53384f, 2.18201f, -0.7311f, 0.6819f, 0.0238f,
		51.9909f, -8.71031f, -69.8259f, -0.15918f, 1.3537f, 0.0494f, 0.4502f, -0.8916f,
		80.7536f, -8.71031f, -68.2315f, -0.588075f, 1.37748f, 0.0481f, 0.4502f, -0.8916f,
		79.2213f, -4.29229f, -66.0856f, -0.565227f, 1.40948f, 0.0494f, 0.4502f, -0.8916f,
		69.8989f, -4.29229f, -40.3707f, -0.426215f, 1.79293f, -0.967f, -0.238f, -0.0914f,
		70.4119f, -8.71031f, -34.3514f, -0.433865f, 1.88268f, -0.9669f, -0.2379f, -0.0922f,
		71.0583f, -8.71031f, -41.1274f, -0.443504f, 1.78164f, -0.9667f, -0.2386f, -0.0928f,
		71.0583f, -8.71031f, -41.1274f, -0.443504f, 1.78164f, 0.0f, 1.0f, 0.0f
	};

	unsigned int humoIndices[] = {
   0, 1, 2,
   0, 2, 3,
	};

	GLfloat humoVertices[] = {
	   -0.5f, 0.0f, 0.5f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // UV: (0,0)
		0.5f, 0.0f, 0.5f,   0.333f, 0.0f,  0.0f, -1.0f, 0.0f,  // UV: (1/8,0) 
		0.5f, 0.0f, -0.5f,  0.333f, 0.333f, 0.0f, -1.0f, 0.0f, // UV: (1/8,1/8)
	   -0.5f, 0.0f, -0.5f,  0.0f, 0.333f,  0.0f, -1.0f, 0.0f,  // UV: (0,1/8)
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(Suelo3DVertices, Suelo3DIndices, 3200, 594);
	meshList.push_back(obj3);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(humoVertices, humoIndices, 32, 6);
	meshList.push_back(obj4); // solo un número
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCilindro(10, 1.0f);//índice 2 en MeshList
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
	cargarRecursos();

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

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.45f, 0.45f,
		0.0f, 0.0f, -1.0f);

	pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,  // Naranja
		0.5f, 0.9f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.25f, 0.15);
	pointLightCount++;
	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	float anguloVuelo = 0.0f;
	float anguloAgua = 0.0f;
	float anguloCamino = 0.0f;
	float direccionActual = 0.0f;
	float diffDirecciones = 0.0f;

	float anguloRotacion2 = 0.0f;

	float cambiarDireccionTrajineraUno = true;
	float cambiarDireccionTrajineraDos = false;
	float desplazamientoTrajineraUno = 0.0f;
	float desplazamientoTrajineraDos = 5.0f;

	float anguloSolRad = 0.0f;
	float dirSolX = 0.0f;
	float dirSolY = 0.0f;
	float dirSolZ = 0.0f;
	float movimientoX = mainWindow.getarticulacion1();
	float movimientoZ = mainWindow.getarticulacion2();
	bool enMovimiento = mainWindow.getmovimientoHawlucha();

	loadKeyframesFromFile();
	glm::vec3 posGlobo = glm::vec3(2.0f, 0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		movimientoX = mainWindow.getarticulacion1();
		movimientoZ = mainWindow.getarticulacion2();
		enMovimiento = mainWindow.getmovimientoHawlucha();

		//Recibir eventos del usuario
		glfwPollEvents();
		animate();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		inputKeyframes(mainWindow.getsKeys());
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
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		angulosol += 0.5f * deltaTime;
		if (angulosol > 360.0f)
			angulosol = 0;
		anguloAgua += 0.5f * deltaTime;
		if (anguloAgua > 360.0f)
			anguloAgua = 0;
		anguloVuelo += 2.0f * deltaTime;
		if (anguloVuelo > 360.0f)
			anguloVuelo = 0;

		// Ciclo día-noche: Actualizar dirección del sol basado en el ángulo
		anguloSolRad = angulosol * toRadians;
		dirSolX = 0.0f;
		dirSolY = -cos(anguloSolRad);  // Se mueve de arriba hacia abajo en arco
		dirSolZ = sin(anguloSolRad);   // Se mueve adelante/atrás
		mainLight.SetDirection(dirSolX, dirSolY, dirSolZ);


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelaux2(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		shaderList[0].SetDirectionalLight(&mainLight);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -5.5f, 0.0f));
		model = glm::scale(model, glm::vec3(400.0f, 1.0f, 400.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		aguaTexture.UseTexture();
		meshList[1]->RenderMesh();
		glDisable(GL_BLEND);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.0f, -4.8f, 8.75f));
		model = glm::scale(model, glm::vec3(2.42f, 1.0f, 2.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		caminoTexture.UseTexture();
		Camino_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 26.0f + 4 * sin(2* anguloVuelo * toRadians), 00.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pidgeyTexture.UseTexture();
		Pidgey_M.RenderModel();

		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 25.0f + 4 * sin(2 * anguloVuelo * toRadians), 00.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo-35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pidgeyTexture.UseTexture();
		Pidgey_M.RenderModel();

		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 28.0f + 4 * sin(2 * anguloVuelo * toRadians), 00.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pidgeyTexture.UseTexture();
		Pidgey_M.RenderModel();

		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 24.0f + 4 * sin(2* anguloVuelo * toRadians), 00.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo+28.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pidgeyTexture.UseTexture();
		Pidgey_M.RenderModel();

		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 27.0f + 4*sin(2 * anguloVuelo * toRadians), 00.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo+37.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pidgeyTexture.UseTexture();
		Pidgey_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-10.0f, 0.9f, 40.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		centroPokemonTexture.UseTexture();
		CentroPokemon_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pokeshopTexture.UseTexture();
		Pokeshop_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 5.0f, -30.0f));
		model = glm::scale(model, glm::vec3(5.0f, 10.0f, 10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		calendarioMayaTexture.UseTexture();
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.2f, 70.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pokeFanTexture.UseTexture();
		Pokefan_M.RenderModel();

		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(-45.0f, 32.0f + 4*sin(anguloVuelo * toRadians), -35.0f));
		model = glm::rotate(model, glm::radians(anguloVuelo+37.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f*sin(anguloVuelo * toRadians)), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		rayquazaTexture.UseTexture();
		Rayquaza_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 9.0f, -35.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		piramideTexture.UseTexture();
		Piramide_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(6.0f, -5.0f, -68.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puertoTexture.UseTexture();
		Puerto_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-4.0f, -4.95f + 0.25 * sin(2 * anguloAgua * toRadians), -80.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		trajineraTexture.UseTexture();
		Trajinera_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(6.0f, -4.95f + 0.25 * sin(2 * anguloAgua * toRadians), -80.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		trajineraTexture.UseTexture();
		Trajinera_M.RenderModel();

		if (cambiarDireccionTrajineraUno) {
			if (desplazamientoTrajineraUno > 10.0f) {
				desplazamientoTrajineraUno = 10.0f;
				cambiarDireccionTrajineraUno = !cambiarDireccionTrajineraUno;
			}
			else {
				desplazamientoTrajineraUno += 0.1f * deltaTime;
			}
		}
		else {
			if (desplazamientoTrajineraUno < 0.0f) {
				desplazamientoTrajineraUno = 0.0f;
				cambiarDireccionTrajineraUno = !cambiarDireccionTrajineraUno;
			}
			else {
				desplazamientoTrajineraUno -= 0.1f * deltaTime;
			}
		}

		if (cambiarDireccionTrajineraDos) {
			if (desplazamientoTrajineraDos > 25.0f) {
				desplazamientoTrajineraDos = 25.0f;
				cambiarDireccionTrajineraDos = !cambiarDireccionTrajineraDos;
			}
			else {
				desplazamientoTrajineraDos += 0.1f * deltaTime;
			}
		}
		else {
			if (desplazamientoTrajineraDos < 0.0f) {
				desplazamientoTrajineraDos = 0.0f;
				cambiarDireccionTrajineraDos = !cambiarDireccionTrajineraDos;
			}
			else {
				desplazamientoTrajineraDos -= 0.1f * deltaTime;
			}
		}

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-6.0f, -5.5f + 0.25 * sin(2 * anguloAgua * toRadians), -130.0f - desplazamientoTrajineraUno));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		trajineraTexture.UseTexture();
		Trajinera_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(8.0f, -5.5f + 0.25 * sin(2 * anguloAgua * toRadians), -130.0f + desplazamientoTrajineraUno));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		trajineraTexture.UseTexture();
		Trajinera_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-10.0f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -130.0f + desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*cambiarDireccionTrajineraDos), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		mudkipTexture.UseTexture();
		Mudkip_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-12.0f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -135.0f + 1.1*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*cambiarDireccionTrajineraDos), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		mudkipTexture.UseTexture();
		Mudkip_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-13.9f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -132.0f + 0.9*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*cambiarDireccionTrajineraDos), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		mudkipTexture.UseTexture();
		Mudkip_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-15.4f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -129.0f + 1.2*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*cambiarDireccionTrajineraDos), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		mudkipTexture.UseTexture();
		Mudkip_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(12.0f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -130.0f - desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*(1.0f-cambiarDireccionTrajineraDos)), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		squirtleTexture.UseTexture();
		Squirtle_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(14.0f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -135.0f - 1.1*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*(1.0f-cambiarDireccionTrajineraDos)), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		squirtleTexture.UseTexture();
		Squirtle_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(16.9f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -132.0f - 0.9*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*(1.0f-cambiarDireccionTrajineraDos)), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		squirtleTexture.UseTexture();
		Squirtle_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(18.4f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -129.0f - 1.2*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*(1.0f-cambiarDireccionTrajineraDos)), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		squirtleTexture.UseTexture();
		Squirtle_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(20.0f, -6.0f + 0.25 * sin(2 * anguloAgua * toRadians), -124.0f - 0.78*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f*(1.0f-cambiarDireccionTrajineraDos)), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		squirtleTexture.UseTexture();
		Squirtle_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(80.0f, -5.0f, -90.0f + 0.78*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		krabbyTexture.UseTexture();
		Krabby_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(72.0f, -5.0f, -82.0f - 0.76*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		krabbyTexture.UseTexture();
		Krabby_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(68.0f, -5.0f, -98.0f + 0.96*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		krabbyTexture.UseTexture();
		Krabby_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(63.0f, -5.0f, -86.0f - 0.82*desplazamientoTrajineraDos));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		krabbyTexture.UseTexture();
		Krabby_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		ichirakuTexture.UseTexture();
		Ichiraku_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, 18.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		toriiTexture.UseTexture();
		Torii_M.RenderModel();

		if (direccionActual > 360.0f || direccionActual < 0.0f) {
			direccionActual = 0.0f;
		}

		diffDirecciones = direccionActual - mainWindow.getrotacionHawlucha();

		if (abs(diffDirecciones) > 10.00f) {
			if (diffDirecciones < 0.0f) {
				direccionActual += 10.0f * deltaTime;
			}
			else {
				direccionActual -= 10.0f * deltaTime;
			}
		}
		else {
			direccionActual = mainWindow.getrotacionHawlucha();
		}


		if (mainWindow.getmovimientoHawlucha()) {
			if (anguloCamino > 360.0f || anguloCamino < 0.0f) {
				anguloCamino = 0.0f;
			}
			else {
				anguloCamino += 5.0f * deltaTime;
			}
		}
		else {
			if (anguloCamino >= 360.0f || anguloCamino <= 0.0f) {
				anguloCamino = 0.0f;
			}
			else if (anguloCamino > 180.0f) {
				anguloCamino += 5.0f * deltaTime;
			}
			else if (anguloCamino > 0.0f) {
				anguloCamino -= 5.0f * deltaTime;
			}
		}

		model = glm::mat4(1.0);
		if(animacionEnCurso) {
			if(agarrarSilla){
				anguloRotacion = 0.0f;
				if(faseAnimacion == 0) {
					if(anguloBrazos < 90.0f){
						anguloBrazos += 4.0f * deltaTime;
					} else {
						anguloBrazos = 90.0f;
						faseAnimacion = 1;
					}
				} else if(faseAnimacion == 1) {
					if(anguloBrazos > -45.0f){
						anguloBrazos -= 15.0f * deltaTime;
						if(anguloBrazos < 45.0f && anguloMachamp > -80.0f) 
							anguloMachamp -= 10.0f * deltaTime;
					} else {
						anguloBrazos = -45.0f;
						faseAnimacion = 2;
					}
				} else if(faseAnimacion == 2) {
					if(anguloBrazos < 0.0f){
						anguloBrazos += 0.5f * deltaTime;
						if(anguloBrazos < 45.0f && anguloMachamp > -80.0f) 
							anguloMachamp -= 10.0f * deltaTime;
					} else {
						anguloBrazos = 0.0f;
						animacionEnCurso = false;
						faseAnimacion = 0;
						anguloMachamp = 0.0f;
					}
				}
				model = glm::translate(model, glm::vec3(62.5f, -2.3f, 60.0f));
			} else {
				if(faseAnimacion == 0) {
					if(anguloRotacion < 135.0f) {
						anguloRotacion += 2.5f * deltaTime;
					} else {
						anguloRotacion = 135.0f;
						faseAnimacion = 1;
					}
				} else if (faseAnimacion == 1) {

					if(movimientoMachamp < 2.5f) {
						movimientoMachamp += 0.10f * deltaTime;
					}

					if(posicionAnimacion[0] < 4.0f){
						posicionAnimacion[0] += 0.20f * deltaTime;
					}
					if(posicionAnimacion[2] < 4.0f){
						posicionAnimacion[2] += 0.20f * deltaTime;
					}
					if(posicionAnimacion[0] >= 4.0f && posicionAnimacion[2] >= 4.0f) {
						posicionAnimacion[0] = 4.0f;
						faseAnimacion = 2;
					}
				}	else if(faseAnimacion == 2) {
					if(posicionAnimacion[0] < 4.6f){
						posicionAnimacion[0] += 0.20f * deltaTime;
					}
					if(posicionAnimacion[1] < 3.0f){
						posicionAnimacion[1] += 0.15f * deltaTime;
					}
					if(posicionAnimacion[2] < 5.6f){
						posicionAnimacion[2] += 0.20f * deltaTime;
					}

					if(movimientoMachamp < 2.5f) {
						movimientoMachamp += 0.10f * deltaTime;
					}

					if(posicionAnimacion[0] >= 4.6f && posicionAnimacion[1] >= 3.0f && posicionAnimacion[2] >= 5.6f) {
						posicionAnimacion[0] = 4.6f;
						posicionAnimacion[1] = 3.0f;
						posicionAnimacion[2] = 5.6f;
						movimientoMachamp = 2.5f;
						faseAnimacion = 3;
					}
				} else if (faseAnimacion == 3) {
					if(anguloRotacion > -45.0f) {
						anguloRotacion -= 2.5f * deltaTime;
					} 
					if(anguloMachamp2 < 45.0f) {
						anguloMachamp2 += 1.0f * deltaTime;
					}
					
					if(anguloRotacion <= -45.0f && anguloMachamp2 >= 45.0f) {
						anguloRotacion = -45.0f;
						faseAnimacion = 4;
					}
				} else if (faseAnimacion == 4) {
					if(posicionAnimacion[0] > 2.3f){
						posicionAnimacion[0] -= 0.20f * deltaTime;
					}
					if(posicionAnimacion[1] < 5.0f){
						posicionAnimacion[1] += 0.15f * deltaTime;
					}
					if(posicionAnimacion[2] > 2.8f){
						posicionAnimacion[2] -= 0.20f * deltaTime;
					}

					if(anguloBrazos2 < 60.0f) {
						anguloBrazos2 += 2.5f * deltaTime;
					}
					if(anguloPiernas2 < 30.0f) {
						anguloPiernas2 += 2.5f * deltaTime;
					}
					if(posicionAnimacion[0] <= 2.3f && posicionAnimacion[1] >= 5.0f && posicionAnimacion[2] <= 2.8f) {
						posicionAnimacion[0] = 2.3f;
						posicionAnimacion[1] = 5.0f;
						posicionAnimacion[2] = 2.8f;
						faseAnimacion = 5;
					}
				} else if (faseAnimacion == 5) {
					if(posicionAnimacion[0] > 0.0f){
						posicionAnimacion[0] -= 0.20f * deltaTime;
					}
					if(posicionAnimacion[1] > 0.0f){
						posicionAnimacion[1] -= 0.15f * deltaTime;
					}
					if(posicionAnimacion[2] > 0.0f){
						posicionAnimacion[2] -= 0.20f * deltaTime;
					}

					if(anguloRotacion2 < 90.0f) {
						anguloRotacion2 += 2.5f * deltaTime;
					}

					if(anguloBrazos2 < 60.0f) {
						anguloBrazos2 += 2.5f * deltaTime;
					}
					if(anguloPiernas2 < 30.0f) {
						anguloPiernas2 += 2.5f * deltaTime;
					}

					if(posicionAnimacion[1] < 2.0f && anguloMachamp > -80.0f) 
						anguloMachamp -= 10.0f * deltaTime;

					if(posicionAnimacion[0] <= 0.0f && posicionAnimacion[1] <= 0.0f && posicionAnimacion[2] <= 0.0f && anguloMachamp <= -80.0f) {
						posicionAnimacion[0] = 0.0f;
						posicionAnimacion[1] = 0.0f;
						posicionAnimacion[2] = 0.0f;
						anguloMachamp = -80.0f;
						faseAnimacion = 6;
					}
				} else if (faseAnimacion == 6) {
					if(anguloBrazos2 > 0.0f) {
						anguloBrazos2 -= 2.5f * deltaTime;
					}
					if(anguloPiernas2 > 0.0f) {
						anguloPiernas2 -= 2.5f * deltaTime;
					}
					if(anguloRotacion2 > 0.0f) {
						anguloRotacion2 -= 2.5f * deltaTime;
					} 
					
					
					if(anguloBrazos2 <= 0.0f && anguloPiernas2 <= 0.0f && anguloRotacion2 <= 0.0f) {
						anguloBrazos2 = 0.0f;
						anguloPiernas2 = 0.0f;
						anguloRotacion2 = 0.0f;
						faseAnimacion = 0;
						anguloMachamp = 0.0f;
						anguloMachamp2 = 0.0f;
						movimientoMachamp = 0.0f;
						animacionEnCurso = false;
					}
				}
				model = glm::translate(model, glm::vec3(62.5f + posicionAnimacion[0], -2.3f + posicionAnimacion[1], 60.0f + posicionAnimacion[2]));
			}
			
			model = glm::rotate(model, glm::radians(anguloRotacion), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(anguloRotacion2), glm::vec3(0.0f, 0.0f, 1.0f));
		} else {
			model = glm::translate(model, glm::vec3(90.0f + movimientoX, -4.7f, 20.0f + movimientoZ));
			if(
				
				movimientoX > -33.0f && movimientoX < -23.0f && 
				movimientoZ > 35.0f && movimientoZ < 45.0f) {
				model = glm::translate(model, glm::vec3(0.0f, 2.6f, 0.0f));
				puedeAnimar = true;
			} else {
				puedeAnimar = false;
			}
			model = glm::rotate(model, glm::radians(direccionActual), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		
		
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		hawluchaTexture.UseTexture();
		HawluchaCuerpo_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, 0.43f));
		model = glm::rotate(model, glm::radians(60.0f - anguloBrazos2), glm::vec3(1.0f, 0.0f, 0.0f));
		if(agarrarSilla){ 
			model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-45.0f - anguloBrazos), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux2 = model;
		} else model = glm::rotate(model, glm::radians(45.0f * sin(anguloCamino * toRadians) * mainWindow.getmovimientoHawlucha()), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		hawluchaTexture.UseTexture();
		HawluchaBrazoIzq_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, -0.43f));
		model = glm::rotate(model, glm::radians(-60.0f + anguloBrazos2), glm::vec3(1.0f, 0.0f, 0.0f));
		if(agarrarSilla){
			model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(45.0f + anguloBrazos), glm::vec3(0.0f, 1.0f, 0.0f));
		} else model = glm::rotate(model, glm::radians(45.0f* sin(anguloCamino*toRadians) * mainWindow.getmovimientoHawlucha()), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		hawluchaTexture.UseTexture();
		HawluchaBrazoDer_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, 0.1f));
		model = glm::rotate(model, glm::radians(30.0f - anguloPiernas2), glm::vec3(1.0f, 0.0f, 0.0f));
		if(!animacionEnCurso) model = glm::rotate(model, glm::radians(-45.0f * sin(anguloCamino * toRadians) * mainWindow.getmovimientoHawlucha()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		hawluchaTexture.UseTexture();
		HawluchaPiernaIzq_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, -0.1f));
		model = glm::rotate(model, glm::radians(-30.0f + anguloPiernas2), glm::vec3(1.0f, 0.0f, 0.0f));
		if(!animacionEnCurso) model = glm::rotate(model, glm::radians(45.0f * sin(anguloCamino * toRadians) *mainWindow.getmovimientoHawlucha()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		hawluchaTexture.UseTexture();
		HawluchaPiernaDer_M.RenderModel();

		if(agarrarSilla) {
			model = modelaux2;
			model = glm::translate(model, glm::vec3(-0.92f, -0.45f, 0.86f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			sillaTexture.UseTexture();
			Silla2_M.RenderModel();
		}
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, -2.0f, 60.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		ringTexture.UseTexture();
		Ring_M.RenderModel();

		if(anguloCharmander > 360.0f) anguloCharmander = 0.0f;

		if(orientacionCharmander == 0) {
			if(movimientoCharmanderX < 28.0f) {
				movimientoCharmanderX += 0.5f * deltaTime;

				if(anguloCharmander < 90.0f) {
					anguloCharmander += 15.0f * deltaTime;
				}
			} else {
				movimientoCharmanderX = 28.0f;
				orientacionCharmander = 1;
			}
		} else if(orientacionCharmander == 1) {
			if(movimientoCharmanderZ > -28.0f) {
				movimientoCharmanderZ -= 0.5f * deltaTime;

				if(anguloCharmander < 180.0f) {
					anguloCharmander += 15.0f * deltaTime;
				}
			} else {
				movimientoCharmanderZ = -28.0f;
				orientacionCharmander = 2;
			}
		} else if(orientacionCharmander == 2) {
			if(movimientoCharmanderX > 0.0f) {
				movimientoCharmanderX -= 0.5f * deltaTime;

				if(anguloCharmander < 270.0f) {
					anguloCharmander += 15.0f * deltaTime;
				}
			} else {
				movimientoCharmanderX = 0.0f;
				anguloCharmander = -90.0f;
				orientacionCharmander = 3;
			}
		} else if(orientacionCharmander == 3) {
			if(movimientoCharmanderZ < 0.0f) {
				movimientoCharmanderZ += 0.5f * deltaTime;

				if(anguloCharmander < 0.0f) {
					anguloCharmander += 15.0f * deltaTime;
				}
			} else {
				movimientoCharmanderZ = 0.0f;
				orientacionCharmander = 0;
			}
		}
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(72.0f - movimientoCharmanderX, -4.6f, 45.0f - movimientoCharmanderZ));
		model = glm::rotate(model, glm::radians(anguloCharmander), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		charmanderTexture.UseTexture();
		Charmander_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 1.3f));
		pointLights[0].SetPos(glm::vec3(model[3].x, model[3].y, model[3].z));

		if(angulosol > 270.0f || angulosol < 90.0f) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1 );
		} else {
			shaderList[0].SetPointLights(pointLights, pointLightCount );
		}
		

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f + movimientoMachamp, -2.4f, 60.0f));
		model = glm::rotate(model, glm::radians(-180.0f - anguloMachamp2), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(anguloMachamp), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		machampTexture.UseTexture();
		Machamp_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 65.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 65.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 65.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 65.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 70.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 70.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 70.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 70.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.0f, 75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, -4.0f, 75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -4.0f, 75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -4.0f, 75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sillaTexture.UseTexture();
		Silla_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -3.7f, 85.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoElotesTexture.UseTexture();
		PuestoElotes_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, -0.6f, -2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, -3.2f, 85.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoTacosTexture.UseTexture();
		PuestoTacos_M.RenderModel();

		// En el main(), en el loop principal:
		tiempoAcumuladoHumo += deltaTime;
		if (tiempoAcumuladoHumo >= intervaloCambioHumo) {
			tiempoAcumuladoHumo = 0.0f;

			// Avanzar horizontalmente
			frameX++;

			// Si llegamos al final de la fila, pasar a la siguiente
			if (frameX > 7) {
				frameX = 0;
				frameY++;

				// Si llegamos al final de todas las filas, reiniciar
				if (frameY > 7) {
					frameY = 0;
				}
			}

			// Calcular offset - cada frame es 1/8 (0.125) de la textura
			toffsetnumerocambiau = frameX * 0.333f;
			toffsetnumerocambiav = frameY * 0.333f;
		}

		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerocambiav);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 4.5f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		humoTexture.UseTexture();
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		toffset = glm::vec2(0.0f, 0.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, -3.5f, 85.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoPozoleTexture.UseTexture();
		PuestoPozole_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, -3.0f, 40.0f));
		//model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoPlantasTexture.UseTexture();
		PuestoPlantas_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, -3.0f, 40.0f));
		//model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoRecuerdosTexture.UseTexture();
		PuestoRecuerdos_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.5f, -0.9f, -1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -3.0f, 60.0f));
		//model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		puestoLuchasTexture.UseTexture();
		PuestoLuchas_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(3.0, -0.9f, -0.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		stoolTexture.UseTexture();
		Stool_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		bangbooTexture.UseTexture();
		Bangboo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, -4.5f, 36.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		stoneTexture.UseTexture();
		Arco_M.RenderModel();

		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		blackMetalTexture.UseTexture();
		PortonIzquierdo_M.RenderModel();

		modelaux = model;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		blackMetalTexture.UseTexture();
		PortonDerecho_M.RenderModel();

		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 7.5f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		letreroTexture.UseTexture();
		Letrero_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(95.0f, -5.0f, 36.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, -5.0f, 36.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -5.0f, 36.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, -5.0f, 50.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, -5.0f, 70.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 60.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -5.0f, 30.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, -5.0f, 10.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, -5.0f, 10.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
        posGlobo = glm::vec3(-20.0f + movGlobo_x, 80.0f + movGlobo_y, 60.0f + movGlobo_z);
        model = glm::translate(model, posGlobo);
        model = glm::rotate(model, giroGlobo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        
        globoTexture.UseTexture();
		Globo_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_F] && !animacionEnCurso)
	{
		agarrarSilla = true;
	}

	if (keys[GLFW_KEY_G] && !animacionEnCurso)
	{
		agarrarSilla = false;
	}

	if (keys[GLFW_KEY_J] && puedeAnimar)
	{
		animacionEnCurso = true;
	}
}
