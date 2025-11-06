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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//Distancia de la camara 3er persona.
float distancia = 8.0f;
float altura = 3.0f;

//Dia y noche
float angulosol = 90.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;

Model CentroPokemon_M;
Texture centroPokemonTexture;

Model Arco_M;
Texture stoneTexture;

Model PortonIzquierdo_M;
Model PortonDerecho_M;
Texture blackMetalTexture;

Model Letrero_M;
Texture letreroTexture;

Model Torii_M;
Texture toriiTexture;

Model Ichiraku_M;
Texture ichirakuTexture;

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

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
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
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	pisoTexture = Texture("Textures/grass_texture.tga");
	pisoTexture.LoadTextureA();

	CentroPokemon_M = Model();
	CentroPokemon_M.LoadModel("Models/centroPokemon.obj");

	centroPokemonTexture = Texture("Textures/centroPokemon.tga");
	centroPokemonTexture.LoadTextureA();

	Ichiraku_M = Model();
	Ichiraku_M.LoadModel("Models/ichiraku.dae");

	ichirakuTexture = Texture("Textures/ichiraku.png");
	ichirakuTexture.LoadTextureA();

	Arco_M = Model();
	Arco_M.LoadModel("Models/Arco.dae");

	stoneTexture = Texture("Textures/stone.tga");
	stoneTexture.LoadTextureA();

	PortonIzquierdo_M = Model();
	PortonIzquierdo_M.LoadModel("Models/Gate-Left.dae");
	PortonDerecho_M = Model();
	PortonDerecho_M.LoadModel("Models/Gate-Left.dae");

	blackMetalTexture = Texture("Textures/black_metal.tga");
	blackMetalTexture.LoadTextureA();

	Letrero_M = Model();
	Letrero_M.LoadModel("Models/Letrero.dae");
	letreroTexture = Texture("Textures/letrero.tga");
	letreroTexture.LoadTextureA();



	Torii_M = Model();
	Torii_M.LoadModel("Models/torii.dae");
	toriiTexture = Texture("Textures/torii.png");
	toriiTexture.LoadTextureA();

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/Mediodia/px.png");
	skyboxFaces.push_back("Textures/Skybox/Mediodia/nx.png");
	skyboxFaces.push_back("Textures/Skybox/Mediodia/ny.png");
	skyboxFaces.push_back("Textures/Skybox/Mediodia/py.png");
	skyboxFaces.push_back("Textures/Skybox/Mediodia/pz.png");
	skyboxFaces.push_back("Textures/Skybox/Mediodia/nz.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	//Variables de camaras
	glm::mat4 vista = glm::mat4(1.0f);
	glm::vec3 posAvatar = glm::vec3(-1.27f, 10.0f, 3.12f);
	//Posición de la cámara respecto al avatar
	
	//Dirección en la que mira el personaje (en este caso hacia -Z)
	glm::vec3 dirAvatar = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 camPos = posAvatar - dirAvatar * distancia + glm::vec3(0.0f, altura, 0.0f)  ;
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);



		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		angulosol += 0.1f * deltaTime;
		if (angulosol > 360.0f)
			angulosol = 0;
		
		skybox.UpdateSkybox(angulosol);





		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//Logica de la camara 
		if (mainWindow.getcamara() == 1.0f) {
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			camera.setModoAereo(false);
			vista = camera.calculateViewMatrix();
			
		}

		//Cámara 2: vista general (desde arriba)
		else if (mainWindow.getcamara() == 2.0f) {
			camera.setModoAereo(true);
			
			vista = camera.calculateViewMatrix();
		}

		//Cámara 3: tercera persona (detrás del avatar)
		else if (mainWindow.getcamara() == 3.0f) {
			camera.setModoAereo(false);
			vista = glm::lookAt(
				camPos,                 //posición de la cámara
				posAvatar + dirAvatar,  //punto al que mira
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
		}
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));

		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 1.0f, 32.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, 0.9f, 3.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		centroPokemonTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		CentroPokemon_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-24.27f, -5.0f, 15.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		ichirakuTexture.UseTexture();
		Ichiraku_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, 0.0f, -10.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		toriiTexture.UseTexture();
		Torii_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, -4.5f, 20.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		stoneTexture.UseTexture();
		Arco_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, -4.5f, 20.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		blackMetalTexture.UseTexture();
		PortonIzquierdo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, -4.5f, 20.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		blackMetalTexture.UseTexture();
		PortonDerecho_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.27f, 3.0f, 19.12f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		letreroTexture.UseTexture();
		Letrero_M.RenderModel();

		/*
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);
		*/
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
