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

BYTE* ResourceGabriel::GetTextFromMessageRecord(BYTE* szMessageRecord)
{
	return (BYTE *)m_pStart + (*((USHORT*)(szMessageRecord + _textOffset)));
}

BOOL ResourceGabriel::SaveHeader( CArchive& ar, BYTE* DataChunkPtr )
{
	BYTE Type = 15;
	USHORT szPacked = 8 + _headerSize + m_MessageCnt * _recordSize + DataChunkPtr - DataChunk;
	USHORT szUnpacked = szPacked;
	USHORT wCompression = 0;//kCompNone;
	BYTE HeaderSize = 0;

	ar << Type;
	ar << m_Num;
	ar << szPacked;
	ar << szUnpacked;
	ar << wCompression;

	return TRUE;
}

BYTE* ResourceGabriel::GetTalker(int MessageIndex)
{
	BYTE* szMessageRecord = m_pData + MessageIndex * _recordSize;
	int talkerIndex = szMessageRecord[4];

	switch (talkerIndex)
	{
	case 1:
		return (BYTE*)gabrielActor[1];
	case 2:
		return (BYTE*)gabrielActor[2];
	case 99:
		return (BYTE*)gabrielActor[4];
	case 97:
		return (BYTE*)gabrielActor[5];
	case 8:
		return (BYTE*)gabrielActor[6];
	case 26:
		return (BYTE*)gabrielActor[7];
	case 6:
		return (BYTE*)gabrielActor[8];
	case 5:
		return (BYTE*)gabrielActor[9];
	default:
		return (BYTE*)gabrielActor[3];
	}
}