#pragma once

#include <Windows.h>
#include <stdint.h>

#include "CasualLibrary1.0/CasualLibrary.hpp"
#pragma comment(lib, "CasualLibrary.lib")

struct Entity
{
public:
	Entity();
	Entity(const Entity& e);
	~Entity();

	Entity& operator= (const Entity& copy);

	void		display() const;	// in Source.cpp

	uintptr_t	addr;

	uintptr_t	a_x;
	uintptr_t	a_y;
	uintptr_t	a_vx;
	uintptr_t	a_vy;

	float		x;
	float		y;
	float		vx;
	float		vy;

};