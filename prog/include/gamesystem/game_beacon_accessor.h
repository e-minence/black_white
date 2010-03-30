//==============================================================================
/**
 * @file    game_beacon_accessor.h
 * @brief   �r�[�R�����̒l�擾�ׂ̈̃A�N�Z�T
 * @author  matsuda
 * @date    2010.01.19(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_beacon.h"


//==============================================================================
//  �萔��`
//==============================================================================
///��w�薳��
#define GAMEBEACON_DETAILS_HOBBY_NULL   (0xff)
///�d���w�薳��
#define GAMEBEACON_DETAILS_JOB_NULL     (0xff)

//�r�[�R���A�C�R����ޒ�`
enum{
 BEACON_ICON_HELLO,       //1   ����
 BEACON_ICON_BTL_START,   //2   �o�g���X�^�[�g&������
 BEACON_ICON_BTL_WIN,     //3   �o�g������
 BEACON_ICON_POKE_GET,    //4   �|�P�����Q�b�g
 BEACON_ICON_POKE_LVUP,   //5   �|�P�������x���A�b�v
 BEACON_ICON_POKE_SHINKA, //6   �|�P�����i��
 BEACON_ICON_GPOWER,      //7   G�p���[�n
 BEACON_ICON_ITEM,        //8   �A�C�e���n
 BEACON_ICON_SPECIAL,     //9   �z�z�n
 BEACON_ICON_UNION,       //10  ���j�I�����[��
 BEACON_ICON_INFO,        //11  �Q�[�����C���t�H���[�V����
 BEACON_ICON_MSG,         //12  ���b�Z�[�W�󂯎��
 BEACON_ICON_MAX,
};

//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �r�[�R���e�[�u������n
//--------------------------------------------------------------
extern int GAMEBEACON_Stack_Update(GAMEBEACON_INFO_TBL *stack_infotbl);
extern BOOL GAMEBEACON_Stack_GetInfo(GAMEBEACON_INFO_TBL *stack_infotbl, GAMEBEACON_INFO *dest_info, u16 *time);
extern void GAMEBEACON_Stack_PutBack(const GAMEBEACON_INFO_TBL *stack_infotbl);

extern int GAMEBEACON_InfoTbl_SetBeacon(GAMEBEACON_INFO_TBL *infotbl, const GAMEBEACON_INFO *set_info, u16 time, BOOL push_out);
extern BOOL GAMEBEACON_InfoTbl_GetBeacon(GAMEBEACON_INFO_TBL *infotbl, GAMEBEACON_INFO *dest_info, u16 *time, int log_no);

extern int GAMEBEACON_InfoTblRing_Ofs2Idx(GAMEBEACON_INFO_TBL *infotbl, int ofs );
extern int GAMEBEACON_InfoTblRing_SetBeacon(GAMEBEACON_INFO_TBL *infotbl, const GAMEBEACON_INFO *set_info, u16 time, BOOL* new_f);
extern BOOL GAMEBEACON_InfoTblRing_GetBeacon(GAMEBEACON_INFO_TBL *infotbl, GAMEBEACON_INFO *dest_info, u16 *time, int ofs );
extern int GAMEBEACON_InfoTblRing_GetEntryNum(GAMEBEACON_INFO_TBL *infotbl );

extern GAMEBEACON_INFO * GAMEBEACON_Alloc(HEAPID heap_id);
extern GAMEBEACON_INFO_TBL * GAMEBEACON_InfoTbl_Alloc(HEAPID heap_id);

//--------------------------------------------------------------
//  �A�N�Z�T
//--------------------------------------------------------------
extern BOOL GAMEBEACON_Check_NPC(const GAMEBEACON_INFO *info);
extern BOOL GAMEBEACON_Check_Error(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_PlayerNameToBuf(const GAMEBEACON_INFO *info, STRBUF* strbuf);
extern int GAMEBEACON_Get_PmVersion(const GAMEBEACON_INFO *info);
extern int GAMEBEACON_Get_Nation(const GAMEBEACON_INFO *info);
extern int GAMEBEACON_Get_Area(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_Job(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_Hobby(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_IntroductionPms(const GAMEBEACON_INFO *info, PMS_DATA *dest_pms);
extern u8 GAMEBEACON_Get_ResearchTeamRank(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_FavoriteColor(GXRgb *dest_buf, const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_TrainerID(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_TrainerView(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_Sex(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_PlayTime(const GAMEBEACON_INFO *info, u16 *dest_hour, u16 *dest_min);
extern GPOWER_ID GAMEBEACON_Get_GPowerID(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_ZoneID(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_TownmapRootZoneID(const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_ThanksRecvCount(const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_SuretigaiCount(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_SelfIntroduction(const GAMEBEACON_INFO *info, STRBUF *dest);
extern void GAMEBEACON_Get_ThankyouMessage(const GAMEBEACON_INFO *info, STRBUF *dest);
extern void GAMEBEACON_Get_FreeWordMessage(const GAMEBEACON_INFO *info, STRBUF *dest);
extern GAMEBEACON_DETAILS_NO GAMEBEACON_Get_Details_DetailsNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Details_BattleTrainer(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Details_BattleMonsNo(const GAMEBEACON_INFO *info);
extern GAMEBEACON_ACTION GAMEBEACON_Get_Action_ActionNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_Monsno(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_ItemNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_WazaNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_DistributionMonsno(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_DistributionItemNo(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_Action_Nickname(const GAMEBEACON_INFO *info, STRBUF *dest);
extern u16 GAMEBEACON_Get_Action_TrNo(const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_Action_ThankyouCount(const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_Action_Hour(const GAMEBEACON_INFO *info);
extern u32 GAMEBEACON_Get_Action_VictoryCount(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_Action_TrialHouseRank(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_GPowerID(const GAMEBEACON_INFO *info);

extern u8 GAMEBEACON_GetActionDataType( GAMEBEACON_ACTION action );
extern u8 GAMEBEACON_GetActionDataIconType( GAMEBEACON_ACTION action );
extern void GAMEBEACON_InfoWordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id);

