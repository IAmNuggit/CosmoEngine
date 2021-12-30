#pragma once
/// The OmniShadowMap HEADER 
#include "ShadowMap.h"

class OmniShadowMap :
	public ShadowMap
{
public:
	OmniShadowMap();
	//Setting up texture
	bool Init(unsigned int width, unsigned int height);

	void Write();

	void Read(GLenum TextureUnit);

	~OmniShadowMap();
};

