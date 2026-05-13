#pragma once
#include "CoreComponents.h"
#include "RenderComponents.h"
#include "PhysicsComponents.h"
#include "InputComponents.h"
#include "AudioComponents.h"
#include "TextComponents.h"
#include "Logger.h"
#include <stdexcept>

#define MATRIX_2D_INT std::vector<std::vector<int>>

namespace Components
{
	// This file now aggregates all component types.
	// Refer to individual headers for definitions.
    inline Color LerpColor(const Color& a, const Color& b, float t) {
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        };
    }
}
