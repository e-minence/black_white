//======================================================================
/**
 * @file  select_moji_mode.c
 * @brief �Q�[���J�n���̊����E�Ђ炪�Ȃ̑I��
 * @author  ariizumi
 * @data  09/05/11
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/bmp_winframe.h"

#include "arc_def.h"
#include "print/printsys.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_select_moji_mode.h"

#include "select_moji_mode.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

//���ڐ�
#define SEL_MOJIMODE_ITEM_NUM (2)

//BGPlane
#define SEL_MOJIMODE_BG_BACK (GFL_BG_FRAME3_M)
#define SEL_MOJIMODE_BG_ITEM (GFL_BG_FRAME1_M)
#define SEL_MOJIMODE_BG_STR  (GFL_BG_FRAME1_S)

//BG�L�����A�h���X
#define SEL_MOJIMODE_FRAMECHR1  (1)
#define SEL_MOJIMODE_FRAMECHR2  (SEL_MOJIMODE_FRAMECHR1 + TALK_WIN_CGX_SIZ)
#define SEL_MOJIMODE_FRAMECHR_TALK  (1)

//�p���b�g
#define SEL_MOJIMODE_PLT_FONT   (0)
#define SEL_MOJIMODE_PLT_SEL    (3)
#define SEL_MOJIMODE_PLT_NOSEL  (4)
#define SEL_MOJIMODE_PLT_S_FONT    (0)
#define SEL_MOJIMODE_PLT_S_TALKWIN (1)

//�t�H���g�I�t�Z�b�g
#define SEL_MOJIMODE_FONT_TOP  (2)
#define SEL_MOJIMODE_FONT_LEFT (2)

//�I�����ʒu�E�T�C�Y
#define SEL_MOJIMODE_ITEM_TOP     (12)
#define SEL_MOJIMODE_ITEM_LEFT    (6)
#define SEL_MOJIMODE_ITEM_HEIGHT  (2)
#define SEL_MOJIMODE_ITEM_WIDTH   (20)

//��b�E�B���h�E
#define SEL_MOJIMODE_TALK_TOP     (19)
#define SEL_MOJIMODE_TALK_LEFT    (1)
#define SEL_MOJIMODE_TALK_HEIGHT  (4)
#define SEL_MOJIMODE_TALK_WIDTH   (29)

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  u8 selectItem;
  
  GFL_FONT   *fontHandle;
  GFL_BMPWIN *strWin;
  GFL_BMPWIN *itemWin[SEL_MOJIMODE_ITEM_NUM];
}SEL_MOJIMODE_WORK;


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

//Proc�f�[�^
static GFL_PROC_RESULT SEL_MOJIMODE_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT SEL_MOJIMODE_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT SEL_MOJIMODE_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void SEL_MOJIMODE_InitGraphic( SEL_MOJIMODE_WORK *work );
static void SEL_OBJMODE_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );
static void SEL_MOJIMODE_InitItem( SEL_MOJIMODE_WORK *work );

static void SEL_MOJIMODE_DrawWinFrame( SEL_MOJIMODE_WORK *work );

const GFL_PROC_DATA SelectMojiModeProcData = {
  SEL_MOJIMODE_ProcInit,
  SEL_MOJIMODE_ProcMain,
  SEL_MOJIMODE_ProcEnd,
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------

static GFL_PROC_RESULT SEL_MOJIMODE_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SEL_MOJIMODE_WORK *work;
    //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SEL_MOJIMODE, 0x80000 );
  work = GFL_PROC_AllocWork( proc, sizeof(SEL_MOJIMODE_WORK), HEAPID_SEL_MOJIMODE );
  
  work->selectItem = 0;

  SEL_MOJIMODE_InitGraphic( work );
  SEL_MOJIMODE_InitItem( work );
  
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT SEL_MOJIMODE_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  u8 i;
  SEL_MOJIMODE_WORK *work = (SEL_MOJIMODE_WORK*)mywk;
  
  GFL_BMPWIN_Delete( work->strWin );
  for( i=0 ; i<SEL_MOJIMODE_ITEM_NUM ; i++ )
  {
    GFL_BMPWIN_Delete( work->itemWin[i] );
  }

  GFL_FONT_Delete( work->fontHandle );
  GFL_BMPWIN_Exit();
  
  GFL_BG_FreeBGControl(SEL_MOJIMODE_BG_BACK);
  GFL_BG_FreeBGControl(SEL_MOJIMODE_BG_ITEM);
  GFL_BG_FreeBGControl(SEL_MOJIMODE_BG_STR);
  GFL_BG_Exit();

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_SEL_MOJIMODE );
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT SEL_MOJIMODE_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SEL_MOJIMODE_WORK *work = (SEL_MOJIMODE_WORK*)mywk;
  //�L�[�X�V
  if( (GFL_UI_KEY_GetTrg() & PAD_KEY_UP) ||
      (GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN) )
  {
    if( work->selectItem == 0 )
    {
      work->selectItem = 1;
    }
    else
    {
      work->selectItem = 0;
    }
    SEL_MOJIMODE_DrawWinFrame( work );
  }
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    GFL_MSGSYS_SetLangID( work->selectItem );
    return GFL_PROC_RES_FINISH;
  }
  
  //TP�X�V
  {
    const GFL_UI_TP_HITTBL hitTbl[SEL_MOJIMODE_ITEM_NUM+1] =
    {
      {
        SEL_MOJIMODE_ITEM_TOP*8 ,
        (SEL_MOJIMODE_ITEM_TOP+SEL_MOJIMODE_ITEM_HEIGHT)*8 ,
        SEL_MOJIMODE_ITEM_LEFT*8 ,
        (SEL_MOJIMODE_ITEM_LEFT+SEL_MOJIMODE_ITEM_WIDTH)*8 ,
      },
      {
        (SEL_MOJIMODE_ITEM_TOP+SEL_MOJIMODE_ITEM_HEIGHT+2)*8 ,
        (SEL_MOJIMODE_ITEM_TOP+SEL_MOJIMODE_ITEM_HEIGHT*2+2)*8 ,
        SEL_MOJIMODE_ITEM_LEFT*8 ,
        (SEL_MOJIMODE_ITEM_LEFT+SEL_MOJIMODE_ITEM_WIDTH)*8 ,
      },
      { GFL_UI_TP_HIT_END,0,0,0 }
    };
    
    const int ret = GFL_UI_TP_HitTrg( hitTbl );
    if( ret != GFL_UI_TP_HIT_NONE )
    {
      GFL_MSGSYS_SetLangID( ret );
      return GFL_PROC_RES_FINISH;
    }
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
//  �`��n������
//--------------------------------------------------------------------------
static void SEL_MOJIMODE_InitGraphic( SEL_MOJIMODE_WORK *work )
{
  static const GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_128_D,      // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_32K,   // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
  };  

  static const GFL_BG_SYS_HEADER sysHeader = {
    GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
  };
  
  static const GFL_BG_BGCNT_HEADER bgCont_Item = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };

  static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
  };

  static const GFL_BG_BGCNT_HEADER bgCont_Str = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };


  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  GFL_DISP_SetBank( &vramBank );
  GFL_BG_Init( HEAPID_SEL_MOJIMODE );
  GFL_BG_SetBGMode( &sysHeader ); 
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

  SEL_OBJMODE_InitBgFunc( &bgCont_BackGround , SEL_MOJIMODE_BG_BACK );
  SEL_OBJMODE_InitBgFunc( &bgCont_Item , SEL_MOJIMODE_BG_ITEM );
  SEL_OBJMODE_InitBgFunc( &bgCont_Str , GFL_BG_FRAME1_S );

  GFL_BMPWIN_Init( HEAPID_SEL_MOJIMODE );
  
  //�t�H���g�p�p���b�g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , SEL_MOJIMODE_PLT_FONT * 32, 16*2, HEAPID_SEL_MOJIMODE );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , SEL_MOJIMODE_PLT_S_FONT * 32, 16*2, HEAPID_SEL_MOJIMODE );
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_SEL_MOJIMODE );

  //WinFrame�p�O���t�B�b�N�ݒ�
  BmpWinFrame_GraphicSet( SEL_MOJIMODE_BG_ITEM , SEL_MOJIMODE_FRAMECHR1 , SEL_MOJIMODE_PLT_SEL  , 1, HEAPID_SEL_MOJIMODE);
  BmpWinFrame_GraphicSet( SEL_MOJIMODE_BG_ITEM , SEL_MOJIMODE_FRAMECHR2 , SEL_MOJIMODE_PLT_NOSEL , 0, HEAPID_SEL_MOJIMODE);
  TalkWinFrame_GraphicSet( SEL_MOJIMODE_BG_STR , SEL_MOJIMODE_FRAMECHR_TALK , SEL_MOJIMODE_PLT_S_TALKWIN , 0 , HEAPID_SEL_MOJIMODE);

  //�w�i�F
  *((u16 *)HW_BG_PLTT) = 0x7d8c;//RGB(12, 12, 31);
  *((u16 *)(HW_BG_PLTT+0x400)) = 0x7d8c;//RGB(12, 12, 31);

  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT , 16 , 0 , ARI_FADE_SPD );
  
}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void SEL_OBJMODE_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  �I���������
//--------------------------------------------------------------------------
static void SEL_MOJIMODE_InitItem( SEL_MOJIMODE_WORK *work )
{
  u8 i;
  
  GFL_MSGDATA *msghandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE ,
                                NARC_message_select_moji_mode_dat , HEAPID_SEL_MOJIMODE );

  //�^�C�g���̕\��
  {
    STRBUF *str;
    work->strWin = GFL_BMPWIN_Create( SEL_MOJIMODE_BG_STR , 
                          SEL_MOJIMODE_TALK_LEFT , SEL_MOJIMODE_TALK_TOP , 
                          SEL_MOJIMODE_TALK_WIDTH, SEL_MOJIMODE_TALK_HEIGHT , 
                          SEL_MOJIMODE_PLT_S_FONT , GFL_BMP_CHRAREA_GET_B );
    GFL_BMPWIN_MakeScreen( work->strWin );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->strWin ), 0xf );

    str = GFL_MSG_CreateString( msghandle , QUESTION_STR );

    TalkWinFrame_Write( work->strWin , WINDOW_TRANS_ON ,
                  SEL_MOJIMODE_FRAMECHR_TALK ,SEL_MOJIMODE_PLT_S_TALKWIN );

    PRINTSYS_Print( GFL_BMPWIN_GetBmp(work->strWin),
            SEL_MOJIMODE_FONT_TOP,
            SEL_MOJIMODE_FONT_LEFT,
            str,
            work->fontHandle);

    GFL_STR_DeleteBuffer( str );

    GFL_BMPWIN_TransVramCharacter( work->strWin );
  }

  for( i=0 ; i<SEL_MOJIMODE_ITEM_NUM ; i++ )
  {
    STRBUF *str;
    work->itemWin[i] = GFL_BMPWIN_Create( SEL_MOJIMODE_BG_ITEM , 
                          SEL_MOJIMODE_ITEM_LEFT , SEL_MOJIMODE_ITEM_TOP+i*(SEL_MOJIMODE_ITEM_HEIGHT+2) , 
                          SEL_MOJIMODE_ITEM_WIDTH, SEL_MOJIMODE_ITEM_HEIGHT , 
                          SEL_MOJIMODE_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    GFL_BMPWIN_MakeScreen( work->itemWin[i] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->itemWin[i] ), 0xf );
    
    str = GFL_MSG_CreateString( msghandle , ITEM1_STR + i );

    PRINTSYS_Print( GFL_BMPWIN_GetBmp(work->itemWin[i]),
            SEL_MOJIMODE_FONT_TOP,
            SEL_MOJIMODE_FONT_LEFT,
            str,
            work->fontHandle);
    
    GFL_STR_DeleteBuffer( str );
    
    GFL_BMPWIN_TransVramCharacter( work->itemWin[i] );
  }
  
  GFL_MSG_Delete( msghandle );
  
  SEL_MOJIMODE_DrawWinFrame( work );
}

//--------------------------------------------------------------------------
//  �I�����̘g��`��
//--------------------------------------------------------------------------
static void SEL_MOJIMODE_DrawWinFrame( SEL_MOJIMODE_WORK *work )
{
  u8 i;

  for( i=0 ; i<SEL_MOJIMODE_ITEM_NUM ; i++ )
  {
    if( i == work->selectItem )
    {
      BmpWinFrame_Write(work->itemWin[i], WINDOW_TRANS_ON, 
              SEL_MOJIMODE_FRAMECHR1, SEL_MOJIMODE_PLT_SEL);
    }
    else
    {
      BmpWinFrame_Write(work->itemWin[i], WINDOW_TRANS_ON, 
              SEL_MOJIMODE_FRAMECHR2, SEL_MOJIMODE_PLT_NOSEL);
    }
  }
}
