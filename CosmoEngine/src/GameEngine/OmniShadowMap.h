#pragma once
/// The OmniShadowMap class initilises the shadow cubemap and renders it to the screen 
///
/// This class inherits from the regular shadow map and is designed to work
/// similar to the regular shadow map but in every direction   
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

