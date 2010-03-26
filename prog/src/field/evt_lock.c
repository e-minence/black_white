//======================================================================
/*
 * @file	evt_lock.c
 * @brief	�C�x���g���b�N
 * @author saito
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "system/main.h"

#include "evt_lock.h"

#define EVT_MASIC_NUM_ROCKCAPSULE    (20100325)    //���b�N�J�v�Z��

//�}�W�b�N�i���o�[�e�[�u��
const u32 EvtMasicNumTbl[8] = 
{
  EVT_MASIC_NUM_ROCKCAPSULE,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};


//--------------------------------------------------------------
/**
 * �h�c���w�肵�āA�L�[���쐬���āA�Z�[�u�f�[�^�ɏ�������
 * @param misc
 * @param inNo
 * @param my
 * @retval none
 */
//--------------------------------------------------------------
void EVTROCK_SetEvtRock( MISC * misc, const int inNo, MYSTATUS *my )
{
  u32 masic;
  u32 key;
  u32 val;

  if (inNo >= EVT_MAX)
  {
    GF_ASSERT_MSG(0,"NO Error");
    return;
  }

  masic = EvtMasicNumTbl[inNo];
  key = MyStatus_GetID(my);
  //�w�n�q���Ƃ�
  val = masic^key;

  //�Z�[�u�f�[�^�ɃZ�b�g
  MISC_SetEvtRockValue( misc, inNo, val );
}

//--------------------------------------------------------------
/**
 * �w�肵���h�c�̃C�x���g���b�N�l�����K�̂��̂��𒲂ׂ�
 * @param misc
 * @param inNo
 * @param my
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL EVTROCK_ChekcEvtRock( MISC * misc, const int inNo, MYSTATUS *my )
{
  u32 masic;
  u32 key;
  u32 val;

  if (inNo>=EVT_MAX) return FALSE;

  key = MyStatus_GetID(my);
  //�Z�[�u�f�[�^�擾
  val = MISC_GetEvtRockValue( misc, inNo );
  masic = val^key;
  if (masic == EvtMasicNumTbl[inNo]) return TRUE;
  
  return FALSE;
}


