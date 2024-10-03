#include <vector>
#include <stdexcept>
#include <cstdlib>

namespace vexed {
    template <typename T>
    class RingBuffer {
    public:
        RingBuffer() 
            : maxSize(50), startIndex(0), endIndex(0), itemCount(0) {
            items.resize(maxSize);
        }

        RingBuffer(size_t maxMessages) 
            : maxSize(maxMessages), startIndex(0), endIndex(0), itemCount(0) {
            items.resize(maxSize);
        }

        // RingBuffer(MessageBuffer &&other) noexcept 
        //     : items(std::move(other.items)),
        //     maxSize(other.maxSize),
        //     startIndex(other.startIndex),
        //     endIndex(other.endIndex),
        //     itemCount(other.itemCount) {
        //     other.startIndex = 0;
        //     other.endIndex = 0;
        //     other.itemCount = 0;
        // }

        void add(const T &m) {
            items[endIndex] = m;
            endIndex = (endIndex + 1) % maxSize;

            if (itemCount < maxSize) {
                itemCount++;
            } else {
                startIndex = (startIndex + 1) % maxSize;
            }
        }

        size_t count() const {
            return itemCount;
        }

        size_t size() const {
            return maxSize;
        }

        T &getAt(size_t index) {
            if (index >= 0 && index < itemCount) {
                int idx = (startIndex + index) % maxSize;
                return items[idx];
            }
            throw std::out_of_range("Index out of range");
        }

        void clear() {
            items.clear();
            items.resize(maxSize);
            startIndex = 0;
            endIndex = 0;
            itemCount = 0;
        }
    private:
        std::vector<T> items;
        size_t maxSize;
        size_t startIndex;
        size_t endIndex;
        size_t itemCount;
    };
}