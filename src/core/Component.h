#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
namespace Core
{

    /**
    * @brief Transform component that defines an entity's position, scale, and rotation in 2D space.
    * 
    * This component is used to represent the spatial properties of an entity:
    * - Position: The entity's location in 2D space
    * - Scale: The entity's size multiplier in both X and Y directions
    * - Rotation: The entity's orientation in radians
    */
    struct CTransform
    {
    Vec2 position = { 0.0f, 0.0f };
    Vec2 scale = { 1.0f, 1.0f };
    float rotation = 0.0f;

    CTransform() = default;
    CTransform(const Vec2& pos, const Vec2& scl, float rot)
    : position(pos), scale(scl), rotation(rot)
    {
    }
    };

	struct CAnimation
	{
		
	};

	struct CAudio
	{
	
	};

	struct CInput
	{
		
	};

    struct CPhysic
    {
        Vec2 velocity = { 0.0f, 0.0f };
        CPhysic() = default;
        CPhysic(const Vec2& vel)
			: velocity(vel)
		{

		}
    };

	
}