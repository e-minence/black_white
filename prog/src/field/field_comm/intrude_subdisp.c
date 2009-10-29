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

///�A�N�^�[Index
enum{
  INTSUB_ACTOR_TOWN_0,
  INTSUB_ACTOR_TOWN_1,
  INTSUB_ACTOR_TOWN_2,
  INTSUB_ACTOR_TOWN_3,
  INTSUB_ACTOR_TOWN_4,
  INTSUB_ACTOR_TOWN_5,
  INTSUB_ACTOR_TOWN_6,
  INTSUB_ACTOR_TOWN_7,
  INTSUB_ACTOR_TOWN_MAX = INTSUB_ACTOR_TOWN_7,
  
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
  
  INTSUB_ACTOR_MARK,        ///<�����̂���G���A���w��

  INTSUB_ACTOR_POWER,
  INTSUB_ACTOR_MISSION,
  INTSUB_ACTOR_INCLUSION,
  
  INTSUB_ACTOR_SENKYO_EFF_0,
  INTSUB_ACTOR_SENKYO_EFF_1,
  INTSUB_ACTOR_SENKYO_EFF_2,
  INTSUB_ACTOR_SENKYO_EFF_3,
  INTSUB_ACTOR_SENKYO_EFF_4,
  INTSUB_ACTOR_SENKYO_EFF_5,
  INTSUB_ACTOR_SENKYO_EFF_6,
  INTSUB_ACTOR_SENKYO_EFF_7,
  INTSUB_ACTOR_SENKYO_EFF_MAX = INTSUB_ACTOR_SENKYO_EFF_7,

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

///OBJ�p���b�gINDEX
enum{
  INTSUB_ACTOR_PAL_BASE_START = 1,    ///<�v���C���[���ɕς��p���b�g�J�n�ʒu
  
  INTSUB_ACTOR_PAL_MAX = 7,
};

///palace_obj.nce�̃A�j���[�V�����V�[�P���X
enum{
  PALACE_ACT_ANMSEQ_TOWN_G,
  PALACE_ACT_ANMSEQ_TOWN_W,
  PALACE_ACT_ANMSEQ_TOWN_B,
  PALACE_ACT_ANMSEQ_AREA,
  PALACE_ACT_ANMSEQ_CUR_S,
  PALACE_ACT_ANMSEQ_CUR_L,
  PALACE_ACT_ANMSEQ_MARK,
  PALACE_ACT_ANMSEQ_POWER,
  PALACE_ACT_ANMSEQ_MISSION,
  PALACE_ACT_ANMSEQ_INCLUSION,
  PALACE_ACT_ANMSEQ_SENKYO_EFF,
  PALACE_ACT_ANMSEQ_LV_NUM,
  PALACE_ACT_ANMSEQ_POINT_NUM,
};

///�A�N�^�[�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_TOWN = 100,
  SOFTPRI_AREA = 50,
  SOFTPRI_CUR_S = 20,
  SOFTPRI_CUR_L = 19,
  SOFTPRI_MARK = SOFTPRI_AREA - 1,
  SOFTPRI_POWER = 10,
  SOFTPRI_MISSION = 10,
  SOFTPRI_INCLUSION = 10,
  SOFTPRI_SENKYO_EFF = SOFTPRI_MARK - 1,
  SOFTPRI_LV_NUM = 5,
  SOFTPRI_POINT_NUM = 5,
};
///�A�N�^�[����BG�v���C�I���e�B
#define BGPRI_ACTOR_COMMON      (2)

///BG�p���b�gINDEX
enum{
  INTSUB_BG_PAL_SYSTEM,       ///<�S�v���C���[���ʂ̃V�X�e���F
  INTSUB_BG_PAL_BASE_START,   ///<�v���C���[���ɕς��p���b�g�J�n�ʒu
  
  INTSUB_BG_PAL_MAX = 5,
};

///�X�A�C�R���X�V���N�G�X�g
enum{
  TOWN_UPDATE_REQ_NONE,           ///<���N�G�X�g����
  TOWN_UPDATE_REQ_NOT_EFFECT,     ///<�X�A�C�R���̍X�V�͂��邪�苒�G�t�F�N�g�͖���
  TOWN_UPDATE_REQ_UPDATE,         ///<�X�A�C�R���̍X�V�{�ω�������ΐ苒�G�t�F�N�g
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///�X�A�C�R���̃^�b�`����̋�`�n�[�t�T�C�Y
#define TOWN_ICON_HITRANGE_HALF   (8)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�N������ʐ��䃏�[�N
typedef struct _INTRUDE_SUBDISP{
  GAMESYS_WORK *gsys;
  
  u32 index_cgr;
  u32 index_pltt;
  u32 index_cell;

	GFL_CLUNIT *clunit;
  GFL_CLWK *act[INTSUB_ACTOR_MAX];
  
  u8 my_net_id;
  u8 town_update_req;     ///<�X�A�C�R���X�V���N�G�X�g(TOWN_UPDATE_REQ_???)
  u8 now_bg_pal;          ///<���݂�BG�̃p���b�g�ԍ�
  u8 padding;
  
  u32 senkyo_eff_bit;     ///<�苒�G�t�F�N�g�N�����̊X(bit�Ǘ�)
}INTRUDE_SUBDISP;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _IntSub_SystemSetup(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_SystemExit(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_BGLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_BGUnload(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorResouceLoad(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorResourceUnload(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorDelete(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_ActorCreate_Town(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_SenkyoEff(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_Area(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorS(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_CursorL(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_Mark(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_Power(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_LvNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorCreate_PointNum(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle);
static void _IntSub_ActorUpdate_Town(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_SenkyoEff(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_Area(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_CursorS(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_CursorL(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_Mark(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_Power(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_LvNum(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static void _IntSub_ActorUpdate_PointNum(
  INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy);
static OCCUPY_INFO * _IntSub_GetArreaOccupy(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static u8 _IntSub_GetProfileRecvNum(INTRUDE_COMM_SYS_PTR intcomm);
static u8 _IntSub_TownPosGet(ZONEID zone_id, GFL_CLACTPOS *dest_pos);
static void _SetRect(int x, int y, int half_size_x, int half_size_y, GFL_RECT *rect);
static BOOL _CheckRectHit(int x, int y, const GFL_RECT *rect);
static void _IntSub_TouchUpdate(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_TownChangeCheck(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, u8 update_req);
static void _IntSub_SenkyoEff_Start(INTRUDE_SUBDISP_PTR intsub, u32 eff_bit);
static void _IntSub_SenkyoEff_EndWatch(INTRUDE_SUBDISP_PTR intsub);
static void _IntSub_TownActChange(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, int town_tblno);
static void _IntSub_TownActBitChange(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, u32 change_bit);
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
  AREA_POST_LEFT = 0xd * 8,                           ///<�z�u���W�͈͂̍��[X
  AREA_POST_RIGHT = 0x13 * 8,                         ///<�z�u���W�͈͂̉E�[X
  AREA_POST_WIDTH = AREA_POST_RIGHT - AREA_POST_LEFT, ///<�z�u���W�͈͂̕�
  
  AREA_POST_Y = 8,                                    ///<�z�u���WY
};

///POWER�n�A�C�R���z�u���W
enum{
  POWER_ICON_POS_X = 0x15*8,
  POWER_ICON_POS_Y = 8*8,

  MISSION_ICON_POS_X = 0x1d*8,
  MISSION_ICON_POS_Y = 3*8,

  INCLUSION_ICON_POS_X = MISSION_ICON_POS_X,
  INCLUSION_ICON_POS_Y = MISSION_ICON_POS_Y,

  POWER_ICON_HITRANGE_HALF_X = 24, ///<�p���[�n�A�C�R���̃^�b�`���蔼�aX
  POWER_ICON_HITRANGE_HALF_Y = 8,  ///<�p���[�n�A�C�R���̃^�b�`���蔼�aY
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
  
  intsub = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(INTRUDE_SUBDISP));
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
  _IntSub_ActorResouceLoad(intsub, handle);
  _IntSub_ActorCreate(intsub, handle);
  
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
  _IntSub_ActorDelete(intsub);
  _IntSub_ActorResourceUnload(intsub);
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
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  OCCUPY_INFO *area_occupy;
  
  if(intcomm == NULL){
    return;
  }
  
  area_occupy = _IntSub_GetArreaOccupy(intcomm, intsub);
  
  //�^�b�`����`�F�b�N
  _IntSub_TouchUpdate(intcomm, intsub);
  
  //BG�X�N���[���J���[�ύX�`�F�b�N
  _IntSub_BGColorUpdate(intcomm, intsub);
  
  //�A�N�^�[�X�V
  _IntSub_ActorUpdate_Town(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_SenkyoEff(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_Area(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_CursorS(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_CursorL(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_Mark(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_Power(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_LvNum(intsub, intcomm, area_occupy);
  _IntSub_ActorUpdate_PointNum(intsub, intcomm, area_occupy);
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
  ;
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
  intsub->clunit = GFL_CLACT_UNIT_Create(INTSUB_ACTOR_MAX, 5, HEAPID_FIELDMAP);
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
  GFL_BG_ClearScreen(INTRUDE_FRAME_S_BAR);
  
  //�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_palace_palace_bg_NCLR, PALTYPE_SUB_BG, 0, 
    0x20 * INTSUB_BG_PAL_MAX, HEAPID_FIELDMAP);

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
    NARC_palace_palace_obj_NANR, HEAPID_FIELDMAP);
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
  _IntSub_ActorCreate_Town(intsub, handle);
  _IntSub_ActorCreate_SenkyoEff(intsub, handle);
  _IntSub_ActorCreate_Area(intsub, handle);
  _IntSub_ActorCreate_CursorS(intsub, handle);
  _IntSub_ActorCreate_CursorL(intsub, handle);
  _IntSub_ActorCreate_Mark(intsub, handle);
  _IntSub_ActorCreate_Power(intsub, handle);
  _IntSub_ActorCreate_LvNum(intsub, handle);
  _IntSub_ActorCreate_PointNum(intsub, handle);
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
 * �X�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_Town(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_TOWN_G,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_TOWN,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  GF_ASSERT(PALACE_TOWN_DATA_MAX == (INTSUB_ACTOR_TOWN_MAX - INTSUB_ACTOR_TOWN_0 + 1));
  GF_ASSERT(PALACE_TOWN_DATA_MAX == INTRUDE_TOWN_MAX);
  for(i = INTSUB_ACTOR_TOWN_0; i <= INTSUB_ACTOR_TOWN_MAX; i++){
    head.pos_x = PalaceTownData[i].subscreen_x;
    head.pos_y = PalaceTownData[i].subscreen_y;
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
  }
  
  intsub->town_update_req = TOWN_UPDATE_REQ_NOT_EFFECT;
}

//--------------------------------------------------------------
/**
 * �苒�G�t�F�N�g�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_SenkyoEff(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_SENKYO_EFF,   //�A�j���[�V�����V�[�P���X
  	SOFTPRI_SENKYO_EFF,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  GF_ASSERT(PALACE_TOWN_DATA_MAX == (INTSUB_ACTOR_SENKYO_EFF_MAX - INTSUB_ACTOR_SENKYO_EFF_0 + 1));
  for(i = INTSUB_ACTOR_SENKYO_EFF_0; i <= INTSUB_ACTOR_SENKYO_EFF_MAX; i++){
    head.pos_x = PalaceTownData[i].subscreen_x;
    head.pos_y = PalaceTownData[i].subscreen_y;
    intsub->act[i] = GFL_CLACT_WK_Create(intsub->clunit, 
      intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], FALSE);  //�\��OFF
  }
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
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
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
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
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
  static const GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_CUR_L,     //�A�j���[�V�����V�[�P���X
  	SOFTPRI_CUR_L,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  intsub->act[INTSUB_ACTOR_CUR_L] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_CUR_L], 
    INTSUB_ACTOR_PAL_BASE_START + intsub->my_net_id, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_CUR_L], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_CUR_L], TRUE);  //�I�[�g�A�j��ON
}

//--------------------------------------------------------------
/**
 * �����̐N���G���A�������}�[�N�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_Mark(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  static const GFL_CLWK_DATA head = {
  	0, 0,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_MARK,     //�A�j���[�V�����V�[�P���X
  	SOFTPRI_MARK,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  intsub->act[INTSUB_ACTOR_MARK] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetPlttOffs(intsub->act[INTSUB_ACTOR_MARK], 
    INTSUB_ACTOR_PAL_BASE_START + intsub->my_net_id, CLWK_PLTTOFFS_MODE_PLTT_TOP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_MARK], FALSE);  //�\��OFF
}

//--------------------------------------------------------------
/**
 * �p���[�}�[�N�A�N�^�[����
 *
 * @param   intsub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _IntSub_ActorCreate_Power(INTRUDE_SUBDISP_PTR intsub, ARCHANDLE *handle)
{
  GFL_CLWK_DATA head = {
  	POWER_ICON_POS_X, POWER_ICON_POS_Y,                       //X, Y���W
  	PALACE_ACT_ANMSEQ_POWER,     //�A�j���[�V�����V�[�P���X
  	SOFTPRI_POWER,               //�\�t�g�v���C�I���e�B
  	BGPRI_ACTOR_COMMON,         //BG�v���C�I���e�B
  };
  
  //POWER
  intsub->act[INTSUB_ACTOR_POWER] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_POWER], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_POWER], TRUE);  //�I�[�g�A�j��ON

  //MISSION
  head.pos_x = MISSION_ICON_POS_X;
  head.pos_y = MISSION_ICON_POS_Y;
  head.anmseq = PALACE_ACT_ANMSEQ_MISSION;
  intsub->act[INTSUB_ACTOR_MISSION] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_MISSION], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_MISSION], TRUE);  //�I�[�g�A�j��ON

  //INCLUSION
  head.pos_x = INCLUSION_ICON_POS_X;
  head.pos_y = INCLUSION_ICON_POS_Y;
  head.anmseq = PALACE_ACT_ANMSEQ_INCLUSION;
  intsub->act[INTSUB_ACTOR_INCLUSION] = GFL_CLACT_WK_Create(intsub->clunit, 
    intsub->index_cgr, intsub->index_pltt, intsub->index_cell, 
    &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_INCLUSION], FALSE);  //�\��OFF
  GFL_CLACT_WK_SetAutoAnmFlag(intsub->act[INTSUB_ACTOR_INCLUSION], TRUE);  //�I�[�g�A�j��ON
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
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
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
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
    GFL_CLACT_WK_SetDrawEnable(intsub->act[i], TRUE);
    head.pos_x -= width;
  }
}

//==============================================================================
//  �X�V
//==============================================================================
//--------------------------------------------------------------
/**
 * �X�V�����F�X�A�N�^�[
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_Town(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  if(intsub->town_update_req == TOWN_UPDATE_REQ_NONE){
    if(intcomm->area_occupy_update == TRUE){
      intsub->town_update_req = TOWN_UPDATE_REQ_UPDATE;
      intcomm->area_occupy_update = FALSE;
    }
  }
  
  if(intsub->town_update_req != TOWN_UPDATE_REQ_NONE && intsub->senkyo_eff_bit == 0){
    _IntSub_TownChangeCheck(intsub, area_occupy, intsub->town_update_req);
    intsub->town_update_req = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * �X�A�N�^�[�Ɛ苒�����r���A�ύX������ΑΉ������A�j���ɕύX�A�苒�G�t�F�N�g�N��������
 *
 * @param   intsub		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_TownChangeCheck(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, u8 update_req)
{
  int i;
  GFL_CLWK *act;
  int white_town, black_town;
  u32 eff_bit;      //�苒�G�t�F�N�g�N������K�v�L
  u32 change_bit;   //�X�A�C�R���Ɛ苒���ɑ��Ⴊ������
  int anmseq;
  
  white_town = 0;
  black_town = 0;
  eff_bit = 0;
  change_bit = 0;
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    act = intsub->act[INTSUB_ACTOR_TOWN_0 + i];
    anmseq = GFL_CLACT_WK_GetAnmSeq(act);
    switch(anmseq){
    case PALACE_ACT_ANMSEQ_TOWN_G:
      switch(area_occupy->town.town_occupy[i]){
      case OCCUPY_TOWN_WHITE:
        white_town++;
        change_bit |= 1 << i;
        eff_bit |= 1 << i;
        break;
      case OCCUPY_TOWN_BLACK:
        black_town++;
        change_bit |= 1 << i;
        eff_bit |= 1 << i;
        break;
      }
      break;
    case PALACE_ACT_ANMSEQ_TOWN_W:
      switch(area_occupy->town.town_occupy[i]){
      case OCCUPY_TOWN_WHITE:
        white_town++;
        break;
      case OCCUPY_TOWN_BLACK:
        black_town++;
        change_bit |= 1 << i;
        eff_bit |= 1 << i;
        break;
      default:
        change_bit |= 1 << i;
        break;
      }
      break;
    case PALACE_ACT_ANMSEQ_TOWN_B:
      switch(area_occupy->town.town_occupy[i]){
      case OCCUPY_TOWN_WHITE:
        white_town++;
        change_bit |= 1 << i;
        eff_bit |= 1 << i;
        break;
      case OCCUPY_TOWN_BLACK:
        black_town++;
        break;
      default:
        change_bit |= 1 << i;
        break;
      }
      break;
    }
  }
  
  if(update_req == TOWN_UPDATE_REQ_NOT_EFFECT){
    eff_bit = 0;
  }
  else{
    if(white_town == INTRUDE_TOWN_MAX || black_town == INTRUDE_TOWN_MAX){
      for(i = 0; i < INTRUDE_TOWN_MAX; i++){
        eff_bit |= 1 << i;  //�S�Ă̊X�Ő苒�G�t�F�N�g���o��悤�ɑSbit��ON
      }
    }
  }
  
  if(eff_bit > 0){
    _IntSub_SenkyoEff_Start(intsub, eff_bit);
    intsub->senkyo_eff_bit |= eff_bit;
  }
  if(change_bit > 0){
    _IntSub_TownActBitChange(intsub, area_occupy, change_bit);
  }
}

//--------------------------------------------------------------
/**
 * �X�V�����F�苒�G�t�F�N�g
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_SenkyoEff(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  _IntSub_SenkyoEff_EndWatch(intsub);
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
  int net_id, profile_num, area_width;
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  
  profile_num = _IntSub_GetProfileRecvNum(intcomm);
  area_width = AREA_POST_WIDTH / (profile_num + 1);
  pos.x = AREA_POST_LEFT + area_width;
  pos.y = AREA_POST_Y;
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    act = intsub->act[INTSUB_ACTOR_AREA_0 + net_id];
    if(net_id == GAMEDATA_GetIntrudeMyID(gamedata) 
        || (intcomm->recv_profile & (1 << net_id))){
      GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
      GFL_CLACT_WK_SetDrawEnable(act, TRUE);
      pos.x += area_width;
    }
    else{ //�����ł͂Ȃ����A�v���t�B�[������M���Ă��Ȃ�
      GFL_CLACT_WK_SetDrawEnable(act, FALSE);
    }
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
  GF_ASSERT(NELEMS(PalaceWearOffset) == FIELD_COMM_MEMBER_MAX);
  
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
      town_no = _IntSub_TownPosGet(ist->zone_id, &pos);
//��check      GF_ASSERT(town_no != 0xff);
      if(town_no == 0xff){
        town_no = 0;
      }
      pos.x += TownWearOffset[town_num[town_no]];
      GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
      town_num[town_no]++;
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
  GFL_CLWK *act;
  GFL_CLACTPOS pos;
  
  _IntSub_TownPosGet(intcomm->intrude_status_mine.zone_id, &pos);
  act = intsub->act[INTSUB_ACTOR_CUR_L];
  GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
  GFL_CLACT_WK_SetDrawEnable(act, TRUE);
}

//--------------------------------------------------------------
/**
 * �X�V�����F�����N���G���A�}�[�N
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_Mark(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intsub->gsys);
  int net_id, profile_num;
  GFL_CLWK *act;
  u32 recv_profile;
  int area_width, s, my_area, pos_count;
  GFL_CLACTPOS pos;
  
  profile_num = _IntSub_GetProfileRecvNum(intcomm);
  area_width = AREA_POST_WIDTH / (profile_num + 1);
  pos.x = AREA_POST_LEFT + area_width;
  pos.y = AREA_POST_Y;

  my_area = intcomm->intrude_status_mine.palace_area;
  if(my_area == PALACE_AREA_NO_NULL){
    my_area = 0;
  }
  pos_count = 0;
  for(s = 0; s < my_area; s++){
    if(intcomm->recv_profile & (1 << s)){
      pos_count++;
    }
  }
  pos.x += area_width * pos_count;
  act = intsub->act[INTSUB_ACTOR_MARK];
  GFL_CLACT_WK_SetPos(act, &pos, CLSYS_DEFREND_SUB);
  GFL_CLACT_WK_SetDrawEnable(act, TRUE);
}

//--------------------------------------------------------------
/**
 * �X�V�����FPOWER
 *
 * @param   intsub		
 * @param   intcomm		
 * @param   area_occupy		
 */
//--------------------------------------------------------------
static void _IntSub_ActorUpdate_Power(INTRUDE_SUBDISP_PTR intsub, INTRUDE_COMM_SYS_PTR intcomm, OCCUPY_INFO *area_occupy)
{
  //MISSION
  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_MISSION], TRUE);
  }
  else{
    GFL_CLACT_WK_SetDrawEnable(intsub->act[INTSUB_ACTOR_MISSION], FALSE);
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
  level = my_occupy->intrude_level;
  
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
  point = my_occupy->intrude_point;
  
  for(i = 0; i <= INTSUB_ACTOR_POINT_NUM_KETA_MAX - INTSUB_ACTOR_POINT_NUM_KETA_0; i++){
    act = intsub->act[INTSUB_ACTOR_POINT_NUM_KETA_0 + i];
    GFL_CLACT_WK_SetAnmIndex(act, point % 10);
    point /= 10;
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
 * 
 * @retval  �XNo(�p���X�G���A�̏ꍇ��0xff)
 */
//--------------------------------------------------------------
static u8 _IntSub_TownPosGet(ZONEID zone_id, GFL_CLACTPOS *dest_pos)
{
  int i;
  
  for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
    if(PalaceTownData[i].reverse_zone_id == zone_id){
      dest_pos->x = PalaceTownData[i].subscreen_x;
      dest_pos->y = PalaceTownData[i].subscreen_y;
      return i;
    }
  }
  dest_pos->x = PALACE_CURSOR_POS_X;
  dest_pos->y = PALACE_CURSOR_POS_Y;
  return 0xff;
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
  int i;
  GFL_RECT rect;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(intsub->gsys);
  FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(intsub->gsys);
  
  if(GFL_UI_TP_GetPointTrg(&x, &y) == FALSE 
      || FIELD_SUBSCREEN_GetAction( subscreen ) != FIELD_SUBSCREEN_ACTION_NONE){
    return;
  }
  
  //�X�^�b�`����
  for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
    _SetRect(PalaceTownData[i].subscreen_x, PalaceTownData[i].subscreen_y, 
      TOWN_ICON_HITRANGE_HALF, TOWN_ICON_HITRANGE_HALF, &rect);
    if(_CheckRectHit(x, y, &rect) == TRUE){
      Intrude_SetWarpTown(game_comm, i);
      FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP);
      return;
    }
  }

  //�p���X���^�b�`����
  _SetRect(PALACE_ICON_POS_X, PALACE_ICON_POS_Y, 
    PALACE_ICON_HITRANGE_HALF, PALACE_ICON_HITRANGE_HALF, &rect);
  if(_CheckRectHit(x, y, &rect) == TRUE){
    Intrude_SetWarpTown(game_comm, PALACE_TOWN_DATA_PALACE);
    FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP);
    return;
  }
  
  //�~�b�V�����A�C�R���^�b�`����
  _SetRect(MISSION_ICON_POS_X, MISSION_ICON_POS_Y, 
    POWER_ICON_HITRANGE_HALF_X, POWER_ICON_HITRANGE_HALF_Y, &rect);
  if(_CheckRectHit(x, y, &rect) == TRUE){
    FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_INTRUDE_MISSION_PUT);
    return;
  }
}

//--------------------------------------------------------------
/**
 * �苒�G�t�F�N�g�J�n
 *
 * @param   intsub		
 * @param   eff_bit		�G�t�F�N�g�Ώۂ̊X(bit�Ǘ�)
 */
//--------------------------------------------------------------
static void _IntSub_SenkyoEff_Start(INTRUDE_SUBDISP_PTR intsub, u32 eff_bit)
{
  GFL_CLWK *act;
  int i;
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    if(eff_bit & (1 << i)){
      act = intsub->act[INTSUB_ACTOR_SENKYO_EFF_0 + i];
      GFL_CLACT_WK_ResetAnm(act);
      GFL_CLACT_WK_SetDrawEnable(act, TRUE);
      GFL_CLACT_WK_SetAutoAnmFlag(act, TRUE);
    }
  }
}

//--------------------------------------------------------------
/**
 * �苒�G�t�F�N�g�I���Ď�
 *
 * @param   intsub		
 */
//--------------------------------------------------------------
static void _IntSub_SenkyoEff_EndWatch(INTRUDE_SUBDISP_PTR intsub)
{
  int i;
  GFL_CLWK *act;

  if(intsub->senkyo_eff_bit > 0){
    for(i = 0; i < INTRUDE_TOWN_MAX; i++){
      if(intsub->senkyo_eff_bit & (1 << i)){
        act = intsub->act[INTSUB_ACTOR_TOWN_0 + i];
        if(GFL_CLACT_WK_CheckAnmActive( act ) == FALSE){
          GFL_CLACT_WK_SetDrawEnable(act, FALSE);
          GFL_CLACT_WK_SetAutoAnmFlag(act, FALSE);
          intsub->senkyo_eff_bit ^= (1 << i);
        }
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���̕\�������݂̃p�����[�^�ōX�V
 *
 * @param   intsub		
 * @param   town_tblno		
 */
//--------------------------------------------------------------
static void _IntSub_TownActChange(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, int town_tblno)
{
  GFL_CLWK *act = intsub->act[INTSUB_ACTOR_TOWN_0 + town_tblno];
  
  if(area_occupy->town.town_occupy[town_tblno] == OCCUPY_TOWN_WHITE){
    GFL_CLACT_WK_SetAnmSeqDiff(act, PALACE_ACT_ANMSEQ_TOWN_W);
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
  }
  else if(area_occupy->town.town_occupy[town_tblno] == OCCUPY_TOWN_BLACK){
    GFL_CLACT_WK_SetAnmSeqDiff(act, PALACE_ACT_ANMSEQ_TOWN_B);
    GFL_CLACT_WK_SetDrawEnable(act, TRUE);
  }
  else{
    GFL_CLACT_WK_SetDrawEnable(act, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���̕\�������݂̃p�����[�^�ōX�V(bit�w��)
 *
 * @param   intsub		
 * @param   change_bit		
 */
//--------------------------------------------------------------
static void _IntSub_TownActBitChange(INTRUDE_SUBDISP_PTR intsub, OCCUPY_INFO *area_occupy, u32 change_bit)
{
  int i;
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    if(change_bit & (1 << i)){
      _IntSub_TownActChange(intsub, area_occupy, i);
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
  int bg_pal;
  
  bg_pal = intcomm->intrude_status_mine.palace_area;
  if(bg_pal == intsub->now_bg_pal){
    return;
  }
  
  GFL_BG_ChangeScreenPalette(
    INTRUDE_FRAME_S_BACKGROUND, 0, 0, 32, 0x13, INTSUB_BG_PAL_BASE_START + bg_pal);
  GFL_BG_LoadScreenV_Req(INTRUDE_FRAME_S_BACKGROUND);
  intsub->now_bg_pal = bg_pal;
}
