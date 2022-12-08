static const uint MAXLIGHTS = 3;
static const float PI = 3.14159265f;

struct RootConstants {
	matrix CameraMatrix;
	float4 ViewDirection;
	float4 ViewPosition;
};

ConstantBuffer<RootConstants> rootConstants : register(b0);

struct PointLight {
	float4 pos;
	float4 color;
};

cbuffer Lights : register(b1) {
	PointLight lights[MAXLIGHTS];
};

struct VSOutput {
	float4 pos : SV_POSITION;
	float4 wPos : WPOS;
	float4 n : NORMAL;
};

VSOutput vsMain( float4 pos : POSITION, float4 n : NORMAL ) {
	VSOutput ret = (VSOutput)0;
	ret.pos = mul(pos, rootConstants.CameraMatrix);
	ret.n = n;
	ret.wPos = pos;
	return ret;
}

float4 SchlickApprox(float theta, float3 r0) {
	theta = pow(1.0f - cos(theta), 5);
	float3 temp = float3(1.0f, 1.0f, 1.0f) - r0;
	temp.x *= theta;
	temp.y *= theta;
	temp.z *= theta;
	return float4(r0 + temp, 1.0f);
}

float4 GetSpecular(float4 pos, PointLight light, float4 n) {
	float4 ret;
	float4 lightDir = normalize(pos - light.pos);
	float4 h = normalize(normalize(rootConstants.ViewPosition - pos) - lightDir);
	float angle = dot(h, n);
	float scale = -dot(n, lightDir) * dot(rootConstants.ViewDirection, normalize(rootConstants.ViewPosition - pos));
	float distScale = 4 * PI;
	distScale /= length(pos - light.pos) * length(pos - light.pos);
	scale *= distScale;
	ret = SchlickApprox(angle, 1.0f) * (float4(1.0f, 1.0f, 1.0f, 1.0f) - float4(1.0f, 1.0f, 1.0f, 1.0f));
	ret.x *= scale;
	ret.y *= scale;
	ret.z *= scale;
	return ret;
}

float4 GetDiffuse(float4 pos, PointLight light, float4 n) {
	float4 ret = light.color;
	float distScale = 4 * PI;
	distScale /= length(pos - light.pos) * length(pos - light.pos);
	float scale = dot(n, normalize(light.pos - pos)) * dot(rootConstants.ViewDirection, normalize(pos - rootConstants.ViewPosition)) * distScale;
	ret.x *= scale;
	ret.y *= scale;
	ret.z *= scale;
	return ret;
}

float4 psMain(VSOutput input) : SV_TARGET{
	float4 light = {0.0f,0.0f,0.0f,1.0f};
	for (int i = 0; i < MAXLIGHTS; i++) {
		light += GetDiffuse(input.wPos, lights[i], normalize(input.n)) + GetSpecular(input.wPos, lights[i], normalize(input.n));
	}
	return light;
}