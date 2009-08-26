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


//==============================================================================
//  �萔��`
//==============================================================================
enum{
  UNISUB_ACTOR_APPEAL_BATTLE,
  UNISUB_ACTOR_APPEAL_TRADE,
  UNISUB_ACTOR_APPEAL_RECORD,
  UNISUB_ACTOR_APPEAL_PICTURE,
  UNISUB_ACTOR_APPEAL_GURUGURU,
  
  UNISUB_ACTOR_MAX,
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

///�C���t�H�o�[�̕��̃X�N���[���I�t�Z�b�g(2byte * 32char * 2��)
#define INFOBAR_SCRN_OFFSET     (2*32*2)

///�`���b�g���O�p��BMPWIN�T�C�Y
enum{
  UNION_BMPWIN_START_X = 8,     //BMPWIN X�ʒu(dot�P��)
  UNION_BMPWIN_START_Y = 8*2,   //BMPWIN Y�ʒu(dot�P��)
  UNION_BMPWIN_SIZE_X = 29*8,   //BMPWIN X�T�C�Y(dot�P��)
  UNION_BMPWIN_SIZE_Y = 6*8,    //BMPWIN Y�T�C�Y(dot�P��)
};

///�v���[�g�̃T�C�Y
enum{
  UNION_PLATE_START_X = 0,      ///<�v���[�g�̃X�N���[���J�n�ʒuX(�L�����P��)
  UNION_PLATE_START_Y = 2,      ///<�v���[�g�̃X�N���[���J�n�ʒuY(�L�����P��)
  UNION_PLATE_SIZE_X = 32,      ///<�v���[�g�̃X�N���[���T�C�YX(�L�����P��)
  UNION_PLATE_SIZE_Y = 6,       ///<�v���[�g�̃X�N���[���T�C�YY(�L�����P��)
  UNION_PLATE_SCREEN_SIZE = UNION_PLATE_SIZE_X * UNION_PLATE_SIZE_Y * 2,  ///�v���[�g��l���̃X�N���[���T�C�Y
};

///�T�uBG��ʂ̃p���b�g�\��
enum{
  UNION_SUBBG_PAL_MALE,             ///<�j�v���[�g
  UNION_SUBBG_PAL_MALE_TOUCH,       ///<�j�v���[�g(�^�b�`��)
  UNION_SUBBG_PAL_FEMALE,           ///<���v���[�g
  UNION_SUBBG_PAL_FEMALE_TOUCH,     ///<���v���[�g(�^�b�`��)
  UNION_SUBBG_PAL_BACKGROUND,       ///<�w�i
  
  UNION_SUBBG_PAL_FONT = 0xf,       ///<�t�H���g
};

///��x�ɉ�ʂɕ\���ł��郍�O����
#define UNION_CHAT_VIEW_LOG_NUM   (3)


//==============================================================================
//  �\���̒�`
//==============================================================================
///���j�I������ʐ��䃏�[�N
typedef struct _UNION_SUBDISP{
  GAMESYS_WORK *gsys;

  u32 index_cgr;
  u32 index_pltt;
  u32 index_cell;

	GFL_CLUNIT *clunit;
  GFL_CLWK *act[UNISUB_ACTOR_MAX];
  
  u8 appeal_no;    ///<�I�𒆂̃A�s�[���ԍ�
  u8 padding[3];
  
  PRINT_QUE *printQue;
  GFL_FONT *font_handle;
  GFL_BMPWIN *bmpwin_chat[UNION_CHAT_VIEW_LOG_NUM];
  PRINT_UTIL print_util[UNION_CHAT_VIEW_LOG_NUM];
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
static void _UniSub_TouchUpdate(UNION_SUBDISP_PTR unisub);
static void _UniSub_IconPalChange(UNION_SUBDISP_PTR unisub, int act_index);
static void _UniSub_BmpWinCreate(UNION_SUBDISP_PTR unisub);
static void _UniSub_BmpWinDelete(UNION_SUBDISP_PTR unisub);
static void _UniSub_TouchUpdate(UNION_SUBDISP_PTR unisub);
static void _UniSub_IconPalChange(UNION_SUBDISP_PTR unisub, int act_index);
static void _UniSub_PrintChatUpdate(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static void _UniSub_Chat_DispWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_DATA *chat, u8 write_pos);
static void _UniSub_Chat_DispAllWrite(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log);
static void _UniSub_Chat_DispCopy(UNION_SUBDISP_PTR unisub, u8 src_pos, u8 dest_pos);
static void _UniSub_Chat_DispScroll(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, s32 offset);


//==============================================================================
//  �f�[�^
//==============================================================================
///�{�^���ƑΉ�����play_category
ALIGN4 static const u8 AppealNo_CategoryTbl[] = {
  UNION_APPEAL_BATTLE,      ///<�퓬
  UNION_APPEAL_TRADE,       ///<����
  UNION_APPEAL_RECORD,      ///<���R�[�h�R�[�i�[
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
  
  unisub = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(UNION_SUBDISP));
  unisub->gsys = gsys;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_UNION, HEAPID_FIELDMAP);
  
  _UniSub_SystemSetup(unisub);
  _UniSub_BGLoad(unisub, handle);
  _UniSub_BmpWinCreate(unisub);
  _UniSub_ActorResouceLoad(unisub, handle);
  _UniSub_ActorCreate(unisub, handle);
  
  GFL_ARC_CloseDataHandle(handle);

  {//���j���[�����ʕ��A�̏ꍇ�p�ɑS�̕`��
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
    if(unisys != NULL){
      _UniSub_Chat_DispAllWrite(unisub, &unisys->chat_log);
    }
  }
  
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
  _UniSub_ActorDelete(unisub);
  _UniSub_ActorResourceUnload(unisub);
  _UniSub_BmpWinDelete(unisub);
  _UniSub_BGUnload(unisub);
  _UniSub_SystemExit(unisub);

  GFL_HEAP_FreeMemory(unisub);
}

//==================================================================
/**
 * ���j�I������ʁF�X�V
 *
 * @param   unisub		
 */
//==================================================================
void UNION_SUBDISP_Update(UNION_SUBDISP_PTR unisub)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(unisub->gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  int i;
  
	PRINTSYS_QUE_Main(unisub->printQue);
	for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
		PRINT_UTIL_Trans(&unisub->print_util[i], unisub->printQue);
  }
  
  //�`���b�g����
  if(unisys != NULL){
    UNION_MY_SITUATION *situ = &unisys->my_situation;
    if(situ->chat_upload == TRUE){
      UnionChat_AddChat(unisys, NULL, &situ->chat_pmsdata);
      situ->chat_upload = FALSE;
    }
    _UniSub_PrintChatUpdate(unisub, &unisys->chat_log);
  }
  
  _UniSub_TouchUpdate(unisub);
  
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
 */
//==================================================================
void UNION_SUBDISP_Draw(UNION_SUBDISP_PTR unisub)
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
  unisub->clunit = GFL_CLACT_UNIT_Create(UNISUB_ACTOR_MAX, 5, HEAPID_FIELDMAP);
  unisub->printQue = PRINTSYS_QUE_Create(HEAPID_FIELDMAP);
  unisub->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_FIELDMAP );
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
    GFL_STD_MemCopy(scrnData->rawData, &scrn_buf[INFOBAR_SCRN_OFFSET/2], 
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
    PRINT_UTIL_Setup(&unisub->print_util[i], unisub->bmpwin_chat[i]);
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
  	0, 192 - 16,                   //X, Y���W
  	0,
  	SOFTPRI_APPEAL_ICON,    //�\�t�g�v���C�I���e�B
  	BGPRI_APPEAL_ICON,      //BG�v���C�I���e�B
  };
  
  for(i = UNISUB_ACTOR_APPEAL_BATTLE; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
    head.pos_x = 32 + i * 48;
    head.anmseq = i;
    unisub->act[i] = GFL_CLACT_WK_Create(unisub->clunit, 
      unisub->index_cgr, unisub->index_pltt, unisub->index_cell, 
      &head, CLSYS_DEFREND_SUB, HEAPID_FIELDMAP);
  }
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
static void _UniSub_TouchUpdate(UNION_SUBDISP_PTR unisub)
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
    return;
  }
  
  for(i = UNISUB_ACTOR_APPEAL_BATTLE; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
    GFL_CLACT_WK_GetPos(unisub->act[i], &clpos, CLSYS_DEFREND_SUB);
    crect.top = clpos.y - 16;
    crect.bottom = clpos.y + 16;
    crect.left = clpos.x - 16;
    crect.right = clpos.x + 16;
    if(crect.left < x && crect.right > x && crect.top < y && crect.bottom > y){
      _UniSub_IconPalChange(unisub, i);
      return;
    }
  }
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
  
  select_appeal = AppealNo_CategoryTbl[act_index - UNISUB_ACTOR_APPEAL_BATTLE];
  if(select_appeal == unisub->appeal_no){
    unisub->appeal_no = UNION_APPEAL_NULL;
  }
  else{
    unisub->appeal_no = select_appeal;
  }
  
  for(i = UNISUB_ACTOR_APPEAL_BATTLE; i <= UNISUB_ACTOR_APPEAL_GURUGURU; i++){
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
  for(i = 0; i < UNION_CHAT_VIEW_LOG_NUM; i++){
    if(PRINTSYS_QUE_IsExistTarget(unisub->printQue, GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[i])) == TRUE){
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
  src_plate_screen += INFOBAR_SCRN_OFFSET/2; //�C���t�H�o�[�̕��A�h���X��i�߂�
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
    STRBUF *buf_name, *buf_pms;
    
    GFL_BMPWIN_MakeScreen(unisub->bmpwin_chat[write_pos]);
    GFL_BG_LoadScreenV_Req(UNION_FRAME_S_MESSAGE);
    
    //���O
    buf_name = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_FIELDMAP);
    GFL_STR_SetStringCode(buf_name, chat->name);
    PRINT_UTIL_Print(
      &unisub->print_util[write_pos], unisub->printQue, 0, 0, buf_name, unisub->font_handle);
    GFL_STR_DeleteBuffer(buf_name);
    
    //�F�B�蒠�̖��O  ��check�@�܂����쐬 2009.08.24(��)
    
    //�ȈՉ�b
    buf_pms = PMSDAT_ToString(&chat->pmsdata, HEAPID_FIELDMAP);
    PRINT_UTIL_Print(
      &unisub->print_util[write_pos], unisub->printQue, 0, 16, buf_pms, unisub->font_handle);
    GFL_STR_DeleteBuffer(buf_pms);
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
void _UniSub_Chat_DispCopy(UNION_SUBDISP_PTR unisub, u8 src_pos, u8 dest_pos)
{
  GFL_BMP_Copy(GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[src_pos]), 
    GFL_BMPWIN_GetBmp(unisub->bmpwin_chat[dest_pos]));
  GFL_BMPWIN_MakeTransWindow_VBlank(unisub->bmpwin_chat[dest_pos]);
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
void _UniSub_Chat_DispScroll(UNION_SUBDISP_PTR unisub, UNION_CHAT_LOG *log, s32 offset)
{
  int src_pos, dest_pos;
  int i, write_pos;
  UNION_CHAT_DATA *chat;
  
  if(offset > 0){ //���̃��O����Ɉړ����Ă���
    dest_pos = 0;
    src_pos = dest_pos + offset;
    while(src_pos < UNION_CHAT_VIEW_LOG_NUM){
      _UniSub_Chat_DispCopy(unisub, src_pos, dest_pos);
      dest_pos++;
      src_pos++;
    }
    
    for(write_pos = UNION_CHAT_VIEW_LOG_NUM - offset; write_pos < UNION_CHAT_VIEW_LOG_NUM; write_pos++)
    {
      chat = UnionChat_GetReadBuffer(log, log->chat_view_no - offset + write_pos);
      GF_ASSERT(chat != NULL);
      _UniSub_Chat_DispWrite(unisub, chat, write_pos);
    }
  }
  else{           //���O�����Ɉړ����Ă���
    dest_pos = UNION_CHAT_VIEW_LOG_NUM - 1;
    src_pos = dest_pos + offset;
    while(src_pos > -1){
      _UniSub_Chat_DispCopy(unisub, src_pos, dest_pos);
      dest_pos--;
      src_pos--;
    }

    for(write_pos = (-offset)-1; write_pos > -1; write_pos--)
    {
      chat = UnionChat_GetReadBuffer(log, log->chat_view_no - (UNION_CHAT_VIEW_LOG_NUM - 1) - write_pos);
      GF_ASSERT(chat != NULL);
      _UniSub_Chat_DispWrite(unisub, chat, write_pos);
    }
  }
}


