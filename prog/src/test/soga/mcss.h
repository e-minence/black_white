
//============================================================================================
/**
 * @file	mcss.h
 * @brief	�}���`�Z���\�t�g�E�G�A�X�v���C�g�`��p�֐�
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#include <gflib.h>

#ifndef	__MCSS_H_
#define	__MCSS_H_

//--------------------------------------------------------------------------
/**
 * �萔��`
 */
//--------------------------------------------------------------------------

#define	MCSS_MEPACHI_ON		( 1 )		//���p�`ON
#define	MCSS_MEPACHI_OFF	( 0 )		//���p�`OFF
#define	MCSS_ANM_STOP_ON	( 1 )		//�A�j���X�g�b�vON
#define	MCSS_ANM_STOP_OFF	( 0 )		//�A�j���X�g�b�vOFF
#define	MCSS_VANISH_ON		( 1 )		//�o�j�b�V��ON
#define	MCSS_VANISH_OFF		( 0 )		//�o�j�b�V��OFF

//--------------------------------------------------------------------------
/**
 * �\���̒�`
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

extern	MCSS_SYS_WORK*	MCSS_Init( int max, GFL_G3D_CAMERA *camera, HEAPID heapID );
extern	void			MCSS_Exit( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_Main( MCSS_SYS_WORK *mcss_sys );
extern	void			MCSS_Draw( MCSS_SYS_WORK *mcss_sys );
extern	MCSS_WORK*		MCSS_Add( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, MCSS_ADD_WORK *maw );
extern	void			MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );
extern	void			MCSS_GetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void			MCSS_SetPosition( MCSS_WORK *mcss, VecFx32 *pos );
extern	void			MCSS_GetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void			MCSS_SetScale( MCSS_WORK *mcss, VecFx32 *scale );
extern	void			MCSS_SetMepachiFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetMepachiFlag( MCSS_WORK *mcss );
extern	void			MCSS_SetAnmStopFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetAnmStopFlag( MCSS_WORK *mcss );
extern	void			MCSS_SetVanishFlag( MCSS_WORK *mcss );
extern	void			MCSS_ResetVanishFlag( MCSS_WORK *mcss );

#endif	//__MCSS_H_
