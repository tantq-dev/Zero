#include "Desk.h"

void Desk::OnUpdate(float dt)
{
}

void Desk::OnFixedUpdate(float dt)
{
}

void Desk::OnStart()
{
}

void Desk::OnDestroy()
{
}

void Desk::OnCustomerLeave()
{
	m_currentCustomer--;
	if (m_currentCustomer == 0)
	{
		//change texture
	}
}

void Desk::OnCustomerEnter()
{
	if (m_currentCustomer == 0)
	{
		//change texture
	}
	m_currentCustomer++;
}
