#include "shader.h"
#include "../glad/glad.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace vexed {
    Shader::Shader() : id(0) {
    }
    
    bool Shader::load(const std::string &vertexSource, const std::string &fragmentSource) {
        if(id > 0)
            return false;

        const GLchar* vertex_shader[1] = {
            vertexSource.c_str()
        };

        const GLchar* fragment_shader[1] = {
            fragmentSource.c_str()
        };

        GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_handle, 1, vertex_shader, nullptr);
        glCompileShader(vert_handle);
        checkShader(vert_handle, "vertex shader");

        GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_handle, 1, fragment_shader, nullptr);
        glCompileShader(frag_handle);
        bool check1 = checkShader(frag_handle, "fragment shader");

        id = glCreateProgram();
        glAttachShader(id, vert_handle);
        glAttachShader(id, frag_handle);
        glLinkProgram(id);
        bool check2 = checkProgram(id, "shader program");

        glDetachShader(id, vert_handle);
        glDetachShader(id, frag_handle);
        glDeleteShader(vert_handle);
        glDeleteShader(frag_handle);

        return (check1 == true && check2 == true);
    }

    bool Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath) {
        auto readFileToString = [](const std::string &path) -> std::string {
            std::ifstream file(path);
            if (!file) {
                throw std::runtime_error("Failed to open file: " + path);
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        };

        try {
            std::string vertexSource = readFileToString(vertexPath);
            std::string fragmentSource = readFileToString(fragmentPath);
            return load(vertexSource, fragmentSource);
        } catch (const std::runtime_error &e) {
            return false;
        }
    }
    
    void Shader::destroy() {
        if(id > 0) {
            glDeleteProgram(id);
            id = 0;
        }
    }

    bool Shader::checkShader(uint32_t handle, const std::string &description) {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to compile %s!\n", description.c_str());
        if (log_length > 1) {
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, nullptr, &buf[0]);
            fprintf(stderr, "%s\n", buf.begin());
        }

        return status == GL_TRUE;
    }

    bool Shader::checkProgram(GLuint handle, const std::string &description) {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to link %s!n", description.c_str());
        if (log_length > 1) {
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, nullptr, &buf[0]);
            fprintf(stderr, "%s\n", buf.begin());
        }
        return status == GL_TRUE;
    }

    void Shader::setFloat(const char *name, float value) {
        glUniform1f(glGetUniformLocation(id, name), value);
    }

    void Shader::setFloat2(const char *name, const float *value) {
        glUniform2fv(glGetUniformLocation(id, name), 1, value);
    }
}