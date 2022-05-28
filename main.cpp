#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include "shader.hpp"
#include "renderer.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#undef STB_IMAGE_WRITE_IMPLEMENTATION


class Config {
public:
    std::filesystem::path sourceDir;
    std::vector<std::filesystem::path> sourceFilters;
    Image sourceImage;
    std::filesystem::path outputDir;
    int outputWidth;
    int outputHeight;


    Config(int argc, char **argv) {
        for(int i=1;i<argc;i+=2) {
            std::string key = argv[i];
            if (key == "-picture") {
                sourceImage.open(argv[i+1], true);
            } else if (key == "-scale") {
                double scale = std::stod(argv[i+1]);
                outputWidth = static_cast<int>(scale * sourceImage.getWidth());
                outputHeight = static_cast<int>(scale * sourceImage.getHeight());
            } else if (key == "-output") {
                outputDir = argv[i+1];
                if (!exists(outputDir)) { create_directories(outputDir); }
            } else if (key == "-filters") {
                sourceDir = argv[i + 1];
                for(auto && current : std::filesystem::recursive_directory_iterator(sourceDir)) {
                    if (!current.is_directory()) {sourceFilters.emplace_back(current); }
                }
            }
        }
    }
};

void callback(int error_code, const char* description) {
    std::cerr << "Error occurs: " << error_code << "\n" << description << std::endl;
}

int main(int argc, char **argv) {
    Config config(argc, argv);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(config.outputWidth, config.outputHeight, "LUT Renderer (HIDDEN WINDOWS)", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, config.outputWidth, config.outputHeight);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSetErrorCallback(callback);
    Renderer renderer;
    Image lutImage;
    nlohmann::json json;
    auto *imageBuf = new std::byte[config.outputWidth * config.outputHeight * 4];
    for(auto && filter : config.sourceFilters) {
        lutImage.open(absolute(filter).c_str(), false);
        renderer.render(config.sourceImage, lutImage);
        glReadPixels(0, 0, config.outputWidth, config.outputHeight, GL_RGB, GL_UNSIGNED_BYTE, imageBuf);
        std::filesystem::path outputPicture = config.outputDir / relative(filter, config.sourceDir).replace_extension("jpg");
        std::filesystem::path outputFilter = outputPicture.parent_path() / filter.stem().concat("_lut.jpg");
        if (!exists(outputPicture.parent_path())) {create_directories(outputPicture.parent_path()); }
        bool ret = stbi_write_jpg(outputPicture.c_str(), config.outputWidth, config.outputHeight, 3, imageBuf, 85);
        std::cout << "Write: " << outputPicture  << " Status: " << ret << std::endl;
//        ret = stbi_write_jpg(outputFilter.c_str(), lutImage.getWidth(), lutImage.getHeight(), 3, lutImage.getBuf(), 100);
        std::cout << "Write: " << outputFilter  << " Status: " << ret << std::endl;
        json[outputPicture.parent_path().filename()].push_back({
            {"image", relative(outputPicture, config.outputDir).string()},
            {"lut", relative(outputFilter, config.outputDir).string()}
        });

    }
    std::ofstream jsonHandle;
    jsonHandle.open(config.outputDir / "meta.json", std::ios::out);
    jsonHandle << json.dump(2);
    jsonHandle.flush();
    jsonHandle.close();
    delete[] imageBuf;
    glfwTerminate();
    exit(EXIT_SUCCESS);
}