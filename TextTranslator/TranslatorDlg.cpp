
// TranslatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Translator.h"
#include "TranslatorDlg.h"
#include "MsgController.h"
#include "ggtrans-api-client.h"


#define ERR_CANTGGTRANSLATE "Cannot translate a cloned sentence. Please translate the original sentence or unclone it first."
#define ERR_TITLE "Error"  
#define WARNING_GGTRANSEXISTS TEXT("This sentence already has a translation defined, do you want to overwrite it?")
#define WARN_ATTENTION "Attention!"  

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
	//DDX_GridControl(pDX, IDC_GRID, m_Grid); //<-----
	DDX_Control(pDX, IDC_TREE2, m_list);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
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
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, &CTranslatorDlg::OnTvnSelchangedTree2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CTranslatorDlg::OnLvnItemchangedList1)
	ON_COMMAND(ID_FILE_OPEN32771, &CTranslatorDlg::OnFileOpen)
END_MESSAGE_MAP()

// CTranslatorDlg message handlers

#include <locale.h>
BOOL CTranslatorDlg::OnInitDialog()
{
	const int iFontSize = 12;
	const CString sFont = _TEXT("Meiryo");

	

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
	

#if defined(PC98)
	m_hJapaneseFont = CreateFont(iFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, sFont);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(m_hJapaneseFont), TRUE);
	m_listCtrl.SendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(m_hJapaneseFont), TRUE);
	
	//GetDlgItem(IDC_TRANSLATED_TEXT)->SendMessage(WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
#endif

	//InitGridControl();

	//List Control
	CRect rt;
	m_listCtrl.GetWindowRect(&rt);
	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listCtrl.InsertColumn(0, TEXT("Id"), LVCFMT_CENTER, rt.Width() * 0.04);
	m_listCtrl.InsertColumn(1, TEXT("Original"), LVCFMT_LEFT, rt.Width() * 0.48);
	m_listCtrl.InsertColumn(2, TEXT("Translated"), LVCFMT_LEFT, rt.Width() * 0.48);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*void CTranslatorDlg::InitGridControl() {

	BOOL m_bEditable = TRUE;
	BOOL m_bListMode = TRUE;
	int m_nRows = 20;
	int m_nCols = 8;
	int m_nFixRows = 1;
	int m_nFixCols = 1;

	m_Grid.SetEditable(m_bEditable);
	m_Grid.SetListMode(m_bListMode);
	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));

	m_Grid.SetRowCount(m_nRows);
	m_Grid.SetColumnCount(m_nCols);
	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);

	DWORD dwTextStyle = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;

	// fill rows/cols with text
	for (int row = 0; row < m_Grid.GetRowCount(); row++) {
		for (int col = 0; col < m_Grid.GetColumnCount(); col++) {
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;

			if (row < m_nFixRows) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;
				Item.strText.Format(_T("Column %d"), col);

			}
			else if (col < m_nFixCols) {
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("Row %d"), row);

			}
			else {
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("%d"), row * col);
			}
			m_Grid.SetItem(&Item);

			if (rand() % 10 == 1) {
				COLORREF clr = RGB(rand() % 128 + 128,
					rand() % 128 + 128,
					rand() % 128 + 128);
				m_Grid.SetItemBkColour(row, col, clr);
				m_Grid.SetItemFgColour(row, col, RGB(255, 0, 0));
			}
		}
	}
	// Make cell 1,1 read-only
	m_Grid.SetItemState(1, 1, m_Grid.GetItemState(1, 1) | GVIS_READONLY);

	m_Grid.AutoSize();
	m_Grid.SetRowHeight(0, 3 * m_Grid.GetRowHeight(0) / 2);
}*/

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
	TCHAR buffer[65];
	_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
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
	TCHAR buffer[65];
	_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer); 

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	
}

void CTranslatorDlg::OnBnClickedButton2()
{
	CheckTextChange();

	m_MsgController->GetNextTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	TCHAR buffer[65];
	_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName()); 

}

void CTranslatorDlg::OnBnClickedButton6()
{
	CheckTextChange();

	m_MsgController->GetPrevTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	TCHAR buffer[65];
	_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
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

	/*CString str, TransText;
	m_MsgController->GetText(str, TransText);

	if(m_TranslatedText != TransText)
	{
		m_MsgController->SaveText(m_TranslatedText);
	}*/


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

HTREEITEM CTranslatorDlg::GetTreeItem(CTreeCtrl* pTree, USHORT resIndex, HTREEITEM hItem, BOOL Recurse)
{	
	CString strIndex;
	strIndex.Format(TEXT("%d"), resIndex);

	//if (hItem == NULL)
		//hItem = pTree->GetSelectedItem();
	if (pTree->ItemHasChildren(hItem))
	{
		hItem = pTree->GetNextItem(hItem, TVGN_CHILD);
		while (hItem)
		{			
			hItem = pTree->GetNextItem(hItem, TVGN_NEXT);

			CString str = pTree->GetItemText(hItem);

			if (str.Compare(strIndex) == 0)
				return hItem;

		}
	}
	return NULL;
}

void CTranslatorDlg::OnBnClickedSearchText()
{
	// TODO: Add your control notification handler code here
	CString SearchText;
	GetDlgItemText(IDC_EDIT3, SearchText);

	if(SearchText == "")
		return;

	CheckTextChange();
	int messageindex = m_MsgController->FindSentence(SearchText);
	if(messageindex >= 0)
	{
		HTREEITEM item = GetTreeItem(&m_list, m_selectedRes);

		if (item == NULL)
			return;

		m_list.SelectItem(item);
		//m_list.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
		//m_list.SetFocus();

		m_MsgController->Load2ListControl(m_selectedRes);
		m_listCtrl.SetItemState(messageindex, LVIS_SELECTED, LVIS_SELECTED);
		m_listCtrl.SetFocus();

		m_MsgController->GetText(m_selectedRes, messageindex, m_OriginalText, m_TranslatedText);
		GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
		GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
		/*TCHAR buffer[65];
		_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
		GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

		GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());*/

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
	TCHAR textdata[10240];
	HWND hwnd = GetDlgItem(IDC_ORIGINAL_TEXT)->GetSafeHwnd();
	::GetWindowText(hwnd, textdata, 10240);

	if (textdata[0] == 0)
		return;
	/*if (textdata[0] != 0)
	{
		//replacement confirmation messagebox
		int btn = AfxMessageBox(WARNING_GGTRANSEXISTS,
			MB_APPLMODAL | MB_ICONQUESTION | MB_OKCANCEL);
		if (btn == IDCANCEL)
		{
			return;
		}
	}*/


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
	::SetWindowTextA(translatedEdit, trans_text);

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


void CTranslatorDlg::OnTvnSelchangedTree2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TVITEM tvi = pNMTreeView->itemNew;
	
	CString str = m_list.GetItemText(tvi.hItem);

	int resIndex = _ttoi(str);

	m_selectedRes = resIndex;
	m_MsgController->Load2ListControl(resIndex);
	*pResult = 0;
}


void CTranslatorDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int selectedIndex = pNMLV->iItem;
	
	m_MsgController->GetText(m_selectedRes, selectedIndex, m_OriginalText, m_TranslatedText);
	
	GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);
	*pResult = 0;
}


void CTranslatorDlg::OnFileOpen()
{
	TCHAR szFilters[] = _T("map Files (*.map)||");
	CFileDialog dlg(TRUE, _T(""), _T("*.map"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (IDOK == dlg.DoModal())
	{

		CString strPathName = dlg.GetPathName();
		CString strFileName = dlg.GetFileName();
		CString strFolderPath = dlg.GetFolderPath();
		CString strRes = strFolderPath + TEXT("\\text.res");

		if (m_MsgController->Load(strPathName, strRes)) {

			m_MsgController->GetText(0, 0, m_OriginalText, m_TranslatedText);
			GetDlgItem(IDC_ORIGINAL_TEXT)->SetWindowText(m_OriginalText);
			GetDlgItem(IDC_TRANSLATED_TEXT)->SetWindowText(m_TranslatedText);

			TCHAR buffer[65];
			_itot_s(m_MsgController->GetMessageIndex(), buffer, 10);
			//GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer); 

			//GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName()); 

			m_selectedRes = 0;
			m_MsgController->Load2ListControl(m_selectedRes);
		}		
	}

	


}
