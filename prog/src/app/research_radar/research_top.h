/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �g�b�v���
 * @file   research_top.h
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
// �g�b�v��ʂ̏I������
typedef enum {
  RRT_RESULT_TO_LIST,  // ���X�g��ʂ�
  RRT_RESULT_TO_GRAPH, // �O���t��ʂ�
  RRT_RESULT_EXIT,     // �������[�_�[�I��
} RRT_RESULT;


//=============================================================================== 
// ���g�b�v��ʃ��[�N�̕s���S�^
//=============================================================================== 
typedef struct _RESEARCH_RADAR_TOP_WORK RRT_WORK;


//=============================================================================== 
// ��
//=============================================================================== 
// ���[�N�𐶐�����
extern RRT_WORK* RRT_CreateWork( RESEARCH_COMMON_WORK* commonWork );
// ���[�N��j������
extern void RRT_DeleteWork( RRT_WORK* work ); 
// ���C������
extern void RRT_Main( RRT_WORK* work );
// �I������
extern BOOL RRT_IsFinished( const RRT_WORK* work );
// �I�����ʂ̎擾
extern RRT_RESULT RRT_GetResult( const RRT_WORK* work );
