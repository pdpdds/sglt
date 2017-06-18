
// GK1Remake.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGK1RemakeApp:
// See GK1Remake.cpp for the implementation of this class
//

class CGK1RemakeApp : public CWinApp
{
public:
	CGK1RemakeApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGK1RemakeApp theApp;