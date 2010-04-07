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

#define EVT_MASIC_NUM_LOCKCAPSULE    (20100325)    //���b�N�J�v�Z��
#define EVT_MASIC_NUM_VICTYTICKET    (20100406)    //�r�N�e�B�̃`�P�b�g

//�}�W�b�N�i���o�[�e�[�u��
const u32 EvtMasicNumTbl[EVT_MAX] = 
{
  EVT_MASIC_NUM_LOCKCAPSULE,
  EVT_MASIC_NUM_VICTYTICKET,
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
void EVTLOCK_SetEvtLock( MISC * misc, const int inNo, MYSTATUS *my )
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
BOOL EVTLOCK_CheckEvtLock( MISC * misc, const int inNo, MYSTATUS *my )
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


