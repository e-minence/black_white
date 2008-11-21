//=============================================================================================
/**
 * @file	printsys.c
 * @brief	Bitmap���p�����`��@�V�X�e������
 * @author	taya
 *
 * @date	2007.02.06	�쐬
 * @date	2008.09.11	�ʐM�Ή��L���[�\�����쐬
 */
//=============================================================================================
#include <gflib.h>
#include <tcbl.h>

#include "print/printsys.h"

//==============================================================
// Consts
//==============================================================
enum {
	LETTER_CHAR_WIDTH = 2,			///< �P���������艡�L������
	LETTER_CHAR_HEIGHT = 2,			///< �P����������c�L������
	LINE_DOT_HEIGHT = 16,			///< �P�s������̃h�b�g��

	// ���s�R�[�h�A�I�[�R�[�h
	EOM_CODE			= 0xffff,
	CR_CODE				= 0xfffe,

	// �ėp�R���g���[���^�C�v
	CTRL_GENERAL_COLOR			= (0x0000),	///< �F�ύX
	CTRL_GENERAL_RESET_COLOR	= (0x0001),	///< �F�ύX
	CTRL_GENERAL_X_RIGHTFIT		= (0x0002),	///< �w���W�E��
	CTRL_GENERAL_X_CENTERING	= (0x0003),	///< �w���W�Z���^�����O

	// ����郁�b�Z�[�W���̂ݗL���ȃR���g���[���^�C�v
	CTRL_STREAM_LINE_FEED		= (0x0000),	///< ���y�[�W�i�s����҂��j
	CTRL_STREAM_PAGE_CLEAR		= (0x0001),	///< ���y�[�W�i�`��N���A�҂��j
	CTRL_STREAM_CHANGE_WAIT		= (0x0002),	///< �`��E�F�C�g�ύX�i�P��j
	CTRL_STREAM_SET_WAIT		= (0x0003),	///< �`��E�F�C�g�ύX�i�i���j
	CTRL_STREAM_RESET_WAIT		= (0x0004),	///< �`��E�F�C�g���f�t�H���g�ɖ߂�
	CTRL_STREAM_CHANGE_ARGV		= (0x0005),	///< �R�[���o�b�N������ύX�i�P��j
	CTRL_STREAM_SET_ARGV		= (0x0006),	///< �R�[���o�b�N������ύX�i�i���j
	CTRL_STREAM_RESET_ARGV		= (0x0007),	///< �R�[���o�b�N�������f�t�H���g�ɖ߂�
	CTRL_STREAM_FORCE_CLEAR		= (0x0008),	///< �����`��N���A

	// �V�X�e���R���g���[��
	CTRL_SYSTEM_COLOR			= (0x0000),

	// �^�O�J�n�R�[�h
	SPCODE_TAG_START_	= 0xf000,
	TAGTYPE_WORD = 0,				///< ������}��
	TAGTYPE_NUMBER = 1,				///< ���l�}��
	TAGTYPE_GENERAL_CTRL = 0xbd,	///< �ėp�R���g���[������
	TAGTYPE_STREAM_CTRL = 0xbe,		///< ����郁�b�Z�[�W�p�R���g���[������
	TAGTYPE_SYSTEM = 0xff,			///< �G�f�B�^�V�X�e���^�O�p�R���g���[������


	// �v�����g�L���[�֘A�萔
	QUE_DEFAULT_BUFSIZE = 1024,
	QUE_DEFAULT_TICK = 2200,		///< �ʐM���A�P�x�̕`�揈���Ɏg�����Ԃ̖ڈ��iTick�j�f�t�H���g�l

};


//==============================================================
// Typedefs
//==============================================================

// �����R�[�h
typedef u16		STRCODE;


//--------------------------------------------------------------------------
/**
 *	Print Job
 *
 *  Bitmap�ւ̕`�揈�����[�N
 */
//--------------------------------------------------------------------------
		typedef struct {
	u16   org_x;		///< �������݊J�n�w���W
	u16   org_y;		///< �������݊J�n�x���W
	u16   write_x;		///< �������ݒ��̂w���W
	u16   write_y;		///< �������ݒ��̂x���W

	u8    colorLetter;		///< �`��F�i�����j
	u8    colorShadow;		///< �`��F�i�e�j
	u8    colorBackGround;	///< �`��F�i�w�i�j

	GFL_FONT*		fontHandle;	///< �t�H���g�n���h��
	GFL_BMP_DATA*	dst;		///< �������ݐ� Bitmap

}PRINT_JOB;

//--------------------------------------------------------------------------
/**
 *	Print Queue
 *
 *	�ʐM���A�`�揈���������݂Ɏ��s�ł���悤�A�������e��~�ς��邽�߂̋@�\�B
 */
//--------------------------------------------------------------------------
struct _PRINT_QUE {

	PRINT_JOB*		runningJob;
	const STRCODE*	sp;

	OSTick	limitPerFrame;

	u16		bufTopPos;
	u16		bufEndPos;
	u16		bufSize;
	u16		dmy;

	u8   buf[0];
};

//--------------------------------------------------------------------------
/**
 *	Print Stream
 *
 *	��b�E�B���h�E���A�P��Ԋu�łP�������`��C�\�����s�����߂̋@�\�B
 */
//--------------------------------------------------------------------------
struct _PRINT_STREAM {

	PRINTSTREAM_STATE		state;
	PRINTSTREAM_PAUSE_TYPE	pauseType;

	GFL_BMPWIN*			dstWin;
	GFL_BMP_DATA*		dstBmp;
	GFL_TCBL*			tcb;

	const STRCODE* sp;

	u16		org_wait;
	u16		current_wait;
	u16		wait;
	u8		pauseReleaseFlag;
	u8		pauseWait;
	u8		clearColor;

	pPrintCallBack	callback_func;
	u32				org_arg;
	u32				current_arg;
	u32				arg;

	PRINT_JOB	printJob;



};


//==============================================================================================
//--------------------------------------------------------------------------
/**
 *	SystemWork
 */
//--------------------------------------------------------------------------
static struct {
	GFL_BMP_DATA*		charBuffer;
	PRINT_JOB			printJob;
}SystemWork;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline BOOL IsNetConnecting( void );
static void setupPrintJob( PRINT_JOB* wk, GFL_FONT* font, GFL_BMP_DATA* dst, u16 org_x, u16 org_y );
static const STRCODE* print_next_char( PRINT_JOB* wk, const STRCODE* sp );
static void put_1char( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, GFL_FONT* fontHandle, STRCODE charCode, u16* charWidth, u16* charHeight );
static const STRCODE* ctrlGeneralTag( PRINT_JOB* wk, const STRCODE* sp );
static const STRCODE* ctrlSystemTag( PRINT_JOB* wk, const STRCODE* sp );
static void print_stream_task( GFL_TCBL* tcb, void* wk_adrs );
static void ctrlStreamTag( PRINT_STREAM* wk );
static u32 get_line_width( const STRCODE* sp, GFL_FONT* font, u16 margin, const STRCODE** endPtr );
static inline u16 STR_TOOL_GetTagType( const STRCODE* sp );
static inline u16 STR_TOOL_GetTagParam( const STRCODE* sp, u16 paramIndex );
static inline const STRCODE* STR_TOOL_SkipTag( const STRCODE* sp );
static inline u16 STR_TOOL_GetTagNumber( const STRCODE* sp );
static u32 Que_CalcStringBufferSize( const STRBUF* str );
static u32 Que_CalcUseBufSize( const STRBUF* buf );
static u32 Que_GetFreeSize( const PRINT_QUE* que );
static void Que_StoreNewJob( PRINT_QUE* que, PRINT_JOB* printJob, const STRBUF* str );
static void* Que_WriteDataUnit( PRINT_QUE* que, const u8* src, u16 totalSize, u16 unitSize );
static PRINT_JOB* Que_ReadNextJobAdrs( const PRINT_QUE* que, const PRINT_JOB* job );
static inline u16 Que_WriteData( PRINT_QUE* que, const u8* src, u16 pos, u16 size );
static inline u16 Que_AdrsToBufPos( const PRINT_QUE* que, const void* bufAdrs );
static inline const void* Que_BufPosToAdrs( const PRINT_QUE* que, u16 bufPos );
static inline u16 Que_FwdBufPos( const PRINT_QUE* que, u16 pos, u16 fwdSize );






//==============================================================================================
/**
 * �V�X�e���������iGFLIB��������ɂP�x�����Ăԁj
 *
 * @param   heapID		�������p�q�[�vID
 *
 */
//==============================================================================================
void PRINTSYS_Init( HEAPID heapID )
{
	GFL_STR_SetEOMCode( EOM_CODE );

	// �P��������Bitmap�̈�̂݊m��
	SystemWork.charBuffer = GFL_BMP_Create( LETTER_CHAR_WIDTH, LETTER_CHAR_HEIGHT, GFL_BMP_16_COLOR, heapID );
}



//==============================================================================================
/**
 * �v�����g�L���[���f�t�H���g�̃o�b�t�@�T�C�Y�Ő�������
 *
 * @param   heapID			�����p�q�[�vID
 *
 * @retval  PRINT_QUE*		�������ꂽ�v�����g�L���[
 */
//==============================================================================================
PRINT_QUE* PRINTSYS_QUE_Create( HEAPID heapID )
{
	return PRINTSYS_QUE_CreateEx( QUE_DEFAULT_BUFSIZE, heapID );
}

//==============================================================================================
/**
 * �v�����g�L���[���T�C�Y�w�肵�Đ�������
 *
 * @param   size			�o�b�t�@�T�C�Y
 * @param   heapID			�����p�q�[�vID
 *
 * @retval  PRINT_QUE*		
 */
//==============================================================================================
PRINT_QUE* PRINTSYS_QUE_CreateEx( u16 buf_size, HEAPID heapID )
{
	PRINT_QUE* que;
	u16 real_size;

	// �����O�o�b�t�@�̏I�[�ō\���̂����f����Ȃ��悤�A
	// �m�ۃT�C�Y�ɗ]�T���������Ă���
	real_size = buf_size + sizeof(PRINT_JOB);
	while( real_size % 4 ){ real_size++; }

	que = GFL_HEAP_AllocMemory( heapID, sizeof(PRINT_QUE) + real_size );

	que->bufTopPos = 0;
	que->bufEndPos = buf_size;
	que->bufSize = buf_size;		// �I�[����͎w��̃T�C�Y�l�ōs��
	que->limitPerFrame = QUE_DEFAULT_TICK;
	que->runningJob = NULL;
	que->sp = NULL;

//	GFL_STD_MemClear( que->buf, size );

	return que;
}

//==============================================================================================
/**
 * �v�����g�L���[���폜����
 *
 * @param   que		�v�����g�L���[
 *
 */
//==============================================================================================
void PRINTSYS_QUE_Delete( PRINT_QUE* que )
{
	GF_ASSERT(que->runningJob==NULL);
	GFL_HEAP_FreeMemory( que );
}

//==============================================================================================
/**
 * �v�����g�L���[�ɒ~�ς��ꂽ������`���Ƃ���������B
 * �ʐM���Ȃ��莞�Ԗ��ɏ�����Ԃ��B
 * �ʐM���łȂ���Έ�C�ɏ������I����i�c�͖̂������j
 *
 * @param   que		�v�����g�L���[
 *
 * @retval  BOOL	�������I�����Ă����TRUE�^����ȊO��FALSE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_Main( PRINT_QUE* que )
{
	if( que->runningJob )
	{
		OSTick start, diff;
		u8 endFlag = FALSE;

		start = OS_GetTick();

		while( que->runningJob )
		{
			while( *(que->sp) != EOM_CODE )
			{
				que->sp = print_next_char( que->runningJob, que->sp );
				diff = OS_GetTick() - start;
				if( diff > que->limitPerFrame )
				{
					endFlag = TRUE;
					break;
				}
			}
			//
			if( *(que->sp) == EOM_CODE )
			{
				u16 pos = Que_AdrsToBufPos( que, que->runningJob );

				que->runningJob = Que_ReadNextJobAdrs( que, que->runningJob );
				if( que->runningJob )
				{
					pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB) );
					pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB*) );
					que->sp = Que_BufPosToAdrs( que, pos );
				}
			}

			if( endFlag ){ break; }

		}

		if( que->runningJob )
		{
			que->bufEndPos = Que_AdrsToBufPos( que, que->sp );
			return FALSE;
		}

		que->bufTopPos = 0;
		que->bufEndPos = que->bufSize;

	}

	return TRUE;
}
//==============================================================================================
/**
 * �v�����g�L���[�̏������S�Ċ������Ă��邩�`�F�b�N
 *
 * @param   que			�v�����g�L���[
 *
 * @retval  BOOL		�������Ă����TRUE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_IsFinished( const PRINT_QUE* que )
{
	return (que->runningJob == NULL);
}

//==============================================================================================
/**
 * �v�����g�L���[�ɁA����Bitmap���o�͑Ώۂ̕`�揈�����c���Ă��邩�`�F�b�N
 *
 * @param   que				�v�����g�L���[
 * @param   targetBmp		�`��Ώ�Bitmap
 *
 * @retval  BOOL			�c���Ă����TRUE
 */
//==============================================================================================
BOOL PRINTSYS_QUE_IsExistTarget( const PRINT_QUE* que, const GFL_BMP_DATA* targetBmp )
{
	if( que->runningJob )
	{
		const PRINT_JOB* job = que->runningJob;

		while( job )
		{
			if( job->dst == targetBmp )
			{
				return TRUE;
			}
			job = Que_ReadNextJobAdrs( que, job );
		}

	}
	return FALSE;
}

//--------------------------------------------------------------------------
/**
 * �ʐM���i�`�揈���𕪊�����K�v������j���ǂ�������
 *
 * @retval  BOOL		�ʐM���Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static inline BOOL IsNetConnecting( void )
{
	return (GFL_NET_GetConnectNum() != 0);
}


//==============================================================================================
/**
 * �v�����g�L���[�����������`�揈��
 *
 * @param   que		[out] �v�����g�L���[
 * @param   dst		[out] �`���Bitmap
 * @param   xpos	[in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos	[in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str		[in]  ������
 * @param   font	[in]  �t�H���g�^�C�v
 *
 */
//==============================================================================================
void PRINTSYS_PrintQue( PRINT_QUE* que, GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font )
{
	GF_ASSERT(que);
	GF_ASSERT(dst);
	GF_ASSERT(str);
	GF_ASSERT(font);

	{
		PRINT_JOB* job = &SystemWork.printJob;
		setupPrintJob( job, font, dst, xpos, ypos );

		if( !IsNetConnecting() )
		{
			const STRCODE* sp = GFL_STR_GetStringCodePointer( str );
			while( *sp != EOM_CODE )
			{
				sp = print_next_char( job, sp );
			}
		}
		else
		{
			u32  size = Que_CalcUseBufSize( str );

			if( size <= Que_GetFreeSize( que ) )
			{
				Que_StoreNewJob( que, job, str );
			}
			else
			{
				GF_ASSERT_MSG(0, "[PRINT ACM] not enough buffer ... strsize = %d\n", size);
			}
		}
	}
}

//==============================================================================================
/**
 * Bitmap �֒��ڂ̕�����`��
 *
 * @param   dst		[out] �`���Bitmap
 * @param   xpos	[in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos	[in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str		[in]  ������
 * @param   font	[in]  �t�H���g
 *
 */
//==============================================================================================
void PRINTSYS_Print( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font )
{
	PRINT_JOB* job = &SystemWork.printJob;
	const STRCODE* sp = GFL_STR_GetStringCodePointer( str );

	setupPrintJob( job, font, dst, xpos, ypos );

	while( *sp != EOM_CODE )
	{
		sp = print_next_char( job, sp );
	}
}


//------------------------------------------------------------------
/**
 * �`�揈�����[�N�̏�����
 *
 * @param   wk			���[�N�|�C���^
 * @param   font_type	�t�H���g�^�C�v
 * @param   dst			�`���Bitmap
 * @param   org_x		�`��J�n�w���W
 * @param   org_y		�`��J�n�x���W
 *
 */
//------------------------------------------------------------------
static void setupPrintJob( PRINT_JOB* wk, GFL_FONT* font, GFL_BMP_DATA* dst, u16 org_x, u16 org_y )
{
	wk->dst = dst;
	wk->fontHandle = font;
	wk->org_x = wk->write_x = org_x;
	wk->org_y = wk->write_y = org_y;
	GFL_FONTSYS_GetColor( &wk->colorLetter, &wk->colorShadow, &wk->colorBackGround );
}
//------------------------------------------------------------------
/**
 * ���s����єėp�R���g���[��������Ώ������A
 * �ʏ핶��������΂P�����`�悵�āA���̕����̃|�C���^��Ԃ�
 *
 * @param   wk		�`�惏�[�N�|�C���^
 * @param   sp		
 *
 * @retval  const STRCODE*		
 */
//------------------------------------------------------------------
static const STRCODE* print_next_char( PRINT_JOB* wk, const STRCODE* sp )
{
	while( 1 )
	{
		switch( *sp ){
		case EOM_CODE:
			return sp;

		case CR_CODE:
			wk->write_x = wk->org_x;
			wk->write_y += LINE_DOT_HEIGHT;
			sp++;
			break;

		case SPCODE_TAG_START_:
			switch( STR_TOOL_GetTagType(sp) ){
			case TAGTYPE_GENERAL_CTRL:
				sp = ctrlGeneralTag( wk, sp );
				break;

			case TAGTYPE_SYSTEM:
				sp = ctrlSystemTag( wk, sp );
				break;

			default:
				sp = STR_TOOL_SkipTag( sp );
				break;
			}
			break;

		default:
			{
				u16 w, h;

				put_1char( wk->dst, wk->write_x, wk->write_y, wk->fontHandle, *sp, &w, &h );
				wk->write_x += (w+1);
				sp++;

				return sp;
			}
			break;
		}
	}
}
//------------------------------------------------------------------
/**
 * Bitmap�P�������`��
 *
 * @param   dst				�`���r�b�g�}�b�v
 * @param   xpos			�`���w���W�i�h�b�g�j
 * @param   ypos			�`���x���W�i�h�b�g�j
 * @param   fontHandle		�`��t�H���g�n���h��
 * @param   charCode		�����R�[�h
 * @param   charWidth		�������擾���[�N
 * @param   charHeight		�������擾���[�N
 *
 */
//------------------------------------------------------------------
static void put_1char( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, GFL_FONT* fontHandle, STRCODE charCode, u16* charWidth, u16* charHeight )
{
	GFL_FONT_GetBitMap( fontHandle, charCode, GFL_BMP_GetCharacterAdrs(SystemWork.charBuffer), charWidth, charHeight );
	GFL_BMP_Print( SystemWork.charBuffer, dst, 0, 0, xpos, ypos, *charWidth, *charHeight, 0x0f );
}
//------------------------------------------------------------------
/**
 * �ėp�R���g���[������
 *
 * @param   wk		�`�揈�����[�N
 * @param   sp		�������̕�����|�C���^�i=SPCODE_TAG_START_���|�C���g���Ă����ԂŌĂԂ��Ɓj
 *
 * @retval  const STRCODE*		������̕�����|�C���^
 */
//------------------------------------------------------------------
static const STRCODE* ctrlGeneralTag( PRINT_JOB* wk, const STRCODE* sp )
{
	switch( STR_TOOL_GetTagNumber(sp) ){
	case CTRL_GENERAL_COLOR:
		{
			u8 colL, colS, colB;

			colL = STR_TOOL_GetTagParam(sp, 0);
			colS = STR_TOOL_GetTagParam(sp, 1);
			colB = STR_TOOL_GetTagParam(sp, 2);

			GFL_FONTSYS_SetColor( colL, colS, colB );
		}
		break;

	case CTRL_GENERAL_RESET_COLOR:
		GFL_FONTSYS_SetColor( wk->colorLetter, wk->colorShadow, wk->colorBackGround );
		break;

	case CTRL_GENERAL_X_RIGHTFIT:
		{
			int bmpWidth, strWidth;

			bmpWidth = GFL_BMP_GetSizeX( wk->dst ) - wk->org_x;
			strWidth = get_line_width( sp, wk->fontHandle, 0, NULL );

			wk->write_x = wk->org_x + ((bmpWidth - strWidth) / 2);

			OS_TPrintf("[PRINTSYS] XfittingR ... bmpW=%d, strW=%d -> wrtX=%d\n",
						bmpWidth, strWidth, wk->write_x);
		}
		break;

	case CTRL_GENERAL_X_CENTERING:
		{
			int bmpWidth, strWidth;

			bmpWidth = GFL_BMP_GetSizeX( wk->dst ) - wk->org_x;
			strWidth = get_line_width( sp, wk->fontHandle, 0, NULL );

			if( strWidth < bmpWidth )
			{
				wk->write_x = wk->org_x + ((bmpWidth - strWidth) / 2);
			}
			else
			{
				wk->write_x = wk->org_x;
			}
		}
		break;

	default:
		GF_ASSERT_MSG(0, "illegal general-ctrl code ->%d\n", STR_TOOL_GetTagNumber(sp));
		break;
	}

	return STR_TOOL_SkipTag( sp );
}
//------------------------------------------------------------------
/**
 * �V�X�e���R���g���[������
 *
 * @param   wk		�`�揈�����[�N
 * @param   sp		�������̕�����|�C���^�i=SPCODE_TAG_START_���|�C���g���Ă����ԂŌĂԂ��Ɓj
 *
 * @retval  const STRCODE*		������̕�����|�C���^
 */
//------------------------------------------------------------------
static const STRCODE* ctrlSystemTag( PRINT_JOB* wk, const STRCODE* sp )
{
	u16 ctrlCode = STR_TOOL_GetTagNumber(sp);

	switch( ctrlCode ){
	case CTRL_SYSTEM_COLOR:
		{
			u8  color = STR_TOOL_GetTagParam( sp, 0 );
			GFL_FONTSYS_SetColor( color*2+1, color*2+2, wk->colorBackGround );
		}
		break;

	default:
		GF_ASSERT_MSG(0, "illegal system-ctrl code ->%d\n", ctrlCode);
		break;
	}

	return STR_TOOL_SkipTag( sp );
}


//==============================================================================================================
// �v�����g�X�g���[���֘A����
//==============================================================================================================

//==============================================================================================
/**
 * �v�����g�X�g���[���쐬�i�ʏ�� - �R�[���o�b�N�Ȃ��j
 *
 * @param   dst		�`���Bitmap
 * @param   xpos	�`��J�n�w���W�i�h�b�g�j
 * @param   ypos	�`��J�n�x���W�i�h�b�g�j
 * @param   str		������
 * @param   font	�t�H���g�^�C�v
 * @param   wait		�P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @param   tcbsys		�g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri		�g�p����^�X�N�v���C�I���e�B
 * @param   heapID		�g�p����q�[�vID
 *
 * @retval	PRINT_STREAM*	�X�g���[���n���h��
 *
 */
//==============================================================================================
PRINT_STREAM* PRINTSYS_PrintStream(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor )
{
	return PRINTSYS_PrintStreamCallBack(
		dst, xpos, ypos, str, font, wait, tcbsys, tcbpri, heapID, clearColor, NULL
	);
}

//==============================================================================================
/**
 * �v�����g�X�g���[���쐬�i�R�[���o�b�N����j
 *
 * @param   dst		�`���Bitmap
 * @param   xpos	�`��J�n�w���W�i�h�b�g�j
 * @param   ypos	�`��J�n�x���W�i�h�b�g�j
 * @param   str		������
 * @param   font	�t�H���g�^�C�v
 * @param   wait		�P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @param   tcbsys		�g�p����GFL_TCBL�V�X�e���|�C���^
 * @param   tcbpri		�g�p����^�X�N�v���C�I���e�B
 * @param   heapID		�g�p����q�[�vID
 * @param   callback	�P�����`�悲�Ƃ̃R�[���o�b�N�֐��A�h���X
 *
 * @retval	PRINT_STREAM*	�X�g���[���n���h��
 *
 */
//==============================================================================================
PRINT_STREAM* PRINTSYS_PrintStreamCallBack(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor, pPrintCallBack callback )
{
	PRINT_STREAM* stwk;
	GFL_TCBL* tcb;
	GFL_BMP_DATA* dstBmp;

	dstBmp = GFL_BMPWIN_GetBmp( dst );;
	tcb = GFL_TCBL_Create( tcbsys, print_stream_task, sizeof(PRINT_STREAM), tcbpri );
	stwk = GFL_TCBL_GetWork( tcb );
	stwk->tcb = tcb;

	setupPrintJob( &stwk->printJob, font, dstBmp, xpos, ypos );

	stwk->sp = GFL_STR_GetStringCodePointer( str );
	stwk->org_wait = wait;
	stwk->current_wait = wait;
	stwk->wait = 0;
	stwk->callback_func = callback;
	stwk->org_arg = 0;
	stwk->current_arg = 0;
	stwk->arg = 0;
	stwk->dstWin = dst;
	stwk->dstBmp = dstBmp;
	stwk->state = PRINTSTREAM_STATE_RUNNING;
	stwk->clearColor = clearColor;
	stwk->pauseType = PRINTSTREAM_PAUSE_CLEAR;
	stwk->pauseWait = 0;
	stwk->pauseReleaseFlag = FALSE;

	return stwk;
}

//==============================================================================================
/**
 * �v�����g�X�g���[����Ԏ擾
 *
 * @param   handle		�X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_STATE		���
 */
//==============================================================================================
PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM* handle )
{
	return handle->state;
}
//==============================================================================================
/**
 * �v�����g�X�g���[����~�^�C�v�擾
 *
 * @param   handle		�X�g���[���n���h��
 *
 * @retval  PRINTSTREAM_PAUSE_TYPE		��~�^�C�v
 */
//==============================================================================================
PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM* handle )
{
	return handle->pauseType;
}
//==============================================================================================
/**
 * �v�����g�X�g���[����~��ԉ���
 *
 * @param   handle		
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM* handle )
{
	handle->pauseReleaseFlag = TRUE;
}
//==============================================================================================
/**
 * �v�����g�X�g���[���폜
 *
 * @param   handle		
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamDelete( PRINT_STREAM* handle )
{
	if( handle->tcb )
	{
		GFL_TCBL_Delete( handle->tcb );
		handle->tcb = NULL;
	}
}
//==============================================================================================
/**
 * �v�����g�X�g���[���E�F�C�g�Z�k�i�L�[�������Ȃǁj
 *
 * @param   handle		�X�g���[���n���h��
 * @param   wait		�ݒ肷��E�F�C�g�t���[����
 *
 */
//==============================================================================================
void PRINTSYS_PrintStreamShortWait( PRINT_STREAM* handle, u16 wait )
{
	if( wait < handle->wait )
	{
		handle->wait = wait;
	}
}

//------------------------------------------------------------------
/**
 * �v�����g�X�g���[���`��^�X�N
 *
 * @param   tcb			
 * @param   wk_adrs		
 *
 */
//------------------------------------------------------------------
static void print_stream_task( GFL_TCBL* tcb, void* wk_adrs )
{
	PRINT_STREAM* wk = wk_adrs;

	switch( wk->state ){
	case PRINTSTREAM_STATE_RUNNING:
		if( wk->wait == 0 )
		{
			switch( *(wk->sp) ){
			case EOM_CODE:
				wk->state = PRINTSTREAM_STATE_DONE;
				break;

			case SPCODE_TAG_START_:
				if( STR_TOOL_GetTagType(wk->sp) == TAGTYPE_STREAM_CTRL )
				{
					ctrlStreamTag( wk );
					break;
				}
				/* fallthru */
			default:
				wk->sp = print_next_char( &wk->printJob, wk->sp );

				if( wk->callback_func )
				{
					wk->callback_func( wk->arg );
					wk->arg = wk->current_arg;
				}

				if( *(wk->sp) == EOM_CODE )
				{
					wk->state = PRINTSTREAM_STATE_DONE;
				}
				else
				{
					wk->wait = wk->current_wait;
				}

				GFL_BMPWIN_TransVramCharacter( wk->dstWin );
				break;
			}
		}
		else
		{
			wk->wait--;
		}
		break;

	case PRINTSTREAM_STATE_PAUSE:
		if( wk->pauseReleaseFlag )
		{
			switch( wk->pauseType ){
			case PRINTSTREAM_PAUSE_LINEFEED:	// @@@ ���݃��C���t�B�[�h�����͖�����
				{
					u16 size_x, size_y;

					size_x = GFL_BMP_GetSizeX( wk->dstBmp );
					size_y = GFL_BMP_GetSizeY( wk->dstBmp );
					GFL_BMP_Print( wk->dstBmp, wk->dstBmp, 0, 1, 0, 0, size_x, size_y-1, 0xff );
					GFL_BMP_Fill( wk->dstBmp, 0, size_y-1, size_x, 1, wk->clearColor );
					GFL_BMPWIN_TransVramCharacter( wk->dstWin );
					if( wk->pauseWait == LINE_DOT_HEIGHT )
					{
						wk->printJob.write_x = 0;
						wk->printJob.write_y = LINE_DOT_HEIGHT;
						wk->state = PRINTSTREAM_STATE_RUNNING;
					}
					else
					{
						wk->pauseWait++;
					}
				}
				break;

			case PRINTSTREAM_PAUSE_CLEAR:
			default:
				GFL_BMP_Clear( wk->dstBmp, wk->clearColor );
				wk->state = PRINTSTREAM_STATE_RUNNING;
				break;
			}
		}
		break;
	}
}

//--------------------------------------------------------------------------
/**
 * �v�����g�X�g���[���p�R���g���[���^�O����
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------
static void ctrlStreamTag( PRINT_STREAM* wk )
{
	BOOL skipCR = FALSE;	// ����̉��s�𖳎�����

	switch( STR_TOOL_GetTagNumber(wk->sp) ){
	case CTRL_STREAM_LINE_FEED:
		wk->state = PRINTSTREAM_STATE_PAUSE;
		wk->pauseType = PRINTSTREAM_PAUSE_LINEFEED;
		wk->pauseWait = 0;
		wk->pauseReleaseFlag = FALSE;
		wk->printJob.write_x = 0;
		wk->printJob.write_y = 0;
		skipCR = TRUE;
		break;
	case CTRL_STREAM_PAGE_CLEAR:
		wk->state = PRINTSTREAM_STATE_PAUSE;
		wk->pauseType = PRINTSTREAM_PAUSE_CLEAR;
		wk->pauseWait = 0;
		wk->pauseReleaseFlag = FALSE;
		wk->printJob.write_x = 0;
		wk->printJob.write_y = 0;
		skipCR = TRUE;
		break;
	case CTRL_STREAM_CHANGE_WAIT:
		wk->wait = STR_TOOL_GetTagParam( wk->sp, 0 );
		break;
	case CTRL_STREAM_SET_WAIT:
		wk->current_wait = STR_TOOL_GetTagParam( wk->sp, 0 );;
		break;
	case CTRL_STREAM_RESET_WAIT:	///< �`��E�F�C�g���f�t�H���g�ɖ߂�
		wk->current_wait = wk->org_wait;
		break;
	case CTRL_STREAM_CHANGE_ARGV:	///< �R�[���o�b�N������ύX�i�P��j
		wk->arg = STR_TOOL_GetTagParam( wk->sp, 0 );;
		break;
	case CTRL_STREAM_SET_ARGV:		///< �R�[���o�b�N������ύX�i�i���j
		wk->current_arg = STR_TOOL_GetTagParam( wk->sp, 0 );
		wk->arg = STR_TOOL_GetTagParam( wk->sp, 0 );
		break;
	case CTRL_STREAM_RESET_ARGV:	///< �R�[���o�b�N�������f�t�H���g�ɖ߂�
		wk->current_arg = wk->org_arg;
		wk->arg = wk->org_arg;
		break;
	case CTRL_STREAM_FORCE_CLEAR:
		GFL_BMP_Clear( wk->dstBmp, wk->clearColor );
		break;
	}

	wk->sp = STR_TOOL_SkipTag( wk->sp );

	if( skipCR && *(wk->sp) == CR_CODE )
	{
		wk->sp++;
	}

}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * ������P�s���̃h�b�g���v�Z
 *
 * @param   sp			������|�C���^
 * @param   font		�t�H���g�^�C�v
 * @param   margin		���ԃX�y�[�X�i�h�b�g�j
 * @param   endPtr		[out] ���s�̐擪�|�C���^���i�[����|�C���^�A�h���X�iNULL�Ȃ疳������j
 *
 * @retval  u32		�h�b�g��
 */
//--------------------------------------------------------------------------
static u32 get_line_width( const STRCODE* sp, GFL_FONT* font, u16 margin, const STRCODE** endPtr )
{
	u32 width = 0;

	while( *sp != EOM_CODE && *sp != CR_CODE )
	{
		if( *sp != SPCODE_TAG_START_ )
		{
			width += ( GFL_FONT_GetWidth( font, *sp ) + margin );
			sp++;
		}
		else
		{
			sp = STR_TOOL_SkipTag( sp );
		}
	}

	if( endPtr )
	{
		if( *sp == CR_CODE ){ sp++; }
		*endPtr = sp;
	}

	return width;
}


//==============================================================================================
/**
 * �������Bitmap�\������ۂ̕��i�h�b�g���j���v�Z
 * �������񂪕����s����ꍇ�A���̍Œ��s�̃h�b�g��
 *
 * @param   str			������
 * @param   font		�t�H���g�^�C�v
 * @param   margin		���ԃX�y�[�X�i�h�b�g�j
 *
 * @retval  u32			�h�b�g��
 */
//==============================================================================================
u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin )
{
	u32 width, max;
	const STRCODE* sp;

	width = max = 0;
	sp = GFL_STR_GetStringCodePointer( str );

	while( *sp != EOM_CODE )
	{
		width = get_line_width( sp, font, margin, &sp );
		if( width > max )
		{
			max = width;
		}
	}

	return max;
}


//=============================================================================================
/**
 * �^�O�J�n�R�[�h���擾
 *
 * @retval  STRCODE		
 */
//=============================================================================================
STRCODE PRINTSYS_GetTagStartCode( void )
{
	return SPCODE_TAG_START_;
}
//=============================================================================================
/**
 * �^�O�R�[�h���P��Z�b�g�p�^�O�R�[�h���ǂ�������
 *
 * @param   sp		������|�C���^�i�^�O�J�n�R�[�h���|�C���g���Ă��邱�Ɓj
 *
 * @retval  BOOL		�P��Z�b�g�p�^�O�R�[�h�Ȃ�TRUE
 */
//=============================================================================================
BOOL PRINTSYS_IsWordSetTagType( const STRCODE* sp )
{
	u16 type = STR_TOOL_GetTagType( sp );
	return (type == TAGTYPE_WORD) || (type==TAGTYPE_NUMBER);
}

//=============================================================================================
/**
 * �^�O�p�����[�^�擾
 *
 * @param   sp		
 * @param   paramIdx		
 *
 * @retval  u16		
 */
//=============================================================================================
u16 PRINTSYS_GetTagParam( const STRCODE* sp, u16 paramIdx )
{
	return STR_TOOL_GetTagParam( sp, paramIdx );
}

//=============================================================================================
/**
 * �^�O�������X�L�b�v
 *
 * @param   sp		
 *
 * @retval  const STRCODE*		
 */
//=============================================================================================
const STRCODE* PRINTSYS_SkipTag( const STRCODE* sp )
{
	return STR_TOOL_SkipTag( sp );
}






//============================================================================================
/**
 * ������|�C���^�̎w���Ă���^�O�R�[�h�E�p�����[�^������A�^�O�^�C�v���擾
 *
 * @param   sp		������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u16		�^�O���
 */
//============================================================================================
static inline u16 STR_TOOL_GetTagType( const STRCODE* sp )
{
	GF_ASSERT( *sp == SPCODE_TAG_START_ );

	if( *sp == SPCODE_TAG_START_ )
	{
		sp++;
		return (((*sp)>>8)&0xff);
	}

	return 0xffff;
}
//============================================================================================
/**
 * ������|�C���^�̂����Ă���^�O�R�[�h�E�p�����[�^������A�p�����[�^�l���擾
 *
 * @param   sp				������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 * @param   paramIndex		�p�����[�^�C���f�b�N�X
 *
 * @retval  u16				�p�����[�^�l
 */
//============================================================================================
static inline u16 STR_TOOL_GetTagParam( const STRCODE* sp, u16 paramIndex )
{
	GF_ASSERT( *sp == SPCODE_TAG_START_ );

	if( *sp == SPCODE_TAG_START_ )
	{
		u32 params;
		sp += 2;
		params = *sp++;
		GF_ASSERT(paramIndex<params);
		return sp[paramIndex];
	}
	return 0;
}
//============================================================================================
/**
 * ������|�C���^�̃^�O�R�[�h�E�p�����[�^�������X�L�b�v�����|�C���^��Ԃ�
 *
 * @param   sp		������|�C���^�i�^�O�R�[�h�J�n�l���w���Ă��邱�Ɓj
 *
 * @retval  const STRCODE*		�X�L�b�v��̃|�C���^
 */
//============================================================================================
static inline const STRCODE* STR_TOOL_SkipTag( const STRCODE* sp )
{
	GF_ASSERT( *sp == SPCODE_TAG_START_ );

	if( *sp == SPCODE_TAG_START_ )
	{
		u32 params;
		sp += 2;
		params = *sp++;
		sp += params;
		return sp;
	}
	return sp;
}
//============================================================================================
/**
 * ������|�C���^�̎w���Ă���^�O�R�[�h�E�p�����[�^������A�^�O�ԍ����擾
 *
 * @param   sp		������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u16		�^�O���
 */
//============================================================================================
static inline u16 STR_TOOL_GetTagNumber( const STRCODE* sp )
{
	GF_ASSERT( *sp == SPCODE_TAG_START_ );

	if( *sp == SPCODE_TAG_START_ )
	{
		sp++;
		return ((*sp)&0xff);
	}

	return 0xffff;
}


//==============================================================================================================
// �v�����g�L���[�֘A����
//==============================================================================================================

//--------------------------------------------------------------------------
/**
 * ������i�[�ɕK�v�ȃo�b�t�@�T�C�Y(byte)���v�Z�B��4�̔{���ɑ�����
 *
 * @param   str		[in] ������i�[�o�b�t�@
 *
 * @retval  u32		�o�b�t�@�T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_CalcStringBufferSize( const STRBUF* str )
{
	u32 size = sizeof(STRCODE) * (GFL_STR_GetBufferLength(str) + 1);
	while( size % 4 ){ size++; }
	return size;
}
//--------------------------------------------------------------------------
/**
 * �P���̏������ݏ��� �v�����g�L���[�ɕۑ����邽�߂ɕK�v�ȃo�b�t�@�T�C�Y�ibyte�j���v�Z
 *
 * @param   buf		[in] �������ޕ����񂪊i�[����Ă��镶����o�b�t�@
 *
 * @retval  u32		�v�����g�L���[���K�v�Ƃ���o�b�t�@�T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_CalcUseBufSize( const STRBUF* buf )
{
	return sizeof(PRINT_JOB) + sizeof(PRINT_QUE*) + Que_CalcStringBufferSize(buf);
}

//--------------------------------------------------------------------------
/**
 * �v�����g�L���[ �̃o�b�t�@�󂫃T�C�Y���擾
 *
 * @param   que		[in] �v�����g�L���[
 *
 * @retval  u32		�󂫃T�C�Y�ibyte�j
 */
//--------------------------------------------------------------------------
static u32 Que_GetFreeSize( const PRINT_QUE* que )
{
	if( que->bufTopPos < que->bufEndPos )
	{
		return que->bufEndPos - que->bufTopPos;
	}
	else
	{
		return (que->bufSize - que->bufTopPos) + que->bufEndPos;
	}
}

//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�ɐV�K�`�揈����ǉ�
 *
 * @param   que			[out] �v�����g�L���[
 * @param   printJob	[in] ���e�ݒ�ς݂̕`�揈�����[�N
 * @param   str			[in] �`�敶����o�b�t�@
 *
 */
//--------------------------------------------------------------------------
static void Que_StoreNewJob( PRINT_QUE* que, PRINT_JOB* printJob, const STRBUF* str )
{
	PRINT_JOB* store_adrs;
	const STRCODE* sp;

	sp = Que_WriteDataUnit(	que,
						(const u8*)(GFL_STR_GetStringCodePointer(str)),
						Que_CalcStringBufferSize(str),
						sizeof(STRCODE) );

	store_adrs = Que_WriteDataUnit( que, (const u8*)(printJob), sizeof(PRINT_JOB), sizeof(PRINT_JOB) );


	// ���ɏ��������Ɠ��e�ւ̃|�C���^�i�V�K�ǉ��Ȃ玟�͖����̂�NULL�ɂ��Ă���
	{
		PRINT_JOB* p = NULL;
		Que_WriteDataUnit( que, (const u8*)(&p), sizeof(PRINT_JOB*), sizeof(PRINT_JOB*) );
	}

	if( que->runningJob != NULL )
	{
		PRINT_JOB *job, *next;
		u16 pos;

		job = que->runningJob;
		while(1)
		{
			next = Que_ReadNextJobAdrs( que, job );
			if( next == NULL )
			{
				break;
			}
			job = next;
		}
		pos = Que_AdrsToBufPos( que, job );
		pos = Que_FwdBufPos( que, pos, sizeof(PRINT_JOB) );
		Que_WriteData( que, (const u8*)(&store_adrs), pos, sizeof(store_adrs) );
	}
	else
	{
		que->runningJob = store_adrs;
		que->sp = sp;
	}
}
//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�̃����O�o�b�t�@�Ƀf�[�^���������݁A
 * ����ɉ����ē����̏������݃|�C���^���ړ�������B
 *
 * @param   que				[io] �v�����g�L���[ �i�����������݃|�C���^���ړ�����j
 * @param   src				[in] �������ރf�[�^�|�C���^
 * @param   totalSize		[in] �������݃f�[�^���T�C�Y
 * @param   unitSize		[in] �A�h���X���f�����ɏ������ޒP��
 *
 * @retval  void*			�i�[�擪�A�h���X
 */
//--------------------------------------------------------------------------
static void* Que_WriteDataUnit( PRINT_QUE* que, const u8* src, u16 totalSize, u16 unitSize )
{
	u16 wroteSize = 0;
	void* topAdrs = &(que->buf[ que->bufTopPos ]);

	GF_ASSERT((que->bufTopPos%4)==0);

	while( wroteSize < totalSize )
	{
		que->bufTopPos = Que_WriteData( que, src, que->bufTopPos, unitSize );
		src += unitSize;
		wroteSize += unitSize;
	}

	GF_ASSERT((que->bufTopPos%4)==0);

	return topAdrs;
}
//--------------------------------------------------------------------------
/**
 * �w�肳�ꂽJob�̎���Job�A�h���X��Ԃ�
 *
 * @param   que		[in] �v�����g�L���[
 * @param   job		[in] �v�����g�L���[�̃o�b�t�@�Ɋi�[����Ă��� Job �A�h���X
 *
 * @retval  PRINT_JOB		����Job�A�h���X�i�������NULL�j
 */
//--------------------------------------------------------------------------
static PRINT_JOB* Que_ReadNextJobAdrs( const PRINT_QUE* que, const PRINT_JOB* job )
{
	u32 bufPos = Que_AdrsToBufPos( que, job );

	bufPos = Que_FwdBufPos( que, bufPos, sizeof(PRINT_JOB) );

	{
		PRINT_JOB** ppjob = (PRINT_JOB**)Que_BufPosToAdrs( que, bufPos );
		return *ppjob;
	}
}


//--------------------------------------------------------------------------
/**
 * �v�����g�L���[�̃����O�o�b�t�@�Ƀf�[�^����������
 *
 * @param   que		[out] �v�����g�L���[
 * @param   src		[in]  �������݃f�[�^�|�C���^
 * @param   pos		[in]  �o�b�t�@�������݃|�C���^
 * @param   size	[in]  �������݃f�[�^�T�C�Y
 *
 * @retval  u16		���ɏ������ނƂ��̃o�b�t�@�������݃|�C���^
 */
//--------------------------------------------------------------------------
static inline u16 Que_WriteData( PRINT_QUE* que, const u8* src, u16 pos, u16 size )
{
	GFL_STD_MemCopy( src, &(que->buf[pos]), size );
	pos += size;
	if( pos >= que->bufSize )
	{
		pos = 0;
	}
	return pos;
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@���̔C�ӂ̃A�h���X���A�������݃|�C���^�ɕϊ�
 *
 * @param   que				[in] �v�����g�L���[
 * @param   bufAdrs			[in] �����O�o�b�t�@���̔C�ӂ̃A�h���X
 *
 * @retval  u16				�������݃|�C���^
 */
//--------------------------------------------------------------------------
static inline u16 Que_AdrsToBufPos( const PRINT_QUE* que, const void* bufAdrs )
{
	u32 pos = ((u32)bufAdrs) - ((u32)que->buf);
	GF_ASSERT(pos < que->bufSize);
	return pos;
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@�̏������݃|�C���^���A�h���X�ɕϊ�
 *
 * @param   que				[in] �v�����g�L���[
 * @param   bufPos			[in] �����O�o�b�t�@�������݃|�C���^
 *
 * @retval  void*			�A�h���X
 */
//--------------------------------------------------------------------------
static inline const void* Que_BufPosToAdrs( const PRINT_QUE* que, u16 bufPos )
{
	return &(que->buf[bufPos]);
}
//--------------------------------------------------------------------------
/**
 * �����O�o�b�t�@�������݃|�C���^���A�w��T�C�Y���i�߂��l�ɕϊ�
 *
 * @param   que			�v�����g�L���[
 * @param   pos			�������݃|�C���^�����l
 * @param   fwdSize	 	�i�߂�T�C�Y�i�o�C�g�P�ʁj
 *
 * @retval  pos �� fwdSize �������i�߂��l
 */
//--------------------------------------------------------------------------
static inline u16 Que_FwdBufPos( const PRINT_QUE* que, u16 pos, u16 fwdSize )
{
	pos += fwdSize;
	if( pos >= que->bufSize )
	{
		pos = 0;
	}
	return pos;
}

