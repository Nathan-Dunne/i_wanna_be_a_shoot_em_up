#pragma once
#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

class Attributes
{
public:

	Attributes();
	
	int m_health;
	int m_movementSpeed;
	int m_damage;
	int m_firingRate;
	int m_projectileSpeed;
	int m_luck;

	~Attributes();
};

#endif
