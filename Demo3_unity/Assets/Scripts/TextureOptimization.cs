using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class TextureOptimization : MonoBehaviour
{
    public Texture2D truthTex;
    //public RenderTexture blackRT;

    public Material gpuDeltaMaterial;
    public Material gpuGatherMaterial;
    public Material gradientMaterial;

    private RenderTexture currRT;
    private Material currMat;

    private float psnr = 0;
    private float prepsnr = 0;
    private int pixelCount;

    //private Color[] currTexPixels;
    //private Color[] truthTexPixels;

    //private Texture2D tempTexture;
    //private Texture2D currTex;

    const float EPS = 0.00001f;
    float STEP = 1000f;
    //float IVSEPS;

    // Start is called before the first frame update
    void Start()
    {
        currMat = GetComponent<Renderer>().material;
        currRT = new RenderTexture(truthTex.width, truthTex.height, 0, RenderTextureFormat.ARGBFloat);
        pixelCount = truthTex.width * truthTex.height;
        //IVSEPS = 1 / EPS;
    }

    // Update is called once per frame
    void Update()
    {
        if(psnr != float.PositiveInfinity)
        {
            float mse = CalculateMse_GPU_Gather();
            prepsnr = psnr;
            UpdatePSNR(mse);
            Debug.Log("psnr: " + psnr);

            if(psnr == float.PositiveInfinity)
            {
                currMat.SetTexture("_MainTex", currRT);
                return;
            }

            if(psnr < prepsnr)
            {
                STEP *= 0.8f;
            }

            float gradientPart = 4.342944819f / (mse * 3 * pixelCount * psnr * psnr) * STEP;
            gradientMaterial.SetTexture("_TruthTex", truthTex);
            gradientMaterial.SetFloat("_GradientPart", gradientPart);
            //gradientMaterial.SetFloat("_Step", STEP);

            RenderTexture newRT = new RenderTexture(currRT.width, currRT.height, 0, RenderTextureFormat.ARGBFloat);
            Graphics.Blit(currRT, newRT, gradientMaterial);

            RenderTexture temp = currRT;
            currRT = newRT;
            Destroy(temp);

        }
        
        currMat.SetTexture("_MainTex", currRT);
    }

    //float CalculateMSE_CPU()
    //{
    //    float sumSquaredDiff = 0f;
    //    int pixelCount = currTexPixels.Length;

    //    for (int i = 0; i < pixelCount; i++)
    //    {
    //        sumSquaredDiff += Mathf.Pow(currTexPixels[i].r - truthTexPixels[i].r, 2)
    //                        + Mathf.Pow(currTexPixels[i].g - truthTexPixels[i].g, 2)
    //                        + Mathf.Pow(currTexPixels[i].b - truthTexPixels[i].b, 2);
    //    }

    //    float mse = sumSquaredDiff / (pixelCount * 3);

    //    return mse;
    //}

    float CalculateMse_GPU_Gather()
    {
        RenderTexture deltaRenderTexture = new RenderTexture(truthTex.width, truthTex.height, 0, RenderTextureFormat.RFloat);
        deltaRenderTexture.enableRandomWrite = true;
        deltaRenderTexture.Create();

        gpuDeltaMaterial.SetTexture("_ResultTex", currRT);
        Graphics.Blit(truthTex, deltaRenderTexture, gpuDeltaMaterial);

        int maxLevel = (int)Mathf.Log(Mathf.Max(deltaRenderTexture.width, deltaRenderTexture.height), 2);
        RenderTexture srcTempRT = deltaRenderTexture;
        for (int i = 0; i < maxLevel; i++)
        {
            RenderTexture destTempRT = new RenderTexture(srcTempRT.width / 2, srcTempRT.height / 2, 0, RenderTextureFormat.RFloat);
            Graphics.Blit(srcTempRT, destTempRT, gpuGatherMaterial);
            RenderTexture temp = srcTempRT;
            srcTempRT = destTempRT;
            Destroy(temp);
        }

        RenderTexture.active = srcTempRT;
        Texture2D resultTexture = new Texture2D(srcTempRT.width, srcTempRT.height, TextureFormat.RFloat, false);
        resultTexture.ReadPixels(new Rect(0, 0, srcTempRT.width, srcTempRT.height), 0, 0);
        resultTexture.Apply();

        Color[] texPixels = resultTexture.GetPixels();

        RenderTexture.active = null;
        Destroy(srcTempRT);
        Destroy(resultTexture);

        return texPixels[0].r / (3 * pixelCount);
    }

    void UpdatePSNR(float mse)
    {
        psnr = 10 * Mathf.Log10(1f / mse);
    }
}
