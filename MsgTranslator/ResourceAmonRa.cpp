#include "StdAfx.h"
#include "ResourceAmonRa.h"

ResourceAmonRa::ResourceAmonRa(int Num)
: SCITextResource(Num)
{
}

ResourceAmonRa::~ResourceAmonRa(void)
{
}

void ResourceAmonRa::Init()
{
	_headerSize = 10;
	_recordSize = 11;
	_textOffset = 5;
}