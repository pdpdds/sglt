#include "StdAfx.h"
#include "MsgController.h"
#include "SCITextResource.h"
#include "ResourceGabriel.h"
#include "ResourceEcho1.h"
#include "ResourceAmonRa.h"
//#include "XLEzAutomation.h"
//#include <afxdao.h>                     // MFC DAO database classes
 //#include <excel8.h>
#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorksheet.h"
#include "CWorkbooks.h"
#include "CWorksheets.h"
#include "CRange.h"
#include "CRanges.h"


static const char *const s_resourceTypeNames[] = {
	"view", "pic", "script", "text", "sound",
	"memory", "vocab", "font", "cursor",
	"patch", "bitmap", "palette", "cdaudio",
	"audio", "sync", "message", "map", "heap",
	"audio36", "sync36", "xlate", "robot", "vmd",
	"chunk", "animation", "etc", "duck", "clut",
	"tga", "zzz", "macibin", "macibis", "macpict"
};

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

	for(iter; iter != m_mapTextResPair.end();)
	{

		TextResPair ResPair = iter->second;
		delete ResPair.pOriginalTextRes;
		delete ResPair.pTranslatedTextRes;

		iter = m_mapTextResPair.erase(iter);
	}

	return TRUE;
}

BOOL MsgController::Load(CString GameName)
{
	m_GameName = GameName;
	int TextResourceCnt = 0;

	CFileFind finder;
	CString strWildCard( "./Original/" );
	strWildCard += "*.*";

	BOOL bWorking = finder.FindFile( strWildCard );
	while( bWorking )
	{
		bWorking = finder.FindNextFile();
		if( finder.IsDots() || finder.IsDirectory() )
			continue;  
		
		CString szFileName = finder.GetFileName();
	
	
		//char szFileName[50];
		//sprintf(szFileName, "%s.%03d", s_resourceTypeNames[15], i);

		CString strExtension;
			strExtension = szFileName.Right(szFileName.GetLength() - szFileName.ReverseFind('.') - 1);

		int i = atoi(strExtension);
		
		
		TextResPair sTextResPair;
		SCITextResource* pOriTextRes = CreateTextResource(i);

		CString szOriginalPath = "./Original/";
		szOriginalPath += szFileName;
		if(FALSE == pOriTextRes->Load(szOriginalPath))
		{
			delete pOriTextRes;
			continue;
		}

		SCITextResource* pTranslateTextRes = CreateTextResource(i);
		CString szTranslatePath = "./Translated/";
		szTranslatePath += szFileName;
		if(FALSE == pTranslateTextRes->Load(szTranslatePath))
			ASSERT(0);

		sTextResPair.pOriginalTextRes = pOriTextRes;
		sTextResPair.pTranslatedTextRes = pTranslateTextRes;

		m_mapTextResPair.insert(std::make_pair(i, sTextResPair));

		TextResourceCnt++;
	}

	finder.Close();

	m_TextResourceCnt = TextResourceCnt;
	m_iter = m_mapTextResPair.begin();
	return TRUE;
}

SCITextResource* MsgController::CreateTextResource(int index)
{
	if(m_GameName == "gabriel")
		return new ResourceGabriel(index);
	if(m_GameName == "eco1")
		return new ResourceEcho1(index);
	if(m_GameName == "amonra")
		return new ResourceAmonRa(index);

	ASSERT(0);
	return new ResourceGabriel(index);
}

BOOL MsgController::Save()
{
	mapTextResPair::iterator iter = m_mapTextResPair.begin();

	for(iter; iter != m_mapTextResPair.end(); iter++)
	{
		SCITextResource* pRes = iter->second.pTranslatedTextRes;
		pRes->Save();
	}

	AfxMessageBox("Save Complete");
	
	return TRUE;
}

BOOL MsgController::GetText(CString& str, CString& TranStr)
{
	TextResPair Pair = m_iter->second;
	return Pair.pOriginalTextRes->ReadText(m_TextNum, str) && Pair.pTranslatedTextRes->ReadText(m_TextNum, TranStr);
}

BOOL MsgController::GetNextText(CString& str, CString& TranStr)
{
	m_TextNum++;

	if(m_TextNum >= m_iter->second.pOriginalTextRes->GetMessageCnt())
	{
		m_iter++;
		m_TextNum--;

		if(m_iter == m_mapTextResPair.end())
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
		if(m_iter == m_mapTextResPair.begin())
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

	if(m_iter == m_mapTextResPair.end())
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
	
	if(m_iter == m_mapTextResPair.begin())
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
	if (PackMessage() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	if (CreateMessageMap() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	AfxMessageBox("Output Complete");

	return TRUE;
}

BOOL MsgController::CreateMessageMap()
{


	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	CFile WriteFile;
	if(!WriteFile.Open("Message.map", CFile::modeWrite))
	{
		return FALSE;
	}

	CArchive ar(&WriteFile, CArchive::store);
	BYTE ResType = 15; //메세지
	USHORT Offset = 6;
	ar << ResType;
	ar << Offset;
	BYTE ResFin = 0xFF;;//리소스 엔드 마크
	ar << ResFin;

	Offset = m_vecTextResInfo.size() * 6 + 6;
	ar << Offset;

	vecTextResInfo::iterator iter = m_vecTextResInfo.begin();

	UINT newfileoffset = 0;
	for(;iter != m_vecTextResInfo.end(); iter++)
	{
		TextResInfo info = *iter;
		ar << info.Num;
		ar << newfileoffset;
		newfileoffset += info.FileSize;
	}

	return TRUE;
}

/*BOOL MsgController::CreateMessageMap()
{
	CFile File;
	if(!File.Open("message.map", CFile::modeRead))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::load);

	int Size = File.GetLength();

	ASSERT(Size > 0);

	BYTE* Buffer = new BYTE[Size];
	memset(Buffer, 0, sizeof(BYTE) * Size);

	ar.Read(Buffer, Size);

	resource_index_t resMap[32];
	memset(resMap, 0, sizeof(resource_index_t) * 32);
	byte type = 0, prevtype = 0;
	byte nEntrySize = 6;

	// Read resource type and offsets to resource offsets block from .MAP file
	// The last entry has type=0xFF (0x1F) and offset equals to map file length

	int i = 0;
	do {
		type = Buffer[i*3];
		type = type & 0x1F;
		resMap[type].wOffset = *((USHORT*)(Buffer + 1 + i * 3));
		resMap[prevtype].wSize = (resMap[type].wOffset
			- resMap[prevtype].wOffset) / nEntrySize;
		prevtype = type;
		i++;
	} while (type != 0x1F); // the last entry is FF

	BYTE* DataPtr = Buffer;
	int newfileoffset =  0;
	// reading each type's offsets
	UINT fileOffset = 0;
	for (type = 0; type < 32; type++) {
		if (resMap[type].wOffset == 0) // this resource does not exist in map
			continue;
		DataPtr += resMap[type].wOffset;
		for (int i = 0; i < resMap[type].wSize; i++) {
			USHORT number = *((USHORT*)DataPtr);
			DataPtr += 2;

			mapTextResInfo::iterator iter = m_mapTextResInfo.find(number);

			if(iter == m_mapTextResInfo.end())
			{
				ASSERT(0);
			}

			int volume_nr = 0;

			// offset/volume stored in 4 bytes
			fileOffset = *((UINT*)DataPtr);
				
			volume_nr = fileOffset >> 28; // most significant 4 bits
			fileOffset &= 0x0FFFFFFF;     // least significant 28 bits

			
			*((UINT*)DataPtr) = newfileoffset;
			newfileoffset +=  iter->second;
			DataPtr += 4;
			
		}
	}

	File.Close();
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

	CFile WriteFile;
	if(!WriteFile.Open("message.map", CFile::modeWrite))
	{
		return FALSE;
	}

	CArchive ar2(&WriteFile, CArchive::store);


	ar2.Write(Buffer, Size);

	return TRUE;
}*/

BOOL MsgController::PackMessage()
{
	m_vecTextResInfo.clear();
	CFile File;

	if(!File.Open("Resource.msg", CFile::modeWrite))
	{
		return FALSE;
	}

	CArchive ar(&File, CArchive::store);

	CFileFind finder;
	CString strWildCard( "./Translated/" );
	strWildCard += "*.*";

	BOOL bWorking = finder.FindFile( strWildCard );
	while( bWorking )
	{
		bWorking = finder.FindNextFile();
		if( finder.IsDots() || finder.IsDirectory() )
			continue;  

		CString szFileName = finder.GetFileName();

		CString strExtension;
		strExtension = szFileName.Right(szFileName.GetLength() - szFileName.ReverseFind('.') - 1);

		int i = atoi(strExtension);

		CFile MsgFile; 

		if(!MsgFile.Open("./Translated/" + szFileName, CFile::modeRead))
		{                          
			continue;
		}

		int Size = MsgFile.GetLength();

		ASSERT(Size > 0);

		TextResInfo info;
		info.Num = i;
		info.FileSize = Size;
		m_vecTextResInfo.push_back(info);

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
	mapTextResPair::iterator iter = m_mapTextResPair.begin();
	BOOL bFind = FALSE;
	int messageindex = -1;

	for(;iter!= m_mapTextResPair.end(); iter++)
	{
		TextResPair Pair = iter->second;
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

BYTE* MsgController::GetTalker()
{
	TextResPair Pair = m_iter->second;
	SCITextResource* pTextResource = Pair.pOriginalTextRes;
	return pTextResource->GetTalker(m_TextNum);
}

BOOL MsgController::ImportExcel(CString& FilePath, CString& FileName)
{
	//fileDlg
	//CXLEzAutomation XL(FALSE); 
	//if(FALSE == XL.OpenExcelFile(FilePath))
		//return FALSE;

	//int temp = _ttoi(XL.GetCellValue(1, 1)); 
	//XL.ReleaseExcel(); 

	/*CDaoDatabase db; 
    db.Open(FilePath,false,false,"Excel 8.0");
	if(db.IsOpen() == FALSE)
		return FALSE;

    CDaoRecordset rs(&db);
    rs.Open(dbOpenTable,"10$",0);

	if(rs.IsOpen() == FALSE)
	{
		db.Close();
		return FALSE;
	}
        rs.MoveFirst();

		COleVariant kkk;
        kkk=rs.GetFieldValue("field1");
      //  uuu = kkk.Detach();
      //  m_text = CCrack::strVARIANT(uuu); 
        rs.Close();
        db.Close();*/

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
      if(!app.CreateDispatch("Excel.Application"))
       {
        AfxMessageBox("Couldn't CreateDispatch() for Excel");
        return FALSE;
       }

      // Register the Analysis ToolPak.
      CString sAppPath;

	  sAppPath.Format ("%s\\Analysis\\Analys32.xll", app.get_LibraryPath());

      if(!app.RegisterXLL(sAppPath))
        AfxMessageBox("Didn't register the Analys32.xll");

      // Get the Workbooks collection.
	  lpDisp = app.get_Workbooks();     // Get an IDispatch pointer.
      ASSERT(lpDisp);
      books.AttachDispatch(lpDisp);    // Attach the IDispatch pointer
                                       // to the books object.

      // Open a new workbook and attach that IDispatch pointer to the
      // Workbook object.
      lpDisp = books.Add( covOptional );
      ASSERT(lpDisp);
      book.AttachDispatch( lpDisp );

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
       covOptional, covOptional, covOptional, covOptional );   // Return Workbook's IDispatch
       

	   book.AttachDispatch(lpDisp);

      // Get the Sheets collection and attach the IDispatch pointer to your
      // sheets object.
	  lpDisp = book.get_Sheets();
      ASSERT(lpDisp);
      sheets.AttachDispatch(lpDisp);

	  // Get sheet #1 and attach the IDispatch pointer to your sheet
      // object.

	
	  int sheetCount = sheets.get_Count();

	  for(int i = 2; i < sheetCount; i++)
	  {
		  lpDisp = sheets.get_Item( COleVariant((short)(1+i)) );
		  ASSERT(lpDisp);
		  sheet.AttachDispatch(lpDisp);
		  CString Name = sheet.get_Name();

		  int MessageIndex = atoi(Name.GetString());

		  mapTextResPair::iterator iter = m_mapTextResPair.find(MessageIndex);

			
		  if(iter == m_mapTextResPair.end())
		  {
			  ASSERT(0);
			  app.Quit();
			  return FALSE;
		  }

		  SCITextResource* pResource = iter->second.pOriginalTextRes;
		  SCITextResource* pTranslateResource = iter->second.pTranslatedTextRes;

		  CString str;
		  str.Format("G%d", pResource->GetMessageCnt());   
		  lpDisp = sheet.get_Range(COleVariant("A1"), COleVariant(str));
		  ASSERT(lpDisp);
		  range.AttachDispatch(lpDisp);

		  COleSafeArray saRet(range.get_Value(covOptional));
	
		  long iRows;    
		  long iCols;
		  saRet.GetUBound(1, &iRows);
		  saRet.GetUBound(2, &iCols);

		
		  for(int j = 0; j < pResource->GetMessageCnt(); j++)
		  {
			  CString szText;
			  if(FALSE == pResource->ReadText(j, szText))
			  {
				  ASSERT(0);
				  app.Quit();
				  return FALSE;
			  }

			  long index[2];
			  index[0]=j+1;
			  index[1]=6;		   
			  COleVariant vData;
			  saRet.GetElement(index,vData);
			  CString szdata(vData);

			  /*if(szText.Compare(szdata) != 0)	  
			  {
				  ASSERT(0);
				  return FALSE;
			  }*/
			  index[0]=j+1;
			  index[1]=1;		   
			  saRet.GetElement(index,vData);
			  CString szFirstText(vData);

			  if(szFirstText.IsEmpty() == TRUE)	  
			  {
				  ASSERT(0);
				  app.Quit();
				  return FALSE;
			  }

			  index[0]=j+1;
			  index[1]=6;		   
			  saRet.GetElement(index,vData);
			  CString szOriginalText(vData);

			  if(szOriginalText.IsEmpty() == TRUE)	  
			  {
				  continue;
			  }


			  index[0]=j+1;
			  index[1]=7;		   
			  saRet.GetElement(index,vData);
			  CString szTranslatedText(vData);

			  TotalTextCount++;
			   if(szTranslatedText.IsEmpty() == TRUE)	  
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
	  szTextDataInfo.Format("TotalMessage Count : %d, TranstlatedMessagCount : %d", TotalTextCount, TranslateTextCount);

	  ::MessageBox(NULL, szTextDataInfo.GetBuffer(), "AmonRa", MB_SETFOREGROUND | MB_OK);

	  app.Quit();

	}
	catch(...)
      {
        ::MessageBox(NULL, "General Exception caught.", "Catch-All",
                           MB_SETFOREGROUND | MB_OK);

      }



	return FALSE;
}


