//============================================================================================
/**
 * @file  worldtrade.c
 * @brief ���E�������C������
 * @author  Akito Mori -> toru_nagihashi WB�ֈڐA
 * @date  06.04.16  -> 09.08.04
 */
//============================================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dwc.h>
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "msg/msg_wifi_lobby.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h"

#include "print/printsys.h"


#include "savedata/wifilist.h"

#include "poke_tool/monsno_def.h"
#include "system/bmp_menu.h"
#include "net_app/worldtrade.h"
#include "sound/pm_sndsys.h"
#include "savedata/save_control.h"

#include "net/dwc_raputil.h"

#include "worldtrade_local.h"

#include "net_app/connect_anm.h"
#include "net/network_define.h"

//#include "msgdata/msg_ev_win.h"

#define WORLDTRADE_WORDSET_BUFLEN ( 64 )  ///< WORDSet������̓W�J�p�o�b�t�@��

#define MYDWC_HEAPSIZE    0x20000
#include "worldtrade.naix"          ///< �O���t�B�b�N�A�[�J�C�u��`
#include "poke_icon.naix"

#define BRIGHT_VAL  (-7)          ///< �p�b�V�u��Ԃ̂��߂̔�������

FS_EXTERN_OVERLAY( dpw_common );

//============================================================================================
//  �萔��`
//============================================================================================
// �|�C���^�Q�Ƃ����ł��鐢�E�������[�N�\����
//typedef struct _WORLDTRADE_WORK WORLDTRADE_WORK;


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
// �v���Z�X��`�f�[�^
static GFL_PROC_RESULT WorldTradeProc_Init( GFL_PROC * proc, int * seq, void * param, void * work );
static GFL_PROC_RESULT WorldTradeProc_Main( GFL_PROC * proc, int * seq, void * param, void * work );
static GFL_PROC_RESULT WorldTradeProc_End( GFL_PROC * proc, int * seq, void * param, void * work );


/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *, void *work );
static void VramBankSet( const GFL_DISP_VRAM * vram );
static void InitWork( WORLDTRADE_WORK *wk, WORLDTRADE_PARAM *param );
static void FreeWork( WORLDTRADE_WORK *wk );
static void InitCellActor(WORLDTRADE_WORK *wk, const GFL_DISP_VRAM * vram );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void InitCLACT( WORLDTRADE_WORK *wk );
static void FreeCLACT( WORLDTRADE_WORK *wk );
static void ServerWaitTimeFunc( WORLDTRADE_WORK *wk );
static void BoxPokeNumGet( WORLDTRADE_WORK *wk );
static void WorldTrade_SelBoxCallback( SELBOX_WORK* sbox, u8 cur_pos, void* work, SBOX_CB_MODE mode);
static void WorldTrade_WndSetting(void);



//============================================================================================
//  �֐��e�[�u����`
//============================================================================================

// ���E�����T�u�v���Z�X�p�֐��z���`
static int (*SubProcessTable[][3])(WORLDTRADE_WORK *wk, int seq)={
  { WorldTrade_Enter_Init,   WorldTrade_Enter_Main,    WorldTrade_Enter_End   },
  { WorldTrade_Title_Init,   WorldTrade_Title_Main,    WorldTrade_Title_End   },
  { WorldTrade_MyPoke_Init,  WorldTrade_MyPoke_Main,   WorldTrade_MyPoke_End  },
  { WorldTrade_Partner_Init, WorldTrade_Partner_Main,  WorldTrade_Partner_End },
  { WorldTrade_Search_Init,  WorldTrade_Search_Main,   WorldTrade_Search_End  },
  { WorldTrade_Box_Init,     WorldTrade_Box_Main,      WorldTrade_Box_End     },
  { WorldTrade_Deposit_Init, WorldTrade_Deposit_Main,  WorldTrade_Deposit_End },
  { WorldTrade_Upload_Init,  WorldTrade_Upload_Main,   WorldTrade_Upload_End  },
  { WorldTrade_Status_Init,  WorldTrade_Status_Main,   WorldTrade_Status_End  },
  { WorldTrade_Demo_Init,    WorldTrade_Demo_Main,     WorldTrade_Demo_End  },
};


WORLDTRADE_WORK *debug_worldtrade;

//============================================================================================
//  �v���Z�X�֐�
//============================================================================================
const GFL_PROC_DATA WorldTrade_ProcData =
{
  WorldTradeProc_Init,
  WorldTradeProc_Main,
  WorldTradeProc_End,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WorldTradeProc_Init( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK * wk;

  GFL_OVERLAY_Load( FS_OVERLAY_ID( dpw_common ) );
  
  GFL_NET_DebugPrintOn();

#ifdef PM_DEBUG
    GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_SYSTEM );
    GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP );
#endif

    // �q�[�v�쐬
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WORLDTRADE, 0x80000 );

    wk = GFL_PROC_AllocWork( proc, sizeof(WORLDTRADE_WORK), HEAPID_WORLDTRADE );
    GFL_STD_MemClear( wk, sizeof(WORLDTRADE_WORK) );

    debug_worldtrade = wk;



    //task
    wk->task_wk_area  = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, GFL_TCB_CalcSystemWorkSize( 8 ) );
    wk->tcbsys        = GFL_TCB_Init( 8, wk->task_wk_area );

    // �^�b�`�t�H���g���[�h
//    FontProc_LoadFont( FONT_TOUCH, HEAPID_WORLDTRADE );
    {
      WORLDTRADE_PARAM *wt_param  = param;

      WT_PRINT_Init( &wk->print, wt_param->config );
    }
    OS_Printf("-------------------------------�v���b�N������\n");

    // ������}�l�[�W���[����
    wk->WordSet        = WORDSET_CreateEx( 11, WORLDTRADE_WORDSET_BUFLEN, HEAPID_WORLDTRADE );
    wk->MsgManager       = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_gts_dat, HEAPID_WORLDTRADE );
    wk->LobbyMsgManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_lobby_dat, HEAPID_WORLDTRADE );
    wk->SystemMsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_WORLDTRADE );
    wk->MonsNameManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat, HEAPID_WORLDTRADE );
    wk->CountryNameManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_place_msg_world_dat, HEAPID_WORLDTRADE );


/* �f�s�r��Wifi�o�g���^���[�ڑ���ʂ̍ŏ���WIPE_ResetBrightness���Ă�ł��܂��Ă��邽�߂�
   �o�b�N�h���b�v�ʂ������Ă��܂���������o�O��Ώ� */
#if AFTERMASTER_070215_GTS_WBTOWER_INIT_FIX
//    WIPE_ResetBrightness( WIPE_DISP_MAIN );
//    WIPE_ResetBrightness( WIPE_DISP_SUB );
#else
    WIPE_ResetBrightness( WIPE_DISP_MAIN );
    WIPE_ResetBrightness( WIPE_DISP_SUB );
#endif


    // ���[�N������
    InitWork( wk, param );

    WorldTrade_InitSystem( wk );

    //WirelessIconEasy();

    // �T�E���h�f�[�^���[�h(�t�B�[���h)
    Snd_DataSetByScene( SND_SCENE_FIELD, SEQ_BLD_BLD_GTC, 1 );
    Snd_DataSetByScene( SND_SCENE_P2P, SEQ_GS_WIFI_ACCESS, 1 );

    // �v���`�i�ŁA�}�b�v�̋ȂƁA��ʂ̋Ȃ��ύX�ɂȂ����̂ŁA
    // ��ʂ���A�t�B�[���h�ɖ߂鎞�̂��߂ɃV�[����ύX

    // ��b�E�C���h�E�̃^�b�`ON
    MsgPrintTouchPanelFlagSet( MSG_TP_ON );

    return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------

static GFL_PROC_RESULT WorldTradeProc_Main( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK * wk  = work;

  if( GFL_NET_IsInit() )
  {
    // ��M���x�����N�𔽉f������
    DWC_UpdateConnection();
    // �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
    WirelessIconEasy_SetLevel(WorldTrade_WifiLinkLevel());

    // Dpw_Tr_Main() �����͗�O�I�ɂ��ł��Ăׂ�
    Dpw_Tr_Main();
  }

  GFL_TCB_Main( wk->tcbsys );
  WT_PRINT_Main( &wk->print );

  switch( *seq ){
  // �T�u����������
  case SEQ_INIT:
    *seq = (*SubProcessTable[wk->sub_process][0])(wk, *seq);
    WorldTrade_WndSetting();
    if(wk->subprocflag){
      //FreeCLACT( wk );
      OS_Printf("OAM�V�X�e���Վ����");
    }
    break;

  // �T�u�����t�F�[�h�C���҂�
  case SEQ_FADEIN:
    if( WIPE_SYS_EndCheck() ){
      *seq = SEQ_MAIN;
    }
    break;

  // �T�u�������C��
  case SEQ_MAIN:
    *seq = (*SubProcessTable[wk->sub_process][1])(wk, *seq);

    break;

  // �T�u�����t�F�[�h�A�E�g�҂�
  case SEQ_FADEOUT:
    if( WIPE_SYS_EndCheck() ){
      // �T�u�������(SEQ_INIT�ɍs�����ASEQ_OUT�ɂ������͂��܂����j
      *seq = (*SubProcessTable[wk->sub_process][2])(wk, *seq);
      if(wk->subprocflag){
        //InitCLACT( wk );
        WorldTrade_SubLcdActorAdd( wk, MyStatus_GetMySex(wk->param->mystatus) );
        WorldTrade_SubLcdMatchObjAppear( wk, wk->SearchResult, 0 );
//        WorldTrade_SubLcdBgGraphicSet( wk );    // �g���[�h���[���]��
//        WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��
        wk->subprocflag = 0;
        OS_Printf("OAM�V�X�e���Վ����A");
      }
    }
    break;

  // ���E�����I������
  case SEQ_OUT:
    *seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GFL_PROC_RES_FINISH;
  }
  ServerWaitTimeFunc( wk );
  BoxPokeNumGet( wk );

  if(wk->clactSet!=NULL){
    GFL_CLACT_SYS_Main();
  }

  GF_ASSERT( GFL_HEAP_CheckHeapSafe(HEAPID_WORLDTRADE) );

  return GFL_PROC_RES_CONTINUE;
}

#define DEFAULT_NAME_MAX    18

// �_�C���E�p�[���ŕς��񂾂낤
#define MALE_NAME_START     0
#define FEMALE_NAME_START   18

//--------------------------------------------------------------------------------------------
/**
 * @brief    �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WorldTradeProc_End( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK  *wk    = work;
  int i;
  GF_ASSERT( GFL_HEAP_CheckHeapSafe(HEAPID_WORLDTRADE) );


  WirelessIconEasyEnd();
  WorldTrade_ExitSystem( wk );
  FreeWork( wk );

  // ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
  GFL_MSG_Delete( wk->MonsNameManager );
  GFL_MSG_Delete( wk->SystemMsgManager );
  GFL_MSG_Delete( wk->LobbyMsgManager );
  GFL_MSG_Delete( wk->MsgManager );
  GFL_MSG_Delete( wk->CountryNameManager );
  WORDSET_Delete( wk->WordSet );

  // ���b�Z�[�W�E�C���h�E�̃^�b�`OFF
  MsgPrintTouchPanelFlagSet( MSG_TP_OFF );
  GFL_TCB_Exit( wk->tcbsys );
  GFL_HEAP_FreeMemory( wk->task_wk_area );

  // �^�b�`�t�H���g�A�����[�h
  WT_PRINT_Exit( &wk->print );
  GFL_PROC_FreeWork( proc );        // GFL_PROC���[�N�J��

  GFL_HEAP_DeleteHeap( HEAPID_WORLDTRADE );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( dpw_common ) );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    VBlank�֐�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *, void *work )
{
  WORLDTRADE_WORK *wk = work;
  // VBlank���]���t�@���N�V�������s
  if(wk->vfunc){
    wk->vfunc(work);
    wk->vfunc = NULL;
  }
  if(wk->vfunc2){
    wk->vfunc2(work);
  }

  GFL_BG_VBlankFunc();

  // �Z���A�N�^�[Vram�]���}�l�[�W���[���s
  GFL_CLACT_SYS_VBlankFunc();

}


//--------------------------------------------------------------------------------------------
/**
 * @brief    VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet( const GFL_DISP_VRAM * vram )
{

  GFL_DISP_SetBank( vram );

}




//------------------------------------------------------------------
/**
 * @brief    ���E�������[�N������
 *
 * @param   wk    WORLDTRADE_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk, WORLDTRADE_PARAM *param )
{
  GF_ASSERT( param->savedata != NULL );
  GF_ASSERT( param->worldtrade_data != NULL );
  GF_ASSERT( param->systemdata != NULL );
  GF_ASSERT( param->myparty != NULL );
  GF_ASSERT( param->mybox != NULL );
//  GF_ASSERT( param->zukanwork != NULL );
  GF_ASSERT( param->wifilist != NULL );
  GF_ASSERT( param->wifihistory != NULL );
  GF_ASSERT( param->mystatus != NULL );
  GF_ASSERT( param->config != NULL );
  GF_ASSERT( param->record != NULL );
//  GF_ASSERT( param->myitem != NULL );



  // �Ăяo�����̃p�����[�^���擾
  wk->param = param;

  wk->sub_process   = WORLDTRADE_ENTER;
  WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, MODE_WIFILOGIN );

  wk->TitleCursorPos  = 0;

  wk->Search.characterNo     = 0;
  wk->Search.gender          = PARA_UNK+1;
  wk->Search.level_min       = 0;
  wk->Search.level_max       = 0;
  wk->SearchBackup.characterNo = 0;
  wk->demo_end               = 0;
  wk->BoxTrayNo          = 18;
  wk->boxPokeNum         = 0;
  wk->boxSearchFlag      = 0;
  wk->SubLcdTouchOK      = 0;
  wk->timeWaitWork             = NULL;

  wk->country_code       = 0;
}

//------------------------------------------------------------------
/**
 * @brief   ���[�N���
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeWork( WORLDTRADE_WORK *wk )
{


}

//--------------------------------------------------------------
/**
 * @brief    �E�B���h�E�ݒ�
 *
 * @param   none
 */
//--------------------------------------------------------------
static void WorldTrade_WndSetting(void)
{
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);
  //��M���x�A�C�R���Ƀt�F�[�h��������Ȃ��悤�ɃE�B���h�E�ň͂�
  GX_SetVisibleWnd(GX_WNDMASK_W0);
  G2_SetWnd0InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2
    | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, FALSE);
  G2_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2
    | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
  G2_SetWnd0Position(256-16, 0, 255, 16);
}


#define TRANS_POKEICON_COLOR_NUM  ( 3*16 )
//------------------------------------------------------------------
/**
 * @brief    ���[�_�[��ʗp�Z���A�N�^�[������
 *
 * @param   wk    ���[�_�[�\���̂̃|�C���^
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCellActor(WORLDTRADE_WORK *wk, const GFL_DISP_VRAM * vram )
{
  int i;
  ARCHANDLE* p_handle;

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

  // ���LOAM�}�l�[�W���쐬
  // �����_���pOAM�}�l�[�W���쐬
  // �����ō쐬����OAM�}�l�[�W�����݂�Ȃŋ��L����
  {
    static const GFL_CLSYS_INIT clsys_init =
    {
      0, 0,
      0, NAMEIN_SUB_ACTOR_DISTANCE,
      4, 124,
      4, 124,
      0,
      32,32,32,32,
      16, 16,
    };

    GFL_CLACT_SYS_Create( &clsys_init, vram, HEAPID_WORLDTRADE );
  }

  // �Z���A�N�^�[������
  wk->clactSet = GFL_CLACT_UNIT_Create( 72+12, 0, HEAPID_WORLDTRADE );



  //---------���ʗp-------------------
  //chara�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, NARC_worldtrade_worldtrade_obj_lz_ncgr, 1, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE);

  //pal�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] = GFL_CLGRP_PLTT_Register(p_handle, NARC_worldtrade_worldtrade_obj_nclr,CLSYS_DRAW_MAIN, /*3*0x20*/0, HEAPID_WORLDTRADE);

  //cell�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register( p_handle, NARC_worldtrade_worldtrade_obj_NCER, NARC_worldtrade_worldtrade_obj_NANR, HEAPID_WORLDTRADE);


  //---------����ʗp-------------------


  //chara�ǂݍ���
  wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, NARC_worldtrade_hero_lz_ncgr, 1, CLSYS_DRAW_SUB, HEAPID_WORLDTRADE);

  //pal�ǂݍ���
  wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] = GFL_CLGRP_PLTT_Register(p_handle, NARC_worldtrade_hero_nclr, CLSYS_DRAW_SUB, /*10*0x20*/0, HEAPID_WORLDTRADE);

  //cell�ǂݍ���
  wk->resObjTbl[SUB_LCD][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register(p_handle, NARC_worldtrade_worldtrade_obj_s_NCER, NARC_worldtrade_worldtrade_obj_s_NANR, HEAPID_WORLDTRADE);


  // �|�P�����A�C�R���p�p���b�g����C�ɓǂݍ����VRAM�]������
  // �|�P�����A�C�R���̃p���b�g���Â������f�[�^���쐬���ē]������
  {
    void              *buf;
    NNSG2dPaletteData *palData;
    int i,r,g,b;
    u16 *pal;

    buf = GFL_ARC_UTIL_LoadPalette( ARCID_POKEICON, NARC_poke_icon_poke_icon_NCLR, &palData, HEAPID_WORLDTRADE );
    DC_FlushRange( palData->pRawData, TRANS_POKEICON_COLOR_NUM*2 );
    GX_LoadOBJPltt( palData->pRawData, POKEICON_PAL_OFFSET*0x20, TRANS_POKEICON_COLOR_NUM*2 );

    pal = (u16*)palData->pRawData;
    for(i=0;i<TRANS_POKEICON_COLOR_NUM;i++){
      r = pal[i]>>10&0x1f;
      g = (pal[i]>>5)&0x1f;
      b = pal[i]&0x1f;

      r /=2;  g /= 2; b /=2;
      pal[i] = (r<<10)|(g<<5)|b;
    }

    DC_FlushRange( palData->pRawData, TRANS_POKEICON_COLOR_NUM*2 );
    GX_LoadOBJPltt( palData->pRawData, (POKEICON_PAL_OFFSET+3)*0x20, TRANS_POKEICON_COLOR_NUM*2 );

    GFL_HEAP_FreeMemory(buf);
  }

  GFL_ARC_CloseDataHandle( p_handle );
}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define TRAINER_NAME_WIN_X    (  3 )
#define TRAINER1_NAME_WIN_Y   (  6 )
#define TRAINER2_NAME_WIN_Y   (  7 )

static const u16 obj_pos_tbl[][2]={
  {212,64},
  {187,77},
  {236,77},
  {194,120},
  {224,120},
};

//------------------------------------------------------------------
/**
 * @brief   �Z���A�N�^�[�o�^
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); //���C�����OBJ�ʂn�m
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON ); //�T�u���OBJ�ʂn�m
}



// �͂��E������
#define BMP_YESNO_PX  ( 24 )

#define BMP_YESNO_PY  ( 13 )
#define BMP_YESNO_SX  ( 7 )
#define BMP_YESNO_SY  ( 4 )
#define BMP_YESNO_PAL ( 13 )

//==============================================================================
/**
 * @brief   �^�b�`�Ή�YESNO�E�C���h�E�i���C���̂݁j
 *
 * @param   bgl       GF_BGL_INI
 * @param   y       Y���W
 * @param   yesno_bmp_cgx VRAM�I�t�Z�b�g�ʒu
 * @param   pltt      �p���b�g�w��
 * @param inPassive   �p�b�V�u���邩
 *
 * @retval  TOUCH_SW_WORK *   �͂��E��������ʃ��[�N
 */
//==============================================================================
TOUCH_SW_SYS *WorldTrade_TouchWinYesNoMake( int y, int yesno_bmp_cgx, int pltt, u8 inPassive )
{
  return WorldTrade_TouchWinYesNoMakeEx( y, yesno_bmp_cgx, pltt, GFL_BG_FRAME0_M, inPassive );
}

//==============================================================================
/**
 * @brief   �^�b�`�Ή�YESNO�E�C���h�E(�t���[���w���)
 *
 * @param   bgl       GF_BGL_INI
 * @param   y           Y���W
 * @param   yesno_bmp_cgx VRAM�I�t�Z�b�g�ʒu
 * @param   pltt        �p���b�g�w��
 * @param   frame     �\���t���[���w��
 * @param inPassive   �p�b�V�u���邩
 *
 * @retval  TOUCH_SW_SYS *    �͂��E��������ʃ��[�N
 */
//==============================================================================
TOUCH_SW_SYS *WorldTrade_TouchWinYesNoMakeEx( int y, int yesno_bmp_cgx,
                        int pltt, int frame, u8 inPassive )
{
  TOUCH_SW_SYS *tss = TOUCH_SW_AllocWork( HEAPID_WORLDTRADE );
  TOUCH_SW_PARAM  param;

  if (inPassive){
    u8 target = 0;
    if (frame <= GFL_BG_FRAME3_M){
      target = 1;   //�Ώۂ̓��C�����
    }
    WorldTrade_SetPassive(target);
  }

  param.bg_frame  = frame;
  param.char_offs = yesno_bmp_cgx;
  param.pltt_offs = pltt;
  param.x         = BMP_YESNO_PX;
  param.y         = y;
  param.kt_st     = GFL_APP_KTST_KEY;
  param.key_pos   = 0;
  param.type      = TOUCH_SW_TYPE_S;

  TOUCH_SW_Init( tss, &param );

  return tss;
}

//------------------------------------------------------------------
/**
 * @brief    YesNo�E�B���h�E�Ď�
 *
 * @param   wk      WORLDTRADE_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
u32 WorldTrade_TouchSwMain(WORLDTRADE_WORK* wk)
{
  u32 ret = TOUCH_SW_Main( wk->tss );
  if ( (ret==TOUCH_SW_RET_YES) || (ret==TOUCH_SW_RET_NO) ){
    //�p�b�V�u����
    WorldTrade_ClearPassive();
  }

  return ret;
}


// �I���{�b�N�X�̕�
#define WORLDTRADESELBOX_X  ( 32 - (WORLDTRADE_SELBOX_W+2)) // ��0�̎���20
#define WORLDTRADESELBOX_CGX_SIZE ( WORLDTRADE_SELBOX_W*2 )
#define WORLDTRADESEL_MAX (3)

///�Z���N�g�{�b�N�X�@�w�b�_�f�[�^�\����
static const SELBOX_HEAD_PRM sbox_sel = {
  FALSE,SBOX_OFSTYPE_CENTER,        ///<���[�v�t���O�A����or�Z���^�����O
  0,                    ///<���ڕ\���I�t�Z�b�gX���W(dot)
  GFL_BG_FRAME0_M,            ///<�t���[��No
  10,                   ///<�t���[���p���b�gID
  0,0,                  ///oam BG Pri,Software Pri
  0x3000/32,                ///<�E�B���h�Ecgx
  0x3000/32+WORLDTRADESELBOX_CGX_SIZE*WORLDTRADESEL_MAX,  // �t���[��cgx
  WORLDTRADESELBOX_CGX_SIZE*WORLDTRADESEL_MAX,///<BMP�E�B���h�E��L�L�����N�^�̈�T�C�Y(char)
};

//==============================================================================
/**
 * @brief   �I���{�b�N�X�V�X�e���Ăяo��
 *
 * @param   wk    GTS��ʃ��[�N
 * @param   count ���X�g�̌�
 * @param   y   �\���ʒuY���W
 *
 * @retval  SELBOX_WORK*    �I���{�b�N�X���[�N�̃|�C���^
 */
//==============================================================================
SELBOX_WORK* WorldTrade_SelBoxInit( WORLDTRADE_WORK *wk, u8 frm, int count, int y )
{
  SELBOX_WORK* list_wk;

  wk->SelBoxSys = SelectBoxSys_AllocWork( HEAPID_WORLDTRADE ,NULL );

  // SELBOX�`��o�^
  {
    SELBOX_HEADER head;

    MI_CpuClear8(&head,sizeof(SELBOX_HEADER));

    head.prm      = sbox_sel;
    head.prm.frm  = frm;
    head.list = (const BMPLIST_DATA*)wk->BmpMenuList;
    head.fontHandle = wk->print.font;

    head.count = count;

    list_wk = SelectBoxSetEx( wk->SelBoxSys, &head, NULL, WORLDTRADE_SELBOX_X, y, WORLDTRADE_SELBOX_W, 0, WorldTrade_SelBoxCallback, NULL, TRUE );
  }

  //�p�b�V�u
  WorldTrade_SetPassive(1);

  return list_wk;
}

//--------------------------------------------------------------
/**
 * @brief    �I���{�b�N�X�V�X�e���̃R�[���o�b�N
 *
 * @param   sbox    �I���{�b�N�X���[�N�̃|�C���^
 * @param   cur_pos   �J�[�\���ʒu
 * @param   work    �I���{�b�N�X�ɓo�^���ă��[�N�|�C���^
 * @param   mode    �I�����[�h
 */
//--------------------------------------------------------------
static void WorldTrade_SelBoxCallback(SELBOX_WORK* sbox, u8 cur_pos, void* work, SBOX_CB_MODE mode)
{
    switch(mode){
    case SBOX_CB_MODE_INI:  //��������
        break;
    case SBOX_CB_MODE_MOVE: //�J�[�\���ړ���
    case SBOX_CB_MODE_DECIDE:   //�I��������
    case SBOX_CB_MODE_CANCEL: //�L�����Z��
        PMSND_PlaySE(SEQ_SE_DP_SELECT);
        break;
    }
}

//==============================================================================
/**
 * @brief   �I���{�b�N�X�V�X�e���I��
 *
 * @param   wk    GTS��ʃ��[�N
 * @param   count ���X�g�̌�
 * @param   y   �\���ʒuY���W
 *
 * @retval  SELBOX_WORK*    �I���{�b�N�X���[�N�̃|�C���^
 */
//==============================================================================
void WorldTrade_SelBoxEnd( WORLDTRADE_WORK *wk )
{
  SelectBoxExit( wk->SelBoxWork );
  SelectBoxSys_Free( wk->SelBoxSys );
  //�p�b�V�u����
  WorldTrade_ClearPassive();
}


//------------------------------------------------------------------
/**
 * @brief   ���̎��̃V�[�P���X��\�񂵂Ă���
 *
 * @param   wk      GTS��ʃ��[�N
 * @param   to_seq    ���̃V�[�P���X
 * @param   next_seq  ���̎��̃V�[�P���X
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_SetNextSeq( WORLDTRADE_WORK *wk, int to_seq, int next_seq )
{
  wk->subprocess_seq      = to_seq;
  wk->subprocess_nextseq  = next_seq;
}

//=============================================================================================
/**
 * @brief �T�u�v���Z�X�I�����̎��̍s�������͂���
 *
 * @param   wk        GTS��ʃ��[�N
 * @param   next_process  ���̃V�[�P���X
 */
//=============================================================================================
void WorldTrade_SetNextProcess( WORLDTRADE_WORK *wk, int next_process )
{
  wk->sub_nextprocess = next_process;
}

//==============================================================================
/**
 * @brief   CLACT�̍��W�ύX
 *
 * @param   act   �A�N�^�[�̃|�C���^
 * @param   x   X���W
 * @param   y   Y���W
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_CLACT_PosChangeSub( GFL_CLWK* act, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( act, &pos, CLSYS_DRAW_SUB );

}


//==============================================================================
/**
 * @brief   WIFI�ڑ��󋵎擾
 *
 * @param   none
 *
 * @retval  int   �ڑ����x�擾
 */
//==============================================================================
int WorldTrade_WifiLinkLevel( void )
{
    return WM_LINK_LEVEL_3 - DWC_GetLinkLevel();
}




//==============================================================================
/**
 * @brief   �T�u�v���Z�X���[�h�̐؂�ւ��w��
 *
 * @param   wk      GTS��ʃ��[�N
 * @param   subprccess  �w��̉�ʃ��[�h�iENTER,TITLE,SEACH�Ȃǁj
 * @param   mode    ��ʃ��[�h���̊J�n�����w��(MODE_...)
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SubProcessChange( WORLDTRADE_WORK *wk, int subprccess, int mode )
{
  wk->sub_nextprocess  = subprccess;
  wk->sub_process_mode = mode;
}


//==============================================================================
/**
 * @brief   ���̃T�u�v���Z�X�֑J�ڂ���i��O������Ă����j
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SubProcessUpdate( WORLDTRADE_WORK *wk )
{
  wk->old_sub_process = wk->sub_process;
  wk->sub_process     = wk->sub_nextprocess;

}
//==============================================================================
/**
 * @brief   ��b�X�s�[�h���R���t�B�O�f�[�^����擾����
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  int   ���b�Z�[�W�X�s�[�h
 */
//==============================================================================
int WorldTrade_GetTalkSpeed( WORLDTRADE_WORK *wk )
{
  return MSGSPEED_GetWait();
}

//==============================================================================
/**
 * @brief   ��b�E�C���h�E�̌`����R���t�B�O�f�[�^����擾����
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  WINTYPE ��b�E�C���h�E�̃X�L���ԍ�
 */
//==============================================================================
WINTYPE WorldTrade_GetMesWinType( WORLDTRADE_WORK *wk )
{
  return CONFIG_GetWindowType( wk->param->config );
}

//------------------------------------------------------------------
/**
 * @brief   �Z���A�N�^�[������
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCLACT( WORLDTRADE_WORK *wk )
{
  static const GFL_DISP_VRAM tbl =
  {
    GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

    GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

    GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

    GX_VRAM_TEX_0_B,        // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_32K,
    GX_OBJVRAMMODE_CHAR_1D_32K,
  };

  // VRAM �o���N�ݒ�
  VramBankSet( &tbl );

  // CellActor�V�X�e��������
  InitCellActor(wk, &tbl);

  // CellActro�\���o�^
  SetCellActor(wk);

}

//------------------------------------------------------------------
/**
 * @brief   �Z���A�N�^�[���
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeCLACT( WORLDTRADE_WORK *wk )
{
  int i;

  // ���j�I���n�a�i�O���t�B�b�N�f�[�^���
  FreeFieldObjData( wk );

  // �L�����E�p���b�g�E�Z���E�Z���A�j���̃��\�[�X�}�l�[�W���[�j��
  GFL_CLGRP_PLTT_Release( wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] );
  GFL_CLGRP_CGR_Release( wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[SUB_LCD][CLACT_U_CELL_RES] );

  GFL_CLGRP_PLTT_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] );
  GFL_CLGRP_CGR_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] );

  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete(wk->clactSet);
  wk->clactSet = NULL;

  //OAM�����_���[�j��
  GFL_CLACT_SYS_Delete();


}


//------------------------------------------------------------------
/**
 * @brief   0�ɂȂ�܂łP���炷
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void ServerWaitTimeFunc( WORLDTRADE_WORK *wk )
{

  if(wk->serverWaitTime){
    wk->serverWaitTime--;
  }
}

//==============================================================================
/**
 * @brief   �Psync�ɂP�g���C����BOX�̐����m�F���Ă����X�^�[�g
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_BoxPokeNumGetStart( WORLDTRADE_WORK *wk )
{
  wk->boxSearchFlag = 1;
  wk->boxPokeNum    = 0;
}


//------------------------------------------------------------------
/**
 * @brief   �Psync�ɂP�g���C����BOX�̐����m�F���Ă���(�풓�����j
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BoxPokeNumGet( WORLDTRADE_WORK *wk )
{
  if(wk->boxSearchFlag){
//    wk->boxPokeNum += BOXDAT_GetPokeExistCount( wk->param->mybox, wk->boxSearchFlag-1 );
    wk->boxPokeNum  = 0;
    wk->boxSearchFlag++;
    if(wk->boxSearchFlag==19){
      wk->boxSearchFlag = 0;
      OS_Printf("BOX������%d\n",wk->boxPokeNum);
    }

  }

}



//------------------------------------------------------------------
/**
 * @brief   ���ԃA�C�R���ǉ�
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TimeIconAdd( WORLDTRADE_WORK *wk )
{
  wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, WORLDTRADE_MESFRAME_CHR );
}

//------------------------------------------------------------------
/**
 * @brief   ���ԃA�C�R�������iNULL�`�F�b�N����j
 *
 * @param   wk    GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TimeIconDel( WORLDTRADE_WORK *wk )
{
  if(wk->timeWaitWork!=NULL){
    TimeWaitIconDel(wk->timeWaitWork);
    wk->timeWaitWork = NULL;
  }
}

//==============================================================================
/**
 * @brief   �Z���A�N�^�[�̍��W�ύX�i�ȈՏ����Łj
 *
 * @param   ptr   clact_work_ptr
 * @param   x   X
 * @param   y   Y
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_CLACT_PosChange( GFL_CLWK* ptr, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( ptr, &pos, CLSYS_DRAW_MAIN );

}

//------------------------------------------------------------------
/**
 * @brief �p�b�V�u��ԃZ�b�g
 * @param inIsMain    �Ώۂ����C����ʂ��H  YES:1 NO:0
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_SetPassive(u8 inIsMain)
{
  if (inIsMain){
    G2_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }else{
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }
}

//------------------------------------------------------------------
/**
 * @brief �p�b�V�u��ԃZ�b�g  mypoke�p
 * @param inIsMain    �Ώۂ����C����ʂ��H  YES:1 NO:0
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_SetPassiveMyPoke(u8 inIsMain)
{
  if (inIsMain){
    G2_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }else{
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }
}

//------------------------------------------------------------------
/**
 * @brief �p�b�V�u��ԉ���
 * @param none
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_ClearPassive(void)
{
  G2_BlendNone();
  G2S_BlendNone();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�X�e���n��������
 *
 *  @param  WORLDTRADE_WORK *wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void WorldTrade_InitSystem( WORLDTRADE_WORK *wk )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );


  InitCLACT( wk );

  GFL_BG_Init( HEAPID_WORLDTRADE );
  GFL_BMPWIN_Init( HEAPID_WORLDTRADE );


  // BG SYSTEM
  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  WorldTrade_WndSetting();

  // VBlank�֐��Z�b�g
  if( wk->vblank_task == NULL )
  { 
    wk->vblank_task = GFUser_VIntr_CreateTCB(VBlankFunc, wk, 0);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�X�e���n��j��
 *
 *  @param  WORLDTRADE_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WorldTrade_ExitSystem( WORLDTRADE_WORK *wk )
{
  // VBlank�֐��Z�b�g
  if( wk->vblank_task )
  { 
    GFL_TCB_DeleteTask( wk->vblank_task );
    wk->vblank_task = NULL;
  }

  G2_BlendNone();
  G2S_BlendNone();

  FreeCLACT( wk );

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GX_SetVisibleWnd(GX_WNDMASK_NONE);
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);


}

