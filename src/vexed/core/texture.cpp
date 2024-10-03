#include "texture.h"
#include "../../glad/glad.h"
#include <stdexcept>

namespace vexed {
    Texture::Texture() 
        :id(0), width(width), height(height), channels(channels) {}

    void Texture::destroy() {
        if(id > 0) {
            glDeleteTextures(1, &id);
            id = 0;
        }
    }

    bool Texture::load(Image *image, const TextureSettings *settings) {
        this->width = image->getWidth();
        this->height = image->getHeight();
        this->channels = image->getChannels();

        if(loadData(image->getData(), settings)) {
            return true;
        }
        return false;
    }

    bool Texture::loadData(const void *data, const TextureSettings *settings) {
        if (data) {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            if(!settings) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings->wrapS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings->wrapT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings->minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings->magFilter);
            }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            switch(channels) {
                case 1:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                    break;
                case 2:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
                    break;
                case 3:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    break;
                case 4:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    break;
                default:
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDeleteTextures(1, &id);
                    std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(channels);
                    throw std::runtime_error(error.c_str());
                    break;
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            return true;
        }

        return false;
    }
}