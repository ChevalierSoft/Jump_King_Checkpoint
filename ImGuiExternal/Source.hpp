#pragma once

#ifndef SOURCE_H
# define SOURCE_H

#include "Functions.h"
#include "Overlay.h"
#include <iostream>
#include <windows.h>
#include <sstream>
#include <string>

#include "Entity.hpp"

#include "CasualLibrary1.0/CasualLibrary.hpp"
#pragma comment(lib, "CasualLibrary.lib")

#define	WAITING	-1
#define RUN		0
#define	QUIT	1

#define	BUFFER_SIZE 256

struct GameData
{
	Address					baseAddr;
	Address					a_entity_list;
	ptrdiff_t				offset;
	uintptr_t				first_ennemie;

	std::vector<Entity>	v;

};

//GameData	g_data;

void	panel_menu(GameData& g_data);
void	esp_menu(GameData& g_data);


#endif