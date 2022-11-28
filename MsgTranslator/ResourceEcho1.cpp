#include "StdAfx.h"
#include "ResourceEcho1.h"

ResourceEcho1::ResourceEcho1(int Num)
: SCITextResource(Num)
{
}

ResourceEcho1::~ResourceEcho1(void)
{
}

void ResourceEcho1::Init()
{
	_headerSize = 6;
	_recordSize = 4;
	_textOffset = 2;
}