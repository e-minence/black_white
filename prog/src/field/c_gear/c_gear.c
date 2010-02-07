//=============================================================================
/**
 * @file	  c_gear.c
 * @brief	  �R�~���j�P�[�V�����M�A
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"
#include "c_gear.h"
#include "savedata/save_tbl.h"
#include "savedata/c_gear_picture.h"
#include "system/main.h"  //HEAPID

#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "c_gear.naix"
#include "c_gear_obj_NANR_LBLDEFS.h"

#include "net/wih_dwc.h"

#include "field/field_beacon_message.h"
#include "gamesystem/game_beacon_accessor.h"

#define _NET_DEBUG (1)  //�f�o�b�O���͂P
#define _BRIGHTNESS_SYNC (0)  // �t�F�[�h�̂r�x�m�b�͗v����

// �T�E���h���z���x��
#define GEAR_SE_DECIDE_ (SEQ_SE_DP_DECIDE)
#define GEAR_SE_CANCEL_ (SEQ_SE_DP_SELECT)

#define MSG_COUNTDOWN_FRAMENUM (30*3)

//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //�E�C���h�E�̃p�^�[����

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // �^��
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (3)    // �E�C���h�E����
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (13)  // ���b�Z�[�W�t�H���g

#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
#define	_BUTTON_WIN_CGX	( 2 )


#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// �ʐM�V�X�e���E�B���h�E�]����

#define	FBMP_COL_WHITE		(15)


//-------------------------

#define PANEL_Y1 (6)
#define PANEL_Y2 (4)
#define PANEL_X1 (-2)

#define PANEL_HEIDHT1 (C_GEAR_PANEL_HEIGHT-1)
#define PANEL_HEIGHT2 (C_GEAR_PANEL_HEIGHT)

#define PANEL_WIDTH (C_GEAR_PANEL_WIDTH)
#define PANEL_SIZEXY (4)   //

typedef enum{
  _CLACT_PLT,
  _CLACT_CHR,
  _CLACT_ANM,
} _CGEAR_CLACT_TYPE;


typedef enum{
  _CGEAR_NET_BIT_IR = (1 << CGEAR_PANELTYPE_IR),
  _CGEAR_NET_BIT_WIRELESS = (1 << CGEAR_PANELTYPE_WIRELESS),
  _CGEAR_NET_BIT_WIFI = (1 << CGEAR_PANELTYPE_WIFI),
} _CGEAR_NET_BIT;


#define _CGEAR_NET_CHANGEPAL_NUM (7)
#define _CGEAR_NET_CHANGEPAL_POSX (0x8)
#define _CGEAR_NET_CHANGEPAL_POSY (1)
#define _CGEAR_NET_CHANGEPAL_MAX (3)


//--------------------------

typedef enum{
  _SELECTANIM_NONE,
  _SELECTANIM_STANDBY,
  _SELECTANIM_ANIMING,

} _SELECTANIM_ENUM;



//--------------------------

typedef struct {
  int leftx;
  int lefty;
  int width;
  int height;
} _WINDOWPOS;




// �\��OAM�̎��ԂƂ��̍ő�
#define _CLACT_TIMEPARTS_MAX (9)

#define _CLACT_EDITMARKOFF (7)
#define _CLACT_EDITMARKON (8)

//����Ⴂ�悤
#define _CLACT_CROSS_MAX  (12)

// �^�C�v
#define _CLACT_TYPE_MAX (3)

static const GFL_UI_TP_HITTBL bttndata[] = {  //�㉺���E
  //�^�b�`�p�l���S��
  {	PANEL_Y2 * 8,  PANEL_Y2 * 8 + (PANEL_SIZEXY * 8 * PANEL_HEIGHT2), 0,32*8-1 },
  { 18, 18+16, 208-16, 208+24-16 },
  { 18, 18+16, 40, 40+32 },
  { 160, 160+24, 44, 44+16*_CLACT_CROSS_MAX },
  {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
};




//-------------------------------------------------------------------------
///	�����\���F��`(default)	-> gflib/fntsys.h�ֈړ�
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))



//--------------------------------------------
// �������[�N
//--------------------------------------------


#define GEAR_MAIN_FRAME   (GFL_BG_FRAME2_S)
#define GEAR_BMPWIN_FRAME   (GFL_BG_FRAME1_S)
#define GEAR_BUTTON_FRAME   (GFL_BG_FRAME0_S)
#define GEAR_FB_MESSAGE   (GFL_BG_FRAME3_S)


typedef void (StateFunc)(C_GEAR_WORK* pState);
typedef BOOL (TouchFunc)(int no, C_GEAR_WORK* pState);


struct _C_GEAR_WORK {
  StateFunc* state;      ///< �n���h���̃v���O�������
  TouchFunc* touch;
  int selectType;   // �ڑ��^�C�v
  HEAPID heapID;
  GFL_BUTTON_MAN* pButton;
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF* pStrBuf;
  STRBUF* pStrBufOrg;
  u32 bgchar;  //GFL_ARCUTIL_TRANSINFO
  //    BMPWINFRAME_AREAMANAGER_POS aPos;
  GFL_FONT* pFontHandle;
  GFL_ARCUTIL_TRANSINFO subchar;
  int windowNum;
  BOOL IsIrc;
  FIELD_SUBSCREEN_WORK* subscreen;
  GAMESYS_WORK* pGameSys;
  CGEAR_SAVEDATA* pCGSV;
  u32 objRes[3];  //CLACT���\�[�X

  GFL_CLUNIT *cellUnit;
  GFL_CLWK  *cellSelect[C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT];
  GFL_CLWK  *cellCursor[_CLACT_TIMEPARTS_MAX];
  GFL_CLWK  *cellType[_CLACT_TYPE_MAX];
  GFL_CLWK  *cellCross[_CLACT_CROSS_MAX];
  GFL_CLWK  *cellRadar;
  u8 crossColor[_CLACT_CROSS_MAX]; //����Ⴂ�J���[ -1���Ă���

  GFL_CLWK  *cellMove;

  int msgCountDown;
  u16 palBase[_CGEAR_NET_CHANGEPAL_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 palChange[_CGEAR_NET_CHANGEPAL_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 palTrans[_CGEAR_NET_CHANGEPAL_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 tpx;
  u16 tpy;

  u8 typeAnim[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];

  u8 plt_counter;  //�p���b�g�A�j���J�E���^
  u8 beacon_bit;   //�r�[�R��bit
  u8 touchx;    //�^�b�`���ꂽ�ꏊ
  u8 touchy;    //�^�b�`���ꂽ�ꏊ
  u8 select_counter;  //�I���������̃A�j���J�E���^
  u8 bAction;
  u8 cellMoveCreateCount;
  u8 cellMoveType;
  BOOL bPanelEdit;

  //FieldBeaconMessage�p
  FIELD_BEACON_MSG_DATA *fbmData;
  GFL_BMPWIN *fbmArea;
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(C_GEAR_WORK* pWork,StateFunc* state);
static void _changeStateDebug(C_GEAR_WORK* pWork,StateFunc* state, int line);
static void _buttonWindowCreate(int num,int* pMsgBuff,C_GEAR_WORK* pWork);
static void _modeSelectMenuInit(C_GEAR_WORK* pWork);
static void _modeSelectMenuWait(C_GEAR_WORK* pWork);
static void _gearXY2PanelScreen(int x,int y, int* px, int* py);

static BOOL _modeSelectMenuButtonCallback(int bttnid,C_GEAR_WORK* pWork);

static void _timeAnimation(C_GEAR_WORK* pWork);
static void _typeAnimation(C_GEAR_WORK* pWork);
static void _editMarkONOFF(C_GEAR_WORK* pWork,BOOL bOn);


#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG


//------------------------------------------------------------------------------
/**
 * @brief   �`�b�v��u���ėǂ����ǂ���
 * @retval  �u���Ă����Ƃ�TRUE
 */
//------------------------------------------------------------------------------

static BOOL _isSetChip(int x,int y)
{
  if((x % 2) == 0){  // �����Ȃ�Z����̕�   y��=��y-1���ׂɂȂ�
    if(y < (C_GEAR_PANEL_HEIGHT-1)){
      return TRUE;
    }
  }
  else{
    if(y < (C_GEAR_PANEL_HEIGHT)){
      return TRUE;
    }
  }
  return FALSE;
}



//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(C_GEAR_WORK* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(C_GEAR_WORK* pWork,StateFunc state, int line)
{
  NET_PRINT("cg: %d\n",line);
  _changeState(pWork, state);
}
#endif


//------------------------------------------------------------------------------
/**
 * @brief   �^����ꂽxy������͂̍��W��Ԃ�
 * @param   x,y          �T���ꏊ
 * @param   xbuff,ybuff  �T�����ꏊ���o�b�t�@����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _getHexAround(int x,int y, int* xbuff,int* ybuff)
{
  if((x % 2) == 0){  // �����Ȃ�Z����̕�   y��=��y-1���ׂɂȂ�
    xbuff[0]=x;   ybuff[0]=y-1;
    xbuff[1]=x;   ybuff[1]=y+1;
    xbuff[2]=x+1; ybuff[2]=y;
    xbuff[3]=x+1; ybuff[3]=y-1;
    xbuff[4]=x-1; ybuff[4]=y;
    xbuff[5]=x-1; ybuff[5]=y-1;
  }
  else{   // ��Ȃ璷����̕�   y��=��y+1���ׂɂȂ�
    xbuff[0]=x;   ybuff[0]=y-1;
    xbuff[1]=x;   ybuff[1]=y+1;
    xbuff[2]=x+1; ybuff[2]=y;
    xbuff[3]=x+1; ybuff[3]=y+1;
    xbuff[4]=x-1; ybuff[4]=y;
    xbuff[5]=x-1; ybuff[5]=y+1;
  }
}



static void _selectAnimInit(C_GEAR_WORK* pWork,int x,int y)
{
  GFL_CLWK_DATA cellInitData;
  int scrx,scry,i;

  _gearXY2PanelScreen(x, y, &scrx, &scry);
  cellInitData.pos_x = scrx * 8+16;
  cellInitData.pos_y = scry * 8+16;
  cellInitData.anmseq = NANR_c_gear_obj_CellAnime01;
  cellInitData.softpri = 0;
  cellInitData.bgpri = 0;
  i = x + y * C_GEAR_PANEL_WIDTH;
  pWork->cellSelect[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CLSYS_DEFREND_SUB ,
                                              pWork->heapID );
  GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellSelect[i], TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->cellSelect[i], TRUE );
}


static void _modeSelectAnimWait(C_GEAR_WORK* pWork)
{
  int x;
  int y,i;
  int xbuff[6];
  int ybuff[6];

  pWork->select_counter++;

  if(	pWork->select_counter % 2 != 1){
    return;
  }

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      if(pWork->typeAnim[x][y] == _SELECTANIM_STANDBY)
      {
        _selectAnimInit(pWork, x,y);
        pWork->typeAnim[x][y] = _SELECTANIM_ANIMING;
      }
    }
  }

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      if(pWork->typeAnim[x][y] == _SELECTANIM_ANIMING)
      {
        _getHexAround(x,y,xbuff,ybuff);
        for(i=0;i<6;i++)
        {
          if((xbuff[i] >= 0) && (xbuff[i] < C_GEAR_PANEL_WIDTH))
          {
            int subnum = 1 - xbuff[i] % 2;
            if((ybuff[i] >= 0) && (ybuff[i] < (C_GEAR_PANEL_HEIGHT-subnum)))
            {
              if(pWork->typeAnim[xbuff[i]][ybuff[i]] == _SELECTANIM_NONE){
                pWork->typeAnim[xbuff[i]][ybuff[i]] = _SELECTANIM_STANDBY;
              }
            }
          }
        }
      }
    }
  }

}



//------------------------------------------------------------------------------
/**
 * @brief   �I�����ꂽ�̂ŃA�j�����s��
 * @param   x,y          �T���ꏊ
 * @param   xbuff,ybuff  �T�����ꏊ���o�b�t�@����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeSelectAnimInit(C_GEAR_WORK* pWork)
{
  int x,y;
  CGEAR_PANELTYPE_ENUM type;

  pWork->typeAnim[pWork->touchx][pWork->touchy] = _SELECTANIM_STANDBY;
  type = CGEAR_SV_GetPanelType(pWork->pCGSV,pWork->touchx,pWork->touchy);

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      if(CGEAR_SV_GetPanelType(pWork->pCGSV,x,y) == type){
        pWork->typeAnim[x][y] = _SELECTANIM_STANDBY;
      }
    }
  }
  _modeSelectAnimWait(pWork);
  _CHANGE_STATE(pWork, _modeSelectAnimWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�C�v�ɕϊ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int getTypeToTouchPos(C_GEAR_WORK* pWork,int touchx,int touchy,int *pxp, int* pyp)
{
  int xp,yp;
  int type = CGEAR_PANELTYPE_NONE;

  {  // �M�A�̃^�C�v����
    int ypos[2] = {PANEL_Y1,PANEL_Y2};
    int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};
    touchx = touchx / 8;
    touchy = touchy / 8;
    xp = (touchx - PANEL_X1) / PANEL_SIZEXY;
    if(touchy - ypos[xp % 2] < 0){
      return CGEAR_PANELTYPE_MAX;
    }
    yp = (touchy - ypos[xp % 2]) / PANEL_SIZEXY;

    if((xp < C_GEAR_PANEL_WIDTH) && (yp < yloop[ xp % 2 ]))
    {
      type = CGEAR_SV_GetPanelType(pWork->pCGSV,xp,yp);
    }
  }
  *pxp=xp;
  *pyp=yp;
  return type;
}


//------------------------------------------------------------------------------
/**
 * @brief   �p���X�ɃW�����v����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeSelectJumpPalace(C_GEAR_WORK* pWork)
{

  if(pWork->select_counter==3){
  }



}


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM���E������A�p���b�g�œ_�ł�����
 * @retval  none
 */
//------------------------------------------------------------------------------
#if 1
static void _PanelPaletteChange(C_GEAR_WORK* pWork)
{
  u8 bittype[_CGEAR_NET_CHANGEPAL_MAX]={_CGEAR_NET_BIT_IR,_CGEAR_NET_BIT_WIRELESS,_CGEAR_NET_BIT_WIFI};
  CGEAR_PANELTYPE_ENUM type[_CGEAR_NET_CHANGEPAL_MAX] = {CGEAR_PANELTYPE_IR,CGEAR_PANELTYPE_WIRELESS,CGEAR_PANELTYPE_WIFI};
  //u16 rgbmask[3] = {0x1f,0x3e,0x7c0};

  int i,x,y,pal;
  u8 plt_counter;  //�p���b�g�A�j���J�E���^

  for(i = 0 ; i < _CGEAR_NET_CHANGEPAL_MAX ; i++)
  {
    if((pWork->plt_counter <= 64) && (pWork->beacon_bit & bittype[ i ]))
    {
      for(pal = 0; pal < _CGEAR_NET_CHANGEPAL_NUM; pal++)
      {
        int add,rgb,base;
        int mod;
        pWork->palTrans[i][pal] = 0;
        for(rgb = 0; rgb < 3; rgb++){
          base = (pWork->palBase[i][pal] >> rgb*5) & 0x1f;
          if(!pWork->bAction){
            base = base / 2;
          }
          add = (pWork->palChange[i][pal] >> rgb*5) & 0x1f - base;
          mod = (pWork->plt_counter % 64);
          if(mod >= 32){
            mod = 63 - mod;
          }
          pWork->palTrans[i][pal] += (add * mod / 32 + base)<<(rgb*5);
        }
      }
      DC_FlushRange(pWork->palTrans[i], _CGEAR_NET_CHANGEPAL_NUM*2);
      GXS_LoadBGPltt(pWork->palTrans[i], (16*(i+1) + _CGEAR_NET_CHANGEPAL_POSX) * 2, _CGEAR_NET_CHANGEPAL_NUM*2);
    }
  }
}


static void _PaletteMake(C_GEAR_WORK* pWork,BOOL rmax,BOOL gmax,BOOL bmax,int type)
{
  int y=type,x;
  u16 r,g,b;

  for(x = 0 ; x < _CGEAR_NET_CHANGEPAL_NUM; x++){
    r = pWork->palBase[y][x] & 0x001f;
    g = pWork->palBase[y][x] & 0x03e0;
    b = pWork->palBase[y][x] & 0x7c00;
    if(rmax){
      r=0x001f;
    }
    if(gmax && !rmax && !bmax){
      g=0x01e0;
    }
    else if(gmax){
      g=0x03e0;
    }
    if(bmax){
      b=0x7c00;
    }
    pWork->palChange[y][x]=r+g+b;
  }
}

#endif


static void _PanelPaletteChangeAction(C_GEAR_WORK* pWork)
{
  u8 bittype[_CGEAR_NET_CHANGEPAL_MAX]={_CGEAR_NET_BIT_IR,_CGEAR_NET_BIT_WIRELESS,_CGEAR_NET_BIT_WIFI};
  CGEAR_PANELTYPE_ENUM type[_CGEAR_NET_CHANGEPAL_MAX] = {CGEAR_PANELTYPE_IR,CGEAR_PANELTYPE_WIRELESS,CGEAR_PANELTYPE_WIFI};
  //u16 rgbmask[3] = {0x1f,0x3e,0x7c0};

  int i,x,y,pal;
  u8 plt_counter;  //�p���b�g�A�j���J�E���^

  for(i = 0 ; i < _CGEAR_NET_CHANGEPAL_MAX ; i++)
  {
    {
      for(pal = 0; pal < _CGEAR_NET_CHANGEPAL_NUM; pal++)
      {
        int add,rgb,base;
        int mod;
        pWork->palTrans[i][pal] = 0;
        for(rgb = 0; rgb < 3; rgb++){
          base = (pWork->palBase[i][pal] >> rgb*5) & 0x1f;
          if(!pWork->bAction){
            base = base / 2;
          }
          pWork->palTrans[i][pal] += ( base)<<(rgb*5);
        }
      }
      DC_FlushRange(pWork->palTrans[i], _CGEAR_NET_CHANGEPAL_NUM*2);
      GXS_LoadBGPltt(pWork->palTrans[i], (16*(i+1) + _CGEAR_NET_CHANGEPAL_POSX) * 2, _CGEAR_NET_CHANGEPAL_NUM*2);
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �M�A�������X�N���[���ɕϊ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _gearXY2PanelScreen(int x,int y, int* px, int* py)
{
  int ypos[2] = {PANEL_Y1,PANEL_Y2};
  *px = PANEL_X1 + x * PANEL_SIZEXY;
  *py = ypos[ x % 2 ] + y * PANEL_SIZEXY;
}

//------------------------------------------------------------------------------
/**
 * @brief   �p�l���̐����擾
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _gearPanelTypeNum(C_GEAR_WORK* pWork, CGEAR_PANELTYPE_ENUM type)
{
  int x,y,i;

  if(type == CGEAR_PANELTYPE_BASE){
    return 1;
  }
  i=0;
  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH; x++)
    {
      if(CGEAR_SV_GetPanelType(pWork->pCGSV,x,y)==type)
      {
        i++;
      }
    }
  }
  return i;
}

























//------------------------------------------------------------------------------
/**
 * @brief   �p�l���^�C�v���X�N���[���ɏ�������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _gearPanelBgScreenMake(C_GEAR_WORK* pWork,int xs,int ys, CGEAR_PANELTYPE_ENUM type)
{
  int ypos[2] = {PANEL_Y1,PANEL_Y2};
  int x,y,i,j;
  int typepos[] = {0,0x0c,0x10,0x14,0x18};
  int palpos[] =  {0,0x1000,0x2000,0x3000};
  int palpos2[] =  {0x7000,0x6000,0x5000,0x4000,0x4000,0x4000,0x5000,0x6000,0x7000};
  u16* pScrAddr = GFL_BG_GetScreenBufferAdrs(GEAR_BUTTON_FRAME );
  int xscr;
  int yscr;

  _gearXY2PanelScreen(xs,ys,&xscr,&yscr);
  for(y = yscr, i = 0; i < PANEL_SIZEXY; y++, i++){
    for(x = xscr, j = 0; j < PANEL_SIZEXY; x++, j++){
      if((x >= 0) && (x < 32)){
        int charpos = typepos[type] + i * 0x20 + j;
        int scr = x + (y * 32);
        if(type == CGEAR_PANELTYPE_NONE){
          charpos = 0;
        }
        if(type==CGEAR_PANELTYPE_BASE){
          pScrAddr[scr] = palpos2[xs] + charpos;
        }
        else{
          pScrAddr[scr] = palpos[type] + charpos;
        }
        //		NET_PRINT("x%d y%d  %d \n",x,y,palpos[type] + charpos);
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �M�A�Z�[�u�f�[�^���݂āA�������o���ǂ��ꏊ��Ԃ�
 * @retval  ������Ȃ�������FALSE
 */
//------------------------------------------------------------------------------

static BOOL _gearGetTypeBestPosition(C_GEAR_WORK* pWork,CGEAR_PANELTYPE_ENUM type, int* px, int* py)
{
  int x,y;

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH; x++)
    {
      if(CGEAR_SV_GetPanelType(pWork->pCGSV,x,y) == type)
      {
        _gearXY2PanelScreen(x,y,px,py);
        return TRUE;
      }
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �M�A�̃p�l�����Z�[�u�f�[�^�ɂ��������č��
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _gearPanelBgCreate(C_GEAR_WORK* pWork)
{
  int x , y;
  int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};

  for(x = 0; x < PANEL_WIDTH; x++){   // X��PANEL_WIDTH��
    for(y = 0; y < yloop[ x % 2]; y++){ //Y�� x�́��Q��yloop�̌J��Ԃ�
      _gearPanelBgScreenMake(pWork, x, y, CGEAR_SV_GetPanelType(pWork->pCGSV,x,y));
    }
  }
  GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
}


//------------------------------------------------------------------------------
/**
 * @brief   �M�A��ARC��ǂݍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static u32 _bgpal[]={NARC_c_gear_c_gear_NCLR,NARC_c_gear_c_gear2_NCLR,NARC_c_gear_c_gear_NCLR};
static u32 _bgcgx[]={NARC_c_gear_c_gear_NCGR,NARC_c_gear_c_gear2_NCGR,NARC_c_gear_c_gear_NCGR};


static u32 _objpal[]={NARC_c_gear_c_gear_obj_NCLR,NARC_c_gear_c_gear2_obj_NCLR,NARC_c_gear_c_gear_obj_NCLR};
static u32 _objcgx[]={NARC_c_gear_c_gear_obj_NCGR,NARC_c_gear_c_gear2_obj_NCGR,NARC_c_gear_c_gear_obj_NCGR};

static void _gearArcCreate(C_GEAR_WORK* pWork)
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );
  MYSTATUS* pMy = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(pWork->pGameSys) );
  u32 sex = MyStatus_GetMySex(pMy);
  u32 scrno=0;

  /*
  {
    int i;
    for(i=0;i<22;i++ ){
      OS_TPrintf("xxxx NARC = %x\n",  GFL_ARC_GetDataSizeByHandle(p_handle, i));
    }
  }
     */
  
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, _bgpal[ sex ],
                                    PALTYPE_SUB_BG, 0, 0,  HEAPID_FIELDMAP);


  {
    int x,y;
    u16* loadPtr = GFL_ARC_LoadDataAllocByHandle(  p_handle,
                                                   _bgpal[ sex ],
                                                   GFL_HEAP_LOWID(HEAPID_FIELDMAP) );

    for(y = 0 ; y < _CGEAR_NET_CHANGEPAL_MAX; y++){
      for(x = 0 ; x < _CGEAR_NET_CHANGEPAL_NUM; x++){
        pWork->palBase[y][x ] = loadPtr[20 + 16*(_CGEAR_NET_CHANGEPAL_POSY+y) + _CGEAR_NET_CHANGEPAL_POSX + x];
        //				pWork->palChange[y][x] = loadPtr[20 +16*(_CGEAR_NET_CHANGEPAL_POSY+_CGEAR_NET_CHANGEPAL_MAX+y) + _CGEAR_NET_CHANGEPAL_POSX + x];
      }
    }
    _PaletteMake(pWork,TRUE,TRUE,TRUE,0);
    _PaletteMake(pWork,TRUE,TRUE,TRUE,1);
    _PaletteMake(pWork,TRUE,TRUE,TRUE,2);

    GFL_HEAP_FreeMemory( loadPtr );

  }


  // �T�u���BG�L�����]��
  pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, _bgcgx[sex],
                                                                GEAR_MAIN_FRAME, 0, 0, HEAPID_FIELDMAP);

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
                                         NARC_c_gear_c_gear01_NSCR,
                                         GEAR_MAIN_FRAME, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         HEAPID_FIELDMAP);

  pWork->objRes[_CLACT_PLT] = GFL_CLGRP_PLTT_Register( p_handle ,
                                                       _objpal[sex],
                                                       CLSYS_DRAW_SUB , 0 , HEAPID_FIELDMAP );



  pWork->objRes[_CLACT_CHR] = GFL_CLGRP_CGR_Register( p_handle ,
                                                      _objcgx[sex] ,
                                                      FALSE , CLSYS_DRAW_SUB , HEAPID_FIELDMAP );

  pWork->objRes[_CLACT_ANM] = GFL_CLGRP_CELLANIM_Register( p_handle ,
                                                           NARC_c_gear_c_gear_obj_NCER ,
                                                           NARC_c_gear_c_gear_obj_NANR ,
                                                           pWork->heapID );


  //�p���b�g�A�j���V�X�e���쐬
  //    ConnectBGPalAnm_Init(&pWork->cbp, p_handle, NARC_ircbattle_connect_anm_NCLR, pWork->heapID);
  GFL_ARC_CloseDataHandle( p_handle );

  GFL_NET_ChangeIconPosition(240-22,10);
  GFL_NET_ReloadIcon();

}



static void _gearDecalScreenArcCreate(C_GEAR_WORK* pWork,BOOL bDecal)
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );
  u32 scrno=0;

  if(bDecal){
    scrno = NARC_c_gear_c_gear00_NSCR;
  }
  else{
    scrno = NARC_c_gear_c_gear00_nodecal_NSCR;
  }

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
                                         scrno,
                                         GEAR_BMPWIN_FRAME, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         HEAPID_FIELDMAP);

  GFL_ARC_CloseDataHandle( p_handle );

}


//------------------------------------------------------------------------------
/**
 * @brief   �T�u��ʂ̐ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _createSubBg(C_GEAR_WORK* pWork)
{
  int i = 0;
  for(i = GFL_BG_FRAME0_S;i <= GFL_BG_FRAME3_S ; i++)
  {
    GFL_BG_SetVisible( i, VISIBLE_OFF );
  }
  {
    int frame = GEAR_MAIN_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 3 );
    //  GFL_BG_FillCharacter( frame, 0x00, 1, 0 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GEAR_BMPWIN_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 1 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  {
    int frame = GEAR_BUTTON_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 2 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  G2S_SetBlendAlpha( GEAR_MAIN_FRAME|GEAR_BUTTON_FRAME, GEAR_BMPWIN_FRAME , 9, 15 );

}


//------------------------------------------------------------------------------
/**
 * @brief   �󂯎�������̃E�C���h�E�𓙊Ԋu�ɍ�� ����3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _buttonWindowCreate(int num,int* pMsgBuff,C_GEAR_WORK* pWork)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  C_GEAR_WORK *pWork = p_work;
  u32 friendNo;
  u32 touchx,touchy;
  int xp,yp;
  int type = CGEAR_PANELTYPE_NONE;

  if(!pWork->bAction){
    return;
  }

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:
    switch(bttnid){
    case 0:
      if(GFL_UI_TP_GetPointCont(&touchx,&touchy)){
        pWork->tpx = touchx;
        pWork->tpy = touchy;
      }
      pWork->cellMoveCreateCount = 0;
      break;
    case 1:
      pWork->bPanelEdit = pWork->bPanelEdit ^ 1;
      _editMarkONOFF(pWork, pWork->bPanelEdit);
      break;
    case 2:
      //GAMEBEACON_Set_Congratulations();
      OS_TPrintf("t2\n");
      break;
    case 3:
      FIELD_SUBSCREEN_SetAction(pWork->subscreen, FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW);
      OS_TPrintf("t3\n");
      return;
    }
    break;
  case GFL_BMN_EVENT_HOLD:
    if(pWork->bPanelEdit){
      if(GFL_UI_TP_GetPointCont(&touchx,&touchy)){
        pWork->tpx=touchx;
        pWork->tpy=touchy;
      }
      if(pWork->cellMove){
        GFL_CLACTPOS pos;
        pos.x = pWork->tpx;  // OBJ�\���ׂ̈̕␳�l
        pos.y = pWork->tpy;
        GFL_CLACT_WK_SetPos(pWork->cellMove, &pos, CLSYS_DEFREND_SUB);
      }
      else if(pWork->cellMoveCreateCount > 20){
        GFL_CLWK_DATA cellInitData;
        int type = getTypeToTouchPos(pWork,touchx,touchy,&xp,&yp);
        if(type != CGEAR_PANELTYPE_MAX){
          pWork->cellMoveType = type;
        }
        if(pWork->cellMoveType != CGEAR_PANELTYPE_NONE)
        {

          //�Z���̐���
          cellInitData.pos_x = pWork->tpx;
          cellInitData.pos_y = pWork->tpy;
          cellInitData.anmseq = NANR_c_gear_obj_CellAnime01 + pWork->cellMoveType - 1;
          cellInitData.softpri = 0;
          cellInitData.bgpri = 0;
          pWork->cellMove = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                 pWork->objRes[_CLACT_CHR],
                                                 pWork->objRes[_CLACT_PLT],
                                                 pWork->objRes[_CLACT_ANM],
                                                 &cellInitData ,
                                                 CLSYS_DEFREND_SUB ,
                                                 pWork->heapID );
          GFL_CLACT_WK_SetDrawEnable( pWork->cellMove, TRUE );
          GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellMove, TRUE );
        }
      }
      else{
        pWork->cellMoveCreateCount++;
      }
    }
    break;

  case GFL_BMN_EVENT_RELEASE:		///< �����ꂽ�u��

    touchx=pWork->tpx;
    touchy=pWork->tpy;
    pWork->tpx=0;
    pWork->tpy=0;
    type = getTypeToTouchPos(pWork,touchx,touchy,&xp,&yp);  // �M�A�̃^�C�v����
    if(type == CGEAR_PANELTYPE_MAX){
      return;
    }

    if(pWork->cellMove){
      GFL_CLACT_WK_Remove( pWork->cellMove );
      pWork->cellMove=NULL;

      if((type == CGEAR_PANELTYPE_BASE) ||(  _gearPanelTypeNum(pWork,type) > 1 && _isSetChip(xp,yp)))  //���ɂ���^�C�v����ȏ゠��ꍇ
      {
        type = pWork->cellMoveType;
        CGEAR_SV_SetPanelType(pWork->pCGSV,xp,yp,type);
        _gearPanelBgScreenMake(pWork, xp, yp,type);
        GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
      }
      return;
    }


    if(pWork->bPanelEdit)  ///< �p�l���^�C�v��ύX
    {
      if(((_gearPanelTypeNum(pWork,type) > 1 ) || (type == CGEAR_PANELTYPE_BASE))&& _isSetChip(xp,yp))
      {
        type = (type+1) % CGEAR_PANELTYPE_MAX;
        CGEAR_SV_SetPanelType(pWork->pCGSV,xp,yp,type);
        _gearPanelBgScreenMake(pWork, xp, yp,type);
        GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
      }
    }
    else{    ///< �M�A���j���[��ύX
      pWork->touchx = xp;
      pWork->touchy = yp;

      if(GFL_NET_IsInit()){  //�ʐMON�Ȃ�
        switch(type){
        case CGEAR_PANELTYPE_IR:
          FIELD_SUBSCREEN_SetAction(pWork->subscreen, FIELD_SUBSCREEN_ACTION_IRC);
          break;
        case CGEAR_PANELTYPE_WIRELESS:
          FIELD_SUBSCREEN_SetAction(pWork->subscreen, FIELD_SUBSCREEN_ACTION_WIRELESS);
          break;
        case CGEAR_PANELTYPE_WIFI:
          FIELD_SUBSCREEN_SetAction(pWork->subscreen, FIELD_SUBSCREEN_ACTION_GSYNC);
          break;
        }
      }
    }
    break;

  default:
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �G�f�B�b�g���[�h��ONOFF
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _editMarkONOFF(C_GEAR_WORK* pWork,BOOL bOn)
{
  if(bOn){
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[_CLACT_EDITMARKOFF], FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[_CLACT_EDITMARKON], TRUE );
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[_CLACT_EDITMARKOFF], TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[_CLACT_EDITMARKON], FALSE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   OBJ�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearObjCreate(C_GEAR_WORK* pWork)
{
  int i;
  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

  //�Z���n�V�X�e���̍쐬
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 56+_CLACT_TIMEPARTS_MAX , 0 , pWork->heapID );

  for(i=0;i < _CLACT_TIMEPARTS_MAX ;i++)
  {
    int anmbuff[]=
    {
      NANR_c_gear_obj_CellAnime_ampm,
      NANR_c_gear_obj_CellAnime_NO2,NANR_c_gear_obj_CellAnime_NO10a,
      NANR_c_gear_obj_CellAnime_colon,
      NANR_c_gear_obj_CellAnime_NO6,NANR_c_gear_obj_CellAnime_NO10b,
      NANR_c_gear_obj_CellAnime_batt1,
      NANR_c_gear_obj_CellAnime0,
      NANR_c_gear_obj_CellAnime1
      };
    int xbuff[]=
    {
      32,
      42,
      48,
      52,
      57,
      63,
      178,
      208,
      208,
    };

    GFL_CLWK_DATA cellInitData;
    //�Z���̐���
    cellInitData.pos_x = xbuff[i];
    cellInitData.pos_y = 22;
    cellInitData.anmseq = anmbuff[i];
    if(NANR_c_gear_obj_CellAnime_batt1==cellInitData.anmseq){
      if( OS_IsRunOnTwl() ){//DSI�Ȃ�
        cellInitData.anmseq = NANR_c_gear_obj_CellAnime_batt2;
      }
    }

    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    //�����
    pWork->cellCursor[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                pWork->objRes[_CLACT_CHR],
                                                pWork->objRes[_CLACT_PLT],
                                                pWork->objRes[_CLACT_ANM],
                                                &cellInitData ,
                                                CLSYS_DEFREND_SUB ,
                                                pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[i], TRUE );
  }
  _editMarkONOFF(pWork,FALSE);

  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    int anmbuff[]=
    {
      NANR_c_gear_obj_CellAnime_IR,
      NANR_c_gear_obj_CellAnime_WIRELESS,
      NANR_c_gear_obj_CellAnime_WIFI,
    };

    GFL_CLWK_DATA cellInitData;
    //�Z���̐���

    cellInitData.pos_x = 8;
    cellInitData.pos_y = 8;
    cellInitData.anmseq = anmbuff[i];
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    //�����
    pWork->cellType[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CLSYS_DEFREND_SUB,
                                              pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellType[i], TRUE );
  }
  _timeAnimation(pWork);
  _typeAnimation(pWork);

}




//------------------------------------------------------------------------------
/**
 * @brief   ����ႢOBJ�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjCreate(C_GEAR_WORK* pWork)
{
  int i;

  for(i=0;i < _CLACT_CROSS_MAX ;i++)
  {

    GFL_CLWK_DATA cellInitData;
    //�Z���̐���

    cellInitData.pos_x = 128-(8*_CLACT_CROSS_MAX/2) + 8*i;
    cellInitData.pos_y = 180;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_sure01;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    pWork->cellCross[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                               pWork->objRes[_CLACT_CHR],
                                               pWork->objRes[_CLACT_PLT],
                                               pWork->objRes[_CLACT_ANM],
                                               &cellInitData ,
                                               CLSYS_DEFREND_SUB,
                                               pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCross[i], FALSE );
    //pWork->crossColor[i]=i+1;
  }

  {
    GFL_CLWK_DATA cellInitData;
    //�Z���̐���

    cellInitData.pos_x = 40;
    cellInitData.pos_y = 170;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_radar;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    pWork->cellRadar = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                               pWork->objRes[_CLACT_CHR],
                                               pWork->objRes[_CLACT_PLT],
                                               pWork->objRes[_CLACT_ANM],
                                               &cellInitData ,
                                               CLSYS_DEFREND_SUB,
                                               pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellRadar, TRUE );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ����ႢOBJ�̎��s
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjMain(C_GEAR_WORK* pWork)
{
  const GAMEBEACON_INFO *beacon_info;
  int i;
  
  for(i=0;i < _CLACT_CROSS_MAX ;i++)
  {
    GFL_CLWK* cp_wk =  pWork->cellCross[i];
    GXRgb dest_buf;
    beacon_info = GAMEBEACON_Get_BeaconLog(i);
    if(beacon_info != NULL){
      //      if(pWork->crossColor[i] != 0){
      u8 col;
      GAMEBEACON_Get_FavoriteColor(&dest_buf, beacon_info);
      col = dest_buf; //pWork->crossColor[i] - 1;
      GFL_CLACT_WK_SetDrawEnable( cp_wk, TRUE );
      if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  col){
        GFL_CLACT_WK_SetAnmIndex( cp_wk,col);
      }
      //    }
    }
    else{
      GFL_CLACT_WK_SetDrawEnable( cp_wk, FALSE );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ����Ⴂ�p�̃J���[�Z�b�g
 * @param   color ���C�ɓ���̐F
 * @param   index ���点��ꏊindex
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_SetCrossColor(C_GEAR_WORK* pWork,int color,int index)
{
  GF_ASSERT(index < _CLACT_CROSS_MAX);
  pWork->crossColor[index] = color + 1;
}

//------------------------------------------------------------------------------
/**
 * @brief   ����Ⴂ�p�̃J���[���Z�b�g
 * @param   index ���点��ꏊindex
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_ResetCrossColor(C_GEAR_WORK* pWork,int index)
{
  GF_ASSERT(index < _CLACT_CROSS_MAX);
  pWork->crossColor[index] = 01;
}


//------------------------------------------------------------------------------
/**
 * @brief   ����ႢOBJ�̊J��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjDelete(C_GEAR_WORK* pWork)
{
  int i;

  for(i=0;i < _CLACT_CROSS_MAX ;i++)
  {
    GFL_CLACT_WK_Remove(pWork->cellCross[i]);
    pWork->cellCross[i] = NULL;
  }
    GFL_CLACT_WK_Remove(  pWork->cellRadar);
  pWork->cellRadar=NULL;
  
}



//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g�S�̂̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
//#define _NUKI_FONT_PALNO  (13)

static void _modeInit(C_GEAR_WORK* pWork)
{
  _gearArcCreate(pWork);  //ARC�ǂݍ��� BG&OBJ
  _gearPanelBgCreate(pWork);	// �p�l���쐬

  _gearObjCreate(pWork); //CLACT�ݒ�
  _gearCrossObjCreate(pWork);


  pWork->IsIrc=FALSE;

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );

}

static void _workEnd(C_GEAR_WORK* pWork)
{

  if(pWork->pButton){
    GFL_BMN_Delete(pWork->pButton);
  }
  _gearCrossObjDelete(pWork);

  if(pWork->cellMove){
    GFL_CLACT_WK_Remove( pWork->cellMove );
    pWork->cellMove=NULL;
  }
  {
    int i;
    for(i = 0;i < _CLACT_TIMEPARTS_MAX; i++){
      if(pWork->cellCursor[i]){
        GFL_CLACT_WK_Remove( pWork->cellCursor[i] );
      }
    }
    for(i = 0;i < _CLACT_TYPE_MAX; i++){
      if( pWork->cellType[i]){
        GFL_CLACT_WK_Remove( pWork->cellType[i] );
      }
    }
    for(i = 0;i < C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT; i++){
      if(pWork->cellSelect[i]){
        GFL_CLACT_WK_Remove( pWork->cellSelect[i] );
      }
    }
  }
  GFL_CLACT_UNIT_Delete( pWork->cellUnit );
  GFL_CLGRP_CELLANIM_Release( pWork->objRes[_CLACT_ANM] );
  GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
  GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );


  GFL_BG_FreeCharacterArea(GEAR_MAIN_FRAME,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subchar));

  GFL_BG_FreeBGControl(GEAR_BUTTON_FRAME);
  GFL_BG_FreeBGControl(GEAR_BMPWIN_FRAME);
  GFL_BG_FreeBGControl(GEAR_MAIN_FRAME);


  GFL_BG_SetVisible( GEAR_BUTTON_FRAME, VISIBLE_OFF );
  GFL_BG_SetVisible( GEAR_BMPWIN_FRAME, VISIBLE_OFF );
  GFL_BG_SetVisible( GEAR_MAIN_FRAME, VISIBLE_OFF );

}


//------------------------------------------------------------------------------
/**
 * @brief   ���ԃA�j���[�V����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _timeAnimation(C_GEAR_WORK* pWork)
{
  RTCTime time;
  BOOL battflg = FALSE;  //�o�b�e���[�\���X�V�t���O

  GFL_RTC_GetTime( &time );

  {  //AMPM
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_ampm];
    int num = time.hour / 12;
    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {  //��10
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO2];
    int num = (time.hour % 12) / 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {  //��1
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO10a];
    int num = (time.hour % 12) % 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }

  {  //�����
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_colon];
    int num = time.second % 2;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }


  {//�b�P�O
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO6];
    int num = time.minute / 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {//�b�P
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO10b];
    int num = time.minute % 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
      battflg = TRUE;  //�ꕪ�����Ɍ���
    }
  }
  if(battflg){//BATT
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_batt1];
    if( OS_IsRunOnTwl() ){//DSI�Ȃ�
      u16 buf;
      if( PM_GetBatteryLevel(&buf) == PM_RESULT_SUCCESS )
      {
        if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  buf){
          GFL_CLACT_WK_SetAnmIndex(cp_wk,buf);
        }
      }
    }
    else{
      PMBattery buf;
      if( PM_GetBattery(&buf) == PM_RESULT_SUCCESS )
      {
        int num = (buf==PM_BATTERY_HIGH ? 1:0);
        if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
          GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
        }
      }
    }
  }





}

//------------------------------------------------------------------------------
/**
 * @brief   �^�C�v�̃A�j���[�V���� �ʒu����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _typeAnimation(C_GEAR_WORK* pWork)
{
  int i;

  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    int x,y;
    GFL_CLACTPOS pos;
    _gearGetTypeBestPosition(pWork, CGEAR_PANELTYPE_IR+i, &x, &y);
    x *= 8;
    y *= 8;
    //		GFL_CLACT_WK_GetPos( pWork->cellType[i], &pos , CLSYS_DEFREND_SUB);
    //		if((pos.x != x) || (pos.y != y)){
    pos.x = x+24;  // OBJ�\���ׂ̈̕␳�l
    pos.y = y+6;
    GFL_CLACT_WK_SetPos(pWork->cellType[i], &pos, CLSYS_DEFREND_SUB);
    //		}
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(C_GEAR_WORK* pWork)
{
  GFL_BMN_Main( pWork->pButton );
  _timeAnimation(pWork);
  _typeAnimation(pWork);
  _PanelPaletteChange(pWork);


#if 0
  ///�C���t�H���[�V�������b�Z�[�W�\����(�L���[���擾���鎞�A���̍\���̂ɕϊ����Ď擾����)
  typedef struct{
    STRBUF *name[INFO_WORDSET_MAX];     ///<���g�p�̏ꍇ��NULL�������Ă��܂�
    u8 wordset_no[INFO_WORDSET_MAX];
    u16 message_id;
    u8 padding[2];
  }GAME_COMM_INFO_MESSAGE;
#endif

}

static BOOL _loadExData(C_GEAR_WORK* pWork,GAMESYS_WORK* pGameSys)
{
  BOOL ret=FALSE;

  {
    int i;
    u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_CGEAR_PICTURE);
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pGameSys));


    if(LOAD_RESULT_OK== SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, HEAPID_FIELDMAP,
                                                   pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE)){
      CGEAR_PICTURE_SAVEDATA* pPic = (CGEAR_PICTURE_SAVEDATA*)pCGearWork;

      {
        u16 crc = GFL_STD_CrcCalc( pPic, CGEAR_PICTURTE_CHAR_SIZE+CGEAR_PICTURTE_PAL_SIZE );
        u16 crc2 = CGEAR_SV_GetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave));
        if(crc == crc2){
          if(CGEAR_PICTURE_SAVE_IsPalette(pPic)){
            ret = TRUE;
            for(i=0;i<CGEAR_DECAL_SIZEY;i++){
              GFL_BG_LoadCharacter(GEAR_MAIN_FRAME,&pCGearWork[CGEAR_DECAL_SIZEX * 32 * i],CGEAR_DECAL_SIZEX * 32, (5 + i)*32);
            }
            GFL_BG_LoadPalette(GEAR_MAIN_FRAME,pPic->palette,CGEAR_PICTURTE_PAL_SIZE, 32*0x0a );
          }
        }
#if PM_DEBUG
        else{
          OS_TPrintf("CRCDIFF chk%x pict%x\n",crc,crc2);
        }
#endif
      }
    }
    SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
    GFL_HEAP_FreeMemory(pCGearWork);
  }
  return ret;
}


//------------------------------------------------------------------------------
/**
 * @brief   �X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
C_GEAR_WORK* CGEAR_Init( CGEAR_SAVEDATA* pCGSV,FIELD_SUBSCREEN_WORK* pSub,GAMESYS_WORK* pGameSys )
{
  C_GEAR_WORK *pWork = NULL;
  BOOL ret = FALSE;

  //GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_CGEAR, 0x8000 );

 // OS_TPrintf("zzzz start field_heap = %x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP));  

  pWork = GFL_HEAP_AllocClearMemory( HEAPID_FIELD_SUBSCREEN, sizeof( C_GEAR_WORK ) );
  pWork->heapID = HEAPID_FIELD_SUBSCREEN;
  pWork->pCGSV = pCGSV;
  pWork->subscreen = pSub;
  pWork->pGameSys = pGameSys;
  pWork->bAction = TRUE;

  //	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, _BRIGHTNESS_SYNC);
  _CHANGE_STATE(pWork,_modeSelectMenuWait);


  _createSubBg(pWork);   //BGVRAM�ݒ�
  _modeInit(pWork);
  if(CGEAR_SV_GetCGearPictureONOFF(pWork->pCGSV)){
    ret = _loadExData(pWork,pGameSys);  //�f�J�[���ǂݍ���
  }
  _gearDecalScreenArcCreate(pWork,ret);

//  OS_TPrintf("zzzz start field_heap = %x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP));  
  
  return pWork;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------

u8 PalleteONOFFTbl[][3]={
  {0,0,0},  //NULL
  {1,0,0},  //IR
  {0,1,0},     ///<���C�����X�ʐM �p���X
  {1,1,0},     ///<���C�����X�ʐM �g�����V�[�o�[
  {0,0,1},     ///<���C�����X�ʐM ���j�I��
  {1,0,0},     ///<���C�����X�ʐM �s�v�c
  {0,1,0},   ///<Wi-Fi�ʐM �o�^�ς�
  {0,0,1},   ///<Wi-Fi�ʐM �C�V���]�[�����C�V������
  {1,1,0},  ///<Wi-Fi�ʐM ��������
  {1,0,0},      ///<Wi-Fi�ʐM ��������
};




void CGEAR_Main( C_GEAR_WORK* pWork,BOOL bAction )
{
  StateFunc* state = pWork->state;
  int st;

  if(pWork->bAction != bAction){
    pWork->bAction = bAction;
    _PanelPaletteChangeAction(pWork);
  }
  pWork->bAction = bAction;

  if(state != NULL && GFL_NET_IsInit())
  {
    GAME_COMM_SYS_PTR pGC = GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys);
    {

      pWork->plt_counter++;
      if(pWork->plt_counter>=66)
      {
        pWork->beacon_bit=0;
        pWork->plt_counter=0;
      }
      if(!pWork->beacon_bit){
        u32 bit = WIH_DWC_GetAllBeaconTypeBit();
          if(bit & GAME_COMM_SBIT_IRC_ALL){
            pWork->beacon_bit |= _CGEAR_NET_BIT_IR;
          }
          if(bit & GAME_COMM_SBIT_WIRELESS_ALL){
            pWork->beacon_bit |= _CGEAR_NET_BIT_WIRELESS;
          }
          if(bit & GAME_COMM_SBIT_WIFI_ALL){
            pWork->beacon_bit |= _CGEAR_NET_BIT_WIFI;
          }
        if(bit & GAME_COMM_STATUS_BIT_IRC){
          st = GAME_COMM_STATUS_IRC;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 0);
        }
        if(bit & GAME_COMM_STATUS_BIT_WIFI){
          st = GAME_COMM_STATUS_WIFI;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 2);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIFI_ZONE){
          st = GAME_COMM_STATUS_WIFI_ZONE;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 2);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIFI_FREE){
          st = GAME_COMM_STATUS_WIFI_FREE;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 2);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIFI_LOCK){
          st = GAME_COMM_STATUS_WIFI_LOCK;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 2);
        }
        
        if(bit & GAME_COMM_STATUS_BIT_WIRELESS_TR){
          PMSND_PlaySE( SEQ_SE_SYS_35 );
          st = GAME_COMM_STATUS_WIRELESS_TR;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 1);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIRELESS){
           st = GAME_COMM_STATUS_WIRELESS;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 1);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIRELESS_UN){
          st = GAME_COMM_STATUS_WIRELESS_UN;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 1);
        }
        else if(bit & GAME_COMM_STATUS_BIT_WIRELESS_FU){
          st = GAME_COMM_STATUS_WIRELESS_FU;
          _PaletteMake(pWork, PalleteONOFFTbl[st][0], PalleteONOFFTbl[st][1], PalleteONOFFTbl[st][2], 1);
        }
        if(!(bit & GAME_COMM_STATUS_BIT_WIRELESS_TR)){
          if(PMSND_CheckPlayingSEIdx(SEQ_SE_SYS_35)){
            NET_PRINT("-������-----\n");
            PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_SYS_35) );
          }
        }

      }
    }
    state(pWork);
  }
  _gearCrossObjMain(pWork);
    GFL_CLACT_SYS_Main();

}


//------------------------------------------------------------------------------
/**
 * @brief   CGEAR_ActionCallback
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_ActionCallback( C_GEAR_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno)
{

  switch(actionno){
  case FIELD_SUBSCREEN_ACTION_GSYNC:
    PMSND_PlaySystemSE( SEQ_SE_DECIDE3 );
    _CHANGE_STATE(pWork,_modeSelectAnimInit);
    break;
  case FIELD_SUBSCREEN_ACTION_IRC:
    PMSND_PlaySystemSE( SEQ_SE_DECIDE3 );
    _CHANGE_STATE(pWork,_modeSelectAnimInit);
    break;
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    PMSND_PlaySystemSE( SEQ_SE_DECIDE3 );
    _CHANGE_STATE(pWork,_modeSelectAnimInit);
    break;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_Exit( C_GEAR_WORK* pWork )
{

  GFL_NET_ChangeIconPosition(GFL_WICON_POSX,GFL_WICON_POSY);
  GFL_NET_ReloadIcon();

  _workEnd(pWork);
  G2S_BlendNone();
  GFL_HEAP_FreeMemory(pWork);

}


