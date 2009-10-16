//======================================================================
/**
 * @file  gym_elec_sv.h
 * @brief  �d�C�W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#ifndef __GYM_ELEC_SV_H__

#define __GYM_ELEC_SV_H__

#define CAPSULE_NUM_MAX (4)
#define LEVER_NUM_MAX (CAPSULE_NUM_MAX)
#define PLATFORM_NO_STOP (-1)

typedef struct CAPSULE_DAT_tag
{
///  int RaleIdx;      //���ݑ��s���̃��[���C���f�b�N�X
  fx32 AnmFrame;    //���[���̌��݃A�j���t���[��
}CAPSULE_DAT;

//�d�C�W���Z�[�u���[�N
typedef struct GYM_ELEC_SV_WORK_tag
{
  CAPSULE_DAT CapDat[CAPSULE_NUM_MAX];
  u8 LeverSw[LEVER_NUM_MAX];    //0or1
  u8 RaleChgReq[CAPSULE_NUM_MAX];
  u8 NowRaleIdx[CAPSULE_NUM_MAX];     //���ݑ��s���Ă��郌�[���̃C���f�b�N�X(�X�C�b�`��ˑ�)
  s8 StopPlatformIdx[CAPSULE_NUM_MAX];  //PLATFORM_NO_STOP (-1)�@���g�p����̂Ń}�C�i�X�l���g�p�ł���悤�ɁB
  u16 EvtFlg[2];    //�C�x���g�i�s�t���O  0�F�C�x���g�g���[�i�[1�l�ځ@1�F�C�x���g�g���[�i�[2�l��
}GYM_ELEC_SV_WORK;

#endif  //__GYM_ELEC_SV_H__
