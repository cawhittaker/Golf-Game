#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			const Transform& GetConstTransform() const {
				return transform;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void SetName(string x) {
				name = x;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				if (name != "goal") return;
				if (otherObject->name != "ball") return;
				winner = otherObject;
				//std::cout << "OnCollision event occured!\n";
			}

			virtual void OnCollisionBegin2(GameObject* otherObject) {
				if (name != "monster") return;
				if (otherObject->name != "ball") return;
				reset = otherObject;
				std::cout << "OnCollision event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollision event occured!\n";
			}

			bool InsideAABB(const Vector3& pos, const Vector3& halfSize);

			GameObject* GetWinner() const{
				return winner;
			}

			GameObject* Getreset() const {
				return reset;
			}

			void SetResetToNull() {
				reset = nullptr;
			}

			
			bool GetBroadphaseAABB(Vector3&outsize) const;
			void UpdateBroadphaseAABB();

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;
			GameObject* winner = nullptr;
			GameObject* reset = nullptr;

			bool	isActive;
			string	name;

			Vector3 broadphaseAABB;
		};
	}
}

