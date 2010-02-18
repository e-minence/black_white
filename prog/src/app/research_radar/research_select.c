////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX���
 * @file   research_select.c
 * @author obata
 * @date   2010.02.03
 */
////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "bg_font.h"
#include "research_select.h"
#include "research_select_index.h"
#include "research_select_def.h"
#include "research_select_data.cdat"
#include "research_common.h"

#include "print/gf_font.h"           // for GFL_FONT_xxxx
#include "print/printsys.h"          // for PRINTSYS_xxxx
#include "system/gfl_use.h"          // for GFUser_xxxx
#include "gamesystem/game_beacon.h"  // for GAMEBEACON_xxxx

#include "system/main.h"                    // for HEAPID_xxxx
#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_research_radar_xxxx
#include "arc/font/font.naix"               // for NARC_font_xxxx
#include "arc/message.naix"                 // for NARC_message_xxxx
#include "msg/msg_research_radar.h"         // for str_xxxx
#include "msg/msg_questionnaire.h"          // for str_xxxx
#include "obj_NANR_LBLDEFS.h"               // for NANR_obj_xxxx
#include "topic_id.h"                       // for TOPIC_ID_xxxx
#include "question_id.h"                    // for QUESTION_ID_xxxx

#include "question_id_topic.cdat"        // for QuestionX_topic[]
#include "string_id_question.cdat"       // for StringID_question[]
#include "string_id_topic_title.cdat"    // for StringID_topicTitle[]
#include "string_id_topic_caption.cdat"  // for StringID_topicCaption[]


//==============================================================================================
// ������������� ���[�N
//==============================================================================================
struct _RESEARCH_SELECT_WORK
{ 
  HEAPID    heapID;  // �q�[�vID
  GFL_FONT* font;    // �t�H���g

  RESEARCH_SELECT_SEQ    seq;    // �����V�[�P���X
  RESEARCH_SELECT_RESULT result; // ��ʏI������

  // ���j���[����
  MENU_ITEM menuCursorPos;  // �J�[�\���ʒu

  // ��������
  u8 selectedTopicIDs[ SELECT_TOPIC_MAX_NUM ];  // �I��������������ID
  u8 selectedTopicNum;                          // �I�������������ڂ̐�

  u8  topicCursorPos;         // �J�[�\���ʒu
  u8  topicCursorNextPos;     // �ړ���̃J�[�\���ʒu 
  int topicScrollFrameCount;  // �X�N���[�� �t���[���J�E���^

  // ���b�Z�[�W
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont[ BG_FONT_NUM ];
  BG_FONT* TopicsBGFont[ TOPIC_ID_NUM ];  // ��������

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ];  // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];                // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                  // �Z���A�N�^�[���[�N
};


//----------------------------------------------------------------------------------------------
// ��LAYER 3 �V�[�P���X����
//----------------------------------------------------------------------------------------------
// �V�[�P���X����
static RESEARCH_SELECT_SEQ Main_SETUP      ( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_KEY_WAIT   ( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_SCROLL_WAIT( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_TO_CONFIRM ( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_CONFIRM    ( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_DETERMINE  ( RESEARCH_SELECT_WORK* work );
static RESEARCH_SELECT_SEQ Main_CLEAN_UP   ( RESEARCH_SELECT_WORK* work );

// �V�[�P���X����
static void SetResult( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
static void SwitchSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq ); // �����V�[�P���X��ύX����
static void SetSequence   ( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ seq );     // �����V�[�P���X��ݒ肷��

// VBlank
static void VBlankFunc( void );

//----------------------------------------------------------------------------------------------
// ��LAYER 3 ���͂ɑ΂��锽��
//---------------------------------------------------------------------------------------------- 
// ���j���[���ڃJ�[�\���̈ړ�
static void MoveMenuCursorUp  ( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����

// �������ڃJ�[�\���̈ړ�
static void MoveTopicCursorUp  ( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveTopicCursorDown( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����

// �������ڂ̑I��
static void SelectTopic( RESEARCH_SELECT_WORK* work ); // �������ڂ�I������

//----------------------------------------------------------------------------------------------
// ��LAYER 2 �ʑ���
//---------------------------------------------------------------------------------------------- 
// �J�[�\���ʒu�̕ύX
static void ShiftMenuCursorPos( RESEARCH_SELECT_WORK* work, int stride ); // ���j���[���ڃJ�[�\���ʒu��ύX����
static void SetTopicCursorNextPos( RESEARCH_SELECT_WORK* work, int stride ); // �������ڃJ�[�\���̈ړ����ݒ肷��
static void TopicCursorScrollStart( RESEARCH_SELECT_WORK* work );  // �������ڃJ�[�\���̃X�N���[�����J�n����

// ��������ID�̓o�^
static void RegisterTopicID( RESEARCH_SELECT_WORK* work ); // ��������ID��o�^����
static void ReleaseTopicID ( RESEARCH_SELECT_WORK* work ); // ��������ID����������
static BOOL IsTopicIDRegistered( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �I���ς݂��𔻒肷��

// �������ڂ̕\������
static void SetTopicCursorOn ( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetTopicCursorOff( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetTopicSelected   ( const RESEARCH_SELECT_WORK* work ); // �I������Ă����Ԃɂ���
static void SetTopicNotSelected( const RESEARCH_SELECT_WORK* work ); // �I������Ă��Ȃ���Ԃɂ���
static void UpdateSubDisplayStrings( RESEARCH_SELECT_WORK* work ); // ���ʂ̃J�[�\���ˑ�������\�����X�V����
static void UpdateTopicScroll( RESEARCH_SELECT_WORK* work );       // �X�N���[���X�V
static BOOL IsTopicScrollEnd ( const RESEARCH_SELECT_WORK* work ); // �X�N���[���I�����o
static u8  CalcTopicScreenPosLeft ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� (�X�N���[���P��)
static u8  CalcTopicScreenPosTop  ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� (�X�N���[���P��)
static int CalcTopicDisplayPosLeft( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� (�h�b�g�P��)
static int CalcTopicDisplayPosTop ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� (�h�b�g�P��)
static int CalcScreenScrollY      ( const RESEARCH_SELECT_WORK* work );             // y�������̃X�N���[���ʂ��Z�o����

// OBJ �\��
static void UpdateScrollControlPos( const RESEARCH_SELECT_WORK* work );  // �X�N���[���o�[�̂܂ݕ����̈ʒu���X�V����
static void UpdateTopicSelectIcons( const RESEARCH_SELECT_WORK* work );  // �������ڑI���A�C�R���̕\����Ԃ��X�V����

//----------------------------------------------------------------------------------------------
// ��LAYER 1 �f�[�^�A�N�Z�X
//----------------------------------------------------------------------------------------------
// �f�[�^�A�N�Z�X
static u32 GetObjResourceRegisterIndex( const RESEARCH_SELECT_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetClactUnit( const RESEARCH_SELECT_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g
static GFL_CLWK* GetClactWork( const RESEARCH_SELECT_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N

//----------------------------------------------------------------------------------------------
// ��LAYER 0 ����������/�I������
//----------------------------------------------------------------------------------------------
// ��ʂ̏���/��Еt�� ( BG )
static void SetupBG  ( RESEARCH_SELECT_WORK* work ); // BG ����
static void CleanUpBG( RESEARCH_SELECT_WORK* work ); // BG ��Еt��
static void SetupSubBG_WINDOW  ( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ����
static void CleanUpSubBG_WINDOW( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ��Еt��
static void SetupSubBG_FONT  ( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �t�H���g�� ) ����
static void CleanUpSubBG_FONT( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �t�H���g�� ) ��Еt��
static void SetupMainBG_BAR  ( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �o�[�� ) ����
static void CleanUpMainBG_BAR( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �o�[�� ) ��Еt��
static void SetupMainBG_WINDOW  ( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ����
static void CleanUpMainBG_WINDOW( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ��Еt��
static void SetupMainBG_FONT  ( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �t�H���g�� ) ����
static void CleanUpMainBG_FONT( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �t�H���g�� ) ��Еt��

// ��ʂ̏���/��Еt�� ( OBJ )
static void CreateClactSystem( RESEARCH_SELECT_WORK* work ); // OBJ �V�X�e�� ����
static void DeleteClactSystem( RESEARCH_SELECT_WORK* work ); // OBJ �V�X�e�� �j��
static void RegisterSubObjResources( RESEARCH_SELECT_WORK* work ); // SUB-OBJ ���\�[�X �o�^
static void ReleaseSubObjResources ( RESEARCH_SELECT_WORK* work ); // SUB-OBJ ���\�[�X ���
static void RegisterMainObjResources( RESEARCH_SELECT_WORK* work ); // MAIN-OBJ ���\�[�X �o�^
static void ReleaseMainObjResources ( RESEARCH_SELECT_WORK* work ); // MAIN-OBJ ���\�[�X ���
static void InitClactUnits  ( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g ������
static void CreateClactUnits( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g ����
static void DeleteClactUnits( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g �j��
static void InitClactWorks  ( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N ������
static void CreateClactWorks( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N ����
static void DeleteClactWorks( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N �j��

// �f�[�^�̏�����/����/�j��
static void InitFont  ( RESEARCH_SELECT_WORK* work ); // �t�H���g ������
static void CreateFont( RESEARCH_SELECT_WORK* work ); // �t�H���g ����
static void DeleteFont( RESEARCH_SELECT_WORK* work ); // �t�H���g �j��
static void InitMessages  ( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W ������
static void CreateMessages( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W ����
static void DeleteMessages( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W �j��
static void InitBGFonts  ( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g ������
static void CreateBGFonts( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g ����
static void DeleteBGFonts( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g �j��
static void InitSelectedTopicIDs( RESEARCH_SELECT_WORK* work ); // �I��������������ID

// DEBUG:
static void DebugPrint_SelectedTopicIDs( const RESEARCH_SELECT_WORK* work );  // �o�^�ς݂̒�������ID���o�͂���


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
RESEARCH_SELECT_WORK* CreateResearchSelectWork( HEAPID heapID )
{
  int i;
  RESEARCH_SELECT_WORK* work;

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_SELECT_WORK) );

  // ������
  work->seq                   = RESEARCH_SELECT_SEQ_SETUP;
  work->result                = RESEARCH_SELECT_RESULT_NONE;
  work->heapID                = heapID;
  work->menuCursorPos         = MENU_ITEM_DETERMINATION_OK;
  work->topicCursorPos        = 0;
  work->topicCursorNextPos    = 0;
  work->topicScrollFrameCount = 0;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitSelectedTopicIDs( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create work\n" );

  return work;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//----------------------------------------------------------------------------------------------
void DeleteResearchSelectWork( RESEARCH_SELECT_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  }
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete work\n" );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  // �V�[�P���X���Ƃ̏���
  switch( work->seq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        nextSeq = Main_SETUP   ( work );     break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     nextSeq = Main_KEY_WAIT( work );     break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  nextSeq = Main_SCROLL_WAIT( work );  break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   nextSeq = Main_TO_CONFIRM( work );   break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      nextSeq = Main_CONFIRM( work );      break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    nextSeq = Main_DETERMINE( work );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     nextSeq = Main_CLEAN_UP( work );     break;
  case RESEARCH_SELECT_SEQ_FINISH:       return work->result;
  default:  GF_ASSERT(0);
  }

  // �V�[�P���X�X�V
  SwitchSequence( work, nextSeq );

  // �Z���A�N�^�[�V�X�e�� ���C������
  GFL_CLACT_SYS_Main();

  return RESEARCH_SELECT_RESULT_CONTINUE;
}


//==============================================================================================
// ��VBlank
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank ���荞�ݏ���
 */
//----------------------------------------------------------------------------------------------
static void VBlankFunc( void )
{
  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();
}


//==============================================================================================
// ���V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_SELECT_SEQ_SETUP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_SETUP( RESEARCH_SELECT_WORK* work )
{
  CreateFont( work );
  CreateMessages( work );

  // BG ����
  SetupBG           ( work );
  SetupSubBG_WINDOW ( work );
  SetupSubBG_FONT   ( work );
  SetupMainBG_BAR   ( work );
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

  return RESEARCH_SELECT_SEQ_KEY_WAIT;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_SELECT_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;
  int trg;

  nextSeq = work->seq;
  trg     = GFL_UI_KEY_GetTrg();

  //--------
  // �� �L�[
  if( trg & PAD_KEY_UP )
  {
    MoveTopicCursorUp( work );
    nextSeq = RESEARCH_SELECT_SEQ_SCROLL_WAIT;  // �V�[�P���X�J��
  }

  //--------
  // �� �L�[
  if( trg & PAD_KEY_DOWN )
  {
    MoveTopicCursorDown( work );
    nextSeq = RESEARCH_SELECT_SEQ_SCROLL_WAIT;  // �V�[�P���X�J��
  }

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A )
  {
    SelectTopic( work );
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B )
  {
    SetResult( work, RESEARCH_SELECT_RESULT_TO_MENU );  // ��ʏI�����ʂ�����
    nextSeq = RESEARCH_SELECT_SEQ_CLEAN_UP;             // �V�[�P���X�J��
  }

  return nextSeq;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������҂��V�[�P���X ( RESEARCH_SELECT_SEQ_SCROLL_WAIT ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  nextSeq = work->seq;

  // �X�N���[������
  UpdateTopicScroll( work );       // �������ڃ{�^��
  UpdateScrollControlPos( work );  // �X�N���[���o�[�̂܂ݕ���
  UpdateTopicSelectIcons( work );  // �������ڑI���A�C�R��
  work->topicScrollFrameCount++;

  // �X�N���[���I��
  if( IsTopicScrollEnd(work) )
  {
    work->topicCursorPos = work->topicCursorNextPos;
    UpdateSubDisplayStrings( work );  // ���ʂ̃J�[�\���ˑ���������X�V
    SetTopicCursorOn( work );
    nextSeq = RESEARCH_SELECT_SEQ_KEY_WAIT;
  }

  return nextSeq;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_SELECT_SEQ_TO_CONFIRM ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_TO_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  nextSeq = work->seq;

  return nextSeq;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X ( RESEARCH_SELECT_SEQ_CONFIRM ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  nextSeq = work->seq;

  return nextSeq;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��V�[�P���X ( RESEARCH_SELECT_SEQ_DETERMINE ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  nextSeq = work->seq;

  return nextSeq;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_SELECT_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static RESEARCH_SELECT_SEQ Main_CLEAN_UP( RESEARCH_SELECT_WORK* work )
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
  CleanUpMainBG_BAR   ( work );
  CleanUpMainBG_WINDOW( work );
  CleanUpSubBG_FONT   ( work );
  CleanUpSubBG_WINDOW ( work );
  CleanUpBG           ( work );

  DeleteMessages( work );
  DeleteFont( work );

  return RESEARCH_SELECT_SEQ_FINISH;
}


//==============================================================================================
// ���V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 * @param nextSeq �ύX��̃V�[�P���X
 */
//----------------------------------------------------------------------------------------------
static void SwitchSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq )
{
  // �ω��Ȃ�
  if( work->seq == nextSeq){ return; }

  // �ύX
  SetSequence( work, nextSeq ); 
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work
 * @parma seq  �ݒ肷��V�[�P���X
 */
//----------------------------------------------------------------------------------------------
static void SetSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ seq )
{
  // �X�V
  work->seq = seq;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set seq ==> %d\n", seq );
}


//==============================================================================================
// �����j���[����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\������ֈړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void MoveMenuCursorUp( RESEARCH_SELECT_WORK* work )
{ 
  ShiftMenuCursorPos( work, -1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�������ֈړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void MoveMenuCursorDown( RESEARCH_SELECT_WORK* work )
{
  ShiftMenuCursorPos( work, 1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�����ړ�����
 *
 * @parma work
 * @param stride �ړ���
 */
//----------------------------------------------------------------------------------------------
static void ShiftMenuCursorPos( RESEARCH_SELECT_WORK* work, int stride )
{
  int nowPos;
  int nextPos;

  // �J�[�\���ʒu���X�V
  nowPos  = work->menuCursorPos;
  nextPos = (nowPos + stride + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  work->menuCursorPos = nextPos; 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: move menu cursor ==> %d\n", nextPos );
}


//==============================================================================================
// ����������
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ�I������B
 *        ���łɑI���ς݂Ȃ�o�^����������B
 *        �����łȂ����, �o�^����B
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SelectTopic( RESEARCH_SELECT_WORK* work )
{
  // �o�^�ς�
  if( IsTopicIDRegistered( work, work->topicCursorPos ) ) 
  {
    SetTopicNotSelected( work );  // �I�����Ă��Ȃ���Ԃɖ߂�
    ReleaseTopicID( work );       // �o�^���Ă�������ID����������
  }
  else
  {
    SetTopicSelected( work );  // �I�����Ă����Ԃɂ���
    RegisterTopicID( work );   // ����ID��o�^����
  }

  // �������ڑI���A�C�R�����X�V
  UpdateTopicSelectIcons( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �I���ςݒ�������ID���X�g������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSelectedTopicIDs( RESEARCH_SELECT_WORK* work )
{
  int i;

  // �I���ςݒ�������ID��������
  for( i=0; i < SELECT_TOPIC_MAX_NUM; i++ )
  { 
    work->selectedTopicIDs[i] = TOPIC_ID_DUMMY;
  }

  // �I�������������ڂ̐���������
  work->selectedTopicNum = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init select topic IDs\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ�I������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void RegisterTopicID( RESEARCH_SELECT_WORK* work )
{
  int topicID;
  int registerPos;

  // �I��Ώۂ� ���̓o�^�ꏊ������
  topicID     = work->topicCursorPos;
  registerPos = work->selectedTopicNum;

  // �G���[�`�F�b�N
  GF_ASSERT( registerPos < SELECT_TOPIC_MAX_NUM ); // �I�𐔃I�[�o�[
  GF_ASSERT( work->selectedTopicIDs[ registerPos ] == TOPIC_ID_DUMMY );  // �����̐����������Ă��Ȃ�

  // �I�������������ڂƂ��ēo�^
  work->selectedTopicIDs[ registerPos ] = topicID;
  work->selectedTopicNum++;

  // DEBUG:
  DebugPrint_SelectedTopicIDs( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ̑I�����L�����Z������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseTopicID( RESEARCH_SELECT_WORK* work )
{
  int topicID;
  int shiftPos;
  int registerPos;

  // �L�����Z���Ώۂ�����
  topicID = work->topicCursorPos;

  // �I������Ă���ID�̒�����, �J�[�\���ʒu�ɂ��鍀�ڂ�ID������
  for( registerPos=0; registerPos < work->selectedTopicNum; registerPos++ )
  {
    // ����
    if( work->selectedTopicIDs[ registerPos ] == topicID ){ break; }
  }

  // �G���[�`�F�b�N
  GF_ASSERT( registerPos < SELECT_TOPIC_MAX_NUM ); // �I������Ă��Ȃ�

  // ����
  for( shiftPos = registerPos; shiftPos < SELECT_TOPIC_MAX_NUM - 1; shiftPos++ )
  {
    work->selectedTopicIDs[ shiftPos ] = work->selectedTopicIDs[ shiftPos + 1 ];
  }
  work->selectedTopicIDs[ SELECT_TOPIC_MAX_NUM - 1 ] = TOPIC_ID_DUMMY;
  work->selectedTopicNum--;

  // DEBUG:
  DebugPrint_SelectedTopicIDs( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ��I������Ă��邩�ǂ����𔻒肷��
 *
 * @param work
 * @parma topicID ��������ID
 *
 * @return �w�肵���������ڂ��I������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------------------------
static BOOL IsTopicIDRegistered( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int idx;
  int selectedNum;

  // �I���ςݒ������ڂ̐�
  selectedNum = work->selectedTopicNum;

  // �I���ςݒ������ڃ��X�g����, �w�肳�ꂽ���ڂ�����
  for( idx=0; idx < selectedNum; idx++ )
  {
    // ����
    if( work->selectedTopicIDs[ idx ] == topicID )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\������Ɉړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void MoveTopicCursorUp( RESEARCH_SELECT_WORK* work )
{
  SetTopicCursorOff( work );          // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, -1 );  // �ړ����ݒ�
  TopicCursorScrollStart( work );     // �X�N���[���J�n
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\�������Ɉړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void MoveTopicCursorDown( RESEARCH_SELECT_WORK* work )
{
  SetTopicCursorOff( work );          // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, 1 );   // �ړ����ݒ�
  TopicCursorScrollStart( work );     // �X�N���[���J�n
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̈ړ����ݒ肷��
 *
 * @param work
 * @param stride �ړ���
 */
//----------------------------------------------------------------------------------------------
static void SetTopicCursorNextPos( RESEARCH_SELECT_WORK* work, int stride )
{
  int nowPos, nextPos;

  // �ړ���̃J�[�\���ʒu���Z�o
  nowPos  = work->topicCursorPos;
  nextPos = (nowPos + stride + TOPIC_ID_NUM) % TOPIC_ID_NUM;

  // �J�[�\���̈ړ����ݒ�
  work->topicCursorNextPos = nextPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set topic cursor next pos ==> %d\n", nextPos );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̃X�N���[�����J�n����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void TopicCursorScrollStart( RESEARCH_SELECT_WORK* work )
{
  // �t���[���J�E���^�����Z�b�g
  work->topicScrollFrameCount = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: topic cursor scroll start\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetTopicCursorOn( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u8 BGFrame;
  u8 left, top, width, height;
  u8 paletteNo;

  // �X�N���[���X�V�p�����[�^������
  topicID   = work->topicCursorPos;
  left      = CalcTopicScreenPosLeft( work, topicID );
  top       = CalcTopicScreenPosTop( work, topicID );
  width     = TOPIC_BUTTON_WIDTH;
  height    = TOPIC_BUTTON_HEIGHT; 
  BGFrame   = MAIN_BG_WINDOW;
  paletteNo = MAIN_BG_PALETTE_WINDOW_ON;

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, left, top, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//----------------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetTopicCursorOff( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u8 BGFrame;
  u8 left, top, width, height;
  u8 paletteNo;

  // �X�N���[���X�V�p�����[�^������
  topicID   = work->topicCursorPos;
  left      = CalcTopicScreenPosLeft( work, topicID );
  top       = CalcTopicScreenPosTop( work, topicID );
  width     = TOPIC_BUTTON_WIDTH;
  height    = TOPIC_BUTTON_HEIGHT; 
  BGFrame   = MAIN_BG_WINDOW;
  paletteNo = MAIN_BG_PALETTE_WINDOW_OFF;

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, left, top, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ� �I�����ꂽ��Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetTopicSelected( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u16* screenBuffer1;
  u16* screenBuffer2;
  int xOffset, yOffset;

  topicID = work->topicCursorPos;

  // �X�N���[���o�b�t�@���擾
  screenBuffer1 = GFL_BG_GetScreenBufferAdrs( MAIN_BG_WINDOW );
  screenBuffer2 = GFL_BG_GetScreenBufferAdrs( MAIN_BG_FONT );

  // �Y������X�N���[���f�[�^������1�L�������V�t�g����
  for( yOffset=0; yOffset < TOPIC_BUTTON_HEIGHT; yOffset++ )
  {
    for( xOffset=0; xOffset < TOPIC_BUTTON_WIDTH; xOffset++ )
    {
      int left, top, x, y;
      int srcPos, destPos;

      left    = CalcTopicScreenPosLeft( work, topicID );
      top     = CalcTopicScreenPosTop( work, topicID );
      x       = left + xOffset;
      y       = top  + yOffset;
      srcPos  = 32 * y + x;
      destPos = srcPos - 1;

      screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
      screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];
    }
  }

  // �ړ����ċ�ɂȂ����X�N���[�����N���A
  {
    int left, top, x, y;
    int srcPos, destPos;

    left    = CalcTopicScreenPosLeft( work, topicID );
    top     = CalcTopicScreenPosTop( work, topicID );
    x       = left + TOPIC_BUTTON_WIDTH - 1;
    y       = top;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];

    y++;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];

    y++;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];
  }
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
  GFL_BG_LoadScreenReq( MAIN_BG_FONT );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ� �I������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetTopicNotSelected( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u16* screenBuffer1;
  u16* screenBuffer2;
  int xOffset, yOffset;

  topicID = work->topicCursorPos;

  // �X�N���[���o�b�t�@���擾
  screenBuffer1 = GFL_BG_GetScreenBufferAdrs( MAIN_BG_WINDOW );
  screenBuffer2 = GFL_BG_GetScreenBufferAdrs( MAIN_BG_FONT );

  // �Y������X�N���[���f�[�^���E��1�L�������V�t�g����
  for( yOffset=TOPIC_BUTTON_HEIGHT-1; 0 <= yOffset; yOffset-- )
  {
    for( xOffset=TOPIC_BUTTON_WIDTH-1; 0 <= xOffset; xOffset-- )
    {
      int left, top, x, y;
      int srcPos, destPos;

      left    = CalcTopicScreenPosLeft( work, topicID );
      top     = CalcTopicScreenPosTop( work, topicID );
      x       = left + xOffset;
      y       = top  + yOffset;
      srcPos  = 32 * y + x;
      destPos = srcPos + 1;

      screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
      screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];
    }
  }

  // �ړ����ċ�ɂȂ����X�N���[�����N���A
  {
    int left, top, x, y;
    int srcPos, destPos;

    left    = CalcTopicScreenPosLeft( work, topicID );
    top     = CalcTopicScreenPosTop( work, topicID );
    x       = left;
    y       = top;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];

    y++;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];

    y++;
    srcPos  = 0;
    destPos = 32 * y + x;

    screenBuffer1[ destPos ] = screenBuffer1[ srcPos ];
    screenBuffer2[ destPos ] = screenBuffer2[ srcPos ];
  }
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
  GFL_BG_LoadScreenReq( MAIN_BG_FONT );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃X�N���[�� x ���W���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵����������ID���Y������X�N���[���͈͂̍���x���W (�X�N���[���P��)
 */
//----------------------------------------------------------------------------------------------
static u8 CalcTopicScreenPosLeft( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u8 left;

  // �f�t�H���g�̈ʒu
  left = TOPIC_BUTTON_X;

  // �I������Ă���ꍇ, 1�L���������ɂ���
  if( IsTopicIDRegistered( work, topicID ) ){ left -= 1; }

  return left;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃X�N���[�� y ���W���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵����������ID���Y������X�N���[���͈͂̍���y���W (�X�N���[���P��)
 */
//----------------------------------------------------------------------------------------------
static u8 CalcTopicScreenPosTop( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u8 top;

  // �f�t�H���g�̈ʒu
  top = TOPIC_BUTTON_Y + TOPIC_BUTTON_HEIGHT * topicID;

  return top;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̍���x���W���擾���� (�h�b�g�P��)
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵���������ڂ̍���x���W (�h�b�g�P��)
 */
//----------------------------------------------------------------------------------------------
static int CalcTopicDisplayPosLeft( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int left;

  // �f�t�H���g�̈ʒu���Z�o
  left = CalcTopicScreenPosLeft( work, topicID ) * DOT_PER_CHARA;

  // BG�X�N���[���̕����l������
  left -= GFL_BG_GetScreenScrollX( MAIN_BG_WINDOW );

  return left;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̍���y���W���擾���� (�h�b�g�P��)
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵���������ڂ̍���y���W (�h�b�g�P��)
 */
//----------------------------------------------------------------------------------------------
static int CalcTopicDisplayPosTop( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int top;

  // �f�t�H���g�̈ʒu���Z�o
  top = CalcTopicScreenPosTop( work, topicID ) * DOT_PER_CHARA;

  // BG�X�N���[���̕����l������
  top -= CalcScreenScrollY( work );

  return top;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃X�N���[����Ԃ��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateTopicScroll( RESEARCH_SELECT_WORK* work )
{
  int scrollSize;

  // �X�N���[���ʂ��Z�o
  scrollSize = CalcScreenScrollY( work );

  // �X�N���[�����N�G�X�g
  GFL_BG_SetScrollReq( MAIN_BG_WINDOW, GFL_BG_SCROLL_Y_SET, scrollSize );
  GFL_BG_SetScrollReq( MAIN_BG_FONT,   GFL_BG_SCROLL_Y_SET, scrollSize );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����I���������ǂ����𔻒肷��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static BOOL IsTopicScrollEnd ( const RESEARCH_SELECT_WORK* work )
{
  return ( SCROLL_FRAME < work->topicScrollFrameCount);
}

//----------------------------------------------------------------------------------------------
/**
 * @brief y�������̃X�N���[���ʂ��Z�o����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static int CalcScreenScrollY( const RESEARCH_SELECT_WORK* work )
{
  int scrollSize;
  int frame;
  int start, end, now;
  int screenTop, screenBottom;
  int min, max;


  // �X�N���[���I���ς�
  if( IsTopicScrollEnd(work) )
  {
    frame = SCROLL_FRAME;
  }
  else
  {
    frame = work->topicScrollFrameCount;
  }

  start        = (TOPIC_BUTTON_Y + TOPIC_BUTTON_HEIGHT * work->topicCursorPos) * DOT_PER_CHARA;
  end          = (TOPIC_BUTTON_Y + TOPIC_BUTTON_HEIGHT * work->topicCursorNextPos) * DOT_PER_CHARA;
  now          = start + (end - start) * frame / SCROLL_FRAME ;
  screenTop    = GFL_BG_GetScreenScrollY( MAIN_BG_WINDOW );
  screenBottom = screenTop + 192;
  min          = screenTop + SCROLL_TOP_MARGIN;
  max          = screenBottom - SCROLL_BOTTOM_MARGIN;
  scrollSize   = screenTop;

  if( now < min )
  {
    scrollSize = now - SCROLL_TOP_MARGIN;
  }
  else if( max < now )
  {
    scrollSize = now - 192 + SCROLL_BOTTOM_MARGIN; 
  } 
  return scrollSize;
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
static void SetResult( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_SELECT_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set result (%d)\n", result );
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
static void InitFont( RESEARCH_SELECT_WORK* work )
{
  // ������
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateFont( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, TRUE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteFont( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete font\n" );
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
static void InitMessages( RESEARCH_SELECT_WORK* work )
{
  int msgIdx;

  // ������
  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    work->message[ msgIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateMessages( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteMessages( RESEARCH_SELECT_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] );

    // �폜
    GFL_MSG_Delete( work->message[ msgIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete messages\n" );
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
static void SetupBG( RESEARCH_SELECT_WORK* work )
{ 
  // BG ���[�h
  GFL_BG_SetBGMode( &BGSysHeader2D );

  // BG �R���g���[��
  GFL_BG_SetBGControl( SUB_BG_WINDOW,  &SubBGControl_WINDOW,  GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_FONT,    &SubBGControl_FONT,    GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_BAR,    &MainBGControl_BAR,    GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_WINDOW, &MainBGControl_WINDOW, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_FONT,   &MainBGControl_FONT,   GFL_BG_MODE_TEXT );

  // ���ݒ�
  GFL_BG_SetVisible( SUB_BG_BACK,    VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_RADAR,   VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_WINDOW,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_FONT,    VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_BACK,   VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_BAR,    VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_WINDOW, VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_FONT,   VISIBLE_ON );

  // ���u�����f�B���O
  G2S_SetBlendAlpha( SUB_BG_BLEND_TARGET_1, SUB_BG_BLEND_TARGET_2, 
                     SUB_BG_BLEND_WEIGHT_1, SUB_BG_BLEND_WEIGHT_2 );
  G2_SetBlendAlpha( MAIN_BG_BLEND_TARGET_1, MAIN_BG_BLEND_TARGET_2, 
                    MAIN_BG_BLEND_WEIGHT_1, MAIN_BG_BLEND_WEIGHT_2 );

  // �r�b�g�}�b�v�E�B���h�E �V�X�e��������
  GFL_BMPWIN_Init( work->heapID );

  // �E�B���h�E�L����
  G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1, TRUE );
  G2_SetWnd0Position( WND0_LEFT, WND0_TOP, WND0_RIGHT, WND0_BOTTOM );
  GX_SetVisibleWnd( GX_WNDMASK_W0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup BG\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpBG( RESEARCH_SELECT_WORK* work )
{
  // �E�B���h�E������
  GX_SetVisibleWnd( GX_WNDMASK_NONE );

  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( SUB_BG_FONT );
  GFL_BG_FreeBGControl( SUB_BG_WINDOW );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up BG\n" );
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
static void SetupSubBG_WINDOW( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup SUB-BG-WINDOW\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up SUB-BG-WINDOW\n" );
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
static void SetupSubBG_FONT( RESEARCH_SELECT_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup SUB-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up SUB-BG-FONT\n" );
}


//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupMainBG_BAR( RESEARCH_SELECT_WORK* work )
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
      datID = NARC_research_radar_graphic_bgd_title_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_BAR, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( MAIN_BG_BAR );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup MAIN-BG-BAR\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_BAR( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-BAR\n" );
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
static void SetupMainBG_WINDOW( RESEARCH_SELECT_WORK* work )
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
      datID = NARC_research_radar_graphic_bgd_searchbtn_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_WINDOW, data->rawData, 0, 0, 32, 32 );
      GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup MAIN-BG-WINDOW\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-WINDOW\n" );
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
static void SetupMainBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup MAIN-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-FONT\n" );
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
static void InitBGFonts( RESEARCH_SELECT_WORK* work )
{
  int idx;

  // �ʏ��BGFont
  for( idx=0; idx < BG_FONT_NUM; idx++ )
  {
    work->BGFont[ idx ] = NULL; 
  }
  // ��������BGFont
  for( idx=0; idx < TOPIC_ID_NUM; idx++ )
  {
    work->TopicsBGFont[ idx ] = NULL; 
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateBGFonts( RESEARCH_SELECT_WORK* work )
{
  int i;

  // �ʏ��BGFont
  for( i=0; i<BG_FONT_NUM; i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->BGFont[i] == NULL ); 

    // �����p�����[�^�I��
    param.BGFrame   = BGFontInitData[i].BGFrame;
    param.posX      = BGFontInitData[i].posX;
    param.posY      = BGFontInitData[i].posY;
    param.sizeX     = BGFontInitData[i].sizeX;
    param.sizeY     = BGFontInitData[i].sizeY;
    param.offsetX   = BGFontInitData[i].offsetX;
    param.offsetY   = BGFontInitData[i].offsetY;
    param.paletteNo = BGFontInitData[i].paletteNo;
    param.colorNo_L = BGFontInitData[i].colorNo_L;
    param.colorNo_S = BGFontInitData[i].colorNo_S;
    param.colorNo_B = BGFontInitData[i].colorNo_B;
    msgData         = work->message[ BGFontInitData[i].messageIdx ];
    strID           = BGFontInitData[i].stringIdx;

    // ����
    work->BGFont[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetString( work->BGFont[i], strID );
  } 

  // ��������BGFont
   for( i=0; i < TOPIC_ID_NUM; i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->TopicsBGFont[i] == NULL ); 

    // �����p�����[�^�I��
    param.BGFrame   = TopicsBGFontInitData[i].BGFrame;
    param.posX      = TopicsBGFontInitData[i].posX;
    param.posY      = TopicsBGFontInitData[i].posY;
    param.sizeX     = TopicsBGFontInitData[i].sizeX;
    param.sizeY     = TopicsBGFontInitData[i].sizeY;
    param.offsetX   = TopicsBGFontInitData[i].offsetX;
    param.offsetY   = TopicsBGFontInitData[i].offsetY;
    param.paletteNo = TopicsBGFontInitData[i].paletteNo;
    param.colorNo_L = TopicsBGFontInitData[i].colorNo_L;
    param.colorNo_S = TopicsBGFontInitData[i].colorNo_S;
    param.colorNo_B = TopicsBGFontInitData[i].colorNo_B;
    msgData         = work->message[ TopicsBGFontInitData[i].messageIdx ];
    strID           = TopicsBGFontInitData[i].stringIdx;

    // ����
    work->TopicsBGFont[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetString( work->TopicsBGFont[i], strID );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create BGFonts\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBGFonts( RESEARCH_SELECT_WORK* work )
{
  int i;
  
  // �ʏ��BGFont
  for( i=0; i < BG_FONT_NUM; i++ )
  {
    GF_ASSERT( work->BGFont[i] );
    BG_FONT_Delete( work->BGFont[i] );
    work->BGFont[i] = NULL;
  }
  // ��������BGFont
  for( i=0; i < TOPIC_ID_NUM; i++ )
  {
    GF_ASSERT( work->TopicsBGFont[i] );
    BG_FONT_Delete( work->TopicsBGFont[i] );
    work->TopicsBGFont[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete BGFonts\n" ); 
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
static void CreateClactSystem( RESEARCH_SELECT_WORK* work )
{
  // �V�X�e���쐬
  GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // VBlank ���荞�݊֐���o�^
  GFUser_SetVIntrFunc( VBlankFunc );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create clact system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_SELECT_WORK* work )
{
  // VBkank ���荞�݊֐�������
  GFUser_ResetVIntrFunc();

  // �V�X�e���j��
  GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete clact system\n" );
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
static void RegisterSubObjResources( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: register SUB-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RESEARCH_SELECT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release SUB-OBJ resources\n" );
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
static void RegisterMainObjResources( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: register MAIN-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RESEARCH_SELECT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release MAIN-OBJ resources\n" );
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
static void InitClactUnits( RESEARCH_SELECT_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void CreateClactUnits( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactUnits( RESEARCH_SELECT_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete clact units\n" );
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
static void InitClactWorks( RESEARCH_SELECT_WORK* work )
{
  int wkIdx;

  // ������
  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    work->clactWork[ wkIdx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateClactWorks( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactWorks( RESEARCH_SELECT_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete clact works\n" );
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
static u32 GetObjResourceRegisterIndex( const RESEARCH_SELECT_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RESEARCH_SELECT_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RESEARCH_SELECT_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}


//----------------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̃J�[�\���ˑ�������\�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateSubDisplayStrings( RESEARCH_SELECT_WORK* work )
{
  int nowPos;

  nowPos = work->topicCursorPos;

  // �������� �薼/�⑫
  BG_FONT_SetString( work->BGFont[ BG_FONT_TOPIC_TITLE ],   StringID_topicTitle[ nowPos ] );
  BG_FONT_SetString( work->BGFont[ BG_FONT_TOPIC_CAPTION ], StringID_topicCaption[ nowPos ] );

  // ����
  BG_FONT_SetString( work->BGFont[ BG_FONT_QUESTION_1 ], StringID_question[ Question1_topic[ nowPos ] ] );
  BG_FONT_SetString( work->BGFont[ BG_FONT_QUESTION_2 ], StringID_question[ Question2_topic[ nowPos ] ] );
  BG_FONT_SetString( work->BGFont[ BG_FONT_QUESTION_3 ], StringID_question[ Question3_topic[ nowPos ] ] );
}


//==============================================================================================
// ��OBJ �\��
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[�̂܂ݕ����̈ʒu���X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateScrollControlPos( const RESEARCH_SELECT_WORK* work )
{
  GFL_CLWK* clactWork;
  GFL_CLACTPOS pos;
  u16 setSurface;
  int frame;
  int start, end;

  // �Z���A�N�^�[���[�N���擾
  clactWork  = GetClactWork( work, CLWK_SCROLL_CONTROL ); 
  setSurface = ClactWorkInitData[ CLWK_SCROLL_CONTROL ].setSurface;

  // �X�N���[���J�n�ʒu�ƏI���ʒu���Z�o
  frame = work->topicScrollFrameCount;
  start = SCROLL_CONTROL_TOP 
        + (SCROLL_CONTROL_BOTTOM - SCROLL_CONTROL_TOP) * work->topicCursorPos / (SCROLL_CONTROL_STEP_NUM - 1); 
  end   = SCROLL_CONTROL_TOP 
        + (SCROLL_CONTROL_BOTTOM - SCROLL_CONTROL_TOP) * work->topicCursorNextPos / (SCROLL_CONTROL_STEP_NUM - 1);

  // �\���ʒu���Z�o
  pos.x = SCROLL_CONTROL_LEFT;
  pos.y = start + (end - start) * frame / SCROLL_FRAME;

  // �\���ʒu��ύX
  GFL_CLACT_WK_SetPos( clactWork, &pos, setSurface );
  GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڑI���A�C�R���̕\����Ԃ��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateTopicSelectIcons( const RESEARCH_SELECT_WORK* work )
{
  int idx;
  int selectedTopicNum = work->selectedTopicNum;
  CLWK_INDEX iconClactWorkIdx[ SELECT_TOPIC_MAX_NUM ] = 
  {
    CLWK_SELECT_ICON_0,
    CLWK_SELECT_ICON_1,
    CLWK_SELECT_ICON_2,
  };

  // �S�A�C�R��������
  for( idx=0; idx < SELECT_TOPIC_MAX_NUM; idx++ )
  {
    GFL_CLWK* clactWork;
    CLWK_INDEX wkIdx;

    wkIdx     = iconClactWorkIdx[ idx ];
    clactWork = GetClactWork( work, wkIdx );
    GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );
  }

  // �I�����ڂ̐������\��
  for( idx=0; idx < selectedTopicNum; idx++ )
  { 
    GFL_CLACTPOS pos;
    GFL_CLWK* clactWork;
    CLWK_INDEX wkIdx;
    u16 setSurface;

    wkIdx      = iconClactWorkIdx[ idx ];
    clactWork  = GetClactWork( work, wkIdx );
    pos.x      = CalcTopicDisplayPosLeft( work, work->selectedTopicIDs[ idx ] ) + SELECT_ICON_DRAW_OFFSET_X;
    pos.y      = CalcTopicDisplayPosTop( work, work->selectedTopicIDs[ idx ] ) + SELECT_ICON_DRAW_OFFSET_Y;
    setSurface = ClactWorkInitData[ wkIdx ].setSurface;
    GFL_CLACT_WK_SetPos( clactWork, &pos, setSurface );
    GFL_CLACT_WK_SetAutoAnmFlag( clactWork, TRUE );
    GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );
  }
}


//==============================================================================================
// ��DEBUG:
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �I���ς݂̒�������ID��\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DebugPrint_SelectedTopicIDs( const RESEARCH_SELECT_WORK* work )
{
  int i;

  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: selected topic IDs =" );
  for( i=0; i < work->selectedTopicNum; i++ )
  {
    OS_TFPrintf( PRINT_TARGET, " %d", work->selectedTopicIDs[i] );
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}
