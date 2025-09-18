using UnityEngine;

public class TestNativePlugin : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        int magic = NativePlugin.GetMagicNumber();
        Debug.Log("MagicNumber = " + magic);
        
        string echoed = NativePlugin.EchoString("Hello Unity");
        Debug.Log("Echoed = " + echoed);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
