//======================================================================
/**
 * @file  gym_ground_sv.h
 * @brief  �n�ʃW���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#pragma once

#define LIFT_NUM_MAX (6)

//�n�ʃW���Z�[�u���[�N
typedef struct GYM_GROUND_SV_WORK_tag
{
  BOOL LiftMoved[LIFT_NUM_MAX]; //���t�g����ʒu���瓮�������@FALSE�Ŋ�ʒu
}GYM_GROUND_SV_WORK;

