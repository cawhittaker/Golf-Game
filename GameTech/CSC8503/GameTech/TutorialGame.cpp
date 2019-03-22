#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

int level = 2;
int shot_Counter_1 = 0;
int shot_Counter_2 = 0;
int updatepathcount = 0;
bool menu;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it 

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh = new OGLMesh("cube.msh");
	cubeMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	cubeMesh->UploadToGPU();

	sphereMesh = new OGLMesh("sphere.msh");
	sphereMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	sphereMesh->UploadToGPU();

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(10, 40));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(10, 40));
	}

	if (goal->GetWinner() != nullptr) {
		level = 2;
		InitWorld();
		return;
	}

	if (monster && monster->Getreset() != nullptr) {
		sphere->GetTransform().SetWorldPosition(Vector3(50,-50,50));
		//std::cout << "hit" << std::endl;
		monster->SetResetToNull();
		Vector3 a;
		a = sphere->GetPhysicsObject()->GetLinearVelocity();
		if (a.Length() > 300.0) {
			monster->GetTransform().SetWorldPosition(Vector3(50, -50, 600));
		}

		return;
	}

	if(level == 1) {
		if (Window::GetMouse()->ButtonPressed(MOUSE_RIGHT)) {
			shot_Counter_1++;
		}
		S_Machine();
		Debug::Print("Total Shots  " + std::to_string(shot_Counter_1), Vector2(10, 60));
		Debug::Print("Press m for Main Menu  ", Vector2(10, 650), Vector4(0.0, 0.0, 1.0, 1.0));
	}
	
	if (level == 2) {
		if (Window::GetMouse()->ButtonPressed(MOUSE_RIGHT)) {
			shot_Counter_2++;
		}
		Debug::Print("Total Shots  " + std::to_string(shot_Counter_2), Vector2(10, 60));
		Debug::Print("Press m for Main Menu  ", Vector2(10, 650), Vector4(0.0, 0.0, 1.0, 1.0));
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_1)) {
		level = 1;
		InitWorld();
		return;
		}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_2)) {
		level = 2;
		InitWorld();
		return;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_M)) {
		Debug::Print("Press 1 for level 1  ", Vector2(10, 630), Vector4(1.0, 0.0, 0.0, 1.0));
		Debug::Print("Press 2 for level 2  ", Vector2(10, 610), Vector4(0.0, 0.0, 1.0, 1.0));
		Debug::Print("Press C for camera reset  ", Vector2(10, 590), Vector4(0.0, 0.0, 1.0, 1.0));
		Debug::Print("Press F1 for level reset  ", Vector2(10, 570), Vector4(0.0, 0.0, 1.0, 1.0));
	}

	if (menu) {
		Debug::Print("Press 1 for level 1  ", Vector2(10, 630), Vector4(1.0, 0.0, 0.0, 1.0));
		Debug::Print("Press 2 for level 2  ", Vector2(10, 610), Vector4(0.0, 0.0, 1.0, 1.0));
		Debug::Print("Press C for camera reset  ", Vector2(10, 590), Vector4(0.0, 0.0, 1.0, 1.0));
		Debug::Print("Press F1 for level reset  ", Vector2(10, 570), Vector4(0.0, 0.0, 1.0, 1.0));
	}
	
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_C)) {
		world->GetMainCamera()->SetPosition(sphere->GetTransform().GetWorldPosition() + Vector3(-100,200,-100));
		world->GetMainCamera()->SetPitch(-45.0f);
		world->GetMainCamera()->SetYaw(225.0f);
	}

		TestPathfinding();
		DisplayPathfinding();
		testNodes.clear();
	

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();
	renderer->Render();
	updatepathcount++;
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_M)) {
		menu = !menu; //Toggle gravity!
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_R)) {
		physics->SetGravity(Vector3(0, 9.81, 0));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_F)) {
		physics->SetGravity(Vector3(0, -9.81, 0));
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F8)) {
		world->ShuffleObjects(false);
	}
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -100));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 100));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(320.0f);
	world->GetMainCamera()->SetPosition(Vector3(-50, 120, 200));
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	if (level == 1) { InitCubeGridWorld(5, 5, 50.0f, 50.0f, Vector3(10, 10, 10)); }
	if (level == 2) { InitCubeGridWorld2(); }
	if (level == 3) { Initmainmenu(); }
	//InitSphereGridWorld(w, 10, 10, 50.0f, 50.0f, 10.0f);

	//InitSphereGridWorld(w, 1, 1, 50.0f, 50.0f, 10.0f);
	//InitCubeGridWorld(w,1, 1, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitCubeGridWorld(w, 1, 1, 50.0f, 50.0f, Vector3(8, 8, 8));

	//InitSphereCollisionTorqueTest(w);
	//InitCubeCollisionTorqueTest(w);

	//InitSphereGridWorld(w, 1, 1, 50.0f, 50.0f, 10.0f);
	//BridgeConstraintTest(w);
	//InitGJKWorld(w);

	//DodgyRaycastTest(w);
	//InitGJKWorld(w);
	//InitSphereAABBTest(w);
	//SimpleGJKTest(w);
	//SimpleAABBTest2(w);

	//InitSphereCollisionTorqueTest(w);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(1000, 30, 1000);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddFloorToWorld2(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(500, 30, 500);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, const Vector3& size) {
	GameObject* wall = new GameObject();

	Vector3 wallSize = size;
	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldScale(wallSize);
	wall->GetTransform().SetWorldPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wall);

	return wall;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	sphere = new GameObject();
	sphere->SetName("ball");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddGoalToWorld(const Vector3& position, float radius, float inverseMass) {
	goal = new GameObject();
	goal->SetName("goal");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	goal->SetBoundingVolume((CollisionVolume*)volume);
	goal->GetTransform().SetWorldScale(sphereSize);
	goal->GetTransform().SetWorldPosition(position);

	goal->SetRenderObject(new RenderObject(&goal->GetTransform(), sphereMesh, basicTex, basicShader));
	goal->SetPhysicsObject(new PhysicsObject(&goal->GetTransform(), goal->GetBoundingVolume()));

	goal->GetPhysicsObject()->SetInverseMass(inverseMass);
	goal->GetPhysicsObject()->InitSphereInertia();

	goal->GetRenderObject()->SetColour(Vector4(1.0, 1.0, 0.0, 0.0));

	world->AddGameObject(goal);

	return goal;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddblockToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	block = new GameObject();
	block->SetName("block");
	AABBVolume* volume = new AABBVolume(dimensions);

	block->SetBoundingVolume((CollisionVolume*)volume);

	block->GetTransform().SetWorldPosition(position);
	block->GetTransform().SetWorldScale(dimensions);

	block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
	block->SetPhysicsObject(new PhysicsObject(&block->GetTransform(), block->GetBoundingVolume()));

	block->GetPhysicsObject()->SetInverseMass(inverseMass);
	block->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(block);

	return block;
}

GameObject* TutorialGame::AddspinToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	spin = new GameObject();
	spin->SetName("spin");
	AABBVolume* volume = new AABBVolume(dimensions);

	spin->SetBoundingVolume((CollisionVolume*)volume);

	spin->GetTransform().SetWorldPosition(position);
	spin->GetTransform().SetWorldScale(dimensions);

	spin->SetRenderObject(new RenderObject(&spin->GetTransform(), cubeMesh, basicTex, basicShader));
	spin->SetPhysicsObject(new PhysicsObject(&spin->GetTransform(), spin->GetBoundingVolume()));

	spin->GetPhysicsObject()->SetInverseMass(inverseMass);
	spin->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(spin);

	return spin;
}

GameObject* TutorialGame::AddmonsterToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	monster = new GameObject();
	monster->SetName("monster");
	AABBVolume* volume = new AABBVolume(dimensions);

	monster->SetBoundingVolume((CollisionVolume*)volume);

	monster->GetTransform().SetWorldPosition(position);
	monster->GetTransform().SetWorldScale(dimensions);

	monster->SetRenderObject(new RenderObject(&monster->GetTransform(), cubeMesh, basicTex, basicShader));
	monster->SetPhysicsObject(new PhysicsObject(&monster->GetTransform(), monster->GetBoundingVolume()));

	monster->GetPhysicsObject()->SetInverseMass(inverseMass);
	monster->GetPhysicsObject()->InitCubeInertia();

	monster->GetRenderObject()->SetColour(Vector4(1.0, 0.0, 0.0, 0.0));

	world->AddGameObject(monster);

	return monster;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, radius, z * rowSpacing);
			AddSphereToWorld(position, radius);
		}
	}
	AddFloorToWorld(Vector3(0, -100, 0));

}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 10.0f;
	Vector3 cubeDims = Vector3(10, 10, 10);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
	AddFloorToWorld(Vector3(0, -100, 0));
}

void TutorialGame::Initmainmenu() {

	//AddFloorToWorld(Vector3(10, -100, 1));
	
	Debug::Print("Press 1 for level 1  " , Vector2(10,120), Vector4(1.0,0.0,0.0,1.0));
	Debug::Print("Press 2 for level 2  " , Vector2(10, 100), Vector4(0.0, 0.0, 1.0, 1.0));
	Debug::Print("Press m for Main Menu  ", Vector2(10, 140), Vector4(0.0, 0.0, 1.0, 1.0));

}


void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	/*for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}*/ 

	AddSphereToWorld(Vector3(0, -50, 0), 5.0f, 100.0f);
	AddGoalToWorld(Vector3(500, -70, 0), 20.0f, 0.0f);
	AddCubeToWorld(Vector3(500, -70, 20), Vector3(15, 20, 5), 0.0f);
	AddCubeToWorld(Vector3(500, -70, -20), Vector3(15, 20, 5), 0.0f);
	AddCubeToWorld(Vector3(480, -70, 0), Vector3(5, 20, 25), 0.0f);
	AddCubeToWorld(Vector3(520, -70, 0), Vector3(5, 20, 25), 0.0f);

	AddFloorToWorld(Vector3(10, -100, 1));

	//
	AddWallToWorld(Vector3(0, -50, -100),Vector3(100, 25, 1)); // side
	AddWallToWorld(Vector3(0, -50, 100), Vector3(100, 25, 1));
	AddWallToWorld(Vector3(101, -50, 0), Vector3(1, 25, 100)); //top/bot
	AddWallToWorld(Vector3(-101, -50, 60), Vector3(1, 25, 40));
	AddWallToWorld(Vector3(-101, -50, -60), Vector3(1, 25, 40));
	AddWallToWorld(Vector3(-252, -50, 20), Vector3(151, 25, 1));
	AddWallToWorld(Vector3(-272, -50, -20), Vector3(171, 25, 1));
	AddWallToWorld(Vector3(-404, -50, 70), Vector3(1, 25, 50));
	AddWallToWorld(Vector3(-444, -50, 90), Vector3(1, 25, 110));
	AddWallToWorld(Vector3(-124, -50, 120), Vector3(280, 25, 1));
	AddWallToWorld(Vector3(-104, -50, 160), Vector3(300, 25, 1));
	AddWallToWorld(Vector3(38, -50, 200), Vector3(482, 25, 1));
	AddWallToWorld(Vector3(156, -50, 0), Vector3(1, 25, 120));
	AddWallToWorld(Vector3(196, -50, 0), Vector3(1, 25, 160));
	AddWallToWorld(Vector3(338, -50, 160), Vector3(142, 25, 1));
	AddWallToWorld(Vector3(480, -50, 90), Vector3(1, 25, 70));
	AddWallToWorld(Vector3(520, -50, 110), Vector3(1, 25, 90));
	AddWallToWorld(Vector3(95, -50, -160), Vector3(100, 25, 1));
	AddWallToWorld(Vector3(-5, -50, -130), Vector3(1, 25, 30));

	AddblockToWorld(Vector3(500, 20,20), Vector3(20, 10, 1), 0.0f);

	BridgeConstraintTest();

}

void TutorialGame::InitCubeGridWorld2() {
	
	std::ifstream infile(Assets::DATADIR + "level2.txt");
	int size, x_length, z_length;
	infile >> size;
	infile >> x_length;
	infile >> z_length;
	
	if (!infile) { std::cout << "no file" << std::endl; }
	
	for (unsigned i = 0; i < (x_length);i++) {
		for (unsigned j = 0; j < (z_length); j++) {
			char a;
			infile >> a;

			if (a == 'x') {
				AddCubeToWorld(Vector3(j*size, -70, i*size), Vector3(25, 15, 25), 0.0f);
			}

			if (a == 'X') {
				AddCubeToWorld(Vector3(j*size, -50, i*size), Vector3(25, 25, 25), 0.0f);
			}
		
				if (a == 'g') {
					AddGoalToWorld(Vector3(j*size, -70, i*size), 20.0f, 0.0f);
				}
				if (a == 's') {
					AddSphereToWorld(Vector3(j*size, -50, i*size), 5.0f, 100.0f);
				}
				if (a == 'M') {
					AddmonsterToWorld(Vector3(j*size, -60, i*size), Vector3(10, 10, 10), 100.0f);
				}
		}
	}
	//infile.close();

	//AddSphereToWorld(Vector3(0, -50, 0), 5.0f, 100.0f);
	//AddGoalToWorld(Vector3(500, -70, 0), 20.0f, 0.0f);

	AddFloorToWorld2(Vector3(475, -100, 475));
	
	
	//BridgeConstraintTest();

	//AddWallToWorld(Vector3(-201, -50, -60), Matrix3::Rotation(22.5f, Vector3(0, 0, 1)) * Vector3(1, 40, 40));
	//
}


void TutorialGame::InitSphereCollisionTorqueTest() {
	AddSphereToWorld(Vector3(15, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-25, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-50, 0, 0), 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);

	AddFloorToWorld(Vector3(0, -100, 0));
}


void TutorialGame::InitCubeCollisionTorqueTest() {
	Vector3 cubeSize(10, 10, 10);
	AddCubeToWorld(Vector3(15, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-25, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-50, 0, 0), cubeSize, 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);

	AddFloorToWorld(Vector3(0, -100, 0));
}

void TutorialGame::InitSphereAABBTest() {
	Vector3 cubeSize(10, 10, 10);

	AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 10.0f);
	AddSphereToWorld(Vector3(2, 0, 0), 5.0f, 10.0f);
}

void TutorialGame::InitGJKWorld() {
	Vector3 dimensions(20, 2, 10);
	float inverseMass = 10.0f;

	for (int i = 0; i < 2; ++i) {
		GameObject* cube = new GameObject();

		OBBVolume* volume = new OBBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform().SetWorldPosition(Vector3(0, 0, 0));
		cube->GetTransform().SetWorldScale(dimensions);

		if (i == 1) {
			cube->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), 90.0f));
		}

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(cube);
	}
}

void TutorialGame::BridgeConstraintTest() {
	float sizeMultiplier = 1.0f;

	Vector3 cubeSize = Vector3(2, 1, 8) * sizeMultiplier;

	float invCubeMass = 1.0;
	int numLinks = 9;
	float maxDistance = 4;
	float cubeDistance = 4;

	Vector3 startPos = Vector3(430, -70, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance , 20, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, i*2, 0), cubeSize,invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::SimpleGJKTest() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}

void TutorialGame::SimpleAABBTest() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(10, 20, 0), dimensions, 10.0f);
}

void TutorialGame::SimpleAABBTest2() {
	Vector3 dimensions		= Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(8, 2, 8);

	GameObject* newFloor	= AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(8, 20, 0), dimensions, 10.0f);
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::MOUSE_LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

				Ray tempray = Ray(selectionObject->GetTransform().GetWorldPosition(), Vector3(0, 0, -1));

				Debug::DrawLine(tempray.GetPosition(), goal->GetTransform().GetWorldPosition(), Vector4(1.0, 0.0, 0.0, 1.0));

				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::S_Machine() {
	
	Vector3 a,b;

	a = block->GetTransform().GetWorldPosition();
	b= sphere->GetTransform().GetWorldPosition();

	switch (state) {
	case MovingUp:
		if ((b - a).Length() > 400.0) state = Far;
		else if (a.y > 30) state = MovingDown;
		else a.y += 1;
		break;
	case MovingDown:
		if ((b - a).Length() > 400.0) state = Far;
		else if (a.y < -40) state = MovingUp;
		else a.y -= 1;
		break;
	case Far:
		if ((b - a).Length() < 400.0) state = MovingUp;
		break;
	}

	block->GetTransform().SetWorldPosition(Vector3(500, a.y, 20));
}

Vector3 TutorialGame::Get_Pos_Goal() {
	Vector3 a;
	a = goal->GetTransform().GetWorldPosition();
	return a;
}

Vector3 TutorialGame::Get_Pos_Sphere() {
	Vector3 a;
	a = sphere->GetTransform().GetWorldPosition();
	return a;
}

void TutorialGame::TestPathfinding() {

	if (monster && sphere)
	{
		//updatepath();
		NavigationGrid grid("level2.txt");

		NavigationPath outPath;
		Vector3 startPos(monster->GetTransform().GetWorldPosition());
		Vector3 endPos(sphere->GetTransform().GetWorldPosition());

		bool found = grid.FindPath(startPos, endPos, outPath);

		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			testNodes.push_back(pos);
		}

		if (found && testNodes.size() > 2) {
			Vector3 a = testNodes[0];
			Vector3 b = monster->GetTransform().GetWorldPosition();
			Vector3 d = testNodes[1];

			Vector3 c = a - b;
			Vector3 e = d - b;

			e.Normalise();
			monster->GetPhysicsObject()->AddForce(e * 3.0);
		}

		if (found && testNodes.size() <= 1 ) {
			Vector3 a = testNodes[0];
			Vector3 b = monster->GetTransform().GetWorldPosition();
			Vector3 d = sphere->GetTransform().GetWorldPosition();

			Vector3 c = a - b;
			Vector3 e = d - b;

			e.Normalise();
			monster->GetPhysicsObject()->AddForce(e * 5.0);
		}
	}
	
}


void TutorialGame::DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 0, 1, 1));
	}
}




void TutorialGame::updatepath() {
	
	if (monster && sphere)
	{
		NavigationGrid grid("level2.txt");

		NavigationPath outPath;
		Vector3 startPos(monster->GetTransform().GetWorldPosition());
		Vector3 endPos(sphere->GetTransform().GetWorldPosition());

		bool found = grid.FindPath(startPos, endPos, outPath);

		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			testNodes.push_back(pos);
		}
	}
	
}