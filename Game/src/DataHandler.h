#pragma once
//#include "json.hpp"
#include "MonsterModel.h"
namespace Tool {
	class DataHandler {

	public:
		DataHandler() = default;
		~DataHandler() = default;
		void ExportBulletConfig(const std::vector<MonsterTypeDefinition*>& monsterDefinitions);

	};
}