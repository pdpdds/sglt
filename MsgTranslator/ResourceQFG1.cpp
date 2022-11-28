#include "StdAfx.h"
#include "ResourceQFG1.h"

ResourceQFG1::ResourceQFG1(int Num)
: SCITextResource(Num)
{
}

ResourceQFG1::~ResourceQFG1(void)
{
}

void ResourceQFG1::Init()
{
	_headerSize = 8;
	_recordSize = 10;
	_textOffset = 5;
}