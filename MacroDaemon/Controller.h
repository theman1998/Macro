#pragma once

#include "Macro/InputUtilities.h"
#include "GlobalData.h"


#include <iostream>
#include <array>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

#include <windows.h>
#define WINVER 0x0500


/// pressing down and up
using INPUT_MOTION = std::array<INPUT, 2>;

struct MACRO_COMBO
{
	INPUT_MOTION inputKey;
	std::chrono::microseconds timePeriod;
	std::chrono::microseconds downTime;
	uint32_t amount;
};

struct MACRO_INPUT {
	INPUT_MOTION phyiscalKey;
	std::vector<MACRO_COMBO> virtualKeys;
};

class Controller
{
public:
	Controller();
	void start();

private:
	using Flag = std::atomic<bool>;
	Flag mShutdown{ false };

	std::map<uint16_t, emm::MacroDef> mMacroList;


	std::thread mThreadKeyboardWatcher;
	std::thread mThreadForceShutdown;

	static void WatchKeyboardRoutine(const Flag *shutdown, const std::map<uint16_t, emm::MacroDef> watchList);
	static void ForceShutDownRoutine(Flag* shutdown);
};