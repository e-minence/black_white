/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���X�g���
 * @file   research_list.h
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
// ���X�g��ʂ̏I������
typedef enum {
  RRL_RESULT_TO_TOP, // �g�b�v��ʂ�
} RRL_RESULT;


//=============================================================================== 
// �����X�g��ʃ��[�N�̕s���S�^
//=============================================================================== 
typedef struct _RESEARCH_RADAR_LIST_WORK RRL_WORK;


//=============================================================================== 
// ��
//=============================================================================== 
// ���[�N�𐶐�����
extern RRL_WORK* RRL_CreateWork( RESEARCH_COMMON_WORK* commonWork );
// ���[�N��j������
extern void RRL_DeleteWork( RRL_WORK* work ); 
// ���C������
extern void RRL_Main( RRL_WORK* work );
// �I������
extern RRL_IsFinished( const RRL_WORK* work );
// �I�����ʂ̎擾
extern RRL_RESULT RRL_GetResult( const RRL_WORK* work );
