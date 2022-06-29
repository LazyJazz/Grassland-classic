struct VSInput
{
	float4 pos : DATA0;
	float4 norm : DATA1;
	float4 tex : DATA2;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

struct PSOutput
{
	float4 color0 : SV_TARGET0;
};

PSInput VSMain(VSInput vs_in)
{
	PSInput res;
	res.pos = vs_in.pos;
	res.color = vs_in.tex;
	return res;
}

PSOutput PSMain(PSInput ps_in)
{
	PSOutput res;
	res.color0 = ps_in.color;
	return res;
}
