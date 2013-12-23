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
	USHORT Num;
	int FileSize;
}TextResInfo;

/** resource type for SCI1 resource.map file */
struct resource_index_t {
	USHORT wOffset;
	USHORT wSize;
};

#pragma pack(pop)

typedef std::map<int, TextResPair> mapTextResPair; 
typedef std::vector<TextResInfo> vecTextResInfo;

class MsgController
{
public:
	MsgController(CTranslatorDlg* pOwner);
	virtual ~MsgController(void);
	BOOL Finally();

	CTranslatorDlg* GetOwner(){return m_pOwner;}

	BOOL Load(CString GameName);
	BOOL Save();
	SCITextResource* CreateTextResource(int index);

	BOOL SaveText(CString& szStr);

	BOOL GetText(CString& str, CString& TranStr);
	BOOL GetNextText(CString& str, CString& TranStr);
	BOOL GetPrevText(CString& str, CString& TranStr);

	int GetMessageIndex(){return m_TextNum;}
	CString& GetCurFileName();
	BOOL GetNextTextRes(CString& str, CString& TranStr);
	BOOL GetPrevTextRes(CString& str, CString& TranStr);

//////////////////
//20131006
//////////////////
	BYTE* GetTalker();

	BOOL GenerateOutput();
	BOOL FindText(CString& SearcgText);

//20131126
	BOOL ImportExcel(CString& FilePath, CString& FileName);

protected:
	BOOL CreateMessageMap();
	BOOL PackMessage();

private:
	CTranslatorDlg* m_pOwner;
	mapTextResPair m_mapTextResPair;
	int m_TextResourceCnt;

	int m_TextResNum;
	int m_TextNum;
	mapTextResPair::iterator m_iter;
	vecTextResInfo m_vecTextResInfo;
	CString m_GameName;
};
