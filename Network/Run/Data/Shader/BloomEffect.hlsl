#include "common.hlsl"

cbuffer material_constants : register(b5){
    float TEXTURE_X;
    float TEXTURE_Y;
    float2 pad;
}


// input to the vertex shader - for now, a special input that is the index of the vertex we're drawing
struct vs_input_t
{
    uint vidx : SV_VERTEXID; // SV_* stands for System Variable (ie, built-in by D3D11 and has special meaning)
                             // in this case, is the the index of the vertex coming in.
};

// Textures & Samplers are also a form of constant
// data - unform/constant across the entire call
// tSlots are for big things
Texture2D <float4> tNormalColor     : register(t0);   // color of the surface
Texture2D <float4> tDiffuse         : register(t1);   // color of the surface
SamplerState sSampler : register(s0);     // sampler are rules on how to sample color per pixel


                                          // Output from Vertex Shader, and Input to Fragment Shader
                                          // For now, just position
struct VertexToFragment_t
{
    // SV_POSITION is a semantic - or a name identifying this variable. 
    // Usually a semantic can be any name we want - but the SV_* family
    // usually denote special meaning (SV = System Variable I believe)
    // SV_POSITION denotes that this is output in clip space, and will be 
    // use for rasterization.  When it is the input (pixel shader stage), it will
    // actually hold the pixel coordinates.
    float4 position : SV_POSITION;
    float2 uv : UV; 
};

//--------------------------------------------------------------------------------------
// constants
//--------------------------------------------------------------------------------------
// The term 'static' refers to this an built into the shader, and not coming
// from a contsant buffer - which we'll get into later (if you remove static, you'll notice
// this stops working). 
static float3 POSITIONS[3] = {
    float3(-1.0f, -1.0f, 0.0f), 
    float3( 3.0f, -1.0f, 0.0f), 
    float3(-1.0f,  3.0f, 0.0f)
};

static float2 UVS[3] = {
    float2( 0.0f,  1.0f ), 
    float2( 2.0f,  1.0f ), 
    float2( 0.0f,  -1.0f )
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VertexToFragment_t VertexFunction( vs_input_t input )
{
    VertexToFragment_t v2f = (VertexToFragment_t)0;

    // The output of a vertex shader is in clip-space, which is a 4D vector
    // so we need to convert out input to a 4D vector.
    v2f.position = float4( POSITIONS[input.vidx], 1.0f );
    v2f.uv = UVS[input.vidx]; 

    // And return - this will pass it on to the next stage in the pipeline;
    return v2f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
// If I'm only returning one value, I can optionally just mark the return value with
// a SEMANTIC - in this case, SV_TARGET0, which means it is outputting to the first colour 
// target.
float4 FragmentFunction( VertexToFragment_t input ) : SV_Target0 // semeantic of what I'm returning
{
    float tex_width;
    float tex_height;
    tDiffuse.GetDimensions( tex_width, tex_height );
    float delta_u = 1 / tex_width;
    float delta_v = 1 / tex_height;
    int up_bound = 1;
    int low_bound = -1;

    
    float4 sum_color = float4( 0, 0, 0, 0 );
    for( int i = low_bound; i <= up_bound; i++ ) {
        for( int j = low_bound; j <= up_bound; j++ ) {
            float2 offset = float2( delta_u * i, delta_v * j );
            float2 final_uv = input.uv + offset;
            final_uv.x = clamp( final_uv.x, 0, 1 );
            final_uv.y = clamp( final_uv.y, 0, 1 );
            sum_color += tDiffuse.Sample( sSampler, final_uv );
        }
    }
    sum_color /= 9;
    float4 original_color = tDiffuse.Sample( sSampler, input.uv );
    float4 bloom = tNormalColor.Sample( sSampler, input.uv );
    float bloom_amount = ( bloom.x + bloom.y + bloom.z ) / 3;
    bloom_amount = smoothstep( 0.f, 1.f, bloom_amount );
    float4 final_color = lerp( original_color, sum_color, bloom_amount );
    return float4( final_color.xyz, 1 );
}