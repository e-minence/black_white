//=============================================================================================
/**
 * @file	printsys.c
 * @brief	Bitmap���p�����`��@�V�X�e������
 * @author	taya
 * @date	2007.02.06
 */
//=============================================================================================
#include "gflib.h"
#include "tcbl.h"

#include "system/main.h"


#include "printsys.h"



enum {
	STR_EXPAND_MAXLEN = 500,		///< �����f�[�^�W�J�p�̈ꎞ�o�b�t�@�T�C�Y
	LINE_DOT_HEIGHT = 16,

	// �ėp�R���g���[���^�C�v
	CTRL_GENERAL_COLOR			= (0x0000),	///< �F�ύX
	CTRL_GENERAL_RESET_COLOR	= (0x0001),	///< �F�ύX
	CTRL_GENERAL_XPOS			= (0x0002),	///< �`��w���W
	CTRL_GENERAL_YPOS			= (0x0003),	///< �`��x���W
	CTRL_GENERAL_X_CENTERING	= (0x0004),	///< �w���W�Z���^�����O

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

	EOM_CODE			= 0xffff,
	CR_CODE				= 0xfffe,
	SPCODE_TAG_START_	= 0xf000,

	TAGTYPE_WORD = 0,				///< ������}��
	TAGTYPE_NUMBER = 1,				///< ���l�}��
	TAGTYPE_PARTY = 2,				///< �p�[�e�B�p������
	TAGTYPE_GENERAL_CTRL = 0xbd,	///< �ėp�R���g���[������
	TAGTYPE_STREAM_CTRL = 0xbe,		///< ����郁�b�Z�[�W�p�R���g���[������
	TAGTYPE_SYSTEM = 0xff,			///< �G�f�B�^�V�X�e���^�O�p�R���g���[������

};

typedef u16		STRCODE;


typedef struct {
	u16   org_x;
	u16   org_y;
	u16   write_x;
	u16   write_y;

	u8    colorLetter;
	u8    colorShadow;
	u8    colorBackGround;

	GFL_FONT*	fontHandle;

	GFL_BMP_DATA*	dst;

}PRINT_WORK;



struct _PRINT_STREAM_WORK {

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

	PRINT_WORK	printWork;

	

};

typedef struct _PRINT_STREAM_WORK	PRINT_STREAM_WORK;





//======================================================================

static struct {
	GFL_BMP_DATA*		charBuffer;
	PRINT_WORK			printWork;
	STRCODE				expandBuffer[ STR_EXPAND_MAXLEN + 1 ];
}SystemWork;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void initPrintWork( PRINT_WORK* wk, GFL_FONT* font, GFL_BMP_DATA* dst, u16 org_x, u16 org_y );
static const STRCODE* print_next_char( PRINT_WORK* wk, const STRCODE* sp, GFL_BMP_DATA* dst );
static void put_1char( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, GFL_FONT* fontHandle, u32 charCode, u16* charWidth, u16* charHeight );
static const STRCODE* ctrlGeneralTag( PRINT_WORK* wk, const STRCODE* sp );
static const STRCODE* ctrlSystemTag( PRINT_WORK* wk, const STRCODE* sp );
static void print_stream_task( GFL_TCBL* tcb, void* wk_adrs );
static void ctrlStreamTag( PRINT_STREAM_WORK* wk );
static u32 get_line_width( const STRCODE* sp, GFL_FONT* font, u16 margin, const STRCODE** endPtr );
static inline u16 STR_TOOL_GetTagType( const STRCODE* sp );
static inline u16 STR_TOOL_GetTagParam( const STRCODE* sp, u16 paramIndex );
static inline const STRCODE* STR_TOOL_SkipTag( const STRCODE* sp );
static inline u16 STR_TOOL_GetTagNumber( const STRCODE* sp );





//==============================================================================================
/**
 * �V�X�e���������iGFLIB��������ɂP�x�����Ăԁj
 */
//==============================================================================================
void PRINTSYS_Init( HEAPID heapID )
{
	GFL_STR_SetEOMCode( EOM_CODE );
//	GFL_FONTSYS_Init();

	SystemWork.charBuffer = GFL_BMP_Create( 2, 2, GFL_BMP_16_COLOR, heapID );
}

//==============================================================================================
/**
 * BITMAP�ɑ΂��镶����`��i�ꊇ�j
 *
 * @param   dst		[out] �`���Bitmap
 * @param   xpos	[in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos	[in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str		[in]  ������
 * @param   font	[in]  �t�H���g�^�C�v
 *
 */
//==============================================================================================
void PRINTSYS_Print( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font )
{
	const STRCODE* sp;
	PRINT_WORK* wk = &SystemWork.printWork;

	initPrintWork( wk, font, dst, xpos, ypos );
	sp = GFL_STR_GetStringCodePointer( str );

	while( *sp != EOM_CODE )
	{
		sp = print_next_char( wk, sp, dst );
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
static void initPrintWork( PRINT_WORK* wk, GFL_FONT* font, GFL_BMP_DATA* dst, u16 org_x, u16 org_y )
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
 * @param   dst		
 * @param   font		
 *
 * @retval  const STRCODE*		
 */
//------------------------------------------------------------------
static const STRCODE* print_next_char( PRINT_WORK* wk, const STRCODE* sp, GFL_BMP_DATA* dst )
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

				put_1char( dst, wk->write_x, wk->write_y, wk->fontHandle, *sp, &w, &h );
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
static void put_1char( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, GFL_FONT* fontHandle, u32 charCode, u16* charWidth, u16* charHeight )
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
static const STRCODE* ctrlGeneralTag( PRINT_WORK* wk, const STRCODE* sp )
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

	case CTRL_GENERAL_XPOS:
		wk->write_x = STR_TOOL_GetTagParam( sp, 0 );
		break;

	case CTRL_GENERAL_YPOS:
		wk->write_y = STR_TOOL_GetTagParam( sp, 0 );;
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
static const STRCODE* ctrlSystemTag( PRINT_WORK* wk, const STRCODE* sp )
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




//==============================================================================================
/**
 * �v�����g�X�g���[���iBITMAP����镶����`��j�J�n
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
 * @param   callback	�P�����`�悲�Ƃ̃R�[���o�b�N�֐��A�h���X�i�s�v�Ȃ�NULL�j
 *
 * @retval	PRINT_STREAM_HANDLE	�X�g���[���n���h��
 *
 */
//==============================================================================================
PRINT_STREAM_HANDLE PRINTSYS_PrintStream(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, u16 heapID, u16 clearColor, pPrintCallBack callback )
{
	PRINT_STREAM_WORK* stwk;
	GFL_TCBL* tcb;
	GFL_BMP_DATA* dstBmp;

	dstBmp = GFL_BMPWIN_GetBmp( dst );;
	tcb = GFL_TCBL_Create( tcbsys, print_stream_task, sizeof(PRINT_STREAM_WORK), tcbpri );
	stwk = GFL_TCBL_GetWork( tcb );
	stwk->tcb = tcb;

	initPrintWork( &stwk->printWork, font, dstBmp, xpos, ypos );

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
PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM_HANDLE handle )
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
PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM_HANDLE handle )
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
void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM_HANDLE handle )
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
void PRINTSYS_PrintStreamDelete( PRINT_STREAM_HANDLE handle )
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
void PRINTSYS_PrintStreamShortWait( PRINT_STREAM_HANDLE handle, u16 wait )
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
	PRINT_STREAM_WORK* wk = wk_adrs;

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
				wk->sp = print_next_char( &wk->printWork, wk->sp, wk->dstBmp );

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
						wk->printWork.write_x = 0;
						wk->printWork.write_y = LINE_DOT_HEIGHT;
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
static void ctrlStreamTag( PRINT_STREAM_WORK* wk )
{
	BOOL skipCR = FALSE;	// ����̉��s�𖳎�����

	switch( STR_TOOL_GetTagNumber(wk->sp) ){
	case CTRL_STREAM_LINE_FEED:
		wk->state = PRINTSTREAM_STATE_PAUSE;
		wk->pauseType = PRINTSTREAM_PAUSE_LINEFEED;
		wk->pauseWait = 0;
		wk->pauseReleaseFlag = FALSE;
		wk->printWork.write_x = 0;
		wk->printWork.write_y = 0;
		skipCR = TRUE;
		break;
	case CTRL_STREAM_PAGE_CLEAR:
		wk->state = PRINTSTREAM_STATE_PAUSE;
		wk->pauseType = PRINTSTREAM_PAUSE_CLEAR;
		wk->pauseWait = 0;
		wk->pauseReleaseFlag = FALSE;
		wk->printWork.write_x = 0;
		wk->printWork.write_y = 0;
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


