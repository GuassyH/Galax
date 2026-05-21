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

				ImGui::Checkbox("HasAtmosphere", &planet->hasAtmosphere);
				if (planet->hasAtmosphere) {
					ImGui::Indent(20);
					if (ImGui::CollapsingHeader("Atmosphere")) {
						ImGui::DragFloat("Atmosphere Height", &planet->atmosphere_config.atmosphereHeight);
						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
						ImGui::DragFloat("Density Falloff", &planet->atmosphere_config.densityFalloff);
						ImGui::DragFloat("Intensity", &planet->atmosphere_config.intensity);

						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
						bool lambda = ImGui::DragFloat3("Wavelengths", &planet->atmosphere_config.wavelengths.r);
						bool strength = ImGui::DragFloat("Scatter Strength", &planet->atmosphere_config.scatteringStrength);
						bool coeff = ImGui::DragFloat("Scatter Coefficient", &planet->atmosphere_config.scatteringCoefficient);

						if (lambda || strength || coeff) {
							planet->atmosphere_config.UpdateWavelengthScatter();
						}


					}
					ImGui::Unindent(20);
				}

				ImGui::Checkbox("HasOcean", &planet->hasOcean);
				if (planet->hasOcean) {
					ImGui::Indent(20);
					if (ImGui::CollapsingHeader("Ocean")) {
						ImGui::DragFloat("Ocean Radius", &planet->ocean_config.radius);
						ImGui::ColorEdit3("Ocean Colour", &planet->ocean_config.oceanColor.r);
						ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						ImGui::DragFloat("Normal Repeat", &planet->ocean_config.normalRepeat);
						ImGui::DragInt("Normal Factor", &planet->ocean_config.normalFactor);
						ImGui::DragFloat("Normal Strength", &planet->ocean_config.normalStrength);
						ImGui::DragFloat("Triplanar Blend", &planet->ocean_config.triplanarBlend);
					}
					ImGui::Unindent(20);
				}
			}
		}
	}
};