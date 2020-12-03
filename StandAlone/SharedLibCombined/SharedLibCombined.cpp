#include "SharedLibCombined.h"

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <StandAlone/ResourceLimits.h>
#include <mutex>

extern "C" {
int spirvc_init_process() { return glslang::InitializeProcess(); }
void spirvc_finalize_process() { glslang::FinalizeProcess(); }

Handle spirvc_compile(ShaderCompileInfo* compileInfo)
{
    auto& info = *compileInfo;
    InstancedData* data = new InstancedData(info.shaderType);
    Handle handle = &data->result;
    handle->data = data; // Points back to the whole data. Could be done cleaner...
    auto& result = data->result;

    data->shader.setStringsWithLengthsAndNames(&info.inputCode, nullptr, &info.shadername, 1);
    data->shader.setEnvInput(info.language, info.shaderType, info.client, info.targetClientVersion);
    data->shader.setEnvClient(info.client, info.targetClientVersion);
    data->shader.setEnvTarget(info.targetLanguage, info.targetLanguageVersion);

    for (int i = 0; i < info.localIncludeDirCount; ++i) {
        data->includer.pushExternalLocalDirectory(info.localIncludeDirs[i]);
    }

    result.parseSuccess = data->shader.parse(&glslang::DefaultTBuiltInResource, info.shaderLanguageVersion, true,
                                             info.errorMessages, data->includer);

    result.infoLog = data->shader.getInfoLog();
    result.infoDebugLog = data->shader.getInfoDebugLog();

    if (!result.parseSuccess) {
        return handle;
    }

    data->program.addShader(&data->shader);

    result.linkSuccess = data->program.link(info.errorMessages);

    if (!result.linkSuccess) {
        // Intentionally not using program info log (why? Maybe we wanted to display missing main function?)
        return handle;
    }

    glslang::GlslangToSpv(*data->program.getIntermediate(info.shaderType), data->outCode, &data->spvLogger,
                          &info.spvOptions);

    data->getAllMessages = data->spvLogger.getAllMessages();

    if (!data->outCode.empty()) {
        result.spvResult = data->outCode.data();
        result.spvResultSize = data->outCode.size();
    }

    return handle;
}

void spirvc_free_compilation(Handle handle) { delete handle->data; }
}
