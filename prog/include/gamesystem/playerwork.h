//======================================================================
/**
 * @file	playerwork.h
 * @brief	�|�P�����Q�[���V�X�e���E�f�[�^��`
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.22
 */
//======================================================================
#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"

//======================================================================
//  define
//======================================================================
typedef u16 ZONEID; ///<ZONEID

//--------------------------------------------------------------
/// PLAYER_MOVE_FORM
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_FORM_NORMAL = 0,   ///<���@�񑫕��s
  PLAYER_MOVE_FORM_CYCLE,  ///<���@���]�Ԉړ�
  PLAYER_MOVE_FORM_SWIM, ///<���@�g���ړ�
  PLAYER_MOVE_FORM_MAX,
}PLAYER_MOVE_FORM;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	PLAYER_WORK ���@�f�[�^��`
 * @note �Œ���́u���@�v���Q�[�����ɑ��݂����邽�߂̃f�[�^�B
 * �ʐM�������Ȃǂő��v���C���[�̃f�[�^�Ȃǂ��ێ�����B
 */
//--------------------------------------------------------------
typedef struct {
	ZONEID zoneID;
	VecFx32 position;
	u16 direction;
  u8 palace_area;       ///<��check�@�p���X�e�X�g�ׁ̈A�ꎞ�I�ɂ����ɔz�u
  u8 padding;
	MYSTATUS mystatus;
  PLAYER_MOVE_FORM move_form;
}PLAYER_WORK;

//======================================================================
//  extern
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	�v���C���[���[�N�̏�����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern void PLAYERWORK_init(PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	�ʒu���Z�b�g����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @param	pos		�ʒu���w�肷��l
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setPosition(PLAYER_WORK * player, const VecFx32 * pos);

//--------------------------------------------------------------
/**
 * @brief	�ʒu���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	VecFx32* �ʒu���w�肷��VecFx32�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern const VecFx32 * PLAYERWORK_getPosition(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	�ʒu���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @param	zoneid	�}�b�v���w�肷��l
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setZoneID(PLAYER_WORK * player, ZONEID zoneid);

//--------------------------------------------------------------
/**
 * @brief	�ʒu���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	ZONEID	�}�b�v���w�肷��l
 */
//--------------------------------------------------------------
extern ZONEID PLAYERWORK_getZoneID(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	�ʒu���擾����
 * @param	player		PLAYER_WORK�ւ̃|�C���^
 * @param	direction	�������w�肷��l
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setDirection(PLAYER_WORK * player, u16 direction);

//--------------------------------------------------------------
/**
 * @brief	�������擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	�������w�肷��l
 */
//--------------------------------------------------------------
extern u16 PLAYERWORK_getDirection(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	�p���X�G���A�ԍ���ݒ肷��
 * @param	player	      PLAYER_WORK�ւ̃|�C���^
 * @param	palace_area   �p���X�ԍ�
 * 
 * ��check�@�p���X�e�X�g�ׂ̈̎b��֐�
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setPalaceArea(PLAYER_WORK * player, u8 palace_area);

//--------------------------------------------------------------
/**
 * @brief	�p���X�G���A�ԍ����擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	�p���X�G���A�ԍ�
 * 
 * ��check�@�p���X�e�X�g�ׂ̈̎b��֐�
 */
//--------------------------------------------------------------
extern u8 PLAYERWORK_getPalaceArea(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * PLAYER_MOVE_FORM�擾
 * @param player PLAYER_WORK�ւ̃|�C���^
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
extern PLAYER_MOVE_FORM PLAYERWORK_GetMoveForm( const PLAYER_WORK *player );

//--------------------------------------------------------------
/**
 * PLAYER_MOVE_FORM�Z�b�g
 * @param player PLAYER_WORK�ւ̃|�C���^
 * @param form PLAYER_MOVE_FORM
 * @retval none
 */
//--------------------------------------------------------------
extern void PLAYERWORK_SetMoveForm( PLAYER_WORK *player, PLAYER_MOVE_FORM form );

#ifdef	__cplusplus
} /* extern "C" */
#endif
