#pragma once
#include <list>
#include <map>
const UINT _headerSize = 8;
const UINT _recordSize = 10;



#pragma pack(push, 1)
typedef struct tag_TextDesc
{
	BYTE TextHeader[_recordSize];

}TextDesc;

typedef struct tag_NewText
{
	CString Str;
}NewText;

#pragma pack(pop)

typedef std::list<TextDesc> listTextDesc;
typedef std::map<int, NewText> mapNewText;
typedef std::map<int, int> mapBlankList;

class SCITextResource
{
public:
	SCITextResource(int Num);
	virtual ~SCITextResource(void);

	BOOL Load(const CString&  szFileName);
	BOOL Save();

	BOOL ReadText(int MessageIndex, CString& szText );
	BOOL ReadOriginalText(int MessageIndex, CString& szText );
	CString& GetFileName(){return m_FileName;}
	UINT GetMessageCnt(){return m_MessageCnt;}

	void SetChangeFlag(BOOL bFlag){m_bChanged = bFlag;}
	BOOL GetChangeFlag(){return m_bChanged;}

	void SetText(int TextNum, CString& szStr);

	int FindText(CString& SearchText);

protected:
	static BYTE DataChunk[65536];

private:
	UINT m_MessageCnt;
	listTextDesc m_listTextDesc;
	BYTE* m_pStart;
	UINT m_Size;
	CString m_FileName;
	USHORT m_Num;
	int m_fileHeader;

	BYTE* m_pData;
	BOOL m_bChanged;
	int m_FirstBlankCount;

	mapNewText m_mapNewText;
	mapBlankList m_mapBlankList;
};
