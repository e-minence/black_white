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
#include "field/event_intrude_subscreen.h"
#include "field/scrcmd_intrude.h"
#include "intrude_field.h"
#include "system/palanm.h"


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
#define INFOMSG_UPDATE_WAIT   (60)

///�t�H���gBG�p���b�g�W�J�ʒu
#define _FONT_BG_PALNO      (14)

///�����_���[�ݒ�
enum{
  INTSUB_CLACT_REND_SUB,
  INTSUB_CLACT_REND_MAX,
};
static const GFL_REND_SURFACE_INIT INTSUB_CLACT_REND[INTSUB_CLACT_REND_MAX] = {
  {
    0, 0,
    256, 192,
    CLSYS_DRAW_SUB,
    CLSYS_REND_CULLING_TYPE_NONE,
  },
};

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

  INTSUB_ACTOR_TOUCH_TOWN_EFF_0,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_1,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_2,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_3,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_4,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_5,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_6,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_7,
  INTSUB_ACTOR_TOUCH_TOWN_EFF_MAX = INTSUB_ACTOR_TOUCH_TOWN_EFF_7,

  INTSUB_ACTOR_TOUCH_PALACE,
  INTSUB_ACTOR_TOUCH_PALACE_EFF,
  
  INTSUB_ACTOR_AREA_0,
  INTSUB_ACTOR_AREA_1,
  INTSUB_ACTOR_AREA_2,
  INTSUB_ACTOR_AREA_MAX = INTSUB_ACTOR_AREA_2,

  INTSUB_ACTOR_CUR_S_0,
  INTSUB_ACTOR_CUR_S_1,
  INTSUB_ACTOR_CUR_S_2,
  INTSUB_ACTOR_CUR_S_MAX = INTSUB_ACTOR_CUR_S_2,

  INTSUB_ACTOR_CUR_L,       ///<�����̋��ꏊ���w��
  INTSUB_ACTOR_ENTRY,       ///<�u�Q���v�{�^��

  INTSUB_ACTOR_WARP_NG_0,
  INTSUB_ACTOR_WARP_NG_1,
  INTSUB_ACTOR_WARP_NG_2,
  INTSUB_ACTOR_WARP_NG_MAX = INTSUB_ACTOR_WARP_NG_2,
  
//  INTSUB_ACTOR_MARK,        ///<�����̂���G���A���w��

  INTSUB_ACTOR_LV_NUM_KETA_0,
  INTSUB_ACTOR_LV_NUM_KETA_1,
  INTSUB_ACTOR_LV_NUM_KETA_2,
  INTSUB_ACTOR_LV_NUM_KETA_MAX = INTSUB_ACTOR_LV_NUM_KETA_2,

  INTSUB_ACTOR_MAX,
};

///BMPOAM�Ŏg�p����A�N�^�[��
#define INTSUB_ACTOR_BMPOAM_NUM   (16 + 4)

///OBJ�p���b�gINDEX
enum{
  INTSUB_ACTOR_PAL_BASE_START = 1,    ///<�v���C���[���ɕς��p���b�g�J�n�ʒu
  
  INTSUB_ACTOR_PAL_TOUCH_NORMAL = 6,  ///<�^�b�`�o����X�F�m�[�}��
  INTSUB_ACTOR_PAL_TOUCH_DECIDE = 7,  ///<�^�b�`�o����X�F���莞
  INTSUB_ACTOR_PAL_TOUCH_MINE = 8,    ///<�^�b�`�o����X�F����������ꏊ
  
  INTSUB_ACTOR_PAL_MAX = 9,
  
  INTSUB_ACTOR_PAL_MISSION_TARGET_PLAYER = 9, //�~�b�V�����Ώۂ̃v���C���[�p���b�g�A�j��
  
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
  PALACE_ACT_ANMSEQ_TOUCH_TOWN_EFF,
  PALACE_ACT_ANMSEQ_TOUCH_PALACE_EFF,
};

///�A�N�^�[�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_TOUCH_TOWN = 90,
  SOFTPRI_TOUCH_TOWN_EFF = 100,
  SOFTPRI_AREA = 50,
  SOFTPRI_CUR_S = 20,
  SOFTPRI_CUR_L = 19,
  SOFTPRI_WARP_NG = 30,
  SOFTPRI_LV_NUM = 10,
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
  TOUCH_RANGE_PLAYER_ICON_X = 12,    ///<����v���C���[�A�C�R���̃^�b�`�͈�X(���a)
  TOUCH_RANGE_PLAYER_ICON_Y = 12,    ///<����v���C���[�A�C�R���̃^�b�`�͈�Y(���a)
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
#define TOWN_ICON_HITRANGE_HALF   (12)

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

///�Q���{�^���̕\���^�C�v
enum{
  ENTRY_BUTTON_MSG_PATERN_ENTRY,    ///<�u���񂩁v
  ENTRY_BUTTON_MSG_PATERN_BACK,     ///<�u���ǂ�v
  
  ENTRY_BUTTON_MSG_PATERN_MAX,
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///�ʐM���肪�s���]�[���ɂ���ꍇ�̕\�����WX
#define NOTHING_ZONE_SUB_POS_X            (240)
///�ʐM���肪�s���]�[���ɂ���ꍇ�̕\�����WY
#define NOTHING_ZONE_SUB_POS_Y            (64)
///�ʐM���肪�s���]�[���ɂ���ꍇ�̕\�����W�ԊuY
#define NOTHING_ZONE_SUB_POS_Y_SPACE      (24)

//--------------------------------------------------------------
//  �C�x���gNo
//--------------------------------------------------------------
enum{
  _EVENT_REQ_NO_NULL,             ///<�C�x���g���N�G�X�g����
  _EVENT_REQ_NO_TOWN_WARP,        ///<�X�փ��[�v
  _EVENT_REQ_NO_PLAYER_WARP,      ///<�v���C���[�̂��鏊�փ��[�v
  _EVENT_REQ_NO_MISSION_ENTRY,    ///<�~�b�V�����ɎQ��
};

//--------------------------------------------------------------
//  �^�b�`�G�t�F�N�g
//--------------------------------------------------------------
///���莞�̃p���b�g�t���b�V���E�F�C�g
#define TOWN_DECIDE_PAL_FLASH_WAIT    (2)
///���莞�̃p���b�g�t���b�V���^�C�}�[MAX�l
#define TOWN_DECIDE_PAL_FLASH_TIMER_MAX     (TOWN_DECIDE_PAL_FLASH_WAIT * 5)

///�v���C���[������X�̃t�F�[�h���x(����8�r�b�g����)
#define PLAYER_TOWN_FADE_EVY_ADD      (0x100)

///�v���C���[������X�̃p���b�g�A�j���F�J�n�J���[�ʒu
#define PLAYER_PALANM_START_COLOR     (1)
///�v���C���[������X�̃p���b�g�A�j���F�I���J���[�ʒu
#define PLAYER_PALANM_END_COLOR       (4)
///�v���C���[������X�̃p���b�g�A�j���F�J���[��
#define PLAYER_PALANM_COLOR_NUM       (PLAYER_PALANM_END_COLOR - PLAYER_PALANM_START_COLOR + 1)
///�v���C���[������X�̃p���b�g�A�j���F�o�b�t�@�T�C�Y
#define PLAYER_PALANM_BUFFER_SIZE     (PLAYER_PALANM_COLOR_NUM * sizeof(u16))

//--------------------------------------------------------------
//  [TIME]�X�N���[��
//--------------------------------------------------------------
///[TIME]��������Ă���X�N���[���ʒu
#define BG_TIME_SCRN_POS_X      (0x14)
#define BG_TIME_SCRN_POS_Y      (0x16)
#define BG_TIME_SCRN_SIZE_X     (12)
#define BG_TIME_SCRN_SIZE_Y     (2)
///[TIME]��������Ă���X�N���[�����N���A���鎞�̃X�N���[���R�[�h
#define BG_TIME_SCRN_CLEAR_CODE   (0x3001)

//--------------------------------------------------------------
//  �~�b�V�����t�H�[�J�X
//--------------------------------------------------------------
///�~�b�V�����^�[�Q�b�g�ɂȂ��Ă��ăt�H�[�J�X�����v���C���[�A�C�R����EVY�l
#define MISSION_FOCUS_PLAYER_EVY      (8)
///�~�b�V�����^�[�Q�b�g�ɂȂ��Ă��ăt�H�[�J�X�����v���C���[�A�C�R���̃J���[
#define MISSION_FOCUS_PLAYER_COLOR    (0x7fff)
///�~�b�V�����^�[�Q�b�g�ɂȂ��Ă��ăt�H�[�J�X�����v���C���[�A�C�R���̃A�j�����x
#define MISSION_FOCUS_PLAYER_ANMWAIT  (15)


//==============================================================================
//  �\���̒�`
//==============================================================================
///intcomm����擾�o����p�����[�^��(��ʐM�̎��̓\���p�̒l���Z�b�g�����)
typedef struct{
  u8 now_palace_area;       ///<���ݎ���������p���X�G���A
  u8 recv_profile;          ///<��M�����v���t�B�[��(bit�Ǘ�)
  u8 recv_num;              ///<��M�����v���t�B�[���l��
  u8 palace_in;             ///<TRUE:�p���X���ɂ��Ƃ��ꂽ
  MISSION_STATUS m_status;  ///<�~�b�V������
  s32 m_timer;              ///<�~�b�V�����^�C�}�[
  const MISSION_DATA *p_md; ///<��M���Ă���~�b�V�����f�[�^�ւ̃|�C���^
}INTRUDE_COMM_PARAM;

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
	GFL_MSGDATA *msgdata_mission;
	GFL_TCB *vintr_tcb;               ///<VBlankTCB�ւ̃|�C���^
	
	STRBUF *strbuf_temp;
	STRBUF *strbuf_info;
	STRBUF *strbuf_title;
	
	PRINT_UTIL printutil_title;  ///<�^�C�g�����b�Z�[�W
	
	GFL_CLUNIT *clunit;
	GFL_CLSYS_REND *clrend;
  GFL_CLWK *act[INTSUB_ACTOR_MAX];

  BMPOAM_SYS_PTR bmpoam_sys;
  GFL_BMP_DATA *entrymsg_bmp[ENTRY_BUTTON_MSG_PATERN_MAX];
  GFL_BMP_DATA *backmsg_bmp;
  GFL_BMP_DATA *infomsg_bmp;
  BMPOAM_ACT_PTR entrymsg_bmpoam[ENTRY_BUTTON_MSG_PATERN_MAX];
  BMPOAM_ACT_PTR infomsg_bmpoam;
  
  INTRUDE_COMM_PARAM comm;  ///<intcomm����擾�o����p�����[�^��(��ʐM�̎��̓\���p�̒l)
  
  u8 my_net_id;
  u8 town_update_req;     ///<�X�A�C�R���X�V���N�G�X�g(TOWN_UPDATE_REQ_???)
  u8 now_bg_pal;          ///<���݂�BG�̃p���b�g�ԍ�
  u8 wfbc_go;             ///<TRUE:WFBC�ւ̃��[�v��������
  
  s16 infomsg_wait;       ///<�C���t�H���[�V�������b�Z�[�W�X�V�E�F�C�g
  u8 wfbc_seq;
  u8 bar_type;            ///<BG_BAR_TYPE_xxx
  
  u8 title_print_type;    ///<_TITLE_PRINT_xxx
  u8 infomsg_trans_req;
  u8 print_mission_status;  ///<���ݕ\�����Ă���~�b�V������
  u8 print_mission_exe_flag;  ///<�~�b�V�������s�󋵂̃��b�Z�[�W�U�蕪���t���O
  
  u16 warp_zone_id;
  u8 event_req;           ///< _EVENT_REQ_NO_xxx
  u8 decide_town_tblno;      ///<�^�b�`�Ō��肵���X�̃e�[�u���ԍ�
  
  u8 decide_pal_occ;
  u8 decide_pal_timer;     ///<�^�b�`�������̃p���b�g�A�j���p�^�C�}�[
  s16 player_pal_evy;     ///<
  s8 player_pal_dir;      ///<
  u8 player_pal_tblno;    ///<���������鏊�̊X�̃e�[�u���ԍ�
  u8 player_pal_trans_req;  ///<TRUE:�p���b�g�]�����N�G�X�g
  u8 vblank_trans;

  u16 player_pal_src[PLAYER_PALANM_COLOR_NUM];        ///<�ω���
  u16 player_pal_next_src[PLAYER_PALANM_COLOR_NUM];   ///<�ω���
  u16 player_pal_buffer[PLAYER_PALANM_COLOR_NUM];     ///<�]���o�b�t�@

  u16 scrnbuf_time[BG_TIME_SCRN_SIZE_X * BG_TIME_SCRN_SIZE_Y];  ///< [TIME]��������Ă���X�N���[���f�[�^��ޔ����郏�[�N

  u8 back_exit;           ///< TRUE:�u���ǂ�v�{�^���������ĉ���ʏI�����[�h�ɂȂ��Ă���
  u8 print_touch_player;  ///< �ʐM����̃A�C�R�����^�b�`�����ꍇ�A���̐l����NetID
  u8 mission_target_focus_netid;    ///<�~�b�V�����^�[�Q�b�g�Ńt�H�[�J�X�Ώۂ̃v���C���[NetID
  u8 mission_target_focus_wait;     ///<�~�b�V�����^�[�Q�b�g�t�H�[�J�X�A�j���E�F�C�g
  u8 padding[2];
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
static void _IntSub_ActorResourceLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorResourceUnload(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorDelete(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate_TouchTown(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_Area(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorS(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorL(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_WarpNG(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_LvNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_EntryButton(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_Delete_EntryButton(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorUpdate_TouchTown(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_Area(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_CursorS(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_CursorS_MissionFocus(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorUpdate_CursorL(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, ZONEID my_zone_id);
static void _IntSub_ActorUpdate_EntryButton(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_LvNum(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy);
static OCCUPY_INFO * _IntSub_GetArreaOccupy(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_TitleMsgUpdate(INTRUDE_SUBDISP_PTR intsub, ZONEID my_zone_id);
static void _IntSub_InfoMsgUpdate(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm);
static void _SetRect(int x, int y, int half_size_x, int half_size_y, GFL_RECT *rect);
static BOOL _CheckRectHit(int x, int y, const GFL_RECT *rect);
static void _IntSub_TouchUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGBarUpdate(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGColorUpdate(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_CommParamInit(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm);
static void _IntSub_CommParamUpdate(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm);
static void _SetPalFlash_DecideTown(INTRUDE_SUBDISP_PTR intsub);
static BOOL _CheckPalFlash_DecideTown(INTRUDE_SUBDISP_PTR intsub);
static void _VblankFunc(GFL_TCB *tcb, void *work);
static void _SetPalFade_PlayerTown(INTRUDE_SUBDISP_PTR intsub, int town_tblno);
static BOOL _TimeNum_CheckEnable(INTRUDE_SUBDISP_PTR intsub);
static void _TimeScrn_Clear(void);
static void _TimeScrn_Recover(INTRUDE_SUBDISP_PTR intsub);


//==============================================================================
//  �f�[�^
//==============================================================================
///�p���X�A�C�R�����W
enum{
  PALACE_ICON_POS_X = 128,        ///<�p���X�A�C�R�����WX
  PALACE_ICON_POS_Y = 0xb*8+4,    ///<�p���X�A�C�R�����WY
  PALACE_ICON_HITRANGE_HALF = 16, ///<�p���X�A�C�R���̃^�b�`���蔼�a
  
  PALACE_CURSOR_POS_X = PALACE_ICON_POS_X,      ///<�p���X�A�C�R�����J�[�\�����w���ꍇ�̍��WX
  PALACE_CURSOR_POS_Y = PALACE_ICON_POS_Y - 16, ///<�p���X�A�C�R�����J�[�\�����w���ꍇ�̍��WY
};

///�G���A�A�C�R���z�u���W
enum{
  AREA_POST_LEFT = 12 * 8,                           ///<�z�u���W�͈͂̍��[X
  AREA_POST_RIGHT = 0x14 * 8,                         ///<�z�u���W�͈͂̉E�[X
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

///�e�ʐM�v���C���[���ɂ��炷�l
static const s8 WearOffset[FIELD_COMM_MEMBER_MAX][2] = {
  {-4, -4}, {4, -4}, {0, 4},
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
  intsub->player_pal_tblno = PALACE_TOWN_DATA_NULL;
  intsub->print_touch_player = INTRUDE_NETID_NULL;
  intsub->mission_target_focus_netid = INTRUDE_NETID_NULL;
  
  _IntSub_CommParamInit(intsub, Intrude_Check_CommConnect(game_comm));
  
  handle = GFL_ARC_OpenDataHandle(ARCID_PALACE, HEAPID_FIELDMAP);
  
  _IntSub_SystemSetup(intsub);
  _IntSub_BGLoad(intsub, handle);
  _IntSub_BmpWinAdd(intsub);
  _IntSub_ActorResourceLoad(intsub, handle);
  _IntSub_ActorCreate(intsub, handle);
  _IntSub_BmpOamCreate(intsub, handle);
  
  GFL_ARC_CloseDataHandle(handle);

  //OBJWINDOW(�ʐM�A�C�R��) �̒�����Blend�ŋP�x�������Ȃ��悤�ɂ���
  GFL_NET_WirelessIconOBJWinON();
  G2S_SetWndOBJInsidePlane(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, FALSE);
  G2S_SetWndOutsidePlane(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, TRUE);
	GXS_SetVisibleWnd(GX_WNDMASK_OW);

	//V�u�����NTCB�o�^
	intsub->vintr_tcb = GFUser_VIntr_CreateTCB(_VblankFunc, intsub, 10);

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
	GFL_TCB_DeleteTask(intsub->vintr_tcb);

  _IntSub_BmpOamDelete(intsub);
  _IntSub_Delete_EntryButton(intsub);
  _IntSub_ActorDelete(intsub);
  _IntSub_ActorResourceUnload(intsub);
  _IntSub_BmpWinDel(intsub);
  _IntSub_BGUnload(intsub);
  _IntSub_SystemExit(intsub);

  GFL_HEAP_FreeMemory(intsub);

  //OBJWINDOW(�ʐM�A�C�R��)��OFF
  GFL_NET_WirelessIconOBJWinOFF();
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
}

//==================================================================
/**
 * �N������ʁF�X�V
 *
 * @param   intsub		
 */
//==================================================================
void INTRUDE_SUBDISP_Update(INTRUDE_SUBDISP_PTR intsub, BOOL bActive)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int i;

  intsub->my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  _IntSub_CommParamUpdate(intsub, intcomm);
  
	GFL_TCBL_Main( intsub->tcbl_sys );
	PRINTSYS_QUE_Main( intsub->print_que );
  PRINT_UTIL_Trans( &intsub->printutil_title, intsub->print_que );
  if(intsub->infomsg_trans_req == TRUE 
      && PRINTSYS_QUE_IsExistTarget(intsub->print_que, intsub->infomsg_bmp) == FALSE){
    BmpOam_ActorBmpTrans(intsub->infomsg_bmpoam);
    intsub->infomsg_trans_req = FALSE;
  }
  for(i = 0; i < ENTRY_BUTTON_MSG_PATERN_MAX; i++){
    if(PRINTSYS_QUE_IsExistTarget(intsub->print_que, intsub->entrymsg_bmp[i]) == FALSE){
      BmpOam_ActorBmpTrans(intsub->entrymsg_bmpoam[i]);
    }
  }
  
  if(bActive == FALSE){
    if(_CheckPalFlash_DecideTown(intsub) == TRUE){  //����A�j�����Ă��鎞�͈Â����Ȃ�
    	G2S_BlendNone();
    }
    else{
      G2S_SetBlendBrightness(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
        GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ |
        GX_BLEND_PLANEMASK_BD, -FIELD_NONE_ACTIVE_EVY);
      return;
    }
  }
  else{
  	G2S_BlendNone();
  }

  if(bActive == TRUE){
    //�^�b�`����`�F�b�N
    _IntSub_TouchUpdate(intcomm, intsub);
  }
  
  //WFBC�ւ̃��[�v�`�F�b�N
  if(intcomm != NULL){
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

            intsub->warp_zone_id = ZONE_ID_PLC10;
            intsub->event_req = _EVENT_REQ_NO_TOWN_WARP;
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
}

//==================================================================
/**
 * �N������ʁF�`��
 *
 * @param   intsub		
 */
//==================================================================
void INTRUDE_SUBDISP_Draw(INTRUDE_SUBDISP_PTR intsub, BOOL bActive)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  PLAYER_WORK *player_work = GAMESYSTEM_GetMyPlayerWork(intsub->gsys);
  ZONEID my_zone_id = PLAYERWORK_getZoneID(player_work);
  OCCUPY_INFO *area_occupy;
  
  area_occupy = _IntSub_GetArreaOccupy(intsub);

  _IntSub_BGBarUpdate(intsub);
  //BG�X�N���[���J���[�ύX�`�F�b�N
  _IntSub_BGColorUpdate(intsub);
  
  //�C���t�H���[�V�������b�Z�[�W
  _IntSub_TitleMsgUpdate(intsub, my_zone_id);
  _IntSub_InfoMsgUpdate(intsub, intcomm);
  
  //�A�N�^�[�X�V
  _IntSub_ActorUpdate_TouchTown(intsub, area_occupy);
  _IntSub_ActorUpdate_Area(intsub, area_occupy);
  _IntSub_ActorUpdate_CursorS(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_CursorL(intsub, area_occupy, my_zone_id);
  _IntSub_ActorUpdate_EntryButton(intsub, area_occupy);
  _IntSub_ActorUpdate_LvNum(intsub, area_occupy);
}

//==================================================================
/**
 * �N������ʁF�C�x���g�N���`�F�b�N
 *
 * @param   intsub		
 * @param   bEvReqOK		TRUE:�C�x���g�N�����Ă��悢�@FALSE�F���̃C�x���g���N����
 *
 * @retval  GMEVENT*		
 */
//==================================================================
GMEVENT* INTRUDE_SUBDISP_EventCheck(INTRUDE_SUBDISP_PTR intsub, BOOL bEvReqOK, FIELD_SUBSCREEN_WORK* subscreen )
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(intsub->gsys);
  GMEVENT *event = NULL;
  
  if(bEvReqOK == FALSE || fieldWork == NULL){
    return NULL;
  }
  
  if(intsub->back_exit == TRUE){
    FIELD_SUBSCREEN_ChangeForce( subscreen, FIELD_SUBSCREEN_NORMAL );
    return NULL;
  }

  switch(intsub->event_req){
  case _EVENT_REQ_NO_TOWN_WARP:
    PMSND_PlaySE( SEQ_SE_FLD_102 );
    _SetPalFlash_DecideTown(intsub);
    event = EVENT_IntrudeTownWarp(intsub->gsys, fieldWork, intsub->warp_zone_id);
    break;
  case _EVENT_REQ_NO_PLAYER_WARP:
    PMSND_PlaySE( SEQ_SE_FLD_102 );
    event = EVENT_IntrudePlayerWarp(intsub->gsys, fieldWork, Intrude_GetWarpPlayerNetID(game_comm));
    break;
  case _EVENT_REQ_NO_MISSION_ENTRY:
    event = EVENT_Intrude_MissionStartWait_Warp(intsub->gsys);
    break;
  }
  if(event != NULL){
    intsub->event_req = _EVENT_REQ_NO_NULL;
    return event;
  }
  
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief   V�u�����NTCB
 *
 * @param   tcb			
 * @param   work		
 */
//--------------------------------------------------------------
static void _VblankFunc(GFL_TCB *tcb, void *work)
{
  INTRUDE_SUBDISP_PTR intsub = work;
  
  intsub->vblank_trans ^= 1;
  if(intsub->vblank_trans){ //�t�B�[���h��1/30�ɍ��킹��悤�ɓ]����2���1��
    if(intsub->player_pal_trans_req){
      DC_FlushRange( (void*)intsub->player_pal_buffer, PLAYER_PALANM_BUFFER_SIZE );
      GXS_LoadOBJPltt((const void *)intsub->player_pal_buffer, 
        INTSUB_ACTOR_PAL_TOUCH_MINE * 0x20 + PLAYER_PALANM_START_COLOR * 2, 
        PLAYER_PALANM_BUFFER_SIZE );
      intsub->player_pal_trans_req = FALSE;
    }
  }
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
  intsub->clrend = GFL_CLACT_USERREND_Create( INTSUB_CLACT_REND, INTSUB_CLACT_REND_MAX, HEAPID_FIELD_SUBSCREEN );
  GFL_CLACT_UNIT_SetUserRend( intsub->clunit, intsub->clrend );

  intsub->bmpoam_sys = BmpOam_Init(HEAPID_FIELD_SUBSCREEN, intsub->clunit);

	intsub->tcbl_sys = GFL_TCBL_Init(HEAPID_FIELD_SUBSCREEN, HEAPID_FIELD_SUBSCREEN, 4, 32);
	intsub->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_FIELD_SUBSCREEN );
	intsub->wordset = WORDSET_Create(HEAPID_FIELD_SUBSCREEN);
  intsub->print_que = PRINTSYS_QUE_Create( HEAPID_FIELD_SUBSCREEN );
  intsub->msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
    NARC_message_invasion_dat, HEAPID_FIELD_SUBSCREEN );
  intsub->msgdata_mission = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
    NARC_message_mission_dat, HEAPID_FIELD_SUBSCREEN );
  
  intsub->strbuf_temp = GFL_STR_CreateBuffer(128, HEAPID_FIELD_SUBSCREEN);
  intsub->strbuf_info = GFL_STR_CreateBuffer(128, HEAPID_FIELD_SUBSCREEN);
  intsub->strbuf_title = GFL_STR_CreateBuffer(128, HEAPID_FIELD_SUBSCREEN);
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
  GFL_MSG_Delete(intsub->msgdata_mission);
  PRINTSYS_QUE_Clear(intsub->print_que);
  PRINTSYS_QUE_Delete(intsub->print_que);
  WORDSET_Delete(intsub->wordset);
	GFL_FONT_Delete(intsub->font_handle);
	GFL_TCBL_Exit(intsub->tcbl_sys);

  BmpOam_Exit(intsub->bmpoam_sys);
  GFL_CLACT_USERREND_Delete( intsub->clrend );
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
  //[TIME]�̃X�N���[�����o�b�t�@�ɓǂݍ���
  {
    NNSG2dScreenData *scrnData;
    void *load_data;
    load_data = GFL_ARCHDL_UTIL_LoadScreen(
      handle, NARC_palace_palace_time_lz_NSCR, TRUE, &scrnData, HEAPID_FIELDMAP);
    GFL_STD_MemCopy16(scrnData->rawData, intsub->scrnbuf_time, 
      BG_TIME_SCRN_SIZE_X * BG_TIME_SCRN_SIZE_Y * sizeof(u16));
    GFL_HEAP_FreeMemory(load_data);
  }
  if(_TimeNum_CheckEnable(intsub) == TRUE){
    _TimeScrn_Recover(intsub);
  }
  
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
    INTSUB_CLACT_REND_SUB,
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
static void _IntSub_ActorResourceLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
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

  //VRAM����p���b�g�A�j���Ώۂ̃f�[�^���o�b�t�@�փR�s�[
  GFL_STD_MemCopy16((void*)(HW_DB_OBJ_PLTT + INTSUB_ACTOR_PAL_TOUCH_DECIDE * 0x20 + PLAYER_PALANM_START_COLOR * 2), intsub->player_pal_src, PLAYER_PALANM_BUFFER_SIZE);
  GFL_STD_MemCopy16(intsub->player_pal_src, intsub->player_pal_buffer, PLAYER_PALANM_BUFFER_SIZE);
  GFL_STD_MemCopy16((void*)(HW_DB_OBJ_PLTT + INTSUB_ACTOR_PAL_TOUCH_MINE * 0x20 + PLAYER_PALANM_START_COLOR * 2), intsub->player_pal_next_src, PLAYER_PALANM_BUFFER_SIZE);
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
  _IntSub_ActorCreate_WarpNG(intsub, handle);
  _IntSub_ActorCreate_LvNum(intsub, handle);
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
  int i, effno;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_TOUCH_TOWN,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_TOUCH_TOWN,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  GFL_CLWK_DATA eff_head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_TOUCH_TOWN_EFF,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_TOUCH_TOWN_EFF,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  GF_ASSERT(PALACE_TOWN_DATA_MAX == (INTSUB_ACTOR_TOUCH_TOWN_MAX - INTSUB_ACTOR_TOUCH_TOWN_0 + 1));
  effno = INTSUB_ACTOR_TOUCH_TOWN_EFF_0;
  for(i = INTSUB_ACTOR_TOUCH_TOWN_0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX; i++){
    head.pos_x = PalaceTownData[i - INTSUB_ACTOR_TOUCH_TOWN_0].subscreen_x;
    head.pos_y = PalaceTownData[i - INTSUB_ACTOR_TOUCH_TOWN_0].subscreen_y;

    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF

    eff_head.pos_x = head.pos_x;
    eff_head.pos_y = head.pos_y;
    intsub->act[effno] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &eff_head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[effno], TRUE);  //�I�[�g�A�j��ON
    GFL_CLACT_WK_SetDrawEnable(intsub->act[effno], FALSE);  //�\��OFF
    effno++;
  }
  
  //�^�b�`�p���X��
  head.anmseq = PALACE_ACT_ANMSEQ_TOUCH_PALACE;
  head.pos_x = PALACE_ICON_POS_X;
  head.pos_y = PALACE_ICON_POS_Y;
  intsub->act[INTSUB_ACTOR_TOUCH_PALACE] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], FALSE);  //�\��OFF
  //�^�b�`�p���X���G�t�F�N�g
  eff_head.anmseq = PALACE_ACT_ANMSEQ_TOUCH_PALACE_EFF;
  eff_head.pos_x = PALACE_ICON_POS_X;
  eff_head.pos_y = PALACE_ICON_POS_Y;
  intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &eff_head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], TRUE);  //�I�[�g�A�j��ON
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], FALSE);  //�\��OFF
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
      &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
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
      &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
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
    &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_CUR_L], 
    INTSUB_ACTOR_PAL_BASE_START + intsub->my_net_id, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_CUR_L], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_CUR_L], TRUE);  //�I�[�g�A�j��ON
}

//--------------------------------------------------------------
/**
 * �ʐM����̃��[�vNG�������A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_WarpNG(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_WARP_NG,    //�A�j���[�V�����V�[�P���X
  	SOFTPRI_WARP_NG,              //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  for(i = INTSUB_ACTOR_WARP_NG_0; i <= INTSUB_ACTOR_WARP_NG_MAX; i++){
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetPlttOffs(intsub->act[i], 
      INTSUB_ACTOR_PAL_BASE_START + i-INTSUB_ACTOR_WARP_NG_0, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
  }
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
  BOOL time_enable = FALSE;
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

  time_enable = _TimeNum_CheckEnable(intsub);
  for(i = INTSUB_ACTOR_LV_NUM_KETA_0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX; i++){
    head.pos_x = LvNumPos[i - INTSUB_ACTOR_LV_NUM_KETA_0].x;
    head.pos_y = LvNumPos[i - INTSUB_ACTOR_LV_NUM_KETA_0].y;
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], time_enable);
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
  int i;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  
  intsub->act[INTSUB_ACTOR_ENTRY] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, INTSUB_CLACT_REND_SUB, HEAPID_FIELD_SUBSCREEN);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], FALSE);  //�\��OFF

  for(i = 0; i < ENTRY_BUTTON_MSG_PATERN_MAX; i++){//�u���񂩁vBMPOAM
    BMPOAM_ACT_DATA bmpoam_head = {
      NULL,   //OAM�Ƃ��ĕ\��������BMP�f�[�^�ւ̃|�C���^
      ENTRY_BUTTON_POS_X - 16, ENTRY_BUTTON_POS_Y - 8,   //X, Y
      0,      //�K�p����p���b�g��index(GFL_CLGRP_PLTT_Register�̖߂�l)
      0,      //pal_offset(pltt_index�̃p���b�g���ł̃I�t�Z�b�g)
      SOFTPRI_ENTRY_BUTTON_MSG,
      BGPRI_ACTOR_COMMON,
      INTSUB_CLACT_REND_SUB,
      CLSYS_DRAW_SUB,
    };
    STRBUF *entry_str;
    
    intsub->entrymsg_bmp[i] = GFL_BMP_Create( 
      ENTRYMSG_BMP_SIZE_X, ENTRYMSG_BMP_SIZE_Y, GFL_BMP_16_COLOR, HEAPID_FIELD_SUBSCREEN );
    
    bmpoam_head.bmp = intsub->entrymsg_bmp[i];
    bmpoam_head.pltt_index = intsub->index_pltt_font;
    intsub->entrymsg_bmpoam[i] = BmpOam_ActorAdd(intsub->bmpoam_sys, &bmpoam_head);
    BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam[i], FALSE);

    entry_str = GFL_MSG_CreateString( intsub->msgdata, msg_invasion_subdisp_000 + i );
    PRINTSYS_PrintQueColor( intsub->print_que, intsub->entrymsg_bmp[i], 0, 0, entry_str, 
      intsub->font_handle, PRINTSYS_MACRO_LSB(15,2,0) );
    GFL_STR_DeleteBuffer(entry_str);
  }

  if(intsub->back_exit == FALSE && GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE){
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], TRUE);
    BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam[ENTRY_BUTTON_MSG_PATERN_BACK], TRUE);
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
  int i;
  
  //�A�N�^�[�͂܂Ƃ߂č폜�����̂ŁA����ȊO��BMPOAM�����폜
  for(i = 0; i < ENTRY_BUTTON_MSG_PATERN_MAX; i++){
    BmpOam_ActorDel(intsub->entrymsg_bmpoam[i]);
    GFL_BMP_Delete(intsub->entrymsg_bmp[i]);
  }
}

//==============================================================================
//  �X�V
//==============================================================================
//--------------------------------------------------------------
/**
 * �X�V�����F�^�b�`�X�A�N�^�[
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_TouchTown(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i;
  
  if(intsub->comm.now_palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    BOOL palace_enable;
    //�����̃G���A�ׁ̈A�p���X�������^�b�`�o���Ȃ�
    for(i = 0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX - INTSUB_ACTOR_TOUCH_TOWN_0; i++){
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + i], FALSE);
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_EFF_0 + i], FALSE);
    }
    //�����̃G���A�̏ꍇ�̓p���X�͕\�t�B�[���h�ɂ��Ȃ��ƃ^�b�`�ł��Ȃ�
    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE){
      palace_enable = TRUE;
    }
    else{
      palace_enable = FALSE;
    }
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], palace_enable);
    
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], 
      INTSUB_ACTOR_PAL_BASE_START + intsub->comm.now_palace_area, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], palace_enable);
  }
  else{
    for(i = 0; i <= INTSUB_ACTOR_TOUCH_TOWN_MAX - INTSUB_ACTOR_TOUCH_TOWN_0; i++){
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + i], TRUE);
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_EFF_0 + i], TRUE);
      GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_EFF_0 + i], 
        INTSUB_ACTOR_PAL_BASE_START + intsub->comm.now_palace_area, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], TRUE);
    
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], 
      INTSUB_ACTOR_PAL_BASE_START + intsub->comm.now_palace_area, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_TOUCH_PALACE_EFF], TRUE);
  }
  
  //���肵���X�����炷
  if(intsub->decide_pal_occ == TRUE){
    if(intsub->decide_pal_timer % TOWN_DECIDE_PAL_FLASH_WAIT == 0){
      int pal_offset;
      if((intsub->decide_pal_timer / TOWN_DECIDE_PAL_FLASH_WAIT) & 1){
        pal_offset = INTSUB_ACTOR_PAL_TOUCH_NORMAL;
      }
      else{
        pal_offset = INTSUB_ACTOR_PAL_TOUCH_DECIDE;
      }
      if(intsub->decide_town_tblno != PALACE_TOWN_DATA_PALACE){
        GFL_CLACT_WK_SetPlttOffs(
          intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + intsub->decide_town_tblno], 
          pal_offset, CLWK_PLTTOFFS_MODE_PLTT_TOP);
      }
      else{
        GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], 
          pal_offset, CLWK_PLTTOFFS_MODE_PLTT_TOP);
      }
    }
    if(intsub->decide_pal_timer < TOWN_DECIDE_PAL_FLASH_TIMER_MAX){
      intsub->decide_pal_timer++;
    }
  }
  //����������X�����炷
  else if(intsub->player_pal_tblno != PALACE_TOWN_DATA_NULL){
    int color_offset;
    if(intsub->player_pal_dir == DIR_UP){
      intsub->player_pal_evy += PLAYER_TOWN_FADE_EVY_ADD;
      if(intsub->player_pal_evy >= (17<<8)){
        intsub->player_pal_evy = (16<<8) - (intsub->player_pal_evy - (16<<8));
        intsub->player_pal_dir = DIR_DOWN;
      }
    }
    else{
      intsub->player_pal_evy -= PLAYER_TOWN_FADE_EVY_ADD;
      if(intsub->player_pal_evy < 0){
        intsub->player_pal_evy = (1<<8) - intsub->player_pal_evy;
        intsub->player_pal_dir = DIR_UP;
      }
    }
    for(color_offset = 0; color_offset < PLAYER_PALANM_COLOR_NUM; color_offset++){
      SoftFade(&intsub->player_pal_src[color_offset], &intsub->player_pal_buffer[color_offset], 1, 
        intsub->player_pal_evy >> 8, intsub->player_pal_next_src[color_offset]);
    }
    intsub->player_pal_trans_req = TRUE;
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�G���A�A�C�R��
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_Area(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i, profile_num, net_id;
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  int my_area, now_tbl_pos = 0;
  u8 pal_tbl[FIELD_COMM_MEMBER_MAX] = {0, 0, 0};
  s32 tbl_count = 0, actno;
  static const pos_tbl[2] = {AREA_POST_LEFT, AREA_POST_RIGHT};
  
  profile_num = intsub->comm.recv_num;
  my_area = intsub->comm.now_palace_area;
  pos.y = AREA_POST_Y;
  
  //�v���t�B�[����M�ς݂̃��[�U�[�̃p���b�g�I�t�Z�b�g�e�[�u�������
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intsub->comm.recv_profile & (1 << net_id)){
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
    GF_ASSERT(actno < NELEMS(pos_tbl));
    pos.x = pos_tbl[actno];
    act = intsub->act[INTSUB_ACTOR_AREA_0 + actno];
    GFL_CLACT_WK_SetPos(act, &pos, INTSUB_CLACT_REND_SUB);
    GFL_CLACT_WK_SetPlttOffs(act, 
      INTSUB_ACTOR_PAL_BASE_START + pal_tbl[i], CLWK_PLTTOFFS_MODE_PLTT_TOP);
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
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
  int my_area, net_id;
  GFL_CLWK *act, *act_hate;
  GFL_CLACTPOS pos, pos_hate;
  INTRUDE_STATUS *ist;
  
  my_area = intsub->comm.now_palace_area;

  //�~�b�V�����^�[�Q�b�g�ɂ���Ă���v���C���[�̃t�H�[�J�X����
  _IntSub_CursorS_MissionFocus(intsub);

  //���W�ړ���\���E��\���ݒ�
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    act = intsub->act[INTSUB_ACTOR_CUR_S_0 + net_id];
    act_hate = intsub->act[INTSUB_ACTOR_WARP_NG_0 + net_id];
    if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
      GFL_CLACT_WK_SetDrawEnable(act_hate, FALSE);
      continue;
    }
    else if(intcomm == NULL){
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
      GFL_CLACT_WK_SetDrawEnable(act_hate, FALSE);
      continue;
    }
    else if(intsub->comm.recv_profile & (1 << net_id)){
      ist = &intcomm->intrude_status[net_id];
    }
    else{
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
      GFL_CLACT_WK_SetDrawEnable(act_hate, FALSE);
      continue;
    }
    
    if(ist->palace_area == my_area){  //���̃v���C���[������X���w��
      if(ZONEDATA_IsPalace(ist->zone_id) == TRUE){
        pos.x = PALACE_CURSOR_POS_X + WearOffset[net_id][0];
        pos.y = PALACE_CURSOR_POS_Y + WearOffset[net_id][1];
      }
      else{
        const PALACE_ZONE_SETTING *zonesetting = IntrudeField_GetZoneSettingData(ist->zone_id);
        if(zonesetting != NULL){
          pos.x = zonesetting->sub_x + WearOffset[net_id][0];
          pos.y = zonesetting->sub_y + WearOffset[net_id][1];
          GFL_CLACT_WK_SetDrawEnable(act_hate, FALSE);
        }
        else{ //�s���]�[��
          pos.x = NOTHING_ZONE_SUB_POS_X - 8;
          pos.y = NOTHING_ZONE_SUB_POS_Y + NOTHING_ZONE_SUB_POS_Y_SPACE * net_id;;
          pos_hate = pos;
          pos_hate.x = NOTHING_ZONE_SUB_POS_X;
          GFL_CLACT_WK_SetPos(act_hate, &pos_hate, INTSUB_CLACT_REND_SUB);
          GFL_CLACT_WK_SetDrawEnable(act_hate, TRUE);
        }
      }
      GFL_CLACT_WK_SetPos(act, &pos, INTSUB_CLACT_REND_SUB);
    }
    else{ //���̃v���C���[������p���X�G���A���w��
      int s;
      GFL_CLACT_WK_SetDrawEnable(act_hate, FALSE);
      for(s = 0; s < FIELD_COMM_MEMBER_MAX; s++){
        if(GFL_CLACT_WK_GetPlttOffs(intsub->act[INTSUB_ACTOR_AREA_0 + s]) == INTSUB_ACTOR_PAL_BASE_START + net_id){
          GFL_CLACT_WK_GetPos( intsub->act[INTSUB_ACTOR_AREA_0 + s], &pos, INTSUB_CLACT_REND_SUB );
          break;
        }
      }
      if(s == FIELD_COMM_MEMBER_MAX){
        GFL_CLACT_WK_SetDrawEnable(act, FALSE);
        continue;
      }
      pos.x += WearOffset[net_id][0];
      pos.y += WearOffset[net_id][1];
      GFL_CLACT_WK_SetPos(act, &pos, INTSUB_CLACT_REND_SUB);
    }
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
  }
}

//--------------------------------------------------------------
/**
 * �~�b�V�����^�[�Q�b�g�ɂ���Ă���v���C���[�A�C�R���̃t�H�[�J�X����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_CursorS_MissionFocus(INTRUDE_SUBDISP_PTR intsub)
{
  if(intsub->comm.m_status == MISSION_STATUS_EXE && intsub->comm.p_md != NULL){
    int target_netid = intsub->comm.p_md->target_info.net_id;
    int pal_offset = -1;
    
    if(intsub->mission_target_focus_netid != INTRUDE_NETID_NULL
        && intsub->mission_target_focus_netid != target_netid){
      //�ʂ̃v���C���[���t�H�[�J�X����Ă����ׁA���̃v���C���[�͌��ɖ߂� ����{�I�ɂ��肦�Ȃ�
      GFL_CLACT_WK_SetPlttOffs(
        intsub->act[INTSUB_ACTOR_CUR_S_0 + intsub->mission_target_focus_netid], 
        INTSUB_ACTOR_PAL_BASE_START + intsub->mission_target_focus_netid, 
        CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
    if(intsub->mission_target_focus_netid != target_netid){
      //���Z�b�g�ׁ̈A�p���b�g���p�̈�փR�s�[
      u16 palbuf[16];
      
      GFL_STD_MemCopy16(
        (void*)(HW_DB_OBJ_PLTT + (INTSUB_ACTOR_PAL_BASE_START + target_netid) * 0x20), 
        palbuf, 0x20);
      SoftFade(palbuf, palbuf, 16, MISSION_FOCUS_PLAYER_EVY, MISSION_FOCUS_PLAYER_COLOR);
      GFL_STD_MemCopy16(
        palbuf, (void*)(HW_DB_OBJ_PLTT + INTSUB_ACTOR_PAL_MISSION_TARGET_PLAYER * 0x20), 0x20);
      
      intsub->mission_target_focus_netid = target_netid;
    }
    
    intsub->mission_target_focus_wait++;
    if(intsub->mission_target_focus_wait == MISSION_FOCUS_PLAYER_ANMWAIT){
      pal_offset = INTSUB_ACTOR_PAL_MISSION_TARGET_PLAYER;
    }
    else if(intsub->mission_target_focus_wait >= MISSION_FOCUS_PLAYER_ANMWAIT*2){
      pal_offset = INTSUB_ACTOR_PAL_BASE_START + target_netid;
      intsub->mission_target_focus_wait = 0;
    }
    if(pal_offset != -1){
      GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_CUR_S_0 + target_netid], 
        pal_offset, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
  }
  else{
    if(intsub->mission_target_focus_netid != INTRUDE_NETID_NULL){
      GFL_CLACT_WK_SetPlttOffs(
        intsub->act[INTSUB_ACTOR_CUR_S_0 + intsub->mission_target_focus_netid], 
        INTSUB_ACTOR_PAL_BASE_START + intsub->mission_target_focus_netid, 
        CLWK_PLTTOFFS_MODE_PLTT_TOP);
      intsub->mission_target_focus_netid = INTRUDE_NETID_NULL;
      intsub->mission_target_focus_wait = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�����J�[�\��
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_CursorL(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, ZONEID my_zone_id)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  int my_net_id, player_town_tblno;
  
  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  player_town_tblno = PALACE_TOWN_DATA_NULL;
  
  if(ZONEDATA_IsPalace(my_zone_id) == TRUE){
    pos.x = PALACE_CURSOR_POS_X + WearOffset[intsub->my_net_id][0];
    pos.y = PALACE_CURSOR_POS_Y + WearOffset[intsub->my_net_id][1];
    player_town_tblno = PALACE_TOWN_DATA_PALACE;
  }
  else{
    const PALACE_ZONE_SETTING *zonesetting = IntrudeField_GetZoneSettingData(my_zone_id);
    if(zonesetting != NULL){
      pos.x = zonesetting->sub_x + WearOffset[intsub->my_net_id][0];
      pos.y = zonesetting->sub_y + WearOffset[intsub->my_net_id][1];
      {
        int i;
        for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
          if(PalaceTownData[i].front_zone_id == zonesetting->zone_id
              || PalaceTownData[i].reverse_zone_id == zonesetting->zone_id){
            player_town_tblno = i;
            break;
          }
        }
      }
    }
    else{
      pos.x = NOTHING_ZONE_SUB_POS_X - 8;
      pos.y = NOTHING_ZONE_SUB_POS_Y + NOTHING_ZONE_SUB_POS_Y_SPACE * intsub->my_net_id;
      player_town_tblno = PALACE_TOWN_DATA_NULL;
    }
  }
  act = intsub->act[INTSUB_ACTOR_CUR_L];
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_CUR_L], 
    INTSUB_ACTOR_PAL_BASE_START + intsub->my_net_id, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  GFL_CLACT_WK_SetPos(act, &pos, INTSUB_CLACT_REND_SUB);
  GFL_CLACT_WK_SetDrawEnable(act, TRUE);
  
  _SetPalFade_PlayerTown(intsub, player_town_tblno);
}

//--------------------------------------------------------------
/**
 * �X�V�����F�Q�����܂��{�^��
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_EntryButton(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);

  if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE){
    switch(intsub->comm.m_status){
    case MISSION_STATUS_NULL:
    case MISSION_STATUS_READY:
    case MISSION_STATUS_EXE:
    case MISSION_STATUS_RESULT:
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], FALSE);
      BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam[ENTRY_BUTTON_MSG_PATERN_ENTRY], FALSE);
      break;
    case MISSION_STATUS_NOT_ENTRY:
      GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_ENTRY], TRUE);
      BmpOam_ActorSetDrawEnable(intsub->entrymsg_bmpoam[ENTRY_BUTTON_MSG_PATERN_ENTRY], TRUE);
      break;
    default:
      GF_ASSERT(0);
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F���x�����l
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_LvNum(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int i, level;
  OCCUPY_INFO *my_occupy;
  GFL_CLWK *act;

  if(_TimeNum_CheckEnable(intsub) == FALSE){
    //�~�b�V����������Ă��Ȃ��̂ɐ������\������Ă���Ȃ��\���ɂ���
    if(GFL_CLACT_WK_GetDrawEnable(intsub->act[INTSUB_ACTOR_LV_NUM_KETA_0]) == TRUE){
      for(i = INTSUB_ACTOR_LV_NUM_KETA_0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX; i++){
        GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);
      }
      _TimeScrn_Clear();
    }
    return;
  }
  else{
    //�~�b�V�������ɐ������\������Ă��Ȃ��Ȃ�\��
    if(GFL_CLACT_WK_GetDrawEnable(intsub->act[INTSUB_ACTOR_LV_NUM_KETA_0]) == FALSE){
      for(i = INTSUB_ACTOR_LV_NUM_KETA_0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX; i++){
        GFL_CLACT_WK_SetDrawEnable(intsub->act[i], TRUE);
      }
      _TimeScrn_Recover(intsub);
    }
  }
  
  my_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
//  level = my_occupy->intrude_level;
  level = intsub->comm.m_timer;
  
  for(i = 0; i <= INTSUB_ACTOR_LV_NUM_KETA_MAX - INTSUB_ACTOR_LV_NUM_KETA_0; i++){
    act = intsub->act[INTSUB_ACTOR_LV_NUM_KETA_0 + i];
    GFL_CLACT_WK_SetAnmIndex(act, level % 10);
    level /= 10;
  }
}

//--------------------------------------------------------------
/**
 * �^�C�g�����b�Z�[�W�X�V����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_TitleMsgUpdate(INTRUDE_SUBDISP_PTR intsub, ZONEID my_zone_id)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int print_type = _TITLE_PRINT_NULL;
  int msg_id;
  
  if(ZONEDATA_IsPalace(my_zone_id) == TRUE){
    if(intsub->comm.now_palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
      print_type = _TITLE_PRINT_MY_PALACE;
      msg_id = msg_invasion_title_001;
    }
    else{
      print_type = _TITLE_PRINT_OTHER_PALACE0 + intsub->comm.now_palace_area;
      msg_id = msg_invasion_title_002;
    }
  }
  else{
    print_type = _TITLE_PRINT_PALACE_GO;
    msg_id = msg_invasion_title_000;
  }
  
  if(intsub->title_print_type == print_type || print_type == _TITLE_PRINT_NULL){
    return; //���ɕ\���ς�
  }
  intsub->title_print_type = print_type;
  
  if(print_type >= _TITLE_PRINT_OTHER_PALACE0 && print_type <= _TITLE_PRINT_OTHER_PALACE3){
    const MYSTATUS *myst;
    myst = GAMEDATA_GetMyStatusPlayer(gamedata, intsub->comm.now_palace_area);
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
static void _IntSub_InfoMsgUpdate(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  GAME_COMM_INFO_MESSAGE infomsg;
  int k;
  BOOL msg_on = FALSE;
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
  
  if(intcomm != NULL && intsub->print_touch_player != INTRUDE_NETID_NULL 
      && Intrude_GetMyStatus(intcomm, intsub->print_touch_player) != NULL){
    MYSTATUS *myst = Intrude_GetMyStatus(intcomm, intsub->print_touch_player);
    
    intsub->print_mission_status = MISSION_STATUS_NULL;
    GFL_MSG_GetString(intsub->msgdata, msg_invasion_info_011, intsub->strbuf_temp );
    WORDSET_RegisterPlayerName( intsub->wordset, 0, myst );
    WORDSET_ExpandStr(intsub->wordset, intsub->strbuf_info, intsub->strbuf_temp);
    intsub->print_touch_player = INTRUDE_NETID_NULL;
    msg_on = TRUE;
  }
  else if(intsub->comm.m_status != MISSION_STATUS_NULL && intsub->comm.p_md != NULL
      && (intsub->comm.m_status == MISSION_STATUS_EXE || intsub->print_mission_status != intsub->comm.m_status)){
    //�~�b�V�����������̏ꍇ�̓~�b�V�����֘A�̃��b�Z�[�W��D�悵�ĕ\��
    //�~�b�V�����͏󋵂��������b�Z�[�W�ׁ̈A�L���[�ɒ��߂��Ɍ��݂̏�Ԃ����̂܂ܕ\��
    switch(intsub->comm.m_status){
    case MISSION_STATUS_NOT_ENTRY: //�~�b�V�����͎��{����Ă��邪�Q�����Ă��Ȃ�
      GFL_MSG_GetString(intsub->msgdata, msg_invasion_info_004, intsub->strbuf_info );
      break;
    case MISSION_STATUS_READY:     //�~�b�V�����J�n�҂�
      GFL_MSG_GetString(intsub->msgdata, msg_invasion_info_005, intsub->strbuf_info );
      break;
    case MISSION_STATUS_EXE:       //�~�b�V������
      if(intsub->print_mission_exe_flag == 0){
        GFL_MSG_GetString(intsub->msgdata, 
          msg_invasion_info_m01 + intsub->comm.p_md->cdata.type, intsub->strbuf_info );
      }
      else{
        GFL_MSG_GetString(intsub->msgdata_mission, 
          intsub->comm.p_md->cdata.msg_id_contents, intsub->strbuf_temp );
        MISSIONDATA_Wordset(&intsub->comm.p_md->cdata, 
          &intsub->comm.p_md->target_info, intsub->wordset, HEAPID_FIELD_SUBSCREEN);
        WORDSET_ExpandStr(intsub->wordset, intsub->strbuf_info, intsub->strbuf_temp);
      }
      intsub->print_mission_exe_flag ^= 1;
      break;
    case MISSION_STATUS_RESULT:   //���ʎ�M
      return;   //���ɕ\��������̂͂Ȃ�
    default:
      GF_ASSERT_MSG(0, "m_status=%d\n", intsub->comm.m_status);
      return;
    }
    if(intsub->comm.m_status != MISSION_STATUS_EXE){
      intsub->print_mission_exe_flag = 0;
    }
    intsub->print_mission_status = intsub->comm.m_status;
    msg_on = TRUE;
  }
  else if(GameCommInfo_GetMessage(game_comm, &infomsg) == TRUE){
    intsub->print_mission_status = MISSION_STATUS_NULL;
    GFL_MSG_GetString(intsub->msgdata, infomsg.message_id, intsub->strbuf_temp );
    for(k = 0 ; k < INFO_WORDSET_MAX; k++){
      if(infomsg.name[k]!=NULL){
        WORDSET_RegisterWord( intsub->wordset, infomsg.wordset_no[k], infomsg.name[k], 
          infomsg.wordset_sex[k], TRUE, PM_LANG);
      }
    }
    WORDSET_ExpandStr(intsub->wordset, intsub->strbuf_info, intsub->strbuf_temp);
    msg_on = TRUE;
  }
  intsub->print_touch_player = INTRUDE_NETID_NULL;
  
  if(msg_on == TRUE){
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
 * @param   intsub		
 *
 * @retval  OCCUPY_INFO *		�N����̐苒���
 */
//--------------------------------------------------------------
static OCCUPY_INFO * _IntSub_GetArreaOccupy(INTRUDE_SUBDISP_PTR intsub)
{
  OCCUPY_INFO *area_occupy;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);

  if(intsub->comm.now_palace_area == GAMEDATA_GetIntrudeMyID(gamedata)
      || intsub->comm.now_palace_area == PALACE_AREA_NO_NULL){
    area_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    area_occupy = GAMEDATA_GetOccupyInfo(gamedata, intsub->comm.now_palace_area);
  }
  
  return area_occupy;
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

  if(intsub->event_req != _EVENT_REQ_NO_NULL || intsub->wfbc_go == TRUE 
      || GFL_UI_TP_GetPointTrg(&x, &y) == FALSE 
      || FIELD_SUBSCREEN_GetAction( subscreen ) != FIELD_SUBSCREEN_ACTION_NONE){
    return;
  }
  
  //�v���C���[�A�C�R���^�b�`����
  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(net_id != my_net_id && (intsub->comm.recv_profile & (1 << net_id))){
      act = intsub->act[INTSUB_ACTOR_CUR_S_0 + net_id];
      if(GFL_CLACT_WK_GetDrawEnable(act) == TRUE){
        GFL_CLACT_WK_GetPos( act, &pos, INTSUB_CLACT_REND_SUB );
        if(x - TOUCH_RANGE_PLAYER_ICON_X <= pos.x && x + TOUCH_RANGE_PLAYER_ICON_X >= pos.x
            && y - TOUCH_RANGE_PLAYER_ICON_Y <= pos.y && y + TOUCH_RANGE_PLAYER_ICON_Y >= pos.y){
        #ifdef PM_DEBUG
          if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
            if(intcomm!= NULL && ZONEDATA_IsWfbc(intcomm->intrude_status[net_id].zone_id) == TRUE){
              intsub->wfbc_go = TRUE;
              intsub->wfbc_seq = 0;
            }
            else{
              Intrude_SetWarpPlayerNetID(game_comm, net_id);
              intsub->event_req = _EVENT_REQ_NO_PLAYER_WARP;
            }
            return;
          }
          else
        #endif
          {
            intsub->print_touch_player = net_id;
            return;
          }
        }
      }
    }
  }

  //�X�^�b�`����
  if(intsub->comm.now_palace_area != GAMEDATA_GetIntrudeMyID(gamedata)){
    for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
      _SetRect(PalaceTownData[i].subscreen_x, PalaceTownData[i].subscreen_y, 
        TOWN_ICON_HITRANGE_HALF, TOWN_ICON_HITRANGE_HALF, &rect);
      if(_CheckRectHit(x, y, &rect) == TRUE){
        if(i == PALACE_TOWN_WFBC){
          intsub->wfbc_go = TRUE;
          intsub->wfbc_seq = 0;
        }
        else{
          const PALACE_ZONE_SETTING *zonesetting = IntrudeField_GetZoneSettingData(PalaceTownData[i].front_zone_id);
          intsub->warp_zone_id = zonesetting->warp_zone_id;
          intsub->event_req = _EVENT_REQ_NO_TOWN_WARP;
        }
        intsub->decide_town_tblno = i;
        return;
      }
    }
  }

  //�p���X���^�b�`����
  if(intsub->comm.now_palace_area != GAMEDATA_GetIntrudeMyID(gamedata)
      || GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE){
    _SetRect(PALACE_ICON_POS_X, PALACE_ICON_POS_Y, 
      PALACE_ICON_HITRANGE_HALF, PALACE_ICON_HITRANGE_HALF, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      intsub->warp_zone_id = ZONE_ID_PALACE01;
      intsub->event_req = _EVENT_REQ_NO_TOWN_WARP;
      intsub->decide_town_tblno = PALACE_TOWN_DATA_PALACE;
      return;
    }
  }
  
  ///�Q���{�^���^�b�`����
  if(intcomm != NULL && intsub->comm.m_status == MISSION_STATUS_NOT_ENTRY){
    _SetRect(ENTRY_BUTTON_POS_X, ENTRY_BUTTON_POS_Y, 
      ENTRY_BUTTON_HITRANGE_HALF_X, ENTRY_BUTTON_HITRANGE_HALF_Y, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      MISSION_SetMissionEntry(intcomm, &intcomm->mission);
      intsub->event_req = _EVENT_REQ_NO_MISSION_ENTRY;
    }
  }
  ///���ǂ�{�^���^�b�`����
  if(intsub->back_exit == FALSE 
      && BmpOam_ActorGetDrawEnable(intsub->entrymsg_bmpoam[ENTRY_BUTTON_MSG_PATERN_BACK]) == TRUE){
    _SetRect(ENTRY_BUTTON_POS_X, ENTRY_BUTTON_POS_Y, 
      ENTRY_BUTTON_HITRANGE_HALF_X, ENTRY_BUTTON_HITRANGE_HALF_Y, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      intsub->back_exit = TRUE;
    }
  }
}

//--------------------------------------------------------------
/**
 * �ڑ��l�����Ď����ABar�X�N���[���̈�l�p�ƒʐM�p��ύX����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BGBarUpdate(INTRUDE_SUBDISP_PTR intsub)
{
  BOOL scrn_load = FALSE;
  
  if(intsub->bar_type == BG_BAR_TYPE_SINGLE){
    if(intsub->comm.recv_num > 1){
      GFL_ARC_UTIL_TransVramScreen(
        ARCID_PALACE, NARC_palace_palace_bar2_lz_NSCR, INTRUDE_FRAME_S_BAR, 0, 
        0x800, TRUE, HEAPID_FIELDMAP);
      intsub->bar_type = BG_BAR_TYPE_COMM;
      scrn_load++;
    }
  }
  else{ //BG_BAR_TYPE_COMM
    if(intsub->comm.recv_num <= 1){
      GFL_ARC_UTIL_TransVramScreen(
        ARCID_PALACE, NARC_palace_palace_bar_lz_NSCR, INTRUDE_FRAME_S_BAR, 0, 
        0x800, TRUE, HEAPID_FIELDMAP);
      intsub->bar_type = BG_BAR_TYPE_SINGLE;
      scrn_load++;
    }
  }

  if(scrn_load && _TimeNum_CheckEnable(intsub) == TRUE){
    _TimeScrn_Recover(intsub);
  }
}

//--------------------------------------------------------------
/**
 * �N���G���A���Ď����ABG�̃J���[��ύX����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_BGColorUpdate(INTRUDE_SUBDISP_PTR intsub)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  int bg_pal;
  
  bg_pal = intsub->comm.now_palace_area;
  if(bg_pal == intsub->now_bg_pal){
    return;
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

//--------------------------------------------------------------
/**
 * intcomm��������o���p�����[�^�o�b�t�@�̏�����
 *
 * @param   intsub		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void _IntSub_CommParamInit(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm)
{
  INTRUDE_COMM_PARAM *comm = &intsub->comm;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  
  if(intcomm == NULL){
    comm->now_palace_area = GAMEDATA_GetIntrudeMyID(gamedata);
    comm->recv_profile = 1 << comm->now_palace_area;
    comm->recv_num = 1;
    comm->palace_in = FALSE;
    comm->m_status = MISSION_STATUS_NULL;
    comm->m_timer = 0;
  }
  else{
    _IntSub_CommParamUpdate(intsub, intcomm);
  }
}

//--------------------------------------------------------------
/**
 * intcomm��������o���p�����[�^�̍X�V����
 *
 * @param   intsub		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void _IntSub_CommParamUpdate(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm)
{
  INTRUDE_COMM_PARAM *comm = &intsub->comm;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  
  if(intcomm == NULL){
    //�ꕔ�������čŌ�̏�Ԃ��牽���X�V���Ȃ�
    intsub->comm.m_status = MISSION_STATUS_NULL;
    intsub->comm.p_md = NULL;
  }
  else{
    const MISSION_DATA *md;

    comm->now_palace_area = intcomm->intrude_status_mine.palace_area;
    comm->recv_profile = intcomm->recv_profile;
    comm->recv_num = MATH_CountPopulation((u32)(comm->recv_profile));
    comm->palace_in = intcomm->palace_in;
    comm->m_status = MISSION_FIELD_CheckStatus(&intcomm->mission);
    comm->m_timer = MISSION_GetMissionTimer(&intcomm->mission);

    comm->p_md = MISSION_GetRecvData(&intcomm->mission);
  }
}

//--------------------------------------------------------------
/**
 * �X�^�b�`�̌���p���b�g�A�j���𔭓�
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _SetPalFlash_DecideTown(INTRUDE_SUBDISP_PTR intsub)
{
  int i;
  
  intsub->decide_pal_timer = 0;
  intsub->decide_pal_occ = TRUE;
  
  //�S�Ẵ^�b�`�A�N�^�[���m�[�}���F�ɂ���
  for(i = 0; i < INTSUB_ACTOR_TOUCH_TOWN_MAX - INTSUB_ACTOR_TOUCH_TOWN_0; i++){
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + i], 
      INTSUB_ACTOR_PAL_TOUCH_NORMAL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], 
    INTSUB_ACTOR_PAL_TOUCH_NORMAL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
}

//--------------------------------------------------------------
/**
 * �X�^�b�`�̌���p���b�g�A�j�������쒆���𒲂ׂ�
 * @param   intsub		
 * @retval  BOOL		  TRUE:���쒆�@FALSE:���삵�Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL _CheckPalFlash_DecideTown(INTRUDE_SUBDISP_PTR intsub)
{
  if(intsub->decide_pal_occ == TRUE && intsub->decide_pal_timer < TOWN_DECIDE_PAL_FLASH_TIMER_MAX){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ����������X�ɑ΂��ăp���b�g�A�j�����N�G�X�g��������
 *
 * @param   intsub		
 * @param   town_tblno		
 */
//--------------------------------------------------------------
static void _SetPalFade_PlayerTown(INTRUDE_SUBDISP_PTR intsub, int town_tblno)
{
  if(intsub->player_pal_tblno == town_tblno || intsub->decide_pal_occ == TRUE){
    return;
  }
  
  //���܂Ō����Ă����ӏ����m�[�}���F�ɂ���
  if(intsub->player_pal_tblno < PALACE_TOWN_DATA_MAX){
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + intsub->player_pal_tblno], 
      INTSUB_ACTOR_PAL_TOUCH_NORMAL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }
  else if(intsub->player_pal_tblno == PALACE_TOWN_DATA_PALACE){
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], 
      INTSUB_ACTOR_PAL_TOUCH_NORMAL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }
  
  //���Ɍ��点��ӏ����p�̃p���b�g�ɕς���
  if(town_tblno < PALACE_TOWN_DATA_MAX){
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_TOWN_0 + town_tblno], 
      INTSUB_ACTOR_PAL_TOUCH_MINE, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }
  else if(town_tblno == PALACE_TOWN_DATA_PALACE){
    GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_TOUCH_PALACE], 
      INTSUB_ACTOR_PAL_TOUCH_MINE, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }
  
  intsub->player_pal_tblno = town_tblno;
  intsub->player_pal_evy = 0;
  intsub->player_pal_dir = DIR_UP;
}

//--------------------------------------------------------------
/**
 * [TIME]�̕�����\������K�v�������Ԃ��𒲂ׂ�
 *
 * @param   intsub		
 *
 * @retval  BOOL		  TRUE:�\������K�v������
 */
//--------------------------------------------------------------
static BOOL _TimeNum_CheckEnable(INTRUDE_SUBDISP_PTR intsub)
{
  switch(intsub->comm.m_status){
  case MISSION_STATUS_READY:     //�~�b�V�����J�n�҂�
  case MISSION_STATUS_EXE:       //�~�b�V������
  case MISSION_STATUS_RESULT:    //���ʂ��͂��Ă��� or ���ʕ\����
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * [TIME]��������Ă���X�N���[�����N���A����
 *
 * @param   none		
 */
//--------------------------------------------------------------
static void _TimeScrn_Clear(void)
{
  GFL_BG_FillScreen( INTRUDE_FRAME_S_BAR, BG_TIME_SCRN_CLEAR_CODE, 
    BG_TIME_SCRN_POS_X, BG_TIME_SCRN_POS_Y, BG_TIME_SCRN_SIZE_X, BG_TIME_SCRN_SIZE_Y, 
    GFL_BG_SCRWRT_PALIN );
  GFL_BG_LoadScreenV_Req(INTRUDE_FRAME_S_BAR);
}
//--------------------------------------------------------------
/**
 * [TIME]��������Ă���X�N���[���ɕ��A����
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _TimeScrn_Recover(INTRUDE_SUBDISP_PTR intsub)
{
  GFL_BG_WriteScreenExpand( INTRUDE_FRAME_S_BAR, BG_TIME_SCRN_POS_X, BG_TIME_SCRN_POS_Y, 
    BG_TIME_SCRN_SIZE_X, BG_TIME_SCRN_SIZE_Y,
    intsub->scrnbuf_time, 0, 0, BG_TIME_SCRN_SIZE_X, BG_TIME_SCRN_SIZE_Y);
  GFL_BG_LoadScreenV_Req(INTRUDE_FRAME_S_BAR);
}
