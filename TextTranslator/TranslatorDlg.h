
// TranslatorDlg.h : header file
//

#pragma once
//#include "GridCtrl.h"

class MsgController;

// CTranslatorDlg dialog
class CTranslatorDlg : public CDialog
{
// Construction
public:
	CTranslatorDlg(CWnd* pParent = NULL);	// standard constructor
	~CTranslatorDlg();

	//void InitGridControl();

// Dialog Data
	enum { IDD = IDD_TRANSLATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	MsgController* m_MsgController;
	BOOL CheckTextChange();
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

	void SingleTranslate();

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
	afx_msg void OnImport();
	afx_msg void OnClickedSingleTranslate();
	

	CTreeCtrl m_list;
	int m_selectedRes;
	//CGridCtrl m_Grid;


	afx_msg void OnTvnSelchangedTree2(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_listCtrl;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
};
