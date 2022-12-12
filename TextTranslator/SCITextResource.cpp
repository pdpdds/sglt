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

bool isDoubleByte(USHORT chr) {
	USHORT ch = chr & 0xFF;
	if ((ch >= 0xA1) && (ch <= 0xFE))
		return true;

	if (((chr >= 0x81) && (chr <= 0x9F)) || ((chr >= 0xE0) && (chr <= 0xEF)))
		return true;

	return false;
}

BOOL SCITextResource::Load(CArchive& ar, USHORT num, UINT size)
{


	BYTE Type;
	USHORT Num;
	USHORT szPacked;
	USHORT szUnpacked;
	USHORT wCompression;

	m_fileHeader = 9;
	ar >> Type;
	ar >> m_Num;
	ar >> szPacked;
	ar >> szUnpacked;
	ar >> wCompression;

	m_Size = size - m_fileHeader;
	
	int Count = 0;

	if (m_Size != 0) {

		m_pStart = new BYTE[m_Size];
		memset(m_pStart, 0, sizeof(BYTE) * m_Size);

		ar.Read(m_pStart, m_Size);

		int TextLen = m_Size;
		char* seeker = (char*)m_pStart;
		USHORT curChar;
		while (TextLen > 0)
		{	
			curChar = (*(const byte*)seeker++);
			TextLen--;


			if (isDoubleByte(curChar)) {
				seeker++;
				TextLen--;
			}

			if ((*seeker) == 0) {
				
				Count++;
				TextLen--;

				if(TextLen > 0)
					seeker++;

			
			}
		}
	}
	
	
			
	m_MessageCnt = Count;


	return TRUE;
}

BOOL SCITextResource::Save()
{
	memset(DataChunk, 0, 65535);

	//if(GetChangeFlag() == FALSE)
	//	return FALSE;

	CFile File;

	if (!File.Open(m_FileName, CFile::modeCreate | CFile::modeWrite))
	{
		return FALSE;
	}	

	CArchive ar(&File, CArchive::store);

//20170519 문제가 있으면 주석 해제할 것
	//if(m_MessageCnt <= 0)
		//return TRUE;

	BYTE* DataChunkPtr = DataChunk;
	int offset = 0;
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

		/*mapBlankList::iterator iter2 = m_mapBlankList.find(i);

		if(iter2 != m_mapBlankList.end())
		{
			if(iter2->second > 0)
			{
				DataChunkPtr = DataChunkPtr + iter2->second;
			}
		}*/

		int t = str.GetLength();
			
		memcpy(DataChunkPtr, str.GetString(), str.GetLength());
		
//안녕 난 마더 구스야. 네가 와서 정말 기쁘구나! 내 동요가 모두 뒤죽박죽이 되었어. 내가 그걸 바로 잡아줄 수 있겠니?
		offset = str.GetLength() + 1;

		DataChunkPtr = DataChunkPtr + offset;
	}

	USHORT szPacked = DataChunkPtr - DataChunk;
	szPacked += 4;
	USHORT szUnpacked = szPacked;
	USHORT wCompression = 0;//kCompNone;

	BYTE Type = 3;

	BYTE Type2 = Type | 0x80;
	ar << Type;
	ar << m_Num;
	ar << szPacked;
	ar << szUnpacked;
	ar << wCompression;

	ar.Write(DataChunk, DataChunkPtr - DataChunk);

	return TRUE;
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
	int textlen = m_Size - m_fileHeader;
	char* seeker = (char *)m_pStart;


	while((textlen) && (*seeker) == 0)
	{
		textlen--;
		seeker++;
	}


	while (MessageIndex--)
	{
		while ((textlen--) && (*seeker++))
			;

		while((textlen) && (*seeker) == 0)
		{
			textlen--;
			seeker++;
		}
	}

	
	if (textlen)
	{
		szText =  seeker;
		return TRUE;
	}
	

	return FALSE;
}

int SCITextResource::FindText( CString& SearchText )
{
	for(int MessageIndex = 0; MessageIndex < m_MessageCnt; MessageIndex++)
	{
		
		CString szText;
		if(FALSE == ReadOriginalText(MessageIndex, szText))
		{
			return -1;
		}
		CString sContentsLower = szText.MakeLower();
		if (-1 != sContentsLower.Find(SearchText.MakeLower())) 

			//if(szText == SearchText)
		{
			return MessageIndex;
		}
	}

	return -1;
}