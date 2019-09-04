#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils.h"

class Camera
{
public:
	Camera();
	~Camera();

	void ComputeView();
	void ComputeProjection();

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;
	glm::vec3 GetCamPos() const;
	glm::vec3 SetCamLookDir() const;
	glm::vec3 SetCamUpDir() const;
	void SetCamPos(glm::vec3 _pos);
	void SetCamLookDir(glm::vec3 _dir);
	void SetCamUpDir(glm::vec3 _dir);

private:
	unsigned int scrWidth;
	unsigned int scrHeight;
	bool isPerspective;
	float fieldOfView;
	float nearPlane;
	float farPlane;
	glm::vec3 camPos;
	glm::vec3 camLookDir;
	glm::vec3 camUpDir;
	glm::mat4 view;
	glm::mat4 proj;
};