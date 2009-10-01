//======================================================================
/**
 * @file  gym_normal_sv.h
 * @bfief  �m�[�}���W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#ifndef __GYM_NORMAL_SV_H__

#define __GYM_NORMAL_SV_H__

#define NRM_WALL_NUM_MAX  (6)
//�m�[�}���W���Z�[�u���[�N
typedef struct GYM_NORMAL_SV_WORK_tag
{
  BOOL GmkUnrock;
  u8 Wall[NRM_WALL_NUM_MAX];
  u8 dummy[2];
}GYM_NORMAL_SV_WORK;

#endif  //__GYM_NORMAL_SV_H__
