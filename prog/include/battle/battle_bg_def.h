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
 *  �퓬�w�i�w��^�C�vID
 *
 *  resource/batt_bg_tbl/wb_battle_bg.xls�̍s���ڂƎ蓮�ň�v������K�v������܂�
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
  BATTLE_BG_TYPE_BIGFOUR_GHOST,   ///< �l�V������(�S�[�X�g)
  BATTLE_BG_TYPE_BIGFOUR_KAKUTOU, ///< �l�V������(�i��)
  BATTLE_BG_TYPE_BIGFOUR_EVIL,    ///< �l�V������(��)
  BATTLE_BG_TYPE_BIGFOUR_ESPER,   ///< �l�V������(�G�X�p�[)
  BATTLE_BG_TYPE_N_CASTLE,        ///< N�̏�(�y���̊�)
  BATTLE_BG_TYPE_CHAMPIOPN,       ///< �`�����s�I������
  BATTLE_BG_TYPE_RYUURASEN7,      ///< �����E���Z���̓�7F
   
  BATTLE_BG_TYPE_MAX,
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
  BATTLE_BG_ATTR_SAGE,	    ///<	�Q�[�`�X

  BATTLE_BG_ATTR_MAX,
}BtlBgAttr;

