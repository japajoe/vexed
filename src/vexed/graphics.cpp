#include "graphics.h"
#include "../glad/glad.h"
#include <cstring>
#include <cmath>
#include <iostream>

namespace vexed {
    Graphics::Graphics() {
        VAO = 0;
        VBO = 0;
        EBO = 0;
        shaderId = 0;
        textureId = 0;
        itemCount = 0;
        vertexCount = 0;
        indiceCount = 0;
        viewport = { 0, 0, 512, 512 };
        elapsedTime = 0.0f;
    }

    void Graphics::initialize() {        
        createBuffers();
        createShader();
        createTexture();
    }

    void Graphics::deinitialize() {
        if(VAO > 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        if(VBO > 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        if(EBO > 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }

        if(shaderId > 0) {
            glDeleteShader(shaderId);
            shaderId = 0;
        }

        if(textureId > 0) {
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }
    }

    void Graphics::newFrame(float deltaTime) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        numDrawCalls = itemCount;

        if(itemCount == 0) {
            elapsedTime += deltaTime;
            return;
        }

        const float L = viewport.x;
        const float R = viewport.x + viewport.width;
        const float T = viewport.y;
        const float B = viewport.y + viewport.height;
        const float near = -1.0f;
        const float far = 1.0f;

        const float projectionMatrix[4][4] = {
            { 2.0f / (R - L),    0.0f,            0.0f,              0.0f },
            { 0.0f,              2.0f / (T - B),  0.0f,              0.0f },
            { 0.0f,              0.0f,           -1.0f / (far - near), 0.0f },
            { -(R + L) / (R - L), -(T + B) / (T - B), (far + near) / (far - near), 1.0f }
        };

        storeState();

        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indiceCount * sizeof(uint32_t), indices.data());

        uint32_t lastShaderId = items[0].shaderId;
        glUseProgram(lastShaderId);
        glActiveTexture(GL_TEXTURE0);

        uint32_t lastTextureId = items[0].textureId;
        glBindTexture(GL_TEXTURE_2D, lastTextureId);

        size_t drawOffset = 0; // Offset for the draw call

        for(size_t i = 0; i < itemCount; i++) {
            Rectangle rect = items[i].clippingRect;
            bool scissorEnabled = false;
            if(!rect.IsZero()) {
                glEnable(GL_SCISSOR_TEST);
                glScissor(rect.x, rect.y, rect.width, rect.height);
                scissorEnabled = true;
            }

            if(items[i].shaderId != lastShaderId) {
                glUseProgram(items[i].shaderId);
                lastShaderId = items[i].shaderId;
            }

            if(items[i].textureId != lastTextureId) {
                glBindTexture(GL_TEXTURE_2D, items[i].textureId);
                lastTextureId = items[i].textureId;
            }

            if(lastShaderId == shaderId) {
                glUniform1i(uniforms[Uniform_Texture], 0);
                glUniformMatrix4fv(uniforms[Uniform_Projection], 1, GL_FALSE, &projectionMatrix[0][0]);
                glUniform1f(uniforms[Uniform_Time], elapsedTime);
                //This uniform is only mandatory on default shader
                glUniform1i(uniforms[Uniform_IsFont], items[i].textureIsFont ? 1 : 0);
            } else {
                // Only dispatch callback for custom shaders
                //These 3 uniforms are mandatory on any shader
                glUniform1i(glGetUniformLocation(lastShaderId, "uTexture"), 0);
                glUniformMatrix4fv(glGetUniformLocation(lastShaderId, "uProjection"), 1, GL_FALSE, &projectionMatrix[0][0]);
                glUniform1f(glGetUniformLocation(lastShaderId, "uTime"), elapsedTime);
                if(uniformUpdate)
                    uniformUpdate(lastShaderId);
            }

            glDrawElements(GL_TRIANGLES, items[i].indiceCount, GL_UNSIGNED_INT, (void*)(drawOffset * sizeof(uint32_t)));

            drawOffset += items[i].indiceCount;

            if(scissorEnabled) {
                glDisable(GL_SCISSOR_TEST);
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        restoreState();

        glDisable(GL_SCISSOR_TEST);

        // Reset counts for the next render
        itemCount = 0;
        vertexCount = 0;
        indiceCount = 0;

        elapsedTime += deltaTime;
    }

    void Graphics::storeState() {
        glState.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        glState.blendEnabled = glIsEnabled(GL_BLEND);
        glGetIntegerv(GL_BLEND_SRC, &glState.blendSrcFactor);
        glGetIntegerv(GL_BLEND_DST, &glState.blendDstFactor);
        glGetIntegerv(GL_BLEND_EQUATION, &glState.blendEquation);
        glGetIntegerv(GL_DEPTH_FUNC, &glState.depthFunc);
    }

    void Graphics::restoreState() {
        if (glState.depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        if (glState.blendEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        glBlendFunc(glState.blendSrcFactor, glState.blendDstFactor);
        glBlendEquation(glState.blendEquation);
        glDepthFunc(glState.depthFunc);
    }

    void Graphics::addRectangle(const Vector3 &position, const Vector2 &size, const Color &color) {
        addRectangle(position, size, 0, color, Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addRectangle(const Vector3 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        Vertex vertices[4] = {
            { Vector3(position.x, position.y, position.z), Vector2(0, 1), color }, // top left
            { Vector3(position.x, position.y + size.y, position.z), Vector2(0, 0), color }, // bottom left
            { Vector3(position.x + size.x, position.y + size.y, position.z), Vector2(1, 0), color }, // bottom right
            { Vector3(position.x + size.x, position.y, position.z), Vector2(1, 1), color }  // top right
        };

        if(rotationDegrees != 0.0f)
            rotateVertices(vertices, 4, rotationDegrees);

        uint32_t indices[6] = {
            0, 1, 2, 
            0, 2, 3
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
        command.numVertices = 4;
        command.numIndices = 6;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }
    
    void Graphics::addCircle(const Vector3 &position, float radius, int segments, const Color &color) {
        addCircle(position, radius, segments, 0, color, Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addCircle(const Vector3 &position, float radius, int segments, float rotationDegrees, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        if(segments < 3)
            segments = 3;

        size_t requiredVertices = segments;
        size_t requiredIndices = segments * 3; // 3 indices per segment

        checkTemporaryVertexBuffer(requiredVertices);
        checkTemporaryIndexBuffer(requiredIndices);

        for (int i = 0; i < segments; ++i) {
            float angle = 2.0f * M_PI * i / segments;
            vertexBufferTemp[i].position = Vector3(radius * cos(angle), radius * sin(angle), position.z);
            vertexBufferTemp[i].uv = Vector2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
            vertexBufferTemp[i].color = color;
            vertexBufferTemp[i].position.x += position.x;
            vertexBufferTemp[i].position.y += position.y;
        }

        if(rotationDegrees != 0.0f)
            rotateVertices(vertexBufferTemp.data(), segments, rotationDegrees);

        for (int i = 0; i < segments; ++i) {
            indexBufferTemp[i * 3] = 0; // Center vertex (if added at 0 index)
            indexBufferTemp[i * 3 + 1] = i;
            indexBufferTemp[i * 3 + 2] = (i + 1) % segments; // Wrap around to form a circle
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
        command.numVertices = segments;
        command.numIndices = segments * 3;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::addLine(const Vector3 &p1, const Vector3 &p2, float thickness, const Color &color) {
        addLine(p1, p2, thickness, color, Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addLine(const Vector3 &p1, const Vector3 &p2, float thickness, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        Vector2 direction(p2.x - p1.x, p2.y - p1.y);
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length == 0) return;

        direction.x /= length;
        direction.y /= length;

        Vector2 perpendicular = Vector2(-direction.y * thickness * 0.5f, 
                                        direction.x * thickness * 0.5f);

        Vertex vertices[4] = {
            { Vector3(p1.x + perpendicular.x, p1.y + perpendicular.y, p1.z), Vector2(0, 1), color }, // Bottom left
            { Vector3(p1.x - perpendicular.x, p1.y - perpendicular.y, p1.z), Vector2(0, 0), color }, // Top left
            { Vector3(p2.x - perpendicular.x, p2.y - perpendicular.y, p2.z), Vector2(1, 0), color }, // Top right
            { Vector3(p2.x + perpendicular.x, p2.y + perpendicular.y, p2.z), Vector2(1, 1), color }  // Bottom right
        };

        uint32_t indices[6] = {
            0, 1, 2, 
            0, 2, 3
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
        command.numVertices = 4;
        command.numIndices = 6;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::addLines(const Vector3 *segments, size_t count, float thickness, const Color &color) {
        addLines(segments, count, thickness, color, Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addLines(const Vector3 *segments, size_t count, float thickness, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        if (count == 0) 
            return;

        if(count % 2 != 0)
            return;

        if (segments == nullptr) 
            return;

        size_t requiredVertices = count * 4; // 4 vertices per line
        size_t requiredIndices = count * 6; // 6 indices per line

        checkTemporaryVertexBuffer(requiredVertices);
        checkTemporaryIndexBuffer(requiredIndices);

        size_t pointCount = count * 2;
        size_t vertexIndex = 0;
        size_t indiceIndex = 0;

        for(size_t i = 0; i < pointCount; i+=2) {
            Vector3 p1 = segments[i+0];
            Vector3 p2 = segments[i+1];

            Vector2 direction(p2.x - p1.x, p2.y - p1.y);

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length == 0) 
                return;

            direction.x /= length;
            direction.y /= length;

            Vector2 perpendicular = Vector2(-direction.y * thickness * 0.5f, 
                                            direction.x * thickness * 0.5f);

            vertexBufferTemp[vertexIndex+0] = { Vector3(p1.x + perpendicular.x, p1.y + perpendicular.y, p1.z), Vector2(0, 1), color };
            vertexBufferTemp[vertexIndex+1] = { Vector3(p1.x - perpendicular.x, p1.y - perpendicular.y, p1.z), Vector2(0, 0), color };
            vertexBufferTemp[vertexIndex+2] = { Vector3(p2.x - perpendicular.x, p2.y - perpendicular.y, p2.z), Vector2(1, 0), color };
            vertexBufferTemp[vertexIndex+3] = { Vector3(p2.x + perpendicular.x, p2.y + perpendicular.y, p2.z), Vector2(1, 1), color };

            indexBufferTemp[indiceIndex+0] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+1] = 1 + vertexIndex;
            indexBufferTemp[indiceIndex+2] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+3] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+4] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+5] = 3 + vertexIndex;

            vertexIndex += 4;
            indiceIndex += 6;
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
        command.numVertices = requiredVertices;
        command.numIndices = requiredIndices;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::addTriangle(const Vector3 &position, const Vector2 &size, const Color &color) {
        addTriangle(position, size, 0, color, Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addTriangle(const Vector3 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle &clippingRect, uint32_t shaderId) {
        float halfWidth = 1.0f * 0.5f;
        float halfHeight = 1.0f * 0.5f;

        Vector2 vertex1(0.0f, halfHeight); // Top vertex
        Vector2 vertex2(-halfWidth, -halfHeight); // Bottom-left vertex
        Vector2 vertex3(halfWidth, -halfHeight); // Bottom-right vertex

        Vector2 uv1(0.5f, 1.0f); // Top vertex UV
        Vector2 uv2(0.0f, 0.0f); // Bottom-left vertex UV
        Vector2 uv3(1.0f, 0.0f); // Bottom-right vertex UV

        Vertex vertices[3] = {
            { Vector3(position.x + (vertex1.x * size.x), position.y + (vertex1.y * size.y), position.z), uv1, color},
            { Vector3(position.x + (vertex2.x * size.x), position.y + (vertex2.y * size.y), position.z), uv2, color},
            { Vector3(position.x + (vertex3.x * size.x), position.y + (vertex3.y * size.y), position.z), uv3, color}
        };

        if(rotationDegrees != 0.0f)
            rotateVertices(vertices, 3, rotationDegrees);

        uint32_t indices[3] = {
            0, 1, 2, 
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
        command.numVertices = 3;
        command.numIndices = 3;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::addText(const Vector3 &position, Font *font, const std::string &text, float fontSize, const Color &color, const Rectangle &clippingRect) {
        if(!font)
            return;
        if(text.size() == 0)
            return;

        size_t requiredVertices = text.size() * 4; // 4 vertices per character
        size_t requiredIndices = text.size() * 6; // 6 indices per character

        checkTemporaryVertexBuffer(requiredVertices);
        checkTemporaryIndexBuffer(requiredIndices);

        float size = fontSize / font->getPixelSize();
        Vector2 pos(position.x, position.y);
        pos.y += font->getLineHeight() * size;
        float originX = pos.x;
        float originY = pos.y;
        auto &packedChars = font->getCharacters();
        auto &alignedQuads = font->getQuads();

        uint32_t codePointOfFirstChar = font->getCodePointOfFirstChar();

        size_t vertexIndex = 0;
        size_t indiceIndex = 0;

        for(size_t i = 0; i < text.size(); i++) {
            char ch = text[i];

            if(ch == '\n') {
                pos.x = originX;
                pos.y += font->getLineHeight() * size;
                continue;
            }

            int glyphIndex = (ch - codePointOfFirstChar);

            //Skip unknown glyphs
            if(glyphIndex < 0 || glyphIndex >= font->getNumberOfCharacters())
                continue;

            auto packedChar = &packedChars[glyphIndex];
            auto alignedQuad = &alignedQuads[glyphIndex];

            Vector2 glyphSize = {
                (packedChar->x1 - packedChar->x0) * size,
                (packedChar->y1 - packedChar->y0) * size
            };

            Vector2 glyphBoundingBoxBottomLeft = {
                pos.x + (packedChar->xoff * size),
                pos.y + (packedChar->yoff + packedChar->y1 - packedChar->y0) * size
            };

            // The order of vertices of a quad goes top-right, top-left, bottom-left, bottom-right
            Vector2 glyphVertices[4] = {
                { glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y - glyphSize.y },
                { glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y - glyphSize.y },
                { glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y },
                { glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y },
            };

            Vector2 glyphTextureCoords[4] = {
                { alignedQuad->s1, alignedQuad->t0 },
                { alignedQuad->s0, alignedQuad->t0 },
                { alignedQuad->s0, alignedQuad->t1 },
                { alignedQuad->s1, alignedQuad->t1 },
            };

            vertexBufferTemp[vertexIndex+0] = { Vector3(glyphVertices[0].x, glyphVertices[0].y, position.z), glyphTextureCoords[0], color };
            vertexBufferTemp[vertexIndex+1] = { Vector3(glyphVertices[1].x, glyphVertices[1].y, position.z), glyphTextureCoords[1], color };
            vertexBufferTemp[vertexIndex+2] = { Vector3(glyphVertices[2].x, glyphVertices[2].y, position.z), glyphTextureCoords[2], color };
            vertexBufferTemp[vertexIndex+3] = { Vector3(glyphVertices[3].x, glyphVertices[3].y, position.z), glyphTextureCoords[3], color };

            indexBufferTemp[indiceIndex+0] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+1] = 1 + vertexIndex;
            indexBufferTemp[indiceIndex+2] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+3] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+4] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+5] = 3 + vertexIndex;

            vertexIndex += 4;
            indiceIndex += 6;

            pos.x += packedChar->xadvance * size;
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
        command.numVertices = vertexIndex;
        command.numIndices = indiceIndex;
        command.textureId = font->getTexture();
        command.textureIsFont = true;
        command.shaderId = this->shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::addImage(const Vector3 &position, const Vector2 &size, uint32_t textureId, const Color &color) {
        addImage(position, size, 0, textureId, color, Vector2(0, 0), Vector2(1, 1), Rectangle(0, 0, 0, 0), 0);
    }

    void Graphics::addImage(const Vector3 &position, const Vector2 &size, float rotationDegrees, uint32_t textureId, const Color &color, const Vector2 &uv0, const Vector2 &uv1, const Rectangle &clippingRect, uint32_t shaderId) {
        Vector2 uvTopLeft = Vector2(uv0.x, uv0.y);
        Vector2 uvBottomLeft = Vector2(uv0.x, uv1.y);
        Vector2 uvBottomRight = Vector2(uv1.x, uv1.y);
        Vector2 uvTopRight = Vector2(uv1.x, uv0.y);

        Vertex vertices[4] = {
            { Vector3(position.x, position.y, position.z), uvTopLeft, color }, // top left
            { Vector3(position.x, position.y + size.y, position.z), uvBottomLeft, color }, // bottom left
            { Vector3(position.x + size.x, position.y + size.y, position.z), uvBottomRight, color }, // bottom right
            { Vector3(position.x + size.x, position.y, position.z), uvTopRight, color }  // top right
        };

        if(rotationDegrees != 0.0f)
            rotateVertices(vertices, 4, rotationDegrees);

        uint32_t indices[6] = {
            0, 1, 2, 
            0, 2, 3
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
        command.numVertices = 4;
        command.numIndices = 6;
        command.textureId = textureId;
        command.textureIsFont = false;
        command.shaderId = shaderId;
        command.clippingRect = clippingRect;

        addVertices(&command);
    }

    void Graphics::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(0, 0, width, height);
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
    }

    void Graphics::setClearColor(const Color &color) {
        this->clearColor = color;
    }

    void Graphics::checkVertexBuffer(size_t numRequiredVertices) {
        size_t verticesNeeded = vertexCount + numRequiredVertices;
        
        if(verticesNeeded > vertices.size()) {
            size_t newSize = vertices.size() * 2;
            while(newSize < verticesNeeded) {
                newSize *= 2;
            }
            vertices.resize(newSize);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, newSize * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void Graphics::checkIndexBuffer(size_t numRequiredIndices) {
        size_t indicesNeeded = indiceCount + numRequiredIndices;
        
        if(indicesNeeded > indices.size()) {
            size_t newSize = indices.size() * 2;
            while(newSize < indicesNeeded) {
                newSize *= 2;
            }
            indices.resize(newSize);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    void Graphics::checkItemBuffer(size_t numRequiredItems) {
        size_t itemsNeeded = itemCount + numRequiredItems;

        if(itemsNeeded > items.size()) {
            size_t newSize = items.size() * 2;
            while(newSize < itemsNeeded) {
                newSize *= 2;
            }
            items.resize(newSize);
        }
    }

    void Graphics::checkTemporaryVertexBuffer(size_t numRequiredVertices) {
        if(vertexBufferTemp.size() < numRequiredVertices) {
            size_t newSize = vertexBufferTemp.size() * 2;
            while(newSize < numRequiredVertices) {
                newSize *= 2;
            }                
            vertexBufferTemp.resize(newSize);
        }
    }

    void Graphics::checkTemporaryIndexBuffer(size_t numRequiredIndices) {
        if(indexBufferTemp.size() < (numRequiredIndices)) {
            size_t newSize = indexBufferTemp.size() * 2;
            while(newSize < numRequiredIndices) {
                newSize *= 2;
            }
            indexBufferTemp.resize(newSize);
        }
    }

    void Graphics::addVertices(const DrawCommand *command) {
        checkVertexBuffer(command->numVertices);
        checkIndexBuffer(command->numIndices);
        checkItemBuffer(1);

        memcpy(&vertices[vertexCount], &command->vertices[0], command->numVertices * sizeof(Vertex));

        for(size_t i = 0; i < command->numIndices; i++) {
            indices[indiceCount+i] = command->indices[i] + vertexCount;
        }

        items[itemCount].vertexCount = command->numVertices;
        items[itemCount].indiceCount = command->numIndices;
        items[itemCount].vertexOffset = vertexCount;
        items[itemCount].indiceOffset = indiceCount;
        items[itemCount].shaderId = command->shaderId == 0 ? this->shaderId : command->shaderId;
        items[itemCount].textureId = command->textureId;
        items[itemCount].textureIsFont = command->textureIsFont;
        items[itemCount].clippingRect = command->clippingRect;

        Rectangle &rect = items[itemCount].clippingRect;

        if(!rect.IsZero()) {
            rect.y = viewport.height - rect.y - rect.height;
        }

        itemCount++;
        vertexCount += command->numVertices;
        indiceCount += command->numIndices;
    }

    void Graphics::rotateVertices(Vertex *vertices, size_t numVertices, float angleDegrees) {
        float centerX = 0.0f;
        float centerY = 0.0f;

        for (size_t i = 0; i < numVertices; ++i) {
            centerX += vertices[i].position.x;
            centerY += vertices[i].position.y;
        }

        centerX /= numVertices;
        centerY /= numVertices;

        float radians = angleDegrees * (M_PI / 180.0f);
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);

        for (size_t i = 0; i < numVertices; ++i) {
            float translatedX = vertices[i].position.x - centerX;
            float translatedY = vertices[i].position.y - centerY;

            float rotatedX = translatedX * cosAngle - translatedY * sinAngle;
            float rotatedY = translatedX * sinAngle + translatedY * cosAngle;

            vertices[i].position.x = rotatedX + centerX;
            vertices[i].position.y = rotatedY + centerY;
        }
    }

    void Graphics::createBuffers() {
        constexpr size_t size = 2 << 15;
        items.resize(size);
        vertices.resize(size);
        indices.resize(size);
        vertexBufferTemp.resize(size);
        indexBufferTemp.resize(size);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

    static bool checkShader(uint32_t handle, const char* desc) {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to compile %s!\n", desc);
        if (log_length > 1) {
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, nullptr, &buf[0]);
            fprintf(stderr, "%s\n", buf.begin());
        }

        return (GLboolean)status == GL_TRUE;
    }

    static bool checkProgram(GLuint handle, const char* desc) {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to link %s!n", desc);
        if (log_length > 1) {
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, nullptr, &buf[0]);
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    void Graphics::createShader() {
        std::string vertexSource = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

uniform mat4 uProjection;
out vec2 oTexCoord;
out vec4 oColor;

void main() {
    gl_Position = uProjection * vec4(aPosition, 1.0);
    oTexCoord = aTexCoord;
    oColor = aColor;
})";

        std::string fragmentSource = R"(#version 330 core
uniform sampler2D uTexture;
uniform float uTime;
uniform int uIsFont;

in vec2 oTexCoord;
in vec4 oColor;
out vec4 FragColor;

void main() {
    if(uIsFont > 0) {
        vec4 sample = texture(uTexture, oTexCoord);
        if(sample.r == 0)
            discard;
        FragColor = sample.r * oColor;
        //float d = sample.r;
        //float aaf = fwidth(d);
        //float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
        //FragColor = vec4(oColor.rgb, alpha) * oColor;
    } else {
        FragColor = texture(uTexture, oTexCoord) * oColor;
    }
})";

        const GLchar* vertex_shader[1] = {
            vertexSource.c_str()
        };

        const GLchar* fragment_shader[1] = {
            fragmentSource.c_str()
        };

        GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_handle, 1, vertex_shader, nullptr);
        glCompileShader(vert_handle);
        checkShader(vert_handle, "vertex shader");

        GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_handle, 1, fragment_shader, nullptr);
        glCompileShader(frag_handle);
        checkShader(frag_handle, "fragment shader");

        shaderId = glCreateProgram();
        glAttachShader(shaderId, vert_handle);
        glAttachShader(shaderId, frag_handle);
        glLinkProgram(shaderId);
        checkProgram(shaderId, "shader program");

        glDetachShader(shaderId, vert_handle);
        glDetachShader(shaderId, frag_handle);
        glDeleteShader(vert_handle);
        glDeleteShader(frag_handle);

        uniforms[Uniform_Texture] = glGetUniformLocation(shaderId, "uTexture");
        uniforms[Uniform_Projection] = glGetUniformLocation(shaderId, "uProjection");
        uniforms[Uniform_IsFont] = glGetUniformLocation(shaderId, "uIsFont");
        uniforms[Uniform_Time] = glGetUniformLocation(shaderId, "uTime");
    }

    void Graphics::createTexture() {
        unsigned char textureData[16];
        memset(textureData, 255, 16);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    }
}