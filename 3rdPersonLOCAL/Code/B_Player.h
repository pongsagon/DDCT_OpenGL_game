#pragma once

#include "BanKEngine.h"
#include "Input.h"
#include "Renderer.h"

#include <learnopengl/shader.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>


bool Safe = false;
bool DoRaycast = true;


namespace Steve {
	class Steve {
	public:

		Animation* idleAnimation;
		Animation* idleAnimation_NOGUN;
		Animation* walkAnimation;
		Animation* runAnimation;
		Animation* runAnimation_NOGUN;
		Animation* DeadAnimation;
		Animation* kickAnimation;
		Animation* HitAnimation;
		Model m_model;

		Model* Bullet_Model;
		Model* Gun_Model;

		Steve()
			: m_model("Assets/Models/mixamo/steve.dae")
		{
			idleAnimation = new Animation("Assets/Models/mixamo/Rifle Aiming Idle.dae", &m_model);
			idleAnimation_NOGUN = new Animation("Assets/Models/mixamo/idle.dae", &m_model);
			walkAnimation = new Animation("Assets/Models/mixamo/walk.dae", &m_model);
			runAnimation = new Animation("Assets/Models/mixamo/Rifle Run.dae", &m_model);
			runAnimation_NOGUN = new Animation("Assets/Models/mixamo/Run.dae", &m_model);
			DeadAnimation = new Animation("Assets/Models/mixamo/Dying.dae", &m_model);
			kickAnimation = new Animation("Assets/Models/mixamo/kick.dae", &m_model);
			HitAnimation = new Animation("Assets/Models/mixamo/Hit Reaction.dae", &m_model);

			Bullet_Model = new Model("Assets/Models/Bullets/Bullets.obj");
			Gun_Model = new Model("Assets/Models/AK47/OBJ/ak7finished.obj");
		}

	}*Data_;

	void Load() {
		Data_ = new Steve;
	}

}





class Bullet : public BanKBehavior
{

public:

	static 	enum Team
	{
		Player = 0,
		Enemy
	};

	float lifespan = 0.64;
	float Speed = 0.8f;
	Model* m_model;
	Collider_Capsule* mCollider_Capsule;
	int Team = Bullet::Player;

	Bullet(Model* m_model) :m_model(m_model) {
	}

	void Init() {
		GameObject->Transform.wScale = glm::vec3(0.16f);
		mCollider_Capsule = GameObject->AddComponent(new Collider_Capsule);
		mCollider_Capsule->Radius = 0.5f;
		mCollider_Capsule->Height = 0.1f;
		mCollider_Capsule->Trigger = true;

	}

	void Update() {
		GameObject->Transform.wPosition += GameObject->Transform.getForwardVector() * Speed;

		lifespan -= Time.Deltatime;
		if (lifespan < 0) {
			GameObject->Destroy = true;
		}

		if (mCollider_Capsule->Event.isCollided) {
			//GameObject->Destroy = true; 
		}
	}

	void Render(Renderer& renderer) {
		renderer.DrawModel(*Steve::Data_->Bullet_Model, GameObject->Transform.modelMatrix);
	}
};

int GunCount = 0;
float SpawnTimer_Gun = 0;
float SpawnTimer_Gun_MAX = 4;
class Gun : public BanKBehavior
{
public:
	int FloatDir = 1;
	Collider_Capsule* mCollider_Capsule;

	void Init() {
		GunCount++;
		GameObject->Transform.wScale = glm::vec3(0.05);
		mCollider_Capsule = GameObject->AddComponent(new Collider_Capsule);
		mCollider_Capsule->Radius = 0.5;
	}
	void Update() {

		GameObject->Transform.wRotation.y += Time.Deltatime * 25;
		if (GameObject->Transform.wPosition.y < 0) { GameObject->Transform.wPosition.y = 0; }

		//if (GameObject->Transform.wPosition.y > 0.5) {
		//	FloatDir = -1;
		//}
		//else if (GameObject->Transform.wPosition.y < 0) {
		//	FloatDir = 1;
		//}
		//GameObject->Transform.wPosition.y += FloatDir * 0.5 * Time.Deltatime;


	}
	void Render(Renderer& renderer)
	{
		renderer.DrawModel(*Steve::Data_->Gun_Model, GameObject->Transform.modelMatrix);
	}
	void Destruct() { GunCount--; }
};


class Player : public BanKBehavior
{
	bool Dead = false;
	float DeadTimer = 3;
public:

	struct Controls {
		bool MOVE_FWD = false;
		bool MOVE_BACK = false;
		bool MOVE_LFT = false;
		bool MOVE_RHT = false;
		bool ATK_1 = false;
		float TURN_X = 0;
		float TURN_Y = 0;
		void Update() {
			MOVE_FWD = Input::GetKey(GLFW_KEY_W);
			MOVE_BACK = Input::GetKey(GLFW_KEY_S);
			MOVE_LFT = Input::GetKey(GLFW_KEY_A);
			MOVE_RHT = Input::GetKey(GLFW_KEY_D);
			ATK_1 = Input::GetKey(GLFW_KEY_N) || Input::GetMouseButton(GLFW_MOUSE_BUTTON_1);

			pair Mouse = Input::getMousePosChange();
			TURN_Y = Mouse.first;
			TURN_X = Mouse.second;
		}
	}Controls;

	Player();
	void Update();
	void Render(Renderer& renderer);

	glm::vec3 Velocity;


	int Health = 3;
	GameObj* Gun_OBJ;
	glm::mat4 Gun_Matrix;
	const float Gun_AnimCooldownMax = 0.25f;
	float Gun_AnimCooldown = 0;
	const float Gun_CooldownMax = 0.1f;
	float Gun_Cooldown = 0;
	bool HasGun = false;
	Collider_Capsule* mCollider_Capsule;

	GameObj* CamArea;
	GameObj* CamSocket;
	GameObj* CamLookat;
	GameObj* BODY_RotProbe;
	float BODY_RotProbe_TargetRot;
	GameObj* BODY;
	float BODY_TargetRot;

	void Init() {

		mCollider_Capsule = GameObject->AddComponent(new Collider_Capsule);

		CamArea = GameObject->CreateChild();
		CamArea->Transform.wPosition = glm::vec3(0, 1.25, 0);

		CamSocket = CamArea->CreateChild();
		CamSocket->Transform.wPosition = glm::vec3(-0.25, 0, -1) * 2.0f;

		CamLookat = CamArea->CreateChild();
		CamLookat->Transform.wPosition = glm::vec3(-0.25, 0, 1) * 99999.0f;
		CamLookat->Transform.wPosition.x += 0.05;



		BODY_RotProbe = GameObject->CreateChild();

		BODY = BODY_RotProbe->CreateChild();

		Gun_OBJ = BODY->CreateChild();
		Gun_OBJ->Transform.wPosition = glm::vec3(-5, 7, 0) * 20.0f;
		Gun_OBJ->Transform.wRotation = glm::vec3(-90, 180, 0);
		Gun_OBJ->Transform.wScale = glm::vec3(3);



	}

private:
	enum AnimState {
		IDLE = 1,
		IDLE_PUNCH,
		PUNCH_IDLE,
		IDLE_KICK,
		KICK_IDLE,
		IDLE_WALK,
		WALK_IDLE,
		WALK
	};

	enum AnimState charState = IDLE;
	float blendAmount = 0.0f;


	Model m_model;
	std::unique_ptr<Animator> m_animator;

	Animation* idleAnimation;
	Animation* walkAnimation;
	Animation* runAnimation;
	Animation* DeadAnimation;
	Animation* kickAnimation;



	bool Input = false;
	void Update_Behavior() {
		Controls.Update();

		Input = false;
		float Accel = Time.Deltatime * 32;

		Gun_Cooldown += Time.Deltatime;
		Gun_AnimCooldown -= Time.Deltatime;
		if (Controls.ATK_1 && HasGun) {
			BODY_RotProbe_TargetRot = CamArea->Transform.wRotation.y;
			BODY_TargetRot = 0;
			Gun_AnimCooldown = Gun_AnimCooldownMax;

			if (Gun_Cooldown > Gun_CooldownMax) {
				Gun_Cooldown = 0;
				GameObj* BulletOBJ = GameObj::Create();
				BulletOBJ->Transform.wPosition = getDirectPosition(Gun_Matrix);
				BulletOBJ->Transform.LookAt(CamLookat->Transform.getWorldPosition());
				BulletOBJ->AddComponent(new Bullet(Steve::Data_->Bullet_Model));

				CamArea->Transform.wRotation.x -= B_irand(-2, 5);
				CamArea->Transform.wRotation.y += B_irand(-5, 5);
			}

		}
		else if (Gun_AnimCooldown <= 0) {


			if (Controls.MOVE_FWD) {
				Input = true;
				Velocity += BODY_RotProbe->Transform.getForwardVector() * Accel;
				BODY_TargetRot = B_lerp(BODY_TargetRot, 0, Time.Deltatime * 9);
			}
			else if (Controls.MOVE_BACK)
			{
				Input = true;
				Velocity -= BODY_RotProbe->Transform.getForwardVector() * Accel;
				BODY_TargetRot = B_lerp(BODY_TargetRot, 180, Time.Deltatime * 9);
			}

			if (Controls.MOVE_LFT) {
				Input = true;
				Velocity += BODY_RotProbe->Transform.getLeftVector() * Accel;
				BODY_TargetRot = B_lerp(BODY_TargetRot, 90, Time.Deltatime * 9);
			}
			else if (Controls.MOVE_RHT)
			{
				Input = true;

				Velocity -= BODY_RotProbe->Transform.getLeftVector() * Accel;

				if (Controls.MOVE_BACK)
				{
					BODY_TargetRot = B_lerp(BODY_TargetRot, 270, Time.Deltatime * 9);
				}
				else
				{
					BODY_TargetRot = B_lerp(BODY_TargetRot, -90, Time.Deltatime * 9);
				}
			}
		}




		if (Input::GetKey(GLFW_KEY_E)) {
			Input = true;
			Velocity.y += Accel;
		}
		else if (Input::GetKey(GLFW_KEY_Q)) {
			Input = true;
			Velocity.y -= Accel;
		}


		if (Input::GetKey(GLFW_KEY_LEFT)) {
			CamArea->Transform.wRotation.y += 64 * Time.Deltatime;
		}
		else if (Input::GetKey(GLFW_KEY_RIGHT)) {
			CamArea->Transform.wRotation.y -= 64 * Time.Deltatime;
		}
		if (Input::GetKey(GLFW_KEY_UP)) {
			CamArea->Transform.wRotation.x -= 64 * Time.Deltatime;
		}
		else if (Input::GetKey(GLFW_KEY_DOWN)) {
			CamArea->Transform.wRotation.x += 64 * Time.Deltatime;
		}




		float MaxVel = 5;
		Velocity.x = B_clamp(Velocity.x, -MaxVel, MaxVel);
		Velocity.y = B_clamp(Velocity.y, -MaxVel, MaxVel);
		Velocity.z = B_clamp(Velocity.z, -MaxVel, MaxVel);
		Velocity = B_lerpVec3(Velocity, glm::vec3(0), Time.Deltatime * 4);
		GameObject->Transform.wPosition += Velocity * Time.Deltatime;


		CamArea->Transform.wRotation.y -= 0.25f * Controls.TURN_Y;
		CamArea->Transform.wRotation.x += 0.25f * Controls.TURN_X;
		CamArea->Transform.wRotation.x = B_clamp(CamArea->Transform.wRotation.x, -90, 90);


		if (Input) {
			BODY_RotProbe_TargetRot = CamArea->Transform.wRotation.y;
		}



		float RotSpd_A = 10 * Time.Deltatime;
		float RotSpd_B = 12 * Time.Deltatime;
		BODY_RotProbe->Transform.wRotation.y = B_lerp(BODY_RotProbe->Transform.wRotation.y, BODY_RotProbe_TargetRot, RotSpd_A);
		BODY->Transform.wRotation.y = B_lerp(BODY->Transform.wRotation.y, BODY_TargetRot, RotSpd_B);



		if (mCollider_Capsule->Event.isCollided) {
			Bullet* GetBull = nullptr;
			GetBull = mCollider_Capsule->Event.Other->GameObject->GetComponent(GetBull);
			if (GetBull) {
				if (GetBull->Team == Bullet::Enemy) {
					m_animator->PlayAnimation(Steve::Data_->HitAnimation, NULL, 0.1, 0.0f, 0.0f);
					GetBull->GameObject->Destroy = true;

					Health--;
					if (Health <= 0) {
						Dead = true;
						m_animator->PlayAnimation(DeadAnimation, NULL, 0.0f, 0.0f, 0.0f);
					}
				}
			}
			else
			{
				Gun* GetGun = nullptr;
				GetGun = mCollider_Capsule->Event.Other->GameObject->GetComponent(GetGun);
				if (GetGun) {
					GetGun->GameObject->Destroy = true;
					HasGun = true;
				}
			}
		}


		if (Input::GetKey(GLFW_KEY_F)) {
			Dead = true;
			m_animator->PlayAnimation(DeadAnimation, NULL, 0.0f, 0.0f, 0.0f);
		}


	}




	void Animate() {
		if (Input::GetKeyDown(GLFW_KEY_1))
			m_animator->PlayAnimation(idleAnimation, NULL, 0.0f, 0.0f, 0.0f);
		if (Input::GetKeyDown(GLFW_KEY_2))
			m_animator->PlayAnimation(walkAnimation, NULL, 0.0f, 0.0f, 0.0f);
		if (Input::GetKeyDown(GLFW_KEY_3))
			m_animator->PlayAnimation(DeadAnimation, NULL, 0.0f, 0.0f, 0.0f);
		if (Input::GetKeyDown(GLFW_KEY_4))
			m_animator->PlayAnimation(kickAnimation, NULL, 0.0f, 0.0f, 0.0f);

		if (HasGun) {
			idleAnimation = Steve::Data_->idleAnimation;
			runAnimation = Steve::Data_->runAnimation;
		}
		else
		{
			idleAnimation = Steve::Data_->idleAnimation_NOGUN;
			runAnimation = Steve::Data_->runAnimation_NOGUN;
		}

		float blendRate = Time.Deltatime * 4;
		switch (charState) {
		case IDLE:
			if (Input) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(idleAnimation, walkAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_WALK;
			}
			else if (Input::GetKey(GLFW_KEY_J)) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(idleAnimation, DeadAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_PUNCH;
			}
			else if (Input::GetKey(GLFW_KEY_K)) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(idleAnimation, kickAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_KICK;
			}
			printf("idle \n");
			break;
		case IDLE_WALK:
			blendAmount += blendRate;
			m_animator->PlayAnimation(idleAnimation, runAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(runAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = WALK;
			}
			printf("idle_walk \n");
			break;
		case WALK:
			m_animator->PlayAnimation(runAnimation, NULL, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (!Input) {
				charState = WALK_IDLE;
			}
			printf("walking\n");
			break;
		case WALK_IDLE:
			blendAmount += blendRate;
			m_animator->PlayAnimation(runAnimation, idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(idleAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = IDLE;
			}
			printf("walk_idle \n");
			break;
		case IDLE_PUNCH:
			blendAmount += blendRate;
			m_animator->PlayAnimation(idleAnimation, DeadAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(DeadAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = PUNCH_IDLE;
			}
			printf("idle_punch\n");
			break;
		case PUNCH_IDLE:
			if (m_animator->m_CurrentTime > 0.7f) {
				blendAmount += blendRate;
				m_animator->PlayAnimation(DeadAnimation, idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = m_animator->m_CurrentTime2;
					m_animator->PlayAnimation(idleAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = IDLE;
				}
				printf("punch_idle \n");
			}
			else {
				// punching
				printf("punching \n");
			}
			break;
		case IDLE_KICK:
			blendAmount += blendRate;
			m_animator->PlayAnimation(idleAnimation, kickAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(kickAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = KICK_IDLE;
			}
			printf("idle_kick\n");
			break;
		case KICK_IDLE:
			if (m_animator->m_CurrentTime > 1.0f) {
				blendAmount += blendRate;
				m_animator->PlayAnimation(kickAnimation, idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = m_animator->m_CurrentTime2;
					m_animator->PlayAnimation(idleAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = IDLE;
				}
				printf("kick_idle \n");
			}
			else {
				// punching
				printf("kicking \n");
			}
			break;
		}
	}


};




#include "Input.h"
#include <GLFW/glfw3.h>

Player::Player()
	: m_model(Steve::Data_->m_model)
	, idleAnimation(Steve::Data_->idleAnimation)
	, walkAnimation(Steve::Data_->walkAnimation)
	, runAnimation(Steve::Data_->runAnimation)
	, DeadAnimation(Steve::Data_->DeadAnimation)
	, kickAnimation(Steve::Data_->kickAnimation)
{
	m_animator = std::make_unique<Animator>(Steve::Data_->idleAnimation_NOGUN);

	//std::map<std::string, BoneInfo>::iterator it;
	//auto boneInfoMap = m_animator->m_CurrentAnimation->GetBoneIDMap();
	//for (it = boneInfoMap.begin(); it != boneInfoMap.end(); it++) {
	//	cout << it->first
	//		<< " " << it->second.id << std::endl;
	//}
}

void Player::Update()
{

	if (Dead) {
		DeadTimer -= Time.Deltatime;
		if (DeadTimer < 0) {
			GameObject->Destroy = true;
		}
	}
	else {
		Animate();
		Update_Behavior();
	}
	m_animator->UpdateAnimation(Time.Deltatime);

}


int BoneIdx = MixamoBone_RightHand;
void Player::Render(Renderer& renderer)
{


	vector<glm::mat4> transforms = m_animator->GetFinalBoneMatrices();
	if (HasGun) {
		Shader& shader2 = renderer.m_basicShader;
		shader2.use();

		glm::mat4 mm_Parent = BODY->Transform.modelMatrix;
		glm::mat4 mm_Child = Gun_OBJ->Transform.modelMatrix;
		glm::mat4 T_asLocal = transforms[BoneIdx];          
		glm::mat4 T_asWorld = mm_Parent * T_asLocal * glm::inverse(mm_Parent);
		Gun_Matrix = T_asWorld * mm_Child;
		renderer.DrawModel(*Steve::Data_->Gun_Model, Gun_Matrix);
	}

	//renderer.DrawAnimation(m_model, GameObject->Transform.wPosition, BODY->Transform.wScale, BODY->Transform.wRotation, m_animator->GetFinalBoneMatrices());
	renderer.DrawAnimation(m_model, BODY->Transform.modelMatrix, m_animator->GetFinalBoneMatrices());
}















static bool isNearMe2D(const glm::vec3& Subject, const glm::vec3& Me, float Range)
{
	return (Subject.x > Me.x - Range && Subject.x < Me.x + Range) &&
		(Subject.z > Me.z - Range && Subject.z < Me.z + Range);
}
vector<B_Triangle> Dust2_Triangles;


void PLR_Raycast_Init(GameObj* RayGameObject, Model mModel) {

	///////////////////////////
	BanKEngine::All_Update();

	//Get ONCE cus the map is STATIC
	glm::vec3 Vert0;
	glm::vec3 Vert1;
	glm::vec3 Vert2;

	Dust2_Triangles.clear();
	const glm::mat4& modelMatrix = RayGameObject->Transform.modelMatrix;

	for (Mesh& m : mModel.meshes) {
		for (unsigned int i = 0; i < m.indices.size(); i += 3) {
			B_Triangle NewTriangle;

			Vert0 = getWorldPosition(m.vertices[m.indices[i]].Position, modelMatrix);
			Vert1 = getWorldPosition(m.vertices[m.indices[i + 1]].Position, modelMatrix);
			Vert2 = getWorldPosition(m.vertices[m.indices[i + 2]].Position, modelMatrix);

			NewTriangle.Vert0 = Vert0;
			NewTriangle.Vert1 = Vert1;
			NewTriangle.Vert2 = Vert2;
			Dust2_Triangles.push_back(NewTriangle);
		}
	}

}



bool inFloatRange(float value) {
	// Check if the value is between the threshold where scientific notation is typically used
	return (std::abs(value) >= 1e-4f && std::abs(value) <= 1e6f);
}

float TargetY_FB = 0;
float TargetY_LR = 0;

void PLR_Raycast_Update(GameObj* PLR_OBJ, Player* PLRbhav) {





	vector<B_Triangle> Triangle4Cast;
	for (const B_Triangle& trian : Dust2_Triangles) {
		if (isNearMe2D(trian.Vert0, PLR_OBJ->Transform.wPosition, 50)) {
			Triangle4Cast.push_back(trian);
		}
	}
	bool HIT = false;


	glm::vec3 Vec3_Up = PLRbhav->BODY_RotProbe->Transform.getUpVector();
	glm::vec3 Vec3_Down = -Vec3_Up;
	glm::vec3 Vec3_Fwd = PLRbhav->BODY_RotProbe->Transform.getForwardVector();
	glm::vec3 Vec3_Back = -Vec3_Fwd;
	glm::vec3 Vec3_LFT = PLRbhav->BODY_RotProbe->Transform.getLeftVector();
	glm::vec3 Vec3_RHT = -Vec3_LFT;


	B_Ray RayDOWN;
	glm::vec3 RayDOWN_Hitpoint;
	RayDOWN.Origin = PLRbhav->BODY_RotProbe->Transform.getWorldPosition();
	RayDOWN.Origin.y += 0.5;
	RayDOWN.Direction = Vec3_Down;
	if (RayIntersectSceneOptimized(RayDOWN, Triangle4Cast, RayDOWN_Hitpoint))
		HIT = true;

	B_Ray RayFRONT_F;
	glm::vec3 RayFRONT_F_Hitpoint;
	RayFRONT_F.Origin = PLRbhav->BODY_RotProbe->Transform.getWorldPosition();
	RayFRONT_F.Origin.y += 0.5;
	RayFRONT_F.Direction = Vec3_Fwd;
	if (RayIntersectSceneOptimized(RayFRONT_F, Triangle4Cast, RayFRONT_F_Hitpoint))
		HIT = true;

	B_Ray RayFRONT_R;
	glm::vec3 RayFRONT_R_Hitpoint;
	RayFRONT_R.Origin = PLRbhav->BODY_RotProbe->Transform.getWorldPosition();
	RayFRONT_R.Origin.y += 0.5;
	RayFRONT_R.Direction = (Vec3_RHT);
	if (RayIntersectSceneOptimized(RayFRONT_R, Triangle4Cast, RayFRONT_R_Hitpoint))
		HIT = true;

	B_Ray RayFRONT_L;
	glm::vec3 RayFRONT_L_Hitpoint;
	RayFRONT_L.Origin = PLRbhav->BODY_RotProbe->Transform.getWorldPosition();
	RayFRONT_L.Origin.y += 0.5;
	RayFRONT_L.Direction = (Vec3_LFT);
	if (RayIntersectSceneOptimized(RayFRONT_L, Triangle4Cast, RayFRONT_L_Hitpoint))
		HIT = true;


	//Hitlocation->Transform.wPosition = rayBack_Hitpoint;
	//HitlocationL->Transform.wPosition = RayDOWN_Hitpoint;
	if (DoRaycast && HIT) {

		Safe = false;
		float TargetY = 0;
		TargetY += RayDOWN_Hitpoint.y;
		PLR_OBJ->Transform.wPosition.y = B_lerp(PLR_OBJ->Transform.wPosition.y, TargetY, 1);

		float Range = 0.8;
		float Alpha = 0.08f;
		if (glm::distance(RayFRONT_F.Origin, RayFRONT_F_Hitpoint) < Range) {
			PLR_OBJ->Transform.wPosition -= RayFRONT_F.Direction * Alpha;
			PLRbhav->Velocity = B_lerpVec3(PLRbhav->Velocity, PLRbhav->Velocity * 0.1f, Time.Deltatime * 64);
		}
		if (glm::distance(RayFRONT_R.Origin, RayFRONT_R_Hitpoint) < Range) {
			PLR_OBJ->Transform.wPosition -= RayFRONT_R.Direction * Alpha;
			PLRbhav->Velocity = B_lerpVec3(PLRbhav->Velocity, PLRbhav->Velocity * 0.1f, Time.Deltatime * 64);
		}
		if (glm::distance(RayFRONT_L.Origin, RayFRONT_L_Hitpoint) < Range) {
			PLR_OBJ->Transform.wPosition -= RayFRONT_L.Direction * Alpha;
			PLRbhav->Velocity = B_lerpVec3(PLRbhav->Velocity, PLRbhav->Velocity * 0.1f, Time.Deltatime * 64);
		}
	}




}
