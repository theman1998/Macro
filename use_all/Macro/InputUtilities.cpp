#include "InputUtilities.h"
#include "MacroDef.h"

namespace emm
{

	std::optional<uint16_t> getKeyboardValue(uint16_t key)
	{
		std::optional<uint16_t> res;
		if (key >= 97 && key <= 127)
		{
			res = key - 32;//a starts at 65 for key bindings
		}
		else if (key >= 65 && key <= 90)
		{
			res = key;
		}
		else if (key >= 43 && key <= 47)
		{
			res = key + 64;
		}
		else if (key >= 48 && key <= 57)
		{
			res = key + 48;
		}
		else
			res = key;

		return res;
	}

	bool isMouseCode(uint16_t key)
	{
		return key == vk::BUTTON_LEFT || key == vk::BUTTON_RIGHT || key == vk::BUTTON_MIDDLE;
	}


	uint16_t convertVKeyToMouseDown(uint16_t key)
	{
		if (vk::BUTTON_LEFT == key)
			return 0x0002;
		else if (vk::BUTTON_RIGHT == key)
			return 0x0008;
		else if (vk::BUTTON_MIDDLE == key)
			return 0x0020;
		return 0;
	}
	uint16_t convertVKeyToMouseUp(uint16_t key)
	{
		if (vk::BUTTON_LEFT == key)
			return 0x0004;
		else if (vk::BUTTON_RIGHT == key)
			return 0x0010;
		else if (vk::BUTTON_MIDDLE == key)
			return 0x0040;
		return 0;
	}

}