#pragma once

#include "BanKEngine.h"
#include "Input.h"
#include "B_Player.h"

#include <GLFW/glfw3.h>

#include <learnopengl/shader.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>



namespace Doozy {
	class Doozy {
		public:

			Animation* idleAnimation;
			Animation* walkAnimation;
			Animation* runAnimation;
			Animation* punchAnimation;
			Animation* KnockAnimation;
			Model m_model;

			Model* Bullet_Model;

			Doozy()
				: m_model("Assets/Models/mixamo/doozy/doozy.dae")
			{
				idleAnimation = new Animation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model);
				walkAnimation = new Animation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model);
				runAnimation = new Animation("Assets/Models/mixamo/doozy/Run.dae", &m_model);
				punchAnimation = new Animation("Assets/Models/mixamo/doozy/Fight Idle.dae", &m_model);
				KnockAnimation = new Animation("Assets/Models/mixamo/doozy/Slipping.dae", &m_model);

				Bullet_Model = new Model("Assets/Models/Bullets/Bullets.obj");
			}

	}*Data_;

	void Load() {
		Data_ = new Doozy;
	}

}

int EnemyCount = 0;
float EnemyMax = 1;
Player* TargetPLR;
class Enemy : public BanKBehavior
{
public:

	struct Controls {
		bool MOVE_FWD = false;
		bool MOVE_BACK = false;
		bool MOVE_LFT = false;
		bool MOVE_RHT = false;
		bool ATK_1 = false;
		float TURN_X = 0;
		float TURN_Y = 0;
	}Controls;

	Enemy();   
	void Update();
	void Render(Shader& shader);

	GameObj* Gun_OBJ;
	Collider_Capsule* mCollider_Capsule;
	int BoneIdx = MixamoBone_LeftHand;

	GameObj* BODY;
	void Init() {
		EnemyCount++;

			BODY = GameObject;
				Gun_OBJ = BODY->CreateChild();
				Gun_OBJ->Transform.wPosition = glm::vec3(4, 6, 0) * 20.0f;
				Gun_OBJ->Transform.wRotation = glm::vec3(0, 90, 0);
				Gun_OBJ->Transform.wScale = glm::vec3(20); 

				mCollider_Capsule = GameObject->AddComponent(new Collider_Capsule);
	}

	void Start() {
	}

	void Destruct() {
		EnemyCount--;
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
	float blendRate = 0.055f;

	Model* m_model;
	std::unique_ptr<Animator> m_animator;

	Animation* idleAnimation;
	Animation* walkAnimation;
	Animation* runAnimation;
	Animation* punchAnimation;
	Animation* KnockAnimation;

	glm::vec3 Velocity;

	glm::vec3 TargetPos;
	float FollowSpeed = 10;
	float Speed = 5;



	float DeathTimer = 1;
	bool DeathTimerStart = false;
	void Update_Behavior() {
		if (Time.Deltatime > 0.1) { return; }
		

		if (DeathTimerStart) {
			DeathTimer -= Time.Deltatime;
			if (DeathTimer < 0) {
				GameObject->Destroy = true;
			}
		}
		else
		{
			if (TargetPLR) {
				TargetPos = B_lerpVec3(TargetPos, TargetPLR->GameObject->Transform.wPosition, Time.Deltatime * FollowSpeed);
				GameObject->Transform.LookAt(TargetPos);

				if (glm::distance(TargetPos, GameObject->Transform.wPosition) > 6) {
					GameObject->Transform.wPosition += GameObject->Transform.getForwardVector() * Time.Deltatime * Speed;
					Anim_TransferTo(runAnimation);
				}
				else if (glm::distance(TargetPos, GameObject->Transform.wPosition) > 1) {
					Shoot();
					Anim_TransferTo(idleAnimation);
				}
			}

			if (mCollider_Capsule->Event.isCollided) {
				Bullet* GetBullet = nullptr;
				GetBullet = mCollider_Capsule->Event.Other->GameObject->GetComponent(GetBullet);
				if (GetBullet && GetBullet->Team == Bullet::Player) {
					//GameObj* newEnemy = GameObj::Create();
					//newEnemy->Transform.wPosition = GameObject->Transform.wPosition + glm::vec3(1, 0, 0);
					//newEnemy->AddComponent(new Enemy);

					DeathTimerStart = true; 
					m_animator->PlayAnimation(KnockAnimation, NULL, 0.2, 0.0f, 0.0f);

					GetBullet->GameObject->Destroy = true;
				}
			}

		}


		if (GameObject->Transform.wPosition.y < 0) { GameObject->Transform.wPosition.y = 0; }
	}


	float ShootCooldown = 2;
	float ShootTimer = 0;
	void Shoot() {

		if (ShootTimer > ShootCooldown) {
			ShootTimer = 0;
			GameObj* BulletOBJ = GameObj::Create();
			BulletOBJ->Transform.wPosition = GameObject->Transform.wPosition;

			BulletOBJ->Transform.wRotation = GameObject->Transform.wRotation;
			Bullet* newBull = BulletOBJ->AddComponent(new Bullet(Doozy::Data_->Bullet_Model));
			newBull->Team = Bullet::Enemy;
			newBull->Speed *= 0.5f;
		}
		else
		{
			ShootTimer += Time.Deltatime;
		}
	}



	void Render(Renderer& renderer)
	{

		//Shader& shader = renderer.m_animShader;
		//shader.use();

		//vector<glm::mat4> transforms = m_animator->GetFinalBoneMatrices();
		//for (int i = 0; i < transforms.size(); ++i)
		//	shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


		//shader.setMat4("model", BODY->Transform.modelMatrix);
		//m_model->Draw(shader); 

		renderer.DrawAnimation(*m_model, BODY->Transform.modelMatrix, m_animator->GetFinalBoneMatrices());

	}


	Animation* Anim_Current;
	Animation* Anim_Dest;
	bool Anim_isTransfering = false;
	float Anim_Alpha = 0;
	void Anim_TransferTo( Animation* Anim_Target) {


			//if (Anim_Target != Anim_Current) {
			//	Anim_Current = Anim_Target;
			//	m_animator->PlayAnimation(Anim_Target, NULL, 0.2, 0.0f, 0.0f);
			//}
		
		 
		if (Anim_isTransfering) {

			if (Anim_Alpha >= 1) {//Transfer success
				Anim_isTransfering = false;
				Anim_Current = Anim_Dest;
				m_animator->PlayAnimation(Anim_Current, Anim_Dest, 0.2, 0.2, 1);
				Anim_Alpha = 0;
			}
			else {
				float Anim_TransferRate = 8;
				Anim_Alpha += Time.Deltatime * Anim_TransferRate;
				m_animator->PlayAnimation(Anim_Current, Anim_Dest, 0.2, 0.2, Anim_Alpha);

			}
		}
		else
		{
			if (Anim_Current) {
				if (Anim_Target != Anim_Current) {
					Anim_Dest = Anim_Target;
					Anim_isTransfering = true;
				}
			}
			else {
				Anim_Current = Anim_Target;
				m_animator->PlayAnimation(Anim_Target, NULL, 0.2, 0.0f, 0.0f);
			}
		}

	}


};




 
Enemy::Enemy()
	: m_model(&Doozy::Data_->m_model)
	, idleAnimation(Doozy::Data_->idleAnimation)
	, walkAnimation(Doozy::Data_->walkAnimation)
	, runAnimation(Doozy::Data_->runAnimation)
	, punchAnimation(Doozy::Data_->punchAnimation)
	, KnockAnimation(Doozy::Data_->KnockAnimation)
{
	m_animator = std::make_unique<Animator>(walkAnimation);
}

void Enemy::Update()
{

	if (TargetPLR) {
		Update_Behavior();
	}


	m_animator->UpdateAnimation(Time.Deltatime);


}



