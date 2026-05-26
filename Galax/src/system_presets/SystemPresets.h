#pragma once

#include <iostream>
#include "universe/UniverseManager.h"
#include "rendering/Renderer.h"


class SystemPresets {
public:
	static std::shared_ptr<Universe::Planet> CreateFirstSystem(Renderer& renderer);
};