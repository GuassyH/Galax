#pragma once

#include "glm/vec2.hpp"

#include <unordered_set>
#include <iostream>

namespace Galax {
	class InputManager {
	private:
		InputManager() = default;
	public:
		static InputManager& Get() {
			static InputManager instance;
			return instance;
		}

		// ==== INPUT ====

		// Store an unordered set of the keys (O(1) lookup time)
		std::unordered_set<int> pressedKeys;
		std::unordered_set<int> justPressedKeys;
		std::unordered_set<int> justReleasedKeys;

		std::unordered_set<int> pressedMouseButtons;
		std::unordered_set<int> justPressedMouseButtons;
		std::unordered_set<int> justReleasedMouseButtons;

		glm::vec2 scrollOffset = glm::vec2(0.0f);
		glm::vec2 mousePosition = glm::vec2(0.0f);

		// ==== SCREEN ====
		glm::vec2 windowSize = glm::vec2(0.0f);
		glm::vec2 monitorSize = glm::vec2(0.0f);

		// KEYBOARD

		void KeyPressed(int keycode) {
			if (!pressedKeys.contains(keycode)) {
				pressedKeys.insert(keycode);
				justPressedKeys.insert(keycode);
			}
		}
		void KeyReleased(int keycode) {
			if (pressedKeys.contains(keycode)) {
				pressedKeys.erase(keycode);
				justReleasedKeys.insert(keycode);
			}
		}

		// MOUSE

		void MouseButtonPressed(int button) {
			if (!pressedKeys.contains(button)) {
				pressedMouseButtons.insert(button);
				justPressedMouseButtons.insert(button);
			}
		}
		void MouseButtonReleased(int button) {
			if (pressedMouseButtons.contains(button)) {
				pressedMouseButtons.erase(button);
				justReleasedMouseButtons.insert(button);
			}
		}
		void SetMousePosition(float x, float y) {
			mousePosition.x = x;
			mousePosition.y = y;
		}
		void MouseScrolled(float xOffset, float yOffset) {
			scrollOffset.x = xOffset;
			scrollOffset.y = yOffset;
		}

		// GENERAL

		void Clear() { // Is called at the end of the frame

			// Reset scroll, since scroll is an instant thing
			scrollOffset = glm::vec2(0.0f);

			// Clear just pressed keys, keys that were pressed THIS frame should not carry on
			justPressedKeys.clear();
			justPressedMouseButtons.clear();

			justReleasedKeys.clear();
			justReleasedMouseButtons.clear();
		}
	};

}

namespace Galax::Input {
	// Easy to call functions, just say "if(Input::IsKeyPressed(YOUR_KEY))"
	inline bool AnyKeyPressed() {
		auto& input = InputManager::Get();
		return !input.pressedKeys.empty();
	}
	inline bool IsKeyPressed(int keycode) {
		auto& input = InputManager::Get();
		return input.pressedKeys.contains(keycode);
	}
	inline bool IsKeyJustPressed(int keycode) {
		auto& input = InputManager::Get();
		return input.justPressedKeys.contains(keycode);
	}
	inline bool IsKeyJustReleased(int keycode) {
		auto& input = InputManager::Get();
		return input.justReleasedKeys.contains(keycode);
	}

	inline bool AnyMouseButtonPressed() {
		auto& input = InputManager::Get();
		return !input.pressedMouseButtons.empty();
	}
	inline bool IsMouseButtonPressed(int button) {
		auto& input = InputManager::Get();
		return input.pressedMouseButtons.contains(button);
	}
	inline bool IsMouseButtonJustPressed(int button) {
		auto& input = InputManager::Get();
		return input.justPressedMouseButtons.contains(button);
	}
	inline bool IsMouseButtonJustReleased(int button) {
		auto& input = InputManager::Get();
		return input.justReleasedMouseButtons.contains(button);
	}

	// Self explanatory
	inline glm::vec2 GetMousePosition() {
		auto& input = InputManager::Get();
		return input.mousePosition;
	}

	// x is left & right, y is up & down
	inline glm::vec2 GetScrollOffset() {
		auto& input = InputManager::Get();
		return input.scrollOffset;
	}
}

namespace Galax::Screen {
	inline glm::vec2 GetWindowSize() {
		auto& screen = InputManager::Get();
		return screen.windowSize;
	}
	inline glm::vec2 GetMonitorSize() {
		auto& screen = InputManager::Get();
		return screen.monitorSize;
	}
}

#define GX_ADD_KEY_PRESSED(...) ::Galax::InputManager::Get().KeyPressed(__VA_ARGS__)
#define GX_ADD_KEY_RELEASED(...) ::Galax::InputManager::Get().KeyReleased(__VA_ARGS__)

#define GX_ADD_MOUSE_BUTTON_PRESSED(...) ::Galax::InputManager::Get().MouseButtonPressed(__VA_ARGS__)
#define GX_ADD_MOUSE_BUTTON_RELEASED(...) ::Galax::InputManager::Get().MouseButtonReleased(__VA_ARGS__)
