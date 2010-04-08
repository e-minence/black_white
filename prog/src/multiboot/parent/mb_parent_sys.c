//======================================================================
/**
 * @file  mb_parent_sys.c
 * @brief �}���`�u�[�g�E�e�@���C��
 * @author  ariizumi
 * @data  09/11/13
 *
 * ���W���[�����FMB_PARENT
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"
#include "system/ds_system.h"
#include "gamesystem/msgspeed.h"
#include "net/wih.h"
#include "net/network_define.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/wipe.h"
#include "savedata/misc.h"
#include "app/app_menu_common.h"
#include "net_app/connect_anm.h"
#include "field/evt_lock.h" //���b�N�J�v�Z���̃C�x���g���b�N�̂���

#include "arc_def.h"
#include "mb_parent.naix"
#include "wifi_login.naix"

#include "../../../../resource/fldmapdata/script/palpark_scr_local.h"

#include "multiboot/mb_parent_sys.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/comm/mbp.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_local_def.h"
#include "test/ariizumi/ari_debug.h"
#include "debug/debug_str_conv.h"
//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define MB_PARENT_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_PARENT_FRAME_BG  (GFL_BG_FRAME2_M)
#define MB_PARENT_FRAME_BG2  (GFL_BG_FRAME3_M)

#define MB_PARENT_FRAME_SUB_MSG  (GFL_BG_FRAME1_S)
#define MB_PARENT_FRAME_SUB_BAR  (GFL_BG_FRAME2_S)
#define MB_PARENT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define MB_PARENT_PLT_SUB_OBJ_APP  (0)

#define MB_PARENT_PLT_SUB_BG  (0)
#define MB_PARENT_PLT_SUB_BAR (8)

#define MB_PARENT_FIRST_TIMEOUT (60*15) //�ʏ�5�b�ȓ��Őڑ�����̂��ł��Ȃ������B

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MPS_FADEIN,
  MPS_WAIT_FADEIN,
  MPS_FADEOUT,
  MPS_WAIT_FADEOUT,
  
  MPS_SEND_IMAGE_INIT,
  MPS_SEND_IMAGE_MAIN,
  MPS_SEND_IMAGE_TERM,

  MPS_SEND_INIT_NET,
  MPS_SEND_INIT_DATA,

  //�|�P�V�t�^�[��p
  MPS_SEND_GAMEDATA_INIT,
  MPS_SEND_GAMEDATA_SEND,

  MPS_WAIT_SELBOX,
  MPS_WAIT_FINISH_SELBOX,
  MPS_WAIT_POST_GAMEDATA,
  MPS_WAIT_FINISH_CAPTURE,
  MPS_WAIT_SEND_POKE,
  MPS_WAIT_CRC_CHECK,
  //�|�P�V�t�^�[��p�����܂�

  MPS_SAVE_INIT,
  MPS_SAVE_MAIN,
  MPS_SAVE_TERM,

  //�I�����|�P�V�t�^�[��p
  MPS_SEND_LEAST_BOX,
  MPS_WAIT_NEXT_GAME_CONFIRM,
  //�I�����|�P�V�t�^�[��p�����܂�

  //�I�����f��z�M��p
  MPS_MOVIE_WAIT_SAVE_MSG,
  MPS_MOVIE_WAIT_LAST_MSG,
  //�I�����f��z�M��p�����܂�

  MPS_EXIT_COMM,
  MPS_WAIT_EXIT_COMM,

  MPS_FAIL_FIRST_CONNECT,
  MPS_WAIT_FAIL_FIRST_CONNECT,
  
  //�f��|�P�]���X�V
  MPS_UPDATE_MOVIE_MODE,
  
  //����OFF���G���[(�����Ă񂻂��}�V���̂�
  MPS_COMM_ERROR_WAIT,
}MB_PARENT_STATE;

typedef enum
{
  MPSS_SEND_IMAGE_INIT_COMM,
  MPSS_SEND_IMAGE_INIT_COMM_WAIT,
  MPSS_SEND_IMAGE_WAIT_SEARCH_CH,
  MPSS_SEND_IMAGE_MBSYS_MAIN,
  
  MPSS_SEND_IMAGE_WAIT_BOOT_INIT,
  MPSS_SEND_IMAGE_WAIT_BOOT,

  MPSS_SEND_IMAGE_NET_EXIT,
  MPSS_SEND_IMAGE_NET_EXIT_WAIT,

  MPSS_SEND_CANCEL_BOOT,
//-------------------------------
  MPSS_SAVE_WAIT_SAVE_INIT,
  MPSS_SAVE_SYNC_SAVE_INIT,
  MPSS_SAVE_INIT,
  MPSS_SAVE_WAIT_FIRST,
  MPSS_SAVE_WAIT_FIRST_SYNC,
  MPSS_SAVE_FIRST_SAVE_LAST,
  MPSS_SAVE_WAIT_SECOND,
  MPSS_SAVE_WAIT_SECOND_SYNC,
  MPSS_SAVE_SECOND_SAVE_LAST,
  MPSS_SAVE_WAIT_LAST_SAVE,
  MPSS_SAVE_WAIT_FINISH_SAVE_SYNC,
  
}MB_PARENT_SUB_STATE;

//�f��]�����̃`�F�b�N
typedef enum
{
  MPMS_WAIT_COUNT_POKE, //�|�P�����`�F�b�N��
  
  MPMS_CONFIRM_POKE_WAIT_MSG,
  MPMS_CONFIRM_POKE_WAIT_YESNO,

  MPMS_CONFIRM_HIDEN_WARN_INIT,
  MPMS_CONFIRM_HIDEN_WARN_WAIT,

  MPMS_CONFIRM_CHECK_ITEM_INIT,
  MPMS_CONFIRM_CHECK_ITEM_WAIT,
  MPMS_CONFIRM_CHECK_ITEM_YESNO,

  MPMS_CONFIRM_CHECK_FULL_WAIT,
  MPMS_CONFIRM_CHECK_FULL_YESNO,

  MPMS_CONFIRM_CANCEL_INIT,
  MPMS_CONFIRM_CANCEL_WAIT,
  MPMS_CONFIRM_CANCEL_YESNO,

  MPMS_CONFIRM_POKE_SEND_YESNO,

  MPMS_POST_POKE_WAIT,
  MPMS_POST_POKE_RET_POST,
  MPMS_POST_POKE_FINISH,
  
  MPMS_BOX_NOT_ENOUGH,
  MPMS_BOX_NOT_ENOUGH_WAIT,

  MPMS_WAIT_CHECK_LOCK_CAPSULE,
  
  MPMS_CONFIRM_LOCK_CAPSULE_WAIT,
  MPMS_CONFIRM_LOCK_CAPSULE_YESNO,
  MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT,
  MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO,
  MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT,

  MPMS_CHECK_SAVE,
}
MB_PARENT_MOVIE_STATE;

typedef enum
{
  MPCS_NONE,
  MPCS_INIT,
  MPCS_WAIT_MSG,
  MPCS_WAIT_CONFIRM,
  MPCS_END,
}MB_PARENT_CONFIRM_STATE;

typedef enum
{
  MPCR_PLT_APP,
  MPCR_NCG_APP,
  MPCR_ANM_APP,

  MPCR_MAX,
  
}MB_PARENT_CELL_RES;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  MB_PARENT_INIT_WORK *initWork;
  MB_COMM_WORK *commWork;
  BOOL         isNetErr;
  
  MB_PARENT_STATE state;
  u8              subState;
  u8              movieState;
  u8              confirmState;
  u8              saveWaitCnt;
  u8              mode;         //�|�P�V�t�^�[���f��z�M���H
  u16             timeOutCnt;   //������ROM�ڑ����Ƀ^�C���A�E�g���`�F�b�N����
  BOOL            isSendGameData;
  BOOL            isSendRom;
  MB_MSG_YESNO_RET yesNoRet;
  
  //�f��p
  BOOL isBoxNotEnough;
  BOOL isPostMoviePoke;
  BOOL isPostMovieCapsule;
  CONNECT_BG_PALANM bgAnmWork;
  
  MISC  *miscSave;
  
  //SendImage
  u16    *romTitleStr;  //DLROM�^�C�g��
  u16    *romInfoStr;   //DLROM����
  
  MB_COMM_INIT_DATA initData;
  void* gameData;
  u32   gameDataSize;
  //���b�Z�[�W�p
  MB_MSG_WORK *msgWork;

  GFL_CLUNIT  *cellUnit;
  u32         cellResIdx[MPCR_MAX];
  GFL_CLWK    *clwkReturn;
  
  MBGameRegistry gameRegistry;  //MB�z�M�p�̃f�[�^
  
  
  
}MB_PARENT_WORK;

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

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
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};


static void MB_PARENT_Init( MB_PARENT_WORK *work );
static void MB_PARENT_Term( MB_PARENT_WORK *work );
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work );
static void MB_PARENT_VBlankFunc(GFL_TCB *tcb, void *wk );
static void MB_PARENT_VSync( void );
static void MB_PARENT_VSyncMovie( void );

static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work );

static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work );
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work );
static BOOL MP_PARENT_WhCallBack( BOOL bResult );

static void MB_PARENT_SaveInit( MB_PARENT_WORK *work );
static void MB_PARENT_SaveInitPoke( MB_PARENT_WORK *work );
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work );
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work );

static void MB_PARENT_UpdateMovieMode( MB_PARENT_WORK *work );

static void MB_PARENT_SetFinishState( MB_PARENT_WORK *work , const u8 state );

BOOL WhCallBackFlg = FALSE;
CONNECT_BG_PALANM *staticBgAnmWork;

FS_EXTERN_OVERLAY(dev_wireless);

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static void MB_PARENT_Init( MB_PARENT_WORK *work )
{
  work->state = MPS_FADEIN;
  work->movieState = MPMS_WAIT_COUNT_POKE;
  work->mode = work->initWork->mode;
  
  MB_PARENT_InitGraphic( work );
  MB_PARENT_LoadResource( work );

  if( work->mode == MPM_POKE_SHIFTER )
  {
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_SUB_MSG , MB_PARENT_FRAME_SUB_MSG , FILE_MSGID_MB , FALSE , FALSE );
  }
  else
  {
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_MSG , MB_PARENT_FRAME_MSG , FILE_MSGID_MB , FALSE , TRUE );
  }
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  
  work->commWork = MB_COMM_CreateSystem( work->heapId );
  work->isSendGameData = FALSE;
  work->gameData = NULL;
  
  if( work->mode == MPM_POKE_SHIFTER )
  {
    SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( work->initWork->gameData );
    work->miscSave = SaveData_GetMisc( svWork );
    //�Ƃ肠�����L�����Z��(��ԒႢ�X�e�[�g)�ɂ��Ă���
    MISC_SetPalparkFinishState( work->miscSave , PALPARK_FINISH_CANCEL );
  }
  else
  {
    work->miscSave = NULL;
  }
  
  work->vBlankTcb = GFUser_VIntr_CreateTCB( MB_PARENT_VBlankFunc , work , 8 );
  if( work->mode == MPM_POKE_SHIFTER )
  {
    GFUser_SetVIntrFunc( MB_PARENT_VSync );
  }
  else
  {
    GFUser_SetVIntrFunc( MB_PARENT_VSyncMovie );
  }

  
  work->isPostMoviePoke = FALSE;
  work->isPostMovieCapsule = FALSE;

}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_PARENT_Term( MB_PARENT_WORK *work )
{
  GFL_TCB_DeleteTask( work->vBlankTcb );
  GFUser_ResetVIntrFunc();
  GFL_NET_WirelessIconEasyEnd();
  if( work->mode == MPM_MOVIE_TRANS )
  {
    ConnectBGPalAnm_End( &work->bgAnmWork );
  }

  if( work->gameData != NULL )
  {
    GFL_HEAP_FreeMemory( work->gameData );
  }
  MB_COMM_DeleteSystem( work->commWork );

  MB_MSG_MessageTerm( work->msgWork );
  MB_PARENT_TermGraphic( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work )
{
  MB_COMM_UpdateSystem( work->commWork );

  if( work->isNetErr == TRUE &&
      work->state != MPS_FADEOUT &&
      work->state != MPS_WAIT_FADEOUT )
  {
    work->state = MPS_FADEOUT;
    MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
  }

  switch( work->state )
  {
  case MPS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    work->state = MPS_WAIT_FADEIN;
    break;
    
  case MPS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( DS_SYSTEM_IsAvailableWireless() == TRUE )
      {
        work->state = MPS_SEND_IMAGE_INIT;
      }
      else
      {
        //MSG���Ⴄ�̂œ���֐�
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageDips_CommDisableError( work->msgWork , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        work->state = MPS_COMM_ERROR_WAIT;
      }
    }
    break;
    
  case MPS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MPS_WAIT_FADEOUT;
    break;
  case MPS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  //-----------------------------------------------
  //ROM�̑��M����
  case MPS_SEND_IMAGE_INIT:
    MP_PARENT_SendImage_Init( work );
    work->state = MPS_SEND_IMAGE_MAIN;
    break;
    
  case MPS_SEND_IMAGE_MAIN:
    {
      const BOOL ret = MP_PARENT_SendImage_Main( work );
      if( ret == TRUE )
      {
        work->state = MPS_SEND_IMAGE_TERM;
      }
    }
    break;
    
  case MPS_SEND_IMAGE_TERM:
    MP_PARENT_SendImage_Term( work );
    if( work->isSendRom == TRUE )
    {
      //�N������
      work->state = MPS_SEND_INIT_NET;
      work->timeOutCnt = 0;
      MB_COMM_InitComm( work->commWork );

      if( work->mode == MPM_POKE_SHIFTER )
      {
        GFL_NET_WirelessIconEasy_HoldLCD( FALSE , work->heapId );
        GFL_NET_ReloadIcon();
      }
      else
      {
        GFL_NET_WirelessIconEasy_HoldLCD( TRUE , work->heapId );
        GFL_NET_ReloadIcon();
      }
    }
    else
    {
      //�N�����s
      work->state = MPS_FADEOUT;
    }
    break;
    
  //�N����
  case MPS_SEND_INIT_NET:
    if( MB_COMM_IsInitComm( work->commWork ) == TRUE )
    {
      work->state = MPS_SEND_INIT_DATA;
      MB_COMM_InitParent( work->commWork );
    }
    break;
    
  case MPS_SEND_INIT_DATA:
    work->timeOutCnt++;
    if( work->timeOutCnt >= MB_PARENT_FIRST_TIMEOUT )
    {
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      work->state = MPS_FAIL_FIRST_CONNECT;
    }
    else
    if( MB_COMM_IsSendEnable( work->commWork ) == TRUE )
    {
      work->initData.msgSpeed = MSGSPEED_GetWait();
      if( work->mode == MPM_POKE_SHIFTER )
      {
        work->initData.highScore = MISC_GetPalparkHighscore(work->miscSave);
      }
      if( MB_COMM_Send_InitData( work->commWork , &work->initData ) == TRUE )
      {
        if( work->mode == MPM_POKE_SHIFTER )
        {
          work->state = MPS_SEND_GAMEDATA_INIT;
        }
        else
        {
          work->state = MPS_UPDATE_MOVIE_MODE;
        }
      }
    }
    break;
    
    //�|�P�V�t�^�[�p����
  case MPS_SEND_GAMEDATA_INIT:
    {
      FSFile file;
      BOOL result;
      const char* arcPath = GFUser_GetFileNameByArcID( ARCID_MB_CAPTER );
      //�ǂݏo��
      FS_InitFile( &file );
      result = FS_OpenFile(&file,arcPath);
      GF_ASSERT( result );
      FS_SeekFileToBegin( &file );
      work->gameDataSize = FS_GetLength( &file );
      work->gameData = GFL_HEAP_AllocClearMemory( work->heapId , work->gameDataSize );
      FS_ReadFile( &file,work->gameData,work->gameDataSize );
      result = FS_CloseFile( &file );
      GF_ASSERT( result );
      MB_TPrintf( "[%d]\n",work->gameDataSize );
    }
    work->state = MPS_SEND_GAMEDATA_SEND;
    break;

  case MPS_SEND_GAMEDATA_SEND:
    work->state = MPS_WAIT_SELBOX;
    break;
    
  case MPS_WAIT_SELBOX:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
    {
      //�ǂݍ��݃G���[����������
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SELECT_BOX )
    {
      if( work->isSendGameData == FALSE )
      {
        work->isSendGameData = TRUE;
        MB_COMM_InitSendGameData( work->commWork , work->gameData , work->gameDataSize );
      }
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_03 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_FINISH_SELBOX;
    }
    break;
    
  case MPS_WAIT_FINISH_SELBOX:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_WAIT_GAME_DATA )
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_04 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_POST_GAMEDATA;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_CANCEL_BOX )
    {
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_CANCEL );

      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
  
  //break;  //MCCS_WAIT_GAME_DATA���o�R���Ȃ����Ƃ�����̂ŃX���[�I�I
  case MPS_WAIT_POST_GAMEDATA:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_CAP_GAME )
    {
      //�Q�[���I����ł��������A�R�R���m���Ȃ̂ŁB
      MB_COMM_ClearSendPokeData(work->commWork);
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_05 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_FINISH_CAPTURE;
    }
    break;

  case MPS_WAIT_FINISH_CAPTURE:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SEND_POKE )
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_06 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_SEND_POKE;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_NEXT_GAME )
    {
      //�����ɗ����Ƃ������Ƃ͕߂܂��Ă��Ȃ��I
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_NO_GET );
      work->state = MPS_SEND_LEAST_BOX;
    }
    break;

  case MPS_WAIT_SEND_POKE:
    if( MB_COMM_IsPostPoke( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_POST_POKE , 0 ) == TRUE )
      {
        work->state = MPS_WAIT_CRC_CHECK;
      }
    }
    break;
  case MPS_WAIT_CRC_CHECK:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE ||
        work->mode == MPM_MOVIE_TRANS ) //�f��̎��̓|�P�����ĂȂ������B
    {
      if( MB_COMM_GetChildState(work->commWork) == MCCS_CRC_OK )
      {
        work->state = MPS_SAVE_INIT;
      }
      else
      if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
      {
        //CRC�`�F�b�N�G���[����������
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
          MB_COMM_ReqDisconnect( work->commWork );
          work->state = MPS_EXIT_COMM;
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_18 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
          work->state = MPS_MOVIE_WAIT_LAST_MSG;
        }
      }
    }
    break;
    
  case MPS_SAVE_INIT:
    MB_PARENT_SaveInit( work );

    work->state = MPS_SAVE_MAIN;
    work->subState = MPSS_SAVE_WAIT_SAVE_INIT;
    break;

  case MPS_SAVE_MAIN:
    MB_PARENT_SaveMain( work );
    break;

  case MPS_SAVE_TERM:
    MB_PARENT_SaveTerm( work );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_08 , MSGSPEED_GetWait() );
      work->state = MPS_SEND_LEAST_BOX;
    }
    else
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_19 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      work->state = MPS_MOVIE_WAIT_SAVE_MSG;
    }
    break;

  //�I�����|�P�V�t�^�[��p
  case MPS_SEND_LEAST_BOX:
    {
      BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
      const u16 leastBoxNum = BOXDAT_GetEmptySpaceTotal( boxMng );
      if( MB_COMM_Send_Flag( work->commWork , MCFT_LEAST_BOX , leastBoxNum ) == TRUE )
      {
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_12 , MSGSPEED_GetWait() );
        MB_Printf("Parent box empty num[%d]\n",leastBoxNum);
        work->state = MPS_WAIT_NEXT_GAME_CONFIRM;
      }
    }
    break;

  case MPS_WAIT_NEXT_GAME_CONFIRM:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SELECT_BOX )
    {
      MB_COMM_ResetFlag( work->commWork);
      work->state = MPS_WAIT_SELBOX;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME ||
        MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
    {
      if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      }
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
    break;
  //�I�����|�P�V�t�^�[��p�����܂�

  //�I�����f��z�M��p
  case MPS_MOVIE_WAIT_SAVE_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_FINISH_MACHINE , 0 ) == TRUE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_15 , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        work->state = MPS_MOVIE_WAIT_LAST_MSG;
      }
    }
    break;
  
  case MPS_MOVIE_WAIT_LAST_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
    break;
  //�I�����f��z�M��p�����܂�
  
  case MPS_EXIT_COMM:
    if( MB_COMM_IsDisconnect( work->commWork ) == TRUE )
    {
      MB_COMM_ExitComm( work->commWork );
      work->state = MPS_WAIT_EXIT_COMM;
    }
    break;
  case MPS_WAIT_EXIT_COMM:
    if( MB_COMM_IsFinishComm( work->commWork ) == TRUE )
    {
      work->state = MPS_FADEOUT;
    }
    break;
    
  //����ڑ����s
  case MPS_FAIL_FIRST_CONNECT:
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_11 , MSGSPEED_GetWait() );
    MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
    work->state = MPS_WAIT_FAIL_FIRST_CONNECT;
    break;
  
  case MPS_WAIT_FAIL_FIRST_CONNECT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_COMM_ExitComm( work->commWork );
      work->state = MPS_WAIT_EXIT_COMM;
    }
  
  //----------------------------------------------------------------
  //  �f��]��
  //----------------------------------------------------------------
  case MPS_UPDATE_MOVIE_MODE:
    MB_PARENT_UpdateMovieMode( work );
    break;

  //����OFF���G���[(�����Ă񂻂��}�V���̂�
  case MPS_COMM_ERROR_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->state = MPS_FADEOUT;
    }
    break;

  }
  
  MB_MSG_MessageMain( work->msgWork );
  
  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();
  
  
  return FALSE;
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void MB_PARENT_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  //OBJ�̍X�V
  GFL_CLACT_SYS_VBlankFunc();
}

static void MB_PARENT_VSync( void )
{
  static u8 cnt = 0;
  if( cnt > 1 )
  {
    cnt = 0;
    GFL_BG_SetScroll( MB_PARENT_FRAME_BG2 , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_BG2 , GFL_BG_SCROLL_Y_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_SUB_BG , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_SUB_BG , GFL_BG_SCROLL_Y_DEC , 1 );
  }
  else
  {
    cnt++;
  }
}

static void MB_PARENT_VSyncMovie( void )
{
  //�w�i�A�j���X�V
  ConnectBGPalAnm_Main( staticBgAnmWork );
}

//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work )
{
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
    // BG2 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i�Q
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 SUB (���b�Z�[�W
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (�o�[
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
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

    MB_PARENT_SetupBgFunc( &header_main1 , MB_PARENT_FRAME_MSG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main2 , MB_PARENT_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main3 , MB_PARENT_FRAME_BG2 , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub1  , MB_PARENT_FRAME_SUB_MSG, GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub2  , MB_PARENT_FRAME_SUB_BAR, GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub3  , MB_PARENT_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }

  //OBJ�n�̏�����
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.CGR_RegisterMax = 64;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    work->cellUnit = GFL_CLACT_UNIT_Create( 8 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );

    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
}

static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work )
{
  GFL_CLACT_WK_Remove( work->clwkReturn );
  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG2 );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BAR );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_MSG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work )
{
  if( work->mode == MPM_POKE_SHIFTER )
  {
    //�|�P�V�t�^�[
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_PARENT , work->heapId );

    //�����
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_sub_NCLR , 
                      PALTYPE_SUB_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_sub_NCGR ,
                      MB_PARENT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_sub_NSCR , 
                      MB_PARENT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
    
    //����
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_main_NCLR , 
                      PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_main_NCGR ,
                      MB_PARENT_FRAME_BG2 , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_main_NSCR , 
                      MB_PARENT_FRAME_BG2 ,  0 , 0, FALSE , work->heapId );
    
    GFL_ARC_CloseDataHandle( arcHandle );
  }
  else
  {
    //�f��]��
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_WIFI_LOGIN , work->heapId );

    //�����
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_wifi_login_connect_win_NCLR , 
                      PALTYPE_SUB_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                      MB_PARENT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win2_d_NSCR , 
                      MB_PARENT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
    
    //����
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_wifi_login_connect_win_NCLR , 
                      PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                      MB_PARENT_FRAME_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win1_u_NSCR , 
                      MB_PARENT_FRAME_BG ,  0 , 0, FALSE , work->heapId );
    //����
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_wifi_login_connect_win_NCGR ,
                      MB_PARENT_FRAME_BG2 , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_wifi_login_connect_win2_u_NSCR , 
                      MB_PARENT_FRAME_BG2 ,  0 , 0, FALSE , work->heapId );
    
    ConnectBGPalAnm_InitBg( &work->bgAnmWork , arcHandle , NARC_wifi_login_connect_ani_NCLR , work->heapId , MB_PARENT_FRAME_BG , MB_PARENT_FRAME_BG2 );
    staticBgAnmWork = &work->bgAnmWork;
    GFL_ARC_CloseDataHandle( arcHandle );
  }

  
  //���ʑf��
  {
    ARCHANDLE *commonArcHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );
    
    //�o�[
    GFL_ARCHDL_UTIL_TransVramPalette( commonArcHandle , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_SUB_BG , MB_PARENT_PLT_SUB_BAR*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( commonArcHandle , APP_COMMON_GetBarCharArcIdx() ,
                      MB_PARENT_FRAME_SUB_BAR , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( commonArcHandle , APP_COMMON_GetBarScrnArcIdx() , 
                      MB_PARENT_FRAME_SUB_BAR , 0 , 0, FALSE , work->heapId );
    GFL_BG_ChangeScreenPalette( MB_PARENT_FRAME_SUB_BAR , 0 , 21 , 32 , 3 , MB_PARENT_PLT_SUB_BAR );
    GFL_BG_LoadScreenReq( MB_PARENT_FRAME_SUB_BAR );

    //�o�[�A�C�R��
    work->cellResIdx[MPCR_PLT_APP] = GFL_CLGRP_PLTT_RegisterEx( commonArcHandle , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_SUB , 
          MB_PARENT_PLT_SUB_OBJ_APP*32 , 0 , 
          APP_COMMON_BARICON_PLT_NUM , work->heapId  );
    work->cellResIdx[MPCR_NCG_APP] = GFL_CLGRP_CGR_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellResIdx[MPCR_ANM_APP] = GFL_CLGRP_CELLANIM_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), 
          work->heapId  );

    GFL_ARC_CloseDataHandle( commonArcHandle );
  }
  
  {
    //OBJ�̍쐬
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 256-24;
    cellInitData.pos_y = 192-24;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    cellInitData.bgpri = 0;
    cellInitData.softpri = 0;

    work->clwkReturn = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellResIdx[MPCR_NCG_APP],
              work->cellResIdx[MPCR_PLT_APP],
              work->cellResIdx[MPCR_ANM_APP],
              &cellInitData ,CLSYS_DRAW_SUB , work->heapId );
  }
}


#pragma mark [>SendImage func
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work );

//--------------------------------------------------------------
//  ROM���M���� ������
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work )
{
  int i;
  STRBUF *titleStr;
  STRBUF *infoStr;
  u16 titleLen,infoLen;
  
  work->subState = MPSS_SEND_IMAGE_INIT_COMM;
  work->confirmState = MPCS_NONE;
  work->romTitleStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_NAME_LENGTH*2 );
  work->romInfoStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_INTRO_LENGTH*2 );

  //�^�C�g��
  if( work->mode == MPM_POKE_SHIFTER )
  {
    titleStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE );
  }
  else
  {
    titleStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE_MOVIE );
  }

  //����
  if( work->mode == MPM_POKE_SHIFTER )
  {
    if( GET_VERSION() == VERSION_BLACK )
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_B );
    }
    else
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_W );
    }
  }
  else
  {
    if( GET_VERSION() == VERSION_BLACK )
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_MOVIE_B );
    }
    else
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_MOVIE_W );
    }
  }
  titleLen = GFL_STR_GetBufferLength( titleStr );
  infoLen = GFL_STR_GetBufferLength( infoStr );
  
  GFL_STD_MemCopy16( GFL_STR_GetStringCodePointer( titleStr ) ,
                     work->romTitleStr ,
                     titleLen*2 );
  GFL_STD_MemCopy16( GFL_STR_GetStringCodePointer( infoStr ) ,
                     work->romInfoStr ,
                     infoLen*2 );
  GFL_STR_DeleteBuffer( titleStr );
  GFL_STR_DeleteBuffer( infoStr );

  //�I�[�R�[�h��ϊ�
  work->romTitleStr[titleLen] = 0x0000;
  work->romInfoStr[infoLen] = 0x0000;
  
  //���s�R�[�h�ϊ�
  for( i=0;i<MB_GAME_INTRO_LENGTH;i++ )
  {
    if( work->romInfoStr[i] == 0xFFFE )
    {
      work->romInfoStr[i] = 0x000a;
    }
  }
  
  work->isSendRom = FALSE;
  GFL_OVERLAY_Load( FS_OVERLAY_ID(dev_wireless));
}

//--------------------------------------------------------------
//  ROM���M���� �J��
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work )
{
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dev_wireless));

  GFL_HEAP_FreeMemory( work->romTitleStr );
  GFL_HEAP_FreeMemory( work->romInfoStr );
}

//--------------------------------------------------------------
//  ROM���M���� �X�V
//--------------------------------------------------------------
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SEND_IMAGE_INIT_COMM:
    WhCallBackFlg = FALSE;
    WH_Initialize(work->heapId , &MP_PARENT_WhCallBack , FALSE );
    
    GFL_NET_WirelessIconEasyXY( 256-16,0,FALSE,work->heapId );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      GFL_NET_WirelessIconEasy_HoldLCD( FALSE,work->heapId );
    }
    else
    {
      GFL_NET_WirelessIconEasy_HoldLCD( TRUE,work->heapId );
    }
    
    work->subState = MPSS_SEND_IMAGE_INIT_COMM_WAIT;
    break;
  case MPSS_SEND_IMAGE_INIT_COMM_WAIT:
    //if( WhCallBackFlg == TRUE )
    {
      if( WH_GetSystemState() == WH_SYSSTATE_IDLE )
      {
        if( WH_StartMeasureChannel() == TRUE )
        {
          work->subState = MPSS_SEND_IMAGE_WAIT_SEARCH_CH;
        }
      }
    }
    break;
  case MPSS_SEND_IMAGE_WAIT_SEARCH_CH:
    if( WH_GetSystemState() == WH_SYSSTATE_MEASURECHANNEL )
    {
      {
        MP_PARENT_SendImage_MBPInit( work );

        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE );

        MB_MSG_MessageCreateWordset( work->msgWork );
        {
          //DS�{�̖��̐ݒ�(10����
      #if (defined(SDK_TWL))
          OSOwnerInfoEx info;
          OS_GetOwnerInfoEx( &info );
      #else
          OSOwnerInfo info;
          OS_GetOwnerInfo( &info );
      #endif    
          {
            u8 i;
            STRBUF *workStr = GFL_STR_CreateBuffer( 16 , work->heapId );
            //EOM�̕ϊ�
            for( i=0;i<11;i++ )
            {
              if( info.nickName[i] == 0 )
              {
                info.nickName[i] = GFL_STR_GetEOMCode();
              }
            }
            GFL_STR_SetStringCode( workStr , info.nickName );
            MB_MSG_MessageWordsetStrBuf( work->msgWork , 0 , workStr );

            GFL_STR_DeleteBuffer( workStr );
          }
        }
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 , MSGSPEED_GetWait() );
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_01 , MSGSPEED_GetWait() );
        }
        MB_MSG_MessageDeleteWordset( work->msgWork );

        work->subState = MPSS_SEND_IMAGE_MBSYS_MAIN;
      }
    }
    break;
  case MPSS_SEND_IMAGE_MBSYS_MAIN:
    MP_PARENT_SendImage_MBPMain( work );
    break;

  case MPSS_SEND_IMAGE_WAIT_BOOT_INIT:
    MB_MSG_MessageHide( work->msgWork );
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_02 );
    }
    else
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_MOVIE_02 );
    }
    MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
    work->subState = MPSS_SEND_IMAGE_WAIT_BOOT;
    break;
    
  case MPSS_SEND_IMAGE_WAIT_BOOT:
    work->isSendRom = TRUE;
    work->subState = MPSS_SEND_IMAGE_NET_EXIT;
    break;
  case MPSS_SEND_CANCEL_BOOT:
    work->subState = MPSS_SEND_IMAGE_NET_EXIT;
    break;

  case MPSS_SEND_IMAGE_NET_EXIT:
    WhCallBackFlg = FALSE;
    WH_End( &MP_PARENT_WhCallBack );
    work->subState = MPSS_SEND_IMAGE_NET_EXIT_WAIT;
    break;
  case MPSS_SEND_IMAGE_NET_EXIT_WAIT:
    if( WhCallBackFlg == TRUE )
    {
      WH_FreeMemory();
      return TRUE;
    }
    break;
    
  }
  GFL_NET_WirelessIconEasy_SetLevel(3-WM_GetLinkLevel());
  GFL_NET_WirelessIconEasyFunc();
  return FALSE;
}

//--------------------------------------------------------------
//  MBP�V�X�e���N��
//--------------------------------------------------------------
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work )
{
  const char *srlPath[2] = {"/dl_rom/child.srl","/dl_rom/child2.srl"};
  const char *charPath[2] = {"/dl_rom/icon_w.char","/dl_rom/icon_b.char"};
  const char *plttPath[2] = {"/dl_rom/icon_w.plt","/dl_rom/icon_b.plt"};
  
  /* ���̃f�����_�E�����[�h������v���O������� */
  //static����Ȃ��Ɠ����Ȃ��I�I�I
  MBGameRegistry mbGameList = {
    NULL,    // �q�@�o�C�i���R�[�h
    NULL ,                  // �Q�[����
    NULL ,                  // �Q�[�����e����
    NULL,                   // �A�C�R���L�����N�^�f�[�^
    NULL,                   // �A�C�R���p���b�g�f�[�^
    MB_DEF_GGID,            // GGID
    2,                      // �ő�v���C�l���A�e�@�̐����܂߂��l��
  };

  const u16 channel = WH_GetMeasureChannel();
  
  GFL_STD_MemCopy( &mbGameList , &work->gameRegistry , sizeof(MBGameRegistry) );
  if( work->mode == MPM_POKE_SHIFTER )
  {
    work->gameRegistry.romFilePathp = srlPath[0];
  }
  else
  {
    work->gameRegistry.romFilePathp = srlPath[1];
  }
  
  if( GET_VERSION() == VERSION_BLACK )
  {
    work->gameRegistry.iconCharPathp = charPath[1];
    work->gameRegistry.iconPalettePathp = plttPath[1];
  }
  else
  {
    work->gameRegistry.iconCharPathp = charPath[0];
    work->gameRegistry.iconPalettePathp = plttPath[0];
  }
  
  work->gameRegistry.gameNamep = work->romTitleStr;
  work->gameRegistry.gameIntroductionp = work->romInfoStr;
  MBP_Init( MB_DEF_GGID , MB_TGID_AUTO );
  MBP_Start( &work->gameRegistry , channel );
  
}

static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work )
{
  static u16 befState = 0;
  const u16 mbpState = MBP_GetState();
  static const GFL_UI_TP_HITTBL hitTbl[2] = 
  {
    { 192-24 , 192 ,
      256-24 , 255 },
    { GFL_UI_TP_HIT_END ,0,0,0 },
  };


  if( befState != mbpState )
  {
    //OS_TFPrintf(2,"MBState[%d]->[%d]\n",befState,mbpState);
    befState = mbpState;
  }

  //�ȉ��T���v�����p
  switch (MBP_GetState())
  {
    //-----------------------------------------
    // �A�C�h�����
  case MBP_STATE_IDLE:
    //�R�R�ɂ͗��Ȃ�(MP_PARENT_SendImage_MBPInit��Start���Ă邩��)
    break;
    //-----------------------------------------
    // �q�@����̃G���g���[��t��
  case MBP_STATE_ENTRY:
    {
      if( work->confirmState == MPCS_NONE )
      {    
        const int ret = GFL_UI_TP_HitTrg( hitTbl );
        if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B ||
            ret == 0 )
        {
          // B�{�^���Ń}���`�u�[�g�L�����Z��
          work->confirmState = MPCS_INIT;
          GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_ON );
          GFL_CLACT_WK_SetAutoAnmFlag( work->clwkReturn , TRUE );

          break;
        }
        //�q�@������Ύ�����DL���n�܂�B�L�����Z�����X�e�[�g�ڍs�ōĐڑ���
        //�ł��Ȃ��Ȃ��Ă��܂��̂Ń��u�[�g�`�F�b�N�������ł��B
        // �G���g���[���̎q�@�����ł����݂���ΊJ�n�\�Ƃ���
        /*
        if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
            MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
            MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
        {
          //�q�@��������Ƃ肠�����n�߂Ă��܂�
          {
            OS_TPrintf("[%d][%d][%d]\n",MBP_GetChildBmp(MBP_BMPTYPE_ENTRY)
                                       ,MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING)
                                       ,MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE));
            // �_�E�����[�h�J�n
            //MBP_StartDownloadAll();
            //MBP_StartDownload(1);
          }
        }
        */
        if (MBP_IsBootableAll())
        {
          // �u�[�g�J�n
          MBP_StartRebootAll();
        }
      }
    }
    break;

    //-----------------------------------------
    // �v���O�����z�M����
  case MBP_STATE_DATASENDING:
    {
      //�����͗��Ȃ��I
      if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
      {
        // B�{�^���Ń}���`�u�[�g�L�����Z��
        work->confirmState = MPCS_INIT;
        GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_ON );
        GFL_CLACT_WK_SetAutoAnmFlag( work->clwkReturn , TRUE );
        break;
      }
      // �S�����_�E�����[�h�������Ă���Ȃ�΃X�^�[�g�\.
      if (MBP_IsBootableAll())
      {
        // �u�[�g�J�n
        MBP_StartRebootAll();
      }
    }
    break;

    //-----------------------------------------
    // ���u�[�g����
  case MBP_STATE_REBOOTING:
    {
    }
    break;

    //-----------------------------------------
    // �Đڑ�����
  case MBP_STATE_COMPLETE:
    {
      // �S�������ɐڑ�����������}���`�u�[�g�����͏I����
      // �ʏ�̐e�@�Ƃ��Ė������ċN������B
      work->subState = MPSS_SEND_IMAGE_WAIT_BOOT_INIT;
    }
    break;

    //-----------------------------------------
    // �G���[����
  case MBP_STATE_ERROR:
    {
      // �ʐM���L�����Z������
      MBP_Cancel();
    }
    break;

    //-----------------------------------------
    // �ʐM�L�����Z��������
  case MBP_STATE_CANCEL:
    // None
    break;

    //-----------------------------------------
    // �ʐM�ُ�I��
  case MBP_STATE_STOP:
    work->subState = MPSS_SEND_CANCEL_BOOT;
    /*
    switch (WH_GetSystemState())
    {
    case WH_SYSSTATE_IDLE:
      (void)WH_End();
      break;
    case WH_SYSSTATE_BUSY:
      break;
    case WH_SYSSTATE_STOP:
      return FALSE;
    default:
      OS_Panic("illegal state\n");
    }
    */
    break;
  }
  
  //B�L�����Z���̊m�F����
  switch( work->confirmState )
  {
  case MPCS_INIT:
    MB_MSG_MessageHide( work->msgWork );

    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE_UP );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_10 , MSGSPEED_GetWait() );
    }
    else
    {
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_22 , MSGSPEED_GetWait() );
    }
    work->confirmState = MPCS_WAIT_MSG;
    break;
  case MPCS_WAIT_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( work->mode == MPM_POKE_SHIFTER )
      {
        MB_MSG_DispYesNo( work->msgWork , MMYT_UP );
      }
      else
      {
        MB_MSG_DispYesNoUpper( work->msgWork , MMYT_UP );
      }
      work->confirmState = MPCS_WAIT_CONFIRM;
    }
    break;
  case MPCS_WAIT_CONFIRM:
    {
      MB_MSG_YESNO_RET ret;
      if( work->mode == MPM_POKE_SHIFTER )
      {
        ret = MB_MSG_UpdateYesNo( work->msgWork );
      }
      else
      {
        ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      }

      if( ret == MMYR_RET1 )
      {
        GF_ASSERT_MSG( MBP_GetState() == MBP_STATE_ENTRY , "state is not[MBP_STATE_ENTRY][%d]!!!\n",MBP_GetState() );
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_CANCEL );
        MBP_Cancel();
        work->confirmState = MPCS_END;
      }
      else
      if( ret == MMYR_RET2 )
      {
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_ClearYesNo( work->msgWork );
        }
        else
        {
          MB_MSG_ClearYesNoUpper( work->msgWork );
        }
        MB_MSG_MessageHide( work->msgWork );
        
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE );
        MB_MSG_MessageCreateWordset( work->msgWork );
        MB_MSG_MessageWordsetName( work->msgWork , 0 , GAMEDATA_GetMyStatus(work->initWork->gameData) );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 , MSGSPEED_GetWait() );
        MB_MSG_MessageDeleteWordset( work->msgWork );
        work->confirmState = MPCS_NONE;
      }
    }
    break;
  }
}

static BOOL MP_PARENT_WhCallBack( BOOL bResult )
{
  WhCallBackFlg = TRUE;
  return TRUE;
}

static void MB_PARENT_SetFinishState( MB_PARENT_WORK *work , const u8 state )
{
  if( work->mode == MPM_POKE_SHIFTER )
  {
    const u8 nowState = MISC_GetPalparkFinishState( work->miscSave );
    BOOL isSet = FALSE;


    MB_TPrintf( "SetFinishState[%d->%d]\n",nowState,state );
    //�A���v���C�����l�����āA�D��x�̍����X�e�[�g������B
    switch( state )
    {
    case PALPARK_FINISH_NORMAL:    // (0)  //�ߊl����
      //�n�C�X�R�A�E�G���[�ȊO
      if( state != PALPARK_FINISH_HIGHSOCRE && 
          state != PALPARK_FINISH_ERROR )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_HIGHSOCRE: // (1)  //�ߊl�����{�n�C�X�R�A
      //�G���[�ȊO
      if( state != PALPARK_FINISH_ERROR )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_NO_GET:    // (2)  //�ߊl�ł��Ȃ�����
      //�L�����Z�����G���[�̎�
      if( state != PALPARK_FINISH_NORMAL &&
          state != PALPARK_FINISH_HIGHSOCRE && 
          state != PALPARK_FINISH_ERROR )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_ERROR:     // (3)  //�G���[�I��
      //�G���[�͐��
      isSet = TRUE;
      break;

    case PALPARK_FINISH_CANCEL:    // (4)  //�L�����Z���I��
      //��ԗD��x���Ⴂ
      if( state != PALPARK_FINISH_NO_GET &&
          state != PALPARK_FINISH_NORMAL &&
          state != PALPARK_FINISH_HIGHSOCRE && 
          state != PALPARK_FINISH_ERROR )
      {
        isSet = TRUE;
      }
      break;
    }
    
    if( isSet == TRUE )
    {
      MB_TPrintf( "SetFinishState Set!!![%d->%d]\n",nowState,state );
      MISC_SetPalparkFinishState( work->miscSave , state );
    }
  }
}


#pragma mark [>save func
//--------------------------------------------------------------
//  �Z�[�u����
//--------------------------------------------------------------
static void MB_PARENT_SaveInit( MB_PARENT_WORK *work )
{
  const u8 pokeNum = MB_COMM_GetPostPokeNum( work->commWork );
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_07 );
  MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
  
  MB_PARENT_SaveInitPoke( work );
  //�X�R�A�`�F�b�N
  if( work->mode == MPM_POKE_SHIFTER )
  {
    const u16 nowScore = MISC_GetPalparkHighscore(work->miscSave);
    const u16 newScore = MB_COMM_GetScore( work->commWork );
    if( nowScore < newScore )
    {
      MISC_SetPalparkHighscore(work->miscSave,newScore);
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_HIGHSOCRE );
    }
    else
    {
      if( pokeNum == 0 )
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_NO_GET );
      }
      else
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_NORMAL );
      }
    }
    
  }
  
  MB_TPrintf( "MB_Parent Save Init\n" );
}

//�f��̂��߂Ƀ|�P�𕪗�
static void MB_PARENT_SaveInitPoke( MB_PARENT_WORK *work )
{
  u8 i;
  BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
  ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( work->initWork->gameData );
  const u8 pokeNum = MB_COMM_GetPostPokeNum( work->commWork );
  for( i=0;i<pokeNum;i++ )
  {
    const POKEMON_PASO_PARAM *ppp = MB_COMM_GetPostPokeData( work->commWork , i );
    const BOOL ret = BOXDAT_PutPokemon( boxMng , ppp );
    POKEMON_PARAM *pp = PP_CreateByPPP( ppp , work->heapId );
    ZUKANSAVE_SetPokeGet( zukan_savedata , pp );
    GFL_HEAP_FreeMemory( pp );
    
    GF_ASSERT_MSG( ret == TRUE , "Multiboot parent Box is full!!\n");
#if DEB_ARI
    {
      char name[32];
      STRBUF *nameStr = GFL_STR_CreateBuffer( 32 , work->heapId );
      PPP_Get( ppp , ID_PARA_nickname , nameStr );
      DEB_STR_CONV_StrBufferToSjis( nameStr , name , 32 );
      MB_TPrintf("[%d][%s]\n",i,name);
      GFL_STR_DeleteBuffer( nameStr );
    }
#endif
  }
}

//--------------------------------------------------------------
//  �Z�[�u�J��
//--------------------------------------------------------------
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work )
{
}

//--------------------------------------------------------------
//  �Z�[�u�X�V
//--------------------------------------------------------------
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SAVE_WAIT_SAVE_INIT:
    if( MB_MSG_CheckPrintQueIsFinish( work->msgWork ) == TRUE &&
        MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_GetIsReadyChildStartSave( work->commWork ) == TRUE )
      {
        if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_START_SAVE , GFUser_GetPublicRand(20)+10  ) == TRUE )
        {
          work->subState = MPSS_SAVE_SYNC_SAVE_INIT;
        }
      }
    }
    break;
  case MPSS_SAVE_SYNC_SAVE_INIT:
    if( MB_COMM_GetIsPermitStartSave( work->commWork ) == TRUE )
    {
      work->subState = MPSS_SAVE_INIT;
    }
    break;
    
  case MPSS_SAVE_INIT:
    GAMEDATA_SaveAsyncStart( work->initWork->gameData );
    work->subState = MPSS_SAVE_WAIT_FIRST;
    MB_TPrintf( "MB_Parent Save Start!\n" );
    break;

  case MPSS_SAVE_WAIT_FIRST:
    {
      const SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( work->initWork->gameData );
      if( ret == SAVE_RESULT_LAST )
      {
        MB_TPrintf( "MB_Parent Finish First!\n" );
  			work->subState = MPSS_SAVE_WAIT_FIRST_SYNC;
      }
    }
    break;

  case MPSS_SAVE_WAIT_FIRST_SYNC:
    if( MB_COMM_GetIsFinishChildFirstSave( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_FIRST_SAVE , GFUser_GetPublicRand(20)+10  ) == TRUE )
      {
  			work->subState = MPSS_SAVE_FIRST_SAVE_LAST;
  			work->saveWaitCnt = 0;
  		}
    }
    break;

  case MPSS_SAVE_FIRST_SAVE_LAST:
    if( MB_COMM_GetIsPermitFirstSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        MB_TPrintf( "MB_Parent FirstLast Save!\n" );
        work->subState = MPSS_SAVE_WAIT_SECOND;
      }
    }
    break;

  case MPSS_SAVE_WAIT_SECOND:
    {
      const SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( work->initWork->gameData );
  		if( ret == SAVE_RESULT_OK )
  		{
        work->subState = MPSS_SAVE_WAIT_SECOND_SYNC;
        MB_TPrintf( "MB_Parent Finish Second!\n" );
        //�e�@�͈ȍ~���ɃZ�[�u�͂��Ȃ�
      }
    }
    
    break;

  case MPSS_SAVE_WAIT_SECOND_SYNC:
    if( MB_COMM_GetIsFinishChildSecondSave( work->commWork ) == TRUE )
    {
      //���ɑ҂K�v�͖����̂ŁA�҂����Ԃ͂O�ő����Ă���
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_SECOND_SAVE , 0  ) == TRUE )
      {
        work->subState = MPSS_SAVE_SECOND_SAVE_LAST;
  			work->saveWaitCnt = 0;
  		}
    }
    break;

  case MPSS_SAVE_SECOND_SAVE_LAST:
    if( MB_COMM_GetIsFinishChildSave( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_FINISH_SAVE , 0  ) == TRUE )
      {
        work->subState = MPSS_SAVE_WAIT_FINISH_SAVE_SYNC;
      }
    }
    break;
    
  case MPSS_SAVE_WAIT_FINISH_SAVE_SYNC:
    if( MB_COMM_GetIsPermitFinishSave( work->commWork ) == TRUE )
    {
      work->state = MPS_SAVE_TERM;
      MB_TPrintf( "MB_Parent Finish All Save Seq!!!\n" );
    }
    break;
  }
}

#pragma mark [>proc movie
static void MB_PARENT_UpdateMovieMode( MB_PARENT_WORK *work )
{
  //���ʕ������番�򂵂Ă���B
  switch( work->movieState )
  {
  //�q�@�̃|�P�W�v�҂�
  case MPMS_WAIT_COUNT_POKE:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
    {
      //�ǂݍ��݃G���[����������
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_21 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_MOVIE_WAIT_LAST_MSG;
    }
    if( MB_COMM_IsPostMoviePokeNum( work->commWork ) == TRUE )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );
      const u16 hidenNum = MB_COMM_GetMoviePokeNumHiden( work->commWork );
      if( num > 0 )
      {
        MB_MSG_MessageHide( work->msgWork );

        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageCreateWordset( work->msgWork );
        //�����ł̕\���͔�`����
        MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num+hidenNum , 3 );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_03 , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        MB_MSG_MessageDeleteWordset( work->msgWork );

        work->movieState = MPMS_CONFIRM_POKE_WAIT_MSG;
      }
      else
      {
        work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
      }

      MB_TPrintf( "MB_Parent post movie poke[%d]\n",num );
    }
    break;
  
  //�A��Ă��܂����H�\���҂�
  case MPMS_CONFIRM_POKE_WAIT_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_POKE_WAIT_YESNO;
    }
    break;
  
  //�A��Ă��܂����H�͂��E�������҂�
  case MPMS_CONFIRM_POKE_WAIT_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      work->yesNoRet = ret;
      if( ret == MMYR_RET1 ) 
      {
        const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );
        BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
        const u16 leastBoxNum = BOXDAT_GetEmptySpaceTotal( boxMng );

        MB_MSG_ClearYesNoUpper( work->msgWork );
        
        if( leastBoxNum < num )
        {
          //�{�b�N�X������Ȃ��I
          work->isBoxNotEnough = TRUE;
          work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
        }
        else
        {
          //��`�`�F�b�N��
          work->isBoxNotEnough = FALSE;
          work->movieState = MPMS_CONFIRM_HIDEN_WARN_INIT;
        }
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
    }
    break;
    
  //��`�`�F�b�N��
  case MPMS_CONFIRM_HIDEN_WARN_INIT:
    if( MB_COMM_GetMoviePokeNumHiden( work->commWork ) > 0 )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );

      MB_MSG_MessageHide( work->msgWork );

      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      MB_MSG_MessageCreateWordset( work->msgWork );
      MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num , 3 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_16 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_MSG_MessageDeleteWordset( work->msgWork );

      work->movieState = MPMS_CONFIRM_HIDEN_WARN_WAIT;
    }
    else
    {
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
    }
    break;
  
  //��`�x���\���҂�
  case MPMS_CONFIRM_HIDEN_WARN_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
    }
    break;
  
  //�A�C�e���̌��`�F�b�N
  case MPMS_CONFIRM_CHECK_ITEM_INIT:
    if( MB_COMM_GetMoviePokeNumHaveItem( work->commWork ) == TRUE )
    {
      MB_MSG_MessageHide( work->msgWork );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_06 , MSGSPEED_GetWait() );
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_WAIT;
    }
    else
    {
      work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
    }
    break;
  
  //�A�C�e���߂��m�F�\���҂�
  case MPMS_CONFIRM_CHECK_ITEM_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_YESNO;
    }
    break;
  
  //�A�C�e���߂��͂��E�������\���҂�
  case MPMS_CONFIRM_CHECK_ITEM_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        if( MB_COMM_GetMoviePokeNumFullItem( work->commWork ) == TRUE )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_07 , MSGSPEED_GetWait() );
          work->movieState = MPMS_CONFIRM_CHECK_FULL_WAIT;
        }
        else
        {
          work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
        }
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CANCEL_INIT;
      }
    }
    break;

  //�A�C�e����t�m�F�\���҂�
  case MPMS_CONFIRM_CHECK_FULL_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CHECK_FULL_YESNO;
    }
    break;
    
  //�A�C�e����t�B�͂��E�������҂�
  case MPMS_CONFIRM_CHECK_FULL_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CANCEL_INIT;
      }
    }
    break;
    
  //�A�C�e���n�L�����Z���m�F
  case MPMS_CONFIRM_CANCEL_INIT:
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_08 , MSGSPEED_GetWait() );
    work->movieState = MPMS_CONFIRM_CANCEL_WAIT;
    break;

  //�A�C�e���n�L�����Z���\���҂�
  case MPMS_CONFIRM_CANCEL_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CANCEL_YESNO;
    }
    break;

  //�A�C�e���n�L�����Z���B�͂��E�������҂�
  case MPMS_CONFIRM_CANCEL_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        work->yesNoRet = MMYR_RET2;
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
      }
    }
    break;
  
  //�q�@�ɑI���̌��ʂ𑗂�
  case MPMS_CONFIRM_POKE_SEND_YESNO:
    {
      BOOL ret;
      MB_COMM_MOVIE_VALUE sendVal;
      if( work->yesNoRet == MMYR_RET1 )
      {
        if( work->isBoxNotEnough == TRUE )
        {
          sendVal = MCMV_POKETRANS_NG;
        }
        else
        {
          sendVal = MCMV_POKETRANS_YES;
        }
      }
      else
      {
        sendVal = MCMV_POKETRANS_NO;
      }
      ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_POKE_TRANS_CONFIRM , sendVal );
      if( ret == TRUE )
      {
        if( sendVal == MCMV_POKETRANS_YES )
        {
          work->movieState = MPMS_POST_POKE_WAIT;
          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_MOVIE_05 );
          MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
          
        }
        else
        if( sendVal == MCMV_POKETRANS_NG )
        {
          work->movieState = MPMS_BOX_NOT_ENOUGH;
        }
        else
        {
          work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
        }
      }
    }
    break;
    
  case MPMS_POST_POKE_WAIT:
    if( MB_COMM_IsPostPoke( work->commWork ) == TRUE )
    {
      MB_PARENT_SaveInitPoke( work );
      work->movieState = MPMS_POST_POKE_RET_POST;
    }
    if( MB_COMM_IsPostMoviePokeFinishSend( work->commWork ) == TRUE )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );

      MB_MSG_MessageHide( work->msgWork );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      MB_MSG_MessageCreateWordset( work->msgWork );
      MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num , 3 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_10 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_MSG_MessageDeleteWordset( work->msgWork );
      work->movieState = MPMS_POST_POKE_FINISH;
      work->isPostMoviePoke = TRUE;
      MB_TPrintf( "MB_Parent Finish poke trans\n" );
    }
    break;
    
  case MPMS_POST_POKE_RET_POST:
    if( MB_COMM_Send_Flag( work->commWork , MCFT_POST_POKE , 0 ) == TRUE )
    {
      MB_COMM_ClearSendPokeData( work->commWork );
      work->movieState = MPMS_POST_POKE_WAIT;
    }
    break;
  
  case MPMS_POST_POKE_FINISH:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
    }
    break;

  case MPMS_WAIT_CHECK_LOCK_CAPSULE:
    if( MB_COMM_IsPostMovieHaveLockCapsule( work->commWork ) == TRUE )
    {
      if( MB_COMM_IsMovieHaveLockCapsule( work->commWork ) == TRUE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_11 , MSGSPEED_GetWait() );

        work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_WAIT;
      }
      else
      {
        work->movieState = MPMS_CHECK_SAVE;
      }
    }
    break;
  
  case MPMS_CONFIRM_LOCK_CAPSULE_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_YESNO;
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
    }
    break;
    
  case MPMS_CONFIRM_LOCK_CAPSULE_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 || ret == MMYR_RET2 )
      {
        MYITEM_PTR myItem = GAMEDATA_GetMyItem( work->initWork->gameData );
        if( ret == MMYR_RET1 &&
           (MYITEM_CheckItem( myItem , ITEM_ROKKUKAPUSERU , 1 , work->heapId ) == TRUE ||
            MYITEM_CheckItem( myItem , ITEM_WAZAMASIN101 , 1 , work->heapId ) == TRUE) )
        {
          //���������Ă�
          work->yesNoRet = MMYR_RET2; //����������
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT;

          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_17 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        }
        else
        {
          work->yesNoRet = ret;
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO;
        }
        
      }
    }
    break;
    
  case MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO;
    }
    break;
  
  case MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO:
    {
      const BOOL flg = ( work->yesNoRet == MMYR_RET1 ? TRUE : FALSE );
      const BOOL ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_LOCK_CAPSULE_TRANS_CONFIRM , flg );
      if( ret == TRUE )
      {
        if( flg == TRUE )
        {
          MYITEM_PTR myItem = GAMEDATA_GetMyItem( work->initWork->gameData );
          MYSTATUS *myStatus = GAMEDATA_GetMyStatus( work->initWork->gameData );
          MISC *miscData = GAMEDATA_GetMiscWork( work->initWork->gameData );
          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_13 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
          
          EVTLOCK_SetEvtLock( miscData , EVT_LOCK_NO_LOCKCAPSULE , myStatus );
          MYITEM_AddItem( myItem , ITEM_ROKKUKAPUSERU , 1 , work->heapId );
          work->isPostMovieCapsule = TRUE;
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT;
        }
        else
        {
          work->movieState = MPMS_CHECK_SAVE;
        }
      }
    }
    break;

  case MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CHECK_SAVE;
    }
    break;
    
  case MPMS_CHECK_SAVE:
    if( work->isPostMoviePoke == TRUE || work->isPostMovieCapsule == TRUE )
    {
      const BOOL ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_START_SAVE_CHECK , 0 );
      if( ret == TRUE )
      {
        work->state = MPS_WAIT_CRC_CHECK;
      }
    }
    else
    {
      if( MB_COMM_GetMoviePokeNum( work->commWork ) == 0 &&
          MB_COMM_IsMovieHaveLockCapsule( work->commWork ) == FALSE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_20 , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      }
      work->state = MPS_MOVIE_WAIT_SAVE_MSG;
    }
    break;
  }
}


#pragma mark [>proc func
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBoot_ProcData =
{
  MB_PARENT_ProcInit,
  MB_PARENT_ProcMain,
  MB_PARENT_ProcTerm
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_INIT_WORK *initWork;
  MB_PARENT_WORK *work;
  
  if( GFL_NET_IsExit() == FALSE )
  {
    OS_TPrintf("!\n");
    return GFL_PROC_RES_CONTINUE;
  }
  
  MB_TPrintf("LeastAppHeap[%x]\n",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x130000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_PARENT_WORK), HEAPID_MULTIBOOT );
  if( pwk == NULL )
  {
    initWork = GFL_HEAP_AllocMemory( HEAPID_MULTIBOOT , sizeof( MB_PARENT_INIT_WORK ));
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      MB_TPrintf("Boot mode movie!\n");
      initWork->mode = MPM_MOVIE_TRANS;
    }
    else
    {
      MB_TPrintf("Boot mode poke shifter!\n");
      initWork->mode = MPM_POKE_SHIFTER;
      initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
    }
  }
  else
  {
    initWork = pwk;
  }
  
  if( initWork->mode == MPM_MOVIE_TRANS )
  {
    initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
  }
  
  work->heapId = HEAPID_MULTIBOOT;
  work->initWork = initWork;
  work->isNetErr = FALSE;
  
  MB_PARENT_Init( work );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  
  MB_PARENT_Term( work );

  if( pwk == NULL )
  {
    GAMEDATA_Delete( work->initWork->gameData );
    GFL_HEAP_FreeMemory( work->initWork );
  }
  else
  {
    if( work->initWork->mode == MPM_MOVIE_TRANS )
    {
      GAMEDATA_Delete( work->initWork->gameData );
      GFL_HEAP_FreeMemory( work->initWork );
    }
  }
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  const BOOL ret = MB_PARENT_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isNetErr == FALSE )
  {
    NetErr_App_ReqErrorDisp();
    work->isNetErr = TRUE;
  }
  
  return GFL_PROC_RES_CONTINUE;
}

