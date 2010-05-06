//======================================================================
/**
 * @file	local_tvt_local_def.h
 * @brief	��ʐM�e���r�g�����V�[�o�[ ��`
 * @author	ariizumi
 * @data	09/11/02
 *
 * ���W���[�����FLOCAL_TVT
 */
//======================================================================
#pragma once

#include "print/printsys.h"
#include "print/wordset.h"
#include "app/app_keycursor.h"
#include "app/app_printsys_common.h"

#include "app/local_tvt_sys.h"

#include "local_tvt_type_def.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define LTVT_FRAME_MESSAGE  (GFL_BG_FRAME0_M)
#define LTVT_FRAME_NAME     (GFL_BG_FRAME0_M)  //�w�i�m�ۂ̂��ߓ���
#define LTVT_FRAME_NOIZE_BG (GFL_BG_FRAME1_M)
#define LTVT_FRAME_CHARA    (GFL_BG_FRAME2_M)
#define LTVT_FRAME_BG       (GFL_BG_FRAME3_M)

#define LTVT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define LTVT_CHARA_SCREEN_WIDTH (16)
#define LTVT_CHARA_SCREEN_HEIGHT (24)

#define LTVT_PLT_MAIN_NOIZE    (11)
#define LTVT_PLT_MAIN_FONT     (12)
#define LTVT_PLT_MAIN_WINFRAME (13)

#define LTVT_PLT_OBJ_MAIN     (0)
#define LTVT_PLT_OBJ_CHARA     (4)  //1*4�{

//�g��BG�͖ʂ��ƂɎ��̂ŗ���0�����OK
#define LTVT_PLT_CHARA (0)
#define LTVT_PLT_BG (0)

#define LTVT_VRAM_ADR_CHARA (0x10000)
#define LTVT_VRAM_ADR_BG    (0x28000)
#define LTVT_VRAM_PAGE_SIZE (0x0C000)

#define LTVT_TRANS_COUNT_MAX (20)
#define LTVT_TRANS_SIZE (0x3000)    //1�t���[���ɑ���]����
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

typedef enum
{
  LTTS_CHARA1,
  LTTS_CHARA2,
  LTTS_CHARA3,
  LTTS_CHARA4,
  LTTS_TRANS_WAIT,
  
  LTTS_MAX,
}LOCAL_TVT_TRANS_STATE;

typedef enum
{
  LTCR_PLT,
  LTCR_NCG,
  LTCR_ANM,

  LTCR_MAX,
  
}LOCAL_TVT_CELL_RES;

typedef enum
{
  LTTZ_NOON,
  LTTZ_EVENING,
  LTTZ_NIGHT,
}LOCAL_TVT_TIME_ZONE;
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _LOCAL_TVT_CHARA LOCAL_TVT_CHARA;

typedef struct
{
  u8 chara[LOCAL_TVT_MEMBER_MAX];
  u8 back[LOCAL_TVT_MEMBER_MAX];
}LOCAL_TVT_SCRIPT_HEADER;

typedef struct
{
  u8  comNo;
  u8  charaNo;
  u16 option;
}LOCAL_TVT_SCRIPT_DATA;

struct _LOCAL_TVT_WORK
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;

  LOCAL_TVT_MODE mode;
  
  u8 state;
  u8 bufNo;
  u8 transCnt;
  LOCAL_TVT_TIME_ZONE timeZone;
  
  LOCAL_TVT_TRANS_STATE transState;
  
  ARCHANDLE *arcHandle;
  LOCAL_TVT_CHARA *charaWork[LOCAL_TVT_MEMBER_MAX];
  LOCAL_TVT_CHARA *transChara;
  
  LOCAL_TVT_INIT_WORK *initWork;
  
  BOOL isFirstCommand;
  void *scriptRes;
  LOCAL_TVT_SCRIPT_HEADER *scriptHead;
  LOCAL_TVT_SCRIPT_DATA *scriptData;
  
  u32 cellRes[LTCR_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkRecIcon;
  
    //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT        *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  GFL_MSGDATA     *talkMsgHandle;
  STRBUF          *msgStr;
  WORDSET         *wordSet;
  PRINT_QUE       *printQue;
  APP_KEYCURSOR_WORK *cursorWork;
  APP_PRINTSYS_COMMON_WORK streamMng;
  u8  talkCharaNo;

  u8 scriptIdx;
  u16 waitCnt;
};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern const u8 LOCAL_TVT_CHARA_BASE_POS[LOCAL_TVT_MEMBER_MAX][2];
