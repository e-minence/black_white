//============================================================================================
/**
 * @file  battle_bg_def.h
 * @brief �퓬�w�i�p�萔��`
 * @author  Miyuki Iwasawa
 * @date  09.10.29
 */
//============================================================================================

#pragma once

//--------------------------------------------------------------
/**
 *  �퓬�w�i�w��^�C�vID�i@todo ID�̂ݐ�s��`�@091028���_�Ŏw�肵�Ă��Ӗ������j
 */
//--------------------------------------------------------------
typedef enum {
  BATTLE_BG_TYPE_GRASS,         ///< ���ނ�
  BATTLE_BG_TYPE_GRASS_SEASON,  ///< ���ނ�(�l�G�L��)
  BATTLE_BG_TYPE_CITY,          ///< �X
  BATTLE_BG_TYPE_CITY_SEASON,   ///< �X(�l�G�L��)
  BATTLE_BG_TYPE_CAVE,          ///< ���A
  BATTLE_BG_TYPE_CAVE_DARK,     ///< ���A(�Â�)
  BATTLE_BG_TYPE_FOREST,        ///< �X
  BATTLE_BG_TYPE_MOUNTAIN,      ///< �R
  BATTLE_BG_TYPE_SEA,           ///< �C
  BATTLE_BG_TYPE_ROOM,          ///< ����
  BATTLE_BG_TYPE_SAND,          ///< ����
   
  BATTLE_BG_TYPE_MAX,

  //��ŏ����\��M�l��
  BATTLE_BG_TYPE_SNOW,
  BATTLE_BG_TYPE_ROCK,
}BtlBgType;

//--------------------------------------------------------------
/*
 *  �퓬�w�i�w��A�g���r���[�gID�i@todo ID�̂ݐ�s��`�@091028���_�Ŏw�肵�Ă��Ӗ������j
 */
//--------------------------------------------------------------
typedef enum {
//�Ő�
BATTLE_BG_ATTR_LAWN,
//�ʏ�n��
BATTLE_BG_ATTR_NORMAL_GROUND,
//�n�ʂP
BATTLE_BG_ATTR_GROUND1,
//�n�ʂQ
BATTLE_BG_ATTR_GROUND2,
//��
BATTLE_BG_ATTR_GRASS,
//����
BATTLE_BG_ATTR_WATER,
//�ጴ
BATTLE_BG_ATTR_SNOW,
//���n
BATTLE_BG_ATTR_SAND,
//�󂢎���
BATTLE_BG_ATTR_MARSH,
//���A
BATTLE_BG_ATTR_CAVE,
//�����܂�
BATTLE_BG_ATTR_POOL,
//��
BATTLE_BG_ATTR_SHOAL,
//�X��
BATTLE_BG_ATTR_ICE,

BATTLE_BG_ATTR_MAX,
}BtlBgAttr;

