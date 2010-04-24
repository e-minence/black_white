//==============================================================================
/**
 * @file    monolith_palacemap.c
 * @brief   ���m���X�F�p���X�}�b�v
 * @author  matsuda
 * @date    2009.11.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "monolith_common.h"
#include "arc_def.h"
#include "monolith_tool.h"
#include "msg/msg_monolith.h"
#include "intrude_work.h"
#include "monolith.naix"


//==============================================================================
//  �萔��`
//==============================================================================
///�o�����X�Q�[�W�`��p�̐ݒ�l
enum{
  BALANCE_GAUGE_CHARA_MAX = 30,     ///<�o�����X�Q�[�W�̉����L������
  BALANCE_GAUGE_DOTTO_MAX = BALANCE_GAUGE_CHARA_MAX * 8,    ///<�o�����X�Q�[�W�̃h�b�g��
  BALANCE_SCRN_START_X = 1,
  BALANCE_SCRN_START_Y = 0x14,
  BALANCE_SCRN_BLACK_CHARNO_START = 1,  ///<�u���b�N�Q�[�W�̃L�����N�^No�J�n�ʒu
  BALANCE_SCRN_MAXWHITE_CHARNO = 9,     ///<�z���C�g�Q�[�W(MAX)�̃L�����N�^No�J�n�ʒu
  ONECHARA_DOT = 8,   ///<1�̃L�����N�^�̃h�b�g��
  BALANCE_GAUGE_PALNO = 1,          ///<�o�����X�Q�[�W�̃p���b�g�ԍ�
};


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  MONOLITH_BMPSTR bmpstr_title;
  GFL_CLWK *act_town[MISSION_LIST_MAX]; ///<�X�A�C�R���A�N�^�[�ւ̃|�C���^
}MONOLITH_PALACEMAP_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithPalaceMapProc_Init(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPalaceMapProc_Main(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPalaceMapProc_End(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup);
static void _TownIcon_AllCreate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk);
static void _TownIcon_AllDelete(MONOLITH_PALACEMAP_WORK *mpw);
static void _TownIcon_AllUpdate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk);
static void _BalanceGaugeRewrite(MONOLITH_APP_PARENT *appwk);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[PROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Up_PalaceMap = {
  MonolithPalaceMapProc_Init,
  MonolithPalaceMapProc_Main,
  MonolithPalaceMapProc_End,
};

//--------------------------------------------------------------
//  �X�A�C�R��
//--------------------------------------------------------------
///�X�A�C�R���\�����W�e�[�u��
static const GFL_POINT TownIconPosTbl[] = {
  {11*8, 6*8},
  {0x15*8, 6*8},

  {8*8, 10*8},
  {0x18*8, 10*8},

  {11*8, 0xe*8},
  {0x15*8, 0xe*8},
};
SDK_COMPILER_ASSERT(NELEMS(TownIconPosTbl) == MISSION_LIST_MAX);




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
static GFL_PROC_RESULT MonolithPalaceMapProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
	ARCHANDLE *hdl;
  
  switch(*seq){
  case 0:
    mpw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_PALACEMAP_WORK), HEAPID_MONOLITH);
    GFL_STD_MemClear(mpw, sizeof(MONOLITH_PALACEMAP_WORK));
    
    //BG
    _Setup_BGFrameSetting();
    _Setup_BGGraphicLoad(appwk, appwk->setup);
    //OBJ
    {
      MYSTATUS *myst = MonolithTool_GetMystatus(appwk);

      STRBUF *strbuf = 	GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_MONOLITH);

    	GFL_STR_SetStringCodeOrderLength(
    	  strbuf, MyStatus_GetMyName(myst), PERSON_NAME_SIZE + EOM_SIZE);
      WORDSET_RegisterWord(appwk->setup->wordset, 0, strbuf, MyStatus_GetMySex(myst), TRUE, PM_LANG);

      GFL_STR_DeleteBuffer(strbuf);
    }
    MonolithTool_Bmpoam_Create(appwk->setup, &mpw->bmpstr_title, COMMON_RESOURCE_INDEX_UP, 
      128, 12, 30, 2, msg_mono_title_000, appwk->setup->wordset);
    _TownIcon_AllCreate(mpw, appwk);
    
    (*seq)++;
    break;
  case 1:
  	GFL_BG_SetVisible(GFL_BG_FRAME2_M, VISIBLE_ON);
  	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
  	return GFL_PROC_RES_FINISH;
  }
  
  return GFL_PROC_RES_CONTINUE;
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
static GFL_PROC_RESULT MonolithPalaceMapProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
  int tp_ret;
  
  if(appwk->up_proc_finish == TRUE || appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }
  
  MonolithTool_Bmpoam_TransUpdate(appwk->setup, &mpw->bmpstr_title);
  _TownIcon_AllUpdate(mpw, appwk);

  switch(*seq){
  case 0:
    return GFL_PROC_RES_CONTINUE;
  }
  
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
static GFL_PROC_RESULT MonolithPalaceMapProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
  
  GFL_DISP_GX_SetVisibleControlDirect(GX_PLANEMASK_BG3);

  //OBJ
  _TownIcon_AllDelete(mpw);
  MonolithTool_Bmpoam_Delete(&mpw->bmpstr_title);
  //BG
  _Setup_BGFrameExit();
  
  GFL_PROC_FreeWork(proc);
  return GFL_PROC_RES_FINISH;
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 */
//--------------------------------------------------------------
static void _Setup_BGFrameSetting(void)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {
		{//GFL_BG_FRAME2_M
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME2_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME2_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME2_M, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_M);
}

//--------------------------------------------------------------
/**
 * @brief   BG�O���t�B�b�N��VRAM�֓]��
 *
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void _Setup_BGGraphicLoad(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup)
{
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgu_map_lz_NSCR, 
		GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_MONOLITH);
  _BalanceGaugeRewrite(appwk);
  
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[����
 */
//--------------------------------------------------------------
static void _TownIcon_AllCreate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  
  for(i = 0; i < MISSION_LIST_MAX; i++){
    mpw->act_town[i] = MonolithTool_TownIcon_Create(
      appwk->setup, occupy, i, &TownIconPosTbl[i], COMMON_RESOURCE_INDEX_UP);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[�폜
 */
//--------------------------------------------------------------
static void _TownIcon_AllDelete(MONOLITH_PALACEMAP_WORK *mpw)
{
  int i;
  
  for(i = 0; i < MISSION_LIST_MAX; i++){
    MonolithTool_TownIcon_Delete(mpw->act_town[i]);
  }
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���S�X�V
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _TownIcon_AllUpdate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  for(i = 0; i < MISSION_LIST_MAX; i++){
    MonolithTool_TownIcon_Update(mpw->act_town[i], occupy, i);
  }
}

//--------------------------------------------------------------
/**
 * �o�����X�Q�[�W�̃X�N���[������������
 *
 * @param   appwk		
 */
//--------------------------------------------------------------
static void _BalanceGaugeRewrite(MONOLITH_APP_PARENT *appwk)
{
  const OCCUPY_INFO *occupy = MonolithTool_GetOccupyInfo(appwk);
  int lv_w, lv_b, lv_total;
  int dot_w, dot_b;
  int pos;
  u16 *scrnbuf;
  
  lv_w = occupy->white_level;
  lv_b = occupy->black_level;
  lv_total = lv_w + lv_b;
  
  if(lv_w == lv_b){ //�����l�����x��0���m�̏ꍇ������̂�
    dot_w = BALANCE_GAUGE_DOTTO_MAX / 2;
    dot_b = BALANCE_GAUGE_DOTTO_MAX - dot_w;
  }
  else{
    dot_w = BALANCE_GAUGE_DOTTO_MAX * lv_w / lv_total;
    if(lv_b > 0){ //�����؎̂Ăɂ���ďo�����������x��0�̃u���b�N�ɉ��Z����Ȃ��悤�Ƀ`�F�b�N
      dot_b = BALANCE_GAUGE_DOTTO_MAX - dot_w;
    }
    else{
      dot_b = 0;
      dot_w = BALANCE_GAUGE_DOTTO_MAX;
    }
  }
  //���x����1�ȏ゠��Ȃ�v�Z��h�b�g��0�ɂȂ��Ă���1�h�b�g�͓����
  if(lv_w > 0 && dot_w == 0){
    dot_w++;
    dot_b--;
  }
  else if(lv_b > 0 && dot_b == 0){
    dot_b++;
    dot_w--;
  }
  
  scrnbuf = GFL_HEAP_AllocClearMemory(HEAPID_MONOLITH, BALANCE_GAUGE_CHARA_MAX * sizeof(u16));
  
  //���̃Q�[�W��`��
  pos = 0;
  while(dot_b > 0){
    if(dot_b >= ONECHARA_DOT){
      scrnbuf[pos] = BALANCE_SCRN_BLACK_CHARNO_START;
      dot_b -= ONECHARA_DOT;
    }
    else{
      scrnbuf[pos] = ONECHARA_DOT - dot_b + BALANCE_SCRN_BLACK_CHARNO_START;
      dot_b = 0;
    }
    pos++;
  }
  //�c���MAX�Ŗ��߂�
  for( ; pos < BALANCE_GAUGE_CHARA_MAX; pos++){
    scrnbuf[pos] = BALANCE_SCRN_MAXWHITE_CHARNO;
  }
  //�p���b�gNo���X�N���[���ɓ����
  for(pos = 0; pos < BALANCE_GAUGE_CHARA_MAX; pos++){
    scrnbuf[pos] |= BALANCE_GAUGE_PALNO << 12;
  }
  
  //�X�N���[���`��
  GFL_BG_WriteScreen( GFL_BG_FRAME2_M, scrnbuf, 
    BALANCE_SCRN_START_X, BALANCE_SCRN_START_Y, BALANCE_GAUGE_CHARA_MAX, 1);
  
  GFL_HEAP_FreeMemory(scrnbuf);
}

