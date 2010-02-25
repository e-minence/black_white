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
#include "queue.h"
#include "research_select.h"
#include "research_select_index.h"
#include "research_select_def.h"
#include "research_select_data.cdat"
#include "research_common.h"

#include "print/gf_font.h"           // for GFL_FONT_xxxx
#include "print/printsys.h"          // for PRINTSYS_xxxx
#include "system/gfl_use.h"          // for GFUser_xxxx
#include "system/palanm.h"           // for PaletteFadeXxxx
#include "system/bmp_oam.h"          // for BmpOam_xxxx
#include "gamesystem/game_beacon.h"  // for GAMEBEACON_xxxx
#include "sound/pm_sndsys.h"         // for PMSND_xxxx
#include "sound/wb_sound_data.sadl"  // for SEQ_SE_XXXX

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

  QUEUE*                 seqQueue;      // �V�[�P���X�L���[
  RESEARCH_SELECT_SEQ    seq;           // �����V�[�P���X
  u32                    seqCount;      // �V�[�P���X�J�E���^
  BOOL                   seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  RESEARCH_SELECT_RESULT result;        // ��ʏI������

  // VBlank
  GFL_TCBSYS* VBlankTCBSystem; // VBlank���Ԓ��̃^�X�N�Ǘ��V�X�e��
  GFL_TCB*    VBlankTask;      // VBlank�^�C�~���O���ɍs���^�X�N

  // ���j���[����
  MENU_ITEM menuCursorPos;  // �J�[�\���ʒu

  // ��������
  u8 selectedTopicIDs[ SELECT_TOPIC_MAX_NUM ];  // �I��������������ID
  u8 selectedTopicNum;                          // �I�������������ڂ̐�

  u8  topicCursorPos;         // �J�[�\���ʒu
  u8  topicCursorNextPos;     // �ړ���̃J�[�\���ʒu 
  int topicScrollFrameCount;  // �X�N���[�� �t���[���J�E���^

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL menuTouchHitTable[ MENU_TOUCH_AREA_NUM ];
  GFL_UI_TP_HITTBL topicTouchHitTable[ TOPIC_TOUCH_AREA_NUM ];

  // ���b�Z�[�W
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont[ BG_FONT_NUM ];
  BG_FONT* TopicsBGFont[ TOPIC_ID_NUM ];  // ��������

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ];  // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];                // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                  // �Z���A�N�^�[���[�N

  // BMP-OAM
  BMPOAM_SYS_PTR BmpOamSystem;                     // BMP-OAM �V�X�e��
  BMPOAM_ACT_PTR BmpOamActor[ BMPOAM_ACTOR_NUM ];  // BMP-OAM�A�N�^�[
  GFL_BMP_DATA*  BmpData[ BMPOAM_ACTOR_NUM ];      // �e�A�N�^�[�ɑΉ�����r�b�g�}�b�v�f�[�^

  // �J���[�p���b�g
  PALETTE_FADE_PTR paletteFadeSystem; // �p���b�g�t�F�[�h�����V�X�e��
};


//----------------------------------------------------------------------------------------------
// ��LAYER 4 �V�[�P���X����
//----------------------------------------------------------------------------------------------
// �V�[�P���X����
static void Main_SETUP      ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void Main_KEY_WAIT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void Main_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void Main_TO_CONFIRM ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_CONFIRM
static void Main_CONFIRM    ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void Main_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_KEY_WAIT
static void Main_DETERMINE  ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void Main_FADE_OUT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void Main_CLEAN_UP   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP

// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq ); // ���̃V�[�P���X��o�^����
static void FinishCurrentSequence( RESEARCH_SELECT_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSequence( RESEARCH_SELECT_WORK* work ); // �����V�[�P���X��ύX����
static void SetSequence   ( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_RESULT result ); // ��ʏI�����ʂ�ݒ肷��

// �V�[�P���X����������
static void InitSequence_SETUP      ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void InitSequence_KEY_WAIT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void InitSequence_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void InitSequence_TO_CONFIRM ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_CONFIRM
static void InitSequence_CONFIRM    ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void InitSequence_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_KEY_WAIT
static void InitSequence_DETERMINE  ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void InitSequence_FADE_OUT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void InitSequence_CLEAN_UP   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP

// �V�[�P���X�I������
static void FinishSequence_SETUP      ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void FinishSequence_KEY_WAIT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void FinishSequence_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void FinishSequence_TO_CONFIRM ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_CONFIRM
static void FinishSequence_CONFIRM    ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void FinishSequence_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_TO_KEY_WAIT
static void FinishSequence_DETERMINE  ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void FinishSequence_FADE_OUT   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void FinishSequence_CLEAN_UP   ( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP

// VBlank�^�X�N
static void VBlankFunc( GFL_TCB* tcb, void* wk );  // VBlank���̏���

//----------------------------------------------------------------------------------------------
// ��LAYER 3 �@�\
//---------------------------------------------------------------------------------------------- 
// ���j���[���ڃJ�[�\��
static void MoveMenuCursorUp  ( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����

// �������ڃJ�[�\��
static void MoveTopicCursorUp    ( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveTopicCursorDown  ( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����
static void MoveTopicCursorDirect( RESEARCH_SELECT_WORK* work, u8 topicID ); // ���ڈړ�����
static BOOL SelectTopic( RESEARCH_SELECT_WORK* work ); // �������ڂ�I������

//----------------------------------------------------------------------------------------------
// ��LAYER 2 �ʑ���
//---------------------------------------------------------------------------------------------- 
// ���j���[���ڃJ�[�\��
static void ShiftMenuCursorPos( RESEARCH_SELECT_WORK* work, int stride ); // ���j���[���ڃJ�[�\���ʒu��ύX����

// �������ڃJ�[�\��
static void SetTopicCursorNextPos( RESEARCH_SELECT_WORK* work, int stride ); // �������ڃJ�[�\���̈ړ����ݒ肷�� ( �I�t�Z�b�g�ړ� )
static void SetTopicCursorPosDirect( RESEARCH_SELECT_WORK* work, int topciID ); // �������ڃJ�[�\���̈ړ����ݒ肷�� ( �_�C���N�g�ړ� )
static void TopicCursorScrollStart( RESEARCH_SELECT_WORK* work );  // �������ڃJ�[�\���̃X�N���[�����J�n����

// �^�b�`�͈�
static void UpdateTopicTouchArea( RESEARCH_SELECT_WORK* work ); // �^�b�`�͈͂��X�V����

// �I��������������ID
static void RegisterTopicID( RESEARCH_SELECT_WORK* work ); // ��������ID��o�^����
static void ReleaseTopicID ( RESEARCH_SELECT_WORK* work ); // ��������ID����������
static BOOL IsTopicIDRegistered( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �I���ς݂��𔻒肷��

// ���j���[���ڂ̕\��
static void SetMenuCursorOn( RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuCursorOff( RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���

// �������ڂ̕\��
static void SetTopicCursorOn ( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetTopicCursorOff( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetTopicSelected   ( const RESEARCH_SELECT_WORK* work ); // �I������Ă����Ԃɂ���
static void SetTopicNotSelected( const RESEARCH_SELECT_WORK* work ); // �I������Ă��Ȃ���Ԃɂ���
static void UpdateSubDisplayStrings( RESEARCH_SELECT_WORK* work ); // ���ʂ̃J�[�\���ˑ�������\�����X�V����
static void UpdateTopicScroll( RESEARCH_SELECT_WORK* work );       // �������ڂ̃X�N���[����Ԃ��X�V����
static void FinishTopicScroll( RESEARCH_SELECT_WORK* work );       // �������ڂ̃X�N���[����Ԃ�����������
static BOOL IsTopicScrollEnd ( const RESEARCH_SELECT_WORK* work ); // �������ڂ̃X�N���[���������������ǂ����𔻒肷��
static u8  CalcTopicScreenPosLeft ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� (�X�N���[���P��)
static u8  CalcTopicScreenPosTop  ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� (�X�N���[���P��)
static int CalcTopicDisplayPosLeft( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� (�h�b�g�P��)
static int CalcTopicDisplayPosTop ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� (�h�b�g�P��)
static int CalcScreenScrollY      ( const RESEARCH_SELECT_WORK* work );             // y�������̃X�N���[���ʂ��Z�o����

// OBJ�̕\��
static void UpdateScrollControlPos( const RESEARCH_SELECT_WORK* work );  // �X�N���[���o�[�̂܂ݕ����̈ʒu���X�V����
static void UpdateTopicSelectIcons( const RESEARCH_SELECT_WORK* work );  // �������ڑI���A�C�R���̕\����Ԃ��X�V����

// �p���b�g�t�F�[�h
static void StartPaletteFadeOut( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�A�E�g���J�n����
static void StartPaletteFadeIn ( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�C�����J�n����
static BOOL IsPaletteFadeEnd   ( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��

// BMP-OAM
static void BmpOamSetDrawEnable( RESEARCH_SELECT_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable );  // �\�����邩�ǂ�����ݒ肷��

//----------------------------------------------------------------------------------------------
// ��LAYER 1 �f�[�^�A�N�Z�X
//----------------------------------------------------------------------------------------------
// OBJ
static u32 GetObjResourceRegisterIndex( const RESEARCH_SELECT_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetClactUnit( const RESEARCH_SELECT_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g
static GFL_CLWK* GetClactWork( const RESEARCH_SELECT_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N

// BMP-OAM
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[

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

// BMP-OAM �̏���/��Еt��
static void InitBitmapDatas  ( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^ ������
static void CreateBitmapDatas( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^ �쐬
static void SetupBitmapDatas ( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^ ����
static void DeleteBitmapDatas( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^ �j��
static void SetupBmpOamSystem  ( RESEARCH_SELECT_WORK* work ); // BMP-OAM �V�X�e�� ����
static void CleanUpBmpOamSystem( RESEARCH_SELECT_WORK* work ); // BMP-OAM �V�X�e�� ��Еt��
static void CreateBmpOamActors( RESEARCH_SELECT_WORK* work ); // BMP-OAM �A�N�^�[ �쐬
static void DeleteBmpOamActors( RESEARCH_SELECT_WORK* work ); // BMP-OAM �A�N�^�[ �j��

// VBlank�^�X�N�̓o�^/����
static void RegisterVBlankTask( RESEARCH_SELECT_WORK* work );
static void ReleaseVBlankTask ( RESEARCH_SELECT_WORK* work );

// �p���b�g�t�F�[�h�V�X�e���̏���/��Еt��
static void InitPaletteFadeSystem   ( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ������
static void SetupPaletteFadeSystem  ( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ����
static void CleanUpPaletteFadeSystem( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��

// �f�[�^�̏�����/����/�j��
static void InitSeqQueue  ( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[ ������
static void CreateSeqQueue( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[ �쐬
static void DeleteSeqQueue( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[ �j��
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
static void SetupTouchArea( RESEARCH_SELECT_WORK* work ); // �^�b�`�̈� ����

// DEBUG:
static void DebugPrint_seqQueue( const RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[�̒��g��\������
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
  work->seqCount              = 0;
  work->seqFinishFlag         = FALSE;
  work->result                = RESEARCH_SELECT_RESULT_NONE;
  work->heapID                = heapID;
  work->menuCursorPos         = MENU_ITEM_DETERMINATION_OK;
  work->topicCursorPos        = 0;
  work->topicCursorNextPos    = 0;
  work->topicScrollFrameCount = 0;
  work->VBlankTCBSystem       = GFUser_VIntr_GetTCBSYS();

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitSeqQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitSelectedTopicIDs( work );
  InitPaletteFadeSystem( work );
  InitBitmapDatas( work );

  CreateSeqQueue( work );

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
  DeleteSeqQueue( work ); // �V�[�P���X�L���[
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
  // �V�[�P���X���Ƃ̏���
  switch( work->seq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        Main_SETUP( work );        break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     Main_KEY_WAIT( work );     break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  Main_SCROLL_WAIT( work );  break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   Main_TO_CONFIRM( work );   break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      Main_CONFIRM( work );      break;
  case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  Main_TO_KEY_WAIT( work );  break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    Main_DETERMINE( work );    break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:     Main_FADE_OUT( work );     break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     Main_CLEAN_UP( work );     break;
  case RESEARCH_SELECT_SEQ_FINISH:       return work->result;
  default:  GF_ASSERT(0);
  }

  // �Z���A�N�^�[�V�X�e�� ���C������
  GFL_CLACT_SYS_Main();

  // �V�[�P���X�J�E���^�X�V
  CountUpSeqCount( work );

  // �V�[�P���X�X�V
  SwitchSequence( work );

  return RESEARCH_SELECT_RESULT_CONTINUE;
}





//==============================================================================================
// ��LAYER 4 �V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_SELECT_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_SETUP( RESEARCH_SELECT_WORK* work )
{
  CreateFont( work );
  CreateMessages( work );
  SetupTouchArea( work );
  UpdateTopicTouchArea( work );

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

  // BMP-OAM ����
  CreateBitmapDatas( work );
  SetupBitmapDatas( work );
  SetupBmpOamSystem( work );
  CreateBmpOamActors( work );

  // �p���b�g�t�F�[�h�V�X�e�� ����
  SetupPaletteFadeSystem( work );

  // VBkank�^�X�N�o�^
  RegisterVBlankTask( work );

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  // ���̃V�[�P���X���Z�b�g
  SetNextSequence( work, RESEARCH_SELECT_SEQ_KEY_WAIT ); 

  // �V�[�P���X�I��
  FinishCurrentSequence( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_SELECT_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{ 
  int trg;
  int touchedAreaIdx;
  BOOL select = FALSE;

  trg            = GFL_UI_KEY_GetTrg();
  touchedAreaIdx = GFL_UI_TP_HitTrg( work->topicTouchHitTable );

  if( trg & PAD_KEY_UP ) {
    MoveTopicCursorUp( work );
    SetNextSequence( work, RESEARCH_SELECT_SEQ_SCROLL_WAIT );
    FinishCurrentSequence( work );
    // --> �X�N���[���҂��V�[�P���X��
  }

  if( trg & PAD_KEY_DOWN ) {
    MoveTopicCursorDown( work );
    SetNextSequence( work, RESEARCH_SELECT_SEQ_SCROLL_WAIT );
    FinishCurrentSequence( work );
    // --> �X�N���[���҂��V�[�P���X��
  } 

  if( trg & PAD_BUTTON_B ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );      // �L�����Z����
    SetNextSequence( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    // --> ��Еt���V�[�P���X��
  }

  if( trg & PAD_BUTTON_A ) {
    select = SelectTopic( work );    // �������ڂ�I��
  } 

  if( (TOPIC_TOUCH_AREA_TOPIC_0 <= touchedAreaIdx) && (touchedAreaIdx <= TOPIC_TOUCH_AREA_TOPIC_9) ) {
    MoveTopicCursorDirect( work, touchedAreaIdx ); // �J�[�\���ړ�
    select = SelectTopic( work );                  // �������ڂ�I��
  } 

  // �ő吔�̒������ڂ�I��
  if( (select == TRUE) && (work->selectedTopicNum == SELECT_TOPIC_MAX_NUM) ) {
    SetNextSequence( work, RESEARCH_SELECT_SEQ_TO_CONFIRM );
    FinishCurrentSequence( work );
    // --> �������ڊm�F�V�[�P���X��
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������҂��V�[�P���X ( RESEARCH_SELECT_SEQ_SCROLL_WAIT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  // �X�N���[������
  UpdateTopicScroll( work );       // �������ڃ{�^��
  UpdateTopicTouchArea( work );    // �^�b�`�͈͂��X�V����
  UpdateScrollControlPos( work );  // �X�N���[���o�[�̂܂ݕ���
  UpdateTopicSelectIcons( work );  // �������ڑI���A�C�R��
  work->topicScrollFrameCount++;

  // �X�N���[���I��
  if( IsTopicScrollEnd(work) ) {
    FinishTopicScroll( work );
    SetNextSequence( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSequence( work );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_SELECT_SEQ_TO_CONFIRM ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_TO_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�t�F�[�h������
  if( IsPaletteFadeEnd( work ) ) {
    SetNextSequence( work, RESEARCH_SELECT_SEQ_CONFIRM );
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X ( RESEARCH_SELECT_SEQ_CONFIRM ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  int trg;
  int touchedAreaIdx;

  trg     = GFL_UI_KEY_GetTrg();
  touchedAreaIdx = GFL_UI_TP_HitTrg( work->menuTouchHitTable );

  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work );
  }
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work );
  } 

  if( (trg & PAD_BUTTON_A) || (touchedAreaIdx == MENU_TOUCH_AREA_OK_BUTTON) ) {
    SetNextSequence( work, MenuItemNextSequence[ work->menuCursorPos ] );
    FinishCurrentSequence( work );
  } 

  if( (trg & PAD_BUTTON_B) || (touchedAreaIdx == MENU_TOUCH_AREA_CANCEL_BUTTON) ) {
    SetNextSequence( work, RESEARCH_SELECT_SEQ_TO_KEY_WAIT );
    FinishCurrentSequence( work );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��֖߂�V�[�P���X ( RESEARCH_SELECT_SEQ_TO_KEY_WAIT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�t�F�[�h������
  if( IsPaletteFadeEnd( work ) ) {
    SetNextSequence( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��V�[�P���X ( RESEARCH_SELECT_SEQ_DETERMINE ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  // ��莞�Ԃ��o��
  if( SEQ_DETERMINE_WAIT_FRAMES <= work->seqCount ) {
    SetNextSequence( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��V�[�P���X ( RESEARCH_SELECT_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_SELECT_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{ 
  // VBlank�^�X�N������
  ReleaseVBlankTask( work );

  // �p���b�g�t�F�[�h�V�X�e�� ��Еt��
  CleanUpPaletteFadeSystem( work );

  // BMP-OAM ��Еt��
  DeleteBitmapDatas( work );
  DeleteBmpOamActors( work );
  CleanUpBmpOamSystem( work );

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

  // ��ʏI�����ʂ�����
  SetResult( work, RESEARCH_SELECT_RESULT_TO_MENU );  
  SetNextSequence( work, RESEARCH_SELECT_SEQ_FINISH );
  FinishCurrentSequence( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_SELECT_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    maxCount = SEQ_DETERMINE_WAIT_FRAMES; break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:     maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     maxCount = 0xffffffff; break;
  case RESEARCH_SELECT_SEQ_FINISH:       maxCount = 0xffffffff; break;
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
static void SetNextSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishCurrentSequence( RESEARCH_SELECT_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish current sequence\n" );
}

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

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 * @param nextSeq �ύX��̃V�[�P���X
 */
//----------------------------------------------------------------------------------------------
static void SwitchSequence( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

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
static void SetSequence( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        FinishSequence_SETUP( work );       break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     FinishSequence_KEY_WAIT( work );    break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  FinishSequence_SCROLL_WAIT( work ); break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   FinishSequence_TO_CONFIRM( work );  break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      FinishSequence_CONFIRM( work );     break;
  case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  FinishSequence_TO_KEY_WAIT( work ); break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    FinishSequence_DETERMINE( work );   break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:     FinishSequence_FADE_OUT( work );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     FinishSequence_CLEAN_UP( work );    break;
  case RESEARCH_SELECT_SEQ_FINISH:                                           break;
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq           = nextSeq;
  work->seqCount      = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        InitSequence_SETUP( work );       break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     InitSequence_KEY_WAIT( work );    break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  InitSequence_SCROLL_WAIT( work ); break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   InitSequence_TO_CONFIRM( work );  break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      InitSequence_CONFIRM( work );     break;
  case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  InitSequence_TO_KEY_WAIT( work ); break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    InitSequence_DETERMINE( work );   break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:     InitSequence_FADE_OUT( work );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     InitSequence_CLEAN_UP( work );    break;
  case RESEARCH_SELECT_SEQ_FINISH:                                         break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set seq ==> " );
  switch( nextSeq )
  {
  case RESEARCH_SELECT_SEQ_SETUP:        OS_TFPrintf( PRINT_TARGET, "SETUP\n" );       break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:     OS_TFPrintf( PRINT_TARGET, "KEY_WAIT\n" );    break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  OS_TFPrintf( PRINT_TARGET, "SCROLL_WAIT\n" ); break;
  case RESEARCH_SELECT_SEQ_TO_CONFIRM:   OS_TFPrintf( PRINT_TARGET, "TO_CONFIRM\n" );  break;
  case RESEARCH_SELECT_SEQ_CONFIRM:      OS_TFPrintf( PRINT_TARGET, "CONFIRM\n" );     break;
  case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  OS_TFPrintf( PRINT_TARGET, "TO_KEY_WAIT\n" ); break;
  case RESEARCH_SELECT_SEQ_DETERMINE:    OS_TFPrintf( PRINT_TARGET, "DETERMINE\n" );   break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:     OS_TFPrintf( PRINT_TARGET, "FADE_OUT\n" );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:     OS_TFPrintf( PRINT_TARGET, "CLEAN_UP\n" );    break;
  case RESEARCH_SELECT_SEQ_FINISH:       OS_TFPrintf( PRINT_TARGET, "FINISH\n" );      break;
  default:  GF_ASSERT(0);
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SETUP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_SETUP( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SCROLL_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_TO_CONFIRM )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_TO_CONFIRM ( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�t�F�[�h�A�E�g�J�n
  StartPaletteFadeOut( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq TO_CONFIRM\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_CONFIRM )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  // �J�[�\���ʒu��������
  work->menuCursorPos = MENU_ITEM_DETERMINATION_OK;

  // �m�F���b�Z�[�W�ƑI�����ڂ�\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, TRUE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, TRUE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, TRUE );


  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq CONFIRM\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_TO_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{ 
  // �m�F���b�Z�[�W�ƑI�����ڂ�����
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, FALSE );

  // �p���b�g�t�F�[�h�C���J�n
  StartPaletteFadeIn( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq TO_KEY_WAIT" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_DETERMINE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  // �m�F���b�Z�[�W�ƑI�����ڂ�����
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, FALSE );

  //�u���傤�����@���������܂��I�v��\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_DETERMINE, TRUE ); 

  // �����J�nSE
  PMSND_PlaySE( SEQ_SE_SYS_80 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq DETERMINE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_FADE_OUT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq FADE OUT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_CLEAN_UP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SETUP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_SETUP( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SCROLL_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_TO_CONFIRM )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_TO_CONFIRM ( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�t�F�[�h�A�E�g�J�n
  StartPaletteFadeOut( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq TO_CONFIRM\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_CONFIRM )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq CONFIRM\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_TO_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_TO_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq TO_KEY_WAIT" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_DETERMINE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq DETERMINE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_FADE_OUT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq FADE_OUT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_CLEAN_UP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank ���荞�ݏ���
 *
 * @param tcb
 * @parma wk
 */
//----------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB* tcb, void* wk )
{
  RESEARCH_SELECT_WORK* work = (RESEARCH_SELECT_WORK*)wk;

  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( work->paletteFadeSystem );
}



//==============================================================================================
// ��LAYER 3 �@�\
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
  SetMenuCursorOff( work );        // �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, -1 );  // �J�[�\���ړ�
  SetMenuCursorOn( work );         // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );  // �J�[�\���ړ���
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
  SetMenuCursorOff( work );       // �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, 1 );  // �J�[�\���ړ�
  SetMenuCursorOn( work );        // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 ); // �J�[�\���ړ���
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
  SetTopicCursorOff( work );         // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, -1 ); // �ړ����ݒ�
  TopicCursorScrollStart( work );    // �X�N���[���J�n
  PMSND_PlaySE( SEQ_SE_SELECT1 );    // �J�[�\���ړ���
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
  SetTopicCursorOff( work );         // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, 1 );  // �ړ����ݒ�
  TopicCursorScrollStart( work );    // �X�N���[���J�n
  PMSND_PlaySE( SEQ_SE_SELECT1 );    // �J�[�\���ړ���
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param topicID �ړ���̒�������ID
 */
//----------------------------------------------------------------------------------------------
static void MoveTopicCursorDirect( RESEARCH_SELECT_WORK* work, u8 topicID )
{
  SetTopicCursorOff( work );                // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorPosDirect( work, topicID ); // �J�[�\���ʒu���X�V
  SetTopicCursorOn( work );                 // �ړ���̍��ڂ�I����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );           // �J�[�\���ړ���
}




//==============================================================================================
// ��LAYER 2 �ʑ���
//==============================================================================================

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
 * @brief �������ڃJ�[�\���̈ʒu��ݒ肷��
 *
 * @param work
 * @param topicID �ݒ肷�钲������ID
 */
//----------------------------------------------------------------------------------------------
static void SetTopicCursorPosDirect( RESEARCH_SELECT_WORK* work, int topicID )
{
  // �J�[�\���ʒu���X�V
  work->topicCursorPos = topicID;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set topic cursor pos direct ==> %d\n", topicID );
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
 * @brief �������ڂ̃^�b�`�͈͂��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateTopicTouchArea( RESEARCH_SELECT_WORK* work )
{
  int idx;
  
  for( idx=TOPIC_TOUCH_AREA_TOPIC_0; idx <= TOPIC_TOUCH_AREA_TOPIC_9; idx++ )
  {
    int left, top;
    int width, height;

    left   = CalcTopicDisplayPosLeft( work, idx );
    top    = CalcTopicDisplayPosTop( work, idx );
    width  = TOPIC_BUTTON_WIDTH * DOT_PER_CHARA;
    height = TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA;

    work->topicTouchHitTable[ idx ].rect.left   = left;
    work->topicTouchHitTable[ idx ].rect.right  = left + width;
    work->topicTouchHitTable[ idx ].rect.top    = top; 
    work->topicTouchHitTable[ idx ].rect.bottom = top + height;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update touch area\n" );
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
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetMenuCursorOn( RESEARCH_SELECT_WORK* work )
{
  BMPOAM_ACT_PTR BmpOamActor;

  // �J�[�\���ʒu�̃��j���[���ڂɑΉ�����BMP-OAM �A�N�^�[���擾
  BmpOamActor = GetBmpOamActorOfMenuItem( work, work->menuCursorPos );

  // BMP-OAM �A�N�^�[�̃p���b�g�I�t�Z�b�g��ύX
  BmpOam_ActorSetPaletteOffset( BmpOamActor, MAIN_OBJ_PALETTE_MENU_ITEM_ON );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, ���j���[���ڂ��J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetMenuCursorOff( RESEARCH_SELECT_WORK* work )
{
  BMPOAM_ACT_PTR BmpOamActor;

  // �J�[�\���ʒu�̃��j���[���ڂɑΉ�����BMP-OAM �A�N�^�[���擾
  BmpOamActor = GetBmpOamActorOfMenuItem( work, work->menuCursorPos );

  // BMP-OAM �A�N�^�[�̃p���b�g�I�t�Z�b�g��ύX
  BmpOam_ActorSetPaletteOffset( BmpOamActor, MAIN_OBJ_PALETTE_MENU_ITEM_OFF );
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
  BG_FONT_SetMessage( work->BGFont[ BG_FONT_TOPIC_TITLE ],   StringID_topicTitle[ nowPos ] );
  BG_FONT_SetMessage( work->BGFont[ BG_FONT_TOPIC_CAPTION ], StringID_topicCaption[ nowPos ] );

  // ����
  BG_FONT_SetMessage( work->BGFont[ BG_FONT_QUESTION_1 ], StringID_question[ Question1_topic[ nowPos ] ] );
  BG_FONT_SetMessage( work->BGFont[ BG_FONT_QUESTION_2 ], StringID_question[ Question2_topic[ nowPos ] ] );
  BG_FONT_SetMessage( work->BGFont[ BG_FONT_QUESTION_3 ], StringID_question[ Question3_topic[ nowPos ] ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update sub display strings \n" );
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
 * @brief �������ڃX�N���[���������̏������s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishTopicScroll( RESEARCH_SELECT_WORK* work )
{
  work->topicCursorPos = work->topicCursorNextPos; // �J�[�\���ʒu���X�V
  UpdateSubDisplayStrings( work );                 // ���ʂ̃J�[�\���ˑ���������X�V
  SetTopicCursorOn( work );                        // �J�[�\��������Ă����Ԃɂ���

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish topic scroll\n" );
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
// ����������
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ�I������B
 *        ���łɑI���ς݂Ȃ�o�^����������B
 *        �����łȂ����, �o�^����B
 *
 * @param work
 *
 * @return �V�������ڂ�I�������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------------------------
static BOOL SelectTopic( RESEARCH_SELECT_WORK* work )
{
  BOOL select = FALSE;  // �V�������ڂ�I���������ǂ���


  if( IsTopicIDRegistered( work, work->topicCursorPos ) ) // �o�^�ς�
  {
    SetTopicNotSelected( work );    // �I�����Ă��Ȃ���Ԃɖ߂�
    ReleaseTopicID( work );         // �o�^���Ă�������ID����������
    PMSND_PlaySE( SEQ_SE_CANCEL1 ); // �L�����Z����
  }
  else if( work->selectedTopicNum == SELECT_TOPIC_MAX_NUM ) // ���łɍő吔��I���ς�
  {
  }
  else // �V�������ڂ�I��
  {
    SetTopicSelected( work );       // �I�����Ă����Ԃɂ���
    RegisterTopicID( work );        // ����ID��o�^����
    PMSND_PlaySE( SEQ_SE_DECIDE1 ); // ���艹
    select = TRUE;
  }

  // �������ڑI���A�C�R�����X�V
  UpdateTopicSelectIcons( work );

  return select;
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
 * @breif �^�b�`�̈�̏������s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupTouchArea( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < MENU_TOUCH_AREA_NUM; idx++ )
  {
    work->menuTouchHitTable[ idx ].rect.left   = MenuTouchAreaInitData[ idx ].left;
    work->menuTouchHitTable[ idx ].rect.right  = MenuTouchAreaInitData[ idx ].right;
    work->menuTouchHitTable[ idx ].rect.top    = MenuTouchAreaInitData[ idx ].top;
    work->menuTouchHitTable[ idx ].rect.bottom = MenuTouchAreaInitData[ idx ].bottom;
  }
  for( idx=0; idx < TOPIC_TOUCH_AREA_NUM; idx++ )
  {
    work->topicTouchHitTable[ idx ].rect.left   = TopicTouchAreaInitData[ idx ].left;
    work->topicTouchHitTable[ idx ].rect.right  = TopicTouchAreaInitData[ idx ].right;
    work->topicTouchHitTable[ idx ].rect.top    = TopicTouchAreaInitData[ idx ].top;
    work->topicTouchHitTable[ idx ].rect.bottom = TopicTouchAreaInitData[ idx ].bottom;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create touch hit table\n" );
}




//==============================================================================================
// ����ʏI������
//==============================================================================================


//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_SELECT_WORK* work )
{
  // ������
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL );

  // �쐬
  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->seqQueue );

  // �폜
  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete seq queue\n" );
}


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
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( MAIN_BG_BAR );
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

  // ��������BGFont
   for( i=0; i < TOPIC_ID_NUM; i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->TopicsBGFont[i] == NULL ); 

    // �����p�����[�^�I��
    param.BGFrame       = TopicsBGFontInitData[i].BGFrame;
    param.posX          = TopicsBGFontInitData[i].posX;
    param.posY          = TopicsBGFontInitData[i].posY;
    param.sizeX         = TopicsBGFontInitData[i].sizeX;
    param.sizeY         = TopicsBGFontInitData[i].sizeY;
    param.offsetX       = TopicsBGFontInitData[i].offsetX;
    param.offsetY       = TopicsBGFontInitData[i].offsetY;
    param.paletteNo     = TopicsBGFontInitData[i].paletteNo;
    param.colorNo_L     = TopicsBGFontInitData[i].colorNo_L;
    param.colorNo_S     = TopicsBGFontInitData[i].colorNo_S;
    param.colorNo_B     = TopicsBGFontInitData[i].colorNo_B;
    param.centeringFlag = TopicsBGFontInitData[i].softCentering; 
    msgData             = work->message[ TopicsBGFontInitData[i].messageIdx ];
    strID               = TopicsBGFontInitData[i].stringIdx;

    // ����
    work->TopicsBGFont[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetMessage( work->TopicsBGFont[i], strID );
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

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitBitmapDatas( RESEARCH_SELECT_WORK* work )
{
  int idx;

  // ������
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    work->BmpData[ idx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���쐬����
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CreateBitmapDatas( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    const BITMAP_INIT_DATA* data;

    // �������f�[�^���擾
    data = &BitmapInitData[ idx ]; 

    // �쐬
    work->BmpData[ idx ] = GFL_BMP_Create( data->width, data->height, data->colorMode, work->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^�̏������s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupBitmapDatas( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    GFL_BMP_DATA* bmp;
    const BITMAP_INIT_DATA* data;
    GFL_MSGDATA* msgData;
    STRBUF* strbuf;
    PRINTSYS_LSB color;
    
    // �������f�[�^�擾
    data = &BitmapInitData[ idx ]; 

    // �w�i�̃L�����N�^���R�s�[����
    bmp = GFL_BMP_LoadCharacter( data->charaDataArcID, data->charaDataArcDatID, FALSE, work->heapID );
    GFL_BMP_Print( bmp, work->BmpData[ idx ], 0, 0, 0, 0, 
                   data->width * DOT_PER_CHARA, data->height * DOT_PER_CHARA, data->colorNo_B );

    // ���������������
    msgData = work->message[ data->messageIdx ];
    color   = PRINTSYS_LSB_Make( data->colorNo_L, data->colorNo_S, data->colorNo_B );
    strbuf  = GFL_MSG_CreateString( msgData, data->stringID );

    PRINTSYS_PrintColor( work->BmpData[ idx ],
                         data->stringDrawPosX, data->stringDrawPosY,
                         strbuf, work->font, color ); 

    GFL_HEAP_FreeMemory( strbuf );
    GFL_BMP_Delete( bmp );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBitmapDatas( RESEARCH_SELECT_WORK* work )
{
  int idx;

  // �j��
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    GFL_BMP_Delete( work->BmpData[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̏������s��
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void SetupBmpOamSystem( RESEARCH_SELECT_WORK* work )
{
  // BMP-OAM �V�X�e�����쐬
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup BMP-OAM system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RESEARCH_SELECT_WORK* work )
{
  // BMP-OAM �V�X�e����j��
  BmpOam_Exit( work->BmpOamSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up BMP-OAM system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CreateBmpOamActors( RESEARCH_SELECT_WORK* work )
{
  int idx;
  BMPOAM_ACT_DATA head;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    // �����p�����[�^���쐬
    head.bmp        = work->BmpData[ idx ];
    head.x          = BmpOamActorInitData[ idx ].x;
    head.y          = BmpOamActorInitData[ idx ].y;
    head.pltt_index = GetObjResourceRegisterIndex( work, BmpOamActorInitData[ idx ].paletteResID );
    head.pal_offset = BmpOamActorInitData[ idx ].paletteOffset;
    head.soft_pri   = BmpOamActorInitData[ idx ].softPriority;
    head.bg_pri     = BmpOamActorInitData[ idx ].BGPriority;
    head.setSerface = BmpOamActorInitData[ idx ].setSurface;
    head.draw_type  = BmpOamActorInitData[ idx ].drawType;

    // BMP-OAM �A�N�^�[��ǉ�
    work->BmpOamActor[ idx ] = BmpOam_ActorAdd( work->BmpOamSystem, &head );

    // ��\���ɐݒ�
    BmpOam_ActorSetDrawEnable( work->BmpOamActor[ idx ], FALSE );

    BmpOam_ActorBmpTrans( work->BmpOamActor[ idx ] );

  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create BMP-OAM actors\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[��j������
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBmpOamActors( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    BmpOam_ActorDel( work->BmpOamActor[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete BMP-OAM actors\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N��o�^����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void RegisterVBlankTask( RESEARCH_SELECT_WORK* work )
{
  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: register VBlank task\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RESEARCH_SELECT_WORK* work )
{ 
  GFL_TCB_DeleteTask( work->VBlankTask );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release VBlank task\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RESEARCH_SELECT_WORK* work )
{ 
  // ������
  work->paletteFadeSystem = NULL;

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init palette fade system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RESEARCH_SELECT_WORK* work )
{
  u32 tcbWorkSize;

  // ���d����
  GF_ASSERT( work->paletteFadeSystem == NULL );

  // �t�F�[�h�����V�X�e���쐬
  work->paletteFadeSystem = PaletteFadeInit( work->heapID ); 

  // �p���b�g�t�F�[�h�̃��N�G�X�g���[�N�𐶐�
  PaletteFadeWorkAllocSet( work->paletteFadeSystem, FADE_MAIN_BG,  FULL_PALETTE_SIZE, work->heapID );
  PaletteFadeWorkAllocSet( work->paletteFadeSystem, FADE_MAIN_OBJ, FULL_PALETTE_SIZE, work->heapID );

  // ���N�G�X�g���[�N������
  PaletteWorkSet_VramCopy( work->paletteFadeSystem, FADE_MAIN_BG,  0, FULL_PALETTE_SIZE );
  PaletteWorkSet_VramCopy( work->paletteFadeSystem, FADE_MAIN_OBJ, 0, FULL_PALETTE_SIZE );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup palette fade system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���̌�Еt�����s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpPaletteFadeSystem( RESEARCH_SELECT_WORK* work )
{ 
  // �p���b�g�t�F�[�h�̃��N�G�X�g���[�N��j��
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_OBJ );

  // �t�F�[�h�Ǘ��V�X�e����j��
  PaletteFadeFree( work->paletteFadeSystem );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up palette fade system\n" );
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
 * @brief ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[
 * 
 * @param work
 * @param menuItem ���j���[���ڂ̃C���f�b�N�X
 *
 * @return �w�肵�����j���[���ڂɑΉ�����BMP-OAM �A�N�^�[
 */
//----------------------------------------------------------------------------------------------
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACTOR_INDEX BmpOamActorIdx;

  // BMP-OAM �A�N�^�[�̃C���f�b�N�X���擾
  BmpOamActorIdx = BmpOamIndexOfMenu[ menuItem ];

  // BMP-OAM �A�N�^�[��Ԃ�
  return work->BmpOamActor[ BmpOamActorIdx ];
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

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update topic select icons\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteFadeOut( RESEARCH_SELECT_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_OUT_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_OUT_WAIT,
                  MAIN_BG_PALETTE_FADE_OUT_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_OUT_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_OUT_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_OUT_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_OUT_WAIT,
                  MAIN_OBJ_PALETTE_FADE_OUT_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_OUT_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_OUT_COLOR,
                  work->VBlankTCBSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: start palette fade out\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�C�����J�n����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteFadeIn( RESEARCH_SELECT_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_IN_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_IN_WAIT,
                  MAIN_BG_PALETTE_FADE_IN_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_IN_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_IN_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_IN_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_IN_WAIT,
                  MAIN_OBJ_PALETTE_FADE_IN_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_IN_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_IN_COLOR,
                  work->VBlankTCBSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: start palette fade in\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �p���b�g�t�F�[�h���������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------------------------
static BOOL IsPaletteFadeEnd( RESEARCH_SELECT_WORK* work )
{
  return PaletteFadeCheck( work->paletteFadeSystem );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �̕\����Ԃ�ݒ肷��
 *
 * @param work
 * @param actorIdx �ݒ�Ώۂ�BMP-OAM���w��
 * @param enable   �\�����邩�ǂ���
 */
//----------------------------------------------------------------------------------------------
static void BmpOamSetDrawEnable( RESEARCH_SELECT_WORK* work, BMPOAM_ACTOR_INDEX actorIdx, BOOL enable )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( actorIdx < BMPOAM_ACTOR_NUM );

  // �\����Ԃ�ύX
  BmpOam_ActorSetDrawEnable( work->BmpOamActor[ actorIdx ], enable );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, 
              "RESEARCH-SELECT: set draw enable BMP-OAM [%d] ==> %d\n", actorIdx, enable );
}


//==============================================================================================
// ��DEBUG:
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_SELECT_WORK* work )
{
  int i;
  int dataNum;
  int value;

  // �L���[���̃f�[�^�̌����擾
  dataNum = QUEUE_GetDataNum( work->seqQueue );

  // �S�Ẵf�[�^���o��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: seq queue = " );
  for( i=0; i < dataNum; i++ )
  { 
    value = QUEUE_PeekData( work->seqQueue, i );
    
    switch( value )
    {
    case RESEARCH_SELECT_SEQ_SETUP:        OS_TFPrintf( PRINT_TARGET, "SETUP \n" );       break;
    case RESEARCH_SELECT_SEQ_KEY_WAIT:     OS_TFPrintf( PRINT_TARGET, "KEY_WAIT \n" );    break;
    case RESEARCH_SELECT_SEQ_SCROLL_WAIT:  OS_TFPrintf( PRINT_TARGET, "SCROLL_WAIT \n" ); break;
    case RESEARCH_SELECT_SEQ_TO_CONFIRM:   OS_TFPrintf( PRINT_TARGET, "TO_CONFIRM \n" );  break;
    case RESEARCH_SELECT_SEQ_CONFIRM:      OS_TFPrintf( PRINT_TARGET, "CONFIRM \n" );     break;
    case RESEARCH_SELECT_SEQ_TO_KEY_WAIT:  OS_TFPrintf( PRINT_TARGET, "TO_KEY_WAIT \n" ); break;
    case RESEARCH_SELECT_SEQ_DETERMINE:    OS_TFPrintf( PRINT_TARGET, "DETERMINE \n" );   break;
    case RESEARCH_SELECT_SEQ_FADE_OUT:     OS_TFPrintf( PRINT_TARGET, "FADE_OUT \n" );   break;
    case RESEARCH_SELECT_SEQ_CLEAN_UP:     OS_TFPrintf( PRINT_TARGET, "CLEAN_UP \n" );    break;
    case RESEARCH_SELECT_SEQ_FINISH:       OS_TFPrintf( PRINT_TARGET, "FINISH \n" );      break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 

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
