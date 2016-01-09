#pragma once
#include <list>
#include <map>

#pragma pack(push, 1)

typedef struct tag_NewText
{
	CString Str;
}NewText;

struct MessageTuple {
	byte noun;
	byte verb;
	byte cond;
	byte seq;

	MessageTuple(byte noun_ = 0, byte verb_ = 0, byte cond_ = 0, byte seq_ = 1)
		: noun(noun_), verb(verb_), cond(cond_), seq(seq_) { }
};

struct MessageRecord {
	MessageTuple tuple;
	MessageTuple refTuple;
	const char *string;
	byte talker;
};

#pragma pack(pop)

typedef std::map<int, NewText> mapNewText;

class SCITextResource
{
public:
	SCITextResource(int Num);
	virtual ~SCITextResource(void);

	virtual void Init() = 0;
	virtual BYTE* GetTextFromMessageRecord(BYTE* szMessageRecord) = 0;
	virtual BOOL SaveHeader(CArchive& ar, BYTE* DataChunkPtr) = 0;
	virtual BYTE* GetTalker(int MessageIndex) {return NULL;}

	BOOL Load(const CString&  szFileName);
	BOOL Save();
	

	BOOL ReadText(int MessageIndex, CString& szText );
	void SetText(int TextNum, CString& szStr);

	BOOL ReadOriginalText(int MessageIndex, CString& szText );
	CString& GetFileName(){return m_FileName;}
	UINT GetMessageCnt(){return m_MessageCnt;}

	void SetChangeFlag(BOOL bFlag){m_bChanged = bFlag;}
	BOOL GetChangeFlag(){return m_bChanged;}


	int FindText(CString& SearchText);

protected:
	static BYTE DataChunk[65536];

protected:
	UINT m_MessageCnt;
	BYTE* m_pStart;
	UINT m_Size;
	CString m_FileName;
	USHORT m_Num;

	BYTE* m_pData;
	BOOL m_bChanged;

	mapNewText m_mapNewText;

	UINT _headerSize;
	UINT _recordSize;
	UINT _textOffset;
};
