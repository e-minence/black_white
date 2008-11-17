//=============================================================================================
/**
 * @file	btl_util.h
 * @brief	�|�P����WB �o�g���V�X�e�� ���L�c�[���Q
 * @author	taya
 *
 * @date	2008.09.25	�쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_util.h"


void BTL_UTIL_Printf( const char* filename, int line, const char* fmt, ... )
{
//	AssertPrintTitle(filename, line_no);
	{
	    va_list vlist;
	    va_start( vlist, fmt );
	    OS_TVPrintf( fmt, vlist );
	    va_end( vlist );
	}

}

