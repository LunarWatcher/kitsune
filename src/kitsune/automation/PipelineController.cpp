#include "PipelineController.hpp"
#include "kitsune/api/LuaApi.hpp"

namespace kitsune {

void PipelineController::loadPipelines(LuaApi* api) {
    this->api = api;

    for (auto& lookupDir : conf.pipelineLookupLocations) {
        if (!std::filesystem::is_directory(lookupDir)) {
            minilog::debug("{} is not a folder", lookupDir);
            continue;
        }

        minilog::debug("Pipelines: loading from {}", lookupDir);
        
        for (const auto& entry : std::filesystem::directory_iterator(lookupDir)) {
            if (entry.is_directory()) {
                continue;
            }
            auto path = entry.path();
            if (path.extension() != ".lua") {
                continue;
            }

            api->run(path);
        }
    }
}

void PipelineController::start() {
    
}

}
