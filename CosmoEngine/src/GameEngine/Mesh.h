#pragma once

/// This class contains all the object information
///
/// The following class is responsible for loading, creating and drawing a mesh into the screen.
///when the mesh is no longer needed, it can be cleared along with the memory buffers - Helping with memory overflow

#include <GL\glew.h>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCounter;
};

