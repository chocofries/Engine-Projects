#pragma once

// Local Includes
#include "mesh.h"

#include <glew.h>
#include <freeglut.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

class MeshSphere : public Mesh
{
public:
	MeshSphere(GameObject* _parent, GLuint _skyBoxTexture);
	~MeshSphere();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();
};

