#pragma once
#include <list>
#include <map>
#include <vector>
#include "Markup.h"

class CGK1RemakeDlg;

class MsgController
{
public:
	MsgController(CGK1RemakeDlg* pOwner);
	virtual ~MsgController(void);
	BOOL Finally();

	CGK1RemakeDlg* GetOwner(){ return m_pOwner; }

//20131126
	BOOL ImportExcel(CString& FilePath, CString& FileName);
	bool SetTranslatedText(CString& szOriginalText, CString& szTranslatedText);

protected:	

private:
	CGK1RemakeDlg* m_pOwner;
	CMarkup xml;
	int m_count;
};
