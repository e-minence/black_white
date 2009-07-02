//======================================================================
/**
 * @file	p_sta_sub.c
 * @brief	�|�P�������X�g �E�������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS_SUB
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

extern PSTATUS_SUB_WORK* PSTATUS_SUB_Init( PSTATUS_WORK *work );
extern void PSTATUS_SUB_Term( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
extern void PSTATUS_SUB_Main( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
extern void PSTATUS_SUB_Draw( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//���\�[�X������
void PSTATUS_SUB_LoadResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , ARCHANDLE *archandle );
void PSTATUS_SUB_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//�\����\���؂�ւ�
void PSTATUS_SUB_DispPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
void PSTATUS_SUB_ClearPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

