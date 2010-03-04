//============================================================================================
/**
 * @file  debug_watanabe.c
 * @brief �f�o�b�O���e�X�g�p�A�v���i�n�ӂ���p�j
 * @author  tamada
 * @date  2007.02.01
 */
//============================================================================================
#include "gflib.h"
#include "system/gfl_use.h"
#include "system\main.h"
#include "arc_def.h"
#include "message.naix"

#include "font/font.naix"
#include "print/printsys.h"
#include "print/str_tool.h"

#include "msg/msg_d_tetsu.h"
//============================================================================================
/**
 *
 *
 *
 *
 * @brief �v���Z�X��`
 *
 *
 *
 *
 */
//============================================================================================
static void allocWork( GFL_PROC* proc );
static void freeWork( GFL_PROC* proc );
static void setNextProc( void* procwork );
static BOOL mainFunc( void* procwork );
//------------------------------------------------------------------
/**
 * @brief �v���Z�X�̏�����
 *
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcInit
        ( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  allocWork(proc);

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief �v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcMain
        ( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  if( mainFunc(mywk) == FALSE ){
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �v���Z�X�̏I������
 *
 * �P�ɏI�������ꍇ�A�e�v���Z�X�ɏ������Ԃ�B
 * GFL_PROC_SysSetNextProc���Ă�ł����ƁA�I���セ�̃v���Z�X��
 * �������J�ڂ���B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcEnd
        ( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  setNextProc(mywk);
  freeWork(proc);

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugWatanabeMainProcData = {
  DebugWatanabeMainProcInit,
  DebugWatanabeMainProcMain,
  DebugWatanabeMainProcEnd,
};



//============================================================================================
/**
 *
 *
 *
 *
 * @brief �f�o�b�O���ڑI��
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   �a�f�ݒ��`
 */
//------------------------------------------------------------------
#define TEXT_FRAME    (GFL_BG_FRAME3_S)
#define TEXT_DISPSIDE (PALTYPE_SUB_BG)
#define TEXT_PLTTID   (15)
#define PLTT_SIZ    (16*2)

//------------------------------------------------------------------
/**
 * @brief   ���ڃ��X�g
 */
//------------------------------------------------------------------
#define LISTITEM_NAMESIZE (32)
typedef struct {
  u32                   msgID;
  FSOverlayID           procOvID;
  const GFL_PROC_DATA*  procData;
}DEBUGITEM_LIST;

FS_EXTERN_OVERLAY(watanabe_sample);
FS_EXTERN_OVERLAY(fieldmap);
extern const GFL_PROC_DATA DebugWatanabeSample5ProcData;

static const DEBUGITEM_LIST debugItemList[] = {
  {DEBUG_TETSU_MENU5, FS_OVERLAY_ID(watanabe_sample), &DebugWatanabeSample5ProcData},
};

//------------------------------------------------------------------
/**
 * @brief   �\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
  HEAPID  heapID;
  u32   seq;
  u32   selectItem;

  PRINT_UTIL      printUtil;
  PRINT_QUE*      printQue;
  GFL_FONT*       fontHandle;
  GFL_MSGDATA*    msgManager;
  GFL_BMPWIN*     bmpwin;

  GFL_UI_TP_HITTBL  tpTable[NELEMS(debugItemList)+1];

  u32         wait;
}DEBUG_WATANABE_WORK;

#define NON_SELECT_ITEM (0xffffffff)
//------------------------------------------------------------------
/**
 * @brief   �v���g�^�C�v�錾
 */
//------------------------------------------------------------------
static void systemSetup(DEBUG_WATANABE_WORK* dw);
static void systemFramework(DEBUG_WATANABE_WORK* dw);
static void systemDelete(DEBUG_WATANABE_WORK* dw);

static BOOL control(DEBUG_WATANABE_WORK* dw);
static void drawList(DEBUG_WATANABE_WORK* dw);
static void makeList(DEBUG_WATANABE_WORK* dw);

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
static void allocWork( GFL_PROC* proc )
{
  DEBUG_WATANABE_WORK* dw;
  HEAPID heapID = GFL_HEAPID_APP;

  dw = GFL_PROC_AllocWork(proc, sizeof(DEBUG_WATANABE_WORK), heapID);
  GFL_STD_MemClear(dw, sizeof(DEBUG_WATANABE_WORK));

  dw->heapID = heapID;
  dw->seq = 0;
  dw->selectItem = NON_SELECT_ITEM;
}

static void freeWork( GFL_PROC* proc )
{
  GFL_PROC_FreeWork(proc);
}

static void setNextProc( void* procwork )
{
  DEBUG_WATANABE_WORK* dw;
  u32 itemID;

  dw = procwork;
  itemID = dw->selectItem;

  GFL_PROC_SysSetNextProc(debugItemList[itemID].procOvID, debugItemList[itemID].procData, NULL);
}

//------------------------------------------------------------------
/**
 *
 * @brief ���C��
 *
 */
//------------------------------------------------------------------
static BOOL mainFunc( void* procwork )
{
  DEBUG_WATANABE_WORK* dw;
  dw = procwork;

  switch(dw->seq){
  case 0:
    systemSetup(dw);
    makeList(dw);

    dw->seq++;
    break;

  case 1:
    if( control(dw) == FALSE ){ dw->seq++; }

    drawList(dw);
    systemFramework(dw);
    break;

  case 2:
    systemDelete(dw);
    return FALSE;
  }
  return TRUE;
}

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief �V�X�e���Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_DISP_VRAM dispVram = {
  GX_VRAM_BG_128_A,       //���C��2D�G���W����BG�Ɋ��蓖��
  GX_VRAM_BGEXTPLTT_NONE,     //���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
  GX_VRAM_SUB_BG_32_H,      //�T�u2D�G���W����BG�Ɋ��蓖��
  GX_VRAM_SUB_BGEXTPLTT_NONE,   //�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
  GX_VRAM_OBJ_64_E,       //���C��2D�G���W����OBJ�Ɋ��蓖��
  GX_VRAM_OBJEXTPLTT_NONE,    //���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
  GX_VRAM_SUB_OBJ_NONE,     //�T�u2D�G���W����OBJ�Ɋ��蓖��
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  //�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
  GX_VRAM_TEX_NONE,       //�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
  GX_VRAM_TEXPLTT_NONE,     //�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
  GX_OBJVRAMMODE_CHAR_1D_128K,  // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
};

static const GFL_BG_SYS_HEADER bgsysHeader = {
  GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};

static const GFL_BG_BGCNT_HEADER Textcont = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

//------------------------------------------------------------------
/**
 * @brief   �a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void bg_init(HEAPID heapID)
{
  //VRAM�ݒ�
  GFL_DISP_SetBank(&dispVram);

  //�a�f�V�X�e���N��
  GFL_BG_Init(heapID);

  //�a�f���[�h�ݒ�
  GFL_BG_SetBGMode(&bgsysHeader);

  //�a�f�R���g���[���ݒ�
  GFL_BG_SetBGControl(TEXT_FRAME, &Textcont, GFL_BG_MODE_TEXT);
  GFL_BG_SetPriority(TEXT_FRAME, 0);
  GFL_BG_SetVisible(TEXT_FRAME, VISIBLE_ON);

  GFL_BG_FillCharacter(TEXT_FRAME, 0, 1, 0);  // �擪�ɃN���A�L�����z�u
  GFL_BG_ClearScreen(TEXT_FRAME);

  //�r�b�g�}�b�v�E�C���h�E�N��
  GFL_BMPWIN_Init(heapID);

  //�f�B�X�v���C�ʂ̑I��
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
  GFL_DISP_SetDispOn();
}

static void bg_exit(void)
{
  GFL_BMPWIN_Exit();
  GFL_BG_FreeBGControl(TEXT_FRAME);
  GFL_BG_Exit();
}

//------------------------------------------------------------------
/**
 * @brief   �Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void systemSetup(DEBUG_WATANABE_WORK* dw)
{
  //��{�Z�b�g�A�b�v
  bg_init(dw->heapID);
  //�t�H���g�p�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(  ARCID_FONT,
                  NARC_font_default_nclr,
                  TEXT_DISPSIDE,
                  TEXT_PLTTID * PLTT_SIZ,
                  PLTT_SIZ,
                  dw->heapID);
  //�t�H���g�n���h���쐬
  dw->fontHandle = GFL_FONT_Create( ARCID_FONT,
                    NARC_font_large_gftr,
                    GFL_FONT_LOADTYPE_FILE,
                    FALSE,
                    dw->heapID);
  //�v�����g�L���[�n���h���쐬
  dw->printQue = PRINTSYS_QUE_Create(dw->heapID);

  dw->msgManager = GFL_MSG_Create
    (GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tetsu_dat, dw->heapID);

  //�`��p�r�b�g�}�b�v�쐬�i��ʑS�́j
  dw->bmpwin = GFL_BMPWIN_Create( TEXT_FRAME,
                  0, 0, 32, 24,
                  TEXT_PLTTID, GFL_BG_CHRAREA_GET_F );
  PRINT_UTIL_Setup(&dw->printUtil, dw->bmpwin);

  GFL_BMPWIN_MakeScreen(dw->bmpwin);
  GFL_BG_LoadScreenReq(TEXT_FRAME);
}

//------------------------------------------------------------------
/**
 * @brief   �t���[�����[�N
 */
//------------------------------------------------------------------
static void systemFramework(DEBUG_WATANABE_WORK* dw)
{
  PRINT_UTIL_Trans(&dw->printUtil, dw->printQue);
  PRINTSYS_QUE_Main(dw->printQue);
}

//------------------------------------------------------------------
/**
 * @brief   �j��
 */
//------------------------------------------------------------------
static void systemDelete(DEBUG_WATANABE_WORK* dw)
{
  PRINTSYS_QUE_Clear(dw->printQue);
  GFL_BMPWIN_Delete(dw->bmpwin);

  GFL_MSG_Delete(dw->msgManager);

  PRINTSYS_QUE_Delete(dw->printQue);
  GFL_FONT_Delete(dw->fontHandle);

  bg_exit();
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief ���X�g�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
#define ITEM_PX (8*2)
#define ITEM_PY (8*2)
#define ITEM_SX (256-ITEM_PX*2)
#define ITEM_SY (16)

//------------------------------------------------------------------
/**
 * @brief �`��
 */
//------------------------------------------------------------------
static void drawList(DEBUG_WATANABE_WORK* dw)
{
  STRBUF*     strBuf;
  PRINTSYS_LSB  lsb;
  int i;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(dw->bmpwin), 15);

  strBuf = GFL_STR_CreateBuffer(LISTITEM_NAMESIZE+1, dw->heapID);

  for( i=0; i<NELEMS(debugItemList); i++ ){
    GFL_MSG_GetString(dw->msgManager, debugItemList[i].msgID, strBuf);

    if( i == dw->selectItem ){ lsb = PRINTSYS_LSB_Make(3,2,15); }
    else { lsb = PRINTSYS_LSB_Make(1,2,15); }

    PRINT_UTIL_PrintColor(  &dw->printUtil, dw->printQue,
                ITEM_PX, i * ITEM_SY + ITEM_PY,
                strBuf, dw->fontHandle, lsb);
  }
  GFL_STR_DeleteBuffer(strBuf);
}

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l������e�[�u���쐬
 */
//------------------------------------------------------------------
static void makeList(DEBUG_WATANABE_WORK* dw)
{
  int i;

  for( i=0; i<NELEMS(debugItemList); i++ ){
    //�^�b�`�p�l������e�[�u���쐬
    dw->tpTable[i].rect.top = i * ITEM_SY + ITEM_PY;
    dw->tpTable[i].rect.bottom = dw->tpTable[i].rect.top + (ITEM_SY-1);
    dw->tpTable[i].rect.left = ITEM_PX;
    dw->tpTable[i].rect.right = dw->tpTable[i].rect.left + (ITEM_SX-1);
  }
  dw->tpTable[NELEMS(debugItemList)].rect.top = GFL_UI_TP_HIT_END;//�I���f�[�^���ߍ���
  dw->tpTable[NELEMS(debugItemList)].rect.bottom = 0;
  dw->tpTable[NELEMS(debugItemList)].rect.left = 0;
  dw->tpTable[NELEMS(debugItemList)].rect.right = 0;
}



//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief ����֐�
 *
 *
 *
 *
 *
 */
//============================================================================================
static  BOOL control(DEBUG_WATANABE_WORK* dw)
{
  if( dw->selectItem == NON_SELECT_ITEM ){
    //���I��
    u32 tblPos = GFL_UI_TP_HitTrg(dw->tpTable);
    if( tblPos != GFL_UI_TP_HIT_NONE ){
			if( debugItemList[tblPos].procData != NULL ){
				dw->selectItem = tblPos;
				dw->wait = 0;
			}
    }
    return TRUE;
  }
  if( dw->wait < 16 ){
    //�I����E�F�C�g����
    dw->wait++;
    return TRUE;
  }
  return FALSE;
}



