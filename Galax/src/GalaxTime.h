
#pragma once

#include <chrono>
#include "Log.h"

class GalaxTime {
private:
	double lastFrameTime = 0.0f;
	int nFrames = 0;
	float frameTimer = 0.0f;

	GalaxTime() :
		lastFrameTime(static_cast<double>(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000.0)
	{
	}
public:
	GalaxTime(const GalaxTime&) = delete;
	GalaxTime& operator=(const GalaxTime&) = delete;

	static GalaxTime& get() {
		static GalaxTime instance;
		return instance;
	}
	void update() {
		// Delta Time Calculation
		double current_time = static_cast<double>(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000.0;
		deltaTime = static_cast<float>(current_time - lastFrameTime);

		frameTimer += deltaTime;

		nFrames++;
		if (frameTimer >= 1.0f) {
			framerate = nFrames;
			nFrames = 0;
			frameTimer = 0.0f;
		}


		lastFrameTime = current_time;
	}


	double GetTime() const {
		return static_cast<double>(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000.0;
	}

	double GetTimeHP() const {
		return static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000000.0;
	}

	float deltaTime = 0.0f;
	int framerate = 0;
};
