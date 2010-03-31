//======================================================================
/**
 * @file  gym_anti_sv.h
 * @brief  �A���`�W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#pragma once

#define ANTI_SW_NUM_MAX  (9)
#define ANTI_DOOR_NUM_MAX  (3)
//�A���`�W���Z�[�u���[�N
typedef struct GYM_ANTI_SV_WORK_tag
{
  u8 Door[ANTI_DOOR_NUM_MAX];
  u8 dummy;
}GYM_ANTI_SV_WORK;

