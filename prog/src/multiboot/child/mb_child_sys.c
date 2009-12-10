//======================================================================
/**
 * @file	mb_child_sys.h
 * @brief	�}���`�u�[�g�E�q�@���C��
 * @author	ariizumi
 * @data	09/11/16
 *
 * ���W���[�����FMB_CHILD
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "mb_child_gra.naix"

#include "multiboot/mb_child_sys.h"
#include "multiboot/mb_select_sys.h"
#include "multiboot/mb_capture_sys.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/mb_util.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_data_main.h"
#include "multiboot/mb_local_def.h"

#include "../../test/ariizumi/ari_debug.h"


//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CHILD_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_CHILD_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_CHILD_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCS_FADEIN,
  MCS_WAIT_FADEIN,
  MCS_FADEOUT,
  MCS_WAIT_FADEOUT,
  
  MCS_WAIT_COMM_INIT,
  MCS_WAIT_CONNECT,
  MCS_CHECK_ROM,
  MCS_SELECT_ROM, //�f�o�b�O�p
  MCS_LOAD_DATA,
  MCS_DATA_CONV,
  
  //�I�����
  MCS_SELECT_FADEOUT,
  MCS_SELECT_FADEOUT_WAIT,
  MCS_SELECT_MAIN,
  MCS_SELECT_TERM,
  MCS_SELECT_FADEIN,
  MCS_SELECT_FADEIN_WAIT,

  //�Q�[���f�[�^�����҂�
  MCS_WAIT_GAME_DATA,
  
  //�ߊl�Q�[��
  MCS_CAPTURE_FADEOUT,
  MCS_CAPTURE_FADEOUT_WAIT,
  MCS_CAPTURE_MAIN,
  MCS_CAPTURE_TERM,
  MCS_CAPTURE_FADEIN,
  MCS_CAPTURE_FADEIN_WAIT,

  //�]���E�ۑ�
  MCS_TRANS_POKE_INIT,
  MCS_TRANS_POKE_SEND,

  MCS_SAVE_INIT,
  MCS_SAVE_MAIN,
  MCS_SAVE_TERM,

  MCS_ERROR,
  
}MB_CHILD_STATE;

typedef enum
{
  MCSS_SAVE_WAIT_SAVE_INIT,
  MCSS_SAVE_SYNC_SAVE_INIT,
  MCSS_SAVE_INIT,
  MCSS_SAVE_WAIT_FIRST,
  MCSS_SAVE_WAIT_FIRST_SYNC,
  MCSS_SAVE_FIRST_SAVE_LAST,
  MCSS_SAVE_WAIT_SECOND,
  MCSS_SAVE_WAIT_SECOND_SYNC,
  MCSS_SAVE_SECOND_SAVE_LAST,
  MCSS_SAVE_WAIT_LAST_SAVE,
  MCSS_SAVE_WAIT_FINISH_SAVE_SYNC,
}MB_CHILD_SUB_STATE;
  
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  
  MB_COMM_INIT_DATA *initData;
  void  *sndData;
  
  MB_CHILD_STATE  state;
  MB_CHILD_SUB_STATE  subState;
  GFL_PROCSYS   *procSys;
  u8 captureNum;
  u8 saveWaitCnt;
  
  POKEMON_PASO_PARAM *boxPoke[MB_POKE_BOX_TRAY][MB_POKE_BOX_POKE];
  STRCODE *boxName[MB_POKE_BOX_TRAY];
  
  DLPLAY_CARD_TYPE cardType;

  MB_MSG_WORK *msgWork;
  MB_COMM_WORK *commWork;
  MB_DATA_WORK *dataWork;
  
  MB_SELECT_INIT_WORK selInitWork;
  MB_CAPTURE_INIT_WORK capInitWork;
  
}MB_CHILD_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MB_CHILD_Init( MB_CHILD_WORK *work );
static void MB_CHILD_Term( MB_CHILD_WORK *work );
static const BOOL MB_CHILD_Main( MB_CHILD_WORK *work );

static void MB_CHILD_InitGraphic( MB_CHILD_WORK *work );
static void MB_CHILD_TermGraphic( MB_CHILD_WORK *work );
static void MB_CHILD_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_CHILD_LoadResource( MB_CHILD_WORK *work );

static void MB_CHILD_DataConvert( MB_CHILD_WORK *work );

static void MB_CHILD_SaveInit( MB_CHILD_WORK *work );
static void MB_CHILD_SaveTerm( MB_CHILD_WORK *work );
static void MB_CHILD_SaveMain( MB_CHILD_WORK *work );


static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B ,           // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,        // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,         // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static void MB_CHILD_Init( MB_CHILD_WORK *work )
{
  u8 i,j;
  work->state = MCS_FADEIN;
  work->msgWork = NULL;

  MB_CHILD_InitGraphic( work );
  MB_CHILD_LoadResource( work );
  work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_MSG , FILE_MSGID_MB );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  work->commWork = MB_COMM_CreateSystem( work->heapId );
  
  work->sndData = GFL_ARC_UTIL_Load( ARCID_MB_CHILD ,
                                     NARC_mb_child_gra_wb_sound_palpark_sdat ,
                                     FALSE ,
                                     work->heapId );
                                     
  PMSND_InitMultiBoot( work->sndData );
  
  work->procSys = GFL_PROC_LOCAL_boot( work->heapId );
  work->captureNum = 0;
  
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      work->boxPoke[i][j] = GFL_HEAP_AllocClearMemory( work->heapId , POKETOOL_GetPPPWorkSize() );
    }
    work->boxName[i] = GFL_HEAP_AllocClearMemory( work->heapId , 20 );  //8����+EOM
  }
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_CHILD_Term( MB_CHILD_WORK *work )
{
  u8 i,j;

  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    GFL_HEAP_FreeMemory( work->boxName[i] );
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      GFL_HEAP_FreeMemory( work->boxPoke[i][j] );
    }
  }

  GFL_PROC_LOCAL_Exit( work->procSys );
  
  PMSND_Exit();
  GFL_HEAP_FreeMemory( work->sndData );
  MB_COMM_DeleteSystem( work->commWork );

  MB_MSG_MessageTerm( work->msgWork );
  MB_CHILD_TermGraphic( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static const BOOL MB_CHILD_Main( MB_CHILD_WORK *work )
{

  MB_COMM_UpdateSystem( work->commWork );

  switch( work->state )
  {
  case MCS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    //PMSND_PlayBGM( SEQ_BGM_PALPARK_BOX );
    work->state = MCS_WAIT_FADEIN;
    break;
    
  case MCS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      MB_COMM_InitComm( work->commWork );
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_01 );
      work->state = MCS_WAIT_COMM_INIT;
    }
    break;
  case MCS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_WAIT_FADEOUT;
    break;
  case MCS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  //--------------------------------------------------------
  //�N���`�`�F�b�N
  case MCS_WAIT_COMM_INIT:
    if( MB_COMM_IsInitComm(work->commWork) == TRUE )
    {
      //�e�@���
      if( MB_IsMultiBootChild() == FALSE )
      {
#if DEB_ARI
        //����05053056��MacAddress(��ł�)
        u8 addTemp[6] = {0x00,0x09,0xbf,0xf4,0x33,0x15};
        MB_COMM_InitChild( work->commWork , addTemp );
        MB_TPrintf("Quick boot!!\n");
#else
        GF_ASSERT_MSG(0,"This DS is not multiboot child!!\n");
#endif
      }
      else
      {
        const MBParentBssDesc *desc = MB_GetMultiBootParentBssDesc();
        MB_COMM_InitChild( work->commWork , (u8*)desc->bssid );
      }
      
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_01 );
      work->state = MCS_WAIT_CONNECT;
    }
    break;
  
  case MCS_WAIT_CONNECT:
    if( MB_COMM_IsPostInitData( work->commWork ) == TRUE )
    {
      work->initData = MB_COMM_GetInitData( work->commWork );
      work->state = MCS_CHECK_ROM;
    }
    break;
  case MCS_CHECK_ROM:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      work->dataWork = MB_DATA_InitSystem( work->heapId );
      work->cardType = MB_DATA_GetCardType( work->dataWork );
      if( work->cardType == CARD_TYPE_DUMMY )
      {
        MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_DEB_01 );
        work->state = MCS_SELECT_ROM;
      }
      else
      if( work->cardType == CARD_TYPE_INVALID )
      {
        //ROM�Ⴄ�I
        MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_02 );
        work->state = MCS_ERROR;
      }
      else
      {
        work->state = MCS_LOAD_DATA;
      }
    }
    break;
  
  //�f�o�b�O�pROM��ޑI��
  case MCS_SELECT_ROM:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_DP );
      work->state = MCS_LOAD_DATA;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_PT );
      work->state = MCS_LOAD_DATA;
      work->cardType = CARD_TYPE_PT;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_GS );
      work->state = MCS_LOAD_DATA;
    }
    break;
  
  case MCS_LOAD_DATA:
    if( MB_DATA_LoadDataFirst( work->dataWork ) == TRUE )
    {
      work->state = MCS_DATA_CONV;
    }
    break;

  case MCS_DATA_CONV:
    MB_CHILD_DataConvert( work );
    work->state = MCS_SELECT_FADEOUT;
    break;
  
  //--------------------------------------------------------
  //�{�b�N�X�Z���N�g���
  case MCS_SELECT_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_SELECT_FADEOUT_WAIT;
    MB_COMM_SetChildState( work->commWork , MCCS_SELECT_BOX );
    break;
    
  case MCS_SELECT_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i,j;
      MB_MSG_MessageTerm( work->msgWork );
      MB_CHILD_TermGraphic( work );
      work->msgWork = NULL;
      
      //InitWork��ppp�͍ŏ��ɓ���Ă���
      work->selInitWork.parentHeap = work->heapId;
      work->selInitWork.cardType = work->cardType;
      for( i=0;i<MB_POKE_BOX_TRAY;i++ )
      {
        for( j=0;j<MB_POKE_BOX_POKE;j++ )
        {
          work->selInitWork.boxData[i][j] = work->boxPoke[i][j];
        }
        work->selInitWork.boxName[i] = work->boxName[i];
      }
      
      GFL_PROC_LOCAL_CallProc( work->procSys , 
                               NO_OVERLAY_ID ,
                               &MultiBootSelect_ProcData ,
                               &work->selInitWork );
      
      work->state = MCS_SELECT_MAIN;
    }
    break;
    
  case MCS_SELECT_MAIN:
    {
      const GFL_PROC_MAIN_STATUS ret = GFL_PROC_LOCAL_Main( work->procSys );
      if( ret == GFL_PROC_MAIN_NULL )
      {
        work->state = MCS_SELECT_TERM;
      }
    }
    break;
    
  case MCS_SELECT_TERM:
    work->state = MCS_SELECT_FADEIN;

    MB_CHILD_InitGraphic( work );
    MB_CHILD_LoadResource( work );
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_MSG , FILE_MSGID_MB );
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
    MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_03 );

    break;
    
  case MCS_SELECT_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_SELECT_FADEIN_WAIT;
    break;

  case MCS_SELECT_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( MB_COMM_IsPostGameData( work->commWork ) == TRUE )
      {
        work->state = MCS_CAPTURE_FADEOUT;
      }
      else
      {
        work->state = MCS_WAIT_GAME_DATA;
        MB_COMM_SetChildState( work->commWork , MCCS_WAIT_GAME_DATA );
      }
    }
    break;
  //--------------------------------------------------------
  //�Q�[���f�[�^�����҂�
    
  case MCS_WAIT_GAME_DATA:
    if( MB_COMM_IsPostGameData( work->commWork ) == TRUE )
    {
      work->state = MCS_CAPTURE_FADEOUT;
    }
    break;

  //--------------------------------------------------------
  //�ߊl�Q�[��
  case MCS_CAPTURE_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_CAPTURE_FADEOUT_WAIT;
    MB_COMM_SetChildState( work->commWork , MCCS_CAP_GAME );
    break;
    
  case MCS_CAPTURE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i;
      MB_MSG_MessageTerm( work->msgWork );
      MB_CHILD_TermGraphic( work );
      PMSND_Exit();

      work->msgWork = NULL;
      
      work->capInitWork.parentHeap = work->heapId;
      work->capInitWork.cardType = work->cardType;
      work->capInitWork.arcHandle = GFL_ARC_OpenDataHandleByMemory( 
                                              GFL_NET_LDATA_GetPostData( 0 ) ,
                                              GFL_NET_LDATA_GetPostDataSize( 0 ) ,
                                              work->heapId );
      MB_TPrintf("fileNum[%d]\n",GFL_ARC_GetDataFileCntByHandle(work->capInitWork.arcHandle));
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        const u8 tray = work->selInitWork.selectPoke[i][0];
        const u8 idx  = work->selInitWork.selectPoke[i][1];
        work->capInitWork.ppp[i] = work->boxPoke[tray][idx];
      }
      
      GFL_PROC_LOCAL_CallProc( work->procSys , 
                               NO_OVERLAY_ID ,
                               &MultiBootCapture_ProcData ,
                               &work->capInitWork );
      
      work->state = MCS_CAPTURE_MAIN;
    }
    break;
    
  case MCS_CAPTURE_MAIN:
    {
      const GFL_PROC_MAIN_STATUS ret = GFL_PROC_LOCAL_Main( work->procSys );
      if( ret == GFL_PROC_MAIN_NULL )
      {
        work->state = MCS_CAPTURE_TERM;
      }
    }
    break;
    
  case MCS_CAPTURE_TERM:
    work->state = MCS_CAPTURE_FADEIN;

    PMSND_InitMultiBoot( work->sndData );
    MB_CHILD_InitGraphic( work );
    MB_CHILD_LoadResource( work );
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_MSG , FILE_MSGID_MB );
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );

    break;
    
  case MCS_CAPTURE_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_CAPTURE_FADEIN_WAIT;
    break;

  case MCS_CAPTURE_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i;
      work->captureNum = 0;
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        if( work->capInitWork.isCapture[i] == TRUE )
        {
          work->captureNum++;
        }
      }
      if( work->captureNum > 1 )
      {
        work->state = MCS_TRANS_POKE_INIT;
        MB_COMM_SetChildState( work->commWork , MCCS_SEND_POKE );
      }
      else
      {
        //TODO ����C��
        work->state = MCS_TRANS_POKE_INIT;
      }
    }
    break;
  //--------------------------------------------------------
  //�]���E�ۑ��t�F�C�Y
  case MCS_TRANS_POKE_INIT:
    {
      u8 i;
      MB_COMM_ClearSendPokeData( work->commWork );
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        if( work->capInitWork.isCapture[i] == TRUE )
        {
          const u8 tray = work->selInitWork.selectPoke[i][0];
          const u8 idx  = work->selInitWork.selectPoke[i][1];
          MB_COMM_AddSendPokeData( work->commWork , work->capInitWork.ppp[i] );
          
          //���f�[�^��ppp������
          MB_DATA_ClearBoxPPP( work->dataWork , tray , idx );
        }
      }
    }
    work->state = MCS_TRANS_POKE_SEND;
    break;
    
  case MCS_TRANS_POKE_SEND:
    if( MB_COMM_Send_PokeData( work->commWork ) == TRUE )
    {
      work->state = MCS_SAVE_INIT;
    }
    break;

  case MCS_SAVE_INIT:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE )
    {
      MB_CHILD_SaveInit( work );
      work->state = MCS_SAVE_MAIN;
      work->subState = MCSS_SAVE_WAIT_SAVE_INIT;
    }
    break;

  case MCS_SAVE_MAIN:
    MB_CHILD_SaveMain( work );
    break;

  case MCS_SAVE_TERM:
    MB_CHILD_SaveTerm( work );
    break;

  }
  
  if( work->msgWork != NULL )
  {
    MB_MSG_MessageMain( work->msgWork );
  }
  PMSND_Main();
  
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START &&
      GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void MB_CHILD_InitGraphic( MB_CHILD_WORK *work )
{
  GFL_DISP_SetDispOn();
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  GFL_DISP_SetBank( &vramBank );

  //BG�n�̏�����
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    // BG1 MAIN (���b�Z�[�W
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (�L����
    //static const GFL_BG_BGCNT_HEADER header_main2 = {
    //  0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
    //  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    //  GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x0C000,
    //  GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    //};
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_CHILD_SetupBgFunc( &header_main1 , MB_CHILD_FRAME_MSG , GFL_BG_MODE_TEXT );
    //MB_CHILD_SetupBgFunc( &header_main2 , LTVT_FRAME_CHARA , GFL_BG_MODE_TEXT );
    MB_CHILD_SetupBgFunc( &header_main3 , MB_CHILD_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_CHILD_SetupBgFunc( &header_sub3  , MB_CHILD_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  
}

static void MB_CHILD_TermGraphic( MB_CHILD_WORK *work )
{
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_BG );
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MB_CHILD_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void MB_CHILD_LoadResource( MB_CHILD_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_CHILD , work->heapId );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_child_gra_bg_sub_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_child_gra_bg_sub_NCGR ,
                    MB_CHILD_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_child_gra_bg_sub_NSCR , 
                    MB_CHILD_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  
  //����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_child_gra_bg_main_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_child_gra_bg_main_NCGR ,
                    MB_CHILD_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_child_gra_bg_main_NSCR , 
                    MB_CHILD_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  
  GFL_ARC_CloseDataHandle( arcHandle );
}

//--------------------------------------------------------------
//  �f�[�^�̕ϊ�
//--------------------------------------------------------------
static void MB_CHILD_DataConvert( MB_CHILD_WORK *work )
{
  u8 i,j;
  //PPP
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      void *pppSrc = MB_DATA_GetBoxPPP(work->dataWork,i,j);
      MB_UTIL_ConvertPPP( pppSrc , work->boxPoke[i][j] , work->cardType );
    }
  }
  //�{�b�N�X��
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    u16 *strSrc = MB_DATA_GetBoxName( work->dataWork,i );
    
    MB_UTIL_ConvertStr( strSrc , work->boxName[i] , 10 , work->cardType );
  }
}

#pragma mark [>save func
//--------------------------------------------------------------
//  �Z�[�u������
//--------------------------------------------------------------
static void MB_CHILD_SaveInit( MB_CHILD_WORK *work )
{
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_04 );

}

//--------------------------------------------------------------
//  �Z�[�u�J��
//--------------------------------------------------------------
static void MB_CHILD_SaveTerm( MB_CHILD_WORK *work )
{
}

//--------------------------------------------------------------
//  �Z�[�u�X�V
//--------------------------------------------------------------
static void MB_CHILD_SaveMain( MB_CHILD_WORK *work )
{
	MB_DATA_SaveData( work->dataWork );

  switch( work->subState )
  {
  case MCSS_SAVE_WAIT_SAVE_INIT:
    if( MB_MSG_CheckPrintQueIsFinish( work->msgWork ) == TRUE &&
        MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_READY_START_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_SYNC_SAVE_INIT;
      }
    }
    break;
  case MCSS_SAVE_SYNC_SAVE_INIT:
    if( MB_COMM_GetIsPermitStartSave( work->commWork ) == TRUE )
    {
      work->subState = MCSS_SAVE_INIT;
    }
    break;
    
  case MCSS_SAVE_INIT:
    work->subState = MCSS_SAVE_WAIT_FIRST;
    MB_TPrintf( "MB_Parent Save Start!\n" );
    break;

  case MCSS_SAVE_WAIT_FIRST:
		if( MB_DATA_IsFinishSaveFirst( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_FIRST_SAVE , 0 ) == TRUE )
      {
  			work->subState = MCSS_SAVE_WAIT_FIRST_SYNC;
        MB_TPrintf( "MB_Parent Finish First!\n" );
  		}
		}
    break;

  case MCSS_SAVE_WAIT_FIRST_SYNC:
    work->subState = MCSS_SAVE_FIRST_SAVE_LAST;
		work->saveWaitCnt = 0;
    break;

  case MCSS_SAVE_FIRST_SAVE_LAST:
    if( MB_COMM_GetIsPermitFirstSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_SECOND;
  			MB_DATA_PermitLastSaveFirst( work->dataWork );
        MB_TPrintf( "MB_Parent FirstLast Save!\n" );
      }
    }
    break;

  case MCSS_SAVE_WAIT_SECOND:
		if( MB_DATA_IsFinishSaveSecond( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_SECOND_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_SECOND_SYNC;
        MB_TPrintf( "MB_Parent Finish Second!\n" );
  		}
		}
    break;

  case MCSS_SAVE_WAIT_SECOND_SYNC:
    work->subState = MCSS_SAVE_SECOND_SAVE_LAST;
		work->saveWaitCnt = 0;
    break;

  case MCSS_SAVE_SECOND_SAVE_LAST:
    if( MB_COMM_GetIsPermitSecondSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_LAST_SAVE;
        MB_TPrintf( "MB_Parent SecondLast Save!\n" );
  			MB_DATA_PermitLastSaveSecond( work->dataWork );
      }
    }
    break;

  case MCSS_SAVE_WAIT_LAST_SAVE:
		if( MB_DATA_IsFinishSaveAll( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_FINISH_SAVE_SYNC;
  		}
    }
    break;
  case MCSS_SAVE_WAIT_FINISH_SAVE_SYNC:
    if( MB_COMM_GetIsPermitFinishSave( work->commWork ) == TRUE )
    {
      work->state = MCS_SAVE_TERM;
      MB_TPrintf( "MB_Parent Finish All Save Seq!!!\n" );
    }
    break;
  }
}

#pragma mark [>proc func
static GFL_PROC_RESULT MB_CHILD_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CHILD_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CHILD_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBootChild_ProcData =
{
  MB_CHILD_ProcInit,
  MB_CHILD_ProcMain,
  MB_CHILD_ProcTerm
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work;
  
  MB_TPrintf("LeastAppHeap[%x]",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x100000 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT_DATA, 0x80000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_CHILD_WORK), HEAPID_MULTIBOOT );

  work->heapId = HEAPID_MULTIBOOT;
  
  MB_CHILD_Init( work );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work = mywk;
  
  MB_CHILD_Term( work );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work = mywk;
  const BOOL ret = MB_CHILD_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}



