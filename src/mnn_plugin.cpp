#include "include/mnn_plugin.h"

#include <string>

#ifdef _WIN32
  #define EXPORT_API __declspec(dllexport)
#else
  #define EXPORT_API
#endif

extern "C" {

    // 返回整数
    EXPORT_API int GetMagicNumber() {
        return 123;
    }

    // 接收 C 字符串并返回新字符串（注意：内存管理需与调用侧协商）
    EXPORT_API const char* EchoString_Internal(const char* input) {
        static std::string buffer;
        buffer = std::string("Echo: ") + input;
        return buffer.c_str();
    }

}