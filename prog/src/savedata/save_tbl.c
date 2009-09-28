//==============================================================================
/**
 * @file	save.c
 * @brief	WB�Z�[�u
 * @author	matsuda
 * @date	2008.08.27(��)
 */
//==============================================================================
#include <gflib.h>
#include <backup_system.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/contest_savedata.h"
#include "savedata/box_savedata.h"
#include "savedata/myitem_savedata.h"
#include "poke_tool/pokeparty.h"
#include "playerdata_local.h"
#include "savedata/wifihistory.h"
#include "savedata/wifilist.h"
#include "savedata/c_gear_data.h"
#include "savedata/trainercard_data.h"
#include "savedata/mystery_data.h"
#include "savedata/situation.h"
#include "perapvoice_local.h"
#include "savedata/system_data.h"
#include "savedata/record.h"
#include "system/pms_word.h"
#include "field/fldmmdl.h"
#include "savedata/wifi_hiroba_save.h"
#include "savedata/musical_save.h"
#include "savedata/randommap_save.h"
#include "savedata/irc_compatible_savedata.h"
#include "savedata/sp_ribbon_save.h"
#include "savedata/worldtrade_data.h"
#include "savedata/regulation.h"
#include "field/eventwork.h"
#include "savedata/gimmickwork.h"
#include "savedata/battle_rec.h"
#include "savedata/battle_box_save.h"
#include "savedata/sodateya_work.h"

//==============================================================================
//	�萔��`
//==============================================================================
#define MAGIC_NUMBER	(0x31053527)
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

///�m�[�}���̈�Ŏg�p����Z�[�u�T�C�Y
#define SAVESIZE_NORMAL		(SECTOR_SIZE * SECTOR_MAX)
///EXTRA�̈�Ő퓬�^�悪�g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_BATTLE_REC		(SECTOR_SIZE * 2)
///EXTRA�̈�ŃX�g���[�~���O���g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_STREAMING		(SECTOR_SIZE * 32)  //128Kbyte

///�~���[�����O�̈�Ŏg�p����Z�[�u�J�n�A�h���X
#define MIRROR_SAVE_ADDRESS		(0x24000)
///�O���Z�[�u���g�p����Z�[�u�J�n�A�h���X
#define EXTRA_SAVE_START_ADDRESS		(0x48000)

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
  
  EXTRA_MM_STREAMING,
};


//--------------------------------------------------------------
//  �ʏ�Z�[�u�f�[�^�̃e�[�u��
//--------------------------------------------------------------
static const GFL_SAVEDATA_TABLE SaveDataTbl_Normal[] = {
	{	//�R���e�X�g
		GMDATA_ID_CONTEST,
		(FUNC_GET_SIZE)CONDATA_GetWorkSize,
		(FUNC_INIT_WORK)CONDATA_Init,
	},
	{	//BOX�f�[�^
		GMDATA_ID_BOXDATA,
		(FUNC_GET_SIZE)BOXDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXDAT_Init,
	},
	{	//BOX�g���[�f�[�^01
		GMDATA_ID_BOXTRAY_01,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^02
		GMDATA_ID_BOXTRAY_02,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^03
		GMDATA_ID_BOXTRAY_03,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^04
		GMDATA_ID_BOXTRAY_04,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^05
		GMDATA_ID_BOXTRAY_05,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^06
		GMDATA_ID_BOXTRAY_06,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^07
		GMDATA_ID_BOXTRAY_07,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^08
		GMDATA_ID_BOXTRAY_08,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^09
		GMDATA_ID_BOXTRAY_09,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^10
		GMDATA_ID_BOXTRAY_10,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^11
		GMDATA_ID_BOXTRAY_11,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^12
		GMDATA_ID_BOXTRAY_12,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^13
		GMDATA_ID_BOXTRAY_13,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^14
		GMDATA_ID_BOXTRAY_14,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^15
		GMDATA_ID_BOXTRAY_15,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^16
		GMDATA_ID_BOXTRAY_16,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^17
		GMDATA_ID_BOXTRAY_17,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^18
		GMDATA_ID_BOXTRAY_18,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^19
		GMDATA_ID_BOXTRAY_19,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^20
		GMDATA_ID_BOXTRAY_20,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^21
		GMDATA_ID_BOXTRAY_21,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^22
		GMDATA_ID_BOXTRAY_22,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^23
		GMDATA_ID_BOXTRAY_23,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//BOX�g���[�f�[�^24
		GMDATA_ID_BOXTRAY_24,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//�p���p�[�N�p�{�b�N�X
		GMDATA_ID_BOXTRAY_PALPARK,
		(FUNC_GET_SIZE)BOXTRAYDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXTRAYDAT_Init,
	},
	{	//�莝���A�C�e��
		GMDATA_ID_MYITEM,
		(FUNC_GET_SIZE)MYITEM_GetWorkSize,
		(FUNC_INIT_WORK)MYITEM_Init,
	},
	{	//�莝���|�P����
		GMDATA_ID_MYPOKE,
		(FUNC_GET_SIZE)PokeParty_GetWorkSize,
		(FUNC_INIT_WORK)PokeParty_InitWork,
	},
	{	//�v���C���[�f�[�^
		GMDATA_ID_PLAYER_DATA,
		(FUNC_GET_SIZE)PLAYERDATA_GetWorkSize,
		(FUNC_INIT_WORK)PLAYERDATA_Init,
	},
	{	//�󋵃f�[�^
		GMDATA_ID_SITUATION,
		(FUNC_GET_SIZE)Situation_GetWorkSize,
		(FUNC_INIT_WORK)Situation_Init,
	},
	{	//WiFi�ʐM�����f�[�^
		GMDATA_ID_WIFIHISTORY,
		(FUNC_GET_SIZE)WIFIHISTORY_GetWorkSize,
		(FUNC_INIT_WORK)WIFIHISTORY_Init,
	},
	{	//WiFi�F�B�R�[�h�f�[�^
		GMDATA_ID_WIFILIST,
		(FUNC_GET_SIZE)WifiList_GetWorkSize,
		(FUNC_INIT_WORK)WifiList_Init,
	},
	{	// C-GEAR
		GMDATA_ID_CGEAR,
		(FUNC_GET_SIZE)CGEAR_SV_GetWorkSize,
		(FUNC_INIT_WORK)CGEAR_SV_Init,
	},
	{	//�g���[�i�[�J�[�h(�T�C��
		GMDATA_ID_TRCARD,
		(FUNC_GET_SIZE)TRCSave_GetSaveDataSize,
		(FUNC_INIT_WORK)TRCSave_InitSaveData,
	},
	{	//�s�v�c�ȑ��蕨
		GMDATA_ID_MYSTERYDATA,
		(FUNC_GET_SIZE)MYSTERYDATA_GetWorkSize,
		(FUNC_INIT_WORK)MYSTERYDATA_Init,
	},
	{	//�؃��b�v�{�C�X
		GMDATA_ID_PERAPVOICE,
		(FUNC_GET_SIZE)PERAPVOICE_GetWorkSize,
		(FUNC_INIT_WORK)PERAPVOICE_Init,
	},
	{	//�V�X�e���f�[�^
		GMDATA_ID_SYSTEM_DATA,
		(FUNC_GET_SIZE)SYSTEMDATA_GetWorkSize,
		(FUNC_INIT_WORK)SYSTEMDATA_Init,
	},
	{	//���R�[�h(�X�R�A
		GMDATA_ID_RECORD,
		(FUNC_GET_SIZE)RECORD_GetWorkSize,
		(FUNC_INIT_WORK)RECORD_Init,
	},
	{	//�ȈՕ�
		GMDATA_ID_PMS,
		(FUNC_GET_SIZE)PMSW_GetSaveDataSize,
		(FUNC_INIT_WORK)PMSW_InitSaveData,
	},
	{
		GMDATA_ID_MMDL,
		(FUNC_GET_SIZE)MMDL_SAVEDATA_GetWorkSize,
		(FUNC_INIT_WORK)MMDL_SAVEDATA_Init,
	},
	{	//Wi�|Fi�Ђ��
		GMDATA_ID_WFHIROBA,
		(FUNC_GET_SIZE)SAVEDATA_WFLBY_GetWorkSize,
		(FUNC_INIT_WORK)SAVEDATA_WFLBY_InitWork,
	},
	{	//�~���[�W�J��
		GMDATA_ID_MUSICAL,
		(FUNC_GET_SIZE)MUSICAL_SAVE_GetWorkSize,
		(FUNC_INIT_WORK)MUSICAL_SAVE_InitWork,
	},
  { //�����_�������}�b�v
    GMDATA_ID_RANDOMMAP,  
    (FUNC_GET_SIZE)RANDOMMAP_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)RANDOMMAP_SAVE_InitWork,
  },
	{	//�����`�F�b�N
		GMDATA_ID_IRCCOMPATIBLE,
    (FUNC_GET_SIZE)IRC_COMPATIBLE_SV_GetWorkSize,
    (FUNC_INIT_WORK)IRC_COMPATIBLE_SV_Init,
	},
  { //�z�z���{��
    GMDATA_ID_SP_RIBBON,
    (FUNC_GET_SIZE)SP_RIBBON_SAVE_GetWorkSize,
    (FUNC_INIT_WORK)SP_RIBBON_SAVE_InitWork,
  },
  { //�C�x���g���[�N
    GMDATA_ID_EVENT_WORK,
    (FUNC_GET_SIZE)EVENTWORK_GetWorkSize,
    (FUNC_INIT_WORK)EVENTWORK_InitWork,
  },
	{	//GTS
    GMDATA_ID_WORLDTRADEDATA,
    (FUNC_GET_SIZE)WorldTradeData_GetWorkSize,
    (FUNC_INIT_WORK)WorldTradeData_Init,
	},
	{	//REGULATION
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
  }
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
///�X�g���[�~���O
static const GFL_SAVEDATA_TABLE SaveDataTbl_Extra_Streaming[] = {
	{
		EXGMDATA_ID_STREAMING,
		(FUNC_GET_SIZE)BattleRec_GetWorkSize,
		(FUNC_INIT_WORK)BattleRec_WorkInit,
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
	0,								//�o�b�N�A�b�v�̈�擪�A�h���X
	MIRROR_SAVE_ADDRESS,			//�~���[�����O�̈�擪�A�h���X
	SAVESIZE_NORMAL,				//�g�p����o�b�N�A�b�v�̈�̑傫��
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
  	SaveDataTbl_Extra_Streaming,
  	NELEMS(SaveDataTbl_Extra_Streaming),
  	EXTRA_MM_STREAMING,             //�o�b�N�A�b�v�̈�擪�A�h���X
  	EXTRA_MM_STREAMING,             //�~���[�����O�̈�擪�A�h���X ���~���[�����w��
  	SAVESIZE_EXTRA_STREAMING,       //�g�p����o�b�N�A�b�v�̈�̑傫��
  	MAGIC_NUMBER,
  },
};
SDK_COMPILER_ASSERT(NELEMS(SaveParam_ExtraTbl) == SAVE_EXTRA_ID_MAX);

