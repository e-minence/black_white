/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX���
 * @file   research_select.h
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "research_common.h"       // for RESEARCH_COMMON_WORK


//===============================================================================
// ���萔
//=============================================================================== 
// ���C���֐� �I������
typedef enum {
  RESEARCH_SELECT_RESULT_NONE,      // �_�~�[
  RESEARCH_SELECT_RESULT_CONTINUE,  // ��ʌp��
  RESEARCH_SELECT_RESULT_TO_MENU,   // �����񍐏�����ʂ�
} RESEARCH_SELECT_RESULT;


//=============================================================================== 
// ������������� ���[�N
//=============================================================================== 
typedef struct _RESEARCH_SELECT_WORK RESEARCH_SELECT_WORK;


//=============================================================================== 
// ������������� ����֐�
//=============================================================================== 
// ���[�N����/�j��
RESEARCH_SELECT_WORK* CreateResearchSelectWork( RESEARCH_COMMON_WORK* commonWork );
void DeleteResearchSelectWork( RESEARCH_SELECT_WORK* work );

// ���C������
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work );
