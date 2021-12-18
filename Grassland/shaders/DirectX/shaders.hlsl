cbuffer GlobalSettings: register(b0)
{
	float4x4 transform;
	int mode;
	float __padding[47];
};

//Texture2D g_texture : register(t0);
//SamplerState g_sampler_nearest : register (s0);

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Normal : DATA0;
	float4 TexCoord : DATA1;
};

PSInput VSMain(
	float4 pos : DATA0,
	float4 norm : DATA1,
	float4 texcoord : DATA2
	)
{
	PSInput res;
	res.Position = mul(transform, pos);
	res.Normal = mul(transform, norm);
	res.TexCoord = texcoord;
	return res;
}

Texture2D g_texture : register(t0);
Texture2D g_texture2 : register(t1);
SamplerState g_sampler : register(s0);
SamplerState g_sampler_linear : register(s1);


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
	//res.Color0 = input.TexCoord;
	//res.Color0 = input.Position;
	if (mode)
	{
		float scale = max(dot(normalize(input.Normal), normalize(float4(0.0, 1.0, -1.0, 0.0))), 0.0) * 0.5;
		scale += 0.5;
		res.Color0 = 
		//g_texture.Sample(g_sampler, input.TexCoord.xy);
		 (g_texture.Sample(g_sampler, input.TexCoord.xy) + g_texture2.Sample(g_sampler, input.TexCoord.xy)) * 0.5;
		res.Color0 = float4(res.Color0.xyz * scale, res.Color0.w);
		//res.Color0 = (input.Normal + 1.0) * 0.5;
		//res.Color0 = input.TexCoord;
	}
	else res.Color0 = float4(input.TexCoord.xyz, input.TexCoord.w);


	return res;
}