texture guiTexture : Diffuse;

sampler2D guiSampler: register(s0) = sampler_state 
{
    Texture = <guiTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

float2 ViewPortSize : ViewPortSize;

struct VertexIn
{
    float4 Position	: POSITION; // paked: x, y; u, v
    float4 Color	: COLOR;
};

struct VertexOut
{
    float4 Position : POSITION;
    float4 Color	: COLOR;
    float2 Tex		: TEXCOORD0;
}; 

//------------------------------------
VertexOut GuiVS(VertexIn IN) 
{
    VertexOut OUT;
  	        
    OUT.Position = IN.Position;
    
    OUT.Position.x = (2.0 * IN.Position.x / ViewPortSize.x) - 1.0;
    OUT.Position.y = 1.0 - (2.0 * IN.Position.y / ViewPortSize.y);
    OUT.Position.z = 1.0;
    OUT.Position.w = 1.0;    
    
    OUT.Tex.xy = IN.Position.zw;
    OUT.Color = IN.Color;
       	   	    
    return OUT;
}

float4 GuiPS(VertexOut IN) : COLOR 
{
  return tex2D(guiSampler, IN.Tex) * IN.Color;    
}

//-----------------------------------
technique Simple
{
    pass p0 
    {	
    	ZEnable = false;
    	ZWriteEnable = false;
    	
    	CullMode = None;
    
    	AlphaBlendEnable = true;
    	SrcBlend = SrcAlpha;
	    DestBlend = InvSrcAlpha;
	    
		VertexShader = compile vs_2_0 GuiVS();
		PixelShader = compile ps_2_0 GuiPS();
    }    
}