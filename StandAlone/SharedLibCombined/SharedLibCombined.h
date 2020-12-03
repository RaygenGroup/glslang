// Created in the Raygen fork of glslang.
// Intended usage is for the specific internal project.

// Exposes a simple api without function name mangling for compiling glsl code to spirv
// This is not intended to be a C api, but it is consumed both sides by a C++ compiler.
// We build this as a release dynamic library and load it at runtime in debug builds.

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>

//
// NOTE: might be beneficial to use the already provided c api (defined in glsl_c_interface)
// when they enable spv options. Or implement it and pull request.
//
struct ShaderCompileInfo {
    // Missing the ability to link multiple shaders in a single program (we don't use this anyway)
    // Missing the ability to use different than DefaultBuiltInResource
    // (if we need this, use the one provided in the original glsl_c_interface api)

    //
    // Create Info Section
    //
    EShLanguage shaderType;
    const char* inputCode;
    const char* shadername;

    // Defaults tailored for most up-to-dateL: glsl -> spv vulkan.
    int shaderLanguageVersion = 460;
    glslang::EShSource language = glslang::EShSourceGlsl;
    glslang::EShClient client = glslang::EShClientVulkan;
    glslang::EShTargetClientVersion targetClientVersion = glslang::EShTargetVulkan_1_2;
    glslang::EShTargetLanguage targetLanguage = glslang::EshTargetSpv;
    glslang::EShTargetLanguageVersion targetLanguageVersion = glslang::EShTargetSpv_1_5;
    //
    // Parse Info Section (uses DefaultTBuiltInResource for now)
    //
    const char* const* localIncludeDirs = nullptr;
    int localIncludeDirCount = 0;

    // Used for parse & link
    EShMessages errorMessages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    // Link has everything from above

    //
    // Generate SPV Section
    //

    // spvOptions is a struct that contains booleans only (no stl structures)
    glslang::SpvOptions spvOptions;
};

struct InstancedData;
struct ShaderCompileResult {
    InstancedData* data; // Don't touch this over dynamic library boundary (should be opaque actually)

    unsigned int* spvResult = nullptr;
    // in unsigned int count (not bytes)
    int spvResultSize = 0;

    bool parseSuccess = false;
    bool linkSuccess = false;

    const char* infoLog = nullptr;
    const char* infoDebugLog = nullptr;
    const char* allMessages = nullptr;
};

struct InstancedData {
    DirStackFileIncluder includer;
    glslang::TShader shader;
    glslang::TProgram program;

    spv::SpvBuildLogger spvLogger;
    std::string getAllMessages;
    ShaderCompileResult result;
    std::vector<unsigned int> outCode;

    InstancedData(EShLanguage shaderType) : shader(shaderType) {}
};

using Handle = ShaderCompileResult*;

extern "C" {
// Init process on the dll
int spirvc_init_process();

// Deinit process on the dll
void spirvc_finalize_process();

// Parses, Links, Generates SPV in a single call.
// MUST cleanup with spirvc_free_compilation afterwards.
Handle spirvc_compile(ShaderCompileInfo* compileInfo);

// cleanup resources after a compilation.
void spirvc_free_compilation(Handle handle);
}
