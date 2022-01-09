struct VSInput
{
	float2 pos : DATA0;
	float3 texcoord : DATA1;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 texcoord: DATA0;
};

PSInput VSMain(
	VSInput input
)
{
	PSInput res;
	res.pos = float4(input.pos, 0.0, 1.0);
	res.texcoord = input.texcoord;
	return res;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


struct PSOutput
{
	float4 Color0 : SV_TARGET0;
};

PSOutput PSMain(PSInput input)
{
	PSOutput res;
	res.Color0 = g_texture.Sample(g_sampler, input.texcolor.xy);
	res.Color0.w = res.Color0.w * input.texcolor.z;
	return res;
}