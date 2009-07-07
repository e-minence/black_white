//======================================================================
/**
 * @file	p_sta_info.c
 * @brief	�|�P�����X�e�[�^�X ���y�[�W
 * @author	ariizumi
 * @data	09/07/06
 *
 * ���W���[�����FPSTATUS_INFO
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

extern PSTATUS_INFO_WORK* PSTATUS_INFO_Init( PSTATUS_WORK *work );
extern void PSTATUS_INFO_Term( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );
extern void PSTATUS_INFO_Main( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );
extern void PSTATUS_INFO_Draw( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );

//���\�[�X������
void PSTATUS_INFO_LoadResource( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , ARCHANDLE *archandle );
void PSTATUS_INFO_ReleaseResource( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );

//�\����\���؂�ւ�
void PSTATUS_INFO_DispPage( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );
void PSTATUS_INFO_ClearPage( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork );

