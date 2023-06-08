#include "Controller.h"
#include <filesystem>
#include <WinUser.h>


enum class InputType {
	Mouse =0,
	Keyboard =1,
	Hardware =2
};

constexpr char DEFAULT_FILE[] = "MacroInputsText.txt";

bool CreateKeyboardInput(uint16_t theKey, INPUT_MOTION & bufArray)
{
	//!KEYEVENTF_KEYUP means down
	auto key = emm::getKeyboardValue(theKey);
	if (!key.has_value())
		return false;
	// Set up a generic keyboard event.
	bufArray[0].type = INPUT_KEYBOARD;
	bufArray[0].ki.wScan = 0; // hardware scan code for key
	bufArray[0].ki.time = 0;
	bufArray[0].ki.dwExtraInfo = 0;
	bufArray[0].ki.wVk = *key; // virtual-key code for the "E" key
	bufArray[0].ki.dwFlags = 0; // 0 for key press  
	// moving up from keystoke
	bufArray[1].type = INPUT_KEYBOARD;
	bufArray[1].ki.wScan = 0; // hardware scan code for key
	bufArray[1].ki.time = 0;
	bufArray[1].ki.dwExtraInfo = 0;
	bufArray[1].ki.wVk = *key; // virtual-key code for the "E" key
	bufArray[1].ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
	return true;
}

bool CreateMouseInput(uint16_t theKey, INPUT_MOTION& bufArray)
{

	if (!emm::isMouseCode(theKey))
		return false;
	//mouse
	bufArray[0].type = INPUT_MOUSE;
	bufArray[0].mi.time = 0;
	bufArray[0].mi.dwExtraInfo = 0;
	bufArray[1].type = INPUT_MOUSE;
	bufArray[1].mi.time = 0;
	bufArray[1].mi.dwExtraInfo = 0;
	// Release the "Left button down" key
	bufArray[0].mi.dwFlags = emm::convertVKeyToMouseDown(theKey); // KEYEVENTF_KEYUP for key release
	bufArray[1].mi.dwFlags = emm::convertVKeyToMouseUp(theKey); // KEYEVENTF_KEYUP for key release
	return true;
}

Controller::Controller()
{

	auto currentPath = std::filesystem::current_path();
	std::string file = currentPath.string() + "\\" + DEFAULT_FILE;
	std::cout << "Starting MACRO DAEMON" << std::endl;
	std::cout << "Load Macro Text file to: " << file << std::endl;

	mMacroList.clear();
	auto data = emm::ReadFile(file);
	auto defList = emm::decodeObject<std::vector<emm::MacroDef>>(data.data(), (uint32_t)data.size());
	for (const auto& item : defList)
	{
		mMacroList[item.key] = item;
	}
	
}

void Controller::start()
{
	mShutdown.store(false);


	
	mThreadForceShutdown = std::thread(&Controller::ForceShutDownRoutine, &mShutdown);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	mThreadKeyboardWatcher = std::thread(&Controller::WatchKeyboardRoutine, &mShutdown, mMacroList);


	mThreadForceShutdown.join();
	mThreadKeyboardWatcher.join();

}


void Controller::WatchKeyboardRoutine(const Flag *shutdown, const std::map<uint16_t, emm::MacroDef> watchList)
{
	std::string errorString;
	std::vector<MACRO_INPUT> macroPairList;

	for (const auto& [key, value] : watchList)
	{
		MACRO_INPUT inputItem;
		if (!CreateKeyboardInput(value.key, inputItem.phyiscalKey))
		{
			errorString += "Couldn't create physical " + std::to_string(value.key) +".";
			continue;
		}
		bool didBreak = false;
		for (const auto& c : value.commands)
		{
			MACRO_COMBO inputCommand;
			bool passState = true;
			if (emm::isMouseCode(c.key))
			{
				passState = CreateMouseInput(c.key, inputCommand.inputKey);
			}
			else
			{
				passState = CreateKeyboardInput(c.key, inputCommand.inputKey);
			}
			if (!passState)
			{
				errorString += "Couldn't create virtual " + std::string((char*)&c.key, 1) + " for " + std::string((char*)&value.key, 1) + ".";
				didBreak = true;
				break;
			}
			inputCommand.amount = c.amount;
			inputCommand.downTime = std::chrono::microseconds(c.downTimeUs);
			inputCommand.timePeriod = std::chrono::microseconds(c.timePeriodUs);
			inputItem.virtualKeys.push_back(inputCommand);
		}
		if (didBreak)
			continue;

		macroPairList.emplace_back(inputItem);
	}

	if (!errorString.empty())
		std::cout << "ERROR: " << errorString << std::endl;


	auto VirtualRoutineLambda = [](MACRO_COMBO & c) {

		for (int i = 0; i < c.amount; i++)
		{
			SendInput(1, &c.inputKey[0], sizeof(INPUT));
			std::this_thread::sleep_for(c.downTime);
			SendInput(1, &c.inputKey[1], sizeof(INPUT));
			std::this_thread::sleep_for(c.timePeriod);
		}
	};

	std::cout << "Turon on/off the macro watcher with: '~'" << std::endl;
	bool doWatchPhysical = false;
	while (!shutdown->load())
	{
		if (GetAsyncKeyState(VK_OEM_3))
		{
			while ((GetAsyncKeyState(VK_OEM_3))) {}//wait tell user lets go off key
			doWatchPhysical = !doWatchPhysical;
			std::string state = (doWatchPhysical ? "On" : "Off");
			std::cout << "Macro Watcher is " << state << std::endl;
		}

		if (!doWatchPhysical)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			continue;
		}

		for (auto& inputCommand : macroPairList)
		{
			if (!(GetAsyncKeyState(inputCommand.phyiscalKey[0].ki.wVk) && 1))
				continue;
			for(auto & vInput : inputCommand.virtualKeys)
				VirtualRoutineLambda(vInput);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));//clears buffer
	}
}

void Controller::ForceShutDownRoutine(Flag* shutdown)
{
	INPUT_MOTION theForceShutdownInput;
	CreateKeyboardInput(emm::vk::F12,theForceShutdownInput);
	std::cout << "To shut down, press: F12" << std::endl;
	while (!shutdown->load())
	{
		if (GetAsyncKeyState(emm::vk::F12))
		{
			shutdown->store(true);
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}
	std::cout << "Shuting down" << std::endl;
}