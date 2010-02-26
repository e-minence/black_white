/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������� ( ���j���[��� )
 * @file   research_menu.h
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
  RESEARCH_MENU_RESULT_NONE,       // �_�~�[
  RESEARCH_MENU_RESULT_CONTINUE,   // ��ʌp��
  RESEARCH_MENU_RESULT_TO_SELECT,  // �������e�ύX��ʂ�
  RESEARCH_MENU_RESULT_TO_CHECK,   // �����񍐊m�F��ʂ�
  RESEARCH_MENU_RESULT_EXIT,       // �������[�_�[�I��
} RESEARCH_MENU_RESULT;


//=============================================================================== 
// ������������� ���[�N
//=============================================================================== 
typedef struct _RESEARCH_MENU_WORK RESEARCH_MENU_WORK;


//=============================================================================== 
// ������������� ����֐�
//=============================================================================== 
// ���[�N����/�j��
RESEARCH_MENU_WORK* CreateResearchMenuWork( RESEARCH_COMMON_WORK* commonWork );
void DeleteResearchMenuWork( RESEARCH_MENU_WORK* work );

// ���C������
RESEARCH_MENU_RESULT ResearchMenuMain( RESEARCH_MENU_WORK* work );
