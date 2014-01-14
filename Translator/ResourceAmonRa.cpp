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
	//_headerSize = 8;
	//_recordSize = 10;
	//_textOffset = 5;
	_headerSize = 10;
	_recordSize = 11;
	_textOffset = 5;
}

BYTE* ResourceAmonRa::GetTextFromMessageRecord(BYTE* szMessageRecord)
{
	return (BYTE *)m_pStart + (*((USHORT*)(szMessageRecord + _textOffset)));
}

BOOL ResourceAmonRa::SaveHeader( CArchive& ar, BYTE* DataChunkPtr )
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
