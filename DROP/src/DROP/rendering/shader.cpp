#include "shader.h"

#include "DROP/core/file.h"

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