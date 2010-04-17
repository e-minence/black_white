//============================================================================================
/**
 * @file  game_data.h
 * @brief �Q�[���i�s�f�[�^�Ǘ�
 * @date  2008.11.04
 *
 */
//============================================================================================
#pragma once

//============================================================================================
//============================================================================================
#include "gamesystem/gamedata_def.h"

#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "field/location.h"
#include "field/rail_location.h"       //RAIL_LOCATION
#include "savedata/myitem_savedata.h"  //BAG_CURSOR MYITEM
#include "poke_tool/pokeparty.h"

#include "field/eventwork.h"
#include "field/field_sound_proc.h"
#include "sound/bgm_info.h"

#include "field/game_beacon_search.h"
#include "savedata/mystatus.h"

//#include "field/field_encount.h"     //ENCOUNT_WORK
#include "savedata/box_savedata.h"     //BOX_MANAGER
//#include "savedata/intrude_save.h"   //OCCUPY_INFO
#include "system/shortcut_data.h"      //SHORTCUT_ID
//#include "savedata/gimmickwork.h"
#include "field/pleasure_boat_ptr.h"
#include "savedata/wifilist.h"
#include "savedata/misc.h"
#include "savedata/wifi_negotiation.h"
#include "savedata/record.h"
#include "savedata/musical_save.h"     //MUSICAL_SAVE
#include "musical/musical_define.h"    //MUSICAL_SCRIPT_WORK
#include "gamesystem/comm_player_support.h"
#include "savedata/dreamworld_data.h"  //DREAMWORLD_SAVEDATA
#include "savedata/dendou_save.h"      // DENDOU_RECORD
#include "savedata/perapvoice.h"       // PERAPVOICE
#include "savedata/playtime.h"         // PLAYTIME

#include "field/bsubway_scr_proc.h"

#include "field/calender.h"
#include "savedata/un_savedata.h"

#include "field/trial_house_ptr.h"
#include "savedata/trainercard_data.h"

//============================================================================================
//============================================================================================

//============================================================================================
//  �Q�[���f�[�^�{�̂̐����E�j������
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief GAMEDATA�̐�������
 * @param heapID    �g�p����q�[�v�w��ID
 * @return  GAMEDATA  ��������GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMEDATA_Create(HEAPID heapID);
//------------------------------------------------------------------
/**
 * @brief GAMEDATA�̔j������
 * @param gamedata  �j������GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMEDATA_Delete(GAMEDATA * gamedata);

//============================================================================================
//    �Z�[�u����
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u���J�n
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_SaveAsyncStart(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_SaveAsyncMain(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �����Z�[�u���L�����Z������
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_SaveAsyncCancel(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�̕����Z�[�u�J�n
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 * @param   extra_id    �O���Z�[�u�f�[�^�ԍ�
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_ExtraSaveAsyncStart(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id);

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�̕����Z�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 * @param   extra_id    �O���Z�[�u�f�[�^�ԍ�
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_ExtraSaveAsyncMain(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id);

//==================================================================
/**
 * �Z�[�u���s�������ׂ�
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  BOOL        TRUE:�Z�[�u���s���@�@FALSE:�Z�[�u���Ă��Ȃ�
 */
//==================================================================
extern BOOL GAMEDATA_GetIsSave(const GAMEDATA *gamedata);

//============================================================================================
//  SAVE_CONTROL_WORK�ւ̃A�N�Z�X
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   �Z�[�u�R���g���[�����[�N���擾����
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  SAVE_CONTROL_WORK �̃|�C���^
 */
//------------------------------------------------------------------
extern SAVE_CONTROL_WORK * GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief   �Z�[�u�R���g���[�����[�N���擾����  CONST��
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  SAVE_CONTROL_WORK �̃|�C���^
 */
//------------------------------------------------------------------
extern const SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWorkConst(const GAMEDATA * gamedata);





//============================================================================================
//  EVENTDATA_SYSTEM�ւ̃A�N�Z�X
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^���擾����
 * @param gamedata      GAMEDATA�ւ̃|�C���^
 * @return  EVENTDATA_SYSTEM  �C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^
 */
//------------------------------------------------------------------
extern EVENTDATA_SYSTEM * GAMEDATA_GetEventData(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief �J�n�ʒu�̎擾
 * @param gamedata      GAMEDATA�ւ̃|�C���^
 * @return  LOCATION    �J�n�ʒu���ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetStartLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief �J�n�ʒu�̃Z�b�g
 * @param gamedata
 * @param loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetStartLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief ���O�̏o�������̎擾
 * @param gamedata      GAMEDATA�ւ̃|�C���^
 * @return  LOCATION    ���O�̏o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEntranceLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief ���O�̏o�������̃Z�b�g
 * @param gamedata
 * @param loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetEntranceLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief ����o�������̎擾
 * @param gamedata    GAMEDATA�ւ̃|�C���^
 * @return  LOCATION    ����o�������ւ̃|�C���^
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetSpecialLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief ����o�������̃Z�b�g
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @param loc     ����o�������ւ̃|�C���^
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
extern const LOCATION * GAMEDATA_GetPalaceReturnLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetPalaceReturnLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetWarpID(GAMEDATA * gamedata, u16 warp_id);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern u16 GAMEDATA_GetWarpID(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�J�[�\���̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  �o�b�O�J�[�\���|�C���^
 */
//--------------------------------------------------------------
extern BAG_CURSOR* GAMEDATA_GetBagCursor(const GAMEDATA * gamedata);
//--------------------------------------------------------------
/**
 * @brief   �莝���A�C�e���ւ̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  �莝���A�C�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYITEM_PTR GAMEDATA_GetMyItem(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   �莝���|�P�����ւ̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  �莝���A�C�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern POKEPARTY * GAMEDATA_GetMyPokemon(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   �{�b�N�X�Ǘ��\���̂̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  �{�b�N�X�Ǘ��\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------
extern BOX_MANAGER * GAMEDATA_GetBoxManager(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS�ւ̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatus(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS�ւ̃|�C���^�擾(�v���C���[ID�w��)
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @param   player_id   �v���C���[ID
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatusPlayer(GAMEDATA * gamedata, u32 player_id);

//--------------------------------------------------------------
/**
 * @brief �J�����_�[�̎擾
 *
 * @param  gamedata GAMEDATA�ւ̃|�C���^
 * @return CALENDER
 */
//--------------------------------------------------------------
extern CALENDER* GAMEDATA_GetCalender( GAMEDATA * gamedata );

//--------------------------------------------------------------
/**
 * @brief �G�߂̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  u8  �G��ID�igamesystem/pm_season.h�Q�Ɓj
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata);



//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʂ̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  subscreen_mode
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʂ̐ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   subscreen_mode
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode);

//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʃ^�C�v�̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  subscreen_type
 */
//--------------------------------------------------------------
extern const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʃ^�C�v�̐ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   subscreen_type
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type);

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삷��ꍇ�̋��t���O�ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   bEnable ��������ꍇTRUE
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFrameSpritEnable(GAMEDATA *gamedata,BOOL bEnable);

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삷��ꍇ�̃J�E���^���Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삷��ꍇ�̃t���[��
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern u8 GAMEDATA_GetAndAddFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삵�Ă��邩�ǂ���
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  ���삵�Ă���ꍇTRUE
 */
//------------------------------------------------------------------
extern BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata);


//--------------------------------------------------------------
/**
 * @brief EVENTWORK�ւ̃|�C���^�擾
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @retval  EVENTWORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   �t�B�[���h�}�b�v�����J�E���g�擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  int �����J�E���g
 */
//------------------------------------------------------------------
extern int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   �t�B�[���h�}�b�v�����J�E���g�Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   count �Z�b�g����J�E���g
 * @return  nothing
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFieldMapWalkCount(GAMEDATA *gamedata,int count);

//--------------------------------------------------------------
/**
 * @brief FIELD_SOUND�擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  FIELD_SOUND*
 */
//--------------------------------------------------------------
extern FIELD_SOUND * GAMEDATA_GetFieldSound( GAMEDATA *gamedata );

//--------------------------------------------------------------
/**
 * @brief �ŐV�̐퓬���ʂ��Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param btl_result  �퓬���ʁiinclude/battle/battle.h��BtlResult�j
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetLastBattleResult( GAMEDATA * gamedata, u32 btl_result );
//--------------------------------------------------------------
/**
 * @brief �ŐV�̐퓬���ʂ��擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  u32 �ŐV�̐퓬���ʁiinclude/battle/battle.h��BtlResult�j
 */
//--------------------------------------------------------------
extern u32 GAMEDATA_GetLastBattleResult( const GAMEDATA * gamedata );

//==================================================================
/**
 * �N���ڑ��l���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   �ڑ��l��
 */
//==================================================================
extern int GAMEDATA_GetIntrudeNum(const GAMEDATA *gamedata);

//==================================================================
/**
 * �N�����̎�����NetID���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   ������NetID
 */
//==================================================================
extern int GAMEDATA_GetIntrudeMyID(const GAMEDATA *gamedata);

//==================================================================
/**
 * �N�����̎�����NetID���Z�b�g
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   ������NetID
 */
//==================================================================
extern void GAMEDATA_SetIntrudeMyID(GAMEDATA *gamedata, int intrude_my_id);

//==================================================================
/**
 * ���t�B�[���h�N���t���O���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  u8  TRUE:���t�B�[���h�Z�����@FALSE:�\�t�B�[���h
 */
//==================================================================
extern int GAMEDATA_GetIntrudeReverseArea(const GAMEDATA *gamedata);

//==================================================================
/**
 * ���t�B�[���h�N���t���O���Z�b�g
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @param   reverse_flag  TRUE:���t�B�[���h�N����
 */
//==================================================================
extern void GAMEDATA_SetIntrudeReverseArea(GAMEDATA *gamedata, u8 reverse_flag);

//----------------------------------------------------------------------------
/**
 *  @brief  �펞�ʐM���[�h�t���O�擾
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  
 *  @return TRUE�펞�ʐM���[�hON FALSE�펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetAlwaysNetFlag( const GAMEDATA *gamedata );

//----------------------------------------------------------------------------
/**
 *  @brief  �펞�ʐM���[�h�t���O�ݒ�
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  is_on       TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
extern void GAMEDATA_SetAlwaysNetFlag( GAMEDATA *gamedata, BOOL is_on );

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���o�^�ݒ�
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  shortcutID  Y�{�^���o�^����ID
 *  @param  is_on       TRUE�Ȃ��Y�{�^���o�^ FALSE�Ȃ�Ή���
 */
//----------------------------------------------------------------------------
extern void GAMEDATA_SetShortCut( GAMEDATA *gamedata, SHORTCUT_ID shortcutID, BOOL is_on );

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���o�^�擾
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  shortcutID  Y�{�^���o�^����ID
 *  @retval TRUE�Ȃ��YBTN�ɓo�^���Ă���  FALSE�Ȃ�Γo�^���Ă��Ȃ�
 */
//----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetShortCut( const GAMEDATA *gamedata, SHORTCUT_ID shortcutID );

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���p���j���[�̃J�[�\���Z�b�g
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @param  cursor    �J�[�\��
 */
//-----------------------------------------------------------------------------
extern void GAMEDATA_SetShortCutCursor( GAMEDATA *gamedata, SHORTCUT_CURSOR *cursor );

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���p���j���[�̃J�[�\���擾
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @retval cursor    �J�[�\��
 */
//-----------------------------------------------------------------------------
extern SHORTCUT_CURSOR * GAMEDATA_GetShortCutCursor( GAMEDATA *gamedata );

//----------------------------------------------------------
/**
 * @brief   �V���D�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �V���D���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
extern PL_BOAT_WORK_PTR *GAMEDATA_GetPlBoatWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------------------------
/**
 *  @brief  GAMEDATA�����WIFIList�̎擾
 *  @param  gamedata      �Q�[���f�[�^
 *  @return WIFIList�̃|�C���^
 */
//-----------------------------------------------------------------------------
extern WIFI_LIST * GAMEDATA_GetWiFiList(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   �o�g���T�u�E�F�C���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  BSUBWAY_SCRWORK*
 */
//----------------------------------------------------------
extern BSUBWAY_SCRWORK * GAMEDATA_GetBSubwayScrWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   MISC���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  MISC*
 */
//----------------------------------------------------------
extern MISC * GAMEDATA_GetMiscWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   WIFI_NEGOTIATION_SAVEDATA���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  WIFI_NEGOTIATION_SAVEDATA*
 */
//----------------------------------------------------------
extern WIFI_NEGOTIATION_SAVEDATA * GAMEDATA_GetWifiNegotiation(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief RECORD���[�N�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  RECORD *    
 */
//----------------------------------------------------------
extern RECORD *GAMEDATA_GetRecordPtr(GAMEDATA * gamedata);


//----------------------------------------------------------
/**
 * @brief   �o�g���T�u�E�F�C���[�N�Z�b�g
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  nothing
 */
//----------------------------------------------------------
extern void GAMEDATA_SetBSubwayScrWork(GAMEDATA * gamedata, BSUBWAY_SCRWORK *bsw_scr );


//----------------------------------------------------------
/**
 * @brief   �g���[�i�[�J�[�h�����N�擾�i�\�[�X��trcard_sys.c)make
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  int         �����N�l
 */
//----------------------------------------------------------
extern int TRAINERCARD_GetCardRank( GAMEDATA *gameData );

//----------------------------------------------------------
/**
 * @brief ���A���[�N�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  UNSV_WORK *    
 */
//----------------------------------------------------------
extern UNSV_WORK *GAMEDATA_GetUnsvWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief �~���[�W�J���Z�[�u�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  UNSV_WORK *    
 */
//----------------------------------------------------------
extern MUSICAL_SAVE *GAMEDATA_GetMusicalSavePtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   �g���C�A���n�E�X���[�N�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �g���C�A�����[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
extern TRIAL_HOUSE_WORK_PTR *GAMEDATA_GetTrialHouseWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   �g���[�i�[�J�[�h�f�[�^�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �g���[�i�[�J�[�h�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
extern TR_CARD_SV_PTR GAMEDATA_GetTrainerCardPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   ���N���A�|�P�����f�[�^�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �a������f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
extern DENDOU_RECORD *GAMEDATA_GetDendouRecord(GAMEDATA * gamedata);


//----------------------------------------------------------
/**
 * @brief   �~���[�W�J���p�X�N���v�g���[�N�Z�b�g
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  nothing
 */
//----------------------------------------------------------
extern void GAMEDATA_SetMusicalScrWork(GAMEDATA * gamedata, MUSICAL_SCRIPT_WORK* musScrWork );

//----------------------------------------------------------
/**
 * @brief   �~���[�W�J���p�X�N���v�g���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  MUSICAL_SCRIPT_WORK*
 */
//----------------------------------------------------------
extern MUSICAL_SCRIPT_WORK* GAMEDATA_GetMusicalScrWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   PDW�̃Z�[�u�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  DREAMWORLD_SAVEDATA*
 */
//----------------------------------------------------------
extern DREAMWORLD_SAVEDATA* GAMEDATA_GetDreamWorkdSaveWork(GAMEDATA * gamedata);


//=============================================================================================
/**
 * @brief �y���b�v�^���f�[�^�̃|�C���^���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  extern PERAPVOICE *   �y���b�v�^���f�[�^�ւ̃|�C���^
 */
//=============================================================================================
extern PERAPVOICE *GAMEDATA_GetPerapVoice( GAMEDATA *gamedata );

//==================================================================
/**
 * �V���{���}�b�v�̌��݈ʒu���Z�b�g����
 *
 * @param   gamedata    
 * @param   symbol_map_id   �V���{���}�b�vID
 */
//==================================================================
extern void GAMEDATA_SetSymbolMapID(GAMEDATA *gamedata, u8 symbol_map_id);

//==================================================================
/**
 * �V���{���}�b�v�̌��݈ʒu���擾����
 *
 * @param   gamedata    
 *
 * @retval  u8    �V���{���}�b�vID
 */
//==================================================================
extern u8 GAMEDATA_GetSymbolMapID(const GAMEDATA *gamedata);


//==================================================================
/**
 * PLAYTIME �J�E���g����
 *
 * @param   gamedata    
 * @param   value       �J�E���g�l
 */
//==================================================================
extern void GAMEDATA_PlayTimeCountUp(GAMEDATA *gamedata, u32 value);


//==================================================================
/**
 * PLAYTIME �Z�[�u���Ԃ̐ݒ�
 *
 * @param   gamedata    
 */
//==================================================================
extern void GAMEDATA_PlayTimeSetSaveTime(GAMEDATA *gamedata);


//==================================================================
/**
 * PLAYTIME �擾 
 *
 * @param   gamedata    
 *
 * @return PLAYTIME (GAMEDATA��������alloc�������[�N)
 */
//==================================================================
extern PLAYTIME* GAMEDATA_GetPlayTimeWork(GAMEDATA *gamedata);



