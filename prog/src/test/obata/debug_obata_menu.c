#include <wchar.h>    // wcslen
#include <gflib.h>
#include "debug_obata_menu_setup.h"
#include "system/main.h" 
#include "arc/arc_def.h"
#include "font/font.naix"
#include "print/printsys.h"


FS_EXTERN_OVERLAY(obata_debug);
extern const GFL_PROC_DATA DebugObataDemoTestMainProcData;
extern const GFL_PROC_DATA DebugObata3DSoundMainProcData;
extern const GFL_PROC_DATA DebugObataIcaTestMainProcData;
extern const GFL_PROC_DATA DebugObataElboardMainProcData;


//============================================================================================
/**
 * @breif �萔
 */
//============================================================================================
// �I�����
typedef enum
{
  SELECT_STATE_NONE,    // ���I��
  SELECT_STATE_DECIDE,  // ����
  SELECT_STATE_EXIT,    // �I��
} SELECT_STATE;

// ���j���[���ڃC���f�b�N�X
typedef enum
{
  MENU_ITEM_DEMO_TEST,
  MENU_ITEM_3D_SOUND,
  MENU_ITEM_ICA_TEST,
  MENU_ITEM_ELBOARD,
  MENU_ITEM_DAMMY,
  MENU_ITEM_NUM
} MENU_ITEM;

// ���j���[���ڕ�����
static const STRCODE* str_menu[] = 
{
  L"�f���e�X�g",
  L"3D�T�E���h",
  L"ICA�Đ��e�X�g",
  L"�d���f����",
  L"�߂�",
};


//============================================================================================
/**
 * @breif ���[�N
 */
//============================================================================================
typedef struct
{
  GFL_FONT* font;
  GFL_BMPWIN* bmpWin;
  int curPos; // �I������
  SELECT_STATE state; // �I�����
}
PROC_WORK;


//============================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//============================================================================================
static void Initialize( PROC_WORK* work );
static void Finalize( PROC_WORK* work );
static void Main( PROC_WORK* work );
static void Draw( PROC_WORK* work );
static void UpdateMenuList( PROC_WORK* work );
static void SetNextProc( PROC_WORK* work );


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_MENU_MainProcFunc_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = NULL;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OBATA_DEBUG, 0x100000 );

	// ���[�N���쐬
	work = GFL_PROC_AllocWork( proc, sizeof( PROC_WORK ), HEAPID_OBATA_DEBUG );

  // ����������
  DEBUG_OBATA_MENU_SETUP_Init( HEAPID_OBATA_DEBUG );
  Initialize( work );

	return GFL_PROC_RES_FINISH;
}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_MENU_MainProcFunc_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;

  switch( *seq )
  {
  case 0:
    Main( work );
    Draw( work ); 
    if( work->state != SELECT_STATE_NONE )
    {
      ++( *seq );
    }
    break;
  case 1:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


//============================================================================================
/**
 * @brief �I���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_MENU_MainProcFunc_End( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;

  // ���̃v���Z�X��ݒ�
  SetNextProc( work );

  // �I������
  Finalize( work );
  DEBUG_OBATA_MENU_SETUP_Exit();

	// ���[�N��j��
	GFL_PROC_FreeWork( proc );

  // �q�[�v��j��
  GFL_HEAP_DeleteHeap( HEAPID_OBATA_DEBUG );

	return GFL_PROC_RES_FINISH;
} 


//============================================================================================
/**
 * @brief �v���Z�X��`�f�[�^
 */
//============================================================================================
const GFL_PROC_DATA DebugObataMenuMainProcData = 
{
	DEBUG_OBATA_MENU_MainProcFunc_Init,
	DEBUG_OBATA_MENU_MainProcFunc_Main,
	DEBUG_OBATA_MENU_MainProcFunc_End,
};


//============================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @breif ������
 */
//-------------------------------------------------------------------------------------------- 
static void Initialize( PROC_WORK* work )
{
  // ������
  work->curPos = 0;
  work->state = SELECT_STATE_NONE;

  // �t�H���g�f�[�^�n���h���쐬
  work->font = GFL_FONT_Create( 
      ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_OBATA_DEBUG );

  // �r�b�g�}�b�v�E�B���h�E���쐬
  work->bmpWin = GFL_BMPWIN_Create( GFL_BG_FRAME1_M, 0, 0, 32, 24, 0, GFL_BMP_CHRAREA_GET_F );

  // ���j���[���X�g�̍X�V
  UpdateMenuList( work );
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �I��
 */
//-------------------------------------------------------------------------------------------- 
static void Finalize( PROC_WORK* work )
{ 
  // �r�b�g�}�b�v�E�B���h�E�̔j��
  GFL_BMPWIN_Delete( work->bmpWin );

  // �t�H���g�f�[�^�n���h���j��
  GFL_FONT_Delete( work->font );
}

//--------------------------------------------------------------------------------------------
/**
 * @breif ���C������ 
 */
//-------------------------------------------------------------------------------------------- 
static void Main( PROC_WORK* work )
{
  int      trg = GFL_UI_KEY_GetTrg();
  int move_dir = 0;

  if( trg & PAD_KEY_UP ) move_dir = -1;
  if( trg & PAD_KEY_DOWN ) move_dir = 1;
  if( trg & PAD_BUTTON_A ) work->state = SELECT_STATE_DECIDE;
  if( trg & PAD_BUTTON_SELECT ) work->state = SELECT_STATE_EXIT;

  // �J�[�\�����ړ������ꍇ
  if( move_dir != 0 )
  {
    // �J�[�\���ʒu��␳
    work->curPos = work->curPos + move_dir;
    if( work->curPos < 0 ) work->curPos = 0;
    work->curPos = work->curPos % MENU_ITEM_NUM;

    // ���j���[���ڍX�V
    UpdateMenuList( work );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//-------------------------------------------------------------------------------------------- 
static void Draw( PROC_WORK* work )
{
}

//--------------------------------------------------------------------------------------------
/**
 * @breif ���j���[���X�g���X�V����
 */
//-------------------------------------------------------------------------------------------- 
static void UpdateMenuList( PROC_WORK* work )
{
  // �r�b�g�}�b�v�E�B���h�E�ɕ�������������
  {
    int i, len;
    STRBUF* strbuf;
    STRCODE strcode[256];
    GFL_BMP_DATA* bmp;
    PRINTSYS_LSB lsb;


    // ��������
    for( i=0; i<MENU_ITEM_NUM; i++ )
    {
      // �o�b�t�@�쐬
      strbuf = GFL_STR_CreateBuffer( 256, HEAPID_OBATA_DEBUG );

      // �����F����
      if( i == work->curPos )
      {
        lsb = PRINTSYS_LSB_Make(1, 3, 0);
      }
      else
      {
        lsb = PRINTSYS_LSB_Make(1, 2, 0);
      }

      // ������쐬
      len = wcslen( str_menu[i] );
      GFL_STD_MemCopy( str_menu[i], strcode, sizeof( STRCODE )*len );
      strcode[len] = GFL_STR_GetEOMCode();
      GFL_STR_SetStringCode( strbuf, strcode );

      // �r�b�g�}�b�v�ɏ�������
      bmp = GFL_BMPWIN_GetBmp( work->bmpWin );
      PRINTSYS_PrintColor( bmp, 0, i*16, strbuf, work->font, lsb );

      // �o�b�t�@�j��
      GFL_STR_DeleteBuffer( strbuf );
    }
  }

  // �r�b�g�}�b�v�E�B���h�E�����o�b�t�@��VRAM�֓]��
  GFL_BMPWIN_TransVramCharacter( work->bmpWin );

  // �r�b�g�}�b�v�p�X�N���[����BG�X�N���[���o�b�t�@�ɍ쐬
  GFL_BMPWIN_MakeScreen( work->bmpWin );

  // BG�X�N���[���f�[�^�𔽉f������
  GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̃v���Z�X��ݒ肷��
 */
//--------------------------------------------------------------------------------------------
static void SetNextProc( PROC_WORK* work )
{
  // �J�[�\���̈ʒu�Ō���
  switch( work->curPos )
  {
  case MENU_ITEM_DEMO_TEST:
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(obata_debug), &DebugObataDemoTestMainProcData, NULL );
      break;
  case MENU_ITEM_3D_SOUND:
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(obata_debug), &DebugObata3DSoundMainProcData, NULL );
      break;
  case MENU_ITEM_ICA_TEST:
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(obata_debug), &DebugObataIcaTestMainProcData, NULL );
      break;
  case MENU_ITEM_ELBOARD:
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(obata_debug), &DebugObataElboardMainProcData, NULL );
      break;
  }
}
