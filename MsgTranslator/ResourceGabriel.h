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

static char* gabrielActor[] = {"dummy", "가브리엘 나이트", "그레이스 나키무라", "unknown", "상황설명", "선택지", "모슬리", "가브리엘 할머니","경사 프릭", "마리아 게데" };

class ResourceGabriel : public SCITextResource
{
public:
	ResourceGabriel(int Num);
	virtual ~ResourceGabriel(void);

	virtual void Init() override;
	virtual BYTE* GetTalker(int MessageIndex) override;
};
