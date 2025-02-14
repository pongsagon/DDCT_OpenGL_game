#pragma once
#include "_Def1.h"
//BanKEngine Default Components


glm::vec3 getWorldPosition(glm::vec3 Position,glm::mat4 ParentModelMatrix)  
{
	return glm::vec3(
		ParentModelMatrix[0][0] * Position.x + ParentModelMatrix[1][0] * Position.y + ParentModelMatrix[2][0] * Position.z + ParentModelMatrix[3][0],
		ParentModelMatrix[0][1] * Position.x + ParentModelMatrix[1][1] * Position.y + ParentModelMatrix[2][1] * Position.z + ParentModelMatrix[3][1],
		ParentModelMatrix[0][2] * Position.x + ParentModelMatrix[1][2] * Position.y + ParentModelMatrix[2][2] * Position.z + ParentModelMatrix[3][2]
	);
		//// Convert local position to homogeneous coordinates
		//glm::vec4 localPosition = glm::vec4(Position, 1.0f);

		//// Calculate world position 
		//glm::vec4 worldPosition = ParentModelMatrix * localPosition;

		//// Return the 3D world position (ignore the w component)
		//return glm::vec3(worldPosition);
	
}

glm::vec3 getWorldRotation(glm::mat4 ParentModelMatrix)
{
	glm::quat rotationQuat = glm::quat_cast(ParentModelMatrix);
	glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
	return eulerAngles;
}


class GameObj;
class Renderer;

class BanKBehavior;
vector<BanKBehavior*> sBanKBehavior;
class BanKBehavior {
	bool didTrue_Init = false;
	bool didTrue_Start = false;
public:

	bool Destroy = false;

	string SerialID = "Unknown";
	GameObj* GameObject;
	BanKBehavior() {
		//cout << "\tBanKBehavior";
		sBanKBehavior.push_back(this);
	}

	void True_Init() {
		if (!didTrue_Init) {
			didTrue_Init = true;
			Init();
		}
	}
	void True_Start() {
		if (!didTrue_Start) {
			didTrue_Start = true;
			Start();
		}
	}

	virtual void Init() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void Destruct() {}//for components with Scene Containers

};






















class Transform;
vector<Transform*> sTransforms;//Dont Auto Update
class Transform : public BanKBehavior {
	const glm::mat4 mat4one = glm::mat4(1.0f);
public:
	Transform* Parent;

	Transform() {
		SerialID = "Transform";
		sTransforms.push_back(this);
	}

	glm::vec3 wPosition = glm::vec3(0,0,0 );
	glm::vec3 wRotation = glm::vec3(0, 0.1, 0);
	glm::vec3 wScale    = glm::vec3(1, 1, 1);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	glm::vec3 getForwardVector() {
		glm::vec3 forward = glm::vec3(modelMatrix[2]);
		forward = glm::normalize(forward);

		return forward;
	}
	glm::vec3 getUpVector() {
		glm::vec3 up = glm::vec3(modelMatrix[1]);
		up = glm::normalize(up);

		return up;
	}
	glm::vec3 getLeftVector() {
		glm::vec3 right = glm::vec3(modelMatrix[0]);
		right = glm::normalize(right);

		return right;
	}


	//Point PositionOBJ2WLD() {
	//	return Point(AffineMatrix[0].w, AffineMatrix[1].w, AffineMatrix[2].w);
	//}

	//Vector getForwardVector() {
	//	return Vector(										
	//		AffineMatrix.row[0].z,		
	//		AffineMatrix.row[1].z,		
	//		AffineMatrix.row[2].z		
	//	);
	//}
	void modelMatrix_Update3D()
	{

		//Quaternion/////////////////////////////
		if (Parent) { 

			modelMatrix = Parent->modelMatrix * glm::translate(mat4one, wPosition);
			modelMatrix *= glm::mat4_cast(glm::quat(glm::vec3(glm::radians(wRotation.x), glm::radians(wRotation.y), glm::radians(wRotation.z))));  // Use mat4_cast to convert quaternion to matrix;
			modelMatrix = glm::scale(modelMatrix, wScale);
		}
		else
		{
			modelMatrix = glm::translate(mat4one, wPosition); 
			modelMatrix *= glm::mat4_cast(glm::quat(glm::vec3(glm::radians(wRotation.x), glm::radians(wRotation.y), glm::radians(wRotation.z))));
			modelMatrix = glm::scale(modelMatrix, wScale);
		}


		//Quaternion/////////////////////////////
		//if (Parent) {
		//	glm::mat4 parentMatrix = Parent->modelMatrix;

		//	// Convert Euler angles to quaternion and then to a rotation matrix
		//	glm::quat rotationQuat = glm::quat(glm::vec3(glm::radians(wRotation.x), glm::radians(wRotation.y), glm::radians(wRotation.z)));
		//	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);  // Use mat4_cast to convert quaternion to matrix

		//	// Apply transformations
		//	modelMatrix = parentMatrix * glm::translate(mat4one, wPosition);
		//	modelMatrix *= rotationMatrix;
		//	modelMatrix = glm::scale(modelMatrix, wScale);
		//}
		//else
		//{
		//	// Convert Euler angles to quaternion and then to a rotation matrix
		//	glm::quat rotationQuat = glm::quat(glm::vec3(glm::radians(wRotation.x), glm::radians(wRotation.y), glm::radians(wRotation.z)));
		//	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);  // Use mat4_cast to convert quaternion to matrix

		//	// Apply transformations
		//	modelMatrix = glm::translate(mat4one, wPosition);
		//	modelMatrix *= rotationMatrix;
		//	modelMatrix = glm::scale(modelMatrix, wScale);
		//}


		//Euler///////////////////////////// 
		//if (Parent) {
		//	modelMatrix = glm::translate(Parent->modelMatrix, wPosition);
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.x, glm::vec3(1, 0, 0));
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.y, glm::vec3(0, 1, 0));
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.z, glm::vec3(0, 0, 1));
		//	modelMatrix = glm::scale(modelMatrix, wScale);
		//}
		//else
		//{
		//	modelMatrix = glm::translate(mat4one, wPosition);
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.x, glm::vec3(1, 0, 0));
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.y, glm::vec3(0, 1, 0));
		//	modelMatrix = glm::rotate(modelMatrix, wRotation.z, glm::vec3(0, 0, 1));
		//	modelMatrix = glm::scale(modelMatrix, wScale);
		//}
	}


	glm::vec3 getWorldPosition() const {
		if (Parent) {
			// Convert local position to homogeneous coordinates
			glm::vec4 localPosition = glm::vec4(wPosition, 1.0f);

			// Get parent's model matrix
			glm::mat4 parentMatrix = Parent->modelMatrix;

			// Calculate world position
			glm::vec4 worldPosition = parentMatrix * localPosition;

			// Return the 3D world position (ignore the w component)
			return glm::vec3(worldPosition);
		}
		else {
			// No parent, world position is the same as local position
			return wPosition;
		}
	}
	glm::vec3 getWorldRotation() const {
		glm::quat worldRotationQuat = glm::quat(glm::vec3(glm::radians(wRotation.x), glm::radians(wRotation.y), glm::radians(wRotation.z)));

		if (Parent) {
			// Multiply the parent’s rotation quaternion with the local rotation quaternion
			glm::quat parentRotationQuat = glm::quat_cast(Parent->modelMatrix);
			worldRotationQuat = parentRotationQuat * worldRotationQuat;
		}

		// Convert quaternion to Euler angles in degrees
		glm::vec3 worldRotation = glm::degrees(glm::eulerAngles(worldRotationQuat));
		return worldRotation;
	}





	void modelMatrix_Update2D()
	{
		modelMatrix = glm::translate(mat4one, wPosition);
		modelMatrix = glm::scale(modelMatrix, wScale);
	}

	void Destruct() {
		for (int i = sTransforms.size() - 1; i >= 0; i--) {
			if (sTransforms[i] == this) {
				sTransforms[i] = nullptr;
				sTransforms.erase(sTransforms.begin() + i);
				break;
			}
		}
	}
};

class Renderer;
									vector<GameObj*> sGameObjs;
									queue <GameObj*> sGameObjsAwait;  // Declare a q sGameObjsAwait;
									class GameObj {

									#define MaxComponent 32

									public:
										struct Try { 
											Renderer* Renderer;
										}Try;
										Transform Transform;

										bool Destroy = false;


										/// Relation  ///////////////////
														GameObj* CreateChild() {
															GameObj* NewOBJ = new GameObj; 
															NewOBJ->Transform.Parent = &Transform;

															NewOBJ->True_Start();
															return NewOBJ;
														}

										/// Comps ///////////////////
														vector<BanKBehavior*> MyComponents;
														template<typename T>
														T* AddComponent(T* comp)
														{
															MyComponents.reserve(MaxComponent);
															comp->GameObject = this;
															comp->True_Init();

															MyComponents.push_back(comp);
															return comp;
														}
														template<typename T>
														T* GetComponent(T Class) {
															for (BanKBehavior* component : MyComponents) {
																if (dynamic_cast<T*>(component)) { return dynamic_cast<T*>(component); }
															}
															return nullptr;
														}
														void ClearComponents() {
															for (BanKBehavior* Each : MyComponents) {
																Each->Destruct();
															}
															MyComponents.clear();
														}

										/// Behaviors ///////////////////
														void True_Init() {
															for (BanKBehavior* Each : MyComponents) {
																Each->True_Init();
															}
														}
														void True_Start() {
															for (BanKBehavior* Each : MyComponents) {
																Each->True_Start();
															}
														}
														void Update() {
															for (BanKBehavior* Each : MyComponents) {
																Each->Update();
															}
														}
														void LateUpdate() {
															for (BanKBehavior* Each : MyComponents) {
																Each->LateUpdate();
															}
														}




										/// Instancing ///////////////////
												GameObj() {
													sGameObjsAwait.push(this);
												}
												static GameObj* Create() {
													GameObj* NewOBJ = new GameObj;
													NewOBJ->True_Start();
													return NewOBJ;
												}
												static void DestroyObj(GameObj* Target) {
													for (size_t i = 0; i < sGameObjs.size(); ++i) {
														if (sGameObjs[i] == Target) {
															sGameObjs[i]->ClearComponents();
															sGameObjs.erase(sGameObjs.begin() + i);
															break;
														}
													}
												}
									};
									GameObj* Edit_Obj;



