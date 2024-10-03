#ifndef VEXED_FONT_H
#define VEXED_FONT_H

#include "../../stb/stb_truetype.h"
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace vexed {
    struct PackedChar
    {
        unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
        float xoff, yoff, xadvance;
        float xoff2, yoff2;
    };

    struct AlignedQuad
    {
        float x0, y0, s0, t0; // top-left
        float x1, y1, s1, t1; // bottom-right
    };

    class Font {
    public:
        Font();
        Font(const Font &other);
        bool load(const std::string &filepath, uint32_t pixelSize);
        bool load(const uint8_t *fontData, uint32_t pixelSize);
        void destroy();
        float computeLineHeight(const std::string &text, float fontSize);
        float computeTextWidth(const std::string &text, float fontSize);
        float computeHeightOfBiggestCharacter(const std::string &text, float fontSize);
        float computeTextHeight(const std::string &text, float fontSize);
        float getStringWidth(const std::string &text, uint32_t from, uint32_t to, float fontSize);
        int getCodePoint(const std::string &text, int to, int i, int32_t &cpOut);
        void computeCursorPosition(const std::string &text, size_t cursorIndex, float fontSize, float &x, float &y);
        inline uint32_t getPixelSize() const { return pixelSize; }
        inline uint32_t getTexture() const { return texture; }
        inline uint32_t getTextureWidth() const { return textureWidth; }
        inline uint32_t getTextureHeight() const { return textureHeight; }
        inline std::vector<PackedChar> &getCharacters() { return characters; }
        inline std::vector<AlignedQuad> &getQuads() { return quads; }
        inline uint32_t getCodePointOfFirstChar() { return codePointOfFirstChar; }
        inline float getLineHeight() const { return lineHeight; }
        inline size_t getNumberOfCharacters() const { return characters.size(); }
        static Font *add(const std::string &name, const Font &font);
        static Font *find(const std::string &name);
        static void remove(const std::string &name, const Font &font);
    private:
        stbtt_fontinfo fontInfo;
        uint32_t pixelSize;
        uint32_t texture;
        uint32_t codePointOfFirstChar;
        uint32_t textureWidth;
        uint32_t textureHeight;
        float lineHeight;
        std::vector<PackedChar> characters;
        std::vector<AlignedQuad> quads;
        static std::unordered_map<std::string,Font> fonts;
        bool loadFromFile(const std::string &filepath);
        bool loadFromMemory(const uint8_t *fontData);
        bool load(const uint8_t *data);
        static bool isHighSurrogate(char32_t c);
        static bool isLowSurrogate(char32_t c);
    };
}

#endif
