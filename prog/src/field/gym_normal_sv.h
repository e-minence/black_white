//======================================================================
/**
 * @file  gym_normal_sv.h
 * @brief  ノーマルジムセーブデータ
 * @author  Saito
 */
//======================================================================
#pragma once

#define NRM_WALL_NUM_MAX  (1)
//ノーマルジムセーブワーク
typedef struct GYM_NORMAL_SV_WORK_tag
{
  BOOL GmkUnrock;
  u8 Wall[NRM_WALL_NUM_MAX];
  u8 dummy[3];
}GYM_NORMAL_SV_WORK;

