#include "StdAfx.h"
#include "SCITextResource.h"

BYTE SCITextResource::DataChunk[65536] = {0,};

SCITextResource::SCITextResource(int Num)
: m_pStart(NULL)
, m_MessageCnt(0)
, m_bChanged(FALSE)
, m_Num(Num)
{
}

SCITextResource::~SCITextResource(void)
{
	if(m_pStart)
		delete m_pStart;
}

BOOL SCITextResource::Load( const CString& szFileName )
{
	
	Init();

	CFile File;

	if(!File.Open(szFileName, CFile::modeRead))
	{
		return FALSE;
	}

	m_Size = File.GetLength();

	m_pStart = new BYTE[m_Size];
	memset(m_pStart, 0, sizeof(BYTE) * m_Size);

	CArchive ar(&File, CArchive::load);

	BYTE Type;
	USHORT Num;
	USHORT szPacked;
	USHORT szUnpacked;
	USHORT wCompression;

	ar >> Type;
	ar >> m_Num;
	ar >> szPacked;
	ar >> szUnpacked;
	ar >> wCompression;

	ar.Read(m_pStart, m_Size - 9);

//	ar.Read(m_pStart, m_Size);

	File.Close();

	m_MessageCnt = *(USHORT*)(m_pStart + _headerSize - 2);
	m_pData = m_pStart + _headerSize;
	m_FileName = szFileName;

	return TRUE;
}

BOOL SCITextResource::Save()
{
	memset(DataChunk, 0, 65535);

	//if(GetChangeFlag() == FALSE)
	//	return FALSE;

	CFile File;

	if(!File.Open(m_FileName, CFile::modeWrite))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::store);

	if(m_MessageCnt <= 0)
		return TRUE;

	BYTE* pHeader = new BYTE[_headerSize + m_MessageCnt * _recordSize];
	memcpy(pHeader, m_pStart, _headerSize + m_MessageCnt * _recordSize);
	BYTE* pTextEntry = pHeader + _headerSize;

	
	BYTE* pTextData = m_pStart + _headerSize + m_MessageCnt * _recordSize;
	USHORT offset = 0;
	BYTE* DataChunkPtr = DataChunk;

	for(int i = 0; i < m_MessageCnt; i++)
	{
		CString str;

		mapNewText::iterator iter = m_mapNewText.find(i);
		if(iter != m_mapNewText.end())
		{
			str = iter->second.Str;
		}
		else
		{
			
			ReadOriginalText(i, str);
		}

		int t = str.GetLength();
			
		memcpy(DataChunkPtr, str.GetString(), str.GetLength());
		(*((USHORT*)(pTextEntry + _textOffset))) = _headerSize + m_MessageCnt * _recordSize + offset;

		offset += str.GetLength() + 1;

		DataChunkPtr = DataChunk + offset;
		pTextEntry += _recordSize;
	}

	SaveHeader(ar, DataChunkPtr);

	/*UINT32 szPacked = 8 + _headerSize + m_MessageCnt * _recordSize + DataChunkPtr - DataChunk;
	UINT32 szUnpacked = szPacked;
	USHORT wCompression = 0;//kCompNone;

	BYTE Type = 15;
	BYTE HeaderSize = 0;

	BYTE Type2 = Type | 0x80;
	//ar << Type2;
	//ar << HeaderSize;
	ar << Type;
	ar << m_Num;
	ar << szPacked;
	ar << szUnpacked;
	ar << wCompression;*/

	ar.Write(pHeader, _headerSize + m_MessageCnt * _recordSize);
	ar.Write(DataChunk, DataChunkPtr - DataChunk);

	return TRUE;
}

BYTE* SCITextResource::GetTextFromMessageRecord(BYTE* szMessageRecord)
{
	return (BYTE*)m_pStart + (*((USHORT*)(szMessageRecord + _textOffset)));
}

BOOL SCITextResource::ReadText(int MessageIndex, CString& szText )
{
	if(MessageIndex >= m_MessageCnt)
		return FALSE;

	mapNewText::iterator iter = m_mapNewText.find(MessageIndex);

	if(iter != m_mapNewText.end())
	{
		NewText& str = iter->second;
		szText = str.Str;
		return TRUE;
	}

	return ReadOriginalText(MessageIndex, szText);
}

void SCITextResource::SetText( int TextNum, CString& szStr )
{

	mapNewText::iterator iter = m_mapNewText.find(TextNum);

	if(iter != m_mapNewText.end())
	{
		NewText& str = iter->second;
		str.Str = szStr;
	}
	else
	{
		NewText Text;

		Text.Str = szStr;
		m_mapNewText.insert(std::make_pair(TextNum, Text));
	}
}

BOOL SCITextResource::ReadOriginalText(int MessageIndex, CString& szText )
{
	BYTE* szMessageRecord = m_pData + MessageIndex * _recordSize;

	szText = GetTextFromMessageRecord(szMessageRecord);

	//szText = (const char *)m_pStart + (*((USHORT*)(szTextHeader + 5)));

	return TRUE;
}



int SCITextResource::FindText( CString& SearchText )
{
	for(int MessageIndex = 0; MessageIndex < m_MessageCnt; MessageIndex++)
	{
		CString szText;
		BYTE* szMessageRecord = m_pData + MessageIndex * _recordSize;

		szText = GetTextFromMessageRecord(szMessageRecord);

		if(szText[0] == NULL)
			continue;

		CString sTemp = szText;

		if(sTemp.IsEmpty())
			continue;

		//CString sContentsLower = sTemp.MakeLower();
		if (-1 != sTemp.Find(SearchText)) 

		//if(szText == SearchText)
		{
			return MessageIndex;
		}
	}

	return -1;
}

BOOL SCITextResource::SaveHeader(CArchive& ar, BYTE* DataChunkPtr)
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