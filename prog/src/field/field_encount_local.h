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

//�G�t�F�N�g�G���J�E���g�@�p�����[�^
typedef struct _EFFENC_PARAM{
  u16   zone_id;
  s16   gx,gy,gz;
  fx32  height;
  u8    type;
  u8    valid_f:4;
  u8    push_f:2;
  u8    push_cancel_f:2;
}EFFENC_PARAM;

///�G���J�E���g�G�t�F�N�g���䃏�[�N
typedef struct _EWK_EFFECT_ENCOUNT
{
  u32 walk_ct;
  EFFENC_PARAM  param;

#ifdef PM_DEBUG
  u8  deb_interval;
  u8  deb_prob;
  u8  deb_ofsx;
  u8  deb_ofsz;
#endif

}EWK_EFFECT_ENCOUNT;

///�G���J�E���g���䃏�[�N
struct _TAG_ENCOUNT_WORK
{
  EWK_PLAYER player;
  EWK_EFFECT_ENCOUNT  effect_encount;
};



