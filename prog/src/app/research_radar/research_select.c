/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX���
 * @file   research_select.c
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "research_select.h"


//===============================================================================
// ���萔
//===============================================================================
// �����V�[�P���X
typedef enum{
  RESEARCH_SELECT_SEQ_SETUP,    // ����
  RESEARCH_SELECT_SEQ_CLEAN_UP, // ��Еt��
  RESEARCH_SELECT_SEQ_FINISH,   // �I��
} RESEARCH_SELECT_SEQ;


//=============================================================================== 
// ���������e�ύX��� ���[�N
//=============================================================================== 
struct _RESEARCH_SELECT_WORK
{
  HEAPID heapID;
  RESEARCH_SELECT_SEQ seq;  // �����V�[�P���X
};


//=============================================================================== 
// ���������e�ύX��� ����֐�
//=============================================================================== 

//-------------------------------------------------------------------------------
/**
 * @brief �������e�ύX��ʃ��[�N�̐���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
RESEARCH_SELECT_WORK* CreateResearchSelectWork( HEAPID heapID )
{
  RESEARCH_SELECT_WORK* work;

  work         = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_SELECT_WORK) );
  work->heapID = heapID;
  work->seq    = RESEARCH_SELECT_SEQ_SETUP;
  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief �������e�ύX��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
void DeleteResearchSelectWork( RESEARCH_SELECT_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  } 
  GFL_HEAP_FreeMemory( work );
} 

//-------------------------------------------------------------------------------
/**
 * @brief �������e�ύX��� ���C���֐�
 *
 * @param work
 */
//-------------------------------------------------------------------------------
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work )
{
  return RESEARCH_SELECT_RESULT_TO_MENU;
} 
