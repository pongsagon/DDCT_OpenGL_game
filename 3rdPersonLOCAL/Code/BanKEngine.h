#pragma once

#include "Internal/_Final.h"


const float Migrate_Scale = 0.007f;

namespace BanKEngine {

	//glDisable(GL_DEPTH_TEST); for 2D
	void Init() { 
		sGameObjs.reserve(999);
	/*	BanKEngine::GlfwGlad::Init();
		B_Textures::Init();
		B_Shaders::Init();



		switch (MeshInit_) {
				case MeshInit::x5 :
					B_Meshes5x::Init();
					break;
				case MeshInit::x8:
					B_Meshes8x::Init();
					break;
				case MeshInit::x16:
					B_Meshes16x::Init();
					break; 
				default:
					cout << "\nNo Mesh Init";
					break;
		}*/
	}

	void All_Update() {
						while (!sGameObjsAwait.empty()) {
							sGameObjsAwait.front()->True_Init();
							sGameObjs.push_back(sGameObjsAwait.front()); 
							sGameObjsAwait.pop();
						}
						for (GameObj* pInst : sGameObjs) {
							pInst->True_Start();
						}
						for (GameObj* pInst : sGameObjs) {
							pInst->Update();
						}

		for (GameObj* pInst : sGameObjs) {//Destroy Children
			if (pInst->Destroy) { 
				for (GameObj* Each : pInst->Children) {
					Each->Destroy = true;
				}
			}
		}

		for (GameObj* pInst : sGameObjs) {//True Destruction
			if (pInst->Destroy) { GameObj::DestroyObj(pInst); }
		}


				for (GameObj* pInst : sGameObjs) {	
					if (!pInst->Transform.Parent) {
						pInst->Transform.modelMatrix_Update3D();
					}
				}
				for (GameObj* pInst : sGameObjs) {	
					if (pInst->Transform.Parent) {
						pInst->Transform.modelMatrix_Update3D();
					}
				}


		for (GameObj* pInst : sGameObjs) {
			pInst->LateUpdate();
		} 


		B_ColliderShape::Update();

		Time.Calculate();

		//cout << "|" << sGameObjsAwait.size() << "|"<<sGameObjs.size() << "|" << Time.Fps << "|";
	}

	void All_Start() {
		All_Update();
	}

}