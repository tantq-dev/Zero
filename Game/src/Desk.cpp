#include "Desk.h"

void Desk::OnUpdate(float dt)
{
}

void Desk::OnFixedUpdate(float dt)
{
}

void Desk::OnStart()
{
	auto& transform = AddComponent<Components::Transform2D>();
	transform.position = m_config.position;

	auto& sprite = AddComponent<Components::Sprite>();
	sprite.texture = m_config.emptyTexture;
	sprite.source = { 0, 0, 32, 64 };
	sprite.layer = m_config.layer;
}

void Desk::OnDestroy()
{
}

void Desk::OnCustomerLeave()
{
	if (m_currentCustomer == 0)
	{
		return;
	}

	m_currentCustomer--;
	if (m_currentCustomer == 0)
	{
		m_needCleanUp = true;
	}
}

void Desk::OnCustomerEnter()
{
    if (m_currentCustomer == 0)
    {
        m_isServedFood = false;

    }
    m_currentCustomer++;
}

void Desk::OnCleaningUp()
{
	m_needCleanUp = false;
	m_isServedFood = false;
	auto& sprite = GetComponent<Components::Sprite>();
	sprite.texture = m_config.emptyTexture;
	sprite.source = { 0, 0, 32, 64 };

}

void Desk::OnFoodServing()
{
		auto& sprite = GetComponent<Components::Sprite>();
		sprite.texture = m_config.occupiedTexture;
		sprite.source = { 0, 0, 32, 64 };
		m_isServedFood = true;
}

bool Desk::NeedsCleanUp() const
{
	return m_needCleanUp;
}

bool Desk::IsClean() const
{
	return !m_needCleanUp;
}

bool Desk::HasCustomer() const
{
	return m_currentCustomer > 0;
}
