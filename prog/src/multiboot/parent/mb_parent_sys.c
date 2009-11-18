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
#include "net/wih.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "mb_parent.naix"

#include "multiboot/mb_parent_sys.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/comm/mbp.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_local_def.h"
//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define MB_PARENT_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_PARENT_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_PARENT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

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
}MB_PARENT_STATE;

typedef enum
{
  MPSS_SEND_IMAGE_INIT_COMM,
  MPSS_SEND_IMAGE_INIT_COMM_WAIT,
  MPSS_SEND_IMAGE_WAIT_SEARCH_CH,
  MPSS_SEND_IMAGE_MBSYS_MAIN,
  
  MPSS_SEND_IMAGE_WAIT_BOOT_INIT,
  MPSS_SEND_IMAGE_WAIT_BOOT,
//-------------------------------

  
}MB_PARENT_SUB_STATE;


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  MB_PARENT_INIT_WORK *initWork;
  MB_COMM_WORK *commWork;
  
  MB_PARENT_STATE state;
  u8              subState;
  
  //SendImage
  STRBUF *romTitle; //DLROM�^�C�g��
  STRBUF *romInfo;  //DLROM����
  u16    *romTitleStr;
  u16    *romInfoStr;
  
  //���b�Z�[�W�p
  MB_MSG_WORK *msgWork;
  
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
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


static void MB_PARENT_Init( MB_PARENT_WORK *work );
static void MB_PARENT_Term( MB_PARENT_WORK *work );
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work );

static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work );

static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work );
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work );


//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static void MB_PARENT_Init( MB_PARENT_WORK *work )
{
  work->state = MPS_FADEIN;
  
  MB_PARENT_InitGraphic( work );
  MB_PARENT_LoadResource( work );
  work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_MSG );
  
  work->commWork = MB_COMM_CreateSystem( work->heapId );
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_PARENT_Term( MB_PARENT_WORK *work )
{
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
      work->state = MPS_SEND_IMAGE_INIT;
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
    break;
    
  }

  MB_MSG_MessageMain( work->msgWork );
  
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

    MB_PARENT_SetupBgFunc( &header_main1 , MB_PARENT_FRAME_MSG , GFL_BG_MODE_TEXT );
    //MB_PARENT_SetupBgFunc( &header_main2 , LTVT_FRAME_CHARA , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main3 , MB_PARENT_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub3  , MB_PARENT_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  
}

static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work )
{
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BG );
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
                    MB_PARENT_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_main_NSCR , 
                    MB_PARENT_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  
  GFL_ARC_CloseDataHandle( arcHandle );
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
  work->romTitleStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_NAME_LENGTH*2 );
  work->romInfoStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_INTRO_LENGTH*2 );
  MB_MSG_MessageCreateWordset( work->msgWork );
  MB_MSG_MessageWordsetName( work->msgWork , 0 , GAMEDATA_GetMyStatus(work->initWork->gameData) );
  {
    STRBUF *workStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE );
    titleStr = GFL_STR_CreateBuffer( 128 , work->heapId );
    WORDSET_ExpandStr( MB_MSG_GetWordSet(work->msgWork) , titleStr , workStr );
    GFL_STR_DeleteBuffer( workStr );
  }
  MB_MSG_MessageDeleteWordset( work->msgWork );

  infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO );
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
}

//--------------------------------------------------------------
//  ROM���M���� �J��
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work )
{
  GFL_HEAP_FreeMemory( work->romTitle );
  GFL_HEAP_FreeMemory( work->romInfo );
}

//--------------------------------------------------------------
//  ROM���M���� �X�V
//--------------------------------------------------------------
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SEND_IMAGE_INIT_COMM:
    MB_COMM_InitComm( work->commWork );
    work->subState = MPSS_SEND_IMAGE_INIT_COMM_WAIT;
    break;
  case MPSS_SEND_IMAGE_INIT_COMM_WAIT:
    if( MB_COMM_IsInitComm( work->commWork ) == TRUE )
    {
      if( WH_StartMeasureChannel() == TRUE )
      {
        work->subState = MPSS_SEND_IMAGE_WAIT_SEARCH_CH;
      }
    }
    break;
  case MPSS_SEND_IMAGE_WAIT_SEARCH_CH:
    if( WH_GetSystemState() == WH_SYSSTATE_MEASURECHANNEL )
    {
      MP_PARENT_SendImage_MBPInit( work );

      MB_MSG_MessageCreateWordset( work->msgWork );
      MB_MSG_MessageWordsetName( work->msgWork , 0 , GAMEDATA_GetMyStatus(work->initWork->gameData) );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 );
      MB_MSG_MessageDeleteWordset( work->msgWork );

      work->subState = MPSS_SEND_IMAGE_MBSYS_MAIN;
    }
    break;
  case MPSS_SEND_IMAGE_MBSYS_MAIN:
    MP_PARENT_SendImage_MBPMain( work );
    break;

  case MPSS_SEND_IMAGE_WAIT_BOOT_INIT:
    MB_COMM_InitParent( work->commWork );
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_02 );
    work->subState = MPSS_SEND_IMAGE_WAIT_BOOT;
    break;
    
  case MPSS_SEND_IMAGE_WAIT_BOOT:
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
//  MBP�V�X�e���N��
//--------------------------------------------------------------
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work )
{
  /* ���̃f�����_�E�����[�h������v���O������� */
  //static����Ȃ��Ɠ����Ȃ��I�I�I
  static MBGameRegistry mbGameList = {
    "/dl_rom/child.srl",    // �q�@�o�C�i���R�[�h
    NULL ,                  // �Q�[����
    NULL ,                  // �Q�[�����e����
    "/dl_rom/icon.char",    // �A�C�R���L�����N�^�f�[�^
    "/dl_rom/icon.plt",     // �A�C�R���p���b�g�f�[�^
    MB_DEF_GGID,            // GGID
    2,                      // �ő�v���C�l���A�e�@�̐����܂߂��l��
  };

  const u16 channel = WH_GetMeasureChannel();
  mbGameList.gameNamep = work->romTitleStr;
  mbGameList.gameIntroductionp = work->romInfoStr;
  MBP_Init( MB_DEF_GGID , MB_TGID_AUTO );
  MBP_Start( &mbGameList , channel );
}

static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work )
{
  const u16 mbpState = MBP_GetState();

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
      if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
      {
        // B�{�^���Ń}���`�u�[�g�L�����Z��
        MBP_Cancel();
        break;
      }
      // �G���g���[���̎q�@�����ł����݂���ΊJ�n�\�Ƃ���
      if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
          MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
          MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
      {
        //�q�@��������Ƃ肠�����n�߂Ă��܂�
        {
          // �_�E�����[�h�J�n
          MBP_StartDownloadAll();
        }
      }
    }
    break;

    //-----------------------------------------
    // �v���O�����z�M����
  case MBP_STATE_DATASENDING:
    {
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
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x150000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_PARENT_WORK), HEAPID_MULTIBOOT );
  if( pwk == NULL )
  {
    initWork = GFL_HEAP_AllocMemory( HEAPID_MULTIBOOT , sizeof( MB_PARENT_INIT_WORK ));
    initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
  }
  else
  {
    initWork = pwk;
  }
  work->heapId = HEAPID_MULTIBOOT;
  work->initWork = initWork;
  
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
  return GFL_PROC_RES_CONTINUE;
}

