//==============================================================================
/**
 * @file    monolith_main.c
 * @brief   ���m���X�F���C������
 * @author  matsuda
 * @date    2009.11.20(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "arc_def.h"
#include "field/monolith_main.h"
#include "monolith_common.h"
#include "monolith.naix"
#include "system/gfl_use.h"
#include "message.naix"
#include "font/font.naix" //NARC_font_large_gftr
#include "system/wipe.h"
#include "gamesystem/g_power.h"



//==============================================================================
//  �萔��`
//==============================================================================
///���m���X���ʑf�ރA�N�^�[�p���b�g�{��
#define MONOLITH_COMMON_PLTT_NUM    (3)


//==============================================================================
//  �\���̒�`
//==============================================================================
///���m���X����V�X�e��
typedef struct{
  MONOLITH_SETUP setup;             ///<���m���X�S��ʋ��ʐݒ�f�[�^
  MONOLITH_COMMON_WORK common;      ///<���m���X�S��ʋ��p���[�N
  MONOLITH_APP_PARENT app_parent;   ///<APP Proc�p��ParentWork
	GFL_TCB *vintr_tcb;               ///<VBlankTCB�ւ̃|�C���^
  GFL_PROCSYS *procsys_up;          ///<����PROC�V�X�e��
  GFL_PROCSYS *procsys_down;        ///<�����PROC�V�X�e��
  u8 menu_index;                    ///<MONOLITH_MENU_???
  u8 proc_up_occ;                   ///<TRUE:����PROC�L��
  u8 proc_down_occ;                 ///<TRUE:�����PROC�L��
  u8 padding;
}MONOLITH_SYSTEM;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithProc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_VramSetting(void);
static void _Setup_VramExit(void);
static void _Setup_LibSetting(void);
static void _Setup_LibExit(void);
static void _Setup_PaletteSetting(MONOLITH_SETUP *setup);
static void _Setup_PaletteExit(MONOLITH_SETUP *setup);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup);
static void _Setup_MessageSetting(MONOLITH_SETUP *setup);
static void _Setup_MessageExit(MONOLITH_SETUP *setup);
static void _Setup_ActorSetting(MONOLITH_SETUP *setup);
static void _Setup_ActorExit(MONOLITH_SETUP *setup);
static void _Setup_OBJGraphicLoad(MONOLITH_SETUP *setup);
static void _Setup_OBJGraphicUnload(MONOLITH_SETUP *setup);
static void _VblankFunc(GFL_TCB *tcb, void *work);


//==============================================================================
//  �f�[�^
//==============================================================================
///�e��ʂŎg�p����PROC�e�[�u��   ��MONOLITH_MENU�ƕ��т𓯂��ɂ��鎖�I
static const struct{
  const GFL_PROC_DATA *proc_up;    ///<����PROC
  const GFL_PROC_DATA *proc_down;  ///<�����PROC
}MonolithProcTbl[] = {
  {//MONOLITH_MENU_TITLE
    &MonolithAppProc_Up_PalaceMap,
    &MonolithAppProc_Down_Title,
  },
  {//MONOLITH_MENU_MISSION
    &MonolithAppProc_Up_MissionExplain,
    &MonolithAppProc_Down_MissionSelect,
  },
  {//MONOLITH_MENU_POWER
    &MonolithAppProc_Up_PowerExplain,
    &MonolithAppProc_Down_PowerSelect,
  },
  {//MONOLITH_MENU_STATUS
    &MonolithAppProc_Up_PalaceMap,
    &MonolithAppProc_Down_Status,
  },
};

///���m���X���C��PROC�f�[�^
const GFL_PROC_DATA MonolithProcData = {
  MonolithProc_Init,
  MonolithProc_Main,
  MonolithProc_End,
};

//--------------------------------------------------------------
//	BG
//--------------------------------------------------------------
///VRAM�o���N�ݒ�
static const GFL_DISP_VRAM MonolithVramBank = {
	GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K,		// �T�uOBJ�}�b�s���O���[�h
};

///BG���[�h�ݒ�
static const GFL_BG_SYS_HEADER MonolithBgSysHeader = {
	GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
};


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_SYSTEM *monosys;
  MONOLITH_PARENT_WORK *parent = pwk;
  
	//��ʂ�^���Â�
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);

	//�e����ʃ��W�X�^������
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
//	G2_BlendNone();
//	G2S_BlendNone();
	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 15, 15);
	G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 15, 15);

	//�㉺��ʐݒ�
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MONOLITH, 0x70000 );
  
  monosys = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_SYSTEM), HEAPID_MONOLITH);
  GFL_STD_MemClear(monosys, sizeof(MONOLITH_SYSTEM));
  monosys->common.power_select_no = GPOWER_ID_NULL;
  
	monosys->setup.hdl = GFL_ARC_OpenDataHandle(ARCID_MONOLITH, HEAPID_MONOLITH);
  {
    _Setup_VramSetting();
    _Setup_LibSetting();
    _Setup_PaletteSetting(&monosys->setup);
    _Setup_BGFrameSetting();
    _Setup_BGGraphicLoad(&monosys->setup);
    _Setup_MessageSetting(&monosys->setup);
    _Setup_ActorSetting(&monosys->setup);
    _Setup_OBJGraphicLoad(&monosys->setup);
    monosys->setup.powerdata = GPOWER_PowerData_LoadAlloc(HEAPID_MONOLITH);
  }

  //���[�J��PROC�쐬
  monosys->procsys_up = GFL_PROC_LOCAL_boot(HEAPID_MONOLITH);
  monosys->procsys_down = GFL_PROC_LOCAL_boot(HEAPID_MONOLITH);
  
	//V�u�����NTCB�o�^
	monosys->vintr_tcb = GFUser_VIntr_CreateTCB(_VblankFunc, monosys, MONOLITH_VINTR_TCB_PRI_MAIN);

  monosys->app_parent.parent = parent;
  monosys->app_parent.setup = &monosys->setup;
  monosys->app_parent.common = &monosys->common;
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_SYSTEM *monosys = mywk;
  MONOLITH_PARENT_WORK *parent = pwk;
  enum{
    SEQ_INIT,
    SEQ_INIT_WAIT,
    SEQ_PROC_MAIN,
    SEQ_END,
    SEQ_END_WAIT,
    SEQ_FINISH,
  };
  
  switch(*seq){
  case SEQ_INIT:
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
      WIPE_DEF_DIV, WIPE_DEF_SYNC, GFL_HEAP_LOWID(HEAPID_MONOLITH));
    (*seq)++;
    break;
  case SEQ_INIT_WAIT:
    if(WIPE_SYS_EndCheck() == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_PROC_MAIN:
    //���m���X���L�҂��ؒf��ԂɂȂ����烂�m���X��ʂ��I��������
    if(parent->palace_area != GAMEDATA_GetIntrudeMyID(GAMESYSTEM_GetGameData(parent->gsys))
        && GFL_NET_IsConnectMember(parent->palace_area) == FALSE){
      monosys->app_parent.force_finish = TRUE;
    }
    
    if(monosys->app_parent.force_finish == FALSE){
      //�㉺�A�ePROC�������ꍇ�͎���PROC�𐶐�
      if(monosys->proc_up_occ == FALSE){
        GFL_PROC_LOCAL_CallProc(monosys->procsys_up, NO_OVERLAY_ID, 
          MonolithProcTbl[monosys->menu_index].proc_up, &monosys->app_parent);
        monosys->proc_up_occ = TRUE;
      }
      if(monosys->proc_down_occ == FALSE){
        GFL_FONTSYS_SetColor(   //�ꉞ�����F��W���ɖ߂����������Ă���
          MONOLITH_FONT_DEFCOLOR_LETTER, 
          MONOLITH_FONT_DEFCOLOR_SHADOW, MONOLITH_FONT_DEFCOLOR_BACK );
        GFL_PROC_LOCAL_CallProc(monosys->procsys_down, NO_OVERLAY_ID, 
          MonolithProcTbl[monosys->menu_index].proc_down, &monosys->app_parent);
        monosys->proc_down_occ = TRUE;
      }
    }

    {
      GFL_PROC_MAIN_STATUS up_status, down_status;
      
      //����ʂ��哱��������̂ŉ����珈��
      down_status = GFL_PROC_LOCAL_Main(monosys->procsys_down);
      if(monosys->app_parent.next_menu_index == MONOLITH_MENU_END){
        monosys->app_parent.up_proc_finish = TRUE;
      }
      else if(monosys->menu_index != monosys->app_parent.next_menu_index
          && MonolithProcTbl[monosys->menu_index].proc_up 
          != MonolithProcTbl[monosys->app_parent.next_menu_index].proc_up){
        //�����PROC������PROC�֍s�����Ƃ��Ă��ď��ʂ�PROC�����ƈ�v���Ă��Ȃ��̂ł����
        //����PROC�ɑ΂���FINISH��m�点��
        monosys->app_parent.up_proc_finish = TRUE;
      }
      
      //����PROC���s
      up_status = GFL_PROC_LOCAL_Main(monosys->procsys_up);
      
      if(up_status != GFL_PROC_MAIN_VALID || down_status != GFL_PROC_MAIN_VALID){
        if(monosys->app_parent.force_finish == TRUE 
            || monosys->app_parent.next_menu_index == MONOLITH_MENU_END){
          OS_TPrintf("���m���X��ʏI���҂� up=%d, down=%d\n", up_status, down_status);
          if(up_status == GFL_PROC_MAIN_NULL && down_status == GFL_PROC_MAIN_NULL){
            (*seq)++;
          }
        }
        else{
          monosys->menu_index = monosys->app_parent.next_menu_index;
          if(up_status != GFL_PROC_MAIN_VALID){
            monosys->proc_up_occ = FALSE;
            monosys->app_parent.up_proc_finish = FALSE;
          }
          if(down_status != GFL_PROC_MAIN_VALID){
            monosys->proc_down_occ = FALSE;
          }
        }
      }
    }
    break;
  case SEQ_END:
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
      WIPE_DEF_DIV, WIPE_DEF_SYNC, GFL_HEAP_LOWID(HEAPID_MONOLITH));
    (*seq)++;
    break;
  case SEQ_END_WAIT:
    if(WIPE_SYS_EndCheck() == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_FINISH:
    return GFL_PROC_RES_FINISH;
  }
  
	GFL_TCBL_Main( monosys->setup.tcbl_sys );
	PRINTSYS_QUE_Main( monosys->setup.printQue );
	GFL_CLACT_SYS_Main();

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_SYSTEM *monosys = mywk;
  
  GFL_PROC_LOCAL_Exit(monosys->procsys_up);
  GFL_PROC_LOCAL_Exit(monosys->procsys_down);
  
	GFL_TCB_DeleteTask(monosys->vintr_tcb);

  GPOWER_PowerData_Unload(monosys->setup.powerdata);
  _Setup_OBJGraphicUnload(&monosys->setup);
  _Setup_ActorExit(&monosys->setup);
  _Setup_MessageExit(&monosys->setup);
  _Setup_BGFrameExit();
  _Setup_PaletteExit(&monosys->setup);
  _Setup_LibExit();
  _Setup_VramExit();

	GFL_ARC_CloseDataHandle(monosys->setup.hdl);

	G2_BlendNone();
	G2S_BlendNone();

  GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_MONOLITH);
  return GFL_PROC_RES_FINISH;
}


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 */
//--------------------------------------------------------------
static void _Setup_VramSetting(void)
{
	GFL_DISP_SetBank( &MonolithVramBank );
	GFL_BG_Init( HEAPID_MONOLITH );
	GFL_BG_SetBGMode( &MonolithBgSysHeader );

	//VRAM�N���A
	GFL_STD_MemClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
}

//--------------------------------------------------------------
/**
 * VRAM�o���N�����[�h�ݒ���
 */
//--------------------------------------------------------------
static void _Setup_VramExit(void)
{
  GFL_BG_Exit();
}

//--------------------------------------------------------------
/**
 * GFLIB�֘A�̏����ݒ�
 */
//--------------------------------------------------------------
static void _Setup_LibSetting(void)
{
  GFL_BMPWIN_Init( HEAPID_MONOLITH );
}

//--------------------------------------------------------------
/**
 * GFLIB�֘A�̔j������
 */
//--------------------------------------------------------------
static void _Setup_LibExit(void)
{
  GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------
/**
 * �p���b�g�V�X�e���쐬
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_PaletteSetting(MONOLITH_SETUP *setup)
{
  setup->pfd = PaletteFadeInit(HEAPID_MONOLITH);
  PaletteFadeWorkAllocSet(setup->pfd, FADE_MAIN_OBJ, 16*32, HEAPID_MONOLITH);
  PaletteFadeWorkAllocSet(setup->pfd, FADE_SUB_OBJ, 16*32, HEAPID_MONOLITH);
  PaletteFadeWorkAllocSet(setup->pfd, FADE_MAIN_BG, 16*32, HEAPID_MONOLITH);
  PaletteFadeWorkAllocSet(setup->pfd, FADE_SUB_BG, 16*32, HEAPID_MONOLITH);
  PaletteTrans_AutoSet(setup->pfd, TRUE);
}

//--------------------------------------------------------------
/**
 * �p���b�g�V�X�e���j��
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_PaletteExit(MONOLITH_SETUP *setup)
{
  PaletteFadeWorkAllocFree(setup->pfd, FADE_MAIN_OBJ);
  PaletteFadeWorkAllocFree(setup->pfd, FADE_SUB_OBJ);
  PaletteFadeWorkAllocFree(setup->pfd, FADE_MAIN_BG);
  PaletteFadeWorkAllocFree(setup->pfd, FADE_SUB_BG);
  PaletteFadeFree(setup->pfd);
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 */
//--------------------------------------------------------------
static void _Setup_BGFrameSetting(void)
{
  int i;
  
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {	//���C�����BG�t���[��
		{//GFL_BG_FRAME3_M
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			MONO_BG_COMMON_SCRBASE, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {	//�T�u���BG�t���[��
		{//GFL_BG_FRAME3_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			MONO_BG_COMMON_SCRBASE, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME3_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME3_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );

//	GFL_BG_FillCharacter( FRAME_BACK_M, 0x00, 1, 0 );

	GFL_BG_FillScreen( GFL_BG_FRAME3_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME3_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

  for(i = GFL_BG_FRAME0_M; i <= GFL_BG_FRAME3_S; i++){
  	GFL_BG_SetVisible(i, VISIBLE_OFF);
  }
	GFL_BG_SetVisible(GFL_BG_FRAME3_M, VISIBLE_ON);
	GFL_BG_SetVisible(GFL_BG_FRAME3_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME3_M, VISIBLE_OFF);
	GFL_BG_SetVisible(GFL_BG_FRAME3_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_M);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
}

//--------------------------------------------------------------
/**
 * @brief   ���ʑf��BG�O���t�B�b�N��VRAM�֓]��
 */
//--------------------------------------------------------------
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup)
{
	//���ʃp���b�g
  PaletteWorkSetEx_ArcHandle(setup->pfd, setup->hdl, 
    NARC_monolith_mono_bgu_NCLR, HEAPID_MONOLITH, FADE_MAIN_BG, 0, 0, 0);
  PaletteWorkSetEx_ArcHandle(setup->pfd, setup->hdl, 
    NARC_monolith_mono_bgd_NCLR, HEAPID_MONOLITH, FADE_SUB_BG, 0, 0, 0);

	//���C����ʁF���ʑf��
	GFL_ARCHDL_UTIL_TransVramBgCharacter(setup->hdl, NARC_monolith_mono_bgu_lz_NCGR, 
		GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_MONOLITH);
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgu_base_lz_NSCR, 
		GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_MONOLITH);

	//�T�u��ʁF���ʑf��
	GFL_ARCHDL_UTIL_TransVramBgCharacter(setup->hdl, NARC_monolith_mono_bgd_lz_NCGR, 
		GFL_BG_FRAME3_S, 0, 0, TRUE, HEAPID_MONOLITH);
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgd_base_lz_NSCR, 
		GFL_BG_FRAME3_S, 0, 0, TRUE, HEAPID_MONOLITH);

	//�t�H���g�p���b�g�]��
  PaletteWorkSetEx_Arc(setup->pfd, ARCID_FONT, NARC_font_default_nclr, HEAPID_MONOLITH,
      FADE_MAIN_BG, 0, MONOLITH_BG_UP_FONT_PALNO * 16, 0);
  PaletteWorkSetEx_Arc(setup->pfd, ARCID_FONT, NARC_font_default_nclr, HEAPID_MONOLITH,
      FADE_SUB_BG, 0, MONOLITH_BG_DOWN_FONT_PALNO * 16, 0);

	GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME3_S );
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`��֘A�̐ݒ�
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_MessageSetting(MONOLITH_SETUP *setup)
{
//	GFL_FONTSYS_Init();
  GFL_FONTSYS_SetColor( 
    MONOLITH_FONT_DEFCOLOR_LETTER, MONOLITH_FONT_DEFCOLOR_SHADOW, MONOLITH_FONT_DEFCOLOR_BACK );

	setup->tcbl_sys = GFL_TCBL_Init(HEAPID_MONOLITH, HEAPID_MONOLITH, 4, 32);
	setup->font_handle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_MONOLITH );
	setup->printQue = PRINTSYS_QUE_Create( HEAPID_MONOLITH );
	setup->wordset = WORDSET_Create(HEAPID_MONOLITH);

	setup->mm_mission_mono = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_mission_monolith_dat, HEAPID_MONOLITH);
	setup->mm_power = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_power_dat, HEAPID_MONOLITH);
	setup->mm_power_explain = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_power_explain_dat, HEAPID_MONOLITH);
	setup->mm_monolith = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monolith_dat, HEAPID_MONOLITH);
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�j��
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_MessageExit(MONOLITH_SETUP *setup)
{
	GFL_MSG_Delete(setup->mm_mission_mono);
	GFL_MSG_Delete(setup->mm_power);
	GFL_MSG_Delete(setup->mm_power_explain);
	GFL_MSG_Delete(setup->mm_monolith);
	WORDSET_Delete(setup->wordset);
	PRINTSYS_QUE_Delete(setup->printQue);
	GFL_FONT_Delete(setup->font_handle);
	GFL_TCBL_Exit(setup->tcbl_sys);
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_ActorSetting(MONOLITH_SETUP *setup)
{
	GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
	
	clsys_init.oamst_main = GFL_CLSYS_OAMMAN_INTERVAL;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_main = 128-GFL_CLSYS_OAMMAN_INTERVAL;
	clsys_init.oamst_sub = GFL_CLSYS_OAMMAN_INTERVAL;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_sub = 128-GFL_CLSYS_OAMMAN_INTERVAL;
	clsys_init.CGR_RegisterMax = 96;  //BMPOAM�ő�ʂɃ��\�[�X�o�^�����
	clsys_init.CELL_RegisterMax = 64; //BMPOAM�ő�ʂɃ��\�[�X�o�^�����
	GFL_CLACT_SYS_Create(&clsys_init, &MonolithVramBank, HEAPID_MONOLITH);
	
	setup->clunit = GFL_CLACT_UNIT_Create(96, 0, HEAPID_MONOLITH);
	GFL_CLACT_UNIT_SetDefaultRend(setup->clunit);

	//OBJ�\��ON
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

  //PLTT SLOT
  setup->plttslot = PLTTSLOT_Init(HEAPID_MONOLITH, 16, 16);

  //BMP OAM
  setup->bmpoam_sys = BmpOam_Init(HEAPID_MONOLITH, setup->clunit);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�j��
 *
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_ActorExit(MONOLITH_SETUP *setup)
{
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_OFF);
	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_OFF);
	
	BmpOam_Exit(setup->bmpoam_sys);
  GFL_CLACT_UNIT_Delete(setup->clunit);
  GFL_CLACT_SYS_Delete();
  
  PLTTSLOT_Exit(setup->plttslot);
}

//--------------------------------------------------------------
/**
 * @brief   ���ʑf��OBJ�O���t�B�b�N��VRAM�֓]��
 */
//--------------------------------------------------------------
static void _Setup_OBJGraphicLoad(MONOLITH_SETUP *setup)
{
  int i;
  ARCHANDLE *font_hdl;
  CLSYS_DRAW_TYPE draw_type;
  
  font_hdl = GFL_ARC_OpenDataHandle(ARCID_FONT, HEAPID_MONOLITH);
  
  for(i = 0; i < COMMON_RESOURCE_INDEX_MAX; i++){
    draw_type = (i == COMMON_RESOURCE_INDEX_UP) ? CLSYS_DRAW_MAIN : CLSYS_DRAW_SUB;
    
  	//���ʃp���b�g
    setup->common_res[i].pltt_index = PLTTSLOT_ResourceSet_PalAnm(setup->pfd, setup->plttslot, 
      setup->hdl, NARC_monolith_mono_obj_NCLR, draw_type, 
      MONOLITH_COMMON_PLTT_NUM, HEAPID_MONOLITH);

  	//���ʑf��CGX
    setup->common_res[i].char_index = GFL_CLGRP_CGR_Register(
      setup->hdl, NARC_monolith_mono_obj_lz_NCGR, TRUE, draw_type, HEAPID_MONOLITH );

    //���ʑf��CELL
    setup->common_res[i].cell_index = GFL_CLGRP_CELLANIM_Register(
      setup->hdl, NARC_monolith_mono_obj_NCER, NARC_monolith_mono_obj_NANR, HEAPID_MONOLITH);

  	//�t�H���g�p���b�g�]��(BMP FONT�p)
    setup->common_res[i].pltt_bmpfont_index = PLTTSLOT_ResourceCompSet_PalAnm(setup->pfd, 
      setup->plttslot, font_hdl, NARC_font_default_nclr, draw_type, 1, HEAPID_MONOLITH);
  }

  GFL_ARC_CloseDataHandle(font_hdl);
}

//--------------------------------------------------------------
/**
 * @brief   ���ʑf��OBJ�O���t�B�b�N�������
 */
//--------------------------------------------------------------
static void _Setup_OBJGraphicUnload(MONOLITH_SETUP *setup)
{
  int i;
  CLSYS_DRAW_TYPE draw_type;
  
  for(i = 0; i < COMMON_RESOURCE_INDEX_MAX; i++){
    draw_type = (i == COMMON_RESOURCE_INDEX_UP) ? CLSYS_DRAW_MAIN : CLSYS_DRAW_SUB;

    PLTTSLOT_ResourceFree(setup->plttslot, setup->common_res[i].pltt_index, draw_type);
    GFL_CLGRP_CGR_Release(setup->common_res[i].char_index);
    GFL_CLGRP_CELLANIM_Release(setup->common_res[i].cell_index);
    PLTTSLOT_ResourceFree(setup->plttslot, setup->common_res[i].pltt_bmpfont_index, draw_type);
  }
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
  MONOLITH_SYSTEM *monosys = work;
  
	GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( monosys->setup.pfd );
}
