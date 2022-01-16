#pragma once
/// Load and render imported models.
///
/// This class will load and render imported models using a .obj and .mtl file

#include <vector>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Mesh.h"
#include "Texture.h"

class BoundingBox
{
public:
	BoundingBox();
	void Update(float x, float y, float z);
	int GetNumPlanes();
	glm::vec3 GetPlanePosition(int index);
	glm::vec3 GetPlaneNormal(int index);

private:
	// X, Y, Z ranges
	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;
};

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();
	BoundingBox GetBoundingBox();

	~Model();

private:

	void LoadNode(aiNode *node, const aiScene *scene);
	void LoadMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterials(const aiScene *scene);

	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;

	BoundingBox bbox;
};

