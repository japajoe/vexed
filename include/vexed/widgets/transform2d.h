#ifndef VEXED_TRANSFORM2D_H
#define VEXED_TRANSFORM2D_H

#include "../vexed.h"

namespace vexed {
    class Transform2D {
    public:
        Transform2D()
            : position{0.0f, 0.0f}, scale{1.0f, 1.0f}, rotation(0.0f), parent(nullptr) {}

        void setPosition(const Vector2 &position) {
            this->position = position;
            this->position = position;
        }

        void setScale(const Vector2 &scale) {
            this->scale = scale;
        }

        void setRotation(float angle) {
            rotation = angle;
        }

        void setParent(Transform2D* newParent) {
            parent = newParent;
        }

        Vector2 getWorldPosition() const {
            // Start with the local position
            Vector2 worldPosition;
            worldPosition.x = position.x;// * scale.x;
            worldPosition.y = position.y;// * scale.y;

            Transform2D* current = parent;
            while (current) {
                // Apply parent's transformation
                worldPosition.x += current->position.x;// * current->scale.x;
                worldPosition.y += current->position.y;// * current->scale.y;
                current = current->parent; // Move up to the next parent
            }

            return worldPosition;
        }

        Vector2 getPosition () const {
            return position;
        }

        Vector2 getScale() const { 
            return scale;
        }

    private:
        Vector2 position; // x, y
        Vector2 scale;    // scaleX, scaleY
        float rotation;     // rotation in degrees
        Transform2D *parent; // Pointer to the parent Transform2D
    };
}

#endif