#ifndef MNN_PLUGIN_H
#define MNN_PLUGIN_H

#ifdef _WIN32
  #define EXPORT_API __declspec(dllexport)
#else
  #define EXPORT_API
#endif

// 定义回调函数类型
typedef void (*CallbackFunction)(const char* message);

extern "C" {
  EXPORT_API void LLMInit(CallbackFunction callback);
}

#endif //MNN_PLUGIN_H