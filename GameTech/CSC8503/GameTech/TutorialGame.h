#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include <iostream>
#include <fstream>
#include "../../Common/Assets.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/NavigationPath.h"



namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

			GameObject* goal = nullptr;
			GameObject* block = nullptr;
			GameObject* sphere = nullptr;
			GameObject* monster = nullptr;
			GameObject* spin = nullptr;

			Vector3 Get_Pos_Goal();
			Vector3 Get_Pos_Sphere();

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void Initmainmenu();
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitCubeGridWorld2();
			void InitSphereCollisionTorqueTest();
			void InitCubeCollisionTorqueTest();
			void InitSphereAABBTest();
			void InitGJKWorld();
			void BridgeConstraintTest();
			void SimpleGJKTest();
			void SimpleAABBTest();
			void SimpleAABBTest2();

			bool SelectObject();
			void MoveSelectedObject();

			void S_Machine();
			void updatepath();
			enum State {
				MovingUp,
				MovingDown,
				Far
			};
			State state = MovingUp;

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddFloorToWorld2(const Vector3& position);
			GameObject* AddWallToWorld(const Vector3& position, const Vector3& size);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddGoalToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddblockToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddmonsterToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddspinToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			vector<Vector3> testNodes;
			void TestPathfinding();
			void DisplayPathfinding();

		};
	}
}

