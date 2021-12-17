

struct VSInput
{
	float3 pos : DATA0;
	float3 color : DATA1;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

struct PSOutput
{
	float4 color0 : SV_TARGET0;
};

PSInput VSMain(VSInput vs_in)
{
	PSInput res;
	res.pos = float4(vs_in.pos, 1.0);
	res.color = vs_in.color;
	return res;
}

PSOutput PSMain(PSInput ps_in)
{
	PSOutput res;
	res.color0 = float4(ps_in.color.xyz, 1.0);
	return res;
}