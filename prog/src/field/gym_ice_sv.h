//======================================================================
/**
 * @file  gym_ice_sv.h
 * @brief  �X�W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#pragma once

#define WALL_NUM_MAX (3)

//�X�W���Z�[�u���[�N
typedef struct GYM_ICE_SV_WORK_tag
{
  BOOL WallMoved[WALL_NUM_MAX]; //��ʒu���瓮�������@FALSE�Ŋ�ʒu
}GYM_ICE_SV_WORK;

