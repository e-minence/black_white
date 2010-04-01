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
 *  �퓬�w�i�w��A�g���r���[�gID
 *
 *  resource/batt_bg_tbl/wb_battle_bg.xls�̗񍀖ڂƎ蓮�ň�v������K�v������܂�
 */
//--------------------------------------------------------------
typedef enum {
  BATTLE_BG_ATTR_LAWN,	    ///<	�Ő�
  BATTLE_BG_ATTR_GROUND,  	///<	�ʏ�n��
  BATTLE_BG_ATTR_GROUND_S1,	///<	�l�G�ω��n�ʂP
  BATTLE_BG_ATTR_GROUND_S2,	///<	�l�G�ω��n�ʂQ
  BATTLE_BG_ATTR_NORMAL,	  ///<	�ړ��\
  BATTLE_BG_ATTR_E_GRASS,	  ///<	�G���J�E���g��
  BATTLE_BG_ATTR_WATER,	    ///<	����
  BATTLE_BG_ATTR_SNOW,	    ///<	�ጴ
  BATTLE_BG_ATTR_SAND,	    ///<	���n
  BATTLE_BG_ATTR_MARSH,	    ///<	�󂢎���
  BATTLE_BG_ATTR_CAVE,	    ///<	���A
  BATTLE_BG_ATTR_POOL,	    ///<	�����܂�
  BATTLE_BG_ATTR_SHOAL,	    ///<	��
  BATTLE_BG_ATTR_ICE,	      ///<	�X��
  BATTLE_BG_ATTR_E_INDOOR,	///<	�����G���J�E���g
  BATTLE_BG_ATTR_PALACE,	  ///<	�p���X
  BATTLE_BG_ATTR_BIGFOUR1,	///<	�l�V���P(�S�[�X�g)
  BATTLE_BG_ATTR_BIGFOUR2,	///<	�l�V���Q(�i��)
  BATTLE_BG_ATTR_BIGFOUR3,	///<	�l�V���R(��)
  BATTLE_BG_ATTR_BIGFOUR4,	///<	�l�V���S(�G�X�p�[)
  BATTLE_BG_ATTR_BOSS,	    ///<	�m��p
  BATTLE_BG_ATTR_SAGE,	    ///<	�Q�[�`�X
  BATTLE_BG_ATTR_CHAMPION,	///<	�`�����s�I��

  BATTLE_BG_ATTR_MAX,
}BtlBgAttr;

