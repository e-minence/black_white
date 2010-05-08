/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �O���t���
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
// �O���t��ʂ̏I������
typedef enum {
  RRG_RESULT_TO_TOP, // �g�b�v��ʂ�
} RRG_RESULT;


//=============================================================================== 
// ���O���t��ʊǗ����[�N�̕s���S�^
//=============================================================================== 
typedef struct _RESEARCH_RADAR_GRAPH_WORK RRG_WORK;


//=============================================================================== 
// �������񍐊m�F��� ����֐�
//=============================================================================== 
// ���[�N�𐶐�����
extern RRG_WORK* RRG_CreateWork( RESEARCH_COMMON_WORK* commonWork );
// ���[�N��j������
extern void RRG_DeleteWork( RRG_WORK* work ); 
// ���C���֐�
extern void RRG_Main( RRG_WORK* work ); 
// �I������
extern BOOL RRG_IsFinished( const RRG_WORK* work );
// �I�����ʂ̎擾
extern RRG_RESULT RRG_GetResult( const RRG_WORK* work );
