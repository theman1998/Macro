#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace emm
{

	std::optional<uint16_t> getKeyboardValue(uint16_t key);

	bool isMouseCode(uint16_t key);
	/// win api contains mouse click with keyboard, but must convert to mouse code
	uint16_t convertVKeyToMouseDown(uint16_t key);
	uint16_t convertVKeyToMouseUp(uint16_t key);


}