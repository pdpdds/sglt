#include "StdAfx.h"
#include "MsgController.h"
#include "SCITextResource.h"

static const char *const s_resourceTypeNames[] = {
	"view", "pic", "script", "text", "sound",
	"memory", "vocab", "font", "cursor",
	"patch", "bitmap", "palette", "cdaudio",
	"audio", "sync", "message", "map", "heap",
	"audio36", "sync36", "xlate", "robot", "vmd",
	"chunk", "animation", "etc", "duck", "clut",
	"tga", "zzz", "macibin", "macibis", "macpict"
};

int texFileMax = 2200;

MsgController::MsgController(CTranslatorDlg* pOwner)
: m_pOwner(pOwner)
, m_TextResourceCnt(0)
, m_TextNum(0)
{
}

MsgController::~MsgController(void)
{
	Finally();
}

BOOL MsgController::Finally()
{
	listTextResPair::iterator iter = m_listTextResPair.begin();

	for(iter; iter != m_listTextResPair.end();)
	{

		TextResPair ResPair = *iter;
		delete ResPair.pOriginalTextRes;
		delete ResPair.pTranslatedTextRes;

		iter = m_listTextResPair.erase(iter);
	}

	return TRUE;
}

BOOL MsgController::Load()
{
	int TextResourceCnt = 0;
	for (int i = 0; i < texFileMax; i++)
	{
		if(i == 900)
		{
			int j = 1;
		}

		char szFileName[50];
		sprintf(szFileName, "%s.%03d", s_resourceTypeNames[3], i);

		
		TextResPair sTextResPair;
		SCITextResource* pOriTextRes = new SCITextResource(i);

		CString szOriginalPath = "./Original/";
		szOriginalPath += szFileName;
		if(FALSE == pOriTextRes->Load(szOriginalPath))

		{
			delete pOriTextRes;
			continue;
		}

		SCITextResource* pTranslateTextRes = new SCITextResource(i);
		CString szTranslatePath = "./Translated/";
		szTranslatePath += szFileName;
		if(FALSE == pTranslateTextRes->Load(szTranslatePath))
			ASSERT(0);

		sTextResPair.pOriginalTextRes = pOriTextRes;
		sTextResPair.pTranslatedTextRes = pTranslateTextRes;

		m_listTextResPair.push_back(sTextResPair);

		TextResourceCnt++;
	}

	m_TextResourceCnt = TextResourceCnt;
	m_iter = m_listTextResPair.begin();
	return TRUE;
}

BOOL MsgController::Save()
{
	listTextResPair::iterator iter = m_listTextResPair.begin();

	for(iter; iter != m_listTextResPair.end(); iter++)
	{
		SCITextResource* pRes = iter->pTranslatedTextRes;
		pRes->Save();
	}

	AfxMessageBox("Save Complete");
	
	return TRUE;
}

BOOL MsgController::GetText(CString& str, CString& TranStr)
{
	TextResPair Pair = *m_iter;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetNextText(CString& str, CString& TranStr)
{
	m_TextNum++;

	if(m_TextNum >= m_iter->pOriginalTextRes->GetMessageCnt())
	{
		m_iter++;
		m_TextNum--;

		if(m_iter == m_listTextResPair.end())
		{
			m_iter--;
			
			return FALSE;
		}

		m_TextNum = 0;
	}

	TextResPair Pair = *m_iter;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetPrevText(CString& str, CString& TranStr)
{
	m_TextNum--;
	if(m_TextNum < 0)
	{
		if(m_iter == m_listTextResPair.begin())
		{
			m_TextNum = 0;
			return FALSE;
		}

		m_iter--;
		m_TextNum = m_iter->pOriginalTextRes->GetMessageCnt() - 1;
	}

	TextResPair Pair = *m_iter;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

CString& MsgController::GetCurFileName()
{
	return m_iter->pOriginalTextRes->GetFileName();

}

BOOL MsgController::GetNextTextRes(CString& str, CString& TranStr)
{
		
	m_iter++;

	if(m_iter == m_listTextResPair.end())
	{
		m_iter--;
		return FALSE;
	}

	m_TextNum = 0;

	TextResPair Pair = *m_iter;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetPrevTextRes(CString& str, CString& TranStr)
{
	
	if(m_iter == m_listTextResPair.begin())
	{
		return FALSE;
	}

	m_iter--;
	m_TextNum = 0;

	TextResPair Pair = *m_iter;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::SaveText( CString& szStr )
{
	TextResPair Pair = *m_iter;
	SCITextResource* pRes = Pair.pTranslatedTextRes;
	pRes->SetChangeFlag(TRUE);
	pRes->SetSaveText(m_TextNum, szStr);
	

	return TRUE;
}

BOOL MsgController::GenerateOutput()
{
	if (PackText() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	if (CreateTextMap() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	AfxMessageBox("Output Complete");

	return TRUE;
}

BOOL MsgController::CreateTextMap()
{


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

	CFile WriteFile;
	if (!WriteFile.Open("text.map", CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}

	CArchive ar(&WriteFile, CArchive::store);
	BYTE ResType = 3; //텍스트, 메세지의 구별된다.
	USHORT Offset = 6;
	ar << ResType;
	ar << Offset;
	BYTE ResFin = 0xFF;;//리소스 엔드 마크
	ar << ResFin;

	Offset = m_mapTextResInfo.size() * 6;
	ar << Offset;

	mapTextResInfo::iterator iter = m_mapTextResInfo.begin();

	UINT newfileoffset = 0;
	for(;iter != m_mapTextResInfo.end(); iter++)
	{
		USHORT number = iter->first;
		ar << number;
		ar << newfileoffset;
		newfileoffset += iter->second;
	}

	return TRUE;
}

BOOL MsgController::PackText()
{
	m_mapTextResInfo.clear();
	CFile File;

	if (!File.Open("Text.res", CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::store);

	for (int i = 0; i < texFileMax; i++)
	{
		char szFileName[50];
		sprintf(szFileName, "Translated/%s.%03d", s_resourceTypeNames[3], i);

		CFile MsgFile;

		if(!MsgFile.Open(szFileName, CFile::modeRead))
		{
			continue;
		}

		int Size = MsgFile.GetLength();

		ASSERT(Size > 0);

		m_mapTextResInfo.insert(std::make_pair(i, Size));

		BYTE* Buffer = new BYTE[Size];
		memset(Buffer, 0, sizeof(BYTE) * Size);

		CArchive ar2(&MsgFile, CArchive::load);
		ar2.Read(Buffer, Size);

		ar.Write(Buffer, Size);

		delete Buffer;

	}
	return TRUE;
}

BOOL MsgController::FindText( CString& SearchText )
{
	listTextResPair::iterator iter = m_listTextResPair.begin();
	BOOL bFind = FALSE;
	int messageindex = -1;

	for(;iter!= m_listTextResPair.end(); iter++)
	{
		TextResPair Pair = *iter;
		messageindex = Pair.pOriginalTextRes->FindText(SearchText);
		if(messageindex >= 0)
		{
			m_iter = iter;
			m_TextNum = messageindex;
			bFind = TRUE;
			break;
		}
	}

	return bFind;
}