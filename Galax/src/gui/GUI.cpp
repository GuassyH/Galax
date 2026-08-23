#include "GUI.h"
#include "universe/UniverseManager.h"
#include "core/Time.h"
#include "PlanetEditorGUI.h"

namespace GUI {

	void GUI::Init(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsClassic();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	// What is the raw time, number of seconds in a minute, minutes in an hour, etc.
	void DrawTime(float raw_time, float spm, float mph, float hpd, float dpy) {

		// Not flawless
		float raw_minutes = glm::floor(raw_time / spm);
		float raw_hours = glm::floor(raw_minutes / mph);
		float raw_days = glm::floor(raw_hours / hpd);
		float raw_years = glm::floor(raw_days / dpy);

		int seconds = glm::mod(raw_time, spm);
		int minutes = glm::mod(raw_minutes, mph);
		int hours	= glm::mod(raw_hours, hpd);
		int days	= glm::mod(raw_days, dpy);
		int years	= raw_years;

		std::ostringstream time_text; time_text << "Time: ";

		if (years > 0) 
			time_text << years << "y ";
		if (days > 0)
			time_text << days << "d ";
		if (hours > 0)
			time_text << hours << "h ";
		if (minutes > 0)
			time_text << minutes << "m ";
		if (seconds > 0)
			time_text << seconds << "s";

		ImGui::Text(time_text.str().c_str());
	}

	void Header(const char* title) {
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2.0f) - (ImGui::CalcTextSize(title).x / 2.0));
		ImGui::Text(title);
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}


	void GUI::Render(Renderer& renderer, GLFWwindow* window, Player& player) {
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		if (ImGui::Begin("tinker box", nullptr, ImGuiWindowFlags_NoMove)) {
			Header("Simulation");

			ImGui::Checkbox("Simulate", &Universe::UniverseManager::Get().isSimulating);
			ImGui::DragFloat("TimeScale", &Galax::Time::Get().timeScale);
			DrawTime(Universe::UniverseManager::Get().time, 60, 60, 24, 365);
	
			Header("Bodies");

			PlanetEditor::DrawPlanetEditor(renderer, player);



			Header("Vertex SSBO");

			float used_vtx = (MAX_VERTICES * sizeof(Vertex));
			for (int i = 0; i < renderer.freeVertexSlices.size(); i++) {
				used_vtx -= renderer.freeVertexSlices[i].stride;
			}

			float vtx_prc = used_vtx / (MAX_VERTICES * sizeof(Vertex));
			vtx_prc *= 100;
			vtx_prc = floor(vtx_prc * 100) / 100.0f;

			std::ostringstream vtx_ssbo_info; vtx_ssbo_info << "Vertex SSBO: " << vtx_prc << "%%";
			ImGui::Text(vtx_ssbo_info.str().c_str());



			Header("Crater SSBO");

			float used_crtr = MAX_CRATERS * sizeof(TerrainGenerator::Crater);
			for (int i = 0; i < renderer.freeCraterSlices.size(); i++) {
				used_crtr -= renderer.freeCraterSlices[i].stride;
			}

			float crtr_prc = used_crtr / (MAX_CRATERS * sizeof(TerrainGenerator::Crater));
			crtr_prc *= 100;
			crtr_prc = floor(crtr_prc * 100) / 100.0f;

			std::ostringstream crtr_ssbo_info; crtr_ssbo_info << "Crater SSBO: " << crtr_prc << "%%";
			ImGui::Text(crtr_ssbo_info.str().c_str());



			Header("NoiseLayer SSBO");

			float used_nslyr = MAX_NOISE_LAYERS * sizeof(NoiseLayer);
			for (int i = 0; i < renderer.freeNoiseSlices.size(); i++) {
				used_nslyr -= renderer.freeNoiseSlices[i].stride;
			}

			float nslyr_prc = used_nslyr / (MAX_NOISE_LAYERS * sizeof(NoiseLayer));
			nslyr_prc *= 100;
			nslyr_prc = floor(nslyr_prc * 100) / 100.0f;

			std::ostringstream nslyr_ssbo_info; nslyr_ssbo_info << "NoiseLayer SSBO: " << nslyr_prc << "%%";
			ImGui::Text(nslyr_ssbo_info.str().c_str());

		}
		ImGui::End();
	}

	void GUI::NewFrame(GLFWwindow* window) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int mode = 0;
		glfwGetInputMode(window, mode);
	}

	void GUI::EndFrame() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};