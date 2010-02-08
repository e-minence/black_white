/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F���
 * @file   research_check.c
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "research_check.h"


//===============================================================================
// ���萔
//===============================================================================
// �����V�[�P���X
typedef enum{
  RESEARCH_CHECK_SEQ_SETUP,    // ����
  RESEARCH_CHECK_SEQ_CLEAN_UP, // ��Еt��
  RESEARCH_CHECK_SEQ_FINISH,   // �I��
} RESEARCH_CHECK_SEQ;


//=============================================================================== 
// �������񍐊m�F��� ���[�N
//=============================================================================== 
struct _RESEARCH_CHECK_WORK
{
  HEAPID heapID;
  RESEARCH_CHECK_SEQ seq;  // �����V�[�P���X
};


//=============================================================================== 
// �������񍐊m�F��� ����֐�
//=============================================================================== 

//-------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̐���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
RESEARCH_CHECK_WORK* CreateResearchCheckWork( HEAPID heapID )
{
  RESEARCH_CHECK_WORK* work;

  work         = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_CHECK_WORK) );
  work->heapID = heapID;
  work->seq    = RESEARCH_CHECK_SEQ_SETUP;
  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
void DeleteResearchCheckWork( RESEARCH_CHECK_WORK* work )
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
 * @brief �����񍐊m�F��� ���C���֐�
 *
 * @param work
 */
//-------------------------------------------------------------------------------
RESEARCH_CHECK_RESULT ResearchCheckMain( RESEARCH_CHECK_WORK* work )
{
  return RESEARCH_CHECK_RESULT_TO_MENU;
} 
