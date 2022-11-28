#pragma once
#include "SCITextResource.h"

class ResourceAmonRa : public SCITextResource
{
public:
	ResourceAmonRa(int Num);
	virtual ~ResourceAmonRa(void);

	virtual void Init() override;
};