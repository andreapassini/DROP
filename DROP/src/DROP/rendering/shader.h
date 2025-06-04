/*
Shader class
- loading Shader source code, Shader Program creation

N.B. ) adaptation of https://github.com/JoeyDeVries/LearnOpenGL/blob/master/includes/learnopengl/shader.h

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2022/2023
Master degree in Computer Science
Universita' degli Studi di Milano
*/

#pragma once

using namespace std;

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// to be removed
#include <filesystem>
#include <DROP/utils/Log.h>


class Shader
{
public:
    GLuint Program;


    //constructor
    Shader(
        const GLchar* const vertexPath
        , const GLchar* const fragmentPath
    )
    {
        LOG_CORE_TRACE(
            "Vertex: {0}, \nFragment: {1}"
            , vertexPath
            , fragmentPath
        );

        // Step 1: we retrieve shaders source code from provided filepaths
        string vertexCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream fShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << e.what() << ", " << e.code().message() << e.code().value() << endl;
            cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
        }

        // Convert strings to char pointers
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        // Step 2: we compile the shaders
        GLuint vertex, fragment;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // check compilation errors
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // check compilation errors
        checkCompileErrors(fragment, "FRAGMENT");

        // Step 3: Shader Program creation
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        // check linking errors
        checkCompileErrors(this->Program, "PROGRAM");

        // Step 4: we delete the shaders because they are linked to the Shader Program, and we do not need them anymore
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader(
        const GLchar* const vertexPath
        , const GLchar* const geometryPath
        , const GLchar* const fragmentPath
    )
    {
        LOG_CORE_TRACE(
            "Vertex: {0}, \nGeometry: {1}, \nFragment: {2}"
            , vertexPath
            , geometryPath
            , fragmentPath
        );


        // Step 1: we retrieve shaders source code from provided filepaths
        string vertexCode;
        string geometryCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream gShaderFile;
        ifstream fShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            gShaderFile.open(geometryPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, gShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            gShaderStream << gShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            gShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            geometryCode = gShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << e.what() << ", " << e.code().message() << e.code().value() << endl;
            cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
        }

        // Convert strings to char pointers
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* gShaderCode = geometryCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();


        // Step 2: we compile the shaders
        GLuint vertex, geometry, fragment;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // check compilation errors
        checkCompileErrors(vertex, "VERTEX");

        // Geometry Shader
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        // check compilation errors
        checkCompileErrors(geometry, "GEOMETRY");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // check compilation errors
        checkCompileErrors(fragment, "FRAGMENT");


        // Step 3: Shader Program creation
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, geometry);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        // check linking errors
        checkCompileErrors(this->Program, "PROGRAM");

        // Step 4: we delete the shaders because they are linked to the Shader Program, and we do not need them anymore
        glDeleteShader(vertex);
        glDeleteShader(geometry);
        glDeleteShader(fragment);
    }

    Shader(
        const GLchar* const computePath
    )
    {
        LOG_CORE_TRACE(
            "Compute: {0}"
            , computePath
        );


        // Step 1: we retrieve shaders source code from provided filepaths
        string computeCode;
        ifstream cShaderFile;

        // ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            // Open files
            cShaderFile.open(computePath);
            stringstream cShaderStream;
            // Read file's buffer contents into streams
            cShaderStream << cShaderFile.rdbuf();
            // close file handlers
            cShaderFile.close();
            // Convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (ifstream::failure e)
        {
            cout << e.what() << ", " << e.code().message() << e.code().value() << endl;
            cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
        }

        // Convert strings to char pointers
        const GLchar* cShaderCode = computeCode.c_str();

        // Step 2: we compile the shaders
        GLuint compute;

        // Compute Shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        // check compilation errors
        checkCompileErrors(compute, "COMPUTE");

        // Step 3: Shader Program creation
        this->Program = glCreateProgram();
        glAttachShader(this->Program, compute);
        glLinkProgram(this->Program);
        // check linking errors
        checkCompileErrors(this->Program, "PROGRAM");

        // Step 4: we delete the shaders because they are linked to the Shader Program, and we do not need them anymore
        glDeleteShader(compute);
    }
    
    // We activate the Shader Program as part of the current rendering process
    void Use() { glUseProgram(this->Program); }

    // We delete the Shader Program when application closes
    void Delete() { glDeleteProgram(this->Program); }

private:

    // Check compilation and linking errors
    void checkCompileErrors(GLuint shader, string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if(type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
			}
		}
	}
};
