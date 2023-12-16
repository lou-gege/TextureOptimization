Shader "Custom/TestShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader
    {


        Pass
        {


            CGPROGRAM

            #pragma vertex vert
            #pragma fragment frag

            ENDCG
        }
    }

    CGINCLUDE

    #include "UnityCG.cginc"

    sampler2D _MainTex;

    struct VertexInput
    {
        float4 vertex : POSITION;
    };

    struct VertexOutput
    {
        float4 pos : SV_POSITION;
    };

    VertexOutput vert (VertexInput v)
    {
        VertexOutput o;
        o.pos = UnityObjectToClipPos(v.vertex);

        return o;
    }

    float4 frag (VertexOutput i) : SV_Target
    {
        // return tex2D(_MainTex, half2(0.125f, 0.125f));
        return tex2D(_MainTex, float2(0.0598155f, 0.176094f));
        // return fixed4(229.5373/255.0, 229.5374/255.0, 0.8, 1.0);
    }

    ENDCG

    FallBack off
}
