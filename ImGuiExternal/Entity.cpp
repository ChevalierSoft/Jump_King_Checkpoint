//#include "Source.hpp"

#include "Entity.hpp"

Entity::Entity() {}

Entity::Entity(const Entity& copy)
{
	*this = copy;
}

Entity::~Entity() {}

Entity& Entity::operator= (const Entity& copy)
{
	if (this != &copy)
	{
		addr = copy.addr;
		a_x = copy.a_x;
		x = copy.x;
	}
	return *this;
}
