#include "camera.h"
#include "gameobject.h"

Camera::Camera()
{
	camPos = glm::vec3(0.0f, 0.0f, 10.0f);
	camLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	camUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	scrWidth = WINDOW_WIDTH;
	scrHeight = WINDOW_HEIGHT;
	fieldOfView = 70.0f;
	nearPlane = 0.01f;
	farPlane = 40000.0f;
	isPerspective = true;
}

Camera::~Camera()
{

}

void Camera::ComputeView()
{
	view = glm::lookAt(camPos, camPos + camLookDir, camUpDir);
}

void Camera::ComputeProjection()
{
	if (isPerspective)
	{
		proj = glm::perspective(fieldOfView, (float)scrWidth / (float)scrHeight, nearPlane, farPlane);
	}
	else
	{
		float halfScreenWidth = (float)scrWidth * 0.5f;
		float halfScreenHeight = (float)scrHeight * 0.5f;
		proj = glm::ortho(-halfScreenWidth, halfScreenWidth, -halfScreenHeight, halfScreenHeight, nearPlane, farPlane);
	}
}

void Camera::ComputeFollow()
{
	if (isFollowing && followTarget != nullptr)
	{
		float halfScreenHeight = (float)scrHeight * 0.5f;
		camPos.x = followTarget->GetX();
		camPos.z = followTarget->GetZ() - 40.0f;
	}
}

glm::mat4 Camera::GetView() const
{
	return view;
}

glm::mat4 Camera::GetProjection() const
{
	return proj;
}

glm::vec3 Camera::GetCamPos() const
{
	return camPos;
}

glm::vec3 Camera::SetCamLookDir() const
{
	return camLookDir;
}

glm::vec3 Camera::SetCamUpDir() const
{
	return camUpDir;
}

void Camera::SetCamPos(glm::vec3 _pos)
{
	camPos = _pos;
}

void Camera::SetCamLookDir(glm::vec3 _dir)
{
	camLookDir = _dir;
}

void Camera::SetCamUpDir(glm::vec3 _dir)
{
	camUpDir = _dir;
}

void Camera::SetIsFollowing(bool _isFollow)
{
	isFollowing = _isFollow;
}

void Camera::SetFollowTarget(GameObject * _target)
{
	followTarget = _target;
}
