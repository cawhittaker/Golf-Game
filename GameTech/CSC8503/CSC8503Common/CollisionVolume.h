#pragma once
namespace NCL {
	enum class VolumeType {
		AABB	= 1,
		OBB		= 2,
		Sphere	= 4, 
		Mesh	= 8,
		Compound= 16,
		Invalid = 256
	};

	enum class layerType {
		Default
	};

	
	class CollisionVolume
	{
	public:
		CollisionVolume() {
			type = VolumeType::Invalid;
			lType = layerType::Default;
		}
		~CollisionVolume() {}

		VolumeType type;
		layerType lType;
	};



}