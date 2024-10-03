#ifndef VEXED_TEXTURE_H
#define VEXED_TEXTURE_H

#include "image.h"
#include <cstdint>
#include <cstdlib>
#include <string>

namespace vexed {
    enum TextureWrapMode{
        TextureWrapMode_Repeat = 0x2901,
        TextureWrapMode_ClampToBorder = 0x812D,
        TextureWrapMode_ClampToEdge = 0x812F,
        TextureWrapMode_MirroredRepeat = 0x8370,
        TextureWrapMode_MirrorClampToEdge = 0x8743
    };

    enum TextureMinFilter {
        TextureMinFilter_Nearest = 0x2600,
        TextureMinFilter_Linear = 0x2601,
        TextureMinFilter_NearestMipMapNearest = 0x2700,
        TextureMinFilter_LinearMipMapNearest = 0x2701,
        TextureMinFilter_NearestMipMapLinear = 0x2702,
        TextureMinFilter_LinearMipMapLinear = 0x2703
    };

    enum TextureMagFilter {
        TextureMagFilter_Nearest = 0x2600,
        TextureMagFilter_Linear = 0x2601,
    };

    struct TextureSettings {
        TextureMinFilter minFilter;
        TextureMagFilter magFilter;
        TextureWrapMode wrapS;
        TextureWrapMode wrapT;
    };

    class Texture {
    public:
        Texture();
        inline uint32_t getId() const { return id; }
        inline uint32_t getWidth() const { return width; }
        inline uint32_t getHeight() const { return height; }
        inline uint32_t getChannels() const { return channels; }
        void destroy();
        bool load(Image *image, const TextureSettings *settings = nullptr);
    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        bool loadData(const void *data, const TextureSettings *settings);
    };
}

#endif