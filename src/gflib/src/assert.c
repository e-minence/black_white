//==============================================================================================
/**
 * @file	assert.c
 * @date	2007.01.19
 * @author	tamada GAME FREAK inc.
 */
//==============================================================================================

#include "gflib.h"


//==============================================================================================
// ���[�U��`�̕\���֐��Q���Ǘ����邽�߂̃|�C���^
//==============================================================================================
static GFL_ASSERT_DispInitFunc		DispInitFunc = NULL;
static GFL_ASSERT_DispMainFunc		DispMainFunc = NULL;
static GFL_ASSERT_DispFinishFunc	DispFinishFunc = NULL;

//==============================================================================================
//==============================================================================================


//------------------------------------------------------------------
/**
 * �A�T�[�g���o�͏����֐������[�U�ݒ肷��
 *
 * @param   initFunc		�\�����������ɌĂ΂��R�[���o�b�N�֐�
 * @param   mainFunc		�����o�͂��ƂɌĂ΂��R�[���o�b�N�֐�
 * @param   finishFunc		�\���I�����ɌĂ΂��R�[���o�b�N�֐�
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc )
{
	DispInitFunc = initFunc;
	DispMainFunc = mainFunc;
	DispFinishFunc = finishFunc;
}


//------------------------------------------------------------------
/**
 * �A�T�[�g���o�͑O����
 *
 * @param   none		
 *
 */
//------------------------------------------------------------------
static void InitAssertDisp( void )
{
	if( DispInitFunc != NULL )
	{
		DispInitFunc();
	}
}

//------------------------------------------------------------------
/**
 * �A�T�[�g���o�̓��C������
 *
 * @param   fmt			�����t��������
 * @param   vlist		�������
 *
 */
//------------------------------------------------------------------
static void AssertVPrintf( const char* fmt, va_list vlist )
{
	if( DispMainFunc != NULL )
	{
		enum {
			TMP_BUFFER_SIZE = 512,
		};

		static char buf[ TMP_BUFFER_SIZE ];

		OS_VSNPrintf( buf, TMP_BUFFER_SIZE, fmt, vlist );
		DispMainFunc( buf );
	}
	else
	{
		OS_TVPrintf(fmt, vlist);
	}
}

//------------------------------------------------------------------
/**
 * �A�T�[�g���o�͏I������
 */
//------------------------------------------------------------------
static void FinishAssertDisp( void )
{
	if( DispFinishFunc != NULL )
	{
		DispFinishFunc();
	}
	else
	{
		OS_Terminate();
	}
}

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
    AssertVPrintf( fmt, vlist );
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
	InitAssertDisp();

	AssertPrintTitle(filename, line_no);
	AssertPrintf("%s\n", exp);

	FinishAssertDisp();
}

//------------------------------------------------------------------
/**
 * @brief	ASSERT�����i�������ƃ��[�U�[�w��̕�����\���j
 */
//------------------------------------------------------------------
void GFL_AssertMsg(const char * filename, unsigned int line_no, const char * fmt, ... )
{
	InitAssertDisp();

	AssertPrintTitle(filename, line_no);
	{
	    va_list vlist;
	    va_start(vlist, fmt);
		AssertVPrintf( fmt, vlist );
	    va_end(vlist);
	}
	FinishAssertDisp();
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

