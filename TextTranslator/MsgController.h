#pragma once
#include <list>
#include <map>
#include <vector>

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

//typedef std::list<TextResPair> listTextResPair; 
typedef std::map<int, int> mapTextResInfo;

typedef std::map<int, TextResPair> mapTextResPair;

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

	BOOL ImportExcel(CString& FilePath, CString& FileName);

protected:
	BOOL CreateTextMap();
	BOOL PackText();	

private:
	CTranslatorDlg* m_pOwner;	
	int m_TextResourceCnt;

	int m_TextResNum;
	int m_TextNum;	
	mapTextResInfo m_mapTextResInfo;

	mapTextResPair::iterator m_iter;
	mapTextResPair m_mapTextResPair;
	
};
