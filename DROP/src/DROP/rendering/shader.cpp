#include "shader.h"

#include <sstream>
#include <string>
#include <unordered_set>

#include "DROP/core/file.h"

#define SHADER_INCLUDE "#include"
#define SHADER_INCLUDE_INIT "\""
#define SHADER_INCLUDE_END "\""
#define SHADER_INCLUDE_FOLDER_DIVIDER '/'

void ShaderHotReloading(Shader* shader)
{
    if (!shader) { return; }

    if (shader->vertex != NULL_SHADER
        && shader->fragment != NULL_SHADER
        && shader->geometry == NULL_SHADER
    ) {
        if (HasNewerWriteTime(&shader->vertexShaderFileTime, shader->vertexShaderFilePath)
            || HasNewerWriteTime(&shader->fragmentShaderFileTime, shader->fragmentShaderFilePath)
        ) {
            shader->ReCompileShader();
        }
    }
    else if (shader->vertex != NULL_SHADER
        && shader->fragment != NULL_SHADER
        && shader->geometry != NULL_SHADER
    ) {
        if (HasNewerWriteTime(&shader->vertexShaderFileTime, shader->vertexShaderFilePath)
            || HasNewerWriteTime(&shader->fragmentShaderFileTime, shader->fragmentShaderFilePath)
            || HasNewerWriteTime(&shader->geometryShaderFileTime, shader->geometryShaderFilePath)
        ) {
            shader->ReCompileShader();
        }
    }
    else if (shader->compute != NULL_SHADER)
    {
        if (HasNewerWriteTime(&shader->computeShaderFileTime, shader->computeShaderFilePath))
        {
            shader->ReCompileShader();
        }
    }
}

bool HasNewerWriteTime(
    FileTime* lastFileTime
    , char* filePath
) {
    bool bHasNewerFileWriteTime = false;

    if (!lastFileTime) return bHasNewerFileWriteTime;
    if (!filePath) return bHasNewerFileWriteTime;

    FileTime newLastFileTime = File::GetLastWriteTime(filePath);

    if (CompareFileTime(&newLastFileTime, lastFileTime) != 0)
    {
        bHasNewerFileWriteTime = true;
    }
    
    return bHasNewerFileWriteTime;
}

void ResolveShaderPreProcessorIncludes(
    GLchar* shaderPath
    , size_t shaderPathSize
    , std::string* shaderCode
) {
    if (!shaderCode) return;
    if (!shaderPath) return;

    // Cached read fun files
    std::unordered_set <std::string> funFiles;

    // parse all the Shader code line by line until string end
    //std::istringstream f(*shaderCode);
    std::string line;
    int32_t lineNumber = 0;
    size_t pos = 0;
    size_t shaderCodeSize = shaderCode->size();
    while ((pos = shaderCode->find(SHADER_INCLUDE, pos)) != std::string::npos)
    {
        std::string functionFileName = "";

        // Pos the first "#include"
        // Parse until the first '"' of the line
        assert(pos + 1 < shaderCodeSize);
        size_t includeInit = shaderCode->find(SHADER_INCLUDE_INIT, pos + 1);
        if (includeInit == std::string::npos) continue;

        // Parse until the last '"' of the line
        assert(includeInit + 1 < shaderCodeSize);
        size_t includeEnd = shaderCode->find(SHADER_INCLUDE_INIT, includeInit + 1);
        if (includeEnd == std::string::npos) continue;
        
        size_t offset = includeEnd - (includeInit+1);
        functionFileName = shaderCode->substr(includeInit+1, offset);

        if (funFiles.find(functionFileName) != funFiles.end())
        {
            // #pragma once
            // Remove that line
            shaderCode->replace(
                pos // offset
                , (includeEnd + 1) - pos // How may characters to replace
                , ""
            );
            continue;
        }
        funFiles.insert(functionFileName);

        std::string includeShaderFunctionCode;
        ReadIncludeFunctionFromFile(
            &functionFileName
            , &includeShaderFunctionCode
            , shaderPath
            , shaderPathSize
        );

        shaderCode->replace(
            pos // offset
            , (includeEnd + 1) - pos // How may characters to replace
            , includeShaderFunctionCode
        );

        pos = includeEnd;
    }
}

// Assuming the shader function file is in the same folder of the shader
void ReadIncludeFunctionFromFile(
    std::string* shaderFunctionName
    , std::string* outIncludeShaderFunctionCode
    , GLchar* shaderPath
    , size_t shaderPathSize
) {
    // obtain the folder
    size_t folderPos = 0;
    for (size_t i = shaderPathSize; i > 0 && folderPos == 0; i--)
    {
        if ((char)shaderPath[i] == SHADER_INCLUDE_FOLDER_DIVIDER)
        {
            folderPos = i;
        }
    }

    // *(char (*)[77])shaderPath
    GLchar* folderPath = &shaderPath[folderPos];
    std::string filePath(shaderPath);
    filePath = filePath.substr(0, folderPos+1);
    filePath.append(*shaderFunctionName);

    // Read the file
    File::ReadTextFile(
        &filePath
        , outIncludeShaderFunctionCode
    );

    //
}