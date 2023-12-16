using UnityEngine;

public class Sample : MonoBehaviour
{
    public Texture2D originTexture;

    private RenderTexture downSampledRenderTexture;
    private RenderTexture upSampledRenderTexture;

    private RenderTexture deltaRenderTexture;

    public Material samplingMaterial;
    public Material gpuDeltaMaterial;
    public Material gpuGatherMaterial;

    void Start()
    {
        downSampledRenderTexture = new RenderTexture(originTexture.width / 2, originTexture.height / 2, 0);
        downSampledRenderTexture.filterMode = FilterMode.Bilinear;
        downSampledRenderTexture.Create();

        upSampledRenderTexture = new RenderTexture(originTexture.width, originTexture.height, 0);
        upSampledRenderTexture.filterMode = FilterMode.Bilinear;
        upSampledRenderTexture.Create();

        // Pass 0 降采样
        Graphics.Blit(originTexture, downSampledRenderTexture, samplingMaterial, 0);
        // Pass 1 上采样
        Graphics.Blit(downSampledRenderTexture, upSampledRenderTexture, samplingMaterial, 1);

        RenderTexture.active = upSampledRenderTexture;
        Texture2D upSampledTexture = new Texture2D(upSampledRenderTexture.width, upSampledRenderTexture.height);
        upSampledTexture.ReadPixels(new Rect(0, 0, upSampledRenderTexture.width, upSampledRenderTexture.height), 0, 0);
        upSampledTexture.Apply();

        // MSE Calculate
        float mse_cpu = CalculateMSE_CPU(originTexture, upSampledTexture);
        Debug.Log("MSE CPU: " + mse_cpu);

        float mse_gpu_delta = CalculateMSE_GPU_Delta(originTexture, upSampledTexture);
        Debug.Log("MSE GPU Delta: " + mse_gpu_delta);

        float mse_gpu_mipmap = CalculateMSE_GPU_MipMap();
        Debug.Log("MSE GPU MipMap: " + mse_gpu_mipmap);

        float mse_gpu_gather = CalculateMse_GPU_Gather();
        Debug.Log("MSE GPU Gather: " + mse_gpu_gather);

        // PSNR Calculate
        Debug.Log("PSNR CPU: " + CalculatePSNR(mse_cpu));
        Debug.Log("PSNR GPU Delta: " + CalculatePSNR(mse_gpu_delta));
        Debug.Log("PSNR GPU MipMap: " + CalculatePSNR(mse_gpu_mipmap));
        Debug.Log("PSNR GPU Gather: " + CalculatePSNR(mse_gpu_gather));

    }

    float CalculateMSE_CPU(Texture2D originTex, Texture2D resultTex)
    {
        Color[] tex1Pixels = originTex.GetPixels();
        Color[] tex2Pixels = resultTex.GetPixels();

        float sumSquaredDiff = 0f;
        int pixelCount = tex1Pixels.Length;

        for (int i = 0; i < pixelCount; i++)
        {
            sumSquaredDiff += Mathf.Pow(tex1Pixels[i].r - tex2Pixels[i].r, 2)
                            + Mathf.Pow(tex1Pixels[i].g - tex2Pixels[i].g, 2)
                            + Mathf.Pow(tex1Pixels[i].b - tex2Pixels[i].b, 2);
        }

        float mse = sumSquaredDiff / (pixelCount * 3);

        return mse;
    }

    float CalculateMSE_GPU_Delta(Texture2D originTex, Texture2D resultTex)
    {
        float mse = 0f;

        deltaRenderTexture = new RenderTexture(originTex.width, originTex.height, 0, RenderTextureFormat.RFloat);
        deltaRenderTexture.enableRandomWrite = true;
        deltaRenderTexture.Create();

        gpuDeltaMaterial.SetTexture("_ResultTex", resultTex);
        Graphics.Blit(originTex, deltaRenderTexture, gpuDeltaMaterial);

        // 读取delta数据
        RenderTexture.active = deltaRenderTexture;
        Texture2D deltaTexture = new Texture2D(deltaRenderTexture.width, deltaRenderTexture.height, TextureFormat.RFloat, false);
        deltaTexture.ReadPixels(new Rect(0, 0, deltaRenderTexture.width, deltaRenderTexture.height), 0, 0);
        deltaTexture.Apply();

        Color[] texPixels = deltaTexture.GetPixels();

        foreach (Color c in texPixels)
        {
            mse += c.r;
        }

        return mse / (texPixels.Length * 3);
    }

    float CalculateMSE_GPU_MipMap()
    {
        int maxLevel = (int)Mathf.Log(Mathf.Max(deltaRenderTexture.width, deltaRenderTexture.height), 2);

        RenderTexture.active = deltaRenderTexture;
        Texture2D deltaTexture = new Texture2D(deltaRenderTexture.width, deltaRenderTexture.height, TextureFormat.RFloat, true);// 生成MipMap
        deltaTexture.ReadPixels(new Rect(0, 0, deltaRenderTexture.width, deltaRenderTexture.height), 0, 0);
        deltaTexture.Apply();

        Color[] texPixels = deltaTexture.GetPixels(maxLevel);

        return texPixels[0].r / 3;
    }

    float CalculateMse_GPU_Gather()
    {
        int maxLevel = (int)Mathf.Log(Mathf.Max(deltaRenderTexture.width, deltaRenderTexture.height), 2);
        int pixelCount = deltaRenderTexture.width * deltaRenderTexture.height;
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

    float CalculatePSNR(float mse)
    {
        float psnr = 10 * Mathf.Log10(1f / mse);

        return psnr;
    }

}
