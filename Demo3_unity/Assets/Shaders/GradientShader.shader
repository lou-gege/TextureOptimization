Shader "Custom/GradientShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _TruthTex ("Texture", 2D) = "white" {}
        _GradientPart ("GradientPart", Float) = 0.0
        // _Step ("Step", Float) = 1.0
    }
    SubShader
    {
        ZWrite Off
        Blend off

        Pass
        {
            ZTest off
            Cull off

            CGPROGRAM

            #pragma vertex vert
            #pragma fragment frag

            ENDCG
        }
    }

    CGINCLUDE

    #include "UnityCG.cginc"

    sampler2D _MainTex, _TruthTex;
    float _GradientPart;

    struct VertexInput
    {
        float4 vertex : POSITION;
        half2 texcoord : TEXCOORD0;
    };

    struct VertexOutput
    {
        float4 pos : SV_POSITION;
        half2 uv : TEXCOORD0;
    };

    VertexOutput vert (VertexInput v)
    {
        VertexOutput o;
        o.pos = UnityObjectToClipPos(v.vertex);
        o.uv = v.texcoord;

        return o;
    }

    float4 frag (VertexOutput i) : SV_Target
    {
        float4 truthCol = tex2D(_TruthTex, i.uv);
        float4 currCol = tex2D(_MainTex, i.uv);
        
        float3 gradient = _GradientPart * (currCol.rgb - truthCol.rgb);
        float3 col = currCol.rgb - gradient;

        return float4(col, 1.0);
    }

    ENDCG

    FallBack off
}
