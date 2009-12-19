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

#include "gamedata_def.h"

#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "field/location.h"

//======================================================================
//  define
//======================================================================
typedef u16 ZONEID; ///<ZONEID

enum {
	PLAYER_MAX = 5,
	
	PLAYER_ID_MINE = PLAYER_MAX - 1,    ///<�������g�̃v���C���[ID
};

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
  RAIL_LOCATION railposition;
	
  u16 direction;        ///<��]�p�x��Radian�l�@�i0�`0xffff�j
  u8 palace_area;       ///<��check�@�p���X�e�X�g�ׁ̈A�ꎞ�I�ɂ����ɔz�u
  u8 pos_type;       ///<LOCATION_POS_TYPE

  u8 objcode_fix; ///<OBJ�R�[�h�ύX�s��
  u8 padding[3]; ///<4byte�␳�]��
  
	MYSTATUS mystatus;
  PLAYER_MOVE_FORM move_form;
}PLAYER_WORK;

//======================================================================
//  extern
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���C���[�f�[�^���擾����
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @param	player_id	�v���C���[�w��ID
 * @return	PLAYER_WORK	�v���C���[�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMEDATA_GetPlayerWork(GAMEDATA * gamedata, u32 player_id);
//------------------------------------------------------------------
/**
 * @brief	�����̃v���C���[�f�[�^���擾����
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @return	PLAYER_WORK	�����̃v���C���[�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMEDATA_GetMyPlayerWork(GAMEDATA * gamedata);

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
 * @brief	���[�����W���Z�b�g����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @param	railpos		���[�����W���w�肷��l
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setRailPosition(PLAYER_WORK * player, const RAIL_LOCATION * railpos);

//--------------------------------------------------------------
/**
 * @brief	���[�����W���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	LOCATION_RAILPOS ���[�����W���w�肷��|�C���^
 */
//--------------------------------------------------------------
extern const RAIL_LOCATION * PLAYERWORK_getRailPosition(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	���W�^�C�v��ݒ肷��
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @param	pos_type ���W�^�C�v LOCATION_POS_TYPE
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setPosType(PLAYER_WORK * player, LOCATION_POS_TYPE pos_type);

//--------------------------------------------------------------
/**
 * @brief	���W�^�C�v���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	LOCATION_POS_TYPE ���W�^�C�v
 */
//--------------------------------------------------------------
extern LOCATION_POS_TYPE PLAYERWORK_getPosType(const PLAYER_WORK * player);

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
 * @brief	�����i�p�x�j��ݒ肷��
 * @param	player		PLAYER_WORK�ւ̃|�C���^
 * @param	direction	�������w�肷��l(���W�A���p�x0�`0xffff)
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setDirection(PLAYER_WORK * player, u16 direction);

//--------------------------------------------------------------
/**
 * @brief	�����i�p�x�j���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	�������w�肷��l(���W�A���p�x0�`0xffff)
 */
//--------------------------------------------------------------
extern u16 PLAYERWORK_getDirection(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	�����^�C�v�ŕ����p�x��ݒ肷��
 * @param	player		PLAYER_WORK�ւ̃|�C���^
 * @param	dir_type	4�������w�肷��l(DIR_UP DIR_DOWN DIR_LEFT DIR_RIGHT)
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setDirection_Type(PLAYER_WORK * player, u16 dir_type);

//--------------------------------------------------------------
/**
 * @brief	�����p�x�̕����^�C�v���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	�������w�肷��l(DIR_UP DIR_DOWN DIR_LEFT DIR_RIGHT)
 */
//--------------------------------------------------------------
extern u16 PLAYERWORK_getDirection_Type(const PLAYER_WORK * player);

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

//--------------------------------------------------------------
/**
 * @brief ���@OBJ�R�[�h�Œ�t���O���Z�b�g
 * @param player PLAYER_WORK�ւ̃|�C���^
 * @param flag TRUE=�Œ� FALSE=��
 * @retval none
 */
//--------------------------------------------------------------
extern void PLAYERWORK_SetFlagOBJCodeFix( PLAYER_WORK *player, BOOL flag );

  //--------------------------------------------------------------
/**
 * @brief ���@OBJ�R�[�h�Œ�t���O���`�F�b�N
 * @param player PLAYER_WORK�ւ̃|�C���^
 * @retval BOOL TRUE=�Œ� FALSE=��
 */
//--------------------------------------------------------------
extern BOOL PLAYERWORK_GetFlagOBJCodeFix( const PLAYER_WORK *player );


#ifdef	__cplusplus
} /* extern "C" */
#endif
