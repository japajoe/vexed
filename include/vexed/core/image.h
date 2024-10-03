#ifndef VEXED_IMAGE_H
#define VEXED_IMAGE_H

#include <string>
#include <cstdint>

namespace vexed {
    class Image {
    public:
        Image();
        Image(const std::string &filepath);
        Image(const uint8_t *compressedData, size_t size);
        Image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels);
        Image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
        ~Image();
        uint8_t *getData() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getChannels() const;
        size_t getDataSize() const;
        bool isLoaded() const;
        static bool saveAsPNG(const std::string &filepath, const void *data, size_t size, size_t width, size_t height, size_t channels);
    private:
        uint8_t *data;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        bool hasLoaded;
        bool loadFromFile(const std::string &filepath);
        bool loadFromMemory(const uint8_t *data, size_t size);
        bool load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
    };
}

#endif