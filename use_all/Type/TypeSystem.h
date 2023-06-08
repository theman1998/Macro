#pragma once

#include <map>
#include <cstdint>
#include <string>
#include <optional>

#include <vector>




namespace emm
{
	

	template<class T>
	std::string TypeTraitName() {
		return "";
	}
	template<class T>
	std::string TypeTraitName(T)
	{
		return TypeTraitName<T>();
	}
	template<class T>
	uint32_t TypeTraitId(){
		return 0;
	}
	
	
	template<class T>
	uint32_t TypeTraitId(T){
		return TypeTraitId<T>();
	}
	
	

	template<class T>
	uint32_t ObjectByteSize(const T&)
	{
		return sizeof(T);
	}




	class TypeSystem
	{
		using ID = uint32_t;
		static inline std::map<ID, std::string> mTypdIdToStringMap;
		static inline std::map<std::string, ID> mStringToTypeIDMap;
	public:
		static std::optional<std::string> LookUpName(ID id)
		{
			std::optional<std::string> res;
			if (mTypdIdToStringMap.find(id) != mTypdIdToStringMap.end())
				res = mTypdIdToStringMap[id];
			return res;
		}
		static std::optional<ID> LookUpId(const std::string & name)
		{
			std::optional<ID> res;
			if (mStringToTypeIDMap.find(name) != mStringToTypeIDMap.end())
				res = mStringToTypeIDMap[name];
			return res;
		}

		template <class T>
		static ID RegisterClass(ID desireID = 0)
		{
			if (mStringToTypeIDMap.find(TypeTraitName<T>()) != mStringToTypeIDMap.end())
				return mStringToTypeIDMap.at(TypeTraitName<T>());

			bool found = (mTypdIdToStringMap.find(desireID) != mTypdIdToStringMap.end());
			while (found)
			{
				desireID++;
				found = (mTypdIdToStringMap.find(desireID) != mTypdIdToStringMap.end());
			}
			mTypdIdToStringMap[desireID] = TypeTraitName<T>();
			mStringToTypeIDMap[TypeTraitName<T>()] = desireID;

			return desireID;
		}

		static std::string toString() {
			std::string res;
			for (const auto& [id, name] : mTypdIdToStringMap)
			{
				res += std::to_string(id) + "=" + name + ",";
			}
			if (!res.empty())
				res.pop_back();
			return res;
		}

	};

	




}