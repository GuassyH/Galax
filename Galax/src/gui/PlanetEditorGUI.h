#pragma once

#include "Player.h"
#include "rendering/Renderer.h"

namespace GUI {

	class PlanetEditor {
	public:
		static void DrawPlanetEditor(Renderer& renderer, Player& player, Universe::Planet* planet);
	};
};