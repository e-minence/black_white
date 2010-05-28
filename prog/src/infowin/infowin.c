//======================================================================
/**
 * @file  ���o�[
 * @brief ��ʏ�ɏ풓������\�����
 * @author  ariizumi
 * @data  09/02/23
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "net/wih_dwc.h"

#include "arc_def.h"
#include "infowin.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  debug
//======================================================================

//======================================================================
//  define
//======================================================================
static const u8 INFOWIN_WIDTH = 32;
static const u8 INFOWIN_HEIGHT = 2;

static const u16 INFOWIN_CLEAR_CHR = 0x00;
static const u16 INFOWIN_BLANK_CHR_UP = 0x0F;
static const u16 INFOWIN_BLANK_CHR_DOWN = 0x1F;
static const u16 INFOWIN_LINE_CHRNUM = 0x10;  //1��̐�

static const u8 INFOWIN_TIMECHR_START = 0x01;
static const u8 INFOWIN_COLONCHR = 0x0B;

static const u8 INFOWIN_TIME_DRAW_X = 1;
static const u8 INFOWIN_TIME_DRAW_WIDTH = 5;
static const u8 INFOWIN_AMPM_DRAW_X = 6;
static const u8 INFOWIN_AMPM_DRAW_WIDTH = 2;

static const u8 INFOWIN_IR_DRAW_X = 9;
static const u8 INFOWIN_IR_DRAW_WIDTH = 4;

static const u8 INFOWIN_WIFI_DRAW_X = 14;
static const u8 INFOWIN_WIFI_DRAW_WIDTH = 4;

static const u8 INFOWIN_BEACON_DRAW_X = 19;
static const u8 INFOWIN_BEACON_DRAW_WIDTH = 7;

static const u8 INFOWIN_WMI_DRAW_X = 30;
static const u8 INFOWIN_WMI_DRAW_WIDTH = 2;

static const u8 INFOWIN_BATTERY_DRAW_X = 28;
static const u8 INFOWIN_BATTERY_DRAW_WIDTH = 2;

static const u16 INFOWIN_CHR_FLIP_X = 0x400;
static const u16 INFOWIN_CHR_FLIP_Y = 0x800;
#define INFOWIN_CALC_NUM_SCR(num,pal) (num+INFOWIN_TIMECHR_START+(pal<<12))
#define INFOWIN_CHECK_BIT(value,bit) (value&bit?TRUE:FALSE)

static const u16 INFOWIN_COLOR_BLACK = 0x0000;
static const u16 INFOWIN_COLOR_WHITE1 = GX_RGB(31,31,31); //�_�Ōv�Z���C��
static const u16 INFOWIN_COLOR_WHITE2 = GX_RGB(19,19,19); //�_�Ōv�Z���C��
static const u16 INFOWIN_COLOR_GRAY1  = GX_RGB( 9, 9, 9); //�_�Ōv�Z���C��
static const u16 INFOWIN_COLOR_GRAY2  = GX_RGB( 6, 6, 6); //�_�Ōv�Z���C��
static const u16 INFOWIN_COLOR_RED1   = GX_RGB(31, 4, 4);
static const u16 INFOWIN_COLOR_RED2   = GX_RGB(16, 2, 2);
static const u16 INFOWIN_COLOR_YERROW1= GX_RGB(31,22, 0);
static const u16 INFOWIN_COLOR_YERROW2= GX_RGB(16,11, 0);
static const u16 INFOWIN_COLOR_GREEN1 = GX_RGB( 0,16,31);
static const u16 INFOWIN_COLOR_GREEN2 = GX_RGB( 0, 8,16);

static const u16 INFOWIN_BEACON_SCAN_SYNC = 10*60;  //10�b�X�L��������
static const u16 INFOWIN_BEACON_WAIT_SYNC = 10*60;//30*60;  //30�b�l�b�g�J�n���܂�

#define INFOWIN_ANIME_MAX (16*5)
#define INFOWIN_ANIME_SPAN (16)
#define INFOWIN_ANIME_SPAN_H (8)

//======================================================================
//  enum
//======================================================================
enum
{
  INFOWIN_REFRESH_TIME    = 0x0001,
  INFOWIN_REFRESH_GPF_SYNC  = 0x0002, //���g�p
  INFOWIN_REFRESH_IR      = 0x0004,
  INFOWIN_REFRESH_WIFI    = 0x0008,
  INFOWIN_REFRESH_BEACON    = 0x0010,
  INFOWIN_REFRESH_BATTERY   = 0x0020,

  INFOWIN_DISP_GPF_SYNC = 0x0100, //���g�p
  INFOWIN_DISP_IR      = 0x0200,
  INFOWIN_DISP_BEACON  = 0x0400,
  INFOWIN_IS_ANIME     = 0x0800,
  INFOWIN_GET_BEACON   = 0x1000,

  INFOWIN_REFRESH_MASK = 0x003F,
  INFOWIN_DISP_MASK = 0x1700,
  INFOWIN_BIT_MASK = 0xFFFF,
  INFOWIN_FLG_NULL = 0x0000,
};

//�p���b�g�ԍ���`
enum
{
  INFOWIN_PLT_NULL,
  INFOWIN_PLT_COMMON_BLACK,
  INFOWIN_PLT_COMMON_WHITE,
  INFOWIN_PLT_TIME_STR,
  INFOWIN_PLT_GPF_SYNC_BACK,  //���g�p
  INFOWIN_PLT_GPF_SYNC_STR,   //���g�p
  INFOWIN_PLT_IR_BACK,
  INFOWIN_PLT_IR_STR,
  INFOWIN_PLT_WIFI_BACK,
  INFOWIN_PLT_WIFI_STR,
  INFOWIN_PLT_BEACON_BACK,
  INFOWIN_PLT_BEACON_STR,
  INFOWIN_PLT_TIME_BACK,
  INFOWIN_PLT_BATTERY_STR,
  INFOWIN_PLT_NOTUSE1,
  INFOWIN_PLT_NOTUSE2,
};


typedef enum
{
  IWS_DISABLE,  //Wifi����
  IWS_ENABLE,   //Wifi�L��(�I����
  IWS_LOCKED,   //�����T�[�o�[
  IWS_GENERAL,  //�������ʏ�T�[�o�[
  IWS_NINTENDO, //NintendoSpot

  IWS_MAX,
}INFOWIN_WIFI_STATE;

typedef enum
{
  ICS_STOP,
  ICS_WAIT_INIT,
  ICS_FINISH_INIT,
  ICS_SCAN_BEACON,
  ICS_WAIT_FINISH,

  ICS_MAX,
}INFOWIN_COMM_STATE;
//======================================================================
//  typedef struct
//======================================================================
//�`��n���[�N
typedef struct
{
  u32 ncgPos;
  u16 isRefresh;
  u8  bgplane;
  u8  pltNo;
  u8  anmCnt;
  WIFI_LIST *wifiList;
  BOOL isCgearOn;

  //���v
  BOOL  isDispColon;
  BOOL  isPm;
  u8    min;
  u8    hour;
  u8    batteryCnt;

  //BATTERY
  u8  batteryVol;   //0~3

  //Wi-Fi
  INFOWIN_WIFI_STATE  wifiState;

  GFL_TCB *vblankFunc;
}INFOWIN_WORK;

//======================================================================
//  proto
//======================================================================

static  void  INFOWIN_VBlankFunc( GFL_TCB* tcb , void* work );
static  void  INFOWIN_SetScrFunc( const u16 topChar , const u8 pltNo , const u8 posX , const u8 posY , const u8 sizeX , const u8 sizeY );
static  void  INFOWIN_SetPltFunc( const u8 pltPos , const u16 *col , const u8 num );
static  void  INFOWIN_InitBg( u8 bgplane , u8 pltNo, HEAPID heapId );
static  void  INFOWIN_UpdateTime(void);

static inline void INFOWIN_SetBit( const u16 bit );
static inline void INFOWIN_ResetBit( const u16 bit );


static INFOWIN_WORK *infoWk = NULL;
//������
//  @param bgplane  BG�� (CHAR��VRAM��0x1000�K�v)
//  @param pltNo  �p���b�g�ԍ�(1�{�K�v)
//  @param commSys  �ʐM�V�X�e��
//  @param heapId �q�[�vID
void  INFOWIN_Init( const u8 bgplane , const u8 pltNo , GAMEDATA *gameData , const HEAPID heapId )
{
  GF_ASSERT_MSG(infoWk == NULL ,"Infobar is initialized!!\n");

  infoWk = GFL_HEAP_AllocMemory( heapId , sizeof( INFOWIN_WORK ) );
  GFL_STD_MemClear( infoWk, sizeof(INFOWIN_WORK) );

  infoWk->bgplane = bgplane;
  infoWk->pltNo = pltNo;
  INFOWIN_InitBg( bgplane,pltNo,heapId );

  infoWk->vblankFunc = GFUser_VIntr_CreateTCB( INFOWIN_VBlankFunc , (void*)infoWk , 1 );

  infoWk->wifiState = IWS_DISABLE;
  infoWk->batteryVol = GFL_UI_GetBattLevel();
  if( gameData != NULL )
  {
    infoWk->wifiList = GAMEDATA_GetWiFiList(gameData);
    infoWk->isCgearOn =  GAMEDATA_GetAlwaysNetFlag( gameData );
  }
  else
  {
    infoWk->wifiList = NULL;
    infoWk->isCgearOn =  FALSE;
  }
  infoWk->anmCnt = 0;
  
  //��񋭐��I�ɍX�V
  INFOWIN_Update();
  infoWk->isRefresh = INFOWIN_REFRESH_MASK;

}

void  INFOWIN_Update( void )
{
  u8 connectNum = 0;
  GFLNetInitializeStruct *netInit = NULL;
  if( infoWk == NULL )
    return;

  INFOWIN_UpdateTime();
  //�o�b�e���[
  infoWk->batteryCnt++;
  if( infoWk->batteryCnt > 60 )
  {
    const u8 nowVol = GFL_UI_GetBattLevel();
    if( infoWk->batteryVol != nowVol )
    {
      infoWk->batteryVol = nowVol;
      infoWk->isRefresh |= INFOWIN_REFRESH_BATTERY;
    }
    infoWk->batteryCnt = 0;
  }

  //�ʐM�n�̍X�V
  if( GFL_NET_IsInit() == TRUE )
  {
    netInit = GFL_NET_GetNETInitStruct();
    connectNum = GFL_NET_GetConnectNum();
  }
  
  if( connectNum < 2 )
  {
    u32 bit = WIH_DWC_GetAllBeaconTypeBit(infoWk->wifiList);
    
    //����Ⴂ�r�[�R���̃`�F�b�N
    int checkCnt=0;
    if( infoWk->isCgearOn == TRUE &&
        GAMEBEACON_Get_UpdateLogNo( &checkCnt ) != GAMEBEACON_SYSTEM_LOG_MAX )
    {
      INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
      INFOWIN_SetBit(INFOWIN_GET_BEACON);
    }
    else
    {
      INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
      INFOWIN_ResetBit(INFOWIN_GET_BEACON);
    }

    
    if(bit & GAME_COMM_SBIT_IRC_ALL)
    {
      /*
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR) == FALSE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_IR);
        INFOWIN_SetBit(INFOWIN_DISP_IR);
      }
      */
      //IR�͒ʏ펞���Ȃ�
    }
    else
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR) == TRUE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_IR);
        INFOWIN_ResetBit(INFOWIN_DISP_IR);
      }
    }
    
    if(bit & GAME_COMM_SBIT_WIRELESS_ALL)
    {
      if( bit & (GAME_COMM_STATUS_BIT_WIRELESS|GAME_COMM_STATUS_BIT_WIRELESS_TR) )
      {
        //�_�Ń��[�h
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_SetBit(INFOWIN_DISP_BEACON);
        INFOWIN_SetBit(INFOWIN_IS_ANIME);
      }
      else
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON) == FALSE )
      {
        infoWk->anmCnt = 0;
        INFOWIN_ResetBit(INFOWIN_IS_ANIME);
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_SetBit(INFOWIN_DISP_BEACON);
      }
    }
    else
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON) == TRUE )
      {
        infoWk->anmCnt = 0;
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_ResetBit(INFOWIN_DISP_BEACON);
        INFOWIN_ResetBit(INFOWIN_IS_ANIME);
      }
    }

    if(bit & GAME_COMM_SBIT_WIFI_ALL)
    {
      if( infoWk->wifiState == IWS_DISABLE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_WIFI);
        infoWk->wifiState = IWS_ENABLE;
      }
    }
    else
    {
      if( infoWk->wifiState != IWS_DISABLE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_WIFI);
        infoWk->wifiState = IWS_DISABLE;
      }
    }
  }
  else
  if( netInit != NULL )
  {
    //�ʏ�ʐM��
    //IR
    if( netInit->bNetType == GFL_NET_TYPE_IRC )
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR) == FALSE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_IR);
        INFOWIN_SetBit(INFOWIN_DISP_IR);
      }
    }
    else
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR) == TRUE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_IR);
        INFOWIN_ResetBit(INFOWIN_DISP_IR);
      }
    }
    
    //WIRELESS
    if( netInit->bNetType == GFL_NET_TYPE_WIRELESS ||
        netInit->bNetType == GFL_NET_TYPE_IRC_WIRELESS ||
        netInit->bNetType == GFL_NET_TYPE_WIRELESS_SCANONLY )
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON) == FALSE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_SetBit(INFOWIN_DISP_BEACON);
        INFOWIN_ResetBit(INFOWIN_IS_ANIME);
        infoWk->anmCnt = 0;
      }
      if( netInit->gsid == WB_NET_PALACE_SERVICEID )
      {
        //�_�Ń��[�h
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_SetBit(INFOWIN_DISP_BEACON);
        INFOWIN_SetBit(INFOWIN_IS_ANIME);
      }
    }
    else
    {
      if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON) == TRUE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_BEACON);
        INFOWIN_ResetBit(INFOWIN_DISP_BEACON);
        INFOWIN_ResetBit(INFOWIN_IS_ANIME);
        infoWk->anmCnt = 0;
      }
    }
    
    //WIFI
    if( netInit->bNetType == GFL_NET_TYPE_WIFI ||
        netInit->bNetType == GFL_NET_TYPE_WIFI_LOBBY ||
        netInit->bNetType == GFL_NET_TYPE_WIFI_GTS )
    {
      if( infoWk->wifiState == IWS_DISABLE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_WIFI);
        infoWk->wifiState = IWS_ENABLE;
      }
    }
    else
    {
      if( infoWk->wifiState != IWS_DISABLE )
      {
        INFOWIN_SetBit(INFOWIN_REFRESH_WIFI);
        infoWk->wifiState = IWS_DISABLE;
      }
    }
  }
}

void  INFOWIN_Exit( void )
{
  if( infoWk == NULL )
    return;

  GFL_TCB_DeleteTask( infoWk->vblankFunc );

  GFL_BG_FreeCharacterArea( infoWk->bgplane , infoWk->ncgPos , INFOWIN_CHARAREA_SIZE );

  GFL_HEAP_FreeMemory(infoWk);
  infoWk = NULL;
}

static  void  INFOWIN_VBlankFunc( GFL_TCB* tcb , void* work )
{
  BOOL  transReq = FALSE;

  if( infoWk == NULL )
    return;

  //���v�̍X�V
  if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_TIME) )
  {
    u8 i;
    u16 scrData[2][5];
    u16 scrDataAmPm[2][2];
    //��
    if( infoWk->hour<10 )
    {
      scrData[0][0] = INFOWIN_BLANK_CHR_UP + (infoWk->pltNo<<12)+infoWk->ncgPos;
      scrData[1][0] = INFOWIN_BLANK_CHR_DOWN + (infoWk->pltNo<<12)+infoWk->ncgPos;
    }
    else
    {
      scrData[0][0] = INFOWIN_CALC_NUM_SCR( infoWk->hour/10,infoWk->pltNo)+infoWk->ncgPos;
      scrData[1][0] = scrData[0][0] + INFOWIN_LINE_CHRNUM;
    }
    scrData[0][1] = INFOWIN_CALC_NUM_SCR( infoWk->hour%10,infoWk->pltNo)+infoWk->ncgPos;
    scrData[0][2] = (infoWk->isDispColon?INFOWIN_COLONCHR:INFOWIN_BLANK_CHR_UP) + (infoWk->pltNo<<12)+infoWk->ncgPos;
    scrData[0][3] = INFOWIN_CALC_NUM_SCR( infoWk->min/10,infoWk->pltNo)+infoWk->ncgPos;
    scrData[0][4] = INFOWIN_CALC_NUM_SCR( infoWk->min%10,infoWk->pltNo)+infoWk->ncgPos;
    scrData[1][1] = scrData[0][1] + INFOWIN_LINE_CHRNUM;
    scrData[1][2] = (infoWk->isDispColon?INFOWIN_COLONCHR+INFOWIN_LINE_CHRNUM:INFOWIN_BLANK_CHR_DOWN) + (infoWk->pltNo<<12)+infoWk->ncgPos;
    scrData[1][3] = scrData[0][3] + INFOWIN_LINE_CHRNUM;
    scrData[1][4] = scrData[0][4] + INFOWIN_LINE_CHRNUM;


    GFL_BG_WriteScreenExpand( infoWk->bgplane ,
              INFOWIN_TIME_DRAW_X , 0 ,
              INFOWIN_TIME_DRAW_WIDTH , INFOWIN_HEIGHT ,
              scrData,0,0,5,2);

    if( infoWk->isPm == TRUE )
    {
      scrDataAmPm[0][0] = 0x0D+(infoWk->pltNo<<12)+infoWk->ncgPos;
      scrDataAmPm[1][0] = 0x1D+(infoWk->pltNo<<12)+infoWk->ncgPos;
    }
    else
    {
      scrDataAmPm[0][0] = 0x0C+(infoWk->pltNo<<12)+infoWk->ncgPos;
      scrDataAmPm[1][0] = 0x1C+(infoWk->pltNo<<12)+infoWk->ncgPos;
    }
    scrDataAmPm[0][1] = (0x0E)+(infoWk->pltNo<<12)+infoWk->ncgPos;
    scrDataAmPm[1][1] = (0x1E)+(infoWk->pltNo<<12)+infoWk->ncgPos;
    GFL_BG_WriteScreenExpand( infoWk->bgplane ,
              INFOWIN_AMPM_DRAW_X , 0 ,
              INFOWIN_AMPM_DRAW_WIDTH , INFOWIN_HEIGHT ,
              scrDataAmPm,0,0,2,2);

    infoWk->isRefresh -= INFOWIN_REFRESH_TIME;
    transReq = TRUE;
  }
  //�ԊO���̍X�V
  if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_IR) )
  {
    static const u16 IR_TOP_CHAR[2] = {0x48,0x48};
    const u8  flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR);
    INFOWIN_SetScrFunc( IR_TOP_CHAR[flg],infoWk->pltNo,
              INFOWIN_IR_DRAW_X,0,
              INFOWIN_IR_DRAW_WIDTH,2 );

    {//Pallet
      static const u16 IR_BACK_COLOR[2][2] = {
            {INFOWIN_COLOR_GRAY2,INFOWIN_COLOR_GRAY1},
            {INFOWIN_COLOR_WHITE2,INFOWIN_COLOR_WHITE1} };

      INFOWIN_SetPltFunc( INFOWIN_PLT_IR_BACK , IR_BACK_COLOR[flg] , 2 );
    }
    infoWk->isRefresh -= INFOWIN_REFRESH_IR;
    transReq = TRUE;

  }

  //Wi-Fi�̍X�V
  if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_WIFI) )
  {
    static const u16 WIFI_TOP_CHAR[IWS_MAX] = {0x28,0x28,0x28,0x28,0x28};
    INFOWIN_SetScrFunc( WIFI_TOP_CHAR[infoWk->wifiState],infoWk->pltNo,
              INFOWIN_WIFI_DRAW_X,0,
              INFOWIN_WIFI_DRAW_WIDTH,2 );

    {//Pallet
      static const u16 WIFI_BACK_COLOR[IWS_MAX][2] = {
            {INFOWIN_COLOR_GRAY2,INFOWIN_COLOR_GRAY1},
            {INFOWIN_COLOR_WHITE2,INFOWIN_COLOR_WHITE1},
            {INFOWIN_COLOR_RED2,INFOWIN_COLOR_RED1},
            {INFOWIN_COLOR_YERROW2,INFOWIN_COLOR_YERROW1},
            {INFOWIN_COLOR_GREEN2,INFOWIN_COLOR_GREEN1} };

      INFOWIN_SetPltFunc( INFOWIN_PLT_WIFI_BACK , WIFI_BACK_COLOR[infoWk->wifiState] , 2 );
    }
    infoWk->isRefresh -= INFOWIN_REFRESH_WIFI;
    transReq = TRUE;

  }

  //���C�����X�̍X�V
  if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_BEACON) )
  {
    static const u16 BEACON_TOP_CHAR = 0x20;
    static const u16 BEACON_BACK_COLOR[2][2] = 
    {
      {INFOWIN_COLOR_GRAY2,INFOWIN_COLOR_GRAY1},
      {INFOWIN_COLOR_WHITE2,INFOWIN_COLOR_WHITE1} 
    };

    INFOWIN_SetScrFunc( BEACON_TOP_CHAR,infoWk->pltNo,
              INFOWIN_BEACON_DRAW_X,0,
              INFOWIN_BEACON_DRAW_WIDTH,2 );
    
    //Pallet
    if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_IS_ANIME) )
    {
      //�_�Ń��[�h
      if( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 )
      {
        infoWk->anmCnt+=2;
      }
      else
      {
        infoWk->anmCnt++;
      }
      if( infoWk->anmCnt >= INFOWIN_ANIME_MAX )
      {
        infoWk->anmCnt = 0;
      }
      {
        const u8 anmSpan = (infoWk->anmCnt/INFOWIN_ANIME_SPAN);
        const u8 anmFrame = (infoWk->anmCnt%INFOWIN_ANIME_SPAN);
        
        if( anmSpan < 4 )
        {
          const u8 shade = INFOWIN_ANIME_SPAN_H-MATH_ABS(anmFrame-(INFOWIN_ANIME_SPAN_H));
          u8 col[2];
          u16 colRGB[2];
          col[0] = ((19-6)*shade/INFOWIN_ANIME_SPAN_H) + 6;
          col[1] = ((31-9)*shade/INFOWIN_ANIME_SPAN_H) + 9;
          colRGB[0] = GX_RGB( col[0],col[0],col[0] );
          colRGB[1] = GX_RGB( col[1],col[1],col[1] );
          INFOWIN_SetPltFunc( INFOWIN_PLT_BEACON_BACK , colRGB , 2 );
        }
        else
        {
          INFOWIN_SetPltFunc( INFOWIN_PLT_BEACON_BACK , BEACON_BACK_COLOR[0] , 2 );
        }
      }
    }
    else
    {
      const u8  flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,(INFOWIN_DISP_BEACON|INFOWIN_GET_BEACON));
      INFOWIN_SetPltFunc( INFOWIN_PLT_BEACON_BACK , BEACON_BACK_COLOR[flg] , 2 );
    }
    
    infoWk->isRefresh -= INFOWIN_REFRESH_BEACON;
    transReq = TRUE;

  }

  //�o�b�e���[�̍X�V
  if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_BATTERY) )
  {
    const u16 BATTERY_TOP_CHAR[6] = {0x40,0x41,0x42,0x43,0x44,0x45};
    const u8 flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON);
    const u8 dispPosArr[6] = {0,0,1,1,2,3};
    u16 scrData[2][2];
    u8 dispPos;
    if( OS_IsRunOnTwl() )//DSI�Ȃ�
    {
      dispPos = dispPosArr[infoWk->batteryVol];
    }
    else
    {
      if( infoWk->batteryVol == GFL_UI_BATTLEVEL_HI )
      {
        dispPos = 5;
      }
      else
      {
        dispPos = 4;
      }
    }
    scrData[0][0] = BATTERY_TOP_CHAR[dispPos] + (infoWk->pltNo<<12) + infoWk->ncgPos;
    scrData[1][0] = scrData[0][0] + INFOWIN_LINE_CHRNUM;
    scrData[0][1] = scrData[0][0] + INFOWIN_CHR_FLIP_X;
    scrData[1][1] = scrData[1][0] + INFOWIN_CHR_FLIP_X;

    GFL_BG_WriteScreenExpand( infoWk->bgplane ,
              INFOWIN_BATTERY_DRAW_X , 0,
              INFOWIN_BATTERY_DRAW_WIDTH , INFOWIN_HEIGHT ,
              scrData,0,0,INFOWIN_BATTERY_DRAW_WIDTH,INFOWIN_HEIGHT);
    {//Pallet
      static const u16 BEACON_BATTERY_COLOR = { INFOWIN_COLOR_WHITE1 };
      INFOWIN_SetPltFunc( INFOWIN_PLT_BATTERY_STR , &BEACON_BATTERY_COLOR , 1 );
    }
    infoWk->isRefresh -= INFOWIN_REFRESH_BATTERY;
    transReq = TRUE;

  }
  if( transReq == TRUE )
  {
    GFL_BG_LoadScreenReq( infoWk->bgplane );
  }
}

static  void  INFOWIN_SetScrFunc( const u16 topChar , const u8 pltNo , const u8 posX , const u8 posY , const u8 sizeX , const u8 sizeY )
{
  u8 x,y;
  //���̂Ƃ���ő�T�C�Y�Ŋm�ۑ傫���Ȃ�����v�ύX
  u16 scrData[14];
  GF_ASSERT_MSG( sizeX<=7,"Xsize over! Plz fix source!!\n");
  for( x=0;x<sizeX;x++ )
  {
    for( y=0;y<sizeY;y++ )
    {
      scrData[y*sizeX+x] = topChar+(pltNo<<12)+x+(y*INFOWIN_LINE_CHRNUM)+infoWk->ncgPos;
    }
  }
  GFL_BG_WriteScreenExpand( infoWk->bgplane ,
            posX , posY ,
            sizeX , sizeY ,
            scrData,0,0,sizeX,sizeY);
}

static  void  INFOWIN_SetPltFunc( const u8 pltPos , const u16 *col , const u8 num )
{
  GFL_BG_LoadPalette( infoWk->bgplane , (void*)col , sizeof(u16)*num , sizeof(u16)*(infoWk->pltNo*16 + pltPos) );
}


static  void  INFOWIN_InitBg( u8 bgplane , u8 pltNo, HEAPID heapId )
{
  const PALTYPE palType = (bgplane<=GFL_BG_FRAME3_M ? PALTYPE_MAIN_BG : PALTYPE_SUB_BG );
  ARCHANDLE *arcHdl = GFL_ARC_OpenDataHandle(ARCID_INFOWIN,heapId);

  //�̈�̊m��
  infoWk->ncgPos = GFL_BG_AllocCharacterArea( bgplane , INFOWIN_CHARAREA_SIZE , GFL_BG_CHRAREA_GET_B );
  GF_ASSERT_MSG( infoWk->ncgPos != AREAMAN_POS_NOTFOUND ,"Infobar�����ɕK�v�ȃL�����u�q�`�l�̈悪����Ȃ�\n" );
  //�f�[�^�̓ǂݍ���
  GFL_ARCHDL_UTIL_TransVramPalette( arcHdl,NARC_infowin_infobar_NCLR,palType,pltNo*2*0x10,2*0x10,heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHdl,NARC_infowin_infobar_NCGR,bgplane,infoWk->ncgPos,0,FALSE,heapId );
  GFL_ARC_CloseDataHandle(arcHdl);

  //�������BG�̐ݒ�

  GFL_BG_FillScreen( bgplane,INFOWIN_BLANK_CHR_UP+infoWk->ncgPos,0,0,INFOWIN_WIDTH,1,pltNo);
  GFL_BG_FillScreen( bgplane,INFOWIN_BLANK_CHR_DOWN+infoWk->ncgPos,0,1,INFOWIN_WIDTH,1,pltNo);
  //GFL_BG_FillScreen( bgplane,INFOWIN_CLEAR_CHR+infoWk->ncgPos,0,INFOWIN_HEIGHT,INFOWIN_WIDTH,32-INFOWIN_HEIGHT,pltNo);
  GFL_BG_LoadScreenReq( bgplane );
}



//--------------------------------------------------------------
//  ���v�̍X�V
//--------------------------------------------------------------
static  void  INFOWIN_UpdateTime(void)
{
  RTCTime time;

  GFL_RTC_GetTime( &time );
  {
    const BOOL isDispColon = (BOOL)(time.second&1);
    if( time.hour != infoWk->hour )
    {
      infoWk->hour = time.hour;
      if( infoWk->hour >= 12 )
      {
        infoWk->hour -=12;
        infoWk->isPm = TRUE;
      }
      else
      {
        infoWk->isPm = FALSE;
      }
      infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
    }
    if( time.minute != infoWk->min )
    {
      infoWk->min = time.minute;
      infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
    }
    if( isDispColon != infoWk->isDispColon )
    {
      infoWk->isDispColon = isDispColon;
      infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
    }
  }
}

static inline void INFOWIN_SetBit( const u16 bit )
{
  infoWk->isRefresh = infoWk->isRefresh|bit;
}

static inline void INFOWIN_ResetBit( const u16 bit )
{
  infoWk->isRefresh = infoWk->isRefresh&(INFOWIN_BIT_MASK-bit);
}
