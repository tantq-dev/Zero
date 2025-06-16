#include "DataHandler.h"
#include "BulletModel.h"
#include "utilities/Logger.h"
#include "MonsterModel.h"
#include <memory>
void Tool::DataHandler::ExportBulletConfig(const std::vector<MonsterTypeDefinition*>& monsterDefinitions)
{
	LOG_INFO("Exporting bullet config for monsters" + std::to_string(monsterDefinitions.size()));
    int id = 0;
    for (auto monsterDef : monsterDefinitions)
    {
        if (monsterDef) {
            LOG_INFO("Have monsterDef ");
            for (auto& behavior: monsterDef->defaultProperties.rootBehavior->childBehaviors)
            {
                LOG_INFO("Have child behavior");
            }
        }
    }
}