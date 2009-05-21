//============================================================================================
/**
 * @file	talkmsgwin.h
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
/*
 * �V�X�e���g�p��̒��ӁF
 *	�EGFL_BMPWIN�V�X�e�����g�p����̂Ŏ��O�ɏ��������Ă����K�v����
 */
#pragma once

typedef struct _TALKMSGWIN_SYS			TALKMSGWIN_SYS;

#define TALKMSGWIN_NUM	(32)
#define TALKMSGWIN_NULL (-1)

typedef struct {
	int			frameID;
	u8			winPltID;
	u8			fontPltID;
}TALKMSGWIN_SYS_INI;

typedef struct {
	HEAPID							heapID;
	GFL_G3D_CAMERA*			g3Dcamera;
	GFL_FONT*						fontHandle;

	TALKMSGWIN_SYS_INI	ini;

}TALKMSGWIN_SYS_SETUP;

typedef struct {
	u8				winpx;			
	u8				winpy;			
	u8				winsx;			
	u8				winsy;			
	GXRgb			color;
}TALKMSGWIN_SETUP;

//============================================================================================
/**
 *
 * @brief	��b�E�C���h�E�V�X�e���֐�
 *
 */
//============================================================================================
extern TALKMSGWIN_SYS*	TALKMSGWIN_SystemCreate( TALKMSGWIN_SYS_SETUP* setup );
extern void							TALKMSGWIN_SystemMain( TALKMSGWIN_SYS* tmsgwinSys );
extern void							TALKMSGWIN_SystemDraw2D( TALKMSGWIN_SYS* tmsgwinSys );
extern void							TALKMSGWIN_SystemDraw3D( TALKMSGWIN_SYS* tmsgwinSys );
extern void							TALKMSGWIN_SystemDelete( TALKMSGWIN_SYS* tmsgwinSys );

//============================================================================================
/**
 *
 * @brief	��b�E�C���h�E�ݒ�֐�
 *
 */
//============================================================================================
#if 0
extern int TALKMSGWIN_CreateWindow( TALKMSGWIN_SYS*		tmsgwinSys, 
																		VecFx32*					pTarget,
																		STRBUF*						msg,
																		TALKMSGWIN_SETUP* tmsgwinSetup );
#endif
extern void TALKMSGWIN_CreateWindowIdx( TALKMSGWIN_SYS*		tmsgwinSys, 
																				int								tmsgwinIdx, 
																				VecFx32*					pTarget,
																				STRBUF*						msg,
																				TALKMSGWIN_SETUP* tmsgwinSetup );
extern void TALKMSGWIN_CreateWindowIdxConnect(	TALKMSGWIN_SYS*		tmsgwinSys, 
																								int								tmsgwinIdx,
																								int								prev_tmsgwinIdx,
																								STRBUF*						msg,
																								TALKMSGWIN_SETUP* tmsgwinSetup );

extern void TALKMSGWIN_DeleteWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx );
extern void TALKMSGWIN_OpenWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx );
extern void TALKMSGWIN_CloseWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx );

