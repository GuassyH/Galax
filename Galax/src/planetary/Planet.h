#pragma once

#include "CubeSphere.h"
#include <iostream>
#include "Transform.h"

namespace Universe {
	class Planet {
	public:
		/// Core
		Planet();
		~Planet();
	
		void Update();
		void Render(Camera& camera, Shader& shader);
	
		void Delete();

		std::vector<CubeSphere::Face> faces;
		std::unique_ptr<Transform> transform;
	};
}