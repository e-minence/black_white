//============================================================================================
/**
 * @file	playerwork.h
 * @brief	�|�P�����Q�[���V�X�e���E�f�[�^��`
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.22
 *
 */
//============================================================================================

#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef u16 ZONEID;

//------------------------------------------------------------------
/**
 * @brief	�����f�[�^��`
 *
 * �Œ����ID�ƂȂ�f�[�^�B
 * �i�r�[�R���ɖ��ߍ��܂�铙�j
 * ���݂͂��̃w�b�_�Œ�`���Ă��邪�����I�ɂ͕��������͂�
 *
 */
//------------------------------------------------------------------
typedef struct {
	u32 id;
	u8 sex;
	u8 region_code;
}MYSTATUS;

//------------------------------------------------------------------
/**
 * @brief	���@�f�[�^��`
 *
 * �Œ���́u���@�v���Q�[�����ɑ��݂����邽�߂̃f�[�^�B
 * �ʐM�������Ȃǂő��v���C���[�̃f�[�^�Ȃǂ��ێ�����B
 */
//------------------------------------------------------------------
typedef struct {
	ZONEID zoneID;
	VecFx32 position;
	u16 direction;

	MYSTATUS mystatus;
}PLAYER_WORK;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern void PLAYERWORK_init(PLAYER_WORK * player);

extern void PLAYERWORK_setPosition(PLAYER_WORK * player, const VecFx32 * pos);
extern const VecFx32 * PLAYERWORK_getPosition(const PLAYER_WORK * player);
extern void PLAYERWORK_setZoneID(PLAYER_WORK * player, ZONEID zoneid);
extern ZONEID PLAYERWORK_getZoneID(const PLAYER_WORK * player);
extern void PLAYERWORK_setDirection(PLAYER_WORK * player, u16 direction);
extern u16 PLAYERWORK_getDirection(const PLAYER_WORK * player);


#ifdef	__cplusplus
} /* extern "C" */
#endif

