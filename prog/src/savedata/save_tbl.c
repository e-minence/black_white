//==============================================================================
/**
 * @file  save.c
 * @brief WB�Z�[�u
 * @author  matsuda
 * @date  2008.08.27(��)
 */
//==============================================================================
#include <gflib.h>
#include <backup_system.h>
#include "gamesystem/game_data.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/box_savedata.h"
#include "savedata/myitem_savedata.h"
#include "poke_tool/pokeparty.h"
#include "playerdata_local.h"
#include "savedata/wifihistory.h"
#include "savedata/wifilist.h"
#include "savedata/c_gear_data.h"
#include "savedata/trainercard_data.h"
#include "savedata/mystery_data.h"
#include "savedata/dreamworld_data.h"
#include "savedata/situation.h"
#include "perapvoice_local.h"
#include "savedata/system_data.h"
#include "savedata/record.h"
#include "system/pms_word.h"
#include "savedata/mail_util.h"
#include "field/fldmmdl.h"
#include "savedata/musical_save.h"
#include "savedata/randommap_save.h"
#include "savedata/irc_compatible_savedata.h"
#include "savedata/worldtrade_data.h"
#include "savedata/regulation.h"
#include "field/eventwork.h"
#include "savedata/gimmickwork.h"
#include "savedata/battle_rec.h"
#include "savedata/battle_box_save.h"
#include "savedata/sodateya_work.h"
#include "savedata/wifi_negotiation.h"
#include "savedata/misc.h"
#include "savedata/intrude_save.h"
#include "savedata/shortcut.h"
#include "savedata/musical_dist_save.h"
#include "savedata/zukan_savedata.h"
#include "savedata/encount_sv.h"
#include "savedata/c_gear_picture.h"
#include "savedata/bsubway_savedata.h"

//==============================================================================
//  �萔��`
//==============================================================================
#define MAGIC_NUMBER  (0x31053527)
#define SECTOR_MAX    (SAVE_PAGE_MAX)

///�m�[�}���̈�Ŏg�p����Z�[�u�T�C�Y
#define SAVESIZE_NORMAL   (SAVE_SECTOR_SIZE * SECTOR_MAX)

///�~���[�����O�̈�Ŏg�p����Z�[�u�J�n�A�h���X
#define MIRROR_SAVE_ADDRESS   (0x24000)
///�O���Z�[�u���g�p����Z�[�u�J�n�A�h���X
#define EXTRA_SAVE_START_ADDRESS    (0x48000)

///�O���Z�[�u�F�������}�b�s���O
enum{
  EXTRA_MM_REC_MINE = EXTRA_SAVE_START_ADDRESS,
  EXTRA_MM_REC_MINE_MIRROR = EXTRA_MM_REC_MINE + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_0 = EXTRA_MM_REC_MINE_MIRROR + SAVESIZE_EXTRA_BATTLE_REC,
  EXTRA_MM_REC_DL_0_MIRROR = EXTRA_MM_REC_DL_0 + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_1 = EXTRA_MM_REC_DL_0_MIRROR + SAVESIZE_EXTRA_BATTLE_REC,
  EXTRA_MM_REC_DL_1_MIRROR = EXTRA_MM_REC_DL_1 + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_2 = EXTRA_MM_REC_DL_1_MIRROR + SAVESIZE_EXTRA_BATTLE_REC,
  EXTRA_MM_REC_DL_2_MIRROR = EXTRA_MM_REC_DL_2 + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_CGEAR = EXTRA_MM_REC_DL_2_MIRROR + SAVESIZE_EXTRA_BATTLE_REC,
  EXTRA_MM_STREAMING = EXTRA_MM_CGEAR + SAVESIZE_EXTRA_CGEAR_PICTURE,
};


//--------------------------------------------------------------
//  �ʏ�Z�[�u�f�[�^�̃e�[�u��
//--------------------------------------------------------------
static const GFL_SAVEDATA_TABLE SaveDataTbl_Normal[] = {
  { //BOX�f�[�^
    GMDATA_ID_BOXDATA,
    (FUNC_GET_SIZE)BOXDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXDAT_Init,
  },
  { //BOX�g���[�f�[�^01
    GMDATA_ID_BOXTRAY_01,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^02
    GMDATA_ID_BOXTRAY_02,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^03
    GMDATA_ID_BOXTRAY_03,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^04
    GMDATA_ID_BOXTRAY_04,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^05
    GMDATA_ID_BOXTRAY_05,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^06
    GMDATA_ID_BOXTRAY_06,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^07
    GMDATA_ID_BOXTRAY_07,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^08
    GMDATA_ID_BOXTRAY_08,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^09
    GMDATA_ID_BOXTRAY_09,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^10
    GMDATA_ID_BOXTRAY_10,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^11
    GMDATA_ID_BOXTRAY_11,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^12
    GMDATA_ID_BOXTRAY_12,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^13
    GMDATA_ID_BOXTRAY_13,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^14
    GMDATA_ID_BOXTRAY_14,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^15
    GMDATA_ID_BOXTRAY_15,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^16
    GMDATA_ID_BOXTRAY_16,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^17
    GMDATA_ID_BOXTRAY_17,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^18
    GMDATA_ID_BOXTRAY_18,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^19
    GMDATA_ID_BOXTRAY_19,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^20
    GMDATA_ID_BOXTRAY_20,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^21
    GMDATA_ID_BOXTRAY_21,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^22
    GMDATA_ID_BOXTRAY_22,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^23
    GMDATA_ID_BOXTRAY_23,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //BOX�g���[�f�[�^24
    GMDATA_ID_BOXTRAY_24,
    (FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
    (FUNC_INIT_WORK)BOXTRAYDAT_Init,
  },
  { //�莝���A�C�e��
    GMDATA_ID_MYITEM,
    (FUNC_GET_SIZE)MYITEM_GetWorkSize,
    (FUNC_INIT_WORK)MYITEM_Init,
  },
  { //�莝���|�P����
    GMDATA_ID_MYPOKE,
    (FUNC_GET_SIZE)PokeParty_GetWorkSize,
    (FUNC_INIT_WORK)PokeParty_InitWork,
  },
  { //�v���C���[�f�[�^
    GMDATA_ID_PLAYER_DATA,
    (FUNC_GET_SIZE)PLAYERDATA_GetWorkSize,
    (FUNC_INIT_WORK)PLAYERDATA_Init,
  },
  { //�󋵃f�[�^
    GMDATA_ID_SITUATION,
    (FUNC_GET_SIZE)Situation_GetWorkSize,
    (FUNC_INIT_WORK)Situation_Init,
  },
  { //WiFi�ʐM�����f�[�^
    GMDATA_ID_WIFIHISTORY,
    (FUNC_GET_SIZE)WIFIHISTORY_GetWorkSize,
    (FUNC_INIT_WORK)WIFIHISTORY_Init,
  },
  { //WiFi�F�B�R�[�h�f�[�^
    GMDATA_ID_WIFILIST,
    (FUNC_GET_SIZE)WifiList_GetWorkSize,
    (FUNC_INIT_WORK)WifiList_Init,
  },
  { //WiFi�l�S�V�G�[�V����
    GMDATA_ID_WIFI_NEGOTIATION,
    (FUNC_GET_SIZE)WIFI_NEGOTIATION_SV_GetWorkSize,
    (FUNC_INIT_WORK)WIFI_NEGOTIATION_SV_Init,
  },
  { // C-GEAR
    GMDATA_ID_CGEAR,
    (FUNC_GET_SIZE)CGEAR_SV_GetWorkSize,
    (FUNC_INIT_WORK)CGEAR_SV_Init,
  },
  { //�g���[�i�[�J�[�h(�T�C��
    GMDATA_ID_TRCARD,
    (FUNC_GET_SIZE)TRCSave_GetSaveDataSize,
    (FUNC_INIT_WORK)TRCSave_InitSaveData,
  },
  { //�s�v�c�ȑ��蕨
    GMDATA_ID_MYSTERYDATA,
    (FUNC_GET_SIZE)MYSTERYDATA_GetWorkSize,
    (FUNC_INIT_WORK)MYSTERYDATA_Init,
  },
  { //PDW
    GMDATA_ID_DREAMWORLD,
    (FUNC_GET_SIZE)DREAMWORLD_SV_GetWorkSize,
    (FUNC_INIT_WORK)DREAMWORLD_SV_Init,
  },
  { //�؃��b�v�{�C�X
    GMDATA_ID_PERAPVOICE,
    (FUNC_GET_SIZE)PERAPVOICE_GetWorkSize,
    (FUNC_INIT_WORK)PERAPVOICE_Init,
  },
  { //�V�X�e���f�[�^
    GMDATA_ID_SYSTEM_DATA,
    (FUNC_GET_SIZE)SYSTEMDATA_GetWorkSize,
    (FUNC_INIT_WORK)SYSTEMDATA_Init,
  },
  { //���R�[�h(�X�R�A
    GMDATA_ID_RECORD,
    (FUNC_GET_SIZE)RECORD_GetWorkSize,
    (FUNC_INIT_WORK)RECORD_Init,
  },
  { //�ȈՕ�
    GMDATA_ID_PMS,
    (FUNC_GET_SIZE)PMSW_GetSaveDataSize,
    (FUNC_INIT_WORK)PMSW_InitSaveData,
  },
  { //���[���f�[�^
    GMDATA_ID_MAILDATA,
    (FUNC_GET_SIZE)MAIL_GetBlockWorkSize,
    (FUNC_INIT_WORK)MAIL_Init,
  },
  {//���샂�f��
    GMDATA_ID_MMDL,
    (FUNC_GET_SIZE)MMDL_SAVEDATA_GetWorkSize,
    (FUNC_INIT_WORK)MMDL_SAVEDATA_Init,
  },
  { //�~���[�W�J��
    GMDATA_ID_MUSICAL,
    (FUNC_GET_SIZE)MUSICAL_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)MUSICAL_SAVE_InitWork,
  },
  { //�����_�������}�b�v
    GMDATA_ID_RANDOMMAP,  
    (FUNC_GET_SIZE)RANDOMMAP_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)RANDOMMAP_SAVE_InitWork,
  },
  { //�����`�F�b�N
    GMDATA_ID_IRCCOMPATIBLE,
    (FUNC_GET_SIZE)IRC_COMPATIBLE_SV_GetWorkSize,
    (FUNC_INIT_WORK)IRC_COMPATIBLE_SV_Init,
  },
  { //�C�x���g���[�N
    GMDATA_ID_EVENT_WORK,
    (FUNC_GET_SIZE)EVENTWORK_GetWorkSize,
    (FUNC_INIT_WORK)EVENTWORK_InitWork,
  },
  { //GTS
    GMDATA_ID_WORLDTRADEDATA,
    (FUNC_GET_SIZE)WorldTradeData_GetWorkSize,
    (FUNC_INIT_WORK)WorldTradeData_Init,
  },
  { //REGULATION
    GMDATA_ID_REGULATION_DATA,
    (FUNC_GET_SIZE)RegulationData_GetWorkSize,
    (FUNC_INIT_WORK)RegulationData_Init,
  },
  { //�t�B�[���h�M�~�b�N���[�N
    GMDATA_ID_GIMMICK_WORK,
    (FUNC_GET_SIZE)GIMMICKWORK_GetWorkSize,
    (FUNC_INIT_WORK)GIMMICKWORK_Init,
  },
  { //�o�g���{�b�N�X
    GMDATA_ID_BATTLE_BOX,
    (FUNC_GET_SIZE)BATTLE_BOX_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)BATTLE_BOX_SAVE_InitWork,
  },
  { // ��ĉ����[�N
    GMDATA_ID_SODATEYA_WORK,
    (FUNC_GET_SIZE)SODATEYA_WORK_GetWorkSize,
    (FUNC_INIT_WORK)SODATEYA_WORK_InitWork,
  },
  { // �����肫��ꏊ�e�[�u��
    GMDATA_ID_ROCKPOS,
    (FUNC_GET_SIZE)MMDL_ROCKPOS_GetWorkSize,
    (FUNC_INIT_WORK)MMDL_ROCKPOS_Init,
  },
  { //�����ރ��[�N
    GMDATA_ID_MISC,
    (FUNC_GET_SIZE)MISC_GetWorkSize,
    (FUNC_INIT_WORK)MISC_Init,
  },
  { //�N���Z�[�u�f�[�^
    GMDATA_ID_INTRUDE,
    (FUNC_GET_SIZE)IntrudeSave_GetWorkSize,
    (FUNC_INIT_WORK)IntrudeSave_WorkInit,
  },
  { //Y�{�^���o�^�Z�[�u�f�[�^
    GMDATA_ID_SHORTCUT,
    (FUNC_GET_SIZE)SHORTCUT_GetWorkSize,
    (FUNC_INIT_WORK)SHORTCUT_Init,
  },
  { //�|�P�����}�ӃZ�[�u�f�[�^
    GMDATA_ID_ZUKAN,
    (FUNC_GET_SIZE)ZUKANSAVE_GetWorkSize,
    (FUNC_INIT_WORK)ZUKANSAVE_Init,
  },
  { //�G���J�E���g�֘A�Z�[�u�f�[�^
    GMDATA_ID_ENCOUNT,
    (FUNC_GET_SIZE)EncDataSave_GetWorkSize,
    (FUNC_INIT_WORK)EncDataSave_Init,
  },
  { //�o�g���T�u�E�F�C�@�v���C�f�[�^
    GMDATA_ID_BSUBWAY_PLAYDATA,
    (FUNC_GET_SIZE)BSUBWAY_PLAYDATA_GetWorkSize,
    (FUNC_INIT_WORK)BSUBWAY_PLAYDATA_Init,
  },
  { //�o�g���T�u�E�F�C�@�X�R�A�f�[�^
    GMDATA_ID_BSUBWAY_SCOREDATA,
    (FUNC_GET_SIZE)BSUBWAY_SCOREDATA_GetWorkSize,
    (FUNC_INIT_WORK)BSUBWAY_SCOREDATA_Init,
  },
};

//--------------------------------------------------------------
//  �O���Z�[�u�f�[�^�̃e�[�u��(�~���[�����O�L)
//--------------------------------------------------------------
///�퓬�^��F���� 
///  ��GFL_SAVEDATA_TABLE�S�̂Ń������͊m�ۂ���܂��B
///    ���̈׃T�C�Y���傫���^��f�[�^�͈��ʃe�[�u���ɂ��Ă��܂�)
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_RecMine[] = {
  {
    EXGMDATA_ID_BATTLE_REC_MINE,
    (FUNC_GET_SIZE)BattleRec_GetWorkSize,
    (FUNC_INIT_WORK)BattleRec_WorkInit,
  },
};

///�퓬�^��F�_�E�����[�h0��
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_RecDl_0[] = {
  {
    EXGMDATA_ID_BATTLE_REC_DL_0,
    (FUNC_GET_SIZE)BattleRec_GetWorkSize,
    (FUNC_INIT_WORK)BattleRec_WorkInit,
  },
};

///�퓬�^��F�_�E�����[�h1��
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_RecDl_1[] = {
  {
    EXGMDATA_ID_BATTLE_REC_DL_1,
    (FUNC_GET_SIZE)BattleRec_GetWorkSize,
    (FUNC_INIT_WORK)BattleRec_WorkInit,
  },
};

///�퓬�^��F�_�E�����[�h2��
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_RecDl_2[] = {
  {
    EXGMDATA_ID_BATTLE_REC_DL_2,
    (FUNC_GET_SIZE)BattleRec_GetWorkSize,
    (FUNC_INIT_WORK)BattleRec_WorkInit,
  },
};

//--------------------------------------------------------------
//  �O���Z�[�u�f�[�^�̃e�[�u��(�~���[�����O��)
//--------------------------------------------------------------
///CGEAR�G�̃f�[�^
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_CGEARPicture[] = {
  {
    EXGMDATA_ID_CGEAR_PICTURE,
    (FUNC_GET_SIZE)CGEAR_PICTURE_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)CGEAR_PICTURE_SAVE_Init,
  },
};
///�X�g���[�~���O
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_Streaming[] = {
  {
    EXGMDATA_ID_STREAMING,
    (FUNC_GET_SIZE)MUSICAL_DIST_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)MUSICAL_DIST_SAVE_InitWork,
  },
};


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�p�����[�^�e�[�u��
 */
//--------------------------------------------------------------
const GFL_SVLD_PARAM SaveParam_Normal = {
  SaveDataTbl_Normal,
  NELEMS(SaveDataTbl_Normal),
  0,                //�o�b�N�A�b�v�̈�擪�A�h���X
  MIRROR_SAVE_ADDRESS,      //�~���[�����O�̈�擪�A�h���X
  SAVESIZE_NORMAL,        //�g�p����o�b�N�A�b�v�̈�̑傫��
  MAGIC_NUMBER,
};

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�p�����[�^�e�[�u��
 *          ��SAVE_EXTRA_ID�ƕ��т𓯂��ɂ��Ă������ƁI�I
 */
//--------------------------------------------------------------
const GFL_SVLD_PARAM SaveParam_ExtraTbl[] = {
  {//�퓬�^��F����
    SaveDataTbl_Extra_RecMine,
    NELEMS(SaveDataTbl_Extra_RecMine),
    EXTRA_MM_REC_MINE,              //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_REC_MINE_MIRROR,       //�~���[�����O�̈�擪�A�h���X
    SAVESIZE_EXTRA_BATTLE_REC,      //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
  {//�퓬�^��F�_�E�����[�h0��
    SaveDataTbl_Extra_RecDl_0,
    NELEMS(SaveDataTbl_Extra_RecDl_0),
    EXTRA_MM_REC_DL_0,              //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_REC_DL_0_MIRROR,       //�~���[�����O�̈�擪�A�h���X
    SAVESIZE_EXTRA_BATTLE_REC,      //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
  {//�퓬�^��F�_�E�����[�h1��
    SaveDataTbl_Extra_RecDl_1,
    NELEMS(SaveDataTbl_Extra_RecDl_1),
    EXTRA_MM_REC_DL_1,              //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_REC_DL_1_MIRROR,       //�~���[�����O�̈�擪�A�h���X
    SAVESIZE_EXTRA_BATTLE_REC,      //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
  {//�퓬�^��F�_�E�����[�h2��
    SaveDataTbl_Extra_RecDl_2,
    NELEMS(SaveDataTbl_Extra_RecDl_2),
    EXTRA_MM_REC_DL_2,              //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_REC_DL_2_MIRROR,       //�~���[�����O�̈�擪�A�h���X
    SAVESIZE_EXTRA_BATTLE_REC,      //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
  {//�O���Z�[�u�p�����[�^�e�[�u���F�X�g���[�~���O(�~���[�����O��)
    SaveDataTbl_Extra_CGEARPicture,
    NELEMS(SaveDataTbl_Extra_CGEARPicture),
    EXTRA_MM_CGEAR,             //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_CGEAR,             //�~���[�����O�̈�擪�A�h���X ���~���[�����w��
    SAVESIZE_EXTRA_CGEAR_PICTURE,       //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
  {//�O���Z�[�u�p�����[�^�e�[�u���F�X�g���[�~���O(�~���[�����O��)
    SaveDataTbl_Extra_Streaming,
    NELEMS(SaveDataTbl_Extra_Streaming),
    EXTRA_MM_STREAMING,             //�o�b�N�A�b�v�̈�擪�A�h���X
    EXTRA_MM_STREAMING,             //�~���[�����O�̈�擪�A�h���X ���~���[�����w��
    SAVESIZE_EXTRA_STREAMING,       //�g�p����o�b�N�A�b�v�̈�̑傫��
    MAGIC_NUMBER,
  },
};
SDK_COMPILER_ASSERT(NELEMS(SaveParam_ExtraTbl) == SAVE_EXTRA_ID_MAX);

