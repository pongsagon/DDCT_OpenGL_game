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

		for (GameObj* pInst : sGameObjs) {	//Render 
			if (pInst->Destroy) { GameObj::DestroyObj(pInst); }
		}

		for (Transform* pInst : sTransforms) {
			if (!pInst->Parent) {
				pInst->modelMatrix_Update3D();
			}
		}
		for (Transform* pInst : sTransforms) {
			if (pInst->Parent) {
				pInst->modelMatrix_Update3D();
			}
		}

		//for (Renderer* pInst : sRenderers) {
		//	pInst->Draw();
		//}

		for (GameObj* pInst : sGameObjs) {
			pInst->LateUpdate();
		} 


		//Input::Update();/////////////////////////////////////////////////
		Time.Calculate();
		//BanKEngine::GlfwGlad::Update();
		//B_Audio.UpdateAllSource();
		//if (FreeCam) {
		//	BanKEngine::GlfwGlad::processInput();
		//}

	}

	void All_Start() {
		All_Update();
	}

}