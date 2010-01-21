//======================================================================
/**
 * @file	enc_cutin_no.c
 * @brief	�G���J�E���g�J�b�g�C���i���o�[
 * @author	Saito
 *
 */
//======================================================================

#include "enc_cutin_no.h"
#include "msg/msg_sptr_name.h"
#include "../../../resource/fld3d_ci/fldci_id_def.h"

static const ENC_CUTIN_DAT EncCutinDat[] = 
{
  {FLDCIID_RIVAL, 0, 0, SPTR_RIVAL, GRA_TRANS_ALL}, //���C�o��
  {FLDCIID_RIVAL, 0, 0, SPTR_SUPPORT, GRA_TRANS_ALL}, //�T�|�[�g
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM01A, GRA_TRANS_ALL}, //�W�����[�_�[01A
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM01B, GRA_TRANS_ALL}, //�W�����[�_�[01B
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM01C, GRA_TRANS_ALL}, //�W�����[�_�[01C
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM02, GRA_TRANS_ALL}, //�W�����[�_�[02
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM03, GRA_TRANS_ALL}, //�W�����[�_�[03
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM04, GRA_TRANS_ALL}, //�W�����[�_�[04
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM05, GRA_TRANS_ALL}, //�W�����[�_�[05
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM06, GRA_TRANS_ALL}, //�W�����[�_�[06
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM07, GRA_TRANS_ALL}, //�W�����[�_�[07
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM08A, GRA_TRANS_ALL}, //�W�����[�_�[08A
  {FLDCIID_RIVAL, 0, 0, SPTR_GYM08B, GRA_TRANS_ALL}, //�W�����[�_�[08B
  {FLDCIID_RIVAL, 0, 0, SPTR_BIGFOUR01, GRA_TRANS_ALL}, //�l�V��1
  {FLDCIID_RIVAL, 0, 0, SPTR_BIGFOUR02, GRA_TRANS_ALL}, //�l�V��2
  {FLDCIID_RIVAL, 0, 0, SPTR_BIGFOUR03, GRA_TRANS_ALL}, //�l�V��3
  {FLDCIID_RIVAL, 0, 0, SPTR_BIGFOUR04, GRA_TRANS_ALL}, //�l�V��4
  {FLDCIID_RIVAL, 0, 0, SPTR_CHAMP, GRA_TRANS_ALL}, //�`�����v
  {FLDCIID_RIVAL, 0, 0, SPTR_BOSS, GRA_TRANS_ALL}, //�{�X
  {FLDCIID_RIVAL, 0, 0, SPTR_SAGE, GRA_TRANS_ALL}, //�Z�[�W
  {0, 0, 0, 0, GRA_TRANS_NONE}, //�v���Y�}�c
};

const ENC_CUTIN_DAT *ENC_CUTIN_NO_GetDat(const inEncCutinNo)
{
  return &EncCutinDat[inEncCutinNo];
}
