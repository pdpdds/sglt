
// TranslatorDlg.h : header file
//

#pragma once

class MsgController;

// CTranslatorDlg dialog
class CTranslatorDlg : public CDialog
{
// Construction
public:
	CTranslatorDlg(CWnd* pParent = NULL);	// standard constructor
	~CTranslatorDlg();

// Dialog Data
	enum { IDD = IDD_TRANSLATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	MsgController* m_MsgController;
	BOOL CheckTextChange();
	BOOL UpdateTextState();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_OriginalText;
	CString m_TranslatedText;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton6();
	
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedGenOutput();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedSearchText();
	afx_msg void OnBnClickedImport();
};
