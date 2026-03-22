#pragma once

#include <glm/glm.hpp>
#include <random>

namespace Galax {



	namespace Random {
		static thread_local std::mt19937 rng(std::random_device{}());
		static std::uniform_real_distribution<float> random(0.0f, 1.0f);

		static float Range(float min, float max) {
			float delta = max - min;
			return (random(rng) * delta) + min;
		}

		static glm::vec3 PointOnUnitSphere() {
			double theta = random(rng) * 2.0 * 3.14159265;        // azimuthal angle
			double cosPhi = (random(rng) * 2.0) - 1.0;      // cos(polar angle)
			double phi = acos(cosPhi);

			double x = sin(phi) * cos(theta);
			double y = sin(phi) * sin(theta);
			double z = cosPhi;

			return glm::vec3(x, y, z);
		}

	}
}