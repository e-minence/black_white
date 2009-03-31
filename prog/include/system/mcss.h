
//============================================================================================
/**
 * @file	mcss.h
 * @brief	マルチセルソフトウエアスプライト描画用関数
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#ifndef __ASM_NO_DEF_

#include <gflib.h>
#include "system/gfl_use.h"

#endif

#ifndef	__MCSS_H_
#define	__MCSS_H_

//--------------------------------------------------------------------------
/**
 * 定数定義
 */
//--------------------------------------------------------------------------

#define	MCSS_MEPACHI_ON		( 1 )		//メパチON
#define	MCSS_MEPACHI_OFF	( 0 )		//メパチOFF
#define	MCSS_MEPACHI_FLIP	( 2 )		//メパチFLIP
#define	MCSS_ANM_STOP_ON	( 1 )		//アニメストップON
#define	MCSS_ANM_STOP_OFF	( 0 )		//アニメストップOFF
#define	MCSS_VANISH_ON		( 1 )		//バニッシュON
#define	MCSS_VANISH_OFF		( 0 )		//バニッシュOFF

#ifndef __ASM_NO_DEF_
//--------------------------------------------------------------------------
/**
 * 構造体定義
 */
//--------------------------------------------------------------------------

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

typedef struct _MCSS_NCEC		MCSS_NCEC;
typedef struct _MCSS_NCEC_WORK	MCSS_NCEC_WORK;
typedef struct _MCSS_WORK		MCSS_WORK;
typedef struct _MCSS_SYS_WORK	MCSS_SYS_WORK;

extern	MCSS_SYS_WORK*	MCSS_Init( int max, HEAPID heapID );
extern	void			MCSS_Exit( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_Main( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_Draw( MCSS_SYS_WORK *mcss_sys );
extern	MCSS_WORK*		MCSS_Add( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, MCSS_ADD_WORK *maw );
extern	void			MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );
extern	void			MCSS_SetOrthoMode( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_ResetOrthoMode( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_GetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void			MCSS_SetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void			MCSS_GetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void			MCSS_SetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void			MCSS_GetRotate( MCSS_WORK *mcss, VecFx32 *rotate );
extern	void			MCSS_SetRotate( MCSS_WORK *mcss, VecFx32 *rotate );
extern	void			MCSS_SetShadowScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void			MCSS_SetMepachiFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetMepachiFlag( MCSS_WORK *mcss );
extern	void			MCSS_FlipMepachiFlag( MCSS_WORK *mcss );
extern	void			MCSS_SetAnmStopFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetAnmStopFlag( MCSS_WORK *mcss );
extern	int				MCSS_GetVanishFlag( MCSS_WORK *mcss );
extern	void			MCSS_SetVanishFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetVanishFlag( MCSS_WORK *mcss );

#endif

#endif	//__MCSS_H_
