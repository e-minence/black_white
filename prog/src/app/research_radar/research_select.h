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


//===============================================================================
// ���萔
//=============================================================================== 
// ���C���֐� �߂�l
typedef enum {
  RESEARCH_SELECT_RESULT_CONTINUE, // ��ʌp��
  RESEARCH_SELECT_RESULT_TO_MENU,  // ���j���[��ʂ�
} RESEARCH_SELECT_RESULT;


//=============================================================================== 
// ���������e�ύX��� ���[�N
//=============================================================================== 
typedef struct _RESEARCH_SELECT_WORK RESEARCH_SELECT_WORK;


//=============================================================================== 
// ���������e�ύX��� ����֐�
//=============================================================================== 
// ���[�N����/�j��
RESEARCH_SELECT_WORK* CreateResearchSelectWork( HEAPID heapID );
void                  DeleteResearchSelectWork( RESEARCH_SELECT_WORK* work );

// ���C���֐�
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work ); 
