//======================================================================
/**
 * @file	irc_battle_main.c
 * @brief	�ԊO���o�g��
 * @author	ariizumi
 * @data	09/10/06
 *
 * ���W���[�����FIRC_BATTLE
 */
//======================================================================
#include <gflib.h>

#include "gamesystem/btl_setup.h"
#include "gamesystem/msgspeed.h"
#include "poke_tool/pokeparty.h"
#include "sound/pm_sndsys.h"
#include "system/bmp_winframe.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "battle_championship.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_irc_battle.h"

#include "test/ariizumi/ari_debug.h"

#include "irc_battle_subproc.h"
#include "irc_battle_local_def.h"
//�f�o�O�f�[�^�쐬�p
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define IRC_BATTLE_FRAME_STR ( GFL_BG_FRAME1_M )
#define IRC_BATTLE_FRAME_BG  ( GFL_BG_FRAME3_M )

#define IRC_BATTLE_FRAME_SUB_BG  ( GFL_BG_FRAME3_S )

#define IRC_BATTLE_BG_PAL_WINFRAME (0xD)
#define IRC_BATTLE_BG_PAL_FONT (0xE)

#define IRC_BATTLE_WINFRAME_CGX (1)

#define IRC_BATTLE_BATTLE_DATA_SEND_SIZE (sizeof(IRC_BATTLE_BATTLE_DATA)+PokeParty_GetWorkSize())

FS_EXTERN_OVERLAY(battle);


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};

static GFL_PROC_RESULT IRC_BATTLE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT IRC_BATTLE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT IRC_BATTLE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static void IRC_BATTLE_CreateCompareData( BATTLE_CHAMPIONSHIP_DATA *csData , IRC_BATTLE_COMPARE_MATCH *compareData );
static const BOOL IRC_BATTLE_CompareMatchData( IRC_BATTLE_WORK *work );
static const BOOL IRC_BATTLE_CompareMatchDataFunc( IRC_BATTLE_COMPARE_MATCH *data1 , IRC_BATTLE_COMPARE_MATCH *data2 );
static void* IRC_BATTLE_CreateBattleData( IRC_BATTLE_WORK *work , void *battleData );

static void IRC_BATTLE_InitBattleProc( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_InitBattleSetupParam( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_ExitBattleProc( IRC_BATTLE_WORK *work );

static void IRC_BATTLE_InitGraphic( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void IRC_BATTLE_TermGraphic( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_LoadResource( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_ReleaseResource( IRC_BATTLE_WORK *work );

static void IRC_BATTLE_InitMessage( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_TermMessage( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_UpdateMessage( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_ShowMessage( IRC_BATTLE_WORK *work  , const u16 msgNo );
static void IRC_BATTLE_HideMessage( IRC_BATTLE_WORK *work );
static const BOOL IRC_BATTLE_IsFinishMessage( IRC_BATTLE_WORK *work );

static void IRC_BATTLE_InitNet( IRC_BATTLE_WORK *work );
static void IRC_BATTLE_ExitNet( IRC_BATTLE_WORK *work );
static const BOOL IRC_BATTLE_Send_Flag( IRC_BATTLE_WORK* work , u8 flagType , u8 value );
static void IRC_BATTLE_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static const BOOL IRC_BATTLE_Send_CompareMatchData( IRC_BATTLE_WORK* work );
static void IRC_BATTLE_Post_CompareMatchData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8* IRC_BATTLE_Post_CompareMatchDataBuff( int netID, void* pWork , int size );
static const BOOL IRC_BATTLE_IsFinishPostCompareMatchData( IRC_BATTLE_WORK* work );
static const BOOL IRC_BATTLE_Send_BattleData( IRC_BATTLE_WORK* work );
static void IRC_BATTLE_Post_BattleData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8* IRC_BATTLE_Post_BattleDataBuff( int netID, void* pWork , int size );
static const BOOL IRC_BATTLE_IsFinishPostBattleData( IRC_BATTLE_WORK* work );

GFL_PROC_DATA IRC_BATTLE_ProcData =
{
  IRC_BATTLE_ProcInit,
  IRC_BATTLE_ProcMain,
  IRC_BATTLE_ProcTerm
};

static const NetRecvFuncTable IRC_BATTLE_CommRecvTable[] = 
{
  { IRC_BATTLE_Post_Flag , NULL },
  { IRC_BATTLE_Post_CompareMatchData , IRC_BATTLE_Post_CompareMatchDataBuff },
  { IRC_BATTLE_Post_BattleData , IRC_BATTLE_Post_BattleDataBuff },
};

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
#pragma mark [>proc 
static GFL_PROC_RESULT IRC_BATTLE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  u8 i;
  IRC_BATTLE_WORK *work;
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRC_BATTLE, 0x18000 );

  work = GFL_PROC_AllocWork( proc, sizeof(IRC_BATTLE_WORK), HEAPID_IRC_BATTLE );
  work->heapId = HEAPID_IRC_BATTLE;
  
  if( pwk == NULL )
  {
    u8 i;
    work->initWork = GFL_HEAP_AllocClearMemory( work->heapId , sizeof(IRC_BATTLE_INIT_WORK) );
    work->initWork->csData = GFL_HEAP_AllocClearMemory( work->heapId , sizeof(BATTLE_CHAMPIONSHIP_DATA) );
    BATTLE_CHAMPIONSHIP_SetDebugData( work->initWork->csData , work->heapId );
    work->initWork->gameData = GAMEDATA_Create( work->heapId );
  }
  else
  {
    work->initWork = pwk;
  }
  
  work->state = IBS_INIT_NET;
  work->subProcWork = NULL;
  work->battleParty = NULL;

  IRC_BATTLE_InitGraphic( work );
  IRC_BATTLE_LoadResource( work );
  IRC_BATTLE_InitMessage( work );
  
  WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT IRC_BATTLE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  IRC_BATTLE_WORK *work = mywk;

  IRC_BATTLE_TermMessage( work );
  IRC_BATTLE_ReleaseResource( work );
  IRC_BATTLE_TermGraphic( work );
  
  if( work->battleParty != NULL )
  {
    GFL_HEAP_FreeMemory( work->battleParty );
  }
  
  if( pwk == NULL )
  {
    GAMEDATA_Delete( work->initWork->gameData );
    BATTLE_CHAMPIONSHIP_TermDebugData( work->initWork->csData );
    GFL_HEAP_FreeMemory( work->initWork->csData );
    GFL_HEAP_FreeMemory( work->initWork );
  }
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_IRC_BATTLE );
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT IRC_BATTLE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  IRC_BATTLE_WORK *work = mywk;
  switch( work->state )
  {
  case IBS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = IBS_FADEIN_WAIT;
    break;

  case IBS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->state = IBS_INIT_NET;
    }
    break;

  case IBS_INIT_NET:
    IRC_BATTLE_InitNet( work );
    work->state = IBS_INIT_NET_WAIT;
    break;
    
  case IBS_INIT_NET_WAIT:
    if( GFL_NET_IsInit() == TRUE )
    {
      work->state = IBS_IRC_CONNECT_MSG;
      ARI_TPrintf("NetInit!!\n");
    }
    break;
    
    //�ԊO�������킹��A�{�^���������Ă�������
  case IBS_IRC_CONNECT_MSG:
    IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_01 );
    work->state = IBS_IRC_CONNECT_MSG_WAIT;
    break;

  case IBS_IRC_CONNECT_MSG_WAIT:
    if( IRC_BATTLE_IsFinishMessage( work ) == TRUE )
    {
      work->state = IBS_IRC_TRY_CONNECT;
      GFL_NET_ChangeoverConnect( NULL );
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_WAIT_COMM );
    }
    break;
    
  //�ڑ���
  case IBS_IRC_TRY_CONNECT:
    if( GFL_NET_GetConnectNum() > 1 )
    {
      ARI_TPrintf("Connect!!\n");
      if( IRC_BATTLE_Send_CompareMatchData( work ) == TRUE )
      {
        work->state = IBS_WAIT_POST_MATCHDATA;
      }
    }
    break;
    
  case IBS_WAIT_POST_MATCHDATA:
    if( IRC_BATTLE_IsFinishPostCompareMatchData( work ) == TRUE )
    {
      if( IRC_BATTLE_CompareMatchData( work ) == TRUE )
      {
        ARI_TPrintf("Match is same!!\n");
        work->state = IBS_MATCHDATA_IS_SAME;
      }
      else
      {
        ARI_TPrintf("Match is not same!!\n");
        work->state = IBS_MACHIDATA_NOT_SAME;
      }
    }
    break;

  case IBS_MATCHDATA_IS_SAME:  //�}�b�`�f�[�^��v
    IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_03 );
    work->state = IBS_MATCHDATA_IS_SAME_WAIT;
    break;
    
  case IBS_MATCHDATA_IS_SAME_WAIT:
    if( IRC_BATTLE_IsFinishMessage( work ) == TRUE )
    {
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_WAIT_COMM );
      work->state = IBS_SEND_BATTLE_DATA;
    }
    break;
    
  case IBS_MACHIDATA_NOT_SAME: //�}�b�`�f�[�^�̕s��v
    IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_02 );
    work->state = IBS_MACHIDATA_NOT_SAME_WAIT;
    break;

  case IBS_MACHIDATA_NOT_SAME_WAIT:
    if( IRC_BATTLE_IsFinishMessage( work ) == TRUE )
    {
      work->state = IBS_EXIT_NET;
    }
    break;
    
  //�o�g���p�f�[�^���M
  case IBS_SEND_BATTLE_DATA:
    if( IRC_BATTLE_Send_BattleData( work ) == TRUE )
    {
      work->state = IBS_SELECT_POKE_FADEOUT;
    }
    break;
    

  //�|�P�����I��
  case IBS_SELECT_POKE_FADEOUT:
    if( IRC_BATTLE_IsFinishPostBattleData( work ) == TRUE )
    {
      WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      work->state = IBS_SELECT_POKE_FADEOUT_WAIT;
    }
    break;
    
  case IBS_SELECT_POKE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      //�J��
      IRC_BATTLE_TermMessage( work );
      IRC_BATTLE_ReleaseResource( work );
      IRC_BATTLE_TermGraphic( work );
      work->subProcWork = IRC_BATTLE_SUBPROC_InitSubProc( work );
      work->state = IBS_SELECT_POKE_WAIT;
    }
    break;

  case IBS_SELECT_POKE_WAIT:
    {
      const BOOL ret = IRC_BATTLE_SUBPROC_UpdateSubProc( work , work->subProcWork );
      if( ret == TRUE )
      {
        IRC_BATTLE_SUBPROC_TermSubProc( work , work->subProcWork );
        work->subProcWork = NULL;
        work->state = IBS_SELECT_POKE_FADEIN;
      }
    }
    break;

  case IBS_SELECT_POKE_FADEIN:
    {
      const BOOL ret = IRC_BATTLE_Send_Flag( work , IBFT_FINISH_POKELIST , 0 );
      if( ret == TRUE )
      {
        IRC_BATTLE_InitGraphic( work );
        IRC_BATTLE_LoadResource( work );
        IRC_BATTLE_InitMessage( work );
        IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_WAIT_COMM );
        WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                        WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
        work->state = IBS_SELECT_POKE_FADEIN_WAIT;
      }
    }
    break;
    
  case IBS_SELECT_POKE_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      GFL_NET_TimingSyncStart( selfHandle , IBT_POKELIST_END );
      work->state = IBS_SELECT_POKE_TIMMING;
    }
    break;

  case IBS_SELECT_POKE_TIMMING:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      const BOOL ret = GFL_NET_IsTimingSync( selfHandle , IBT_POKELIST_END );
      if( ret == TRUE )
      {
        work->state = IBS_START_BATTLE;
      }
    }
    break;
    
    
  case IBS_START_BATTLE: //�o�g���J�n�ҋ@
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_04 );
      work->state = IBS_START_BATTLE_WAIT;
    break;
    
  case IBS_START_BATTLE_WAIT:
    if( IRC_BATTLE_IsFinishMessage( work ) == TRUE )
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      GFL_NET_TimingSyncStart( selfHandle , IBT_BATTLE_START );
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_WAIT_COMM );
      //IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_04 );
      work->state = IBS_START_BATTLE_TIMMING;
    }
    break;
    
  case IBS_START_BATTLE_TIMMING:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      const BOOL ret = GFL_NET_IsTimingSync( selfHandle , IBT_BATTLE_START );
      if( ret == TRUE )
      {
        OS_TPrintf("Battle!!\n");
        work->state = IBS_BATTLE_FADEOUT;
      }
    }
    break;

  //�o�g��
  case IBS_BATTLE_FADEOUT://�o�g��
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = IBS_BATTLE_FADEOUT_WAIT;
    break;

  case IBS_BATTLE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->state = IBS_BATTLE_WAIT;
      IRC_BATTLE_InitBattleProc( work );
      GFL_PROC_SysCallProc( NO_OVERLAY_ID, &BtlProcData, &work->battleParam );
    }
    break;

  case IBS_BATTLE_WAIT:
    OS_TPrintf("FinishBattle!!\n");
    IRC_BATTLE_ExitBattleProc( work );
    work->state = IBS_BATTLE_FADEIN;
    break;

  case IBS_BATTLE_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = IBS_BATTLE_FADEIN_WAIT;
    break;

  case IBS_BATTLE_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      GFL_NET_TimingSyncStart( selfHandle , IBT_BATTLE_END );
      work->state = IBS_BATTLE_TIMMING;
    }
    break;

  case IBS_BATTLE_TIMMING:  //�����҂�
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      const BOOL ret = GFL_NET_IsTimingSync( selfHandle , IBT_BATTLE_END );
      if( ret == TRUE )
      {
        work->state = IBS_DRAW_RESULT;
      }
    }
    break;

  //���ʔ��\
  case IBS_DRAW_RESULT:
    OS_TPrintf("result[%d]\n",work->battleParam.result);
    switch( work->battleParam.result )
    {
    case BTL_RESULT_WIN:
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_05 );
      break;
    case BTL_RESULT_LOSE:
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_06 );
      break;
    case BTL_RESULT_DRAW:
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_07 );
      break;
    default:
      //GF_ASSERT_MSG( NULL , "Battle result is unknown[%d]\n",work->battleParam.result );
      IRC_BATTLE_ShowMessage( work , IRC_BATTLE_MSG_07 );
      break;
    }
    work->state = IBS_DRAW_RESULT_WAIT;
    break;

  case IBS_DRAW_RESULT_WAIT:
    if( IRC_BATTLE_IsFinishMessage( work ) == TRUE )
    {
      work->state = IBS_FADEOUT;
    }
    break;
    
  //�I��
  case IBS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = IBS_FADEOUT_WAIT;
    break;

  case IBS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->state = IBS_EXIT_NET;
    }
    break;

  case IBS_EXIT_NET:
    IRC_BATTLE_ExitNet( work );
    work->state = IBS_EXIT_NET_WAIT;
    break;
    
  case IBS_EXIT_NET_WAIT:
    if( GFL_NET_IsExit() == TRUE )
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  
  IRC_BATTLE_UpdateMessage( work );

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    work->selfCmpareData.championsipNumber++;
    work->selfCmpareData.championsipLeague++;
  }

  return GFL_PROC_RES_CONTINUE;
//  return GFL_PROC_RES_FINISH;
}

//��r�p�f�[�^�̍쐬
static void IRC_BATTLE_CreateCompareData( BATTLE_CHAMPIONSHIP_DATA *csData , IRC_BATTLE_COMPARE_MATCH *compareData )
{
  compareData->championsipNumber = csData->number;
  compareData->championsipLeague = csData->league;
}

//�f�[�^�̔�r
static const BOOL IRC_BATTLE_CompareMatchData( IRC_BATTLE_WORK *work )
{
  u8 i;
  for( i=0;i<IRC_BATTLE_MEMBER_NUM;i++ )
  {
    if( IRC_BATTLE_CompareMatchDataFunc( &work->selfCmpareData , &work->postCmpareData[i] ) == FALSE )
    {
      return FALSE;
    }
  }
  return TRUE;
}
static const BOOL IRC_BATTLE_CompareMatchDataFunc( IRC_BATTLE_COMPARE_MATCH *data1 , IRC_BATTLE_COMPARE_MATCH *data2 )
{
  if( data1->championsipNumber == data2->championsipNumber &&
      data1->championsipLeague == data2->championsipLeague )
  {
    return TRUE;
  }
  return FALSE;
}

//���M�p�o�g���f�[�^�쐬
static void* IRC_BATTLE_CreateBattleData( IRC_BATTLE_WORK *work , void *battleData )
{
  u8 i;
  IRC_BATTLE_BATTLE_DATA *btlData = battleData;
  {
    //���O�E����
    MYSTATUS *myStatsu = GAMEDATA_GetMyStatus( work->initWork->gameData );
    MyStatus_CopyNameStrCode( myStatsu , btlData->name , PERSON_NAME_SIZE + EOM_SIZE );
    btlData->sex = MyStatus_GetMySex( myStatsu );
  }
  {
    //Party
    POKEPARTY *pokeParty = (POKEPARTY*)((u32)battleData + sizeof(IRC_BATTLE_BATTLE_DATA));
    PokeParty_Init( pokeParty , CHAMPIONSHIP_POKE_NUM );
    for( i=0;i<CHAMPIONSHIP_POKE_NUM;i++ )
    {
      if( PPP_Get( work->initWork->csData->ppp[i] , ID_PARA_poke_exist , NULL ) )
      {
        POKEMON_PARAM *pp = PP_CreateByPPP( work->initWork->csData->ppp[i] , work->heapId );
        PokeParty_Add( pokeParty , pp );
        GFL_HEAP_FreeMemory( pp );
      }
    }
  }
  
  return battleData;
}

POKEPARTY* IRC_BATTLE_BattleData_GetPokeParty( void *battleData )
{
  return (POKEPARTY*)((u32)battleData + sizeof(IRC_BATTLE_BATTLE_DATA));
  
}

#pragma mark [>battle proc
static void IRC_BATTLE_InitBattleProc( IRC_BATTLE_WORK *work )
{
  //�J��
  IRC_BATTLE_TermMessage( work );
  IRC_BATTLE_ReleaseResource( work );
  IRC_BATTLE_TermGraphic( work );

  OS_TPrintf("BattleStart least heap[%x]\n",GFL_HEAP_GetHeapFreeSize( HEAPID_IRC_BATTLE ) );

  //�Ăяo��
  IRC_BATTLE_InitBattleSetupParam( work );

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
  GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);

}
static void IRC_BATTLE_InitBattleSetupParam( IRC_BATTLE_WORK *work )
{
  BTL_SETUP_Single_Comm( &work->battleParam , work->initWork->gameData , 
                         GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, work->heapId );

  BATTLE_PARAM_SetPokeParty( &work->battleParam, work->battleParty, BTL_CLIENT_PLAYER );
  
  OS_TPrintf("[[%d]]\n",PokeParty_GetPokeCount(work->battleParam.partyPlayer));
}

static void IRC_BATTLE_ExitBattleProc( IRC_BATTLE_WORK *work )
{
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
  GFL_NET_DelCommandTable( GFL_NET_CMD_BATTLE );

  BATTLE_PARAM_Release( &work->battleParam );
  OS_TPrintf("BattleEnd least heap[%x]\n",GFL_HEAP_GetHeapFreeSize( HEAPID_IRC_BATTLE ) );

  IRC_BATTLE_InitGraphic( work );
  IRC_BATTLE_LoadResource( work );
  IRC_BATTLE_InitMessage( work );
}

#pragma mark [>graphic func
static void IRC_BATTLE_InitGraphic( IRC_BATTLE_WORK *work )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
  GFL_DISP_SetBank( &vramBank );

  //BG�n�̏�����
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
/*
    // BG0 MAIN (
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
*/
    // BG1 MAIN (����
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
/*
    // BG2 MAIN (
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
*/
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );

    IRC_BATTLE_SetupBgFunc( &header_main3 , IRC_BATTLE_FRAME_BG , GFL_BG_MODE_TEXT );
    IRC_BATTLE_SetupBgFunc( &header_main1 , IRC_BATTLE_FRAME_STR , GFL_BG_MODE_TEXT );

    IRC_BATTLE_SetupBgFunc( &header_sub3 , IRC_BATTLE_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
  }
}
//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void IRC_BATTLE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

static void IRC_BATTLE_TermGraphic( IRC_BATTLE_WORK *work )
{
  GFL_BG_FreeBGControl( IRC_BATTLE_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( IRC_BATTLE_FRAME_STR );
  GFL_BG_FreeBGControl( IRC_BATTLE_FRAME_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

static void IRC_BATTLE_LoadResource( IRC_BATTLE_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_BATTLE_CHAMPIONSHIP , work->heapId );

  ////BG���\�[�X
  //����ʔw�i
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_battle_championship_connect_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_battle_championship_connect_sub_NCGR ,
                    IRC_BATTLE_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_battle_championship_connect_sub_NSCR , 
                    IRC_BATTLE_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  //���ʔw�i
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_battle_championship_connect_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_battle_championship_connect_NCGR ,
                    IRC_BATTLE_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_battle_championship_connect_01_NSCR , 
                    IRC_BATTLE_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );

  GFL_ARC_CloseDataHandle( arcHandle );
}

static void IRC_BATTLE_ReleaseResource( IRC_BATTLE_WORK *work )
{
  
}

#pragma mark [>message func
static void IRC_BATTLE_InitMessage( IRC_BATTLE_WORK *work )
{
  //���b�Z�[�W
  work->msgWin = GFL_BMPWIN_Create( IRC_BATTLE_FRAME_STR , 
                  1 , 19 , 30 , 4 , IRC_BATTLE_BG_PAL_FONT ,
                  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( work->msgWin );

  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_irc_battle_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , IRC_BATTLE_BG_PAL_FONT*16*2, 16*2, work->heapId );
  BmpWinFrame_GraphicSet( IRC_BATTLE_FRAME_STR , IRC_BATTLE_WINFRAME_CGX , 
                          IRC_BATTLE_BG_PAL_WINFRAME , 1 , work->heapId );
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
  work->msgStr = NULL;
  
}

static void IRC_BATTLE_TermMessage( IRC_BATTLE_WORK *work )
{
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
  }
  if( work->printHandle != NULL )
  {
    PRINTSYS_PrintStreamDelete( work->printHandle );
  }
  GFL_TCBL_Exit( work->tcblSys );
  GFL_BMPWIN_Delete( work->msgWin );
  GFL_MSG_Delete( work->msgHandle );
  GFL_FONT_Delete( work->fontHandle );
}

static void IRC_BATTLE_UpdateMessage( IRC_BATTLE_WORK *work )
{
  GFL_TCBL_Main( work->tcblSys );

  if( work->printHandle != NULL  )
  {
    const PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( work->printHandle );
    if( state == PRINTSTREAM_STATE_DONE )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
      {
        PRINTSYS_PrintStreamDelete( work->printHandle );
        work->printHandle = NULL;
      }
    }
    else
    if( state == PRINTSTREAM_STATE_PAUSE )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
      {
        PRINTSYS_PrintStreamReleasePause( work->printHandle );
      }
    }
  }
}
static void IRC_BATTLE_ShowMessage( IRC_BATTLE_WORK *work  , const u16 msgNo )
{
  if( work->printHandle != NULL )
  {
    OS_TPrintf( "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }
  {
    if( work->msgStr != NULL )
    {
      GFL_STR_DeleteBuffer( work->msgStr );
      work->msgStr = NULL;
    }
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0xF );
    work->msgStr = GFL_MSG_CreateString( work->msgHandle , msgNo );
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, work->msgStr ,work->fontHandle ,
                        MSGSPEED_GetWait() , work->tcblSys , 2 , work->heapId , 0xf );
    BmpWinFrame_Write( work->msgWin , WINDOW_TRANS_ON_V , 
                       IRC_BATTLE_WINFRAME_CGX , IRC_BATTLE_BG_PAL_WINFRAME );
  }
}

static void IRC_BATTLE_HideMessage( IRC_BATTLE_WORK *work )
{
  if( work->printHandle != NULL )
  {
    OS_TPrintf( "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
    work->msgStr = NULL;
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  BmpWinFrame_Clear( work->msgWin , WINDOW_TRANS_ON_V );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );

}

static const BOOL IRC_BATTLE_IsFinishMessage( IRC_BATTLE_WORK *work )
{
  if( work->printHandle == NULL )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#pragma mark [>net func
static void IRC_BATTLE_InitNet( IRC_BATTLE_WORK *work )
{
  static GFLNetInitializeStruct aGFLNetInit = {
    IRC_BATTLE_CommRecvTable,  // ��M�֐��e�[�u��
    NELEMS(IRC_BATTLE_CommRecvTable), //��M�e�[�u���v�f��
    NULL,   ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,   ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    NULL,//IrcBattleBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    NULL,//FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
    0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    IRC_BATTLE_MEMBER_NUM,     // �ő�ڑ��l��
    100,  //�ő呗�M�o�C�g��
    16,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_IRC_BATTLE,  //GameServiceID
    0xfffe,	// �ԊO���^�C���A�E�g����
    0,//MP�ʐM�e�@���M�o�C�g��
    0,//dummy
  };
  GFL_NET_Init(&aGFLNetInit, NULL, work);	//�ʐM������

  {
    u8 i;
    work->sendBattleData = GFL_HEAP_AllocMemory( work->heapId , IRC_BATTLE_BATTLE_DATA_SEND_SIZE );
    for( i=0;i<IRC_BATTLE_MEMBER_NUM;i++)
    {
      work->postBattleData[i] = GFL_HEAP_AllocMemory( work->heapId , IRC_BATTLE_BATTLE_DATA_SEND_SIZE );
      work->isPostCompareData[i] = FALSE;
      work->isPostBattleData[i] = FALSE;
    }
  }
}

static void IRC_BATTLE_ExitNet( IRC_BATTLE_WORK *work )
{
  u8 i;
  GFL_HEAP_FreeMemory( work->sendBattleData );
  for( i=0;i<IRC_BATTLE_MEMBER_NUM;i++)
  {
    GFL_HEAP_FreeMemory( work->postBattleData[i] );
  }

  GFL_NET_Exit( NULL );
}

#pragma mark [> send/post func

//�t���O����M
typedef struct
{
  u8 flg;
  u8 value;
}IRC_BATTLE_FLAG_PACKET;

static const BOOL IRC_BATTLE_Send_Flag( IRC_BATTLE_WORK* work , u8 flagType , u8 value )
{
  IRC_BATTLE_FLAG_PACKET pkt;
  ARI_TPrintf("Send FlagData[%d:%d]\n",flagType,value );
  pkt.flg = flagType;
  pkt.value = value;
  //�f�[�^�쐬
  {
    GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                              IBST_FLAG , sizeof(IRC_BATTLE_FLAG_PACKET) , 
                              &pkt , TRUE , FALSE , FALSE );
    if( ret == FALSE )
    {
      ARI_TPrintf("Send FlagData is failued!!\n");
    }
    return ret;
  }
}

static void IRC_BATTLE_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  const IRC_BATTLE_FLAG_PACKET *pkt = (IRC_BATTLE_FLAG_PACKET*)pData;
  IRC_BATTLE_WORK *work = (IRC_BATTLE_WORK*)pWork;
  ARI_TPrintf("Finish Post FlagData[%d:%d]\n",pkt->flg,pkt->value);

  switch(pkt->flg)
  {
  case IBFT_FINISH_POKELIST:
    if( work->subProcWork != NULL )
    {
      IRC_BATTLE_SUBPROC_PostFinishPokelist( work->subProcWork );
    }
    break;
  }
}


//�}�b�`���O��r�f�[�^���M
static const BOOL IRC_BATTLE_Send_CompareMatchData( IRC_BATTLE_WORK* work )
{
  ARI_TPrintf("Send MatchData \n");
  
  //�f�[�^�쐬
  IRC_BATTLE_CreateCompareData( work->initWork->csData , &work->selfCmpareData );
  {
    GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                              IBST_MATCH_DATA , sizeof(IRC_BATTLE_COMPARE_MATCH) , 
                              &work->selfCmpareData , TRUE , FALSE , TRUE );
    if( ret == FALSE )
    {
      ARI_TPrintf("Send MatchData is failued!!\n");
    }
    return ret;
  }
}

static void IRC_BATTLE_Post_CompareMatchData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  IRC_BATTLE_WORK *work = (IRC_BATTLE_WORK*)pWork;
  ARI_TPrintf("Finish Post MatchData.\n");
  work->isPostCompareData[netID] = TRUE;
}

static u8* IRC_BATTLE_Post_CompareMatchDataBuff( int netID, void* pWork , int size )
{
  IRC_BATTLE_WORK *work = (IRC_BATTLE_WORK*)pWork;
  ARI_TPrintf("Start Post MatchData.\n");
  return (u8*)&work->postCmpareData[netID];
}

static const BOOL IRC_BATTLE_IsFinishPostCompareMatchData( IRC_BATTLE_WORK* work )
{
  u8 i;
  for( i=0;i<IRC_BATTLE_MEMBER_NUM;i++ )
  {
    if( work->isPostCompareData[i] == FALSE )
    {
      return FALSE;
    }
  }
  return TRUE;
}


//�o�g���p���l�f�[�^���M
//���M����POKEPARTY���f�[�^�̌��ɂ���
static const BOOL IRC_BATTLE_Send_BattleData( IRC_BATTLE_WORK* work )
{
  ARI_TPrintf("Send BattleData \n");
  //�f�[�^�̍쐬
  work->sendBattleData = IRC_BATTLE_CreateBattleData( work , work->sendBattleData );
  {
    GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                              IBST_BATTLE_DATA , IRC_BATTLE_BATTLE_DATA_SEND_SIZE , 
                              work->sendBattleData , TRUE , FALSE , TRUE );
    if( ret == FALSE )
    {
      ARI_TPrintf("Send BattleData is failued!!\n");
    }
    return ret;
  }
}

static void IRC_BATTLE_Post_BattleData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  IRC_BATTLE_WORK *work = (IRC_BATTLE_WORK*)pWork;
  ARI_TPrintf("Finish Post BattleData.\n");
  work->isPostBattleData[netID] = TRUE;
  
  //POKEPARTY�̃|�C���^�ݒ菈��
  {
    IRC_BATTLE_BATTLE_DATA *battleData = work->postBattleData[netID];
    battleData->pokeParty = IRC_BATTLE_BattleData_GetPokeParty( work->postBattleData[netID] );
  }
}

static u8* IRC_BATTLE_Post_BattleDataBuff( int netID, void* pWork , int size )
{
  IRC_BATTLE_WORK *work = (IRC_BATTLE_WORK*)pWork;
  ARI_TPrintf("Start Post BattleData.\n");
  return (u8*)work->postBattleData[netID];
}

static const BOOL IRC_BATTLE_IsFinishPostBattleData( IRC_BATTLE_WORK* work )
{
  u8 i;
  for( i=0;i<IRC_BATTLE_MEMBER_NUM;i++ )
  {
    if( work->isPostBattleData[i] == FALSE )
    {
      return FALSE;
    }
  }
  return TRUE;
}
