
//============================================================================================
/**
 * @file	mcss.h
 * @brief	�}���`�Z���\�t�g�E�G�A�X�v���C�g�`��p�֐�
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#ifndef __ASM_NO_DEF_

#include <gflib.h>
#include "system/gfl_use.h"

#endif

#pragma once

//--------------------------------------------------------------------------
/**
 * �萔��`
 */
//--------------------------------------------------------------------------

#define	MCSS_MEPACHI_ON		( 1 )		//���p�`ON
#define	MCSS_MEPACHI_OFF	( 0 )		//���p�`OFF
#define	MCSS_MEPACHI_FLIP	( 2 )		//���p�`FLIP
#define	MCSS_ANM_STOP_ON	( 1 )		//�A�j���X�g�b�vON
#define	MCSS_ANM_STOP_OFF	( 0 )		//�A�j���X�g�b�vOFF
#define	MCSS_VANISH_ON		( 1 )		//�o�j�b�V��ON
#define	MCSS_VANISH_OFF		( 0 )		//�o�j�b�V��OFF
#define	MCSS_VANISH_FLIP	( 2 )		//�o�j�b�V��FLIP

#define MCSS_SHADOW_ALPHA_AUTO ( 32 )//�e�̃A���t�@�l��{�̂̔����ɂ��܂�

#define MCSS_CELL_ANIM_NONSTOP ( 0xff )    //�}���`�Z���A�j����1�����Ă��~�߂铮������Ȃ�

#ifndef __ASM_NO_DEF_
//--------------------------------------------------------------------------
/**
 * �\���̒�`
 */
//--------------------------------------------------------------------------
typedef struct _MCSS_NCEC				MCSS_NCEC;
typedef struct _MCSS_NCEC_WORK	MCSS_NCEC_WORK;
typedef struct _MCSS_NCEN_WORK	MCSS_NCEN_WORK;
typedef struct _MCSS_WORK				MCSS_WORK;
typedef struct _MCSS_SYS_WORK		MCSS_SYS_WORK;

typedef struct
{
	ARCID		arcID;
	ARCDATID	ncbr;
	ARCDATID	nclr;
	ARCDATID	ncer;
	ARCDATID	nanr;
	ARCDATID	nmcr;
	ARCDATID	nmar;
	ARCDATID	ncec;
}MCSS_ADD_WORK;

typedef struct	
{
	NNSG2dCharacterData*			pCharData;			//�e�N�X�`���L����
	NNSG2dPaletteData*				pPlttData;			//�e�N�X�`���p���b�g
	NNSG2dImageProxy*					image_p;
	NNSG2dImagePaletteProxy*	palette_p;
	void*											pBufChar;			//�e�N�X�`���L�����o�b�t�@
	void*											pBufPltt;			//�e�N�X�`���p���b�g�o�b�t�@
	u32												chr_ofs;
	u32												pal_ofs;
	MCSS_WORK*								mcss;
}TCB_LOADRESOURCE_WORK;

#ifdef PM_DEBUG
typedef struct
{
	void	*ncbr;
	void	*nclr;
	void	*ncer;
	void	*nanr;
	void	*nmcr;
	void	*nmar;
	void	*ncec;
}MCSS_ADD_DEBUG_WORK;
#endif

typedef BOOL (MCSS_CALLBACK_FUNC)( const MCSS_ADD_WORK*, TCB_LOADRESOURCE_WORK*, void * );

extern	MCSS_SYS_WORK*	MCSS_Init( int max, HEAPID heapID );
extern	void						MCSS_Exit( MCSS_SYS_WORK *mcss_sys );
extern	void						MCSS_Main( MCSS_SYS_WORK *mcss_sys );
extern  void	          MCSS_SetCallBackFunc( MCSS_SYS_WORK *mcss_sys, MCSS_CALLBACK_FUNC* func, void* work );
extern  void	          MCSS_SetCallBackWork( MCSS_SYS_WORK *mcss_sys, void* work );
extern	void						MCSS_Draw( MCSS_SYS_WORK *mcss_sys );
extern	MCSS_WORK*			MCSS_Add( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_WORK *maw );
extern	void						MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );
extern	void						MCSS_SetOrthoMode( MCSS_SYS_WORK *mcss_sys );
extern	void						MCSS_ResetOrthoMode( MCSS_SYS_WORK *mcss_sys );
extern	void						MCSS_GetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void						MCSS_SetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void						MCSS_GetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void						MCSS_SetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void						MCSS_GetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void						MCSS_SetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void						MCSS_GetOfsScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void						MCSS_SetOfsScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void						MCSS_GetRotate( MCSS_WORK *mcss, VecFx32 *rotate );
extern	void						MCSS_SetRotate( MCSS_WORK *mcss, VecFx32 *rotate );
extern	void						MCSS_SetShadowScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void	          MCSS_SetShadowRotate( MCSS_WORK *mcss, const u16 rot );
extern	void						MCSS_GetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs );
extern	void						MCSS_SetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs );
extern	void						MCSS_SetMepachiFlag( MCSS_WORK *mcss );
extern	void						MCSS_ResetMepachiFlag( MCSS_WORK *mcss );
extern	void						MCSS_FlipMepachiFlag( MCSS_WORK *mcss );
extern	void						MCSS_SetAnmStopFlag( MCSS_WORK *mcss );
extern	void						MCSS_ResetAnmStopFlag( MCSS_WORK *mcss );
extern	int							MCSS_GetVanishFlag( MCSS_WORK *mcss );
extern	void						MCSS_SetVanishFlag( MCSS_WORK *mcss );
extern	void						MCSS_ResetVanishFlag( MCSS_WORK *mcss );
extern	void						MCSS_FlipVanishFlag( MCSS_WORK *mcss );
extern  NNSG2dMultiCellAnimation* MCSS_GetAnimCtrl( MCSS_WORK *mcss );
extern  u16             MCSS_GetMCellAnmNum( MCSS_WORK *mcss );
extern  u8              MCSS_GetStopCellAnms( MCSS_WORK *mcss );
extern  u8              MCSS_GetStopNode( MCSS_WORK *mcss, u8 index );
extern	void						MCSS_SetAnimCtrlCallBack( MCSS_WORK *mcss, u32 param, NNSG2dAnmCallBackPtr pFunc, u16 frameIdx );
extern  void	          MCSS_SetTraverseMCNodesCallBack( MCSS_WORK *mcss, u32 param, NNSG2dMCTraverseNodeCallBack pFunc );
extern	void						MCSS_SetPaletteFade( MCSS_WORK *mcss, u8 start_evy, u8 end_evy, u8 wait, u32 rgb );
extern  u8              MCSS_GetAlpha( MCSS_WORK *mcss );
extern  void	          MCSS_SetAlpha( MCSS_WORK *mcss, u8 alpha );
extern  u8              MCSS_GetShadowAlpha( MCSS_WORK *mcss );
extern  void	          MCSS_SetShadowAlpha( MCSS_WORK *mcss, u8 alpha );
extern  u8              MCSS_GetShadowVanishFlag( MCSS_WORK *mcss );
extern  void	          MCSS_SetShadowVanishFlag( MCSS_WORK *mcss, u8 flag );
extern  BOOL            MCSS_CheckExecutePaletteFade( MCSS_WORK*  mcss );
extern  void            MCSS_SetTextureTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs );
extern  void            MCSS_SetTexPaletteTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs );
extern  void	          MCSS_SetAnimeIndex( MCSS_WORK* mcss, int index );

#ifdef PM_DEBUG
extern	MCSS_WORK*			MCSS_AddDebug( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_DEBUG_WORK *madw );
#endif

#endif

