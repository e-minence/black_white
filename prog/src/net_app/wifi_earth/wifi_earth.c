//=============================================================================
/**
 *
 *  @file   wifi_earth.c
 *  @brief    �W�I�l�b�g �n���V
 *  @author   watanabe ���� (DP)
 *  @author   takahasi tomoya ����iPL�j
 *  @author   hosaka genya (GS)
 *  @data   2009.02.25
 *
 *  @note ����Œǉ������^�b�`��A�{�^�������͂Ȃ����܂����B
 */
//=============================================================================
#include <wchar.h>
#include "gflib.h"
#include "calctool.h"
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"
#include "font/font.naix"

#include "system/main.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/wipe.h"

#include "print/printsys.h"
#include "print/str_tool.h"
#include "print/wordset.h"
#include "app/app_keycursor.h"
#include "app/app_taskmenu.h"
#include "gamesystem/game_beacon.h"

#include "gamesystem/game_data.h"
#include "gamesystem\msgspeed.h"

#include "savedata/mystatus.h"
#include "savedata/wifihistory.h"

#include "sound/pm_sndsys.h"

#include "net_app/wifi_country.h"
#include "wifi_earth_place.naix"
#include "wifi_earth.naix"
#include "msg/msg_wifi_earth_guide.h"
#include "msg/msg_wifi_place_msg_world.h"

#include "wifi_earth_snd.h"

typedef struct Vec2DS32_tag{
  s32 x;
  s32 y;
}Vec2DS32;

#define MAKING

BOOL  WIFI_LocalAreaExistCheck(int nationID);
//============================================================================================
//  �萔��`
//============================================================================================
#ifdef PM_DEBUG
//#define WIFI_ERATH_DEBUG
#define WIFI_ERATH_DEBUG_ALL_DRAW
#endif

//�n�_�o�^�ő吔
#define WORLD_PLACE_NUM_MAX (0x400) //���������P�O�O�O�����炢�Ƃ�

//���b�Z�[�W�o�b�t�@�T�C�Y
#define EARTH_STRBUF_SIZE (0x400)
#define EARTH_NAME_SIZE   (64)

//�a�f�����p���b�g
#define EARTH_NULL_PALETTE    (0x4753)

//�a�f�R���g���[���萔
#define EARTH_TEXT_PLANE    (GFL_BG_FRAME2_S)
#define EARTH_ICON_PLANE    (GFL_BG_FRAME2_M)
#define EARTH_BACK_S_PLANE    (GFL_BG_FRAME3_S)
#define EARTH_BACK_M_PLANE    (GFL_BG_FRAME3_M)
#define PALSIZE         (2*16)
#define DOTSIZE         (8)

// �p���b�g�]���ʒu
#define EARTH_BACK_PAL      (0) //�S�{��
#define EARTH_SYSFONT_PAL   (4)
#define EARTH_ICONWIN_PAL   (5)
#define EARTH_TALKWIN_PAL   (6)
#define EARTH_MENUWIN_PAL   (7)
#define EARTH_TOUCHFONT_PAL   (8)
#define EARTH_TASKMENU_PAL  (9) // �݂�E��߂郁�j���[�p�p���b�g

// �J���[�w��
#define FBMP_COL_BLACK    (1)
#define FBMP_COL_BLK_SDW  (2)
#define FBMP_COL_WHITE    (15)

// ��b�E�B���h�E�L�����iwindow.h�Q�Ɓj
#define EARTH_TALKWINCHR_SIZ  (TALK_WIN_CGX_SIZ)
//#define EARTH_TALKWINCHR_NUM  (512-EARTH_TALKWINCHR_SIZ)
#define EARTH_TALKWINCHR_NUM  (4)

// ���j���[�E�B���h�E�L�����iwindow.h�Q�Ɓj
#define EARTH_MENUWINCHR_SIZ  (MENU_WIN_CGX_SIZ)
//#define EARTH_MENUWINCHR_NUM  (EARTH_TALKWINCHR_NUM - EARTH_MENUWINCHR_SIZ)
#define EARTH_MENUWINCHR_NUM  (EARTH_TALKWINCHR_NUM + EARTH_TALKWINCHR_SIZ)

// ���b�Z�[�W�E�B���h�E�ifldbmp.h�Q�ƁA�b�f�w�ȊO�t�B�[���h�Ɠ����F�R�����g�͐��l�j
#define EARTH_MSG_WIN_PX    (2)
#define EARTH_MSG_WIN_PY    (19)
#define EARTH_MSG_WIN_SX    (28)//(27)
#define EARTH_MSG_WIN_SY    (4)
#define EARTH_MSG_WIN_PAL   (EARTH_SYSFONT_PAL)
//#define EARTH_MSG_WIN_CGX   (EARTH_MENUWINCHR_NUM-(EARTH_MSG_WIN_SX*EARTH_MSG_WIN_SY))
#define EARTH_MSG_WIN_CGX   (512-(EARTH_MSG_WIN_SX*EARTH_MSG_WIN_SY))

// �͂�/�������E�B���h�E�ifldbmp.h�Q�ƁA�b�f�w�ȊO�t�B�[���h�Ɠ����F�R�����g�͐��l�j
#define EARTH_YESNO_WIN_PX    (24)
#define EARTH_YESNO_WIN_PY    (13)
#define EARTH_YESNO_WIN_SX    (7)
#define EARTH_YESNO_WIN_SY    (4)
#define EARTH_YESNO_WIN_PAL   (EARTH_SYSFONT_PAL)
#define EARTH_YESNO_WIN_CGX   (EARTH_MSG_WIN_CGX-(EARTH_YESNO_WIN_SX*EARTH_YESNO_WIN_SY))

// ���C�����j���[�E�B���h�E
#define EARTH_MENU_WIN_PX   (18)//(19)
#define EARTH_MENU_WIN_PY   (11)
#define EARTH_MENU_WIN_SX   (13)//(12)
#define EARTH_MENU_WIN_SY   (6)
#define EARTH_MENU_WIN_PAL    (EARTH_SYSFONT_PAL)
#define EARTH_MENU_WIN_CGX    (EARTH_MSG_WIN_CGX-(EARTH_MENU_WIN_SX*EARTH_MENU_WIN_SY))

// �����X�g�E�B���h�E
#define EARTH_PLACE_WIN_PX    (3)
#define EARTH_PLACE_WIN_PY    (2)
#define EARTH_PLACE_WIN_SX    (26)
#define EARTH_PLACE_WIN_SY    (13)//(14)
#define EARTH_PLACE_WIN_PAL   (EARTH_SYSFONT_PAL)
#define EARTH_PLACE_WIN_CGX   (EARTH_MSG_WIN_CGX-(EARTH_PLACE_WIN_SX*EARTH_PLACE_WIN_SY))

// ���E�B���h�E
#define EARTH_INFO_WIN_PX   (2)
#define EARTH_INFO_WIN_PY   (1)
#define EARTH_INFO_WIN_SX   (27)
#define EARTH_INFO_WIN_SY   (6)
#define EARTH_INFO_WIN_PAL    (EARTH_SYSFONT_PAL)
#define EARTH_INFO_WIN_CGX    (EARTH_YESNO_WIN_CGX-(EARTH_INFO_WIN_SX*EARTH_INFO_WIN_SY))

// �A�C�R���E�B���h�E
#define EARTH_ICON_WIN_PX   (25)
#define EARTH_ICON_WIN_PY   (21)
#define EARTH_ICON_WIN_SX   (6)
#define EARTH_ICON_WIN_SY   (2)
#define EARTH_ICON_WIN_PAL    (EARTH_TOUCHFONT_PAL)
//#define EARTH_ICON_WIN_CGX    (EARTH_MENUWINCHR_NUM-(EARTH_ICON_WIN_SX*EARTH_ICON_WIN_SY))
#define EARTH_ICON_WIN_CGX    (512-(EARTH_ICON_WIN_SX*EARTH_ICON_WIN_SY))

// �u�݂�v�E�B���h�E
#define EARTH_LOOK_WIN_PX   (1)
#define EARTH_LOOK_WIN_PY   (21)
#define EARTH_LOOK_WIN_SX   (6)
#define EARTH_LOOK_WIN_SY   (2)
#define EARTH_LOOK_WIN_PAL    (EARTH_TOUCHFONT_PAL)
#define EARTH_LOOK_WIN_CGX    (EARTH_ICON_WIN_CGX - EARTH_LOOK_WIN_SX * EARTH_LOOK_WIN_SY )

//�n�����f����������`
#define INIT_EARTH_TRANS_XVAL (0)
#define INIT_EARTH_TRANS_YVAL (0)
#define INIT_EARTH_TRANS_ZVAL (0)
#define INIT_EARTH_SCALE_XVAL (FX32_ONE)
#define INIT_EARTH_SCALE_YVAL (FX32_ONE)
#define INIT_EARTH_SCALE_ZVAL (FX32_ONE)
#define INIT_EARTH_ROTATE_XVAL  (0x1980)  //�������f�t�H���g�ʒu�ɂ���
#define INIT_EARTH_ROTATE_YVAL  (0xcfe0)  //�������f�t�H���g�ʒu�ɂ���
#define INIT_EARTH_ROTATE_ZVAL  (0)

//�J������������`
#define INIT_CAMERA_TARGET_XVAL (0)
#define INIT_CAMERA_TARGET_YVAL (0)
#define INIT_CAMERA_TARGET_ZVAL (0)
#define INIT_CAMERA_POS_XVAL  (0)
#define INIT_CAMERA_POS_YVAL  (0)
#define INIT_CAMERA_POS_ZVAL  (0x128000)
#define INIT_CAMERA_DISTANCE_NEAR (0x050000)
#define INIT_CAMERA_DISTANCE_FAR  (0x128000)
#define INIT_CAMERA_PERSPWAY  (0x05c1)
#define INIT_CAMERA_CLIP_NEAR (0)
#define INIT_CAMERA_CLIP_FAR  (FX32_ONE*100)
#define CAMERA_ANGLE_MIN    (-0x4000+0x200)
#define CAMERA_ANGLE_MAX    (0x4000-0x200)

//���C�g��������`
#define USE_LIGHT_NUM     (0)
#define LIGHT_VECDEF      (FX32_ONE-1)
#define INIT_LIGHT_ANGLE_XVAL (0)
#define INIT_LIGHT_ANGLE_YVAL (0)
#define INIT_LIGHT_ANGLE_ZVAL (-LIGHT_VECDEF)

//���쏈���p�萔
#define CAMERA_MOVE_SPEED_NEAR  (0x20)  //�J�����ߋ������㉺���E�ړ��X�s�[�h  
#define CAMERA_MOVE_SPEED_FAR (0x200) //�J�������������㉺���E�ړ��X�s�[�h
#define CAMERA_INOUT_SPEED  (0x8000)  //�J�������߈ړ��X�s�[�h
#define MARK_SCALE_INCDEC (0x80)    //�n�_�}�[�N���߈ړ����g�k�䗦
#define EARTH_LIMIT_ROTATE_XMIN (0x1300)  //���{�g��n�}�̏c��]���~�b�g�����i����j
#define EARTH_LIMIT_ROTATE_XMAX (0x2020)  //���{�g��n�}�̏c��]���~�b�g����i�k�C���t���j
#define EARTH_LIMIT_ROTATE_YMIN (0xcc80)  //���{�g��n�}�̉���]���~�b�g�����i�k�C�����H�j
#define EARTH_LIMIT_ROTATE_YMAX (0xd820)  //���{�g��n�}�̉���]���~�b�g����i����j

// �^�X�N���j���[�w��p
#define TASKMENU_MIRU     ( 0 )   // �u�݂�v
#define TASKMENU_YAMERU   ( 1 )   // �u��߂�v

//���C���֐��V�[�P���X�i���o�[��`
enum{
  EARTHDEMO_SEQ_MODELLOAD = 0,          //���f���f�[�^���[�h
  EARTHDEMO_SEQ_DISPON,             //�`��n�m
  EARTHDEMO_SEQ_WELCOME_MSG,            //�u�悤�����v
  EARTHDEMO_SEQ_MAINMENU,             //���C�����j���[�ݒ�
  EARTHDEMO_SEQ_MAINMENU_SELECT,          //���C�����j���[�I�����[�h
  EARTHDEMO_SEQ_REGISTRATIONMENU,         //�o�^���j���[�ݒ�
  EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT,      //�o�^���j���[�I�����[�h
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION,      //���ʓo�^���X�g�ݒ�
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2,     // 2010.2.24 add by tetsu
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT, //���ʓo�^���X�g�I�����[�h
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA,      //�n��ʓo�^���X�g�ݒ�
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2,     // 2010.2.24 add by tetsu
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT,   //�n��ʓo�^���X�g�I�����[�h
  EARTHDEMO_SEQ_FINAL_REGISTRATION,       //�o�^�ŏI�m�F�ݒ�
  EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO,     //�o�^�ŏI�m�F
  EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT,    //�o�^�ŏI�m�F�I�����[�h
  EARTHDEMO_SEQ_EARTH_DISPON,           //�R�c�n���V���[�h�n�m
  EARTHDEMO_SEQ_MOVE_EARTH,           //���C������
  EARTHDEMO_SEQ_MOVE_CAMERA,            //�J�������߈ړ�
  EARTHDEMO_SEQ_BUTTON_ANIME,       // �{�^���A�j���҂�
  EARTHDEMO_SEQ_END,                //�I�������J�n
  EARTHDEMO_SEQ_EXIT,               //�I��
};

//���b�Z�[�W�֐��V�[�P���X�i���o�[��`
enum{
  MSGSET = 0,
  MSGDRAW,
  MSGWAIT,
};

//���b�Z�[�W�֐�����w���`
enum {
  A_BUTTON_WAIT = 0,
  A_BUTTON_NOWAIT,
};

//�o�^���j���[�h�c�i���o�[��`
enum{
  MENU_WORLD = 0,
  MENU_REGISTRATION,
  MENU_END,
};

//�n����]���[�h��`
enum {
  JAPAN_MODE = 0,
  GLOBAL_MODE,
};

//�J���������t���O��`
enum {
  CAMERA_FAR = 0,
  CAMERA_NEAR,
};

//�R�c�`��t���O��`
enum {
  DRAW3D_DISABLE = 0,
  DRAW3D_ENABLE,
  DRAW3D_BANISH,
};

//�n�_�f�[�^�\���J���[��`�i���f���I���ɂ���ĐF�ւ��Fwifihistory��`�ɉ������Ԃɂ���j
enum {
  MARK_NULL = 0,  //���ʐM�ꏊ
  MARK_BLUE,    //�{���͂��߂ĒʐM����
  MARK_YELLOW,  //�ʐM��
  MARK_RED,   //�����̓o�^�ꏊ
  MARK_GREEN,   //�J�[�\��

  MARK_ARRAY_MAX,
};


//============================================================================================
//  �\���̒�`
//============================================================================================
//�f�[�^�ݒ�\���́i���o�C�i���f�[�^�j
typedef struct EARTH_DATA_NATION_tag
{
  u16 flag;
  s16 x;
  s16 y;
}EARTH_DATA_NATION;

//�f�[�^�ݒ�\���́i�n��o�C�i���f�[�^�j
typedef struct EARTH_DATA_AREA_tag
{
  s16 x;
  s16 y;
}EARTH_DATA_AREA;

//�f�[�^�\���́i���n��e�[�u���f�[�^�j
typedef struct EARTH_AREATABLE_tag
{
  u16   nationID;
  u16   msg_arcID;
  const char* place_dataID;
}EARTH_AREATABLE;

//���X�g�f�[�^�\���́i�o�C�i���f�[�^�����ۂɎg�p���郊�X�g�p�f�[�^�ɕϊ��j
typedef struct EARTH_DEMO_LIST_DATA_tag
{
  s16 x;
  s16 y;
  MtxFx33 rotate; //���Ή�]�s������炩���ߌv�Z���ĕۑ�
  u16 col;
  u16 nationID;
  u16 areaID;
}EARTH_DEMO_LIST_DATA;

//���X�g�f�[�^�\���́i�o�C�i���f�[�^�����ۂɎg�p���郊�X�g�p�f�[�^�ɕϊ��j
typedef struct EARTH_DEMO_LIST_tag
{
  u32 listcount;
  EARTH_DEMO_LIST_DATA  place[WORLD_PLACE_NUM_MAX];
}EARTH_DEMO_LIST;

//�����F�ݒ�\����
typedef struct {
  u8 l;
  u8 s;
  u8 b;
}PRINTLSB;

//���[�N�\����
typedef struct EARTH_DEMO_WORK_tag
{
  //�q�[�v�ݒ胏�[�N
  int heapID;

  //�Z�[�u�f�[�^�|�C���^
  GAMEDATA*           gamedata;
  SAVE_CONTROL_WORK*  scw;
  MYSTATUS*           mystatus;
  WIFI_HISTORY*       wifi_sv;

  //�n�_���X�g���[�N
  EARTH_DEMO_LIST placelist;

  //�a�f�V�X�e���|�C���^
  //GFL_BG_INI*     bgl;
  GFL_BMPWIN*   msgwin;
  GFL_BMPWIN*   listwin;
  GFL_BMPWIN*   iconwin;  ///< ��߂�{�^��
  GFL_BMPWIN*   lookwin;  ///< �݂�{�^�� @ gs�ǉ�
  GFL_BMPWIN*   infowin;
  BMPMENULIST_WORK*   bmplist;
  BMP_MENULIST_DATA*  bmplistdata;
  BMPMENU_WORK*       yesnowin;
  APP_TASKMENU_RES      *TaskMenuRes;     // ���j���[���\�[�X
  APP_TASKMENU_WIN_WORK *TaskMenuWork[2]; // �u�݂�v�u��߂�v���j���[
  STRBUF *menustr[2];

  //������R���g���[��
  GFL_FONT*       fontHandle;
  GFL_MSGDATA*    msg_man;
  PRINT_STREAM*   printStream;
  PRINT_UTIL      printUtil;
  PRINT_QUE*      printQue;
  PRINTLSB        printLSB;
  APP_KEYCURSOR_WORK *printCursor;
  APP_TASKMENU_ITEMWORK menuitem[2];
  GFL_TCBLSYS*    tcbl;
  WORDSET*        wordset;
  STRBUF*         msgstr;
  int             msgseq;

  //���f�����[�h���[�N
  NNSG3dRenderObj     renderobj;      //�I�����v�q�[�v�J��
  NNSG3dResMdl*     resmodel;
  NNSG3dResFileHeader*  resfileheader;

  NNSG3dRenderObj     mark_renderobj[5];  //�I�����v�q�[�v�J��
  NNSG3dResMdl*     mark_resmodel[5];
  NNSG3dResFileHeader*  mark_resfileheader[5];

  //���f���ݒ胏�[�N
  VecFx32 trans;
  VecFx32 scale;
  VecFx32 rotate;

  VecFx32 mark_scale;

  //�J�����X�e�[�^�X
  u16 camera_status;

  //���쏈�����[�N
  int   Draw3Dsw;
  BOOL  fade_end_flag;
  BOOL  Japan_ROM_mode;
  u16   earth_mode;
  u16   rotate_speed;

  //�^�b�`�p�l�����o�ۑ����[�N
  int tp_result;
  int tp_seq;
  int tp_x;
  int tp_y;
  int tp_lenx;
  int tp_leny;
  int tp_count;

  //�n�_�o�^�e���|����
  int   my_nation;
  int   my_area;
  int   my_nation_tmp;
  int   my_area_tmp;
  BOOL  my_worldopen_flag;

  int   info_mode;

#ifdef WIFI_ERATH_DEBUG
  int debug_work[8];
#endif  

  //���͏��
  int   trg;
  int   cont;
  BOOL  tptrg;
  BOOL  tpcont;
  u32   tpx;
  u32   tpy;

  //3D�C���^�[�t�F�[�X
  GFL_G3D_CAMERA*     g3Dcamera;
  GFL_G3D_LIGHTSET*   g3Dlightset;

  GFL_G3D_RES*    g3DresEarth;
  GFL_G3D_RES*    g3DresMark[MARK_ARRAY_MAX];
  GFL_G3D_RND*    g3DrndEarth;
  GFL_G3D_RND*    g3DrndMark[MARK_ARRAY_MAX];
  GFL_G3D_OBJ*    g3DobjEarth;
  GFL_G3D_OBJ*    g3DobjMark[MARK_ARRAY_MAX];

}EARTH_DEMO_WORK;

//�r�b�g�}�b�v���X�g�\����
typedef struct EARTH_BMPLIST_tag
{
  u32 msgID;
  u32 retID;
}EARTH_BMPLIST;

typedef struct {
  u8 frmNum;
  u8 px;
  u8 py;
  u8 sx;
  u8 sy;
  u8 palIdx;
  u8 cgxIdx;
}BMPWIN_DAT;

//============================================================================================
//  �O���[�o���֐���`
//============================================================================================
void  WIFI_RegistratonInit( GAMEDATA* gamedata );
BOOL  WIFI_NationAreaNameGet(int nationID,int areaID,
                STRBUF* nation_str,STRBUF* area_str,int heapID);
BOOL  WIFI_LocalAreaExistCheck(int nationID);

//============================================================================================
//  ���[�J���֐���`
//============================================================================================
static void Earth_VramBankSet(void);
static void Earth_BGsysSet(HEAPID heapID);
static void Earth_BGsysRelease( void );
static void Earth_BGdataLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle );
static void Earth_BGdataRelease( EARTH_DEMO_WORK * wk );

static void EarthListLoad( EARTH_DEMO_WORK * wk );
static void EarthListSet( EARTH_DEMO_WORK * wk,u32 index, s16 x,s16 y,u16 nationID,u16 areaID );
static void EarthList_NationAreaListSet( EARTH_DEMO_WORK * wk );
static int  EarthAreaTableGet(int nationID);

static void Earth_TouchPanel( EARTH_DEMO_WORK * wk );
static void Earth_TouchPanelParamGet( EARTH_DEMO_WORK * wk,
  int prevx,int prevy,int* dirx_p,int* lenx_p,int* diry_p,int* leny_p );

static BOOL Earth_StrPrint
( EARTH_DEMO_WORK * wk, GFL_BMPWIN* bmpwin, const STRBUF* strbuf, u8 x, u8 y );
static BOOL Earth_MsgPrint( EARTH_DEMO_WORK * wk,u32 msgID,int button_mode );
static void Earth_BmpListAdd( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                              const BMPMENULIST_HEADER* listheader,const EARTH_BMPLIST* list);
static void Earth_BmpListAddGmmAll( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID);
static void Earth_BmpListDel( EARTH_DEMO_WORK* wk );

static void Earth_MyPlaceInfoWinSet( EARTH_DEMO_WORK* wk );
static void Earth_MyPlaceInfoWinSet2( EARTH_DEMO_WORK* wk, int nation, int area );
static void Earth_MyPlaceInfoWinRelease( EARTH_DEMO_WORK* wk );
static void Earth_PosInfoPut( EARTH_DEMO_WORK* wk );

static void Earth_ModelLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle );
static void Earth_ModelRelease( EARTH_DEMO_WORK * wk );
static void EarthDataInit( EARTH_DEMO_WORK * wk );
static void EarthCameraInit( EARTH_DEMO_WORK * wk );
static void EarthLightInit( EARTH_DEMO_WORK * wk );
static void EarthCameraStart( EARTH_DEMO_WORK * wk );

static BOOL Earth3D_Control( EARTH_DEMO_WORK * wk,int keytrg,int keycont );
static BOOL Earth3D_CameraMoveNearFar( EARTH_DEMO_WORK * wk );
static void Earth3D_Draw( EARTH_DEMO_WORK * wk );
static void EarthVecFx32_to_MtxFx33( MtxFx33* dst, VecFx32* src );
static void EarthVecFx32_to_MtxFx33_place( MtxFx33* dst, VecFx32* src );

static void WIFI_MinusRotateChange( Vec2DS32* p_rot );
static u32 WIFI_EarthGetRotateDist( const Vec2DS32* cp_earth, const Vec2DS32* cp_place );
static void Earth_TaskMenuInit( EARTH_DEMO_WORK *wk );
static void TaskMenuOff( EARTH_DEMO_WORK *wk, int id );
static void TaskMenuPut( EARTH_DEMO_WORK *wk, int id );

static BOOL _menu_anime_check( EARTH_DEMO_WORK *wk );
static void _menu_main( EARTH_DEMO_WORK *wk );
static void _menu_anime_on( EARTH_DEMO_WORK *wk, int id );
static GFL_PROC_RESULT SubSeq_Main( EARTH_DEMO_WORK *wk, int *seq );


#ifdef WIFI_ERATH_DEBUG
static void EarthDebugWinRotateInfoWrite( EARTH_DEMO_WORK * wk );
static void EarthDebugWinNameInfoWrite( EARTH_DEMO_WORK * wk );
static void EarthDebugNationMarkSet( EARTH_DEMO_WORK * wk );
static void EarthDebugAreaMarkSet( EARTH_DEMO_WORK * wk );
#endif  

//============================================================================================
//  �O���֐���`
//============================================================================================
//extern void Main_SetNextProc(FSOverlayID ov_id, const PROC_DATA * proc_data);
extern u8 MsgSpeedGet(void);

//============================================================================================
//  �o�q�n�b��`�f�[�^
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Earth_Demo_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Earth_Demo_Exit(GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA Earth_Demo_proc_data = {
  Earth_Demo_Init,
  Earth_Demo_Main,
  Earth_Demo_Exit,
};

//============================================================================================
//  �a�f��`�f�[�^
//============================================================================================
static const GFL_BG_BGCNT_HEADER Earth_Demo_BGtxt_header = {
  0,0,          //�����\���w�x�ݒ�
  0x800,0,        //�X�N���[���o�b�t�@�T�C�Y(0 = �g�p���Ȃ�)���I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,  //�X�N���[���T�C�Y
  GX_BG_COLORMODE_16,   //�J���[���[�h
  GX_BG_SCRBASE_0x7000,   //�X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x00000, //�L�����N�^�x�[�X�u���b�N 
  GFL_BG_CHRSIZ_128x256,  //�L�����N�^�f�[�^�T�C�Y
  GX_BG_EXTPLTT_01,   //�g���p���b�g�I��(�g�p���Ȃ��ݒ�̂Ƃ��͖��������)
  0,            //�\���v���C�I���e�B�[
  0,            //�G���A�I�[�o�[�t���O
  0,            //
  FALSE         //���U�C�N�ݒ�
};

static const GFL_BG_BGCNT_HEADER Earth_Demo_Back_header = {
  0,0,          //�����\���w�x�ݒ�
  0x800,0,        //�X�N���[���o�b�t�@�T�C�Y(0 = �g�p���Ȃ�)���I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,  //�X�N���[���T�C�Y
  GX_BG_COLORMODE_16,   //�J���[���[�h
  GX_BG_SCRBASE_0x7800,   //�X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x04000, //�L�����N�^�x�[�X�u���b�N 
  GFL_BG_CHRSIZ_128x256,  //�L�����N�^�f�[�^�T�C�Y
  GX_BG_EXTPLTT_01,   //�g���p���b�g�I��(�g�p���Ȃ��ݒ�̂Ƃ��͖��������)
  3,            //�\���v���C�I���e�B�[
  0,            //�G���A�I�[�o�[�t���O
  0,            //
  FALSE         //���U�C�N�ݒ�
};

static const BMPWIN_DAT EarthMsgWinData =
{
  EARTH_TEXT_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_MSG_WIN_PX,EARTH_MSG_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_MSG_WIN_SX,EARTH_MSG_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_MSG_WIN_PAL,  //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_MSG_WIN_CGX,  //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const BMPWIN_YESNO_DAT EarthYesNoWinData =
{
  EARTH_TEXT_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_YESNO_WIN_PX,EARTH_YESNO_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  //EARTH_YESNO_WIN_SX,EARTH_YESNO_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_YESNO_WIN_PAL,  //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_YESNO_WIN_CGX,  //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const BMPWIN_DAT EarthMenuWinData =
{
  EARTH_TEXT_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_MENU_WIN_PX,EARTH_MENU_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_MENU_WIN_SX,EARTH_MENU_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_MENU_WIN_PAL, //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_MENU_WIN_CGX, //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const BMPWIN_DAT EarthPlaceListWinData =
{
  EARTH_TEXT_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_PLACE_WIN_PX,EARTH_PLACE_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_PLACE_WIN_SX,EARTH_PLACE_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_PLACE_WIN_PAL,  //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_PLACE_WIN_CGX,  //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const BMPWIN_DAT EarthInfoWinData =
{
  EARTH_TEXT_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_INFO_WIN_PX,EARTH_INFO_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_INFO_WIN_SX,EARTH_INFO_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_INFO_WIN_PAL, //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_INFO_WIN_CGX, //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const BMPWIN_DAT EarthIconWinData =
{
  EARTH_ICON_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_ICON_WIN_PX,EARTH_ICON_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_ICON_WIN_SX,EARTH_ICON_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_ICON_WIN_PAL, //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_ICON_WIN_CGX, //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

// �݂�{�^�� @ GS�ǉ�
static const BMPWIN_DAT EarthLookWinData =
{
  EARTH_ICON_PLANE, //�E�C���h�E�g�p�t���[��
  EARTH_LOOK_WIN_PX,EARTH_LOOK_WIN_PY,//�E�C���h�E�̈�̍���XY���W�i�L�����P�ʎw��j
  EARTH_LOOK_WIN_SX,EARTH_LOOK_WIN_SY,//�E�C���h�E�̈��XY�T�C�Y�i�L�����P�ʎw��j
  EARTH_LOOK_WIN_PAL, //�E�C���h�E�̈�̃p���b�g�i���o�[ 
  EARTH_LOOK_WIN_CGX, //�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};

static const EARTH_BMPLIST MenuList[] =
{
  {mes_earth_02_05,MENU_WORLD},
  {mes_earth_02_06,MENU_REGISTRATION},
  {mes_earth_02_07,MENU_END},
};

static const BMPMENULIST_HEADER MenuListHeader = {
  /* �\�������f�[�^�|�C���^(�v���O�����ݒ�) */NULL,/* �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐� */NULL,
  /* ���\�����Ƃ̃R�[���o�b�N�֐� */NULL,/* �Ή��r�b�g�}�b�v(�v���O�����ݒ�) */NULL,
  /* ���X�g���ڐ� */NELEMS(MenuList),/* �\���ő區�ڐ� */NELEMS(MenuList),    
  /* ���x���\���w���W */0,/* ���ڕ\���w���W */12,/* �J�[�\���\���w���W */0,/* �\���x���W */0,
  /* �����F */FBMP_COL_BLACK,/* �w�i�F */FBMP_COL_WHITE,/* �����e�F */FBMP_COL_BLK_SDW,   
  /* �����Ԋu�w */0,/* �����Ԋu�x */16,/* �y�[�W�X�L�b�v�^�C�v */BMPMENULIST_NO_SKIP,   
  /* �����w�� */0,/*�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)*/0,           
  NULL,
  /* �����T�C�YX(�h�b�g) */0,/* �����T�C�YY(�h�b�g) */0,
  /* �\���Ɏg�p���郁�b�Z�[�W�o�b�t�@ */ NULL,
  /* �\���Ɏg�p����v�����g���[�e�B���e�B */NULL,
  /* �\���Ɏg�p����v�����g�L���[ */NULL,
  /* �\���Ɏg�p����t�H���g�n���h�� */NULL,
};

static const BMPMENULIST_HEADER PlaceListHeader = {
  /* �\�������f�[�^�|�C���^(�v���O�����ݒ�) */NULL,/* �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐� */NULL,
  /* ���\�����Ƃ̃R�[���o�b�N�֐� */NULL,/* �Ή��r�b�g�}�b�v(�v���O�����ݒ�) */NULL,
  /* ���X�g���ڐ�(�v���O�����ݒ�) */0,/* �\���ő區�ڐ� */7,
  /* ���x���\���w���W */0,/* ���ڕ\���w���W */12,/* �J�[�\���\���w���W */0,/* �\���x���W */0,
  /* �����F */FBMP_COL_BLACK,/* �w�i�F */FBMP_COL_WHITE,/* �����e�F */FBMP_COL_BLK_SDW,   
  /* �����Ԋu�w */0,/* �����Ԋu�x */16,/* �y�[�W�X�L�b�v�^�C�v */BMPMENULIST_LRKEY_SKIP,    
  /* �����w�� */0,/*�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)*/0,           
  NULL,
  /* �����T�C�YX(�h�b�g) */0,/* �����T�C�YY(�h�b�g) */0,
  /* �\���Ɏg�p���郁�b�Z�[�W�o�b�t�@ */ NULL,
  /* �\���Ɏg�p����v�����g���[�e�B���e�B */NULL,
  /* �\���Ɏg�p����v�����g�L���[ */NULL,
  /* �\���Ɏg�p����t�H���g�n���h�� */NULL,
};

//============================================================================================
//
//  ���C���֐�
//
//============================================================================================
//============================================================================================
//  �������v���Z�X
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk;
  HEAPID heapID = HEAPID_WIFIEARTH;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x80000 );

  //���[�N�G���A�擾
  wk = GFL_PROC_AllocWork(proc, sizeof(EARTH_DEMO_WORK), heapID);
  GFL_STD_MemClear(wk, sizeof(EARTH_DEMO_WORK));
  wk->heapID = heapID;

  //�Z�[�u�f�[�^�|�C���^�쐬
  wk->gamedata = (GAMEDATA*)pwk;
  wk->scw = GAMEDATA_GetSaveControlWork( wk->gamedata );
  wk->mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
  wk->wifi_sv = SaveData_GetWifiHistory( wk->scw );

  //���{��ł��ǂ����̔��ʃt���O
  if( CasetteLanguage == LANG_JAPAN ){
    // ���{��o�[�W�����ł͓��{�����o�^�ł��Ȃ�
    wk->Japan_ROM_mode = TRUE;
  } else {
    // ���{��o�[�W�����ȊO�͍ŏ�����n���V���S��������
    wk->Japan_ROM_mode = FALSE;
  }
#ifdef PM_DEBUG
  // �k�{�^���������Ă���Ɠ��͏����N���A������Ő��E������͂ł���
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
    WIFI_RegistratonInit( wk->gamedata );
    wk->Japan_ROM_mode = FALSE;
  }
#endif
  //�X�e�[�^�X�擾
  wk->my_nation = MyStatus_GetMyNation( wk->mystatus );
  wk->my_area = MyStatus_GetMyArea( wk->mystatus );
  wk->my_worldopen_flag = WIFIHISTORY_GetWorldFlag( wk->wifi_sv );
  wk->TaskMenuWork[0]   = NULL;
  wk->TaskMenuWork[1]   = NULL;

  //�u�q�`�l�ݒ�
  Earth_VramBankSet();
  //�a�f���C�u�����ݒ�
  Earth_BGsysSet( wk->heapID );
  //���b�Z�[�W�\���V�X�e��������
  wk->wordset = WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, EARTH_NAME_SIZE, wk->heapID );

  //�R�c�`��X�C�b�`�ݒ�
  wk->Draw3Dsw = DRAW3D_DISABLE;
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);

  //�^�X�N�V�X�e���ݒ�
  wk->tcbl = GFL_TCBL_Init(wk->heapID, wk->heapID, 32, 32);
  wk->printCursor = APP_KEYCURSOR_Create( 15, TRUE, FALSE, wk->heapID );
  wk->printQue = PRINTSYS_QUE_Create(wk->heapID);


  //�L�[�V�X�e���ݒ�
  //GFL_UI_KEY_SetRepeatSpeed( 8/*SYS_KEYREPEAT_SPEED_DEF*/, 4/*SYS_KEYREPEAT_WAIT_DEF*/ );

#if PL_T0864_080714_FIX
  // ���b�Z�[�W�\���֌W��ݒ�
  MsgPrintSkipFlagSet(MSG_SKIP_ON);
  MsgPrintAutoFlagSet(MSG_AUTO_OFF);
  MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
#endif

  //�n�惊�X�g�f�[�^�쐬
  EarthListLoad(wk);
  
  return  GFL_PROC_RES_FINISH;
}

static BOOL Earth_SearchPosInfo( EARTH_DEMO_WORK* wk, u32* outIndex );
//============================================================================================
//  ���C���v���Z�X
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk  = (EARTH_DEMO_WORK*)mywk;
  GFL_PROC_RESULT sys_result = GFL_PROC_RES_CONTINUE;

  // �^�X�N���C��
  GFL_TCBL_Main(wk->tcbl);

  // ���j���[�̃A�j�����s���s���Ă��Ȃ�������T�u�V�[�P���X���s�\
  sys_result = SubSeq_Main( wk, seq );
  Earth3D_Draw(wk);   //�R�c�`��G���W��
  _menu_main(wk);     // �^�X�N���j���[�`��
  PRINTSYS_QUE_Main( wk->printQue );
  return  sys_result;
}




//----------------------------------------------------------------------------------
/**
 * @brief �{�^������A�j��
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void _menu_anime_on( EARTH_DEMO_WORK *wk, int id )
{
  // �{�^�������݂��Ă���Ȃ�{�^������A�j������
  if(wk->TaskMenuWork[id]!=NULL){
    if(id==TASKMENU_MIRU){
      APP_TASKMENU_WIN_SetActive( wk->TaskMenuWork[TASKMENU_MIRU], TRUE );
    }else if(id==TASKMENU_YAMERU){
      APP_TASKMENU_WIN_SetDecide( wk->TaskMenuWork[TASKMENU_YAMERU], TRUE );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[���C������
 *
 * @param   wk    
 *
 * @retval  static    
 */
//----------------------------------------------------------------------------------
static void _menu_main( EARTH_DEMO_WORK *wk )
{
  if(wk->TaskMenuWork[0]!=NULL){
    APP_TASKMENU_WIN_Update( wk->TaskMenuWork[0] );
  }
  if(wk->TaskMenuWork[1]!=NULL){
    APP_TASKMENU_WIN_Update( wk->TaskMenuWork[1] );
  }

}

static const GFL_UI_TP_HITTBL touch_move_table[]={
  {   0, 167,   0, 239, },
  { 168, 191,  72, 183, },
  {GFL_UI_TP_HIT_END,0,0,0},    // �I���f�[�^
};

//----------------------------------------------------------------------------------
/**
 * @brief �T�u�V�[�P���X����
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  GFL_PROC_RESULT   
 */
//----------------------------------------------------------------------------------
static GFL_PROC_RESULT SubSeq_Main( EARTH_DEMO_WORK *wk, int *seq )
{
  ARCHANDLE* p_handle;
  GFL_PROC_RESULT sys_result = GFL_PROC_RES_CONTINUE;

  wk->trg = GFL_UI_KEY_GetTrg(); 
  wk->cont = GFL_UI_KEY_GetCont();

  if(GFL_UI_TP_HitCont( touch_move_table )!=GFL_UI_TP_HIT_NONE){
    wk->tpcont = GFL_UI_TP_GetPointCont(&wk->tpx, &wk->tpy); 
  }else{
    wk->tpcont = FALSE;
  }
  if(wk->tpcont == TRUE){
    wk->tptrg = GFL_UI_TP_GetTrg(); 
  } else {
    wk->tptrg = GFL_UI_TP_GetPointTrg(&wk->tpx, &wk->tpy); 
  }

  // ����Œǉ������^�b�`=A�{�^�������͂Ȃ����܂����B
//  // �n���V���[�h�ȊO�Ȃ�^�b�`=A�{�^��
//  if( (*seq) != EARTHDEMO_SEQ_EARTH_DISPON &&
//    (*seq) != EARTHDEMO_SEQ_MOVE_EARTH && 
//    (*seq) != EARTHDEMO_SEQ_MOVE_CAMERA )
//  { 
//    if( GFL_UI_TP_GetTrg() ){ wk->trg = PAD_BUTTON_A; }
//  }

  switch(*seq){
  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MODELLOAD://���f���f�[�^���[�h
    //���b�Z�[�W�f�[�^�ǂݍ���
    wk->msg_man = GFL_MSG_Create
        (GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_earth_guide_dat, wk->heapID);

    p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_EARTH, wk->heapID );

    Earth_ModelLoad(wk, p_handle);  //���f�����[�h���C��
    Earth_BGdataLoad(wk, p_handle); //�a�f�f�[�^���[�h

    GFL_ARC_CloseDataHandle( p_handle );
    //�P�x�ύX�Z�b�g�i�h�m�j
    wk->fade_end_flag = FALSE;
    WIPE_SYS_Start( WIPE_PATTERN_WMS,WIPE_TYPE_FADEIN,WIPE_TYPE_FADEIN,
            WIPE_FADE_BLACK,WIPE_DEF_DIV,WIPE_DEF_SYNC,wk->heapID);

    GFL_BG_SetVisible( EARTH_TEXT_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_ICON_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_BACK_S_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_BACK_M_PLANE, VISIBLE_ON );

    *seq = EARTHDEMO_SEQ_DISPON;
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_DISPON:  //�`��n�m
    if(WIPE_SYS_EndCheck() == TRUE){  //�P�x�ύX�҂�
      *seq = EARTHDEMO_SEQ_WELCOME_MSG;
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_WELCOME_MSG: //�u�悤�����v
    if(Earth_MsgPrint(wk,mes_earth_01_01,A_BUTTON_NOWAIT) == TRUE){
      //�Z��ł���ꏊ���͍ρH
      if(wk->my_nation == 0){
        //���o�^
        *seq = EARTHDEMO_SEQ_MAINMENU;  //���C�����j���[�ݒ��
      }else{
        //�o�^��
        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //�R�c�n���V���[�h�n�m��
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MAINMENU:  //���C�����j���[�ݒ�

    if(Earth_MsgPrint(wk,mes_earth_01_02,A_BUTTON_NOWAIT) == TRUE){
      //�I�����X�g�\��
      Earth_BmpListAdd(wk, &EarthMenuWinData, &MenuListHeader, MenuList);
      *seq = EARTHDEMO_SEQ_MAINMENU_SELECT; //�I�����[�h��
    }
    break;

  case EARTHDEMO_SEQ_MAINMENU_SELECT: //���C�����j���[�I�����[�h
    {
      u32 list_result = BmpMenuList_Main(wk->bmplist);

      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//�I�����X�g�폜����

      PMSND_PlaySE( WIFIEARTH_SND_SELECT );

      switch(list_result){
      default:
      case MENU_WORLD:  //�u�������v
        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //�R�c�n���V���[�h�n�m��
        break;
      case MENU_REGISTRATION: //�u�Ƃ��낭�v
        *seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //�o�^���j���[�ݒ��
        break;
      case BMPMENULIST_CANCEL:
      case MENU_END:  //�u��߂�v
        *seq = EARTHDEMO_SEQ_END; //�I��������
        break;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_REGISTRATIONMENU:  //�o�^���j���[�ݒ�

    if(Earth_MsgPrint(wk,mes_earth_01_03,A_BUTTON_NOWAIT) == TRUE){
      //�͂�/�������E�B���h�E�\��
      wk->yesnowin = BmpMenu_YesNoSelectInit
        (&EarthYesNoWinData, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL, 0, wk->heapID);
      *seq = EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT; //�I�����[�h��
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT: //�o�^���j���[�I�����[�h
    {
      u32 list_result = BmpMenu_YesNoSelectMain(wk->yesnowin);

      switch(list_result){
      case 0:   //�u�͂��v
        if(wk->Japan_ROM_mode == TRUE){
          wk->my_nation_tmp = country106; //���{��Ō��菈���i�����̓X�L�b�v�j
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA; //�n��ʓo�^���X�g�I����
        }else{
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION;//���ʓo�^���X�g�I����
        }
        break;
      case BMPMENU_CANCEL:  //�u�������v
        *seq = EARTHDEMO_SEQ_MAINMENU;
        break;
      }
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION: //���ʓo�^���X�g�ݒ�
    wk->my_nation_tmp = 0;//�o�^���e���|����������
    Earth_BmpListAddGmmAll
      (wk, &EarthPlaceListWinData, &PlaceListHeader, NARC_message_wifi_place_msg_world_dat);
    *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2;  //���ʓo�^���X�g�I����
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2:
    if(Earth_MsgPrint(wk,mes_earth_01_04,A_BUTTON_NOWAIT) == TRUE){
      *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT;  //���ʓo�^���X�g�I����
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT:  //���ʓo�^���X�g�I�����[�h
    {
      int list_result = BmpMenuList_Main(wk->bmplist);
#ifdef WIFI_ERATH_DEBUG
      EarthDebugNationMarkSet(wk);
#endif
      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//�I�����X�g�폜����

      PMSND_PlaySE( WIFIEARTH_SND_SELECT );

      switch(list_result){
      default:
        {
          wk->my_nation_tmp = list_result;  //���o�^

          if(WIFI_LocalAreaExistCheck(wk->my_nation_tmp) == TRUE){
            //�n�悪���݂���ꍇ
            *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA; //�n��ʓo�^���X�g�I����
          }else{
            //�n�悪���݂��Ȃ��ꏊ�̓o�^
            wk->my_area_tmp = 0;        //�n��o�^�i�Ȃ��j
            *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION;  //���n��o�^�ŏI�m�F��
          }
        }
        break;
      case BMPMENULIST_CANCEL:  //�u�������v�L�����Z��
        //*seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //�o�^���j���[��
        *seq = EARTHDEMO_SEQ_MAINMENU;
        break;
      }
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA: //�n��ʓo�^���X�g�ݒ�
    wk->my_area_tmp = 0;//�o�^���e���|����������
    Earth_BmpListAddGmmAll(wk, &EarthPlaceListWinData,&PlaceListHeader,
          WIFI_COUNTRY_CountryCodeToPlaceMsgDataID(wk->my_nation_tmp));
    *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2;
    break;
  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2:  //�n��ʓo�^���X�g�ݒ�
    if(Earth_MsgPrint(wk,mes_earth_01_05,A_BUTTON_NOWAIT) == TRUE){
      *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT;  //�n��ʓo�^���X�g�I����
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT:  //�n��ʓo�^���X�g�I�����[�h
    {
      int list_result = BmpMenuList_Main(wk->bmplist);
#ifdef WIFI_ERATH_DEBUG
      EarthDebugAreaMarkSet(wk);
#endif
      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//�I�����X�g�폜����

      PMSND_PlaySE( WIFIEARTH_SND_SELECT );

      switch(list_result){
      default:
        wk->my_area_tmp = list_result;      //�n��o�^
        *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION;  //���n��o�^�ŏI�m�F��
        break;
      case BMPMENULIST_CANCEL://�L�����Z��
        if(wk->Japan_ROM_mode == TRUE){
          //*seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //�o�^���j���[��(���{�ł̏ꍇ)
          *seq = EARTHDEMO_SEQ_MAINMENU;
        } else {
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION; //���ʓo�^���X�g�ݒ��
        }
      }
    }
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION:  //�o�^�ŏI�m�F�ݒ�
    Earth_MyPlaceInfoWinSet2( wk, wk->my_nation_tmp, wk->my_area_tmp );//�o�^�n���\��
    *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO;  //�o�^�ŏI�m�F��
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO:  //�o�^�ŏI�m�F

    if(Earth_MsgPrint(wk,mes_earth_01_06,A_BUTTON_NOWAIT) == TRUE){
      //�͂�/�������E�B���h�E�\��
      wk->yesnowin = BmpMenu_YesNoSelectInit
        (&EarthYesNoWinData, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL, 0, wk->heapID);
      *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT; //�o�^�ŏI�m�F�I�����[�h��
    }
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT: //�o�^�ŏI�m�F�I�����[�h
    {
      u32 list_result = BmpMenu_YesNoSelectMain(wk->yesnowin);

      switch(list_result){
      case 0:   //�u�͂��v
        Earth_MyPlaceInfoWinRelease( wk );
        //�o�^�f�[�^�Z�[�u
        WIFIHISTORY_SetMyNationArea
          (wk->wifi_sv, wk->mystatus, wk->my_nation_tmp, wk->my_area_tmp);
        // �r�[�R���f�[�^�ɂ��o�^
        GAMEBEACON_SendDataUpdate_NationArea( wk->my_nation_tmp, wk->my_area_tmp );

        wk->my_nation = wk->my_nation_tmp;
        wk->my_area = wk->my_area_tmp;

        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //�R�c�n���V���[�h�n�m��
        break;
      case BMPMENU_CANCEL:  //�u�������v
        Earth_MyPlaceInfoWinRelease( wk );
        *seq = EARTHDEMO_SEQ_MAINMENU;
        break;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_EARTH_DISPON:  //�R�c�n���V���[�h�n�m
    //���{��ŁH
    if((wk->Japan_ROM_mode == TRUE)&&(wk->my_worldopen_flag == FALSE)){
      //���{��łŊC�O�̃��[�h���J���Ă��Ȃ��ꍇ�͓��{����}�b�v
      wk->earth_mode = JAPAN_MODE;
    }else{
      //���̑��A���E�}�b�v
      wk->earth_mode = GLOBAL_MODE;
    }
#ifdef WIFI_ERATH_DEBUG_ALL_DRAW
    wk->earth_mode = GLOBAL_MODE; // DEBUG
#endif
    EarthDataInit(wk);                //�n���f�[�^������
    EarthList_NationAreaListSet(wk);  //�o�^�n��f�[�^�ݒ�
    EarthCameraStart(wk);             //�J�����J�n�ݒ�

    //���b�Z�[�W��ʃN���A
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
    GFL_BMPWIN_TransVramCharacter(wk->msgwin);
    //�u��߂�v�A�C�R���n�m
    TaskMenuPut( wk, 1 );
//    GFL_BMPWIN_MakeScreen(wk->iconwin);
//    BmpWinFrame_Write(wk->iconwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL);

    //�Z��ł���ꏊ���͍ρH
    if(wk->my_nation != 0){
      Earth_MyPlaceInfoWinSet( wk );//�o�^�n���\��
    }
    Earth_PosInfoPut( wk );
    wk->info_mode = 0;

    wk->Draw3Dsw = DRAW3D_ENABLE;//�R�c�`��X�C�b�`�ݒ�n�m

    *seq = EARTHDEMO_SEQ_MOVE_EARTH;
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MOVE_EARTH:  //�n����]
    {
      u32 minindex; // �_�~�[
      u16 camera_status_backup = wk->camera_status;

      //�^�b�`�p�l���R���g���[��
      Earth_TouchPanel(wk);

      if( Earth_SearchPosInfo(wk, &minindex ) ){
        // �u�݂�v�A�C�R���n�m
        TaskMenuPut( wk, 0 );
      }else{
        //�u�݂�v�A�C�R���n�e�e
        TaskMenuOff( wk, 0 );
      }

      //�I������
      if((wk->trg & PAD_BUTTON_B)||(wk->tp_result & PAD_BUTTON_B)){
        //�u��߂�v�A�C�R���n�e�e
        TaskMenuOff( wk, TASKMENU_MIRU );
        _menu_anime_on( wk, TASKMENU_YAMERU );
        PMSND_PlaySE( WIFIEARTH_SND_YAMERU );

        //���b�Z�[�W��ʃN���A
        GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
        GFL_BMPWIN_TransVramCharacter(wk->msgwin);
        *seq = EARTHDEMO_SEQ_BUTTON_ANIME;
      }else{
        // �u�݂�v�@�\
        if( ((wk->trg & PAD_BUTTON_X)||(wk->tp_result & PAD_BUTTON_X))&&(wk->info_mode == 0) ){
          wk->info_mode = 1;
          _menu_anime_on( wk, TASKMENU_MIRU );
          Earth_PosInfoPut( wk );

          // info_mode�ɂ� Earth_PosInfoPut �̌������ʂ������Ă���
          if( wk->info_mode == 1 ){
            PMSND_PlaySE( WIFIEARTH_SND_XSELECT );
          }
          break;
        }

        // ��������
        if((wk->trg & (PAD_BUTTON_X|PAD_BUTTON_A|PAD_BUTTON_B)||(wk->tp_result & PAD_BUTTON_X))
            &&(wk->info_mode == 1)){
          wk->info_mode = 0;
          Earth_PosInfoPut( wk );
          break;
        }

        {
          BOOL move_flag;
          //�n����]�R���g���[��
          move_flag = Earth3D_Control(wk, wk->trg, wk->cont);

          if( (move_flag == TRUE)&&(wk->info_mode == 1) ){
            wk->info_mode = 0;
            Earth_PosInfoPut( wk );
          }
        }
        //�J�������߈ړ�����
        if(camera_status_backup != wk->camera_status){
          *seq = EARTHDEMO_SEQ_MOVE_CAMERA;
          if( wk->camera_status == CAMERA_FAR ){
            PMSND_PlaySE( WIFIEARTH_SND_ZOMEOUT );
          }else{
            PMSND_PlaySE( WIFIEARTH_SND_ZOMEIN );
          }
        }
      }
    }

#ifdef WIFI_ERATH_DEBUG
    EarthDebugWinRotateInfoWrite(wk);//�f�o�b�O���\��
#endif
    break;

  case EARTHDEMO_SEQ_BUTTON_ANIME:
    if(APP_TASKMENU_WIN_IsFinish( wk->TaskMenuWork[1]) ){
      //�Z��ł���ꏊ���͍ρH
      if(wk->my_nation == 0){
        //���j���[��ʂ�
        wk->Draw3Dsw = DRAW3D_BANISH;//�R�c��ʏ������X�C�b�`�ݒ�n�e�e
        *seq = EARTHDEMO_SEQ_MAINMENU;
      }else{
        Earth_MyPlaceInfoWinRelease( wk );
        *seq = EARTHDEMO_SEQ_END;//�I��
      }
      TaskMenuOff( wk, TASKMENU_YAMERU );
    }
    break;

  case EARTHDEMO_SEQ_MOVE_CAMERA: //�J���������ړ�
    {
      BOOL end_flag = Earth3D_CameraMoveNearFar(wk);

      if(end_flag == TRUE){
        *seq = EARTHDEMO_SEQ_MOVE_EARTH;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_END:   //�I������
    wk->fade_end_flag = FALSE;//�P�x�ύX�҂��t���O���Z�b�g
    WIPE_SYS_Start( WIPE_PATTERN_WMS,WIPE_TYPE_FADEOUT,WIPE_TYPE_FADEOUT,
            WIPE_FADE_BLACK,WIPE_DEF_DIV,WIPE_DEF_SYNC,wk->heapID);
    *seq = EARTHDEMO_SEQ_EXIT;
    break;

  case EARTHDEMO_SEQ_EXIT:  //�I��

    if(WIPE_SYS_EndCheck() == TRUE){  //�P�x�ύX�҂�
      wk->Draw3Dsw = DRAW3D_ENABLE;//�R�c�`��X�C�b�`�ݒ�n�e�e
      //�a�f�f�[�^�j��
      Earth_BGdataRelease(wk);
      //���f���f�[�^�j��
      Earth_ModelRelease(wk);
      //���b�Z�[�W�f�[�^�j��
      GFL_MSG_Delete(wk->msg_man);

      (*seq) = 0;
      sys_result = GFL_PROC_RES_FINISH; //�I���t���O
    }
    break;
  }

  return sys_result;
}

//============================================================================================
//  �I���v���Z�X
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Exit(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk  = (EARTH_DEMO_WORK*)mywk;
  int heapID = wk->heapID;

  PRINTSYS_QUE_Clear( wk->printQue ); 
  PRINTSYS_QUE_Delete(wk->printQue);

  //�a�f�\����~
  GFL_BG_SetVisible( EARTH_TEXT_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_ICON_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_BACK_S_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_BACK_M_PLANE, VISIBLE_OFF );
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);

  APP_KEYCURSOR_Delete(wk->printCursor);
  GFL_TCBL_Exit(wk->tcbl);

  //�e������m�ۃ��[�N�J��
  WORDSET_Delete( wk->wordset );
  Earth_BGsysRelease();

  //���[�N�J��
  GFL_PROC_FreeWork( proc );

  //�q�[�v�J��
  GFL_HEAP_DeleteHeap( heapID );

  return  GFL_PROC_RES_FINISH;
}

//============================================================================================
//
//  ���[�J���֐��i���C���V�X�e���֘A�j
//
//============================================================================================
//----------------------------------
//�u�q�`�l�ݒ�
//----------------------------------
static void Earth_VramBankSet(void)
{
  GFL_DISP_VRAM vramSetTable = {
    GX_VRAM_BG_128_C,                 // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,           // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_32_H,              // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,       // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_16_F,                 // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,          // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,             // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,      // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_01_AB,                // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0123_E,           // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_32K,       // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K,       // �T�uOBJ�}�b�s���O���[�h
  };
  // VRAM�S�N���A
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
  MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

  MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
  MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

  GFL_DISP_SetBank( &vramSetTable );
}

//----------------------------------
//�a�f�V�X�e���ݒ�
//----------------------------------
static void Earth_BGsysSet( HEAPID heapID )
{
  GFL_BG_SYS_HEADER bgsysHeader = {
    GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
  };

  //�a�f�V�X�e���N��
  GFL_BG_Init(heapID);
  //�a�f���[�h�ݒ�
  GFL_BG_SetBGMode(&bgsysHeader);

  //�R�c�V�X�e���N��
  GFL_G3D_Init( GFL_G3D_VMANLNK,
                GFL_G3D_TEX384K,
                GFL_G3D_VMANLNK,
                GFL_G3D_PLT64K,
                0,
                heapID,
                NULL );
  GFL_BG_SetBGControl3D(1);

  //�r�b�g�}�b�v�E�C���h�E�N��
  GFL_BMPWIN_Init(heapID);

  GFL_DISP_SetDispOn();
}

static void Earth_BGsysRelease( void )
{
  GFL_BMPWIN_Exit();
  GFL_G3D_Exit();
  GFL_BG_Exit();
}


//============================================================================================
//
//  ���[�J���֐��i�n��f�[�^�擾�֘A�j
//
//============================================================================================
//----------------------------------
//�n�_���X�g���[�h
//----------------------------------
static void EarthListLoad( EARTH_DEMO_WORK * wk )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_EARTH_PLACE, wk->heapID );

  //�n�_���X�g����������
  wk->placelist.listcount = 0;

  {//�n�_�}�[�N��]�������i���f�[�^�o�C�i���f�[�^���[�h�j
    void* filep;
    EARTH_DATA_NATION* listp;
    u32 size;
    int i,listcount;

    filep = GFL_ARCHDL_UTIL_LoadEx
      ( p_handle, NARC_wifi_earth_place_place_pos_wrd_dat, FALSE, wk->heapID, &size );

    listp = (EARTH_DATA_NATION*)filep;  //�t�@�C���ǂݍ��ݗp�ɕϊ�
    listcount = size/6;       //�n�_���擾�i�f�[�^���F�P�n�_�ɂ��U�o�C�g�j

    listp++;  //1origin�̂��ߓǂݔ�΂�
    for(i=1;i<listcount;i++){
      if(listp->flag != 2){ //2�̏ꍇ�͒n�惊�X�g�����݂���
        OS_Printf("pre y=%d,", listp->y);
        EarthListSet(wk,wk->placelist.listcount,listp->x,listp->y,i,0);
        wk->placelist.listcount++;
      }
      listp++;
    }
    GFL_HEAP_FreeMemory(filep);
  }
  {//�n�_�}�[�N��]�������i�n��f�[�^�o�C�i���f�[�^���[�h�j
    void* filep;
    EARTH_DATA_AREA* listp;
    u32 size, data_id;
    int i,datLen,index,listcount;

    index = 1;  //1orgin
    datLen = WIFI_COUNTRY_GetDataLen();

    while(index < datLen){

      data_id = WIFI_COUNTRY_DataIndexToPlaceDataID( index );
      filep = GFL_ARCHDL_UTIL_LoadEx( p_handle, data_id, FALSE, wk->heapID, &size );

      listp = (EARTH_DATA_AREA*)filep;  //�t�@�C���ǂݍ��ݗp�ɕϊ�
      listcount = size/4;   //�n�_���擾�i�f�[�^���F�P�n�_�ɂ��S�o�C�g�j

      listp++;  //1origin�̂��ߓǂݔ�΂�
      for(i=1;i<listcount;i++){//1orgin
        EarthListSet(wk,wk->placelist.listcount,listp->x,listp->y,
                WIFI_COUNTRY_DataIndexToCountryCode(index),i);
        wk->placelist.listcount++;
        listp++;
      }
      GFL_HEAP_FreeMemory(filep);
      index++;
    }
  }
  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------
//���X�g�o�^
//----------------------------------
static void EarthListSet( EARTH_DEMO_WORK * wk,u32 index,s16 x,s16 y,u16 nationID,u16 areaID )
{
  MtxFx33 rotMtx = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};
  VecFx32 rotVec;
  OS_Printf(" func in y=%d\n", y);

  wk->placelist.place[index].x = x; //�w��]�I�t�Z�b�g�擾
  wk->placelist.place[index].y = y; //�x��]�I�t�Z�b�g�擾
  
  rotVec.x = x;
  rotVec.y = y;
  rotVec.z = INIT_EARTH_ROTATE_ZVAL;
  EarthVecFx32_to_MtxFx33_place(&rotMtx,&rotVec); //�����ʒu����̑��΍��W�v�Z
  wk->placelist.place[index].rotate = rotMtx;

  //�}�[�N�F�ݒ�(wifihistory��`�ɉ�������)
  wk->placelist.place[index].col = WIFIHISTORY_GetStat(wk->wifi_sv,nationID,areaID);

#ifdef WIFI_ERATH_DEBUG_ALL_DRAW
  wk->placelist.place[index].col = WIFIHIST_STAT_EXIST; // DEBUG
#endif

  wk->placelist.place[index].nationID = nationID; //�Y�����h�c
  wk->placelist.place[index].areaID = areaID;   //�Y���n��h�c
}

//----------------------------------
//�o�^�n����Z�b�g
//----------------------------------
static void EarthList_NationAreaListSet( EARTH_DEMO_WORK * wk )
{
  int i;

  for(i=0;i<wk->placelist.listcount;i++){
    if((wk->placelist.place[i].nationID == wk->my_nation) //�Y�����h�c
      &&(wk->placelist.place[i].areaID == wk->my_area)){  //�Y���n��h�c

      wk->placelist.place[i].col = MARK_RED;  //�}�[�N�F�ݒ�
      wk->rotate.x  = wk->placelist.place[i].x; //�n���V�A��
      wk->rotate.y  = wk->placelist.place[i].y; //�n���V�A��
    }
  }
}

//----------------------------------
//���[�J���G���A���݃`�F�b�N
//----------------------------------
static int  EarthAreaTableGet(int nationID)
{
  return WIFI_COUNTRY_CountryCodeToDataIndex( nationID );
}

// �u�݂�v�u��߂�v���W
#define MENU_BT0_X  (  0 )
#define MENU_BT1_X  ( 23 )
#define MENU_BT_Y   ( 21 )
#define MENU_BT_W   (  9 )
#define MENU_BT_H   (  3 )


// �u�݂�v�u��߂�v�{�^���͈͒�`
static const GFL_UI_TP_HITTBL touch_tbl[]={
  { MENU_BT_Y*8,(MENU_BT_Y+MENU_BT_H)*8-1, MENU_BT0_X*8, (MENU_BT0_X+MENU_BT_W)*8-1 },
  { MENU_BT_Y*8,(MENU_BT_Y+MENU_BT_H)*8-1, MENU_BT1_X*8, (MENU_BT1_X+MENU_BT_W)*8-1 },
  {GFL_UI_TP_HIT_END, 0, 0, 0 },
};

//============================================================================================
//
//  ���[�J���֐��i�^�b�`�p�l���֘A�j
//
// ��GS�ύX�_ 09.02.26 : by hosaka genya
// �E�����̗����S�ʓI�ɉ���
// �E�Y�[���C���^�A�E�g�́u�A�\�r�v������
//  (1SYNC�̃^�b�`�ړ��ʂ���萔�ȉ��̎��̂݁A�Y�[���C���^�A�E�g����悤�ɂ���)
//============================================================================================
static void Earth_TouchPanel( EARTH_DEMO_WORK * wk )
{
  u32 ret;
  enum {
    ZOOM_LEN_LIMIT = 4, ///< �Y�[���C���^�A�E�g�̃A�\�r
  };

  int dirx,lenx,diry,leny;
  int button_area = 0;  ///< �O���t�B�b�N�{�^���͈̔�

  // ������
  wk->tp_result = 0;
  
  //------------------------------------------------------------------------------
  // Touch Graphic Button Check
  //------------------------------------------------------------------------------
  ret = GFL_UI_TP_HitTrg( touch_tbl );
  if(ret==0){
    button_area = PAD_BUTTON_X;
  }else if(ret==1){
    button_area = PAD_BUTTON_B;
  }
/*
  if( (wk->tpx >= ((EARTH_ICON_WIN_PX-1) * DOTSIZE))&&
    (wk->tpx <= ((EARTH_ICON_WIN_PX+1 + EARTH_ICON_WIN_SX) * DOTSIZE))&&
    (wk->tpy >= ((EARTH_ICON_WIN_PY-1) * DOTSIZE))&&
    (wk->tpy <= ((EARTH_ICON_WIN_PY + EARTH_ICON_WIN_SY) * DOTSIZE))) {
    // �u��߂�v�{�^��
    button_area = PAD_BUTTON_B;
  } else if( (wk->tpx >= ((EARTH_LOOK_WIN_PX-1) * DOTSIZE))&&
    (wk->tpx <= ((EARTH_LOOK_WIN_PX+1 + EARTH_LOOK_WIN_SX) * DOTSIZE))&&
    (wk->tpy >= ((EARTH_LOOK_WIN_PY-1) * DOTSIZE))&&
    (wk->tpy <= ((EARTH_LOOK_WIN_PY + EARTH_LOOK_WIN_SY) * DOTSIZE))) {
    //�u�݂�v�{�^��
    button_area = PAD_BUTTON_X;
  }
*/
  //------------------------------------------------------------------------------
  // Touch Trg
  //------------------------------------------------------------------------------
  if(wk->tptrg){
    if( button_area ) {
      // ���Ɍ��ʂ��o�Ă��� = �{�^���͈�
      // �{�^�������������Ƃɂ��Ĕ�����
      wk->tp_result = button_area;
      return;
    } else {
      // ���̂ق��̏ꍇ�A�󋵂����Z�b�g
      wk->tp_seq = 0;
      wk->tp_lenx = 0;
      wk->tp_leny = 0;
      wk->tp_count = 0;
      wk->tp_result = 0;
      //����̌��o�ʒu��ۑ�
      wk->tp_x = wk->tpx;
      wk->tp_y = wk->tpy;
      wk->tp_count = 4;
    }
  }
  
  //------------------------------------------------------------------------------
  // Touch Cont
  //------------------------------------------------------------------------------
  if(wk->tpcont){
    switch(wk->tp_seq){
    case 0:
      //�ŏ��̃J�E���g�̓g���K�[�F���p�ɖ���
      if(!wk->tp_count){
        wk->tp_seq++;
      }else{
        wk->tp_count--;
      }
    case 1:
      // �{�^���͈̔͂�������n���V�^�b�`�������΂�
      if( button_area ){ break; }

      Earth_TouchPanelParamGet(wk, wk->tp_x,wk->tp_y,&dirx,&lenx,&diry,&leny);
      wk->tp_result = dirx | diry;
      wk->tp_lenx = lenx;
      wk->tp_leny = leny;
      wk->tp_x = wk->tpx;
      wk->tp_y = wk->tpy;
      break;
    }
  //------------------------------------------------------------------------------
  // Touch None
  //------------------------------------------------------------------------------
  }else{
    if( !button_area && wk->tp_count && wk->tp_lenx < ZOOM_LEN_LIMIT && wk->tp_leny < ZOOM_LEN_LIMIT ) {
      // �Y�[���C���^�A�E�g
      wk->tp_result = PAD_BUTTON_A;
    }
    wk->tp_seq = 0;
    wk->tp_lenx = 0;
    wk->tp_leny = 0;
    wk->tp_count = 0;
  }
}

static void Earth_TouchPanelParamGet( EARTH_DEMO_WORK * wk,
  int prevx,int prevy,int* dirx_p,int* lenx_p,int* diry_p,int* leny_p )
{
  int x_dir = 0;
  int y_dir = 0;
  int x_len = 0;
  int y_len = 0;

  //�w�������ړ����擾
  if(wk->tpx != 0xffff){
    x_len = wk->tpx - prevx;
    if(x_len < 0){
      x_len ^= -1;
      x_dir = PAD_KEY_RIGHT;
    }else{
      if(x_len > 0){
        x_dir = PAD_KEY_LEFT;
      }
    }
  }
  x_len &= 0x3f;  //���~�b�^�[
  *dirx_p = x_dir;
  *lenx_p = x_len;

  //�x�������ړ����擾
  if(wk->tpy != 0xffff){
    y_len = wk->tpy - prevy;
    if(y_len < 0){
      y_len ^= -1;
      y_dir = PAD_KEY_DOWN;
    }else{
      if(y_len > 0){
        y_dir = PAD_KEY_UP;
      }
    }
  }
  y_len &= 0x3f;  //���~�b�^�[
  *diry_p = y_dir;
  *leny_p = y_len;
}


//============================================================================================
//
//  ���[�J���֐��i�Q�c�֘A�j
//
//============================================================================================
//----------------------------------
//�a�l�o�v�h�m�쐬
//----------------------------------
static GFL_BMPWIN* _createBmpWin(const BMPWIN_DAT* wDat, BOOL makeScreen)
{
  GFL_BMPWIN* bmpwin;

  //�E�C���h�E�쐬
  bmpwin = GFL_BMPWIN_Create( wDat->frmNum, 
                              wDat->px, wDat->py, 
                              wDat->sx, wDat->sy, 
                              wDat->palIdx, GFL_BMP_CHRAREA_GET_B);
  //�E�C���h�E�N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), FBMP_COL_WHITE );
  if(makeScreen == TRUE){
    GFL_BMPWIN_MakeScreen(bmpwin);
  }
  return bmpwin;
}

//----------------------------------
//�a�f�ݒ�
//----------------------------------
static void Earth_BGdataLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle )
{
  //�t�H���g�n���h���쐬
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT,
                                    NARC_font_large_gftr,
                                    GFL_FONT_LOADTYPE_FILE,
                                    FALSE,
                                    wk->heapID);

  //�����F�ݒ�o�b�N�A�b�v
  GFL_FONTSYS_GetColor(&wk->printLSB.l, &wk->printLSB.s, &wk->printLSB.b);
    
  //�����F�ݒ�
  GFL_FONTSYS_SetColor(FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_WHITE);
  //--------�T�u�a�f��-------------------------------------------
  //�e�L�X�g�a�f�ʃR���g���[���ݒ�
  GFL_BG_SetBGControl(EARTH_TEXT_PLANE,&Earth_Demo_BGtxt_header,GFL_BG_MODE_TEXT);
  GFL_BG_ClearScreen(EARTH_TEXT_PLANE);//�e�L�X�g�a�f�ʃN���A

  //�w�i�a�f�ʃR���g���[���ݒ�
  GFL_BG_SetBGControl(EARTH_BACK_S_PLANE,&Earth_Demo_Back_header,GFL_BG_MODE_TEXT);

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, 
                                        NARC_wifi_earth_earth_bg_NCGR,
                                        EARTH_BACK_S_PLANE,
                                        0,
                                        0,
                                        FALSE,
                                        wk->heapID);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,
                                    NARC_wifi_earth_earth_bg_NCLR,
                                    PALTYPE_SUB_BG,
                                    EARTH_BACK_PAL*PALSIZE,
                                    PALSIZE*4,
                                    wk->heapID);
  GFL_ARCHDL_UTIL_TransVramScreen(p_handle,
                                  NARC_wifi_earth_earth_bg_NSCR,
                                  EARTH_BACK_S_PLANE,
                                  0,
                                  0,
                                  FALSE,
                                  wk->heapID);
  //���b�Z�[�W�E�C���h�E�L�������p���b�g�ǂݍ��݁i�E�C���h�E�O���j
  BmpWinFrame_GraphicSet
    (EARTH_TEXT_PLANE, EARTH_TALKWINCHR_NUM, EARTH_TALKWIN_PAL, MENU_TYPE_SYSTEM, wk->heapID);
  BmpWinFrame_GraphicSet
    (EARTH_TEXT_PLANE, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL, MENU_TYPE_SYSTEM, wk->heapID);

  //�t�H���g�p���b�g�ǂݍ���
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_SUB_BG,
                                EARTH_SYSFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);

  //NULL�L�������p���b�g�ݒ�
  GFL_BG_SetClearCharacter( EARTH_TEXT_PLANE, 32, 0, wk->heapID );
  GFL_BG_SetBackGroundColor( EARTH_TEXT_PLANE,EARTH_NULL_PALETTE );

  //���b�Z�[�W�E�C���h�E�r�b�g�}�b�v�쐬�i�E�C���h�E�����j
  wk->msgwin = _createBmpWin( &EarthMsgWinData, TRUE );
  BmpWinFrame_Write(wk->msgwin, WINDOW_TRANS_ON, EARTH_TALKWINCHR_NUM,EARTH_TALKWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->msgwin);

  //���b�Z�[�W�V�X�e��������
  wk->msgseq = MSGSET;

  //--------���C���a�f��------------------------------------------
  //�A�C�R���a�f�ʃR���g���[���ݒ�
  GFL_BG_SetBGControl(EARTH_ICON_PLANE,&Earth_Demo_BGtxt_header,GFL_BG_MODE_TEXT);
  GFL_BG_ClearScreen(EARTH_ICON_PLANE);//�A�C�R���a�f�ʃN���A

  // �R�c�ʃv���C�I���e�B�ݒ�
  GFL_BG_SetBGControl3D( 1 );     // �{�^�����͉���

  //�w�i�a�f�ʃR���g���[���ݒ�
  GFL_BG_SetBGControl(EARTH_BACK_M_PLANE,&Earth_Demo_Back_header,GFL_BG_MODE_TEXT);

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, 
                                        NARC_wifi_earth_earth_bg_NCGR,
                                        EARTH_BACK_M_PLANE,
                                        0,
                                        0,
                                        FALSE,
                                        wk->heapID);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,
                                    NARC_wifi_earth_earth_bg_NCLR,
                                    PALTYPE_MAIN_BG,
                                    EARTH_BACK_PAL*PALSIZE,
                                    PALSIZE*4,
                                    wk->heapID);
  GFL_ARCHDL_UTIL_TransVramScreen(p_handle,
                                  NARC_wifi_earth_earth_bg_NSCR,
                                  EARTH_BACK_M_PLANE,
                                  0,
                                  0,
                                  FALSE,
                                  wk->heapID);
  //���j���[�E�C���h�E�L�������p���b�g�ǂݍ��݁i�E�C���h�E�O���j
  BmpWinFrame_GraphicSet
    (EARTH_ICON_PLANE, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL, 0, wk->heapID);

  //�t�H���g�p���b�g�ǂݍ���
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_MAIN_BG,
                                EARTH_SYSFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_MAIN_BG,
                                EARTH_TOUCHFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);

  //NULL�L�������p���b�g�ݒ�
  GFL_BG_SetClearCharacter( EARTH_ICON_PLANE, 32, 0, wk->heapID );
  GFL_BG_SetBackGroundColor( EARTH_ICON_PLANE,0x0000 );
  {
    
#if 0
    STRBUF* temp_str;

    //���b�Z�[�W�o�b�t�@�̐���
    temp_str= GFL_STR_CreateBuffer(16, wk->heapID);

    //���b�Z�[�W�E�C���h�E�r�b�g�}�b�v�쐬�i�E�C���h�E�����j
    wk->iconwin = _createBmpWin( &EarthIconWinData, FALSE );
    //������̎擾�i��߂�j
    GFL_MSG_GetString(wk->msg_man, mes_earth_03_03, temp_str);
    //������̕\��
    Earth_StrPrint(wk, wk->iconwin, temp_str, 4, 0);  
    GFL_BMPWIN_TransVramCharacter(wk->iconwin);
    
    //���b�Z�[�W�E�C���h�E�r�b�g�}�b�v�쐬�i�E�C���h�E�����j
    wk->lookwin = _createBmpWin( &EarthLookWinData, FALSE );
    //������̎擾�i�݂�j
    GFL_MSG_GetString(wk->msg_man, mes_earth_02_08, temp_str);
    //������̕\��
    Earth_StrPrint(wk, wk->lookwin, temp_str, 0, 0);
    GFL_BMPWIN_TransVramCharacter(wk->lookwin);

    //���b�Z�[�W�o�b�t�@�̊J��
    GFL_STR_DeleteBuffer(temp_str);

#endif
    
    // �^�X�N���j���[�̓o�^
    Earth_TaskMenuInit( wk );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief ��ʉ��Q�̃{�^����TASKMENU�V�X�e���ŕ`��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Earth_TaskMenuInit( EARTH_DEMO_WORK *wk )
{
  
  //������̎擾�i�݂�E��߂�j
  wk->menustr[0] = GFL_MSG_CreateString(wk->msg_man, mes_earth_02_08);
  wk->menustr[1] = GFL_MSG_CreateString(wk->msg_man, mes_earth_03_03 );
  // �^�X�N�����j���[���\�[�X�ǂݍ���
  wk->TaskMenuRes = APP_TASKMENU_RES_Create( EARTH_ICON_PLANE, 
                                             EARTH_TASKMENU_PAL, 
                                             wk->fontHandle, 
                                             wk->printQue, 
                                             wk->heapID );
  // �u�݂�v���j���[���ڐݒ�
  wk->menuitem[0].str      = wk->menustr[0];
  wk->menuitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->menuitem[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
  // �u���ǂ�v
  wk->menuitem[1].str      = wk->menustr[1];
  wk->menuitem[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->menuitem[1].type     = APP_TASKMENU_WIN_TYPE_RETURN;

}


static const int pos_tbl[][3] = {
  { MENU_BT0_X ,MENU_BT_Y, MENU_BT_W },
  { MENU_BT1_X ,MENU_BT_Y, MENU_BT_W },
};

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[�\��ON
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void TaskMenuPut( EARTH_DEMO_WORK *wk, int id )
{
  // �o�^                                             
  if(wk->TaskMenuWork[id]==NULL){
    wk->TaskMenuWork[id] =  APP_TASKMENU_WIN_Create( 
                                wk->TaskMenuRes, &wk->menuitem[id],  
                                pos_tbl[id][0], 
                                pos_tbl[id][1],
                                pos_tbl[id][2], wk->heapID );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[�\��OFF
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void TaskMenuOff( EARTH_DEMO_WORK *wk, int id )
{
  // �폜
  if(wk->TaskMenuWork[id]!=NULL){
    APP_TASKMENU_WIN_Delete( wk->TaskMenuWork[id] );
    GFL_BG_LoadScreenV_Req( EARTH_ICON_PLANE );
    wk->TaskMenuWork[id] = NULL;
  }
}



static void Earth_BGdataRelease( EARTH_DEMO_WORK * wk )
{
  // �^�X�N���j���[�����
  TaskMenuOff( wk, TASKMENU_MIRU );
  TaskMenuOff( wk, TASKMENU_YAMERU );
  APP_TASKMENU_RES_Delete( wk->TaskMenuRes );
  GFL_STR_DeleteBuffer( wk->menustr[0]);
  GFL_STR_DeleteBuffer( wk->menustr[1]);
  wk->TaskMenuWork[0]   = NULL;
  wk->TaskMenuWork[1]   = NULL;

  // BMPWIN��������
//  GFL_BMPWIN_Delete(wk->iconwin);
//  GFL_BMPWIN_Delete(wk->lookwin);
  GFL_BMPWIN_Delete(wk->msgwin);

  GFL_BG_FreeBGControl(EARTH_ICON_PLANE);
  GFL_BG_FreeBGControl(EARTH_TEXT_PLANE);
  GFL_BG_FreeBGControl(EARTH_BACK_M_PLANE);
  GFL_BG_FreeBGControl(EARTH_BACK_S_PLANE);

  GFL_FONT_Delete(wk->fontHandle);
  //�����F�ݒ蕜�A
  GFL_FONTSYS_SetColor(wk->printLSB.l, wk->printLSB.s, wk->printLSB.b);
}

//----------------------------------
//������\��
//----------------------------------
static BOOL Earth_StrPrint
( EARTH_DEMO_WORK * wk, GFL_BMPWIN* bmpwin, const STRBUF* strbuf, u8 x, u8 y )
{
  //�v�����g�L���[�n���h���쐬
  PRINT_UTIL      printUtil;
  PRINT_UTIL_Setup(&printUtil, bmpwin);

  //������̕\��
  PRINT_UTIL_Print( &printUtil, wk->printQue, x, y, strbuf, wk->fontHandle);

  return TRUE;
}

//----------------------------------
//���b�Z�[�W�\��
//----------------------------------
static BOOL Earth_MsgPrint( EARTH_DEMO_WORK * wk,u32 msgID,int button_mode )
{
  BOOL result = FALSE;

  switch(wk->msgseq){
  case MSGSET:
    //�̈�N���A
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );

    //������o�b�t�@�̍쐬
    wk->msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

    //������̎擾
    GFL_MSG_GetString(wk->msg_man, msgID, wk->msgstr);

    //������̕\��
    wk->printStream = PRINTSYS_PrintStream( wk->msgwin,             // GFL_BMPWIN
                                            0, 0,                   // u16 x, y
                                            wk->msgstr,             // STRBUF*
                                            wk->fontHandle,         // GFL_FONT*
                                            MSGSPEED_GetWait(),     // int
                                            wk->tcbl,               // GFL_TCBLSYS*
                                            0,                      // u32 tcbpri
                                            wk->heapID,             // HEAPID
                                            FBMP_COL_WHITE );       // u16 clrCol
    wk->msgseq = MSGDRAW;
    break;

  case MSGDRAW:
    {
      PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( wk->printStream );
      APP_KEYCURSOR_Main( wk->printCursor, wk->printStream, wk->msgwin );
      switch(state){
      case PRINTSTREAM_STATE_RUNNING:///< �������s���i�����񂪗���Ă���j
        if(GFL_UI_KEY_GetCont()&(PAD_BUTTON_DECIDE)){
          PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
        }
        break;
      case PRINTSTREAM_STATE_PAUSE:  ///< �ꎞ��~���i�y�[�W�؂�ւ��҂����j
        if(wk->trg & PAD_BUTTON_A || wk->trg & PAD_BUTTON_B){
          // �����񑗂菈��
          PRINTSYS_PrintStreamReleasePause(wk->printStream);
          PMSND_PlaySE( WIFIEARTH_MOVE_CURSOR );
        }
        break;
      case PRINTSTREAM_STATE_DONE:   ///< ������I�[�܂ŕ\������
        wk->msgseq = MSGWAIT;
        break;
      }
    }
    break;

  case MSGWAIT:
    //�I��
    if((button_mode != A_BUTTON_WAIT)||(wk->trg & PAD_BUTTON_A)){
      GFL_STR_DeleteBuffer(wk->msgstr);
      PRINTSYS_PrintStreamDelete(wk->printStream);

      wk->msgseq = MSGSET;
      result = TRUE;
    }
    break;
  }
  return result;
}

//----------------------------------
//���X�g�\���p�R�[���o�b�N
//----------------------------------
static void Earth_BmpListMoveSeCall(BMPMENULIST_WORK * wk,u32 param,u8 mode)
{
  if( mode == 0 ){//���������͖炳�Ȃ�
    PMSND_PlaySE( WIFIEARTH_MOVE_CURSOR );
  }
}

//----------------------------------
//���X�g�\��:BmpList�ݒ�Q��
//----------------------------------
static void Earth_BmpListAdd( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                              const BMPMENULIST_HEADER* listheader,const EARTH_BMPLIST* list)
{
  BMPMENULIST_HEADER  listheader_tmp;
  int i;

  //���j���[�r�b�g�}�b�v�ǉ�
  wk->listwin = _createBmpWin( windata, TRUE );
  //���j���[���X�g�p������o�b�t�@�쐬
  wk->bmplistdata = BmpMenuWork_ListCreate(listheader->count,wk->heapID);
  //���j���[���X�g�p������o�b�t�@�擾
  for( i=0; i<listheader->count; i++ ){
    BmpMenuWork_ListAddArchiveString
      (wk->bmplistdata, wk->msg_man, list[i].msgID, list[i].retID, wk->heapID);
  }
  //�v�����gUTIL�n���h���쐬
  PRINT_UTIL_Setup(&wk->printUtil, wk->listwin);

  //���j���[�r�b�g�}�b�v���X�g�w�b�_�쐬
  listheader_tmp = *listheader;
  listheader_tmp.list = wk->bmplistdata;
  listheader_tmp.win  = wk->listwin;
  listheader_tmp.call_back = Earth_BmpListMoveSeCall;
  listheader_tmp.font_size_x = 12;
  listheader_tmp.font_size_y = GFL_FONT_GetLineHeight( wk->fontHandle );
  listheader_tmp.msgdata = wk->msg_man;
  listheader_tmp.print_util = &wk->printUtil;
  listheader_tmp.print_que = wk->printQue;
  listheader_tmp.font_handle = wk->fontHandle;

  //���j���[�r�b�g�}�b�v���X�g�쐬
  wk->bmplist = BmpMenuList_Set(&listheader_tmp, 0, 0, wk->heapID);
  BmpMenuList_SetCursorBmp(wk->bmplist, wk->heapID);

  //�E�C���h�E�`��
  BmpWinFrame_Write(wk->listwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->listwin);
}

//----------------------------------
//���X�g�\���Q:gmm�t�@�C���ꊇ�A���X�g�I��Ԃ�l�̓��X�g�̏��ԂƓ���(1orgin)
//----------------------------------
static void Earth_BmpListAddGmmAll( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID)
{
  BMPMENULIST_HEADER  listheader_tmp;
  GFL_MSGDATA* msg_man;
  u32 listcount;
  int i;

  //���j���[�r�b�g�}�b�v�ǉ�
  wk->listwin = _createBmpWin( windata, TRUE );
  //���b�Z�[�W�}�l�[�W���쐬
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, listarcID, wk->heapID);
  //���b�Z�[�W�����擾
  listcount = GFL_MSG_GetStrCount(msg_man);
  //���j���[���X�g�p������o�b�t�@�쐬
  wk->bmplistdata = BmpMenuWork_ListCreate(listcount,wk->heapID);

  //���j���[���X�g�p������o�b�t�@�擾
  for( i=1; i<listcount; i++ ){ //1�I���W���̂���
    BmpMenuWork_ListAddArchiveString(wk->bmplistdata, msg_man, i, i, wk->heapID);
  }
  //���b�Z�[�W�f�[�^�j��
  GFL_MSG_Delete(msg_man);

  //�v�����g�L���[�n���h���쐬
  PRINT_UTIL_Setup(&wk->printUtil, wk->listwin);

  //���j���[�r�b�g�}�b�v���X�g�w�b�_�쐬
  listheader_tmp = *listheader;
  listheader_tmp.list = wk->bmplistdata;
  listheader_tmp.count = listcount-1; //1origin�̂��ߕ␳
  listheader_tmp.win  = wk->listwin;
  listheader_tmp.call_back = Earth_BmpListMoveSeCall;
  listheader_tmp.font_size_x = 12;
  listheader_tmp.font_size_y = GFL_FONT_GetLineHeight( wk->fontHandle );
  listheader_tmp.print_util = &wk->printUtil;
  listheader_tmp.print_que = wk->printQue;
  listheader_tmp.font_handle = wk->fontHandle;

  //���j���[�r�b�g�}�b�v���X�g�쐬
  wk->bmplist = BmpMenuList_Set(&listheader_tmp, 0, 0, wk->heapID);
  BmpMenuList_SetCursorBmp(wk->bmplist, wk->heapID);

  //�E�C���h�E�`��
  BmpWinFrame_Write(wk->listwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->listwin);
}

//----------------------------------
//���X�g�폜
//----------------------------------
static void Earth_BmpListDel( EARTH_DEMO_WORK* wk )
{
  //�I�����X�g�폜����
  BmpWinFrame_Clear( wk->listwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete(wk->listwin);

  BmpMenuList_Exit(wk->bmplist,NULL,NULL);
  BmpMenuWork_ListDelete(wk->bmplistdata);

}

//----------------------------------
//�o�^�n���\���E�C���h�E
//----------------------------------
static void Earth_MyPlaceInfoWinSet( EARTH_DEMO_WORK* wk )
{
  STRBUF* msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 
  STRBUF* msgtmp = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

  wk->infowin = _createBmpWin(&EarthInfoWinData, TRUE );

  WORDSET_RegisterCountryName( wk->wordset, 0, wk->my_nation );
  WORDSET_RegisterLocalPlaceName( wk->wordset, 1, wk->my_nation, wk->my_area );

  //������̎擾
  GFL_MSG_GetString(wk->msg_man, mes_earth_03_01, msgtmp);
  WORDSET_ExpandStr(wk->wordset, msgstr, msgtmp);

  //������̕\��
  Earth_StrPrint(wk, wk->infowin, msgstr, 0, 0);

  GFL_STR_DeleteBuffer( msgtmp );
  GFL_STR_DeleteBuffer( msgstr );

  BmpWinFrame_Write(wk->infowin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->infowin);
}

static void Earth_MyPlaceInfoWinSet2( EARTH_DEMO_WORK* wk, int nation, int area )
{
  STRBUF* str1 = GFL_STR_CreateBuffer( EARTH_NAME_SIZE, wk->heapID );
  STRBUF* str2 = GFL_STR_CreateBuffer( EARTH_NAME_SIZE, wk->heapID );

  wk->infowin = _createBmpWin(&EarthInfoWinData, TRUE );

  WIFI_NationAreaNameGet(nation, area, str1, str2, wk->heapID);
  if( area != 0 ){
    Earth_StrPrint(wk, wk->infowin, str2, 0, 16);
  }
  Earth_StrPrint(wk, wk->infowin, str1, 0, 0);
  GFL_STR_DeleteBuffer(str2);
  GFL_STR_DeleteBuffer(str1);

  BmpWinFrame_Write(wk->infowin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->infowin);
}

static void Earth_MyPlaceInfoWinRelease( EARTH_DEMO_WORK* wk )
{
  BmpWinFrame_Clear( wk->infowin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete(wk->infowin);
}

// ��ԋ߂��n�_��\������悤�ɕύX tomoya 08.04.03
//-----------------------------------------------------------------------------
/**
 *  @brief  ��ԋ߂��n�_������
 *
 *  @param  EARTH_DEMO_WORK* wk 
 *
 *  @retval �����������ۂ�
 *
 *  @note tomoya���� 08.04.03�ɍ�������̂�hosaka���֐��� 09.03.19
 */
//-----------------------------------------------------------------------------
static BOOL Earth_SearchPosInfo( EARTH_DEMO_WORK* wk, u32* outIndex )
{  
  int   i;
  BOOL  search_result = FALSE;
  s16   minx = (s16)(wk->rotate.x - 0x80);
  s16   maxx = (s16)(wk->rotate.x + 0x80);
  s32   miny = (s16)(wk->rotate.y - 0x80);
  s32   maxy = (s16)(wk->rotate.y + 0x80);
  s32   miny2;
  s32   maxy2;
  u32   mindist   = 0x80*2;
  u32   minindex  = wk->placelist.listcount;
  u32   dist;
  Vec2DS32 rot_a, rot_b;

  rot_a.x = wk->rotate.x;
  rot_a.y = wk->rotate.y;
  WIFI_MinusRotateChange( &rot_a );

  // Y�͓r���ŕ������ς��̂łQ�p�^�[���͈̔͂�����
  if( MATH_ABS( maxy - miny ) > (0x80*2) )
  {
    miny2 = (s16)wk->rotate.y;
    maxy2 = (s16)wk->rotate.y;
    if( miny2 > 0 )
    {
      miny = -0x10000 + miny2;
      maxy = -0x10000 + maxy2;
    }
    else
    {
      miny = miny2 + 0x10000;
      maxy = maxy2 + 0x10000;
    }
    
    miny2 -= 0x80;
    maxy2 += 0x80;
    miny  -= 0x80;
    maxy  += 0x80;
  }
  else
  {
    miny2 = miny;
    maxy2 = maxy;
  }

//  OS_TPrintf( "minx=%d maxx=%d miny=%d maxy=%d miny2=%d maxy2=%d\n", 
//                minx, maxx, miny, maxy, miny2, maxy2 );

  for(i=0;i<wk->placelist.listcount;i++){
    if( (wk->placelist.place[i].x > minx)&&(wk->placelist.place[i].x < maxx) && 
      ( (wk->placelist.place[i].y > miny)&&(wk->placelist.place[i].y < maxy) || (wk->placelist.place[i].y > miny2)&&(wk->placelist.place[i].y < maxy2) ) &&
       (wk->placelist.place[i].col != MARK_NULL) ){

      rot_b.x = wk->placelist.place[i].x;
      rot_b.y = wk->placelist.place[i].y;
      WIFI_MinusRotateChange( &rot_b );

      dist =WIFI_EarthGetRotateDist( &rot_a, &rot_b );
      if( dist < mindist ){
        mindist   = dist;
        minindex  = i;
      }
    }
  }
  
  *outIndex = minindex;

  if( minindex != wk->placelist.listcount ){
    search_result = TRUE;
  }

  return search_result;
}


//----------------------------------
//�n���\��
//----------------------------------
static void Earth_PosInfoPut( EARTH_DEMO_WORK* wk )
{
  if( wk->info_mode == 0 ){
    {
      STRBUF* msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
      GFL_MSG_GetString(wk->msg_man, mes_earth_03_02, msgstr);
      Earth_StrPrint(wk, wk->msgwin, msgstr, 0, 0);
      GFL_STR_DeleteBuffer( msgstr );
    }
  } else {
    u32 minindex;

    //�J�[�\���ʒu�n���\��  
    if( Earth_SearchPosInfo( wk, &minindex ) ) {
      STRBUF* str1 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, wk->heapID );
      STRBUF* str2 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, wk->heapID );

      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
      WIFI_NationAreaNameGet( wk->placelist.place[minindex].nationID,
                  wk->placelist.place[minindex].areaID,
                  str1,str2,wk->heapID);
      if( wk->placelist.place[minindex].areaID != 0 ){
        Earth_StrPrint(wk, wk->msgwin, str2, 0, 16);
      }
      Earth_StrPrint(wk, wk->msgwin, str1, 0, 0);
      GFL_STR_DeleteBuffer(str2);
      GFL_STR_DeleteBuffer(str1);

      // �����Ă���ʒu��n��̈ʒu�ɂ���
      wk->rotate.x = wk->placelist.place[minindex].x;
      wk->rotate.y = wk->placelist.place[minindex].y;
    }else{
      wk->info_mode = 0;
    }
  }
  GFL_BMPWIN_TransVramCharacter(wk->msgwin);
}

//============================================================================================
//
//  ���[�J���֐��i�R�c�֘A�j
//
//============================================================================================
typedef struct {
  u16 idx;
  u16 datID;
}MARK_DATA;
static const MARK_DATA mark_array_data[] = {
  { MARK_RED,     NARC_wifi_earth_earth_mark_r_nsbmd },
  { MARK_GREEN,   NARC_wifi_earth_earth_mark_g_nsbmd },
  { MARK_BLUE,    NARC_wifi_earth_earth_mark_b_nsbmd },
  { MARK_YELLOW,  NARC_wifi_earth_earth_mark_y_nsbmd },
};
//----------------------------------
//�R�c�f�[�^���[�h�֐����R�c�f�[�^�J���֐�
//----------------------------------
static void Earth_ModelLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle )
{
  u32 arcID = ARCID_WIFI_EARTH;
  int i;

  //�n�����f��
  wk->g3DresEarth = GFL_G3D_CreateResourceArc(arcID, NARC_wifi_earth_wifi_earth_nsbmd);
  GFL_G3D_TransVramTexture( wk->g3DresEarth );
  wk->g3DrndEarth = GFL_G3D_RENDER_Create(wk->g3DresEarth, 0, wk->g3DresEarth); 
  wk->g3DobjEarth = GFL_G3D_OBJECT_Create(wk->g3DrndEarth, NULL, 0); 

  //�n�_�}�[�N���f��
  for(i=0; i<NELEMS(mark_array_data); i++){
    u16 p = mark_array_data[i].idx;
    wk->g3DresMark[p] = GFL_G3D_CreateResourceArc(arcID, mark_array_data[i].datID);
    GFL_G3D_TransVramTexture( wk->g3DresMark[p] );
    wk->g3DrndMark[p] = GFL_G3D_RENDER_Create(wk->g3DresMark[p], 0, wk->g3DresMark[p]); 
    wk->g3DobjMark[p] = GFL_G3D_OBJECT_Create(wk->g3DrndMark[p], NULL, 0); 
  }

  // �J�����E���C�g�Z�b�g�A�b�v
  EarthCameraInit( wk );
  EarthLightInit( wk );
}

static void Earth_ModelRelease( EARTH_DEMO_WORK * wk )
{
  int i;

  GFL_G3D_CAMERA_Delete( wk->g3Dcamera );
  GFL_G3D_LIGHT_Delete( wk->g3Dlightset );

  GFL_G3D_OBJECT_Delete( wk->g3DobjEarth ); 
  GFL_G3D_RENDER_Delete( wk->g3DrndEarth ); 
  GFL_G3D_FreeVramTexture( wk->g3DresEarth );
  GFL_G3D_DeleteResource( wk->g3DresEarth );

  for(i=0; i<NELEMS(mark_array_data); i++){
    u16 p = mark_array_data[i].idx;
    GFL_G3D_OBJECT_Delete( wk->g3DobjMark[p] ); 
    GFL_G3D_RENDER_Delete( wk->g3DrndMark[p] ); 
    GFL_G3D_FreeVramTexture( wk->g3DresMark[p] );
    GFL_G3D_DeleteResource( wk->g3DresMark[p] );
  }
}

//----------------------------------
//���f���֘A�f�[�^�������i�n�_�f�[�^���[�h����j
//----------------------------------
static void EarthDataInit( EARTH_DEMO_WORK * wk )
{
  {//�n���̍��W�������i�n�_�}�[�N�Ƌ��L�j
    
    wk->trans.x = INIT_EARTH_TRANS_XVAL; 
    wk->trans.y = INIT_EARTH_TRANS_YVAL; 
    wk->trans.z = INIT_EARTH_TRANS_ZVAL; 
  }
  {//�n���̃X�P�[��������
    wk->scale.x = INIT_EARTH_SCALE_XVAL;
    wk->scale.y = INIT_EARTH_SCALE_YVAL;
    wk->scale.z = INIT_EARTH_SCALE_ZVAL;
  }
  {//�n���̉�]������
    wk->rotate.x  = INIT_EARTH_ROTATE_XVAL;
    wk->rotate.y  = INIT_EARTH_ROTATE_YVAL;
    wk->rotate.z  = INIT_EARTH_ROTATE_ZVAL;
  }
  {//�n�_�}�[�N�X�P�[��������
    wk->mark_scale.x  = INIT_EARTH_SCALE_XVAL;
    wk->mark_scale.y  = INIT_EARTH_SCALE_YVAL;
    wk->mark_scale.z  = INIT_EARTH_SCALE_ZVAL;
  }
}

//----------------------------------
//�J����������
//----------------------------------
static const VecFx32  target_pos = { 
  INIT_CAMERA_TARGET_XVAL, INIT_CAMERA_TARGET_YVAL, INIT_CAMERA_TARGET_ZVAL 
};
static const VecFx32  camera_pos = {
  INIT_CAMERA_POS_XVAL, INIT_CAMERA_POS_YVAL, INIT_CAMERA_POS_ZVAL 
};

static void EarthCameraInit( EARTH_DEMO_WORK * wk )
{
  wk->g3Dcamera = GFL_G3D_CAMERA_CreateDefault( &camera_pos, &target_pos, wk->heapID );
  GFL_G3D_CAMERA_Switching(wk->g3Dcamera);
}

static void EarthCameraStart( EARTH_DEMO_WORK * wk )
{
  fx32 near = INIT_CAMERA_CLIP_NEAR;
  fx32 far = INIT_CAMERA_CLIP_FAR;

  GFL_G3D_CAMERA_SetPos( wk->g3Dcamera, &camera_pos );
  GFL_G3D_CAMERA_SetTarget( wk->g3Dcamera, &target_pos );
  GFL_G3D_CAMERA_SetfovySin( wk->g3Dcamera, FX_SinIdx( INIT_CAMERA_PERSPWAY ) );
  GFL_G3D_CAMERA_SetfovyCos( wk->g3Dcamera, FX_CosIdx( INIT_CAMERA_PERSPWAY ) );
  GFL_G3D_CAMERA_SetNear( wk->g3Dcamera, &near );
  GFL_G3D_CAMERA_SetFar( wk->g3Dcamera, &far );

  if(wk->earth_mode == JAPAN_MODE){
    //�J���������t���O�������i�J�n���͋ߋ����j
    wk->camera_status = CAMERA_NEAR;
  }else{
    //�J���������t���O�������i�J�n���͉������j
    wk->camera_status = CAMERA_FAR;
  }
  while(1){
    //�J�����ݒ�
    if(Earth3D_CameraMoveNearFar(wk) == TRUE){
      break;
    }
  }
}

//----------------------------------
//���C�g������
//----------------------------------
static const GFL_G3D_LIGHT_DATA lightTbl[] = {
  { USE_LIGHT_NUM, 
    {{INIT_LIGHT_ANGLE_XVAL, INIT_LIGHT_ANGLE_YVAL, INIT_LIGHT_ANGLE_ZVAL}, GX_RGB(31,31,31) } 
  },
};
static const GFL_G3D_LIGHTSET_SETUP lightSetup = { lightTbl, NELEMS(lightTbl) };

static void EarthLightInit( EARTH_DEMO_WORK * wk )
{
  wk->g3Dlightset = GFL_G3D_LIGHT_Create( &lightSetup, wk->heapID );
  GFL_G3D_LIGHT_Switching(wk->g3Dlightset);
}

//----------------------------------
//�R�c��]�v�Z
//----------------------------------
static void  EarthVecFx32_to_MtxFx33( MtxFx33* dst, VecFx32* src )
{
  MtxFx33 tmp;

  MTX_RotY33( dst,FX_SinIdx((u16)src->y),FX_CosIdx((u16)src->y));

  MTX_RotX33( &tmp,FX_SinIdx((u16)src->x),FX_CosIdx((u16)src->x));
  MTX_Concat33(dst,&tmp,dst);

  MTX_RotZ33( &tmp,FX_SinIdx((u16)src->z),FX_CosIdx((u16)src->z));
  MTX_Concat33(dst,&tmp,dst);
}

//----------------------------------
//�R�c��]�v�Z(����)
//----------------------------------
static void  EarthVecFx32_to_MtxFx33_place( MtxFx33* dst, VecFx32* src )
{
  MtxFx33 tmp;

  MTX_RotY33( dst,FX_SinIdx((u16)src->x),FX_CosIdx((u16)src->x));

  MTX_RotX33( &tmp,FX_SinIdx((u16)-src->y),FX_CosIdx((u16)-src->y));
  MTX_Concat33(dst,&tmp,dst);

  MTX_RotZ33( &tmp,FX_CosIdx((u16)src->z),FX_SinIdx((u16)src->z));
  MTX_Concat33(dst,&tmp,dst);
}

//----------------------------------
//�n������֐�
//----------------------------------
static BOOL Earth3D_Control( EARTH_DEMO_WORK * wk,int keytrg,int keycont )
{
  u16 rotate_speed_x;
  u16 rotate_speed_y;
  s16 rotate_x;
  s16 rotate_y;
  BOOL result = FALSE;

  rotate_x = wk->rotate.x;
  rotate_y = wk->rotate.y;

  //�J�������߈ړ�����i���E�n���V���[�h�̂݁j
  if((keytrg & PAD_BUTTON_A)||(wk->tp_result & PAD_BUTTON_A)){
    if(wk->earth_mode == GLOBAL_MODE){
      if(wk->camera_status == CAMERA_FAR){
        wk->camera_status = CAMERA_NEAR;
      }else{
        wk->camera_status = CAMERA_FAR;
      }
    }
    result = TRUE;
    return result;
  }
  //�J�����ړ��X�s�[�h�����ݒ�
  // �J�����ړ��X�s�[�h�����������ύX tomoya 08.04.03
  if(wk->camera_status == CAMERA_FAR){
    //������
    if((wk->tp_lenx)||(wk->tp_leny)){ //�^�b�`�p�l�����͂���
      rotate_speed_x = CAMERA_MOVE_SPEED_FAR/6 * wk->tp_lenx;
      rotate_speed_y = CAMERA_MOVE_SPEED_FAR/6 * wk->tp_leny;
    }else{
      rotate_speed_x = CAMERA_MOVE_SPEED_FAR;
      rotate_speed_y = CAMERA_MOVE_SPEED_FAR;
    }
  }else{
    //�ߋ���
    if((wk->tp_lenx)||(wk->tp_leny)){ //�^�b�`�p�l�����͂���
      rotate_speed_x = CAMERA_MOVE_SPEED_NEAR/3 * wk->tp_lenx;
      rotate_speed_y = CAMERA_MOVE_SPEED_NEAR/3 * wk->tp_leny;
    }else{
      rotate_speed_x = CAMERA_MOVE_SPEED_NEAR;
      rotate_speed_y = CAMERA_MOVE_SPEED_NEAR;
    }
  }
  //�J�����㉺���E�ړ�����
  if((keycont & PAD_KEY_LEFT)||(wk->tp_result & PAD_KEY_LEFT)){
    if(wk->earth_mode == GLOBAL_MODE){
      wk->rotate.y += rotate_speed_x;
    }else{
      //if((u16)wk->rotate.y < EARTH_LIMIT_ROTATE_YMAX){
      if(rotate_y < (s16)EARTH_LIMIT_ROTATE_YMAX){
        wk->rotate.y += rotate_speed_x;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_RIGHT)||(wk->tp_result & PAD_KEY_RIGHT)){
    if(wk->earth_mode == GLOBAL_MODE){
      wk->rotate.y -= rotate_speed_x;
    }else{
      //if((u16)wk->rotate.y > EARTH_LIMIT_ROTATE_YMIN){
      if(rotate_y > (s16)EARTH_LIMIT_ROTATE_YMIN){
        wk->rotate.y -= rotate_speed_x;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_UP)||(wk->tp_result & PAD_KEY_UP)){
    if(wk->earth_mode == GLOBAL_MODE){
      //if(((u16)(wk->rotate.x + rotate_speed_y)) < CAMERA_ANGLE_MAX){
      if((rotate_x + rotate_speed_y) < CAMERA_ANGLE_MAX){
        wk->rotate.x += rotate_speed_y;
      }else{
        wk->rotate.x = CAMERA_ANGLE_MAX;
      }
    }else{
      //if((u16)wk->rotate.x < EARTH_LIMIT_ROTATE_XMAX){
      if(rotate_x < (s16)EARTH_LIMIT_ROTATE_XMAX){
        wk->rotate.x += rotate_speed_y;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_DOWN)||(wk->tp_result & PAD_KEY_DOWN)){
    if(wk->earth_mode == GLOBAL_MODE){
      //if(((u16)(wk->rotate.x - rotate_speed_y)) > CAMERA_ANGLE_MIN){
      if((rotate_x - rotate_speed_y) > CAMERA_ANGLE_MIN){
        wk->rotate.x -= rotate_speed_y;
      }else{
        wk->rotate.x = CAMERA_ANGLE_MIN;
      }
    }else{
      //if((u16)wk->rotate.x > EARTH_LIMIT_ROTATE_XMIN){
      if(rotate_x > (s16)EARTH_LIMIT_ROTATE_XMIN){
        wk->rotate.x -= rotate_speed_y;
      }
    }
    result = TRUE;
  }

#ifdef PM_DEBUG
  if(result){
    OS_TPrintf("rot_x=0x%x, rot_y=0x%x\n", wk->rotate.x, wk->rotate.y);
  }
#endif
  return result;
}

//----------------------------------
//�J�������߈ړ��֐�
//----------------------------------
static BOOL Earth3D_CameraMoveNearFar( EARTH_DEMO_WORK * wk )
{
  VecFx32 camPos, camTarget, vec;
  fx32 distance;
  BOOL result = FALSE;

  GFL_G3D_CAMERA_GetPos( wk->g3Dcamera, &camPos );
  GFL_G3D_CAMERA_GetTarget( wk->g3Dcamera, &camTarget );
  VEC_Subtract(&camPos, &camTarget, &vec);
  distance = VEC_Mag(&vec);   // �����擾
  VEC_Normalize(&vec, &vec);  // �����P�ʃx�N�g���擾

  switch(wk->camera_status){

  case CAMERA_NEAR://�߂Â�
    if(distance > (INIT_CAMERA_DISTANCE_NEAR + CAMERA_INOUT_SPEED)){
      distance -= CAMERA_INOUT_SPEED;
      wk->mark_scale.x -= MARK_SCALE_INCDEC;
      wk->mark_scale.y = wk->mark_scale.x;
    }else{
      distance = INIT_CAMERA_DISTANCE_NEAR;
      result = TRUE;
    }
    break;

  case CAMERA_FAR://��������
    if(distance < (INIT_CAMERA_DISTANCE_FAR - CAMERA_INOUT_SPEED)){
      distance += CAMERA_INOUT_SPEED;
      wk->mark_scale.x += MARK_SCALE_INCDEC;
      wk->mark_scale.y = wk->mark_scale.x;
    }else{
      distance = INIT_CAMERA_DISTANCE_FAR;
      result = TRUE;
    }
    break;
  }
  VEC_MultAdd(distance, &vec, &camTarget, &camPos); 
  GFL_G3D_CAMERA_SetPos( wk->g3Dcamera, &camPos );

  return result;
}

//----------------------------------
//�R�c�`��֐�
//----------------------------------
static void Earth3D_Draw( EARTH_DEMO_WORK * wk )
{
  MtxFx33 rotate_world = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};

  //�`��t���O����
  switch(wk->Draw3Dsw){

  case DRAW3D_DISABLE:
    break;

  case DRAW3D_BANISH:
    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_End();
    wk->Draw3Dsw = DRAW3D_DISABLE;
    break;

  case DRAW3D_ENABLE:
    GFL_G3D_LIGHT_Switching(wk->g3Dlightset);
    GFL_G3D_CAMERA_Switching(wk->g3Dcamera);

    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_SetLookAt();
    {
      GFL_G3D_OBJSTATUS statusEarth;
      statusEarth.trans = wk->trans;
      MTX_Identity33( &statusEarth.rotate );

      //���S�}�[�N�`��i��_�̂��߉�]�Ȃ��j
      statusEarth.scale = wk->mark_scale;
      GFL_G3D_DRAW_DrawObject( wk->g3DobjMark[MARK_GREEN], &statusEarth);
      
      //�n���`��
      statusEarth.scale = wk->scale;
      EarthVecFx32_to_MtxFx33(&statusEarth.rotate, &wk->rotate); //�O���[�o����]�s��擾
      GFL_G3D_DRAW_DrawObject( wk->g3DobjEarth, &statusEarth);
  
      //�n�_�}�[�N�`��
      {
        GFL_G3D_OBJSTATUS statusMark;
        int i;

        statusMark.trans = wk->trans;
        statusMark.scale = wk->mark_scale;

        for(i=0;i<wk->placelist.listcount;i++){
          MTX_Concat33(&wk->placelist.place[i].rotate, &statusEarth.rotate, &statusMark.rotate);
  
          if(wk->placelist.place[i].col != MARK_NULL){
            GFL_G3D_DRAW_DrawObject( wk->g3DobjMark[wk->placelist.place[i].col], &statusMark);
          }
        }
      }
    }
    GFL_G3D_DRAW_End();
    break;
  }
}
  
//============================================================================================
//
//  �O���[�o���֐��i�n��f�[�^�擾�֘A�j
//
//============================================================================================
//============================================================================================
/**
 *  �n��f�[�^�������i�Z�[�u���[�N�̏������j
 *
 * @param 
 * @retval
 */
//============================================================================================
void  WIFI_RegistratonInit( GAMEDATA* gamedata )
{
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gamedata );
  MYSTATUS*           mystatus = GAMEDATA_GetMyStatus( gamedata );
  WIFI_HISTORY*       wh = SaveData_GetWifiHistory( scw );

  WIFIHISTORY_SetMyNationArea(wh, mystatus, 0, 0);
  WIFIHISTORY_SetWorldFlag(wh, FALSE);
}

//============================================================================================
/**
 *  �����擾
 *
 * @param[in] nationID  ���h�c
 * @param[in] areaID    �n��h�c
 * @param[in] nation_str  �����i�[�o�b�t�@�|�C���^
 * @param[in] area_str  �n�於�i�[�o�b�t�@�|�C���^
 * @param[in] heapID    �e���|�����q�[�v�h�c
 *
 * @retval  FALSE:�n�悪�Ȃ����@TRUE:�n�悪���鍑
 */
//============================================================================================
BOOL  WIFI_NationAreaNameGet
      (int nationID,int areaID, STRBUF* nation_str,STRBUF* area_str,int heapID)
{
  GFL_MSGDATA*  msg_man;
  int areaIndex = EarthAreaTableGet(nationID);  //�n��f�[�^�e�[�u���h�m�c�d�w�擾
  int datID;
  BOOL result;

  datID = NARC_message_wifi_place_msg_world_dat;
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, heapID);
  GFL_MSG_GetString(msg_man, nationID, nation_str);//�����擾
  GFL_MSG_Delete(msg_man);

  if(areaIndex == 0){
    //�n�悪���݂��Ȃ����̏ꍇ�u�Ȃ��v�Ƃ�����������ꉞ�Z�b�g
    areaIndex = 1;
    areaID = 0;
    result = FALSE;
  }else{
    result = TRUE;
  }

  datID = WIFI_COUNTRY_DataIndexToPlaceMsgDataID(areaIndex);
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, heapID); 
  GFL_MSG_GetString(msg_man, areaID, area_str);//�n�於�擾
  GFL_MSG_Delete(msg_man);

  return result;
}

//============================================================================================
/**
 *  ���[�J���G���A���݃`�F�b�N
 *
 * @param[in] nationID  ���h�c
 *
 * @retval  FALSE:�n��Ȃ��@TRUE:�n�悠��
 */
//============================================================================================
BOOL  WIFI_LocalAreaExistCheck(int nationID)
{
  if(EarthAreaTableGet(nationID)){
    return TRUE;
  }else{
    return FALSE;
  }
}




//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�i�X�̒l�̉�]�p�x��R�U�O�x�ȏ�̉�]�p�x���Ȃ���
 *
 *  @param  p_rot ��]�x�N�g��
 */
//-----------------------------------------------------------------------------
static void WIFI_MinusRotateChange( Vec2DS32* p_rot )
{
  if( p_rot->x >= 0 ){
    p_rot->x = p_rot->x % 0xffff;
  }else{
    p_rot->x = p_rot->x + ( 0xffff* ((MATH_ABS( p_rot->x )/0xffff)+1) );
  }
  if( p_rot->y >= 0 ){
    p_rot->y = p_rot->y % 0xffff;
  }else{
    p_rot->y = p_rot->y + ( 0xffff* ((MATH_ABS( p_rot->y )/0xffff)+1) );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  ��]�x�N�g���̋��������߂�
 *
 *  @param  cp_earth    �n���V�̉�]�x�N�g��
 *  @param  cp_place    �n��̉�]�x�N�g��
 *
 *  @return �����iu32�P�ʁj
 */
//-----------------------------------------------------------------------------
static u32 WIFI_EarthGetRotateDist( const Vec2DS32* cp_earth, const Vec2DS32* cp_place )
{
  s32 dif_x, dif_y;
  u32 dist;

  // �������߂��ق���I��
  dif_x = MATH_ABS(cp_earth->x - cp_place->x);
  dif_y = MATH_ABS(cp_earth->y - cp_place->y);

  if( dif_x > GFL_CALC_RotKey(180) ){
    dif_x = 0xffff - dif_x;
  }
  if( dif_y > GFL_CALC_RotKey(180) ){
    dif_y = 0xffff - dif_y;
  }
  dist = FX_Sqrt( ((dif_x*dif_x)+(dif_y*dif_y)) << FX32_SHIFT ) >> FX32_SHIFT;

  return dist;
}


//----------------------------------
// �f�o�b�O
//----------------------------------
#ifdef WIFI_ERATH_DEBUG
static void EarthDebugWinRotateInfoWrite( EARTH_DEMO_WORK * wk )
    
{
  STRBUF* str1 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);
  STRBUF* str2 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);

  STRBUF_SetHexNumber( str1, (u16)wk->rotate.x, 4, 
            NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
  STRBUF_SetHexNumber( str2, (u16)wk->rotate.y, 4, 
            NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
  Earth_StrPrint(wk, wk->msgwin, str1, 0, 0);
  Earth_StrPrint(wk, wk->msgwin, str2, 0, 16);

  GFL_STR_DeleteBuffer(str2);
  GFL_STR_DeleteBuffer(str1);
}

static void EarthDebugWinNameInfoWrite( EARTH_DEMO_WORK * wk )
    
{
  STRBUF  *str1,*str2;
  int i;

  if(!wk->debug_work[0]){
    for(i=0;i<wk->placelist.listcount;i++){
      if((wk->placelist.place[i].x == (s16)wk->rotate.x)
        &&(wk->placelist.place[i].y == (s16)wk->rotate.y)){
  
        str1 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, HEAPID_BASE_DEBUG);
        str2 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, HEAPID_BASE_DEBUG);

        WIFI_NationAreaNameGet( wk->placelist.place[i].nationID,
                    wk->placelist.place[i].areaID,
                    str1,str2,wk->heapID);
        Earth_StrPrint(wk, wk->msgwin, str1, 16*4, 0);
        Earth_StrPrint(wk, wk->msgwin, str2, 16*4, 16);

        GFL_STR_DeleteBuffer(str2);
        GFL_STR_DeleteBuffer(str1);

        wk->debug_work[0] = 1;
      }
    }
  }else{
    str1 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);
    str2 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);

    GF_BGL_BmpWinFill(&wk->msgwin,FBMP_COL_WHITE,0,0,
              EARTH_MSG_WIN_SX*DOTSIZE,EARTH_MSG_WIN_SY*DOTSIZE);
    STRBUF_SetHexNumber( str1, (u16)wk->rotate.x, 4, 
              NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
    STRBUF_SetHexNumber( str2, (u16)wk->rotate.y, 4, 
              NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
    Earth_StrPrint(wk, wk->msgwin, str1, 0, 0);
    Earth_StrPrint(wk, wk->msgwin, str2, 0, 16);

    GFL_STR_DeleteBuffer(str2);
    GFL_STR_DeleteBuffer(str1);

    wk->debug_work[0] = 0;
  }
}

static void EarthDebugNationMarkSet( EARTH_DEMO_WORK * wk )
{
  u16 pattern_flag = WIFIHIST_STAT_NODATA;
  u16 cursor_pos;
  int nationID;

  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_NEW;
  }
  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_EXIST;
  }
  if(pattern_flag == WIFIHIST_STAT_NODATA){
    return;
  }
  BmpListDirectPosGet(wk->bmplist,&cursor_pos);

  nationID = cursor_pos+1;  //1origin

  if(WIFI_LocalAreaExistCheck(nationID) == FALSE){
    //�n�悪���݂��Ȃ��ꍇ�̂ݐݒ�
    WIFIHISTORY_SetStat(wk->wifi_sv,nationID,0,pattern_flag);
  }
  EarthListLoad(wk);  //reload
}

static void EarthDebugAreaMarkSet( EARTH_DEMO_WORK * wk )
{
  u16 pattern_flag = WIFIHIST_STAT_NODATA;
  u16 cursor_pos;
  int areaID;

  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_NEW;
  }
  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_EXIST;
  }
  if(pattern_flag == WIFIHIST_STAT_NODATA){
    return;
  }
  BmpListDirectPosGet(wk->bmplist,&cursor_pos);

  areaID = cursor_pos+1;  //1origin

  WIFIHISTORY_SetStat(wk->wifi_sv,wk->my_nation_tmp,areaID,pattern_flag);
  EarthListLoad(wk);  //reload
}

#endif




















