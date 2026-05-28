#include "PlanetEditorGUI.h"
#include "universe/UniverseManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace GUI {


	void Spacing() {
		ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
	}


	void PlanetEditor::DrawPlanetEditor(Player& player) {
		Universe::UniverseManager& universeManager = Universe::UniverseManager::Get();

		for (auto planet : universeManager.GetPlanets()) {
			std::string id = std::string("##") + std::string(planet->name);

			if(ImGui::CollapsingHeader(planet->name)) {
				ImGui::Spacing();	ImGui::Spacing();	ImGui::Spacing();
				
				if (ImGui::Button(("Move to" + id + "_move_to").c_str())) {
					glm::vec3 new_pos = planet->transform->world_position - (player.camera.transform->forward * planet->radius * 4.0f);
					player.transform->SetWorldPosition(new_pos);
				}
				
				Spacing();

				// Physics Settings
				ImGui::Text("Physics");
				ImGui::DragFloat3(("Position" + id + "_position").c_str(), &planet->transform->local_position.x);
				ImGui::DragFloat3(("Velocity" + id + "_velocity").c_str(), &planet->physicsBody.velocity.x, 0.01f);
				ImGui::DragFloat(("Mass" + id + "_mass").c_str(), &planet->physicsBody.mass);


				// Body Settings
				Spacing();

				ImGui::Text("Body");
				ImGui::DragFloat(("Radius" + id + "_radius").c_str(), &planet->radius);


				Spacing();

				ImGui::Text("Noise Settings");


				Spacing();

				ImGui::Text("Crater Settings");

				Spacing();

				if (ImGui::Button(("Rebuild Body" + id + "_rebuild_body").c_str())) {
					GX_TRACE("Rebuild {}, not implemented", planet->name);
				}

				// Debug Settings
				Spacing();

				ImGui::Text("Debug");
				ImGui::Checkbox(("Debug Path" + id + "_path").c_str(), &planet->physicsBody.debug_path);
				
				if (planet->physicsBody.debug_path) {
					ImGui::DragInt(("NumPoints" + id + "_points").c_str(), &planet->physicsBody.num_points);
					ImGui::DragFloat(("TimeStep" + id + "_step").c_str(), &planet->physicsBody.time_step);
				}
				

				// Atmosphere Settings
				Spacing();

				ImGui::Checkbox(("HasAtmosphere" + id + "_has_atmosphere").c_str(), & planet->hasAtmosphere);
				if (planet->hasAtmosphere) {

					ImGui::DragFloat(("Atmosphere Height" + id + "_atmos_height").c_str(), &planet->atmosphere_config.atmosphereHeight);
					ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
					ImGui::DragFloat(("Density Falloff" + id + "_density_falloff").c_str(), &planet->atmosphere_config.densityFalloff);
					ImGui::DragFloat(("Intensity" + id + "_intensity").c_str(), &planet->atmosphere_config.intensity);
					ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
					bool lambda = ImGui::DragFloat3(("Wavelengths" + id + "_wavelengths").c_str(), &planet->atmosphere_config.wavelengths.r);
					bool strength = ImGui::DragFloat(("Scatter Strength" + id + "_scatter_strength").c_str(), &planet->atmosphere_config.scatteringStrength);
					bool coeff = ImGui::DragFloat(("Scatter Coefficient" + id + "_scatter_coefficient").c_str(), &planet->atmosphere_config.scatteringCoefficient);

					if (lambda || strength || coeff) {
						planet->atmosphere_config.UpdateWavelengthScatter();
					}
				}
				Spacing();

				// Ocean Settings
				ImGui::Checkbox(("HasOcean" + id + "_has_ocean").c_str(), & planet->hasOcean);
				if (planet->hasOcean) {

					ImGui::DragFloat(("Ocean Radius" + id + "_ocean_radius").c_str(), &planet->ocean_config.radius);
					ImGui::ColorEdit3(("Ocean Colour" + id + "_ocean_colour").c_str(), &planet->ocean_config.oceanColor.r);
					ImGui::ColorEdit3(("Fresnel Colour" + id + "_fresnel_colour").c_str(), &planet->ocean_config.fresnelColor.r);
					ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
					ImGui::DragFloat(("Normal Repeat" + id + "_normal_repeat").c_str(), &planet->ocean_config.normalRepeat);
					ImGui::DragInt(("Normal Factor" + id + "_normal_factor").c_str(), &planet->ocean_config.normalFactor);
					ImGui::DragFloat(("Normal Strength" + id + "_normal_strength").c_str(), &planet->ocean_config.normalStrength);
					ImGui::DragFloat(("Triplanar Blend" + id + "_triplanar_blend").c_str(), &planet->ocean_config.triplanarBlend);
				}

				Spacing();
			}
		}
	}
};