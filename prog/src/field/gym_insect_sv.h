//======================================================================
/**
 * @file  gym_insect_sv.h
 * @brief  ���W���Z�[�u�f�[�^
 * @author  Saito
 */
//======================================================================
#pragma once

#define INSECT_SW_MAX (8)
#define INSECT_PL_MAX (10)
#define INSECT_WALL_MAX (7)

typedef enum
{
  WALL_ST_NORMAL,
  WALL_ST_WEAKNESS,
}GYM_INSECT_WALL_ST;

//���W���Z�[�u���[�N
typedef struct GYM_INSECT_SV_WORK_tag
{
  BOOL Sw[INSECT_SW_MAX];    //�X�C�b�`��     8
  BOOL Pl[INSECT_PL_MAX];     //�|�[����      10
  GYM_INSECT_WALL_ST WallSt[INSECT_WALL_MAX]; //�E�H�[���o���h���  7
}GYM_INSECT_SV_WORK;

