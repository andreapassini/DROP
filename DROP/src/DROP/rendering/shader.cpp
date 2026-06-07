#include "shader.h"

#include "DROP/core/file.h"

void ShaderHotReloading(Shader* shader)
{
    if (!shader) { return; }

    if (shader->vertex != NULL_SHADER
        && shader->fragment != NULL_SHADER
        && shader->geometry == NULL_SHADER
    ) {
        if (HasNewerWriteTime(shader, shader->vertex)
            || HasNewerWriteTime(shader, shader->fragment)
        ) {
            shader->ReCompileShader();
        }
    }
    else if (shader->vertex != NULL_SHADER
        && shader->fragment != NULL_SHADER
        && shader->geometry != NULL_SHADER
    ) {
        if (HasNewerWriteTime(shader, shader->vertex)
            || HasNewerWriteTime(shader, shader->fragment)
            || HasNewerWriteTime(shader, shader->geometry)
        ) {
            shader->ReCompileShader();
        }
    }
    else if (shader->compute != NULL_SHADER)
    {
        if (HasNewerWriteTime(shader, shader->compute))
        {
            shader->ReCompileShader();
        }
    }
}

bool HasNewerWriteTime(
    Shader* shader
    , char* filePath
) {
    FileTime lastFileTime = File::GetLastWriteTime(filePath);
    return false;
}