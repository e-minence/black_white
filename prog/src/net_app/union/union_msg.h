//==============================================================================
/**
 * @file    union_msg.h
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#pragma once

#include "union_types.h"

//==============================================================================
//  �萔��`
//==============================================================================
///UnionMsg_Menu_SelectLoop�֐��̖߂�l(PLAY_CATEGORY�ȊO�̎w��)
enum{
  UNION_MENU_SELECT_CANCEL = UNION_PLAY_CATEGORY_UNION,///<�L�����Z��(���j�I���ɖ߂�Ƃ����Ӗ�)
  
  //���ȉ��A���M����鎖�͂Ȃ��ԍ�(���[�J�����j���[�̔���ł̂ݎg�p)
  //  ���[�J���ł����g�p���Ȃ��̂ŏ����̔ԍ��Ɣ���Ă����v
  UNION_MENU_NO_SEND_BATTLE = UNION_PLAY_CATEGORY_MAX,       ///<�ΐ�(���ꂪ���M����鎖�͂Ȃ�)
  UNION_MENU_SELECT_NULL = 0xff,  ///<�����I������Ă��Ȃ�(�����l�Ƃ��Ďg�p)
};

enum{ //��BattleMenuDataTbl�ƕ��т𓯂��ɂ��Ă������ƁI
  BATTLE_MENU_INDEX_NUMBER,
  BATTLE_MENU_INDEX_CUSTOM,
  BATTLE_MENU_INDEX_MODE,
  BATTLE_MENU_INDEX_RULE,
  BATTLE_MENU_INDEX_SHOOTER,
  //�}���`�p���j���[
  BATTLE_MENU_INDEX_CUSTOM_MULTI,
  BATTLE_MENU_INDEX_RULE_MULTI,
  BATTLE_MENU_INDEX_SHOOTER_MULTI,
};

typedef enum{
  REGWIN_TYPE_RULE,        ///<���[��������
  REGWIN_TYPE_NG_TEMOTI,   ///<�莝��NG�`�F�b�N
  REGWIN_TYPE_NG_BBOX,     ///<�o�g���{�b�N�XNG�`�F�b�N
}REGWIN_TYPE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern int UnionMsg_GetMemberMax(UNION_PLAY_CATEGORY menu_index);
extern void UnionMsg_AllDel(UNION_SYSTEM_PTR unisys);

extern void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys);
extern void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id);
extern BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys);
extern void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, u32 str_id);

extern void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys);
extern BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result);

extern void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys);
extern u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys);

extern void UnionMsg_Menu_BattleMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, int menu_index, UNION_MENU_REGULATION *menu_reg);
extern void UnionMsg_Menu_BattleMenuDel(UNION_SYSTEM_PTR unisys);
extern u32 UnionMsg_Menu_BattleMenuSelectLoop(UNION_SYSTEM_PTR unisys, BOOL *next_sub_menu, 
  UNION_MENU_REGULATION *menu_reg, BOOL *reg_look);

extern void UnionMsg_Menu_BattleMenuMultiTitleSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_Menu_BattleMenuMultiTitleDel(UNION_SYSTEM_PTR unisys);

extern void UnionMsg_Menu_PokePartySelectMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_Menu_PokePartySelectMenuDel(UNION_SYSTEM_PTR unisys);
extern u32 UnionMsg_Menu_PokePartySelectMenuSelectLoop(UNION_SYSTEM_PTR unisys);

extern void UnionMsg_Menu_RegulationSetup(UNION_SYSTEM_PTR unisys, 
  FIELDMAP_WORK *fieldWork, u32 fail_bit, BOOL shooter_type, REGWIN_TYPE regwin_type);
extern BOOL UnionMsg_Menu_RegulationWait(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork);
extern void UnionMsg_Menu_RegulationDel(UNION_SYSTEM_PTR unisys);


//--------------------------------------------------------------
//  GetMsgID
//--------------------------------------------------------------
extern u16 UnionMsg_GetMsgID_TalkTimeout(UNION_SYSTEM_PTR unisys);
extern u16 UnionMsg_GetMsgID_ParentConnectStart(int target_sex);
extern u16 UnionMsg_GetMsgID_TalkCancel(int target_sex);
extern u16 UnionMsg_GetMsgID_TalkCancelPartner(int target_sex);
extern u16 UnionMsg_GetMsgID_ParentMainMenuSelectAnswerWait(u32 mainmenu_select, int target_sex);
extern u16 UnionMsg_GetMsgID_ParentMainMenuSelectAnswerNG(u32 mainmenu_select, int target_sex);
extern u16 UnionMsg_GetMsgID_ChildMainMenuSelectView(u32 mainmenu_select);
extern u16 UnionMsg_GetMsgID_ChildMainMenuSelectRefuse(u32 mainmenu_select);
extern u16 UnionMsg_GetMsgID_TalkContinue(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiStart(int target_sex);
extern u16 UnionMsg_GetMsgID_TradeMinePokeNG(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruMineEggNG(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruMineDametamago(UNION_SYSTEM_PTR unisys);
extern u16 UnionMsg_GetMsgID_GuruguruDameTamagoNG(UNION_SYSTEM_PTR unisys);
extern u16 UnionMsg_GetMsgID_PlayGameBattle(int target_sex);
extern u16 UnionMsg_GetMsgID_PlayGameMainMenuSelect(int target_sex);
extern u16 UnionMsg_GetMsgID_PlayGameTrainerCard(int target_sex);
extern u16 UnionMsg_GetMsgID_PlayGamePicture(int target_sex);
extern u16 UnionMsg_GetMsgID_PlayGameTrade(int target_sex);
extern u16 UnionMsg_GetMsgID_PlayGameGuruguru(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiIntrude(int target_sex);
extern u16 UnionMsg_GetMsgID_GurugurIntrude(int target_sex);
extern u16 UnionMsg_GetMsgID_PictureIntrude(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiIntrudeRefuses(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeRefuses(int target_sex);
extern u16 UnionMsg_GetMsgID_PictureIntrudeRefuses(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeNoEgg(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeDametamago(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeWait(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeSuccess(int target_sex);
extern u16 UnionMsg_GetMsgID_GuruguruIntrudeParentNG(int target_sex);
extern u16 UnionMsg_GetMsgID_PictureIntrudeWait(int target_sex);
extern u16 UnionMsg_GetMsgID_PictureIntrudeSuccess(int target_sex);
extern u16 UnionMsg_GetMsgID_PictureIntrudeParentNG(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiIntrudeWait(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiIntrudeSuccess(int target_sex);
extern u16 UnionMsg_GetMsgID_MultiIntrudeParentNG(int target_sex);
extern u16 UnionMsg_GetMsgID_TrainerCardStart(UNION_SYSTEM_PTR unisys, int target_nation, int target_area);

