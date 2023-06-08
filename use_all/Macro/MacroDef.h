#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>


#include <Type/TypeSystem.h>

namespace emm
{
	constexpr char MACRO_FILE[] = "C:\\CodeProjects\\MacroText.txt";

	/// Key codes are found from win32 api 
	namespace vk
	{
		constexpr uint16_t BUTTON_LEFT = 0X01;
		constexpr uint16_t BUTTON_RIGHT = 0X02;
		constexpr uint16_t CANCLE = 0X03;
		constexpr uint16_t BUTTON_MIDDLE = 0X04;
		constexpr uint16_t BACKSPACE = 0X08;
		constexpr uint16_t TAB = 0X09;
		constexpr uint16_t CLEAR = 0X0C;
		constexpr uint16_t ENTER = 0X0D;
		constexpr uint16_t SHIFT = 0X10;
		constexpr uint16_t CONTROL = 0X11;
		constexpr uint16_t MENU = 0X12;
		constexpr uint16_t PAUSE = 0X13;
		constexpr uint16_t ESCAPE = 0X1B;
		constexpr uint16_t SPACE = 0X20;
		constexpr uint16_t HOME = 0X24;
		constexpr uint16_t ARROW_LEFT = 0X25;
		constexpr uint16_t ARROW_UP = 0X26;
		constexpr uint16_t ARROW_RIGHT = 0X27;
		constexpr uint16_t ARROW_DOWN = 0X28;
		constexpr uint16_t DELETE = 0X2E;

		constexpr uint16_t F12 = 0X7B;
	}


	bool IsBigEndian();

	#pragma pack(push, 1)
	struct MacroCommand{
		/// Target key in this command
		uint16_t key{0x00};
		/// Amount of time this key will be held down
		uint32_t downTimeUs{0};
		/// How many times this key will pressed during this command
		uint32_t amount{0};
		/// Time period between button presses(This is the inverse of frequency). counted after downTime
		uint32_t timePeriodUs{0};

		bool operator==(const MacroCommand& obj) { return (key == obj.key && downTimeUs == obj.downTimeUs && amount == obj.amount && timePeriodUs == obj.timePeriodUs); }
		std::string toString() const {
			std::string res;
			res = "key=" + std::string((char*)&key,1) + ",downTimeMS=" + std::to_string(downTimeUs) + ", amount=" + std::to_string(amount) + ", breakTimeMs=" + std::to_string(timePeriodUs);
			return res;
		}
	};
	#pragma pack(pop)



	struct MacroDef {
		/// Physical key on device, pressed my human
		uint16_t key{ 0x00 };
		/// Virtual keys pressed by OS.
		std::vector<MacroCommand> commands;
	};


	template<class T>
	std::string encodeObject(const T& command);
	template<class T>
	T decodeObject(const char * data, uint32_t size );



	template<>
	inline std::string TypeTraitName<MacroCommand>()
	{
		return "MacroCommand";
	}
	template<>
	inline std::string TypeTraitName<MacroDef>()
	{
		return "MacroDef";
	}
	template<>
	inline uint32_t TypeTraitId<MacroCommand>()
	{
		static auto value = TypeSystem::RegisterClass<MacroCommand>(1000);
		return value;
	}
	template<>
	inline uint32_t TypeTraitId<MacroDef>()
	{
		static auto value = TypeSystem::RegisterClass<MacroDef>(1001);
		return value;
	}

	template<>
	inline std::string TypeTraitName<std::vector<MacroDef>>()
	{
		return "MacroDefList";
	}

	template<>
	inline uint32_t TypeTraitId<std::vector<MacroDef>>() {
		static auto value = TypeSystem::RegisterClass<MacroDef>(1001);
		return value;
	}

	template<>
	inline uint32_t ObjectByteSize<MacroDef>(const MacroDef&obj)
	{
		return 2 + 4 + ((uint32_t)obj.commands.size() * (sizeof(MacroCommand)+8)); // key + listSize + list contents * header size
	}

	template<>
	inline uint32_t ObjectByteSize<std::vector<MacroDef>>(const std::vector<MacroDef>& obj)
	{
		uint32_t size = 4;
		for (const auto& item : obj)
			size += ObjectByteSize(item);
		return size; // key + listSize + list contents
	}





	template<>
	std::string encodeObject<MacroCommand>(const MacroCommand& command);

	template<>
	MacroCommand decodeObject<MacroCommand>(const char* data, uint32_t size);

	template<>
	std::string encodeObject<MacroDef>(const MacroDef& command);

	template<>
	MacroDef decodeObject<MacroDef>(const char* data, uint32_t size);

	template<>
	std::string encodeObject<std::vector<MacroDef>>(const std::vector<MacroDef>& defList);

	template<>
	std::vector<MacroDef> decodeObject<std::vector<MacroDef>>(const char* data, uint32_t size);


	inline int xxxxx = 1234;




	inline std::string ReadFile(const std::string& fileName)
	{
		std::ifstream input_file(fileName);
		if (!input_file.is_open()) {
			std::cerr << "Could not open the file - '" << fileName << "'" << std::endl;
			exit(EXIT_FAILURE);
		}
		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}

	inline bool OutputFile(const std::string& fileName, const char* data, int amount)
	{
		std::ofstream  outputFile(fileName);
		if (!outputFile.is_open()) {
			std::cerr << "Could not open the file - '" << fileName << "'" << std::endl;
			return false;
		}
		outputFile.write(data, amount);
		return true;
	}


}