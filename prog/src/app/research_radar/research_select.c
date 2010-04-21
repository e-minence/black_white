///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX���
 * @file   research_select.c
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <gflib.h>
#include "bg_font.h"
#include "queue.h"
#include "palette_anime.h"
#include "research_select.h"
#include "research_select_index.h"
#include "research_select_def.h"
#include "research_select_data.cdat"
#include "research_common.h"
#include "research_common_data.cdat"

#include "system/main.h"             // for HEAPID_xxxx
#include "system/gfl_use.h"          // for GFUser_xxxx
#include "system/palanm.h"           // for PaletteFadeXxxx
#include "system/bmp_oam.h"          // for BmpOam_xxxx
#include "gamesystem/game_beacon.h"  // for GAMEBEACON_xxxx
#include "sound/pm_sndsys.h"         // for PMSND_xxxx
#include "sound/wb_sound_data.sadl"  // for SEQ_SE_XXXX
#include "print/gf_font.h"           // for GFL_FONT_xxxx
#include "print/printsys.h"          // for PRINTSYS_xxxx

#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_research_radar_xxxx
#include "arc/app_menu_common.naix"         // for NARC_app_menu_common_xxxx
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

#include "../../../../resource/fldmapdata/flagwork/flag_define.h" // for FE_xxxx


//=========================================================================================
// ���������e�ύX��ʃ��[�N
//=========================================================================================
struct _RESEARCH_SELECT_WORK
{ 
  RESEARCH_COMMON_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID                heapID;
  GAMESYS_WORK*         gameSystem;
  GAMEDATA*             gameData;

  GFL_FONT*    font;
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  QUEUE*                 seqQueue;      // �V�[�P���X�L���[
  RESEARCH_SELECT_SEQ    seq;           // ���݂̃V�[�P���X
  u32                    seqCount;      // �V�[�P���X�J�E���^
  BOOL                   seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  RESEARCH_SELECT_RESULT result;        // ��ʏI������
  u32                    waitFrame;     // �t���[���o�ߑ҂��V�[�P���X�̑҂�����

  // ���j���[����
  MENU_ITEM menuCursorPos; // �J�[�\���ʒu

  // ��������
  u8 selectableTopicNum; // �I���\�Ȓ������ڂ̐�
  u8 selectedTopicID;    // �I��������������ID 
  u8 topicCursorPos;     // �J�[�\���ʒu ( == ��������ID )
  u8 topicCursorNextPos; // �ړ���̃J�[�\���ʒu 

  // �X�N���[��
  int scrollCursorPos;  // �X�N���[���J�[�\���ʒu
  int scrollStartPos;   // �X�N���[���J�n���̃J�[�\���ʒu
  int scrollEndPos;     // �X�N���[���I�����̃J�[�\���ʒu
  int scrollFrames;     // �X�N���[���t���[����
  int scrollFrameCount; // �X�N���[���t���[�����J�E���^

  // VBlank
  GFL_TCBSYS* VBlankTCBSystem; // VBlank���Ԓ��̃^�X�N�Ǘ��V�X�e��
  GFL_TCB*    VBlankTask;      // VBlank�^�C�~���O���ɍs���^�X�N

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL menuTouchHitTable[ MENU_TOUCH_AREA_NUM ];
  GFL_UI_TP_HITTBL topicTouchHitTable[ TOPIC_TOUCH_AREA_NUM ];
  GFL_UI_TP_HITTBL scrollTouchHitTable[ SCROLL_TOUCH_AREA_NUM ];

  // �p���b�g�A�j���[�V����
  PALETTE_ANIME* paletteAnime[ PALETTE_ANIME_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont[ BG_FONT_NUM ];
  BG_FONT* TopicsBGFont[ TOPIC_ID_NUM ]; // ��������

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ]; // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];               // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                 // �Z���A�N�^�[���[�N

  // BMP-OAM
  BMPOAM_SYS_PTR BmpOamSystem;                    // BMP-OAM �V�X�e��
  BMPOAM_ACT_PTR BmpOamActor[ BMPOAM_ACTOR_NUM ]; // BMP-OAM�A�N�^�[
  GFL_BMP_DATA*  BmpData[ BMPOAM_ACTOR_NUM ];     // �e�A�N�^�[�ɑΉ�����r�b�g�}�b�v�f�[�^

  // �J���[�p���b�g
  PALETTE_FADE_PTR paletteFadeSystem; // �p���b�g�t�F�[�h�����V�X�e��
};



//=========================================================================================
// ���֐��C���f�b�N�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
// ��LAYER 3 �V�[�P���X
//-----------------------------------------------------------------------------------------
// �V�[�P���X����������
static void InitSeq_SETUP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void InitSeq_STANDBY( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_STANDBY
static void InitSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void InitSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void InitSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_CONTROL
static void InitSeq_CONFIRM( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void InitSeq_DETERMINE( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void InitSeq_FADE_IN( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_IN 
static void InitSeq_FADE_OUT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void InitSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FRAME_WAIT
static void InitSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_RESET
static void InitSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_PALETTE_RESET
static void InitSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP 
// �V�[�P���X����
static void MainSeq_SETUP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void MainSeq_STANDBY( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_STANDBY
static void MainSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void MainSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void MainSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_CONTROL
static void MainSeq_CONFIRM( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void MainSeq_DETERMINE( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void MainSeq_FADE_IN( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_IN 
static void MainSeq_FADE_OUT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void MainSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FRAME_WAIT
static void MainSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_RESET
static void MainSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_PALETTE_RESET
static void MainSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP 
// �V�[�P���X�I������
static void FinishSeq_SETUP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SETUP
static void FinishSeq_STANDBY( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_STANDBY
static void FinishSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_KEY_WAIT
static void FinishSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_WAIT
static void FinishSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_CONTROL
static void FinishSeq_CONFIRM( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CONFIRM
static void FinishSeq_DETERMINE( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_DETERMINE
static void FinishSeq_FADE_IN( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_IN 
static void FinishSeq_FADE_OUT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FADE_OUT
static void FinishSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_FRAME_WAIT
static void FinishSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_SCROLL_RESET
static void FinishSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_PALETTE_RESET
static void FinishSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work ); // RESEARCH_SELECT_SEQ_CLEAN_UP 
// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSeq( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq ); // ���̃V�[�P���X���L���[�ɓo�^����
static void FinishCurrentSeq( RESEARCH_SELECT_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSeq( RESEARCH_SELECT_WORK* work ); // �����V�[�P���X��ύX����
static void SetSeq( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
static void SetWaitFrame( RESEARCH_SELECT_WORK* work, u32 frame ); // �t���[���o�ߑ҂��V�[�P���X�̑҂����Ԃ�ݒ肷��
static u32 GetWaitFrame( const RESEARCH_SELECT_WORK* work ); // �t���[���o�ߑ҂��V�[�P���X�̑҂����Ԃ��擾����
// VBlank�^�X�N
static void RegisterVBlankTask( RESEARCH_SELECT_WORK* work ); // VBlank�^�X�N��o�^����
static void ReleaseVBlankTask ( RESEARCH_SELECT_WORK* work ); // VBlank�^�X�N�̓o�^����������
static void VBlankFunc( GFL_TCB* tcb, void* wk ); // VBlank���̏���
//-----------------------------------------------------------------------------------------
// ��LAYER 2 �@�\
//-----------------------------------------------------------------------------------------
// ���j���[���ڃJ�[�\��
static void MoveMenuCursorUp( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����
static void MoveMenuCursorDirect( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // ���ڈړ�����
// �������ڃJ�[�\��
static void MoveTopicCursorUp( RESEARCH_SELECT_WORK* work ); // ��ֈړ�����
static void MoveTopicCursorDown( RESEARCH_SELECT_WORK* work ); // ���ֈړ�����
static void MoveTopicCursorDirect( RESEARCH_SELECT_WORK* work, u8 topicID ); // ���ڈړ�����
//-----------------------------------------------------------------------------------------
// ��LAYER 1 �ʑ���
//-----------------------------------------------------------------------------------------
// �Z�[�u�f�[�^
static u8 GetInvestigatingTopicID( const RESEARCH_SELECT_WORK* work ); // ���ݒ������̒�������ID���擾����
static void UpdateInvestigatingTopicID( const RESEARCH_SELECT_WORK* work ); // �������鍀�ڂ��X�V����
// ���j���[���ڃJ�[�\��
static void ShiftMenuCursorPos( RESEARCH_SELECT_WORK* work, int stride ); // ���j���[���ڃJ�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetMenuCursorPos( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڃJ�[�\���ʒu��ύX���� ( ���l�w�� ) 
// �������ڃJ�[�\��
static void SetTopicCursorPosDirect( RESEARCH_SELECT_WORK* work, int topciID ); // �������ڃJ�[�\���̈ړ����ݒ肷�� ( �_�C���N�g�ړ� ) 
static void SetTopicCursorNextPos( RESEARCH_SELECT_WORK* work, int stride ); // �������ڃJ�[�\���̈ړ����ݒ肷�� ( �I�t�Z�b�g�ړ� )
// �^�b�`�͈�
static void UpdateTopicTouchArea( RESEARCH_SELECT_WORK* work ); // �^�b�`�͈͂��X�V����
// ��������
static int GetNextTopicID( const RESEARCH_SELECT_WORK* work, int topicID ); // ���̒�������ID���擾����
static int GetPrevTopicID( const RESEARCH_SELECT_WORK* work, int topicID ); // �O�̒�������ID���擾����
static void SetupSelectableTopicNum( RESEARCH_SELECT_WORK* work ); // �I���\�Ȓ������ڂ̐����Z�b�g�A�b�v����
static u8 GetSelectableTopicNum( const RESEARCH_SELECT_WORK* work ); // �I���\�Ȓ������ڂ̐����擾����
static u8 GetSelectedTopicID( const RESEARCH_SELECT_WORK* work ); // �I�𒆂̒�������ID���擾����
static void SetSelectedTopicID( RESEARCH_SELECT_WORK* work, u8 topicID ); // ��������ID��I������
static void ResetSelectedTopicID( RESEARCH_SELECT_WORK* work ); // ��������ID�̑I������������
static BOOL IsTopicIDSelected( const RESEARCH_SELECT_WORK* work ); // �I���ς݂��𔻒肷��
static BOOL CheckTopicCanSelect( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ�I���\���ǂ������`�F�b�N����
// ���j���[���ڂ̕\��
static void SetMenuItemCursorOn( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuItemCursorOff( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
// �������ڂ̕\��
static void SetTopicButtonCursorOn( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetTopicButtonCursorOff( const RESEARCH_SELECT_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetTopicButtonInvestigating( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������̏�Ԃɂ���
static void SetTopicButtonNotInvestigating( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������łȂ���Ԃɖ߂�
static u8 CalcScreenLeftOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� ( �X�N���[���P�� )
static u8 CalcScreenTopOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� ( �X�N���[���P�� )
static int CalcDisplayLeftOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڃ{�^���̍���x���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� )
static int CalcDisplayTopOfTopicButton ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڃ{�^���̍���y���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� ) 
static int CalcDisplayBottomOfTopicButton ( const RESEARCH_SELECT_WORK* work, u8 topicID ); // �������ڃ{�^���̉E��y���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� ) 
static void UpdateTopicButtonMask( const RESEARCH_SELECT_WORK* work ); // �������ڃ{�^���̃X�N���[����荞�݂��B�����߂̃E�B���h�E���X�V����
// ���ʂ̕\��
static void UpdateSubDisplayStrings( RESEARCH_SELECT_WORK* work ); // ���ʂ̃J�[�\���ˑ�������\�����X�V����
// �X�N���[���o�[
static void UpdateScrollControlPos( const RESEARCH_SELECT_WORK* work ); // ���݂̃X�N���[�������l�ɍ��킹��, �X�N���[���o�[�̂܂ݕ����̈ʒu���X�V����
static int CalcScrollControlPos_byScrollValue( const RESEARCH_SELECT_WORK* work ); // �X�N���[�������l����, �X�N���[���o�[�̂܂ݕ����̈ʒu���v�Z����
static int CalcScrollCursorPos_byScrollControlPos( const RESEARCH_SELECT_WORK* work, int controlPos ); // �X�N���[���o�[�̂܂ݕ����̕\���ʒu����, �X�N���[���J�[�\���ʒu���Z�o����
static int GetScrollControlPos( const RESEARCH_SELECT_WORK* work ); // �X�N���[���o�[�̂܂ݕ����̕\���ʒu���擾����
// �X�N���[��
static void StartScroll( RESEARCH_SELECT_WORK* work, int startPos, int endPos, int frames ); // �X�N���[�����J�n����
static void UpdateScroll( RESEARCH_SELECT_WORK* work ); // �X�N���[�����X�V����
static BOOL CheckScrollEnd( RESEARCH_SELECT_WORK* work ); // �X�N���[�����I���������ǂ����𔻒肷��
static void ShiftScrollCursorPos( RESEARCH_SELECT_WORK* work, int offset ); // �X�N���[���J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetScrollCursorPos( RESEARCH_SELECT_WORK* work, int pos ); // �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w�� )
static void SetScrollCursorPosForce( RESEARCH_SELECT_WORK* work, int pos ); // �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w��E�͈͌���Ȃ� )
static void AdjustScrollCursor( RESEARCH_SELECT_WORK* work ); // �X�N���[�������l�ɍ��킹, �X�N���[���J�[�\���ʒu��␳����
static void UpdateScrollValue( const RESEARCH_SELECT_WORK* work ); // �X�N���[�������l���X�V����
static void AdjustScrollValue( const RESEARCH_SELECT_WORK* work ); // �X�N���[�������l���X�N���[���J�[�\���ʒu�ɍ��킹�čX�V����
static int GetScrollValue(); // �X�N���[�������l���擾����
static int GetMinScrollCursorMarginPos(); // �X�N���[���J�[�\���]���͈͂̍ŏ��l���擾����
static int GetMaxScrollCursorMarginPos(); // �X�N���[���J�[�\���]���͈͂̍ő�l���擾����
static int CalcTopicID_byScrollCursorPos( int pos ); // �w�肵���X�N���[���J�[�\���ʒu�ɂ���{�^���̒�������ID���v�Z����
static int CalcScrollCursorPosOfTopicButtonTop( int topicID ); // �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
static int CalcScrollCursorPosOfTopicButtonBottom( int topicID ); // �w�肵���������ڃ{�^���̒�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
static int GetMaxScrollValue( const RESEARCH_SELECT_WORK* work ); // �ő�X�N���[���l���擾����
static int GetMaxScrollCursorPos( const RESEARCH_SELECT_WORK* work ); // �X�N���[���J�[�\���̍ő�l���擾����
static BOOL CheckScrollControlCan( const RESEARCH_SELECT_WORK* work ); // �X�N���[�����삪�\���ǂ����𔻒肷��
// �������A�C�R��
static void UpdateTopicSelectIcon( const RESEARCH_SELECT_WORK* work ); // �������A�C�R���̕\����Ԃ��X�V����
// �V�[�P���X�L���[
static void InitSeqQueue( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[������������
static void CreateSeqQueue( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[�𐶐�����
static void DeleteSeqQueue( RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[��j������
// �^�b�`�̈�
static void SetupTouchArea( RESEARCH_SELECT_WORK* work ); // �^�b�`�̈���Z�b�g�A�b�v����
// �t�H���g�f�[�^
static void InitFont( RESEARCH_SELECT_WORK* work ); // �t�H���g�f�[�^������������
static void CreateFont( RESEARCH_SELECT_WORK* work ); // �t�H���g�f�[�^�𐶐�����
static void DeleteFont( RESEARCH_SELECT_WORK* work ); // �t�H���g�f�[�^��j������
// ���b�Z�[�W�f�[�^
static void InitMessages( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W�f�[�^������������
static void CreateMessages( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W�f�[�^�𐶐�����
static void DeleteMessages( RESEARCH_SELECT_WORK* work ); // ���b�Z�[�W�f�[�^��j������
// BG
static void SetupBG( RESEARCH_SELECT_WORK* work ); // BG�S�ʂ̃Z�b�g�A�b�v���s��
static void SetupSubBG_WINDOW( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ���Z�b�g�A�b�v����
static void SetupSubBG_FONT( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �t�H���g�� ) ���Z�b�g�A�b�v����
static void SetupMainBG_BAR( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �o�[�� ) ���Z�b�g�A�b�v����
static void SetupMainBG_WINDOW( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���Z�b�g�A�b�v����
static void SetupMainBG_FONT( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �t�H���g�� ) ���Z�b�g�A�b�v����
static void CleanUpBG( RESEARCH_SELECT_WORK* work ); // BG�S�ʂ̃N���[���A�b�v���s��
static void CleanUpSubBG_WINDOW( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ���N���[���A�b�v����
static void CleanUpSubBG_FONT( RESEARCH_SELECT_WORK* work ); // SUB-BG ( �t�H���g�� ) ���N���[���A�b�v����
static void CleanUpMainBG_BAR( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �o�[�� ) ���N���[���A�b�v����
static void CleanUpMainBG_WINDOW( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���N���[���A�b�v����
static void CleanUpMainBG_FONT( RESEARCH_SELECT_WORK* work ); // MAIN-BG ( �t�H���g�� ) ���N���[���A�b�v����
// ������`��I�u�W�F�N�g
static void InitBGFonts( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g������������
static void CreateBGFonts( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g�𐶐�����
static void DeleteBGFonts( RESEARCH_SELECT_WORK* work ); // ������`��I�u�W�F�N�g��j������
// OBJ
static void CreateClactSystem( RESEARCH_SELECT_WORK* work ); // OBJ �V�X�e���𐶐�����
static void DeleteClactSystem( RESEARCH_SELECT_WORK* work ); // OBJ �V�X�e����j������
static void InitOBJResources( RESEARCH_SELECT_WORK* work ); // OBJ ���\�[�X������������
static void RegisterSubObjResources( RESEARCH_SELECT_WORK* work ); // SUB-OBJ ���\�[�X��o�^����
static void ReleaseSubObjResources( RESEARCH_SELECT_WORK* work ); // SUB-OBJ ���\�[�X���������
static void RegisterMainObjResources( RESEARCH_SELECT_WORK* work ); // MAIN-OBJ ���\�[�X��o�^����
static void ReleaseMainObjResources( RESEARCH_SELECT_WORK* work ); // MAIN-OBJ ���\�[�X���������
static void InitClactUnits( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���j�b�g��j������
static void InitClactWorks( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N������������
static void CreateClactWorks( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RESEARCH_SELECT_WORK* work ); // �Z���A�N�^�[���[�N��j������
static u32 GetObjResourceRegisterIndex( const RESEARCH_SELECT_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
static GFL_CLUNIT* GetClactUnit( const RESEARCH_SELECT_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RESEARCH_SELECT_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
// BMP-OAM
static void InitBitmapDatas( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^������������
static void CreateBitmapDatas( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^���쐬����
static void SetupBitmapData_forDefault( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^���Z�b�g�A�b�v���� ( �f�t�H���g )
static void SetupBitmapData_forOK( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^���Z�b�g�A�b�v���� (�u�����Ă��v)
static void SetupBitmapData_forCANCEL( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^���Z�b�g�A�b�v���� (�u��߂�v)
static void DeleteBitmapDatas( RESEARCH_SELECT_WORK* work ); // �r�b�g�}�b�v�f�[�^��j������
static void SetupBmpOamSystem( RESEARCH_SELECT_WORK* work ); // BMP-OAM �V�X�e�����Z�b�g�A�b�v����
static void CleanUpBmpOamSystem( RESEARCH_SELECT_WORK* work ); // BMP-OAM �V�X�e�����N���[���A�b�v����
static void CreateBmpOamActors( RESEARCH_SELECT_WORK* work ); // BMP-OAM �A�N�^�[���쐬����
static void DeleteBmpOamActors( RESEARCH_SELECT_WORK* work ); // BMP-OAM �A�N�^�[��j������
static void BmpOamSetDrawEnable( RESEARCH_SELECT_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable ); // �\�����邩�ǂ�����ݒ肷��
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[���擾����
// �p���b�g�t�F�[�h
static void InitPaletteFadeSystem( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e��������������
static void SetupPaletteFadeSystem( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����Z�b�g�A�b�v����
static void CleanUpPaletteFadeSystem( RESEARCH_SELECT_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����N���[���A�b�v����
static void StartPaletteFadeOut( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�A�E�g���J�n����
static void StartPaletteFadeIn ( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�C�����J�n����
static BOOL IsPaletteFadeEnd( RESEARCH_SELECT_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RESEARCH_SELECT_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RESEARCH_SELECT_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RESEARCH_SELECT_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RESEARCH_SELECT_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RESEARCH_SELECT_WORK* work );  // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
static void StartPaletteAnime( RESEARCH_SELECT_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RESEARCH_SELECT_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void UpdatePaletteAnime( RESEARCH_SELECT_WORK* work );  // �p���b�g�A�j���[�V�������X�V����
// �ʐM�A�C�R��
static void SetupWirelessIcon( const RESEARCH_SELECT_WORK* work ); // �ʐM�A�C�R�����Z�b�g�A�b�v����
//-----------------------------------------------------------------------------------------
// ��LAYER 0 �f�o�b�O
//-----------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_SELECT_WORK* work ); // �V�[�P���X�L���[�̒��g��\������




//=========================================================================================
// ������������� ����֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̐���
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return �������e�ύX��ʃ��[�N
 */
//-----------------------------------------------------------------------------------------
RESEARCH_SELECT_WORK* CreateResearchSelectWork( RESEARCH_COMMON_WORK* commonWork )
{
  int i;
  RESEARCH_SELECT_WORK* work;
  HEAPID heapID;

  heapID = RESEARCH_COMMON_GetHeapID( commonWork );

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_SELECT_WORK) );

  // ������
  work->commonWork            = commonWork;
  work->heapID                = heapID;
  work->gameSystem            = RESEARCH_COMMON_GetGameSystem( commonWork );
  work->gameData              = RESEARCH_COMMON_GetGameData( commonWork );
  work->seq                   = RESEARCH_SELECT_SEQ_SETUP;
  work->seqCount              = 0;
  work->seqFinishFlag         = FALSE;
  work->result                = RESEARCH_SELECT_RESULT_NONE;
  work->waitFrame             = WAIT_FRAME_BUTTON;
  work->menuCursorPos         = MENU_ITEM_DETERMINATION_OK;
  work->topicCursorPos        = 0;
  work->topicCursorNextPos    = 0;
  work->selectableTopicNum    = 0;
  work->selectedTopicID       = TOPIC_ID_DUMMY;
  work->VBlankTCBSystem       = GFUser_VIntr_GetTCBSYS();
  work->scrollCursorPos       = MIN_SCROLL_CURSOR_POS;
  work->scrollStartPos        = 0;
  work->scrollEndPos          = 0;
  work->scrollFrames          = 0;
  work->scrollFrameCount      = 0;

  InitSeqQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitOBJResources( work );
  InitPaletteFadeSystem( work );
  InitBitmapDatas( work );
  InitPaletteAnime( work );

  CreateSeqQueue( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create work\n" );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work )
{
  // �V�[�P���X���Ƃ̏���
  switch( work->seq ) {
  case RESEARCH_SELECT_SEQ_SETUP:           MainSeq_SETUP( work );           break;
  case RESEARCH_SELECT_SEQ_STANDBY:         MainSeq_STANDBY( work );         break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:        MainSeq_KEY_WAIT( work );        break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:     MainSeq_SCROLL_WAIT( work );     break;
  case RESEARCH_SELECT_SEQ_SCROLL_CONTROL:  MainSeq_SCROLL_CONTROL( work );  break;
  case RESEARCH_SELECT_SEQ_CONFIRM:         MainSeq_CONFIRM( work );         break;
  case RESEARCH_SELECT_SEQ_DETERMINE:       MainSeq_DETERMINE( work );       break;
  case RESEARCH_SELECT_SEQ_FADE_IN:         MainSeq_FADE_IN( work );         break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:        MainSeq_FADE_OUT( work );        break;
  case RESEARCH_SELECT_SEQ_FRAME_WAIT:      MainSeq_FRAME_WAIT( work );      break;
  case RESEARCH_SELECT_SEQ_SCROLL_RESET:    MainSeq_SCROLL_RESET( work );    break;
  case RESEARCH_SELECT_SEQ_PALETTE_RESET:   MainSeq_PALETTE_RESET( work );   break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:        MainSeq_CLEAN_UP( work );        break;
  case RESEARCH_SELECT_SEQ_FINISH:          return work->result; // �I��
  default:  GF_ASSERT(0);
  }

  RESEARCH_COMMON_UpdatePaletteAnime( work->commonWork ); // ���ʃp���b�g�A�j���[�V�������X�V
  UpdatePaletteAnime( work ); // �p���b�g�A�j���[�V�������X�V
  GFL_CLACT_SYS_Main(); // �Z���A�N�^�[�V�X�e�� ���C������

  CountUpSeqCount( work ); // �V�[�P���X�J�E���^�X�V
  SwitchSeq( work );  // �V�[�P���X�X�V

  // �p��
  return RESEARCH_SELECT_RESULT_CONTINUE;
}





//=========================================================================================
// ��LAYER 4 �V�[�P���X����
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_SELECT_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SETUP( RESEARCH_SELECT_WORK* work )
{
  CreateFont( work );
  CreateMessages( work );
  SetupTouchArea( work );
  UpdateTopicTouchArea( work );
  SetupSelectableTopicNum( work );

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
  SetupBitmapData_forDefault( work );
  SetupBitmapData_forOK( work );
  SetupBitmapData_forCANCEL( work );
  SetupBmpOamSystem( work );
  CreateBmpOamActors( work );

  // �p���b�g�t�F�[�h�V�X�e�� ����
  SetupPaletteFadeSystem( work );

  // �p���b�g�A�j���[�V����
  CreatePaletteAnime( work );
  SetupPaletteAnime( work );

  // VBkank�^�X�N�o�^
  RegisterVBlankTask( work );

  // �ʐM�A�C�R��
  SetupWirelessIcon( work );

  // ���̃V�[�P���X���Z�b�g
  SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_WAIT ); 
  SetNextSeq( work, RESEARCH_SELECT_SEQ_FADE_IN ); 
  SetNextSeq( work, RESEARCH_SELECT_SEQ_STANDBY ); 

  // �V�[�P���X�I��
  FinishCurrentSeq( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[���͑҂��V�[�P���X ( RESEARCH_SELECT_SEQ_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_STANDBY( RESEARCH_SELECT_WORK* work )
{
  int trg;
  int touchTrg;
  int commonTouch;

  trg   = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->topicTouchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //�u���ǂ�v�{�^��
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    RESEARCH_COMMON_StartPaletteAnime( work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FRAME_WAIT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  }

  // �\���L�[ or A or B
  if( (trg & PAD_KEY_UP) ||
      (trg & PAD_KEY_DOWN) ||
      (trg & PAD_KEY_LEFT) ||
      (trg & PAD_KEY_RIGHT) ||
      (trg & PAD_BUTTON_A) || 
      (trg & PAD_BUTTON_B) ) {
    SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  //�u�������ځv�{�^��
  if( (TOPIC_TOUCH_AREA_TOPIC_0 <= touchTrg) && (touchTrg <= TOPIC_TOUCH_AREA_TOPIC_9) ) {
    // �I���\
    if( CheckTopicCanSelect( work, touchTrg ) == TRUE ) {
      StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );
      MoveTopicCursorDirect( work, touchTrg );            // �J�[�\���ړ�
      SetSelectedTopicID( work, work->topicCursorPos );   // �J�[�\���ʒu�̒������ڂ�I��
      UpdateSubDisplayStrings( work );                    // ���ʂ̃J�[�\���ˑ���������X�V
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                     // ���艹
      SetNextSeq( work, RESEARCH_SELECT_SEQ_CONFIRM );
      FinishCurrentSeq( work );
    }
    return;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_SELECT_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{ 
  int trg;
  int touchTrg;
  int commonTouch;
  BOOL select = FALSE;

  trg   = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->topicTouchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //�u���ǂ�v�{�^��
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );      // �L�����Z����
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FRAME_WAIT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  }

  // ���L�[
  if( trg & PAD_KEY_UP ) {
    MoveTopicCursorUp( work );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_WAIT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveTopicCursorDown( work );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_WAIT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  } 

  // A�{�^��
  if( trg & PAD_BUTTON_A ) {
    StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );
    SetSelectedTopicID( work, work->topicCursorPos ); // �J�[�\���ʒu�̒������ڂ�I��
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                   // ���艹
    SetNextSeq( work, RESEARCH_SELECT_SEQ_CONFIRM );
    FinishCurrentSeq( work );
    return;
  } 

  //�u�������ځv�{�^��
  if( (TOPIC_TOUCH_AREA_TOPIC_0 <= touchTrg) && (touchTrg <= TOPIC_TOUCH_AREA_TOPIC_9) ) {
    // �I���\
    if( CheckTopicCanSelect( work, touchTrg ) == TRUE ) {
      StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );
      MoveTopicCursorDirect( work, touchTrg );          // �J�[�\���ړ�
      UpdateSubDisplayStrings( work );                  // ���ʂ̃J�[�\���ˑ���������X�V
      SetSelectedTopicID( work, work->topicCursorPos ); // �J�[�\���ʒu�̒������ڂ�I��
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                   // ���艹
      SetNextSeq( work, RESEARCH_SELECT_SEQ_CONFIRM );
      FinishCurrentSeq( work );
    }
    return;
  }

  // B�{�^��
  if( trg & PAD_BUTTON_B ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 ); // �L�����Z����
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FRAME_WAIT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  } 

  // �X�N���[���o�[
  if( GFL_UI_TP_HitCont( work->scrollTouchHitTable ) == SCROLL_TOUCH_AREA_BAR ) {
    // �X�N���[������\
    if( CheckScrollControlCan( work ) == TRUE ) {
      SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_CONTROL );
      SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
      FinishCurrentSeq( work );
    }
    return;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������҂��V�[�P���X ( RESEARCH_SELECT_SEQ_SCROLL_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  // �X�N���[������
  UpdateScroll( work );           // �X�N���[�����X�V
  UpdateScrollValue( work );      // �X�N���[�������l���X�V
  UpdateTopicTouchArea( work );   // �^�b�`�͈͂��X�V����
  UpdateScrollControlPos( work ); // �X�N���[���o�[�̂܂ݕ���
  UpdateTopicSelectIcon( work );  // �������ڑI���A�C�R��
  UpdateTopicButtonMask( work );  // �������ڂ̃}�X�N�E�B���h���X�V����

  // �X�N���[���I��
  if( CheckScrollEnd(work) ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[������V�[�P���X ( RESEARCH_SELECT_SEQ_SCROLL_CONTROL ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work )
{
  u32 x, y;
  BOOL touch;

  touch = GFL_UI_TP_GetPointCont( &x, &y );

  // �^�b�`�������ꂽ
  if( touch == FALSE ) {
    FinishCurrentSeq( work );
    return;
  }

  // �X�N���[������
  {
    int scrollCursorPos;
    scrollCursorPos = CalcScrollCursorPos_byScrollControlPos( work, y ); // �^�b�`�ꏊ����, �X�N���[���J�[�\���ʒu���Z�o
    SetScrollCursorPos( work, scrollCursorPos );// �X�N���[���J�[�\���ʒu���X�V
    AdjustScrollValue( work );                  // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );               // �^�b�`�͈͂��X�V����
    UpdateScrollControlPos( work );             // �X�N���[���o�[�̂܂ݕ������X�V
    UpdateTopicSelectIcon( work );              // �������ڑI���A�C�R�����X�V
  }

  // �J�[�\���ʒu�X�V����
  {
    int min = GetMinScrollCursorMarginPos();
    int max = GetMaxScrollCursorMarginPos();
    int curTop = CalcScrollCursorPosOfTopicButtonTop( work->topicCursorPos );
    int curBottom = CalcScrollCursorPosOfTopicButtonBottom( work->topicCursorPos );
    if( curBottom <= min ) {
      MoveTopicCursorDirect( work, CalcTopicID_byScrollCursorPos(curBottom + TOPIC_BUTTON_HEIGHT/2) );
    }
    if( max <= curTop ) {
      MoveTopicCursorDirect( work, CalcTopicID_byScrollCursorPos(curTop - TOPIC_BUTTON_HEIGHT/2) );
    }
  }
}


//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X ( RESEARCH_SELECT_SEQ_CONFIRM ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  int trg;
  int touchTrg;

  trg = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->menuTouchHitTable );

  // �X�N���[������
  if( CheckScrollEnd(work) == FALSE ) {
    UpdateScroll( work );           // �X�N���[�����X�V
    UpdateScrollValue( work );      // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );   // �^�b�`�͈͂��X�V����
    UpdateScrollControlPos( work ); // �X�N���[���o�[�̂܂ݕ���
    UpdateTopicSelectIcon( work );  // �������ڑI���A�C�R��
    UpdateTopicButtonMask( work );  // �������ڂ̃}�X�N�E�B���h���X�V����
  }


  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work );
  }
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work );
  } 

  // A�{�^��
  if( trg & PAD_BUTTON_A ) {
    switch( work->menuCursorPos ) {
    case MENU_ITEM_DETERMINATION_OK:
      SetNextSeq( work, RESEARCH_SELECT_SEQ_DETERMINE );
      break;
    case MENU_ITEM_DETERMINATION_CANCEL:
      PMSND_PlaySE( SEQ_SE_CANCEL1 ); // �L�����Z����
      SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_RESET );
      SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
      SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
      break;
    default:
      GF_ASSERT(0);
    }
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );
    FinishCurrentSeq( work );
    return;
  } 

  // �u�����Ă��v�{�^��
  if( touchTrg == MENU_TOUCH_AREA_OK_BUTTON ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_OK );
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_DETERMINE );
    FinishCurrentSeq( work );
    return;
  } 

  // B�{�^�� or �u��߂�v�{�^��
  if( (trg & PAD_BUTTON_B) || 
      (touchTrg == MENU_TOUCH_AREA_CANCEL_BUTTON) ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_CANCEL );
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );
    PMSND_PlaySE( SEQ_SE_CANCEL1 ); // �L�����Z����
    SetNextSeq( work, RESEARCH_SELECT_SEQ_SCROLL_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��V�[�P���X ( RESEARCH_SELECT_SEQ_DETERMINE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  // ��莞�Ԃ��o��
  if( SEQ_DETERMINE_WAIT_FRAMES <= work->seqCount ) {
    SetNextSeq( work, RESEARCH_SELECT_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_PALETTE_RESET );
    SetNextSeq( work, RESEARCH_SELECT_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�C�� �V�[�P���X ( RESEARCH_SELECT_SEQ_FADE_IN ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FADE_IN( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSeq( work );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g �V�[�P���X ( RESEARCH_SELECT_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSeq( work );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂� �V�[�P���X ( RESEARCH_SELECT_SEQ_FRAME_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work )
{
  // �҂����Ԃ��o��
  if( GetWaitFrame(work) < work->seqCount ) {
    FinishCurrentSeq( work ); 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g���A�V�[�P���X ( RESEARCH_SELECT_SEQ_PALETTE_RESET ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�t�F�[�h����
  if( IsPaletteFadeEnd( work ) ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g���A�V�[�P���X ( RESEARCH_SELECT_SEQ_SCROLL_RESET ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work )
{
  // �X�N���[������
  UpdateScroll( work );           // �X�N���[�����X�V
  UpdateScrollValue( work );      // �X�N���[�������l���X�V
  UpdateTopicTouchArea( work );   // �^�b�`�͈͂��X�V����
  UpdateScrollControlPos( work ); // �X�N���[���o�[�̂܂ݕ���
  UpdateTopicSelectIcon( work );  // �������ڑI���A�C�R��
  UpdateTopicButtonMask( work );  // �������ڂ̃}�X�N�E�B���h���X�V����

  // �X�N���[���I��
  if( CheckScrollEnd(work) ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_SELECT_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{ 
  // VBlank�^�X�N������
  ReleaseVBlankTask( work );

  // �p���b�g�A�j���[�V���� 
  CleanUpPaletteAnime( work );
  DeletePaletteAnime( work );

  // ���ʃp���b�g�A�j���[�V����
  RESEARCH_COMMON_StopAllPaletteAnime( work->commonWork ); // ��~����, 
  RESEARCH_COMMON_ResetAllPalette( work->commonWork );     // �p���b�g�����ɖ߂�

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
  SetNextSeq( work, RESEARCH_SELECT_SEQ_FINISH );
  FinishCurrentSeq( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_SELECT_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq ) {
  case RESEARCH_SELECT_SEQ_SETUP:          maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_STANDBY:        maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:       maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:    maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_SCROLL_CONTROL: maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_CONFIRM:        maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_DETERMINE:      maxCount = SEQ_DETERMINE_WAIT_FRAMES; break;
  case RESEARCH_SELECT_SEQ_FADE_IN:        maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:       maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_FRAME_WAIT:     maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_SCROLL_RESET:   maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_PALETTE_RESET:  maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:       maxCount = 0xffffffff;                break;
  case RESEARCH_SELECT_SEQ_FINISH:         maxCount = 0xffffffff;                break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < work->seqCount ) { work->seqCount = maxCount; }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X���L���[�ɓo�^����
 *
 * @param work
 * @param nextSeq �o�^����V�[�P���X
 */
//-----------------------------------------------------------------------------------------
static void SetNextSeq( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishCurrentSeq( RESEARCH_SELECT_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish current sequence\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//-----------------------------------------------------------------------------------------
static void SetResult( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_SELECT_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set result (%d)\n", result );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂��V�[�P���X�̑҂����Ԃ�ݒ肷��
 *
 * @param work
 * @param frame �ݒ肷��҂��t���[����
 */
//-----------------------------------------------------------------------------------------
static void SetWaitFrame( RESEARCH_SELECT_WORK* work, u32 frame )
{
  work->waitFrame = frame;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂��V�[�P���X�̑҂����Ԃ��擾����
 *
 * @param work
 *
 * @return �҂��t���[����
 */
//-----------------------------------------------------------------------------------------
static u32 GetWaitFrame( const RESEARCH_SELECT_WORK* work )
{
  return work->waitFrame;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SwitchSeq( RESEARCH_SELECT_WORK* work )
{
  RESEARCH_SELECT_SEQ nextSeq;

  if( work->seqFinishFlag == FALSE ){ return; }  // ���݂̃V�[�P���X���I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->seqQueue ) ){ return; } // �V�[�P���X�L���[�ɓo�^����Ă��Ȃ�

  // �ύX
  nextSeq = QUEUE_DeQueue( work->seqQueue );
  SetSeq( work, nextSeq ); 

  // DEBUG: �V�[�P���X�L���[��\��
  DebugPrint_seqQueue( work );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work
 * @parma nextSeq �ݒ肷��V�[�P���X
 */
//-----------------------------------------------------------------------------------------
static void SetSeq( RESEARCH_SELECT_WORK* work, RESEARCH_SELECT_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq ) {
  case RESEARCH_SELECT_SEQ_SETUP:            FinishSeq_SETUP( work );            break;
  case RESEARCH_SELECT_SEQ_STANDBY:          FinishSeq_STANDBY( work );          break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:         FinishSeq_KEY_WAIT( work );         break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:      FinishSeq_SCROLL_WAIT( work );      break;
  case RESEARCH_SELECT_SEQ_SCROLL_CONTROL:   FinishSeq_SCROLL_CONTROL( work );   break;
  case RESEARCH_SELECT_SEQ_CONFIRM:          FinishSeq_CONFIRM( work );          break;
  case RESEARCH_SELECT_SEQ_DETERMINE:        FinishSeq_DETERMINE( work );        break;
  case RESEARCH_SELECT_SEQ_FADE_IN:          FinishSeq_FADE_IN( work );          break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:         FinishSeq_FADE_OUT( work );         break;
  case RESEARCH_SELECT_SEQ_FRAME_WAIT:       FinishSeq_FRAME_WAIT( work );       break;
  case RESEARCH_SELECT_SEQ_SCROLL_RESET:     FinishSeq_SCROLL_RESET( work );     break;
  case RESEARCH_SELECT_SEQ_PALETTE_RESET:    FinishSeq_PALETTE_RESET( work );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:         FinishSeq_CLEAN_UP( work );         break;
  case RESEARCH_SELECT_SEQ_FINISH:           break;
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq = nextSeq;
  work->seqCount = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq ) {
  case RESEARCH_SELECT_SEQ_SETUP:            InitSeq_SETUP( work );            break;
  case RESEARCH_SELECT_SEQ_STANDBY:          InitSeq_STANDBY( work );          break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:         InitSeq_KEY_WAIT( work );         break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:      InitSeq_SCROLL_WAIT( work );      break;
  case RESEARCH_SELECT_SEQ_SCROLL_CONTROL:   InitSeq_SCROLL_CONTROL( work );   break;
  case RESEARCH_SELECT_SEQ_CONFIRM:          InitSeq_CONFIRM( work );          break;
  case RESEARCH_SELECT_SEQ_DETERMINE:        InitSeq_DETERMINE( work );        break;
  case RESEARCH_SELECT_SEQ_FADE_IN:          InitSeq_FADE_IN( work );          break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:         InitSeq_FADE_OUT( work );         break;
  case RESEARCH_SELECT_SEQ_FRAME_WAIT:       InitSeq_FRAME_WAIT( work );       break;
  case RESEARCH_SELECT_SEQ_SCROLL_RESET:     InitSeq_SCROLL_RESET( work );     break;
  case RESEARCH_SELECT_SEQ_PALETTE_RESET:    InitSeq_PALETTE_RESET( work );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:         InitSeq_CLEAN_UP( work );         break;
  case RESEARCH_SELECT_SEQ_FINISH:           break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set seq ==> " );
  switch( nextSeq ) {
  case RESEARCH_SELECT_SEQ_SETUP:            OS_TFPrintf( PRINT_TARGET, "SETUP\n" );            break;
  case RESEARCH_SELECT_SEQ_STANDBY:          OS_TFPrintf( PRINT_TARGET, "STANDBY\n" );          break;
  case RESEARCH_SELECT_SEQ_KEY_WAIT:         OS_TFPrintf( PRINT_TARGET, "KEY_WAIT\n" );         break;
  case RESEARCH_SELECT_SEQ_SCROLL_WAIT:      OS_TFPrintf( PRINT_TARGET, "SCROLL_WAIT\n"    );   break;
  case RESEARCH_SELECT_SEQ_SCROLL_CONTROL:   OS_TFPrintf( PRINT_TARGET, "SCROLL_CONTROL\n" );   break;
  case RESEARCH_SELECT_SEQ_CONFIRM:          OS_TFPrintf( PRINT_TARGET, "CONFIRM\n" );          break;
  case RESEARCH_SELECT_SEQ_DETERMINE:        OS_TFPrintf( PRINT_TARGET, "DETERMINE\n" );        break;
  case RESEARCH_SELECT_SEQ_FADE_IN:          OS_TFPrintf( PRINT_TARGET, "FADE_IN\n" );          break;
  case RESEARCH_SELECT_SEQ_FADE_OUT:         OS_TFPrintf( PRINT_TARGET, "FADE_OUT\n" );         break;
  case RESEARCH_SELECT_SEQ_FRAME_WAIT:       OS_TFPrintf( PRINT_TARGET, "FRAME_WAIT\n" );       break;
  case RESEARCH_SELECT_SEQ_SCROLL_RESET:     OS_TFPrintf( PRINT_TARGET, "SCROLL_RESET\n" );     break;
  case RESEARCH_SELECT_SEQ_PALETTE_RESET:    OS_TFPrintf( PRINT_TARGET, "PALETTE_RESET\n" );    break;
  case RESEARCH_SELECT_SEQ_CLEAN_UP:         OS_TFPrintf( PRINT_TARGET, "CLEAN_UP\n" );         break;
  case RESEARCH_SELECT_SEQ_FINISH:           OS_TFPrintf( PRINT_TARGET, "FINISH\n" );           break;
  default:  GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SETUP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_SETUP( RESEARCH_SELECT_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SETUP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_STANDBY )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_STANDBY( RESEARCH_SELECT_WORK* work )
{
  SetTopicButtonCursorOff( work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq STANDBY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_KEY_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  SetTopicButtonCursorOn( work ); // �J�[�\��������Ă����Ԃɂ���

  // �m�F���b�Z�[�W�ƑI�����ڂ�����
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq KEY_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  // �X�N���[���J�n
  {
    int startPos = work->scrollCursorPos;
    int endPos;
    int frames;
    if( work->topicCursorPos < work->topicCursorNextPos ) {
      // ���֐i�ޏꍇ��, �X�N���[���J�[�\���̏I�_�̓{�^���̒�ӈʒu + 1
      endPos = CalcScrollCursorPosOfTopicButtonBottom( work->topicCursorNextPos ) + 1;
    }
    else {
      // ��֐i�ޏꍇ��, �X�N���[���J�[�\���̏I�_�̓{�^���̏�ӈʒu
      endPos = CalcScrollCursorPosOfTopicButtonTop( work->topicCursorNextPos );
    }
    // �X�N���[�������l�ɕύX������X�N���[���̏ꍇ,
    // �X�N���[���J�[�\���n�_���W��[�ɃZ�b�g����
    {
      int min = GetMinScrollCursorMarginPos();
      int max = GetMaxScrollCursorMarginPos();
      if( (endPos < min) || (max < endPos) )
      {
        if( startPos < endPos ) {
          startPos = max;
        }
        else {
          startPos = min;
        }
      }
    }
    frames = ( MATH_MAX(startPos, endPos) - MATH_MIN(startPos, endPos) ) / 3;
    // �X�N���[�������l�ɕύX���Ȃ��X�N���[���̏ꍇ,
    // 1�t���[���ŏ�������
    {
      int min = GetMinScrollCursorMarginPos();
      int max = GetMaxScrollCursorMarginPos();
      if( ( min <= startPos) && (startPos <= max) && 
          ( min <= endPos) && (endPos <= max)  ) 
      {
        frames = 1;
      }
    } 
    StartScroll( work, startPos, endPos, frames  );  
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SCROLL_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_CONTROL )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SCROLL_CONTROL\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_CONFIRM )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_CONFIRM( RESEARCH_SELECT_WORK* work )
{
  SetMenuCursorPos( work, MENU_ITEM_DETERMINATION_OK );         // �J�[�\���ʒu��������
  SetMenuItemCursorOff( work, MENU_ITEM_DETERMINATION_CANCEL ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
  SetMenuItemCursorOn( work, MENU_ITEM_DETERMINATION_OK );      // �J�[�\��������Ă����Ԃɂ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );      // �p���b�g�A�j���[�V�����J�n

  // �m�F���b�Z�[�W�ƑI�����ڂ�\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, TRUE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, TRUE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, TRUE );

  BG_FONT_SetPalette( work->TopicsBGFont[ work->topicCursorPos ], 0xe); // �t�H���g�̃p���b�g��ύX ( �p���b�g�t�F�[�h�������ɂȂ�悤�� )
  StartPaletteFadeOut( work ); // �p���b�g�t�F�[�h�A�E�g�J�n

  // �X�N���[���J�n
  {
    int centerPos = ( GetMinScrollCursorMarginPos() + GetMaxScrollCursorMarginPos() ) / 2;
    int startPos = work->scrollCursorPos;
    int endPos   = CalcScreenTopOfTopicButton( work, work->topicCursorPos ) * DOT_PER_CHARA;
    int frames;
    // �J�[�\���ʒu�̒������ڂ���ʂ̔C�ӂ̈ʒu�܂ŗ���悤�ɃX�N���[��������
    if( centerPos < endPos )
    { // �i�ޕ����ŃJ�[�\���ʒu�̎n�_�E�I�_��ς���
      startPos = GetMaxScrollCursorMarginPos();
      //endPos = startPos + (endPos - centerPos);
      endPos = GetMaxScrollCursorMarginPos() + (endPos - centerPos);
      //endPos = 24 * 20;
    }
    else
    {
      startPos = GetMinScrollCursorMarginPos();
      //endPos = startPos - (centerPos - endPos);
      endPos = GetMinScrollCursorMarginPos() - (centerPos - endPos);
    }
    frames = ( MATH_MAX(startPos, endPos) - MATH_MIN(startPos, endPos) ) / 2;
    StartScroll( work, startPos, endPos, frames  );  
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq CONFIRM\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_DETERMINE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_DETERMINE( RESEARCH_SELECT_WORK* work )
{
  // ���������������ڂ̃{�^����, �������Ă��Ȃ���Ԃɖ߂�
  SetTopicButtonNotInvestigating( work, GetInvestigatingTopicID(work) );

  // �������鍀�ڂ��X�V
  UpdateInvestigatingTopicID( work ); 

  // �V���ɒ������鍀�ڂ̃{�^����, �������̏�Ԃɂ���
  SetTopicButtonInvestigating( work, GetSelectedTopicID(work) );
  UpdateTopicSelectIcon( work );

  //�u���傤�����@���������܂��I�v��\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_DETERMINE, TRUE ); 

  // �����J�nSE
  PMSND_PlaySE( SEQ_SE_SYS_80 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq DETERMINE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_FADE_IN )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FADE_IN( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h�C���J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq FADE IN\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_FADE_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq FADE OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_FRAME_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq FRAME WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_PALETTE_RESET )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work )
{
  // �p���b�g�̃t�F�[�h�C�����J�n����
  StartPaletteFadeIn( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq PALETTE_RESET\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_RESET )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work )
{
  // �X�N���[���J�n
  {
    int startPos = work->scrollCursorPos;
    int endPos = startPos;
    int frames;
    // �X�N���[���J�[�\���I�_�ʒu������
    if( GetMaxScrollCursorPos(work) < startPos ) {
      endPos = GetMaxScrollValue(work) + SCROLL_CURSOR_TOP_MARGIN;
    }
    else if( startPos < MIN_SCROLL_CURSOR_POS ) {
      endPos = MIN_SCROLL_VALUE + DISP_DOT_HEIGHT - SCROLL_CURSOR_BOTTOM_MARGIN;
    }
    // �X�N���[�������l�ɕύX������X�N���[���̏ꍇ,
    // �X�N���[���J�[�\���n�_���W��[�ɃZ�b�g����
    {
      int min = GetMinScrollCursorMarginPos();
      int max = GetMaxScrollCursorMarginPos();
      if( (endPos < min) || (max < endPos) )
      {
        if( startPos < endPos ) {
          startPos = max;
        }
        else {
          startPos = min;
        }
      }
    }
    frames = ( MATH_MAX(startPos, endPos) - MATH_MIN(startPos, endPos) ) / 3;
    // �X�N���[�������l�ɕύX���Ȃ��X�N���[���̏ꍇ,
    // 1�t���[���ŏ�������
    {
      int min = GetMinScrollCursorMarginPos();
      int max = GetMaxScrollCursorMarginPos();
      if( ( min <= startPos) && (startPos <= max) && 
          ( min <= endPos) && (endPos <= max)  ) 
      {
        frames = 1;
      }
    } 
    StartScroll( work, startPos, endPos, frames  );  
  }
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq SCROLL_RESET\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_SELECT_SEQ_CLEAN_UP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq CLEAN_UP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SETUP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_SETUP( RESEARCH_SELECT_WORK* work )
{
  u8 nowTopicID;

  // �������̍���ID���擾
  nowTopicID = GetInvestigatingTopicID( work );

  // �������̍��ڂ�����ꍇ
  if( nowTopicID != INVESTIGATING_QUESTION_NULL ) {
    // �������̍��ڂ�I����Ԃɂ���
    SetTopicButtonInvestigating( work, nowTopicID ); // �I�����Ă����Ԃɂ���
    SetSelectedTopicID( work, nowTopicID );     // �������̍��ڂ�I��
    UpdateTopicSelectIcon( work );              // �������ڑI���A�C�R�����X�V
  }

  // �p���b�g�A�j���[�V�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_ON );

  // �X�N���[���o�[��OBJ��\��
  {
    GFL_CLWK* scrollbar;
    GFL_CLACTPOS pos;
    u16 setSurface;

    scrollbar = GetClactWork( work, CLWK_SCROLL_BAR );
    pos.x = 128;
    pos.y = 96;
    setSurface = ClactWorkInitData[ CLWK_SCROLL_BAR ].setSurface; 

    GFL_CLACT_WK_SetPos( scrollbar, &pos, setSurface );
    GFL_CLACT_WK_SetDrawEnable( scrollbar, TRUE );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SETUP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_STANDBY )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_STANDBY( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq STANDBY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_KEY_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_KEY_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq KEY_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_SCROLL_WAIT( RESEARCH_SELECT_WORK* work )
{
  SetTopicCursorPosDirect( work, work->topicCursorNextPos ); // �������ڃJ�[�\���ʒu���X�V
  UpdateSubDisplayStrings( work ); // ���ʂ̃J�[�\���ˑ���������X�V
  UpdateTopicButtonMask( work ); // �E�B���h�E��؂�

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SCROLL_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_CONTROL )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_SCROLL_CONTROL( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SCROLL_CONTROL\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_CONFIRM )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_CONFIRM( RESEARCH_SELECT_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq CONFIRM\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_DETERMINE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_DETERMINE( RESEARCH_SELECT_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq DETERMINE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_FADE_IN )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FADE_IN( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq FADE_IN\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_FADE_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FADE_OUT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq FADE_OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_FRAME_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FRAME_WAIT( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq FRAME_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_PALETTE_RESET )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_PALETTE_RESET( RESEARCH_SELECT_WORK* work )
{
  // �t�H���g�̃p���b�g�����ɖ߂�
  BG_FONT_SetPalette( work->TopicsBGFont[ work->topicCursorPos ], 0xf); 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq PALETTE_RESET\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_SCROLL_RESET )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_SCROLL_RESET( RESEARCH_SELECT_WORK* work )
{
  UpdateTopicButtonMask( work ); // �E�B���h�E��؂�

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq SCROLL_RESET\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_SELECT_SEQ_CLEAN_UP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_CLEAN_UP( RESEARCH_SELECT_WORK* work )
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE ); //�E�B���h�E�𖳌���

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: finish seq CLEAN_UP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank ���荞�ݏ���
 *
 * @param tcb
 * @parma wk
 */
//-----------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB* tcb, void* wk )
{
  RESEARCH_SELECT_WORK* work = (RESEARCH_SELECT_WORK*)wk;

  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( work->paletteFadeSystem );
}



//=========================================================================================
// ��LAYER 3 �@�\
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorUp( RESEARCH_SELECT_WORK* work )
{ 
  SetMenuItemCursorOff( work, work->menuCursorPos );// �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, -1 );                   // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos ); // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                   // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorDown( RESEARCH_SELECT_WORK* work )
{
  SetMenuItemCursorOff( work, work->menuCursorPos );// �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, 1 );                    // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos ); // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                   // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param menuItem �ړ���̃��j���[����
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorDirect( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  SetMenuItemCursorOff( work, work->menuCursorPos );// �J�[�\��������Ă��Ȃ���Ԃɂ���
  SetMenuCursorPos( work, menuItem );               // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos ); // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                   // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\������Ɉړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveTopicCursorUp( RESEARCH_SELECT_WORK* work )
{
  SetTopicButtonCursorOff( work );   // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, -1 ); // �ړ����ݒ�
  PMSND_PlaySE( SEQ_SE_SELECT1 );    // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\�������Ɉړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveTopicCursorDown( RESEARCH_SELECT_WORK* work )
{
  SetTopicButtonCursorOff( work );   // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorNextPos( work, 1 );  // �ړ����ݒ�
  PMSND_PlaySE( SEQ_SE_SELECT1 );    // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param topicID �ړ���̒�������ID
 */
//-----------------------------------------------------------------------------------------
static void MoveTopicCursorDirect( RESEARCH_SELECT_WORK* work, u8 topicID )
{
  SetTopicButtonCursorOff( work );          // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorPosDirect( work, topicID ); // �J�[�\���ʒu���X�V
  SetTopicButtonCursorOn( work );           // �ړ���̍��ڂ�I����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );           // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_SET ); // �p���b�g�A�j���J�n
}




//=========================================================================================
// ��LAYER 2 �ʑ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�����ړ�����
 *
 * @parma work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void ShiftMenuCursorPos( RESEARCH_SELECT_WORK* work, int stride )
{
  int nowPos;
  int nextPos;

  // �J�[�\���ʒu���X�V
  nowPos  = work->menuCursorPos;
  nextPos = (nowPos + stride + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  work->menuCursorPos = nextPos; 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: shift menu cursor ==> %d\n", nextPos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���ʒu��ύX���� ( ���l�w�� )
 *
 * @param work
 * @param menuItem �ݒ肷�郁�j���[���ڈʒu
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorPos( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  // �J�[�\���ʒu���X�V
  work->menuCursorPos = menuItem;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set menu cursor ==> %d\n", menuItem );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̈ړ����ݒ肷��
 *
 * @param work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void SetTopicCursorNextPos( RESEARCH_SELECT_WORK* work, int stride )
{
  int nowPos, nextPos;

  // �ړ���̃J�[�\���ʒu���Z�o
  nowPos  = work->topicCursorPos;
  nextPos = (nowPos + stride + GetSelectableTopicNum(work)) % GetSelectableTopicNum(work);

  // �J�[�\���̈ړ����ݒ�
  work->topicCursorNextPos = nextPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set topic cursor next pos ==> %d\n", nextPos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̈ʒu��ݒ肷��
 *
 * @param work
 * @param topicID �ݒ肷�钲������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicCursorPosDirect( RESEARCH_SELECT_WORK* work, int topicID )
{
  // �J�[�\���ʒu���X�V
  work->topicCursorPos = topicID;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set topic cursor pos direct ==> %d\n", topicID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃^�b�`�͈͂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicTouchArea( RESEARCH_SELECT_WORK* work )
{
  int idx;
  
  for( idx=TOPIC_TOUCH_AREA_TOPIC_0; idx <= TOPIC_TOUCH_AREA_TOPIC_9; idx++ )
  {
    int left, top;

    left = CalcDisplayLeftOfTopicButton( work, idx );
    top  = CalcDisplayTopOfTopicButton( work, idx );

    work->topicTouchHitTable[ idx ].rect.left   = left;
    work->topicTouchHitTable[ idx ].rect.right  = left + TOPIC_BUTTON_TOUCH_AREA_WIDTH;
    work->topicTouchHitTable[ idx ].rect.top    = top; 
    work->topicTouchHitTable[ idx ].rect.bottom = top + TOPIC_BUTTON_TOUCH_AREA_HEIGHT;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update touch area\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I���\�Ȓ������ڂ̐����Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSelectableTopicNum( RESEARCH_SELECT_WORK* work )
{
  EVENTWORK* evwork;
  int num;

  evwork = GAMEDATA_GetEventWork( work->gameData );

  // �A���P�[�g�ɓ����������J�E���g
  num = 1; // �擪���ڂ͍ŏ�����I�ׂ�
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_YOU      ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_FAVARITE ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_WISH     ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_PARTNER  ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_TASTE    ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_HOBBY    ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_SCHOOL   ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_PLAY     ) == TRUE ) { num++; }
  if( EVENTWORK_CheckEventFlag( evwork, FE_RES_QUESTION_POKEMON  ) == TRUE ) { num++; }

  // �I���\�Ȓ������ڂ̐����L��
  work->selectableTopicNum = num;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup selectable topic num ==> %d\n", num );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I���\�Ȓ������ڂ̐����擾����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static u8 GetSelectableTopicNum( const RESEARCH_SELECT_WORK* work )
{
  return work->selectableTopicNum;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̒�������ID���擾����
 *
 * @parma work
 *
 * @return �I�𒆂̒�������ID ( TOPIC_ID_XXXX )
 *         ���I���̏ꍇ TOPIC_ID_DUMMY
 */
//-----------------------------------------------------------------------------------------
static u8 GetSelectedTopicID( const RESEARCH_SELECT_WORK* work )
{
  return work->selectedTopicID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ�I������
 *
 * @param work
 * @param topicID �I�����钲������ID
 */
//-----------------------------------------------------------------------------------------
static void SetSelectedTopicID( RESEARCH_SELECT_WORK* work, u8 topicID )
{
  work->selectedTopicID = topicID;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: selected topicID ==>%d\n", topicID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ̑I�����L�����Z������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ResetSelectedTopicID( RESEARCH_SELECT_WORK* work )
{
  work->selectedTopicID = TOPIC_ID_DUMMY;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release topicID\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ�I�����Ă��邩�ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �������ڂ�I�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------------
static BOOL IsTopicIDSelected( const RESEARCH_SELECT_WORK* work )
{
  return (work->selectedTopicID != TOPIC_ID_DUMMY);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ�I���\���ǂ������`�F�b�N����
 *
 * @param work
 * @param topicID ���肷�钲�����ڂ�ID
 *
 * @return �w�肵���������ڂ�I���\�Ȃ� TRUE
 *         �����łȂ��Ȃ� FALSE
 */
//-----------------------------------------------------------------------------------------
static BOOL CheckTopicCanSelect( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  if( topicID < GetSelectableTopicNum( work ) ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 * @param menuItem ���j���[����
 */
//-----------------------------------------------------------------------------------------
static void SetMenuItemCursorOn( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACT_PTR BmpOamActor;

  // �J�[�\���ʒu�̃��j���[���ڂɑΉ�����BMP-OAM �A�N�^�[���擾
  BmpOamActor = GetBmpOamActorOfMenuItem( work, menuItem );

  // BMP-OAM �A�N�^�[�̃p���b�g�I�t�Z�b�g��ύX
  BmpOam_ActorSetPaletteOffset( BmpOamActor, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����j���[���ڂ�, ���j���[���ڂ��J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 * @param menuItem ���j���[����
 */
//-----------------------------------------------------------------------------------------
static void SetMenuItemCursorOff( RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACT_PTR BmpOamActor;

  // �J�[�\���ʒu�̃��j���[���ڂɑΉ�����BMP-OAM �A�N�^�[���擾
  BmpOamActor = GetBmpOamActorOfMenuItem( work, menuItem );

  // BMP-OAM �A�N�^�[�̃p���b�g�I�t�Z�b�g��ύX
  BmpOam_ActorSetPaletteOffset( BmpOamActor, 1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonCursorOn( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u8 BGFrame;
  u8 left, top, width, height;
  u8 paletteNo;

  // �X�N���[���X�V�p�����[�^������
  topicID   = work->topicCursorPos;
  left      = CalcScreenLeftOfTopicButton( work, topicID );
  top       = CalcScreenTopOfTopicButton( work, topicID );
  width     = TOPIC_BUTTON_WIDTH;
  height    = TOPIC_BUTTON_HEIGHT; 
  BGFrame   = MAIN_BG_WINDOW;
  paletteNo = MAIN_BG_PALETTE_WINDOW_ON;

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, left, top, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//-----------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonCursorOff( const RESEARCH_SELECT_WORK* work )
{
  u8 topicID;
  u8 BGFrame;
  u8 left, top, width, height;
  u8 paletteNo;

  // �X�N���[���X�V�p�����[�^������
  topicID   = work->topicCursorPos;
  left      = CalcScreenLeftOfTopicButton( work, topicID );
  top       = CalcScreenTopOfTopicButton( work, topicID );
  width     = TOPIC_BUTTON_WIDTH;
  height    = TOPIC_BUTTON_HEIGHT; 
  BGFrame   = MAIN_BG_WINDOW;
  paletteNo = MAIN_BG_PALETTE_WINDOW_OFF;

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, left, top, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڂ̃{�^����, �������̏�Ԃɐݒ肷��
 *
 * @param work
 * @param topicID ��������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonInvestigating( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u16* screenBuffer1;
  u16* screenBuffer2;
  int xOffset, yOffset;

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

      left    = TOPIC_BUTTON_X;
      top     = CalcScreenTopOfTopicButton( work, topicID );
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

    left    = TOPIC_BUTTON_X;
    top     = CalcScreenTopOfTopicButton( work, topicID );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڂ̃{�^����, �������łȂ���Ԃɖ߂�
 *
 * @param work
 * @param topicID ��������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonNotInvestigating( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u16* screenBuffer1;
  u16* screenBuffer2;
  int xOffset, yOffset;

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

      left    = TOPIC_BUTTON_X - 1;
      top     = CalcScreenTopOfTopicButton( work, topicID );
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

    left    = TOPIC_BUTTON_X - 1;
    top     = CalcScreenTopOfTopicButton( work, topicID );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̃J�[�\���ˑ�������\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃X�N���[�� x ���W���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵����������ID���Y������X�N���[���͈͂̍���x���W (�X�N���[���P��)
 */
//-----------------------------------------------------------------------------------------
static u8 CalcScreenLeftOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u8 left;

  // �f�t�H���g�̈ʒu
  left = TOPIC_BUTTON_X;

  // �������̏ꍇ, 1�L���������ɂ���
  if( GetInvestigatingTopicID(work) == topicID ){ left -= 1; }

  return left;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̃X�N���[�� y ���W���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵����������ID���Y������X�N���[���͈͂̍���y���W (�X�N���[���P��)
 */
//-----------------------------------------------------------------------------------------
static u8 CalcScreenTopOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  u8 top;

  // �f�t�H���g�̈ʒu
  top = (TOPIC_BUTTON_Y + TOPIC_BUTTON_HEIGHT) * topicID;

  return top;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̍���x���W���擾���� (�h�b�g�P��)
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵���������ڂ̍���x���W (�h�b�g�P��)
 */
//-----------------------------------------------------------------------------------------
static int CalcDisplayLeftOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int left;

  // �f�t�H���g�̈ʒu���Z�o
  left = CalcScreenLeftOfTopicButton( work, topicID ) * DOT_PER_CHARA;

  // BG�X�N���[���̕����l������
  left -= GFL_BG_GetScreenScrollX( MAIN_BG_WINDOW );

  return left;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̍���y���W���擾���� (�h�b�g�P��)
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵���������ڂ̍���y���W (�h�b�g�P��)
 */
//-----------------------------------------------------------------------------------------
static int CalcDisplayTopOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int top;

  // �f�t�H���g�̈ʒu���Z�o
  top = CalcScreenTopOfTopicButton( work, topicID ) * DOT_PER_CHARA;

  // BG�X�N���[���̕����l������
  top -= GetScrollValue();

  return top;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̉E��y���W���擾���� (�h�b�g�P��)
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �w�肵���������ڂ̍���y���W (�h�b�g�P��)
 */
//-----------------------------------------------------------------------------------------
static int CalcDisplayBottomOfTopicButton( const RESEARCH_SELECT_WORK* work, u8 topicID )
{
  int bottom;

  // �f�t�H���g�̈ʒu���Z�o
  bottom = CalcScreenTopOfTopicButton( work, topicID ) * DOT_PER_CHARA + TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA - 1;

  // BG�X�N���[���̕����l������
  bottom -= GetScrollValue();

  return bottom;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃ{�^���̃X�N���[����荞�݂��B�����߂̃E�B���h�E���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicButtonMask( const RESEARCH_SELECT_WORK* work )
{
  int top, bottom;
  BOOL wndEnable = FALSE;
  int wndLeft, wndRight, wndTop, wndBottom;

  // �������ڃ{�^���̏㉺���[�̃f�B�X�v���C���W���擾
  top = CalcDisplayTopOfTopicButton( work, 0 );
  bottom = CalcDisplayBottomOfTopicButton( work, TOPIC_ID_MAX );

  // �E�B���h�E�̃p�����[�^������
  if( 0 < top ) { // ��ʏ㕔�ɉ�荞�݂��N���Ă���
    wndEnable = TRUE;
    wndLeft   = 0;
    wndRight  = 240;
    wndTop    = 0;
    wndBottom = top - 1;
  }
  else if( bottom < DISP_DOT_HEIGHT ) { // ��ʉ����ɉ�肱�݂��N���Ă���
    wndEnable = TRUE;
    wndLeft   = 0;
    wndRight  = 240;
    wndTop    = bottom + 1;
    wndBottom = DISP_DOT_HEIGHT;
  }
  else {
  }

  // �E�B���h�E��ݒ�
  if( wndEnable ) {
    GX_SetVisibleWnd( GX_WNDMASK_W0 ); // �E�B���h�E��L����
    G2_SetWnd0Position( wndLeft, wndTop, wndRight, wndBottom ); // �E�B���h�E�̍��W��ݒ�
    G2_SetWndOutsidePlane( 
        GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
        GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
        TRUE ); // �E�B���h�E�̊O����ݒ�
    G2_SetWnd0InsidePlane( 
        GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_OBJ,
        TRUE ); // �E�B���h�E�̓�����ݒ�
  }
  else {
    GX_SetVisibleWnd( GX_WNDMASK_NONE ); //�E�B���h�E�𖳌���
  }
}




//-----------------------------------------------------------------------------------------
/**
 * @breif �^�b�`�̈�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
  for( idx=0; idx < SCROLL_TOUCH_AREA_NUM; idx++ )
  {
    work->scrollTouchHitTable[ idx ].rect.left   = ScrollTouchAreaInitData[ idx ].left;
    work->scrollTouchHitTable[ idx ].rect.right  = ScrollTouchAreaInitData[ idx ].right;
    work->scrollTouchHitTable[ idx ].rect.top    = ScrollTouchAreaInitData[ idx ].top;
    work->scrollTouchHitTable[ idx ].rect.bottom = ScrollTouchAreaInitData[ idx ].bottom;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create touch hit table\n" );
}




//=========================================================================================
// ����ʏI������
//=========================================================================================


//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_SELECT_WORK* work )
{
  // ������
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init seq queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL );

  // �쐬
  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create seq queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->seqQueue );

  // �폜
  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete seq queue\n" );
}


//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitFont( RESEARCH_SELECT_WORK* work )
{
  // ������
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init font\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateFont( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, FALSE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create font\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteFont( RESEARCH_SELECT_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete font\n" );
}


//=========================================================================================
// �����b�Z�[�W
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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


//=========================================================================================
// ��BG
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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


//=========================================================================================
// ������ �E�B���h�EBG��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSubBG_WINDOW( RESEARCH_SELECT_WORK* work )
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

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup SUB-BG-WINDOW\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up SUB-BG-WINDOW\n" );
}


//=========================================================================================
// ������ �t�H���gBG��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RESEARCH_SELECT_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup SUB-BG-FONT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up SUB-BG-FONT\n" );
}


//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_BAR( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-BAR\n" );
}

//=========================================================================================
// ������� �E�B���h�EBG��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_WINDOW( RESEARCH_SELECT_WORK* work )
{
  ARCHANDLE* handle;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, work->heapID ); 

  // �X�N���[���f�[�^��ǂݍ���
  {
    void* src;
    ARCDATID datID; 
    NNSG2dScreenData* data;
    int sx, sy;

    sx    = 32;
    sy    = GetSelectableTopicNum( work ) * TOPIC_BUTTON_HEIGHT;
    datID = NARC_research_radar_graphic_bgd_searchbtn_NSCR;
    src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID ); 

    NNS_G2dGetUnpackedScreenData( src, &data );
    GFL_BG_WriteScreen( MAIN_BG_WINDOW, data->rawData, 0, 0, sx, sy );
    GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
    GFL_HEAP_FreeMemory( src );
  }

  // �n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup MAIN-BG-WINDOW\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_SELECT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-WINDOW\n" );
}


//=========================================================================================
// ������� �t�H���gBG��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup MAIN-BG-FONT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RESEARCH_SELECT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up MAIN-BG-FONT\n" );
}


//=========================================================================================
// ��������`��I�u�W�F�N�g
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
   for( i=0; i < GetSelectableTopicNum(work); i++ )
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
  for( i=0; i < GetSelectableTopicNum(work); i++ )
  {
    GF_ASSERT( work->TopicsBGFont[i] );
    BG_FONT_Delete( work->TopicsBGFont[i] );
    work->TopicsBGFont[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete BGFonts\n" ); 
} 


//=========================================================================================
// ��OBJ
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e�����쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateClactSystem( RESEARCH_SELECT_WORK* work )
{
  // �V�X�e���쐬
  //GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create clact system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_SELECT_WORK* work )
{ 
  // �V�X�e���j��
  //GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete clact system\n" );
}



//-----------------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitOBJResources( RESEARCH_SELECT_WORK* work )
{
  int i;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ )
  {
    work->objResRegisterIdx[i] = 0; 
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init OBJ resources\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RESEARCH_SELECT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release SUB-OBJ resources\n" );
}


//=========================================================================================
// ��MAIN-OBJ ���\�[�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterMainObjResources( RESEARCH_SELECT_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, palette3, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID );

  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_research_radar_graphic_obj_NCGR, 
                                      FALSE, CLSYS_DRAW_MAIN, heapID ); 
  palette = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_research_radar_graphic_obj_NCLR,
                                       CLSYS_DRAW_MAIN, 
                                       ONE_PALETTE_SIZE*6, 0, 4, 
                                       heapID ); 
  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_research_radar_graphic_obj_NCER,
                                           NARC_research_radar_graphic_obj_NANR,
                                           heapID ); 
  GFL_ARC_CloseDataHandle( arcHandle );

  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, heapID ); 
  palette3 = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_app_menu_common_task_menu_NCLR,
                                       CLSYS_DRAW_MAIN,
                                       ONE_PALETTE_SIZE*4, 0, 2, 
                                       heapID );
  GFL_ARC_CloseDataHandle( arcHandle );

  // �o�^�C���f�b�N�X���L��
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ]  = character;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ]    = palette;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] = cellAnime;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON ] = palette3;


  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: register MAIN-OBJ resources\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RESEARCH_SELECT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release MAIN-OBJ resources\n" );
}


//=========================================================================================
// ���Z���A�N�^�[���j�b�g
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
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


//=========================================================================================
// ���Z���A�N�^�[���[�N
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �f�t�H���g�����������ɂ��, �r�b�g�}�b�v�f�[�^�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forDefault( RESEARCH_SELECT_WORK* work )
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

    // �f�t�H���g�Z�b�g�A�b�v�ΏۂłȂ�
    if( data->defaultSetupFlag == FALSE ) { continue; }

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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup bitmap data for Default\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�����Ă��v�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forOK( RESEARCH_SELECT_WORK* work )
{
  int x, y;
  GFL_BMP_DATA* srcBMP; 
  GFL_MSGDATA* msgData;
  STRBUF* strbuf;
  PRINTSYS_LSB color;
  const BITMAP_INIT_DATA* data;

  // �R�s�[����L�����N�^�ԍ�
  int charaNo[ BMPOAM_ACTOR_OK_CHARA_SIZE_Y ][ BMPOAM_ACTOR_OK_CHARA_SIZE_X ] = 
  {
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {3, 4, 4, 4, 4, 4, 4, 4, 4, 5},
    {6, 7, 7, 7, 7, 7, 7, 7, 7, 8},
  };

  // �������f�[�^�擾
  data = &BitmapInitData[ BMPOAM_ACTOR_OK ]; 

  // �L�����N�^�f�[�^��ǂݍ���
  srcBMP = GFL_BMP_LoadCharacter( data->charaDataArcID, 
                                  data->charaDataArcDatID,
                                  FALSE, 
                                  work->heapID ); 
  // �L�����f�[�^���R�s�[����
  for( y=0; y < BMPOAM_ACTOR_OK_CHARA_SIZE_Y; y++)
  {
    for(x=0; x < BMPOAM_ACTOR_OK_CHARA_SIZE_X; x++)
    {
      GFL_BMP_Print( srcBMP, work->BmpData[ BMPOAM_ACTOR_OK ], 
                     (charaNo[y][x] % 3) * DOT_PER_CHARA, 
                     (charaNo[y][x] / 3) * DOT_PER_CHARA, 
                     x * DOT_PER_CHARA, y * DOT_PER_CHARA, 
                     DOT_PER_CHARA, DOT_PER_CHARA, 0 );
    }
  }
  GFL_BMP_Delete( srcBMP );

  // ���������������
  msgData = work->message[ data->messageIdx ];
  color   = PRINTSYS_LSB_Make( data->colorNo_L, data->colorNo_S, data->colorNo_B );
  strbuf  = GFL_MSG_CreateString( msgData, data->stringID );

  PRINTSYS_PrintColor( work->BmpData[ BMPOAM_ACTOR_OK ],
                       data->stringDrawPosX, data->stringDrawPosY,
                       strbuf, work->font, color ); 

  GFL_HEAP_FreeMemory( strbuf );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup bitmap data for OK\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u��߂�v�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forCANCEL( RESEARCH_SELECT_WORK* work )
{
  int x, y;
  GFL_BMP_DATA* srcBMP; 
  GFL_MSGDATA* msgData;
  STRBUF* strbuf;
  PRINTSYS_LSB color;
  const BITMAP_INIT_DATA* data;

  // �R�s�[����L�����N�^�ԍ�
  int charaNo[ BMPOAM_ACTOR_CANCEL_CHARA_SIZE_Y ][ BMPOAM_ACTOR_CANCEL_CHARA_SIZE_X ] = 
  {
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {3, 4, 4, 4, 4, 4, 4, 4, 4, 5},
    {6, 7, 7, 7, 7, 7, 7, 7, 7, 8},
  };

  // �������f�[�^�擾
  data = &BitmapInitData[ BMPOAM_ACTOR_CANCEL ]; 

  // �L�����N�^�f�[�^��ǂݍ���
  srcBMP = GFL_BMP_LoadCharacter( data->charaDataArcID, 
                                  data->charaDataArcDatID,
                                  FALSE, 
                                  work->heapID ); 
  // �L�����f�[�^���R�s�[����
  for( y=0; y < BMPOAM_ACTOR_CANCEL_CHARA_SIZE_Y; y++)
  {
    for(x=0; x < BMPOAM_ACTOR_CANCEL_CHARA_SIZE_X; x++)
    {
      GFL_BMP_Print( srcBMP, work->BmpData[ BMPOAM_ACTOR_CANCEL ], 
                     (charaNo[y][x] % 3) * DOT_PER_CHARA, 
                     (charaNo[y][x] / 3) * DOT_PER_CHARA, 
                     x * DOT_PER_CHARA, y * DOT_PER_CHARA, 
                     DOT_PER_CHARA, DOT_PER_CHARA, 0 );
    }
  }
  GFL_BMP_Delete( srcBMP );

  // ���������������
  msgData = work->message[ data->messageIdx ];
  color   = PRINTSYS_LSB_Make( data->colorNo_L, data->colorNo_S, data->colorNo_B );
  strbuf  = GFL_MSG_CreateString( msgData, data->stringID );

  PRINTSYS_PrintColor( work->BmpData[ BMPOAM_ACTOR_CANCEL ],
                       data->stringDrawPosX, data->stringDrawPosY,
                       strbuf, work->font, color ); 

  GFL_HEAP_FreeMemory( strbuf );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup bitmap data for CANCEL\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̏������s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void SetupBmpOamSystem( RESEARCH_SELECT_WORK* work )
{
  // BMP-OAM �V�X�e�����쐬
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup BMP-OAM system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RESEARCH_SELECT_WORK* work )
{
  // BMP-OAM �V�X�e����j��
  BmpOam_Exit( work->BmpOamSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up BMP-OAM system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreatePaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL ); // ���d����

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: create palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeletePaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Delete( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: delete palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupPaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Setup( work->paletteAnime[ idx ],
                         PaletteAnimeData[ idx ].destAdrs,
                         PaletteAnimeData[ idx ].srcAdrs,
                         PaletteAnimeData[ idx ].colorNum);
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: clean up palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RESEARCH_SELECT_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       PaletteAnimeData[ index ].animeType,
                       PaletteAnimeData[ index ].fadeColor );
  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: start palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StopPaletteAnime( RESEARCH_SELECT_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: stop palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RESEARCH_SELECT_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Update( work->paletteAnime[ idx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterVBlankTask( RESEARCH_SELECT_WORK* work )
{
  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: register VBlank task\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RESEARCH_SELECT_WORK* work )
{ 
  GFL_TCB_DeleteTask( work->VBlankTask );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: release VBlank task\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RESEARCH_SELECT_WORK* work )
{ 
  // ������
  work->paletteFadeSystem = NULL;

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: init palette fade system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���̌�Еt�����s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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


//=========================================================================================
// ��OBJ �A�N�Z�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X�̓o�^�C���f�b�N�X���擾����
 *
 * @param work
 * @param resID ���\�[�XID
 *
 * @return �w�肵�����\�[�X�̓o�^�C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
static u32 GetObjResourceRegisterIndex( const RESEARCH_SELECT_WORK* work, OBJ_RESOURCE_ID resID )
{
  return work->objResRegisterIdx[ resID ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���j�b�g�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���j�b�g
 */
//-----------------------------------------------------------------------------------------
static GFL_CLUNIT* GetClactUnit( const RESEARCH_SELECT_WORK* work, CLUNIT_INDEX unitIdx )
{
  return work->clactUnit[ unitIdx ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���[�N�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------------------
static GFL_CLWK* GetClactWork( const RESEARCH_SELECT_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[
 * 
 * @param work
 * @param menuItem ���j���[���ڂ̃C���f�b�N�X
 *
 * @return �w�肵�����j���[���ڂɑΉ�����BMP-OAM �A�N�^�[
 */
//-----------------------------------------------------------------------------------------
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RESEARCH_SELECT_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACTOR_INDEX BmpOamActorIdx;

  // BMP-OAM �A�N�^�[�̃C���f�b�N�X���擾
  BmpOamActorIdx = BmpOamIndexOfMenu[ menuItem ];

  // BMP-OAM �A�N�^�[��Ԃ�
  return work->BmpOamActor[ BmpOamActorIdx ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݒ������̒�������ID���擾����
 *
 * @param work
 *
 * @return ���ݒ������̒�������ID
 */
//-----------------------------------------------------------------------------------------
static u8 GetInvestigatingTopicID( const RESEARCH_SELECT_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;

  save  = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave = SaveData_GetQuestionnaire( save );

  // �Z�[�u�f�[�^����擾
  return QuestionnaireWork_GetInvestigatingQuestion( QSave, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������鍀�ڂ��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateInvestigatingTopicID( const RESEARCH_SELECT_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  u8 topicID;

  // ���ڂ�I�����Ă��Ȃ�
  GF_ASSERT( IsTopicIDSelected(work) );

  topicID = GetSelectedTopicID( work ); // �I��������������ID
  save    = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave   = SaveData_GetQuestionnaire( save );
  
  // �Z�[�u�f�[�^���X�V
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question1_topic[ topicID ], 0 );
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question2_topic[ topicID ], 1 );
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question3_topic[ topicID ], 2 );
}


//=========================================================================================
// ��OBJ �\��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[�̂܂ݕ����̈ʒu���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateScrollControlPos( const RESEARCH_SELECT_WORK* work )
{
  GFL_CLWK* clactWork;

  // �Z���A�N�^�[���[�N���擾
  clactWork  = GetClactWork( work, CLWK_SCROLL_CONTROL ); 

  // �\�����Ȃ�
  if( CheckScrollControlCan( work ) == FALSE ) 
  {
    GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );
  }
  // �\������
  else { 
    GFL_CLACTPOS pos;
    u16 setSurface;

    // �\���ʒu���Z�o
    pos.x = SCROLL_CONTROL_LEFT;
    pos.y = CalcScrollControlPos_byScrollValue( work );
    setSurface = ClactWorkInitData[ CLWK_SCROLL_CONTROL ].setSurface;

    // �\���ʒu��ύX
    GFL_CLACT_WK_SetPos( clactWork, &pos, setSurface );
    GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );

    OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update scroll control pos ==> %d\n", pos.y );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[�̂܂ݕ����̈ʒu���v�Z����
 *
 * @param work
 *
 * @return �w�肵���X�N���[�������l���狁�߂�, �X�N���[���o�[�܂ݕ�����y���W
 */
//-----------------------------------------------------------------------------------------
static int CalcScrollControlPos_byScrollValue( const RESEARCH_SELECT_WORK* work )
{
  int controlRange;
  int valueRange;
  int value;
  int pos;
  float rate;

  rate = (float)(GetScrollValue() - MIN_SCROLL_VALUE) / (float)(GetMaxScrollValue(work) - MIN_SCROLL_VALUE);
  pos = SCROLL_CONTROL_TOP + (SCROLL_CONTROL_BOTTOM - SCROLL_CONTROL_TOP) * rate;

  if( pos < SCROLL_CONTROL_TOP ) { pos = SCROLL_CONTROL_TOP; } // �ŏ��l�␳
  if( SCROLL_CONTROL_BOTTOM < pos ) { pos = SCROLL_CONTROL_BOTTOM; } // �ő�l�␳

  return pos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[�̂܂ݕ����̕\���ʒu����, �X�N���[���J�[�\���ʒu���Z�o����
 *
 * @param work
 * @param controlPos �܂ݕ����̕\���ʒu
 *
 * @return �X�N���[���J�[�\���̈ʒu
 */
//-----------------------------------------------------------------------------------------
static int CalcScrollCursorPos_byScrollControlPos( const RESEARCH_SELECT_WORK* work, 
                                                   int controlPos )
{
  float rate;
  int min, max;
  int cursorPos;

  if( controlPos < SCROLL_CONTROL_TOP ) { controlPos = SCROLL_CONTROL_TOP; }
  if( SCROLL_CONTROL_BOTTOM < controlPos ) { controlPos = SCROLL_CONTROL_BOTTOM; }

  rate = (float)(controlPos - SCROLL_CONTROL_TOP) / (float)(SCROLL_CONTROL_BOTTOM - SCROLL_CONTROL_TOP);
  min = MIN_SCROLL_VALUE + SCROLL_CURSOR_TOP_MARGIN;
  max = GetMaxScrollValue(work) + SCROLL_CURSOR_TOP_MARGIN;
  cursorPos = min + (max - min) * rate;

  return cursorPos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[�̂܂ݕ����̕\���ʒu���擾����
 *
 * @param work
 *
 * @return �X�N���[���o�[�̂܂ݕ����̕\���ʒu
 */
//-----------------------------------------------------------------------------------------
static int GetScrollControlPos( const RESEARCH_SELECT_WORK* work )
{
  int controlPos;
  GFL_CLWK* clwk;
  GFL_CLACTPOS pos;
  u16 setSurface;

  clwk = GetClactWork( work, CLWK_SCROLL_CONTROL );
  setSurface = ClactWorkInitData[ CLWK_SCROLL_CONTROL ].setSurface;
  GFL_CLACT_WK_GetPos( clwk, &pos, setSurface );
  controlPos = pos.y;

  return controlPos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����J�n����
 *
 * @param work
 * @param startPos �J�n�X�N���[���J�[�\���ʒu
 * @param endPos   �I���X�N���[���J�[�\���ʒu
 * @param frames   �t���[����
 */
//-----------------------------------------------------------------------------------------
static void StartScroll( RESEARCH_SELECT_WORK* work, int startPos, int endPos, int frames )
{
  // �X�N���[���p�����[�^��ݒ�
  work->scrollStartPos   = startPos;
  work->scrollEndPos     = endPos;
  work->scrollFrames     = frames;
  work->scrollFrameCount = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, 
      "RESEARCH-SELECT: start scroll: startPos=%d, endPos=%d, frames=%d\n",
      startPos, endPos, frames);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateScroll( RESEARCH_SELECT_WORK* work )
{
  int maxFrame, nowFrame;
  int startPos, endPos, nowPos;

  GF_ASSERT( work->scrollFrameCount <= work->scrollFrames ); // �s���Ăяo��

  // �t���[�������X�V
  work->scrollFrameCount++;

  // �X�V��̃J�[�\���ʒu���Z�o
  maxFrame = work->scrollFrames;
  nowFrame = work->scrollFrameCount;
  startPos = work->scrollStartPos;
  endPos   = work->scrollEndPos; 
  nowPos   = startPos + (endPos - startPos) * nowFrame / maxFrame;

  // �X�N���[���J�[�\���ʒu���X�V
  SetScrollCursorPosForce( work, nowPos );
}


//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����I���������ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �X�N���[�����I�����Ă����� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------------
static BOOL CheckScrollEnd( RESEARCH_SELECT_WORK* work )
{
  int maxFrame, nowFrame;

  maxFrame = work->scrollFrames;
  nowFrame = work->scrollFrameCount;

  // �I��
  if( maxFrame <= nowFrame ) { return TRUE; }

  // �I�����Ă��Ȃ�
  return FALSE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
 *
 * @param work
 * @param offset �ړ��I�t�Z�b�g�l
 */
//-----------------------------------------------------------------------------------------
static void ShiftScrollCursorPos( RESEARCH_SELECT_WORK* work, int offset )
{
  int now;
  int next;

  // �ړ���̃J�[�\���ʒu������
  now  = work->scrollCursorPos;
  next = now + offset; 
  if( next < MIN_SCROLL_CURSOR_POS ) { next = MIN_SCROLL_CURSOR_POS; } // �ŏ��l�␳
  if( GetMaxScrollCursorPos(work) < next ) { next = GetMaxScrollCursorPos(work); } // �ő�l�␳

  // �J�[�\���ʒu���X�V
  work->scrollCursorPos = next;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: shift scroll cursor ==> %d\n", next );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w�� )
 *
 * @param work
 * @param pos �ݒ肷��J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetScrollCursorPos( RESEARCH_SELECT_WORK* work, int pos )
{
  // ������␳
  if( pos < MIN_SCROLL_CURSOR_POS ) { pos = MIN_SCROLL_CURSOR_POS; } // �ŏ��l�␳
  if( GetMaxScrollCursorPos(work) < pos ) { pos = GetMaxScrollCursorPos(work); } // �ő�l�␳

  // �J�[�\���ʒu���X�V
  work->scrollCursorPos = pos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set scroll cursor ==> %d\n", pos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w��E�͈͌���Ȃ� )
 *
 * @param work
 * @param pos �ݒ肷��J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetScrollCursorPosForce( RESEARCH_SELECT_WORK* work, int pos )
{
  // �J�[�\���ʒu���X�V
  work->scrollCursorPos = pos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: set scroll cursor force ==> %d\n", pos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��␳����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustScrollCursor( RESEARCH_SELECT_WORK* work )
{
  int minPos, maxPos, adjustPos;

  minPos    = GetMinScrollCursorMarginPos(); // �J�[�\���ŏ��ʒu
  maxPos    = GetMaxScrollCursorMarginPos(); // �J�[�\���ő�ʒu
  adjustPos = work->scrollCursorPos;         // �␳��̍��W

  // �X�N���[�������l�ɍ��킹�ĕ␳����
  if( adjustPos < minPos ) { adjustPos = minPos; } // �ŏ��l�␳
  if( maxPos < adjustPos ) { adjustPos = maxPos; } // �ő�l�␳

  // �X�N���[���J�[�\���ʒu���X�V
  work->scrollCursorPos = adjustPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: adjust scroll cursor ==> %d\n", adjustPos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������l���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateScrollValue( const RESEARCH_SELECT_WORK* work )
{
  int nowPos, minPos, maxPos;
  int value;

  nowPos = work->scrollCursorPos;         // �J�[�\�����݈ʒu
  minPos = GetMinScrollCursorMarginPos(); // �J�[�\���]���͈͂̍ŏ��l
  maxPos = GetMaxScrollCursorMarginPos(); // �J�[�\���]���͈͂̍ő�l

  // �J�[�\�����]���͈͊O�ɂ�������X�V����
  if( nowPos < minPos ) {
    value = nowPos - SCROLL_CURSOR_TOP_MARGIN;
    GFL_BG_SetScrollReq( MAIN_BG_WINDOW, GFL_BG_SCROLL_Y_SET, value );
    GFL_BG_SetScrollReq( MAIN_BG_FONT,   GFL_BG_SCROLL_Y_SET, value );
  }
  else if( maxPos < nowPos ) {
    value = nowPos + SCROLL_CURSOR_BOTTOM_MARGIN - DISP_DOT_HEIGHT;
    GFL_BG_SetScrollReq( MAIN_BG_WINDOW, GFL_BG_SCROLL_Y_SET, value );
    GFL_BG_SetScrollReq( MAIN_BG_FONT,   GFL_BG_SCROLL_Y_SET, value );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������l���X�N���[���J�[�\���ʒu�ɍ��킹�čX�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustScrollValue( const RESEARCH_SELECT_WORK* work )
{
  int value;

  // �X�N���[�������l���v�Z
  value = work->scrollCursorPos - SCROLL_CURSOR_TOP_MARGIN;

  // �X�N���[�������l��␳
  if( value < MIN_SCROLL_VALUE ) { value = MIN_SCROLL_VALUE; } // �ŏ��l�␳
  if( GetMaxScrollValue(work) < value ) { value = GetMaxScrollValue(work); } // �ő�l�␳

  // �X�N���[�������l���X�V
  GFL_BG_SetScroll( MAIN_BG_WINDOW, GFL_BG_SCROLL_Y_SET, value );
  GFL_BG_SetScroll( MAIN_BG_FONT,   GFL_BG_SCROLL_Y_SET, value );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: adjust scroll value ==> %d\n", value );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������l���擾����
 *
 * @return ���݂̃X�N���[�������l
 */
//-----------------------------------------------------------------------------------------
static int GetScrollValue()
{
  return GFL_BG_GetScrollY( MAIN_BG_WINDOW );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���]���͈͂̍ŏ��l���擾����
 *
 * @return �X�N���[���J�[�\���]���ړ��͈͂̍ŏ��l
 */
//-----------------------------------------------------------------------------------------
static int GetMinScrollCursorMarginPos()
{
  return (GetScrollValue() + SCROLL_CURSOR_TOP_MARGIN);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���]���͈͂̍ő�l���擾����
 *
 * @return �X�N���[���J�[�\���]���ړ��͈͂̍ő�l
 */
//-----------------------------------------------------------------------------------------
static int GetMaxScrollCursorMarginPos()
{
  return (GetScrollValue() + DISP_DOT_HEIGHT - SCROLL_CURSOR_BOTTOM_MARGIN);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���X�N���[���J�[�\���ʒu�ɂ���{�^���̒�������ID���v�Z����
 *
 * @param pos �X�N���[���J�[�\���ʒu
 *
 * @param �w�肵���X�N���[���J�[�\���ʒu�ɊY������{�^���̒�������ID
 */
//-----------------------------------------------------------------------------------------
static int CalcTopicID_byScrollCursorPos( int pos )
{
  return (((TOPIC_BUTTON_Y * DOT_PER_CHARA) + pos) / (TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA));
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
 *
 * @param pos 
 *
 * @param �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static int CalcScrollCursorPosOfTopicButtonTop( int topicID )
{
  return (TOPIC_BUTTON_Y * DOT_PER_CHARA) + (topicID * TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
 *
 * @param pos 
 *
 * @param �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static int CalcScrollCursorPosOfTopicButtonBottom( int topicID )
{
  return CalcScrollCursorPosOfTopicButtonTop(topicID) + TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA - 1; 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �ő�X�N���[���l���擾����
 *
 * @param work
 *
 * @return �ő�X�N���[���l
 */
//-----------------------------------------------------------------------------------------
static int GetMaxScrollValue( const RESEARCH_SELECT_WORK* work )
{
  int pos;

  pos = 
    GetSelectableTopicNum( work ) * TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA 
    + SCROLL_CURSOR_BOTTOM_MARGIN 
    - DISP_DOT_HEIGHT;

  return pos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���̍ő�l���擾����
 *
 * @param work
 *
 * @return �X�N���[���J�[�\���̍ő�l
 */
//-----------------------------------------------------------------------------------------
static int GetMaxScrollCursorPos( const RESEARCH_SELECT_WORK* work )
{
  int pos;

  pos = GetMaxScrollValue( work ) + DISP_DOT_HEIGHT - SCROLL_CURSOR_BOTTOM_MARGIN;

  return pos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����삪�\���ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �X�N���[�����삪�\�Ȃ� TRUE
 *         �����łȂ��Ȃ� FALSE
 */
//-----------------------------------------------------------------------------------------
static BOOL CheckScrollControlCan( const RESEARCH_SELECT_WORK* work )
{
  int height;
  int min;

  // ���ڃ{�^���̍��� ( �h�b�g�P�� ) ���Z�o
  height = GetSelectableTopicNum( work ) * TOPIC_BUTTON_HEIGHT * DOT_PER_CHARA;
  min    = DISP_DOT_HEIGHT - SCROLL_CURSOR_TOP_MARGIN - SCROLL_CURSOR_BOTTOM_MARGIN;

  // ���ڃ{�^�������ʂɎ��܂�Ȃ��ꍇ�ɂ̂�, �X�N���[�����삪�\
  if( min < height ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̒�������ID���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return ���̒�������ID ( �[�̓��[�v���� )
 */
//-----------------------------------------------------------------------------------------
static int GetNextTopicID( const RESEARCH_SELECT_WORK* work, int topicID )
{
  return (topicID + 1) % GetSelectableTopicNum( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �O�̒�������ID���擾����
 *
 * @param work
 * @param topicID ��������ID
 *
 * @return �O�̒�������ID ( �[�̓��[�v���� )
 */
//-----------------------------------------------------------------------------------------
static int GetPrevTopicID( const RESEARCH_SELECT_WORK* work, int topicID )
{
  return (topicID - 1 + GetSelectableTopicNum(work)) % GetSelectableTopicNum(work);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڑI���A�C�R���̕\����Ԃ��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicSelectIcon( const RESEARCH_SELECT_WORK* work )
{
  int topicID;
  GFL_CLWK* clactWork;

  topicID   = GetInvestigatingTopicID( work );          // �������̍���ID
  clactWork = GetClactWork( work, CLWK_SELECT_ICON_0 ); // �A�C�R���̃Z���A�N�^�[���[�N

  // �������̍��ڂ�����
  if( topicID == INVESTIGATING_QUESTION_NULL ) {
    GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE ); // ��\��
  }
  else {
    GFL_CLACTPOS pos;
    u16 setSurface;

    // �\����Ԃ��X�V
    pos.x      = CalcDisplayLeftOfTopicButton( work, topicID ) + SELECT_ICON_DRAW_OFFSET_X;
    pos.y      = CalcDisplayTopOfTopicButton( work, topicID ) + SELECT_ICON_DRAW_OFFSET_Y;
    setSurface = ClactWorkInitData[ CLWK_SELECT_ICON_0 ].setSurface;
    GFL_CLACT_WK_SetPos( clactWork, &pos, setSurface );
    GFL_CLACT_WK_SetAutoAnmFlag( clactWork, TRUE );
    GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: update topic select icon\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�C�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �p���b�g�t�F�[�h���������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------------
static BOOL IsPaletteFadeEnd( RESEARCH_SELECT_WORK* work )
{
  return (PaletteFadeCheck( work->paletteFadeSystem ) == 0);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �̕\����Ԃ�ݒ肷��
 *
 * @param work
 * @param actorIdx �ݒ�Ώۂ�BMP-OAM���w��
 * @param enable   �\�����邩�ǂ���
 */
//-----------------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------------
/**
 * @brief �ʐM�A�C�R�����Z�b�g�A�b�v����
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupWirelessIcon( const RESEARCH_SELECT_WORK* work )
{
  GFL_NET_ChangeIconPosition( WIRELESS_ICON_X, WIRELESS_ICON_Y );
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, work->heapID );
  GFL_NET_ReloadIcon();
}

//=========================================================================================
// ��DEBUG:
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
    case RESEARCH_SELECT_SEQ_SETUP:            OS_TFPrintf( PRINT_TARGET, "SETUP " );            break;
    case RESEARCH_SELECT_SEQ_STANDBY:          OS_TFPrintf( PRINT_TARGET, "STANDBY " );          break;
    case RESEARCH_SELECT_SEQ_KEY_WAIT:         OS_TFPrintf( PRINT_TARGET, "KEY_WAIT " );         break;
    case RESEARCH_SELECT_SEQ_SCROLL_WAIT:      OS_TFPrintf( PRINT_TARGET, "SCROLL_WAIT " );      break;
    case RESEARCH_SELECT_SEQ_SCROLL_CONTROL:   OS_TFPrintf( PRINT_TARGET, "SCROLL_CONTROL " );   break;
    case RESEARCH_SELECT_SEQ_CONFIRM:          OS_TFPrintf( PRINT_TARGET, "CONFIRM " );          break;
    case RESEARCH_SELECT_SEQ_DETERMINE:        OS_TFPrintf( PRINT_TARGET, "DETERMINE " );        break;
    case RESEARCH_SELECT_SEQ_FADE_IN:          OS_TFPrintf( PRINT_TARGET, "FADE_IN " );          break;
    case RESEARCH_SELECT_SEQ_FADE_OUT:         OS_TFPrintf( PRINT_TARGET, "FADE_OUT " );         break;
    case RESEARCH_SELECT_SEQ_FRAME_WAIT:       OS_TFPrintf( PRINT_TARGET, "FRAME_WAIT " );       break;
    case RESEARCH_SELECT_SEQ_SCROLL_RESET:     OS_TFPrintf( PRINT_TARGET, "SCROLL_RESET " );     break;
    case RESEARCH_SELECT_SEQ_PALETTE_RESET:    OS_TFPrintf( PRINT_TARGET, "PALETTE_RESET " );    break;
    case RESEARCH_SELECT_SEQ_CLEAN_UP:         OS_TFPrintf( PRINT_TARGET, "CLEAN_UP " );         break;
    case RESEARCH_SELECT_SEQ_FINISH:           OS_TFPrintf( PRINT_TARGET, "FINISH " );           break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 
