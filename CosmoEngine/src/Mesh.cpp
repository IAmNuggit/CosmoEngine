#include "Mesh.h"
/// The Mesh class contains all the object information
///
/// The following class is responsible for loading, creating and drawing a mesh into the screen.
///when the mesh is no longer needed, it can be cleared along with the memory buffers - Helping with memory overflow


///**Constructor** - 
///Sets starting values for variables used in the class
Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCounter = 0;
}

///Brief desc.
/// 
/// Draws the mesh to the screen using glDrawElements
void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCounter, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

///Brief desc.
/// 
/// Creates a mesh using relevent variables. Creates an VAO,IBO and VBO which are stored locally to the object
///@warning At the end of this function, the mesh needs to be unbinded otherwise, clashes can occur if you are trying to load multiple objects on scene
void Mesh::CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCounter = numOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	//Unbind - Allow multiple objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

///Brief desc.
/// 
/// Clears mesh from the screen. Contains error checking which deletes buffer off the graphics card if IBO,VBO and VAO does not equal zero - Memory management to prevent memory overflow
void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		//Delete buffer off GPU
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCounter = 0;
}

///Destructor
Mesh::~Mesh()
{
	ClearMesh();
}
