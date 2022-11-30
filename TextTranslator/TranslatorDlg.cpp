
// TranslatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Translator.h"
#include "TranslatorDlg.h"
#include "MsgController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTranslatorDlg dialog




CTranslatorDlg::CTranslatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTranslatorDlg::IDD, pParent)
	, m_OriginalText(_T(""))
	, m_TranslatedText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTranslatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ORIGINAL_TEXT, m_OriginalText);
	DDX_Text(pDX, IDC_TRANSLATED_TEXT, m_TranslatedText);
}

BEGIN_MESSAGE_MAP(CTranslatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTranslatorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CTranslatorDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON2, &CTranslatorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON6, &CTranslatorDlg::OnBnClickedButton6)
	ON_EN_CHANGE(IDC_TRANSLATED_TEXT, &CTranslatorDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTranslatorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTranslatorDlg::OnBnClickedButton4)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON7, &CTranslatorDlg::OnBnClickedGenOutput)
	ON_EN_CHANGE(IDC_EDIT3, &CTranslatorDlg::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON8, &CTranslatorDlg::OnBnClickedSearchText)
	ON_BN_CLICKED(IDC_BUTTON9, &CTranslatorDlg::OnImport)
	ON_BN_CLICKED(IDC_SINGLE_TRANSLATE, &CTranslatorDlg::OnClickedSingleTranslate)
END_MESSAGE_MAP()

// CTranslatorDlg message handlers

#include <locale.h>
BOOL CTranslatorDlg::OnInitDialog()
{
	const int iFontSize = 25;
	const CString sFont = "Meiryo";

	

	CDialog::OnInitDialog();

	

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	m_MsgController = new MsgController(this);
	m_MsgController->Load();

#if defined(PC98)
	HFONT hFont = CreateFont(iFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, sFont);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
	//GetDlgItem(IDC_TRANSLATED_TEXT)->SendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
#endif

	m_MsgController->GetText(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);

	TCHAR buffer[65];
	_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer); 

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName()); 


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTranslatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTranslatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTranslatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CTranslatorDlg::~CTranslatorDlg()
{
	delete m_MsgController;
}
/*#include <string>
using namespace std;
string sjis2utf8(const string& sjis) {
	string utf8_string;

	//一旦SJISからutf-16へ変換
	LPCCH pSJIS = (LPCCH)sjis.c_str();
	int utf16size = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, pSJIS, -1, 0, 0);
	if (utf16size != 0) {
		LPWSTR pUTF16 = new WCHAR[utf16size];
		if (::MultiByteToWideChar(CP_ACP, 0, (LPCCH)pSJIS, -1, pUTF16, utf16size) != 0) {
			//次にutf-16からutf-8へ変換
			int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, pUTF16, -1, 0, 0, 0, 0);
			if (utf8size != 0) {
				LPTSTR pUTF8 = new TCHAR[utf8size + 16];
				ZeroMemory(pUTF8, utf8size + 16);
				if (::WideCharToMultiByte(CP_UTF8, 0, pUTF16, -1, pUTF8, utf8size, 0, 0) != 0) {
					utf8_string = string(pUTF8);
				}
				delete pUTF8;
			}
		}
		delete pUTF16;
	}
	return utf8_string;
}*/


void CTranslatorDlg::OnBnClickedButton1()
{
	CheckTextChange();

	m_MsgController->GetNextText(m_OriginalText, m_TranslatedText);

	//string str = m_OriginalText;
	//sjis2utf8(str);
	//m_OriginalText = str.c_str();
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

}

void CTranslatorDlg::OnBnClickedButton5()
{
	CheckTextChange();

	if(FALSE == m_MsgController->GetPrevText(m_OriginalText, m_TranslatedText))
		return;

	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer); 

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	
}

void CTranslatorDlg::OnBnClickedButton2()
{
	CheckTextChange();

	m_MsgController->GetNextTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName()); 

}

void CTranslatorDlg::OnBnClickedButton6()
{
	CheckTextChange();

	m_MsgController->GetPrevTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	
}

void CTranslatorDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//CString str, TranStr;
	//m_MsgController->GetText(str, TranStr);

	//GetDlgItemText(IDC_TRANSLATED_TEXT, m_TranslatedText);
}

void CTranslatorDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	UpdateData();
	//m_MsgController->SaveText(m_TranslatedText);

	m_MsgController->Save();
}

void CTranslatorDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CTranslatorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTranslatorDlg::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CTranslatorDlg::PreTranslateMessage( MSG* pMsg )
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTranslatorDlg::CheckTextChange()
{
	UpdateData();

	CString str, TransText;
	m_MsgController->GetText(str, TransText);

	if(m_TranslatedText != TransText)
	{
		m_MsgController->SaveText(m_TranslatedText);
	}


	return TRUE;
}
void CTranslatorDlg::OnBnClickedGenOutput()
{
	// TODO: Add your control notification handler code here
	m_MsgController->GenerateOutput();
}

void CTranslatorDlg::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

}

void CTranslatorDlg::OnBnClickedSearchText()
{
	// TODO: Add your control notification handler code here
	CString SearchText;
	GetDlgItemText(IDC_EDIT3, SearchText);

	if(SearchText == "")
		return;

	CheckTextChange();

	if(m_MsgController->FindText(SearchText))
	{
		m_MsgController->GetText(m_OriginalText, m_TranslatedText);
		GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
		GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
		char buffer[65];
		_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
		GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

		GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	}
}


void CTranslatorDlg::OnImport()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("xls Files (*.xls)|*.All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T(""), _T("*.xls"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	CString FilePath;
	CString FileName;

	if (fileDlg.DoModal() == IDOK)
	{

		FileName = fileDlg.GetFileName();
		FilePath = fileDlg.GetPathName();

		m_MsgController->ImportExcel(FilePath, FileName);
	}
}

#include "ggtrans-api-client.h"

#define ERR_CANTGGTRANSLATE "Cannot translate a cloned sentence. Please translate the original sentence or unclone it first."
#define ERR_TITLE "Error"  
#define WARNING_GGTRANSEXISTS "This sentence already has a translation defined, do you want to overwrite it?"
#define WARN_ATTENTION "Attention!"  

void CTranslatorDlg::SingleTranslate()
{

	/*long i = msgObj->currentString();

	//check if string is not a clone string...
	if (msgObj->isClone(i))
	{
		//message error
		AfxMessageBox(ERR_CANTGGTRANSLATE, MB_ICONINFORMATION | MB_OK); //

		return;
	}*/

	//check if string is not being translated
	//if (msgObj->getTranslation(i) != 0) 
	char textdata[10240];
	HWND hwnd = GetDlgItem(IDC_ORIGINAL_TEXT)->GetSafeHwnd();
	::GetWindowText(hwnd, textdata, 10240);
	if (textdata[0] != 0)
	{
		//replacement confirmation messagebox
		int btn = AfxMessageBox(WARNING_GGTRANSEXISTS,
			MB_APPLMODAL | MB_ICONQUESTION | MB_OKCANCEL);
		if (btn == IDCANCEL)
		{
			return;
		}
	}


	ggtrans_init_client();

//#if defined(PC98)
	//char* trans_text = ggtrans_translate_multiline_text((char*)textdata, "ja", "en");  //"en", "it");
//#else
	char* trans_text = ggtrans_translate_multiline_text((char*)textdata, "en", "ko");  //"en", "it");
//#endif

	int nBufferSize = strlen(trans_text) * 2 + 10;
	WCHAR* tBuffer = (WCHAR*)malloc(nBufferSize);


	MultiByteToWideChar(
		CP_UTF8,
		0,
		trans_text,
		-1,
		tBuffer,
		nBufferSize
	);

	WideCharToMultiByte(
		CP_ACP,
		0,
		tBuffer,
		-1,
		trans_text,
		strlen(trans_text) + 1,
		"?UNK?",
		0
	);

	free(tBuffer);


	//as this is the selected item, let's first update the Edit field
	//msgObj->setTranslation(i, trans_text);

	HWND translatedEdit = GetDlgItem(IDC_TRANSLATED_TEXT)->GetSafeHwnd();
	::SetWindowText(translatedEdit, trans_text);

	//now let's replace the list item
	/*LV_ITEM lvi;
	lvi.pszText = trans_text;
	lvi.cchTextMax = 10240;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = i;
	lvi.iSubItem = 7;
	ListView_SetItem(hwndListV, &lvi);*/

	//msgObj->setTranslation(i, trans_text);

	//datasaved = false;
	//EnableMenuItem(menu, IDM_FILESAVE, MF_ENABLED);

	free(trans_text);

	ggtrans_cleanup_client();

}


void CTranslatorDlg::OnClickedSingleTranslate()
{
	SingleTranslate();
}
