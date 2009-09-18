//==============================================================================
/**
 * @file    union_msg.c
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "net_app/union/union_msg.h"
#include "message.naix"
#include "msg/msg_union_room.h"
#include "system/main.h"


//==============================================================================
//  �v���g�^�C�v�錾�F�f�[�^
//==============================================================================
static const FLDMENUFUNC_LIST BattleMenuList_Number[3];
static const FLDMENUFUNC_LIST BattleMenuList_Mode[5];
static const FLDMENUFUNC_LIST BattleMenuList_Reg[4];
static const FLDMENUFUNC_LIST BattleMenuList_RegDouble[4];


//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  �e�Q�[���̍ő�Q���l��
//--------------------------------------------------------------
static const struct{
  u8 member_max;
  u8 padding[3];
}MenuMemberMax[] = {
  {2},      //UNION_PLAY_CATEGORY_UNION,          ///<���j�I�����[��
  {2},      //UNION_PLAY_CATEGORY_TALK,           ///<��b��
  {2},      //UNION_PLAY_CATEGORY_TRAINERCARD,    ///<�g���[�i�[�J�[�h
  {4},      //UNION_PLAY_CATEGORY_PICTURE,        ///<���G����
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50,         ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50,         ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD,   ///<�R���V�A��
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI,      ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_TRADE,          ///<�|�P��������
  {4},      //UNION_PLAY_CATEGORY_GURUGURU,       ///<���邮�����
  {5},      //UNION_PLAY_CATEGORY_RECORD,         ///<���R�[�h�R�[�i�[
};

//--------------------------------------------------------------
//  ���j���[�w�b�_�[
//--------------------------------------------------------------
///���C�����j���[���X�g
static const FLDMENUFUNC_LIST MainMenuList[] =
{
  {msg_union_select_01, (void*)UNION_PLAY_CATEGORY_TRAINERCARD},   //���A
  {msg_union_select_04, (void*)UNION_PLAY_CATEGORY_TRADE},   //����
  {msg_union_select_03, (void*)UNION_MENU_NO_SEND_BATTLE},   //�ΐ�
  {msg_union_select_06, (void*)UNION_PLAY_CATEGORY_GURUGURU}, //���邮�����
  {msg_union_select_07, (void*)UNION_PLAY_CATEGORY_RECORD},   //���R�[�h�R�[�i�[
  {msg_union_select_02, (void*)UNION_PLAY_CATEGORY_PICTURE},   //���G����
  {msg_union_select_05, (void*)UNION_MENU_SELECT_CANCEL},   //��߂�
};

///���j���[�w�b�_�[(���C�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	7,		//���X�g���ڐ�
	7,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-14,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	13,		//�\���T�C�YX �L�����P��
	14,		//�\���T�C�YY �L�����P��
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);

//--------------------------------------------------------------
//  �ΐ탁�j���[
//--------------------------------------------------------------
///��l�ΐ�A�l�l�ΐ�A��߂�
static const FLDMENUFUNC_LIST BattleMenuList_Number[] =
{
  {msg_union_battle_01_01, (void*)BattleMenuList_Mode},   //��l�ΐ�
  {msg_union_battle_01_02, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_MULTI},    //�l�l�ΐ�
  {msg_union_battle_01_03, (void*)FLDMENUFUNC_CANCEL},    //��߂�
};

///��l�ΐ�F�V���O���A�_�u���A�g���v���A���[�e�[�V�����A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_Mode[] =
{
  {msg_union_battle_01_04, (void*)BattleMenuList_Reg},      //�V���O��
  {msg_union_battle_01_05, (void*)BattleMenuList_RegDouble},   //�_�u��
  {msg_union_battle_01_06, (void*)BattleMenuList_Number},   //�g���v��
  {msg_union_battle_01_07, (void*)BattleMenuList_Number},   //���[�e�[�V����
  {msg_union_battle_01_08, (void*)BattleMenuList_Number},   //���ǂ�
};

///��l�ΐ�F�V���O���F��������Ȃ��A�X�^���_�[�h�A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_Reg[] =
{
  {msg_union_battle_01_09, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50},//���x���T�O���[��
  {msg_union_battle_01_10, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE}, //�����Ȃ�
  {msg_union_battle_01_11, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD},//�X�^���_�[�h
  {msg_union_battle_01_12, (void*)BattleMenuList_Mode},   //���ǂ�
};

///��l�ΐ�F�_�u���F��������Ȃ��A�X�^���_�[�h�A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_RegDouble[] =
{
  {msg_union_battle_01_09, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50},//���x���T�O���[��
  {msg_union_battle_01_10, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE}, //�����Ȃ�
  {msg_union_battle_01_11, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD},//�X�^���_�[�h
  {msg_union_battle_01_12, (void*)BattleMenuList_Mode},   //���ǂ�
};

///�ΐ탁�j���[�̃f�[�^�e�[�u��
static const struct{
  const FLDMENUFUNC_LIST *list;
  u8 list_max;
}BattleMenuDataTbl[] = {
  {
    BattleMenuList_Number,
    3,
  },
  {
    BattleMenuList_Mode,
    5,
  },
  {
    BattleMenuList_Reg,
    4,
  },
  {
    BattleMenuList_RegDouble,
    4,
  },
};

///���j���[�w�b�_�[(�ΐ탁�j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_Battle =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-16,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	15,		//�\���T�C�YX �L�����P��
	14,		//�\���T�C�YY �L�����P��
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);

//--------------------------------------------------------------
//  POKEPARTY�I�����j���[�w�b�_�[
//--------------------------------------------------------------
///POKEPARTY�I�����j���[���X�g
static const FLDMENUFUNC_LIST PokePartySelectMenuList[] =
{
  {msg_union_battle_01_23, (void*)COLOSSEUM_SELECT_PARTY_TEMOTI},  //�莝��
  {msg_union_battle_01_24, (void*)COLOSSEUM_SELECT_PARTY_BOX_A},   //�o�g���{�b�N�X�P
  {msg_union_battle_01_25, (void*)COLOSSEUM_SELECT_PARTY_BOX_B},   //�o�g���{�b�N�X�Q
};

///���j���[�w�b�_�[(POKEPARTY�I�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_PokePartySelect =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-14,		//�\�����WX �L�����P��
	24-6-7,		//�\�����WY �L�����P��
	13,		//�\���T�C�YX �L�����P��
	6,		//�\���T�C�YY �L�����P��
};

//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �e���j���[�őI�������Q�[���̒ʐM�ő�Q���l�����擾����
 *
 * @param   menu_index		UNION_MSG_MENU_SELECT_???
 *
 * @retval  int		�ő�Q���l��
 */
//==================================================================
int UnionMsg_GetMemberMax(UNION_PLAY_CATEGORY menu_index)
{
  if(menu_index >= UNION_PLAY_CATEGORY_MAX){
    GF_ASSERT(menu_index < UNION_PLAY_CATEGORY_MAX);
    return 0;
  }
  return MenuMemberMax[menu_index].member_max;
}

//==================================================================
/**
 * UnionMsg�n�̂��̂�S�Ĉꊇ�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_AllDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_YesNo_Del(unisys);
  UnionMsg_Menu_MainMenuDel(unisys);
  UnionMsg_TalkStream_WindowDel(unisys);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;
  
  if(unisys->fld_msgwin_stream == NULL){
    GF_ASSERT(unisys->msgdata == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_union_room_dat );
    unisys->fld_msgwin_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, unisys->msgdata);
  }
  else{ //���ɃE�B���h�E���\������Ă���ꍇ�̓��b�Z�[�W�G���A�̃N���A���s��
    FLDMSGWIN_STREAM_ClearMessage(unisys->fld_msgwin_stream);
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream != NULL){
    FLDMSGWIN_STREAM_ClearWindow(unisys->fld_msgwin_stream);
    FLDMSGWIN_STREAM_Delete(unisys->fld_msgwin_stream);
    unisys->fld_msgwin_stream = NULL;
  }

  if(unisys->msgdata != NULL){
    FLDMSGBG_DeleteMSGDATA(unisys->msgdata);
    unisys->msgdata = NULL;
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n
 *
 * @param   unisys		
 * @param   str_id		���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id)
{
  FLDMSGPRINT *msg_print;
  
  GF_ASSERT(unisys->fld_msgwin_stream != NULL);
  FLDMSGWIN_STREAM_PrintStart(unisys->fld_msgwin_stream, 0, 0, str_id);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�I���`�F�b�N
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�I���B�@FALSE:�p����
 */
//==================================================================
BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream == NULL){
    return TRUE;
  }
  return FLDMSGWIN_STREAM_Print(unisys->fld_msgwin_stream);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n�ɉ����A���b�Z�[�W�E�B���h�E�������ꍇ�͐������s��
 *
 * @param   unisys		  
 * @param   fieldWork		
 * @param   str_id		  ���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, u32 str_id)
{
  UnionMsg_TalkStream_WindowSetup(unisys, fieldWork);
  UnionMsg_TalkStream_Print(unisys, str_id);
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;

  if(unisys->fldmenu_yesno_func == NULL){
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->fldmenu_yesno_func = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_yesno_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_yesno_func);
    unisys->fldmenu_yesno_func = NULL;
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I��҂�
 *
 * @param   unisys		
 * @param   result		���ʑ����(TRUE:�͂��AFALSE:������)
 *
 * @retval  BOOL		TRUE:�I�������B�@FALSE:�I��
 */
//==================================================================
BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result)
{
  FLDMENUFUNC_YESNO yes_no;
  
  GF_ASSERT(unisys->fldmenu_yesno_func != NULL);
  
  yes_no = FLDMENUFUNC_ProcYesNoMenu(unisys->fldmenu_yesno_func);
  if(yes_no == FLDMENUFUNC_YESNO_NULL){
    return FALSE;
  }
  else if(yes_no == FLDMENUFUNC_YESNO_YES){
    *result = TRUE;
  }
  else{
    *result = FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *menuhead)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  
  if(unisys->fldmenu_func == NULL){
    fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, unisys->msgdata, HEAPID_UNION);
    unisys->fldmenu_func = FLDMENUFUNC_AddMenu(fldmsg_bg, menuhead, fldmenu_listdata);
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_func);
    unisys->fldmenu_func = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//--------------------------------------------------------------
static u32 UnionMsg_Menu_SelectLoop(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT(unisys->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(unisys->fldmenu_func);
}

//==================================================================
/**
 * ���C�����j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    MainMenuList, NELEMS(MainMenuList), &MenuHeader_MainMenu);
}

//==================================================================
/**
 * ���C�����j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * ���C�����j���[�F�I��҂�
 *
 * @param   unisys		
 * 
 * @retval  u32		����
 */
//==================================================================
u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

//==================================================================
/**
 * �ΐ탁�j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, int menu_index)
{
  int i;
  FLDMENUFUNC_HEADER head;
  
  head = MenuHeader_Battle;
  head.count = BattleMenuDataTbl[menu_index].list_max;
  head.line = BattleMenuDataTbl[menu_index].list_max;
  head.bmpsize_y = head.count * 2;
  
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    BattleMenuDataTbl[menu_index].list, BattleMenuDataTbl[menu_index].list_max, &head);
}

//==================================================================
/**
 * �ΐ탁�j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * �ΐ탁�j���[�F�I��҂�
 *
 * @param   unisys		
 * @param   next_sub_menu		TRUE:���̃T�u���j���[������@FALSE:�ŏI����
 *
 * @retval  u32		next_sub_menu��TRUE�̏ꍇ�A���̃��j���[Index
 * @retval  u32		next_sub_menu��FALSE�̏ꍇ�A�ŏI���ʔԍ�(�����ʐM�ő���)
 */
//==================================================================
u32 UnionMsg_Menu_BattleMenuSelectLoop(UNION_SYSTEM_PTR unisys, BOOL *next_sub_menu)
{
  u32 menu_ret;
  int i;
  
  *next_sub_menu = FALSE;
  
  menu_ret = UnionMsg_Menu_SelectLoop(unisys);
  if(menu_ret < UNION_PLAY_CATEGORY_MAX 
      || menu_ret == FLDMENUFUNC_NULL || menu_ret == FLDMENUFUNC_CANCEL){
    return menu_ret;
  }
  
  for(i = 0; i < NELEMS(BattleMenuDataTbl); i++){
    if(BattleMenuDataTbl[i].list == (void*)menu_ret){
      *next_sub_menu = TRUE;
      return i;
    }
  }
  
  GF_ASSERT_MSG(0, "menu_ret=%d\n", menu_ret);
  return 0;
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_PokePartySelectMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    PokePartySelectMenuList, NELEMS(PokePartySelectMenuList), &MenuHeader_PokePartySelect);
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_PokePartySelectMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//==================================================================
u32 UnionMsg_Menu_PokePartySelectMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

