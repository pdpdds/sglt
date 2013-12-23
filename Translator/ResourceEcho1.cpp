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

BYTE* ResourceEcho1::GetTextFromMessageRecord(BYTE* szMessageRecord)
{
	return (BYTE *)m_pStart + (*((USHORT*)(szMessageRecord + _textOffset)));
}

BOOL ResourceEcho1::SaveHeader( CArchive& ar, BYTE* DataChunkPtr )
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