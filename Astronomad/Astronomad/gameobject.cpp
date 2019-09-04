#include "gameobject.h"
#include "audiomanager.h"

GameObject::GameObject()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, xRot(0.0f)
	, yRot(0.0f)
	, zRot(0.0f)
	, xScale(1.0f)
	, yScale(1.0f)
	, zScale(1.0f)
	, rotationAngle(0.0f)
	, isLoaded(false)
	, isActive(true)
{
}

GameObject::~GameObject()
{
}

void GameObject::Initialise()
{
}

void GameObject::Update(double dTime)
{
}

void GameObject::Draw()
{
}

void GameObject::SetCamera(Camera * _cam)
{
	levelCamera = _cam;
}

void GameObject::SetX(float _x)
{
	x = _x;
}

void GameObject::SetY(float _y)
{
	y = _y;
}

void GameObject::SetZ(float _z)
{
	z = _z;
}

void GameObject::SetXRot(float _xRot)
{
	xRot = _xRot;
}

void GameObject::SetYRot(float _yRot)
{
	yRot = _yRot;
}

void GameObject::SetZRot(float _zRot)
{
	zRot = _zRot;
}

void GameObject::SetXScale(float _xScale)
{
	xScale = _xScale;
}

void GameObject::SetYScale(float _yScale)
{
	yScale = _yScale;
}

void GameObject::SetZScale(float _zScale)
{
	zScale = _zScale;
}

void GameObject::SetUniformScale(float _scale)
{
	xScale = _scale;
	yScale = _scale;
	zScale = _scale;
}

void GameObject::SetRotationAngle(float _angle)
{
	rotationAngle = _angle;
}

void GameObject::SetImageFilePath(const char * _imagefilepath)
{
	imageFilePath = _imagefilepath;
}

void GameObject::SetShaderLoader(ShaderLoader * _sloader)
{
	shaderLoader = _sloader;
}

void GameObject::SetTextureLoader(AssetLoader * _tloader)
{
	assetLoader = _tloader;
}

void GameObject::SetInputManager(InputManager * _imanager)
{
	inputManager = _imanager;
}

void GameObject::SetVBO(GLuint _vbo)
{
	VBO = _vbo;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void GameObject::SetClock(Clock * _clock)
{
	clock = _clock;
}

void GameObject::SetVecTexts(std::vector<Texter*>* _vecTexts)
{
	vecTexts = _vecTexts;
}

void GameObject::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}

void GameObject::SetAvoidRadius(float _avoid)
{
	avoidRadius = _avoid;
}

void GameObject::SetVelocity(glm::vec2 _velocity)
{
	velocity = _velocity;
}

float GameObject::GetX() const
{
	return x;
}

float GameObject::GetY() const
{
	return y;
}

float GameObject::GetZ() const
{
	return z;
}

float GameObject::GetXRot() const
{
	return xRot;
}

float GameObject::GetYRot() const
{
	return yRot;
}

float GameObject::GetZRot() const
{
	return zRot;
}

float GameObject::GetRotationAngle() const
{
	return rotationAngle;
}

float GameObject::GetXScale() const
{
	return xScale;
}

float GameObject::GetYScale() const
{
	return yScale;
}

float GameObject::GetZScale() const
{
	return zScale;
}

float GameObject::GetAvoidRadius() const
{
	return avoidRadius;
}

float GameObject::GetMaxSpeed() const
{
	return maxSpeed;
}

float GameObject::GetDirection() const
{
	return direction;
}

glm::vec2 GameObject::GetVelocity() const
{
	return velocity;
}

bool GameObject::GetIsActive() const
{
	return isActive;
}

ShaderLoader * GameObject::GetShaderLoader()
{
	return shaderLoader;
}

AssetLoader * GameObject::GetAssetLoader()
{
	return assetLoader;
}

GLuint GameObject::GetVBO()
{
	return VBO;
}

GLuint GameObject::GetSkyBoxTexture()
{
	return skyBoxTexture;
}

Camera * GameObject::GetCamera()
{
	return levelCamera;
}

Clock * GameObject::GetClock()
{
	return clock;
}

int GameObject::GetScreenX() const
{
	return screenX;
}

int GameObject::GetScreenY() const
{
	return screenY;
}

//------------------ Steering Behaviours -----------------------

glm::vec2 GameObject::Seek(float _tX, float _tY, float _maxSpeed, float _turnSpeed, bool _arrival, bool _flee)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 desired = glm::vec2(_tX - x, _tY - z);
	float distance = sqrt((desired.x * desired.x) + (desired.y * desired.y));

	if (desired.x != 0.0f && desired.y != 0.0f)
	{
		desired = glm::normalize(desired);
	}

	if (_arrival && distance < arrivalDist)
	{
		if (distance > arrivalDist*0.1f)
		{
			float m = (distance / arrivalDist);
			desired *= m;
		}
		else
		{
			desired *= 0.0f;
			velocity *= 0.0f;
		}
	}
	else
	{
		desired *= _maxSpeed;
	}

	if (_flee == false)
	{
		steer = desired - velocity;
	}
	else
	{
		steer = -desired - velocity;
	}

	steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
	steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);

	return steer;
}

glm::vec2 GameObject::Pursue(GameObject * _target, float _maxSpeed, float _turnSpeed, bool _arrival, bool _evade)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	float targetX, targetZ;

	if (_target != nullptr)
	{
		//Pursue Target (with velocity)
		glm::vec2 tVelocity = _target->GetVelocity() * 10.0f;
		targetX = _target->GetX() + tVelocity.x;
		targetZ = _target->GetZ() + tVelocity.y; 
	}

	steer = Seek(targetX, targetZ, _maxSpeed, _turnSpeed, _arrival, _evade);

	return steer;
}

glm::vec2 GameObject::Wander(float _maxSpeed, float _turnSpeed, float _ahead, float _radius, float _rate)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 wA = glm::normalize(velocity);
	wA *= _ahead;

	static float timeElapsed = 0.0f;
	static float dir = (float)(rand() % 360);

	if (timeElapsed > _rate)
	{
		dir = (float)(rand() % 360);
		timeElapsed = 0;
	}
	else
	{
		timeElapsed += 1.0f * deltaTime;
	}

	glm::vec2 wB;
	wB.x = x + wA.x + Utils::LengthDirX(_radius, dir);
	wB.y = z + wA.y + Utils::LengthDirY(_radius, dir);

	steer = Seek(wB.x, wB.y, _maxSpeed, _turnSpeed, false, false);

	return steer;
}

glm::vec2 GameObject::Separation(float _maxSpeed, float _turnSpeed, float _buffer, std::vector<GameObject*>* _vecOther)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 sum = glm::vec2(0.0f, 0.0f);
	int count = 0;
	if (_vecOther != nullptr && !_vecOther->empty())
	{
		for (auto it = _vecOther->begin(); it != _vecOther->end(); it++)
		{
			GameObject* other = *it;
			glm::vec2 myPos = glm::vec2(x, z);
			glm::vec2 otherPos = glm::vec2(other->GetX(), other->GetZ());
			float dist = glm::distance(myPos, otherPos);

			//If not itself and within the other's seperation radius
			if (dist > 0.0f && dist < _buffer + other->GetAvoidRadius())
			{
				glm::vec2 diff = myPos - otherPos;
				diff = glm::normalize(diff);
				diff = diff / dist;
				sum += diff;
				count++;
			}
		}
		if (count > 0)
		{
			sum /= count;
			sum = glm::normalize(sum);
			sum *= _maxSpeed;
			steer = sum - velocity;
			steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
			steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);
		}
	}
	return steer;
}

glm::vec2 GameObject::Cohesion(float _maxSpeed, float _turnSpeed, float _radius, std::vector<GameObject*>* _vecOther)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 sum = glm::vec2(0.0f, 0.0f);

	int count = 0;
	for (auto it = _vecOther->begin(); it != _vecOther->end(); it++)
	{
		GameObject* other = *it;
		glm::vec2 myPos = glm::vec2(x, z);
		glm::vec2 otherPos = glm::vec2(other->GetX(), other->GetZ());
		float dist = glm::distance(myPos, otherPos);

		if (dist > 0 && dist < _radius)
		{
			sum += otherPos;
			count++;
		}
	}
	if (count > 0)
	{
		sum /= (float)count;
		steer = Seek(sum.x, sum.y, _maxSpeed, _turnSpeed, false, false);
		steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
		steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);
	}

	return steer;
}

glm::vec2 GameObject::Alignment(float _maxSpeed, float _turnSpeed, float _radius, std::vector<GameObject*>* _vecOther)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 sum = glm::vec2(0.0f, 0.0f);
	float speed = _maxSpeed;
	int count = 0;
	for (auto it = _vecOther->begin(); it != _vecOther->end(); it++)
	{
		GameObject* other = *it;
		glm::vec2 myPos = glm::vec2(x, z);
		glm::vec2 otherPos = glm::vec2(other->GetX(), other->GetZ());
		float dist = glm::distance(myPos,otherPos);
		//float otherDir = -other->GetDirection() + 180.0f;
		//glm::vec2 otherVelo = glm::vec2(Utils::LengthDirX(_maxSpeed, -otherDir), Utils::LengthDirY(_maxSpeed, -otherDir));
		//otherVelo.x = Utils::Clamp(otherVelo.x, -_maxSpeed, _maxSpeed);
		//otherVelo.y = Utils::Clamp(otherVelo.y, -_maxSpeed, _maxSpeed);

		if (dist > 0 && dist < _radius)
		{
			//sum += otherVelo;
			sum += other->GetVelocity();
			count++;
		}
	}
	if (count > 0)
	{
		sum /= (float)count; //Get average velocity
		//speed = sqrt((sum.x * sum.y) + (sum.y * sum.y));
		if (sum.x != 0.0f && sum.y != 0.0f)
		{
			sum = glm::normalize(sum);
		}
		sum *= _maxSpeed;

		steer = sum - velocity;
		steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
		steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);
	}

	return steer;
}

glm::vec2 GameObject::Orbit(float _maxSpeed, float _turnSpeed, float _orbitDist, float _orbitSpeed, std::vector<GameObject*>* _vecOther, GameObject* _target)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 myPos = glm::vec2(x, z);
	static glm::vec2 behind = glm::vec2(0.0f, 0.0f);

	if (_target != nullptr)
	{
		glm::vec2 leaderPos = glm::vec2(_target->GetX(), _target->GetZ());

		//Find the orbit position
		float msPassed = clock->GetMsPassed() * 0.001f * _orbitSpeed;
		behind = glm::vec2(_orbitDist * cos(msPassed), _orbitDist * sin(msPassed));

		steer += Alignment(_maxSpeed, _turnSpeed, 10.0f, _vecOther);
		steer += Separation(_maxSpeed, _turnSpeed, 1.0f, _vecOther);
		steer += Seek(leaderPos.x + behind.x, leaderPos.y + behind.y, _maxSpeed, _turnSpeed, false, false);
	}

	steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
	steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);

	return steer;
}

glm::vec2 GameObject::Containment(float _maxSpeed, float _turnSpeed, float _buffer, float _boundW, float _boundH)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 desired = glm::vec2(0.0f, 0.0f);

	if (x < -(_boundW - _buffer))
	{
		desired = glm::vec2(_maxSpeed * 10.0f, velocity.y);
	}
	if (x > (_boundW - _buffer))
	{
		desired = glm::vec2(-_maxSpeed * 10.0f, velocity.y);
	}
	if (z < -(_boundH - _buffer))
	{
		desired = glm::vec2(velocity.x, _maxSpeed * 10.0f);
	}
	if (z > (_boundH - _buffer))
	{
		desired = glm::vec2(velocity.x, -_maxSpeed * 10.0f);
	}

	steer = desired - velocity;
	steer.x = Utils::Clamp(steer.x, -_turnSpeed, _turnSpeed);
	steer.y = Utils::Clamp(steer.y, -_turnSpeed, _turnSpeed);

	return steer;
}

glm::vec2 GameObject::FollowPath(float _maxSpeed, float _turnSpeed, Path _path)
{
	glm::vec2 steer = glm::vec2(0.0f, 0.0f);
	glm::vec2 myPos = glm::vec2(x, z);

	//Find predicted position
	glm::vec2 predict = velocity;
	predict = glm::normalize(predict);
	predict *= _maxSpeed;
	glm::vec2 predictPos = myPos + predict;

	//Find the projected point on the path
	glm::vec2 a = predictPos - _path.start;
	glm::vec2 b = _path.end - _path.start;

	b = glm::normalize(b);
	b *= glm::dot(a, b);
	glm::vec2 normalPoint = _path.start + b;

	float dist = glm::distance(predictPos, normalPoint);
	if (dist > _path.radius)
	{
		b = glm::normalize(b);
		b *= _maxSpeed;
		glm::vec2 target = normalPoint + b;

		steer = Seek(target.x, target.y, _maxSpeed, _turnSpeed, true, false);
	}

	return steer;
}
