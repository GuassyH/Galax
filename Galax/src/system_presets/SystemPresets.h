#pragma once

#include <iostream>
#include "universe/UniverseManager.h"



class SystemPresets {
public:
	static std::shared_ptr<Universe::Planet> CreateFirstSystem();
};