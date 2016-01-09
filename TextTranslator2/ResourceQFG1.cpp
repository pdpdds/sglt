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
	//_headerSize = 8;
	//_recordSize = 10;
	//_textOffset = 5;
	_headerSize = 8;
	_recordSize = 10;
	_textOffset = 5;
}

BYTE* ResourceQFG1::GetTextFromMessageRecord(BYTE* szMessageRecord)
{
	return (BYTE *)m_pStart + (*((USHORT*)(szMessageRecord + _textOffset)));
}

BOOL ResourceQFG1::SaveHeader( CArchive& ar, BYTE* DataChunkPtr )
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
