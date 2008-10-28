//======================================================================
/**
 *
 * @file	dlplay_func.c
 * @brief	�_�E�����[�h�p�@�\�֐�
 * @author	ariizumi
 * @data	08/10/20
 */
//======================================================================
#include <gflib.h>
#include <textprint.h>

#include "system/gfl_use.h"
#include "system/main.h"

#include "dlplay_func.h"
#include "../ariizumi/ari_debug.h"
//======================================================================
//	define
//======================================================================

#define DLPLAY_FUNC_MSG_X (16)
#define DLPLAY_FUNC_MSG_LINE_NUM (24)
#define DLPLAY_FUNC_MSG_LINE_MAX (20)
//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
struct _DLPLAY_MSG_SYS
{
	u8	line_;
	u8	bgPlane_;

	GFL_BMPWIN			*bmpwin_[DLPLAY_FUNC_MSG_LINE_NUM ];
	GFL_TEXT_PRINTPARAM	*textParam_[DLPLAY_FUNC_MSG_LINE_NUM ];

};


//======================================================================
//	proto
//======================================================================
DLPLAY_MSG_SYS*	DLPlayFunc_MsgInit( int	heapID , u8 bgPlane );
void	DLPlayFunc_MsgTerm( DLPLAY_MSG_SYS *msgSys );

void DLPlayFunc_PutString( char* str , DLPLAY_MSG_SYS *msgSys);
void DLPlayFunc_MsgUpdate( DLPLAY_MSG_SYS *msgSys , const u8 line , const BOOL isRefresh );

const u16 DLPlayFunc_DPTStrCode_To_UTF16( const u16 *dptStr , u16* utfStr , const u16 len );

//======================================================================
//	DL�v���C���b�Z�[�W�V�X�e��������
//======================================================================
DLPLAY_MSG_SYS*	DLPlayFunc_MsgInit( int	heapID , u8 bgPlane )
{
	DLPLAY_MSG_SYS *msgSys;
	u8 i;

	msgSys = GFL_HEAP_AllocClearMemory( heapID , sizeof( DLPLAY_MSG_SYS ) );
	msgSys->bgPlane_ = bgPlane;
	msgSys->line_ = 0;

	//�t�H���g�p���b�g�쐬���]��
	{
		const u16 G2D_BACKGROUND_COL = 0x0000;
		const u16 G2D_FONT_COL = 0x7fff;
		const u16 G2D_FONTSELECT_COL = 0x001f;

		u16* plt = GFL_HEAP_AllocClearMemoryLo( heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		GFL_BG_LoadPalette( bgPlane, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_LoadPalette( bgPlane, plt, 16*2, 16*2 );
	
		GFL_HEAP_FreeMemory( plt );
	}
	//�r�b�g�}�b�v�쐬
	for( i=0;i<DLPLAY_FUNC_MSG_LINE_NUM;i++ ){
		msgSys->bmpwin_[i]	= GFL_BMPWIN_Create( bgPlane , 0, i, 32, 1, 0, 
												GFL_BG_CHRAREA_GET_F );
	}

	{
		GFL_TEXT_PRINTPARAM defaultParam;

		defaultParam.writex	= DLPLAY_FUNC_MSG_X;
		defaultParam.writey	= 0;
		defaultParam.spacex	= 1;
		defaultParam.spacey	= 1;
		defaultParam.colorF	= 1;
		defaultParam.colorB	= 0;
		defaultParam.mode	= GFL_TEXT_WRITE_16;
		for( i=0;i<DLPLAY_FUNC_MSG_LINE_NUM;i++ ){
			//�e�L�X�g���r�b�g�}�b�v�ɕ\��
			msgSys->textParam_[i] = GFL_HEAP_AllocClearMemory( heapID , sizeof( GFL_TEXT_PRINTPARAM )); 
			*msgSys->textParam_[i] = defaultParam;
			msgSys->textParam_[i]->bmp	= GFL_BMPWIN_GetBmp( msgSys->bmpwin_[i] );
			
			//DLPlayFunc_MsgUpdate( msgSys , i , (i==DLPLAY_FUNC_MSG_LINE_NUM-1?TRUE:FALSE));
		}
	}


	return msgSys;
}

//======================================================================
//	DL�v���C���b�Z�[�W�V�X�e���J��
//======================================================================
void	DLPlayFunc_MsgTerm( DLPLAY_MSG_SYS *msgSys )
{
	u8 i;
	for( i=0;i<DLPLAY_FUNC_MSG_LINE_NUM;i++ ){
		GFL_BMPWIN_Delete( msgSys->bmpwin_[i] );
	}
}

//======================================================================
//	DL�v���C���b�Z�[�W�V�X�e���`��

//======================================================================
void DLPlayFunc_PutString( char* str , DLPLAY_MSG_SYS *msgSys)
{
	u8 i;

	ARI_TPrintf("MsgPut:[%s]\n",str);
	//GFL_BMP_Clear( msgSys->textParam_[msgSys->line_]->bmp , 0x0000 );

	msgSys->textParam_[msgSys->line_]->writex	= DLPLAY_FUNC_MSG_X;
	msgSys->textParam_[msgSys->line_]->writey	= 0;
	GFL_TEXT_PrintSjisCode( str , msgSys->textParam_[msgSys->line_] );
	DLPlayFunc_MsgUpdate( msgSys , msgSys->line_ , FALSE );
	
	msgSys->line_++;
	if( msgSys->line_ >= DLPLAY_FUNC_MSG_LINE_MAX )
		msgSys->line_ = 0;
	//���̃��C�����N���A���Ă���:
	//
	GFL_BMP_Clear( msgSys->textParam_[msgSys->line_]->bmp , 0x0000 );
	DLPlayFunc_MsgUpdate( msgSys , msgSys->line_ , TRUE );
	
}

//======================================================================
//	DL�v���C���b�Z�[�W�V�X�e���`��
//======================================================================
void DLPlayFunc_MsgUpdate( DLPLAY_MSG_SYS *msgSys , const u8 line , const BOOL isRefresh )
{
	GFL_BMPWIN_TransVramCharacter( msgSys->bmpwin_[line] );
	
	if( isRefresh == TRUE ){
		GFL_BMPWIN_MakeScreen( msgSys->bmpwin_[line] );
		GFL_BG_LoadScreenReq( msgSys->bgPlane_ );
	}
}

//======================================================================
//	DPT����UTF-16�֕ϊ�
//	@param  [in] DPT�ł̕����R�[�h
//�@@param  [out]UTF-16�`�����i�[����|�C���^
//�@@param  [in] ������
//	@return [out]�ϊ���̕�����
//======================================================================
#include "pt_str_arr.dat"
const u16 DLPlayFunc_DPTStrCode_To_UTF16( const u16 *dptStr , u16* utfStr , const u16 len )
{
	static const u16 EomCode = 0x0000;
	static const u16 UnknownCode = L'?';
	int i,j;
	for( i=0;i<len;i++ )
	{
		if( dptStr[i] == 0xFFFF ){
			utfStr[i] = EomCode;
			break;
		}
		for( j=0;j<PT_STR_ARR_NUM;j++ ){
			if( dptStr[i] == PT_STR_ARR[j][1] ){
				utfStr[i] = PT_STR_ARR[j][0];
				break;
			}
		}
		if( j == PT_STR_ARR_NUM ){
			//�Y�������Ȃ�
			utfStr[i] = UnknownCode;
		}
	}
	return i;
}



