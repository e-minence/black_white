//============================================================================================
/**
 * @file	text_cont.c
 * @brief	�e�L�X�g����
 */
//============================================================================================
#include "gflib.h"
#include "textprint.h"

#include "setup.h"
#include "player_cont.h"
#include "text_cont.h"

static char*	SetNumSpc( char* dst, u16 num, BOOL* f );
static char*	Copy2Dtext( const char* src, char* dst );
static char*	Make2Dtext_SetNumber_u16( char* dst, u16 value );
//============================================================================================
//
//
//	�X�e�[�^�X�E�C���h�E�R���g���[��
//
//
//============================================================================================
//bmp, writex, writey, spacex, spacey, colorF, colorB, mode
static const GFL_TEXT_PRINTPARAM playerStatusWinPrintParam = {
	NULL, 0, 0, 1, 1, 15, 1, GFL_TEXT_WRITE_16
};

static const char	player1[]	= {"Player1"};
static const char	hpMsg[]	= {"HP"};
static const char	srushMsg[]	= {"/"};
static void	Make2Dtext_HP( char* text, u16 nowHP, u16 maxHP );
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _STATUSWIN_CONTROL {
	HEAPID				heapID;
	PLAYER_STATUS*		targetStatus;
	GFL_BMPWIN*			targetBmpwin;
	BOOL				reloadFlag;
};

//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�E�C���h�E�R���g���[���N��
 */
//------------------------------------------------------------------
STATUSWIN_CONTROL* AddStatusWinControl
					( PLAYER_STATUS* targetStatus, GFL_BMPWIN* targetBmpwin, HEAPID heapID )
{
	STATUSWIN_CONTROL* swc = GFL_HEAP_AllocClearMemory( heapID, sizeof(STATUSWIN_CONTROL) );
	swc->heapID = heapID;
	swc->targetStatus = targetStatus;
	swc->targetBmpwin = targetBmpwin;
	swc->reloadFlag = TRUE;

	return swc;
}

//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�E�C���h�E�R���g���[���I��
 */
//------------------------------------------------------------------
void RemoveStatusWinControl( STATUSWIN_CONTROL* swc )
{
	GFL_HEAP_FreeMemory( swc ); 
}

//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�E�C���h�E���C���R���g���[��
 */
//------------------------------------------------------------------
void MainStatusWinControl( STATUSWIN_CONTROL* swc )
{
	if( swc->reloadFlag == TRUE ){
		GFL_BMP_DATA* bmp = GFL_BMPWIN_GetBmp( swc->targetBmpwin );
		GFL_TEXT_PRINTPARAM printParam = { NULL, 1, 1, 1, 1, 15, 1, GFL_TEXT_WRITE_16 };

		//�w�i�h��Ԃ�
		GFL_BMP_Clear( bmp, 1 );

		//�e�L�X�g���r�b�g�}�b�v�ɕ\��
		printParam.bmp = bmp;
		printParam.writex = 1;
		printParam.writey = 1;
		GFL_TEXT_PrintSjisCode( swc->targetStatus->name, &printParam );
		{
			char* text = GFL_HEAP_AllocClearMemory( swc->heapID, 32 );
			Make2Dtext_HP( text, swc->targetStatus->hp, swc->targetStatus->hpMax );
			printParam.writex = 1;
			printParam.writey = 10;
			GFL_TEXT_PrintSjisCode( text, &printParam );
			GFL_HEAP_FreeMemory( text );
		}
		//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
		GFL_BMPWIN_TransVramCharacter( swc->targetBmpwin );
		swc->reloadFlag = FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�����[�h�Z�b�g
 */
//------------------------------------------------------------------
void SetStatusWinReload( STATUSWIN_CONTROL* swc )
{
	swc->reloadFlag = TRUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void	Make2Dtext_HP( char* text, u16 nowHP, u16 maxHP )
{
	text = Copy2Dtext( hpMsg, text );
	text = Make2Dtext_SetNumber_u16( text, nowHP );
	text = Copy2Dtext( srushMsg, text );
	text = Make2Dtext_SetNumber_u16( text, maxHP );
}



//------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W������쐬
 */
//------------------------------------------------------------------
static char*	SetNumSpc( char* dst, u16 num, BOOL* f )
{
	if(( num == 0 )&&( *f == FALSE )){
		*dst = ' ';
		dst++;
	} else {
		*dst = '0' + num;
		*f = TRUE;
		dst++;
	}
	return dst;
}

static char*	Copy2Dtext( const char* src, char* dst )
{
	while( *src != 0 ){
		*dst = *src;
		src++;
		dst++;
	}
	*dst = 0;
	return dst;
}

static char*	Make2Dtext_SetNumber_u16( char* dst, u16 value )
{
	u16		num;
	BOOL	f = FALSE;

	num = value / 10000;
	dst = SetNumSpc( dst, num, &f );
	value %= 10000;

	num = value / 1000;
	dst = SetNumSpc( dst, num, &f );
	value %= 1000;

	num = value / 100;
	dst = SetNumSpc( dst, num, &f );
	value %= 100;

	num = value / 10;
	dst = SetNumSpc( dst, num, &f );
	value %= 10;

	num = value;
	f = TRUE;
	dst = SetNumSpc( dst, num, &f );
	*dst = 0;

	return dst;
}

