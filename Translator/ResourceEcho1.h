#pragma once
#include "SCITextResource.h"

class ResourceEcho1 : public SCITextResource
{
public:
	ResourceEcho1(int Num);
	virtual ~ResourceEcho1(void);

	virtual void Init() override;
	virtual BYTE* GetTextFromMessageRecord(BYTE* szMessageRecord) override;

	virtual BOOL SaveHeader(CArchive& ar, BYTE* DataChunkPtr) override;
};