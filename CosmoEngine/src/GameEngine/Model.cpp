#include "Model.h"

/// Load and render imported models.
///
/// This class will load and render imported models using a .obj and .mtl file


///Brief desc.
///
///**Constructor** Loads no starting variables
BoundingBox::BoundingBox()
{
	// Reset X, Y, Z ranges
	xmin = ymin = zmin = +std::numeric_limits<float>::max();
	xmax = ymax = zmax = -std::numeric_limits<float>::max();
}

///Brief desc.
///
///This function updates ranges in each dimension for the given position
void BoundingBox::Update(float x, float y, float z)
{
	// Update X, Y, Z ranges
	if (x < xmin) xmin = x;
	if (x > xmax) xmax = x;
	if (y < ymin) ymin = y;
	if (y > ymax) ymax = y;
	if (z < zmin) zmin = z;
	if (z > zmax) zmax = z;
}

///Brief desc.
///
///This function returns the number of planes in the bounding box
int BoundingBox::GetNumPlanes()
{
	return 6;
}

///Brief desc.
///
///This function returns the plane position for index 0..5
glm::vec3 BoundingBox::GetPlanePosition(int index)
{
	// Corner position corresponding to index
	switch (index)
	{
	case 0: return glm::vec3(xmin, ymin, zmin); // min corner position
	case 1: return glm::vec3(xmin, ymin, zmin); // min corner position
	case 2: return glm::vec3(xmin, ymin, zmin); // min corner position
	case 3: return glm::vec3(xmax, ymax, zmax); // max corner position
	case 4: return glm::vec3(xmax, ymax, zmax); // max corner position
	case 5: return glm::vec3(xmax, ymax, zmax); // max corner position
	}

	return glm::vec3(0.0f);
}

///Brief desc.
///
///This function returns the plane normal for index 0..5
glm::vec3 BoundingBox::GetPlaneNormal(int index)
{
	// Side normal corresponding to index
	switch (index)
	{
	case 0: return glm::vec3(-1.0f, 0.0f, 0.0f); // left
	case 1: return glm::vec3(0.0f, -1.0f, 0.0f); // bottom
	case 2: return glm::vec3(0.0f, 0.0f, -1.0f); // back
	case 3: return glm::vec3(+1.0f, 0.0f, 0.0f); // right
	case 4: return glm::vec3(0.0f, +1.0f, 0.0f); // top
	case 5: return glm::vec3(0.0f, 0.0f, +1.0f); // front
	}

	return glm::vec3(0.0f);
}

///Brief desc.
///
///**Constructor** Loads no starting variables
Model::Model()
{
}

void Model::RenderModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];

		if (materialIndex < textureList.size() && textureList[materialIndex])
		{
			textureList[materialIndex]->UseTexture();
		}

		meshList[i]->RenderMesh();
	}
}

///Brief desc.
///
///This function will load all of the model nodes (if any) and all of the children nodes
void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

///Brief desc.
///
///This function gets model file and loads it. To do this it uses assimp which is a asset import library
///@note This function uses error checking to ensure model load success
void Model::LoadModel(const std::string & fileName)
{
	//Create importer
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
		return;
	}
	LoadNode(scene->mRootNode, scene);

	LoadMaterials(scene);
}

///Brief desc.
///
///This function will load the model mesh by processing vertex positions, normals and texture coordinates
void Model::LoadMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		if (mesh->mTextureCoords[0])
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });

		// Update the bounding box with vertex position
		bbox.Update(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);
}

///Brief desc.
///
///Clear function used to set "meshList" and "textureList" variables to null
void Model::ClearModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < textureList.size(); i++)
	{
		if (textureList[i])
		{
			delete textureList[i];
			textureList[i] = nullptr;
		}
	}
}

///Brief desc.
///
///Get function which returns bounding box
BoundingBox Model::GetBoundingBox()
{
	return bbox;
}

///Brief desc.
///
///Will attempt to load the models textures. If for whatever reason texture is missing, load plain.png texture

void Model::LoadMaterials(const aiScene * scene)
{
	textureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		textureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/") + filename;

				textureList[i] = new Texture(texPath.c_str());

				if (!textureList[i]->LoadTexture())
				{
					printf("Failed to load texture at: %s\n", texPath);
					delete textureList[i];
					textureList[i] = nullptr;
				}
			}
		}

		if (!textureList[i])
		{
			textureList[i] = new Texture("Textures/plain.png");
			textureList[i]->LoadTextureA();
		}
	}
}

///Destructor
Model::~Model()
{
}
