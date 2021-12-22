cbuffer GlobalSettings: register(b0)
{
	float4x4 transform;
	int mode;
	float __padding[47];
};

struct VSInput
{
	float2 pos : DATA0;
	float4 texcolor : DATA1;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 texcolor: DATA0;
};

PSInput VSMain(
	VSInput input
)
{
	PSInput res;
	res.pos = mul(transform, float4(input.pos, 0.0, 1.0));
	res.texcolor = input.texcolor;
	return res;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


struct PSOutput
{
	float4 Color0 : SV_TARGET0;
	//float4 Color1 : SV_TARGET1;
	//float4 Color2 : SV_TARGET2;
	//float4 Color3 : SV_TARGET3;
};

PSOutput PSMain(PSInput input)
{
	PSOutput res;
	if (mode == 1) // Texture Mode.
	{
		res.Color0 = g_texture.Sample(g_sampler, input.texcolor.xy);
		res.Color0.w = res.Color0.w * input.texcolor.w;
	}
	else // Color Mode
	{
		res.Color0 = input.texcolor;
	}
	return res;
}