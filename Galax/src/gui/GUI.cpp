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


	void GUI::Render(GLFWwindow* window) {
		if (ImGui::Begin("tinker box")) {
			ImGui::Checkbox("Simulate", &Universe::UniverseManager::Get().isSimulating);
			ImGui::DragFloat("TimeScale", &Galax::Time::Get().timeScale);
			DrawTime(Universe::UniverseManager::Get().time, 60, 60, 24, 365);
			ImGui::Separator();

			PlanetEditor::DrawPlanetEditor();
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