//==============================================================================
/**
 * @file    union_subdisp.c
 * @brief   ���j�I�������
 * @author  matsuda
 * @date    2009.07.27(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_subdisp.h"
#include "unionroom.naix"
#include "system\pms_data.h"
#include "print\printsys.h"
#include "font/font.naix"
#include "print\str_tool.h"
#include "union_local.h"
#include "union_chat.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "field/fieldmap.h"
#include "system/pms_draw.h"
#include "sound/pm_sndsys.h"
#include "union_snd_def.h"
#include "msg/msg_union_sub_disp.h"
#include "message.naix"


//==============================================================================
//  �萔��`
//==============================================================================
enum{
  UNISUB_ACTOR_APPEAL_CHAT,
  UNISUB_ACTOR_APPEAL_BATTLE,
  UNISUB_ACTOR_APPEAL_TRADE,
  UNISUB_ACTOR_APPEAL_PICTURE,
  UNISUB_ACTOR_APPEAL_GURUGURU,
  UNISUB_ACTOR_APPEAL_SCROLL,
  
  UNISUB_ACTOR_MAX,
  
  UNISUB_DECOME_NUM = 6,
};

///�A�N�^�[�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_APPEAL_ICON = 10,
};

///�A�N�^�[BG�v���C�I���e�B
enum{
  BGPRI_APPEAL_ICON = 0,
};

///�T�u���BG�t���[����`
enum{
  UNION_FRAME_S_NULL = GFL_BG_FRAME0_S,           ///<���g�p��
  UNION_FRAME_S_MESSAGE = GFL_BG_FRAME1_S,        ///<������
  UNION_FRAME_S_PLATE = GFL_BG_FRAME2_S,          ///<�v���[�g��
  UNION_FRAME_S_BACKGROUND = GFL_BG_FRAME3_S,     ///<�w�i��
};

///���j���[�o�[�ւ̃X�N���[���I�t�Z�b�g(2byte * 32char * )
#define MENUBAR_SCRN_START      (2*32*(192/8-3))
///���j���[�o�[�ւ̃X�N���[���T�C�Y
#define MENUBAR_SCRN_SIZE       (3*32*2)

///�`���b�g���O�p��BMPWIN�T�C�Y
enum{
  UNION_BMPWIN_START_X = 8,     //BMPWIN X�ʒu(dot�P��)
  UNION_BMPWIN_START_Y = 0*8,   //BMPWIN Y�ʒu(dot�P��)
  UNION_BMPWIN_SIZE_X = 29*8,   //BMPWIN X�T�C�Y(dot�P��)
  UNION_BMPWIN_SIZE_Y = 7*8,    //BMPWIN Y�T�C�Y(dot�P��)
};

///�v���[�g�̃T�C�Y
enum{
  UNION_PLATE_START_X = 0,      ///<�v���[�g�̃X�N���[���J�n�ʒuX(�L�����P��)
  UNION_PLATE_START_Y = 0,      ///<�v���[�g�̃X�N���[���J�n�ʒuY(�L�����P��)
  UNION_PLATE_SIZE_X = 32,      ///<�v���[�g�̃X�N���[���T�C�YX(�L�����P��)
  UNION_PLATE_SIZE_Y = 7,       ///<�v���[�g�̃X�N���[���T�C�YY(�L�����P��)
  UNION_PLATE_SCREEN_SIZE = UNION_PLATE_SIZE_X * UNION_PLATE_SIZE_Y * 2,  ///�v���[�g��l���̃X�N���[���T�C�Y
};

///�v���[�g�̃^�b�`�͈�
enum{
  PLATE_LEFT = 0,
  PLATE_RIGHT = 0x1d*8,
  PLATE_TOP = 0*8,
  PLATE_BOTTOM = PLATE_TOP + UNION_PLATE_SIZE_Y*8,
  
  PLATE_HEIGHT = PLATE_BOTTOM - PLATE_TOP,
};

///�^�b�`��A�v���[�g�̐F���ς���Ă��鎞��
#define UNION_PLATE_TOUCH_LIFE      (90)

typedef enum{
  PLATE_COLOR_NORMAL,    ///<�m�[�}���F(�^�b�`���Ă��Ȃ����)
  PLATE_COLOR_TOUCH,     ///<�^�b�`�F
}PLATE_COLOR;

///�T�uBG��ʂ̃p���b�g�\��
enum{
  UNION_SUBBG_PAL_MALE,             ///<�j�v���[�g
  UNION_SUBBG_PAL_MALE_TOUCH,       ///<�j�v���[�g(�^�b�`��)
  UNION_SUBBG_PAL_FEMALE,           ///<���v���[�g
  UNION_SUBBG_PAL_FEMALE_TOUCH,     ///<���v���[�g(�^�b�`��)
  UNION_SUBBG_PAL_BACKGROUND,       ///<�w�i
  
  UNION_SUBBG_PAL_MENU_BAR = 0xd,
//  UNION_SUBBG_PAL_INFOWIN = 0xe,
  UNION_SUBBG_PAL_FONT = 0xf,       ///<�t�H���g
};

///OBJ�̃p���b�g�\��
enum{
  UNION_SUBOBJ_PAL_COMMON = 0,
  
  UNION_SUBOBJ_PAL_PMSWORD_START = 9,
  UNION_SUBOBJ_PAL_PMSWORD_END = 13,
};

///��x�ɉ�ʂɕ\���ł��郍�O����
#define UNION_CHAT_VIEW_LOG_NUM   (3)

///wb_union_icon.nce�̃A�j���[�V�����V�[�P���X
enum{
  UNION_ACT_ANMSEQ_CHAT,
  UNION_ACT_ANMSEQ_BATTLE,
  UNION_ACT_ANMSEQ_TRADE,
  UNION_ACT_ANMSEQ_PICTURE,
  UNION_ACT_ANMSEQ_GURUGURU,
  UNION_ACT_ANMSEQ_SCROLL,
};

///�X�N���[���o�[�̍��W
enum{
  ACT_SCROLL_BAR_X = 256-12,
  ACT_SCROLL_BAR_Y_TOP = 4*8-4,
  ACT_SCROLL_BAR_Y_BOTTOM = 192-5*8+4,
  ACT_SCROLL_BAR_Y_OFFSET = ACT_SCROLL_BAR_Y_BOTTOM - ACT_SCROLL_BAR_Y_TOP,
  ACT_SCROLL_BAR_WIDTH = 22,
  ACT_SCROLL_BAR_HEIGHT = 22,

  ///�X�N���[���o�[�G���A�͈̔�
  SCROLL_AREA_LEFT = ACT_SCROLL_BAR_X - ACT_SCROLL_BAR_WIDTH/2,
  SCROLL_AREA_RIGHT = ACT_SCROLL_BAR_X + ACT_SCROLL_BAR_WIDTH/2,
  SCROLL_AREA_TOP = ACT_SCROLL_BAR_Y_TOP,
  SCROLL_AREA_BOTTOM = ACT_SCROLL_BAR_Y_BOTTOM,
};

///���j���[�o�[��CGX�T�C�Y
#define MENUBAR_CGX_SIZE    (4*0x20)

///�`���b�g�̃t�H���g�F
#define _CHAT_FONT_LSB      (PRINTSYS_MACRO_LSB(1,2,0))

typedef enum{
  _PRINT_TYPE_PMS,    ///<�ȈՉ�b
  _PRINT_TYPE_MSG,    ///<�ʏ탁�b�Z�[�W
}_PRINT_TYPE;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�v���[�g�^�b�`�Ǘ����[�N
typedef struct{
  u16 life;      ///<�v���[�g�̃^�b�`��̐F�ύX����
  u8 view_no;
  u8 padding;
}PLATE_TOUCH_WORK;

///���j�I������ʐ��䃏�[�N
typedef struct _UNION_SUBDISP{
  GAMESYS_WORK *gsys;
  
  u32 alloc_menubar_pos;
  
  u32 index_cgr;
  u32 index_pltt;
  u32 index_cell;

	GFL_CLUNIT *clunit;
  GFL_CLWK *act[UNISUB_ACTOR_MAX];
  
  u8 appeal_no;    ///<�I�𒆂̃A�s�[���ԍ�
  u8 scrollbar_touch;   ///<TRUE:�X�N���[���o�[�^�b�`��
  u8 padding[2];
  
  PLATE_TOUCH_WORK plate_touch[UNION_CHAT_VIEW_LOG_NUM];
  
  PRINT_QUE *printQue;
  GFL_FONT *font_handle;
  GFL_MSGDATA *msgdata;
  GFL_BMPWIN *bmpwin_chat[UNION_CHAT_VIEW_LOG_NUM];
  _PRINT_TYPE print_type[UNION_CHAT_VIEW_LOG_NUM];
  PRINT_UTIL print_util[UNION_CHAT_VIEW_LOG_NUM];
  PMS_DRAW_WORK *pmsdraw;   ///<�ȈՉ�b�\���V�X�e��
}UNION_SUBDISP;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _UniSub_SystemSetup(UNION_SUBDISP_PTR unisub);
static void _UniSub_SystemExit(UNION_SUBDISP_PTR unisub);
static void _UniSub_BGLoad(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle);
static void _UniSub_BGUnload(UNION_SUBDISP_PTR unisub);
static void _UniSub_ActorResouceLoad(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle);
static void _UniSub_ActorResourceUnload(UNION_SUBDISP_PTR unisub);
static void _UniSub_ActorCreate(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle);
static void _UniSub_ActorDelete(UNION_SUBDISP_PTR unisub);
static void _UniSub_MenuBarLoad(UNION_SUBDISP_PTR unisub);
static void _UniSub_MenuBarFree(UNION_SUBDISP_PTR unisub);
static void _UniSub_BmpWinCreate(UNION_SUBDISP_PTR unisub);
static void _UniSub_BmpWinDelete(UNION_SUBDISP_PTR unisub);
static BOOL _UniSub_TouchUpdate(UNION_SUBDISP_PTR unisub);
static void _UniSub_IconPalChange(UNION_SUBDISP_PTR unisub, int act_index);
static void _UniSub_PrintChatUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static void _UniSub_Chat_DispWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_DATA *chat, u8 write_pos);
static void _UniSub_Chat_DispAllWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static void _UniSub_Chat_DispCopy(UNION_SUBDISP_PTR unisub, u8 src_pos, u8 dest_pos, UNION_CHAT_LOG *log);
static void _UniSub_Chat_DispScroll(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, s32 offset);
static BOOL _UniSub_ScrollBar_TouchCheck(UNION_SUBDISP_PTR unisub);
static void _UniSub_ScrollBar_Update(UNION_SYSTEM_PTR unisys, UNION_SUBDISP_PTR unisub);
static void _UniSub_ScrollBar_PosUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static void _UniSub_ScrollBar_ViewPosUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static u32 _UniSub_ScrollBar_GetPageMax(UNION_CHAT_LOG *log);
static u32 _UniSub_ScrollBar_GetPageY(UNION_CHAT_LOG *log, u32 page_max);
static BOOL _UniSub_ScrollArea_TouchCheck(UNION_SUBDISP_PTR unisub);
static BOOL _UniSub_ChatPlate_TouchCheck(UNION_SYSTEM_PTR unisys, UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static PLATE_TOUCH_WORK * _UniSub_GetPlateTouchPtr(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, int view_no);
static BOOL _UniSub_EntryPlateTouchWork(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, int view_no);

static void _UniSub_ChatPlate_Update(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static BOOL _UniSub_ChatPlate_ChangeColor(UNION_SUBDISP_PTR unisub, int plate_no, PLATE_COLOR color);


//==============================================================================
//  �f�[�^
//==============================================================================
///�{�^���ƑΉ�����play_category
ALIGN4 static const u8 AppealNo_CategoryTbl[] = {
  UNION_APPEAL_NULL,        ///<�`���b�g
  UNION_APPEAL_BATTLE,      ///<�퓬
  UNION_APPEAL_TRADE,       ///<����
  UNION_APPEAL_PICTURE,     ///<���G����
  UNION_APPEAL_GURUGURU,    ///<���邮�����
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ���j�I������ʁF�����ݒ�
 *
 * @param   gsys		
 *
 * @retval  UNION_SUBDISP_PTR		
 */
//==================================================================
UNION_SUBDISP_PTR UNION_SUBDISP_Init(GAMESYS_WORK *gsys)
{
  UNION_SUBDISP_PTR unisub;
  ARCHANDLE *handle;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);

  unisub = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(UNION_SUBDISP));
  unisub->gsys = gsys;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_UNION, HEAPID_FIELDMAP);
  
  _UniSub_SystemSetup(unisub);
  _UniSub_BGLoad(unisub, handle);
  _UniSub_BmpWinCreate(unisub);
  _UniSub_ActorResouceLoad(unisub, handle);
  _UniSub_ActorCreate(unisub, handle);
  _UniSub_MenuBarLoad(unisub);
  GFL_ARC_CloseDataHandle(handle);

  unisub->pmsdraw = PMS_DRAW_Init(unisub->clunit, CLSYS_DRAW_SUB, 
    unisub->printQue, unisub->font_handle, UNION_SUBOBJ_PAL_PMSWORD_START, 
    UNION_CHAT_VIEW_LOG_NUM, HEAPID_FIELDMAP);
  PMS_DRAW_SetNullColorPallet( unisub->pmsdraw, 0 );
  
  {//���j���[�����ʕ��A�̏ꍇ�p�ɑS�̕`��
    UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
    if(unisys != NULL){
      _UniSub_Chat_DispAllWrite(unisub, &unisys->chat_log);
    }
  }
  
  //OBJWINDOW(�ʐM�A�C�R��) �̒�����Blend�ŋP�x�������Ȃ��悤�ɂ���
  GFL_NET_WirelessIconOBJWinON();
  G2S_SetWndOBJInsidePlane(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, FALSE);
  G2S_SetWndOutsidePlane(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, TRUE);
	GXS_SetVisibleWnd(GX_WNDMASK_OW);
  
  return unisub;
}

//==================================================================
/**
 * ���j�I������ʁF�j���ݒ�
 *
 * @param   unisub		
 */
//==================================================================
void UNION_SUBDISP_Exit(UNION_SUBDISP_PTR unisub)
{
  PMS_DRAW_Exit(unisub->pmsdraw);
  _UniSub_MenuBarFree(unisub);
  _UniSub_ActorDelete(unisub);
  _UniSub_ActorResourceUnload(unisub);
  _UniSub_BmpWinDelete(unisub);
  _UniSub_BGUnload(unisub);
  _UniSub_SystemExit(unisub);

  GFL_HEAP_FreeMemory(unisub);
  G2S_BlendNone();
  
  //OBJWINDOW(�ʐM�A�C�R��) �̒�����Blend�ŋP�x�������Ȃ��悤�ɂ���
  GFL_NET_WirelessIconOBJWinOFF();
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
}

//==================================================================
/**
 * ���j�I������ʁF�X�V
 *
 * @param   unisub		
 * @param   bActive   TRUE:����ʃA�N�e�B�u���
 *                    FALSE:��A�N�e�B�u(���̃C�x���g���F������󂯕t���Ă͂����Ȃ�)
 */
//==================================================================
void UNION_SUBDISP_Update(UNION_SUBDISP_PTR unisub, BOOL bActive)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(unisub->gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  int i;
  
  GFL_NET_WirelessIconOBJWinON();
	if(bActive == TRUE){
  	G2S_BlendNone();
  }
  else{
    G2S_SetBlendBrightness(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ |
      GX_BLEND_PLANEMASK_BD, -FIELD_NONE_ACTIVE_EVY);
  }

	PRINTSYS_QUE_Main(unisub->printQue);
  PMS_DRAW_Main(unisub->pmsdraw);
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(unisub->print_type[i] == _PRINT_TYPE_MSG){
      PRINT_UTIL_Trans( &unisub->print_util[i], unisub->printQue );
    }
  }
  
  //�`���b�g����
  if(unisys != NULL){
    UNION_MY_SITUATION *situ = &unisys->my_situation;
    if(situ->chat_upload == TRUE){
      UnionChat_AddChat(unisys, NULL, &situ->chat_pmsdata, situ->chat_type);
      situ->chat_upload = FALSE;
    }
  #if PM_DEBUG
    else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_L){
//      PMSDAT_SetDebugRandom(&situ->chat_pmsdata);
      PMSDAT_SetDebugRandomDeco( &situ->chat_pmsdata, HEAPID_FIELDMAP );
      UnionChat_AddChat(unisys, NULL, &situ->chat_pmsdata, UNION_CHAT_TYPE_NORMAL);
    }
  #endif
    _UniSub_PrintChatUpdate(unisub, &unisys->chat_log);
    if(unisys->chat_log.chat_log_count >= UNION_CHAT_VIEW_LOG_NUM){
      GFL_CLACT_WK_SetDrawEnable(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], TRUE);
    }
  }
  
  if(bActive == TRUE){
    u32 icon_update;
    
    icon_update = _UniSub_TouchUpdate(unisub);
    if(unisys != NULL && icon_update == TRUE){
      switch(unisub->appeal_no){
      case UNION_APPEAL_BATTLE:      //�퓬
        UnionChat_SetRecruit(unisys, UNION_CHAT_TYPE_RECRUIT_BATTLE);
        break;
      case UNION_APPEAL_TRADE:       //����
        UnionChat_SetRecruit(unisys, UNION_CHAT_TYPE_RECRUIT_TRADE);
        break;
      case UNION_APPEAL_PICTURE:     //���G����
        UnionChat_SetRecruit(unisys, UNION_CHAT_TYPE_RECRUIT_PICTURE);
        break;
      case UNION_APPEAL_GURUGURU:    //���邮�����
        UnionChat_SetRecruit(unisys, UNION_CHAT_TYPE_RECRUIT_GURUGURU);
        break;
      }
    }

    unisub->scrollbar_touch = _UniSub_ScrollBar_TouchCheck(unisub);
    if(unisub->scrollbar_touch == FALSE){
      unisub->scrollbar_touch = _UniSub_ScrollArea_TouchCheck(unisub);
    }
  }
  
  if(unisys != NULL){
    _UniSub_ScrollBar_Update(unisys, unisub);
    if(bActive == TRUE){
      _UniSub_ChatPlate_TouchCheck(unisys, unisub, &unisys->chat_log);
    }
    _UniSub_ChatPlate_Update(unisub, &unisys->chat_log);
  }
  
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_UNION){
    if(unisys != NULL){
      UnionMain_SetAppealNo(unisys, unisub->appeal_no);
    }
  }
}

//==================================================================
/**
 * ���j�I������ʁF�`��
 *
 * @param   unisub		
 * @param   bActive   TRUE:����ʃA�N�e�B�u���
 *                    FALSE:��A�N�e�B�u(���̃C�x���g���F������󂯕t���Ă͂����Ȃ�)
 */
//==================================================================
void UNION_SUBDISP_Draw(UNION_SUBDISP_PTR unisub, BOOL bActive)
{
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̃Z�b�g�A�b�v
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_SystemSetup(UNION_SUBDISP_PTR unisub)
{
  unisub->clunit = GFL_CLACT_UNIT_Create(UNISUB_ACTOR_MAX + UNISUB_DECOME_NUM, 5, HEAPID_FIELDMAP);
  unisub->printQue = PRINTSYS_QUE_Create(HEAPID_FIELDMAP);
  unisub->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_FIELDMAP );
  unisub->msgdata = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, 
    ARCID_MESSAGE, NARC_message_union_sub_disp_dat, HEAPID_FIELDMAP );
}

//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̔j��
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_SystemExit(UNION_SUBDISP_PTR unisub)
{
  GFL_MSG_Delete(unisub->msgdata);
  GFL_FONT_Delete(unisub->font_handle);
  PRINTSYS_QUE_Delete(unisub->printQue);
  GFL_CLACT_UNIT_Delete(unisub->clunit);
}

//--------------------------------------------------------------
/**
 * BG���[�h
 *
 * @param   unisub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _UniSub_BGLoad(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle)
{
	static const GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
  	{//UNION_FRAME_S_MESSAGE
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6800,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
  	{//UNION_FRAME_S_PLATE
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x6800,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
  	{//UNION_FRAME_S_BACKGROUND
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x6800,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
	};
	GFL_BG_SetBGControl( UNION_FRAME_S_MESSAGE, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( UNION_FRAME_S_PLATE, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( UNION_FRAME_S_BACKGROUND, &TextBgCntDat[2], GFL_BG_MODE_TEXT );

	//BG VRAM�N���A
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);

	//���W�X�^OFF
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2S_BlendNone();
	
  //�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    handle, NARC_unionroom_wb_union_plate_NCGR, UNION_FRAME_S_BACKGROUND, 0, 
    0, FALSE, HEAPID_FIELDMAP);
  
  //�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_unionroom_wb_unionbg_NSCR, UNION_FRAME_S_BACKGROUND, 0, 
    0x800, FALSE, HEAPID_FIELDMAP);
  //�v���[�g�̃X�N���[��(VRAM�ɂ͓W�J�����X�N���[���o�b�t�@�ɂ̂�1���̃v���[�g������]��
  {
    u16 *scrn_buf, *load_buf;
    NNSG2dScreenData *scrnData;
    
    scrn_buf = GFL_BG_GetScreenBufferAdrs(UNION_FRAME_S_PLATE);
    load_buf = GFL_ARCHDL_UTIL_LoadScreen(
      handle, NARC_unionroom_wb_unionbg_plate_NSCR, FALSE, &scrnData, HEAPID_FIELDMAP);
    GFL_STD_MemCopy(scrnData->rawData, scrn_buf, 
      UNION_PLATE_SIZE_X * UNION_PLATE_SIZE_Y * 2);
    GFL_HEAP_FreeMemory(load_buf);
  }
  
  //�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_unionroom_wb_unionbg_NCLR, PALTYPE_SUB_BG, 0, 
    0x20*5, HEAPID_FIELDMAP);
  //�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
    UNION_SUBBG_PAL_FONT * 0x20, 0x20, HEAPID_FIELDMAP);

	GFL_BG_SetVisible( UNION_FRAME_S_NULL, VISIBLE_OFF );
	GFL_BG_SetVisible( UNION_FRAME_S_BACKGROUND, VISIBLE_ON );
	GFL_BG_SetVisible( UNION_FRAME_S_PLATE, VISIBLE_ON );
	GFL_BG_SetVisible( UNION_FRAME_S_MESSAGE, VISIBLE_ON );
}

//--------------------------------------------------------------
/**
 * BG�A�����[�h
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_BGUnload(UNION_SUBDISP_PTR unisub)
{
	GFL_BG_SetVisible( UNION_FRAME_S_MESSAGE, VISIBLE_OFF );
	GFL_BG_SetVisible( UNION_FRAME_S_PLATE, VISIBLE_OFF );
	GFL_BG_SetVisible( UNION_FRAME_S_BACKGROUND, VISIBLE_OFF );
	GFL_BG_FreeBGControl(UNION_FRAME_S_MESSAGE);
	GFL_BG_FreeBGControl(UNION_FRAME_S_PLATE);
	GFL_BG_FreeBGControl(UNION_FRAME_S_BACKGROUND);
}

//--------------------------------------------------------------
/**
 * ���j���[�o�[�̃��[�h
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_MenuBarLoad(UNION_SUBDISP_PTR unisub)
{
  ARCHANDLE *app_handle;
  u32 menu_charno;

	app_handle = GFL_ARC_OpenDataHandle(ARCID_APP_MENU_COMMON, HEAPID_FIELDMAP);

  menu_charno = GFL_BG_AllocCharacterArea(
    UNION_FRAME_S_MESSAGE, MENUBAR_CGX_SIZE, GFL_BG_CHRAREA_GET_B );
  unisub->alloc_menubar_pos = menu_charno;

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    app_handle, NARC_app_menu_common_menu_bar_NCGR, UNION_FRAME_S_MESSAGE, menu_charno, 
    MENUBAR_CGX_SIZE, FALSE, HEAPID_FIELDMAP);

  //���j���[�o�[�̃X�N���[��
  {
    u16 *scrn_buf, *load_buf, *read_buf;
    NNSG2dScreenData *scrnData;
    int i;
    
    scrn_buf = GFL_BG_GetScreenBufferAdrs(UNION_FRAME_S_MESSAGE);
    load_buf = GFL_ARCHDL_UTIL_LoadScreen(
      app_handle, NARC_app_menu_common_menu_bar_NSCR, FALSE, &scrnData, HEAPID_FIELDMAP);
    read_buf = (u16*)scrnData->rawData;
    GFL_STD_MemCopy(&read_buf[MENUBAR_SCRN_START/2], 
      &scrn_buf[MENUBAR_SCRN_START/2], MENUBAR_SCRN_SIZE);
    GFL_HEAP_FreeMemory(load_buf);
    for(i = 0; i < MENUBAR_SCRN_SIZE/2; i++){
      scrn_buf[MENUBAR_SCRN_START/2 + i] = (scrn_buf[MENUBAR_SCRN_START/2 + i] & 0x0fff) + menu_charno + (UNION_SUBBG_PAL_MENU_BAR << 12);
    }
  }

  GFL_ARCHDL_UTIL_TransVramPalette(app_handle, NARC_app_menu_common_menu_bar_NCLR, 
    PALTYPE_SUB_BG, UNION_SUBBG_PAL_MENU_BAR*0x20, 0x20*1, HEAPID_FIELDMAP);

	GFL_ARC_CloseDataHandle(app_handle);
}

//--------------------------------------------------------------
/**
 * ���j���[�o�[�̉��
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_MenuBarFree(UNION_SUBDISP_PTR unisub)
{
  GFL_BG_FreeCharacterArea(UNION_FRAME_S_MESSAGE, unisub->alloc_menubar_pos, MENUBAR_CGX_SIZE);
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_BmpWinCreate(UNION_SUBDISP_PTR unisub)
{
  int i;
  
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    unisub->bmpwin_chat[i] = GFL_BMPWIN_Create(UNION_FRAME_S_MESSAGE, 
      UNION_BMPWIN_START_X / 8, (UNION_BMPWIN_START_Y + UNION_BMPWIN_SIZE_Y * i) / 8, 
      UNION_BMPWIN_SIZE_X / 8, UNION_BMPWIN_SIZE_Y / 8, 
      UNION_SUBBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B);
    PRINT_UTIL_Setup( &unisub->print_util[i], unisub->bmpwin_chat[i] );
  }
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_BmpWinDelete(UNION_SUBDISP_PTR unisub)
{
  int i;
  
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    GFL_BMPWIN_Delete(unisub->bmpwin_chat[i]);
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p���郊�\�[�X�̃��[�h
 *
 * @param   unisub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _UniSub_ActorResouceLoad(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle)
{
  //OBJ���ʃp���b�g
  unisub->index_pltt = GFL_CLGRP_PLTT_RegisterEx(
    handle, NARC_unionroom_wb_unionobj_NCLR, CLSYS_DRAW_SUB, 0, 0, 3, HEAPID_FIELDMAP );

  //�A�s�[���A�C�R��
  unisub->index_cgr = GFL_CLGRP_CGR_Register(
    handle, NARC_unionroom_wb_union_icon_NCGR, FALSE, CLSYS_DRAW_SUB, HEAPID_FIELDMAP);
  unisub->index_cell = GFL_CLGRP_CELLANIM_Register(
    handle, NARC_unionroom_wb_union_icon_NCER, 
    NARC_unionroom_wb_union_icon_NANR, HEAPID_FIELDMAP);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p����A�N�^�[�̃A�����[�h
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_ActorResourceUnload(UNION_SUBDISP_PTR unisub)
{
  GFL_CLGRP_PLTT_Release(unisub->index_pltt);
  GFL_CLGRP_CGR_Release(unisub->index_cgr);
  GFL_CLGRP_CELLANIM_Release(unisub->index_cell);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�쐬
 *
 * @param   unisub		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _UniSub_ActorCreate(UNION_SUBDISP_PTR unisub, ARCHANDLE *handle)
{
  int i;
  GFL_CLWK_DATA head = {
  	0, 192 - 12,                   //X, Y���W
  	0,
  	SOFTPRI_APPEAL_ICON,    //�\�t�g�v���C�I���e�B
  	BGPRI_APPEAL_ICON,      //BG�v���C�I���e�B
  };
  
  //�A�s�[���A�C�R��
  for(i = UNISUB_ACTOR_APPEAL_CHAT; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
    head.pos_x = 32 + i * 48;
    head.anmseq = i;
    unisub->act[i] = GFL_CLACT_WK_Create(unisub->clunit, 
      unisub->index_cgr, unisub->index_pltt, unisub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  }
  
  //�X�N���[���o�[
  head.anmseq = UNION_ACT_ANMSEQ_SCROLL;
  head.pos_x = ACT_SCROLL_BAR_X;
  head.pos_y = ACT_SCROLL_BAR_Y_BOTTOM;
  unisub->act[UNISUB_ACTOR_APPEAL_SCROLL] = GFL_CLACT_WK_Create(unisub->clunit, 
      unisub->index_cgr, unisub->index_pltt, unisub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  GFL_CLACT_WK_SetDrawEnable(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], FALSE);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�폜
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_ActorDelete(UNION_SUBDISP_PTR unisub)
{
  int i;
  
  for(i = 0; i < UNISUB_ACTOR_MAX; i++){
    GFL_CLACT_WK_Remove(unisub->act[i]);
  }
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �^�b�`����
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static BOOL _UniSub_TouchUpdate(UNION_SUBDISP_PTR unisub)
{
  u32 x, y;
  int i;
  GFL_CLACTPOS clpos;
  struct{
    s16 top;
    s16 bottom;
    s16 left;
    s16 right;
  }crect;
  
  if(GFL_UI_TP_GetPointTrg(&x, &y) == FALSE){
    return FALSE;
  }
  
  for(i = UNISUB_ACTOR_APPEAL_CHAT; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
    GFL_CLACT_WK_GetPos(unisub->act[i], &clpos, CLSYS_DEFREND_SUB);
    crect.top = clpos.y - 10;
    crect.bottom = clpos.y + 10;
    crect.left = clpos.x - 10;
    crect.right = clpos.x + 10;
    if(crect.left < x && crect.right > x && crect.top < y && crect.bottom > y){
      if(i == UNISUB_ACTOR_APPEAL_CHAT){
        FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(unisub->gsys);
        FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
        
        FIELD_SUBSCREEN_SetAction(subscreen, FIELD_SUBSCREEN_ACTION_UNION_CHAT);
      }
      else{
        _UniSub_IconPalChange(unisub, i);
//        if(unisub->appeal_no != UNION_APPEAL_NULL 
      }
      PMSND_PlaySE(UNION_SE_APPEAL_ICON_TOUCH);
      return TRUE;
    }
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �Ώۂ̃A�C�R���̐F��ς���
 *
 * @param   unisub		
 * @param   act_index		
 */
//--------------------------------------------------------------
static void _UniSub_IconPalChange(UNION_SUBDISP_PTR unisub, int act_index)
{
  int i;
  UNION_APPEAL select_appeal;
  
  select_appeal = AppealNo_CategoryTbl[act_index - UNISUB_ACTOR_APPEAL_CHAT];
  if(select_appeal == unisub->appeal_no){
    unisub->appeal_no = UNION_APPEAL_NULL;
  }
  else{
    unisub->appeal_no = select_appeal;
  }
  
  for(i = UNISUB_ACTOR_APPEAL_CHAT; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
    if(i == act_index && unisub->appeal_no != UNION_APPEAL_NULL){
      GFL_CLACT_WK_SetPlttOffs(unisub->act[i], 1, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
    else{
      GFL_CLACT_WK_SetPlttOffs(unisub->act[i], 0, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
  }
}

//==================================================================
/**
 * �`���b�g���O��ʁF�X�V����
 *
 * @param   unisub		
 * @param   log		    �`���b�g���O�Ǘ��f�[�^�ւ̃|�C���^
 */
//==================================================================
static void _UniSub_PrintChatUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  s32 offset;
  int i;
  
  if(log->old_chat_view_no == log->chat_view_no){
    return;
  }
  
  //PrintQue�ɂ܂��c���Ă���̂�����Ȃ�΍X�V�͂��Ȃ�
  if(PMS_DRAW_IsPrintEnd(unisub->pmsdraw) == FALSE){
    return;
  }
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(unisub->print_type[i] == _PRINT_TYPE_MSG 
        && PRINT_UTIL_Trans( &unisub->print_util[i], unisub->printQue ) == FALSE){
      return;
    }
  }
  
  
  if(log->chat_log_count < UNION_CHAT_VIEW_LOG_NUM){ //�܂��X�N���[�����������Ȃ����
    UNION_CHAT_DATA *chat;
    for(i = log->old_chat_view_no; i <= log->chat_view_no; i++){
      if(i > -1){
        chat = UnionChat_GetReadBuffer(log, i);
        GF_ASSERT(chat != NULL);
        _UniSub_Chat_DispWrite(unisub, chat, i);
      }
    }
  }
  else{
    offset = log->chat_view_no - log->old_chat_view_no;
    if(MATH_ABS(offset) >= UNION_CHAT_VIEW_LOG_NUM){
      _UniSub_Chat_DispAllWrite(unisub, log); //�S�̍ĕ`��
    }
    else{
      _UniSub_Chat_DispScroll(unisub, log, offset);
    }
  }
  
  log->old_chat_view_no = log->chat_view_no;
}

//--------------------------------------------------------------
/**
 * �`���b�g�`��
 *
 * @param   unisub		  
 * @param   chat		    �`���b�g�f�[�^�ւ̃|�C���^
 * @param   write_pos		�o�͈ʒu
 */
//--------------------------------------------------------------
void _UniSub_Chat_DispWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_DATA *chat, u8 write_pos)
{
  u16 *src_plate_screen, *dest_plate_screen;
  int palno;
  
  src_plate_screen = GFL_BG_GetScreenBufferAdrs(UNION_FRAME_S_PLATE);
  dest_plate_screen = src_plate_screen;
  dest_plate_screen += UNION_PLATE_SCREEN_SIZE/2 * write_pos;
  
  palno = (chat->sex == PM_MALE) ? UNION_SUBBG_PAL_MALE : UNION_SUBBG_PAL_FEMALE;
  
  //�v���[�g�̃X�N���[���`��(����VRAM�ɓW�J����Ă���擪�̃v���[�g�X�N���[�����R�s�[)
  GFL_STD_MemCopy(src_plate_screen, dest_plate_screen, UNION_PLATE_SCREEN_SIZE);
  GFL_BG_ChangeScreenPalette(UNION_FRAME_S_PLATE, 
    UNION_PLATE_START_X, UNION_PLATE_START_Y + UNION_PLATE_SIZE_Y * write_pos, 
    UNION_PLATE_SIZE_X, UNION_PLATE_SIZE_Y, palno);
  GFL_BG_LoadScreenV_Req(UNION_FRAME_S_PLATE);
  
  //�����ʕ`��
  {
    STRBUF *buf_name;
    GFL_POINT point = {0, 16+4};
    
    if(unisub->print_type[write_pos] == _PRINT_TYPE_PMS){
      if(PMS_DRAW_IsPrinting(unisub->pmsdraw, write_pos) == TRUE){
        PMS_DRAW_Clear(unisub->pmsdraw, write_pos, TRUE);
      }
    }
    else if(unisub->print_type[write_pos] == _PRINT_TYPE_MSG){
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[write_pos]), 0 );
      GFL_BMPWIN_TransVramCharacter( unisub->bmpwin_chat[write_pos] );
    }
    
    //�{��
    if(chat->chat_type == UNION_CHAT_TYPE_NORMAL){
      //�ȈՉ�b
      PMS_DRAW_PrintOffset(unisub->pmsdraw, unisub->bmpwin_chat[write_pos], 
        &chat->pmsdata, write_pos, &point);
      unisub->print_type[write_pos] = _PRINT_TYPE_PMS;
    }
    else{
      STRBUF *buf_msg;
      
      switch(chat->chat_type){
      case UNION_CHAT_TYPE_UNION_IN:             //�u���j�I�����[���ɂ͂���܂����I�v
        buf_msg = GFL_MSG_CreateString( unisub->msgdata, msg_union_sub_appeal_05 );
        break;
      case UNION_CHAT_TYPE_RECRUIT_BATTLE:       //�o�g����W
        buf_msg = GFL_MSG_CreateString( unisub->msgdata, msg_union_appeal_01 );
        break;
      case UNION_CHAT_TYPE_RECRUIT_TRADE:        //������W
        buf_msg = GFL_MSG_CreateString( unisub->msgdata, msg_union_appeal_02 );
        break;
      case UNION_CHAT_TYPE_RECRUIT_PICTURE:      //���G������W
        buf_msg = GFL_MSG_CreateString( unisub->msgdata, msg_union_appeal_03 );
        break;
      case UNION_CHAT_TYPE_RECRUIT_GURUGURU:     //���邮���W
        buf_msg = GFL_MSG_CreateString( unisub->msgdata, msg_union_appeal_04 );
        break;
      default:
        GF_ASSERT(0);
        return;
      }
    	GFL_BMPWIN_MakeScreen( unisub->bmpwin_chat[write_pos] );
    	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(unisub->bmpwin_chat[write_pos]) );
      PRINT_UTIL_PrintColor( &unisub->print_util[write_pos], unisub->printQue, 0, point.y, 
        buf_msg, unisub->font_handle, _CHAT_FONT_LSB );
      GFL_STR_DeleteBuffer(buf_msg);
      unisub->print_type[write_pos] = _PRINT_TYPE_MSG;
    }

    //���O
    buf_name = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_FIELDMAP);
    GFL_STR_SetStringCode(buf_name, chat->name);
    PRINTSYS_PrintQueColor( unisub->printQue, GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[write_pos]), 
      0, 0, buf_name, unisub->font_handle, _CHAT_FONT_LSB);
    GFL_STR_DeleteBuffer(buf_name);
    
    //�F�B�蒠�̖��O
    if(chat->friend == TRUE){
      STRBUF *buf_friend = GFL_MSG_CreateString( unisub->msgdata, msg_union_sub_friend );
      PRINTSYS_PrintQueColor( unisub->printQue, GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[write_pos]), 
        UNION_BMPWIN_SIZE_X/2, 0, buf_friend, unisub->font_handle, _CHAT_FONT_LSB);
      GFL_STR_DeleteBuffer(buf_friend);
    }
  }
}

//--------------------------------------------------------------
/**
 * ���ʒ��̑S�Ẵ`���b�g��`��
 *
 * @param   unisub		
 * @param   log		    
 */
//--------------------------------------------------------------
void _UniSub_Chat_DispAllWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  int start, end, write_pos, i;
  UNION_CHAT_DATA *chat;
  
  start = log->chat_view_no;
  end = start - UNION_CHAT_VIEW_LOG_NUM;
  if(end < 0){
    end = -1;
  }
  write_pos = start - end - 1;

  for(i = start; i > end; i--){
    chat = UnionChat_GetReadBuffer(log, i);
    GF_ASSERT(chat != NULL);
    GF_ASSERT(write_pos > -1);
    _UniSub_Chat_DispWrite(unisub, chat, write_pos);
    write_pos--;
  }
}

//--------------------------------------------------------------
/**
 * �`�悳��Ă���`���b�gBMP��ʂ�BMP�ɃR�s�[
 *
 * @param   unisub		
 * @param   src_pos		  �R�s�[���`��ʒu
 * @param   dest_pos		�R�s�[��`��ʒu
 */
//--------------------------------------------------------------
static void _UniSub_Chat_DispCopy(UNION_SUBDISP_PTR unisub, u8 src_pos, u8 dest_pos, UNION_CHAT_LOG *log)
{
  u16 *src_plate_screen, *dest_plate_screen;

  //�v���[�g�̃R�s�[
  src_plate_screen = GFL_BG_GetScreenBufferAdrs(UNION_FRAME_S_PLATE);
  dest_plate_screen = src_plate_screen;
  dest_plate_screen += UNION_PLATE_SCREEN_SIZE/2 * dest_pos;
  src_plate_screen += UNION_PLATE_SCREEN_SIZE/2 * src_pos;
  GFL_STD_MemCopy(src_plate_screen, dest_plate_screen, UNION_PLATE_SCREEN_SIZE);
  GFL_BG_LoadScreenV_Req(UNION_FRAME_S_PLATE);
  
  //BMP�̃R�s�[
  if(unisub->print_type[src_pos] == _PRINT_TYPE_PMS 
      && unisub->print_type[dest_pos] == _PRINT_TYPE_PMS){
    PMS_DRAW_Copy(unisub->pmsdraw, src_pos, dest_pos);
  }
  else if(unisub->print_type[src_pos] == _PRINT_TYPE_MSG 
      && unisub->print_type[dest_pos] == _PRINT_TYPE_MSG){
    GFL_BMP_Copy( GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[src_pos]), 
      GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[dest_pos]) );
    GFL_BMPWIN_TransVramCharacter(unisub->bmpwin_chat[dest_pos]);
  }
  else{
    UNION_CHAT_DATA *chat;
    chat = UnionChat_GetReadBuffer(log, log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM-1) + src_pos);
    _UniSub_Chat_DispWrite(unisub, chat, dest_pos);
  }
}

//--------------------------------------------------------------
/**
 * �`���b�g���X�N���[��
 *
 * @param   unisub		
 * @param   log		
 * @param   offset		
 *
 * �R�s�[�ōςނ��̂̓R�s�[�ōς܂���̂�(_UniSub_Chat_DispAllWrite�Ƃ̈Ⴂ)
 */
//--------------------------------------------------------------
static void _UniSub_Chat_DispScroll(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, s32 offset)
{
  int src_pos, dest_pos;
  int i, write_pos;
  UNION_CHAT_DATA *chat;
  
  if(offset > 0){ //���̃��O����Ɉړ����Ă���
    dest_pos = 0;
    src_pos = dest_pos + offset;
    while(src_pos < UNION_CHAT_VIEW_LOG_NUM){
      _UniSub_Chat_DispCopy(unisub, src_pos, dest_pos, log);
      dest_pos++;
      src_pos++;
    }
    
    for(write_pos = UNION_CHAT_VIEW_LOG_NUM - offset; write_pos < UNION_CHAT_VIEW_LOG_NUM; write_pos++)
    {
      chat = UnionChat_GetReadBuffer(log, log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM-1) + write_pos);
      GF_ASSERT(chat != NULL);
      _UniSub_Chat_DispWrite(unisub, chat, write_pos);
    }
  }
  else{           //���O�����Ɉړ����Ă���
    dest_pos = UNION_CHAT_VIEW_LOG_NUM - 1;
    src_pos = dest_pos + offset;
    while(src_pos > -1){
      _UniSub_Chat_DispCopy(unisub, src_pos, dest_pos, log);
      dest_pos--;
      src_pos--;
    }

    for(write_pos = (-offset)-1; write_pos > -1; write_pos--)
    {
      chat = UnionChat_GetReadBuffer(log, log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM - 1) + write_pos);
      GF_ASSERT_MSG(chat != NULL, "view_no=%d, write_pos=%d, offset=%d\n", log->chat_view_no, write_pos, offset);
      _UniSub_Chat_DispWrite(unisub, chat, write_pos);
    }
  }
}

//--------------------------------------------------------------
/**
 * �X�N���[���o�[�^�b�`����
 *
 * @param   unisub		
 *
 * @retval  BOOL		TRUE�F�^�b�`���Ă���@FALSE�F���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL _UniSub_ScrollBar_TouchCheck(UNION_SUBDISP_PTR unisub)
{
  GFL_CLACTPOS bar;
  u32 tp_x, tp_y;
  
  if(GFL_UI_TP_GetPointCont(&tp_x, &tp_y) == FALSE){
    return FALSE;
  }
  
  GFL_CLACT_WK_GetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);

  if(unisub->scrollbar_touch == FALSE){
    if(GFL_UI_TP_GetTrg() == FALSE){
      return FALSE;
    }
    if(bar.x - ACT_SCROLL_BAR_WIDTH/2 <= tp_x && bar.x + ACT_SCROLL_BAR_WIDTH/2 >= tp_x
        && bar.y - ACT_SCROLL_BAR_HEIGHT/2 <= tp_y && bar.y + ACT_SCROLL_BAR_HEIGHT/2 >= tp_y){
      return TRUE;
    }
  }
  else{
    if(tp_y < ACT_SCROLL_BAR_Y_TOP){
      tp_y = ACT_SCROLL_BAR_Y_TOP;
    }
    else if(tp_y > ACT_SCROLL_BAR_Y_BOTTOM){
      tp_y = ACT_SCROLL_BAR_Y_BOTTOM;
    }
    bar.y = tp_y;
    GFL_CLACT_WK_SetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �X�N���[���o�[�X�V����
 *
 * @param   unisys		
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_ScrollBar_Update(UNION_SYSTEM_PTR unisys, UNION_SUBDISP_PTR unisub)
{
  
  if(unisub->scrollbar_touch == TRUE){
    _UniSub_ScrollBar_PosUpdate(unisub, &unisys->chat_log);
  }
  else{
    _UniSub_ScrollBar_ViewPosUpdate(unisub, &unisys->chat_log);
  }
}

//--------------------------------------------------------------
/**
 * �X�N���[���o�[�̍��W�ʒu�ɏ]���ă`���b�g���O��ʂ�View�|�W�V������ύX����
 *
 * @param   unisub		
 * @param   log		
 */
//--------------------------------------------------------------
static void _UniSub_ScrollBar_PosUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  s32 page_y, page_max, half_page_y, view_page;
  GFL_CLACTPOS bar;
  s32 check_y, view_no;
  int i;
  
  GFL_CLACT_WK_GetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);
  check_y = (bar.y - ACT_SCROLL_BAR_Y_TOP) << 8;
  
  page_max = _UniSub_ScrollBar_GetPageMax(log);
  page_y = _UniSub_ScrollBar_GetPageY(log, page_max);
  
  view_page = 0;
  half_page_y = page_y >> 1;
  check_y += half_page_y;
  for(i = 0; i < page_max; i++){
//    if(check_y >= page_y * (view_page+1) - half_page_y
//        && check_y <= page_y * (view_page+1) + half_page_y){
    if(check_y <= page_y * (view_page + 1)){
      break;
    }
    view_page++;
  }
  if(view_page >= page_max){
    //�����Œ[�����o�Ă���ƍŌ�܂�for�����񂵂Ă��Ō���������肩��R���׃t�H���[
    view_page = page_max;
  }
  
  log->chat_view_no = log->chat_log_count - page_max + view_page;
  if(log->chat_view_no < UNION_CHAT_VIEW_LOG_NUM){
    if(log->chat_log_count < UNION_CHAT_VIEW_LOG_NUM){
      log->chat_view_no = log->chat_log_count;
    }
    else{
      log->chat_view_no = UNION_CHAT_VIEW_LOG_NUM - 1;
    }
  }
  
//  OS_TPrintf("ccc page_y = %d, bar.y = %d, page_y8 = %d, log_count = %d, view_no = %d, view_page = %d, page_max = %d, check_y = %d\n", page_y, bar.y, page_y >> 8, log->chat_log_count, log->chat_view_no, view_page, page_max, check_y);
  if(log->chat_view_no < 0 || log->chat_view_no > log->chat_log_count){
    OS_TPrintf("OVER!!! chat_view_no = %d, log_count = %d, view_page = %d\n", 
      log->chat_view_no, log->chat_log_count, view_page);
    log->chat_view_no = log->chat_log_count;
    GF_ASSERT(0);
  }

#if 0
  bar.x = ACT_SCROLL_BAR_X;
  bar.y = page_y;
  GFL_CLACT_WK_SetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);
#endif
}

//--------------------------------------------------------------
/**
 * �`���b�g���O��ʂ�View�|�W�V�����ɍ��킹�ăX�N���[���o�[�̍��W��ύX����
 *
 * @param   unisub		
 * @param   log		
 */
//--------------------------------------------------------------
static void _UniSub_ScrollBar_ViewPosUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  GFL_CLACTPOS bar;
  int log_offset;
  u32 page_y;
  
  page_y = _UniSub_ScrollBar_GetPageY(log, _UniSub_ScrollBar_GetPageMax(log));

  log_offset = log->chat_log_count - log->chat_view_no;
  bar.y = ACT_SCROLL_BAR_Y_BOTTOM - ((page_y * log_offset) >> 8);
  
  bar.x = ACT_SCROLL_BAR_X;
  GFL_CLACT_WK_SetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);
  
//  OS_TPrintf("aaa page_y = %d, bar.y = %d, log_offset = %d, page_y8 = %d, log_count = %d, view_no = %d\n", page_y, bar.y, log_offset, page_y >> 8, log->chat_log_count, log->chat_view_no);
}

//--------------------------------------------------------------
/**
 * �`���b�g���O��ʂ̕\���ł���y�[�W���擾(�ő�l��UNION_CHAT_LOG_MAX�܂�)
 *
 * @param   log		
 *
 * @retval  u32		�\���\�y�[�W��
 */
//--------------------------------------------------------------
static u32 _UniSub_ScrollBar_GetPageMax(UNION_CHAT_LOG *log)
{
  s32 page_max;
  
  if(log->chat_log_count < UNION_CHAT_LOG_MAX){
    page_max = log->chat_log_count;
  }
  else{
    page_max = UNION_CHAT_LOG_MAX - 1;
  }
  
  page_max -= UNION_CHAT_VIEW_LOG_NUM - 1;
  if(page_max < 0){
    page_max = 0;
  }
  return page_max;
}

//--------------------------------------------------------------
/**
 * �`���b�g���O��1�y�[�W�����X�N���[���o�[�̃h�b�g��(Y)���擾����
 *
 * @param   log		
 *
 * @retval  u32		�h�b�g��(�Œ菬���F����8�r�b�g����)
 */
//--------------------------------------------------------------
static u32 _UniSub_ScrollBar_GetPageY(UNION_CHAT_LOG *log, u32 page_max)
{
  u32 page_y;

  page_y = ACT_SCROLL_BAR_Y_OFFSET << 8;
  page_y /= page_max;
  return page_y;
}

//--------------------------------------------------------------
/**
 * �X�N���[���G���A�^�b�`����
 *
 * @param   unisub		
 *
 * @retval  BOOL		TRUE�F�^�b�`�����@FALSE�F���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL _UniSub_ScrollArea_TouchCheck(UNION_SUBDISP_PTR unisub)
{
  GFL_CLACTPOS bar;
  u32 tp_x, tp_y;
  
  if(GFL_UI_TP_GetPointTrg(&tp_x, &tp_y) == FALSE){
    return FALSE;
  }
  if(tp_x >= SCROLL_AREA_LEFT && tp_x <= SCROLL_AREA_RIGHT
      && tp_y >= SCROLL_AREA_TOP && tp_y <= SCROLL_AREA_BOTTOM){
    bar.x = ACT_SCROLL_BAR_X;
    bar.y = tp_y;
    GFL_CLACT_WK_SetPos(unisub->act[UNISUB_ACTOR_APPEAL_SCROLL], &bar, CLSYS_DEFREND_SUB);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �v���[�g�̃^�b�`�`�F�b�N
 *
 * @param   unisub		
 *
 * @retval  BOOL		TRUE:�^�b�`����
 */
//--------------------------------------------------------------
static BOOL _UniSub_ChatPlate_TouchCheck(UNION_SYSTEM_PTR unisys, UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  u32 tp_x, tp_y;
  int y;
  
  if(GFL_UI_TP_GetPointTrg(&tp_x, &tp_y) == FALSE){
    return FALSE;
  }
  
  if(tp_x >= PLATE_LEFT && tp_x <= PLATE_RIGHT){
    for(y = 0; y < UNION_CHAT_VIEW_LOG_NUM; y++){
      if(tp_y >= (PLATE_TOP + PLATE_HEIGHT * y) && tp_y <= (PLATE_BOTTOM + PLATE_HEIGHT * y)){
        break;
      }
    }
  }
  
  if(y >= UNION_CHAT_VIEW_LOG_NUM){
    return FALSE;
  }
  
  if(_UniSub_ChatPlate_ChangeColor(unisub, y, PLATE_COLOR_TOUCH) == TRUE){
    //int view_no = log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM-1) + y;
    int view_no;
    if(log->chat_view_no < UNION_CHAT_VIEW_LOG_NUM){
      view_no = y;
    }
    else{
      view_no = log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM-1) + y;
    }
    _UniSub_EntryPlateTouchWork(unisub, log, view_no);
    {//�^�b�`����������t�H�[�J�X�ΏۂɃZ�b�g����
      UNION_CHAT_DATA *chat = UnionChat_GetReadBuffer(log, view_no);
      if(chat != NULL){
        GFL_STD_MemCopy(chat->mac_address, unisys->my_situation.focus_mac_address, 6);
      }
    }
    PMSND_PlaySE( UNION_SE_PANEL_TOUCH );
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �v���[�g�^�b�`���[�N����󂫂�T��
 *
 * @param   unisub		
 * @param   view_no		
 *
 * @retval  PLATE_TOUCH_WORK *		
 */
//--------------------------------------------------------------
static PLATE_TOUCH_WORK * _UniSub_GetPlateTouchPtr(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, int view_no)
{
  int i;
  
  //���ɃG���g���[�ς݂Ȃ瓯���[�N�̒l���X�V
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(unisub->plate_touch[i].life > 0 && view_no == unisub->plate_touch[i].view_no){
      return &unisub->plate_touch[i];
    }
  }

  //�󂫂�T��
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(unisub->plate_touch[i].life == 0){
      return &unisub->plate_touch[i];
    }
  }
  
  //��ʊO�ɏo�Ă���view_no�̗̈��ׂ��Ďg�p����
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(unisub->plate_touch[i].view_no > log->chat_view_no
        || unisub->plate_touch[i].view_no <= log->chat_view_no - UNION_CHAT_VIEW_LOG_NUM){
      return &unisub->plate_touch[i];
    }
  }
  
  GF_ASSERT(0);
  return NULL;
}

//--------------------------------------------------------------
/**
 * �v���[�g�^�b�`���[�N�ɃG���g���[����
 *
 * @param   unisub		
 * @param   view_no		
 * 
 * @retval  TRUE:�G���g���[�����@FALSE:�G���g���[���s
 */
//--------------------------------------------------------------
static BOOL _UniSub_EntryPlateTouchWork(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, int view_no)
{
  PLATE_TOUCH_WORK *pt;
  
  pt = _UniSub_GetPlateTouchPtr(unisub, log, view_no);
  if(pt == NULL){
    return FALSE; //�ʏ킠�肦�Ȃ��@������ASSERT���d����ł���
  }
  pt->life = UNION_PLATE_TOUCH_LIFE;
  pt->view_no = view_no;
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �v���[�g�̐F�X�V����
 *
 * @param   unisub		
 */
//--------------------------------------------------------------
static void _UniSub_ChatPlate_Update(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log)
{
  int y, write_pos;
  
  for(y = 0; y < UNION_CHAT_VIEW_LOG_NUM; y++){
    if(unisub->plate_touch[y].life > 0){
      unisub->plate_touch[y].life--;
      if(unisub->plate_touch[y].life == 0){
        if(log->chat_view_no < UNION_CHAT_VIEW_LOG_NUM){
          write_pos = unisub->plate_touch[y].view_no;
        }
        else{
          write_pos = (UNION_CHAT_VIEW_LOG_NUM-1) 
            + unisub->plate_touch[y].view_no - log->chat_view_no;
        }
        if(write_pos >= 0 && write_pos < UNION_CHAT_VIEW_LOG_NUM){
          _UniSub_ChatPlate_ChangeColor(unisub, write_pos, PLATE_COLOR_NORMAL);
        }
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �v���[�g�̐F���^�b�`�F���A�m�[�}���F���ύX����
 *
 * @param   unisub		
 * @param   plate_no		�v���[�g�ԍ�
 * @param   color		    �ύX��̐F�w��
 * 
 * @retval  TRUE:�F�ς�����
 */
//--------------------------------------------------------------
static BOOL _UniSub_ChatPlate_ChangeColor(UNION_SUBDISP_PTR unisub, int plate_no, PLATE_COLOR color)
{
  u16 *scrn_buf;
  u16 change_palno, now_palno;
  
  scrn_buf = GFL_BG_GetScreenBufferAdrs(UNION_FRAME_S_PLATE);
  scrn_buf += ((UNION_PLATE_START_X + UNION_PLATE_SIZE_X * UNION_PLATE_SIZE_Y) 
    + (UNION_PLATE_SIZE_X * UNION_PLATE_SIZE_Y) * plate_no) / 2;
  now_palno = (*scrn_buf) >> 12;
  switch(now_palno){
  case UNION_SUBBG_PAL_MALE:
  case UNION_SUBBG_PAL_MALE_TOUCH:
    if(color == PLATE_COLOR_NORMAL){
      change_palno = UNION_SUBBG_PAL_MALE;
    }
    else{
      change_palno = UNION_SUBBG_PAL_MALE_TOUCH;
    }
    break;
  case UNION_SUBBG_PAL_FEMALE:
  case UNION_SUBBG_PAL_FEMALE_TOUCH:
    if(color == PLATE_COLOR_NORMAL){
      change_palno = UNION_SUBBG_PAL_FEMALE;
    }
    else{
      change_palno = UNION_SUBBG_PAL_FEMALE_TOUCH;
    }
    break;
  default:
    return FALSE;
  }
  
  GFL_BG_ChangeScreenPalette(UNION_FRAME_S_PLATE, 
    UNION_PLATE_START_X, UNION_PLATE_START_Y + UNION_PLATE_SIZE_Y * plate_no, 
    UNION_PLATE_SIZE_X, UNION_PLATE_SIZE_Y, change_palno);
  GFL_BG_LoadScreenV_Req(UNION_FRAME_S_PLATE);
  return TRUE;
}
