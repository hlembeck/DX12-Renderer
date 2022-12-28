cbuffer Cameras : register(b0) {
	matrix CameraMatrix[6];
};

struct GSOutput {
    float4 pos : SV_Position;
    uint arraySlice : SV_RenderTargetArrayIndex;
};

float4 vsMain(float4 pos : POSITION, float4 n : NORMAL) : SV_POSITION{
    return pos;
}

[maxvertexcount(18)]
void gsMain(triangle float4 input[3] : SV_POSITION, inout TriangleStream<GSOutput> OutStream)
{
    GSOutput output = (GSOutput)0;

    for (uint i = 0; i < 6; i++)
    {
        output.arraySlice = i;
        output.pos = mul(input[0], CameraMatrix[i]);
        OutStream.Append(output);
        output.pos = mul(input[1], CameraMatrix[i]);
        OutStream.Append(output);
        output.pos = mul(input[2], CameraMatrix[i]);
        OutStream.Append(output);
        OutStream.RestartStrip();
    }
}

float psMain(GSOutput input) : SV_TARGET {
	return 1.0f;
}