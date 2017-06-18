#include "StdAfx.h"
#include "MsgController.h"
#include "Markup.h"
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
#include <locale.h>


static const char *const s_resourceTypeNames[] = {
	"view", "pic", "script", "text", "sound",
	"memory", "vocab", "font", "cursor",
	"patch", "bitmap", "palette", "cdaudio",
	"audio", "sync", "message", "map", "heap",
	"audio36", "sync36", "xlate", "robot", "vmd",
	"chunk", "animation", "etc", "duck", "clut",
	"tga", "zzz", "macibin", "macibis", "macpict"
};

MsgController::MsgController(CGK1RemakeDlg* pOwner)
: m_pOwner(pOwner)
{
	m_count = 0;
}

MsgController::~MsgController(void)
{
	
}

BOOL MsgController::ImportExcel(CString& FilePath, CString& FileName)
{
	////////////////////////////////////////////////////////////
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
		if (!app.CreateDispatch("Excel.Application"))
		{
			AfxMessageBox("Couldn't CreateDispatch() for Excel");
			return FALSE;
		}

		// Register the Analysis ToolPak.
		CString sAppPath;

		sAppPath.Format("%s\\Analysis\\Analys32.xll", app.get_LibraryPath());

		if (!app.RegisterXLL(sAppPath))
			AfxMessageBox("Didn't register the Analys32.xll");

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


////////////////////////////////////////////////////////////
//Excel Load Complete

		xml.Load("3767_Dictionary.xml");

		int sheetCount = sheets.get_Count();	
		for (int i = 2; i < sheetCount; i++)
		{
			lpDisp = sheets.get_Item(COleVariant((short)(1 + i)));
			ASSERT(lpDisp);
			sheet.AttachDispatch(lpDisp);
			CString Name = sheet.get_Name();

			CString str;
			str.Format("G%d", 400);
			lpDisp = sheet.get_Range(COleVariant("A1"), COleVariant(str));
			ASSERT(lpDisp);
			range.AttachDispatch(lpDisp);



			COleSafeArray saRet(range.get_Value(covOptional));

			long iRows;
			long iCols;
			saRet.GetUBound(1, &iRows);
			saRet.GetUBound(2, &iCols);


			for (int j = 0; j < iRows; j++)
			{
				long index[2];
				index[0] = j + 1;
				index[1] = 6;
				COleVariant vData;
				saRet.GetElement(index, vData);
				CString szdata(vData);

				index[0] = j + 1;
				index[1] = 6;
				saRet.GetElement(index, vData);
				CString szOriginalText(vData);

				if (szOriginalText.IsEmpty() == TRUE)
				{
					continue;
				}


				index[0] = j + 1;
				index[1] = 7;
				saRet.GetElement(index, vData);
				CString szTranslatedText(vData);

				SetTranslatedText(szOriginalText, szTranslatedText);

				TotalTextCount++;
			}
	
			range.ReleaseDispatch();
			sheet.ReleaseDispatch();
		}



	xml.Save("3767_Dictionary_new.xml");
	
	  // Get sheet #1 and attach the IDispatch pointer to your sheet
      // object.

	
	  


	 // app.ReleaseDispatch();
	 // app.DetachDispatch();
	  
	  CString szTextDataInfo;
	  szTextDataInfo.Format("TotalMessage Count : %d, TranstlatedMessagCount : %d", TotalTextCount, m_count);

	  ::MessageBox(NULL, szTextDataInfo.GetBuffer(), "Success!!", MB_SETFOREGROUND | MB_OK);

	  app.Quit();

	}
	catch(...)
      {
        ::MessageBox(NULL, "General Exception caught.", "Catch-All",
                           MB_SETFOREGROUND | MB_OK);

      }



	return FALSE;
}



bool MsgController::SetTranslatedText(CString& szOriginalText, CString& szTranslatedText)
{
	if (szOriginalText.IsEmpty() || szTranslatedText.IsEmpty())
		return false;	

	char* subText2 = strstr(szOriginalText.GetBuffer(), ")");

	if (subText2 == nullptr)
		return false;

	xml.FindChildElem("Dictionary");		
	while (xml.FindChildElem("Entry"))
	{		
		xml.IntoElem();
		xml.FindChildElem("ConversationName");
		xml.FindChildElem("Key");
		xml.FindChildElem("Value");
		
		std::string gameText = xml.GetChildData();

		if (gameText.empty())
		{
			xml.OutOfElem();
			continue;
		}						

		if (gameText.compare((subText2+1)) != 0)
		{
			xml.OutOfElem();
			continue;
		}					
		
		xml.SetChildData(szTranslatedText.GetBuffer());
		m_count++;		


		/*if (xml.GetChildData().compare(szOriginalText.GetBuffer()) == 0)
		{
			xml.SetChildData(szTranslatedText.GetBuffer());
			m_count++;
		}*/		

		xml.OutOfElem();
	}
	
	xml.ResetPos();

	return true;
}