//======================================================================
/**
 * @file	ctvt_talk.c
 * @brief	�ʐMTVT�V�X�e���F��b�������
 * @author	ariizumi
 * @data	09/12/18
 *
 * ���W���[�����FCTVT_TALK
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/camera_system.h"
#include "net/network_define.h"
#include "app/app_menu_common.h"

#include "msg/msg_comm_tvt.h"

#include "comm_tvt.naix"

#include "ctvt_talk.h"
#include "ctvt_comm.h"
#include "ctvt_camera.h"
#include "ctvt_mic.h"
#include "enc_adpcm.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define CTVT_TALK_SLIDER_X (8)
#define CTVT_TALK_SLIDER_Y (116)
#define CTVT_TALK_SLIDER_MOVE_Y (40)  //��ɂ����ɂ�40

#define CTVT_TALK_BAR_ICON_Y (192-12)
#define CTVT_TALK_YOBIDASHI_X (152)
#define CTVT_TALK_PAUSE_X (192)
#define CTVT_TALK_RETURN_X (224)

#define CTVT_TALK_REC_BUTTON_LEFT (4)
#define CTVT_TALK_REC_BUTTON_TOP (1)
#define CTVT_TALK_REC_BUTTON_WIDTH (23)
#define CTVT_TALK_REC_BUTTON_HEIGHT (13)

#define CTVT_TALK_DRAW_BUTTON_LEFT   (4)
#define CTVT_TALK_DRAW_BUTTON_TOP    (15)
#define CTVT_TALK_DRAW_BUTTON_WIDTH  (23)
#define CTVT_TALK_DRAW_BUTTON_HEIGHT (6)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//�X�e�[�g
typedef enum
{
  CTS_FADEIN,
  CTS_FADEIN_WAIT,

  CTS_FADEOUT_DOWN, //�������t�F�[�h
  CTS_FADEOUT_BOTH, //�����t�F�[�h
  CTS_FADEOUT_WAIT,

  CTS_WAIT,

  CTS_REQ_TALK,
  CTS_REQ_TALK_WAIT,
  CTS_TALKING,
}CTVT_TALK_STATE;

typedef enum
{
  CTSS_INIT_REC,
  CTSS_RECORDING,
  CTSS_REC_TRANS, //�]��
  CTSS_WAIT_PLAY, 
  
  CTSS_GO_DRAW,
  CTSS_GO_END,
}CTVT_TALK_SUB_STATE;

//Rec�{�^�����
typedef enum
{
  CRBT_NONE,
  CRBT_TALKING,
  CRBT_DISALE,
  
  CRBT_MAX,
}CTVT_REC_BUTTON_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//���[�N
struct _CTVT_TALK_WORK
{
  CTVT_TALK_STATE state;
  CTVT_TALK_SUB_STATE subState;
  s16         sliderPos;

  //�^���֌W  
  CTVT_MIC_WORK *micWork;
  BOOL  reqSendWave;
  u8    sendCnt;
  
  void  *sendWaveBuf;  //����2�͂�������A�h���X������
  CTVT_COMM_WAVE_HEADER *sendWaveData;
  void *sendWaveBufTop;
  
  BOOL isUpdateMsgRec;
  BOOL isUpdateMsgDraw;
  GFL_BMPWIN *recWin;
  GFL_BMPWIN *drawWin;
  
  
  //�Z���֌W
  GFL_CLWK    *clwkSlider;
  GFL_CLWK    *clwkYobidasi;
  GFL_CLWK    *clwkPause;
  GFL_CLWK    *clwkReturn;

  CTVT_REC_BUTTON_TYPE recButtonState;
  CTVT_REC_BUTTON_TYPE befRecButtonState;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void CTVT_TALK_UpdateWait( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork );
static void CTVT_TALK_UpdateTalk( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork );
static void CTVT_TALK_UpdateButton( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork );

static const GFL_UI_TP_HITTBL CTVT_TALK_HitRecButton[2] = 
{
  {
    CTVT_TALK_REC_BUTTON_TOP*8,
    (CTVT_TALK_REC_BUTTON_TOP+CTVT_TALK_REC_BUTTON_HEIGHT)*8,
    CTVT_TALK_REC_BUTTON_LEFT*8,
    (CTVT_TALK_REC_BUTTON_LEFT+CTVT_TALK_REC_BUTTON_WIDTH)*8,
  },
  {GFL_UI_TP_HIT_END,0,0,0}
};

static const GFL_UI_TP_HITTBL CTVT_TALK_HitDrawButton[2] = 
{
  {
    CTVT_TALK_DRAW_BUTTON_TOP*8,
    (CTVT_TALK_DRAW_BUTTON_TOP+CTVT_TALK_DRAW_BUTTON_HEIGHT)*8,
    CTVT_TALK_DRAW_BUTTON_LEFT*8,
    (CTVT_TALK_DRAW_BUTTON_LEFT+CTVT_TALK_DRAW_BUTTON_WIDTH)*8,
  },
  {GFL_UI_TP_HIT_END,0,0,0}
};
//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
CTVT_TALK_WORK* CTVT_TALK_InitSystem( COMM_TVT_WORK *work , const HEAPID heapId )
{
  u8 i;
  CTVT_TALK_WORK* talkWork = GFL_HEAP_AllocClearMemory( heapId , sizeof(CTVT_TALK_WORK) );
  
  talkWork->micWork = CTVT_MIC_Init( heapId );
  talkWork->sendWaveBuf = GFL_HEAP_AllocClearMemory( heapId , sizeof(CTVT_COMM_WAVE_HEADER)+CTVT_SEND_WAVE_SIZE_ONE );
  talkWork->sendWaveData = talkWork->sendWaveBuf;
  talkWork->sendWaveBufTop = (void*)((u32)talkWork->sendWaveBuf+sizeof(CTVT_COMM_WAVE_HEADER));
  talkWork->sliderPos = 0;
  return talkWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void CTVT_TALK_TermSystem( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  
  GFL_HEAP_FreeMemory( talkWork->sendWaveData );
  CTVT_MIC_Term( talkWork->micWork );
  GFL_HEAP_FreeMemory( talkWork );
}
//--------------------------------------------------------------
//	���[�h�ؑ֎�������
//--------------------------------------------------------------
void CTVT_TALK_InitMode( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  ARCHANDLE* arcHandle = COMM_TVT_GetArcHandle( work );

  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NSCR , 
                    CTVT_FRAME_SUB_MISC ,  0 , 0, FALSE , heapId );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MISC );
  
  {
    //�X���C�_�[
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = CTVT_TALK_SLIDER_X;
    cellInitData.pos_y = CTVT_TALK_SLIDER_Y - talkWork->sliderPos;
    cellInitData.anmseq = CTOAS_SLIDER;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    talkWork->clwkSlider = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( talkWork->clwkSlider , TRUE );

    //�Ăяo���{�^��
    cellInitData.pos_x = CTVT_TALK_YOBIDASHI_X;
    cellInitData.pos_y = CTVT_TALK_BAR_ICON_Y;
    cellInitData.anmseq = CTOAS_YOBIDASHI;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    talkWork->clwkYobidasi = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( talkWork->clwkYobidasi , TRUE );

    //�ꎞ��~�{�^��
    cellInitData.pos_x = CTVT_TALK_PAUSE_X;
    cellInitData.pos_y = CTVT_TALK_BAR_ICON_Y;
    //TODO �ꎞ��~�Ή�
    cellInitData.anmseq = CTOAS_PAUSE;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    talkWork->clwkPause = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( talkWork->clwkPause , TRUE );

    //�߂�{�^��
    cellInitData.pos_x = CTVT_TALK_RETURN_X;
    cellInitData.pos_y = 192-24;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    talkWork->clwkReturn = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( talkWork->clwkReturn , TRUE );
  }

  talkWork->state = CTS_FADEIN;
  talkWork->recButtonState = CRBT_NONE;
  talkWork->befRecButtonState = CRBT_MAX;

  talkWork->recWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                        7 , 10 , 17 , 2 ,
                                        CTVT_PAL_BG_SUB_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  talkWork->drawWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                        7 , 17 , 17 , 2 ,
                                        CTVT_PAL_BG_SUB_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( talkWork->recWin ) , 0x0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( talkWork->drawWin) , 0x0 );
  GFL_BMPWIN_MakeScreen( talkWork->recWin );
  GFL_BMPWIN_MakeScreen( talkWork->drawWin );
  {
    GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
    GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    STRBUF *str;
    u16 len;
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_TALK_01 );
    len = PRINTSYS_GetStrWidth( str , fontHandle , 0 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( talkWork->recWin ) , 
            (17*8-len)/2 , 0 , str , fontHandle ,CTVT_FONT_COLOR_WHITE );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_TALK_02 );
    len = PRINTSYS_GetStrWidth( str , fontHandle , 0 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( talkWork->drawWin ) , 
            (17*8-len)/2 , 0 , str , fontHandle ,CTVT_FONT_COLOR_WHITE );
    GFL_STR_DeleteBuffer( str );
    
  }
  talkWork->isUpdateMsgRec = TRUE;
  talkWork->isUpdateMsgDraw = TRUE;
  GFL_BG_LoadScreenReq(CTVT_FRAME_SUB_MSG);

}

//--------------------------------------------------------------
//	���[�h�ؑ֎��J��
//--------------------------------------------------------------
void CTVT_TALK_TermMode( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{

  GFL_BMPWIN_ClearTransWindow( talkWork->recWin );
  GFL_BMPWIN_ClearTransWindow( talkWork->drawWin );
  GFL_BMPWIN_Delete( talkWork->recWin );
  GFL_BMPWIN_Delete( talkWork->drawWin );
  
  GFL_CLACT_WK_Remove( talkWork->clwkReturn );
  GFL_CLACT_WK_Remove( talkWork->clwkPause );
  GFL_CLACT_WK_Remove( talkWork->clwkYobidasi );
  GFL_CLACT_WK_Remove( talkWork->clwkSlider );
  GFL_BG_ClearScreen( CTVT_FRAME_SUB_MISC );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MISC );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
const COMM_TVT_MODE CTVT_TALK_Main( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  switch( talkWork->state )
  {
  case CTS_FADEIN:
    if( COMM_TVT_GetUpperFade( work ) == TRUE )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
              WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    }
    else
    {
      WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
              WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    }
    talkWork->state = CTS_FADEIN_WAIT;
    break;
  case CTS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      talkWork->state = CTS_WAIT;
    }
    break;

  case CTS_FADEOUT_DOWN:
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    COMM_TVT_SetUpperFade( work , FALSE );
    talkWork->state = CTS_FADEOUT_WAIT;
    break;
  case CTS_FADEOUT_BOTH:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    COMM_TVT_SetUpperFade( work , TRUE );
    talkWork->state = CTS_FADEOUT_WAIT;
    break;

  case CTS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( talkWork->subState == CTSS_GO_DRAW )
      {
        return CTM_DRAW;
      }
      else
      {
        return CTM_END;
      }
    }
    break;
    
  case CTS_WAIT:
    CTVT_TALK_UpdateWait( work , talkWork );
    break;
    
  case CTS_REQ_TALK:
    {
      const int isContTbl = GFL_UI_TP_HitCont( CTVT_TALK_HitRecButton );
      if( isContTbl == 0 || (GFL_UI_KEY_GetCont() & CTVT_BUTTON_TALK) )
      {
        CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
        const BOOL ret = CTVT_COMM_SendFlg( work , commWork , CCFT_REQ_TALK , 0 );
        if( ret == TRUE )
        {
          talkWork->state = CTS_REQ_TALK_WAIT;
        }
      }
      else
      {
        talkWork->state = CTS_WAIT;
      }
    }
    break;
  
  case CTS_REQ_TALK_WAIT:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      const u8 talkMember = CTVT_COMM_GetTalkMember( work , commWork );
      const u8 selfId = CTVT_COMM_GetSelfNetId( work , commWork );
      const int isContTbl = GFL_UI_TP_HitCont( CTVT_TALK_HitRecButton );
      if( isContTbl == 0 || (GFL_UI_KEY_GetCont() & CTVT_BUTTON_TALK)  )
      {
        if( talkMember != CTVT_COMM_INVALID_MEMBER )
        {
          if( talkMember == selfId )
          {
            //��b�J�n
            talkWork->state = CTS_TALKING;
            talkWork->subState = CTSS_INIT_REC;
          }
          else
          {
            //���I����
            talkWork->state = CTS_WAIT;
          }
        }
      }
      else
      {
        //��b�L�����Z��
        talkWork->state = CTS_WAIT;
      }
    }
    break;
  case CTS_TALKING:
    CTVT_TALK_UpdateTalk( work , talkWork );
    break;
  }

  CTVT_TALK_UpdateButton( work , talkWork );
  CTVT_MIC_Main( talkWork->micWork );

  if( talkWork->isUpdateMsgRec == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( talkWork->recWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( talkWork->recWin );
      talkWork->isUpdateMsgRec = FALSE;
    }
  }
  if( talkWork->isUpdateMsgDraw == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( talkWork->drawWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( talkWork->drawWin );
      talkWork->isUpdateMsgDraw = FALSE;
    }
  }

  return CTM_TALK;
}


//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
static void CTVT_TALK_UpdateWait( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );

  const u8 connectNum = COMM_TVT_GetConnectNum( work );
  //u32 trgX,trgY,contX,contY;
  //const BOOL isTrg = GFL_UI_TP_GetPointTrg( &trgX,&trgY );
  //const BOOL isCont = GFL_UI_TP_GetPointTrg( &contX,&contY );


  //��b�{�^��
  if( connectNum == 1 )
  {
    talkWork->recButtonState = CRBT_DISALE;
  }
  else
  {
    const u8 selfNexId = CTVT_COMM_GetSelfNetId( work , commWork );
    const u8 talkMember = CTVT_COMM_GetTalkMember( work , commWork );
    BOOL isActiveRecButton;
    
    if( talkMember != CTVT_COMM_INVALID_MEMBER )
    {
      if( talkMember == selfNexId )
      {
        CTVT_COMM_SendFlg( work , commWork , CCFT_CANCEL_TALK , 0 );
      }
      talkWork->recButtonState = CRBT_DISALE;
    }
    else
    {
      if( GFL_UI_KEY_GetTrg() & CTVT_BUTTON_TALK )
      {
        talkWork->state = CTS_REQ_TALK;
      }
      else
      if( GFL_UI_TP_HitTrg( CTVT_TALK_HitRecButton ) == 0 )
      {
        talkWork->state = CTS_REQ_TALK;
      }
      
      talkWork->recButtonState = CRBT_NONE;
    }
  }
  
  //���G�`���{�^��
  if( talkWork->state == CTS_WAIT )
  {
    if( GFL_UI_KEY_GetTrg() & CTVT_BUTTON_DRAW ||
        (GFL_UI_TP_HitTrg( CTVT_TALK_HitDrawButton ) == 0) )
    {
      talkWork->state = CTS_FADEOUT_BOTH;
      talkWork->subState = CTSS_GO_DRAW;
    }
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    talkWork->subState = CTSS_GO_END;
    talkWork->state = CTS_FADEOUT_BOTH;
    COMM_TVT_SetSusspend( work , TRUE );
  }
}

//--------------------------------------------------------------
//	�^�������̍X�V
//--------------------------------------------------------------
static void CTVT_TALK_UpdateTalk( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  const int isContTbl = GFL_UI_TP_HitCont( CTVT_TALK_HitRecButton );
  switch( talkWork->subState )
  {
  case CTSS_INIT_REC:
    {
      const BOOL ret = CTVT_MIC_StartRecord( talkWork->micWork );
      if( ret == TRUE )
      {
        talkWork->recButtonState = CRBT_TALKING;
        talkWork->subState = CTSS_RECORDING;
        talkWork->sendCnt = 0;
        talkWork->reqSendWave = FALSE;
        ENC_ADPCM_ResetParam();
      }
    }
    break;
  case CTSS_RECORDING:
    if( !(isContTbl == 0) && !(GFL_UI_KEY_GetCont() & CTVT_BUTTON_TALK) )
    {
      CTVT_MIC_StopRecord( talkWork->micWork );
    }

    if( CTVT_MIC_IsRecording( talkWork->micWork ) == FALSE )
    {
      talkWork->subState = CTSS_REC_TRANS;
      talkWork->recButtonState = CRBT_DISALE;
    }
    //�]���͘^���������̂ŃX�L�b�v
    //break through;
  case CTSS_REC_TRANS:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      const BOOL isSendWave = CTVT_COMM_GetCommWaveData( work , commWork );
      const u32 recSize = CTVT_MIC_GetRecSize( talkWork->micWork );
      if( talkWork->reqSendWave == FALSE && isSendWave == FALSE )
      {
        if( recSize >= CTVT_SEND_WAVE_SIZE_ONE*(talkWork->sendCnt+1) ||
            talkWork->subState == CTSS_REC_TRANS)
        {
          //�G���R�[�h
          void* recBuffer = CTVT_MIC_GetRecBuffer( talkWork->micWork );
          void* sendTopBuf = (void*)((u32)recBuffer+talkWork->sendCnt*CTVT_SEND_WAVE_SIZE_ONE );
          const u32 endSize = CTVT_MIC_EncodeData( talkWork->micWork , sendTopBuf , talkWork->sendWaveBufTop , CTVT_SEND_WAVE_SIZE_ONE );
          //���M
          talkWork->sendWaveData->dataNo = talkWork->sendCnt;
          talkWork->sendWaveData->encSize = endSize;
          talkWork->sendWaveData->recSize = recSize;
          if( talkWork->subState == CTSS_REC_TRANS && 
            recSize <= CTVT_SEND_WAVE_SIZE_ONE*(talkWork->sendCnt+1) )
          {
            talkWork->sendWaveData->isLast = TRUE;
          }
          else
          {
            talkWork->sendWaveData->isLast = FALSE;
          }
              
          talkWork->reqSendWave = TRUE;
        }
      }
      
      if( talkWork->reqSendWave == TRUE )
      {
        const BOOL ret = CTVT_COMM_SendWave( work , commWork , talkWork->sendWaveData );
        if( ret == TRUE )
        {
          talkWork->sendCnt++;
          talkWork->reqSendWave = FALSE;
          if( talkWork->sendWaveData->isLast == TRUE )
          {
            talkWork->subState = CTSS_WAIT_PLAY;
          }
        }
      }
    }

    break;
  case CTSS_WAIT_PLAY:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      const BOOL isSendWave = CTVT_COMM_GetCommWaveData( work , commWork );
      if( isSendWave == FALSE )
      {
        const BOOL ret = CTVT_COMM_SendFlg( work , commWork , CCFT_FINISH_TALK , 0 );
        if( ret == TRUE )
        {
          talkWork->state = CTS_WAIT;
        }
      }
    }
    break;
  }
}


//--------------------------------------------------------------
//	�{�^���̐F�ύX
//--------------------------------------------------------------
static void CTVT_TALK_UpdateButton( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  if( talkWork->recButtonState != talkWork->befRecButtonState )
  {
    u8 palNo;
    if( talkWork->recButtonState == CRBT_TALKING )
    {
      palNo = CTVT_PAL_BG_BUTTON_ACTIVE;
    }
    else
    if( talkWork->recButtonState == CRBT_DISALE )
    {
      palNo = CTVT_PAL_BG_BUTTON_DISABLE;
    }
    else
    {
      palNo = CTVT_PAL_BG_BUTTON_NONE;
    }
    
    GFL_BG_ChangeScreenPalette( CTVT_FRAME_SUB_MISC ,
                                CTVT_TALK_REC_BUTTON_LEFT , 
                                CTVT_TALK_REC_BUTTON_TOP , 
                                CTVT_TALK_REC_BUTTON_WIDTH , 
                                CTVT_TALK_REC_BUTTON_HEIGHT , 
                                palNo );
    GFL_BG_LoadScreenV_Req( CTVT_FRAME_SUB_MISC );
    talkWork->befRecButtonState = talkWork->recButtonState;
  }
  
}


#pragma mark[>outer func
CTVT_MIC_WORK* CTVT_TALK_GetMicWork( COMM_TVT_WORK *work , CTVT_TALK_WORK *talkWork )
{
  return talkWork->micWork;
}
