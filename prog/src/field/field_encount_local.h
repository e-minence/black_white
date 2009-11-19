/*
 *  @file field_encount_local.h
 *  @brief  field_encount.c ���[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   09.10.19
 */

#pragma once

#include "effect_encount.h"

//======================================================================
//  struct
//======================================================================

/////////////////////////////////////////////////////
///�ʏ�G���J�E���g����@�v���C���[���
typedef struct _EWK_PLAYER{
  u16 pos_x;    //�Ō�ɃG���J�E���g�����v���C���[�O���b�h�|�W�V����
  u16 pos_y;
  u16 pos_z;
  u16 move_f;   //
  u32 walk_ct;
}EWK_PLAYER;

/////////////////////////////////////////////////////
///�t�B�[���h�G���J�E���g���䃏�[�N fieldWork�풓
struct _TAG_FIELD_ENCOUNT
{
  FIELDMAP_WORK *fwork;
  GAMESYS_WORK *gsys;
  GAMEDATA *gdata;
  ENCOUNT_DATA* encdata;
  EFFECT_ENCOUNT* eff_enc;
};


///�G���J�E���g�G�t�F�N�g���䃏�[�N
typedef struct _EWK_EFFECT_ENCOUNT
{
  u32 walk_ct;
}EWK_EFFECT_ENCOUNT;

///�G���J�E���g���䃏�[�N
struct _TAG_ENCOUNT_WORK
{
  EWK_PLAYER player;
  EWK_EFFECT_ENCOUNT  effect_encount;
};



