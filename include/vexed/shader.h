#include <cstdint>
#include <string>

namespace vexed {
    class Shader {
    public:
        Shader();
        inline uint32_t getId() const { return id; }
        bool load(const std::string &vertexSource, const std::string &fragmentSource);
        bool loadFromFile(const std::string &vertexPath, const std::string &fragmentPath);
        void destroy();
        void setFloat(const char *name, float value);
        void setFloat2(const char *name, const float *value);
    private:
        uint32_t id;
        static bool checkShader(uint32_t handle, const std::string &description);
        static bool checkProgram(uint32_t handle, const std::string &description);
    };
}