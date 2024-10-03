#ifndef VEXED_GRAPHICS_H
#define VEXED_GRAPHICS_H

#include "font.h"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>

namespace vexed {
    struct Vector2 {
        float x;
        float y;
        Vector2() 
            : x(0), y(0){}
        Vector2(float x, float y) 
            : x(x), y(y) {}
    };

    struct Rectangle {
        float x;
        float y;
        float width;
        float height;
        Rectangle() 
            : x(0), y(0), width(0), height(0) {}
        Rectangle(float x, float y, float width, float height) 
            : x(x), y(y), width(width), height(height) {}
        bool isZero() const {
            return x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f;
        }
        static Rectangle getRectAtRowAndColumn(float leftIndent, float topIndent, float width, float height, int row, int column, int offsetX = 0, int offsetY = 0) {
            float x = leftIndent + (column * (width + offsetX));
            float y = topIndent + (row * (height + offsetY));
            return Rectangle(x, y, width, height);
        }
    };

    struct Color {
        float r;
        float g;
        float b;
        float a;
        Color() 
            : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(float r, float g, float b, float a) 
            : r(r), g(g), b(b), a(a) {
            auto normalize = [] (float x) {
                if(x > 1.0f)
                    x /= 255.0f;
                return x;
            };

            this->r = normalize(r);
            this->g = normalize(g);
            this->b = normalize(b);
            this->a = normalize(a);
        }

        static Color lightGray() {
            return { 200, 200, 200, 255 };
        }

        static Color gray() {
            return { 130, 130, 130, 255 };
        }

        static Color darkGray() {
            return { 80, 80, 80, 255 };
        }

        static Color yellow() {
            return { 253, 249, 0, 255 };
        }

        static Color gold()
        {
            return { 255, 203, 0, 255 };
        }

        static Color orange()
        {
            return { 255, 161, 0, 255 };
        }

        static Color pink() {
            return { 255, 109, 194, 255 };
        }

        static Color red() {
            return { 255, 0, 0, 255 };
        }

        static Color maroon() {
            return { 190, 33, 55, 255 };
        }

        static Color green() {
            return { 0, 255, 0, 255 };
        }

        static Color lime() {
            return { 0, 158, 47, 255 };
        }

        static Color lightGreen() {
            return { 13, 224, 77, 255 };
        }

        static Color darkGreen() {
            return { 0, 117, 44, 255 };
        }

        static Color skyBlue() {
            return { 102, 191, 255, 255 };
        }

        static Color blue() {
            return { 0, 0, 255, 255 };
        }

        static Color darkBlue() {
            return { 0, 82, 172, 255 };
        }

        static Color purple() {
            return { 200, 122, 255, 255 };
        }

        static Color violet() {
            return { 135, 60, 190, 255 };
        }

        static Color darkPurple() {
            return { 112, 31, 126, 255 };
        }

        static Color beige() {
            return { 211, 176, 131, 255 };
        }

        static Color brown() {
            return { 127, 106, 79, 255 };
        }

        static Color darkBrown() {
            return { 76, 63, 47, 255 };
        }

        static Color white() {
            return { 255, 255, 255, 255 };
        }

        static Color black() {
            return { 0, 0, 0, 255 };
        }

        static Color blank() {
            return { 0, 0, 0, 0 };
        }

        static Color magenta() {
            return { 255, 0, 255, 255 };
        }

        static Color transparent() {
            return { 0, 0, 0, 0 };
        }
    };

    struct Vertex {
        Vector2 position;
        Vector2 uv;
        Color color;
        Vertex() : 
            position(Vector2(0, 0)), 
            uv(Vector2(0, 0)), 
            color(Color(1.0f, 1.0f, 1.0f, 1.0f)) {}

        Vertex(const Vector2 &position, const Vector2 &uv) :
            position(position), 
            uv(uv),
            color(Color(1.0f, 1.0f, 1.0f, 1.0f)) {}

        Vertex(const Vector2 &position, const Vector2 &uv, const Color &color) :
            position(position), 
            uv(uv), 
            color(color) {}
    };

    struct DrawListItem {
        uint32_t shaderId;
        uint32_t textureId;
        size_t vertexOffset;
        size_t vertexCount;
        size_t indiceCount;
        size_t indiceOffset;
        bool textureIsFont;
        Rectangle clippingRect;
        void *userData;
    };

    struct DrawCommand {
        Vertex *vertices;
        size_t numVertices;
        uint32_t *indices;
        size_t numIndices;
        uint32_t textureId;
        uint32_t shaderId;
        bool textureIsFont;
        Rectangle clippingRect;
        void *userData;
        DrawCommand() : vertices(nullptr), 
            numVertices(0), 
            indices(nullptr), 
            numIndices(0), 
            textureId(0), 
            textureIsFont(false),
            clippingRect(Rectangle(0, 0, 0, 0)),
            userData(nullptr) {}
    };

    struct Viewport {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    struct GLState {
        unsigned char depthTestEnabled;
        unsigned char blendEnabled;
        int blendSrcFactor;
        int blendDstFactor;
        int blendEquation;
        int depthFunc;
    };

    enum Uniform {
        Uniform_Projection,
        Uniform_Texture,
        Uniform_Time,
        Uniform_IsFont,
        Uniform_COUNT
    };

    using UniformUpdateCallback = std::function<void(uint32_t shaderId, void *userData)>;

    class Graphics {
    public:
        UniformUpdateCallback uniformUpdate;
        Graphics();
        void initialize();
        void deinitialize();
        void newFrame(float deltaTime);
        void addRectangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addCircle(const Vector2 &position, float radius, int segments, float rotationDegrees, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addLine(const Vector2 &p1, const Vector2 &p2, float thickness, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addLines(const Vector2 *segments, size_t count, float thickness, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addPlotLines(const Vector2 &position, const Vector2 &size, const float *data, int valuesCount, float thickness, const Color &color, float scaleMin = 3.402823466e+38F, float scaleMax = 3.402823466e+38F, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addTriangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        void addText(const Vector2 &position, Font *font, bool richText, const std::string &text, float fontSize, const Color &color, const Rectangle &clippingRect = Rectangle(0, 0, 0, 0));
        void addImage(const Vector2 &position, const Vector2 &size, float rotationDegrees, uint32_t textureId, const Color &color = Color(1, 1, 1, 1), const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1), const Rectangle &clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
        inline Viewport getViewport() const { return viewport; }
        void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        inline Color getClearColor() const { return clearColor; }
        void setClearColor(const Color &color);
        inline size_t getDrawCalls() const { return numDrawCalls; }
    private:
        uint32_t VAO;
        uint32_t VBO;
        uint32_t EBO;
        uint32_t shaderId;
        uint32_t textureId;
        int32_t uniforms[Uniform_COUNT];
        std::vector<DrawListItem> items;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        size_t itemCount;
        size_t vertexCount;
        size_t indiceCount;
        std::vector<Vertex> vertexBufferTemp; //Temporary buffer used by some 'add' functions with dynamic size requirements
        std::vector<uint32_t> indexBufferTemp; //Temporary buffer used by some 'add' functions with dynamic size requirements
        Viewport viewport;
        Color clearColor;
        GLState glState;
        float elapsedTime;
        size_t numDrawCalls;
        void storeState();
        void restoreState();
        void checkVertexBuffer(size_t numRequiredVertices);
        void checkIndexBuffer(size_t numRequiredIndices);
        void checkItemBuffer(size_t numRequiredItems);
        void checkTemporaryVertexBuffer(size_t numRequiredVertices);
        void checkTemporaryIndexBuffer(size_t numRequiredIndices);
        void addVertices(const DrawCommand *command);
        void rotateVertices(Vertex *vertices, size_t numVertices, float angleDegrees);
        void createBuffers();
        void createShader();
        void createTexture();
    };
};

#endif