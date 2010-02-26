////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F���
 * @file   research_check.c
 * @author obata
 * @date   2010.02.20
 */
////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "queue.h"
#include "bg_font.h"
#include "circle_graph.h"
#include "research_data.h"
#include "research_check.h"
#include "research_check_index.h"
#include "research_check_def.h"
#include "research_check_data.cdat"
#include "research_common.h"

#include "system/gfl_use.h"              // for GFUser_xxxx
#include "system/palanm.h"               // for PaletteFadeXxxx
#include "system/bmp_oam.h"              // for BmpOam_xxxx
#include "gamesystem/gamesystem.h"       // for GAMESYS_WORK
#include "gamesystem/game_beacon.h"      // for GAMEBEACON_xxxx
#include "savedata/save_control.h"       // for SaveControl_xxxx
#include "savedata/questionnaire_save.h" // for QuestionnareWork_xxxx
#include "sound/pm_sndsys.h"             // for PMSND_xxxx
#include "sound/wb_sound_data.sadl"      // for SEQ_SE_XXXX
#include "print/gf_font.h"               // for GFL_FONT_xxxx
#include "print/printsys.h"              // for PRINTSYS_xxxx
#include "print/wordset.h"               // for WORDSET_xxxx

#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_research_radar_xxxx
#include "arc/font/font.naix"               // for NARC_font_xxxx
#include "arc/message.naix"                 // for NARC_message_xxxx
#include "msg/msg_research_radar.h"         // for str_xxxx
#include "msg/msg_questionnaire.h"          // for str_xxxx
#include "obj_NANR_LBLDEFS.h"               // for NANR_obj_xxxx
#include "topic_id.h"                       // for TOPIC_ID_xxxx
#include "question_id.h"                    // for QUESTION_ID_xxxx
#include "answer_id.h"                      // for ANSWER_ID_xxxx

#include "answer_id_question.cdat"         // for AnswerID_question[17][]
#include "answer_num_question.cdat"        // for AnswerNum_question[]
#include "question_id_topic.cdat"          // for QuestionX_topic[]
#include "string_id_answer.cdat"           // for StringID_answer[]
#include "string_id_question.cdat"         // for StringID_question[]
#include "string_id_topic_title.cdat"      // for StringID_topicTitle[]
#include "string_id_topic_caption.cdat"    // for StringID_topicCaption[]
#include "caption_string_id_question.cdat" // for CaptionStringID_question[]
#include "color_r_answer.cdat"             // for ColorR_answer[]
#include "color_g_answer.cdat"             // for ColorG_answer[]
#include "color_b_answer.cdat"             // for ColorB_answer[]



//==============================================================================================
// �������񍐊m�F��� ���[�N
//==============================================================================================
struct _RESEARCH_CHECK_WORK
{ 
  RESEARCH_COMMON_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID                heapID;
  GAMESYS_WORK*         gameSystem;
  GAMEDATA*             gameData;

  GFL_FONT*    font;                   // �t�H���g
  GFL_MSGDATA* message[ MESSAGE_NUM ]; // ���b�Z�[�W
  WORDSET*     wordset;                // ���[�h�Z�b�g

  QUEUE*                seqQueue;      // �V�[�P���X�L���[
  RESEARCH_CHECK_SEQ    seq;           // ���݂̏����V�[�P���X
  BOOL                  seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  u32                   seqCount;      // �V�[�P���X�J�E���^
  RESEARCH_CHECK_RESULT result;        // ��ʏI������

  MENU_ITEM      cursorPos;     // �J�[�\���ʒu
  RESEARCH_DATA  researchData;  // �����f�[�^
  BOOL           analyzeFlag;   // ��͂��ς�ł��邩�ǂ���
  u8             questionIdx;   // �\�����̎���C���f�b�N�X
  u8             answerIdx;     // �I�𒆂̉񓚃C���f�b�N�X
  DATA_DISP_TYPE dispType;      // ���� or ���v �̂ǂ���̃f�[�^��\������̂�

  // �~�O���t
  CIRCLE_GRAPH* mainGraph[ DATA_DISP_TYPE_NUM ]; // �ʏ펞�ɕ\������O���t
  CIRCLE_GRAPH* subGraph [ DATA_DISP_TYPE_NUM ]; // �X�V���ɕ\������O���t

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL touchHitTable[ TOUCH_AREA_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont[ BG_FONT_NUM ];

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

  // VBlank
  GFL_TCBSYS* VBlankTCBSystem; // VBlank���Ԓ��̃^�X�N�Ǘ��V�X�e��
  GFL_TCB*    VBlankTask;      // VBlank�^�C�~���O���ɍs���^�X�N
};


//----------------------------------------------------------------------------------------------
// ��LAYER 4 �V�[�P���X����
//----------------------------------------------------------------------------------------------
// �V�[�P���X����
static void Main_SETUP   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void Main_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void Main_ANALYZE ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void Main_FLASH   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH
static void Main_UPDATE  ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void Main_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void Main_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP

// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSequence( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq ); // ���̃V�[�P���X��o�^����
static void FinishCurrentSequence( RESEARCH_CHECK_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSequence( RESEARCH_CHECK_WORK* work ); // �����V�[�P���X��ύX����
static void SetSequence( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_RESULT result ); // ��ʏI�����ʂ�ݒ肷��

// �V�[�P���X����������
static void InitSequence_SETUP   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void InitSequence_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void InitSequence_ANALYZE ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void InitSequence_FLASH   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH
static void InitSequence_UPDATE  ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void InitSequence_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void InitSequence_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP

// �V�[�P���X�I������
static void FinishSequence_SETUP   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void FinishSequence_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void FinishSequence_ANALYZE ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void FinishSequence_FLASH   ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH
static void FinishSequence_UPDATE  ( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void FinishSequence_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void FinishSequence_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP

// �~�O���t ���C������
static void UpdateCircleGraphs( RESEARCH_CHECK_WORK* work ); // ���ׂẲ~�O���t���X�V����

// VBlank�^�X�N
static void VBlankFunc( GFL_TCB* tcb, void* wk );  // VBlank���̏���

//----------------------------------------------------------------------------------------------
// ��LAYER 3 �@�\
//---------------------------------------------------------------------------------------------- 
// �J�[�\��
static void MoveMenuCursorUp  ( RESEARCH_CHECK_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RESEARCH_CHECK_WORK* work ); // ���ֈړ�����

// �\�����鎿��
static void ChangeQuestionToNext( RESEARCH_CHECK_WORK* work ); // ���̎���ɕύX����
static void ChangeQuestionToPrev( RESEARCH_CHECK_WORK* work ); // �O�̎���ɕύX����

// �\�������
static void ChangeAnswerToNext( RESEARCH_CHECK_WORK* work ); // ���̉񓚂ɕύX����
static void ChangeAnswerToPrev( RESEARCH_CHECK_WORK* work ); // �O�̉񓚂ɕύX����
static void ChangeAnswerToTop ( RESEARCH_CHECK_WORK* work ); // �擪�̉񓚂ɕύX����

// �����f�[�^�̕\���^�C�v
static void SwitchDataDisplayType( RESEARCH_CHECK_WORK* work ); // �����f�[�^�̕\���^�C�v��؂�ւ���

//----------------------------------------------------------------------------------------------
// ��LAYER 2 �ʑ���
//---------------------------------------------------------------------------------------------- 
// �J�[�\��
static void ShiftMenuCursorPos( RESEARCH_CHECK_WORK* work, int stride ); // �J�[�\���ʒu��ύX����

// �\�����鎿��C���f�b�N�X
static void ShiftQuestionIdx( RESEARCH_CHECK_WORK* work, int stride ); // �\�����鎿��̃C���f�b�N�X��ύX����

// �I�𒆂̉񓚃C���f�b�N�X
static void ShiftAnswerIdx( RESEARCH_CHECK_WORK* work, int stride ); // �I�𒆂̉񓚃C���f�b�N�X��ύX����
static void ResetAnswerIdx( RESEARCH_CHECK_WORK* work );             // �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����

// �����f�[�^�\���^�C�v
static void SetDataDisplayType( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // �����f�[�^�̕\���^�C�v��ݒ肷��

// ���j���[���ڂ̕\��
static void SetMenuCursorOn ( RESEARCH_CHECK_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuCursorOff( RESEARCH_CHECK_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���

// �^�b�`�͈�
static void UpdateTouchArea( RESEARCH_CHECK_WORK* work ); // �^�b�`�͈͂��X�V����

// BG
static void UpdateMainBG_WINDOW( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���X�V����

// BGFont
static void UpdateBGFont_TopicCaption   ( RESEARCH_CHECK_WORK* work ); // �������ڂ̕⑫�����X�V����
static void UpdateBGFont_QuestionCaption( RESEARCH_CHECK_WORK* work ); // ����̕⑫�����X�V����
static void UpdateBGFont_Question       ( RESEARCH_CHECK_WORK* work ); // ������X�V���� 
static void UpdateBGFont_Answer         ( RESEARCH_CHECK_WORK* work ); // �񓚂��X�V����
static void UpdateBGFont_MyAnswer       ( RESEARCH_CHECK_WORK* work ); // �����̉񓚂��X�V����
static void UpdateBGFont_Count          ( RESEARCH_CHECK_WORK* work ); // �񓚐l�����X�V����
static void UpdateBGFont_NoData         ( RESEARCH_CHECK_WORK* work ); //�u�������� ���傤�����イ�v�̕\�����X�V����
static void UpdateBGFont_DataReceiving  ( RESEARCH_CHECK_WORK* work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����

// OBJ�̕\��
static void UpdateControlCursor( RESEARCH_CHECK_WORK* work ); // ����J�[�\���̕\�����X�V����

// BMP-OAM
static void BmpOamSetDrawEnable( RESEARCH_CHECK_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable );  // �\�����邩�ǂ�����ݒ肷��

// �p���b�g�t�F�[�h
static void StartPaletteFadeOut( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�A�E�g���J�n����
static void StartPaletteFadeIn ( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�C�����J�n����
static BOOL IsPaletteFadeEnd   ( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��

// �~�O���t
static void SetupMainCircleGraph( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // ���C���~�O���t�����݂̒����f�[�^�ō\������
static void SetupSubCircleGraph ( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // �T�u�~�O���t�����݂̒����f�[�^�ō\������
static void InterchangeCircleGraph( RESEARCH_CHECK_WORK* work ); // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���

//----------------------------------------------------------------------------------------------
// ��LAYER 1 �f�[�^�A�N�Z�X
//----------------------------------------------------------------------------------------------
// �����f�[�^
static u8 GetTopicID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̒�������ID
static u8 GetQuestionID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ID
static u8 GetAnswerNum( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���񓚑I�����̐�
static u16 GetAnswerID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉�ID
static u16 GetCountOfQuestion     ( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfQuestion( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfQuestion( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
static u16 GetCountOfAnswer     ( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfAnswer( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfAnswer( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��

// �~�O���t
static CIRCLE_GRAPH* GetMainGraph( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̃��C���~�O���t
static CIRCLE_GRAPH* GetSubGraph ( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̃T�u�~�O���t

// OBJ
static u32 GetObjResourceRegisterIndex( const RESEARCH_CHECK_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetClactUnit( const RESEARCH_CHECK_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g
static GFL_CLWK* GetClactWork( const RESEARCH_CHECK_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N

// BMP-OAM
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RESEARCH_CHECK_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[

//----------------------------------------------------------------------------------------------
// ��LAYER 0 ����������/�I������
//----------------------------------------------------------------------------------------------
// ��ʂ̏���/��Еt�� ( BG )
static void SetupBG  ( RESEARCH_CHECK_WORK* work ); // BG ����
static void CleanUpBG( RESEARCH_CHECK_WORK* work ); // BG ��Еt��
static void SetupSubBG_WINDOW  ( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ����
static void CleanUpSubBG_WINDOW( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ��Еt��
static void SetupSubBG_FONT  ( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �t�H���g�� ) ����
static void CleanUpSubBG_FONT( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �t�H���g�� ) ��Еt��
static void SetupMainBG_WINDOW  ( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ����
static void CleanUpMainBG_WINDOW( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ��Еt��
static void SetupMainBG_FONT  ( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �t�H���g�� ) ����
static void CleanUpMainBG_FONT( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �t�H���g�� ) ��Еt��

// ��ʂ̏���/��Еt�� ( OBJ )
static void CreateClactSystem( RESEARCH_CHECK_WORK* work ); // OBJ �V�X�e�� ����
static void DeleteClactSystem( RESEARCH_CHECK_WORK* work ); // OBJ �V�X�e�� �j��
static void RegisterSubObjResources( RESEARCH_CHECK_WORK* work ); // SUB-OBJ ���\�[�X �o�^
static void ReleaseSubObjResources ( RESEARCH_CHECK_WORK* work ); // SUB-OBJ ���\�[�X ���
static void RegisterMainObjResources( RESEARCH_CHECK_WORK* work ); // MAIN-OBJ ���\�[�X �o�^
static void ReleaseMainObjResources ( RESEARCH_CHECK_WORK* work ); // MAIN-OBJ ���\�[�X ���
static void InitClactUnits  ( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g ������
static void CreateClactUnits( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g ����
static void DeleteClactUnits( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g �j��
static void InitClactWorks  ( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N ������
static void CreateClactWorks( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N ����
static void DeleteClactWorks( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N �j��

// BMP-OAM �̏���/��Еt��
static void InitBitmapDatas  ( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^ ������
static void CreateBitmapDatas( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^ �쐬
static void SetupBitmapDatas ( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^ ����
static void DeleteBitmapDatas( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^ �j��
static void SetupBmpOamSystem  ( RESEARCH_CHECK_WORK* work ); // BMP-OAM �V�X�e�� ����
static void CleanUpBmpOamSystem( RESEARCH_CHECK_WORK* work ); // BMP-OAM �V�X�e�� ��Еt��
static void CreateBmpOamActors( RESEARCH_CHECK_WORK* work ); // BMP-OAM �A�N�^�[ �쐬
static void DeleteBmpOamActors( RESEARCH_CHECK_WORK* work ); // BMP-OAM �A�N�^�[ �j��

// 3D ����
static void Setup3D();

// VBlank�^�X�N�̓o�^/����
static void RegisterVBlankTask( RESEARCH_CHECK_WORK* work );
static void ReleaseVBlankTask ( RESEARCH_CHECK_WORK* work );

// �p���b�g�t�F�[�h�V�X�e���̏���/��Еt��
static void InitPaletteFadeSystem   ( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ������
static void SetupPaletteFadeSystem  ( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ����
static void CleanUpPaletteFadeSystem( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��

// �f�[�^�̏�����/����/�j��
static void InitSeqQueue  ( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ ������
static void CreateSeqQueue( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ �쐬
static void DeleteSeqQueue( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ �j��
static void InitFont  ( RESEARCH_CHECK_WORK* work ); // �t�H���g ������
static void CreateFont( RESEARCH_CHECK_WORK* work ); // �t�H���g ����
static void DeleteFont( RESEARCH_CHECK_WORK* work ); // �t�H���g �j��
static void InitMessages  ( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W ������
static void CreateMessages( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W ����
static void DeleteMessages( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W �j��
static void InitWordset  ( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g ������
static void CreateWordset( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g ����
static void DeleteWordset( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g �j��
static void InitBGFonts  ( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g ������
static void CreateBGFonts( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g ����
static void DeleteBGFonts( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g �j��
static void InitCircleGraphs ( RESEARCH_CHECK_WORK* work ); // �~�O���t ������
static void CreateCircleGraph( RESEARCH_CHECK_WORK* work ); // �~�O���t ����
static void DeleteCircleGraph( RESEARCH_CHECK_WORK* work ); // �~�O���t �j��
static void InitResearchData( RESEARCH_CHECK_WORK* work ); // �����f�[�^ ������
static void SetupResearchData( RESEARCH_CHECK_WORK* work ); // �����f�[�^ �擾
static void SetupTouchArea( RESEARCH_CHECK_WORK* work ); // �^�b�`�̈� ����

//----------------------------------------------------------------------------------------------
// ��LAYER -1 �f�o�b�O
//----------------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[�̒��g��\������
static void DebugPrint_researchData( const RESEARCH_CHECK_WORK* work ); // �����f�[�^��\������
static void Debug_SetupResearchData( RESEARCH_CHECK_WORK* work ); // �����f�[�^��ݒ肷��


//==============================================================================================
// �������񍐊m�F��� ����֐�
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̐���
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return �����񍐊m�F��ʃ��[�N
 */
//----------------------------------------------------------------------------------------------
RESEARCH_CHECK_WORK* CreateResearchCheckWork( RESEARCH_COMMON_WORK* commonWork )
{
  int i;
  RESEARCH_CHECK_WORK* work;
  HEAPID heapID;

  heapID = RESEARCH_COMMON_GetHeapID( commonWork );

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_CHECK_WORK) );

  // ������
  work->commonWork      = commonWork;
  work->heapID          = heapID;
  work->gameSystem      = RESEARCH_COMMON_GetGameSystem( commonWork );
  work->gameData        = RESEARCH_COMMON_GetGameData( commonWork );
  work->seq             = RESEARCH_CHECK_SEQ_SETUP;
  work->seqFinishFlag   = FALSE;
  work->seqCount        = 0;
  work->result          = RESEARCH_CHECK_RESULT_NONE;
  work->cursorPos       = MENU_ITEM_QUESTION;
  work->analyzeFlag     = FALSE;
  work->questionIdx     = 0;
  work->answerIdx       = 0;
  work->dispType        = DATA_DISP_TYPE_TODAY;
  work->VBlankTCBSystem = GFUser_VIntr_GetTCBSYS();

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitResearchData( work );
  InitSeqQueue( work );
  InitCircleGraphs( work );
  InitMessages( work );
  InitWordset( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitBitmapDatas( work );
  InitPaletteFadeSystem( work );

  CreateSeqQueue( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create work\n" );

  return work;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//----------------------------------------------------------------------------------------------
void DeleteResearchCheckWork( RESEARCH_CHECK_WORK* work )
{
  if( work == NULL )
  {
    GF_ASSERT(0);
    return;
  }
  DeleteSeqQueue( work ); // �V�[�P���X�L���[
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete work\n" );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��� ���C������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
RESEARCH_CHECK_RESULT ResearchCheckMain( RESEARCH_CHECK_WORK* work )
{
  // �V�[�P���X���Ƃ̏���
  switch( work->seq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:     Main_SETUP( work );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:  Main_KEY_WAIT( work );  break;
  case RESEARCH_CHECK_SEQ_ANALYZE:   Main_ANALYZE( work );   break;
  case RESEARCH_CHECK_SEQ_FLASH:     Main_FLASH( work );     break;
  case RESEARCH_CHECK_SEQ_UPDATE:    Main_UPDATE( work );    break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:  Main_FADE_OUT( work );  break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:  Main_CLEAN_UP( work );  break;
  case RESEARCH_CHECK_SEQ_FINISH:    return work->result;
  default: GF_ASSERT(0);
  }

  // �Z���A�N�^�[�V�X�e�� ���C������
  GFL_CLACT_SYS_Main();

  // �~�O���t ���C������
  UpdateCircleGraphs( work );

  // �V�[�P���X�J�E���^�X�V
  CountUpSeqCount( work );

  // �V�[�P���X�X�V
  SwitchSequence( work ); 

  // 3D�`��
  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );

  return RESEARCH_CHECK_RESULT_CONTINUE;
}




//==============================================================================================
// ��LAYER 4 �V�[�P���X����
//==============================================================================================

//----------------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_CHECK_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_SETUP( RESEARCH_CHECK_WORK* work )
{
  CreateFont( work );
  CreateMessages( work );
  CreateWordset( work );
  SetupTouchArea( work );
  UpdateTouchArea( work );

  // 3D �����ݒ�
  Setup3D();

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

  // BMP-OAM ����
  CreateBitmapDatas( work );
  SetupBitmapDatas( work );
  SetupBmpOamSystem( work );
  CreateBmpOamActors( work );

  // �f�[�^���擾
  //SetupResearchData( work );
  Debug_SetupResearchData( work ); // TEST:
  DebugPrint_researchData( work ); // TEST:

  // �~�O���t �쐬
  CreateCircleGraph( work );

  // �p���b�g�t�F�[�h�V�X�e�� ����
  SetupPaletteFadeSystem( work );

  // VBkank�^�X�N�o�^
  RegisterVBlankTask( work );

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  // ���̃V�[�P���X���Z�b�g
  SetNextSequence( work, RESEARCH_CHECK_SEQ_KEY_WAIT ); 

  // �V�[�P���X�I��
  FinishCurrentSequence( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_CHECK_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  int key;
  int trg;
  int touchedAreaIdx;

  key = GFL_UI_KEY_GetCont();
  trg = GFL_UI_KEY_GetTrg(); 
  touchedAreaIdx = GFL_UI_TP_HitTrg( work->touchHitTable );

  // TEST:
  if( trg & PAD_BUTTON_DEBUG )
  {
    SetNextSequence( work, RESEARCH_CHECK_SEQ_UPDATE );
    SetNextSequence( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSequence( work );
    return;
  }

  //--------
  // ���L�[
  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work );
    return;
  }

  //--------
  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work );
    return;
  } 

  //----------------------
  // ���L�[ or ���J�[�\��
  if( (trg & PAD_KEY_LEFT) ||
      (touchedAreaIdx == TOUCH_AREA_CONTROL_CURSOR_L) ) {
    switch( work->cursorPos )
    {
    case MENU_ITEM_QUESTION:  ChangeQuestionToPrev( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToPrev( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
  }

  //----------------------
  // ���L�[ or �E�J�[�\��
  if( (trg & PAD_KEY_RIGHT) ||
      (touchedAreaIdx == TOUCH_AREA_CONTROL_CURSOR_R) ) {
    switch( work->cursorPos )
    {
    case MENU_ITEM_QUESTION:  ChangeQuestionToNext( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToNext( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
  }

  if( touchedAreaIdx == TOUCH_AREA_QUESTION ) {
    if( (work->analyzeFlag == FALSE ) &&
        (GetCountOfQuestion(work) != 0) ) {
      // �V�[�P���X�ύX
      SetNextSequence( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSequence( work, RESEARCH_CHECK_SEQ_FLASH );
      SetNextSequence( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSequence( work );
      return;
    }
  }

  if( trg & PAD_BUTTON_A ) {
    if( (work->analyzeFlag == FALSE ) &&
        (GetCountOfQuestion(work) != 0) && 
        (work->cursorPos == MENU_ITEM_QUESTION)  ) {
      // �V�[�P���X�ύX
      SetNextSequence( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSequence( work, RESEARCH_CHECK_SEQ_FLASH );
      SetNextSequence( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSequence( work );
      return;
    }
  } 

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    // �V�[�P���X�ύX
    SetNextSequence( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }

  //-----------------
  //�u���ǂ�v�{�^��
  if( touchedAreaIdx == TOUCH_AREA_RETURN_BUTTON ) {
    // �V�[�P���X�ύX
    SetNextSequence( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSequence( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSequence( work );
    return;
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��̓V�[�P���X ( RESEARCH_CHECK_SEQ_ANALYZE ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_ANALYZE( RESEARCH_CHECK_WORK* work )
{ 
  // SE ����~���Ă���
  if( PMSND_CheckPlaySE() == FALSE ) { 
    // ��莞�Ԃ��o�߂Ŏ��̃V�[�P���X��
    if( SEQ_ANALYZE_FRAMES <= work->seqCount ) {
      FinishCurrentSequence( work );
    } 
    else { 
      // �f�[�^��͒�SE�����[�v������
      PMSND_PlaySE( SEQ_SE_SYS_81 );
    }
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��ʃt���b�V���V�[�P���X ( RESEARCH_CHECK_SEQ_FLASH ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_FLASH( RESEARCH_CHECK_WORK* work )
{
  // ��莞�Ԃ��o�߂Ŏ��̃V�[�P���X��
  if( SEQ_FLASH_FRAMES <= work->seqCount )
  {
    FinishCurrentSequence( work );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_CHECK_SEQ_UPDATE ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_UPDATE( RESEARCH_CHECK_WORK* work )
{
  // ��莞�Ԃ��o�߂Ŏ��̃V�[�P���X��
  if( SEQ_UPDATE_FRAMES <= work->seqCount )
  {
    FinishCurrentSequence( work );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_CHECK_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Main_FADE_OUT( RESEARCH_CHECK_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSequence( work );
  } 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_CHECK_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//----------------------------------------------------------------------------------------------
static void Main_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{ 
  // VBlank�^�X�N������
  ReleaseVBlankTask( work );

  // �~�O���t �폜
  DeleteCircleGraph( work ); 

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
  CleanUpMainBG_WINDOW( work );
  CleanUpSubBG_FONT   ( work );
  CleanUpSubBG_WINDOW ( work );
  CleanUpBG           ( work );

  DeleteWordset( work );
  DeleteMessages( work );
  DeleteFont( work );

  // ��ʏI�����ʂ�����
  SetResult( work, RESEARCH_CHECK_RESULT_TO_MENU );  

  // �V�[�P���X�I��
  SetNextSequence( work, RESEARCH_CHECK_SEQ_FINISH );
  FinishCurrentSequence( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishCurrentSequence( RESEARCH_CHECK_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish current sequence\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//----------------------------------------------------------------------------------------------
static void SetResult( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_CHECK_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set result (%d)\n", result );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_CHECK_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:    maxCount = 0xffffffff;         break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT: maxCount = 0xffffffff;         break;
  case RESEARCH_CHECK_SEQ_ANALYZE:  maxCount = SEQ_ANALYZE_FRAMES; break;
  case RESEARCH_CHECK_SEQ_FLASH:    maxCount = SEQ_FLASH_FRAMES;   break;
  case RESEARCH_CHECK_SEQ_UPDATE:   maxCount = SEQ_UPDATE_FRAMES;  break;
  case RESEARCH_CHECK_SEQ_FADE_OUT: maxCount = 0xffffffff;         break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP: maxCount = 0xffffffff;         break;
  case RESEARCH_CHECK_SEQ_FINISH:   maxCount = 0xffffffff;         break;
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
static void SetNextSequence( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SwitchSequence( RESEARCH_CHECK_WORK* work )
{
  RESEARCH_CHECK_SEQ nextSeq;

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
static void SetSequence( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:    FinishSequence_SETUP( work );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT: FinishSequence_KEY_WAIT( work );  break;
  case RESEARCH_CHECK_SEQ_ANALYZE:  FinishSequence_ANALYZE( work );   break;
  case RESEARCH_CHECK_SEQ_FLASH:    FinishSequence_FLASH( work );     break;
  case RESEARCH_CHECK_SEQ_UPDATE:   FinishSequence_UPDATE( work );    break;
  case RESEARCH_CHECK_SEQ_FADE_OUT: FinishSequence_FADE_OUT( work );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP: FinishSequence_CLEAN_UP( work );  break;
  case RESEARCH_CHECK_SEQ_FINISH:   break;
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq           = nextSeq;
  work->seqCount      = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:    InitSequence_SETUP( work );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT: InitSequence_KEY_WAIT( work );  break;
  case RESEARCH_CHECK_SEQ_ANALYZE:  InitSequence_ANALYZE( work );   break;
  case RESEARCH_CHECK_SEQ_FLASH:    InitSequence_FLASH( work );     break;
  case RESEARCH_CHECK_SEQ_UPDATE:   InitSequence_UPDATE( work );    break;
  case RESEARCH_CHECK_SEQ_FADE_OUT: InitSequence_FADE_OUT( work );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP: InitSequence_CLEAN_UP( work );  break;
  case RESEARCH_CHECK_SEQ_FINISH:   break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set seq ==> " );
  switch( nextSeq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:    OS_TFPrintf( PRINT_TARGET, "SETUP" );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT: OS_TFPrintf( PRINT_TARGET, "KEY_WAIT" );  break;
  case RESEARCH_CHECK_SEQ_ANALYZE:  OS_TFPrintf( PRINT_TARGET, "ANALYZE" );   break;
  case RESEARCH_CHECK_SEQ_FLASH:    OS_TFPrintf( PRINT_TARGET, "FLASH" );     break;
  case RESEARCH_CHECK_SEQ_UPDATE:   OS_TFPrintf( PRINT_TARGET, "UPDATE" );    break;
  case RESEARCH_CHECK_SEQ_FADE_OUT: OS_TFPrintf( PRINT_TARGET, "FADE_OUT" );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP: OS_TFPrintf( PRINT_TARGET, "CLEAN_UP" );  break;
  case RESEARCH_CHECK_SEQ_FINISH:   OS_TFPrintf( PRINT_TARGET, "FINISH" );    break;
  default:  GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_SETUP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_SETUP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
  UpdateBGFont_Answer( work );        // �񓚂��X�V����

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_ANALYZE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_ANALYZE( RESEARCH_CHECK_WORK* work )
{
  //�u�c�����������イ�c�v��\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, TRUE );

  // �f�[�^���擾
  //SetupResearchData( work );
  Debug_SetupResearchData( work ); // TEST:
  DebugPrint_researchData( work ); // TEST:

  // �~�O���t�쐬
  SetupMainCircleGraph( work, DATA_DISP_TYPE_TODAY );
  SetupMainCircleGraph( work, DATA_DISP_TYPE_TOTAL );

  // �~�O���t�\���J�n
  CIRCLE_GRAPH_SetDrawEnable( GetMainGraph(work), TRUE );
  CIRCLE_GRAPH_AnalyzeReq( GetMainGraph(work) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq ANALYZE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_FLASH )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_FLASH( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq FLASH\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_UPDATE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_UPDATE( RESEARCH_CHECK_WORK* work )
{
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����

  // �����f�[�^���擾
  //SetupResearchData( work );
  Debug_SetupResearchData( work ); // TEST:
  DebugPrint_researchData( work ); // TEST:

  // �T�u�~�O���t�쐬
  SetupSubCircleGraph( work, DATA_DISP_TYPE_TODAY );
  SetupSubCircleGraph( work, DATA_DISP_TYPE_TOTAL );

  // �~�O���t�\���J�n
  CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), TRUE );
  CIRCLE_GRAPH_UpdateReq( GetSubGraph(work) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq UPDATE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_FADE_OUT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_FADE_OUT( RESEARCH_CHECK_WORK* work )
{ 
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq FADE_OUT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_CLEAN_UP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSequence_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_SETUP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_SETUP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq SETUP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_KEY_WAIT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq KEY_WAIT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_ANALYZE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_ANALYZE( RESEARCH_CHECK_WORK* work )
{
  // ��͍ς݃t���O�𗧂Ă�
  work->analyzeFlag = TRUE;

  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, FALSE ); //�u�c�����������イ�c�v������
  UpdateMainBG_WINDOW( work );   // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );       // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_Answer( work );   // �񓚂��X�V����
  UpdateBGFont_MyAnswer( work ); // �����̉񓚂��X�V����
  UpdateBGFont_Count( work );    // �񓚐l�����X�V����

  // �������ʕ\��SE
  PMSND_PlaySE( SEQ_SE_SYS_82 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq ANALYZE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_FLASH )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_FLASH( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq FLASH\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_UPDATE )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_UPDATE( RESEARCH_CHECK_WORK* work )
{
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����

  // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
  InterchangeCircleGraph( work );

  // �T�u�~�O���t ( �����C�� ) �\���I��
  CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq UPDATE\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_FADE_OUT )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_FADE_OUT( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq FADE_OUT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_CLEAN_UP )
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void FinishSequence_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq CLEAN_UP\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ׂẲ~�O���t���X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateCircleGraphs( RESEARCH_CHECK_WORK* work )
{
  int typeIdx;

  for( typeIdx=0; typeIdx < DATA_DISP_TYPE_NUM; typeIdx++ )
  {
    CIRCLE_GRAPH_Main( work->mainGraph[ typeIdx ] );
    CIRCLE_GRAPH_Main( work->subGraph[ typeIdx ] );
  }
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
  RESEARCH_CHECK_WORK* work = (RESEARCH_CHECK_WORK*)wk;

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
static void MoveMenuCursorUp( RESEARCH_CHECK_WORK* work )
{ 
  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, -1 );

  // �J�[�\�����u�񓚁v�̈ʒu�ɂ���ꍇ
  if( work->cursorPos == MENU_ITEM_ANSWER )
  {
    if( work->analyzeFlag == FALSE ) { // �����
      ShiftMenuCursorPos( work, -1 ); 
    }
    else if( GetCountOfQuestion(work) == 0 ) { //�u�������� ���傤�����イ�v
      ShiftMenuCursorPos( work, -1 ); 
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �^�b�`�͈͂��X�V
  UpdateTouchArea( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�������ֈړ�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void MoveMenuCursorDown( RESEARCH_CHECK_WORK* work )
{
  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, 1 );

  // �J�[�\�����u�񓚁v�̈ʒu�ɂ���ꍇ
  if( work->cursorPos == MENU_ITEM_ANSWER )
  {
    if( work->analyzeFlag == FALSE ) { // �����
      ShiftMenuCursorPos( work, 1 ); 
    }
    else if( GetCountOfQuestion(work) == 0 ) { //�u�������� ���傤�����イ�v
      ShiftMenuCursorPos( work, 1 ); 
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �^�b�`�͈͂��X�V
  UpdateTouchArea( work );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿���, ���̎���ɕύX����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------------- 
static void ChangeQuestionToNext( RESEARCH_CHECK_WORK* work )
{
  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
  }

  // �f�[�^���X�V
  ShiftQuestionIdx( work, 1 ); // �\�����鎿��C���f�b�N�X��ύX
  work->analyzeFlag = FALSE;   // ��͍ς݃t���O�𕚂���

  // �\�����X�V
  UpdateMainBG_WINDOW( work );          // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );              // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_QuestionCaption( work ); // ����̕⑫�����X�V����
  UpdateBGFont_Question( work );        // ������X�V���� 
  ChangeAnswerToTop( work );            // �񓚂��X�V����
  UpdateBGFont_MyAnswer( work );        // �����̉񓚂��X�V����
  UpdateBGFont_Count( work );           // �񓚐l�����X�V����
  UpdateBGFont_NoData( work );          //�u�������� ���傤�����イ�v�̕\�����X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿���, �O�̎���ɕύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ChangeQuestionToPrev( RESEARCH_CHECK_WORK* work )
{
  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
  }

  // �f�[�^���X�V
  ShiftQuestionIdx( work, -1 ); // �\�����鎿��C���f�b�N�X��ύX
  work->analyzeFlag = FALSE;    // ��͍ς݃t���O�𕚂���

  // �\�����X�V
  UpdateMainBG_WINDOW( work );          // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );              // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_QuestionCaption( work ); // ����̕⑫�����X�V����
  UpdateBGFont_Question( work );        // ������X�V���� 
  ChangeAnswerToTop( work );            // �񓚂��X�V����
  UpdateBGFont_MyAnswer( work );        // �����̉񓚂��X�V����
  UpdateBGFont_Count( work );           // �񓚐l�����X�V����
  UpdateBGFont_NoData( work );          //�u�������� ���傤�����イ�v�̕\�����X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, ���̉񓚂ɕύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ChangeAnswerToNext( RESEARCH_CHECK_WORK* work )
{
  ShiftAnswerIdx( work, 1 ); // �\������񓚃C���f�b�N�X��ύX

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, �O�̉񓚂ɕύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ChangeAnswerToPrev( RESEARCH_CHECK_WORK* work )
{
  ShiftAnswerIdx( work, -1 ); // �\������񓚃C���f�b�N�X��ύX

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, �擪�̉񓚂ɕύX����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ChangeAnswerToTop( RESEARCH_CHECK_WORK* work )
{
  ResetAnswerIdx( work ); // �\������񓚃C���f�b�N�X�����Z�b�g

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^�̕\���^�C�v��؂�ւ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SwitchDataDisplayType( RESEARCH_CHECK_WORK* work )
{
  DATA_DISP_TYPE nextType;
  BOOL newGraphWait = FALSE; // �V�����\������~�O���t��, �\������܂ł̑҂����Ԃ��K�v���ǂ���

  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
    newGraphWait = TRUE;
  }

  // �ύX��̕\���^�C�v��I��
  switch( work->dispType )
  {
  case DATA_DISP_TYPE_TODAY: nextType = DATA_DISP_TYPE_TOTAL; break;
  case DATA_DISP_TYPE_TOTAL: nextType = DATA_DISP_TYPE_TODAY; break;
  default: GF_ASSERT(0);
  }

  // �\���^�C�v��ύX
  SetDataDisplayType( work, nextType );

  // �\�����X�V
  UpdateMainBG_WINDOW( work ); // MAIN-BG ( �E�B���h�E�� ) ���X�V
  SetMenuCursorOn( work );     // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
  UpdateBGFont_Count( work );  // �񓚐l�����X�V����
  UpdateBGFont_NoData( work ); //�u�������� ���傤�����イ�v�̕\�����X�V����

  // �~�O���t���o��������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    if( newGraphWait ) { CIRCLE_GRAPH_StopGraph( GetMainGraph(work), 20 ); } // �O�O���t�̏�����҂�
    CIRCLE_GRAPH_AppearReq( GetMainGraph(work) );
    CIRCLE_GRAPH_SetDrawEnable( GetMainGraph(work), TRUE );
  }

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
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
static void ShiftMenuCursorPos( RESEARCH_CHECK_WORK* work, int stride )
{
  int nowPos;
  int nextPos;

  // �J�[�\���ʒu���X�V
  nowPos  = work->cursorPos;
  nextPos = (nowPos + stride + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  work->cursorPos = nextPos; 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: shift menu cursor ==> %d\n", nextPos );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿��̃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �ړ���
 */
//----------------------------------------------------------------------------------------------
static void ShiftQuestionIdx( RESEARCH_CHECK_WORK* work, int stride )
{
  int nowIdx;
  int nextIdx;

  // �C���f�b�N�X��ύX
  nowIdx  = work->questionIdx;
  nextIdx = ( nowIdx + stride + QUESTION_NUM_PER_TOPIC ) % QUESTION_NUM_PER_TOPIC;
  work->questionIdx = nextIdx;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: shift question index ==> %d\n", nextIdx );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �ύX��
 */
//----------------------------------------------------------------------------------------------
static void ShiftAnswerIdx( RESEARCH_CHECK_WORK* work, int stride )
{
  int nowIdx;
  int nextIdx;
  int answerNum;

  // �C���f�b�N�X��ύX
  answerNum = GetAnswerNum( work );
  nowIdx    = work->answerIdx;
  nextIdx   = ( nowIdx + stride + answerNum ) % answerNum;
  work->answerIdx = nextIdx;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: shift answer index ==> %d\n", nextIdx );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ResetAnswerIdx( RESEARCH_CHECK_WORK* work )
{
  work->answerIdx = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: reset answer index ==> %d\n", work->answerIdx );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^�̕\���^�C�v��ݒ肷��
 *
 * @param work
 * @param dispType �ݒ肷��\���^�C�v
 */
//----------------------------------------------------------------------------------------------
static void SetDataDisplayType( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType )
{
  work->dispType = dispType;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set data display type ==> " );
  switch( dispType )
  {
  case DATA_DISP_TYPE_TODAY: OS_TFPrintf( PRINT_TARGET, "TODAY" ); break;
  case DATA_DISP_TYPE_TOTAL: OS_TFPrintf( PRINT_TARGET, "TOTAL" ); break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetMenuCursorOn( RESEARCH_CHECK_WORK* work )
{
  // �Y������X�N���[�����X�V
  switch( work->cursorPos )
  {
  case MENU_ITEM_QUESTION:  // ����
  case MENU_ITEM_MY_ANSWER: // �����̉�
  case MENU_ITEM_COUNT:     // �񓚐l��
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW, 
                                MenuItemDrawParam[ work->cursorPos ].left_chara,
                                MenuItemDrawParam[ work->cursorPos ].top_chara,
                                MenuItemDrawParam[ work->cursorPos ].width_chara,
                                MenuItemDrawParam[ work->cursorPos ].height_chara,
                                MAIN_BG_PALETTE_MENU_ON );
    break;

  // �� ( �㉺2�������Ă��邽��, �ʏ��� )
  case MENU_ITEM_ANSWER:
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW,
                                ANSWER_UPPER_X, ANSWER_UPPER_Y, 
                                ANSWER_UPPER_WIDTH, ANSWER_UPPER_HEIGHT,
                                MAIN_BG_PALETTE_MENU_ON );
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW,
                                ANSWER_LOWER_X, ANSWER_LOWER_Y, 
                                ANSWER_LOWER_WIDTH, ANSWER_LOWER_HEIGHT,
                                MAIN_BG_PALETTE_MENU_ON );
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���]�����N�G�X�g
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );


  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set menu cursor on (%d)\n", work->cursorPos );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, ���j���[���ڂ��J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetMenuCursorOff( RESEARCH_CHECK_WORK* work )
{
  // �Y������X�N���[�����X�V
  switch( work->cursorPos )
  {
  case MENU_ITEM_QUESTION:  // ����
  case MENU_ITEM_MY_ANSWER: // �����̉�
  case MENU_ITEM_COUNT:     // �񓚐l��
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW, 
                                MenuItemDrawParam[ work->cursorPos ].left_chara,
                                MenuItemDrawParam[ work->cursorPos ].top_chara,
                                MenuItemDrawParam[ work->cursorPos ].width_chara,
                                MenuItemDrawParam[ work->cursorPos ].height_chara,
                                MAIN_BG_PALETTE_MENU_OFF );
    break;

  // �� ( �㉺2�������Ă��邽��, �ʏ��� )
  case MENU_ITEM_ANSWER:
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW,
                                ANSWER_UPPER_X, ANSWER_UPPER_Y, 
                                ANSWER_UPPER_WIDTH, ANSWER_UPPER_HEIGHT,
                                MAIN_BG_PALETTE_MENU_OFF );
    GFL_BG_ChangeScreenPalette( MAIN_BG_WINDOW,
                                ANSWER_LOWER_X, ANSWER_LOWER_Y, 
                                ANSWER_LOWER_WIDTH, ANSWER_LOWER_HEIGHT,
                                MAIN_BG_PALETTE_MENU_OFF );
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���]�����N�G�X�g
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set menu cursor off (%d)\n", work->cursorPos );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �^�b�`�͈͂��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateTouchArea( RESEARCH_CHECK_WORK* work )
{
  const MENU_ITEM_DRAW_PARAM* menu;
  GFL_UI_TP_HITTBL* area;

  // ���J�[�\��
  menu = &( MenuItemDrawParam[ work->cursorPos ] );
  area = &( work->touchHitTable[ TOUCH_AREA_CONTROL_CURSOR_L ] );
  area->rect.left   = menu->left_dot + menu->leftCursorOffsetX - CLWK_CONTROL_CURSOR_L_WIDTH/2;
  area->rect.top    = menu->top_dot  + menu->leftCursorOffsetY - CLWK_CONTROL_CURSOR_L_HEIGHT/2;
  area->rect.right  = area->rect.left + TOUCH_AREA_CONTROL_CURSOR_WIDTH;
  area->rect.bottom = area->rect.top  + TOUCH_AREA_CONTROL_CURSOR_HEIGHT;

  // �E�J�[�\��
  menu = &( MenuItemDrawParam[ work->cursorPos ] );
  area = &( work->touchHitTable[ TOUCH_AREA_CONTROL_CURSOR_R ] );
  area->rect.left   = menu->left_dot + menu->rightCursorOffsetX - CLWK_CONTROL_CURSOR_R_WIDTH/2;
  area->rect.top    = menu->top_dot  + menu->rightCursorOffsetY - CLWK_CONTROL_CURSOR_R_HEIGHT/2;
  area->rect.right  = area->rect.left + TOUCH_AREA_CONTROL_CURSOR_WIDTH;
  area->rect.bottom = area->rect.top  + TOUCH_AREA_CONTROL_CURSOR_HEIGHT;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update touch area\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief MAIN-BG ( �E�B���h�E�� ) ���X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateMainBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  ARCHANDLE* handle;
  ARCDATID datID;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, work->heapID ); 

  // ���f������X�N���[���f�[�^������
  if( work->seq == RESEARCH_CHECK_SEQ_UPDATE ) {
    datID = NARC_research_radar_graphic_bgd_graphbtn3_NSCR; //�u�f�[�^ ����Ƃ����イ�v
  }
  else if( GetCountOfQuestion( work ) == 0 ) {
    datID = NARC_research_radar_graphic_bgd_graphbtn1_NSCR; //�u�������� ���傤�����イ�v
  }
  else if( work->analyzeFlag == FALSE ) {
    datID = NARC_research_radar_graphic_bgd_graphbtn1_NSCR; // �����
  }
  else {
    datID = NARC_research_radar_graphic_bgd_graphbtn2_NSCR; // �ʏ�\��
  }

  // �X�N���[���f�[�^�ǂݍ���
  {
    void* src;
    NNSG2dScreenData* data;
    src = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
    NNS_G2dGetUnpackedScreenData( src, &data );
    GFL_BG_WriteScreen( MAIN_BG_WINDOW, data->rawData, 0, 0, 32, 32 );
    GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
    GFL_HEAP_FreeMemory( src );
  }

  // �n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup MAIN-BG-WINDOW\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �������ڂ̕⑫�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_TopicCaption( RESEARCH_CHECK_WORK* work )
{
  u32 topicID;
  u32 strID;

  // �������ڂ̕⑫���̕�����ID���擾
  topicID = work->researchData.topicID;
  strID = StringID_topicCaption[ topicID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( work->BGFont[ SUB_BG_FONT_TOPIC_CAPTION ], strID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont topic caption\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����̕⑫�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_QuestionCaption( RESEARCH_CHECK_WORK* work )
{
  u32 questionID;
  u32 strID;

  // ����̕⑫���̕�����ID���擾
  questionID  = GetQuestionID( work );
  strID       = CaptionStringID_question[ questionID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( work->BGFont[ SUB_BG_FONT_QUESTION_CAPTION ], strID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont question caption\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������X�V���� 
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_Question( RESEARCH_CHECK_WORK* work )
{
  u32 questionID;
  u32 strID;
  BG_FONT* BGFont;

  BGFont = work->BGFont[ MAIN_BG_FONT_QUESTION ];

  // ����̕�����ID���擾
  questionID = GetQuestionID( work );
  strID      = StringID_question[ questionID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( BGFont, strID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont question\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �񓚂��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_Answer( RESEARCH_CHECK_WORK* work )
{
  u16 answerID;
  u16 answerIdx;
  u8 answerRank;
  u16 count;
  u8 percentage;
  BG_FONT* BGFont;
  STRBUF* strbuf_expand; // �W�J��̕�����
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_answer; // �񓚂̕�����
  const CIRCLE_GRAPH* graph;

  BGFont = work->BGFont[ MAIN_BG_FONT_ANSWER ];
  graph  = GetMainGraph( work );

  // ��͑O�Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    BG_FONT_SetDrawEnable( BGFont, FALSE );
    return;
  }

  //�u�������� ���傤�����イ�v�Ȃ�, �\�����Ȃ�
  if( GetCountOfQuestion(work) == 0 ) {
    BG_FONT_SetDrawEnable( BGFont, FALSE );
    return;
  }

  // �񓚂̕�����ID�E���Ԃ��擾
  answerID   = GetAnswerID( work );
  answerIdx  = work->answerIdx;
  answerRank = CIRCLE_GRAPH_GetComponentRank( graph, answerID );
  count      = GetCountOfAnswer( work );
  percentage = CIRCLE_GRAPH_GetComponentPercentage( graph, answerID );

  // ���l��񓚂𕶎���ɓW�J
  {
    strbuf_plain  = GFL_MSG_CreateString( work->message[ MESSAGE_STATIC ], str_check_answer );
    strbuf_answer = GFL_MSG_CreateString( work->message[ MESSAGE_QUESTIONNAIRE ], StringID_answer[ answerID ] );
    strbuf_expand = GFL_STR_CreateBuffer( 128, work->heapID );
    WORDSET_RegisterNumber( work->wordset, 0, answerRank, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT ); // ���Ԗڂ�
    WORDSET_RegisterWord( work->wordset, 1, strbuf_answer, 0, TRUE, PM_LANG ); // �񓚕�����
    WORDSET_RegisterNumber( work->wordset, 2, count, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT ); // �l��
    WORDSET_RegisterNumber( work->wordset, 3, percentage, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT ); // �p�[�Z���e�[�W
    WORDSET_ExpandStr( work->wordset, strbuf_expand, strbuf_plain );
  }

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetString( BGFont, strbuf_expand );

  GFL_STR_DeleteBuffer( strbuf_plain );
  GFL_STR_DeleteBuffer( strbuf_expand );
  GFL_STR_DeleteBuffer( strbuf_answer );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont answer\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚂��X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_MyAnswer( RESEARCH_CHECK_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* questionnaireSave;
  QUESTIONNAIRE_ANSWER_WORK* myAnswerWork;
  u8 questionID;
  u16 answerID;
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_expand; // �W�J��̕�����
  STRBUF* strbuf_myAnswer;  // �����̉񓚕�����

  // ��͑O�Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_MY_ANSWER ], FALSE );
    return;
  }

  // �Z�[�u�f�[�^���擾
  save              = GAMEDATA_GetSaveControlWork( work->gameData );
  questionnaireSave = SaveData_GetQuestionnaire( save );
  myAnswerWork      = Questionnaire_GetAnswerWork( questionnaireSave );

  // ���ݕ\�����̎���ɑ΂���, �����̉�ID���擾
  questionID = GetQuestionID( work );
  answerID   = QuestionnaireAnswer_ReadBit( myAnswerWork, questionID );

  // �񓚕������W�J
  {
    strbuf_plain    = GFL_MSG_CreateString( work->message[ MESSAGE_STATIC ], str_check_my_answer );
    strbuf_myAnswer = GFL_MSG_CreateString( work->message[ MESSAGE_QUESTIONNAIRE ], StringID_answer[ answerID ] );
    strbuf_expand   = GFL_STR_CreateBuffer( 128, work->heapID );
    WORDSET_RegisterWord( work->wordset, 0, strbuf_myAnswer, 0, TRUE, PM_LANG );
    WORDSET_ExpandStr( work->wordset, strbuf_expand, strbuf_plain );
  }

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetString( work->BGFont[ MAIN_BG_FONT_MY_ANSWER ], strbuf_expand );

  GFL_STR_DeleteBuffer( strbuf_plain );
  GFL_STR_DeleteBuffer( strbuf_expand );
  GFL_STR_DeleteBuffer( strbuf_myAnswer );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont my answer\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �񓚐l�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_Count( RESEARCH_CHECK_WORK* work )
{
  u32 strID; 
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_expand; // �W�J��̕�����

  // ��͑O�Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_COUNT ], FALSE );
    return;
  }

  // ������ID������
  switch( work->dispType )
  {
  case DATA_DISP_TYPE_TODAY: strID = str_check_today_count; break;
  case DATA_DISP_TYPE_TOTAL: strID = str_check_total_count; break;
  default: GF_ASSERT(0);
  }

  // �񓚐l����W�J
  {
    s32 count;
    WORDSET* wordset;
    GFL_MSGDATA* message;

    count   = GetCountOfQuestion( work );
    message = work->message[ MESSAGE_STATIC ];
    wordset = work->wordset;
    strbuf_plain  = GFL_MSG_CreateString( message, strID );
    strbuf_expand = GFL_STR_CreateBuffer( 128, work->heapID ); 
    WORDSET_RegisterNumber( wordset, 0, count, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, strbuf_expand, strbuf_plain );
  }

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetString( work->BGFont[ MAIN_BG_FONT_COUNT ], strbuf_expand );

  GFL_STR_DeleteBuffer( strbuf_plain );
  GFL_STR_DeleteBuffer( strbuf_expand );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont count\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief�u�������� ���傤�����イ�v�̕\�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_NoData( RESEARCH_CHECK_WORK* work )
{
  if( GetCountOfQuestion(work) == 0 ) { 
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_NO_DATA ], TRUE ); // �\��
  }
  else { 
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_NO_DATA ], FALSE ); // �N���A
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont no data\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @breif�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdateBGFont_DataReceiving( RESEARCH_CHECK_WORK* work )
{
  if( work->seq == RESEARCH_CHECK_SEQ_UPDATE ) {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_DATA_RECEIVING ], TRUE ); // �\��  
  }
  else {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_DATA_RECEIVING ], FALSE ); // �N���A
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont data receiving\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����J�[�\���̕\�����X�V����
 *
 * @param work 
 */
//----------------------------------------------------------------------------------------------
static void UpdateControlCursor( RESEARCH_CHECK_WORK* work )
{
  const MENU_ITEM_DRAW_PARAM* menuParam;
  GFL_CLACTPOS pos;
  u16 setsf;
  GFL_CLWK* cursorL;
  GFL_CLWK* cursorR;

  // �J�[�\���ʒu�̃��j���[�̕\���p�����[�^���擾
  menuParam = &MenuItemDrawParam[ work->cursorPos ]; 

  // �Z���A�N�^�[���[�N���擾
  cursorL = GetClactWork( work, CLWK_CONTROL_CURSOR_L );
  cursorR = GetClactWork( work, CLWK_CONTROL_CURSOR_R );


  //�u�����̉񓚁v��I�����Ă���ꍇ�͕\�����Ȃ�
  if( work->cursorPos == MENU_ITEM_MY_ANSWER ) {
    GFL_CLACT_WK_SetDrawEnable( cursorL, FALSE );
    GFL_CLACT_WK_SetDrawEnable( cursorR, FALSE );
  }
  else { 
    // �\�����W��ύX
    pos.x = menuParam->left_dot + menuParam->leftCursorOffsetX;
    pos.y = menuParam->top_dot + menuParam->leftCursorOffsetY;
    setsf = ClactWorkInitData[ CLWK_CONTROL_CURSOR_L ].setSurface;
    GFL_CLACT_WK_SetPos( cursorL, &pos, setsf );

    pos.x = menuParam->left_dot + menuParam->rightCursorOffsetX;
    pos.y = menuParam->top_dot + menuParam->rightCursorOffsetY;
    setsf = ClactWorkInitData[ CLWK_CONTROL_CURSOR_R ].setSurface;
    GFL_CLACT_WK_SetPos( cursorR, &pos, setsf );

    // �\����Ԃ�ݒ�
    GFL_CLACT_WK_SetDrawEnable( cursorL, TRUE );
    GFL_CLACT_WK_SetDrawEnable( cursorR, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( cursorL, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( cursorR, TRUE );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update control cursor\n" );
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
static void BmpOamSetDrawEnable( RESEARCH_CHECK_WORK* work, BMPOAM_ACTOR_INDEX actorIdx, BOOL enable )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( actorIdx < BMPOAM_ACTOR_NUM );

  // �\����Ԃ�ύX
  BmpOam_ActorSetDrawEnable( work->BmpOamActor[ actorIdx ], enable );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, 
              "RESEARCH-CHECK: set draw enable BMP-OAM [%d] ==> %d\n", actorIdx, enable );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL );

  // �쐬
  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->seqQueue );

  // �폜
  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete seq queue\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitFont( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateFont( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, TRUE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create font\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteFont( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete font\n" );
}


//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitMessages( RESEARCH_CHECK_WORK* work )
{
  int msgIdx;

  // ������
  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    work->message[ msgIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateMessages( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteMessages( RESEARCH_CHECK_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] );

    // �폜
    GFL_MSG_Delete( work->message[ msgIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete messages\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitWordset( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->wordset = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init wordset\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateWordset( RESEARCH_CHECK_WORK* work )
{
  // ���d����
  GF_ASSERT( work->wordset == NULL );

  // �쐬
  work->wordset = WORDSET_Create( work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create wordset\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteWordset( RESEARCH_CHECK_WORK* work )
{
  // �폜
  WORDSET_Delete( work->wordset );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete wordset\n" );
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
static void SetupBG( RESEARCH_CHECK_WORK* work )
{ 
  // BG ���[�h
  GFL_BG_SetBGMode( &BGSysHeader3D );

  // BG �R���g���[��
  GFL_BG_SetBGControl3D( MAIN_BG_3D_PRIORITY );
  GFL_BG_SetBGControl( SUB_BG_WINDOW,  &SubBGControl_WINDOW,  GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_FONT,    &SubBGControl_FONT,    GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_WINDOW, &MainBGControl_WINDOW, GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( MAIN_BG_FONT,   &MainBGControl_FONT,   GFL_BG_MODE_TEXT );

  // ���ݒ�
  GFL_BG_SetVisible( SUB_BG_BACK,    VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_RADAR,   VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_WINDOW,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_FONT,    VISIBLE_ON );
  GFL_BG_SetVisible( MAIN_BG_3D,     VISIBLE_ON );
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup BG\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpBG( RESEARCH_CHECK_WORK* work )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_3D );
  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( SUB_BG_FONT );
  GFL_BG_FreeBGControl( SUB_BG_WINDOW );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up BG\n" );
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
static void SetupSubBG_WINDOW( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup SUB-BG-WINDOW\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up SUB-BG-WINDOW\n" );
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
static void SetupSubBG_FONT( RESEARCH_CHECK_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup SUB-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up SUB-BG-FONT\n" );
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
static void SetupMainBG_WINDOW( RESEARCH_CHECK_WORK* work )
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
      datID = NARC_research_radar_graphic_bgd_graphbtn2_NSCR;
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup MAIN-BG-WINDOW\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up MAIN-BG-WINDOW\n" );
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
static void SetupMainBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup MAIN-BG-FONT\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up MAIN-BG-FONT\n" );
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
static void InitBGFonts( RESEARCH_CHECK_WORK* work )
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
static void CreateBGFonts( RESEARCH_CHECK_WORK* work )
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

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create BGFonts\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBGFonts( RESEARCH_CHECK_WORK* work )
{
  int i;
  
  // �ʏ��BGFont
  for( i=0; i < BG_FONT_NUM; i++ )
  {
    GF_ASSERT( work->BGFont[i] );
    BG_FONT_Delete( work->BGFont[i] );
    work->BGFont[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete BGFonts\n" ); 
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �~�O���t������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitCircleGraphs( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < DATA_DISP_TYPE_NUM; idx++ )
  {
    work->mainGraph[ idx ] = NULL;
    work->subGraph[ idx ]  = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init circle graph\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �~�O���t�𐶐�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateCircleGraph( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < DATA_DISP_TYPE_NUM; idx++ )
  {
    // ���d����
    GF_ASSERT( work->mainGraph[ idx ] == NULL );
    GF_ASSERT( work->subGraph[ idx ] == NULL );

    work->mainGraph[ idx ] = CIRCLE_GRAPH_Create( work->heapID );
    work->subGraph[ idx ]  = CIRCLE_GRAPH_Create( work->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create circle graph\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteCircleGraph( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < DATA_DISP_TYPE_NUM; idx++ )
  {
    // ������
    GF_ASSERT( work->mainGraph[ idx ] != NULL );
    GF_ASSERT( work->subGraph[ idx ] != NULL );

    CIRCLE_GRAPH_Delete( work->mainGraph[ idx ] );
    CIRCLE_GRAPH_Delete( work->subGraph[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete circle graph\n" ); 
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitResearchData( RESEARCH_CHECK_WORK* work )
{
  // 0�N���A
  GFL_STD_MemClear( &( work->researchData ), sizeof(RESEARCH_DATA) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init research data\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^���擾����
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void SetupResearchData( RESEARCH_CHECK_WORK* work )
{
  int qIdx, aIdx;
  u8 topicID;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u8 answerNum[ QUESTION_NUM_PER_TOPIC ];
  u16 todayCount_question[ QUESTION_NUM_PER_TOPIC ];
  u16 totalCount_question[ QUESTION_NUM_PER_TOPIC ];
  u16 answerID[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  u16 todayCount_answer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  u16 totalCount_answer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* questionnaireSave;

  // �Z�[�u�f�[�^�擾
  save              = GAMEDATA_GetSaveControlWork( work->gameData );
  questionnaireSave = SaveData_GetQuestionnaire( save );
  topicID = QuestionnaireWork_GetInvestigatingQuestion( questionnaireSave, 0 ); // ��������ID
  questionID[0] = Question1_topic[ topicID ]; // ����ID
  questionID[1] = Question2_topic[ topicID ]; // ����ID
  questionID[2] = Question3_topic[ topicID ]; // ����ID
  answerNum[0] = AnswerNum_question[ questionID[0] ]; // �񓚑I�����̐�
  answerNum[1] = AnswerNum_question[ questionID[1] ]; // �񓚑I�����̐�
  answerNum[2] = AnswerNum_question[ questionID[2] ]; // �񓚑I�����̐�
  todayCount_question[0] = QuestionnaireWork_GetTodayCount( questionnaireSave, questionID[0] ); // �����̉񓚐l��
  todayCount_question[1] = QuestionnaireWork_GetTodayCount( questionnaireSave, questionID[1] ); // �����̉񓚐l��
  todayCount_question[2] = QuestionnaireWork_GetTodayCount( questionnaireSave, questionID[2] ); // �����̉񓚐l��
  totalCount_question[0] = QuestionnaireWork_GetTotalCount( questionnaireSave, questionID[0] ); // ���܂܂ł̉񓚐l��
  totalCount_question[1] = QuestionnaireWork_GetTotalCount( questionnaireSave, questionID[1] ); // ���܂܂ł̉񓚐l��
  totalCount_question[2] = QuestionnaireWork_GetTotalCount( questionnaireSave, questionID[2] ); // ���܂܂ł̉񓚐l��
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    u8 qID = questionID[ qIdx ];
    for( aIdx=0; aIdx < answerNum[ qIdx ]; aIdx++ )
    {
      answerID[ qIdx ][ aIdx ] = AnswerID_question[ qID ][ aIdx ]; // ��ID
      todayCount_answer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTodayAnswerNum( questionnaireSave, qIdx, aIdx ); // �����̉񓚐l��
      totalCount_answer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTotalAnswerNum( questionnaireSave, qIdx, aIdx ); // ���܂܂ł̉񓚐l��
    }
  }

  // �f�[�^�ݒ�
  work->researchData.topicID = topicID; // ��������ID
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    work->researchData.questionData[ qIdx ].ID         = questionID[ qIdx ];           // ����ID
    work->researchData.questionData[ qIdx ].answerNum  = answerNum[ qIdx ];            // �񓚑I�����̐�
    work->researchData.questionData[ qIdx ].todayCount = todayCount_question[ qIdx ];  // �����̉񓚐l��
    work->researchData.questionData[ qIdx ].totalCount = totalCount_question[ qIdx ];  // ���܂܂ł̉񓚐l��

    for( aIdx=0; aIdx < MAX_ANSWER_NUM_PER_QUESTION; aIdx++ )
    {
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID = answerID[ qIdx ][ aIdx ]; // ��ID
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorR = (aIdx * 3) % 31; // �\���J���[(R)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorG = (aIdx * 5) % 31; // �\���J���[(G)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorB = (aIdx * 7) % 31; // �\���J���[(B)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].todayCount = todayCount_answer[ qIdx ][ aIdx ]; // �����̉񓚐l��
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount = totalCount_answer[ qIdx ][ aIdx ];  // ���܂܂ł̉񓚐l��
    }
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @breif �^�b�`�̈�̏������s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupTouchArea( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create touch hit table\n" );
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
static void CreateClactSystem( RESEARCH_CHECK_WORK* work )
{
  // �V�X�e���쐬
  //GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create clact system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_CHECK_WORK* work )
{ 
  // �V�X�e���j��
  //GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete clact system\n" );
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
static void RegisterSubObjResources( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: register SUB-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RESEARCH_CHECK_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release SUB-OBJ resources\n" );
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
static void RegisterMainObjResources( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: register MAIN-OBJ resources\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RESEARCH_CHECK_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release MAIN-OBJ resources\n" );
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
static void InitClactUnits( RESEARCH_CHECK_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void CreateClactUnits( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create clact units\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactUnits( RESEARCH_CHECK_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete clact units\n" );
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
static void InitClactWorks( RESEARCH_CHECK_WORK* work )
{
  int wkIdx;

  // ������
  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    work->clactWork[ wkIdx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreateClactWorks( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeleteClactWorks( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete clact works\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitBitmapDatas( RESEARCH_CHECK_WORK* work )
{
  int idx;

  // ������
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    work->BmpData[ idx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���쐬����
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CreateBitmapDatas( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^�̏������s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupBitmapDatas( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBitmapDatas( RESEARCH_CHECK_WORK* work )
{
  int idx;

  // �j��
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    GFL_BMP_Delete( work->BmpData[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete bitmap datas\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̏������s��
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void SetupBmpOamSystem( RESEARCH_CHECK_WORK* work )
{
  // BMP-OAM �V�X�e�����쐬
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup BMP-OAM system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RESEARCH_CHECK_WORK* work )
{
  // BMP-OAM �V�X�e����j��
  BmpOam_Exit( work->BmpOamSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up BMP-OAM system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void CreateBmpOamActors( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create BMP-OAM actors\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[��j������
 *
 * @parma work
 */
//----------------------------------------------------------------------------------------------
static void DeleteBmpOamActors( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    BmpOam_ActorDel( work->BmpOamActor[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete BMP-OAM actors\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief 3D �����ݒ�
 */
//-------------------------------------------------------------------------------
static void Setup3D()
{
  G3X_Init();
  G3X_InitMtxStack();

  G3_MtxMode( GX_MTXMODE_PROJECTION );
  G3_Identity();
  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  G3_Identity(); 

  G3X_AntiAlias( TRUE );   // �A���`�G�C���A�X
  G3X_EdgeMarking( TRUE ); // �G�b�W�}�[�L���O
  G3X_SetEdgeColorTable( EdgeColor );  // �G�b�W�J���[
}


//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N��o�^����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void RegisterVBlankTask( RESEARCH_CHECK_WORK* work )
{
  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: register VBlank task\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RESEARCH_CHECK_WORK* work )
{ 
  GFL_TCB_DeleteTask( work->VBlankTask );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release VBlank task\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RESEARCH_CHECK_WORK* work )
{ 
  // ������
  work->paletteFadeSystem = NULL;

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init palette fade system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup palette fade system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���̌�Еt�����s��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpPaletteFadeSystem( RESEARCH_CHECK_WORK* work )
{ 
  // �p���b�g�t�F�[�h�̃��N�G�X�g���[�N��j��
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_OBJ );

  // �t�F�[�h�Ǘ��V�X�e����j��
  PaletteFadeFree( work->paletteFadeSystem );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up palette fade system\n" );
}


//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̒�������ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̒�������ID
 */
//----------------------------------------------------------------------------------------------
static u8 GetTopicID( const RESEARCH_CHECK_WORK* work )
{
  return work->researchData.topicID;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ID
 */
//----------------------------------------------------------------------------------------------
static u8 GetQuestionID( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  qIdx = work->questionIdx;
  return work->researchData.questionData[ qIdx ].ID;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���񓚑I�����̐� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �񓚂̑I�����̐�
 */
//---------------------------------------------------------------------------------------------- 
static u8 GetAnswerNum( const RESEARCH_CHECK_WORK* work )
{
  u8 questionIdx;

  questionIdx = work->questionIdx;

  return work->researchData.questionData[ questionIdx ].answerNum;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉�ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̉�ID
 */
//----------------------------------------------------------------------------------------------
static u16 GetAnswerID( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, �\���̉񓚐l�� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �\���̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  switch( work->dispType )
  {
  case DATA_DISP_TYPE_TODAY: return GetTodayCountOfQuestion( work ); break;
  case DATA_DISP_TYPE_TOTAL: return GetTotalCountOfQuestion( work ); break;
  default: GF_ASSERT(0);
  }

  // �G���[
  GF_ASSERT(0);
  return 0;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, �����̉񓚐l�����擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �����̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetTodayCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;

  qIdx = work->questionIdx;

  return work->researchData.questionData[ qIdx ].todayCount;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetTotalCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;

  qIdx = work->questionIdx;

  return work->researchData.questionData[ qIdx ].totalCount;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, �\���̉񓚐l�� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, �\���̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetCountOfAnswer( const RESEARCH_CHECK_WORK* work )
{
  switch( work->dispType )
  {
  case DATA_DISP_TYPE_TODAY: return GetTodayCountOfAnswer( work ); break;
  case DATA_DISP_TYPE_TOTAL: return GetTotalCountOfAnswer( work ); break;
  default: GF_ASSERT(0);
  }

  // �G���[
  GF_ASSERT(0);
  return 0;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetTodayCountOfAnswer( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].todayCount;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
 */
//----------------------------------------------------------------------------------------------
static u16 GetTotalCountOfAnswer( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̃��C���~�O���t ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̃��C���~�O���t
 */
//----------------------------------------------------------------------------------------------
static CIRCLE_GRAPH* GetMainGraph( const RESEARCH_CHECK_WORK* work )
{
  return work->mainGraph[ work->dispType ];
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̃T�u�~�O���t
 *
 * @param work
 *
 * @return ���ݕ\�����̃T�u�~�O���t
 */
//----------------------------------------------------------------------------------------------
static CIRCLE_GRAPH* GetSubGraph ( const RESEARCH_CHECK_WORK* work )
{
  return work->subGraph[ work->dispType ];
}

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
static u32 GetObjResourceRegisterIndex( const RESEARCH_CHECK_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RESEARCH_CHECK_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RESEARCH_CHECK_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteFadeOut( RESEARCH_CHECK_WORK* work )
{
#if 0
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
#endif 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: start palette fade out\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�C�����J�n����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteFadeIn( RESEARCH_CHECK_WORK* work )
{
#if 0
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
#endif

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: start palette fade in\n" );
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
static BOOL IsPaletteFadeEnd( RESEARCH_CHECK_WORK* work )
{
  return PaletteFadeCheck( work->paletteFadeSystem );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InterchangeCircleGraph( RESEARCH_CHECK_WORK* work )
{
  int typeIdx;
  CIRCLE_GRAPH* temp;

  for( typeIdx=0; typeIdx < DATA_DISP_TYPE_NUM; typeIdx++ )
  {
    CIRCLE_GRAPH* temp         = work->mainGraph[ typeIdx ];
    work->mainGraph[ typeIdx ] = work->subGraph[ typeIdx ];
    work->subGraph[ typeIdx ]  = temp;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: interchange circle graph\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief ���C���~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param dispType �\���^�C�v
 */
//----------------------------------------------------------------------------------------------
static void SetupMainCircleGraph( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType )
{
  int aIdx;
  CIRCLE_GRAPH* graph;
  GRAPH_COMPONENT_ADD_DATA components[ MAX_ANSWER_NUM_PER_QUESTION ];
  u8 answerNum;
  const QUESTION_DATA* questionData;
  const ANSWER_DATA* answerData;

  graph        = work->mainGraph[ dispType ];
  answerNum    = GetAnswerNum( work );
  questionData = &( work->researchData.questionData[ work->questionIdx ] );

  // �O���t�̍\���v�f�f�[�^���쐬
  for( aIdx=0; aIdx < answerNum; aIdx++ )
  {
    answerData = &( questionData->answerData[ aIdx ] );
    components[ aIdx ].ID = answerData->ID;
    components[ aIdx ].outerColorR = answerData->colorR;
    components[ aIdx ].outerColorG = answerData->colorG;
    components[ aIdx ].outerColorB = answerData->colorB;
    components[ aIdx ].centerColorR = answerData->colorR;
    components[ aIdx ].centerColorG = answerData->colorG;
    components[ aIdx ].centerColorB = answerData->colorB;
    switch( dispType )
    {
    case DATA_DISP_TYPE_TODAY: components[ aIdx ].value = answerData->todayCount; break;
    case DATA_DISP_TYPE_TOTAL: components[ aIdx ].value = answerData->totalCount; break;
    default: GF_ASSERT(0);
    }
  }

  // �O���t�̍\���v�f���Z�b�g
  CIRCLE_GRAPH_SetupComponents( graph, components, answerNum );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup main circle graph\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param dispType �\���^�C�v
 */
//----------------------------------------------------------------------------------------------
static void SetupSubCircleGraph( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType )
{
  int aIdx;
  CIRCLE_GRAPH* graph;
  GRAPH_COMPONENT_ADD_DATA components[ MAX_ANSWER_NUM_PER_QUESTION ];
  u8 answerNum;
  const QUESTION_DATA* questionData;
  const ANSWER_DATA* answerData;

  graph        = work->subGraph[ dispType ];
  answerNum    = GetAnswerNum( work );
  questionData = &( work->researchData.questionData[ work->questionIdx ] );

  // �O���t�̍\���v�f�f�[�^���쐬
  for( aIdx=0; aIdx < answerNum; aIdx++ )
  {
    answerData = &( questionData->answerData[ aIdx ] );
    components[ aIdx ].ID = answerData->ID;
    components[ aIdx ].outerColorR = answerData->colorR;
    components[ aIdx ].outerColorG = answerData->colorG;
    components[ aIdx ].outerColorB = answerData->colorB;
    components[ aIdx ].centerColorR = 31;
    components[ aIdx ].centerColorG = 31;
    components[ aIdx ].centerColorB = 31;
    switch( dispType )
    {
    case DATA_DISP_TYPE_TODAY: components[ aIdx ].value = answerData->todayCount; break;
    case DATA_DISP_TYPE_TOTAL: components[ aIdx ].value = answerData->totalCount; break;
    default: GF_ASSERT(0);
    }
  }

  // �O���t�̍\���v�f���Z�b�g
  CIRCLE_GRAPH_SetupComponents( graph, components, answerNum );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup sub circle graph\n" );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_CHECK_WORK* work )
{
  int i;
  int dataNum;
  int value;

  // �L���[���̃f�[�^�̌����擾
  dataNum = QUEUE_GetDataNum( work->seqQueue );

  // �S�Ẵf�[�^���o��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: seq queue = " );
  for( i=0; i < dataNum; i++ )
  { 
    value = QUEUE_PeekData( work->seqQueue, i );
    
    switch( value )
    {
    case RESEARCH_CHECK_SEQ_SETUP:     OS_TFPrintf( PRINT_TARGET, "SETUP " );    break;
    case RESEARCH_CHECK_SEQ_KEY_WAIT:  OS_TFPrintf( PRINT_TARGET, "KEY-WAIT " ); break;
    case RESEARCH_CHECK_SEQ_ANALYZE:   OS_TFPrintf( PRINT_TARGET, "ANALYZE " );  break;
    case RESEARCH_CHECK_SEQ_FLASH:     OS_TFPrintf( PRINT_TARGET, "FLASH " );    break;
    case RESEARCH_CHECK_SEQ_UPDATE:    OS_TFPrintf( PRINT_TARGET, "UPDATE " );   break;
    case RESEARCH_CHECK_SEQ_FADE_OUT:  OS_TFPrintf( PRINT_TARGET, "FADE_OUT " );   break;
    case RESEARCH_CHECK_SEQ_CLEAN_UP:  OS_TFPrintf( PRINT_TARGET, "CLEAN-UP " ); break;
    case RESEARCH_CHECK_SEQ_FINISH:    OS_TFPrintf( PRINT_TARGET, "FINISH " );   break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 

//----------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^��\������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DebugPrint_researchData( const RESEARCH_CHECK_WORK* work )
{
  const RESEARCH_DATA* research = &( work->researchData );
  int qIdx, aIdx;

  // ��������ID
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: topicID = %d\n", research->topicID );

  // ����f�[�^
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    const QUESTION_DATA* question = &( research->questionData[ qIdx ] );
    OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: questionID = %d\n", question->ID );
    OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: answerNum = %d\n",  question->answerNum );
    OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: todayCount = %d\n", question->todayCount );
    OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: totalCount = %d\n", question->totalCount );

    // �񓚃f�[�^
    for( aIdx=0; aIdx < MAX_ANSWER_NUM_PER_QUESTION; aIdx++ )
    {
      const ANSWER_DATA* answer = &( question->answerData[ aIdx ] );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: answerID = %d\n", answer->ID );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: colorR = %d\n", answer->colorR );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: colorG = %d\n", answer->colorG );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: colorB = %d\n", answer->colorB );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: todayCount = %d\n", answer->todayCount );
      OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: totalCount = %d\n", answer->totalCount );
    }
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @breif �����f�[�^��ݒ肷��
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void Debug_SetupResearchData( RESEARCH_CHECK_WORK* work )
{
  int qIdx, aIdx;
  u8 topicID;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u16 answerID[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  u8 answerNum[ QUESTION_NUM_PER_TOPIC ];
  u16 todayCount_question[ QUESTION_NUM_PER_TOPIC ] = { 0, 200, 300 };
  u16 totalCount_question[ QUESTION_NUM_PER_TOPIC ] = { 300, 500, 900 };
  u16 todayCount_answer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ] = 
  {
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  };
  u16 totalCount_answer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ] = 
  {
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 },
  };

  topicID = TOPIC_ID_STYLE; // ��������ID
  questionID[0] = Question1_topic[ topicID ]; // ����ID
  questionID[1] = Question2_topic[ topicID ]; // ����ID
  questionID[2] = Question3_topic[ topicID ]; // ����ID
  answerNum[0] = AnswerNum_question[ questionID[0] ]; // �񓚑I�����̐�
  answerNum[1] = AnswerNum_question[ questionID[1] ]; // �񓚑I�����̐�
  answerNum[2] = AnswerNum_question[ questionID[2] ]; // �񓚑I�����̐�
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    u8 qID = questionID[ qIdx ];
    todayCount_question[ qIdx ] = GFUser_GetPublicRand( 10 );
    totalCount_question[ qIdx ] = GFUser_GetPublicRand( 9999 );
    for( aIdx=0; aIdx < answerNum[ qIdx ]; aIdx++ )
    {
      answerID[ qIdx ][ aIdx ] = AnswerID_question[ qID ][ aIdx ]; // ��ID
      todayCount_answer[ qIdx ][ aIdx ] = GFUser_GetPublicRand( 10 ); 
      totalCount_answer[ qIdx ][ aIdx ] = GFUser_GetPublicRand( 500 ); 
    }
  }

  // �f�[�^�ݒ�
  work->researchData.topicID = topicID; // ��������ID
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    work->researchData.questionData[ qIdx ].ID         = questionID[ qIdx ];           // ����ID
    work->researchData.questionData[ qIdx ].answerNum  = answerNum[ qIdx ];            // �񓚑I�����̐�
    work->researchData.questionData[ qIdx ].todayCount = todayCount_question[ qIdx ];  // �����̉񓚐l��
    work->researchData.questionData[ qIdx ].totalCount = totalCount_question[ qIdx ];  // ���܂܂ł̉񓚐l��

    for( aIdx=0; aIdx < MAX_ANSWER_NUM_PER_QUESTION; aIdx++ )
    {
      u16 aID = answerID[ qIdx ][ aIdx ];
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID = aID; // ��ID
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorR = ColorR_answer[ aID ]; // �\���J���[(R)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorG = ColorG_answer[ aID ]; // �\���J���[(G)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorB = ColorB_answer[ aID ]; // �\���J���[(B)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].todayCount = todayCount_answer[ qIdx ][ aIdx ]; // �����̉񓚐l��
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount = totalCount_answer[ qIdx ][ aIdx ];  // ���܂܂ł̉񓚐l��
    }
  }
}
