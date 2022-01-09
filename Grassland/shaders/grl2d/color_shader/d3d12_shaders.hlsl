struct VSInput
{
	float2 pos : DATA0;
	float4 color : DATA1;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 color: DATA0;
};

PSInput VSMain(
	VSInput input
)
{
	PSInput res;
	res.pos = float4(input.pos, 0.0, 1.0);
	res.color = input.color;
	return res;
}

struct PSOutput
{
	float4 Color0 : SV_TARGET0;
};

PSOutput PSMain(PSInput input)
{
	PSOutput res;
	res.Color0 = input.color;
	return res;
}