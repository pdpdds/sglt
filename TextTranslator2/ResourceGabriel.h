#pragma once
#include "SCITextResource.h"

static char* gabrielActor[] = {"dfgdg", "���긮�� ����Ʈ", "�׷��̽� ��Ű����", "unknown", "��Ȳ����", "������", "�𽽸�", "���긮�� �ҸӴ�","��� ����", "������ �Ե�" };

class ResourceGabriel : public SCITextResource
{
public:
	ResourceGabriel(int Num);
	virtual ~ResourceGabriel(void);

	virtual void Init() override;
	virtual BYTE* GetTextFromMessageRecord(BYTE* szMessageRecord) override;

	virtual BOOL SaveHeader(CArchive& ar, BYTE* DataChunkPtr) override;
	virtual BYTE* GetTalker(int MessageIndex) override;
};
