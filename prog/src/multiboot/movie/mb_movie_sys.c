//======================================================================
/**
 * @file	mb_movie_sys.h
 * @brief	�}���`�u�[�g�E�q�@���C���E�f��z�M�]�����u
 * @author	ariizumi
 * @data	10/03/15
 *
 * ���W���[�����FMB_MOVIE
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/wipe.h"
#include "system/net_err.h"
#include "poke_tool/pokepara_conv.h"
#include "poke_tool/poke_memo.h"
#include "../../net_app/connect_anm.h"

#include "arc_def.h"
#include "mb_child_gra.naix"
#include "wifi_login.naix"
#include "multiboot/wb_sound_palpark.sadl"
#include "sound/pm_sndsys.h"


#include "multiboot/mb_util.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_data_main.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/mb_local_def.h"

#include "../../test/ariizumi/ari_debug.h"


//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_MOVIE_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_MOVIE_FRAME_BG  (GFL_BG_FRAME2_M)
#define MB_MOVIE_FRAME_BG2 (GFL_BG_FRAME3_M)

#define MB_MOVIE_FRAME_SUB_MSG  (GFL_BG_FRAME1_S)
#define MB_MOVIE_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define MB_MOVIE_FIRST_TIMEOUT (60*15) //�ʏ�5�b�ȓ��Őڑ�����̂��ł��Ȃ������B

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

  MCS_LOAD_ERROR,
  
  MCS_COUNT_POKE,
  MCS_SEND_COUNT_POKE,
  MCS_WAIT_SEND_POKE_SELECT,

  //�|�P�����]��
  MCS_TRANS_POKE_INIT,
  MCS_TRANS_POKE_SEND,
  MCS_TRANS_POKE_SEND_WAIT,

  MCS_TRANS_POKE_SEND_FINISH,

  MCS_CHECK_LOCK_CAPSULE,
  MCS_POST_TRANS_LOCK_CAPSULE,
  
  MCS_CHECK_SAVE,
  
  MCS_CHECK_ROM_CRC_LOAD,
  MCS_CHECK_ROM_CRC,

  MCS_SAVE_INIT,
  MCS_SAVE_MAIN,
  MCS_SAVE_TERM,

  MCS_SAVE_FINISH_WAIT,

  MCS_WAIT_NEXT_GAME_ERROR_MSG,

  MCS_WAIT_EXIT_COMM,
  MCS_EXIT_GAME,
  
  MCS_ERROR,
  
}MB_MOVIE_STATE;

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
}MB_MOVIE_SUB_STATE;
  
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  
  MB_COMM_INIT_DATA *initData;
  void  *sndData;
  
  MB_MOVIE_STATE  state;
  MB_MOVIE_SUB_STATE  subState;
  GFL_PROCSYS   *procSys;
  u32 moviePokeNum;
  u8  saveWaitCnt;
  u16 timeoutCnt;
  
  BOOL isSendPoke;
  BOOL isSendCapsule;
  
  POKEMON_PASO_PARAM *boxPoke[MB_POKE_BOX_TRAY][MB_POKE_BOX_POKE];
  STRCODE *boxName[MB_POKE_BOX_TRAY];
  
  DLPLAY_CARD_TYPE cardType;

  MB_MSG_WORK *msgWork;
  MB_COMM_WORK *commWork;
  MB_DATA_WORK *dataWork;
  
  BOOL isNetErr;
  BOOL isInitCellSys;
  
  CONNECT_BG_PALANM bgAnmWork;
  
}MB_MOVIE_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MB_MOVIE_Init( MB_MOVIE_WORK *work );
static void MB_MOVIE_Term( MB_MOVIE_WORK *work );
static const BOOL MB_MOVIE_Main( MB_MOVIE_WORK *work );
static void MB_MOVIE_VBlankFunc(GFL_TCB *tcb, void *wk );
static void MB_MOVIE_VSyncMovie( void );

static void MB_MOVIE_InitGraphic( MB_MOVIE_WORK *work );
static void MB_MOVIE_TermGraphic( MB_MOVIE_WORK *work );
static void MB_MOVIE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_MOVIE_LoadResource( MB_MOVIE_WORK *work );

static void MB_MOVIE_DataConvert( MB_MOVIE_WORK *work );
static const u16 MB_MOVIE_GetLeastPoke( MB_MOVIE_WORK *work );
static const u32 MB_MOVIE_GetMoviePokeNum( MB_MOVIE_WORK *work );
static const BOOL MB_MOVIE_CheckMoviePoke( POKEMON_PASO_PARAM *ppp );


static const BOOL MB_MOVIE_ErrCheck( MB_MOVIE_WORK *work , const BOOL noFade );

static void MB_MOVIE_SaveInit( MB_MOVIE_WORK *work );
static void MB_MOVIE_SaveTerm( MB_MOVIE_WORK *work );
static void MB_MOVIE_SaveMain( MB_MOVIE_WORK *work );

CONNECT_BG_PALANM *staticBgAnmWork;

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
static void MB_MOVIE_Init( MB_MOVIE_WORK *work )
{
  u8 i,j;
  work->state = MCS_FADEIN;
  work->dataWork = NULL;
  work->isInitCellSys = FALSE;

  MB_MOVIE_InitGraphic( work );
  MB_MOVIE_LoadResource( work );
  work->msgWork = MB_MSG_MessageInit( work->heapId , MB_MOVIE_FRAME_MSG , MB_MOVIE_FRAME_MSG , FILE_MSGID_MB , TRUE , FALSE );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  work->commWork = MB_COMM_CreateSystem( work->heapId );
  
  work->sndData = GFL_ARC_UTIL_Load( ARCID_MB_CHILD ,
                                     NARC_mb_child_gra_wb_sound_palpark_sdat ,
                                     FALSE ,
                                     work->heapId );
                                     
  work->isNetErr = FALSE;
  PMSND_InitMultiBoot( work->sndData );
  
  work->procSys = GFL_PROC_LOCAL_boot( work->heapId );
  work->moviePokeNum = 0;
  work->timeoutCnt = 0;
  work->initData = NULL;
  work->isSendPoke = FALSE;
  work->isSendCapsule = FALSE;

  
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      work->boxPoke[i][j] = GFL_HEAP_AllocClearMemory( work->heapId , POKETOOL_GetPPPWorkSize() );
    }
    work->boxName[i] = GFL_HEAP_AllocClearMemory( work->heapId , 20 );  //8����+EOM
  }

  work->vBlankTcb = GFUser_VIntr_CreateTCB( MB_MOVIE_VBlankFunc , work , 8 );

  PMSND_PlayBGM( SEQ_BGM_PALPARK_BOX );

  GFUser_SetVIntrFunc( MB_MOVIE_VSyncMovie );
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE , work->heapId );
  GFL_NET_ReloadIcon();
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_MOVIE_Term( MB_MOVIE_WORK *work )
{
  u8 i,j;
  GFL_TCB_DeleteTask( work->vBlankTcb );
  GFUser_ResetVIntrFunc();

  ConnectBGPalAnm_End( &work->bgAnmWork );

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

  if( work->dataWork != NULL )
  {
    MB_DATA_TermSystem( work->dataWork );
  }

  if( work->msgWork != NULL )
  {
    MB_MSG_MessageTerm( work->msgWork );
  }
  MB_MOVIE_TermGraphic( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static const BOOL MB_MOVIE_Main( MB_MOVIE_WORK *work )
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
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_MOVIE_01 );
      MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
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
      if( MB_IsMultiBootChild() == FALSE ||
          GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
      {
        GF_ASSERT_MSG(0,"This DS is not multiboot child!!\n");
      }
      else
      {
        const MBParentBssDesc *desc = MB_GetMultiBootParentBssDesc();
        MB_COMM_InitChild( work->commWork , (u8*)desc->bssid );
      }
      
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_MOVIE_01 );
      MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
      work->state = MCS_WAIT_CONNECT;
    }
    break;
  
  case MCS_WAIT_CONNECT:
    work->timeoutCnt++;
    if( work->timeoutCnt >= MB_MOVIE_FIRST_TIMEOUT )
    {
      //�����ڑ��^�C���A�E�g
      //MsgSpeed����M�ł��ĂȂ��I
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_07 , 1 );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    else
    if( MB_MOVIE_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_COMM_IsPostInitData( work->commWork ) == TRUE )
      {
        work->initData = MB_COMM_GetInitData( work->commWork );
        work->state = MCS_CHECK_ROM;
      }
    }
    break;
  case MCS_CHECK_ROM:
    work->dataWork = MB_DATA_InitSystem( work->heapId );
    work->cardType = MB_DATA_GetCardType( work->dataWork );
    MB_DATA_ResetSaveLoad( work->dataWork );
    if( work->cardType == CARD_TYPE_DUMMY )
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_MOVIE_DEB_01 );
      work->state = MCS_SELECT_ROM;
    }
    else
    if( work->cardType == CARD_TYPE_INVALID )
    {
      //ROM�Ⴄ�I
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_02 , work->initData->msgSpeed );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    else
    {
      work->state = MCS_LOAD_DATA;
    }
    break;
  
  //�f�o�b�O�pROM��ޑI��
  case MCS_SELECT_ROM:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_DP );
      work->state = MCS_LOAD_DATA;
      //work->cardType = CARD_TYPE_DP;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_PT );
      work->state = MCS_LOAD_DATA;
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
      if( MB_DATA_GetErrorState( work->dataWork ) == DES_NONE )
      {
        work->state = MCS_DATA_CONV;
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_09 , work->initData->msgSpeed );
      }
      else
      {
        work->state = MCS_LOAD_ERROR;
      }
    }
    break;

  case MCS_DATA_CONV:
    MB_MOVIE_DataConvert( work );
    work->state = MCS_COUNT_POKE;
    MB_MOVIE_ErrCheck( work , FALSE );
    break;
    
  case MCS_LOAD_ERROR:
    //�ǂݍ��ݎ��s
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_06 , work->initData->msgSpeed );
    MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
    MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
    work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    break;
    
  //--------------------------------------------------------
  //�|�P�����`�F�b�N
  case MCS_COUNT_POKE:
    work->moviePokeNum = MB_MOVIE_GetMoviePokeNum( work );
    work->state = MCS_SEND_COUNT_POKE;
    break;
  
  case MCS_SEND_COUNT_POKE:
    {
      const BOOL flg = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_POKE_NUM , work->moviePokeNum );
      if( flg == TRUE )
      {
        if( work->moviePokeNum > 0 )
        {
          work->state = MCS_WAIT_SEND_POKE_SELECT;
        }
        else
        {
          work->state = MCS_CHECK_LOCK_CAPSULE;
        }
      }
    }
    break;

  case MCS_WAIT_SEND_POKE_SELECT:
    {
      if( MB_COMM_IsPostMoviePokeConfirm( work->commWork ) == TRUE )
      {
        const u8 val = MB_COMM_GetMoviePokeConfirm( work->commWork );
        if( val == MCMV_POKETRANS_YES )
        {
          work->state = MCS_TRANS_POKE_INIT;
        }
        else
        {
          work->state = MCS_CHECK_LOCK_CAPSULE;
        }
      }
    }
    break;

  //--------------------------------------------------------
  //�]���E�ۑ��t�F�C�Y
  case MCS_TRANS_POKE_INIT:
    {
      u8 i,j;
      u8 setNum = 0;
      MB_COMM_INIT_DATA *commInit = MB_COMM_GetInitData( work->commWork );
      MB_COMM_ClearSendPokeData( work->commWork );
      for( i=0;i<MB_POKE_BOX_TRAY;i++ )
      {
        for( j=0;j<MB_POKE_BOX_POKE;j++ )
        {
          if( PPP_Get( work->boxPoke[i][j] , ID_PARA_poke_exist , NULL ) == TRUE )
          {
            if( MB_MOVIE_CheckMoviePoke( work->boxPoke[i][j] ) == TRUE )
            {
              const u32 itemNo = PPP_Get( work->boxPoke[i][j] , ID_PARA_item , NULL );
              if( itemNo != 0 )
              {
                MB_DATA_AddItem( work->dataWork , itemNo );
                PPP_Put( work->boxPoke[i][j] , ID_PARA_item , 0 );
              }
              MB_COMM_AddSendPokeData( work->commWork , work->boxPoke[i][j] );
              //���f�[�^��ppp������
              PPP_Clear( work->boxPoke[i][j] );
              setNum++;
              if( setNum >= MB_CAP_POKE_NUM )
              {
                break;
              }
              MB_TPrintf("Trans![%2d:%2d]\n",i,j);
            }
          }
          if( setNum >= MB_CAP_POKE_NUM )
          {
            break;
          }
        }
        if( setNum >= MB_CAP_POKE_NUM )
        {
          break;
        }
      }
      if( setNum > 0 )
      {
        work->state = MCS_TRANS_POKE_SEND;
      }
      else
      {
        work->state = MCS_TRANS_POKE_SEND_FINISH;
        work->isSendPoke = TRUE;
        MB_DATA_ResetSaveLoad( work->dataWork );
      }
    }

    break;
    
  case MCS_TRANS_POKE_SEND:
    if( MB_MOVIE_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_COMM_Send_PokeData( work->commWork ) == TRUE )
      {
        work->state = MCS_TRANS_POKE_SEND_WAIT;
      }
    }
    break;
    
  case MCS_TRANS_POKE_SEND_WAIT:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE )
    {
      work->state = MCS_TRANS_POKE_INIT;
    }
    break;
    
  case MCS_TRANS_POKE_SEND_FINISH:
    {
      const BOOL flg = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_FINISH_SEND_POKE , 0 );
      if( flg == TRUE )
      {
        work->state = MCS_CHECK_LOCK_CAPSULE;
        MB_TPrintf("MB_Child Finish poke trans\n");
      }
    }
    break;
    
  case MCS_CHECK_LOCK_CAPSULE:
    {
      const BOOL flg = MB_DATA_CheckLockCapsule( work->dataWork );
      const BOOL ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_HAVE_LOCK_CAPSULE , flg );
      if( ret == TRUE )
      {
        if( flg == TRUE )
        {
          work->state = MCS_POST_TRANS_LOCK_CAPSULE;
          MB_TPrintf("MB_Child Lock capsule found!\n");
        }
        else
        {
          work->state = MCS_CHECK_SAVE;
          MB_TPrintf("MB_Child Lock capsule not found...\n");
        }
      }
    }
    break;
    
  case MCS_POST_TRANS_LOCK_CAPSULE:
    if( MB_COMM_IsPostMovieTransLockCapsule( work->commWork ) == TRUE )
    {
      if( MB_COMM_IsMovieTransLockCapsule( work->commWork ) == TRUE )
      {
        MB_DATA_RemoveLockCapsule( work->dataWork );
        work->isSendCapsule = TRUE;
        MB_TPrintf("MB_Child Remove lock capsule!!\n");
      }
      work->state = MCS_CHECK_SAVE;
    }
    break;
  
  case MCS_CHECK_SAVE:
    if( work->isSendPoke == TRUE || work->isSendCapsule == TRUE )
    {
      work->state = MCS_CHECK_ROM_CRC_LOAD;
    }
    else
    {
      work->state = MCS_SAVE_FINISH_WAIT;
    }
    break;
    
  case MCS_CHECK_ROM_CRC_LOAD:
    if( MB_DATA_LoadRomCRC( work->dataWork ) == TRUE )
    {
      work->state = MCS_CHECK_ROM_CRC;
    }
    break;

  case MCS_CHECK_ROM_CRC:
    if( MB_MOVIE_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_DATA_CheckRomCRC( work->dataWork ) == TRUE )
      {
        MB_COMM_SetChildState( work->commWork , MCCS_CRC_OK );
        work->state = MCS_SAVE_INIT;
      }
      else
      {
        //CRC�`�F�b�N���s
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_08 , work->initData->msgSpeed );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
        work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
      }
    }
    break;

  case MCS_SAVE_INIT:
    if( MB_MOVIE_ErrCheck( work , FALSE ) == FALSE )
    {
      MB_MOVIE_SaveInit( work );
      work->state = MCS_SAVE_MAIN;
      work->subState = MCSS_SAVE_WAIT_SAVE_INIT;
    }
    break;

  case MCS_SAVE_MAIN:
    MB_MOVIE_SaveMain( work );
    MB_MOVIE_ErrCheck( work , FALSE );
    break;

  case MCS_SAVE_TERM:
    MB_MOVIE_SaveTerm( work );
    MB_MOVIE_ErrCheck( work , FALSE );
    break;

  case MCS_SAVE_FINISH_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      if( MB_COMM_IsPostMovieFinishMachine( work->commWork ) == TRUE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_05 , work->initData->msgSpeed );
        MB_COMM_ReqDisconnect( work->commWork );
        work->state = MCS_WAIT_EXIT_COMM;
      }
    }
    break;

  
  case MCS_WAIT_NEXT_GAME_ERROR_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      //�����^�C���A�E�g���痈���msgSpd������ĂȂ��B
      const int msgSpd = ( work->initData!= NULL ? work->initData->msgSpeed : 1 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_05 , msgSpd );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MCS_WAIT_EXIT_COMM;
      //�Q�[����̃|�P�s���ł�����B���̍ۂ̓G���[����Ȃ��̂ŁA�����ŃX�e�[�g�Z�b�g�͂��Ȃ��I
      //MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
    }
    MB_MOVIE_ErrCheck( work , FALSE );
    break;
  
  case MCS_WAIT_EXIT_COMM:
    if( MB_COMM_IsDisconnect( work->commWork ) == TRUE )
    {
      work->state = MCS_EXIT_GAME;
    }
    break;

  case MCS_EXIT_GAME:
    break;
  }

  if( work->isInitCellSys == TRUE )
  {
    //OBJ�̍X�V
    GFL_CLACT_SYS_Main();
  }
    
  if( work->msgWork != NULL )
  {
    MB_MSG_MessageMain( work->msgWork );
  }
  PMSND_Main();
  
  return FALSE;
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void MB_MOVIE_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  MB_MOVIE_WORK *work = wk;
  //OBJ�̍X�V
  if( work->isInitCellSys == TRUE )
  {
    GFL_CLACT_SYS_VBlankFunc();
  }
}

static void MB_MOVIE_VSyncMovie( void )
{
  //�w�i�A�j���X�V
  ConnectBGPalAnm_Main( staticBgAnmWork );
}

//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void MB_MOVIE_InitGraphic( MB_MOVIE_WORK *work )
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
    // BG2 MAIN (�w�i(�X�N���[��
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 SUB (���b�Z�[�W
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_MOVIE_SetupBgFunc( &header_main1 , MB_MOVIE_FRAME_MSG , GFL_BG_MODE_TEXT );
    MB_MOVIE_SetupBgFunc( &header_main2 , MB_MOVIE_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_MOVIE_SetupBgFunc( &header_main3 , MB_MOVIE_FRAME_BG2 , GFL_BG_MODE_TEXT );
    MB_MOVIE_SetupBgFunc( &header_sub1  , MB_MOVIE_FRAME_SUB_MSG , GFL_BG_MODE_TEXT );
    MB_MOVIE_SetupBgFunc( &header_sub3  , MB_MOVIE_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &vramBank ,work->heapId );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE , work->heapId );
  GFL_NET_ReloadIcon();
  work->isInitCellSys = TRUE;
  
}

static void MB_MOVIE_TermGraphic( MB_MOVIE_WORK *work )
{
  work->isInitCellSys = FALSE;
  GFL_CLACT_SYS_Delete();
  GFL_BG_FreeBGControl( MB_MOVIE_FRAME_SUB_MSG );
  GFL_BG_FreeBGControl( MB_MOVIE_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_MOVIE_FRAME_BG2 );
  GFL_BG_FreeBGControl( MB_MOVIE_FRAME_BG );
  GFL_BG_FreeBGControl( MB_MOVIE_FRAME_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MB_MOVIE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void MB_MOVIE_LoadResource( MB_MOVIE_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_WIFI_LOGIN , work->heapId );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_wifi_login_connect_win_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                    MB_MOVIE_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win2_d_NSCR , 
                    MB_MOVIE_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  
  //����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_wifi_login_connect_win_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                    MB_MOVIE_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win1_u_NSCR , 
                    MB_MOVIE_FRAME_BG ,  0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                    MB_MOVIE_FRAME_BG2 , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win2_u_NSCR , 
                    MB_MOVIE_FRAME_BG2 ,  0 , 0, FALSE , work->heapId );
  
  ConnectBGPalAnm_InitBg( &work->bgAnmWork , arcHandle , NARC_wifi_login_connect_ani_NCLR , work->heapId , MB_MOVIE_FRAME_BG , MB_MOVIE_FRAME_BG2 );
  staticBgAnmWork = &work->bgAnmWork;
  GFL_ARC_CloseDataHandle( arcHandle );
}

//--------------------------------------------------------------
//  �f�[�^�̕ϊ�
//--------------------------------------------------------------
static void MB_MOVIE_DataConvert( MB_MOVIE_WORK *work )
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
}

//--------------------------------------------------------------
//  �{�b�N�X�̎c���`�F�b�N
//--------------------------------------------------------------
static const u16 MB_MOVIE_GetLeastPoke( MB_MOVIE_WORK *work )
{
  u8 i,j;
  u16 num = 0;
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      if( PPP_Get( work->boxPoke[i][j] , ID_PARA_poke_exist , NULL ) == TRUE )
      {
        const u8 ret = MB_UTIL_CheckPlay_PalGate( work->boxPoke[i][j] , work->cardType );
        if( ret == MUCPR_OK )
        {
          num++;
        }
      }
    }
  }
  
  MB_TPrintf("Child box least poke [%d].\n",num);
  return num;
}
//--------------------------------------------------------------
//  �f��|�P�̎擾(���M�p��bit��F�X�������Ă�
//--------------------------------------------------------------
static const u32 MB_MOVIE_GetMoviePokeNum( MB_MOVIE_WORK *work )
{
  u32 retVal = 0;
  u16 i,j;
  u16 num = 0;
  u16 hidenNum = 0;
  BOOL isHidenPoke = FALSE;
  BOOL isItemPoke = FALSE;
  BOOL isFullItem = FALSE;
  
  u16 *itemNoArr = GFL_HEAP_AllocClearMemory( work->heapId , 2*MB_POKE_BOX_TRAY*MB_POKE_BOX_POKE );
  
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      POKEMON_PASO_PARAM *ppp = work->boxPoke[i][j];
      if( PPP_Get( ppp , ID_PARA_poke_exist , NULL ) == TRUE )
      {
        if( MB_MOVIE_CheckMoviePoke( ppp ) == TRUE )
        {
          MB_UTIL_CHECK_PLAY_RET ret = MB_UTIL_CheckPlay_PalGate( ppp , work->cardType );
          if( ret == MUCPR_HIDEN )
          {
            hidenNum++;
            MB_TPrintf("movie poke found(Hiden)!![%d:%d].\n",i,j);
          }
          else
          {
            itemNoArr[i*MB_POKE_BOX_POKE+j] = PPP_Get( ppp , ID_PARA_item , NULL );
            num++;
            MB_TPrintf("movie poke found!![%d:%d].\n",i,j);
          }
        }
      }
    }
  }
  //�A�C�e���`�F�b�N
  for( i=0;i<MB_POKE_BOX_TRAY*MB_POKE_BOX_POKE;i++ )
  {
    if( itemNoArr[i] != 0 )
    {
      u16 sameItem = 1;
      isItemPoke = TRUE;
      MB_TPrintf("ChekcItem!![%d].\n",i);

      for( j=i+1;j<MB_POKE_BOX_TRAY*MB_POKE_BOX_POKE;j++ )
      {
        if( itemNoArr[i] == itemNoArr[j] )
        {
          itemNoArr[j] = 0;
          sameItem++;
        }
      }
      {
        u16 haveNum = MB_DATA_GetItemNum( work->dataWork , itemNoArr[i] );
        MB_TPrintf("Have[%d] Check[%d].\n",haveNum,sameItem);
        if( haveNum + sameItem > 999 )
        {
          isFullItem = TRUE;
        }
      }
    }
  }
  
  GFL_HEAP_FreeMemory(itemNoArr);
  
  retVal = num + (hidenNum<<16);
  if( isItemPoke == TRUE )
  {
    retVal += 0x40000000;
  }
  if( isFullItem == TRUE )
  {
    retVal += 0x80000000;
  }
  
  MB_TPrintf("Child box movie poke [%d][%x].\n",num,retVal);
  
  return retVal;
}

static const BOOL MB_MOVIE_CheckMoviePoke( POKEMON_PASO_PARAM *ppp )
{
  const u32 monsNo = PPP_Get( ppp, ID_PARA_monsno , NULL );
  const u32 birthPlace = PPP_Get( ppp, ID_PARA_birth_place , NULL );
  const u32 isEvent = PPP_Get( ppp, ID_PARA_event_get_flag , NULL );
  const BOOL isRare = PPP_CheckRare( ppp );
  
  //2010�C�x���g�Ή�
  if( monsNo == MONSNO_SEREBHI &&
      birthPlace == POKE_MEMO_PLACE_SEREBIXI_BEFORE &&
      isEvent == 1 )
  {
    return TRUE;
  }
  else
  if( ( monsNo == MONSNO_RAIKOU || monsNo == MONSNO_ENTEI || monsNo == MONSNO_SUIKUN ) &&
      birthPlace == POKE_MEMO_PLACE_ENRAISUI_BEFORE &&
      isRare == TRUE &&
      isEvent == 1 )
  {
    return TRUE;
  }

#if PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    return TRUE;
  }
#endif //PM_DEBUG
  
  return FALSE;
}

//--------------------------------------------------------------
//  �G���[�̃`�F�b�N
//--------------------------------------------------------------
static const BOOL MB_MOVIE_ErrCheck( MB_MOVIE_WORK *work , const BOOL noFade )
{
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isNetErr == FALSE )
  {
    NetErr_App_ReqErrorDisp();
    work->isNetErr = TRUE;
    if( noFade == FALSE )
    {
      work->state = MCS_FADEOUT;
    }
    else
    {
      work->state = MCS_WAIT_FADEOUT;
    }
    return TRUE;
  }
  return FALSE;
}

#pragma mark [>save func
//--------------------------------------------------------------
//  �Z�[�u������
//--------------------------------------------------------------
static void MB_MOVIE_SaveInit( MB_MOVIE_WORK *work )
{
  MB_DATA_ResetSaveLoad( work->dataWork );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_MOVIE_03 );
  MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );

}

//--------------------------------------------------------------
//  �Z�[�u�J��
//--------------------------------------------------------------
static void MB_MOVIE_SaveTerm( MB_MOVIE_WORK *work )
{
  MB_MSG_MessageDisp( work->msgWork , MSG_MB_MOVIE_04 , work->initData->msgSpeed );
  work->state = MCS_SAVE_FINISH_WAIT;
}

//--------------------------------------------------------------
//  �Z�[�u�X�V
//--------------------------------------------------------------
static void MB_MOVIE_SaveMain( MB_MOVIE_WORK *work )
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
    MB_TPrintf( "MB_Child Save Start!\n" );
    break;

  case MCSS_SAVE_WAIT_FIRST:
		if( MB_DATA_IsFinishSaveFirst( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_FIRST_SAVE , 0 ) == TRUE )
      {
  			work->subState = MCSS_SAVE_WAIT_FIRST_SYNC;
        MB_TPrintf( "MB_Child Finish First!\n" );
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
        MB_TPrintf( "MB_Child FirstLast Save!\n" );
      }
    }
    break;

  case MCSS_SAVE_WAIT_SECOND:
		if( MB_DATA_IsFinishSaveSecond( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_SECOND_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_SECOND_SYNC;
        MB_TPrintf( "MB_Child Finish Second!\n" );
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
        MB_TPrintf( "MB_Child SecondLast Save!\n" );
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
      MB_TPrintf( "MB_Child Finish All Save Seq!!!\n" );
    }
    break;
  }
}

#pragma mark [>proc func
static GFL_PROC_RESULT MB_MOVIE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_MOVIE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_MOVIE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBootMovie_ProcData =
{
  MB_MOVIE_ProcInit,
  MB_MOVIE_ProcMain,
  MB_MOVIE_ProcTerm
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_MOVIE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_MOVIE_WORK *work;
  
  MB_TPrintf("LeastAppHeap[%x]",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x100000 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT_DATA, 0x70000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_MOVIE_WORK), HEAPID_MULTIBOOT );

  work->heapId = HEAPID_MULTIBOOT;
  
  MB_MOVIE_Init( work );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_MOVIE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_MOVIE_WORK *work = mywk;
  
  MB_MOVIE_Term( work );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  NetErr_DispCall(FALSE);
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_MOVIE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_MOVIE_WORK *work = mywk;
  const BOOL ret = MB_MOVIE_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}



