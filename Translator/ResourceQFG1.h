#pragma once
#include "SCITextResource.h"

class ResourceQFG1 : public SCITextResource
{
public:
	ResourceQFG1(int Num);
	virtual ~ResourceQFG1(void);

	virtual void Init() override;
	virtual BYTE* GetTextFromMessageRecord(BYTE* szMessageRecord) override;

	virtual BOOL SaveHeader(CArchive& ar, BYTE* DataChunkPtr) override;
};