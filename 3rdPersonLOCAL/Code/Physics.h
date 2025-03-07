#pragma once

#include <vector>

enum class ColliderShape
{
	None,
	Capsule,
	Sphere
};

class Collider;

std::vector<Collider*> s_colliders;

struct ColliderEvent
{
	bool isCollided = false;
	glm::vec3 HitNormal;
	glm::vec3 HitPoint;
	Collider* Other;
};

class Collider
{

public:
	glm::vec3 m_position{ 0.0f };

	ColliderShape shape = ColliderShape::None;
	bool Trigger = false;

	ColliderEvent Event;

	Collider() {
		s_colliders.push_back(this);
	}

	virtual ~Collider() = default;

	void Destruct() {
		for (size_t i = 0; i < s_colliders.size(); ++i) {
			if (s_colliders[i] == this) {
				s_colliders.erase(s_colliders.begin() + i);
				break;
			}
		}
	}
};

class SphereCollider : public Collider {
public:
	float Radius = 0.32;

	SphereCollider()
	{
		shape = ColliderShape::Sphere;
	}
};

class CapsuleCollider : public Collider
{
public:

	float radius = 0.32f;
	float height = 2.0f;


	CapsuleCollider()
	{
		shape = ColliderShape::Capsule;
	}
};

class Physics
{
public:
	Physics()
	{
		s_instance = this;
	}

	void CapsuleToCapsule(Collider* Coll_A_CAP, Collider* Coll_B_CAP)
	{
		CapsuleCollider* Coll_A = static_cast<CapsuleCollider*>(Coll_A_CAP);
		CapsuleCollider* Coll_B = static_cast<CapsuleCollider*>(Coll_B_CAP);

		float Distance =  glm::distance(
			glm::vec2{ Coll_A->m_position.x, Coll_A->m_position.z },
			glm::vec2{ Coll_B->m_position.x, Coll_B->m_position.z }
		);

		float RadSum = Coll_A->radius + Coll_B->radius;

		float Ay_BOT = Coll_A->m_position.y;
		float Ay_TOP = Coll_A->m_position.y + Coll_A->height;
		float By_BOT = Coll_B->m_position.y;
		float By_TOP = Coll_B->m_position.y + Coll_B->height;

		bool HeightCollide = (Ay_TOP > By_BOT) && (Ay_BOT < By_TOP);

		if (Distance < RadSum && HeightCollide) {
			Coll_A->Event.isCollided = Coll_B->Event.isCollided = true;

			//std::cout << "CapsuleToCapsule Collided" << std::endl;

			Coll_A->Event.Other = Coll_B; Coll_B->Event.Other = Coll_A;

			if (Coll_A->Trigger || Coll_B->Trigger) { return; }
			/*
			Transform Hit_Cal;
			Hit_Cal.wPosition = Coll_A->m_position;
			Hit_Cal.LookAt(Coll_B->m_position);
			Hit_Cal.modelMatrix_Update3D();

			Coll_A->Event.HitNormal = Hit_Cal.getForwardVector();
			Coll_B->Event.HitNormal = -Coll_A->Event.HitNormal;

			float Displace = (RadSum - Distance) * 0.5f;
			glm::vec3 A_TargetPos = Coll_A->m_position + (Coll_B->Event.HitNormal * Displace);
			glm::vec3 B_TargetPos = Coll_B->m_position + (Coll_A->Event.HitNormal * Displace);
			Coll_A->m_position = A_TargetPos;
			Coll_B->m_position = B_TargetPos;
			*/
		}
	}

	void CapsuleToSphere(Collider* Coll_A_CAP, Collider* Coll_B_SPH) {
		CapsuleCollider* CollA_cap = static_cast<CapsuleCollider*>(Coll_A_CAP);
		SphereCollider* CollB_sph = static_cast<SphereCollider*>(Coll_B_SPH);
	}

	void SphereToSphere(Collider* Coll_A_SPH, Collider* Coll_B_SPH) {
		SphereCollider* CollA_sph = static_cast<SphereCollider*>(Coll_A_SPH);
		SphereCollider* CollB_sph = static_cast<SphereCollider*>(Coll_B_SPH);
	}

	void Update(float dt)
	{
		for (int i1 = 0; i1 < s_colliders.size() - 1; i1++)
		{
			for (int i2 = i1 + 1; i2 < s_colliders.size(); i2++)
			{

				switch (s_colliders[i1]->shape)
				{
				case ColliderShape::Capsule:
					switch (s_colliders[i2]->shape)
					{
					case ColliderShape::Capsule:
						CapsuleToCapsule(s_colliders[i1], s_colliders[i2]);
						break;
					case ColliderShape::Sphere:
						CapsuleToSphere(s_colliders[i1], s_colliders[i2]);
						break;

					default:
						break;
					}
					break;

				case ColliderShape::Sphere:
					switch (s_colliders[i2]->shape)
					{
					case ColliderShape::Capsule:
						CapsuleToSphere(s_colliders[i2], s_colliders[i1]);
						break;
					case ColliderShape::Sphere:
						SphereToSphere(s_colliders[i1], s_colliders[i2]);
						break;

					default:
						break;
					}
					break;


				default:
					break;
				}
			}
		}
	}

	static Physics* Get()
	{
		return s_instance;
	}

protected:
	inline static Physics* s_instance = nullptr;
};