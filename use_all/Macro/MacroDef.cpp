#include "MacroDef.h"

#include <iostream>



namespace emm
{
	constexpr uint32_t HEADER_TYPE_ID = 4;
	constexpr uint32_t HEADER_OBJ_SIZE = 4;
	constexpr uint32_t HEADER_SIZE = HEADER_TYPE_ID + HEADER_OBJ_SIZE;



	static inline void logHexString(const char* d, int size)
	{
		for (int i = 0; i < size; i++)
			std::cout << std::hex << +d[i] << " ";
		std::cout << std::dec << std::endl;
	}


	template<class T >
	T refRemoval(T& a) { return T(); }

	static std::string encode(const char* start,int sizeOfStruct) {
		std::string res;
		if (IsBigEndian())
		{
			for (int i = 0; i < sizeOfStruct; i++)
			{
				res += start[i];
			}
		}
		else
		{
			for (int i = sizeOfStruct-1; i >= 0; i--)
			{
				res += start[i];
			}
		}
		return res;
	}

	// If small endian do something
	static std::string reverseBits(const char* start, int sizeOfStruct) {
		std::string res;
		if (!IsBigEndian())
		{
			for (int i = sizeOfStruct - 1; i >= 0; i--)
			{
				res += start[i];
			}
		}
		else
		{
			res = std::string(start, sizeOfStruct);
		}
		return res;
	}

	template< class T>
	T decodeString(const char* data) {
		int pos = 0;
		std::string stringData = reverseBits(data, sizeof(T));
		
		
		return *(T*)(stringData.data());
	}

	bool IsBigEndian()
	{
		static bool bigEndian = true;
		static bool doOnce = true;
		if (doOnce)
		{
			doOnce = false;
			union {
				uint32_t i;
				char c[4];
			} bint = { 0x01020304 };
			bigEndian = (bint.c[0] == 1);
		}
		return bigEndian;
	}

	template<>
	std::string encodeObject<MacroCommand>(const MacroCommand& command) {
		std::string res;
		

		auto id = TypeTraitId<MacroCommand>();
		int size = sizeof(MacroCommand);

		res += encode((const char*)&id, sizeof(id));
		res += encode((const char*)&size, sizeof(size));

		const char* pRead = (const char*)&command;
		res += encode(pRead,2);
		pRead += 2;
		res += encode(pRead, 4);
		pRead += 4;
		res += encode(pRead, 4);
		pRead += 4;
		res += encode(pRead, 4);
		pRead += 4;

		return res;
	}


	template<>
	MacroCommand decodeObject<MacroCommand>(const char* data, uint32_t size)
	{
		auto LH = [](int size) {if (IsBigEndian())return 0; else return size-1; };
		MacroCommand res;
		auto readRoutine = [&data](auto & member) {
			member = decodeString<decltype(refRemoval(member))>(data);

			data += sizeof(decltype(member));
		};


		uint32_t getIdValue;
		readRoutine(getIdValue);
		if (getIdValue != TypeTraitId<MacroCommand>())
		{
			std::cout << "ID doesn't match up. got " << getIdValue << ", was looking for " << TypeTraitId<MacroCommand>() << std::endl;
			return res;
		}
		int sizeOfObject = -1;
		readRoutine(sizeOfObject);
		if (sizeOfObject != sizeof(MacroCommand))
		{
			std::cout << "size of MacroCommand isn't " << sizeof(MacroCommand) << std::endl;
			return res;
		}

		readRoutine(res.key);
		readRoutine(res.downTimeUs);
		readRoutine(res.amount);
		readRoutine(res.timePeriodUs);

		return res;
	}

	template<>
	std::string encodeObject<MacroDef>(const MacroDef& command) {
		std::string res;


		auto id = TypeTraitId<MacroDef>();
		// This object holds dynamic memory. so we need to calculate it
		int size = ObjectByteSize(command);

		res += encode((const char*)&id, sizeof(id));
		res += encode((const char*)&size, sizeof(size));

		const char* pRead = (const char*)&command;
		res += encode(pRead, 2);
		pRead += 2;
		
		int sizeOfList = (int)command.commands.size();
		res += encode((const char*)&sizeOfList, sizeof(sizeOfList));

		for (const auto & c : command.commands)
		{
			res += encodeObject(c);
		}
		
		return res;
	}


	template<>
	MacroDef decodeObject<MacroDef>(const char* data, uint32_t size)
	{
		uint32_t totalCount = 0;

		auto LH = [](int size) {if (IsBigEndian())return 0; else return size - 1; };
		auto readRoutine = [&data,&size,&totalCount](auto& member) {
			member = decodeString<decltype(refRemoval(member))>(data);
			totalCount += sizeof(decltype(member));
			if (totalCount > size)
				std::cout << "decodeObject<MacroDef> Reading to much data" << std::endl;
			data += sizeof(decltype(member));
		};
		MacroDef res;

		uint32_t getIdValue;
		readRoutine(getIdValue);
		if (getIdValue != TypeTraitId<MacroDef>())
		{
			std::cout << "ID doesn't match up. got " << getIdValue << ", was looking for " << TypeTraitId<MacroDef>() << std::endl;
			return res;
		}
		int sizeOfObject = -1;
		readRoutine(sizeOfObject);

		readRoutine(res.key);
		int sizeOfCommandList;
		readRoutine(sizeOfCommandList);
		
		for (int i = 0; i < sizeOfCommandList; i++)
		{
			MacroCommand c =  decodeObject<MacroCommand>(data, sizeof(MacroCommand));
			res.commands.push_back(std::move(c));
			data += HEADER_SIZE + sizeof(MacroCommand);
		}
		return res;
	}

	template<>
	std::string encodeObject<std::vector<MacroDef>>(const std::vector<MacroDef>& defList) {
		std::string res;


		auto id = TypeTraitId<std::vector<MacroDef>>();
		// This object holds dynamic memory. so we need to calculate it
		int size = ObjectByteSize(defList);

		res += encode((const char*)&id, sizeof(id));
		res += encode((const char*)&size, sizeof(size));

		int sizeOfList = (int)defList.size();
		res += encode((const char*)&sizeOfList, sizeof(sizeOfList));

		for (const auto& c : defList)
		{
			res += encodeObject(c);
		}
		return res;
	}
	template<>
	std::vector<MacroDef> decodeObject<std::vector<MacroDef>>(const char* data, uint32_t size)
	{
		uint32_t totalCount = 0;

		auto LH = [](int size) {if (IsBigEndian())return 0; else return size - 1; };
		auto readRoutine = [&data, &size, &totalCount](auto& member) {
			member = decodeString<decltype(refRemoval(member))>(data);
			totalCount += sizeof(decltype(member));
			if (totalCount > size)
				std::cout << "decodeObject<MacroDef> Reading to much data" << std::endl;
			data += sizeof(decltype(member));
		};
		std::vector<MacroDef> res;

		uint32_t getIdValue;
		readRoutine(getIdValue);
		if (getIdValue != TypeTraitId<std::vector<MacroDef>>())
		{
			std::cout << "ID doesn't match up. got " << getIdValue << ", was looking for " << TypeTraitId<std::vector<MacroDef>>() << std::endl;
			return res;
		}
		int sizeOfObject = -1;
		readRoutine(sizeOfObject);


		int sizeOfLoop = -1;
		readRoutine(sizeOfLoop);

		for (int i = 0; i < sizeOfLoop; i++)
		{
			MacroDef c = decodeObject<MacroDef>(data, size);
			data += (ObjectByteSize(c)) + 8;
			res.push_back(c);
		}
		return res;
	}

}