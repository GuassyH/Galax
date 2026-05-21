#pragma once

#include "universe/planetary/Planet.h"
#include "Player.h"

namespace Universe {
	class UniverseDebug {
	public:
		UniverseDebug() = default;
	public:
		UniverseDebug(const UniverseDebug&) = delete;
		UniverseDebug& operator=(const UniverseDebug&) = delete;

		static UniverseDebug& Get() {
			static UniverseDebug instance;
			return instance;
		}

		void Init();
		void Update(Player& player);
		void Render(Player& player);

		/// Path stuff
		void DrawPredictedPath(Player& player, Planet* planet, Planet* centre = nullptr);
		std::shared_ptr<FragShader> pathShader;
		VAO pathVAO;
		VBO pathVBO;
	};
}