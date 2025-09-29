#include "mnn_plugin.h"
#include "llm/llm.hpp"
#include <MNN/AutoTime.hpp>
#include <MNN/expr/ExecutorScope.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

using namespace MNN::Transformer;

class LlmStreamBuffer final : public std::streambuf {
public:
    using CallBack = std::function<void(const char *str, size_t len)>;;
    explicit LlmStreamBuffer(CallBack
                             callback) :
            callback_(std::move(callback)) {}

protected:

    std::streamsize xsputn(const char *s, std::streamsize n)

    override {
        if (callback_) {
            callback_(s, n
            );
        }
        return
                n;
    }

private:
    CallBack callback_ = nullptr;
};

namespace mls {
    class Utf8StreamProcessor {
    public:
        explicit Utf8StreamProcessor(std::function<void(const std::string &)> callback)
                : callback(std::move(callback)) {}
        void processStream(const char *str, const size_t len) {
            utf8Buffer.append(str, len);

            size_t i = 0;
            std::string completeChars;
            while (i < utf8Buffer.size()) {
                int length = utf8CharLength(static_cast<unsigned char>(utf8Buffer[i]));
                if (length == 0 || i + length > utf8Buffer.size()) {
                    break;
                }
                completeChars.append(utf8Buffer, i, length);
                i += length;
            }
            utf8Buffer = utf8Buffer.substr(i);
            if (!completeChars.empty()) {
                callback(completeChars);
            }
        }

        static int utf8CharLength(unsigned char byte) {
            if ((byte & 0x80) == 0) return 1;
            if ((byte & 0xE0) == 0xC0) return 2;
            if ((byte & 0xF0) == 0xE0) return 3;
            if ((byte & 0xF8) == 0xF0) return 4;
            return 0;
        }

    private:
        std::string utf8Buffer;
        std::function<void(const std::string &)> callback;
    };
}

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

    EXPORT_API void LLMInit(CallbackFunction callback)
    {
        // // 准备要传递的字符串
        // const auto message = "Hello from C++ Plugin!";
        //
        // // 调用 C# 回调函数
        // callback(message);

        const MNN::BackendConfig backendConfig;
        const auto executor = MNN::Express::Executor::newExecutor(MNN_FORWARD_CPU, backendConfig, 1);
        MNN::Express::ExecutorScope s(executor);

        std::string config_path = R"(D:\Workspaces\MNN\resource\model\FastVLM\config.json)";
        std::cout << "config path is " << config_path << std::endl;
        const std::unique_ptr<Llm> llm(Llm::createLLM(config_path));
        llm->set_config("{\"tmp_path\":\"tmp\"}");
        {
            AUTOTIME;
            llm->load();
        }
        if constexpr (true) {
            AUTOTIME;
            MNN_PRINT("Prepare for tuning opt Begin\n");
            llm->tuning(OP_ENCODER_NUMBER, {1, 5, 10, 20, 30, 50, 100});
            MNN_PRINT("Prepare for tuning opt End\n");
        }

        int current_size = 0;
        bool stop_requested = false;
        std::stringstream response_buffer;
        mls::Utf8StreamProcessor processor([&response_buffer, &stop_requested, &callback](const std::string& utf8Char) {
            bool is_eop = utf8Char.find("<eop>") != std::string::npos;
            if (!is_eop) {
                response_buffer << utf8Char;
            } else {
                std::string response_result =  response_buffer.str();
                callback(response_result.c_str());
                stop_requested = true;
            }
        });
        LlmStreamBuffer stream_buffer{[&processor](const char* str, size_t len){
            processor.processStream(str, len);
        }};
        std::ostream output_ostream(&stream_buffer);

        const std::string prompt = R"(<img>https://qianwen-res.oss-cn-beijing.aliyuncs.com/Qwen-VL/assets/demo.jpeg<hw>280, 280</hw></img>请详细描述下面这张图片)";

        llm->response(prompt, &output_ostream, "<eop>", 1);
        current_size++;

        int max_new_tokens = 1000;
        while (!stop_requested  && current_size < max_new_tokens) {
            llm->generate(1);
            current_size++;
        }

        auto context = llm->getContext();
        int prompt_len = context->prompt_len;
        int decode_len = context->gen_seq_len;
        int64_t vision_time = context->vision_us;
        int64_t audio_time = context->audio_us;
        int64_t prefill_time = context->prefill_us;
        int64_t decode_time = context->decode_us;
        int64_t sample_time = context->sample_us;

        float vision_s = vision_time / 1e6;
        float audio_s = audio_time / 1e6;
        float prefill_s = prefill_time / 1e6;
        float decode_s = decode_time / 1e6;
        float sample_s = sample_time / 1e6;
        printf("\n#################################\n");
        printf("prompt tokens num = %d\n", prompt_len);
        printf("decode tokens num = %d\n", decode_len);
        printf(" vision time = %.2f s\n", vision_s);
        printf("  audio time = %.2f s\n", audio_s);
        printf("prefill time = %.2f s\n", prefill_s);
        printf(" decode time = %.2f s\n", decode_s);
        printf(" sample time = %.2f s\n", sample_s);
        printf("prefill speed = %.2f tok/s\n", prompt_len / prefill_s);
        printf(" decode speed = %.2f tok/s\n", decode_len / decode_s);
        printf("##################################\n");

        return;
    }

}