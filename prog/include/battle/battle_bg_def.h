//============================================================================================
/**
 * @file	battle_bg_def.h
 * @brief	�퓬�w�i�p�萔��`
 * @author	Miyuki Iwasawa
 * @date	09.10.29
 */
//============================================================================================

#pragma once

//--------------------------------------------------------------
/**
 *  �퓬�w�iID�i@todo ID�̂ݐ�s��`�@091028���_�Ŏw�肵�Ă��Ӗ������j
 */
//--------------------------------------------------------------
typedef enum {
  BTL_BG_GRASS,   ///< ���ނ�
  BTL_BG_SEA,     ///< �C
  BTL_BG_CITY,    ///< �X
  BTL_BG_FOREST,  ///< �X
  BTL_BG_SAND,    ///< ���n
  BTL_BG_SNOW,    ///< �ጴ
  BTL_BG_CAVE,    ///< ���A
  BTL_BG_ROCK,    ///< ���
  BTL_BG_ROOM,    ///< ����
  BTL_BG_MAX,
}BtlBgType;

//--------------------------------------------------------------
/**
 *  �n�`�i@@@ �܂��b��I�Ȃ��̂ł��B�s���ȏꍇ�AGRASS���w�肵�Ă��������j
 */
//--------------------------------------------------------------
typedef enum {
  BTL_LANDFORM_GRASS,   ///< ���ނ�
  BTL_LANDFORM_SAND,    ///< ���n
  BTL_LANDFORM_SEA,     ///< �C
  BTL_LANDFORM_RIVER,   ///< ��
  BTL_LANDFORM_SNOW,    ///< �ጴ
  BTL_LANDFORM_CAVE,    ///< ���A
  BTL_LANDFORM_ROCK,    ///< ���
  BTL_LANDFORM_ROOM,    ///< ����

  BTL_LANDFORM_MAX,
}BtlLandForm;


