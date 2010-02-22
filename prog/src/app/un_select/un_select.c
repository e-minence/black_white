//=============================================================================
/**
 *
 *  @file   un_select.c
 *  @brief  ���A �t���A�I��
 *  @author genya hosaka
 *  @data   2010.02.05
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/bmp_winframe.h" // for BmpWinFrame_
#include "system/brightness.h" // for

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"

#include "gamesystem/msgspeed.h"  // for MSGSPEED_GetWait

// ������֘A
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h"
#include "font/font.naix"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "un_select_graphic.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

#include "ui/print_tool.h" // for 

//�^�b�`�o�[
#include "ui/touchbar.h"

// BG�t���[�����X�g
#include "ui/frame_list.h"

// �V�[���R���g���[���[
#include "ui/ui_scene.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "app/un_select.h"

#include "message.naix"
#include "un_select_gra.naix" // �A�[�J�C�u

#include "savedata/wifihistory.h" // for WIFIHISTORY_CheckCountryBit, WIFI_COUNTRY_MAX

// �T�E���h
#include "sound/pm_sndsys.h"

#include "msg/msg_wifi_place_msg_UN.h"  // GMM

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define UN_SELECT_TOUCHBAR

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================

//--------------------------------------------------------------
/// �t���A�e�[�u��
//==============================================================
static const int g_FloorTable[]={
  country001, //�A�t�K�j�X�^��
  country093, //�A�C�X�����h
  country098, //�A�C�������h
  country014, //�A�[���o�C�W����
  country220, //�A�����J
  country004, //�A�����J��傤�T���A
  country221, //�A�����J��傤�o�[�W������Ƃ�
  country218, //�A���u���タ�傤����
  country003, //�A���W�F���A
  country009, //�A���[���`��
  country011, //�A���o
  country002, //�A���o�j�A
  country010, //�A�����j�A
  country007, //�A���M��
  country006, //�A���S��
  country008, //�A���e�B�O�A�E�o�[�u�[�_
  country005, //�A���h��
  country231, //�C�G����
  country219, //�C�M���X
  country029, //�C�M���X��傤�o�[�W������Ƃ�
  country100, //�C�X���G��
  country101, //�C�^���A
  country097, //�C���N
  country096, //�C����
  country094, //�C���h
  country095, //�C���h�l�V�A
  country216, //�E�K���_
  country217, //�E�N���C�i
  country223, //�E�Y�x�L�X�^��
  country222, //�E���O�A�C
  country060, //�G�N�A�h��
  country061, //�G�W�v�g
  country065, //�G�X�g�j�A
  country066, //�G�`�I�s�A
  country064, //�G���g���A
  country062, //�G���T���o�h��
  country012, //�I�[�X�g�����A
  country013, //�I�[�X�g���A
  country157, //�I�}�[��
  country146, //�I�����_
  country147, //�I�����_��傤�A���e�B��
  country078, //�K�[�i
  country037, //�J�[�{�x���f
  country088, //�K�C�A�i
  country106, //�J�U�t�X�^��
  country105, //�J�V�~�[��
  country169, //�J�^�[��
  country036, //�J�i�_
  country074, //�K�{��
  country035, //�J�����[��
  country110, //���񂱂�
  country075, //�K���r�A
  country034, //�J���{�W�A
  country109, //�������傤����
  country155, //�����}���A�i����Ƃ�
  country086, //�M�j�A
  country087, //�M�j�A�r�T�E
  country054, //�L�v���X
  country053, //�L���[�o
  country080, //�M���V��
  country108, //�L���o�X
  country112, //�L���M�X
  country085, //�O�A�e�}��
  country083, //�O�A�h���[�v�Ƃ�
  country084, //�O�A��
  country111, //�N�E�F�[�g
  country049, //�N�b�N����Ƃ�
  country081, //�O���[�������h
  country044, //�N���X�}�X�Ƃ�
  country076, //�O���W�A
  country082, //�O���i�_
  country052, //�N���A�`�A
  country038, //�P�C�}������Ƃ�
  country107, //�P�j�A
  country051, //�R�[�g�W�{���[��
  country050, //�R�X�^���J
  country046, //�R����
  country045, //�R�����r�A
  country048, //�R���S���傤�킱��
  country047, //�R���S�݂񂵂カ�傤�킱��
  country182, //�T�E�W�A���r�A
  country179, //�T���A
  country181, //�T���g���E�v�����V�y
  country232, //�U���r�A
  country177, //�T���s�G�[���E�~�N����
  country180, //�T���}���m
  country186, //�V�G�����I�l
  country057, //�W�u�`
  country079, //�W�u�����^��
  country102, //�W���}�C�J
  country201, //�V���A
  country187, //�V���K�|�[��
  country233, //�W���o�u�G
  country200, //�X�C�X
  country199, //�X�E�F�[�f��
  country195, //�X�[�_��
  country197, //�X�o�[���o���E�����}���G���Ƃ�
  country193, //�X�y�C��
  country196, //�X���i��
  country194, //�X�������J
  country188, //�X���o�L�A
  country189, //�X���x�j�A
  country198, //�X���W�����h
  country185, //�Z�[�V�F��
  country063, //�����ǂ��M�j�A
  country183, //�Z�l�K��
  country184, //�Z���r�A�E�����e�l�O��
  country175, //�Z���g�N���X�g�t�@�[�E�l�[�r�X
  country178, //�Z���g�r���Z���g�E�O���i�f�B�[��
  country174, //�Z���g�w���i�Ƃ�
  country176, //�Z���g���V�A
  country191, //�\�}���A
  country190, //�\����������Ƃ�
  country214, //�^�[�N�X�E�J�C�R�X����Ƃ�
  country205, //�^�C
  country202, //�������
  country203, //�^�W�L�X�^��
  country204, //�^���U�j�A
  country055, //�`�F�R
  country040, //�`���h
  country041, //�`���l������Ƃ�
  country039, //���イ�����A�t���J
  country043, //���イ����
  country211, //�`���j�W�A
  country042, //�`��
  country215, //�c�o��
  country056, //�f���}�[�N
  country077, //�h�C�c
  country207, //�g�[�S
  country208, //�g�P���E
  country058, //�h�~�j�J
  country059, //�h�~�j�J���傤�킱��
  country210, //�g���j�_�[�h�E�g�o�S
  country213, //�g���N���j�X�^��
  country212, //�g���R
  country209, //�g���K
  country152, //�i�C�W�F���A
  country144, //�i�E��
  country143, //�i�~�r�A
  country153, //�j�E�G
  country150, //�j�J���O�A
  country151, //�j�W�F�[��
  country230, //�ɂ��T�n��
  country103, //�ɂق�
  country148, //�j���[�J���h�j�A
  country149, //�j���[�W�[�����h
  country145, //�l�p�[��
  country154, //�m�[�t�H�[�N�Ƃ�
  country156, //�m���E�F�[
  country016, //�o�[���[��
  country089, //�n�C�`
  country158, //�p�L�X�^��
  country225, //�o�`�J��
  country160, //�p�i�}
  country224, //�o�k�A�c
  country015, //�o�n�}
  country161, //�p�v�A�j���[�M�j�A
  country023, //�o�~���[�_
  country159, //�p���I
  country162, //�p���O�A�C
  country018, //�o���o�h�X
  country229, //�p���X�`�i������
  country092, //�n���K���[
  country017, //�o���O���f�V��
  country206, //�Ђ����e�B���[��
  country165, //�s�g�P�A��
  country069, //�t�B�W�[����Ƃ�
  country164, //�t�B���s��
  country070, //�t�B�������h
  country024, //�u�[�^��
  country168, //�v�G���g���R
  country067, //�t�F���[����Ƃ�
  country068, //�t�H�[�N�����h�i�}���r�i�X)����Ƃ�
  country028, //�u���W��
  country071, //�t�����X
  country072, //�t�����X��傤�M�A�i
  country073, //�t�����X��傤�|���l�V�A
  country031, //�u���K���A
  country032, //�u���L�i�t�@�\
  country030, //�u���l�C
  country033, //�u�����W
  country227, //�x�g�i��
  country022, //�x�i��
  country226, //�x�l�Y�G��
  country019, //�x�����[�V
  country021, //�x���[�Y
  country163, //�y���[
  country020, //�x���M�[
  country166, //�|�[�����h
  country026, //�{�X�j�A�E�w���c�F�S�r�i
  country027, //�{�c���i
  country025, //�{���r�A
  country167, //�|���g�K��
  country091, //�z���R��
  country090, //�z���W�����X
  country130, //�}�[�V��������Ƃ�
  country122, //�}�J�I
  country123, //�}�P�h�j�A���イ���[�S�X���r�A
  country124, //�}�_�K�X�J��
  country134, //�}���b�g
  country125, //�}���E�C
  country128, //�}��
  country129, //�}���^
  country131, //�}���`�j�[�N
  country126, //�}���[�V�A
  country099, //�}���Ƃ�
  country136, //�~�N���l�V�A
  country192, //�݂Ȃ݃A�t���J
  country142, //�~�����}�[
  country135, //���L�V�R
  country133, //���[���V���X
  country132, //���[���^�j�A
  country141, //���U���r�[�N
  country138, //���i�R
  country127, //�����f�B�u
  country137, //�����h�o
  country140, //�����b�R
  country139, //�����S��
  country104, //�����_��
  country113, //���I�X
  country114, //���g�r�A
  country120, //���g�A�j�A
  country118, //���r�A
  country119, //���q�e���V���^�C��
  country117, //���x���A
  country171, //���[�}�j�A
  country121, //���N�Z���u���N
  country173, //�������_
  country116, //���\�g
  country115, //���o�m��
  country170, //�����j�I��
  country172, //���V�A
  country228, //�����X�E�t�e���i����Ƃ�
};


//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================
enum
{ 
  UN_SELECT_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y

  UN_LIST_MAX = WIFI_COUNTRY_MAX, ///< ���ڐ�
};

// �p�b�V�u�萔
#define YESNO_MASK_DISP ( MASK_DOUBLE_DISPLAY )
#define YESNO_MASK_PLANE ( PLANEMASK_BG3 | PLANEMASK_OBJ | PLANEMASK_BD )
#define BRIGHT_PASSIVE_SYNC (8)
#define BRIGHT_PASSIVE_VOL (-8)

// �t�H���g�J���[
#define FCOL_WP01WN   ( PRINTSYS_LSB_Make(14,15,0) )  // �t�H���g�J���[�F����

//���p�O�P����

//--------------------------------------------------------------
/// �A�v���I�����[�h
//==============================================================
typedef enum { 
  END_MODE_DECIDE,
  END_MODE_CANCEL
} END_MODE;

//-------------------------------------
/// �t���[��
//=====================================
enum
{ 
  BG_FRAME_TEXT_M = GFL_BG_FRAME0_M,
  BG_FRAME_MENU_M = GFL_BG_FRAME1_M,
  BG_FRAME_LIST_M = GFL_BG_FRAME2_M,
  BG_FRAME_BACK_M = GFL_BG_FRAME3_M,

  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
  BG_FRAME_LIST_S = GFL_BG_FRAME1_S,
  BG_FRAME_BACK_S = GFL_BG_FRAME2_S,
};

//-------------------------------------
/// �p���b�g
//=====================================
enum
{ 
  //���C��BG
  PLTID_BG_BACK_M       = 0,
  PLTID_BG_TEXT_M       = 1,
  PLTID_BG_TEXT_WIN_M   = 2,
  PLTID_BG_TASKMENU_M   = 11,
  PLTID_BG_TOUCHBAR_M   = 13,
  PLTID_BG_INFOWIN_M    = 15,
  //�T�uBG
  PLTID_BG_BACK_S       = 0,

  //���C��OBJ
  PLTID_OBJ_TOUCHBAR_M  = 0, // 3�{�g�p
  PLTID_OBJ_TYPEICON_M  = 3, // 3�{�g�p
  PLTID_OBJ_OAM_MAPMODEL_M = 6, // 1�{�g�p
  PLTID_OBJ_POKEICON_M = 7,     // 3�{�g�p
  PLTID_OBJ_POKEITEM_M = 10,    // 1�{�g�p
  PLTID_OBJ_ITEMICON_M = 11,
  PLTID_OBJ_POKE_M = 12,
  PLTID_OBJ_BALLICON_M = 13, // 1�{�g�p
  PLTID_OBJ_TOWNMAP_M = 14,   

  //�T�uOBJ
  PLTID_OBJ_PMS_DRAW = 0, // 5�{�g�p
};

//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
/// BG�Ǘ����[�N
//==============================================================
typedef struct 
{
  int dummy;
} UN_SELECT_BG_WORK;

//--------------------------------------------------------------
/// ���b�Z�[�W�Ǘ����[�N
//==============================================================
typedef struct
{ 
  //[IN]
  HEAPID  heap_id;

  //[PRIVATE]
  GFL_TCBLSYS   *tcblsys;     ///< �^�X�N�V�X�e��
  GFL_MSGDATA   *msghandle;   ///< ���b�Z�[�W�n���h��
  GFL_FONT      *font;        ///< �t�H���g
  PRINT_QUE     *print_que;   ///< �v�����g�L���[
  WORDSET       *wordset;     ///< ���[�h�Z�b�g

  STRBUF        *strbuf;
  STRBUF        *exp_strbuf;

  // �X�g���[���Đ�
  PRINT_STREAM* print_stream;
  GFL_BMPWIN    *win_talk;

  STRBUF* name[ UN_LIST_MAX ]; ///< ���O�o�b�t�@

} UN_SELECT_MSG_CNT_WORK;

//--------------------------------------------------------------
/// ���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heap_id;

  UN_SELECT_BG_WORK       wk_bg;

  //�`��ݒ�
  UN_SELECT_GRAPHIC_WORK  *graphic;

#ifdef UN_SELECT_TOUCHBAR
  //�^�b�`�o�[
  TOUCHBAR_WORK             *touchbar;
  //�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
  u32                       ncg_btn;
  u32                       ncl_btn;
  u32                       nce_btn;
#endif //UN_SELECT_TOUCHBAR
  
  //�^�X�N���j���[
  APP_TASKMENU_RES          *menu_res;
  APP_TASKMENU_WORK         *menu;

  UN_SELECT_PARAM* pwk;

  UI_SCENE_CNT_PTR cnt_scene;

  UN_SELECT_MSG_CNT_WORK* cnt_msg;

  FRAMELIST_WORK* lwk;
  
} UN_SELECT_MAIN_WORK;

//=============================================================================
/**
 *              �V�[����`
 */
//=============================================================================

// ���X�g����
static BOOL SceneListMake( UI_SCENE_CNT_PTR cnt, void* work );

// �t���A�I��
static BOOL SceneSelectFloor_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneSelectFloor_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneSelectFloor_End( UI_SCENE_CNT_PTR cnt, void* work );

// �m�F���
static BOOL SceneConfirm_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneConfirm_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneConfirm_End( UI_SCENE_CNT_PTR cnt, void* work );

//--------------------------------------------------------------
/// SceneID
//==============================================================
typedef enum
{ 
  UNS_SCENE_ID_LIST_MAKE = 0, ///< ���X�g����
  UNS_SCENE_ID_SELECT_FLOOR,  ///< �t���A�I��
  UNS_SCENE_ID_CONFIRM,       ///< �m�F���

  UNS_SCENE_ID_MAX,
} UNS_SCENE_ID;

//--------------------------------------------------------------
/// SceneFunc
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ UNS_SCENE_ID_MAX ] = 
{
  // UNS_SCENE_ID_LIST_MAKE
  {
    NULL,
    NULL,
    SceneListMake,
    NULL,
    NULL,
  },
  // UNS_SCENE_ID_SELECT_FLOOR
  {
    SceneSelectFloor_Init,
    NULL,
    SceneSelectFloor_Main,
    NULL,
    SceneSelectFloor_End,
  },
  // UNS_SCENE_ID_CONFIRM
  {
    SceneConfirm_Init,
    NULL,
    SceneConfirm_Main,
    NULL,
    SceneConfirm_End,
  },
};

//=============================================================================
/**
 *              �v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
/// PROC
//=====================================
static GFL_PROC_RESULT UNSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
/// �ėp�������[�e�B���e�B
//=====================================
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heap_id );

#ifdef UN_SELECT_TOUCHBAR
//-------------------------------------
/// �^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id );
static void UNSelect_TOUCHBAR_Exit( TOUCHBAR_WORK *touchbar );
static void UNSelect_TOUCHBAR_Main( TOUCHBAR_WORK *touchbar );
#endif //UN_SELECT_TOUCHBAR

//-------------------------------------
/// ���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heap_id );
static void UNSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UNSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu );

//-------------------------------------
// �p�b�V�u
//=====================================
static void PASSIVE_Request( void );
static void PASSIVE_Reset( void );

//-------------------------------------
// ���b�Z�[�W�Ǘ����W���[��
//=====================================
static UN_SELECT_MSG_CNT_WORK* MSG_CNT_Create( HEAPID heap_id );
static void MSG_CNT_Delete( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_Main( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_SetPrint( UN_SELECT_MSG_CNT_WORK* wk, int str_id );
static BOOL MSG_CNT_PrintProc( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_PrintClear( UN_SELECT_MSG_CNT_WORK* wk );
static GFL_FONT* MSG_CNT_GetFont( UN_SELECT_MSG_CNT_WORK* wk );
static PRINT_QUE* MSG_CNT_GetPrintQue( UN_SELECT_MSG_CNT_WORK* wk );
static GFL_MSGDATA* MSG_CNT_GetMsgData( UN_SELECT_MSG_CNT_WORK* wk );

// PROTOTYPE
static UN_SELECT_MAIN_WORK* app_init( GFL_PROC* proc, UN_SELECT_PARAM* prm );
static void app_end( UN_SELECT_MAIN_WORK* wk, END_MODE end_mode );

static void LIST_Make( UN_SELECT_MAIN_WORK* wk );
static void LIST_Delete( UN_SELECT_MAIN_WORK* wk );

//=============================================================================
/**
 *                �O�����J
 */
//=============================================================================
const GFL_PROC_DATA UNSelectProcData = 
{
  UNSelectProc_Init,
  UNSelectProc_Main,
  UNSelectProc_Exit,
};
//=============================================================================
/**
 *                PROC
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC ����������
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          UN_SELECT_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  UN_SELECT_MAIN_WORK *wk;
  
  GF_ASSERT( pwk );

  switch( *seq )
  {
  case 0:
    wk = app_init( proc, pwk );
  
    // �t�F�[�h�C�� ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1 );

    (*seq)++;
    break;

  case 1:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      return GFL_PROC_RES_FINISH;
    }
    break;

  default : GF_ASSERT(0);
  }

  return GFL_PROC_RES_CONTINUE;
}
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �I������
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          UN_SELECT_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  UN_SELECT_MAIN_WORK* wk = mywk;
    
  if( GFL_FADE_CheckFade() == TRUE )
  {
     return GFL_PROC_RES_CONTINUE;
  }
    
  // ���X�g�J��
  LIST_Delete( wk ); 

  // �V�[���R���g�[���폜
  UI_SCENE_CNT_Delete( wk->cnt_scene );

  // ���b�Z�[�W����
  MSG_CNT_Delete( wk->cnt_msg );

  //TASKMENU ���\�[�X�j��
  APP_TASKMENU_RES_Delete( wk->menu_res );  

#ifdef UN_SELECT_TOUCHBAR
  //�^�b�`�o�[
  UNSelect_TOUCHBAR_Exit( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

  //�`��ݒ�j��
  UN_SELECT_GRAPHIC_Exit( wk->graphic );

  //PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heap_id );

  //�I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

  HOSAKA_Printf(" PROC�I���I \n");

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �又��
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          UN_SELECT_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  UN_SELECT_MAIN_WORK* wk = mywk;
  
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cnt_scene ) )
  {
    return GFL_PROC_RES_FINISH;
  }

  // ���b�Z�[�W�又��
  MSG_CNT_Main( wk->cnt_msg );

  //2D�`��
  UN_SELECT_GRAPHIC_2D_Draw( wk->graphic );

  //3D�`��
  UN_SELECT_GRAPHIC_3D_StartDraw( wk->graphic );

  UN_SELECT_GRAPHIC_3D_EndDraw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *                static�֐�
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *  @param  UN_SELECT_BG_WORK* wk BG�Ǘ����[�N
 *  @param  heap_id  �q�[�vID 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heap_id )
{
  ARCHANDLE *handle;

  handle  = GFL_ARC_OpenDataHandle( ARCID_UN_SELECT_GRA, heap_id );

  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0, heap_id );
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0, heap_id );
  
  //  ----- ����� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_un_select_gra_kokuren_bg_NCGR,
            BG_FRAME_BACK_S, 0, 0, 0, heap_id );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_un_select_gra_kokuren_bgu_NSCR,
            BG_FRAME_BACK_S, 0, 0, 0, heap_id );  
  
  //  ----- ���� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_un_select_gra_kokuren_bg_NCGR,
            BG_FRAME_BACK_M, 0, 0, 0, heap_id );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_un_select_gra_kokuren_bgd_NSCR,
            BG_FRAME_BACK_M, 0, 0, 0, heap_id );    
  
  // ----- ���X�g�o�[ -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_un_select_gra_kokuren_bg_listframe_NCGR,
            BG_FRAME_LIST_M, 0, 0, 0, heap_id );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_un_select_gra_kokuren_bg_listframe_NCGR,
            BG_FRAME_LIST_S, 0, 0, 0, heap_id );

  GFL_ARC_CloseDataHandle( handle );
}


#ifdef UN_SELECT_TOUCHBAR
//=============================================================================
/**
 *  TOUCHBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR������
 *
 *  @param  GFL_CLUNIT *clunit  CLUNIT
 *  @param  heap_id             �q�[�vID
 *
 *  @return TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id )
{ 
  TOUCHBAR_WORK* touchbar;

  //�A�C�R���̐ݒ�
  //�������
  TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]  =
  { 
    { 
      TOUCHBAR_ICON_RETURN,
      { TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
    },
    { 
      TOUCHBAR_ICON_CUR_L,
      { TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
    },
    { 
      TOUCHBAR_ICON_CUR_R,
      { TOUCHBAR_ICON_X_02, TOUCHBAR_ICON_Y },
    },
  };

  //�ݒ�\����
  //�����قǂ̑����{���\�[�X���������
  TOUCHBAR_SETUP  touchbar_setup;
  GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

  touchbar_setup.p_item   = touchbar_icon_tbl;        //��̑����
  touchbar_setup.item_num = NELEMS(touchbar_icon_tbl);//�����������邩
  touchbar_setup.p_unit   = clunit;                   //OBJ�ǂݍ��݂̂��߂�CLUNIT
  touchbar_setup.bar_frm  = BG_FRAME_MENU_M;            //BG�ǂݍ��݂̂��߂�BG��
  touchbar_setup.bg_plt   = PLTID_BG_TOUCHBAR_M;      //BG��گ�
  touchbar_setup.obj_plt  = PLTID_OBJ_TOUCHBAR_M;     //OBJ��گ�
  touchbar_setup.mapping  = APP_COMMON_MAPPING_128K;  //�}�b�s���O���[�h

  touchbar =  TOUCHBAR_Init( &touchbar_setup, heap_id );

  TOUCHBAR_SetBGPriorityAll( touchbar, 1 ); // BG�v���C�I���e�B��ݒ�

  return touchbar;
}
//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR�j��
 *
 *  @param  TOUCHBAR_WORK *touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UNSelect_TOUCHBAR_Exit( TOUCHBAR_WORK *touchbar )
{ 
  TOUCHBAR_Exit( touchbar );
}

//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR���C������
 *
 *  @param  TOUCHBAR_WORK *touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UNSelect_TOUCHBAR_Main( TOUCHBAR_WORK *touchbar )
{ 
  TOUCHBAR_Main( touchbar );
}
#endif //UN_SELECT_TOUCHBAR

//----------------------------------------------------------------------------
/**
 *  @brief  TASKMENU�̏�����
 *
 *  @param  menu_res  ���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heap_id )
{ 
  int i;
  APP_TASKMENU_INITWORK init;
  APP_TASKMENU_ITEMWORK item[2];
  APP_TASKMENU_WORK     *menu;  

  //���̐ݒ�
  for( i = 0; i < NELEMS(item); i++ )
  { 
    item[i].str       = GFL_MSG_CreateString( msg, un_reception_msg_02 + i ); //������
    item[i].msgColor  = APP_TASKMENU_ITEM_MSGCOLOR; //�����F
    item[i].type      = APP_TASKMENU_WIN_TYPE_NORMAL; //���̎��
  }

  //������
  init.heapId   = heap_id;
  init.itemNum  = NELEMS(item);
  init.itemWork = item;
  init.posType  = ATPT_RIGHT_DOWN;
  init.charPosX = 32;
  init.charPosY = 18;
  init.w        = APP_TASKMENU_PLATE_WIDTH;
  init.h        = APP_TASKMENU_PLATE_HEIGHT;

  menu  = APP_TASKMENU_OpenMenu( &init, menu_res );

  //��������
  for( i = 0; i < NELEMS(item); i++ )
  { 
    GFL_STR_DeleteBuffer( item[i].str );
  }

  return menu;

}
//----------------------------------------------------------------------------
/**
 *  @brief  TASKMENU�̔j��
 *
 *  @param  APP_TASKMENU_WORK *menu ���[�N
 */
//-----------------------------------------------------------------------------
static void UNSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
{ 
  APP_TASKMENU_CloseMenu( menu );
}
//----------------------------------------------------------------------------
/**
 *  @brief  TASKMENU�̃��C������
 *
 *  @param  APP_TASKMENU_WORK *menu ���[�N
 */
//-----------------------------------------------------------------------------
static void UNSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{ 
  APP_TASKMENU_UpdateMenu( menu );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  �p�b�V�u���N�G�X�g
 *
 *  @param  void 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void PASSIVE_Request( void )
{
  BrightnessChgReset( YESNO_MASK_DISP );
  ChangeBrightnessRequest( BRIGHT_PASSIVE_SYNC, BRIGHT_PASSIVE_VOL, 0, YESNO_MASK_PLANE, YESNO_MASK_DISP );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �p�b�V�u���Z�b�g
 *
 *  @param  void 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void PASSIVE_Reset( void )
{
  BrightnessChgReset( YESNO_MASK_DISP ); // �p�b�V�u����
  ChangeBrightnessRequest( BRIGHT_PASSIVE_SYNC, 0, BRIGHT_PASSIVE_VOL, YESNO_MASK_PLANE, YESNO_MASK_DISP );
}


//=============================================================================
//
//
// ���b�Z�[�W�Ǘ��N���X
//
//
//=============================================================================
enum
{ 
  CGX_BMPWIN_FRAME_POS = 1,
  STRBUF_SIZE = 1600,
};

#define MSG_CNT_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X ����
 *
 *  @param  HEAPID heap_id 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static UN_SELECT_MSG_CNT_WORK* MSG_CNT_Create( HEAPID heap_id )
{
  UN_SELECT_MSG_CNT_WORK* wk;

  // ������ �A���b�N
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof( UN_SELECT_MSG_CNT_WORK ) );

  // �����o������
  wk->heap_id = heap_id;
  
  // ���b�Z�[�W�p�t�H���g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_TEXT_M, 0x20, heap_id );
  
  // �t���[���E�B���h�E�p�̃L������p��
  BmpWinFrame_GraphicSet( BG_FRAME_TEXT_M, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_WIN_M, MENU_TYPE_FIELD, heap_id );
  
  // ���[�h�Z�b�g����
  wk->wordset = WORDSET_Create( heap_id );

  //���b�Z�[�W
  wk->msghandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
      NARC_message_wifi_place_msg_UN_dat, heap_id );

  //PRINT_QUE�쐬
  wk->print_que   = PRINTSYS_QUE_Create( heap_id );

  //�t�H���g�쐬
  wk->font      = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, heap_id );

  // STRBU ����
  wk->strbuf      = GFL_STR_CreateBuffer( STRBUF_SIZE, heap_id );
  wk->exp_strbuf  = GFL_STR_CreateBuffer( STRBUF_SIZE, heap_id );

  wk->tcblsys = GFL_TCBL_Init( heap_id, heap_id, 1, 0 );
  
  // �E�B���h�E����
  wk->win_talk = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 1, 19, 30, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  
  return wk;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X �j��
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_Delete( UN_SELECT_MSG_CNT_WORK* wk )
{
  // �X�g���[���j��
  if( wk->print_stream )
  {
    PRINTSYS_PrintStreamDelete( wk->print_stream );
    wk->print_stream = NULL;
  }

  // BMPWIN �j��
  GFL_BMPWIN_Delete( wk->win_talk );

  // STRBUF �j��
  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_STR_DeleteBuffer( wk->exp_strbuf );
  //���b�Z�[�W�j��
  GFL_MSG_Delete( wk->msghandle );
  //PRINT_QUE
  PRINTSYS_QUE_Delete( wk->print_que );
  //FONT
  GFL_FONT_Delete( wk->font );
  // TCBL
  GFL_TCBL_Exit( wk->tcblsys );
  // ���[�h�Z�b�g �j��
  WORDSET_Delete( wk->wordset );

  // �������j��
  GFL_HEAP_FreeMemory( wk );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X �又��
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_Main( UN_SELECT_MSG_CNT_WORK* wk )
{
  PRINTSYS_QUE_Main( wk->print_que );
  
  GFL_TCBL_Main( wk->tcblsys );
}


// �������Q�s�ɂȂ�ꍇ�̕������̋��E�i����ȏ�͉��s���K�v�E����ȉ��͉��s�͂����Ă��Ȃ��͂��j
#define MULTI_LINE_WORD_NUM ( 10 )

//-----------------------------------------------------------------------------
/**
 *  @brief  ���X�g���ڕ`��(�����`��)
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk
 *  @param  util 
 *  @pawram itemNum
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_DrawListElem( UN_SELECT_MSG_CNT_WORK* wk, PRINT_UTIL* util, PRINT_QUE *que, GFL_FONT *font, u32 itemNum )
{
  int ypos = 12;
  int idx;
  int floor;

  GF_ASSERT(wk);

  // �t���A�\��
  idx = itemNum;
  floor = UN_LIST_MAX - itemNum + 1; //�t���A��2�e����
  GFL_MSG_GetString( wk->msghandle, un_reception_msg_04, wk->strbuf );
  WORDSET_RegisterNumber( wk->wordset, 2, floor, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wordset, wk->exp_strbuf, wk->strbuf );

  PRINTTOOL_PrintColor( util, que, 0, 12, wk->exp_strbuf, font, FCOL_WP01WN, PRINTTOOL_MODE_LEFT );

  // ���� �\��(�P�s�E�Q�s�Ή��j
  if(GFL_STR_GetBufferLength(wk->name[idx]) > MULTI_LINE_WORD_NUM){
    ypos = 4;
  }
  PRINTTOOL_PrintColor( util, que, 32, ypos, wk->name[idx], font, FCOL_WP01WN, PRINTTOOL_MODE_LEFT );

  return;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W�v�����g ���N�G�X�g
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk
 *  @param  str_id 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_SetPrint( UN_SELECT_MSG_CNT_WORK* wk, int str_id )
{
  const u8 clear_color = 15;
  GFL_BMPWIN* win;
  int msgspeed;

  GF_ASSERT(wk);
  
  msgspeed  = MSGSPEED_GetWait();
  win       = wk->win_talk;
  
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win), clear_color);
  GFL_FONTSYS_SetColor(1, 2, clear_color);

  GFL_MSG_GetString( wk->msghandle, str_id, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< �j���`�F�b�N
#endif
  
  WORDSET_ExpandStr( wk->wordset, wk->exp_strbuf, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< �j���`�F�b�N
#endif

  wk->print_stream = PRINTSYS_PrintStream( win, 4, 0, wk->exp_strbuf, wk->font, msgspeed,
                                           wk->tcblsys, 0xffff, wk->heap_id, clear_color );
  
  // �t���[������
  BmpWinFrame_Write( wk->win_talk, WINDOW_TRANS_OFF, CGX_BMPWIN_FRAME_POS, GFL_BMPWIN_GetPalette(wk->win_talk) );

  // �]��
  { 
    GFL_BMPWIN_TransVramCharacter( win );
    GFL_BMPWIN_MakeScreen( win );

    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W����
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval TRUE:�v�����g�I��
 */
//-----------------------------------------------------------------------------
static BOOL MSG_CNT_PrintProc( UN_SELECT_MSG_CNT_WORK* wk )
{
  PRINTSTREAM_STATE state;

  GF_ASSERT(wk);

//  HOSAKA_Printf("print state= %d \n", state );

  if( wk->print_stream )
  {
    state = PRINTSYS_PrintStreamGetState( wk->print_stream );

    switch( state )
    {
    case PRINTSTREAM_STATE_DONE : // �I��
      PRINTSYS_PrintStreamDelete( wk->print_stream );
      wk->print_stream = NULL;
      return TRUE;

    case PRINTSTREAM_STATE_PAUSE : // �ꎞ��~��
      // �L�[���͑҂�
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || (  GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL ) || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamReleasePause( wk->print_stream );
        GFL_SOUND_PlaySE( SEQ_SE_DECIDE1 );
      }
      break;

    case PRINTSTREAM_STATE_RUNNING :  // ���s��
      // ���b�Z�[�W�X�L�b�v
      if( (GFL_UI_KEY_GetCont() & MSG_CNT_SKIP_BTN) || GFL_UI_TP_GetCont() )
      {
        PRINTSYS_PrintStreamShortWait( wk->print_stream, 0 );
      }
      break;

    default : GF_ASSERT(0);
    }

    return FALSE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�t���[�����N���A
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_PrintClear( UN_SELECT_MSG_CNT_WORK* wk )
{
  BmpWinFrame_Clear( wk->win_talk, WINDOW_TRANS_ON_V );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X �t�H���g�擾
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static GFL_FONT* MSG_CNT_GetFont( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->font;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X �v�����g�L���[�擾
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static PRINT_QUE* MSG_CNT_GetPrintQue( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->print_que;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W�n���h�����擾
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static GFL_MSGDATA* MSG_CNT_GetMsgData( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->msghandle;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g
 *
 *  @param  UN_SELECT_MSG_CNT_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static WORDSET* MSG_CNT_GetWordset( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->wordset;
}

//=============================================================================
//
//
// ���X�g�Ǘ��N���X
//
//
//=============================================================================
static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ListCallBack_Scroll( void * work, s8 mv );

//--------------------------------------------------------------
/// 
//==============================================================
static const FRAMELIST_CALLBACK FRMListCallBack = {
  ListCallBack_Draw,    // �`�揈��
  ListCallBack_Move,    // �ړ�����
  ListCallBack_Scroll,  // �X�N���[��
};

//--------------------------------------------------------------
/// 
//==============================================================
static const FRAMELIST_TOUCH_DATA TouchHitTbl[] =
{
  { { 0,    8*5-1,  8*12, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },   // 00: 
  { { 8*5,  8*10-1, 8*12, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },   // 01: 
  { { 8*10, 8*15-1, 8*12, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },   // 02: 
  { { 8*15, 8*20-1, 8*12, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },   // 03: 

  { { 8, 159, 240, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },         // 07: ���[��

  { { 168, 191, 16, 16+3*8-1 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },      // 08: ��
  { { 168, 191, 80, 80+3*8-1 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },    // 09: �E

  { { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

//-----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�`��
 *
 *  @param  void * work
 *  @param  itemNum
 *  @param  * util
 *  @param  py
 *  @param  disp 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{ 
  UN_SELECT_MAIN_WORK* wk = work;
  u32 prm;
  
  prm = FRAMELIST_GetItemParam( wk->lwk, itemNum );

  {
    PRINT_QUE * que;
    GFL_FONT* font;
    WORDSET* wset;
    
    que  = FRAMELIST_GetPrintQue( wk->lwk );
    font = MSG_CNT_GetFont( wk->cnt_msg );

    // �e�K�t���A���`��i�����j
    MSG_CNT_DrawListElem( wk->cnt_msg, util, que, font, itemNum );
  }

  HOSAKA_Printf("draw!\n");
}

//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  void * work
 *  @param  listPos
 *  @param  flg 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg )
{ 
  UN_SELECT_MAIN_WORK* wk = work;
  HOSAKA_Printf("move!\n");
}

//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  void * work
 *  @param  mv 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void ListCallBack_Scroll( void * work, s8 mv )
{
  UN_SELECT_MAIN_WORK* wk = work;
  HOSAKA_Printf("scroll!\n");
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���X�g����
 *
 *  @param  UN_SELECT_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void LIST_Make( UN_SELECT_MAIN_WORK* wk )
{
  ARCHANDLE* ah;

  static FRAMELIST_HEADER header = {
    BG_FRAME_LIST_M,
    BG_FRAME_LIST_S,

    11,         // ���ڃt���[���\���J�n�w���W
    0,          // ���ڃt���[���\���J�n�x���W
    18,         // ���ڃt���[���\���w�T�C�Y
    5,          // ���ڃt���[���\���x�T�C�Y

    1,              // �t���[�����ɕ\������BMPWIN�̕\���w���W
    0,              // �t���[�����ɕ\������BMPWIN�̕\���x���W
    15,             // �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
    5,              // �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
    1,              // �t���[�����ɕ\������BMPWIN�̃p���b�g

    { 40, 20, 10, 8, 5, 4 },    // �X�N���[�����x [0] = �ő� ��itemSizY������؂��l�ł��邱�ƁI

    3,              // �I�����ڂ̃p���b�g

    8,              // �X�N���[���o�[�̂x�T�C�Y

    UN_LIST_MAX,    // ���ړo�^��
    2,              // �w�i�o�^��

    0,              // �����ʒu
    4,              // �J�[�\���ړ��͈�
    UN_LIST_MAX-5,  // �����X�N���[���l //@TODO

    TouchHitTbl,      // �^�b�`�f�[�^

    &FRMListCallBack, // �R�[���o�b�N�֐�
    NULL,
  };

  header.cbWork = wk;

  wk->lwk = FRAMELIST_Create( &header, wk->heap_id );

  // �A�[�J�C�u �I�[�v��
  ah = GFL_ARC_OpenDataHandle( ARCID_UN_SELECT_GRA, wk->heap_id );

  // ���ڔw�i�ݒ�
  FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_un_select_gra_kokuren_bg_listframe_NSCR, FALSE, 0 );
  FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_un_select_gra_kokuren_bg_listframe2_NSCR, FALSE, 1 );

  // �_�ŃA�j���p���b�g�ݒ�
  FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_un_select_gra_kokuren_bg_NCLR, 2, 3 );

//  FRAMELIST_ChangePosPalette(

  // �A�[�J�C�u �N���[�Y
  GFL_ARC_CloseDataHandle( ah );
        
  // ���X�g���ڐ���
  {
    int i;
    GFL_MSGDATA* mman;
    
    mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
        NARC_message_wifi_place_msg_UN_dat, wk->heap_id );

    for( i=0; i<UN_LIST_MAX; i++)
    {
      int type;
      int idx;
      idx = UN_LIST_MAX - i - 1;

      //@TODO �����A���ڂ̑���
      type = GFUser_GetPublicRand0( 2 );

      // �p�����[�^�͏����ɏ���ID
      FRAMELIST_AddItem( wk->lwk, type, idx );

      // ���ڂ���
      if( type == 0 )
      {
        // ���ڗp������ �擾
        wk->cnt_msg->name[i] = GFL_MSG_CreateString( mman, idx );
      }
      // ���ڂȂ�
      else
      {
        // ���ڗp������ �擾
        wk->cnt_msg->name[i] = GFL_MSG_CreateString( mman, un_reception_msg_05 );
      }
    }
      
    // ���b�Z�[�W����
//    MSG_CNT_CreateNameBuffer( wk->cnt_msg, i, buf );
    
    GFL_MSG_Delete( mman );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�J��
 *
 *  @param  UN_SELECT_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void LIST_Delete( UN_SELECT_MAIN_WORK* wk )
{
  int i;
  
  for( i=0; i<UN_LIST_MAX; i++)
  {
    // ���ڗp������ �J��
    GFL_STR_DeleteBuffer( wk->cnt_msg->name[i] );
  }
  
  // �t���[�����X�g �J��
  FRAMELIST_Exit( wk->lwk );
}
  
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC ������
 *
 *  @param  GFL_PROC* proc
 *  @param  prm 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static UN_SELECT_MAIN_WORK* app_init( GFL_PROC* proc, UN_SELECT_PARAM* prm )
{
  UN_SELECT_MAIN_WORK* wk;

  //�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
  
  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UN_SELECT, UN_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(UN_SELECT_MAIN_WORK), HEAPID_UN_SELECT );
  GFL_STD_MemClear( wk, sizeof(UN_SELECT_MAIN_WORK) );

  // ������
  wk->heap_id = HEAPID_UN_SELECT;
  wk->pwk = prm;
  
  //�`��ݒ菉����
  wk->graphic = UN_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

  // ���b�Z�[�W����
  wk->cnt_msg = MSG_CNT_Create( wk->heap_id );
  
  // �V�[���R���g���[���쐬
  wk->cnt_scene = UI_SCENE_CNT_Create( 
      wk->heap_id, c_scene_func_tbl, UNS_SCENE_ID_MAX, 
      UNS_SCENE_ID_LIST_MAKE, wk );

  //BG���\�[�X�ǂݍ���
  UNSelect_BG_LoadResource( &wk->wk_bg, wk->heap_id );

#ifdef UN_SELECT_TOUCHBAR
  //�^�b�`�o�[�̏�����
  { 
    GFL_CLUNIT  *clunit = UN_SELECT_GRAPHIC_GetClunit( wk->graphic );
    wk->touchbar  = UNSelect_TOUCHBAR_Init( clunit, wk->heap_id );
  }
#endif //UN_SELECT_TOUCHBAR

  //TASKMENU���\�[�X�ǂݍ��݁�������
  wk->menu_res  = APP_TASKMENU_RES_Create( BG_FRAME_MENU_M, PLTID_BG_TASKMENU_M,
      MSG_CNT_GetFont( wk->cnt_msg ),
      MSG_CNT_GetPrintQue( wk->cnt_msg ),
      wk->heap_id );
  
  // ���X�g����
  LIST_Make( wk );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�v���P�[�V�����I������
 *
 *  @param  UN_SELECT_MAIN_WORK* wk
 *  @param  end_mode 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void app_end( UN_SELECT_MAIN_WORK* wk, END_MODE end_mode )
{
  switch( end_mode )
  {
  case END_MODE_DECIDE: 
      // �f�[�^�f���o��
      //@TODO �I���t���A
    break;
  case END_MODE_CANCEL:
      // �f�[�^�f���o��
      //@TODO �L�����Z��
    break;
  }
  
  // �t�F�[�h�A�E�g ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE ���X�g���� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneListMake( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  switch( seq )
  {
  case 0:
    // ���X�g������
    if( FRAMELIST_Init( wk->lwk ) == FALSE )
    {
      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;
  case 1:
    // �t���A�I����
    UI_SCENE_CNT_SetNextScene( cnt, UNS_SCENE_ID_SELECT_FLOOR );
    HOSAKA_Printf("���X�g��������\n");
    return TRUE;
  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �t���A�I�� ����������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �t���A�I�� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  // �t���A�I������
  // @TODO �^�b�`�y�уJ�[�\���őI��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
  {
    UI_SCENE_CNT_SetNextScene( cnt, UNS_SCENE_ID_CONFIRM );
    return TRUE;
  }

  FRAMELIST_Main( wk->lwk );

#ifdef UN_SELECT_TOUCHBAR
  //�^�b�`�o�[���C������
  UNSelect_TOUCHBAR_Main( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

  switch( TOUCHBAR_GetTrg( wk->touchbar ) )
  {
  case TOUCHBAR_ICON_CUR_L : // ��
    //@TODO
    HOSAKA_Printf("L!\n");
    break;

  case TOUCHBAR_ICON_CUR_R : // ��
    //@TODO
    HOSAKA_Printf("R!\n"); 
    break;

  case TOUCHBAR_ICON_RETURN : // ���^�[���{�^��
    // �I��
    app_end( wk, END_MODE_CANCEL ); 
    UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
    return TRUE;

  case TOUCHBAR_SELECT_NONE : // �I���Ȃ�
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �t���A�I�� �㏈��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �m�F��� ����������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  switch(seq)
  {
  case 0:
    // �p�b�V�u��ԂɑJ��
    PASSIVE_Request();

#if 0
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_CUR_L, FALSE );
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_CUR_R, FALSE );
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_RETURN, FALSE );
#endif

    MSG_CNT_SetPrint( wk->cnt_msg, un_reception_msg_01 );
    UI_SCENE_CNT_IncSubSeq( cnt );
    break;

  case 1:
    if( MSG_CNT_PrintProc(wk->cnt_msg) )
    {
      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;

  case 2:
    // �^�X�N���j���[�\��
    wk->menu = UNSelect_TASKMENU_Init( 
        wk->menu_res, 
        MSG_CNT_GetMsgData(wk->cnt_msg), 
        wk->heap_id );

    return TRUE;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �m�F��� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  //�^�X�N���j���[���C������
  UNSelect_TASKMENU_Main( wk->menu );

  if( APP_TASKMENU_IsFinish( wk->menu ) )
  {
    u8 pos = APP_TASKMENU_GetCursorPos( wk->menu );

    switch( pos )
    {
    case 0 :
      // �I��
      app_end( wk, END_MODE_DECIDE );
      UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
      break;

    case 1 :
      // �I����ʂɖ߂�
      PASSIVE_Reset();
      MSG_CNT_PrintClear( wk->cnt_msg );
      UNSelect_TASKMENU_Exit( wk->menu );
      UI_SCENE_CNT_SetNextScene( cnt, UNS_SCENE_ID_SELECT_FLOOR );
      break;

    default : GF_ASSERT(0);
    }

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SCENE �m�F��� �㏈��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

