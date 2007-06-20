//==============================================================================================
/**
 * @file	assert.c
 * @date	2007.01.19
 * @author	tamada GAME FREAK inc.
 */
//==============================================================================================

#include "gflib.h"

//==============================================================================================
//==============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ASSERT�����p�\�����C��
 * @param	fmt
 */
//------------------------------------------------------------------
static void AssertPrintf(const char * fmt, ... )
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);
}

//------------------------------------------------------------------
/**
 * @brief	�A�T�[�g���s�ӏ��̃\�[�X�����o��
 */
//------------------------------------------------------------------
static void AssertPrintTitle(const char * filename, unsigned int line_no)
{
	AssertPrintf("  **** ASSERTION FAILED ! **** \n");
	AssertPrintf("%s(%d)\n", filename, line_no);
}

//------------------------------------------------------------------
/**
 * @brief	ASSERT�����ȈՔŁi�������Ɣ����ӏ��̕\���j
 */
//------------------------------------------------------------------
void GFL_Assert(const char * filename, unsigned int line_no, const char * exp)
{
	AssertPrintTitle(filename, line_no);

	AssertPrintf("%s\n", exp);

	OS_Terminate();
}

//------------------------------------------------------------------
/**
 * @brief	ASSERT�����i�������ƃ��[�U�[�w��̕�����\���j
 */
//------------------------------------------------------------------
void GFL_AssertMsg(const char * filename, unsigned int line_no, const char * fmt, ... )
{
	va_list vlist;
	AssertPrintTitle(filename, line_no);
	va_start(vlist, fmt);
	OS_TVPrintf(fmt, vlist);
	va_end(vlist);

	OS_Terminate();
}

#if 0
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void GFL_AssertPrintf(const char * fmt, ... )
{
}
#endif

