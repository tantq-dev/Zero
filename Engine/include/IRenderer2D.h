#pragma once
#include "Components.h"
class IRenderer2D {
public:
	virtual ~IRenderer2D() = default;

	virtual void PushSpriteToRenderQueue(const Components::Sprite&, const Components::Transform2D&) = 0;
	virtual Components::Texture GetTextureFromFile(const std::string& path) = 0;
	virtual void CallRender() = 0;

};