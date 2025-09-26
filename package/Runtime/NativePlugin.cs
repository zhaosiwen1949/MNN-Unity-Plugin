using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace MNNPlugin.Runtime
{
	public static class NativePlugin
    {
        // 定义与C++对应的回调委托类型
        public delegate void StringCallbackDelegate(string message);
        
        // 库名：Windows 下不带 .dll，macOS/Linux 不带 lib 前缀和 .dylib/.so
        const string DLL = 
        #if UNITY_IOS
            "__Internal";     // iOS 静态链接
        #elif UNITY_STANDALONE_OSX
            "mnn_plugin";       // 对应 libMyPlugin.dylib
        #elif UNITY_STANDALONE_WIN
            "mnn_plugin";       // 对应 MyPlugin.dll
        #elif UNITY_ANDROID
            "mnn_plugin";       // 对应 libMyPlugin.so
        #else
            "mnn_plugin";
        #endif
#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
        [DllImport(DLL)]
        public static extern int GetMagicNumber();
    
        [DllImport(DLL)]
        public static extern IntPtr EchoString_Internal(IntPtr str);
        
        [DllImport(DLL)]
        public static extern void LLMInit(StringCallbackDelegate callback);
#endif
        public static string EchoString(string input)
        {
            // 将托管字符串转为 char*
            var bytes = System.Text.Encoding.UTF8.GetBytes(input + "\0");
            var ptr = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, ptr, bytes.Length);
    
            // 调用本地方法
            IntPtr resultPtr = EchoString_Internal(ptr);
            string result = Marshal.PtrToStringUTF8(resultPtr);
    
            Marshal.FreeHGlobal(ptr);
            return result;
        }

        public static int EchoTest()
        {
            return 1;
        }
    }
}

