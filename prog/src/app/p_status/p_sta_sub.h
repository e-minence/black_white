//======================================================================
/**
 * @file	p_sta_sub.c
 * @brief	�|�P�����X�e�[�^�X �E�������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS_SUB
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

#define PSTATUS_SUB_SHADOW_SCALE_X (FX32_ONE)
#define PSTATUS_SUB_SHADOW_SCALE_Y (FX32_CONST(2.2f))
#define PSTATUS_SUB_SHADOW_SCALE_Z (FX32_ONE)
#define PSTATUS_SUB_SHADOW_OFFSET_X (FX32_CONST(0.3f))
#define PSTATUS_SUB_SHADOW_OFFSET_Y (0)
#define PSTATUS_SUB_SHADOW_OFFSET_Z (-FX32_ONE)
#define PSTATUS_SUB_SHADOW_ROTATE (0xD6C1)

extern PSTATUS_SUB_WORK* PSTATUS_SUB_Init( PSTATUS_WORK *work );
extern void PSTATUS_SUB_Term( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
extern void PSTATUS_SUB_Main( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
extern void PSTATUS_SUB_Draw( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//���\�[�X������
void PSTATUS_SUB_LoadResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , ARCHANDLE *archandle );
void PSTATUS_SUB_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//�Z��������
void PSTATUS_SUB_InitCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
void PSTATUS_SUB_TermCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//�\����\���؂�ւ�
void PSTATUS_SUB_DispPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
void PSTATUS_SUB_ClearPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
void PSTATUS_SUB_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
void PSTATUS_SUB_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );


//�f�o�O�p
void PSTATUS_SUB_SetShadowScale( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
