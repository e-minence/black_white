//============================================================================================
/**
* @file   d_font_view.c
* @brief  �t�H���g�r���[�A
* @author ichibayashi
* @date   2010.05.20
*/
//============================================================================================

#ifdef PM_DEBUG

//���C�u����
#include <gflib.h>
#include <msgdata.h>
#include <wchar.h>

//�V�X�e��
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//Proc
#include "title/title.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_d_ncl.h"

// �����Z�b�g������킷�z�� UserInputCode�ADefinedCode
#include "d_font_view.cdat"

//======================================================================
//  define
//======================================================================

#define VIEWHEAD_BUFSIZE (128)
#define VIEWBODY_LINESIZE (16)
#define VIEWBODY_LINECOUNT (8)
#define VIEWBODY_INDEXCOUNT (VIEWBODY_LINESIZE*VIEWBODY_LINECOUNT)  // 1��ʂɕ\�����镶����
#define VIEWBODY_BUFSIZE ((VIEWBODY_LINESIZE+2)*VIEWBODY_LINECOUNT+1)  // (1�s16����+�E�[�𕪂���₷�����邽�߂̕���+���s)*8�s+EOM

#define WORDSET_BUFNUM (5)
#define WORDSET_BUFLEN (64)
#define WORDINDEX_CODEBEGIN (0)
#define WORDINDEX_CODEEND (1)
#define WORDINDEX_FONTNAME (2)
#define WORDINDEX_INDEXMODE (3)
#define WORDINDEX_LOADTYPE (4)

// �㉺�{�^���ŕ����R�[�h���ω������
#define CODEMOVE_COUNT (VIEWBODY_LINESIZE*VIEWBODY_LINECOUNT)

// �\�����Ă���t�H���g�E�͈͂̏��̈ʒu
#define VIEWHEAD_PX (1)
#define VIEWHEAD_PY (0)

// �t�H���g��\������ʒu
#define VIEWBODY_PX (1)
#define VIEWBODY_PY (24)

// �w���v��\������ʒu
#define HELP_PX (8)
#define HELP_PY (0)

#define BG_PLANE_VIEW GFL_BG_FRAME1_M
#define BG_PLANE_HELP GFL_BG_FRAME1_S
#define FONTVIEWER_PLT_FONT (0)
#define FONTVIEWER_CLEAR_COLOR (15)

#define FONTID_MAX (4)

#define CR_CODE (0xfffe)


// �����C���f�b�N�X���\�����������肷�郂�[�h
typedef enum
{
  INDEXMODE_DEFINED = 0,  // syachi�Œ�`����Ă��镶��
  INDEXMODE_USERINPUT,    // ���[�U�[���͉\�ȕ���
  INDEXMODE_ALL,         // �SUnicode (U+0000�`U+FFFF)
  INDEXMODE_MAX = INDEXMODE_ALL,
}FONTVIEWER_INDEXMODE;

//======================================================================
//  typedef struct
//======================================================================
typedef struct
{
  HEAPID heapId;

  GFL_FONT      *fontDefault;
  GFL_FONT      *fontView;
  GFL_BMPWIN    *bmpWinView;  //���ʃt�H���g�\��
  GFL_BMPWIN    *bmpWinHelp;  //����ʃw���v
  GFL_MSGDATA   *msgman;
  WORDSET       *wordset;

  FONTVIEWER_INDEXMODE indexMode;
  int           indexBegin;    // ���ݕ\�����Ă��镶���̎n�_
  int           indexEnd;      // ���ݕ\�����Ă��镶���̏I�_
  u32           fontId;       // ���ݕ\�����Ă���t�H���gId
  int           fontLoadType; // �t�H���g���풓���ǂ���
  STRBUF        *viewHeadStr; // �\�����Ă���t�H���g�E�͈͂̃^�O����O������
  STRBUF        *viewHeadBuf; // �\�����Ă���t�H���g�E�͈͂̏��
  STRBUF        *viewBodyBuf; // �\�����Ă��镶����
} FONTVIEWER_WORK;

//======================================================================
//  proto
//======================================================================
static void InitGraphic( FONTVIEWER_WORK *work );
static void InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );
static GFL_BMPWIN *CreateFullScreenBmpWin( FONTVIEWER_WORK *work, u8 bgPlane );
static void PrintHelp(FONTVIEWER_WORK *work, GFL_BMPWIN *dst);
static void PrintViewHead(FONTVIEWER_WORK *work, GFL_BMPWIN *dst);
static void PrintViewBody(FONTVIEWER_WORK *work, GFL_BMPWIN *dst);
static void PrintView(FONTVIEWER_WORK *work, GFL_BMPWIN *dst);

static STRCODE IndexToCode(FONTVIEWER_WORK *work, int index);
static int GetMaxIndex(FONTVIEWER_WORK *work);
static void SetIndex(FONTVIEWER_WORK *work, int new_begin);
static void MoveIndex(FONTVIEWER_WORK *work, int diff);

static void RegisterMessage(WORDSET* wordset, u32 bufID, GFL_MSGDATA *msgman, int msgid);

static GFL_PROC_RESULT FontViewerProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT FontViewerProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT FontViewerProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);

//--------------------------------------------------------------------------
//  �`����菉����
//--------------------------------------------------------------------------
static void FONTVIEWER_InitGraphic( FONTVIEWER_WORK *work )
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

  static const GFL_BG_BGCNT_HEADER bgContOnePlane = {
    0, 0, 0x800, 0,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };

  GX_SetMasterBrightness(0);
  GXS_SetMasterBrightness(0);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  GFL_DISP_SetBank( &vramBank );
  GFL_BG_SetBGMode( &sysHeader );
  GFL_BG_Init( work->heapId );

  InitBgFunc( &bgContOnePlane , BG_PLANE_VIEW );
  InitBgFunc( &bgContOnePlane , BG_PLANE_HELP );

  GFL_BMPWIN_Init( work->heapId );
}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  �S���BMPWIN�쐬
//--------------------------------------------------------------------------
static GFL_BMPWIN *CreateFullScreenBmpWin( FONTVIEWER_WORK *work, u8 bgPlane )
{
  GFL_BMPWIN *bmp_win;

  bmp_win = GFL_BMPWIN_Create(bgPlane, 0, 0, 32, 24, FONTVIEWER_PLT_FONT, GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen(bmp_win);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmp_win), FONTVIEWER_CLEAR_COLOR );
  GFL_BMPWIN_TransVramCharacter(bmp_win);
  GFL_BG_LoadScreenReq( bgPlane );

  return bmp_win;
}

//--------------------------------------------------------------------------
//  �w���v��ʍ쐬�\��
//--------------------------------------------------------------------------
static void PrintHelp(FONTVIEWER_WORK *work, GFL_BMPWIN *dst)
{
  STRBUF *strbuf = GFL_MSG_CreateString(work->msgman, DEBUG_FONTVIEW_HELP);
  PRINTSYS_Print(GFL_BMPWIN_GetBmp(dst), HELP_PX, HELP_PY, strbuf, work->fontDefault);
  GFL_STR_DeleteBuffer(strbuf);
}

//--------------------------------------------------------------------------
//  �t�H���g�\����ʕ\��
//--------------------------------------------------------------------------
static void PrintViewHead(FONTVIEWER_WORK *work, GFL_BMPWIN *dst)
{
  GFL_BMP_DATA *const bmp = GFL_BMPWIN_GetBmp(dst);

  WORDSET_RegisterHexNumber(work->wordset, WORDINDEX_CODEBEGIN, IndexToCode(work, work->indexBegin), 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU);
  WORDSET_RegisterHexNumber(work->wordset, WORDINDEX_CODEEND, IndexToCode(work, work->indexEnd), 4, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU);
  RegisterMessage(work->wordset, WORDINDEX_FONTNAME, work->msgman, DEBUG_FONTVIEW_FONTNAME01+work->fontId);
  RegisterMessage(work->wordset, WORDINDEX_INDEXMODE, work->msgman, DEBUG_FONTVIEW_INDEXMODE01+work->indexMode);
  RegisterMessage(work->wordset, WORDINDEX_LOADTYPE, work->msgman, DEBUG_FONTVIEW_LOADTYPE01+work->fontLoadType);

  WORDSET_ExpandStr(work->wordset, work->viewHeadBuf, work->viewHeadStr);
  PRINTSYS_Print(bmp, VIEWHEAD_PX, VIEWHEAD_PY, work->viewHeadBuf, work->fontDefault);
}

static void PrintViewBody(FONTVIEWER_WORK *work, GFL_BMPWIN *dst)
{
  GFL_BMP_DATA *const bmp = GFL_BMPWIN_GetBmp(dst);
  STRBUF *const buf = work->viewBodyBuf;
  int index;
  int code_count = 0;

  GFL_STR_ClearBuffer(buf);
  for (index = work->indexBegin; index <= work->indexEnd; index ++)
  {
    GFL_STR_AddCode(buf, IndexToCode(work, index));
    code_count ++;
    if (code_count % 16 == 0)
    {
      GFL_STR_AddCode(buf, IndexToCode(work, index));
      GFL_STR_AddCode(buf, CR_CODE);
    }
  }

  PRINTSYS_Print(bmp, VIEWBODY_PX, VIEWBODY_PY, buf, work->fontView);
}

static void PrintView(FONTVIEWER_WORK *work, GFL_BMPWIN *dst)
{
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(dst), FONTVIEWER_CLEAR_COLOR);
  PrintViewHead(work, dst);
  PrintViewBody(work, dst);
}

//--------------------------------------------------------------------------
//  ���݂̃��[�h�̕����C���f�b�N�X��Unicode�ɕϊ�
//--------------------------------------------------------------------------
static STRCODE IndexToCode(FONTVIEWER_WORK *work, int index)
{
  switch (work->indexMode)
  {
  case INDEXMODE_DEFINED:
    GF_ASSERT(index < NELEMS(DefinedCode));
    return DefinedCode[index];
  case INDEXMODE_USERINPUT:
    GF_ASSERT(index < NELEMS(UserInputCode));
    return UserInputCode[index];
  case INDEXMODE_ALL:
    GF_ASSERT(index < 0x10000);
    return (STRCODE)index;
  default:
    GF_ASSERT(0);
  }
  return 0;
}

//--------------------------------------------------------------------------
//  ���݂̃��[�h�̕����C���f�b�N�X�ő�𓾂�
//--------------------------------------------------------------------------
static int GetMaxIndex(FONTVIEWER_WORK *work)
{
  switch (work->indexMode)
  {
  case INDEXMODE_DEFINED:
    return NELEMS(DefinedCode)-1;
  case INDEXMODE_USERINPUT:
    return NELEMS(UserInputCode)-1;
  case INDEXMODE_ALL:
    return 0xffff;
  default:
    GF_ASSERT(0);
  }
  return 0;
}

//--------------------------------------------------------------------------
//  �����C���f�b�N�X���[�h�̕ύX
//--------------------------------------------------------------------------
static void SetIndexMode(FONTVIEWER_WORK *work, FONTVIEWER_INDEXMODE new_mode)
{
  GF_ASSERT(0 <= new_mode && new_mode <= INDEXMODE_MAX);

  work->indexMode = new_mode;
}

//--------------------------------------------------------------------------
//  �\�����镶���C���f�b�N�X��new_begin����ɂ���
//--------------------------------------------------------------------------
static void SetIndex(FONTVIEWER_WORK *work, int new_begin)
{
  const int max_index = GetMaxIndex(work);
  int new_end;

  // �I���C���f�b�N�X�̌v�Z
  new_end = new_begin + VIEWBODY_INDEXCOUNT - 1;
  if (new_end > max_index)
  {
    new_end = max_index;
  }

  work->indexBegin = new_begin;
  work->indexEnd = new_end;
}

//--------------------------------------------------------------------------
//  �\�����镶���C���f�b�N�X��diff�������炷
//--------------------------------------------------------------------------
static void MoveIndex(FONTVIEWER_WORK *work, int diff)
{
  const int max_index = GetMaxIndex(work);
  int new_begin = work->indexBegin + diff;

  if (new_begin < 0)
  {
    // �Ō�̃y�[�W
    new_begin = (max_index / VIEWBODY_INDEXCOUNT) * VIEWBODY_INDEXCOUNT;
  }
  else if (new_begin > GetMaxIndex(work))
  {
    // �ŏ��̃y�[�W
    new_begin = 0;
  }
  GF_ASSERT((0 <= new_begin) && (new_begin < max_index));

  SetIndex(work, new_begin);
}

//--------------------------------------------------------------------------
//  �w�肵���t�H���g��ǂݍ���
//--------------------------------------------------------------------------
static void LoadViewFont(FONTVIEWER_WORK *work, u32 font_id, int load_type)
{
  if (work->fontView != NULL)
  {
    GFL_FONT_Delete( work->fontView );
  }

  work->fontId = font_id;
  work->fontLoadType = load_type;
  work->fontView = GFL_FONT_Create( ARCID_FONT , font_id, load_type, FALSE , work->heapId );
}

//--------------------------------------------------------------------------
//  WORDSET�Ɏw�胁�b�Z�[�WID��o�^
//--------------------------------------------------------------------------
static void RegisterMessage(WORDSET* wordset, u32 bufID, GFL_MSGDATA *msgman, int msgid)
{
  STRBUF *const strbuf = GFL_MSG_CreateString(msgman, msgid);
  WORDSET_RegisterWord(wordset, bufID, strbuf, FALSE, TRUE, PM_LANG);
  GFL_STR_DeleteBuffer(strbuf);
}

//============================================================================================
//
//
//  �v���Z�X�R���g���[��
//
//
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA FontViewerProcData = {
  FontViewerProcInit,
  FontViewerProcMain,
  FontViewerProcEnd,
};

//------------------------------------------------------------------
/**
 * @brief   ������
 *
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FontViewerProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  FONTVIEWER_WORK *work;
  HEAPID  heapId = HEAPID_NCL_DEBUG;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapId, 0x80000 );

  work = GFL_PROC_AllocWork( proc, sizeof(FONTVIEWER_WORK), heapId );
  GFL_STD_MemClear(work, sizeof(FONTVIEWER_WORK));
  work->heapId = heapId;

  FONTVIEWER_InitGraphic( work );
  GFL_FONTSYS_Init();

  //�t�H���g�p�p���b�g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , FONTVIEWER_PLT_FONT * 32, 16*2, work->heapId );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , FONTVIEWER_PLT_FONT * 32, 16*2, work->heapId );

  // �t�H���g
  work->fontDefault = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  work->fontView = NULL;

  // BmpWin
  work->bmpWinView = CreateFullScreenBmpWin(work, BG_PLANE_VIEW);
  work->bmpWinHelp = CreateFullScreenBmpWin(work, BG_PLANE_HELP);

  // ������
  work->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_ncl_dat, work->heapId );
  work->wordset = WORDSET_CreateEx( WORDSET_BUFNUM, WORDSET_BUFLEN, work->heapId );

  work->viewHeadStr = GFL_MSG_CreateString(work->msgman, DEBUG_FONTVIEW_HEAD);
  work->viewHeadBuf = GFL_STR_CreateBuffer(VIEWHEAD_BUFSIZE, work->heapId);
  work->viewBodyBuf = GFL_STR_CreateBuffer(VIEWBODY_BUFSIZE, work->heapId);

  LoadViewFont(work, 0, GFL_FONT_LOADTYPE_FILE);
  SetIndexMode(work, INDEXMODE_DEFINED);
  SetIndex(work, 0);

  PrintHelp(work, work->bmpWinHelp);
  GFL_BMPWIN_TransVramCharacter(work->bmpWinHelp);

  PrintView(work, work->bmpWinView);
  GFL_BMPWIN_TransVramCharacter(work->bmpWinView);

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief   ���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FontViewerProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  FONTVIEWER_WORK *work = mywk;
  BOOL need_repaint = FALSE;

  // �\�������A�߂�
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  {
    MoveIndex(work, -CODEMOVE_COUNT);
    need_repaint = TRUE;
  }
  // �\�������A�i��
  else if( (GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN) || (GFL_UI_KEY_GetCont() & PAD_BUTTON_A) )
  {
    MoveIndex(work, +CODEMOVE_COUNT);
    need_repaint = TRUE;
  }
  // �t�H���g�؂�ւ�
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
  {
    LoadViewFont(work, (work->fontId + (FONTID_MAX+1) - 1) % (FONTID_MAX+1), work->fontLoadType);
    need_repaint = TRUE;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
  {
    LoadViewFont(work, (work->fontId + 1) % (FONTID_MAX+1), work->fontLoadType);
    need_repaint = TRUE;
  }
  // �����Z�b�g�؂�ւ�
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
  {
    work->indexMode = (work->indexMode+1) % (INDEXMODE_MAX+1);
    SetIndex(work, 0);
    need_repaint = TRUE;
  }
  // �풓�؂�ւ�
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    switch (work->fontLoadType)
    {
    case GFL_FONT_LOADTYPE_FILE:
      LoadViewFont(work, work->fontId, GFL_FONT_LOADTYPE_MEMORY);
      break;
    case GFL_FONT_LOADTYPE_MEMORY:
      LoadViewFont(work, work->fontId, GFL_FONT_LOADTYPE_FILE);
      break;
    default:
      GF_ASSERT(0);
    }
    need_repaint = TRUE;
  }
  // �I��
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    return GFL_PROC_RES_FINISH;
  }

  if (need_repaint)
  {
    PrintView(work, work->bmpWinView);
    GFL_BMPWIN_TransVramCharacter(work->bmpWinView);
  }

  return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   �I��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FontViewerProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  FONTVIEWER_WORK *work = mywk;

  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);

  //�J������
  GFL_FONT_Delete( work->fontDefault );
  GFL_FONT_Delete( work->fontView );

  GFL_BMPWIN_Delete( work->bmpWinView );
  GFL_BMPWIN_Delete( work->bmpWinHelp );

  GFL_MSG_Delete( work->msgman );
  WORDSET_Delete( work->wordset );

  GFL_STR_DeleteBuffer( work->viewHeadStr );
  GFL_STR_DeleteBuffer( work->viewHeadBuf );
  GFL_STR_DeleteBuffer( work->viewBodyBuf );

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap( HEAPID_NCL_DEBUG );

  return GFL_PROC_RES_FINISH;
}

#endif // #ifdef PM_DEBUG

