#pragma once
#include "SCITextResource.h"

enum {
	DUMMY = 0,
	GABRIEL,
	GRACE,
	UNKNOWN,
	SITUATION,
	SELECT,
	MOSLY,
	GRANDMOTHER,
	FRIC,
	MARIA,
};

static char* gabrielActor[] = {"dummy", "���긮�� ����Ʈ", "�׷��̽� ��Ű����", "unknown", "��Ȳ����", "������", "�𽽸�", "���긮�� �ҸӴ�","��� ����", "������ �Ե�" };

class ResourceGabriel : public SCITextResource
{
public:
	ResourceGabriel(int Num);
	virtual ~ResourceGabriel(void);

	virtual void Init() override;
	virtual BYTE* GetTalker(int MessageIndex) override;
};
