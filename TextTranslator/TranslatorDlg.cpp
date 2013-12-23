
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
	DDX_Text(pDX, IDC_EDIT1, m_OriginalText);
	DDX_Text(pDX, IDC_EDIT2, m_TranslatedText);
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
	ON_EN_CHANGE(IDC_EDIT2, &CTranslatorDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTranslatorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTranslatorDlg::OnBnClickedButton4)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON7, &CTranslatorDlg::OnBnClickedGenOutput)
	ON_EN_CHANGE(IDC_EDIT3, &CTranslatorDlg::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON8, &CTranslatorDlg::OnBnClickedSearchText)
END_MESSAGE_MAP()

// CTranslatorDlg message handlers

BOOL CTranslatorDlg::OnInitDialog()
{
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

	m_MsgController->GetText(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText); 
	GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 

	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
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

void CTranslatorDlg::OnBnClickedButton1()
{
	CheckTextChange();

	m_MsgController->GetNextText(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText); 
	GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 
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

	GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText);
	GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer); 

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	
}

void CTranslatorDlg::OnBnClickedButton2()
{
	CheckTextChange();

	m_MsgController->GetNextTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText); 
	GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 
	char buffer[65];
	_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
	GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName()); 

}

void CTranslatorDlg::OnBnClickedButton6()
{
	CheckTextChange();

	m_MsgController->GetPrevTextRes(m_OriginalText, m_TranslatedText);
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText); 
	GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 
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

	//GetDlgItemText(IDC_EDIT2, m_TranslatedText);
}

void CTranslatorDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	UpdateData();
	m_MsgController->SaveText(m_TranslatedText);

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
		GetDlgItem(IDC_EDIT1)->SetWindowText(m_OriginalText); 
		GetDlgItem(IDC_EDIT2)->SetWindowText(m_TranslatedText); 
		char buffer[65];
		_itoa_s(m_MsgController->GetMessageIndex(), buffer, 10);
		GetDlgItem(IDC_STATIC_MESSAGEINDEX)->SetWindowText(buffer);  

		GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_MsgController->GetCurFileName());

	}
}
