#pragma once
#include <list>
#include <map>

class CTranslatorDlg;
class SCITextResource;

#pragma pack(push, 1)
typedef struct tagTextResPair
{
	SCITextResource* pOriginalTextRes;
	SCITextResource* pTranslatedTextRes;
}TextResPair;

typedef struct tagTextResInfo
{
	int Num;
	int FileSize;
}TextResInfo;

/** resource type for SCI1 resource.map file */
struct resource_index_t {
	USHORT wOffset;
	USHORT wSize;
};

#pragma pack(pop)

typedef std::list<TextResPair> listTextResPair; 
typedef std::map<int, int> mapTextResInfo;

class MsgController
{
public:
	MsgController(CTranslatorDlg* pOwner);
	virtual ~MsgController(void);
	BOOL Finally();

	CTranslatorDlg* GetOwner(){return m_pOwner;}

	BOOL Load();
	BOOL Save();

	BOOL SaveText(CString& szStr);

	BOOL GetText(CString& str, CString& TranStr);
	BOOL GetNextText(CString& str, CString& TranStr);
	BOOL GetPrevText(CString& str, CString& TranStr);

	int GetMessageIndex(){return m_TextNum;}
	CString& GetCurFileName();
	BOOL GetNextTextRes(CString& str, CString& TranStr);
	BOOL GetPrevTextRes(CString& str, CString& TranStr);

	BOOL GenerateOutput();
	BOOL FindText(CString& SearcgText);

protected:
	BOOL CreateTextMap();
	BOOL PackText();

private:
	CTranslatorDlg* m_pOwner;
	listTextResPair m_listTextResPair;
	int m_TextResourceCnt;

	int m_TextResNum;
	int m_TextNum;
	listTextResPair::iterator m_iter;
	mapTextResInfo m_mapTextResInfo;
};
