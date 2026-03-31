#pragma once

#include "universe/planetary/Planet.h"
#include "Player.h"

namespace Universe {
	class UniverseDebug {
	public:
		void Init();

		/// Path stuff
		void DrawPredictedPath(Player& player, Planet* planet, Planet* centre = nullptr);
		std::shared_ptr<FragShader> pathShader;
		VAO pathVAO;
		VBO pathVBO;
	};
}