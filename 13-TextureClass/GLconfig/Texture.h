//
// Created by ROG on 2025/4/3.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include "core.h"
#include <string>

/**
 * 纹理类
 */
class Texture {
public:
    /**
     * 创建纹理对象
     * @param path 文件路径
     * @param textureUnit 要绑定的纹理单元号
     */
    Texture(const std::string& path, int textureUnit);
    ~Texture();

    // 将纹理对象绑定到当前激活的纹理单元上
    void bindTexture() const;
    // 将纹理对象绑定到指定的纹理单元上
    void bindTexture(int textureUnit);

    int getWidth() const;
    int getHeight() const;

private:
    GLuint texture{0}; // OpenGL纹理对象
    int width{0}; // 纹理宽高
    int height{0};

    // 绑定的纹理单元的偏移量 (GL_TEXTURE0 + textureUnit = 实际绑定的纹理单元号)
    int textureUnit{0};
};

#endif //TEXTURE_H
