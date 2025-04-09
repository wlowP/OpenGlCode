//
// Created by ROG on 2025/4/3.
//

#ifndef TEXTUREMIPMAP_H
#define TEXTUREMIPMAP_H
#include "core.h"
#include <string>

/**
 * 纹理类, 包含对MipMap的手动实现
 */
class TextureMipMapManual {
public:
    /**
     * 创建纹理对象
     * @param path 文件路径
     * @param textureUnit 要绑定的纹理单元号
     */
    TextureMipMapManual(const std::string& path, int textureUnit);
    ~TextureMipMapManual();

    // 将纹理对象绑定到当前激活的纹理单元上
    void bindTexture() const;
    // 将纹理对象绑定到指定的纹理单元上
    void bindTexture(int textureUnit);

private:
    GLuint texture{0}; // OpenGL纹理对象
    int width{0}; // 纹理宽高
    int height{0};

    // 绑定的纹理单元的偏移量 (GL_TEXTURE0 + textureUnit = 实际绑定的纹理单元号)
    int textureUnit{0};
};

/**
 * 纹理类, 调用了OpenGL的自动MipMap实现
 */
class TextureMipMap {
public:
    /**
     * 创建纹理对象
     * @param path 文件路径
     * @param textureUnit 要绑定的纹理单元号
     */
    TextureMipMap(const std::string& path, int textureUnit);
    ~TextureMipMap();

    // 将纹理对象绑定到当前激活的纹理单元上
    void bindTexture() const;
    // 将纹理对象绑定到指定的纹理单元上
    void bindTexture(int textureUnit);

private:
    GLuint texture{0}; // OpenGL纹理对象
    int width{0}; // 纹理宽高
    int height{0};

    // 绑定的纹理单元的偏移量 (GL_TEXTURE0 + textureUnit = 实际绑定的纹理单元号)
    int textureUnit{0};
};

#endif //TEXTUREMIPMAP_H
