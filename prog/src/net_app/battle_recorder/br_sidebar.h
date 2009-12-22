//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_sidebar.h
 *	@brief  �T�C�h�o�[����
 *	@author	Toru=Nagihashi
 *	@date		2009.12.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>
//�o�g�����R�[�_�[�����W���[��
#include "br_res.h"
#include "br_fade.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�T�C�h�o�[����
//=====================================
typedef struct _BR_SIDEBAR_WORK BR_SIDEBAR_WORK;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//�V�X�e��
extern BR_SIDEBAR_WORK * BR_SIDEBAR_Init( GFL_CLUNIT *p_clunit, BR_FADE_WORK *p_fade, BR_RES_WORK *p_res, HEAPID heapID );
extern void BR_SIDEBAR_Exit( BR_SIDEBAR_WORK *p_wk, BR_RES_WORK *p_res );
extern void BR_SIDEBAR_Main( BR_SIDEBAR_WORK *p_wk );

//����
extern void BR_SIDEBAR_StartBoot( BR_SIDEBAR_WORK *p_wk );
extern void BR_SIDEBAR_StartShake( BR_SIDEBAR_WORK *p_wk );
extern void BR_SIDEBAR_StartClose( BR_SIDEBAR_WORK *p_wk );

extern void BR_SIDEBAR_SetShakePos(BR_SIDEBAR_WORK *p_wk  );

extern BOOL BR_SIDEBAR_IsMoveEnd( const BR_SIDEBAR_WORK *cp_wk );

//br_musicallook_proc.c�ɏ��ʔj���E�ǂݍ��ݗp
extern void BR_SIDEBAR_UnLoadMain( BR_SIDEBAR_WORK *p_wk, BR_RES_WORK *p_res );
extern void BR_SIDEBAR_LoadMain( BR_SIDEBAR_WORK *p_wk, GFL_CLUNIT *p_clunit, BR_FADE_WORK *p_fade, BR_RES_WORK *p_res, HEAPID heapID );
