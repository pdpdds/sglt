﻿#include "StdAfx.h"
#include "MsgController.h"
#include "SCITextResource.h"
#include "resource.h"

#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorksheet.h"
#include "CWorkbooks.h"
#include "CWorksheets.h"
#include "CRange.h"
#include "CRanges.h"
#include <locale.h>
#include "TranslatorDlg.h"

static const TCHAR *const s_resourceTypeNames[] = {
	TEXT("view"), TEXT("pic"), TEXT("script"), TEXT("text"), TEXT("sound"),
	TEXT("memory"), TEXT("vocab"), TEXT("font"), TEXT("cursor"),
	TEXT("patch"), TEXT("bitmap"), TEXT("palette"), TEXT("cdaudio"),
	TEXT("audio"), TEXT("sync"), TEXT("message"), TEXT("map"), TEXT("heap"),
	TEXT("audio36"), TEXT("sync36"), TEXT("xlate"), TEXT("robot"), TEXT("vmd"),
	TEXT("chunk"), TEXT("animation"), TEXT("etc"), TEXT("duck"), TEXT("clut"),
	TEXT("tga"), TEXT("zzz"), TEXT("macibin"), TEXT("macibis"), TEXT("macpict")
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
	mapTextResPair::iterator iter = m_mapTextResPair.begin();

	for (iter; iter != m_mapTextResPair.end(); iter++)
	{

		TextResPair ResPair = iter->second;
		delete ResPair.pOriginalTextRes;
		delete ResPair.pTranslatedTextRes;		

		iter = m_mapTextResPair.erase(iter);
	}	

	return TRUE;
}

BOOL MsgController::LoadTextMap(CString& mapName) {

	CFile File;
	TCHAR buffer[256];
	GetCurrentDirectory(256, buffer);
	if (!File.Open(mapName, CFile::modeRead))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::load);
	BYTE ResType = 0; //텍스트, 메세지의 구별된다.
	USHORT element_size = 0;
	USHORT size = 0;

	ar >> ResType;
	ar >> element_size;
	BYTE ResFin = 0;//리소스 엔드 마크
	ar >> ResFin;

	ar >> size;

	USHORT number = 0;
	UINT msg_offset = 0;
	UINT byteRead = 0;
	UINT textCount = 0;
	while (1) {

		byteRead = ar.Read(&number, sizeof(USHORT));

		if (byteRead == 0)
			break;

		byteRead = ar.Read(&msg_offset, sizeof(UINT));

		if (byteRead == 0)
			break;
		
		m_mapTextResInfo.insert(std::make_pair(number, msg_offset));

		textCount++;
	}

	m_TextResourceCnt = size / element_size;

	ASSERT(m_TextResourceCnt == textCount);


	return TRUE;
}

BOOL MsgController::LoadResourceFile(CString& resName) {

	CFile File;
	TCHAR buffer[256];
	GetCurrentDirectory(256, buffer);
	if (!File.Open(resName, CFile::modeRead))
	{
		return FALSE;
	}

	mapTextResInfo::iterator iter = m_mapTextResInfo.begin();
	mapTextResInfo::iterator iter2 = iter;
	
	CArchive ar(&File, CArchive::load);

	for (; iter != m_mapTextResInfo.end(); iter++)
	{
		TextResPair sTextResPair;
		SCITextResource* pOriTextRes = new SCITextResource(iter->first);

		CString str;
		str.Format(TEXT("%d"), iter->first);

		GetOwner()->m_list.InsertItem(str.GetString(), 0/* nImage */, 1/* nSelectedImage */, TVI_ROOT, TVI_LAST);
	
		iter2++;
		UINT resSize = 0;

		if (iter2 != m_mapTextResInfo.end())
			resSize = iter2->second - iter->second;
		else
			resSize = File.GetLength() - iter->second;

		if (FALSE == pOriTextRes->Load(ar, iter->first, resSize))
		{
			delete pOriTextRes;
			continue;
		}

		SCITextResource* pTranslateTextRes = new SCITextResource(iter->first);


		//	if (FALSE == pTranslateTextRes->Load(ar, iter->second))
			//	ASSERT(0);

		sTextResPair.pOriginalTextRes = pOriTextRes;
		sTextResPair.pTranslatedTextRes = pTranslateTextRes;

		m_mapTextResPair.insert(std::make_pair(iter->first, sTextResPair));
	

	}

	m_iter = m_mapTextResPair.begin();

	return TRUE;
}

BOOL MsgController::Load(CString& mapName, CString& resName)
{
	if (!LoadTextMap(mapName) || !LoadResourceFile(resName))
		return FALSE;
	
	return TRUE;
}

void MsgController::Load2ListControl(USHORT index) {

	GetOwner()->m_listCtrl.DeleteAllItems();

	mapTextResPair::iterator iter = m_mapTextResPair.find(index);

	if (iter != m_mapTextResPair.end()) {
		SCITextResource* pOriginal = iter->second.pOriginalTextRes;
		SCITextResource* pTranslated = iter->second.pTranslatedTextRes;

		int count = pOriginal->GetMessageCnt();
		for (int j = 0; j < count; j++)
		{
			CString szText;
			if (FALSE == pOriginal->ReadText(j, szText))
			{
				ASSERT(0);
				return;
			}

			CString str;
			str.Format(_T("%d"), j);
			GetOwner()->m_listCtrl.InsertItem(j, str);

			GetOwner()->m_listCtrl.SetItem(j, 1, LVIF_TEXT, szText, NULL, NULL, NULL, NULL);
			GetOwner()->m_listCtrl.SetItem(j, 2, LVIF_TEXT, szText, NULL, NULL, NULL, NULL);
		}

	}
	
}

BOOL MsgController::Save()
{
	mapTextResPair::iterator iter = m_mapTextResPair.begin();

	for (iter; iter != m_mapTextResPair.end(); iter++)
	{
		SCITextResource* pRes = iter->second.pTranslatedTextRes;
		pRes->Save();
	}

	AfxMessageBox(TEXT("Save Complete"));
	
	return TRUE;
}

BOOL MsgController::GetText(int selectedRes, int SelectedIndex, CString& str, CString& TranStr)
{
	mapTextResPair::iterator iter = m_mapTextResPair.find(selectedRes);

	if (iter == m_mapTextResPair.end())
		return FALSE;

	TextResPair Pair = iter->second;
	return Pair.pOriginalTextRes->ReadText(SelectedIndex, str) && Pair.pTranslatedTextRes->ReadText(SelectedIndex, TranStr);
}

BOOL MsgController::GetNextText(CString& str, CString& TranStr)
{
	m_TextNum++;

	if(m_TextNum >= m_iter->second.pOriginalTextRes->GetMessageCnt())
	{
		m_iter++;
		m_TextNum--;

		if (m_iter == m_mapTextResPair.end())
		{
			m_iter--;
			
			return FALSE;
		}

		m_TextNum = 0;
	}

	TextResPair Pair = m_iter->second;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetPrevText(CString& str, CString& TranStr)
{
	m_TextNum--;
	if(m_TextNum < 0)
	{
		if (m_iter == m_mapTextResPair.begin())
		{
			m_TextNum = 0;
			return FALSE;
		}

		m_iter--;
		m_TextNum = m_iter->second.pOriginalTextRes->GetMessageCnt() - 1;
	}

	TextResPair Pair = m_iter->second;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

CString& MsgController::GetCurFileName()
{
	return m_iter->second.pOriginalTextRes->GetFileName();

}

BOOL MsgController::GetNextTextRes(CString& str, CString& TranStr)
{
		
	m_iter++;

	if (m_iter == m_mapTextResPair.end())
	{
		m_iter--;
		return FALSE;
	}

	m_TextNum = 0;

	TextResPair Pair = m_iter->second;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetPrevTextRes(CString& str, CString& TranStr)
{
	
	if (m_iter == m_mapTextResPair.begin())
	{
		return FALSE;
	}

	m_iter--;
	m_TextNum = 0;

	TextResPair Pair = m_iter->second;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::SaveText( CString& szStr )
{
	TextResPair Pair = m_iter->second;
	SCITextResource* pRes = Pair.pTranslatedTextRes;
	pRes->SetChangeFlag(TRUE);
	pRes->SetText(m_TextNum, szStr);
	

	return TRUE;
}

BOOL MsgController::GenerateOutput()
{
	if (CreateTextMap() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	if (PackText() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}	

	AfxMessageBox(TEXT("Output Complete"));

	return TRUE;
}

BOOL MsgController::CreateTextMap()
{


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

	CFile WriteFile;
	if (!WriteFile.Open(TEXT("text2.map"), CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}

	CArchive ar(&WriteFile, CArchive::store);
	BYTE ResType = 3;
	USHORT Offset = 6;
	ar << ResType;
	ar << Offset;
	BYTE ResFin = 0xFF;;//리소스 엔드 마크
	ar << ResFin;

	Offset = m_mapTextResInfo.size() * 6;
	ar << Offset;

	mapTextResInfo::iterator iter = m_mapTextResInfo.begin();

	UINT newfileoffset = 0;
	mapTextResPair::iterator textRes;
	for(;iter != m_mapTextResInfo.end(); iter++)
	{
		USHORT number = iter->first;
		ar << number;
		

		/*textRes = m_mapTextResPair.find(number);

		ASSERT(textRes != m_mapTextResPair.end());

		SCITextResource* pResource = textRes->second.pOriginalTextRes;
		SCITextResource* pTranslateResource = textRes->second.pTranslatedTextRes;

		
		newfileoffset = GetTotalLength();*/

		ar << newfileoffset;
		newfileoffset += iter->second;
	}

	return TRUE;
}

BOOL MsgController::PackText()
{
	m_mapTextResInfo.clear();
	CFile File;

	if (!File.Open(TEXT("Text2.res"), CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::store);

	for (int i = 0; i < texFileMax; i++)
	{
		TCHAR szFileName[50];
		_stprintf(szFileName, TEXT("Translated/%s.%03d"), s_resourceTypeNames[3], i);

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

int MsgController::FindSentence( CString& SearchText )
{
	mapTextResPair::iterator iter = m_mapTextResPair.begin();
	BOOL bFind = FALSE;
	int messageindex = -1;

	for (; iter != m_mapTextResPair.end(); iter++)
	{
		TextResPair Pair = iter->second;
		messageindex = Pair.pOriginalTextRes->FindSentence(SearchText);
		if(messageindex >= 0)
		{
			m_iter = iter;
			m_TextNum = messageindex;
			GetOwner()->m_selectedRes = iter->first;
			bFind = TRUE;
			break;
		}
	}

	return messageindex;
}

BOOL MsgController::ImportExcel(CString& FilePath, CString& FileName)
{


	int TotalTextCount = 0;
	int TranslateTextCount = 0;

	try
	{
		CApplication app;     // app is an _Application object.
		CWorkbook book;       // More object declarations.
		CWorksheet sheet;
		CWorkbooks books;
		CWorksheets sheets;
		CRange range;

		//CRange range;          // Used for Microsoft Excel 97 components.
		LPDISPATCH lpDisp;    // Often reused variable.

		// Common OLE variants. Easy variants to use for calling arguments.
		COleVariant
			covTrue((short)TRUE),
			covFalse((short)FALSE),
			covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

		// Start Microsoft Excel, get _Application object,
		// and attach to app object.
		if (!app.CreateDispatch(TEXT("Excel.Application")))
		{
			AfxMessageBox(TEXT("Couldn't CreateDispatch() for Excel"));
			return FALSE;
		}

		// Register the Analysis ToolPak.
		CString sAppPath;

		sAppPath.Format(TEXT("%s\\Analysis\\Analys32.xll"), app.get_LibraryPath());

		if (!app.RegisterXLL(sAppPath))
			AfxMessageBox(TEXT("Didn't register the Analys32.xll"));

		// Get the Workbooks collection.
		lpDisp = app.get_Workbooks();     // Get an IDispatch pointer.
		ASSERT(lpDisp);
		books.AttachDispatch(lpDisp);    // Attach the IDispatch pointer
		// to the books object.

		// Open a new workbook and attach that IDispatch pointer to the
		// Workbook object.
		lpDisp = books.Add(covOptional);
		ASSERT(lpDisp);
		book.AttachDispatch(lpDisp);

		// To open an existing workbook, you need to provide all
		// arguments for the Open member function. In the case of 
		// Excel 2002 you must provide 16 arguments.
		// However in Excel 2003 you must provide 15 arguments.
		// The code below opens a workbook and adds it to the Workbook's
		// Collection object. It shows 13 arguments, required for Excel
		// 2000.
		// You need to modify the path and file name for your own
		// workbook.

		// 
		lpDisp = books.Open(FilePath,     // Test.xls is a workbook.
			covOptional, covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional);   // Return Workbook's IDispatch


		book.AttachDispatch(lpDisp);

		// Get the Sheets collection and attach the IDispatch pointer to your
		// sheets object.
		lpDisp = book.get_Sheets();
		ASSERT(lpDisp);
		sheets.AttachDispatch(lpDisp);

		// Get sheet #1 and attach the IDispatch pointer to your sheet
		// object.


		int sheetCount = sheets.get_Count();

		for (int i = 0; i < sheetCount; i++)
		{
			lpDisp = sheets.get_Item(COleVariant((short)(1 + i)));
			ASSERT(lpDisp);
			sheet.AttachDispatch(lpDisp);
			CString Name = sheet.get_Name();

			int MessageIndex = _tstoi(Name.GetString());

			mapTextResPair::iterator iter = m_mapTextResPair.find(MessageIndex);


			if (i == 103)
			{
				int  A = 1;
			}

			if (iter == m_mapTextResPair.end())
			{
				ASSERT(0);
				app.Quit();
				return FALSE;
			}

			SCITextResource* pResource = iter->second.pOriginalTextRes;
			SCITextResource* pTranslateResource = iter->second.pTranslatedTextRes;

			CString str;
			int msgCount = pResource->GetMessageCnt();

			if (msgCount == 0)
				continue;

			str.Format(TEXT("G%d"), msgCount);

			lpDisp = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(str));
			ASSERT(lpDisp);
			range.AttachDispatch(lpDisp);

			COleSafeArray saRet(range.get_Value(covOptional));

			long iRows;
			long iCols;
			saRet.GetUBound(1, &iRows);
			saRet.GetUBound(2, &iCols);

			int count = pResource->GetMessageCnt();
			for (int j = 0; j < count; j++)
			{
				CString szText;
				if (FALSE == pResource->ReadText(j, szText))
				{
					ASSERT(0);
					app.Quit();
					return FALSE;
				}

				long index[2];
				COleVariant vData;
				
				index[0] = j + 1;
				index[1] = 1;
				saRet.GetElement(index, vData);
				CString szFirstText(vData);

				if (szFirstText.IsEmpty() == TRUE)
				{
					CString szTextDataInfo;
					szTextDataInfo.Format(TEXT("ErrorPoint. SheetNum : %d, RowCount : %d"), i, j);

					::MessageBox(NULL, szTextDataInfo.GetBuffer(), TEXT("Error!!"), MB_SETFOREGROUND | MB_OK);
					//ASSERT(0);
					app.Quit();
					return FALSE;
				}

//2017
//King's Quest 5 PC98, Castle of Dr, Brain = 2
//Else 3
				index[0] = j + 1;
				index[1] = 3;
				saRet.GetElement(index, vData);
				CString szTranslatedText(vData);

				TotalTextCount++;
				if (szTranslatedText.IsEmpty() == TRUE)
				{
					pTranslateResource->SetText(j, szText);
				}
				else
				{
					TranslateTextCount++;
					pTranslateResource->SetText(j, szTranslatedText);
				}
			}

			range.ReleaseDispatch();
			sheet.ReleaseDispatch();
		}


		// app.ReleaseDispatch();
		// app.DetachDispatch();

		CString szTextDataInfo;
		szTextDataInfo.Format(TEXT("TotalMessage Count : %d, TranstlatedMessagCount : %d"), TotalTextCount, TranslateTextCount);

		::MessageBox(NULL, szTextDataInfo.GetBuffer(), TEXT("Success!!"), MB_SETFOREGROUND | MB_OK);

		app.Quit();

	}
	catch (...)
	{
		::MessageBoxA(NULL, "General Exception caught.", "Catch-All",
			MB_SETFOREGROUND | MB_OK);

	}



	return FALSE;
}


