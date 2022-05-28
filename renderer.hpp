//
// Created by ColorsWind on 2022/5/12.
//

#pragma once

#include "shader.hpp"

const static void* ZERO_OFFSET = nullptr;
const static GLfloat VERTEX_POS[] = {
         1,  1, -1, 1, 0,
         1, -1, -1, 1, 1,
        -1,  1, -1, 0, 0,
        -1, -1, -1, 0, 1,
};
const static GLuint VERTEX_INDEX[] = {
        0, 1, 2,
        1, 2, 3,
};

class Renderer {
private:
    GLuint vao = -1, vbo = -1, ebo = -1;
    GLuint textureImage = -1, textureLUT = -1;
    Program program;
public:
    Renderer() {
        glUseProgram(program.getProgramId());
        program.addShader("assets/shader/vertex.vsh", GL_VERTEX_SHADER);
        program.addShader("assets/shader/fragment.fsh", GL_FRAGMENT_SHADER);
        program.create();
        glUseProgram(program.getProgramId());
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);


        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POS), VERTEX_POS, GL_STATIC_DRAW);
//        glUseProgram(program.getProgramId());

        glBindVertexArray(vao);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), ZERO_OFFSET);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<const void *>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VERTEX_INDEX), VERTEX_INDEX, GL_STATIC_DRAW);
        glEnable(GL_TEXTURE_2D);

        glGenTextures(1, &textureImage);
        glGenTextures(1, &textureLUT);
        setUniform1("tex_image", 0);
        setUniform1("tex_lut", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureImage);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glActiveTexture(GL_TEXTURE1);


        glBindTexture(GL_TEXTURE_2D, textureLUT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUseProgram(0);
    }

    void setUniform1(const char *name, int value) {
        GLint location = glGetUniformLocation(program.getProgramId(), name);
        if (location < 0) { throw std::runtime_error("Not uniform value named: " + std::string(name)); }
        glUniform1i(location, value);
    }


    void render(Image &picture, Image &imageLUT) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program.getProgramId());
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureLUT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageLUT.getWidth(), imageLUT.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imageLUT.getBuf());


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureImage);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picture.getWidth(), picture.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, picture.getBuf());


        glDrawElements(GL_TRIANGLES, sizeof(VERTEX_INDEX) / sizeof(GLuint), GL_UNSIGNED_INT, ZERO_OFFSET);
        glUseProgram(0);
        glFinish();
    }

    ~Renderer() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }



};