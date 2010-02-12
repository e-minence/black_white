/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������� ( ���j���[��� )
 * @file   research_menu.c
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "research_menu.h"
#include "research_common.h"
#include "bg_font.h"

#include "print/gf_font.h"   // for GFL_FONT_xxxx
#include "print/printsys.h"  // for PRINTSYS_xxxx

#include "system/main.h"            // for HEAPID_xxxx
#include "arc/arc_def.h"            // for ARCID_xxxx
#include "arc/research_radar.naix"  // for NARC_research_radar_xxxx
#include "arc/font/font.naix"       // for NARC_font_xxxx
#include "arc/message.naix"         // for NARC_message_xxxx


//===============================================================================
// ���萔
//===============================================================================
// �p���b�g�ԍ�
#define MAIN_BG_PALETTE_BACK_GROUND_0 (0x0)
#define MAIN_BG_PALETTE_BACK_GROUND_1 (0x1)
#define MAIN_BG_PALETTE_BACK_GROUND_2 (0x2)
#define MAIN_BG_PALETTE_BACK_GROUND_3 (0x3)
#define MAIN_BG_PALETTE_BACK_GROUND_4 (0x4)
#define MAIN_BG_PALETTE_BACK_GROUND_5 (0x5)
#define MAIN_BG_PALETTE_WINDOW_ON     (0xa)  // @todo ������
#define MAIN_BG_PALETTE_WINDOW_OFF    (0x7)
#define MAIN_BG_PALETTE_FONT          (0x8)

#define SUB_BG_PALETTE_BACK_GROUND_0 (0x0)
#define SUB_BG_PALETTE_BACK_GROUND_1 (0x1)
#define SUB_BG_PALETTE_BACK_GROUND_2 (0x2)
#define SUB_BG_PALETTE_BACK_GROUND_3 (0x3)
#define SUB_BG_PALETTE_BACK_GROUND_4 (0x4)
#define SUB_BG_PALETTE_BACK_GROUND_5 (0x5)
#define SUB_BG_PALETTE_WINDOW        (0x6)
#define SUB_BG_PALETTE_FONT          (0x7)

// SUB-BG
#define SUB_BG_WINDOW  (GFL_BG_FRAME1_S)  // �E�B���h�E��
#define SUB_BG_FONT    (GFL_BG_FRAME2_S)  // �t�H���g��
// SUB-BG �\���D�揇��
#define SUB_BG_PRIORITY_WINDOW (2)  // �E�B���h�E��
#define SUB_BG_PRIORITY_FONT   (1)  // �t�H���g��

// MAIN-BG
#define MAIN_BG_WINDOW (GFL_BG_FRAME2_M)  // �E�B���h�E��
#define MAIN_BG_FONT   (GFL_BG_FRAME3_M)  // �t�H���g��
// MAIN-BG �\���D�揇��
#define MAIN_BG_PRIORITY_WINDOW (2)  // �E�B���h�E��
#define MAIN_BG_PRIORITY_FONT   (1)  // �t�H���g��

// ���� �������E�B���h�E
#define DIRECTION_WINDOW_X               (0)   // X���W   (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_Y               (17)  // Y���W   (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_HEIGHT          (6)   // Y�T�C�Y (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define DIRECTION_WINDOW_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define DIRECTION_WINDOW_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define DIRECTION_WINDOW_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define DIRECTION_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �{�^�� ( �������e�̌��� )
#define CHANGE_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_Y               (6)   // Y���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHANGE_BUTTON_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define CHANGE_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �{�^�� ( �����񍐂����� )
#define CHECK_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_Y               (13)  // Y���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHECK_BUTTON_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define CHECK_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�


// �����V�[�P���X
typedef enum{
  RESEARCH_MENU_SEQ_SETUP,    // ����
  RESEARCH_MENU_SEQ_KEY_WAIT, // �L�[���͑҂�
  RESEARCH_MENU_SEQ_CLEAN_UP, // ��Еt��
  RESEARCH_MENU_SEQ_FINISH,   // �I��
} RESEARCH_MENU_SEQ;

// ���j���[����
typedef enum{
  MENU_ITEM_CHANGE_RESEARCH,  //�u�������e��ύX����v
  MENU_ITEM_CHECK_RESEARCH,   //�u�����񍐂��m�F����v
  MENU_ITEM_NUM,              // ���j���[���ڐ�
} MENU_ITEM;


// ���� ������
static const BG_FONT_PARAM BGFontParam_direction = 
{
  SUB_BG_FONT,
  DIRECTION_WINDOW_X,
  DIRECTION_WINDOW_Y,
  DIRECTION_WINDOW_WIDTH,
  DIRECTION_WINDOW_HEIGHT,
  DIRECTION_WINDOW_STRING_OFFSET_X,
  DIRECTION_WINDOW_STRING_OFFSET_Y,
  SUB_BG_PALETTE_FONT,
  DIRECTION_WINDOW_STRING_COLOR_L,
  DIRECTION_WINDOW_STRING_COLOR_S,
  DIRECTION_WINDOW_STRING_COLOR_B,
};

// ����ʁu�������e�̌���v
static const BG_FONT_PARAM BGFontParam_changeButton = 
{
  MAIN_BG_FONT,
  CHANGE_BUTTON_X,
  CHANGE_BUTTON_Y,
  CHANGE_BUTTON_WIDTH,
  CHANGE_BUTTON_HEIGHT,
  CHANGE_BUTTON_STRING_OFFSET_X,
  CHANGE_BUTTON_STRING_OFFSET_Y,
  MAIN_BG_PALETTE_FONT,
  CHANGE_BUTTON_STRING_COLOR_L,
  CHANGE_BUTTON_STRING_COLOR_S,
  CHANGE_BUTTON_STRING_COLOR_B,
};

// ����ʁu�����񍐂̊m�F�v
static const BG_FONT_PARAM BGFontParam_checkButton = 
{
  MAIN_BG_FONT,
  CHECK_BUTTON_X,
  CHECK_BUTTON_Y,
  CHECK_BUTTON_WIDTH,
  CHECK_BUTTON_HEIGHT,
  CHECK_BUTTON_STRING_OFFSET_X,
  CHECK_BUTTON_STRING_OFFSET_Y,
  MAIN_BG_PALETTE_FONT,
  CHECK_BUTTON_STRING_COLOR_L,
  CHECK_BUTTON_STRING_COLOR_S,
  CHECK_BUTTON_STRING_COLOR_B,
};

// OBJ ���\�[�X�C���f�b�N�X
typedef enum{
  OBJ_RESOURCE_CHR,    // �L�����N�^
  OBJ_RESOURCE_PLT,    // �p���b�g
  OBJ_RESOURCE_CEL,    // �Z��
  OBJ_RESOURCE_ANM,    // �A�j���[�V����
} OBJ_RESOURCE_INDEX;


//=============================================================================== 
// ������������� ���[�N
//=============================================================================== 
struct _RESEARCH_MENU_WORK
{ 
  RESEARCH_MENU_SEQ    seq;        // �����V�[�P���X
  RESEARCH_MENU_RESULT result;     // ��ʏI������
  MENU_ITEM            cursorPos;  // �J�[�\���ʒu

  HEAPID       heapID;   // �q�[�vID
  GFL_FONT*    font;     // �t�H���g
  GFL_MSGDATA* message;  // ���b�Z�[�W

  // ����
  BG_FONT* BGFont_direction;  // ������

  // �����
  BG_FONT* BGFont_changeButton;  //�u�������e�̌���v�{�^��
  BG_FONT* BGFont_checkButton;   //�u�����񍐂�����v�{�^��
};


//===============================================================================
// ������J�֐�
//===============================================================================
// �V�[�P���X����
static RESEARCH_MENU_SEQ Main_SETUP   ( RESEARCH_MENU_WORK* work );
static RESEARCH_MENU_SEQ Main_KEY_WAIT( RESEARCH_MENU_WORK* work );
static RESEARCH_MENU_SEQ Main_CLEAN_UP( RESEARCH_MENU_WORK* work );
// �V�[�P���X����
static void SwitchSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq );
static void SetSeq   ( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ seq );
// �J�[�\���ړ�
static void MoveCursorUp  ( RESEARCH_MENU_WORK* work );
static void MoveCursorDown( RESEARCH_MENU_WORK* work );
// ��ʏI������
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result );

// �t�H���g
static void SetupFont  ( RESEARCH_MENU_WORK* work );
static void CleanUpFont( RESEARCH_MENU_WORK* work );
// ���b�Z�[�W
static void SetupMessage  ( RESEARCH_MENU_WORK* work );
static void CleanUpMessage( RESEARCH_MENU_WORK* work );

// BG
static void SetupBG  ( RESEARCH_MENU_WORK* work );
static void CleanUpBG( RESEARCH_MENU_WORK* work );
// ���� �E�B���h�EBG��
static void SetupSubBG_window  ( RESEARCH_MENU_WORK* work );
static void CleanUpSubBG_window( RESEARCH_MENU_WORK* work );
// ���� �t�H���gBG��
static void SetupSubBG_font  ( RESEARCH_MENU_WORK* work );
static void CleanUpSubBG_font( RESEARCH_MENU_WORK* work );
// ����� �E�B���h�EBG��
static void SetupMainBG_window  ( RESEARCH_MENU_WORK* work );
static void CleanUpMainBG_window( RESEARCH_MENU_WORK* work );
// ����� �t�H���gBG��
static void SetupMainBG_font  ( RESEARCH_MENU_WORK* work );
static void CleanUpMainBG_font( RESEARCH_MENU_WORK* work );

// ���j���[���ڑ���
static void MenuItemSwitchOn ( MENU_ITEM menuItem );
static void MenuItemSwitchOff( MENU_ITEM menuItem );


//=============================================================================== 
// ������������� ����֐�
//=============================================================================== 

//-------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̐���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
RESEARCH_MENU_WORK* CreateResearchMenuWork( HEAPID heapID )
{
  RESEARCH_MENU_WORK* work;

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_MENU_WORK) );

  // ������
  work->seq                 = RESEARCH_MENU_SEQ_SETUP;
  work->result              = RESEARCH_MENU_RESULT_NONE;
  work->cursorPos           = MENU_ITEM_CHANGE_RESEARCH;
  work->heapID              = heapID;
  work->font                = NULL;
  work->message             = NULL;
  work->BGFont_direction    = NULL;
  work->BGFont_changeButton = NULL;
  work->BGFont_checkButton  = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create work\n" );

  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
void DeleteResearchMenuWork( RESEARCH_MENU_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  }
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete work\n" );
} 

//-------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
RESEARCH_MENU_RESULT ResearchMenuMain( RESEARCH_MENU_WORK* work )
{
  RESEARCH_MENU_SEQ nextSeq;

  // �V�[�P���X���Ƃ̏���
  switch( work->seq )
  {
  case RESEARCH_MENU_SEQ_SETUP:     nextSeq = Main_SETUP   ( work );  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:  nextSeq = Main_KEY_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:  nextSeq = Main_CLEAN_UP( work );  break;
  case RESEARCH_MENU_SEQ_FINISH:    return work->result;
  default:  GF_ASSERT(0);
  }

  // �V�[�P���X�X�V
  SwitchSeq( work, nextSeq );

  return RESEARCH_MENU_RESULT_CONTINUE;
}


//===============================================================================
// ���V�[�P���X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//-------------------------------------------------------------------------------
static RESEARCH_MENU_SEQ Main_SETUP( RESEARCH_MENU_WORK* work )
{
  // BG ����
  SetupFont         ( work );
  SetupMessage      ( work );
  SetupBG           ( work );
  SetupSubBG_window ( work );
  SetupSubBG_font   ( work );
  SetupMainBG_window( work );
  SetupMainBG_font  ( work );

  // �����J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
  MenuItemSwitchOn( work->cursorPos );

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  return RESEARCH_MENU_SEQ_KEY_WAIT;
}

//-------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//-------------------------------------------------------------------------------
static RESEARCH_MENU_SEQ Main_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  RESEARCH_MENU_SEQ nextSeq;
  int trg;

  nextSeq = work->seq;
  trg     = GFL_UI_KEY_GetTrg();

  //--------
  // ��L�[
  if( trg & PAD_KEY_UP )
  {
    MoveCursorUp( work );
  }

  //--------
  // ���L�[
  if( trg & PAD_KEY_DOWN )
  {
    MoveCursorDown( work );
  }

  //--------
  // A�{�^��
  if( trg & PAD_BUTTON_A )
  {
    switch( work->cursorPos )
    {
    //�u�������e��ύX����v
    case MENU_ITEM_CHANGE_RESEARCH:
      SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT );  // ��ʏI�����ʂ�����
      nextSeq = RESEARCH_MENU_SEQ_CLEAN_UP;               // �V�[�P���X�J��
      break;

    //�u�����񍐂��m�F����v
    case MENU_ITEM_CHECK_RESEARCH:
      SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK );  // ��ʏI�����ʂ�����
      nextSeq = RESEARCH_MENU_SEQ_CLEAN_UP;              // �V�[�P���X�J��
      break;

    // �G���[
    default:
      GF_ASSERT(0);
    }
  } 
  return nextSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//-------------------------------------------------------------------------------
static RESEARCH_MENU_SEQ Main_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // BG ��Еt��
  CleanUpMainBG_font  ( work );
  CleanUpMainBG_window( work );
  CleanUpSubBG_font   ( work );
  CleanUpSubBG_window ( work );
  CleanUpBG           ( work );
  CleanUpMessage      ( work );
  CleanUpFont         ( work );

  return RESEARCH_MENU_SEQ_FINISH;
}


//===============================================================================
// ���V�[�P���X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 * @param nextSeq �ύX��̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static void SwitchSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq )
{
  // �ω��Ȃ�
  if( work->seq == nextSeq){ return; }

  // �ύX
  SetSeq( work, nextSeq ); 
} 

//-------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work
 * @parma seq  �ݒ肷��V�[�P���X
 */
//-------------------------------------------------------------------------------
static void SetSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ seq )
{
  work->seq = seq;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set seq ==> %d\n", seq );
}


//===============================================================================
// ���J�[�\���ړ�
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �J�[�\������ֈړ�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void MoveCursorUp( RESEARCH_MENU_WORK* work )
{
  int nowPos;
  int nextPos;

  // �X�V��̃J�[�\���ʒu���Z�o
  nowPos  = work->cursorPos;
  nextPos = (nowPos - 1 + MENU_ITEM_NUM) % MENU_ITEM_NUM;

  // ���ڂ̕\�����X�V
  MenuItemSwitchOff( nowPos );
  MenuItemSwitchOn( nextPos );

  // �J�[�\���ʒu���X�V
  work->cursorPos = nextPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor ==> %d\n", work->cursorPos );
}

//-------------------------------------------------------------------------------
/**
 * @brief �J�[�\�������ֈړ�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void MoveCursorDown( RESEARCH_MENU_WORK* work )
{
  int nowPos;
  int nextPos;

  // �X�V��̃J�[�\���ʒu���Z�o
  nowPos  = work->cursorPos;
  nextPos = (nowPos + 1) % MENU_ITEM_NUM;

  // ���ڂ̕\�����X�V
  MenuItemSwitchOff( nowPos );
  MenuItemSwitchOn( nextPos );

  // �J�[�\���ʒu���X�V
  work->cursorPos = nextPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor ==> %d\n", work->cursorPos );
}


//===============================================================================
// ����ʏI������
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//-------------------------------------------------------------------------------
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_MENU_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set result (%d)\n", result );
}


//===============================================================================
// ���t�H���g
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�� ����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupFont( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, TRUE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup font\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�� ��Еt��
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpFont( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up font\n" );
}


//===============================================================================
// �����b�Z�[�W
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^ ����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupMessage( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->message == NULL );

  // �쐬
  work->message = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 0, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup message\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^ ��Еt��
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpMessage( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->message );

  // �폜
  GFL_MSG_Delete( work->message );
  work->message = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up message\n" );
}


//===============================================================================
// ��BG
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief BG������������
 */
//-------------------------------------------------------------------------------
static void SetupBG( RESEARCH_MENU_WORK* work )
{
  // BG �V�X�e�� �w�b�_
  static const GFL_BG_SYS_HEADER BGSysHeader = 
  {
    GX_DISPMODE_GRAPHICS,   // �\�����[�h�w��
    GX_BGMODE_0,            // �a�f���[�h�w��(���C���X�N���[��)
    GX_BGMODE_0,            // �a�f���[�h�w��(�T�u�X�N���[��)
    GX_BG0_AS_2D            // �a�f�O�̂Q�c�A�R�c���[�h�I��
  };
  // BG �R���g���[�� �w�b�_
  const GFL_BG_BGCNT_HEADER BGCnt1S = 
  {
    0, 0,					          // �����\���ʒu
    0x800,						      // �X�N���[���o�b�t�@�T�C�Y
    0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
    GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
    GX_BG_COLORMODE_16,			// �J���[���[�h
    GX_BG_SCRBASE_0x0800,		// �X�N���[���x�[�X�u���b�N
    GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
    GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
    GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
    SUB_BG_PRIORITY_WINDOW, // �\���v���C�I���e�B�[
    0,							        // �G���A�I�[�o�[�t���O
    0,							        // DUMMY
    FALSE,						      // ���U�C�N�ݒ�
  }; 
  const GFL_BG_BGCNT_HEADER BGCnt2S = 
  {
    0, 0,					          // �����\���ʒu
    0x800,						      // �X�N���[���o�b�t�@�T�C�Y
    0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
    GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
    GX_BG_COLORMODE_16,			// �J���[���[�h
    GX_BG_SCRBASE_0x1000,		// �X�N���[���x�[�X�u���b�N
    GX_BG_CHARBASE_0x10000,	// �L�����N�^�x�[�X�u���b�N
    GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
    GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
    SUB_BG_PRIORITY_FONT,   // �\���v���C�I���e�B�[
    0,							        // �G���A�I�[�o�[�t���O
    0,							        // DUMMY
    FALSE,						      // ���U�C�N�ݒ�
  }; 
  const GFL_BG_BGCNT_HEADER BGCnt2M = 
  {
    0, 0,					          // �����\���ʒu
    0x800,						      // �X�N���[���o�b�t�@�T�C�Y
    0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
    GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
    GX_BG_COLORMODE_16,			// �J���[���[�h
    GX_BG_SCRBASE_0x0800,		// �X�N���[���x�[�X�u���b�N
    GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
    GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
    GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
    MAIN_BG_PRIORITY_WINDOW,// �\���v���C�I���e�B�[
    0,							        // �G���A�I�[�o�[�t���O
    0,							        // DUMMY
    FALSE,						      // ���U�C�N�ݒ�
  }; 
  const GFL_BG_BGCNT_HEADER BGCnt3M = 
  {
    0, 0,					          // �����\���ʒu
    0x800,						      // �X�N���[���o�b�t�@�T�C�Y
    0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
    GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
    GX_BG_COLORMODE_16,			// �J���[���[�h
    GX_BG_SCRBASE_0x1000,		// �X�N���[���x�[�X�u���b�N
    GX_BG_CHARBASE_0x10000,	// �L�����N�^�x�[�X�u���b�N
    GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
    GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
    MAIN_BG_PRIORITY_FONT,  // �\���v���C�I���e�B�[
    0,							        // �G���A�I�[�o�[�t���O
    0,							        // DUMMY
    FALSE,						      // ���U�C�N�ݒ�
  }; 

  GFL_BG_SetBGMode( &BGSysHeader );
  GFL_BG_SetBGControl( SUB_BG_WINDOW,  &BGCnt1S, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_FONT,    &BGCnt2S, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_WINDOW, &BGCnt2M, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_FONT,   &BGCnt3M, GFL_BG_MODE_TEXT );

  GFL_BG_SetVisible( SUB_BG_BACK_GROUND,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_WINDOW,       VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_FONT,         VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_BACK_GROUND, VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_WINDOW,      VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_FONT,        VISIBLE_ON );

  GFL_BMPWIN_Init( work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup BG\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief BG��Еt��
 */
//-------------------------------------------------------------------------------
static void CleanUpBG( RESEARCH_MENU_WORK* work )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( SUB_BG_FONT );
  GFL_BG_FreeBGControl( SUB_BG_WINDOW );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up BG\n" );
}


//===============================================================================
// ������ �E�B���h�EBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupSubBG_window( RESEARCH_MENU_WORK* work )
{
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR, work->heapID ); 

    // �p���b�g�f�[�^
    // ���w�iBG�ʂƓ����f�[�^���Q��
    /*
    {
      void* src;
      u16 offset;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_bgu_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( SUB_BG_WINDOW, data->pRawData,
                          ONE_PALETTE_SIZE * 1,
                          ONE_PALETTE_SIZE * 6 );
      GFL_HEAP_FreeMemory( src );
    }
    */
    // �L�����N�^�f�[�^
    // ���w�iBG�ʂƓ����f�[�^���Q��
    /*
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_bgd_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( MAIN_BG_BACK_GROUND, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    */
    // �X�N���[���f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_win1_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_WINDOW, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_WINDOW );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup SUB-BG-WINDOW\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpSubBG_window( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-WINDOW\n" );
}


//===============================================================================
// ������ �t�H���gBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���� �t�H���gBG�� ����
 * 
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupSubBG_font( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->BGFont_direction == NULL ); 

  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // �����`��I�u�W�F�N�g�쐬
  work->BGFont_direction = BG_FONT_Create( &BGFontParam_direction, 
                                           work->font, work->message, work->heapID );
  BG_FONT_SetString( work->BGFont_direction, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup SUB-BG-FONT\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpSubBG_font( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->BGFont_direction );

  // �����`��I�u�W�F�N�g�j��
  BG_FONT_Delete( work->BGFont_direction );

  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-FONT\n" );
}


//===============================================================================
// ������� �E�B���h�EBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupMainBG_window( RESEARCH_MENU_WORK* work )
{
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR, work->heapID ); 

    // �p���b�g�f�[�^
    // ���w�iBG�ʂƓ����f�[�^���Q��
    /*
    {
      void* src;
      u16 offset;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_bgu_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( SUB_BG_WINDOW, data->pRawData,
                          ONE_PALETTE_SIZE * 1,
                          ONE_PALETTE_SIZE * 6 );
      GFL_HEAP_FreeMemory( src );
    }
    */
    // �L�����N�^�f�[�^
    // ���w�iBG�ʂƓ����f�[�^���Q��
    /*
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_bgd_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( MAIN_BG_BACK_GROUND, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    */
    // �X�N���[���f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgd_topbtn_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_WINDOW, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup MAIN-BG-WINDOW\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpMainBG_window( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-WINDOW\n" );
}


//===============================================================================
// ������� �t�H���gBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupMainBG_font( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->BGFont_changeButton == NULL ); 
  GF_ASSERT( work->BGFont_checkButton == NULL ); 

  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // ������`��I�u�W�F�N�g�쐬
  work->BGFont_changeButton = BG_FONT_Create( &BGFontParam_changeButton, 
                                              work->font, work->message, work->heapID );
  work->BGFont_checkButton = BG_FONT_Create( &BGFontParam_checkButton, 
                                             work->font, work->message, work->heapID );
  BG_FONT_SetString( work->BGFont_changeButton, 0 );
  BG_FONT_SetString( work->BGFont_checkButton, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup MAIN-BG-FONT\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpMainBG_font( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->BGFont_checkButton );
  GF_ASSERT( work->BGFont_changeButton );
  
  // ������`��I�u�W�F�N�g�j��
  BG_FONT_Delete( work->BGFont_checkButton );
  BG_FONT_Delete( work->BGFont_changeButton ); 
  work->BGFont_checkButton  = NULL;
  work->BGFont_changeButton = NULL;

  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-FONT\n" );
}


//===============================================================================
// �����j���[���ڑ���
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ�I����Ԃɂ���
 *
 * @param menuItem �I����Ԃɂ��郁�j���[���ڂ��w��
 */
//-------------------------------------------------------------------------------
static void MenuItemSwitchOn( MENU_ITEM menuItem )
{
  u8 BGFrame;
  u8 x, y, width, height;
  u8 paletteNo;

  // �p�����[�^������
  switch( menuItem )
  {
  //�u�������e��ύX����v
  case MENU_ITEM_CHANGE_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHANGE_BUTTON_X;
    y         = CHANGE_BUTTON_Y;
    width     = CHANGE_BUTTON_WIDTH;
    height    = CHANGE_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_ON;
    break;

  //�u�����񍐂��m�F����v
  case MENU_ITEM_CHECK_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHECK_BUTTON_X;
    y         = CHECK_BUTTON_Y;
    width     = CHECK_BUTTON_WIDTH;
    height    = CHECK_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_ON;
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, x, y, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//-------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ��I����Ԃɂ���
 *
 * @param menuItem ��I����Ԃɂ��郁�j���[���ڂ��w��
 */
//-------------------------------------------------------------------------------
static void MenuItemSwitchOff( MENU_ITEM menuItem )
{
  u8 BGFrame;
  u8 x, y, width, height;
  u8 paletteNo;

  // �p�����[�^������
  switch( menuItem )
  {
  //�u�������e��ύX����v
  case MENU_ITEM_CHANGE_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHANGE_BUTTON_X;
    y         = CHANGE_BUTTON_Y;
    width     = CHANGE_BUTTON_WIDTH;
    height    = CHANGE_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_OFF;
    break;

  //�u�����񍐂��m�F����v
  case MENU_ITEM_CHECK_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHECK_BUTTON_X;
    y         = CHECK_BUTTON_Y;
    width     = CHECK_BUTTON_WIDTH;
    height    = CHECK_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_OFF;
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, x, y, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}
