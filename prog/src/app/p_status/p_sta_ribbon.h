//======================================================================
/**
 * @file	p_sta_ribbon.c
 * @brief	�|�P�����X�e�[�^�X ���{���y�[�W
 * @author	ariizumi
 * @data	09/07/06
 *
 * ���W���[�����FPSTATUS_RIBBON
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

extern PSTATUS_RIBBON_WORK* PSTATUS_RIBBON_Init( PSTATUS_WORK *work );
extern void PSTATUS_RIBBON_Term( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
extern void PSTATUS_RIBBON_Main( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
extern void PSTATUS_RIBBON_Draw( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );

//���\�[�X������
void PSTATUS_RIBBON_LoadResource( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork , ARCHANDLE *archandle );
void PSTATUS_RIBBON_ReleaseResource( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );

//�Z��������
void PSTATUS_RIBBON_InitCell( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
void PSTATUS_RIBBON_TermCell( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );

//�\����\���؂�ւ�
void PSTATUS_RIBBON_DispPage( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
void PSTATUS_RIBBON_ClearPage( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );

void PSTATUS_RIBBON_CreateRibbonBar( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
void PSTATUS_RIBBON_DeleteRibbonBar( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork );
