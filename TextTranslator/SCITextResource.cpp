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
	CFile File;
	static int aa = 0;
	
	m_mapBlankList.clear();

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

	File.Close();

	int TextLen = m_Size - 9;
	char* seeker = (char *) m_pStart;

	int Count = 0;

	while (TextLen != 0)
	{
	
		int BlankCount = 0;

		while((TextLen) && (*seeker) == 0)
		{
			BlankCount++;
			TextLen--;
			seeker++;
		}

//20130125 뒤죽박죽 요정 이야기 10바이트 파일 처리...
		if(TextLen == 0)
			break;//원래는 FALSE로 되어 있었다.. 안되면 수정 요망

		if(BlankCount > 0)
		{
			m_mapBlankList.insert(std::make_pair(Count, BlankCount));
		}

		while ((TextLen--) && (*seeker++));

		Count++;
	}
			
	m_MessageCnt = Count;
	aa++;

	if (aa == 140)
		int j = 1;

	if (m_MessageCnt == 0)
		int j = 1;
	

	m_pData = m_pStart;
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

		mapBlankList::iterator iter2 = m_mapBlankList.find(i);

		if(iter2 != m_mapBlankList.end())
		{
			if(iter2->second > 0)
			{
				DataChunkPtr = DataChunkPtr + iter2->second;
			}
		}

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
	int textlen = m_Size - 9;
	char* seeker = (char *) m_pData;


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