#include "font.h"
#include "../glad/glad.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cuchar>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../stb/stb_truetype.h"

namespace vexed {
    Font::Font() {
        texture = 0;
        pixelSize = 14;
        textureWidth = 512;
        textureHeight = 512;
    }

    Font::Font(const std::string &filepath, uint32_t pixelSize) {
        this->pixelSize = pixelSize;
        texture = 0;
        loadFromFile(filepath);
    }

    Font::Font(const uint8_t *fontData, uint32_t pixelSize) {
        this->pixelSize = pixelSize;
        texture = 0;
        loadFromMemory(fontData);
    }

    bool Font::load(const std::string &filepath, uint32_t pixelSize) {
        if(texture > 0) //already loaded
            return false;
        this->pixelSize = pixelSize;
        texture = 0;
        return loadFromFile(filepath);
    }

    bool Font::load(const uint8_t *fontData, uint32_t pixelSize) {
        if(texture > 0) //already loaded
            return false;
        this->pixelSize = pixelSize;
        texture = 0;
        return loadFromMemory(fontData);
    }

    void Font::destroy() {
        if(texture > 0) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
    }

    float Font::computeLineHeight(const std::string &text, float fontSize) {
        if(text.size() == 0)
            return 0;

        float height = 0;

        for(auto c : text) {
            if(c == '\n') {
                return height;
            }
            
            int index = c - codePointOfFirstChar;
            
            if(index >= characters.size())
                continue;
            
            auto glyph = &characters[index];

            float glyphHeight = (glyph->y1 - glyph->y0);
            
            if (glyphHeight > height) {
                height = glyphHeight;
            }
        }

        return height * (fontSize / getPixelSize());
    }

    float Font::computeTextWidth(const std::string &text, float fontSize) {
        if(text.size() == 0)
            return 0;
        
        float maxWidth = 0.0f;
        float currentWidth = 0.0f;
        const uint32_t codePointOfFirstChar = getCodePointOfFirstChar();
        const auto &packedChars = getCharacters();

        for (char ch : text) {
            if (ch == '\n') {
                maxWidth = std::max(maxWidth, currentWidth);
                currentWidth = 0.0f;
            } else {
                int glyphIndex = (ch - codePointOfFirstChar);
                if (glyphIndex >= 0 && glyphIndex < getNumberOfCharacters()) {
                    auto packedChar = &packedChars[glyphIndex];
                    currentWidth += packedChar->xadvance;
                }
            }
        }

        maxWidth = std::max(maxWidth, currentWidth); // Check last line
        return maxWidth * (fontSize / getPixelSize());
    }

    void Font::computeCursorPosition(const std::string &text, size_t cursorIndex, float fontSize, float &x, float &y) {
        if(text.size() == 0)
            return;

        fontSize = fontSize / getPixelSize();
        float startPosX = x;
        float startPosY = y;
        float cursorPosX = x;
        float cursorPosY = y;

        const uint32_t codePointOfFirstChar = getCodePointOfFirstChar();
        auto &packedChars = getCharacters();

        // Calculate the cursor position based on the cursorIndex
        for (int i = 0; i < cursorIndex; ++i) {
            char ch = text[i];

            // Handle line breaks
            if (ch == '\n') {
                cursorPosX = startPosX; // Reset X position for a new line
                cursorPosY += getLineHeight() * fontSize;
                continue;
            }

            int glyphIndex = (ch - codePointOfFirstChar);
            // Skip unknown glyphs
            if (glyphIndex < 0 || glyphIndex >= getNumberOfCharacters()) {
                continue;
            }

            auto packedChar = &packedChars[glyphIndex];

            // Update the cursor position based on the glyph's xadvance
            cursorPosX += packedChar->xadvance * fontSize;
        }

        x = cursorPosX;
        y = cursorPosY;
    }

    float Font::computeTextHeight(const std::string &text, float fontSize) {
        int lineCount = 0;

        for(char ch : text) {
            if(ch == '\n') {
                lineCount++;
                continue;
            }
        }

        //If no new line characters exist, return default value
        if(lineCount == 0)
            return getLineHeight() * (fontSize / getPixelSize());

        return lineCount * (getLineHeight() * (fontSize / getPixelSize()));
    }

    float Font::getStringWidth(const std::string &text, uint32_t from, uint32_t to, float fontSize) {
        int width = 0;

        int pCodePoint = 0;
        int pAdvancedWidth = 0;
        int pLeftSideBearing = 0;

        int i = from;
        while (i < to) {
            i += getCodePoint(text, to, i, pCodePoint);
            int cp = pCodePoint;

            stbtt_GetCodepointHMetrics(&fontInfo, cp, &pAdvancedWidth, &pLeftSideBearing);
            width += pAdvancedWidth;
        }

        fontSize = fontSize / getPixelSize();

        return width * stbtt_ScaleForPixelHeight(&fontInfo, fontSize);
    }

    int Font::getCodePoint(const std::string &text, int to, int i, int32_t &cpOut) {
        char32_t c1 = text[i];
        if (isHighSurrogate(c1) && i + 1 < to) {
            char32_t c2 = text[i + 1];
            if (isLowSurrogate(c2)) {
                cpOut = (c1 << 10) + c2 - 0x35FDC00; // Combining high and low surrogate to get code point
                return 2;
            }
        }
        cpOut = c1;
        return 1;
    }

    bool Font::isHighSurrogate(char32_t c) {
        return (c >= 0xD800 && c <= 0xDBFF);
    }

    bool Font::isLowSurrogate(char32_t c) {
        return (c >= 0xDC00 && c <= 0xDFFF);
    }

    bool Font::loadFromFile(const std::string &filepath) {
        std::ifstream inputStream(filepath.c_str(), std::ios::binary | std::ios::ate);
        if (!inputStream) {
            std::cerr << "Failed to open font file\n";
            return false;
        }

        std::streamsize fontFileSize = inputStream.tellg();
        inputStream.seekg(0, std::ios::beg);

        std::vector<uint8_t> fontDataBuf;
        fontDataBuf.resize(fontFileSize);

        inputStream.read((char*)fontDataBuf.data(), fontFileSize);

        uint8_t *buffer = fontDataBuf.data();

        return load(fontDataBuf.data());
    }

    bool Font::loadFromMemory(const uint8_t *fontData) {
        if(!fontData) {
            std::cerr << "Failed to read font data because it is null\n";
            return false;
        }
        return load(fontData);
    }

    bool Font::load(const uint8_t *data) {
        uint32_t fontCount = stbtt_GetNumberOfFonts(data);

        if(!stbtt_InitFont(&fontInfo, data, 0)) {
            std::cerr << "stbtt_InitFont() Failed!\n";
            return false;
        }

        const uint32_t fontAtlasWidth = 512;
        const uint32_t fontAtlasHeight = 512;

        textureWidth = fontAtlasWidth;
        textureHeight = fontAtlasHeight;

        codePointOfFirstChar = 32;                     // ASCII of ' '(Space)
        const uint32_t charsToIncludeInFontAtlas = 95; // Include 95 charecters

        std::vector<uint8_t> fontAtlasTextureData;
        std::vector<stbtt_packedchar> packedChars;
        std::vector<stbtt_aligned_quad> alignedQuads;

        fontAtlasTextureData.resize(fontAtlasWidth * fontAtlasHeight);
        packedChars.resize(charsToIncludeInFontAtlas);
        alignedQuads.resize(charsToIncludeInFontAtlas);
        characters.resize(charsToIncludeInFontAtlas);
        quads.resize(charsToIncludeInFontAtlas);

        float fontSize = (float)pixelSize;
        
        stbtt_pack_context ctx;

        stbtt_PackBegin(
            &ctx,                                     // stbtt_pack_context (this call will initialize it) 
            fontAtlasTextureData.data(),              // Font Atlas texture data
            fontAtlasWidth,                           // Width of the font atlas texture
            fontAtlasHeight,                          // Height of the font atlas texture
            0,                                        // Stride in bytes
            1,                                        // Padding between the glyphs
            nullptr);

        stbtt_PackFontRange(
            &ctx,                                     // stbtt_pack_context
            data,                                     // Font Atlas texture data
            0,                                        // Font Index                                 
            fontSize,                                 // Size of font in pixels. (Use STBTT_POINT_SIZE(fontSize) to use points) 
            codePointOfFirstChar,                     // Code point of the first charecter
            charsToIncludeInFontAtlas,                // No. of charecters to be included in the font atlas 
            packedChars.data()                        // stbtt_packedchar array, this struct will contain the data to render a glyph
        );

        stbtt_PackEnd(&ctx);

        for (int i = 0; i < charsToIncludeInFontAtlas; i++) {
            float unusedX, unusedY;

            stbtt_GetPackedQuad(
                packedChars.data(),                  // Array of stbtt_packedchar
                fontAtlasWidth,                      // Width of the font atlas texture
                fontAtlasHeight,                     // Height of the font atlas texture
                i,                                   // Index of the glyph
                &unusedX, &unusedY,                  // current position of the glyph in screen pixel coordinates, (not required as we have a different corrdinate system)
                &alignedQuads[i],                    // stbtt_alligned_quad struct. (this struct mainly consists of the texture coordinates)
                0                                    // Allign X and Y position to a integer (doesn't matter because we are not using 'unusedX' and 'unusedY')
            );
        }

        memcpy(characters.data(), packedChars.data(), packedChars.size() * sizeof(stbtt_packedchar));
        memcpy(quads.data(), alignedQuads.data(), alignedQuads.size() * sizeof(stbtt_aligned_quad));

        lineHeight = 0.0f;

        //Estimate line height based on the maximum height of all glyphs
        for (const auto &glyph : characters) {
            float glyphHeight = (glyph.y1 - glyph.y0);
            if (glyphHeight > lineHeight) {
                lineHeight = glyphHeight;
            }
        }

        // // Scale factors
        // float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

        // // Get font metrics
        // int ascent, descent, lineGap;
        // stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

        // lineHeight = (ascent - descent + lineGap) * scale;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // The given texture data is a single channel 1 byte per pixel data 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontAtlasWidth, fontAtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, fontAtlasTextureData.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }
}