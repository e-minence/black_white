//============================================================================================
/**
 * @file	game_data.h
 * @brief	�Q�[���i�s�f�[�^�Ǘ�
 * @date	2008.11.04
 *
 * ��{�I�ɃQ�[���i�s�f�[�^�̓Z�[�u�f�[�^�Ƃ��đ��݂��邪�A
 * �ʐM���̕����v���C���[����t�B�[���h�}�b�v���ȂǃZ�[�u�f�[�^��
 * �̂�Ƃ͌���Ȃ���������B
 * ���̂悤�ȃZ�[�u�f�[�^�E��Z�[�u�f�[�^�֕���ɃA�N�Z�X����C���^�[�t�F�C�X��
 * �e�p�[�g���Ƃɍ\������ƃp�[�g���ƂɃA�N�Z�X��@���������A�܂������@�\��
 * ���̂��������݂���Ȃǃv���O�����S�̂̕��G���������Ă��܂��\���������B
 * ���������邽�߁A���ʃC���^�[�t�F�C�X����邱�Ƃɂ���Ċȗ������鎎��
 * �c���݂Ȃ̂œr���ŕ��j��ς��邩���B
 */
//============================================================================================
#pragma once

#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "field/location.h"
#include "field/rail_location.h"  //RAIL_LOCATION
#include "savedata/myitem_savedata.h"  //BAG_CURSOR MYITEM
#include "poke_tool/pokeparty.h"
#include "field/field_rail_loader.h"    //FIELD_RAIL_LOADER
#include "field/fldmmdl.h"
#include "field/eventwork.h"
#include "gamesystem/game_data.h"       //MAP_MATRIX
#include "field/field_sound_proc.h"
#include "sound/bgm_info.h"

//============================================================================================
//============================================================================================
enum {
	PLAYER_MAX = 5,
	
	PLAYER_ID_MINE = PLAYER_MAX - 1,    ///<�������g�̃v���C���[ID
};

///�}�b�v���[�h
typedef enum{
  MAPMODE_NORMAL,     ///<�ʏ���
  MAPMODE_INTRUDE,    ///<�N����
}MAPMODE;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	GAMEDATA�^��`
 *
 * _GAMEDATA�̎��̂�game_data.c���ɒ�`����Ă���
 */
//------------------------------------------------------------------
typedef struct _GAMEDATA GAMEDATA;

//============================================================================================
//	�Q�[���f�[�^�{�̂̐����E�j������
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	GAMEDATA�̐�������
 * @param	heapID		�g�p����q�[�v�w��ID
 * @return	GAMEDATA	��������GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMEDATA_Create(HEAPID heapID);
//------------------------------------------------------------------
/**
 * @brief	GAMEDATA�̔j������
 * @param	gamedata	�j������GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMEDATA_Delete(GAMEDATA * gamedata);

//============================================================================================
//	PLAYER_WORK�ւ̃A�N�Z�X
//============================================================================================
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

//============================================================================================
//	EVENTDATA_SYSTEM�ւ̃A�N�Z�X
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^���擾����
 * @param	gamedata			GAMEDATA�ւ̃|�C���^
 * @return	EVENTDATA_SYSTEM	�C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^
 */
//------------------------------------------------------------------
extern EVENTDATA_SYSTEM * GAMEDATA_GetEventData(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief	�J�n�ʒu�̎擾
 * @param	gamedata			GAMEDATA�ւ̃|�C���^
 * @return	LOCATION		�J�n�ʒu���ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetStartLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	�J�n�ʒu�̃Z�b�g
 * @param	gamedata
 * @param	loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetStartLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief	���O�̏o�������̎擾
 * @param	gamedata			GAMEDATA�ւ̃|�C���^
 * @return	LOCATION		���O�̏o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEntranceLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	���O�̏o�������̃Z�b�g
 * @param	gamedata
 * @param	loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetEntranceLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief	����o�������̎擾
 * @param	gamedata		GAMEDATA�ւ̃|�C���^
 * @return	LOCATION		����o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetSpecialLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	����o�������̃Z�b�g
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @param	loc			����o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetSpecialLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEscapeLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetEscapeLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern const RAIL_LOCATION * GAMEDATA_GetRailLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetRailLocation(GAMEDATA * gamedata, const RAIL_LOCATION * railLoc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetWarpID(GAMEDATA * gamedata, u16 warp_id);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern u16 GAMEDATA_GetWarpID(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�J�[�\���̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �o�b�O�J�[�\���|�C���^
 */
//--------------------------------------------------------------
extern BAG_CURSOR* GAMEDATA_GetBagCursor(const GAMEDATA * gamedata);
//--------------------------------------------------------------
/**
 * @brief   �莝���A�C�e���ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �莝���A�C�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYITEM_PTR GAMEDATA_GetMyItem(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   �莝���|�P�����ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �莝���A�C�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern POKEPARTY * GAMEDATA_GetMyPokemon(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS�ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatus(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS�ւ̃|�C���^�擾(�v���C���[ID�w��)
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   player_id   �v���C���[ID
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatusPlayer(GAMEDATA * gamedata, u32 player_id);

//--------------------------------------------------------------
/**
 * @brief FIELD_RAIL_LOADER�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @return  FIELD_RAIL_LOADER ���[���f�[�^���[�_�[�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern FIELD_RAIL_LOADER * GAMEDATA_GetFieldRailLoader(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief BGM_INFO_SYS�ւ̃|�C���^�擾
 * @param gamedata    GAMEDATA�ւ̃|�C���^
 * preturn BGM_INFO_SYS BGM���擾�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern BGM_INFO_SYS * GAMEDATA_GetBGMInfoSys(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief ���샂�f���ւ̃|�C���^�擾
 * @param gamedata GAMEDATA�ւ̃|�C���^
 * @retval MMDLSYS�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MMDLSYS * GAMEDATA_GetMMdlSys(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	�G�߂̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	u8	�G��ID�igamesystem/pm_season.h�Q�Ɓj
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	�G�߂̃Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	season_id	�G��ID�igamesystem/pm_season.h�Q�Ɓj
 *
 * ���ۂ̋@�\�Ƃ��ĕK�v���ǂ����͋^�₾���f�o�b�O�p�Ƃ��Ēǉ�
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSeasonID(GAMEDATA *gamedata, u8 season_id);

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʂ̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	subscreen_mode
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʂ̐ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  subscreen_mode
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode);

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʃ^�C�v�̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	subscreen_type
 */
//--------------------------------------------------------------
extern const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʃ^�C�v�̐ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  subscreen_type
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type);

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̋��t���O�ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  bEnable ��������ꍇTRUE
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFrameSpritEnable(GAMEDATA *gamedata,BOOL bEnable);

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̃J�E���^���Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̃t���[��
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern u8 GAMEDATA_GetAndAddFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삵�Ă��邩�ǂ���
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  ���삵�Ă���ꍇTRUE
 */
//------------------------------------------------------------------
extern BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata);


//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�����s
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	EVENTWORK�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @retval	EVENTWORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   MAP_MATRIX�ւ̃|�C���^�擾
 * @param	  gamedata	GAMEDATA�ւ̃|�C���^
 * @retval  MAP_MATRIX�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MAP_MATRIX * GAMEDATA_GetMapMatrix(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief	  �t�B�[���h�}�b�v�����J�E���g�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  int �����J�E���g
 */
//------------------------------------------------------------------
extern int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  �t�B�[���h�}�b�v�����J�E���g�Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param   count �Z�b�g����J�E���g
 * @return  nothing
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFieldMapWalkCount(GAMEDATA *gamedata,int count);

//--------------------------------------------------------------
/**
 * @brief FIELD_SOUND�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  FIELD_SOUND*
 */
//--------------------------------------------------------------
extern FIELD_SOUND * GAMEDATA_GetFieldSound( GAMEDATA *gamedata );

//==================================================================
/**
 * �}�b�v���[�h�ݒ�
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   map_mode		�}�b�v���[�h
 */
//==================================================================
extern void GAMEDATA_SetMapMode(GAMEDATA *gamedata, MAPMODE map_mode);

//==================================================================
/**
 * �}�b�v���[�h�擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  MAPMODE		�}�b�v���[�h
 */
//==================================================================
extern MAPMODE GAMEDATA_GetMapMode(const GAMEDATA *gamedata);

//==================================================================
/**
 * �N���ڑ��l���擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  int		�ڑ��l��
 */
//==================================================================
extern int GAMEDATA_GetIntrudeNum(const GAMEDATA *gamedata);

//==================================================================
/**
 * �N�����̎�����NetID���擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  int		������NetID
 */
//==================================================================
extern int GAMEDATA_GetIntrudeMyID(const GAMEDATA *gamedata);

//----------------------------------------------------------------------------
/**
 *	@brief	�펞�ʐM���[�h�t���O�擾
 *
 *	@param	gamedata		GAMEDATA�ւ̃|�C���^
 *	
 *	@return	TRUE�펞�ʐM���[�hON FALSE�펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetAlwaysNetFlag( const GAMEDATA *gamedata );

//----------------------------------------------------------------------------
/**
 *	@brief	�펞�ʐM���[�h�t���O�ݒ�
 *
 *	@param	gamedata		GAMEDATA�ւ̃|�C���^
 *	@param	is_on				TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
extern void GAMEDATA_SetAlwaysNetFlag( GAMEDATA *gamedata, BOOL is_on );

//============================================================================================
//	SAVE_CONTROL_WORK�ւ̃A�N�Z�X
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	  �Z�[�u�R���g���[�����[�N���擾����
 * @param	  gamedata			GAMEDATA�ւ̃|�C���^
 * @return	SAVE_CONTROL_WORK	�̃|�C���^
 */
//------------------------------------------------------------------
extern SAVE_CONTROL_WORK * GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata);



