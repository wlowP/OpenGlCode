//
// Created by ROG on 2025/4/3.
//

#include <iostream>

#include "Texture.h"

// stb_image要求定义宏STB_IMAGE_IMPLEMENTATION才能用
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//=====================================================================
//====================调用OpenGL的自动实现MipMap========================
//=====================================================================

Texture::Texture(const std::string& path, const int textureUnit) {
    // 1. stbImage 读取图片文件
    int channels;
    // 反转Y轴(因为OpenGL纹理的坐标系是左下角为原点, 而图片文件的坐标系是左上角为原点)
    stbi_set_flip_vertically_on_load(true);
    // 读取图片文件, 并且全部转换为RGBA格式
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    // 2. 创建并绑定纹理对象
    glGenTextures(1, &texture);
    // 激活纹理单元0 (虽然默认情况下也会激活0) (默认一共有0-15, 共16个纹理单元)
    // GL_TEXTURE0宏本身的值虽然不是0, 但是跟GL_TEXTURE1, 2..都是连续的, 于是可以GL_TEXTURE0 + 1来表示1号纹理单元
    this->textureUnit = textureUnit;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    // 绑定纹理对象 (到gl状态机的GL_TEXTURE_2D插槽)
    // 📌📌同时还会将纹理对象自动绑定到当前激活的纹理单元上
    glBindTexture(GL_TEXTURE_2D, texture);

    // 3. 传输纹理数据到GPU (会开辟GPU内存)
    // 如果没有绑定纹理对象, 则传输数据会失败
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 📌调用OpenGL的自动生成MipMap
    glGenerateMipmap(GL_TEXTURE_2D);

    // 📌别忘了释放图片数据(已经传输到GPU, 内存中就不需要了)
    stbi_image_free(data);

    // 4. 设置纹理参数
    // 纹理过滤方式. 图片被放大时采用插值, 缩小时就不插值(取临近点像素)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 📌📌手动实现MipMap: 别忘了设置当图片被缩小时的MipMap过滤方式
    // 最后一个参数为两个参数的组合: (GL_NEAREST|GL_LINEAR)_(MIPMAP_LINEAR|NEAREST)
    // GL_NEAREST: 在单个MipMap级别内使用最邻近采样
    // MIPMAP_LINEAR: 在判断MipMap级别时, 采用线性插值(当前计算的MipMap级别为小数时, 取其前后两个级别进行线性插值)
    // 效果: MIPMAP_LINEAR: 图片变小时会产生渐变切换效果; MIPMAP_NEAREST: 图片变小时会突然切换到下一个MipMap级别
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    // 纹理环绕方式
    // 纹理坐标超出范围时, 采用重复的方式(也是默认方式)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::~Texture() {
    // 删除纹理对象
    glDeleteTextures(1, &texture);
}

void Texture::bindTexture() const {
    // 先切换激活的纹理单元, 然后绑定纹理对象
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::bindTexture(const int textureUnit) {
    this->textureUnit = textureUnit;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

GLuint Texture::TextureFromFile(const char *path, const std::string &directory) {
    auto filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
