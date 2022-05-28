#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
static std::string toString(GLuint type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "VERTEX";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT";
        default:
            throw std::runtime_error("Unknown type" + std::to_string(type));
    }
}

class Image {
private:
    std::shared_ptr<unsigned char> buf;
    int width;
    int height;
    int channel;
public:
    void open(const char *filename, bool flip=false) {
        stbi_set_flip_vertically_on_load(flip);
        buf = std::shared_ptr<unsigned char>(stbi_load(filename, &width, &height, &channel, 3),
                [](unsigned char* ptr){ stbi_image_free(ptr); });
        if (!buf || !width || !height || channel != 3) {
            throw std::runtime_error("Unable to open: " + std::string(filename));
        }
    }

    [[nodiscard]] const unsigned char* getBuf() const {
        return buf.get();
    }

    [[nodiscard]] int getWidth() const {
        return width;
    }

    [[nodiscard]] int getHeight() const {
        return height;
    }

    [[nodiscard]] int getChannel() const {
        return channel;
    }
};


class Program {
private:
    GLuint programId;
    std::vector<GLint> shadersId;
public:
    Program() {
        programId = glCreateProgram();
    }

    void addShader(const char *filename, GLuint type) {
        glUseProgram(programId);
        GLint shaderId = glCreateShader(type);
        std::ifstream handle;
        handle.open(filename, std::ios::in);
        if (handle.fail()) {
            throw std::runtime_error("Cannot open: " + std::string(filename));
        }
        std::stringstream ss;
        ss << handle.rdbuf();
        std::string content = ss.str();
        const char *tmp = content.c_str();
        glShaderSource(shaderId, 1, &tmp, nullptr);
        glCompileShader(shaderId);
        GLint success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            char buffer[1024] = {};
            glGetShaderInfoLog(shaderId, sizeof(buffer) / sizeof(char), nullptr, buffer);
            std::cerr << "ERRO1::PROGRAM_COMPILE_ERROR of type: " << toString(type) << "\n" << buffer
                      << "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(buffer);
        }
        glAttachShader(programId, shaderId);
        shadersId.emplace_back(shaderId);
        handle.close();
    }

    void create() {
        glLinkProgram(programId);
        char buffer[1024];
        GLint success;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, sizeof(buffer) / sizeof(char), nullptr, buffer);
            std::cerr << "ERRO1::PROGRAM_LINK_ERROR of type: "  << "\n" << buffer
                      << "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(buffer);
        }
        for(auto && shaderId : shadersId) {
            glDeleteShader(shaderId);
        }
    }

    [[nodiscard]] GLuint getProgramId() const {
        return programId;
    }

};