using System;
using System.Threading;
using AOT;
using UnityEngine;
using MNNPlugin.Runtime;

public class TestNativePlugin : MonoBehaviour
{
    // 保持对委托的引用，防止被垃圾回收器回收
    private static NativePlugin.StringCallbackDelegate callbackDelegate;
    
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // int magic = NativePlugin.GetMagicNumber();
        // Debug.Log("MagicNumber = " + magic);
        //
        // string echoed = NativePlugin.EchoString("Hello Unity");
        // Debug.Log("Echoed = " + echoed);
        //
        // int num = NativePlugin.EchoTest();
        // Debug.Log(num);
        // 创建回调委托实例
        callbackDelegate = new NativePlugin.StringCallbackDelegate(OnStringReceived);
        
        // 调用process函数，传入回调
        // NativePlugin.LLMInit(callbackDelegate);
        
        // 创建具有 8MB 栈大小的新线程
        Thread thread = new Thread(() => 
        {
            try 
            {
                NativePlugin.LLMInit(callbackDelegate); // 在新线程中调用 C++ 函数
            }
            catch (Exception e) 
            {
                Debug.LogError("Native call failed: " + e.Message);
            }
        }, 8 * 1024 * 1024); // 8MB 栈空间
        
        thread.Start();
        thread.Join(); // 等待线程完成
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    
    // 必须标记为MonoPInvokeCallback，确保可以从非托管代码调用
    [MonoPInvokeCallback(typeof(NativePlugin.StringCallbackDelegate))]
    public static void OnStringReceived(string message)
    {
        // 处理从C++接收到的字符串
        Debug.Log("Received from C++: " + message);
        
        // 在这里添加您的业务逻辑
    }
}
