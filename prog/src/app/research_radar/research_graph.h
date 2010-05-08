/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F���
 * @file   research_graph.h
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
// ���C���֐� �߂�l
typedef enum {
  RESEARCH_CHECK_RESULT_NONE,      // �_�~�[
  RESEARCH_CHECK_RESULT_CONTINUE,  // ��ʌp��
  RESEARCH_CHECK_RESULT_TO_MENU,   // ���j���[��ʂ�
} RESEARCH_CHECK_RESULT;


//=============================================================================== 
// �������񍐊m�F��� ���[�N
//=============================================================================== 
typedef struct _RESEARCH_CHECK_WORK RESEARCH_CHECK_WORK;


//=============================================================================== 
// �������񍐊m�F��� ����֐�
//=============================================================================== 
// ���[�N����/�j��
RESEARCH_CHECK_WORK* CreateResearchCheckWork( RESEARCH_COMMON_WORK* commonWork );
void                 DeleteResearchCheckWork( RESEARCH_CHECK_WORK* work );

// ���C���֐�
RESEARCH_CHECK_RESULT ResearchCheckMain( RESEARCH_CHECK_WORK* work ); 
