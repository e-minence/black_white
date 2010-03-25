//==============================================================================
/**
 * @file    comm_entry_menu.c
 * @brief   �Q����W���j���[����
 * @author  matsuda
 * @date    2009.07.28(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "message.naix"
#include "msg/msg_connect.h"
#include "net_app/comm_entry_menu.h"
#include "print/wordset.h"
#include "savedata/mystatus_local.h"
#include "comm_entry_menu_comm_command.h"


//==============================================================================
//  �萔��`
//==============================================================================
///EXPAND�œW�J���郁�b�Z�[�W�o�b�t�@�̃T�C�Y
#define EXPAND_STRBUF_SIZE      (100)
///LIST EXPAND�œW�J���郁�b�Z�[�W�o�b�t�@�̃T�C�Y
#define LIST_EXPAND_STRBUF_SIZE      (64)
///NUM EXPAND�œW�J���郁�b�Z�[�W�o�b�t�@�̃T�C�Y
#define NUM_EXPAND_STRBUF_SIZE      (32)

///���[�U�[�X�e�[�^�X
enum{
  USER_STATUS_NULL,         ///<���[�U�[�����Ȃ�
  USER_STATUS_READY,        ///<������(���X�g�ɂ܂����O���ڂ��Ă��Ȃ�)
  USER_STATUS_EXAMINATION,  ///<�R���Ώ�
  USER_STATUS_EXAMINATION_WAIT, ///<�R�������҂�
  USER_STATUS_COMPLETION,   ///<�G���g���[����
  USER_STATUS_REFUSE,       ///<�f����
};

///�G���g���[���X�gBMP�T�C�Y(�L�����P��)
enum{
  LISTBMP_POS_LEFT = 2,
  LISTBMP_POS_RIGHT = 20,
  LISTBMP_POS_TOP = 6,
  LISTBMP_POS_BOTTOM_PARENT = LISTBMP_POS_TOP + 10,   //�e�̎���Y�T�C�Y
  LISTBMP_POS_BOTTOM_CHILD = LISTBMP_POS_TOP + 8,     //�q�̎���Y�T�C�Y
};

///�G���g���[���X�g��BMP�������݈ʒu(�h�b�g�P��)
enum{
  LISTBMP_START_NAME_X = 0,
  LISTBMP_START_NAME_Y = 0,
  LISTBMP_START_NUM_X = 0,
  LISTBMP_START_NUM_Y = 2*4*8,
};

///�^�C�g��BMP�T�C�Y(�L�����P��)
enum{
  TITLEBMP_POS_LEFT = 2,
  TITLEBMP_POS_RIGHT = 30,
  TITLEBMP_POS_TOP = 1,
  TITLEBMP_POS_BOTTOM = TITLEBMP_POS_TOP + 2,
};


///�ŏI����
typedef enum{
  DECIDE_TYPE_NULL,
  DECIDE_TYPE_OK,
  DECIDE_TYPE_NG,
}DECIDE_TYPE;

///�u���U���܂����H�v����
typedef enum{
  BREAKUP_TYPE_NULL,
  BREAKUP_TYPE_OK,
  BREAKUP_TYPE_NG,
}BREAKUP_TYPE;


///�e�@�T�����j���[���X�g�ő吔
#define PARENT_LIST_MAX     (16)
///�e�@�T�����j���[���X�g�ɍڂ��Ă������
#define PARENT_LIST_LIFE    (30*5)

///_ParentSearchList_Update�̍ŏI����
typedef enum{
  PARENT_SEARCH_LIST_SELECT_NULL,       ///<���s��
  PARENT_SEARCH_LIST_SELECT_OK,         ///<�e�@�Ɛڑ�����
  PARENT_SEARCH_LIST_SELECT_NG,         ///<�e�@�ɒf��ꂽ
  PARENT_SEARCH_LIST_SELECT_CANCEL,     ///<�L�����Z������
}PARENT_SEARCH_LIST_SELECT;


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
//  �g�p���郁�b�Z�[�W�̍\����
//--------------------------------------------------------------
typedef struct{
  u16 msgid_title;              ///<�J�Â��Ă���Q�[���^�C�g��
  u16 msgid_breakup_confirm;    ///<�u���U���܂����H�v
  u16 msgid_breakup;            ///<�u���U���܂����v
  u16 msgid_breakup_parent_only;  ///<�u�e�@�݂̂ɂȂ����̂ŉ��U���܂��v
  u16 msgid_breakup_game;       ///<�u�s���̈��������o�[�����邽�߃Q�[���𒆎~���܂��v
  u16 msgid_child_entry;        ///<�u������������@�Ƃ��������@�I��ł��������v
}ENTRYMENU_MESSAGE_PACK;

struct _ENTRYMENU_MEMBER_INFO{
  MYSTATUS mystatus[COMM_ENTRY_USER_MAX];   ///<�Q���҂�MYSTATUS
  u8 member_bit;                            ///<�L���ȃ����o�[(bit�Ǘ�)
  u8 padding[3];
};

///�G���g���[�҂̃f�[�^
typedef struct{
  MYSTATUS mystatus;
  u8 status;
  u8 force_entry;        ///<TRUE:�����I�Ɏ󂯓����
  u8 send_flag;          ///<TRUE:�Ԏ����M�ς�(�O�����痧�Ă�)
  
  u8 mac_address[6];      ///<MAC�A�h���X
  u8 padding[2];
}COMM_ENTRY_USER;

///�u�͂��E�������v����
typedef struct{
  FLDMENUFUNC *menufunc;
  BREAKUP_TYPE breakup_type;
  u8 seq;
  u8 padding[3];
}COMM_ENTRY_YESNO;

///�e�@���
typedef struct{
  MYSTATUS mystatus;
  u8 mac_address[6];
  u8 occ;                 ///<TRUE�F�f�[�^�L��
  u8 padding;
  s16 life;
  u8 padding2[2];
}COMM_PARENTUSER;

///�e�T�����X�g����
typedef struct{
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  FLDMENUFUNC *menufunc;
  COMM_PARENTUSER parentuser[PARENT_LIST_MAX];  ///<�r�[�R���Ɉ������������e�@���
  COMM_PARENTUSER *connect_parent;              ///<�ڑ���Ƃ��đI�񂾐e�@���
  PARENT_SEARCH_LIST_SELECT final_select;       ///<�ŏI����
  u8 local_seq;
  u8 list_update_req;
  u8 list_strbuf_create;
  u8 padding;
}PARENTSEARCH_LIST;

///�Q����W�Ǘ��V�X�e��
typedef struct _COMM_ENTRY_MENU_SYSTEM{
  FIELDMAP_WORK *fieldWork;
  WORDSET *wordset;
  GFL_MSGDATA *msgdata;
  FLDMSGWIN *fldmsgwin_list;
  FLDMSGWIN *fldmsgwin_title;
  FLDMSGWIN_STREAM *fld_stream;
  STRBUF *strbuf_expand;
  STRBUF *strbuf_list_template;         ///<�G���g���[�҂�Expand���ɂȂ�msg
  STRBUF *strbuf_list_expand;           ///<�G���g���[�҂�Expand�p�o�b�t�@
  STRBUF *strbuf_num_template;          ///<�c��l����Expand���ɂȂ�msg
  STRBUF *strbuf_num_expand;            ///<�c��l����Expand�p�o�b�t�@
  
  COMM_ENTRY_RESULT entry_result;       ///<�ŏI����
  MYSTATUS mine_mystatus;
  COMM_ENTRY_USER user[COMM_ENTRY_USER_MAX];    ///<�G���g���[�҂̃f�[�^
  BOOL examination_occ;                         ///<TRUE:�R�����̃��[�U�[������
  COMM_ENTRY_USER examination_user;             ///<�R�����̃��[�U�[
  COMM_ENTRY_YESNO yesno;                       ///<�u�͂��E�������v����
  PARENTSEARCH_LIST parentsearch;                 ///<�e�@�T�����X�g
  
  ENTRYMENU_MEMBER_INFO member_info;            ///<�Q���ҏ��(�e�@�̏ꍇ�͑��M�o�b�t�@�Ƃ��Ďg�p)
  u8 member_info_recv;                          ///<TRUE:�Q���ҏ�����M����
  u8 game_start;                                ///<TRUE:�Q�[���J�n��M
  u8 game_cancel;                               ///<TRUE:�Q�[���L�����Z����M
  u8 mp_mode;                                   ///<TRUE:MP�ʐM  FALSE:DS�ʐM
  
  u8 min_num;         ///<�ŏ��l��
  u8 max_num;         ///<�ő�l��
  u8 seq;
  u8 entry_mode;      ///<COMM_ENTRY_MODE_xxx
  
  u8 game_type;       ///<COMM_ENTRY_GAMETYPE_xxx
  u8 send_bit_entry_ok;   ///<�G���g���[OK�̕Ԏ�(bit�Ǘ�)
  u8 send_bit_entry_ng;   ///<�G���g���[NG�̕Ԏ�(bit�Ǘ�)
  u8 update_member_info;          ///<�����o�[��񑗐M���N�G�X�g
  u8 update_member_info_reserve;  ///<�����o�[��񑗐M�\��
  u8 member_info_sending;         ///<TRUE:�����o�[��񑗐M��
  HEAPID heap_id;
  
  ENTRY_PARENT_ANSWER entry_parent_answer;
  MYSTATUS entry_parent_myst;
  u8 final_user_status[COMM_ENTRY_USER_MAX];
  
  u8 parent_mac_address[6];       ///<mode��COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE���̂�
  
  u8 draw_mac[COMM_ENTRY_USER_MAX][6];  ///<���X�g�ɖ��O��������Ă���v���C���[��MacAddress
  
  s8 draw_player_num;             ///<�c��l���v�̌���
  s8 padding[3];
}COMM_ENTRY_MENU_SYSTEM;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL _Update_Parent(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_Child(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_ChildParentConnect(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_ChildParentDesignate(COMM_ENTRY_MENU_PTR em);
static void _SendUpdate_Parent(COMM_ENTRY_MENU_PTR em);
static u8 _MemberInfoSendBufCreate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_LeaveUserUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_UserStatusUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_EraseUser(COMM_ENTRY_MENU_PTR em, NetID net_id);
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user);
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em);
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em);
static BREAKUP_TYPE CommEntryMenu_BreakupUpdate(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em);
static void _MemberInfo_Setup(COMM_ENTRY_MENU_PTR em);
static void _MemberInfo_Exit(COMM_ENTRY_MENU_PTR em);
static void _ParentEntry_NumDraw(COMM_ENTRY_MENU_PTR em, int player_num);
static void _ParentEntry_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID);
static void _ParentEntry_NameErase(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID);
static void _MemberInfo_NameDraw(COMM_ENTRY_MENU_PTR em, const MYSTATUS *myst, NetID netID);
static void _MemberInfo_NameErase(COMM_ENTRY_MENU_PTR em, int netID);
static void _ChildEntryMember_ListRewriteUpdate(COMM_ENTRY_MENU_PTR em);
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id);
static void CommEntryMenu_SetFinalUser(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_CheckFinalUser(COMM_ENTRY_MENU_PTR em);
static void _SendBitClear(COMM_ENTRY_MENU_PTR em, NetID netID);
static void _Req_SendMemberInfo(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_Setup(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_Exit(COMM_ENTRY_MENU_PTR em);
static PARENT_SEARCH_LIST_SELECT _ParentSearchList_Update(COMM_ENTRY_MENU_PTR em);
static BOOL _ParentSearchList_ListSelectUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_ParentLifeUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_BeaconUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_SetNewParent(COMM_ENTRY_MENU_PTR em, const u8 *mac_address, const MYSTATUS *myst);
static void _ParentSearchList_DeleteParent(COMM_ENTRY_MENU_PTR em, int parent_no);
static void _ParentSearchList_SetListString(COMM_ENTRY_MENU_PTR em);


//==============================================================================
//  �f�[�^
//==============================================================================
///�Q�[���^�C�v���ɈقȂ郁�b�Z�[�WID�̃f�[�^�p�b�P�[�W
ALIGN4 static const ENTRYMENU_MESSAGE_PACK _GameTypeMsgPack[] = {
  {//COMM_ENTRY_GAMETYPE_COLOSSEUM
    msg_connect_01_04,
    msg_connect_03_01,
    msg_connect_05_01,
    msg_connect_07_01,
    msg_connect_09_01,
    msg_child_00,         //���g�p
  },
  {//COMM_ENTRY_GAMETYPE_MUSICAL
    msg_connect_01_04_2,
    msg_connect_03_01_2,
    msg_connect_05_01_2,
    msg_connect_07_01,    //���g�p
    msg_connect_09_01_2,
    msg_child_00,
  },
  {//COMM_ENTRY_GAMETYPE_SUBWAY
    msg_connect_01_04_3,
    msg_connect_03_01_3,
    msg_connect_05_01_3,
    msg_connect_07_01,    //���g�p
    msg_connect_09_01_3,
    msg_child_00_01,
  },
};
SDK_COMPILER_ASSERT(NELEMS(_GameTypeMsgPack) == COMM_ENTRY_GAMETYPE_MAX);

///�v���C���[�����Ȃ���������MacAddress
ALIGN4 static const u8 _NullMacAddress[6] = {0, 0, 0, 0, 0, 0};


///�e�T�����j���[�w�b�_�[
static const FLDMENUFUNC_HEADER ParentSearchMenuListHeader = {
  16,                     //���X�g���ڐ�
  5,                      //�\���ő區�ڐ�
  0,                      //���x���\���w���W
  13,                     //���ڕ\���w���W
  0,                      //�J�[�\���\���w���W
  0,                      //�\���x���W
  1,                      //�\�������F
  15,                     //�\���w�i�F
  2,                      //�\�������e�F
  0,                      //�����Ԋu�w
  0,                      //�����Ԋu�x
  FLDMENUFUNC_SKIP_NON,   //�y�[�W�X�L�b�v�^�C�v
  12,                     //�����T�C�YX(�h�b�g
  16,                     //�����T�C�YY(�h�b�g
	1,                      //�\�����WX �L�����P��
	1,                      //�\�����WY �L�����P��
	20,                     //�\���T�C�YX �L�����P��
	10,                     //�\���T�C�YY �L�����P��
};




//==================================================================
/**
 * �Q����W���j���[�F�Z�b�g�A�b�v
 *
 * @param   fieldWork		
 * @param   min_num		  �ŏ��l��
 * @param   max_num		  �ő�l��
 * @param   heap_id		  �q�[�vID
 * @param   entry_mode  COMM_ENTRY_MODE_xxx
 * @param   game_type   COMM_ENTRY_GAMETYPE_xxx
 * @param   parent_mac_address  �e��MacAddress(entry_mode��COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE���̂ݕK�v)
 *
 * @retval  COMM_ENTRY_MENU_PTR		���������Q����W���j���[���[�N�ւ̃|�C���^
 */
//==================================================================
COMM_ENTRY_MENU_PTR CommEntryMenu_Setup(const MYSTATUS *myst, FIELDMAP_WORK *fieldWork, int min_num, int max_num, HEAPID heap_id, COMM_ENTRY_MODE entry_mode, COMM_ENTRY_GAMETYPE game_type, const u8 *parent_mac_address)
{
  COMM_ENTRY_MENU_PTR em;
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  
  OS_TPrintf("CommEntryMenu_Setup\n");
  GF_ASSERT(max_num <= COMM_ENTRY_USER_MAX);
  
  em = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_ENTRY_MENU_SYSTEM));
  
  em->wordset = WORDSET_Create( heap_id );
  em->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_connect_dat );
  em->fld_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, em->msgdata);
  em->strbuf_expand = GFL_STR_CreateBuffer(EXPAND_STRBUF_SIZE, heap_id);
  em->strbuf_list_template = GFL_MSG_CreateString( em->msgdata, msg_connect_01_04_4 );
  em->strbuf_list_expand = GFL_STR_CreateBuffer(LIST_EXPAND_STRBUF_SIZE, heap_id);
  em->strbuf_num_template = GFL_MSG_CreateString( em->msgdata, msg_connect_01_03 );
  em->strbuf_num_expand = GFL_STR_CreateBuffer(NUM_EXPAND_STRBUF_SIZE, heap_id);
  em->min_num = min_num;
  em->max_num = max_num;
  em->fieldWork = fieldWork;
  em->entry_mode = entry_mode;
  em->game_type = game_type;
  em->heap_id = heap_id;
  em->draw_player_num = -1;   //�ŏ��͕K���`�悳���悤��-1��ݒ肵�Ă���
  MyStatus_Copy(myst, &em->mine_mystatus);
  if(parent_mac_address != NULL){
    GFL_STD_MemCopy(parent_mac_address, em->parent_mac_address, 6);
  }
  
  GFL_NET_SetNoChildErrorCheck(FALSE);  //�q�@������ɂ��Ȃ��Ȃ��Ă��G���[�Ƃ��Ȃ�
  CommEntryMenu_AddCommandTable(em);
  
  if(game_type == COMM_ENTRY_GAMETYPE_MUSICAL){
    em->mp_mode = TRUE;
  }
  
  return em;
}

//==================================================================
/**
 * �Q����W���j���[�폜
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_Exit(COMM_ENTRY_MENU_PTR em)
{
  GFL_STR_DeleteBuffer(em->strbuf_num_expand);
  GFL_STR_DeleteBuffer(em->strbuf_num_template);
  GFL_STR_DeleteBuffer(em->strbuf_list_expand);
  GFL_STR_DeleteBuffer(em->strbuf_list_template);
  GFL_STR_DeleteBuffer(em->strbuf_expand);
  
  FLDMSGWIN_STREAM_Delete(em->fld_stream);
  FLDMSGBG_DeleteMSGDATA(em->msgdata);
  WORDSET_Delete( em->wordset );
  
  CommEntryMenu_DelCommandTable();
  GFL_HEAP_FreeMemory(em);
}

//==================================================================
/**
 * �v���C���[�G���g���[
 *
 * @param   em		  
 * @param   netID		      �l�b�gID
 * @param   name		      ���O
 * @param   id		        ID(�g���[�i�[�J�[�h��ID)
 * @param   force_entry		TRUE:�����󂯓���B�@FALSE:�󂯓���邩�I������
 * @param   mac_address   MAC�A�h���X
 */
//==================================================================
void CommEntryMenu_Entry(COMM_ENTRY_MENU_PTR em, int netID, const MYSTATUS *myst, BOOL force_entry, const u8 *mac_address)
{
  COMM_ENTRY_USER *p_user = &em->user[netID];
  
  GF_ASSERT(netID <= COMM_ENTRY_USER_MAX);
  
  MyStatus_Copy(myst, &p_user->mystatus);
  p_user->status = USER_STATUS_READY;
  p_user->force_entry = force_entry;
  GFL_STD_MemCopy(mac_address, p_user->mac_address, 6);
  
  _SendBitClear(em, netID);
  _Req_SendMemberInfo(em);
}

//==================================================================
/**
 * �Q����W���j���[�F�X�V����
 *
 * @param   em		
 *
 * @retval  COMM_ENTRY_RESULT_SUCCESS
 */
//==================================================================
COMM_ENTRY_RESULT CommEntryMenu_Update(COMM_ENTRY_MENU_PTR em)
{
  BOOL ret;
  
  switch(em->entry_mode){
  case COMM_ENTRY_MODE_PARENT:
    ret = _Update_Parent(em);
    break;
  case COMM_ENTRY_MODE_CHILD:
    ret = _Update_Child(em);
    break;
  case COMM_ENTRY_MODE_CHILD_PARENT_CONNECTED:
    ret = _Update_ChildParentConnect(em);
    break;
  case COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE:
    ret = _Update_ChildParentDesignate(em);
    break;
  }
  
  if(ret == TRUE){
    return em->entry_result;
  }
  return COMM_ENTRY_RESULT_NULL;
}

//--------------------------------------------------------------
/**
 * �X�V�����F�e�@
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�S�Ă̏������I���B
 */
//--------------------------------------------------------------
static BOOL _Update_Parent(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_INIT_SERVER,
    _SEQ_FIRST_CONNECT_WAIT,
    _SEQ_FIRST_NEGO_WAIT,
    _SEQ_ENTRY_SEND,
    _SEQ_ENTRY,
    _SEQ_FINAL_CHECK,
    _SEQ_BREAKUP_CHECK,
    _SEQ_SEND_GAMESTART,
    _SEQ_SEND_GAMECANCEL,
    _SEQ_FINISH,
  };

  if(em->seq > _SEQ_INIT){
    CommEntryMenu_LeaveUserUpdate(em);
    CommEntryMenu_ListUpdate(em);
    _SendUpdate_Parent(em);
  }
  
  switch(em->seq){
  case _SEQ_INIT:
    _MemberInfo_Setup(em);

    //�ŏ��̃��b�Z�[�W�`��
    _StreamMsgSet(em, msg_connect_02_01);
    em->seq++;
    break;
  case _SEQ_INIT_SERVER:
    if(em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM){
      em->seq = _SEQ_ENTRY_SEND;
    }
    else{
      GFL_NET_InitServer();
      em->seq++;
    }
    break;
  case _SEQ_FIRST_CONNECT_WAIT:
    if(GFL_NET_SystemGetConnectNum() > 1){
      if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
        OS_TPrintf("�l�S�V�G�[�V�������M\n");
        em->seq++;
      }
    }
    else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
      em->seq = _SEQ_BREAKUP_CHECK;
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_FIRST_NEGO_WAIT:
    if( GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) == TRUE ){
      em->seq = _SEQ_ENTRY_SEND;
    }
    break;
  case _SEQ_ENTRY_SEND:
    if(CemSend_Entry(&em->mine_mystatus, TRUE, em->mp_mode) == TRUE){
      em->seq++;
    }
    break;
  case _SEQ_ENTRY:   //��W��
    CommEntryMenu_UserStatusUpdate(em);
    
    //�G���g���[�҂��󂯓���邩�I��
    CommEntryMenu_ExaminationUpdate(em);
    if(CommEntryMenu_InterruptCheck(em) == TRUE){
      int entry_num = CommEntryMenu_GetCompletionNum(em);
      if(entry_num >= em->max_num
          || (entry_num >= em->min_num && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE))){
        CommEntryMenu_SetFinalUser(em);
        em->seq = _SEQ_FINAL_CHECK;
      }
      else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
        em->seq = _SEQ_BREAKUP_CHECK;
      }
    }
    break;
  case _SEQ_FINAL_CHECK:   //���̃����o�[�ł�낵���ł����H
    {
      DECIDE_TYPE type;
      type = CommEntryMenu_DecideUpdate(em);
      if(type == DECIDE_TYPE_OK){
        GFL_NET_SetNoChildErrorCheck(TRUE);
        GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE);
        em->seq = _SEQ_SEND_GAMESTART;
      }
      else if(type == DECIDE_TYPE_NG){
        em->seq = _SEQ_ENTRY;
      }
    }
    break;
  case _SEQ_BREAKUP_CHECK:  //���U���܂����H
    {
      BREAKUP_TYPE type;
      type = CommEntryMenu_BreakupUpdate(em);
      if(type == BREAKUP_TYPE_OK){
        GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE);
        em->seq = _SEQ_SEND_GAMECANCEL;
      }
      else if(type == BREAKUP_TYPE_NG){
        em->seq = _SEQ_ENTRY;
      }
    }
    break;
  case _SEQ_SEND_GAMESTART:
    if(CemSend_GameStart(em->mp_mode) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_SEND_GAMECANCEL:
    if(CemSend_GameCancel(em->mp_mode) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_FINISH:   //�I������
    _MemberInfo_Exit(em);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �X�V�����F�q�@
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�S�Ă̏������I���B
 */
//--------------------------------------------------------------
static BOOL _Update_Child(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT_TALK,
    _SEQ_INIT_TALK_WAIT,
    _SEQ_PARENT_LIST_INIT,
    _SEQ_PARENT_LIST_MAIN,
    _SEQ_MEMBER_INIT,
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_CANCEL_INIT,
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT_TALK:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_child_entry);
    em->seq++;
    break;
  case _SEQ_INIT_TALK_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq++;
    }
    break;
    
  case _SEQ_PARENT_LIST_INIT:
    _ParentSearchList_Exit(em);
    _MemberInfo_Exit(em);

    em->parentsearch.connect_parent = NULL;
    _ParentSearchList_Setup(em);
    GFL_NET_StartBeaconScan();
    em->seq++;
    break;
  case _SEQ_PARENT_LIST_MAIN:   //�e�@�T��
    {
      PARENT_SEARCH_LIST_SELECT list_select = _ParentSearchList_Update(em);
      
      switch(list_select){
      case PARENT_SEARCH_LIST_SELECT_OK:
        _ParentSearchList_Exit(em);
        em->seq = _SEQ_MEMBER_INIT;
        break;
      case PARENT_SEARCH_LIST_SELECT_NG:
        _ParentSearchList_Exit(em);
        em->seq = _SEQ_NG_INIT;
        break;
      case PARENT_SEARCH_LIST_SELECT_CANCEL:
        _ParentSearchList_Exit(em);
        em->seq = _SEQ_CANCEL_INIT;
        break;
      }
    }
    break;
    
  case _SEQ_MEMBER_INIT:      //���̃����o�[�̎Q�������҂�
    _MemberInfo_Setup(em);
    em->seq++;
    break;
  case _SEQ_MEMBER_WAIT:
    _ChildEntryMember_ListRewriteUpdate(em);
    if(em->game_start == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_START;
    }
    else if(em->game_cancel == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_CANCEL;
    }
    break;
  
  case _SEQ_GAME_START:
    _StreamMsgSet(em, msg_game_start);
    em->seq = _SEQ_GAME_START_WAIT;
    break;
  case _SEQ_GAME_START_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq = _SEQ_INIT_TALK;
    }
    break;
  
  case _SEQ_NG_INIT:
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->entry_parent_myst);  //&em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq = _SEQ_INIT_TALK;
    }
    break;
    
  case _SEQ_CANCEL_INIT:      //�L�����Z������
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
    
  case _SEQ_FINISH:
    _ParentSearchList_Exit(em);
    _MemberInfo_Exit(em);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �X�V�����F�q�@(���ɐe�ƃl�S�V�G�[�V�������݂Őڑ��ς�) ���R���V�A���ŏ��̃����o�[
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�S�Ă̏������I���B
 */
//--------------------------------------------------------------
static BOOL _Update_ChildParentConnect(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_FORCE_ENTRY_SEND,
    _SEQ_MEMBER_INIT,
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_CANCEL_INIT,
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT:
    _StreamMsgSet(em, msg_connect_08_01);
    em->seq++;
    break;
  case _SEQ_FORCE_ENTRY_SEND:
    if(CemSend_Entry(&em->mine_mystatus, TRUE, em->mp_mode) == TRUE){
      em->seq++;
    }
    break;
  
  case _SEQ_MEMBER_INIT:      //���̃����o�[�̎Q�������҂�
    _MemberInfo_Setup(em);
    em->seq++;
    break;
  case _SEQ_MEMBER_WAIT:
    _ChildEntryMember_ListRewriteUpdate(em);
    if(em->game_start == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_START;
    }
    else if(em->game_cancel == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_CANCEL;
    }
    break;
  
  case _SEQ_GAME_START:
    _StreamMsgSet(em, msg_game_start);
    em->seq = _SEQ_GAME_START_WAIT;
    break;
  case _SEQ_GAME_START_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  
  case _SEQ_NG_INIT:
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_NG;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_CANCEL_INIT:      //�L�����Z������
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
    
  case _SEQ_FINISH:
    _MemberInfo_Exit(em);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �X�V�����F�q�@(�ڑ�����e�@���O������w��) ���R���V�A������
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�S�Ă̏������I���B
 */
//--------------------------------------------------------------
static BOOL _Update_ChildParentDesignate(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_PARENT_ENTRY,
    _SEQ_PARENT_ANSWER_WAIT,
    _SEQ_MEMBER_INIT,      //���̃����o�[�̎Q�������҂�
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_CANCEL_INIT,      //�L�����Z������
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT:
    _StreamMsgSet(em, msg_connect_08_01);
    em->seq++;
    break;
  case _SEQ_PARENT_ENTRY:
    em->entry_parent_answer = ENTRY_PARENT_ANSWER_NULL;
    if(CemSend_Entry(&em->mine_mystatus, FALSE, em->mp_mode) == TRUE){
      em->seq = _SEQ_PARENT_ANSWER_WAIT;
    }
    break;
  case _SEQ_PARENT_ANSWER_WAIT:
    if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_OK){
      em->seq = _SEQ_MEMBER_INIT;
    }
    else if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_NG){
      em->seq = _SEQ_NG_INIT;
    }
    break;
  
  case _SEQ_MEMBER_INIT:      //���̃����o�[�̎Q�������҂�
    _MemberInfo_Setup(em);
    em->seq++;
    break;
  case _SEQ_MEMBER_WAIT:
    _ChildEntryMember_ListRewriteUpdate(em);
    if(em->game_start == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_START;
    }
    else if(em->game_cancel == TRUE){
      _MemberInfo_Exit(em);
      em->seq = _SEQ_GAME_CANCEL;
    }
    break;
  
  case _SEQ_GAME_START:
    _StreamMsgSet(em, msg_game_start);
    em->seq = _SEQ_GAME_START_WAIT;
    break;
  case _SEQ_GAME_START_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  
  case _SEQ_NG_INIT:
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->entry_parent_myst);  //&em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_NG;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_CANCEL_INIT:      //�L�����Z������
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
    
  case _SEQ_FINISH:
    _MemberInfo_Exit(em);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���M�f�[�^�X�V�F�e�@�p
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _SendUpdate_Parent(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->send_bit_entry_ok & (1 << net_id)){
      if(CemSend_EntryOK(net_id, em->mp_mode, &em->mine_mystatus) == TRUE){
        em->send_bit_entry_ok ^= 1 << net_id;
      }
    }
    else if(em->send_bit_entry_ng & (1 << net_id)){
      if(CemSend_EntryNG(net_id, em->mp_mode, &em->mine_mystatus) == TRUE){
        em->send_bit_entry_ng ^= 1 << net_id;
      }
    }
  }

  if(em->update_member_info == TRUE && em->member_info_sending == FALSE){
    u8 send_bit = _MemberInfoSendBufCreate(em);
    if(send_bit > 1){
      if(CemSend_MemberInfo(&em->member_info, send_bit, em->mp_mode) == TRUE){
        em->update_member_info = FALSE;
        em->member_info_sending = TRUE;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �ŐV�̃����o�[���𑗐M�o�b�t�@�ɔ��f
 *
 * @param   em		
 *
 * @retval  u8		���M����bit
 */
//--------------------------------------------------------------
static u8 _MemberInfoSendBufCreate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  u8 send_bit = 0;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL){
      MyStatus_Copy(&em->user[net_id].mystatus, &em->member_info.mystatus[net_id]);
      send_bit |= 1 << net_id;
    }
  }
  em->member_info.member_bit = send_bit;
  return (send_bit | 1);  //���Mbit�ɐe�@�̏��͕K���܂߂�
}

//--------------------------------------------------------------
/**
 * ���[�U�[�����Ȃ��Ȃ��Ă��Ȃ����X�V
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_LeaveUserUpdate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;

  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL && GFL_NET_IsConnectMember(net_id) == FALSE){
      OS_TPrintf("%d�Ԃ̃v���C���[�����Ȃ��Ȃ���\n", net_id);
      CommEntryMenu_EraseUser(em, net_id);
      _SendBitClear(em, net_id);
      _Req_SendMemberInfo(em);
    }
  }
}

//--------------------------------------------------------------
/**
 * ���X�g�X�V����
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em)
{
  int net_id, player_num = 0;

  //���X�g�̖��O�`��
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL){
      _ParentEntry_NameDraw(em,  &em->user[net_id], net_id);
      player_num++;
    }
    else{
      _ParentEntry_NameErase(em, &em->user[net_id], net_id);
    }
  }
  
  if(em->draw_player_num != player_num){
    _ParentEntry_NumDraw(em, player_num);
    em->draw_player_num = player_num;
  }
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�X�V
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_UserStatusUpdate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  COMM_ENTRY_USER *user = em->user;

  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    //���v���C���[�̃X�e�[�^�X�X�V
    switch(user->status){
    case USER_STATUS_READY:
      if(user->force_entry == TRUE){
        user->status = USER_STATUS_COMPLETION;
        em->send_bit_entry_ok |= 1 << net_id;
      }
      else{
        user->status = USER_STATUS_EXAMINATION;
      }
      break;
    case USER_STATUS_EXAMINATION:
      if(CommEntryMenu_SetExamination(em, user) == TRUE){
        user->status = USER_STATUS_EXAMINATION_WAIT;
      }
      break;
    }
    
    user++;
  }
}

//--------------------------------------------------------------
/**
 * �Ώۃ��[�U�[������
 *
 * @param   em		
 * @param   net_id		NetID
 */
//--------------------------------------------------------------
static void CommEntryMenu_EraseUser(COMM_ENTRY_MENU_PTR em, NetID net_id)
{
  em->user[net_id].status = USER_STATUS_NULL;
}

//--------------------------------------------------------------
/**
 * �R���ΏۂɃ��[�U�[��o�^����
 *
 * @param   em		
 * @param   user		
 *
 * @retval  BOOL		TRUE:�o�^����
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user)
{
  if(em->examination_occ == FALSE){
    em->examination_user = *user;
    em->examination_occ = TRUE;
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �G���g���[�҂��󂯓���邩�I��
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em)
{
  COMM_ENTRY_USER *user = &em->examination_user;
  COMM_ENTRY_YESNO *yesno = &em->yesno;
  FLDMSGBG *fldmsg_bg;
  NetID net_id;
  
  if(em->examination_occ == FALSE || FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return;
  }
  
  switch(yesno->seq){
  case 0:
    WORDSET_RegisterPlayerName( em->wordset, 0, &user->mystatus );
    _StreamMsgSet(em, msg_connect_02_02);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      int status;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      else if(ret == FLDMENUFUNC_YESNO_YES){
        status = USER_STATUS_COMPLETION;
      }
      else{
        status = USER_STATUS_REFUSE;
      }
      for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
        if(em->user[net_id].status == USER_STATUS_EXAMINATION_WAIT){
          em->user[net_id].status = status;
          if(status == USER_STATUS_COMPLETION){
            em->send_bit_entry_ok |= 1 << net_id;
          }
          else{
            em->send_bit_entry_ng |= 1 << net_id;
          }
          break;
        }
      }
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      _StreamMsgSet(em, msg_connect_02_01);
      em->examination_occ = FALSE;
      yesno->seq = 0;
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * �ŏI���菈��
 *
 * @param   em		
 *
 * @retval  
 */
//--------------------------------------------------------------
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  if(FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return DECIDE_TYPE_NULL;
  }

  switch(yesno->seq){
  case 0:
    _StreamMsgSet(em, msg_connect_02_03);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      if(CommEntryMenu_CheckFinalUser(em) == FALSE){
        ret = FLDMENUFUNC_YESNO_NO; //���[�U�[�̏󋵂ɕύX�����������߁A�ŏI������L�����Z������
      }
      else{
        ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
        if(ret == FLDMENUFUNC_YESNO_NULL){
          break;
        }
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      yesno->seq = 0;
      if(ret == FLDMENUFUNC_YESNO_YES){
        return DECIDE_TYPE_OK;
      }
      else{
        return DECIDE_TYPE_NG;
      }
    }
    break;
  }
  return DECIDE_TYPE_NULL;
}

//--------------------------------------------------------------
/**
 * �ŏI���菈��
 *
 * @param   em		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BREAKUP_TYPE CommEntryMenu_BreakupUpdate(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  if(FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return BREAKUP_TYPE_NULL;
  }

  switch(yesno->seq){
  case 0: //���U���܂����H
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_confirm);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      
      if(ret == FLDMENUFUNC_YESNO_NO){
        yesno->breakup_type = BREAKUP_TYPE_NG;
        yesno->seq = 100;
      }
      else{
        _StreamMsgSet(em, msg_connect_04_01);
        yesno->seq++;
      }
    }
    break;
  case 3: //�{���ɉ��U���܂����H
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 4:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      if(ret == FLDMENUFUNC_YESNO_YES){
        _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup);
        yesno->seq++;
      }
      else{
        yesno->breakup_type = BREAKUP_TYPE_NG;
        yesno->seq = 100;
      }
    }
    break;
  case 5:
    yesno->seq = 0xff;
    yesno->breakup_type = BREAKUP_TYPE_OK;
    break;
  case 100: //����ω��U�͂��Ȃ�
    _StreamMsgSet(em, msg_connect_02_01);
    yesno->seq = 0xff;
    break;
  case 0xff:
    if(yesno->menufunc != NULL){
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
    }
    yesno->seq = 0;
    return yesno->breakup_type;
  }
  return BREAKUP_TYPE_NULL;
}

//==================================================================
/**
 * �Q�[���J�n�t���O�Z�b�g
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_SetGameStart(COMM_ENTRY_MENU_PTR em)
{
  em->game_start = TRUE;
}

//==================================================================
/**
 * �Q�[���L�����Z���t���O�Z�b�g
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_SetGameCancel(COMM_ENTRY_MENU_PTR em)
{
  em->game_cancel = TRUE;
}


//==================================================================
/**
 * �G���g���[���ʂ̎擾
 *
 * @param   em		
 * @param   netID		
 * @param   exit_mode   TRUE:�e���Q�������o�[�������A�܂��Ԏ��𑗐M���Ă��Ȃ������o�[���擾���郂�[�h�ł�(FALSE�̎��̈Ⴂ�́ATRUE�̏ꍇ�͐R���ɂ���R�ꂽ�҂̏�Ԃ��Ԃ����ł�)
 *
 * @retval  COMM_ENTRY_ANSWER		
 *
 * ���M�ς݃t���O�������Ă�����̂�NULL��Ԃ��܂�
 */
//==================================================================
COMM_ENTRY_ANSWER CommEntryMenu_GetAnswer(COMM_ENTRY_MENU_PTR em, int netID, BOOL exit_mode)
{
  if(em->user[netID].send_flag == FALSE){
    if(em->user[netID].status == USER_STATUS_COMPLETION){
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    else if(em->user[netID].status == USER_STATUS_REFUSE){
//      return COMM_ENTRY_ANSWER_REFUSE;
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    
    if(exit_mode == TRUE){
      if(em->user[netID].status != USER_STATUS_NULL){
        return COMM_ENTRY_ANSWER_REFUSE;
      }
    }
  }
  return COMM_ENTRY_ANSWER_NULL;
}

//==================================================================
/**
 * ���M�ς݃t���O�𗧂Ă�
 *
 * @param   em		
 * @param   netID		
 *
 * CommEntryMenu_GetAnswer�Ŏ擾�����Ԏ���ʐM����ɑ��M������A����Ńt���O�𗧂ĂĂ�������
 * CommEntryMenu_GetAnswer��NULL���Ԃ�悤�ɂȂ�̂�2�d���M�Ȃǂ��N���Ȃ��Ȃ�܂�
 * ���ʂ��u���ہv�������ꍇ�̓��X�g����폜���܂�
 */
//==================================================================
void CommEntryMenu_SetSendFlag(COMM_ENTRY_MENU_PTR em, int netID)
{
  GF_ASSERT(em->user[netID].status == USER_STATUS_COMPLETION || em->user[netID].status == USER_STATUS_REFUSE);
  
  if(em->user[netID].status == USER_STATUS_COMPLETION){
    em->user[netID].send_flag = TRUE;
  }
  else if(em->user[netID].status == USER_STATUS_REFUSE){
    _ParentEntry_NameErase(em, &em->user[netID], netID);
    GFL_STD_MemClear(&em->user[netID], sizeof(COMM_ENTRY_USER));
  }
}

//==================================================================
/**
 * �G���g���[���������l�����擾����
 *
 * @param   em		
 *
 * @retval  int		�G���g���[�����l��
 */
//==================================================================
int CommEntryMenu_GetCompletionNum(COMM_ENTRY_MENU_PTR em)
{
  int i, count = 0;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    if(em->user[i].status == USER_STATUS_COMPLETION){
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------
/**
 * ���̑I�����s���Ă��Ȃ��t���[�̏󋵂����ׂ�
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE�F�t���[
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em)
{
  if(em->examination_occ == FALSE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �Q�������o�[��񃊃X�g�Z�b�g�A�b�v
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _MemberInfo_Setup(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
  int bmpwin_y_size;
  
  GF_ASSERT(em->fldmsgwin_list == NULL);
  GF_ASSERT(em->fldmsgwin_title == NULL);
  
  if(em->entry_mode == COMM_ENTRY_MODE_PARENT){
    bmpwin_y_size = LISTBMP_POS_BOTTOM_PARENT;
  }
  else{
    bmpwin_y_size = LISTBMP_POS_BOTTOM_CHILD;
  }
  
  em->fldmsgwin_list = FLDMSGWIN_Add( fldmsg_bg, em->msgdata, 
    LISTBMP_POS_LEFT, LISTBMP_POS_TOP, 
    LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT, bmpwin_y_size - LISTBMP_POS_TOP);
  em->fldmsgwin_title = FLDMSGWIN_Add( fldmsg_bg, em->msgdata, 
    TITLEBMP_POS_LEFT, TITLEBMP_POS_TOP, 
    TITLEBMP_POS_RIGHT - TITLEBMP_POS_LEFT, TITLEBMP_POS_BOTTOM - TITLEBMP_POS_TOP);

  FLDMSGWIN_Print( em->fldmsgwin_title, 0, 0, _GameTypeMsgPack[em->game_type].msgid_title );
}

//--------------------------------------------------------------
/**
 * �Q�������o�[��񃊃X�g�j��
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _MemberInfo_Exit(COMM_ENTRY_MENU_PTR em)
{
  if(em->fldmsgwin_title != NULL){
    FLDMSGWIN_Delete(em->fldmsgwin_title);
    em->fldmsgwin_title = NULL;
  }
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_Delete(em->fldmsgwin_list);
    em->fldmsgwin_list = NULL;
  }
}

//--------------------------------------------------------------
/**
 * �c��Q���l���`��
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NumDraw(COMM_ENTRY_MENU_PTR em, int player_num)
{
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_FillClearWindow(em->fldmsgwin_list, 
      LISTBMP_START_NUM_X, LISTBMP_START_NUM_Y, 
      (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NUM_Y + 8*2);

    WORDSET_RegisterNumber( em->wordset, 0, em->max_num - player_num, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( em->wordset, em->strbuf_num_expand, em->strbuf_num_template );
    FLDMSGWIN_PrintStrBuf(em->fldmsgwin_list, 
      LISTBMP_START_NUM_X, LISTBMP_START_NUM_Y, em->strbuf_num_expand);
  }
}

//--------------------------------------------------------------
/**
 * ���O�`��
 *
 * @param   em		
 * @param   user		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID)
{
  if(GFL_STD_MemComp(user->mac_address, em->draw_mac[netID], 6) == 0){
    return; //���ɕ`��ς�
  }
  
  _ParentEntry_NameErase(em, user, netID); //�܂����ɏ�����Ă��閼�O������
  
  _MemberInfo_NameDraw(em, &user->mystatus, netID);

  GFL_STD_MemCopy(user->mac_address, em->draw_mac[netID], 6);
}

//--------------------------------------------------------------
/**
 * ���O����
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NameErase(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID)
{
  if(GFL_STD_MemComp(em->draw_mac[netID], _NullMacAddress, 6) == 0){
    return;
  }
  
  _MemberInfo_NameErase(em, netID);
  GFL_STD_MemCopy(_NullMacAddress, em->draw_mac[netID], 6);
  
  OS_TPrintf("Name Erase = %d, %d, %d, %d\n",     
    LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
    (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NAME_Y + netID*8*4);
}

//--------------------------------------------------------------
/**
 * ���O�`��
 *
 * @param   em		
 * @param   myst
 */
//--------------------------------------------------------------
static void _MemberInfo_NameDraw(COMM_ENTRY_MENU_PTR em, const MYSTATUS *myst, NetID netID)
{
  if(em->fldmsgwin_list != NULL){
    WORDSET_RegisterPlayerName( em->wordset, 0, myst );
    WORDSET_RegisterNumber( em->wordset, 1, MyStatus_GetID_Low(myst), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( em->wordset, em->strbuf_list_expand, em->strbuf_list_template );
    FLDMSGWIN_PrintStrBuf(em->fldmsgwin_list, 
      LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, em->strbuf_list_expand);
  }
  else{
    GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * ���O����
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _MemberInfo_NameErase(COMM_ENTRY_MENU_PTR em, int netID)
{
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_FillClearWindow(em->fldmsgwin_list, 
      LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
      (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NAME_Y + netID*8*2);
  }
}

//--------------------------------------------------------------
/**
 * �Q���ҏ��S�̂��ĕ`��(�q�@�p)
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ChildEntryMember_ListRewriteUpdate(COMM_ENTRY_MENU_PTR em)
{
  int net_id;
  
  if(em->member_info_recv == FALSE){
    return;
  }
  em->member_info_recv = FALSE;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    _ParentEntry_NameErase(em, &em->user[net_id], net_id);
    if(em->member_info.member_bit & (1 << net_id)){
      _MemberInfo_NameDraw(em, &em->member_info.mystatus[net_id], net_id);
    }
  }
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�X�g���[���`��
 *
 * @param   em		
 * @param   msg_id		���b�Z�[�WID
 *
 * WORDSET�͂��炩���߃Z�b�g������ŌĂ�ł�������
 */
//--------------------------------------------------------------
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id)
{
  STRBUF *temp_strbuf;
  
  temp_strbuf = GFL_MSG_CreateString( em->msgdata, msg_id );
  WORDSET_ExpandStr( em->wordset, em->strbuf_expand, temp_strbuf );
  GFL_STR_DeleteBuffer(temp_strbuf);
  
  FLDMSGWIN_STREAM_ClearMessage(em->fld_stream);
  FLDMSGWIN_STREAM_PrintStrBufStart(em->fld_stream, 0, 0, em->strbuf_expand);
}

//--------------------------------------------------------------
/**
 * �����[�U�[���ŏI���҃��[�U�[�Ƃ��ăZ�b�g����
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_SetFinalUser(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    em->final_user_status[net_id] = em->user[net_id].status;
  }
}

//--------------------------------------------------------------
/**
 * �ŏI���҃��[�U�[�ƌ����[�U�[�̃X�e�[�^�X����v���Ă��邩���ׂ�
 *
 * @param   em		  
 *
 * @retval  BOOL		TRUE:��v���Ă���@FALSE:��v���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_CheckFinalUser(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->final_user_status[net_id] != em->user[net_id].status){
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �w��NetID�ɑ΂��鑗�Mbit��S�ăN���A����
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _SendBitClear(COMM_ENTRY_MENU_PTR em, NetID netID)
{
  em->send_bit_entry_ok &= 0xff ^ (1 << netID);
  em->send_bit_entry_ng &= 0xff ^ (1 << netID);
}

//--------------------------------------------------------------
/**
 * �����o�[��񑗐M���N�G�X�g
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _Req_SendMemberInfo(COMM_ENTRY_MENU_PTR em)
{
  if(em->update_member_info == TRUE || em->member_info_sending == TRUE){
    //����f�[�^�ׁ̈A���ɑ��M���N�G�X�g���������Ă���ꍇ�́A
    //���M���ł���A�Ƃ��������l�����ė\��Ƃ����`�Ń��N�G�X�g�������Ă���
    em->update_member_info_reserve = TRUE;
  }
  else{
    em->update_member_info = TRUE;
    em->update_member_info_reserve = FALSE;
  }
}

//==================================================================
/**
 * �����o�[����e�@����M�������ɁA���̃����o�[��񑗐M���N�G�X�g���������Ă���Δ��f������
 *
 * @param   em		
 */
//==================================================================
void CommEntyrMenu_MemberInfoReserveUpdate(COMM_ENTRY_MENU_PTR em)
{
  GF_ASSERT(em->member_info_sending == TRUE);
  em->member_info_sending = FALSE;
  em->update_member_info = em->update_member_info_reserve;
  em->update_member_info_reserve = FALSE;
}

//==================================================================
/**
 * �����o�[���̃o�b�t�@�|�C���^���擾
 *
 * @param   em		
 *
 * @retval  ENTRYMENU_MEMBER_INFO *		
 */
//==================================================================
ENTRYMENU_MEMBER_INFO * CommEntryMenu_GetMemberInfo(COMM_ENTRY_MENU_PTR em)
{
  return &em->member_info;
}

//==================================================================
/**
 * �����o�[������M�������̃Z�b�g�֐�
 *
 * @param   em		
 * @param   member_info		��M���������o�[���
 */
//==================================================================
void CommEntryMenu_RecvMemberInfo(COMM_ENTRY_MENU_PTR em, const ENTRYMENU_MEMBER_INFO *member_info)
{
  GFL_STD_MemCopy(member_info, &em->member_info, sizeof(ENTRYMENU_MEMBER_INFO));
  em->member_info_recv = TRUE;
}


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �e�@�T�����X�g�V�X�e���Z�b�g�A�b�v
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_Setup(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *field_msgbg = FIELDMAP_GetFldMsgBG(em->fieldWork);

  GF_ASSERT(em->parentsearch.menufunc == NULL);
  
  em->parentsearch.fldmenu_listdata = FLDMENUFUNC_CreateListData( 
    PARENT_LIST_MAX, em->heap_id );
  _ParentSearchList_SetListString(em);
  em->parentsearch.menufunc = FLDMENUFUNC_AddMenu( field_msgbg,
	  &ParentSearchMenuListHeader, em->parentsearch.fldmenu_listdata );
}

//--------------------------------------------------------------
/**
 * �e�@�T�����X�g�V�X�e���I��
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_Exit(COMM_ENTRY_MENU_PTR em)
{
  if(em->parentsearch.menufunc != NULL){
    FLDMENUFUNC_DeleteMenu(em->parentsearch.menufunc);
    em->parentsearch.menufunc = NULL;
    em->parentsearch.list_strbuf_create = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * �e�@�T�����X�g�V�X�e���X�V
 *
 * @param   em		
 *
 * @retval  PARENT_SEARCH_LIST_SELECT		
 */
//--------------------------------------------------------------
static PARENT_SEARCH_LIST_SELECT _ParentSearchList_Update(COMM_ENTRY_MENU_PTR em)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  BOOL ret;
  enum{
    _SEQ_UPDATE,
    _SEQ_NEGO_WAIT,
    _SEQ_PARENT_ANSWER_WAIT,
    _SEQ_FINISH,
  };
  
  switch(psl->local_seq){
  case _SEQ_UPDATE:
    if(psl->connect_parent == NULL){
      _ParentSearchList_ParentLifeUpdate(em);
      _ParentSearchList_BeaconUpdate(em);
      if(_ParentSearchList_ListSelectUpdate(em) == FALSE){
        psl->final_select = PARENT_SEARCH_LIST_SELECT_CANCEL;
        psl->local_seq = _SEQ_FINISH;
        break;
      }
      
      if(em->parentsearch.list_update_req == TRUE){
        _ParentSearchList_SetListString(em);
        FLDMENUFUNC_Rewrite( em->parentsearch.menufunc );
        em->parentsearch.list_update_req = FALSE;
      }
    }
    else{
      if(GFL_NET_SystemGetConnectNum() > 1){
        if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
          OS_TPrintf("�l�S�V�G�[�V�������M\n");
          psl->local_seq++;
        }
      }
      else{
        //��check�@�����Ɉ�莞�Ԍo���Ă��e�@�ɐڑ��o���Ȃ�������L�����Z�������ɂ��鏈��������
      }
    }
    break;
  case _SEQ_NEGO_WAIT: //�l�S�V�G�[�V���������҂�
    if( GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) == TRUE ){
      OS_TPrintf("�l�S�V�G�[�V��������\n");
      em->entry_parent_answer = ENTRY_PARENT_ANSWER_NULL;
      if(CemSend_Entry(&em->mine_mystatus, FALSE, em->mp_mode) == TRUE){
        psl->local_seq = _SEQ_PARENT_ANSWER_WAIT;
        OS_TPrintf("�e�ɃG���g���[�𑗐M\n");
      }
    }
    break;
  case _SEQ_PARENT_ANSWER_WAIT: 
    if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_OK){
      OS_TPrintf("�e����̃G���g���[�Ԏ��FOK\n");
      psl->final_select = PARENT_SEARCH_LIST_SELECT_OK;
      psl->local_seq = _SEQ_FINISH;
    }
    else if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_NG){
      OS_TPrintf("�e����̃G���g���[�Ԏ��FNG\n");
      psl->final_select = PARENT_SEARCH_LIST_SELECT_NG;
      psl->local_seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_FINISH:
    return psl->final_select;
  }
  
  return PARENT_SEARCH_LIST_SELECT_NULL;
}

//--------------------------------------------------------------
/**
 * �e�@�T�����X�g�I���X�V����
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�I�𒆁@FALSE:�L�����Z����������
 */
//--------------------------------------------------------------
static BOOL _ParentSearchList_ListSelectUpdate(COMM_ENTRY_MENU_PTR em)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  u32 select_ret;
  
  select_ret = FLDMENUFUNC_ProcMenu( psl->menufunc );
  switch(select_ret){
  case FLDMENUFUNC_NULL:
    break;
  case FLDMENUFUNC_CANCEL:
    return FALSE;
  default:
    if(psl->parentuser[select_ret].occ == TRUE){
      GF_ASSERT(psl->connect_parent == NULL);
      psl->connect_parent = &psl->parentuser[select_ret];
      GFL_NET_ConnectToParent(psl->connect_parent->mac_address);
    }
    break;
  }
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �e�@���̎����X�V
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_ParentLifeUpdate(COMM_ENTRY_MENU_PTR em)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  int i;

  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == TRUE){
      psl->parentuser[i].life--;
      if(psl->parentuser[i].life <= 0){
        _ParentSearchList_DeleteParent(em, i);
        psl->list_update_req = TRUE;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �r�[�R�����擾���ă��X�g���X�V
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_BeaconUpdate(COMM_ENTRY_MENU_PTR em)
{
  int i;
  COMM_ENTRY_BEACON *bcon_buff;
  
  for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++)
  {
  	bcon_buff = GFL_NET_GetBeaconData(i);
  	if(bcon_buff != NULL )
  	{
      _ParentSearchList_SetNewParent(em, bcon_buff->mac_address, &bcon_buff->mystatus);
  	}
  }
}

//--------------------------------------------------------------
/**
 * �V�����e�@���G���g���[����
 *
 * @param   em		
 * @param   mac_address		
 * @param   myst		
 */
//--------------------------------------------------------------
static void _ParentSearchList_SetNewParent(COMM_ENTRY_MENU_PTR em, const u8 *mac_address, const MYSTATUS *myst)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  int i;
  
  //���ɓo�^�ς݂łȂ����`�F�b�N
  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == TRUE){
      if(GFL_STD_MemComp(psl->parentuser[i].mac_address, mac_address, 6) == 0){
        psl->parentuser[i].life = PARENT_LIST_LIFE; //����������
        return; //���ɓo�^�ς�
      }
    }
  }
  //�󂢂Ă���ꏊ�̈�ԏ�ɓo�^
  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == FALSE){
      GFL_STD_MemCopy(mac_address, psl->parentuser[i].mac_address, 6);
      MyStatus_Copy(myst, &psl->parentuser[i].mystatus);
      psl->parentuser[i].life = PARENT_LIST_LIFE;
      psl->parentuser[i].occ = TRUE;
      psl->list_update_req = TRUE;
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * �e�@���폜
 *
 * @param   em		
 * @param   parent_no		�폜����e�@No
 */
//--------------------------------------------------------------
static void _ParentSearchList_DeleteParent(COMM_ENTRY_MENU_PTR em, int parent_no)
{
  em->parentsearch.parentuser[parent_no].occ = FALSE;
}

//--------------------------------------------------------------
/**
 * �e�@�T�����X�g�̃��j���[����STRBUF���X�V����
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_SetListString(COMM_ENTRY_MENU_PTR em)
{
	int i;
  STRBUF *strbuf_expand, *strbuf_src, *strbuf_eom;
  STRCODE str_eom = GFL_STR_GetEOMCode();
  
  if(em->parentsearch.list_strbuf_create == TRUE){
    FLDMENUFUNC_ListSTRBUFDelete(em->parentsearch.fldmenu_listdata);
  }
  
  strbuf_src = GFL_STR_CreateBuffer(64, em->heap_id);
  strbuf_expand = GFL_STR_CreateBuffer(64, em->heap_id);
  strbuf_eom = GFL_STR_CreateBuffer(4, em->heap_id);
	
  GFL_MSG_GetString( em->msgdata, msg_connect_search_000, strbuf_src );
	for( i = 0; i < PARENT_LIST_MAX; i++ ){
    WORDSET_RegisterNumber( em->wordset, 0, i, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    if(em->parentsearch.parentuser[i].occ == TRUE){
      WORDSET_RegisterPlayerName( em->wordset, 1, &em->parentsearch.parentuser[i].mystatus );
      WORDSET_RegisterNumber( em->wordset, 2, 
        MyStatus_GetID_Low(&em->parentsearch.parentuser[i].mystatus), 
        5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    }
    else{
    	GFL_STR_SetStringCode(strbuf_eom, &str_eom);
    	WORDSET_RegisterWord( em->wordset, 1, strbuf_eom, 0, TRUE, PM_LANG );
    	WORDSET_RegisterWord( em->wordset, 2, strbuf_eom, 0, TRUE, PM_LANG );
    }
    WORDSET_ExpandStr( em->wordset, strbuf_expand, strbuf_src );
    FLDMENUFUNC_AddStringListData( em->parentsearch.fldmenu_listdata,
  		strbuf_expand, i, em->heap_id );
	}
	
	GFL_STR_DeleteBuffer(strbuf_src);
	GFL_STR_DeleteBuffer(strbuf_expand);
	GFL_STR_DeleteBuffer(strbuf_eom);
	
	em->parentsearch.list_strbuf_create = TRUE;
}

//==================================================================
/**
 * �G���g���[���̐e�@����̌��ʂ��Z�b�g
 *
 * @param   em		
 * @param   answer		
 * @param   myst		  
 */
//==================================================================
void CommEntryMenu_SetEntryAnswer(COMM_ENTRY_MENU_PTR em, ENTRY_PARENT_ANSWER answer, const MYSTATUS *myst)
{
  em->entry_parent_answer = answer;
  MyStatus_Copy(myst, &em->entry_parent_myst);
}

//==================================================================
/**
 * ENTRYMENU_MEMBER_INFO�\���̂̃T�C�Y�擾
 *
 * @param   none		
 *
 * @retval  u32		
 */
//==================================================================
u32 CommEntryMenu_GetMemberInfoSize(void)
{
  return sizeof(ENTRYMENU_MEMBER_INFO);
}
