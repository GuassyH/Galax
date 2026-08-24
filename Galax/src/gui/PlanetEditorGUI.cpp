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


	void PlanetEditor::DrawPlanetEditor(Renderer& renderer, Player& player, Universe::Planet* planet) {
		Universe::UniverseManager& universeManager = Universe::UniverseManager::Get();
		std::string id = "##" + std::string(planet->name);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2.0f) - (ImGui::CalcTextSize(planet->name).x / 2.0));
		ImGui::Text(planet->name);
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2.0f) - (ImGui::CalcTextSize("Move to").x / 2.0));
		if (ImGui::Button(("Move to" + id + "_move_to").c_str())) {
			glm::vec3 new_pos = planet->transform->world_position - (player.camera.transform->forward * planet->radius * 4.0f);
			player.transform->SetWorldPosition(new_pos);
		}
		
		Spacing();

		// Physics Settings
		if (ImGui::CollapsingHeader("Physics")) {
			ImGui::DragFloat3(("Position" + id + "_position").c_str(), &planet->transform->local_position.x);
			ImGui::DragFloat3(("Velocity" + id + "_velocity").c_str(), &planet->physicsBody.velocity.x, 0.01f);
			ImGui::DragFloat(("Mass" + id + "_mass").c_str(), &planet->physicsBody.mass);
		}

		// Body Settings
		Spacing();

		ImGui::Text("Body");
		ImGui::DragFloat(("Radius" + id + "_radius").c_str(), &planet->radius);

		Spacing();

		if (ImGui::CollapsingHeader("Noise Settings")) {
			for (int i = 0; i < planet->terrainGenerator.noiseLayers.size(); i++) {
				ImGui::Text("NoiseLayer %i", i);
				ImGui::Indent();
				NoiseLayer& noiseLayer = planet->terrainGenerator.noiseLayers[i];

				ImGui::DragFloat3(("Centre" + id + "_noise_layer_" + std::to_string(i) + "_centre").c_str(), &noiseLayer.centre.x);
				ImGui::DragFloat(("Frequency" + id + "_noise_layer_" + std::to_string(i) + "_frequency").c_str(), &noiseLayer.frequency);
				ImGui::DragFloat(("Frequency Factor" + id + "_noise_layer_" + std::to_string(i) + "_frequencyFactor").c_str(), &noiseLayer.frequencyFactor);
				ImGui::DragFloat(("Intensity" + id + "_noise_layer_" + std::to_string(i) + "_intensity").c_str(), &noiseLayer.intensity);
				ImGui::DragFloat(("Intensity Factor" + id + "_noise_layer_" + std::to_string(i) + "_intensityFactor").c_str(), &noiseLayer.intensityFactor);
				ImGui::DragFloat(("Height Shift" + id + "_noise_layer_" + std::to_string(i) + "_heightShift").c_str(), &noiseLayer.heightShift);
				ImGui::DragFloat(("Wobble" + id + "_noise_layer_" + std::to_string(i) + "_wobble").c_str(), &noiseLayer.wobble);
				if (noiseLayer.type == NoiseType::Voronoi)
					ImGui::DragFloat(("Power" + id + "_noise_layer_" + std::to_string(i) + "_power").c_str(), &noiseLayer.power);
				ImGui::DragFloat(("Min Mask" + id + "_noise_layer_" + std::to_string(i) + "_min_mask").c_str(), &noiseLayer.minMask);

				ImGui::Unindent();
			}
		}

		Spacing();

		if (ImGui::CollapsingHeader("Crater Settings")) {

		}

		Spacing();

		if (ImGui::CollapsingHeader("Color Settings")) {
			for (int i = 0; i < planet->shader.colorMaps.size(); i++) {
				ImGui::Text("Color Map %i", i);
				ImGui::DragFloat4(("Color" + id + "_color_map_" + std::to_string(i) + "_col").c_str(), &planet->shader.colorMaps[i].col.x);
				ImGui::DragFloat(("Height" + id + "_color_map_" + std::to_string(i) + "_height").c_str(), &planet->shader.colorMaps[i].height);
				ImGui::DragFloat(("Height Dither" + id + "_color_map_" + std::to_string(i) + "_height_dither").c_str(), &planet->shader.colorMaps[i].heightDither);
				ImGui::DragFloat(("Height Sharpness" + id + "_color_map_" + std::to_string(i) + "_height_sharpness").c_str(), &planet->shader.colorMaps[i].heightSharpness);


				ImGui::DragFloat4(("Steep Color" + id + "_color_map_" + std::to_string(i) + "_steepness").c_str(), &planet->shader.colorMaps[i].steepCol.x);
				ImGui::DragFloat(("Steep" + id + "_color_map_" + std::to_string(i) + "_Steep").c_str(), &planet->shader.colorMaps[i].steepness);
				ImGui::DragFloat(("Steep Dither" + id + "_color_map_" + std::to_string(i) + "_steep_dither").c_str(), &planet->shader.colorMaps[i].steepDither);
				ImGui::DragFloat(("Steep Sharpness" + id + "_color_map_" + std::to_string(i) + "_Steep_sharpness").c_str(), &planet->shader.colorMaps[i].steepSharpness);

			}
		}

		Spacing(); 

		if (ImGui::Button(("Rebuild Body" + id + "_rebuild_body").c_str())) {
			GX_TRACE("Rebuild {}, not implemented", planet->name);

			// NOT FINISHED
			for (auto face : planet->faces) {
				planet->RemoveNodesFromVSSBO(renderer, face.root_chunk);
			}

			for (auto face : planet->faces) {
				CubeSphere::DestroyChunkNodes(face.root_chunk);
			}

			planet->terrainGenerator.ComputeBuffers(renderer, planet->radius);
		}

		// Debug Settings
		Spacing();

		if (ImGui::CollapsingHeader("Debug")) {
			ImGui::Checkbox(("Debug Path" + id + "_path").c_str(), &planet->physicsBody.debug_path);
				
			if (planet->physicsBody.debug_path) {
				ImGui::DragInt(("NumPoints" + id + "_points").c_str(), &planet->physicsBody.num_points);
				ImGui::DragFloat(("TimeStep" + id + "_step").c_str(), &planet->physicsBody.time_step);
			}
		}

		// Atmosphere Settings
		Spacing();

		ImGui::Checkbox(("HasAtmosphere" + id + "_has_atmosphere").c_str(), & planet->hasAtmosphere);
		if (planet->hasAtmosphere) {
			ImGui::Indent();
			if(ImGui::DragFloat(("Atmosphere Height" + id + "_atmos_height").c_str(), &planet->atmosphere_config.atmosphereHeight))
				renderer.atmosphereRenderer->BakeOpticalDepth(planet->atmosphere_config, planet->radius);

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
			if (ImGui::DragFloat(("Density Falloff" + id + "_density_falloff").c_str(), &planet->atmosphere_config.densityFalloff))
				renderer.atmosphereRenderer->BakeOpticalDepth(planet->atmosphere_config, planet->radius);

			ImGui::DragFloat(("Intensity" + id + "_intensity").c_str(), &planet->atmosphere_config.intensity);
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
						
			bool lambda = ImGui::DragFloat3(("Wavelengths" + id + "_wavelengths").c_str(), &planet->atmosphere_config.wavelengths.r);
			bool strength = ImGui::DragFloat(("Scatter Strength" + id + "_scatter_strength").c_str(), &planet->atmosphere_config.scatteringStrength);
			bool coeff = ImGui::DragFloat(("Scatter Coefficient" + id + "_scatter_coefficient").c_str(), &planet->atmosphere_config.scatteringCoefficient);

			if (lambda || strength || coeff) {
				planet->atmosphere_config.UpdateWavelengthScatter();
			}
			ImGui::Unindent();

		}
		Spacing();

		// Ocean Settings
		ImGui::Checkbox(("HasOcean" + id + "_has_ocean").c_str(), & planet->hasOcean);
		if (planet->hasOcean) {
			ImGui::Indent();
			ImGui::DragFloat(("Ocean Radius" + id + "_ocean_radius").c_str(), &planet->ocean_config.radius);
			ImGui::ColorEdit3(("Ocean Colour" + id + "_ocean_colour").c_str(), &planet->ocean_config.oceanColor.r);
			ImGui::ColorEdit3(("Fresnel Colour" + id + "_fresnel_colour").c_str(), &planet->ocean_config.fresnelColor.r);
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
			ImGui::DragFloat(("Normal Repeat" + id + "_normal_repeat").c_str(), &planet->ocean_config.normalRepeat);
			ImGui::DragInt(("Normal Factor" + id + "_normal_factor").c_str(), &planet->ocean_config.normalFactor);
			ImGui::DragFloat(("Normal Strength" + id + "_normal_strength").c_str(), &planet->ocean_config.normalStrength);
			ImGui::DragFloat(("Triplanar Blend" + id + "_triplanar_blend").c_str(), &planet->ocean_config.triplanarBlend);
			ImGui::Unindent();
		}

	}
};