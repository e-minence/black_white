//======================================================================
/**
 * @file  gym_anti_sv.h
 * @brief  �A���`�W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#ifndef __GYM_ANTI_SV_H__

#define __GYM_ANTI_SV_H__

#define ANTI_SW_NUM_MAX  (3)
//�A���`�W���Z�[�u���[�N
typedef struct GYM_ANTI_SV_WORK_tag
{
  u8 Sw[ANTI_SW_NUM_MAX];
  u8 dummy;
}GYM_ANTI_SV_WORK;

#endif  //__GYM_ANTI_SV_H__
