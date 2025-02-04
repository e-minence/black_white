//=============================================================================
/**
 * @file	fatal_error.h
 * @brief	FATALエラー表示関数
 * @author	k.ohno
 * @date    2007.2.28
 */
//=============================================================================

#ifndef __FATAL_ERROR_H__
#define __FATAL_ERROR_H__

#include "gflib.h"

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork);

#endif //__FATAL_ERROR_H__

