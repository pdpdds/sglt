#include "StdAfx.h"
#include "ResourceGabriel.h"

ResourceGabriel::ResourceGabriel(int Num)
: SCITextResource(Num)
{
}

ResourceGabriel::~ResourceGabriel(void)
{
}

void ResourceGabriel::Init()
{
	_headerSize = 10;
	_recordSize = 11;
	_textOffset = 5;


}

BYTE* ResourceGabriel::GetTalker(int MessageIndex)
{
	BYTE* szMessageRecord = m_pData + MessageIndex * _recordSize;
	int talkerIndex = szMessageRecord[4];

	switch (talkerIndex)
	{
	case 1:
		return (BYTE*)gabrielActor[GABRIEL];
	case 2:
		return (BYTE*)gabrielActor[GRACE];
	case 99:
		return (BYTE*)gabrielActor[SITUATION];
	case 97:
		return (BYTE*)gabrielActor[SELECT];
	case 8:
		return (BYTE*)gabrielActor[MOSLY];
	case 26:
		return (BYTE*)gabrielActor[GRANDMOTHER];
	case 6:
		return (BYTE*)gabrielActor[FRIC];
	case 5:
		return (BYTE*)gabrielActor[MARIA];
	default:
		return (BYTE*)gabrielActor[UNKNOWN];
	}
}