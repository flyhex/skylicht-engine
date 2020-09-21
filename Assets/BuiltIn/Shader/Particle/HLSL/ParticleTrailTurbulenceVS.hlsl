struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorld;
	float3 uNoiseParam;
};
float hash(float3 p)
{
    p = frac( p*0.3183099+.1 );
	p *= 17.0;
    return frac( p.x*p.y*p.z*(p.x+p.y+p.z) );
}
float noise( in float3 x )
{
    float3 i = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    return lerp(lerp(lerp( hash(i+float3(0,0,0)),
                        hash(i+float3(1,0,0)),f.x),
                   lerp( hash(i+float3(0,1,0)),
                        hash(i+float3(1,1,0)),f.x),f.y),
               lerp(lerp( hash(i+float3(0,0,1)),
                        hash(i+float3(1,0,1)),f.x),
                   lerp( hash(i+float3(0,1,1)),
                        hash(i+float3(1,1,1)),f.x),f.y),f.z);
}
static const float3x3 m = float3x3( 0.00, 0.80, 0.60,
                    -0.80, 0.36, -0.48,
                    -0.60, -0.48, 0.64 );
float pnoise( float3 q )
{
	float f = 0.5000*noise( q );
	q = mul(q, m)*2.01;
	f += 0.2500*noise( q );
	q = mul(q, m)*2.02;
	f += 0.1250*noise( q );
	q = mul(q, m)*2.03;
	f += 0.0625*noise( q );
	q = mul(q, m)*2.01;
	return f;
}
float fbm(float3 p)
{
	float z=2.;
	float rz = 0.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	return rz;
}
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float3 worldPos = mul(input.pos, uWorld).xyz;
	float n = pnoise(worldPos * uNoiseParam.x) * 2.0 - 1.0;
	float weight = clamp(input.tex0.y * uNoiseParam.z, 0.0, 1.0);
	float4 noisePosition = input.pos + float4(n * input.norm * weight * uNoiseParam.y, 0.0);
	output.pos = mul(noisePosition, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	return output;
}