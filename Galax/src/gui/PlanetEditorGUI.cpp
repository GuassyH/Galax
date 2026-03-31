#include "PlanetEditorGUI.h"
#include "universe/UniverseManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace GUI {

	void PlanetEditor::DrawPlanetEditor() {
		Universe::UniverseManager& universeManager = Universe::UniverseManager::Get();

		for (auto planet : universeManager.GetPlanets()) {
			std::string id = std::string("##") + std::string(planet->name);

			if(ImGui::CollapsingHeader(planet->name)) {
				ImGui::DragFloat3(("Velocity" + id + "_velocity").c_str(), &planet->physicsBody.velocity.x, 0.01f);
				ImGui::DragFloat3(("Position" + id + "_position").c_str(), &planet->transform->local_position.x);

				ImGui::Checkbox(("Debug Path" + id + "_path").c_str(), &planet->physicsBody.debug_path);
				
				if (planet->physicsBody.debug_path) {
					ImGui::DragInt(("NumPoints" + id + "_points").c_str(), &planet->physicsBody.num_points);
					ImGui::DragFloat(("TimeStep" + id + "_step").c_str(), &planet->physicsBody.time_step);
				}
			}
		}
	}
};