#include "image.h"
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../stb/stb_image_write.h"

namespace vexed {
    Image::Image() {
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
    }

    Image::Image(const std::string &filepath) {
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
        if(loadFromFile(filepath)) {
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *compressedData, size_t size) {
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        if(loadFromMemory(compressedData, size)) {
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels) {
        this->width = width;
        this->height = height;
        this->channels = channels;
        this->data = new uint8_t[size];
        memcpy(data, uncompressedData, size);
        this->hasLoaded = true;
    }

    Image::Image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) {
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = width;
        this->height = height;
        this->channels = channels;
        if(load(width, height, channels, r, g, b, a)) {
        this->hasLoaded = true;
        }
    }

    Image::~Image() {
        if(data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

    uint8_t *Image::getData() const {
        return data;
    }

    uint32_t Image::getWidth() const {
        return width;
    }

    uint32_t Image::getHeight() const {
        return height;
    }

    uint32_t Image::getChannels() const {
        return channels;
    }

    size_t Image::getDataSize() const {
        return width * height * channels;
    }

    bool Image::isLoaded() const {
        return hasLoaded;
    }

    bool Image::loadFromFile(const std::string &filepath) {
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

        if (uncompressedData) {
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];
            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::loadFromMemory(const uint8_t *data, size_t size) {
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load_from_memory(data, size, &width, &height, &channels, 0);

        if (uncompressedData)     {
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];

            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) {
        if(channels < 3 || channels > 4)
            return false;

        size_t size = width * height * channels;

        if(size == 0)
            return false;

        this->data = new uint8_t[size];

        auto clamp = [] (float value, float min, float max) {
            if(value < min)
                return min;
            else if(value > max)
                return max;
            return value;
        };

        if(channels == 3) {
            for(size_t i = 0; i < size; i += 3) {
                uint8_t r = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t g = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t b = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));

                data[i+0] = r;
                data[i+1] = g;
                data[i+2] = b;
            }
        } else {
            for(size_t i = 0; i < size; i += 4) {
                uint8_t r = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t g = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t b = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));
                uint8_t a = static_cast<uint8_t>(clamp(a * 255, 0.0, 255.0));

                data[i+0] = r;
                data[i+1] = g;
                data[i+2] = b;
                data[i+3] = a;
            }
        }
        
        return true;
    }

    bool Image::saveAsPNG(const std::string &filepath, const void *data, size_t size, size_t width, size_t height, size_t channels) {
        return stbi_write_png(filepath.c_str(), width, height, channels, data, width * channels) > 0;
    }
}