#pragma once

#include "_Def4.h"



namespace B_ColliderShape{
	enum ColliderShape
	{
		NaN = 0,

		Capsule,
		Sphere,
		 
		Null
	};

}


class Collider_Base;
vector<Collider_Base*> sCollider_Base;

struct CollideEvent {
	bool isCollided = false;
	glm::vec3 HitNormal;
	glm::vec3 HitPoint;
	Collider_Base* Other;
};

class Collider_Base : public BanKBehavior {
	
	public:
		int Shape = B_ColliderShape::NaN;
		bool Trigger = false;

		CollideEvent Event;
		

		Collider_Base() {
			sCollider_Base.push_back(this);
		}	

		void Destruct() {
			for (size_t i = 0; i < sCollider_Base.size(); ++i) {
				if (sCollider_Base[i] == this) {
					sCollider_Base.erase(sCollider_Base.begin() + i);
					break;
				}
			}
		}
};




class Collider_Sphere : public Collider_Base {
public:
	float Radius = 0.32;
	Collider_Sphere() {
		Shape = B_ColliderShape::Sphere;
	}
};

class Collider_Capsule : public Collider_Base {
public:
	float Radius = 0.32;
	float Height = 2;
	Collider_Capsule() {
		Shape = B_ColliderShape::Capsule;
	}
};



namespace B_ColliderShape {

	namespace Contact {
		void _Cap_Cap(Collider_Base* Coll_A_CAP, Collider_Base* Coll_B_CAP) {
			Collider_Capsule* Coll_A = static_cast<Collider_Capsule*>(Coll_A_CAP);
			Collider_Capsule* Coll_B = static_cast<Collider_Capsule*>(Coll_B_CAP);

			float Distance = B_distance2D(
				Coll_A->GameObject->Transform.wPosition.x, Coll_A->GameObject->Transform.wPosition.z,
				Coll_B->GameObject->Transform.wPosition.x, Coll_B->GameObject->Transform.wPosition.z
			);

			float RadSum = Coll_A->Radius + Coll_B->Radius;

			float Ay_BOT = Coll_A->GameObject->Transform.wPosition.y ;
			float Ay_TOP = Coll_A->GameObject->Transform.wPosition.y + Coll_A->Height;
			float By_BOT = Coll_B->GameObject->Transform.wPosition.y ;
			float By_TOP = Coll_B->GameObject->Transform.wPosition.y + Coll_B->Height;
			bool HeightCollide = (Ay_TOP>By_BOT)&&(Ay_BOT < By_TOP);

			if (Distance < RadSum && HeightCollide) {
				Coll_A->Event.isCollided = Coll_B->Event.isCollided = true;
				Coll_A->Event.Other = Coll_B; Coll_B->Event.Other = Coll_A;

				if (Coll_A->Trigger || Coll_B->Trigger) { return; }

				Transform Hit_Cal; 
				Hit_Cal.wPosition = Coll_A->GameObject->Transform.wPosition;
				Hit_Cal.LookAt(Coll_B->GameObject->Transform.wPosition);
				Hit_Cal.modelMatrix_Update3D(); 

				Coll_A->Event.HitNormal = Hit_Cal.getForwardVector();
				Coll_B->Event.HitNormal = -Coll_A->Event.HitNormal;

				float Displace = (RadSum - Distance)*0.5f;
				glm::vec3 A_TargetPos = Coll_A->GameObject->Transform.wPosition + (Coll_B->Event.HitNormal * Displace);
				glm::vec3 B_TargetPos = Coll_B->GameObject->Transform.wPosition + (Coll_A->Event.HitNormal * Displace);
				Coll_A->GameObject->Transform.wPosition = A_TargetPos;
				Coll_B->GameObject->Transform.wPosition = B_TargetPos;
			}
		}

		void _Cap_Sph(Collider_Base* Coll_A_CAP, Collider_Base* Coll_B_SPH) {
			Collider_Capsule* CollA_cap = static_cast<Collider_Capsule*>(Coll_A_CAP);
			Collider_Sphere* CollB_sph = static_cast<Collider_Sphere*>(Coll_B_SPH);
		}

		void _Sph_Sph(Collider_Base* Coll_A_SPH, Collider_Base* Coll_B_SPH) {
			Collider_Sphere* CollA_sph = static_cast<Collider_Sphere*>(Coll_A_SPH);
			Collider_Sphere* CollB_sph = static_cast<Collider_Sphere*>(Coll_B_SPH);
		}

	}




	void Update() {



		if (sCollider_Base.size() > 0) {
			for (int i1 = 0; i1 < sCollider_Base.size() - 1; i1++) {
				for (int i2 = i1 + 1; i2 < sCollider_Base.size(); i2++) {




					switch (sCollider_Base[i1]->Shape)
					{
						case B_ColliderShape::Capsule:
							switch (sCollider_Base[i2]->Shape)
							{
								case B_ColliderShape::Capsule:
									Contact::_Cap_Cap(sCollider_Base[i1], sCollider_Base[i2]);
									break;
								case B_ColliderShape::Sphere:
									Contact::_Cap_Sph(sCollider_Base[i1], sCollider_Base[i2]);
									break;

								default:
									break;
							}
							break;

						case B_ColliderShape::Sphere:
							switch (sCollider_Base[i2]->Shape)
								{
								case B_ColliderShape::Capsule:
									Contact::_Cap_Sph(sCollider_Base[i2], sCollider_Base[i1]);
									break;
								case B_ColliderShape::Sphere:
									Contact::_Sph_Sph(sCollider_Base[i1], sCollider_Base[i2]);
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
	}

}