#include "Skybox.h"



Skybox::Skybox()
{
}

Skybox::Skybox(std::vector<std::string> faceLocations)
{
	skyShader = new Shader();
	skyShader->CreateFromFiles("shaders/skybox.vert", "shaders/skybox.frag");
	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	int width, height, bitDepth;
	for (size_t i = 0; i < 6; i++)
	{

		unsigned char *texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0); //el tipo unsigned char es para un array de bytes de la imagen, obtener datos de la imagen 
		if (!texData)
		{
			printf("No se encontr� : %s", faceLocations[i].c_str());
			return;
		}
		//para cambiar el origen a la esquina inferior izquierda como necesitamos
		//stbi_set_flip_vertically_on_load(true);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData); //SIN CANAL ALPHA A ENOS QUE QUERAMOS AGREGAR EFECTO DE PARALLAX
		stbi_image_free(texData); //para liberar la informaci�n de la imagen
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Creando el Mesh del skybox
	unsigned int skyboxIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v0
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V1
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V2
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V3
		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V4
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V5
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V6
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f//V7
	};
	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
	skyboxSets = {
		{ //Amanecer
			"Textures/Skybox/Amanecer/px.png",
			"Textures/Skybox/Amanecer/nx.png",
			"Textures/Skybox/Amanecer/ny.png",
			"Textures/Skybox/Amanecer/py.png",
			"Textures/Skybox/Amanecer/pz.png",
			"Textures/Skybox/Amanecer/nz.png"
		},
		{ //Ma�ana
			"Textures/Skybox/Manana/px.png",
			"Textures/Skybox/Manana/nx.png",
			"Textures/Skybox/Manana/ny.png",
			"Textures/Skybox/Manana/py.png",
			"Textures/Skybox/Manana/pz.png",
			"Textures/Skybox/Manana/nz.png"
		},
		{ //Mediod�a
			"Textures/Skybox/Mediodia/px.png",
			"Textures/Skybox/Mediodia/nx.png",
			"Textures/Skybox/Mediodia/ny.png",
			"Textures/Skybox/Mediodia/py.png",
			"Textures/Skybox/Mediodia/pz.png",
			"Textures/Skybox/Mediodia/nz.png"
		},
		{ // Tarde
			"Textures/Skybox/Tarde/px.png",
			"Textures/Skybox/Tarde/nx.png",
			"Textures/Skybox/Tarde/ny.png",
			"Textures/Skybox/Tarde/py.png",
			"Textures/Skybox/Tarde/pz.png",
			"Textures/Skybox/Tarde/nz.png"
		},
		{ //Puesta de sol
			"Textures/Skybox/PuestaSol/px.png",
			"Textures/Skybox/PuestaSol/nx.png",
			"Textures/Skybox/PuestaSol/ny.png",
			"Textures/Skybox/PuestaSol/py.png",
			"Textures/Skybox/PuestaSol/pz.png",
			"Textures/Skybox/PuestaSol/nz.png"
		},
		{ //Noche
			"Textures/Skybox/Noche/px.png",
			"Textures/Skybox/Noche/nx.png",
			"Textures/Skybox/Noche/ny.png",
			"Textures/Skybox/Noche/py.png",
			"Textures/Skybox/Noche/pz.png",
			"Textures/Skybox/Noche/nz.png"
		}
	};
}
void Skybox::UpdateSkybox(float anguloSol)
{
	//Normaliza el �ngulo (0 a 360)
	if (anguloSol < 0.0f) anguloSol += 360.0f;
	if (anguloSol > 360.0f) anguloSol -= 360.0f;

	int newSkyIndex = 0;

	//Definir intervalos (puedes ajustarlos)
	if (anguloSol >= 0.0f && anguloSol < 60.0f) newSkyIndex = 3;
	else if (anguloSol >= 60.0f && anguloSol < 120.0f) newSkyIndex = 4;
	else if (anguloSol >= 120.0f && anguloSol < 180.0f) newSkyIndex = 5;
	else if (anguloSol >= 180.0f && anguloSol < 240.0f) newSkyIndex = 0;
	else if (anguloSol >= 240.0f && anguloSol < 300.0f) newSkyIndex = 1;
	else newSkyIndex = 2;



	if (newSkyIndex == currentSkyIndex) return;
	currentSkyIndex = newSkyIndex;

	glDeleteTextures(1, &textureId);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;
	for (size_t i = 0; i < 6; i++)
	{
		unsigned char* texData = stbi_load(skyboxSets[newSkyIndex][i].c_str(), &width, &height, &bitDepth, 0);
		if (!texData)
		{
			printf("No se encontr�: %s\n", skyboxSets[newSkyIndex][i].c_str());
			return;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	glDepthMask(false);
	skyShader->UseShader();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	//skyShader->Validate();
	skyMesh->RenderMesh();
	glDepthMask(true);

}


Skybox::~Skybox()
{
}
