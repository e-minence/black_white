////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������� ( ���j���[��� )
 * @file   research_menu.c
 * @author obata
 * @date   2010.02.03
 */
////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "bg_font.h"
#include "queue.h"
#include "research_menu.h"
#include "research_menu_index.h"
#include "research_menu_def.h"
#include "research_menu_data.cdat"
#include "research_common.h"

#include "print/gf_font.h"          // for GFL_FONT_xxxx
#include "print/printsys.h"         // for PRINTSYS_xxxx
#include "system/gfl_use.h"         // for GFUser_xxxx
#include "gamesystem/game_beacon.h" // for GAMEBEACON_xxxx
#include "sound/pm_sndsys.h"        // for PMSND_xxxx
#include "sound/wb_sound_data.sadl" // for SEQ_SE_XXXX

#include "system/main.h"                    // for HEAPID_xxxx
#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_research_radar_xxxx
#include "arc/font/font.naix"               // for NARC_font_xxxx
#include "arc/message.naix"                 // for NARC_message_xxxx
#include "msg/msg_research_radar.h"         // for str_xxxx
#include "obj_NANR_LBLDEFS.h"               // for NANR_obj_xxxx


//==============================================================================================
// ������������� ���[�N
//==============================================================================================
struct _RESEARCH_MENU_WORK
{ 
  HEAPID       heapID;  // �q�[�vID
  GFL_FONT*    font;    // �t�H���g
  GFL_MSGDATA* message[ MESSAGE_NUM ]; // ���b�Z�[�W

  QUEUE*               seqQueue;      // �V�[�P���X�L���[
  RESEARCH_MENU_SEQ    seq;           // �����V�[�P���X
  u32                  seqCount;      // �V�[�P���X�J�E���^
  BOOL                 seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  RESEARCH_MENU_RESULT result;        // ��ʏI������

  MENU_ITEM cursorPos;     // �J�[�\���ʒu
  BOOL      newEntryFlag;  // �V�����l���Ƃ����������ǂ���

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL touchHitTable[ TOUCH_AREA_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont[ BG_FONT_NUM ];

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ];  // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];                // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                  // �Z���A�N�^�[���[�N
};


//==============================================================================================
// ������J�֐�
//==============================================================================================
// �V�[�P���X����
static void Main_SETUP   ( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void Main_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void Main_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_WAIT
static void Main_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void Main_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP

// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_MENU_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSequence( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq ); // ���̃V�[�P���X��o�^����
static void FinishCurrentSequence( RESEARCH_MENU_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSequence( RESEARCH_MENU_WORK* work ); // �����V�[�P���X��ύX����
static void SetSequence( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result ); // ��ʏI�����ʂ�ݒ肷��

// �V�[�P���X����������
static void InitSequence_SETUP   ( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void InitSequence_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void InitSequence_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_WAIT
static void InitSequence_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void InitSequence_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP

// �V�[�P���X�I������
static void FinishSequence_SETUP   ( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void FinishSequence_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void FinishSequence_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_WAIT
static void FinishSequence_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void FinishSequence_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP

// �f�[�^�X�V
static void CheckNewEntry( RESEARCH_MENU_WORK* work );
// �J�[�\���ړ�
static void MoveCursorUp  ( RESEARCH_MENU_WORK* work );
static void MoveCursorDown( RESEARCH_MENU_WORK* work );

// �t�H���g
static void InitFont  ( RESEARCH_MENU_WORK* work );
static void CreateFont( RESEARCH_MENU_WORK* work );
static void DeleteFont( RESEARCH_MENU_WORK* work );
// ���b�Z�[�W
static void InitMessages  ( RESEARCH_MENU_WORK* work );
static void CreateMessages( RESEARCH_MENU_WORK* work );
static void DeleteMessages( RESEARCH_MENU_WORK* work );
// �^�b�`�̈�
static void SetupTouchArea( RESEARCH_MENU_WORK* work );

static void InitSeqQueue  ( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[ ������
static void CreateSeqQueue( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[ �쐬
static void DeleteSeqQueue( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[ �j��

// BG
static void SetupBG  ( RESEARCH_MENU_WORK* work );
static void CleanUpBG( RESEARCH_MENU_WORK* work );
// SUB-BG ( �E�B���h�E�� )
static void SetupSubBG_WINDOW  ( RESEARCH_MENU_WORK* work );
static void CleanUpSubBG_WINDOW( RESEARCH_MENU_WORK* work );
// SUB-BG ( �t�H���g�� )
static void SetupSubBG_FONT  ( RESEARCH_MENU_WORK* work );
static void CleanUpSubBG_FONT( RESEARCH_MENU_WORK* work );
// MAIN-BG ( �E�B���h�E�� )
static void SetupMainBG_WINDOW  ( RESEARCH_MENU_WORK* work );
static void CleanUpMainBG_WINDOW( RESEARCH_MENU_WORK* work );
// MAIN-BG ( �t�H���g�� )
static void SetupMainBG_FONT  ( RESEARCH_MENU_WORK* work );
static void CleanUpMainBG_FONT( RESEARCH_MENU_WORK* work );

// ������`��I�u�W�F�N�g
static void InitBGFonts  ( RESEARCH_MENU_WORK* work );
static void CreateBGFonts( RESEARCH_MENU_WORK* work );
static void DeleteBGFonts( RESEARCH_MENU_WORK* work );

// ���j���[���ڑ���
static void MenuItemSwitchOn ( MENU_ITEM menuItem );
static void MenuItemSwitchOff( MENU_ITEM menuItem );

// OBJ �V�X�e��
static void CreateClactSystem( RESEARCH_MENU_WORK* work );
static void DeleteClactSystem( RESEARCH_MENU_WORK* work );
// SUB-OBJ ���\�[�X
static void RegisterSubObjResources( RESEARCH_MENU_WORK* work );
static void ReleaseSubObjResources ( RESEARCH_MENU_WORK* work );
// MAIN-OBJ ���\�[�X
static void RegisterMainObjResources( RESEARCH_MENU_WORK* work );
static void ReleaseMainObjResources ( RESEARCH_MENU_WORK* work );
// �Z���A�N�^�[���j�b�g
static void InitClactUnits  ( RESEARCH_MENU_WORK* work );
static void CreateClactUnits( RESEARCH_MENU_WORK* work );
static void DeleteClactUnits( RESEARCH_MENU_WORK* work );
// �Z���A�N�^�[���[�N
static void InitClactWorks  ( RESEARCH_MENU_WORK* work );
static void CreateClactWorks( RESEARCH_MENU_WORK* work );
static void DeleteClactWorks( RESEARCH_MENU_WORK* work );
// OBJ �A�N�Z�X
static u32 GetObjResourceRegisterIndex( const RESEARCH_MENU_WORK* work, OBJ_RESOURCE_ID resID );
static GFL_CLUNIT* GetClactUnit( const RESEARCH_MENU_WORK* work, CLUNIT_INDEX unitIdx );
static GFL_CLWK*   GetClactWork( const RESEARCH_MENU_WORK* work, CLWK_INDEX wkIdx );

// "new" �A�C�R��
static void NewIconDispOn ( const RESEARCH_MENU_WORK* work );
static void NewIconDispOff( const RESEARCH_MENU_WORK* work );

// �f�o�b�O
static void DebugPrint_seqQueue( const RESEARCH_MENU_WORK* work );


//==============================================================================================
// ������������� ����֐�
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̐���
 *
 * @param heapID
 */
//----------------------------------------------------------------------------------------------
RESEARCH_MENU_WORK* CreateResearchMenuWork( HEAPID heapID )
{
  int i;
  RESEARCH_MENU_WORK* work;

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_MENU_WORK) );

  // ������
  work->seq          = RESEARCH_MENU_SEQ_SETUP;
  work->seqFinishFlag= FALSE;
  work->seqCount     = 0;
  work->result       = RESEARCH_MENU_RESULT_NONE;
  work->cursorPos    = MENU_ITEM_CHANGE_RESEARCH;
  work->newEntryFlag = FALSE;
  work->heapID       = heapID;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitSeqQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );

  CreateSeqQueue( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create work\n" );

  return work;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//----------------------------------------------------------------------------------------------
void DeleteResearchMenuWork( RESEARCH_MENU_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  }
  DeleteSeqQueue( work ); // �V�[�P���X�L���[
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete work\n" );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
RESEARCH_MENU_RESULT ResearchMenuMain( RESEARCH_MENU_WORK* work )
{
  // �V�[�P���X���Ƃ̏���
  switch( work->seq )
  {
  case RESEARCH_MENU_SEQ_SETUP:     Main_SETUP   ( work );  break;
  case RESEARCH_MENU_SEQ_STAND_BY:  Main_STAND_BY( work );  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:  Main_KEY_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:  Main_FADE_OUT( work );  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:  Main_CLEAN_UP( work );  break;
  case RESEARCH_MENU_SEQ_FINISH:    return work->result;
  default:  GF_ASSERT(0);
  }

  // �f�[�^�X�V
  CheckNewEntry( work );

  // �Z���A�N�^�[�V�X�e�� ���C������
  GFL_CLACT_SYS_Main();

  // �V�[�P���X�J�E���^�X�V
  CountUpSeqCount( work ); 

  // �V�[�P���X�X�V
  SwitchSequence( work );

  return RESEARCH_MENU_RESULT_CONTINUE;
}


//==============================================================================================
// ���V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_SETUP( RESEARCH_MENU_WORK* work )
{
  CreateFont( work );
  CreateMessages( work );
  SetupTouchArea( work );

  // BG ����
  SetupBG           ( work );
  SetupSubBG_WINDOW ( work );
  SetupSubBG_FONT   ( work );
  SetupMainBG_WINDOW( work );
  SetupMainBG_FONT  ( work );

  // ������`��I�u�W�F�N�g ����
  CreateBGFonts( work );

  // OBJ ����
  CreateClactSystem( work );
  RegisterSubObjResources( work );
  RegisterMainObjResources( work );
  CreateClactUnits( work );
  CreateClactWorks( work );

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  // ���̃V�[�P���X���Z�b�g
  SetNextSequence( work, RESEARCH_MENU_SEQ_STAND_BY ); 

  // �V�[�P���X�I��
  FinishCurrentSequence( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_STAND_BY( RESEARCH_MENU_WORK* work )
{
  int trg;
  int touchedAreaIdx;

  trg            = GFL_UI_KEY_GetTrg();
  touchedAreaIdx = GFL_UI_TP_HitTrg( work->touchHitTable );

  //--------------------
  // �\���L�[ or A or B
  if( (trg & PAD_KEY_UP) ||
      (trg & PAD_KEY_DOWN) ||
      (trg & PAD_KEY_LEFT) ||
      (trg & PAD_KEY_RIGHT) ||
      (trg & PAD_BUTTON_A) || 
      (trg & PAD_BUTTON_B) ) {
    SetNextSequence( work, RESEARCH_MENU_SEQ_KEY_WAIT );
    FinishCurrentSequence( work );
  }

  //-------------------------------------
  //�u�������e��ύX����v�{�^�����^�b�`
  if( touchedAreaIdx == TOUCH_AREA_CHANGE_BUTTON ) {
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                    // ���艹
    SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT ); // ��ʏI�����ʂ�����
    SetNextSequence( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }
  //-------------------------------------
  //�u�����񍐂��m�F����v�{�^�����^�b�`
  if( touchedAreaIdx == TOUCH_AREA_CHECK_BUTTON ) {
    MoveCursorDown( work );
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                    // ���艹
    SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK );  // ��ʏI�����ʂ�����
    SetNextSequence( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }

  //------------------
  // L �{�^�� (DEBUG)
  if( trg & PAD_BUTTON_L ) {
    DEBUG_GAMEBEACON_Set_NewEntry();
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  int trg;
  int touchedAreaIdx;

  trg            = GFL_UI_KEY_GetTrg();
  touchedAreaIdx = GFL_UI_TP_HitTrg( work->touchHitTable );

  //--------
  // �� �L�[
  if( trg & PAD_KEY_UP ) {
    MoveCursorUp( work );
  }

  //--------
  // �� �L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveCursorDown( work );
  }

  //----------------------------
  //�u�������e��ύX����v�{�^��
  if( ( (trg & PAD_BUTTON_A) && (work->cursorPos == MENU_ITEM_CHANGE_RESEARCH) ) ||
      ( touchedAreaIdx == TOUCH_AREA_CHANGE_BUTTON ) ) {
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                    // ���艹
    SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT ); // ��ʏI�����ʂ�����
    SetNextSequence( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }
  //----------------------------
  //�u�����񍐂��m�F����v�{�^��
  if( ( (trg & PAD_BUTTON_A) && (work->cursorPos == MENU_ITEM_CHECK_RESEARCH) ) ||
      ( touchedAreaIdx == TOUCH_AREA_CHECK_BUTTON ) ) {
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                   // ���艹
    SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK ); // ��ʏI�����ʂ�����
    SetNextSequence( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );               // �L�����Z����
    SetResult( work, RESEARCH_MENU_RESULT_EXIT ); // ��ʏI�����ʂ�����
    SetNextSequence( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }

  //------------------
  // L �{�^�� (DEBUG)
  if( trg & PAD_BUTTON_L ) {
    DEBUG_GAMEBEACON_Set_NewEntry();
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // OBJ ��Еt��
  DeleteClactWorks( work );
  DeleteClactUnits( work );
  ReleaseSubObjResources ( work );
  ReleaseMainObjResources( work );
  DeleteClactSystem ( work );

  // ������`��I�u�W�F�N�g ��Еt��
  DeleteBGFonts( work );

  // BG ��Еt��
  CleanUpMainBG_FONT  ( work );
  CleanUpMainBG_WINDOW( work );
  CleanUpSubBG_FONT   ( work );
  CleanUpSubBG_WINDOW ( work );
  CleanUpBG           ( work );

  DeleteMessages( work );
  DeleteFont( work );

  SetNextSequence( work, RESEARCH_MENU_SEQ_FINISH );
  FinishCurrentSequence( work );
}


//==============================================================================================
// ���V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_MENU_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq )
  {
  case RESEARCH_MENU_SEQ_SETUP:     maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_STAND_BY:  maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:  maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:  maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:  maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_FINISH:    maxCount = 0xffffffff;  break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < work->seqCount )
  { 
    work->seqCount = maxCount;
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X��o�^����
 *
 * @param work
 * @param nextSeq �o�^����V�[�P���X
 */
//----------------------------------------------------------------------------------------------
static void SetNextSequence( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishCurrentSequence( RESEARCH_MENU_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish current sequence\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SwitchSequence( RESEARCH_MENU_WORK* work )
{
  RESEARCH_MENU_SEQ nextSeq;

  if( work->seqFinishFlag == FALSE ){ return; }  // ���݂̃V�[�P���X���I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->seqQueue ) ){ return; } // �V�[�P���X�L���[�ɓo�^����Ă��Ȃ�

  // �ύX
  nextSeq = QUEUE_DeQueue( work->seqQueue );
  SetSequence( work, nextSeq ); 

  // DEBUG: �V�[�P���X�L���[��\��
  DebugPrint_seqQueue( work );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work
 * @parma nextSeq �ݒ肷��V�[�P���X
 */
//----------------------------------------------------------------------------------------------
static void SetSequence( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq )
  {
  case RESEARCH_MENU_SEQ_SETUP:    FinishSequence_SETUP( work );     break;
  case RESEARCH_MENU_SEQ_STAND_BY: FinishSequence_STAND_BY( work );  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT: FinishSequence_KEY_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT: FinishSequence_FADE_OUT( work );  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP: FinishSequence_CLEAN_UP( work );  break;
  case RESEARCH_MENU_SEQ_FINISH:   break;
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq           = nextSeq;
  work->seqCount      = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq )
  {
  case RESEARCH_MENU_SEQ_SETUP:    InitSequence_SETUP( work );     break;
  case RESEARCH_MENU_SEQ_STAND_BY: InitSequence_STAND_BY( work );  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT: InitSequence_KEY_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT: InitSequence_FADE_OUT( work );  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP: InitSequence_CLEAN_UP( work );  break;
  case RESEARCH_MENU_SEQ_FINISH:   break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set seq ==> " );
  switch( nextSeq )
  {
  case RESEARCH_MENU_SEQ_SETUP:    OS_TFPrintf( PRINT_TARGET, "SETUP" );     break;
  case RESEARCH_MENU_SEQ_STAND_BY: OS_TFPrintf( PRINT_TARGET, "STAND_BY" );  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT: OS_TFPrintf( PRINT_TARGET, "KEY_WAIT" );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT: OS_TFPrintf( PRINT_TARGET, "FADE_OUT" );  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP: OS_TFPrintf( PRINT_TARGET, "CLEAN_UP" );  break;
  case RESEARCH_MENU_SEQ_FINISH:   OS_TFPrintf( PRINT_TARGET, "FINISH" );    break;
  default:  GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_SETUP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[�҂��V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_STAND_BY( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq STAND_BY\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
  MenuItemSwitchOn( work->cursorPos );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq FADE_OUT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_SETUP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_STAND_BY( RESEARCH_MENU_WORK* work )
{
  // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
  MenuItemSwitchOn( work->cursorPos );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq STAND_BY\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq FADE_OUT\n" );
}

//==============================================================================================
// ���f�[�^�X�V
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�����l���Ƃ����������ǂ������`�F�b�N����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CheckNewEntry( RESEARCH_MENU_WORK* work )
{
  if( work->newEntryFlag == FALSE )
  {
    // �V�����l���Ƃ�������
    if( GAMEBEACON_Get_NewEntry() == TRUE )
    {
      work->newEntryFlag = TRUE;
      NewIconDispOn( work );      // "new" �A�C�R����\������
    }
  }
}

//==============================================================================================
// ���J�[�\���ړ�
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\������ֈړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
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

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor ==> %d\n", work->cursorPos );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\�������ֈړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
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

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor ==> %d\n", work->cursorPos );
}


//==============================================================================================
// ����ʏI������
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//----------------------------------------------------------------------------------------------
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_MENU_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set result (%d)\n", result );
}


//==============================================================================================
// ���t�H���g
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitFont( RESEARCH_MENU_WORK* work )
{
  // ������
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateFont( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, TRUE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteFont( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete font\n" );
}


//==============================================================================================
// �����b�Z�[�W
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitMessages( RESEARCH_MENU_WORK* work )
{
  int msgIdx;

  // ������
  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    work->message[ msgIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateMessages( RESEARCH_MENU_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    // ���d����
    GF_ASSERT( work->message[ msgIdx ] == NULL );

    // �쐬
    work->message[ msgIdx ] = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                              ARCID_MESSAGE, 
                                              MessageDataID[ msgIdx ],
                                              work->heapID ); 
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteMessages( RESEARCH_MENU_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] );

    // �폜
    GFL_MSG_Delete( work->message[ msgIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �^�b�`�͈͂��쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupTouchArea( RESEARCH_MENU_WORK* work )
{
  int idx;

  for( idx=0; idx < TOUCH_AREA_NUM; idx++ )
  {
    work->touchHitTable[ idx ].rect.left   = TouchAreaInitData[ idx ].left;
    work->touchHitTable[ idx ].rect.right  = TouchAreaInitData[ idx ].right;
    work->touchHitTable[ idx ].rect.top    = TouchAreaInitData[ idx ].top;
    work->touchHitTable[ idx ].rect.bottom = TouchAreaInitData[ idx ].bottom;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create touch hit table\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_MENU_WORK* work )
{
  // ������
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL );

  // �쐬
  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->seqQueue );

  // �폜
  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete seq queue\n" );
}


//==============================================================================================
// ��BG
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupBG( RESEARCH_MENU_WORK* work )
{ 
  // BG ���[�h
  GFL_BG_SetBGMode( &BGSysHeader2D );

  // BG �R���g���[��
  GFL_BG_SetBGControl( SUB_BG_WINDOW,  &SubBGControl_WINDOW,  GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_FONT,    &SubBGControl_FONT,    GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_WINDOW, &MainBGControl_WINDOW, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_FONT,   &MainBGControl_FONT,   GFL_BG_MODE_TEXT );

  // ���ݒ�
  GFL_BG_SetVisible( SUB_BG_BACK,    VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_RADAR,   VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_WINDOW,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_FONT,    VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_BACK,   VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_WINDOW, VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_FONT,   VISIBLE_ON );

  // ���u�����f�B���O
  G2S_SetBlendAlpha( SUB_BG_BLEND_TARGET_1, SUB_BG_BLEND_TARGET_2, 
                     SUB_BG_BLEND_WEIGHT_1, SUB_BG_BLEND_WEIGHT_2 );
  G2_SetBlendAlpha( MAIN_BG_BLEND_TARGET_1, MAIN_BG_BLEND_TARGET_2, 
                    MAIN_BG_BLEND_WEIGHT_1, MAIN_BG_BLEND_WEIGHT_2 );

  // �r�b�g�}�b�v�E�B���h�E �V�X�e��������
  GFL_BMPWIN_Init( work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup BG\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
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


//==============================================================================================
// ������ �E�B���h�EBG��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupSubBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, work->heapID ); 

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

//----------------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-WINDOW\n" );
}


//==============================================================================================
// ������ �t�H���gBG��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RESEARCH_MENU_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup SUB-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RESEARCH_MENU_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-FONT\n" );
}


//==============================================================================================
// ������� �E�B���h�EBG��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupMainBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, work->heapID ); 

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

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-WINDOW\n" );
}


//==============================================================================================
// ������� �t�H���gBG��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RESEARCH_MENU_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup MAIN-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RESEARCH_MENU_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-FONT\n" );
}


//==============================================================================================
// ��������`��I�u�W�F�N�g
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitBGFonts( RESEARCH_MENU_WORK* work )
{
  int idx;

  for( idx=0; idx < BG_FONT_NUM; idx++ )
  {
    work->BGFont[ idx ] = NULL; 
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateBGFonts( RESEARCH_MENU_WORK* work )
{
  int i;

  // ������`��I�u�W�F�N�g�쐬
  for( i=0; i<BG_FONT_NUM; i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->BGFont[i] == NULL ); 

    // �����p�����[�^�I��
    param.BGFrame       = BGFontInitData[i].BGFrame;
    param.posX          = BGFontInitData[i].posX;
    param.posY          = BGFontInitData[i].posY;
    param.sizeX         = BGFontInitData[i].sizeX;
    param.sizeY         = BGFontInitData[i].sizeY;
    param.offsetX       = BGFontInitData[i].offsetX;
    param.offsetY       = BGFontInitData[i].offsetY;
    param.paletteNo     = BGFontInitData[i].paletteNo;
    param.colorNo_L     = BGFontInitData[i].colorNo_L;
    param.colorNo_S     = BGFontInitData[i].colorNo_S;
    param.colorNo_B     = BGFontInitData[i].colorNo_B;
    param.centeringFlag = BGFontInitData[i].softCentering;
    msgData             = work->message[ BGFontInitData[i].messageIdx ];
    strID               = BGFontInitData[i].stringIdx;

    // ����
    work->BGFont[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetMessage( work->BGFont[i], strID );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create BGFonts\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBGFonts( RESEARCH_MENU_WORK* work )
{
  int i;
  
  // ������`��I�u�W�F�N�g�j��
  for( i=0; i<BG_FONT_NUM; i++ )
  {
    GF_ASSERT( work->BGFont[i] );
    BG_FONT_Delete( work->BGFont[i] );
    work->BGFont[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete BGFonts\n" ); 
}


//==============================================================================================
// �����j���[���ڑ���
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ�I����Ԃɂ���
 *
 * @param menuItem �I����Ԃɂ��郁�j���[���ڂ��w��
 */
//----------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ��I����Ԃɂ���
 *
 * @param menuItem ��I����Ԃɂ��郁�j���[���ڂ��w��
 */
//----------------------------------------------------------------------------------------------
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

//==============================================================================================
// ��OBJ
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e�����쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateClactSystem( RESEARCH_MENU_WORK* work )
{
  // �V�X�e���쐬
  GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // VBlank ���荞�݊֐���o�^
  GFUser_SetVIntrFunc( GFL_CLACT_SYS_VBlankFunc );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create clact system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_MENU_WORK* work )
{
  // VBkank ���荞�݊֐�������
  GFUser_ResetVIntrFunc();

  // �V�X�e���j��
  GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete clact system\n" );
}


//==============================================================================================
// ��SUB-OBJ ���\�[�X
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void RegisterSubObjResources( RESEARCH_MENU_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID );

  // ���\�[�X��o�^
  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_research_radar_graphic_obj_NCGR, 
                                      FALSE, CLSYS_DRAW_SUB, heapID ); 

  palette = GFL_CLGRP_PLTT_Register( arcHandle, 
                                     NARC_research_radar_graphic_obj_NCLR,
                                     CLSYS_DRAW_SUB, 0, heapID );

  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_research_radar_graphic_obj_NCER,
                                           NARC_research_radar_graphic_obj_NANR,
                                           heapID ); 
  // �o�^�C���f�b�N�X���L��
  work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ]  = character;
  work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ]    = palette;
  work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] = cellAnime;

  GFL_ARC_CloseDataHandle( arcHandle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: register SUB-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RESEARCH_MENU_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release SUB-OBJ resources\n" );
}


//==============================================================================================
// ��MAIN-OBJ ���\�[�X
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void RegisterMainObjResources( RESEARCH_MENU_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID );

  // ���\�[�X��o�^
  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_research_radar_graphic_obj_NCGR, 
                                      FALSE, CLSYS_DRAW_MAIN, heapID ); 

  palette = GFL_CLGRP_PLTT_Register( arcHandle, 
                                     NARC_research_radar_graphic_obj_NCLR,
                                     CLSYS_DRAW_MAIN, 0, heapID );

  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_research_radar_graphic_obj_NCER,
                                           NARC_research_radar_graphic_obj_NANR,
                                           heapID ); 
  // �o�^�C���f�b�N�X���L��
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ]  = character;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ]    = palette;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] = cellAnime;

  GFL_ARC_CloseDataHandle( arcHandle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: register MAIN-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RESEARCH_MENU_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release MAIN-OBJ resources\n" );
}


//==============================================================================================
// ���Z���A�N�^�[���j�b�g
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitClactUnits( RESEARCH_MENU_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void CreateClactUnits( RESEARCH_MENU_WORK* work )
{
  int unitIdx;
  u16 workNum;
  u8 priority;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] == NULL );

    workNum  = ClactUnitWorkSize[ unitIdx ];
    priority = ClactUnitPriority[ unitIdx ];
    work->clactUnit[ unitIdx ] = GFL_CLACT_UNIT_Create( workNum, priority, work->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactUnits( RESEARCH_MENU_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete clact units\n" );
}


//==============================================================================================
// ���Z���A�N�^�[���[�N
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitClactWorks( RESEARCH_MENU_WORK* work )
{
  int wkIdx;

  // ������
  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    work->clactWork[ wkIdx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateClactWorks( RESEARCH_MENU_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    GFL_CLUNIT* unit;
    GFL_CLWK_DATA wkData;
    u32 charaIdx, paletteIdx, cellAnimeIdx;
    u16 surface;

    // ���d����
    GF_ASSERT( work->clactWork[ wkIdx ] == NULL );

    // �����p�����[�^�I��
    wkData.pos_x   = ClactWorkInitData[ wkIdx ].posX;
    wkData.pos_y   = ClactWorkInitData[ wkIdx ].posY;
    wkData.anmseq  = ClactWorkInitData[ wkIdx ].animeSeq;
    wkData.softpri = ClactWorkInitData[ wkIdx ].softPriority; 
    wkData.bgpri   = ClactWorkInitData[ wkIdx ].BGPriority; 
    unit           = GetClactUnit( work, ClactWorkInitData[ wkIdx ].unitIdx );
    charaIdx       = GetObjResourceRegisterIndex( work, ClactWorkInitData[ wkIdx ].characterResID );
    paletteIdx     = GetObjResourceRegisterIndex( work, ClactWorkInitData[ wkIdx ].paletteResID );
    cellAnimeIdx   = GetObjResourceRegisterIndex( work, ClactWorkInitData[ wkIdx ].cellAnimeResID );
    surface        = ClactWorkInitData[ wkIdx ].setSurface;

    // ����
    work->clactWork[ wkIdx ] = GFL_CLACT_WK_Create( 
        unit, charaIdx, paletteIdx, cellAnimeIdx, &wkData, surface, work->heapID );

    // ��\���ɐݒ�
    GFL_CLACT_WK_SetDrawEnable( work->clactWork[ wkIdx ], FALSE );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactWorks( RESEARCH_MENU_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    // ��������Ă��Ȃ�
    GF_ASSERT( work->clactWork[ wkIdx ] );

    // �j��
    GFL_CLACT_WK_Remove( work->clactWork[ wkIdx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete clact works\n" );
}


//==============================================================================================
// ��OBJ �A�N�Z�X
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X�̓o�^�C���f�b�N�X���擾����
 *
 * @param work
 * @param resID ���\�[�XID
 *
 * @return �w�肵�����\�[�X�̓o�^�C���f�b�N�X
 */
//----------------------------------------------------------------------------------------------
static u32 GetObjResourceRegisterIndex( const RESEARCH_MENU_WORK* work, OBJ_RESOURCE_ID resID )
{
  return work->objResRegisterIdx[ resID ];
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���j�b�g�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���j�b�g
 */
//----------------------------------------------------------------------------------------------
static GFL_CLUNIT* GetClactUnit( const RESEARCH_MENU_WORK* work, CLUNIT_INDEX unitIdx )
{
  return work->clactUnit[ unitIdx ];
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���[�N�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���[�N
 */
//----------------------------------------------------------------------------------------------
static GFL_CLWK* GetClactWork( const RESEARCH_MENU_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}


//==============================================================================================
// ��"new" �A�C�R��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * "new" �A�C�R����\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void NewIconDispOn( const RESEARCH_MENU_WORK* work )
{
  GFL_CLWK* clactWork;

  clactWork = GetClactWork( work, CLWK_NEW_ICON );
  GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( clactWork, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( clactWork, FX32_ONE );
  GFL_CLACT_WK_SetAnmFrame( clactWork, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: new icon disp on\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * "new" �A�C�R�����\���ɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void NewIconDispOff( const RESEARCH_MENU_WORK* work )
{
  GFL_CLWK* clactWork;

  clactWork = GetClactWork( work, CLWK_NEW_ICON );
  GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: new icon disp off\n" );
}


//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_MENU_WORK* work )
{
  int i;
  int dataNum;
  int value;

  // �L���[���̃f�[�^�̌����擾
  dataNum = QUEUE_GetDataNum( work->seqQueue );

  // �S�Ẵf�[�^���o��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: seq queue = " );
  for( i=0; i < dataNum; i++ )
  { 
    value = QUEUE_PeekData( work->seqQueue, i );
    
    switch( value )
    {
    case RESEARCH_MENU_SEQ_SETUP:     OS_TFPrintf( PRINT_TARGET, "SETUP " );    break;
    case RESEARCH_MENU_SEQ_STAND_BY:  OS_TFPrintf( PRINT_TARGET, "STAND_BY " );    break;
    case RESEARCH_MENU_SEQ_KEY_WAIT:  OS_TFPrintf( PRINT_TARGET, "KEY-WAIT " ); break;
    case RESEARCH_MENU_SEQ_FADE_OUT:  OS_TFPrintf( PRINT_TARGET, "FADE_OUT " ); break;
    case RESEARCH_MENU_SEQ_CLEAN_UP:  OS_TFPrintf( PRINT_TARGET, "CLEAN-UP " ); break;
    case RESEARCH_MENU_SEQ_FINISH:    OS_TFPrintf( PRINT_TARGET, "FINISH " );   break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 
