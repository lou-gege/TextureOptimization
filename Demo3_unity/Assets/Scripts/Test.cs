using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Test : MonoBehaviour
{
    public Texture2D originTexture;

    private RenderTexture destRenderTexture;

    public Material samplingMaterial;

    // Start is called before the first frame update
    void Update()
    {
        destRenderTexture = new RenderTexture(originTexture.width / 2, originTexture.height / 2, 0);
        destRenderTexture.filterMode = FilterMode.Bilinear;
        destRenderTexture.anisoLevel = 1;
        destRenderTexture.Create();

        originTexture.anisoLevel = 1;

        Graphics.Blit(originTexture, destRenderTexture, samplingMaterial);

        RenderTexture.active = destRenderTexture;
        Texture2D destTexture = new Texture2D(destRenderTexture.width, destRenderTexture.height);
        destTexture.ReadPixels(new Rect(0, 0, destRenderTexture.width, destRenderTexture.height), 0, 0);
        destTexture.Apply();

        Color[] pixels = destTexture.GetPixels();

        Debug.Log(pixels[0].r * 255 + " " + pixels[0].g * 255 + " " + pixels[0].b * 255);
    }
}
