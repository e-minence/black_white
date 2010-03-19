//==============================================================================
/**
 * @file    intrude_subdisp.c
 * @brief   �N�������
 * @author  matsuda
 * @date    2009.10.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "field/fieldmap.h"
#include "field/intrude_subdisp.h"
#include "field/intrude_common.h"
#include "palace.naix"
#include "fieldmap/zone_id.h"
#include "intrude_types.h"
#include "infowin\infowin.h"
#include "intrude_main.h"
#include "intrude_work.h"
#include "intrude_mission.h"
#include "field/zonedata.h"
#include "intrude_comm_command.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "system/bmp_oam.h"
#include "intrude_mission_field.h"
#include "sound/pm_sndsys.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�T�u���BG�t���[����`
enum{
  INTRUDE_FRAME_S_NULL_0 = GFL_BG_FRAME0_S,          ///<���g�p
  INTRUDE_FRAME_S_NULL_1 = GFL_BG_FRAME1_S,          ///<���g�p
  INTRUDE_FRAME_S_BAR = GFL_BG_FRAME2_S,             ///<�v���[�g��
  INTRUDE_FRAME_S_BACKGROUND = GFL_BG_FRAME3_S,      ///<�w�i��
};

///�C���t�H���[�V�������b�Z�[�W�X�V�E�F�C�g
#define INFOMSG_UPDATE_WAIT   (90)

///�t�H���gBG�p���b�g�W�J�ʒu
#define _FONT_BG_PALNO      (14)

///�A�N�^�[Index
enum{
  INTSUB_ACTOR_TOUCH_TOWN_0,
  INTSUB_ACTOR_TOUCH_TOWN_1,
  INTSUB_ACTOR_TOUCH_TOWN_2,
  INTSUB_ACTOR_TOUCH_TOWN_3,
  INTSUB_ACTOR_TOUCH_TOWN_4,
  INTSUB_ACTOR_TOUCH_TOWN_5,
  INTSUB_ACTOR_TOUCH_TOWN_6,
  INTSUB_ACTOR_TOUCH_TOWN_7,
  INTSUB_ACTOR_TOUCH_TOWN_MAX = INTSUB_ACTOR_TOUCH_TOWN_7,

  INTSUB_ACTOR_TOUCH_PALACE,
  
  INTSUB_ACTOR_AREA_0,
  INTSUB_ACTOR_AREA_1,
  INTSUB_ACTOR_AREA_2,
  INTSUB_ACTOR_AREA_3,
  INTSUB_ACTOR_AREA_MAX = INTSUB_ACTOR_AREA_3,

  INTSUB_ACTOR_CUR_S_0,
  INTSUB_ACTOR_CUR_S_1,
  INTSUB_ACTOR_CUR_S_2,
  INTSUB_ACTOR_CUR_S_3,
  INTSUB_ACTOR_CUR_S_MAX = INTSUB_ACTOR_CUR_S_3,
  
  INTSUB_ACTOR_CUR_L,       ///<�����̋��ꏊ���w��
  INTSUB_ACTOR_ENTRY,       ///<�u�Q���v�{�^��
  
//  INTSUB_ACTOR_MARK,        ///<�����̂���G���A���w��

  INTSUB_ACTOR_LV_NUM_KETA_0,
  INTSUB_ACTOR_LV_NUM_KETA_1,
  INTSUB_ACTOR_LV_NUM_KETA_2,
  INTSUB_ACTOR_LV_NUM_KETA_MAX = INTSUB_ACTOR_LV_NUM_KETA_2,

  INTSUB_ACTOR_POINT_NUM_KETA_0,
  INTSUB_ACTOR_POINT_NUM_KETA_1,
  INTSUB_ACTOR_POINT_NUM_KETA_2,
  INTSUB_ACTOR_POINT_NUM_KETA_3,
  INTSUB_ACTOR_POINT_NUM_KETA_4,
  INTSUB_ACTOR_POINT_NUM_KETA_5,
  INTSUB_ACTOR_POINT_NUM_KETA_6,
  INTSUB_ACTOR_POINT_NUM_KETA_7,
  INTSUB_ACTOR_POINT_NUM_KETA_8,
  INTSUB_ACTOR_POINT_NUM_KETA_MAX = INTSUB_ACTOR_POINT_NUM_KETA_8,

  INTSUB_ACTOR_MAX,
};

///BMPOAM�Ŏg�p����A�N�^�[��
#define INTSUB_ACTOR_BMPOAM_NUM   (16 + 2)

///OBJ�p���b�gINDEX
enum{
  INTSUB_ACTOR_PAL_BASE_START = 1,    ///<�v���C���[���ɕς��p���b�g�J�n�ʒu
  
  INTSUB_ACTOR_PAL_MAX = 7,
  
  INTSUB_ACTOR_PAL_FONT = 0xd,
};

///palace_obj.nce�̃A�j���[�V�����V�[�P���X
enum{
//  PALACE_ACT_ANMSEQ_TOWN_G,
//  PALACE_ACT_ANMSEQ_TOWN_W,
//  PALACE_ACT_ANMSEQ_TOWN_B,
  PALACE_ACT_ANMSEQ_AREA,
  PALACE_ACT_ANMSEQ_CUR_S,
  PALACE_ACT_ANMSEQ_CUR_L_MALE,
  PALACE_ACT_ANMSEQ_CUR_L_FEMALE,
//  PALACE_ACT_ANMSEQ_MARK,
//  PALACE_ACT_ANMSEQ_POWER,
//  PALACE_ACT_ANMSEQ_MISSION,
//  PALACE_ACT_ANMSEQ_INCLUSION,
  PALACE_ACT_ANMSEQ_SENKYO_EFF,
  PALACE_ACT_ANMSEQ_LV_NUM,
  PALACE_ACT_ANMSEQ_POINT_NUM,
  PALACE_ACT_ANMSEQ_TOUCH_TOWN,
  PALACE_ACT_ANMSEQ_TOUCH_PALACE,

  PALACE_ACT_ANMSEQ_BTN,
  PALACE_ACT_ANMSEQ_WARP_OK,
  PALACE_ACT_ANMSEQ_WARP_NG,
};

///�A�N�^�[�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_TOUCH_TOWN = 90,
  SOFTPRI_AREA = 50,
  SOFTPRI_CUR_S = 20,
  SOFTPRI_CUR_L = 19,
  SOFTPRI_LV_NUM = 10,
  SOFTPRI_POINT_NUM = 10,
  SOFTPRI_INFOMSG = 8,
  SOFTPRI_ENTRY_BUTTON = 5,
  SOFTPRI_ENTRY_BUTTON_MSG = 4,
};
///�A�N�^�[����BG�v���C�I���e�B
#define BGPRI_ACTOR_COMMON      (2)

///BG�p���b�gINDEX
enum{
  INTSUB_BG_PAL_INFOMSG_BACK_NORMAL = 0, ///<info���b�Z�[�W�̔w�i�F(�m�[�}��)
  INTSUB_BG_PAL_INFOMSG_BACK_ACTIVE = 9, ///<info���b�Z�[�W�̔w�i�F(�A�N�e�B�u��)
  
  INTSUB_BG_PAL_BASE_START_TOWN = 1,   ///<�v���C���[���ɕς��p���b�g�J�n�ʒu(�XBG)
  INTSUB_BG_PAL_BASE_START_BACK = 5,   ///<�v���C���[���ɕς��p���b�g�J�n�ʒu(�w�i)
  
  INTSUB_BG_PAL_MAX = 10,
};

///�X�A�C�R���X�V���N�G�X�g
enum{
  TOWN_UPDATE_REQ_NONE,           ///<���N�G�X�g����
  TOWN_UPDATE_REQ_NOT_EFFECT,     ///<�X�A�C�R���̍X�V�͂��邪�苒�G�t�F�N�g�͖���
  TOWN_UPDATE_REQ_UPDATE,         ///<�X�A�C�R���̍X�V�{�ω�������ΐ苒�G�t�F�N�g
};

enum{
  _TOWN_NO_FREE = 0xfe,       ///<�X�ł��p���X�ł��Ȃ�
  _TOWN_NO_PALACE = 0xff,     ///<�p���X
};

enum{
  NO_TOWN_CURSOR_POS_SPACE_X = 24,
  NO_TOWN_CURSOR_POS_X = 128 - NO_TOWN_CURSOR_POS_SPACE_X * 2,
  NO_TOWN_CURSOR_POS_Y = 0xe*8,
};

enum{
  TOUCH_RANGE_PLAYER_ICON_X = 8,    ///<����v���C���[�A�C�R���̃^�b�`�͈�X(���a)
  TOUCH_RANGE_PLAYER_ICON_Y = 8,    ///<����v���C���[�A�C�R���̃^�b�`�͈�Y(���a)
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///�N���G���A�ɂ���ăJ���[��ύX����BG�X�N���[���̈ʒu�ƃT�C�Y
enum{
  _AREA_SCREEN_COLOR_START_X = 0,
  _AREA_SCREEN_COLOR_START_Y = 2,
  _AREA_SCREEN_COLOR_SIZE_X = 32,
  _AREA_SCREEN_COLOR_SIZE_Y = 16,
};

///�C���t�H���[�V�������b�Z�[�W�̃A�N�e�B�u���ɂ���ăJ���[��ύX����BG�X�N���[���̈ʒu�ƃT�C�Y
enum{
  _INFOMSG_SCREEN_COLOR_START_X = 0,
  _INFOMSG_SCREEN_COLOR_START_Y = 0x12,
  _INFOMSG_SCREEN_COLOR_SIZE_X = 32,
  _INFOMSG_SCREEN_COLOR_SIZE_Y = 4,
};

enum{
  INFOMSG_BMP_SIZE_X = 30,  //�L�����P��
  INFOMSG_BMP_SIZE_Y = 4,   //�L�����P��
};

enum{
  ENTRYMSG_BMP_SIZE_X = 6,  //�L�����P��
  ENTRYMSG_BMP_SIZE_Y = 2,   //�L�����P��
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///�X�A�C�R���̃^�b�`����̋�`�n�[�t�T�C�Y
#define TOWN_ICON_HITRANGE_HALF   (8)

enum{
  BG_BAR_TYPE_SINGLE,   ///<��l�p��BG�X�N���[��(palace_bar.nsc)
  BG_BAR_TYPE_COMM,     ///<�ʐM�p��BG�X�N���[��(palace_bar2.nsc)
};

///�^�C�g�����b�Z�[�W�\���^�C�v
enum{
  _TITLE_PRINT_NULL,              ///<�����\�����Ă��Ȃ�
  _TITLE_PRINT_PALACE_GO,         ///<�^�b�`�Ńp���X�ւ����܂�
  _TITLE_PRINT_MY_PALACE,         ///<�����̃p���X�ɂ��܂�
  _TITLE_PRINT_OTHER_PALACE0,     ///<�ʐM����̃p���X�ɂ��܂�
  _TITLE_PRINT_OTHER_PALACE1,     ///<�ʐM����̃p���X�ɂ��܂�
  _TITLE_PRINT_OTHER_PALACE2,     ///<�ʐM����̃p���X�ɂ��܂�
  _TITLE_PRINT_OTHER_PALACE3,     ///<�ʐM����̃p���X�ɂ��܂�
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�N������ʐ��䃏�[�N
typedef struct _INTRUDE_SUBDISP{
  GAMESYS_WORK *gsys;
  
  u32 index_cgr;
  u32 index_pltt;
  u32 index_pltt_font;
  u32 index_cell;

  GFL_FONT *font_handle;
  GFL_TCBLSYS *tcbl_sys;
  WORDSET *wordset;
  PRINT_QUE *print_que;
	GFL_MSGDATA *msgdata;
	
	STRBUF *strbuf_temp;
	STRBUF *strbuf_info;
	STRBUF *strbuf_title;
	
	PRINT_UTIL printutil_title;  ///<�^�C�g�����b�Z�[�W
	
	GFL_CLUNIT *clunit;
  GFL_CLWK *act[INTSUB_ACTOR_MAX];

  BMPOAM_SYS_PTR bmpoam_sys;
  GFL_BMP_DATA *entrymsg_bmp;
  GFL_BMP_DATA *infomsg_bmp;
  BMPOAM_ACT_PTR entrymsg_bmpoam;
  BMPOAM_ACT_PTR infomsg_bmpoam;
  
  u8 my_net_id;
  u8 town_update_req;     ///<�X�A�C�R���X�V���N�G�X�g(TOWN_UPDATE_REQ_???)
  u8 now_bg_pal;          ///<���݂�BG�̃p���b�g�ԍ�
  u8 wfbc_go;             ///<TRUE:WFBC�ւ̃��[�v��������
  
  s16 infomsg_wait;       ///<�C���t�H���[�V�������b�Z�[�W�X�V�E�F�C�g
  u8 wfbc_seq;
  u8 bar_type;            ///<BG_BAR_TYPE_xxx
  
  u8 title_print_type;    ///<_TITLE_PRINT_xxx
  u8 infomsg_trans_req;
  u8 mission_ready_msg_seq;
  u8 mission_exe_msg_seq;
}INTRUDE_SUBDISP;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _IntSub_SystemSetup(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_SystemExit(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_BGUnload(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BmpWinAdd(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BmpWinDel(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BmpOamCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_BmpOamDelete(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorResouceLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorResourceUnload(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorDelete(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate_TouchTown(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_Area(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorS(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorL(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_LvNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_PointNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_EntryButton(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_Delete_EntryButton(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorUpdate_TouchTown(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_Area(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_CursorS(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_CursorL(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_EntryButton(INTRUDE_SUBDISP_PTR intsub, 
  INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_LvNum(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_PointNum(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static OCCUPY_INFO * _IntSub_GetArreaOccupy(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_TitleMsgUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_InfoMsgUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static u8 _IntSub_GetProfileRecvNum(INTRUDE_COMM_SYS_PTR intcomm);
static u8 _IntSub_TownPosGet(ZONEID zone_id, GFL_CLACTPOS *dest_pos, int net_id);
static void _SetRect(int x, int y, int half_size_x, int half_size_y, GFL_RECT *rect);
static BOOL _CheckRectHit(int x, int y, const GFL_RECT *rect);
static void _IntSub_TouchUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGBarUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGColorUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);


//==============================================================================
//  �f�[�^
//==============================================================================
///�p���X�A�C�R�����W
enum{
  PALACE_ICON_POS_X = 128,      ///<�p���X�A�C�R�����WX
  PALACE_ICON_POS_Y = 8*0xa,    ///<�p���X�A�C�R�����WY
  PALACE_ICON_HITRANGE_HALF = 16, ///<�p���X�A�C�R���̃^�b�`���蔼�a
  
  PALACE_CURSOR_POS_X = PALACE_ICON_POS_X,      ///<�p���X�A�C�R�����J�[�\�����w���ꍇ�̍��WX
  PALACE_CURSOR_POS_Y = PALACE_ICON_POS_Y - 20, ///<�p���X�A�C�R�����J�[�\�����w���ꍇ�̍��WY
};

///�G���A�A�C�R���z�u���W
enum{
  AREA_POST_LEFT = 0x5 * 8,                           ///<�z�u���W�͈͂̍��[X
  AREA_POST_RIGHT = 0x1b * 8,                         ///<�z�u���W�͈͂̉E�[X
  AREA_POST_WIDTH = AREA_POST_RIGHT - AREA_POST_LEFT, ///<�z�u���W�͈͂̕�
  
  AREA_POST_Y = 3*8 + 4,                              ///<�z�u���WY
};

///�Q���{�^���̃A�C�R�����W
enum{
  ENTRY_BUTTON_POS_X = 0x1c*8,        ///<�Q���{�^���̃A�C�R�����WX
  ENTRY_BUTTON_POS_Y = 0x10*8,        ///<�Q���{�^���̃A�C�R�����WY
  ENTRY_BUTTON_HITRANGE_HALF_X = 16,  ///<�Q���{�^���̃^�b�`���蔼�aX
  ENTRY_BUTTON_HITRANGE_HALF_Y = 8,   ///<�Q���{�^���̃^�b�`���蔼�aY
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �N������ʁF�����ݒ�
 *
 * @param   gsys		
 *
 * @retval  INTRUDE_SUBDISP_PTR		
 */
//==================================================================
INTRUDE_SUBDISP_PTR INTRUDE_SUBDISP_Init(GAMESYS_WORK *gsys)
{
  INTRUDE_SUBDISP_PTR intsub;
  ARCHANDLE *handle;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  intsub = GFL_HEAP_AllocClearMemory(HEAPID_FIELD_SUBSCREEN, sizeof(INTRUDE_SUBDISP));
  intsub->gsys = gsys;
  intsub->now_bg_pal = 0xff;  //����ɕK���X�V��������悤��0xff
  
  intsub->my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  if(intsub->my_net_id > FIELD_COMM_MEMBER_MAX){
    GF_ASSERT_MSG(0, "my_net_id = %d\n", intsub->my_net_id);
    intsub->my_net_id = 0;
  }
  
  handle = GFL_ARC_OpenDataHandle(ARCID_PALACE, HEAPID_FIELDMAP);
  
  _IntSub_SystemSetup(intsub);
  _IntSub_BGLoad(intsub, handle);
  _IntSub_BmpWinAdd(intsub);
  _IntSub_ActorResouceLoad(intsub, handle);
  _IntSub_ActorCreate(intsub, handle);
  _IntSub_BmpOamCreate(intsub, handle);
  
  GFL_ARC_CloseDataHandle(handle);

  return intsub;
}

//==================================================================
/**
 * �N������ʁF�j���ݒ�
 *
 * @param   intsub		
 */
//==================================================================
void INTRUDE_SUBDISP_Exit(INTRUDE_SUBDISP_PTR intsub)
{
  _IntSub_BmpOamDelete(intsub);
  _IntSub_Delete_EntryButton(intsub);
  _IntSub_ActorDelete(intsub);
  _IntSub_ActorResourceUnload(intsub);
  _IntSub_BmpWinDel(intsub);
  _IntSub_BGUnload(intsub);
  _IntSub_SystemExit(intsub);

  GFL_HEAP_FreeMemory(intsub);
}

//==================================================================
/**
 * �N������ʁF�X�V
 *
 * @param   intsub		
 */
//==================================================================
void INTRUDE_SUBDISP_Update(INTRUDE_SUBDISP_PTR intsub)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
	GFL_TCBL_Main( intsub->tcbl_sys );
	PRINTSYS_QUE_Main( intsub->print_que );
  PRINT_UTIL_Trans( &intsub->printutil_title, intsub->print_que );
  if(intsub->infomsg_trans_req == TRUE 
      && PRINTSYS_QUE_IsExistTarget(intsub->print_que, intsub->infomsg_bmp) == FALSE){
    BmpOam_ActorBmpTrans(intsub->infomsg_bmpoam);
    intsub->infomsg_trans_req = FALSE;
  }
  if(PRINTSYS_QUE_IsExistTarget(intsub->print_que, intsub->entrymsg_bmp) == FALSE){
    BmpOam_ActorBmpTrans(intsub->entrymsg_bmpoam);
  }
  
  //�^�b�`����`�F�b�N
  _IntSub_TouchUpdate(intcomm, intsub);
  
  //WFBC�ւ̃��[�v�`�F�b�N
  if(intsub->wfbc_go == TRUE){
    int palace_area = Intrude_GetPalaceArea(intcomm);
    
    if(GFL_NET_IsConnectMember(palace_area) == FALSE){
      intsub->wfbc_go = FALSE;
      OS_TPrintf("WFBC�ւ̔�ѐ摊�肪���Ȃ��Ȃ����̂ŃL�����Z�� netID=%d\n", palace_area);
    }
    else{
      switch(intsub->wfbc_seq){
      case 0:
        if(IntrudeSend_WfbcReq(intcomm, palace_area) == TRUE){
          intsub->wfbc_seq++;
        }
        break;
      case 1:
        if(Intrude_GetRecvWfbc(intcomm) == TRUE){
          FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(intsub->gsys);
          FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);

          Intrude_SetWarpTown(game_comm, PALACE_TOWN_WFBC);
          FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP);
          intsub->wfbc_go = FALSE;
        }
        break;
      default:
        GF_ASSERT(0);
        break;
      }
    }
  }
}

//==================================================================
/**
 * �N������ʁF�`��
 *
 * @param   intsub		
 */
//==================================================================
void INTRUDE_SUBDISP_Draw(INTRUDE_SUBDISP_PTR intsub)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  OCCUPY_INFO *area_occupy;
  
  if(intcomm == NULL){
    return;
  }

  area_occupy = _IntSub_GetArreaOccupy(intcomm, intsub);

  _IntSub_BGBarUpdate(intcomm, intsub);
  //BG�X�N���[���J���[�ύX�`�F�b�N
  _IntSub_BGColorUpdate(intcomm, intsub);
  
  //�C���t�H���[�V�������b�Z�[�W
  _IntSub_TitleMsgUpdate(intcomm, intsub);
  _IntSub_InfoMsgUpdate(intcomm, intsub);
  
  //�A�N�^�[�X�V
  _IntSub_ActorUpdate_TouchTown(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_Area(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_CursorS(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_CursorL(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_EntryButton(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_LvNum(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_PointNum(intsub, intcomm, area_occupy);
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̃Z�b�g�A�b�v
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_SystemSetup(INTRUDE_SUBDISP_PTR intsub)
{
  intsub->clunit = GFL_CLACT_UNIT_Create(
    INTSUB_ACTOR_MAX + INTSUB_ACTOR_BMPOAM_NUM, 5, HEAPID_FIELD_SUBSCREEN);
  intsub->bmpoam_sys = BmpOam_Init(HEAPID_FIELD_SUBSCREEN, intsub->clunit);

	intsub->tcbl_sys = GFL_TCBL_Init(HEAPID_FIELD_SUBSCREEN, HEAPID_FIELD_SUBSCREEN, 4, 32);
	intsub->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_FIELD_SUBSCREEN );
	intsub->wordset = WORDSET_Create(HEAPID_FIELD_SUBSCREEN);
  intsub->print_que = PRINTSYS_QUE_Create( HEAPID_FIELD_SUBSCREEN );
  intsub->msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
    NARC_message_invasion_dat, HEAPID_FIELD_SUBSCREEN );
  
  intsub->strbuf_temp = GFL_STR_CreateBuffer(64, HEAPID_FIELD_SUBSCREEN);
  intsub->strbuf_info = GFL_STR_CreateBuffer(64, HEAPID_FIELD_SUBSCREEN);
  intsub->strbuf_title = GFL_STR_CreateBuffer(64, HEAPID_FIELD_SUBSCREEN);
}

//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̔j��
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_SystemExit(INTRUDE_SUBDISP_PTR intsub)
{
  GFL_STR_DeleteBuffer(intsub->strbuf_temp);
  GFL_STR_DeleteBuffer(intsub->strbuf_info);
  GFL_STR_DeleteBuffer(intsub->strbuf_title);
  
  GFL_MSG_Delete(intsub->msgdata);
  PRINTSYS_QUE_Clear(intsub->print_que);
  PRINTSYS_QUE_Delete(intsub->print_que);
  WORDSET_Delete(intsub->wordset);
	GFL_FONT_Delete(intsub->font_handle);
	GFL_TCBL_Exit(intsub->tcbl_sys);

  BmpOam_Exit(intsub->bmpoam_sys);
  GFL_CLACT_UNIT_Delete(intsub->clunit);
}

//--------------------------------------------------------------
/**
 * BG���[�h
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_BGLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
	static const GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
  	{//INTRUDE_FRAME_S_BAR
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x7000,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
  	{//INTRUDE_FRAME_S_BACKGROUND
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x7000,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
	};
	GFL_BG_SetBGControl( INTRUDE_FRAME_S_BAR, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( INTRUDE_FRAME_S_BACKGROUND, &TextBgCntDat[1], GFL_BG_MODE_TEXT );

	//BG VRAM�N���A
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);

	//���W�X�^OFF
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2S_BlendNone();
	
  //�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    handle, NARC_palace_palace_bg_lz_NCGR, INTRUDE_FRAME_S_BACKGROUND, 0, 
    0, TRUE, HEAPID_FIELDMAP);
  
  //�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_palace_palace_bg_lz_NSCR, INTRUDE_FRAME_S_BACKGROUND, 0, 
    0x800, TRUE, HEAPID_FIELDMAP);
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_palace_palace_bar_lz_NSCR, INTRUDE_FRAME_S_BAR, 0, 
    0x800, TRUE, HEAPID_FIELDMAP);
  
  //�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_palace_palace_bg_NCLR, PALTYPE_SUB_BG, 0, 
    0x20 * INTSUB_BG_PAL_MAX, HEAPID_FIELDMAP);
	//�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, 
    PALTYPE_SUB_BG, _FONT_BG_PALNO * 0x20, 0x20, HEAPID_FIELDMAP);

	GFL_BG_SetVisible( INTRUDE_FRAME_S_NULL_0, VISIBLE_OFF );
	GFL_BG_SetVisible( INTRUDE_FRAME_S_NULL_1, VISIBLE_OFF );
	GFL_BG_SetVisible( INTRUDE_FRAME_S_BAR, VISIBLE_ON );
	GFL_BG_SetVisible( INTRUDE_FRAME_S_BACKGROUND, VISIBLE_ON );
}

//--------------------------------------------------------------
/**
 * BG�A�����[�h
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BGUnload(INTRUDE_SUBDISP_PTR intsub)
{
	GFL_BG_SetVisible( INTRUDE_FRAME_S_BAR, VISIBLE_OFF );
	GFL_BG_SetVisible( INTRUDE_FRAME_S_BACKGROUND, VISIBLE_OFF );
	GFL_BG_FreeBGControl(INTRUDE_FRAME_S_BAR);
	GFL_BG_FreeBGControl(INTRUDE_FRAME_S_BACKGROUND);
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BmpWinAdd(INTRUDE_SUBDISP_PTR intsub)
{
  GFL_BMPWIN *bmpwin;
  
  bmpwin = GFL_BMPWIN_Create( INTRUDE_FRAME_S_BAR, 
    0, 0, 0x20, 2, _FONT_BG_PALNO, GFL_BMP_CHRAREA_GET_B );
  PRINT_UTIL_Setup( &intsub->printutil_title, bmpwin );
  GFL_BMPWIN_MakeTransWindow(bmpwin);
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BmpWinDel(INTRUDE_SUBDISP_PTR intsub)
{
  GFL_BMPWIN_Delete(intsub->printutil_title.win);
}

//--------------------------------------------------------------
/**
 * BMPOAM�쐬
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_BmpOamCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  BMPOAM_ACT_DATA bmpoam_head = {
    NULL,   //OAM�Ƃ��ĕ\��������BMP�f�[�^�ւ̃|�C���^
    8, 0x12*8,   //X, Y
    0,      //�K�p����p���b�g��index(GFL_CLGRP_PLTT_Register�̖߂�l)
    0,      //pal_offset(pltt_index�̃p���b�g���ł̃I�t�Z�b�g)
    SOFTPRI_INFOMSG,
    BGPRI_ACTOR_COMMON,
    CLSYS_DEFREND_SUB,
    CLSYS_DRAW_SUB,
  };

  intsub->infomsg_bmp = GFL_BMP_Create( 
    INFOMSG_BMP_SIZE_X, INFOMSG_BMP_SIZE_Y, GFL_BMP_16_COLOR, HEAPID_FIELD_SUBSCREEN );
  
  bmpoam_head.bmp = intsub->infomsg_bmp;
  bmpoam_head.pltt_index = intsub->index_pltt_font;
  intsub->infomsg_bmpoam = BmpOam_ActorAdd(intsub->bmpoam_sys, &bmpoam_head);
}

//--------------------------------------------------------------
/**
 * BMPOAM�폜
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BmpOamDelete(INTRUDE_SUBDISP_PTR intsub)
{
  BmpOam_ActorDel(intsub->infomsg_bmpoam);
  GFL_BMP_Delete(intsub->infomsg_bmp);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p���郊�\�[�X�̃��[�h
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorResouceLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  //OBJ���ʃp���b�g
  intsub->index_pltt = GFL_CLGRP_PLTT_RegisterEx(
    handle, NARC_palace_palace_obj_NCLR, CLSYS_DRAW_SUB, 0, 0, 
    INTSUB_ACTOR_PAL_MAX, HEAPID_FIELDMAP );

  //OBJ���ʃ��\�[�X
  intsub->index_cgr = GFL_CLGRP_CGR_Register(
    handle, NARC_palace_palace_obj_lz_NCGR, TRUE, CLSYS_DRAW_SUB, HEAPID_FIELDMAP);
  intsub->index_cell = GFL_CLGRP_CELLANIM_Register(
    handle, NARC_palace_palace_obj_NCER, 
    NARC_palace_palace_obj_NANR, HEAPID_FIELD_SUBSCREEN);
  
  //font oam�p�̃t�H���g�p���b�g
  {
    ARCHANDLE *font_pal_handle;
    font_pal_handle = GFL_ARC_OpenDataHandle(ARCID_FONT, HEAPID_FIELDMAP);
    intsub->index_pltt_font = GFL_CLGRP_PLTT_RegisterComp(
      font_pal_handle, NARC_font_default_nclr, CLSYS_DRAW_SUB, 
      INTSUB_ACTOR_PAL_FONT * 0x20, HEAPID_FIELDMAP );
    GFL_ARC_CloseDataHandle(font_pal_handle);
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p����A�N�^�[�̃A�����[�h
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_ActorResourceUnload(INTRUDE_SUBDISP_PTR intsub)
{
  GFL_CLGRP_PLTT_Release(intsub->index_pltt_font);
  GFL_CLGRP_PLTT_Release(intsub->index_pltt);
  GFL_CLGRP_CGR_Release(intsub->index_cgr);
  GFL_CLGRP_CELLANIM_Release(intsub->index_cell);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�쐬
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  _IntSub_ActorCreate_TouchTown(intsub, handle);
  _IntSub_ActorCreate_Area(intsub, handle);
  _IntSub_ActorCreate_CursorS(intsub, handle);
  _IntSub_ActorCreate_CursorL(intsub, handle);
  _IntSub_ActorCreate_LvNum(intsub, handle);
  _IntSub_ActorCreate_PointNum(intsub, handle);
  _IntSub_ActorCreate_EntryButton(intsub, handle);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�폜
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_ActorDelete(INTRUDE_SUBDISP_PTR intsub)
{
  int i;
  
  for(i = 0; i < INTSUB_ACTOR_MAX; i++){
    GFL_CLACT_WK_Remove(intsub->act[i]);
  }
}

//--------------------------------------------------------------
/**
 * �^�b�`�o����X�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_TouchTown(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_TOUCH_TOWN,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_TOUCH_TOWN,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  GF_ASSERT(PALACE_TOWN_DATA_MAX == (INTSUB_ACTOR_TOUCH_TOWN_MAX - INTSUB_ACTOR_TOUCH_TOWN_0 + 1));
  for(i = INTSUB_ACTOR_TOUCH_TOWN_0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX; i++){
    head.pos_x = PalaceTownData[i - INTSUB_ACTOR_TOUCH_TOWN_0].subscreen_x;
    head.pos_y = PalaceTownData[i - INTSUB_ACTOR_TOUCH_TOWN_0].subscreen_y;
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
  }
  
  //�^�b�`�p���X��
  head.anmseq = PALACE_ACT_ANMSEQ_TOUCH_PALACE;
  head.pos_x = 128;
  head.pos_y = 0xb*8+4;
  intsub->act[INTSUB_ACTOR_TOUCH_PALACE] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], FALSE);  //�\��OFF
}

//--------------------------------------------------------------
/**
 * �ڑ��G���A�A�C�R���A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_Area(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_AREA,     //�A�j���[�V�����V�[�P���X
  	SOFTPRI_AREA,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  for(i = INTSUB_ACTOR_AREA_0; i <= INTSUB_ACTOR_AREA_MAX; i++){
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetPlttOffs(intsub->act[i], 
      INTSUB_ACTOR_PAL_BASE_START + i-INTSUB_ACTOR_AREA_0, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
  }
}

//--------------------------------------------------------------
/**
 * �ʐM����̋��ꏊ�������J�[�\���A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_CursorS(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_CUR_S,    //�A�j���[�V�����V�[�P���X
  	SOFTPRI_CUR_S,              //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  for(i = INTSUB_ACTOR_CUR_S_0; i <= INTSUB_ACTOR_CUR_S_MAX; i++){
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetPlttOffs(intsub->act[i], 
      INTSUB_ACTOR_PAL_BASE_START + i-INTSUB_ACTOR_CUR_S_0, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
    GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[i], TRUE);  //�I�[�g�A�j��ON
  }
}

//--------------------------------------------------------------
/**
 * �����̋��ꏊ�������J�[�\���A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_CursorL(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_CUR_L_MALE,     //�A�j���[�V�����V�[�P���X
  	SOFTPRI_CUR_L,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  MYSTATUS *myst = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(intsub->gsys));
  
  if(MyStatus_GetMySex(myst) == PM_FEMALE){
    head.anmseq = PALACE_ACT_ANMSEQ_CUR_L_FEMALE;
  }
  intsub->act[INTSUB_ACTOR_CUR_L] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_CUR_L], 
    INTSUB_ACTOR_PAL_BASE_START + intsub->my_net_id, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_CUR_L], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_CUR_L], TRUE);  //�I�[�g�A�j��ON
}

//--------------------------------------------------------------
/**
 * ���x�����l�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_LvNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_LV_NUM,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_LV_NUM,             //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  static const struct{
    u16 x;
    u16 y;
  }LvNumPos[] = {
    {0x1a*8 + 4*8, 0x16*8},   //��̈�
    {0x1a*8 + 2*8, 0x16*8},   //�\�̈�
    {0x1a*8, 0x16*8},         //�S�̈�
  };
  
  GF_ASSERT(NELEMS(LvNumPos) == (INTSUB_ACTOR_LV_NUM_KETA_MAX - INTSUB_ACTOR_LV_NUM_KETA_0 + 1));
  for(i = INTSUB_ACTOR_LV_NUM_KETA_0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX; i++){
    head.pos_x = LvNumPos[i - INTSUB_ACTOR_LV_NUM_KETA_0].x;
    head.pos_y = LvNumPos[i - INTSUB_ACTOR_LV_NUM_KETA_0].y;
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], TRUE);
  }
}

//--------------------------------------------------------------
/**
 * �|�C���g���l�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_PointNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	4 * 8, 0x17 * 8,               //X, Y���W
  	PALACE_ACT_ANMSEQ_POINT_NUM,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_POINT_NUM,             //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,            //BG�v���C�I���e�B
  };
  u16 width = 8;
  
  head.pos_x += width * (INTSUB_ACTOR_POINT_NUM_KETA_MAX - INTSUB_ACTOR_POINT_NUM_KETA_0);
  for(i = INTSUB_ACTOR_POINT_NUM_KETA_0; i <= INTSUB_ACTOR_POINT_NUM_KETA_MAX; i++){
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], TRUE);
    head.pos_x -= width;
  }
}

//--------------------------------------------------------------
/**
 * �Q���{�^���A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_EntryButton(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  static const GFL_CLWK_DATA head = {
  	ENTRY_BUTTON_POS_X, ENTRY_BUTTON_POS_Y,             //X, Y���W
  	PALACE_ACT_ANMSEQ_BTN,      //�A�j���[�V�����V�[�P���X
  	SOFTPRI_ENTRY_BUTTON,       //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  intsub->act[INTSUB_ACTOR_ENTRY] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], FALSE);  //�\��OFF

  {//�u���񂩁vBMPOAM
    BMPOAM_ACT_DATA bmpoam_head = {
      NULL,   //OAM�Ƃ��ĕ\��������BMP�f�[�^�ւ̃|�C���^
      ENTRY_BUTTON_POS_X - 16, ENTRY_BUTTON_POS_Y - 8,   //X, Y
      0,      //�K�p����p���b�g��index(GFL_CLGRP_PLTT_Register�̖߂�l)
      0,      //pal_offset(pltt_index�̃p���b�g���ł̃I�t�Z�b�g)
      SOFTPRI_ENTRY_BUTTON_MSG,
      BGPRI_ACTOR_COMMON,
      CLSYS_DEFREND_SUB,
      CLSYS_DRAW_SUB,
    };
    STRBUF *entry_str;
    
    intsub->entrymsg_bmp = GFL_BMP_Create( 
      ENTRYMSG_BMP_SIZE_X, ENTRYMSG_BMP_SIZE_Y, GFL_BMP_16_COLOR, HEAPID_FIELD_SUBSCREEN );
    
    bmpoam_head.bmp = intsub->entrymsg_bmp;
    bmpoam_head.pltt_index = intsub->index_pltt_font;
    intsub->entrymsg_bmpoam = BmpOam_ActorAdd(intsub->bmpoam_sys, &bmpoam_head);
    BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam, FALSE);

    entry_str = GFL_MSG_CreateString( intsub->msgdata, msg_invasion_subdisp_000 );
    PRINTSYS_PrintQueColor( intsub->print_que, intsub->entrymsg_bmp, 0, 0, entry_str, 
      intsub->font_handle, PRINTSYS_MACRO_LSB(15,2,0) );
    GFL_STR_DeleteBuffer(entry_str);
  }
}

//--------------------------------------------------------------
/**
 * �u���񂩁v�{�^���̍폜
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_Delete_EntryButton(INTRUDE_SUBDISP_PTR intsub)
{
  //�A�N�^�[�͂܂Ƃ߂č폜�����̂ŁA����ȊO��BMPOAM�����폜
  BmpOam_ActorDel(intsub->entrymsg_bmpoam);
  GFL_BMP_Delete(intsub->entrymsg_bmp);
}

//==============================================================================
//  �X�V
//==============================================================================
//--------------------------------------------------------------
/**
 * �X�V�����F�^�b�`�X�A�N�^�[
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_TouchTown(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i;
  
  if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    //�����̃G���A�ׁ̈A�p���X�������^�b�`�o���Ȃ�
    for(i = INTSUB_ACTOR_TOUCH_TOWN_0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX; i++){
      GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);
    }
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], TRUE);
  }
  else{
    for(i = INTSUB_ACTOR_TOUCH_TOWN_0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX; i++){
      GFL_CLACT_WK_SetDrawEnable(intsub->act[i], TRUE);
    }
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], TRUE);
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�G���A�A�C�R��
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_Area(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i, profile_num, area_width, net_id;
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  int my_area, now_tbl_pos = 0;
  u8 pal_tbl[FIELD_COMM_MEMBER_MAX] = {0, 0, 0};
  s32 tbl_count = 0, actno;
  
  profile_num = _IntSub_GetProfileRecvNum(intcomm);
  my_area = intcomm->intrude_status_mine.palace_area;
  area_width = AREA_POST_WIDTH / profile_num;
  pos.x = AREA_POST_LEFT + area_width;
  pos.y = AREA_POST_Y;
  
  //�v���t�B�[����M�ς݂̃��[�U�[�̃p���b�g�I�t�Z�b�g�e�[�u�������
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intcomm->recv_profile & (1 << net_id)){
      pal_tbl[tbl_count] = net_id;  //net_id = �p���b�g�I�t�Z�b�g
      if(net_id == my_area){
        now_tbl_pos = tbl_count;  //������������p���X�G���A�̃e�[�u���ʒu���L��
      }
      tbl_count++;
    }
  }
  
  actno = 0;
  for(i = 0; i < tbl_count; i++){
    if(now_tbl_pos == i){
      continue;
    }
    act = intsub->act[INTSUB_ACTOR_AREA_0 + actno];
    GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
    GFL_CLACT_WK_SetPlttOffs(act, 
      INTSUB_ACTOR_PAL_BASE_START + pal_tbl[i], CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
    pos.x += area_width;
    actno++;
  }
  for( ; i < FIELD_COMM_MEMBER_MAX; i++){
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_AREA_0 + i], FALSE);
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�ʐM����J�[�\��
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_CursorS(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int my_area, net_id, profile_num, pos_count, s, area_width;
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  INTRUDE_STATUS *ist;
  s16 base_x;
  u8 palace_num[FIELD_COMM_MEMBER_MAX];
  u8 town_num[PALACE_TOWN_DATA_MAX];
  static const s8 TownWearOffset[] = {3, -3, 6, 0};   //�Ō��0��4BYTE�A���C�����g
  static const s8 PalaceWearOffset[] = {0, 3, -3, 0}; //�Ō��0��4BYTE�A���C�����g
  int town_no;
  
  profile_num = _IntSub_GetProfileRecvNum(intcomm);
  area_width = AREA_POST_WIDTH / (profile_num + 1);
  base_x = AREA_POST_LEFT + area_width;

  my_area = intcomm->intrude_status_mine.palace_area;
  pos_count = -1;
  
  GFL_STD_MemClear(town_num, sizeof(town_num[0]) * PALACE_TOWN_DATA_MAX);
  GFL_STD_MemClear(palace_num, sizeof(palace_num[0]) * FIELD_COMM_MEMBER_MAX);
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    act = intsub->act[INTSUB_ACTOR_CUR_S_0 + net_id];
    if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
      continue;
    }
    else if(intcomm->recv_profile & (1 << net_id)){
      ist = &intcomm->intrude_status[net_id];
    }
    else{
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
      continue;
    }
    
    if(ist->palace_area == my_area){  //���̃v���C���[������X���w��
      town_no = _IntSub_TownPosGet(ist->zone_id, &pos, net_id);
      if(town_no == _TOWN_NO_PALACE){
        town_no = 0;
      }
      if(town_no != _TOWN_NO_FREE){
        pos.x += TownWearOffset[town_num[town_no]];
        town_num[town_no]++;
      }
      GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
    }
    else{ //���̃v���C���[������p���X�G���A���w��
      if(intcomm->recv_profile & (1 << ist->palace_area)){
        pos_count = 0;
        for(s = 0; s < ist->palace_area; s++){
          if(intcomm->recv_profile & (1 << s)){
            pos_count++;
          }
        }
        pos.x = base_x + area_width * pos_count + PalaceWearOffset[palace_num[pos_count]];
        pos.y = AREA_POST_Y;
        GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
        palace_num[pos_count]++;
      }
      else{
        //���̃v���C���[������G���A���A������͂܂��v���t�B�[������M���Ă��Ȃ��G���A�̏ꍇ�͖���
        GFL_CLACT_WK_SetDrawEnable(act, FALSE);
        continue;
      }
    }
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�����J�[�\��
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_CursorL(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  PLAYER_WORK *player_work = GAMESYSTEM_GetMyPlayerWork(intsub->gsys);
  ZONEID mine_zone_id = PLAYERWORK_getZoneID(player_work);
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  int my_net_id;
  int town_no;
  
  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  town_no = _IntSub_TownPosGet(mine_zone_id, &pos, my_net_id);
  act = intsub->act[INTSUB_ACTOR_CUR_L];
  GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
  GFL_CLACT_WK_SetDrawEnable(act, TRUE);
}

//--------------------------------------------------------------
/**
 * �X�V�����F�Q�����܂��{�^��
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_EntryButton(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  MISSION_STATUS m_st;
  
  m_st = MISSION_FIELD_CheckStatus(&intcomm->mission);
  switch(m_st){
  case MISSION_STATUS_NULL:
  case MISSION_STATUS_ENTRY:
  case MISSION_STATUS_READY:
  case MISSION_STATUS_EXE:
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], FALSE);
    BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam, FALSE);
    break;
  case MISSION_STATUS_NOT_ENTRY:
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], TRUE);
    BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam, TRUE);
    break;
  default:
    GF_ASSERT(0);
    return;
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F���x�����l
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_LvNum(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i, level;
  OCCUPY_INFO *my_occupy;
  GFL_CLWK *act;
  
  my_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
//  level = my_occupy->intrude_level;
  level = MISSION_GetMissionTimer(&intcomm->mission);
  
  for(i = 0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX - INTSUB_ACTOR_LV_NUM_KETA_0; i++){
    act = intsub->act[INTSUB_ACTOR_LV_NUM_KETA_0 + i];
    GFL_CLACT_WK_SetAnmIndex(act, level % 10);
    level /= 10;
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�|�C���g���l
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_PointNum(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i, point;
  OCCUPY_INFO *my_occupy;
  GFL_CLWK *act;
  
  my_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  point = 0;//my_occupy->intrude_point;
  
  for(i = 0; i <= INTSUB_ACTOR_POINT_NUM_KETA_MAX - INTSUB_ACTOR_POINT_NUM_KETA_0; i++){
    act = intsub->act[INTSUB_ACTOR_POINT_NUM_KETA_0 + i];
    GFL_CLACT_WK_SetAnmIndex(act, point % 10);
    point /= 10;
  }
}

//--------------------------------------------------------------
/**
 * �^�C�g�����b�Z�[�W�X�V����
 *
 * @param   intcomm		
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_TitleMsgUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int print_type = _TITLE_PRINT_NULL;
  int msg_id;
  
  if(intcomm == NULL || intcomm->palace_in == FALSE){
    print_type = _TITLE_PRINT_PALACE_GO;
    msg_id = msg_invasion_title_000;
  }
  else if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    print_type = _TITLE_PRINT_MY_PALACE;
    msg_id = msg_invasion_title_001;
  }
  else{
    print_type = _TITLE_PRINT_OTHER_PALACE0 + intcomm->intrude_status_mine.palace_area;
    msg_id = msg_invasion_title_002;
  }
  
  if(intsub->title_print_type == print_type || print_type == _TITLE_PRINT_NULL){
    return; //���ɕ\���ς�
  }
  intsub->title_print_type = print_type;
  
  if(print_type >= _TITLE_PRINT_OTHER_PALACE0 && print_type <= _TITLE_PRINT_OTHER_PALACE3){
    const MYSTATUS *myst;
    myst = GAMEDATA_GetMyStatusPlayer(gamedata, intcomm->intrude_status_mine.palace_area);
    WORDSET_RegisterPlayerName( intsub->wordset, 0, myst );
  }
  
  GFL_MSG_GetString( intsub->msgdata, msg_id, intsub->strbuf_temp );
  WORDSET_ExpandStr(intsub->wordset, intsub->strbuf_title, intsub->strbuf_temp);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(intsub->printutil_title.win), 0 );
  PRINT_UTIL_PrintColor( &intsub->printutil_title, intsub->print_que, 
    0, 0, intsub->strbuf_title, intsub->font_handle, PRINTSYS_MACRO_LSB(15,2,0) );
}

//--------------------------------------------------------------
/**
 * �C���t�H���[�V�������b�Z�[�W�X�V����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_InfoMsgUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAME_COMM_INFO_MESSAGE infomsg;
  int k;
  const MISSION_DATA *md;
#if 0
  MISSION_STATUS mission_status;
#endif

  if(intsub->infomsg_trans_req == TRUE){
    return;
  }
  if(intsub->infomsg_wait > 0){
    intsub->infomsg_wait--;
    return;
  }
  
  md = MISSION_GetRecvData(&intcomm->mission);
  if(md != NULL){ //�~�b�V�����������̏ꍇ�̓~�b�V������\��
#if 0
    if(MISSION_FIELD_CheckStatus(mission) == MISSION_STATUS_READY){
#else
    if(0){
#endif
      intsub->mission_exe_msg_seq = 0;
      switch(intsub->mission_ready_msg_seq){
      case 0:
        break;
      }
    }
    else{
      intsub->mission_ready_msg_seq = 0;
    }
  }
  else if(GameCommInfo_GetMessage(game_comm, &infomsg) == TRUE){
    intsub->mission_exe_msg_seq = 0;
    intsub->mission_ready_msg_seq = 0;
    GFL_MSG_GetString(intsub->msgdata, infomsg.message_id, intsub->strbuf_temp );
    for(k = 0 ; k < INFO_WORDSET_MAX; k++){
      if(infomsg.name[k]!=NULL){
        WORDSET_RegisterWord( intsub->wordset, infomsg.wordset_no[k], infomsg.name[k], 
          PM_MALE, TRUE, PM_LANG);
      }
    }
    WORDSET_ExpandStr(intsub->wordset, intsub->strbuf_info, intsub->strbuf_temp);
    GFL_BMP_Clear( intsub->infomsg_bmp, 0 );
    PRINTSYS_PrintQueColor( intsub->print_que, intsub->infomsg_bmp, 0, 0, intsub->strbuf_info, 
      intsub->font_handle, PRINTSYS_MACRO_LSB(15,2,0) );
    intsub->infomsg_trans_req = TRUE;
    intsub->infomsg_wait = INFOMSG_UPDATE_WAIT;
    OS_TPrintf("���b�Z�[�W�`��Fid=%d\n", infomsg.message_id);
  }
}


//==============================================================================
//  �c�[��
//==============================================================================
//--------------------------------------------------------------
/**
 * �N�����Ă���G���A�̐苒���|�C���^���擾����
 *
 * @param   intcomm		
 * @param   intsub		
 *
 * @retval  OCCUPY_INFO *		�N����̐苒���
 */
//--------------------------------------------------------------
static OCCUPY_INFO * _IntSub_GetArreaOccupy(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  OCCUPY_INFO *area_occupy;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);

  if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)
      || intcomm->intrude_status_mine.palace_area == PALACE_AREA_NO_NULL){
    area_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    area_occupy = GAMEDATA_GetOccupyInfo(gamedata, intcomm->intrude_status_mine.palace_area);
  }
  
  return area_occupy;
}

//--------------------------------------------------------------
/**
 * �v���t�B�[����M�l�����擾
 * @param   intcomm		
 * @retval  u8		�v���t�B�[����M�l��
 */
//--------------------------------------------------------------
static u8 _IntSub_GetProfileRecvNum(INTRUDE_COMM_SYS_PTR intcomm)
{
  u32 recv_profile;
  u8 profile_num;
  
  recv_profile = intcomm->recv_profile; //MATH_CountPopulation���g�p���邽��u32�ɑ��
  profile_num = MATH_CountPopulation(recv_profile);
  if(profile_num > FIELD_COMM_MEMBER_MAX){
    GF_ASSERT_MSG(0, "profile_num = %d\n", profile_num);
    profile_num = 1;
  }
  return profile_num;
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���z�u���W���擾����
 *
 * @param   zone_id		
 * @param   dest_pos		
 * @param   net_id
 * 
 * @retval  �XNo(�p���X�G���A�̏ꍇ��_TOWN_NO_PALACE, �ǂ��ɂ������Ȃ��ꍇ��_TOWN_NO_FREE)
 */
//--------------------------------------------------------------
static u8 _IntSub_TownPosGet(ZONEID zone_id, GFL_CLACTPOS *dest_pos, int net_id)
{
  int i;
  
  for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
    if(PalaceTownData[i].reverse_zone_id == zone_id
        || PalaceTownData[i].front_zone_id == zone_id){
      dest_pos->x = PalaceTownData[i].subscreen_x;
      dest_pos->y = PalaceTownData[i].subscreen_y;
      return i;
    }
  }
  
  if(ZONEDATA_IsPalace(zone_id) == TRUE){
    dest_pos->x = PALACE_CURSOR_POS_X;
    dest_pos->y = PALACE_CURSOR_POS_Y;
    return _TOWN_NO_PALACE;
  }
  else{
    dest_pos->x = NO_TOWN_CURSOR_POS_X + NO_TOWN_CURSOR_POS_SPACE_X * net_id;
    dest_pos->y = NO_TOWN_CURSOR_POS_Y;
    return _TOWN_NO_FREE;
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * ���S�l�Ɣ��a���w�肵��RECT���쐬����
 *
 * @param   x		        ���S�lX
 * @param   y		        ���S�lY
 * @param   half_size		���aX
 * @param   half_size		���aY
 * @param   rect		    �����
 */
//--------------------------------------------------------------
static void _SetRect(int x, int y, int half_size_x, int half_size_y, GFL_RECT *rect)
{
  rect->top = y - half_size_y;
  rect->bottom = y + half_size_y;
  rect->left = x - half_size_x;
  rect->right = x + half_size_x;
}

//--------------------------------------------------------------
/**
 * �w����W����`���Ɏ��܂��Ă��邩�q�b�g�`�F�b�N
 *
 * @param   x		    ���WX
 * @param   y		    ���WY
 * @param   rect		��`
 *
 * @retval  BOOL		TRUE:�q�b�g�B�@FALSE:���q�b�g
 */
//--------------------------------------------------------------
static BOOL _CheckRectHit(int x, int y, const GFL_RECT *rect)
{
  if(rect->left < x && rect->right > x && rect->top < y && rect->bottom > y){
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * �^�b�`����
 *
 * @param   intcomm		
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_TouchUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  u32 x, y;
  int i, my_net_id, net_id;
  GFL_RECT rect;
  GFL_CLACTPOS pos;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(intsub->gsys);
  FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  GFL_CLWK *act;

  if(intsub->wfbc_go == TRUE || GFL_UI_TP_GetPointTrg(&x, &y) == FALSE 
      || FIELD_SUBSCREEN_GetAction( subscreen ) != FIELD_SUBSCREEN_ACTION_NONE){
    return;
  }
  
  //�v���C���[�A�C�R���^�b�`����
  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(net_id != my_net_id && (intcomm->recv_profile & (1 << net_id))){
      act = intsub->act[INTSUB_ACTOR_CUR_S_0 + net_id];
      if(GFL_CLACT_WK_GetDrawEnable(act) == TRUE){
        GFL_CLACT_WK_GetPos( act, &pos, CLSYS_DRAW_SUB );
        if(x - TOUCH_RANGE_PLAYER_ICON_X <= pos.x && x + TOUCH_RANGE_PLAYER_ICON_X >= pos.x
            && y - TOUCH_RANGE_PLAYER_ICON_Y <= pos.y && y + TOUCH_RANGE_PLAYER_ICON_Y >= pos.y){
          if(ZONEDATA_IsWfbc(intcomm->intrude_status[net_id].zone_id) == TRUE){
            intsub->wfbc_go = TRUE;
            intsub->wfbc_seq = 0;
          }
          else{
            Intrude_SetWarpPlayerNetID(game_comm, net_id);
            FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_PLAYER_WARP);
          }
          PMSND_PlaySE( SEQ_SE_FLD_102 ); //��check �T�u�X�N���[���C�x���g���烏�[�v�C�x���g���N������悤�ɂ����炻�����Ɉڂ�
          return;
        }
      }
    }
  }
  
  //�X�^�b�`����
  if(intcomm->intrude_status_mine.palace_area != GAMEDATA_GetIntrudeMyID(gamedata)){
    for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
      _SetRect(PalaceTownData[i].subscreen_x, PalaceTownData[i].subscreen_y, 
        TOWN_ICON_HITRANGE_HALF, TOWN_ICON_HITRANGE_HALF, &rect);
      if(_CheckRectHit(x, y, &rect) == TRUE){
        if(i == PALACE_TOWN_WFBC){
          intsub->wfbc_go = TRUE;
          intsub->wfbc_seq = 0;
        }
        else{
          Intrude_SetWarpTown(game_comm, i);
          FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP);
        }
        PMSND_PlaySE( SEQ_SE_FLD_102 ); //��check �T�u�X�N���[���C�x���g���烏�[�v�C�x���g���N������悤�ɂ����炻�����Ɉڂ�
        return;
      }
    }
  }

  //�p���X���^�b�`����
  _SetRect(PALACE_ICON_POS_X, PALACE_ICON_POS_Y, 
    PALACE_ICON_HITRANGE_HALF, PALACE_ICON_HITRANGE_HALF, &rect);
  if(_CheckRectHit(x, y, &rect) == TRUE){
    Intrude_SetWarpTown(game_comm, PALACE_TOWN_DATA_PALACE);
    FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP);
    PMSND_PlaySE( SEQ_SE_FLD_102 ); //��check �T�u�X�N���[���C�x���g���烏�[�v�C�x���g���N������悤�ɂ����炻�����Ɉڂ�
    return;
  }
  
  ///�Q���{�^���^�b�`����
  if(MISSION_FIELD_CheckStatus(&intcomm->mission) == MISSION_STATUS_NOT_ENTRY){
    _SetRect(ENTRY_BUTTON_POS_X, ENTRY_BUTTON_POS_Y, 
      ENTRY_BUTTON_HITRANGE_HALF_X, ENTRY_BUTTON_HITRANGE_HALF_Y, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      MISSION_SetMissionEntry(intcomm, &intcomm->mission);
      FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_MISSION_ENTRY);
    }
  }
  
#if 0
  //�~�b�V�����A�C�R���^�b�`����
  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    _SetRect(MISSION_ICON_POS_X, MISSION_ICON_POS_Y, 
      POWER_ICON_HITRANGE_HALF_X, POWER_ICON_HITRANGE_HALF_Y, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_MISSION_PUT);
      return;
    }
  }
#endif
}

//--------------------------------------------------------------
/**
 * �ڑ��l�����Ď����ABar�X�N���[���̈�l�p�ƒʐM�p��ύX����
 *
 * @param   intcomm		
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BGBarUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  if(intsub->bar_type == BG_BAR_TYPE_SINGLE){
    if(_IntSub_GetProfileRecvNum(intcomm) > 1){
      GFL_ARC_UTIL_TransVramScreen(
        ARCID_PALACE, NARC_palace_palace_bar2_lz_NSCR, INTRUDE_FRAME_S_BAR, 0, 
        0x800, TRUE, HEAPID_FIELDMAP);
      intsub->bar_type = BG_BAR_TYPE_COMM;
    }
  }
  else{ //BG_BAR_TYPE_COMM
    if(_IntSub_GetProfileRecvNum(intcomm) <= 1){
      GFL_ARC_UTIL_TransVramScreen(
        ARCID_PALACE, NARC_palace_palace_bar_lz_NSCR, INTRUDE_FRAME_S_BAR, 0, 
        0x800, TRUE, HEAPID_FIELDMAP);
      intsub->bar_type = BG_BAR_TYPE_SINGLE;
    }
  }
}

//--------------------------------------------------------------
/**
 * �N���G���A���Ď����ABG�̃J���[��ύX����
 *
 * @param   intcomm		
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BGColorUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  int bg_pal;
  
  bg_pal = intcomm->intrude_status_mine.palace_area;
  if(bg_pal == intsub->now_bg_pal){
    return;
  }
  
  if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    GameCommInfo_MessageEntry_MyPalace(game_comm, intcomm->intrude_status_mine.palace_area);
  }
  else{
    GameCommInfo_MessageEntry_IntrudePalace(game_comm, bg_pal);
  }
  
  GFL_BG_ChangeScreenPalette(
    INTRUDE_FRAME_S_BACKGROUND, _AREA_SCREEN_COLOR_START_X, _AREA_SCREEN_COLOR_START_Y, 
    _AREA_SCREEN_COLOR_SIZE_X, _AREA_SCREEN_COLOR_SIZE_Y, 
    INTSUB_BG_PAL_BASE_START_BACK + bg_pal);
  GFL_BG_ChangeScreenPalette(
    INTRUDE_FRAME_S_BAR, _AREA_SCREEN_COLOR_START_X, _AREA_SCREEN_COLOR_START_Y, 
    _AREA_SCREEN_COLOR_SIZE_X, _AREA_SCREEN_COLOR_SIZE_Y, 
    INTSUB_BG_PAL_BASE_START_TOWN + bg_pal);

  GFL_BMPWIN_MakeScreen(intsub->printutil_title.win);

  GFL_BG_LoadScreenV_Req(INTRUDE_FRAME_S_BACKGROUND);
  GFL_BG_LoadScreenV_Req(INTRUDE_FRAME_S_BAR);
  intsub->now_bg_pal = bg_pal;
}

