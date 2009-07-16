//======================================================================
/**
 * @file	p_sta_skill.c
 * @brief	�|�P�����X�e�[�^�X �Z�y�[�W
 * @author	ariizumi
 * @data	09/07/10
 *
 * ���W���[�����FPSTATUS_SKILL
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

extern PSTATUS_SKILL_WORK* PSTATUS_SKILL_Init( PSTATUS_WORK *work );
extern void PSTATUS_SKILL_Term( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
extern void PSTATUS_SKILL_Main( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
extern void PSTATUS_SKILL_Draw( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );

//���\�[�X������
void PSTATUS_SKILL_LoadResource( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , ARCHANDLE *archandle );
void PSTATUS_SKILL_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );

//�Z��������
void PSTATUS_SKILL_InitCell( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
void PSTATUS_SKILL_TermCell( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );

//�\����\���؂�ւ�
void PSTATUS_SKILL_DispPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
void PSTATUS_SKILL_ClearPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
void PSTATUS_SKILL_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
void PSTATUS_SKILL_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
