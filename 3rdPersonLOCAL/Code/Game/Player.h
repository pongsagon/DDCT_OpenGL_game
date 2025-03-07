#pragma once

#include "GameObject.h"
#include <learnopengl/model_animation.h>
#include <learnopengl/animator.h>
#include "Gun.h"
#include "../Input.h"
#include "Bullet.h"
#include "../Math.h"
#include "../Physics.h"
#include "ThirdPersonCamera.h"

class Player : public GameObject
{
public:
	struct Controls
	{
		bool MOVE_FWD = false;
		bool MOVE_BACK = false;
		bool MOVE_LFT = false;
		bool MOVE_RHT = false;
		bool ATK_1 = false;
		float TURN_X = 0;
		float TURN_Y = 0;

		void Update()
		{
			MOVE_FWD = Input::GetKey(GLFW_KEY_W);
			MOVE_BACK = Input::GetKey(GLFW_KEY_S);
			MOVE_LFT = Input::GetKey(GLFW_KEY_A);
			MOVE_RHT = Input::GetKey(GLFW_KEY_D);
			ATK_1 = Input::GetKey(GLFW_KEY_N) || Input::GetMouseButton(GLFW_MOUSE_BUTTON_1);

			//pair Mouse = Input::getMousePosChange();
			//TURN_Y = Mouse.first;
			//TURN_X = Mouse.second;
		}
	} Controls;

	Player()
		: m_model("Assets/Models/mixamo/steve.dae")
		, idleAnimation("Assets/Models/mixamo/Rifle Aiming Idle.dae", &m_model)
		, idleAnimation_NOGUN("Assets/Models/mixamo/idle.dae", &m_model)
		, walkAnimation("Assets/Models/mixamo/walk.dae", &m_model)
		, runAnimation("Assets/Models/mixamo/run.dae", &m_model)
		, deadAnimation("Assets/Models/mixamo/Dying.dae", &m_model)
		, kickAnimation("Assets/Models/mixamo/kick.dae", &m_model)
		, hitAnimation("Assets/Models/mixamo/Hit Reaction.dae", &m_model)
	{
		m_animator = std::make_unique<Animator>(&idleAnimation);
		m_capsuleCollider = std::make_unique<CapsuleCollider>();
		m_scale = glm::vec3(1.0f);

		m_gun.m_position = glm::vec3(-5, 7, 0) * 20.0f;
		m_gun.m_rotation = glm::vec3(glm::pi<float>() / -2.0f, glm::pi<float>(), 0);
		m_gun.m_scale = glm::vec3(3.0f);
	}

	ThirdPersonCamera* m_thirdPersonCamera = nullptr;
	int Health = 3;
	//GameObj* Gun_OBJ;
	glm::mat4 Gun_Matrix;
	const float Gun_AnimCooldownMax = 0.25f;
	float Gun_AnimCooldown = 0;
	const float Gun_CooldownMax = 0.1f;
	float Gun_Cooldown = 0;
	bool HasGun = false;


	std::unique_ptr<CapsuleCollider> m_capsuleCollider = nullptr;

	bool m_input = false;

	GameObject* CamArea;
	GameObject* CamSocket;
	GameObject* CamLookat;
	GameObject* BODY_RotProbe;
	float BODY_RotProbe_TargetRot;
	GameObject* BODY;
	float BODY_TargetRot;


	std::vector<std::unique_ptr<Bullet>> m_shotBullets;


	float yRotation = 30.0f;

	float acc = 18.0f;

	glm::vec3 Velocity;

	bool Dead = false;
	float DeadTimer = 3;

	Gun m_gun;


	glm::vec3 getDirectPosition(const glm::mat4& ModelMatrix) {
		return glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
	}


	void Update_Behavior(float dt)
	{
		Controls.Update();

		m_input = false;

		Gun_Cooldown += dt;
		Gun_AnimCooldown -= dt;


		float targetYRotation = yRotation;

		if (m_thirdPersonCamera)
			targetYRotation = m_thirdPersonCamera->yRotation;



		float rightAngle = targetYRotation - (glm::pi<float>() / 2.0f);
		float leftAngle = targetYRotation + (glm::pi<float>() / 2.0f);

		glm::vec3 forward{ glm::sin(targetYRotation), 0, glm::cos(targetYRotation) };
		glm::vec3 right{ glm::sin(rightAngle), 0, glm::cos(rightAngle) };
		glm::vec3 left{ glm::sin(leftAngle), 0, glm::cos(leftAngle) };

		glm::vec3 movingVector{ 0.0f };

		if (Controls.ATK_1 && HasGun)
		{
			//BODY_RotProbe_TargetRot = CamArea->m_rotation.y;
			BODY_TargetRot = 0;
			Gun_AnimCooldown = Gun_AnimCooldownMax;

			if (Gun_Cooldown > Gun_CooldownMax)
			{
				Gun_Cooldown = 0;

				//BulletOBJ->m_position = getDirectPosition(Gun_Matrix);
				//BulletOBJ->Transform.LookAt(CamLookat->Transform.getWorldPosition());
				//BulletOBJ->AddComponent(new Bullet(Steve::Data_->Bullet_Model));

				m_shotBullets.emplace_back(new Bullet(m_position, forward));

				//CamArea->m_rotation.x -= B_irand(-2, 5);
				//CamArea->m_rotation.y += B_irand(-5, 5);
			}

		}
		else if (Gun_AnimCooldown <= 0) {


			if (Controls.MOVE_FWD) {
				m_input = true;
				//Velocity += glm::vec3(0, 0, 1) * Accel;
				//Velocity += forward * Accel;

				movingVector += forward;
				m_rotation.y = Math::Lerp(m_rotation.y, targetYRotation, 10.0f * dt);
				//BODY_TargetRot = B_lerp(BODY_TargetRot, 0, dt * 9);
			}
			else if (Controls.MOVE_BACK)
			{
				m_input = true;
				movingVector -= forward;

				//Velocity -= forward * Accel;
				//BODY_TargetRot = B_lerp(BODY_TargetRot, 180, dt * 9);
			}

			if (Controls.MOVE_LFT) {
				m_input = true;
				//Velocity += left * Accel;
				movingVector += left;

				m_rotation.y = Math::Lerp(m_rotation.y, leftAngle, 10.0f * dt);
				//BODY_TargetRot = B_lerp(BODY_TargetRot, 90, dt * 9);
			}
			else if (Controls.MOVE_RHT)
			{
				m_input = true;
				movingVector += right;

				m_rotation.y = Math::Lerp(m_rotation.y, rightAngle, 10.0f * dt);
				//Velocity += right * Accel;

				if (Controls.MOVE_BACK)
				{
					//BODY_TargetRot = B_lerp(BODY_TargetRot, 270, dt * 9);
				}
				else
				{
					//BODY_TargetRot = B_lerp(BODY_TargetRot, -90, dt * 9);
				}
			}
		}


		glm::vec3 norm = glm::length(movingVector) > 0.0f ? glm::normalize(movingVector) : movingVector;

		Velocity += norm * acc * dt;
		Velocity = Math::Lerp(Velocity, glm::vec3{ 0.0f }, dt * 5.0f);

		m_position += Velocity * dt;

		if (m_capsuleCollider->Event.isCollided)
		{
			Bullet* GetBull = nullptr;
			//GetBull = m_capsuleCollider->Event.Other->GameObject->GetComponent(GetBull);

			if (GetBull)
			{
				/*
				if (GetBull->Team == Bullet::Enemy) {
					m_animator->PlayAnimation(Steve::Data_->HitAnimation, NULL, 0.1, 0.0f, 0.0f);
					GetBull->GameObject->Destroy = true;

					Health--;
					if (Health <= 0) {
						Dead = true;
						m_animator->PlayAnimation(DeadAnimation, NULL, 0.0f, 0.0f, 0.0f);
					}
				}
				*/
			}
			else
			{
				HasGun = true;
				/*
				Gun* GetGun = nullptr;
				GetGun = m_capsuleCollider->Event.Other->GameObject->GetComponent(GetGun);
				if (GetGun) {
					GetGun->GameObject->Destroy = true;
					HasGun = true;
				}
				*/
			}
		}


		if (Input::GetKey(GLFW_KEY_F)) {
			Dead = true;
			m_animator->PlayAnimation(&deadAnimation, NULL, 0.0f, 0.0f, 0.0f);
		}


		for (size_t i = 0; i < m_shotBullets.size();) {
			if (m_shotBullets[i]->m_lifeTimer >= Bullet::s_lifeDuration) {
				m_shotBullets.erase(m_shotBullets.begin() + i);  // Erase bullet
			}
			else {
				m_shotBullets[i]->Update(dt);
				++i;  // Only increment if not erased
			}
		}
	}


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


	void Animate(float dt)
	{

		if (HasGun) {
			//idleAnimation = Steve::Data_->idleAnimation;
			//runAnimation = Steve::Data_->runAnimation;
		}
		else
		{
			//idleAnimation = Steve::Data_->idleAnimation_NOGUN;
			//runAnimation = Steve::Data_->runAnimation_NOGUN;
		}

		float blendRate = dt * 4;
		switch (charState) {
		case IDLE:
			if (m_input) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(&idleAnimation, &walkAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_WALK;
			}
			else if (Input::GetKey(GLFW_KEY_J)) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(&idleAnimation, &deadAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_PUNCH;
			}
			else if (Input::GetKey(GLFW_KEY_K)) {
				blendAmount = 0.0f;
				m_animator->PlayAnimation(&idleAnimation, &kickAnimation, m_animator->m_CurrentTime, 0.0f, blendAmount);
				charState = IDLE_KICK;
			}
			break;
		case IDLE_WALK:
			blendAmount += blendRate;
			m_animator->PlayAnimation(&idleAnimation, &runAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(&runAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = WALK;
			}
			break;
		case WALK:
			m_animator->PlayAnimation(&runAnimation, NULL, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (!m_input) {
				charState = WALK_IDLE;
			}
			break;
		case WALK_IDLE:
			blendAmount += blendRate;
			m_animator->PlayAnimation(&runAnimation, &idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = IDLE;
			}
			break;
		case IDLE_PUNCH:
			blendAmount += blendRate;
			m_animator->PlayAnimation(&idleAnimation, &deadAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(&deadAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = PUNCH_IDLE;
			}
			break;
		case PUNCH_IDLE:
			if (m_animator->m_CurrentTime > 0.7f) {
				blendAmount += blendRate;
				m_animator->PlayAnimation(&deadAnimation, &idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = m_animator->m_CurrentTime2;
					m_animator->PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = IDLE;
				}
			}
			else {
				// punching

			}
			break;
		case IDLE_KICK:
			blendAmount += blendRate;
			m_animator->PlayAnimation(&idleAnimation, &kickAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = m_animator->m_CurrentTime2;
				m_animator->PlayAnimation(&kickAnimation, NULL, startTime, 0.0f, blendAmount);
				charState = KICK_IDLE;
			}
			break;
		case KICK_IDLE:
			if (m_animator->m_CurrentTime > 1.0f) {
				blendAmount += blendRate;
				m_animator->PlayAnimation(&kickAnimation, &idleAnimation, m_animator->m_CurrentTime, m_animator->m_CurrentTime2, blendAmount);
				if (blendAmount > 0.9f) {
					blendAmount = 0.0f;
					float startTime = m_animator->m_CurrentTime2;
					m_animator->PlayAnimation(&idleAnimation, NULL, startTime, 0.0f, blendAmount);
					charState = IDLE;
				}
			}
			else {
				// punching
			}
			break;
		}
	}

	virtual void Update(float dt) override
	{
		Update_Behavior(dt);
		Animate(dt);

		m_animator->UpdateAnimation(dt);

		m_capsuleCollider->m_position = m_position;
	}

	int rightHandBoneId = MixamoBone_RightHand;

	/*
	virtual void Render(Renderer& renderer) override
	{
		Shader& shader = renderer.m_animShader;
		shader.use();


		glm::mat4 model = GetModelMatrix();
		shader.setMat4("model", model);

		vector<glm::mat4> transforms = m_animator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		m_model.Draw(shader);




		//Gun_OBJ->Transform.wPosition = glm::vec3(-5, 7, 0) * 20.0f;
		//Gun_OBJ->Transform.wRotation = glm::vec3(-90, 180, 0);
		//Gun_OBJ->Transform.wScale = glm::vec3(3);

		if (HasGun)
		{
			Shader& shader2 = renderer.m_basicShader;
			shader2.use();

			glm::mat4 mm_Parent = model;
			glm::mat4 mm_Child = model * m_gun.GetModelMatrix();
			glm::mat4 T_asLocal = transforms[rightHandBoneId];
			glm::mat4 T_asWorld = mm_Parent * T_asLocal * glm::inverse(mm_Parent);
			Gun_Matrix = T_asWorld * mm_Child;

			glm::mat4 gunModelMatrix = model * transforms[rightHandBoneId];

			shader2.setMat4("model", Gun_Matrix);

			Model* model = ResourceManager::Get()->GetModel("gun");

			if (model)
				model->Draw(shader2);
		}


		for (size_t i = 0; i < m_shotBullets.size(); i++)
		{
			m_shotBullets[i]->Render(renderer);
		}

	}
	*/

	virtual void Render(Renderer& renderer) override
	{
		renderer.DrawAnimation(m_model, m_position, m_scale, m_rotation, m_animator->GetFinalBoneMatrices());
	}
	
	Model m_model;
	std::unique_ptr<Animator> m_animator;

	Animation idleAnimation;
	Animation idleAnimation_NOGUN;
	Animation walkAnimation;
	Animation runAnimation;
	Animation runAnimation_NOGUN;
	Animation deadAnimation;
	Animation kickAnimation;
	Animation hitAnimation;
};