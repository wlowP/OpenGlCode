//
// Created by ROG on 2025/4/3.
//

#include "Texture.h"

// stb_image要求定义宏STB_IMAGE_IMPLEMENTATION才能用
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    // 📌别忘了释放图片数据(已经传输到GPU, 内存中就不需要了)
    stbi_image_free(data);

    // 4. 设置纹理参数
    // 纹理过滤方式. 图片被放大时采用插值, 缩小时就不插值(取临近点像素)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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