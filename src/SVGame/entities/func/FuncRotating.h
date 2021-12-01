#pragma once

class SynchedEntityBase.h;
class SVGBaseMover;

class FuncRotating : public SVGBaseMover {
public:
	FuncRotating( ServerEntity* entity );
	virtual ~FuncRotating() = default;

	DefineMapClass( "func_rotating", FuncRotating, SVGBaseMover );

	// Spawn flags
	static constexpr int32_t SF_StartOn = 1 << 0;
	static constexpr int32_t SF_Reverse = 1 << 1;
	static constexpr int32_t SF_YAxis = 1 << 2;
	static constexpr int32_t SF_XAxis = 1 << 3;
	static constexpr int32_t SF_HurtTouch = 1 << 4;
	static constexpr int32_t SF_StopOnBlock = 1 << 5;
	static constexpr int32_t SF_Animated = 1 << 6;
	static constexpr int32_t SF_AnimatedFast = 1 << 7;

	void Spawn() override;

	void RotatorBlocked( SynchedEntityBase * other );
	void RotatorHurtTouch( SynchedEntityBase * self, SynchedEntityBase * other, cplane_t* plane, csurface_t* surf );
	void RotatorUse( SynchedEntityBase * other, SynchedEntityBase * activator );
};