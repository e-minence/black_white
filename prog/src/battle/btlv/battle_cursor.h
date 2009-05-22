//==============================================================================
/**
 * @file	battle_cursor.h
 * @brief	�퓬����ʗp�J�[�\���̃w�b�_
 * @author	matsuda changed by soga
 * @date	2009.04.13(��)
 */
//==============================================================================

#pragma once

#include "system/palanm.h"

//==============================================================================
//	�萔��`
//==============================================================================
///�퓬�J�[�\���̃A�j���^�C�v
typedef enum{
	BCURSOR_ANMTYPE_LU,		///<����J�[�\��
	BCURSOR_ANMTYPE_RU,		///<�E��J�[�\��
	BCURSOR_ANMTYPE_LD,		///<�����J�[�\��
	BCURSOR_ANMTYPE_RD,		///<�E���J�[�\��
}BCURSOR_ANMTYPE;

//==============================================================================
//	�^��`
//==============================================================================
///BCURSOR_WORK�̕s��`�|�C���^
typedef struct _BCURSOR_WORK * BCURSOR_PTR;
///BCURSOR_RES_WORK�̕s��`�|�C���^
typedef struct _BCURSOR_RES_WORK * BCURSOR_RES_PTR;

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern	BCURSOR_RES_PTR BCURSOR_ResourceLoad( PALETTE_FADE_PTR pfd, HEAPID heap_id );
extern	void BCURSOR_ResourceFree( BCURSOR_RES_PTR bcrp );
extern	BCURSOR_PTR BCURSOR_ActorCreate( BCURSOR_RES_PTR bcrp, HEAPID heap_id, u32 soft_pri, u32 bg_pri);

extern void BCURSOR_ActorDelete( BCURSOR_PTR cursor );
extern void BCURSOR_PosSetON( BCURSOR_PTR cursor, int left, int right, int top, int bottom );
extern void BCURSOR_ExPosSetON( BCURSOR_PTR cursor, int x, int y, BCURSOR_ANMTYPE anm_type );
extern void BCURSOR_OFF( BCURSOR_PTR cursor );
extern void BCURSOR_ExOFF( BCURSOR_PTR cursor );
extern void BCURSOR_IndividualPosSetON( BCURSOR_PTR cursor, int lu_x, int lu_y, int ru_x, int ru_y,
	int ld_x, int ld_y, int rd_x, int rd_y );

