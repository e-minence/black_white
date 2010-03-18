///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F���
 * @file   research_check.c
 * @author obata
 * @date   2010.02.20
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "queue.h"
#include "circle_graph.h"
#include "arrow.h"
#include "percentage.h"
#include "bg_font.h"
#include "palette_anime.h"
#include "research_data.h"
#include "research_check.h"
#include "research_check_index.h"
#include "research_check_def.h"
#include "research_check_data.cdat"
#include "research_common.h"
#include "research_common_data.cdat"

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

#include "answer_id_question.cdat"         // for AnswerID_question[][]
#include "answer_num_question.cdat"        // for AnswerNum_question[]
#include "question_id_topic.cdat"          // for QuestionX_topic[]
#include "string_id_question.cdat"         // for StringID_question[]
#include "string_id_topic_title.cdat"      // for StringID_topicTitle[]
#include "string_id_topic_caption.cdat"    // for StringID_topicCaption[]
#include "caption_string_id_question.cdat" // for CaptionStringID_question[]
#include "color_r_answer.cdat"             // for ColorR_answer[]
#include "color_g_answer.cdat"             // for ColorG_answer[]
#include "color_b_answer.cdat"             // for ColorB_answer[]



//=========================================================================================
// �������񍐊m�F��� ���[�N
//=========================================================================================
struct _RESEARCH_CHECK_WORK
{ 
  RESEARCH_COMMON_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID                heapID;
  GAMESYS_WORK*         gameSystem;
  GAMEDATA*             gameData;

  GFL_FONT*    font;                   // �t�H���g
  GFL_MSGDATA* message[ MESSAGE_NUM ]; // ���b�Z�[�W
  WORDSET*     wordset;                // ���[�h�Z�b�g

  // �V�[�P���X
  QUEUE*                seqQueue;      // �V�[�P���X�L���[
  RESEARCH_CHECK_SEQ    seq;           // ���݂̏����V�[�P���X
  BOOL                  seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  u32                   seqCount;      // �V�[�P���X�J�E���^
  RESEARCH_CHECK_RESULT result;        // ��ʏI������

  // �J�[�\��
  MENU_ITEM      cursorPos;     // �J�[�\���ʒu
  u8             questionIdx;   // �\�����̎���C���f�b�N�X
  u8             answerIdx;     // �I�𒆂̉񓚃C���f�b�N�X
  DATA_DISP_TYPE dispType;      // ���� or ���v �̂ǂ���̃f�[�^��\������̂�

  // �����f�[�^
  RESEARCH_DATA researchData;

  // �t���O
  BOOL analyzeFlag; // ��͂��ς�ł��邩�ǂ���
  BOOL updateFlag;  // �X�V�����ǂ���

  // �~�O���t
  CIRCLE_GRAPH* mainGraph[ DATA_DISP_TYPE_NUM ]; // �ʏ펞�ɕ\������O���t
  CIRCLE_GRAPH* subGraph [ DATA_DISP_TYPE_NUM ]; // �X�V���ɕ\������O���t

  // ���
  ARROW* arrow;

  // ���\���I�u�W�F�N�g
  PERCENTAGE* percentage[ PERCENTAGE_NUM ];
  u8          percentageNum;     // �L���ȃI�u�W�F�N�g�̐�
  u8          percentageDispNum; // �\�����̃I�u�W�F�N�g�̐�p

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL touchHitTable[ TOUCH_AREA_NUM ];

  // �p���b�g�A�j���[�V����
  PALETTE_ANIME* paletteAnime[ PALETTE_ANIME_NUM ];

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




//=========================================================================================
// ���֐��C���f�b�N�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
// ��LAYER 4 �V�[�P���X
//-----------------------------------------------------------------------------------------
// �V�[�P���X����������
static void InitSeq_SETUP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void InitSeq_STANDBY( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_STANDBY
static void InitSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void InitSeq_ANALYZE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void InitSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_PERCENTAGE
static void InitSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_OUT
static void InitSeq_FLASH_IN( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_IN
static void InitSeq_UPDATE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void InitSeq_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void InitSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP
// �V�[�P���X����
static void MainSeq_SETUP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void MainSeq_STANDBY( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_STANDBY
static void MainSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void MainSeq_ANALYZE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void MainSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_PERCENTAGE
static void MainSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_OUT
static void MainSeq_FLASH_IN( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_IN
static void MainSeq_UPDATE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void MainSeq_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void MainSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP 
// �V�[�P���X�I������
static void FinishSeq_SETUP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_SETUP
static void FinishSeq_STANDBY( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_STANDBY
static void FinishSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_KEY_WAIT
static void FinishSeq_ANALYZE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_ANALYZE
static void FinishSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_PERCENTAGE
static void FinishSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_OUT
static void FinishSeq_FLASH_IN( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FLASH_IN
static void FinishSeq_UPDATE( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_UPDATE
static void FinishSeq_FADE_OUT( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_FADE_OUT
static void FinishSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work ); // RESEARCH_CHECK_SEQ_CLEAN_UP
// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSeq( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq ); // ���̃V�[�P���X���L���[�ɓo�^����
static void FinishCurrentSeq( RESEARCH_CHECK_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSeq( RESEARCH_CHECK_WORK* work ); // �����V�[�P���X��ύX����
static void SetSeq( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
//-----------------------------------------------------------------------------------------
// ��LAYER 3 �@�\
//-----------------------------------------------------------------------------------------
// �J�[�\��
static void MoveMenuCursorUp( RESEARCH_CHECK_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RESEARCH_CHECK_WORK* work ); // ���ֈړ�����
static void MoveMenuCursorDirect( RESEARCH_CHECK_WORK* work, MENU_ITEM menuItem ); // ���ڈړ�����
// �\�����鎿��
static void ChangeQuestionToNext( RESEARCH_CHECK_WORK* work ); // ���̎���ɕύX����
static void ChangeQuestionToPrev( RESEARCH_CHECK_WORK* work ); // �O�̎���ɕύX����
// �\�������
static void ChangeAnswerToNext( RESEARCH_CHECK_WORK* work ); // ���̉񓚂ɕύX����
static void ChangeAnswerToPrev( RESEARCH_CHECK_WORK* work ); // �O�̉񓚂ɕύX����
static void ChangeAnswerToTop( RESEARCH_CHECK_WORK* work ); // �擪�̉񓚂ɕύX����
// �����f�[�^�̕\���^�C�v
static void SwitchDataDisplayType( RESEARCH_CHECK_WORK* work ); // �����f�[�^�̕\���^�C�v��؂�ւ���
//-----------------------------------------------------------------------------------------
// ��LAYER 2 �ʑ���
//-----------------------------------------------------------------------------------------
// ���j���[���ڃJ�[�\��
static void ShiftMenuCursorPos( RESEARCH_CHECK_WORK* work, int stride ); // �J�[�\���ʒu��ύX����
static void SetMenuCursorPos( RESEARCH_CHECK_WORK* work, MENU_ITEM menuItem ); // �J�[�\���ʒu��ύX����
// ���j���[���ڂ̕\��
static void SetMenuCursorOn( RESEARCH_CHECK_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuCursorOff( RESEARCH_CHECK_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
// �\�����鎿��C���f�b�N�X
static void ShiftQuestionIdx( RESEARCH_CHECK_WORK* work, int stride ); // �\�����鎿��̃C���f�b�N�X��ύX����
// �I�𒆂̉񓚃C���f�b�N�X
static void ShiftAnswerIdx( RESEARCH_CHECK_WORK* work, int stride ); // �I�𒆂̉񓚃C���f�b�N�X��ύX����
static void ResetAnswerIdx( RESEARCH_CHECK_WORK* work );             // �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����
// �����f�[�^�\���^�C�v
static void SetDataDisplayType( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // �����f�[�^�̕\���^�C�v��ݒ肷��
// �~�O���t
static void UpdateCircleGraphs( RESEARCH_CHECK_WORK* work ); // ���ׂẲ~�O���t���X�V����
static void DrawCircleGraphs( const RESEARCH_CHECK_WORK* work ); // ���ׂẲ~�O���t��`�悷��
static void SetupMainCircleGraph( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // ���C���~�O���t�����݂̒����f�[�^�ō\������
static void SetupSubCircleGraph ( RESEARCH_CHECK_WORK* work, DATA_DISP_TYPE dispType ); // �T�u�~�O���t�����݂̒����f�[�^�ō\������
static void InterchangeCircleGraph( RESEARCH_CHECK_WORK* work ); // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
static void SetupCenterColorOfGraphComponent( GRAPH_COMPONENT_ADD_DATA* component ); // �O���t�\���v�f�̒��S�_�J���[���Z�b�g�A�b�v����
static void AdjustCircleGraphLayer( RESEARCH_CHECK_WORK* work ); // ���C���~�O���t, �T�u�~�O���t�̏d�Ȃ���𒲐�����
static CIRCLE_GRAPH* GetMainGraph( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̃��C���~�O���t���擾����
static CIRCLE_GRAPH* GetSubGraph( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̃T�u�~�O���t���擾����
static BOOL CheckAllGraphAnimeEnd( const RESEARCH_CHECK_WORK* work ); // �A�j���[�V�������̉~�O���t�����邩�ǂ����𔻒肷��
// �񓚃}�[�J�[
static void DrawAnswerMarker( const RESEARCH_CHECK_WORK* work ); // �񓚃}�[�J�[��`�悷��
// ���
static void UpdateArrow( RESEARCH_CHECK_WORK* work ); // ���̕\�����X�V����
// % �\��
static void SetupPercentages( RESEARCH_CHECK_WORK* work ); // ���݂̒����f�[�^�ɂ��킹�ăZ�b�g�A�b�v����
static void VanishAllPercentage( RESEARCH_CHECK_WORK* work ); // �S�Ắ��\������������
static void DispPercentage( RESEARCH_CHECK_WORK* work, u8 index ); // ���I�u�W�F�N�g��\������
static void DispAllPercentage( RESEARCH_CHECK_WORK* work ); // �S�Ắ��I�u�W�F�N�g��\������
// �^�b�`�͈�
static void UpdateTouchArea( RESEARCH_CHECK_WORK* work ); // �^�b�`�͈͂��X�V����
// BG
static void SetupBG( RESEARCH_CHECK_WORK* work ); // BG�S�ʂɂ������Z�b�g�A�b�v���s��
static void SetupSubBG_WINDOW( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ���Z�b�g�A�b�v����
static void SetupSubBG_FONT( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �t�H���g�� ) ���Z�b�g�A�b�v����
static void SetupMainBG_WINDOW( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���Z�b�g�A�b�v����
static void SetupMainBG_FONT( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �t�H���g�� ) ���Z�b�g�A�b�v����
static void CleanUpBG( RESEARCH_CHECK_WORK* work ); // BG�S�ʂɂ������N���[���A�b�v���s��
static void CleanUpSubBG_WINDOW( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ���N���[���A�b�v����
static void CleanUpSubBG_FONT( RESEARCH_CHECK_WORK* work ); // SUB-BG ( �t�H���g�� ) ���N���[���A�b�v����
static void CleanUpMainBG_WINDOW( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���N���[���A�b�v����
static void CleanUpMainBG_FONT( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �t�H���g�� ) ���N���[���A�b�v����
static void UpdateMainBG_WINDOW( RESEARCH_CHECK_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���X�V����
// BGFont
static void UpdateBGFont_TopicTitle( RESEARCH_CHECK_WORK* work ); // �������ږ����X�V����
static void UpdateBGFont_QuestionCaption( RESEARCH_CHECK_WORK* work ); // ����̕⑫�����X�V����
static void UpdateBGFont_Question( RESEARCH_CHECK_WORK* work ); // ������X�V���� 
static void UpdateBGFont_Answer( RESEARCH_CHECK_WORK* work ); // �񓚂��X�V����
static void UpdateBGFont_MyAnswer( RESEARCH_CHECK_WORK* work ); // �����̉񓚂��X�V����
static void UpdateBGFont_Count( RESEARCH_CHECK_WORK* work ); // �񓚐l�����X�V����
static void UpdateBGFont_NoData( RESEARCH_CHECK_WORK* work ); //�u�������� ���傤�����イ�v�̕\�����X�V����
static void UpdateBGFont_DataReceiving( RESEARCH_CHECK_WORK* work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
// OBJ
static void UpdateControlCursor( RESEARCH_CHECK_WORK* work ); // ����J�[�\���̕\�����X�V����
static void UpdateMyAnswerIconOnButton( RESEARCH_CHECK_WORK* work ); // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����
static void UpdateMyAnswerIconOnGraph( RESEARCH_CHECK_WORK* work ); // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
// BMP-OAM
static void InitBitmapDatas( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^������������
static void CreateBitmapDatas( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^�𐶐�����
static void SetupBitmapData_forDefault( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^���Z�b�g�A�b�v���� ( �f�t�H���g ) 
static void SetupBitmapData_forANALYZE_BUTTON( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^���Z�b�g�A�b�v���� (�u�񍐂�����v�{�^�� ) 
static void DeleteBitmapDatas( RESEARCH_CHECK_WORK* work ); // �r�b�g�}�b�v�f�[�^��j������
static void SetupBmpOamSystem( RESEARCH_CHECK_WORK* work ); // BMP-OAM �V�X�e�����Z�b�g�A�b�v����
static void CleanUpBmpOamSystem( RESEARCH_CHECK_WORK* work ); // BMP-OAM �V�X�e�����N���[���A�b�v����
static void CreateBmpOamActors( RESEARCH_CHECK_WORK* work ); // BMP-OAM �A�N�^�[�𐶐�����
static void DeleteBmpOamActors( RESEARCH_CHECK_WORK* work ); // BMP-OAM �A�N�^�[��j������
static void BmpOamSetDrawEnable( RESEARCH_CHECK_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable );  // �\�����邩�ǂ�����ݒ肷��
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
static void StartPaletteAnime( RESEARCH_CHECK_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RESEARCH_CHECK_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void UpdatePaletteAnime( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
// �p���b�g�t�F�[�h
static void InitPaletteFadeSystem( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ������
static void SetupPaletteFadeSystem( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ����
static void CleanUpPaletteFadeSystem( RESEARCH_CHECK_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��
static void StartPaletteFadeOut( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�A�E�g���J�n����
static void StartPaletteFadeIn ( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�C�����J�n����
static void StartPaletteFadeFlashOut( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���ɂ��t���b�V�� ( �A�E�g ) ���J�n����
static void StartPaletteFadeFlashIn ( RESEARCH_CHECK_WORK* work ); // �p���b�g�A�j���ɂ��t���b�V�� ( �C�� ) ���J�n����
static BOOL IsPaletteFadeEnd( RESEARCH_CHECK_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
// VBlank�^�X�N
static void RegisterVBlankTask( RESEARCH_CHECK_WORK* work ); // VBlank �^�X�N��o�^����
static void ReleaseVBlankTask( RESEARCH_CHECK_WORK* work ); // VBlank �^�X�N�̓o�^����������
static void VBlankFunc( GFL_TCB* tcb, void* wk );  // VBlank���̏���
// �����f�[�^
static u8 GetTopicID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̒�������ID
static u8 GetQuestionID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ID
static u8 GetAnswerNum( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���񓚑I�����̐�
static u16 GetAnswerID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉�ID
static u16 GetCountOfQuestion( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfQuestion( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfQuestion( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
static u16 GetCountOfAnswer( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfAnswer( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfAnswer( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
static u8 GetInvestigatingTopicID( const RESEARCH_CHECK_WORK* work ); // ���ݒ������̒�������ID���擾����
static u8 GetMyAnswerID( const RESEARCH_CHECK_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �����̉�ID���擾����
// OBJ
static void CreateClactSystem( RESEARCH_CHECK_WORK* work ); // OBJ �V�X�e���𐶐�����
static void DeleteClactSystem( RESEARCH_CHECK_WORK* work ); // OBJ �V�X�e����j������
static void RegisterSubObjResources( RESEARCH_CHECK_WORK* work ); // SUB-OBJ ���\�[�X��o�^����
static void ReleaseSubObjResources( RESEARCH_CHECK_WORK* work ); // SUB-OBJ ���\�[�X �������
static void RegisterMainObjResources( RESEARCH_CHECK_WORK* work ); // MAIN-OBJ ���\�[�X��o�^����
static void ReleaseMainObjResources( RESEARCH_CHECK_WORK* work ); // MAIN-OBJ ���\�[�X���������
static void InitClactUnits( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���j�b�g��j������
static void InitClactWorks( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N������������
static void CreateClactWorks( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RESEARCH_CHECK_WORK* work ); // �Z���A�N�^�[���[�N��j������
static u32 GetObjResourceRegisterIndex( const RESEARCH_CHECK_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
static GFL_CLUNIT* GetClactUnit( const RESEARCH_CHECK_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RESEARCH_CHECK_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
// 3D
static void Setup3D(); // 3D �`��̃Z�b�g�A�b�v���s��
// �f�[�^�̏�����/����/�j��
static void InitSeqQueue( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ ������
static void CreateSeqQueue( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ �쐬
static void DeleteSeqQueue( RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[ �j��
static void InitFont( RESEARCH_CHECK_WORK* work ); // �t�H���g ������
static void CreateFont( RESEARCH_CHECK_WORK* work ); // �t�H���g ����
static void DeleteFont( RESEARCH_CHECK_WORK* work ); // �t�H���g �j��
static void InitMessages( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W ������
static void CreateMessages( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W ����
static void DeleteMessages( RESEARCH_CHECK_WORK* work ); // ���b�Z�[�W �j��
static void InitWordset( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g ������
static void CreateWordset( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g ����
static void DeleteWordset( RESEARCH_CHECK_WORK* work ); // ���[�h�Z�b�g �j��
static void InitBGFonts( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g ������
static void CreateBGFonts( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g ����
static void DeleteBGFonts( RESEARCH_CHECK_WORK* work ); // ������`��I�u�W�F�N�g �j��
static void InitCircleGraphs( RESEARCH_CHECK_WORK* work ); // �~�O���t ������
static void CreateCircleGraph( RESEARCH_CHECK_WORK* work ); // �~�O���t ����
static void DeleteCircleGraph( RESEARCH_CHECK_WORK* work ); // �~�O���t �j��
static void InitResearchData( RESEARCH_CHECK_WORK* work ); // �����f�[�^ ������
static void SetupResearchData( RESEARCH_CHECK_WORK* work ); // �����f�[�^ �擾
static void SetupTouchArea( RESEARCH_CHECK_WORK* work ); // �^�b�`�̈� ����
static void InitArrow( RESEARCH_CHECK_WORK* work ); // ��� ������
static void CreateArrow( RESEARCH_CHECK_WORK* work ); // ��� ����
static void DeleteArrow( RESEARCH_CHECK_WORK* work ); // ��� �j��
static void InitPercentage( RESEARCH_CHECK_WORK* work ); // % �\���I�u�W�F�N�g ������
static void CreatePercentage( RESEARCH_CHECK_WORK* work ); // % �\���I�u�W�F�N�g ����
static void DeletePercentage( RESEARCH_CHECK_WORK* work ); // % �\���I�u�W�F�N�g �j��
//-----------------------------------------------------------------------------------------
// ��LAYER -1 �f�o�b�O
//-----------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_CHECK_WORK* work ); // �V�[�P���X�L���[�̒��g��\������
static void DebugPrint_researchData( const RESEARCH_CHECK_WORK* work ); // �����f�[�^��\������
static void Debug_SetupResearchData( RESEARCH_CHECK_WORK* work ); // �f�o�b�O�p�����f�[�^��ݒ肷��


//=========================================================================================
// �������񍐊m�F��� ����֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̐���
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return �����񍐊m�F��ʃ��[�N
 */
//-----------------------------------------------------------------------------------------
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
  work->updateFlag      = FALSE;
  work->questionIdx     = 0;
  work->answerIdx       = 0;
  work->dispType        = DATA_DISP_TYPE_TODAY;
  work->VBlankTCBSystem = GFUser_VIntr_GetTCBSYS();
  work->percentageNum   = 0;
  work->percentageDispNum = 0;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitResearchData( work );
  InitSeqQueue( work );
  InitCircleGraphs( work );
  InitArrow( work );
  InitPercentage( work );
  InitMessages( work );
  InitWordset( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitBitmapDatas( work );
  InitPaletteFadeSystem( work );
  InitPaletteAnime( work );

  CreateSeqQueue( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create work\n" );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��� ���C������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
RESEARCH_CHECK_RESULT ResearchCheckMain( RESEARCH_CHECK_WORK* work )
{
  // �V�[�P���X���Ƃ̏���
  switch( work->seq ) {
  case RESEARCH_CHECK_SEQ_SETUP:      MainSeq_SETUP( work );      break;
  case RESEARCH_CHECK_SEQ_STANDBY:    MainSeq_STANDBY( work );    break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:   MainSeq_KEY_WAIT( work );   break;
  case RESEARCH_CHECK_SEQ_ANALYZE:    MainSeq_ANALYZE( work );    break;
  case RESEARCH_CHECK_SEQ_PERCENTAGE: MainSeq_PERCENTAGE( work ); break;
  case RESEARCH_CHECK_SEQ_FLASH_OUT:  MainSeq_FLASH_OUT( work );  break;
  case RESEARCH_CHECK_SEQ_FLASH_IN:   MainSeq_FLASH_IN( work );   break;
  case RESEARCH_CHECK_SEQ_UPDATE:     MainSeq_UPDATE( work );     break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:   MainSeq_FADE_OUT( work );   break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:   MainSeq_CLEAN_UP( work );   break;
  case RESEARCH_CHECK_SEQ_FINISH:     return work->result;
  default: GF_ASSERT(0);
  }

  GFL_CLACT_SYS_Main();       // �Z���A�N�^�[�V�X�e�� ���C������
  UpdateCircleGraphs( work ); // �~�O���t ���C������
  ARROW_Main( work->arrow );  // ��� ���C������
  RESEARCH_COMMON_UpdatePaletteAnime( work->commonWork ); // ���ʃp���b�g�A�j���[�V�������X�V
  UpdatePaletteAnime( work ); // �p���b�g�A�j���[�V�����X�V

  CountUpSeqCount( work ); // �V�[�P���X�J�E���^�X�V
  SwitchSeq( work );  // �V�[�P���X�X�V

  // 3D�`��
  DrawCircleGraphs( work ); // �~�O���t��`��
  DrawAnswerMarker( work ); // �񓚃}�[�J�[��`��
  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );

  return RESEARCH_CHECK_RESULT_CONTINUE;
}




//=========================================================================================
// ��LAYER 4 �V�[�P���X����
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_CHECK_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SETUP( RESEARCH_CHECK_WORK* work )
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
  //CreateClactSystem( work );
  RegisterSubObjResources( work );
  RegisterMainObjResources( work );
  CreateClactUnits( work );
  CreateClactWorks( work );

  // BMP-OAM ����
  CreateBitmapDatas( work );
  SetupBitmapData_forDefault( work );
  SetupBitmapData_forANALYZE_BUTTON( work );
  SetupBmpOamSystem( work );
  CreateBmpOamActors( work );

  SetupResearchData( work );   // �����f�[�^���擾
  //Debug_SetupResearchData( work ); // TEST:
  //DebugPrint_researchData( work ); // TEST: 
  CreateCircleGraph( work );       // �~�O���t �쐬
  CreateArrow( work );             // ��� �쐬
  CreatePercentage( work );        // % �\���I�u�W�F�N�g����

  SetupPaletteFadeSystem( work ); // �p���b�g�t�F�[�h�V�X�e�� ����
  CreatePaletteAnime( work );     // �p���b�g�A�j���[�V�������[�N�𐶐�
  SetupPaletteAnime( work );      // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v
  RegisterVBlankTask( work ); // VBkank�^�X�N�o�^

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0);

  SetNextSeq( work, RESEARCH_CHECK_SEQ_STANDBY ); // ���̃V�[�P���X���Z�b�g
  FinishCurrentSeq( work ); // �V�[�P���X�I��
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_CHECK_SEQ_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_STANDBY( RESEARCH_CHECK_WORK* work )
{
  int key;
  int trg;
  int touch;
  int commonTouch;

  key = GFL_UI_KEY_GetCont();
  trg = GFL_UI_KEY_GetTrg(); 
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //�u���ǂ�v�{�^��
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  }

  // �L�[���� of�u�~�O���t�v�^�b�`
  if( (trg & PAD_KEY_UP) ||
      (trg & PAD_KEY_DOWN) ||
      (trg & PAD_KEY_LEFT) ||
      (trg & PAD_KEY_RIGHT) ||
      (trg & PAD_BUTTON_A) ||
      (touch == TOUCH_AREA_GRAPH) ) 
  {
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  //�u����v�{�^��
  if( touch == TOUCH_AREA_QUESTION ) {
    MoveMenuCursorDirect( work, MENU_ITEM_ANSWER );
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      SetNextSeq( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSeq( work );
    }
    return;
  }
  //�u�񓚁v�{�^��
  if( touch == TOUCH_AREA_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_ANSWER );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  } 
  //�u�����̉񓚁v�{�^��
  if( touch == TOUCH_AREA_MY_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_MY_ANSWER );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  } 
  //�u�񓚐l���v�{�^��
  if( touch == TOUCH_AREA_COUNT ) {
    MoveMenuCursorDirect( work, MENU_ITEM_COUNT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  // ���J�[�\��
  if( touch == TOUCH_AREA_CONTROL_CURSOR_L ) {
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToPrev( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToPrev( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  // �E�J�[�\��
  if( touch == TOUCH_AREA_CONTROL_CURSOR_R ) {
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToNext( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToNext( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    // �V�[�P���X�ύX
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_CHECK_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  int key;
  int trg;
  int touch;
  int commonTouch;

  key = GFL_UI_KEY_GetCont();
  trg = GFL_UI_KEY_GetTrg(); 
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  // �����f�[�^�̍X�V�����o
  if( (work->analyzeFlag == TRUE) && (GAMEBEACON_Get_NewEntry() == TRUE) ) {
    // �X�V�V�[�P���X��
    SetNextSeq( work, RESEARCH_CHECK_SEQ_UPDATE );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  } 

  // TEST:
  if( trg & PAD_BUTTON_DEBUG ) {
    SetNextSeq( work, RESEARCH_CHECK_SEQ_UPDATE );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
    return;
  }

  //�u���ǂ�v�{�^��
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  }

  // ���L�[
  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work );
    return;
  } 
  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work );
    return;
  } 

  //�u�~�O���t�v�^�b�`
  if( touch == TOUCH_AREA_GRAPH ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      SetNextSeq( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSeq( work );
    }
    return;
  }
  //�u�ق��������݂�v�{�^��
  if( touch == TOUCH_AREA_ANALYZE_BUTTON ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSeq( work );
    }
    return;
  }
  //�u����v�{�^��
  if( touch == TOUCH_AREA_QUESTION ) {
    MoveMenuCursorDirect( work, MENU_ITEM_QUESTION );
    return;
  }
  //�u�񓚁v�{�^��
  if( touch == TOUCH_AREA_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_ANSWER );
    return;
  } 
  //�u�����̉񓚁v�{�^��
  if( touch == TOUCH_AREA_MY_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_MY_ANSWER );
    return;
  } 
  //�u�񓚐l���v�{�^��
  if( touch == TOUCH_AREA_COUNT ) {
    MoveMenuCursorDirect( work, MENU_ITEM_COUNT );
    return;
  }

  // ���L�[ or ���J�[�\��
  if( (trg & PAD_KEY_LEFT) || (touch == TOUCH_AREA_CONTROL_CURSOR_L) ) {
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToPrev( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToPrev( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
  }

  // ���L�[ or �E�J�[�\��
  if( (trg & PAD_KEY_RIGHT) || (touch == TOUCH_AREA_CONTROL_CURSOR_R) ) {
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToNext( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToNext( work );    break;
    case MENU_ITEM_MY_ANSWER: break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    default: GF_ASSERT(0);
    }
  }

  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    if( (work->analyzeFlag == FALSE ) &&
        (GetCountOfQuestion(work) != 0) && 
        (work->cursorPos == MENU_ITEM_QUESTION)  ) {
      // �V�[�P���X�ύX
      SetNextSeq( work, RESEARCH_CHECK_SEQ_ANALYZE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_OUT );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_FLASH_IN );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_PERCENTAGE );
      SetNextSeq( work, RESEARCH_CHECK_SEQ_KEY_WAIT );
      FinishCurrentSeq( work );
      return;
    }
  } 

  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    // �V�[�P���X�ύX
    SetNextSeq( work, RESEARCH_CHECK_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_CHECK_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��̓V�[�P���X ( RESEARCH_CHECK_SEQ_ANALYZE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_ANALYZE( RESEARCH_CHECK_WORK* work )
{ 
  // SE ����~���Ă���
  if( PMSND_CheckPlaySE() == FALSE ) { 
    // ��莞�Ԃ��o�߂Ŏ��̃V�[�P���X��
    if( SEQ_ANALYZE_FRAMES <= work->seqCount ) {
      FinishCurrentSeq( work );
    } 
    else { 
      // �f�[�^��͒�SE�����[�v������
      PMSND_PlaySE( SEQ_SE_SYS_81 );
    }
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��̓V�[�P���X ( RESEARCH_CHECK_SEQ_PERCENTAGE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work )
{ 
  // �V���ȁ���\��
  if( work->seqCount % 10 == 0 ) {
    DispPercentage( work, work->percentageDispNum++ );
  }

  // �S�Ă̐��l��\�������玟�̃V�[�P���X��
  if( work->percentageNum <= work->percentageDispNum ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʃt���b�V���V�[�P���X ( RESEARCH_CHECK_SEQ_FLASH_OUT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work )
{
  // �p���b�g�t�F�[�h�I��
  if( IsPaletteFadeEnd( work ) ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʃt���b�V���V�[�P���X ( RESEARCH_CHECK_SEQ_FLASH_IN ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FLASH_IN( RESEARCH_CHECK_WORK* work )
{
  // �p���b�g�t�F�[�h�I��
  if( IsPaletteFadeEnd( work ) ) {
    FinishCurrentSeq( work );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_CHECK_SEQ_UPDATE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_UPDATE( RESEARCH_CHECK_WORK* work )
{
  // SE ����~���Ă���
  if( PMSND_CheckPlaySE() == FALSE ) { 
    // ��莞�Ԃ��o��
    if( SEQ_UPDATE_FRAMES <= work->seqCount ) {
      FinishCurrentSeq( work ); // �V�[�P���X�I��
    } 
    else { 
      PMSND_PlaySE( SEQ_SE_SYS_81 ); // �f�[�^��M��SE�����[�v������
    }
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F�V�[�P���X�ւ̏����V�[�P���X ( RESEARCH_CHECK_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FADE_OUT( RESEARCH_CHECK_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSeq( work );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_CHECK_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 *
 * @return �V�[�P���X���ω�����ꍇ ���̃V�[�P���X�ԍ�
 *         �V�[�P���X���p������ꍇ ���݂̃V�[�P���X�ԍ�
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{ 
  // �p���b�g�A�j���[�V�������[�N
  CleanUpPaletteAnime( work );
  DeletePaletteAnime( work );

  // ���ʃp���b�g�A�j���[�V����
  RESEARCH_COMMON_StopAllPaletteAnime( work->commonWork ); // ��~����, 
  RESEARCH_COMMON_ResetAllPalette( work->commonWork );     // �p���b�g�����ɖ߂�

  ReleaseVBlankTask( work );        // VBlank�^�X�N������
  DeletePercentage( work );         // % �\���I�u�W�F�N�g�j��
  DeleteArrow( work );              // ��� �폜
  DeleteCircleGraph( work );        // �~�O���t �폜
  CleanUpPaletteFadeSystem( work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��

  // BMP-OAM ��Еt��
  DeleteBitmapDatas( work );
  DeleteBmpOamActors( work );
  CleanUpBmpOamSystem( work );

  // OBJ ��Еt��
  DeleteClactWorks( work );
  DeleteClactUnits( work );
  ReleaseSubObjResources ( work );
  ReleaseMainObjResources( work );
  //DeleteClactSystem ( work );

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
  SetNextSeq( work, RESEARCH_CHECK_SEQ_FINISH );
  FinishCurrentSeq( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishCurrentSeq( RESEARCH_CHECK_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish current Seq\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//-----------------------------------------------------------------------------------------
static void SetResult( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_CHECK_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set result (%d)\n", result );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_CHECK_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq )
  {
  case RESEARCH_CHECK_SEQ_SETUP:      maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_STANDBY:    maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:   maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_ANALYZE:    maxCount = SEQ_ANALYZE_FRAMES;   break;
  case RESEARCH_CHECK_SEQ_PERCENTAGE: maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_FLASH_OUT:  maxCount = SEQ_FLASH_OUT_FRAMES; break;
  case RESEARCH_CHECK_SEQ_FLASH_IN:   maxCount = SEQ_FLASH_IN_FRAMES;  break;
  case RESEARCH_CHECK_SEQ_UPDATE:     maxCount = SEQ_UPDATE_FRAMES;    break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:   maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:   maxCount = 0xffffffff;           break;
  case RESEARCH_CHECK_SEQ_FINISH:     maxCount = 0xffffffff;           break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < work->seqCount )
  { 
    work->seqCount = maxCount;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X��o�^����
 *
 * @param work
 * @param nextSeq �o�^����V�[�P���X
 */
//-----------------------------------------------------------------------------------------
static void SetNextSeq( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SwitchSeq( RESEARCH_CHECK_WORK* work )
{
  RESEARCH_CHECK_SEQ nextSeq;

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
static void SetSeq( RESEARCH_CHECK_WORK* work, RESEARCH_CHECK_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq ) {
  case RESEARCH_CHECK_SEQ_SETUP:      FinishSeq_SETUP( work );       break;
  case RESEARCH_CHECK_SEQ_STANDBY:    FinishSeq_STANDBY( work );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:   FinishSeq_KEY_WAIT( work );    break;
  case RESEARCH_CHECK_SEQ_ANALYZE:    FinishSeq_ANALYZE( work );     break;
  case RESEARCH_CHECK_SEQ_PERCENTAGE: FinishSeq_PERCENTAGE( work );  break;
  case RESEARCH_CHECK_SEQ_FLASH_OUT:  FinishSeq_FLASH_OUT( work );   break;
  case RESEARCH_CHECK_SEQ_FLASH_IN:   FinishSeq_FLASH_IN( work );    break;
  case RESEARCH_CHECK_SEQ_UPDATE:     FinishSeq_UPDATE( work );      break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:   FinishSeq_FADE_OUT( work );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:   FinishSeq_CLEAN_UP( work );    break;
  case RESEARCH_CHECK_SEQ_FINISH:     break;
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq           = nextSeq;
  work->seqCount      = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq ) {
  case RESEARCH_CHECK_SEQ_SETUP:      InitSeq_SETUP( work );       break;
  case RESEARCH_CHECK_SEQ_STANDBY:    InitSeq_STANDBY( work );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:   InitSeq_KEY_WAIT( work );    break;
  case RESEARCH_CHECK_SEQ_ANALYZE:    InitSeq_ANALYZE( work );     break;
  case RESEARCH_CHECK_SEQ_PERCENTAGE: InitSeq_PERCENTAGE( work );  break;
  case RESEARCH_CHECK_SEQ_FLASH_OUT:  InitSeq_FLASH_OUT( work );   break;
  case RESEARCH_CHECK_SEQ_FLASH_IN:   InitSeq_FLASH_IN( work );    break;
  case RESEARCH_CHECK_SEQ_UPDATE:     InitSeq_UPDATE( work );      break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:   InitSeq_FADE_OUT( work );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:   InitSeq_CLEAN_UP( work );    break;
  case RESEARCH_CHECK_SEQ_FINISH:     break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set seq ==> " );
  switch( nextSeq ) {
  case RESEARCH_CHECK_SEQ_SETUP:      OS_TFPrintf( PRINT_TARGET, "SETUP" );       break;
  case RESEARCH_CHECK_SEQ_STANDBY:    OS_TFPrintf( PRINT_TARGET, "STANDBY" );     break;
  case RESEARCH_CHECK_SEQ_KEY_WAIT:   OS_TFPrintf( PRINT_TARGET, "KEY_WAIT" );    break;
  case RESEARCH_CHECK_SEQ_ANALYZE:    OS_TFPrintf( PRINT_TARGET, "ANALYZE" );     break;
  case RESEARCH_CHECK_SEQ_PERCENTAGE: OS_TFPrintf( PRINT_TARGET, "PERCENTAGE" );  break;
  case RESEARCH_CHECK_SEQ_FLASH_OUT:  OS_TFPrintf( PRINT_TARGET, "FLASH_OUT" );   break;
  case RESEARCH_CHECK_SEQ_FLASH_IN:   OS_TFPrintf( PRINT_TARGET, "FLASH_IN" );    break;
  case RESEARCH_CHECK_SEQ_UPDATE:     OS_TFPrintf( PRINT_TARGET, "UPDATE" );      break;
  case RESEARCH_CHECK_SEQ_FADE_OUT:   OS_TFPrintf( PRINT_TARGET, "FADE_OUT" );    break;
  case RESEARCH_CHECK_SEQ_CLEAN_UP:   OS_TFPrintf( PRINT_TARGET, "CLEAN_UP" );    break;
  case RESEARCH_CHECK_SEQ_FINISH:     OS_TFPrintf( PRINT_TARGET, "FINISH" );      break;
  default:  GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_SETUP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_SETUP( RESEARCH_CHECK_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq SETUP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_STANDBY )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_STANDBY( RESEARCH_CHECK_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq STANDBY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_KEY_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  SetMenuCursorOn( work );     // �J�[�\��������Ă����Ԃɂ���

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq KEY_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_ANALYZE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_ANALYZE( RESEARCH_CHECK_WORK* work )
{
  //�u�c�����������イ�c�v��\��
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, TRUE );

  // �~�O���t�쐬
  SetupMainCircleGraph( work, DATA_DISP_TYPE_TODAY );
  SetupMainCircleGraph( work, DATA_DISP_TYPE_TOTAL );

  // �~�O���t�\���J�n
  CIRCLE_GRAPH_SetDrawEnable( GetMainGraph(work), TRUE );
  CIRCLE_GRAPH_AnalyzeReq( GetMainGraph(work) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq ANALYZE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_PERCENTAGE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work )
{
  SetupPercentages( work ); // ���\���I�u�W�F�N�g���Z�b�g�A�b�v

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq PERCENTAGE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_FLASH_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work )
{
  // �p���b�g�t�F�[�h�J�n
  StartPaletteFadeFlashOut( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq FLASH_OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_FLASH_IN )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FLASH_IN( RESEARCH_CHECK_WORK* work )
{
  // �p���b�g�t�F�[�h�J�n
  StartPaletteFadeFlashIn( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq FLASH_IN\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_UPDATE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_UPDATE( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->updateFlag == FALSE ); // �s���Ăяo��

  // �X�V�J�n
  work->updateFlag = TRUE;

  // �����f�[�^���ăZ�b�g�A�b�v
  SetupResearchData( work );
  //Debug_SetupResearchData( work ); // TEST:

  // �\�����X�V
  SetMenuCursorOff( work );           // �J�[�\��������Ă��Ȃ���Ԃɂ���
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  UpdateBGFont_Answer( work );        //�u�񓚁v������̕\�����X�V����
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
  UpdateArrow( work );                // ���̕\�����X�V����
  UpdateControlCursor( work );        // ���E�̃J�[�\���\�����X�V����
  VanishAllPercentage( work );        // ���\������������

  // �T�u�~�O���t�쐬
  SetupSubCircleGraph( work, DATA_DISP_TYPE_TODAY );
  SetupSubCircleGraph( work, DATA_DISP_TYPE_TOTAL );

  // �~�O���t�̏d�Ȃ���𒲐�
  AdjustCircleGraphLayer( work );

  // �~�O���t�\���J�n
  CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), TRUE );
  CIRCLE_GRAPH_UpdateReq( GetSubGraph(work) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq UPDATE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_FADE_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_FADE_OUT( RESEARCH_CHECK_WORK* work )
{ 
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq FADE_OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X������������ ( ==> RESEARCH_CHECK_SEQ_CLEAN_UP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq CLEAN_UP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_SETUP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_SETUP( RESEARCH_CHECK_WORK* work )
{
  UpdateBGFont_TopicTitle( work );      // �������ږ����X�V����
  UpdateBGFont_QuestionCaption( work ); // ����̕⑫�����X�V����
  UpdateBGFont_Question( work );        // ������X�V���� 
  UpdateBGFont_Answer( work );          // �񓚂��X�V����
  UpdateBGFont_MyAnswer( work );        // �����̉񓚂��X�V����
  UpdateBGFont_Count( work );           // �񓚐l�����X�V����
  UpdateBGFont_NoData( work );          //�u�������� ���傤�����イ�v�̕\�����X�V����
  UpdateBGFont_DataReceiving( work );   //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
  UpdateControlCursor( work );          // ���E�J�[�\�����X�V
  UpdateMainBG_WINDOW( work );          // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  UpdateMyAnswerIconOnButton( work );   // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZE_BUTTON, TRUE ); //�u�ق��������݂�v�{�^����\��
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_RECEIVE_BUTTON ); //�u�f�[�^��M���v�{�^���̃p���b�g�A�j�����J�n

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq SETUP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_STANDBY )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_STANDBY( RESEARCH_CHECK_WORK* work )
{
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq STANDBY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_KEY_WAIT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_KEY_WAIT( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq KEY_WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_ANALYZE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_ANALYZE( RESEARCH_CHECK_WORK* work )
{
  // ��͍ς݃t���O�𗧂Ă�
  work->analyzeFlag = TRUE;

  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, FALSE ); //�u�c�����������イ�c�v������
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_Answer( work );        // �񓚂��X�V����
  UpdateBGFont_MyAnswer( work );      // �����̉񓚂��X�V����
  UpdateBGFont_Count( work );         // �񓚐l�����X�V����
  UpdateArrow( work );                // �����X�V����
  UpdateMyAnswerIconOnButton( work ); // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����

  // �������ʕ\��SE
  PMSND_PlaySE( SEQ_SE_SYS_82 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq ANALYZE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_PERCENTAGE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_PERCENTAGE( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq PERCENTAGE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_FLASH_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FLASH_OUT( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq FLASH_OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_FLASH_IN )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FLASH_IN( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq FLASH_IN\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_UPDATE )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_UPDATE( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->updateFlag == TRUE ); // �s���Ăяo��

  // �X�V�I��
  work->updateFlag = FALSE;

  // �~�O���t���X�V
  InterchangeCircleGraph( work ); // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
  CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), FALSE ); // �T�u�~�O���t ( �����C�� ) �\���I��

  // �\�����X�V
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
  UpdateBGFont_Answer( work );        // �񓚂��X�V����
  UpdateArrow( work );                // �����X�V����
  UpdateControlCursor( work );        // ���E�̃J�[�\���\�����X�V����
  DispAllPercentage( work );          // ���\�L��\������

  // �X�V����SE
  PMSND_PlaySE( SEQ_SE_SYS_82 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq UPDATE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_FADE_OUT )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_FADE_OUT( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq FADE_OUT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�I������ ( ==> RESEARCH_CHECK_SEQ_CLEAN_UP )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishSeq_CLEAN_UP( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: finish seq CLEAN_UP\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ׂẲ~�O���t���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateCircleGraphs( RESEARCH_CHECK_WORK* work )
{
  int typeIdx;

  for( typeIdx=0; typeIdx < DATA_DISP_TYPE_NUM; typeIdx++ )
  {
    CIRCLE_GRAPH_Main( work->mainGraph[ typeIdx ] );
    CIRCLE_GRAPH_Main( work->subGraph[ typeIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ׂẲ~�O���t���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DrawCircleGraphs( const RESEARCH_CHECK_WORK* work )
{
  int typeIdx;

  for( typeIdx=0; typeIdx < DATA_DISP_TYPE_NUM; typeIdx++ )
  {
    CIRCLE_GRAPH_Draw( work->mainGraph[ typeIdx ] );
    CIRCLE_GRAPH_Draw( work->subGraph[ typeIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �񓚃}�[�J�[��`�悷��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DrawAnswerMarker( const RESEARCH_CHECK_WORK* work )
{
  int colorR, colorG, colorB;
  GXRgb color;
  const RESEARCH_DATA* data;

  // ����� or �X�V���͕\�����Ȃ�
  if( (work->analyzeFlag == FALSE) || (work->updateFlag == TRUE) ) { return; }

  // �\�����̉񓚂̃J���[���擾
  data = &(work->researchData);
  colorR = RESEARCH_DATA_GetColorR( data, work->questionIdx, work->answerIdx );
  colorG = RESEARCH_DATA_GetColorG( data, work->questionIdx, work->answerIdx );
  colorB = RESEARCH_DATA_GetColorB( data, work->questionIdx, work->answerIdx );
  color = GX_RGB( colorR, colorG, colorB );

  // �|���S���֘A�����l��ݒ�
  G3_PolygonAttr( GX_LIGHTMASK_NONE,        // no lights
                  GX_POLYGONMODE_MODULATE,  // modulation mode
                  GX_CULL_NONE,             // cull none
                  0,                        // polygon ID(0 - 63)
                  31,                       // alpha(0 - 31)
                  0 );                      // OR of GXPolygonAttrMisc's value

  // �`��|���S����o�^
  G3_Begin( GX_BEGIN_QUADS ); // �l�p�`�|���S��
  G3_Color( color ); // ���_�J���[
  G3_Vtx( ANSWER_MARKER_LEFT,  ANSWER_MARKER_TOP,    ANSWER_MARKER_Z ); // ���_1
  G3_Vtx( ANSWER_MARKER_LEFT,  ANSWER_MARKER_BOTTOM, ANSWER_MARKER_Z ); // ���_2
  G3_Vtx( ANSWER_MARKER_RIGHT, ANSWER_MARKER_BOTTOM, ANSWER_MARKER_Z ); // ���_3
  G3_Vtx( ANSWER_MARKER_RIGHT, ANSWER_MARKER_TOP,    ANSWER_MARKER_Z ); // ���_4 
  G3_End();
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
  RESEARCH_CHECK_WORK* work = (RESEARCH_CHECK_WORK*)wk;

  GFL_BG_VBlankFunc(); // BG
  GFL_CLACT_SYS_VBlankFunc(); // OBJ
  PaletteFadeTrans( work->paletteFadeSystem ); // �p���b�g�t�F�[�h
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
static void MoveMenuCursorUp( RESEARCH_CHECK_WORK* work )
{ 
  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, -1 );

  // �J�[�\�����u�񓚁v�u�����̉񓚁v�̈ʒu�ɂ���ꍇ
  while( (work->cursorPos == MENU_ITEM_ANSWER) || (work->cursorPos == MENU_ITEM_MY_ANSWER) )
  {
    if( work->analyzeFlag == FALSE ) { // �����
      ShiftMenuCursorPos( work, -1 ); 
    }
    else if( GetCountOfQuestion(work) == 0 ) { //�u�������� ���傤�����イ�v
      ShiftMenuCursorPos( work, -1 ); 
    }
    else {
      break;
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V
  UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �^�b�`�͈͂��X�V
  UpdateTouchArea( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorDown( RESEARCH_CHECK_WORK* work )
{
  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, 1 );

  // �J�[�\�����u�񓚁v�u�����̉񓚁v�̈ʒu�ɂ���ꍇ
  while( (work->cursorPos == MENU_ITEM_ANSWER) || (work->cursorPos == MENU_ITEM_MY_ANSWER) )
  {
    if( work->analyzeFlag == FALSE ) { // �����
      ShiftMenuCursorPos( work, 1 ); 
    }
    else if( GetCountOfQuestion(work) == 0 ) { //�u�������� ���傤�����イ�v
      ShiftMenuCursorPos( work, 1 ); 
    }
    else {
      break;
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V
  UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �^�b�`�͈͂��X�V
  UpdateTouchArea( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param menuItem �ړ���̃��j���[����
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorDirect( RESEARCH_CHECK_WORK* work, MENU_ITEM menuItem )
{
  // �ړ��悪�u�񓚁v�u�����̉񓚁v�̏ꍇ
  if( (menuItem == MENU_ITEM_ANSWER) || (menuItem == MENU_ITEM_MY_ANSWER) )
  {
    if( (work->analyzeFlag == FALSE) ||   // �����
        (GetCountOfQuestion(work) == 0) ) //�u�������� ���傤�����イ�v
    { // �ړ����󂯕t���Ȃ�
      return;
    }
  }

  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  SetMenuCursorPos( work, menuItem );

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V
  UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �^�b�`�͈͂��X�V
  UpdateTouchArea( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿���, ���̎���ɕύX����
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
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
  VanishAllPercentage( work );          // ���\����S����
  UpdateMyAnswerIconOnButton( work );   // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿���, �O�̎���ɕύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
  VanishAllPercentage( work );          // ���\����S����
  UpdateMyAnswerIconOnButton( work );   // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, ���̉񓚂ɕύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ChangeAnswerToNext( RESEARCH_CHECK_WORK* work )
{
  ShiftAnswerIdx( work, 1 ); // �\������񓚃C���f�b�N�X��ύX

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
  UpdateArrow( work );         // �����X�V

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, �O�̉񓚂ɕύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ChangeAnswerToPrev( RESEARCH_CHECK_WORK* work )
{
  ShiftAnswerIdx( work, -1 ); // �\������񓚃C���f�b�N�X��ύX

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
  UpdateArrow( work );         // �����X�V

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\������񓚂�, �擪�̉񓚂ɕύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ChangeAnswerToTop( RESEARCH_CHECK_WORK* work )
{
  ResetAnswerIdx( work ); // �\������񓚃C���f�b�N�X�����Z�b�g

  // �\�����X�V
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
  UpdateArrow( work );         // �����X�V
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^�̕\���^�C�v��؂�ւ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SwitchDataDisplayType( RESEARCH_CHECK_WORK* work )
{
  CIRCLE_GRAPH* graph;
  DATA_DISP_TYPE nextType;
  BOOL newGraphWait = FALSE; // �V�����\������~�O���t��, �\������܂ł̑҂����Ԃ��K�v���ǂ���

  // �~�O���t�̃A�j���[�V�������͎󂯕t���Ȃ�
  if( CheckAllGraphAnimeEnd(work) == FALSE ) { return; }

  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
    newGraphWait = TRUE;
  }

  // �\���^�C�v��ύX
  switch( work->dispType ) {
  case DATA_DISP_TYPE_TODAY: nextType = DATA_DISP_TYPE_TOTAL; break;
  case DATA_DISP_TYPE_TOTAL: nextType = DATA_DISP_TYPE_TODAY; break;
  default: GF_ASSERT(0);
  } 
  SetDataDisplayType( work, nextType );

  // �\�����X�V
  UpdateMainBG_WINDOW( work ); // MAIN-BG ( �E�B���h�E�� ) ���X�V
  SetMenuCursorOn( work );     // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_Answer( work ); // �񓚂��X�V����
  UpdateBGFont_Count( work );  // �񓚐l�����X�V����
  UpdateBGFont_NoData( work ); //�u�������� ���傤�����イ�v�̕\�����X�V����
  UpdateArrow( work );         // �����X�V����

  // ��͍ς݂̏ꍇ�݂̂̍X�V
  if( work->analyzeFlag ) {
    VanishAllPercentage( work ); // ���I�u�W�F�N�g��S����
    SetupPercentages( work );    // ���I�u�W�F�N�g���ăZ�b�g�A�b�v
    DispAllPercentage( work );   // ���I�u�W�F�N�g��S�\��
  }

  // �~�O���t���o��������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    if( newGraphWait ) { CIRCLE_GRAPH_StopGraph( GetMainGraph(work), 20 ); } // �O�O���t�̏�����҂�
    CIRCLE_GRAPH_AppearReq( GetMainGraph(work) );
    CIRCLE_GRAPH_SetDrawEnable( GetMainGraph(work), TRUE );
  }

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���𒼐ڎw�肵�ĕύX����
 *
 * @param work
 * @param menuItem �ύX��̃J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorPos( RESEARCH_CHECK_WORK* work, MENU_ITEM menuItem )
{
  // �J�[�\���ʒu���X�V
  work->cursorPos = menuItem;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: set menu cursor ==> %d\n", menuItem );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿��̃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �����N�ύX��
 */
//-----------------------------------------------------------------------------------------
static void ShiftAnswerIdx( RESEARCH_CHECK_WORK* work, int stride )
{
  int questionID;
  int answerID, nextAnswerID;
  int answerRank, nextAnswerRank;
  int answerNum;
  int nextAnswerIdx;
  CIRCLE_GRAPH* graph;
  
  // ��{�����擾
  graph = GetMainGraph( work );
  questionID = GetQuestionID( work );
  answerNum = GetAnswerNum( work );

  // �\�����̉񓚂̃����N���擾
  answerID = GetAnswerID( work );
  answerRank = CIRCLE_GRAPH_GetComponentRank_byID( graph, answerID );

  // �X�V��̃����N�̉񓚃C���f�b�N�X���擾
  nextAnswerRank = (answerRank + stride + answerNum) % answerNum;
  nextAnswerID = CIRCLE_GRAPH_GetComponentID_byRank( graph, nextAnswerRank );
  nextAnswerIdx = RESEARCH_DATA_GetAnswerIndex_byID( &(work->researchData), questionID, nextAnswerID );

  // �C���f�b�N�X��ύX
  work->answerIdx = nextAnswerIdx;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: shift answer index ==> %d\n", nextAnswerIdx );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ResetAnswerIdx( RESEARCH_CHECK_WORK* work )
{
  work->answerIdx = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: reset answer index ==> %d\n", work->answerIdx );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^�̕\���^�C�v��ݒ肷��
 *
 * @param work
 * @param dispType �ݒ肷��\���^�C�v
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorOn( RESEARCH_CHECK_WORK* work )
{
  // �Y������X�N���[�����X�V
  switch( work->cursorPos ) {
  //�u����v�u�����̉񓚁v�u�񓚐l���v
  case MENU_ITEM_QUESTION:  
  case MENU_ITEM_MY_ANSWER: 
  case MENU_ITEM_COUNT:     
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, ���j���[���ڂ��J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateArrow( RESEARCH_CHECK_WORK* work )
{
  // ����
  ARROW_Vanish( work->arrow );

  // �X�V���͕\�����Ȃ�
  if( work->updateFlag == TRUE ) { return; }

  // �\���J�n
  if( work->analyzeFlag ) {
    int endX, endY;
    CIRCLE_GRAPH_GetComponentPointPos_byID( GetMainGraph(work), GetAnswerID(work), &endX, &endY );
    ARROW_Setup( work->arrow, ARROW_START_X, ARROW_START_Y, endX, endY );
    ARROW_StartAnime( work->arrow );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update arrow\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���\���I�u�W�F�N�g��, ���݂̒����f�[�^�ɂ��킹�ăZ�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupPercentages( RESEARCH_CHECK_WORK* work )
{
  int rank;
  int answerNum;
  int percentageCount;
  CIRCLE_GRAPH* graph;

  answerNum = GetAnswerNum( work );
  graph = GetMainGraph( work );
  percentageCount = 0;

  // �����N���Ⴂ�\���v�f���猩�Ă���
  for( rank=answerNum-1; 0 <= rank; rank-- )
  {
    int x, y, num;
    PERCENTAGE* percentage;

    // �G���[�`�F�b�N
    GF_ASSERT( percentageCount < PERCENTAGE_NUM ); // �\���ł���ő吔���I�[�o�[

    // ���l�E���W���擾
    num = CIRCLE_GRAPH_GetComponentPercentage_byRank( graph, rank );
    CIRCLE_GRAPH_GetComponentPointPos_byRank( graph, rank, &x, &y );

    // ���l�����̗v�f�͖���
    if( num < MIN_PERCENTAGE ) { continue; }

    // ���l�E���W��ݒ�
    percentage = work->percentage[ percentageCount ];
    PERCENTAGE_SetValue( percentage, num );
    PERCENTAGE_SetPos( percentage, x, y );

    // �L���ȁ��I�u�W�F�N�g�̐����J�E���g
    percentageCount++;
  }

  // �L���ȁ��I�u�W�F�N�g�̐����L��
  work->percentageNum = percentageCount;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup percentages\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�Ắ��\������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void VanishAllPercentage( RESEARCH_CHECK_WORK* work )
{
  int idx;

  // ���ׂĔ�\���ɂ���
  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    PERCENTAGE_SetDrawEnable( work->percentage[ idx ], FALSE );
  }
  work->percentageDispNum = 0;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: vanish all percentage\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���I�u�W�F�N�g��\������
 *
 * @param work
 * @param index �\�����道�I�u�W�F�N�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
static void DispPercentage( RESEARCH_CHECK_WORK* work, u8 index )
{
  PERCENTAGE_SetDrawEnable( work->percentage[ index ], TRUE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: disp percentage[%d]\n", index );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�Ắ��I�u�W�F�N�g��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DispAllPercentage( RESEARCH_CHECK_WORK* work )
{
  int idx;
  int num;

  num = work->percentageNum;

  for( idx=0; idx < num; idx++ )
  {
    DispPercentage( work, idx );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: disp all percentage\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �^�b�`�͈͂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-BG ( �E�B���h�E�� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateMainBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  ARCHANDLE* handle;
  ARCDATID datID;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, work->heapID ); 

  // ���f������X�N���[���f�[�^������
  if( work->updateFlag ) {
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ږ����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_TopicTitle( RESEARCH_CHECK_WORK* work )
{
  u32 topicID;
  u32 strID;

  // �������ڂ̕⑫���̕�����ID���擾
  topicID = work->researchData.topicID;
  strID = StringID_topicTitle[ topicID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( work->BGFont[ SUB_BG_FONT_TOPIC_TITLE ], strID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update BGFont topic title\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����̕⑫�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ������X�V���� 
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �񓚂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_Answer( RESEARCH_CHECK_WORK* work )
{
  u16 answerID;
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

  // ��͑O or �X�V���Ȃ�\�����Ȃ�
  if( (work->analyzeFlag == FALSE) || (work->updateFlag == TRUE) ) {
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
  answerRank = CIRCLE_GRAPH_GetComponentRank_byID( graph, answerID );
  count      = GetCountOfAnswer( work );
  percentage = CIRCLE_GRAPH_GetComponentPercentage_byID( graph, answerID );

  // ���l��񓚂𕶎���ɓW�J
  {
    strbuf_plain  = GFL_MSG_CreateString( work->message[ MESSAGE_STATIC ], str_check_answer );
    strbuf_answer = GFL_MSG_CreateString( work->message[ MESSAGE_ANSWER ], answerID );
    strbuf_expand = GFL_STR_CreateBuffer( 128, work->heapID );
    WORDSET_RegisterNumber( work->wordset, 0, answerRank+1, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT ); // ���Ԗڂ�
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_MyAnswer( RESEARCH_CHECK_WORK* work )
{
  u16 answerID;
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_expand; // �W�J��̕�����
  STRBUF* strbuf_myAnswer;  // �����̉񓚕�����

  // ��͑O�Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_MY_ANSWER ], FALSE );
    return;
  }

  // ���ݕ\�����̎���ɑ΂���, �����̉�ID���擾
  answerID = GetMyAnswerID( work );

  // �񓚕������W�J
  {
    strbuf_plain    = GFL_MSG_CreateString( work->message[ MESSAGE_STATIC ], str_check_my_answer );
    strbuf_myAnswer = GFL_MSG_CreateString( work->message[ MESSAGE_ANSWER ], answerID );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �񓚐l�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_Count( RESEARCH_CHECK_WORK* work )
{
  u32 strID; 
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_expand; // �W�J��̕�����

  // ������ID������
  if( work->analyzeFlag ) { // ��͍ς�
    switch( work->dispType ) {
    case DATA_DISP_TYPE_TODAY: strID = str_check_today_count; break;
    case DATA_DISP_TYPE_TOTAL: strID = str_check_total_count; break;
    default: GF_ASSERT(0);
    }
  }
  else { // �����
    switch( work->dispType ) {
    case DATA_DISP_TYPE_TODAY: strID = str_check_today_count01; break;
    case DATA_DISP_TYPE_TOTAL: strID = str_check_total_count01; break;
    default: GF_ASSERT(0);
    }
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

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�������� ���傤�����イ�v�̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @breif�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ����J�[�\���̕\�����X�V����
 *
 * @param work 
 */
//-----------------------------------------------------------------------------------------
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

  // �X�V�� or�u�����̉񓚁v��I�����Ă���ꍇ�͕\�����Ȃ�
  if( (work->updateFlag == TRUE ) || (work->cursorPos == MENU_ITEM_MY_ANSWER) ) {
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateMyAnswerIconOnButton( RESEARCH_CHECK_WORK* work )
{
  // ����͂Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    GFL_CLACT_WK_SetDrawEnable( work->clactWork[ CLWK_MY_ANSWER_ICON_ON_BUTTON ], FALSE );
    return;
  }

  // �\��
  GFL_CLACT_WK_SetDrawEnable( work->clactWork[ CLWK_MY_ANSWER_ICON_ON_BUTTON ], TRUE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update my answer icon on button\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
void UpdateMyAnswerIconOnGraph( RESEARCH_CHECK_WORK* work )
{
  int x, y;
  GFL_CLACTPOS pos;
  GFL_CLWK* clwk;
  const CLWK_INIT_DATA* initData;
  int answerID;

  clwk = work->clactWork[ CLWK_MY_ANSWER_ICON_ON_GRAPH ];
  initData = &( ClactWorkInitData[ CLWK_MY_ANSWER_ICON_ON_GRAPH ] );

  // ����͂Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
    return;
  }

  // �J�[�\���ʒu���u�����̉񓚁v�łȂ��Ȃ�\�����Ȃ�
  if( work->cursorPos != MENU_ITEM_MY_ANSWER ) {
    GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
    return;
  }

  // �����̉�ID���擾
  answerID = GetMyAnswerID( work );

  // ���񓚂Ȃ�\�����Ȃ�
  if( answerID == ANSWER_ID_000 ) {
    GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
    return; 
  }

  // �\��
  CIRCLE_GRAPH_GetComponentPointPos_byID( GetMainGraph(work), answerID, &x, &y );
  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( clwk, &pos, initData->setSurface );
  GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: update my answer icon on graph\n" );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init seq queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL );

  // �쐬
  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create seq queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->seqQueue );

  // �폜
  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete seq queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitFont( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init font\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateFont( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, FALSE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create font\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteFont( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete font\n" );
}


//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitWordset( RESEARCH_CHECK_WORK* work )
{
  // ������
  work->wordset = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init wordset\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateWordset( RESEARCH_CHECK_WORK* work )
{
  // ���d����
  GF_ASSERT( work->wordset == NULL );

  // �쐬
  work->wordset = WORDSET_Create( work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create wordset\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteWordset( RESEARCH_CHECK_WORK* work )
{
  // �폜
  WORDSET_Delete( work->wordset );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete wordset\n" );
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
// @todo �Z�[�u�f�[�^�̏C���ɍ��킹�ĕύX����
#if 1
static u8 GetInvestigatingTopicID( const RESEARCH_CHECK_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;

  save   = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave = SaveData_GetQuestionnaire( save );

  // �Z�[�u�f�[�^����擾
  return QuestionnaireWork_GetInvestigatingQuestion( QSave, 0 );
}
#endif
#if 0
static u8 GetInvestigatingTopicID( const RESEARCH_CHECK_WORK* work )
{
  int qIdx;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u8 topicID;

  save  = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave = SaveData_GetQuestionnaire( save );

  // �������̎���ID���擾
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    questionID[ qIdx ] = QuestionnaireWork_GetInvestigatingQuestion( QSave, qIdx );
  }

  // ����ID����, ��������ID�����߂�
  for( topicID=0; topicID < TOPIC_ID_NUM; topicID++ )
  {
    if( (questionID[0] == Question1_topic[ topicID ]) &&
        (questionID[1] == Question2_topic[ topicID ]) &&
        (questionID[2] == Question3_topic[ topicID ]) ) { return topicID; }
  }

  // ����̑g�ݍ��킹�ɊY�����钲�����ڂ����݂��Ȃ�
  GF_ASSERT(0);
  return 0;
}
#endif

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, �����̉�ID���擾���� 
 * 
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �����̉�ID
 *         ���񓚂̏ꍇ, ANSWER_ID_000
 */
//-----------------------------------------------------------------------------------------
u8 GetMyAnswerID( const RESEARCH_CHECK_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  QUESTIONNAIRE_ANSWER_WORK* myAnswer;
  const RESEARCH_DATA* researchData;
  u8 questionID, questionIdx;
  u16 answerID, answerIdx;

  // �Z�[�u�f�[�^���擾
  save  = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave = SaveData_GetQuestionnaire( save );
  myAnswer = Questionnaire_GetAnswerWork( QSave );

  // ���ݕ\�����̎���ɑ΂���, �����̉񓚃C���f�b�N�X���擾
  researchData = &( work->researchData );
  questionIdx  = work->questionIdx;
  questionID   = GetQuestionID( work );
  answerIdx    = QuestionnaireAnswer_ReadBit( myAnswer, questionID );

  // ���񓚂��l������
  if( answerIdx == 0 ) {
    return ANSWER_ID_000;
  }
  else {
    answerIdx--;
  }

  // ��ID���擾
  answerID = RESEARCH_DATA_GetAnswerID_byIndex( researchData, questionIdx, answerIdx );

  return answerID;
}


//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up SUB-BG-WINDOW\n" );
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
static void SetupSubBG_FONT( RESEARCH_CHECK_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup SUB-BG-FONT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up SUB-BG-FONT\n" );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_CHECK_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up MAIN-BG-WINDOW\n" );
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
static void SetupMainBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup MAIN-BG-FONT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RESEARCH_CHECK_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up MAIN-BG-FONT\n" );
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
static void InitBGFonts( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < BG_FONT_NUM; idx++ )
  {
    work->BGFont[ idx ] = NULL; 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t�𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitResearchData( RESEARCH_CHECK_WORK* work )
{
  // 0�N���A
  GFL_STD_MemClear( &( work->researchData ), sizeof(RESEARCH_DATA) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init research data\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^���擾����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void SetupResearchData( RESEARCH_CHECK_WORK* work )
{
  int qIdx, aIdx;
  u8 topicID;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u8 answerNumOfQuestion[ QUESTION_NUM_PER_TOPIC ];
  u16 todayCountOfQuestion[ QUESTION_NUM_PER_TOPIC ];
  u16 totalCountOfQuestion[ QUESTION_NUM_PER_TOPIC ];
  u16 answerID[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  u16 todayCountOfAnswer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  u16 totalCountOfAnswer[ QUESTION_NUM_PER_TOPIC ][ MAX_ANSWER_NUM_PER_QUESTION ];
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( work->gameData );
  QSave = SaveData_GetQuestionnaire( save );
  topicID = QuestionnaireWork_GetInvestigatingQuestion( QSave, 0 ); // ��������ID
  questionID[0] = Question1_topic[ topicID ]; // ��������ID ==> ����ID
  questionID[1] = Question2_topic[ topicID ]; // ��������ID ==> ����ID
  questionID[2] = Question3_topic[ topicID ]; // ��������ID ==> ����ID
  answerNumOfQuestion[0] = AnswerNum_question[ questionID[0] ]; // ����ID ==> �񓚑I�����̐�
  answerNumOfQuestion[1] = AnswerNum_question[ questionID[1] ]; // ����ID ==> �񓚑I�����̐�
  answerNumOfQuestion[2] = AnswerNum_question[ questionID[2] ]; // ����ID ==> �񓚑I�����̐�
  todayCountOfQuestion[0] = QuestionnaireWork_GetTodayCount( QSave, questionID[0] ); // ����ɑ΂���, �����̉񓚐l��
  todayCountOfQuestion[1] = QuestionnaireWork_GetTodayCount( QSave, questionID[1] ); // ����ɑ΂���, �����̉񓚐l��
  todayCountOfQuestion[2] = QuestionnaireWork_GetTodayCount( QSave, questionID[2] ); // ����ɑ΂���, �����̉񓚐l��
  totalCountOfQuestion[0] = QuestionnaireWork_GetTotalCount( QSave, questionID[0] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  totalCountOfQuestion[1] = QuestionnaireWork_GetTotalCount( QSave, questionID[1] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  totalCountOfQuestion[2] = QuestionnaireWork_GetTotalCount( QSave, questionID[2] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: topicID=%d\n", topicID ); // DEBUG: ��������ID
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: questionID[0]=%d\n", questionID[0] ); // DEBUG: ����ID
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: questionID[1]=%d\n", questionID[1] ); // DEBUG: ����ID
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: questionID[2]=%d\n", questionID[2] ); // DEBUG: ����ID
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: answerNumOfQuestion[0]=%d\n", answerNumOfQuestion[0] ); // DEBUG: ����ɑ΂���, �񓚑I�����̐�
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: answerNumOfQuestion[1]=%d\n", answerNumOfQuestion[1] ); // DEBUG: ����ɑ΂���, �񓚑I�����̐�
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: answerNumOfQuestion[2]=%d\n", answerNumOfQuestion[2] ); // DEBUG: ����ɑ΂���, �񓚑I�����̐�
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: todayCountOfQuestion[0]=%d\n", todayCountOfQuestion[0] ); // DEBUG: ����ɑ΂���, �����̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: todayCountOfQuestion[1]=%d\n", todayCountOfQuestion[1] ); // DEBUG: ����ɑ΂���, �����̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: todayCountOfQuestion[2]=%d\n", todayCountOfQuestion[2] ); // DEBUG: ����ɑ΂���, �����̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: totalCountOfQuestion[0]=%d\n", totalCountOfQuestion[0] ); // DEBUG: ����ɑ΂���, ���܂܂ł̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: totalCountOfQuestion[1]=%d\n", totalCountOfQuestion[1] ); // DEBUG: ����ɑ΂���, ���܂܂ł̉񓚐l��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup research data: totalCountOfQuestion[2]=%d\n", totalCountOfQuestion[2] ); // DEBUG: ����ɑ΂���, ���܂܂ł̉񓚐l��
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    u8 qID = questionID[ qIdx ];

    for( aIdx=0; aIdx < answerNumOfQuestion[ qIdx ]; aIdx++ )
    {
      answerID[ qIdx ][ aIdx ] = AnswerID_question[ qID ][ aIdx ]; // ��ID
      todayCountOfAnswer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTodayAnswerNum( QSave, qID, aIdx + 1 ); // �񓚂ɑ΂���, �����̉񓚐l��
      totalCountOfAnswer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTotalAnswerNum( QSave, qID, aIdx + 1 ); // �񓚂ɑ΂���, ���܂܂ł̉񓚐l��
    }
  }

  // �f�[�^�ݒ�
  // �����܂܂� = ���� + ����܂�
  work->researchData.topicID = topicID; // ��������ID
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    work->researchData.questionData[ qIdx ].ID = questionID[ qIdx ]; // ����ID
    work->researchData.questionData[ qIdx ].answerNum  = answerNumOfQuestion[ qIdx ]; // �񓚑I�����̐�
    work->researchData.questionData[ qIdx ].todayCount = todayCountOfQuestion[ qIdx ]; // �����̉񓚐l��
    work->researchData.questionData[ qIdx ].totalCount = todayCountOfQuestion[ qIdx ] + totalCountOfQuestion[ qIdx ]; // ���܂܂ł̉񓚐l��

    for( aIdx=0; aIdx < MAX_ANSWER_NUM_PER_QUESTION; aIdx++ )
    {
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID = answerID[ qIdx ][ aIdx ]; // ��ID
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorR = (aIdx * 3) % 31; // �\���J���[(R)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorG = (aIdx * 5) % 31; // �\���J���[(G)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorB = (aIdx * 7) % 31; // �\���J���[(B)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].todayCount = todayCountOfAnswer[ qIdx ][ aIdx ]; // �����̉񓚐l��
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount = todayCountOfAnswer[ qIdx ][ aIdx ] + totalCountOfAnswer[ qIdx ][ aIdx ];  // ���܂܂ł̉񓚐l��
    }
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @breif �^�b�`�̈�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------------------
/**
 * @brief ��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitArrow( RESEARCH_CHECK_WORK* work )
{
  work->arrow = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init arrow\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateArrow( RESEARCH_CHECK_WORK* work )
{
  ARROW_DISP_PARAM dispParam;

  // ���d����
  GF_ASSERT( work->arrow == NULL );

  // ���𐶐�����
  dispParam.cgrIndex      = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CHARACTER );
  dispParam.plttIndex     = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_PALETTE );
  dispParam.cellAnimIndex = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CELL_ANIME );
  dispParam.setSerface    = CLSYS_DEFREND_MAIN;
  dispParam.anmseqH       = NANR_obj_yoko;
  dispParam.anmseqV       = NANR_obj_tate;
  dispParam.anmseqCorner  = NANR_obj_corner;
  dispParam.anmseqEnd     = NANR_obj_point;
  work->arrow = ARROW_Create( work->heapID, &dispParam );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create arrow\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteArrow( RESEARCH_CHECK_WORK* work )
{
  GF_ASSERT( work->arrow );

  // �j��
  ARROW_Delete( work->arrow );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete arrow\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitPercentage( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    work->percentage[ idx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init percentage\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g�𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreatePercentage( RESEARCH_CHECK_WORK* work )
{
  int idx; 
  PERCENTAGE_DISP_PARAM param;

  // �\���p�����[�^��ݒ�
  param.cgrIndex = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CHARACTER );
  param.plttIndex = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_PALETTE );
  param.cellAnimIndex = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CELL_ANIME );
  param.setSerface = CLSYS_DEFREND_MAIN;
  param.anmseqNumber = NANR_obj_nom;
  param.anmseqBase = NANR_obj_percent;

  // �S�񓚗p�̃I�u�W�F�N�g�𐶐�
  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    GF_ASSERT( work->percentage[ idx ] == NULL );

    // ����
    work->percentage[ idx ] =
      PERCENTAGE_Create( work->heapID, &param, GetClactUnit(work, CLUNIT_PERCENTAGE) );

    // ��\���ɐݒ�
    PERCENTAGE_SetDrawEnable( work->percentage[ idx ], FALSE );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create percentage\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeletePercentage( RESEARCH_CHECK_WORK* work )
{
  int idx;

  // �S�񓚗p�̃I�u�W�F�N�g��j��
  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    GF_ASSERT( work->percentage[ idx ] );

    // �j��
    PERCENTAGE_Delete( work->percentage[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete percentage\n" );
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
static void CreateClactSystem( RESEARCH_CHECK_WORK* work )
{
  // �V�X�e���쐬
  GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create clact system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_CHECK_WORK* work )
{ 
  // �V�X�e���j��
  GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete clact system\n" );
}


//=========================================================================================
// ��SUB-OBJ ���\�[�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RESEARCH_CHECK_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release SUB-OBJ resources\n" );
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
static void RegisterMainObjResources( RESEARCH_CHECK_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, commonPalette, cellAnime;

  heapID = work->heapID;

  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID ); 
  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_research_radar_graphic_obj_NCGR, 
                                      FALSE, CLSYS_DRAW_MAIN, heapID ); 
  palette = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_research_radar_graphic_obj_NCLR,
                                       CLSYS_DRAW_MAIN,
                                       ONE_PALETTE_SIZE*6, 0, 3, 
                                       heapID ); 
  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_research_radar_graphic_obj_NCER,
                                           NARC_research_radar_graphic_obj_NANR,
                                           heapID ); 
  GFL_ARC_CloseDataHandle( arcHandle );


  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, heapID ); 
  commonPalette = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_app_menu_common_task_menu_NCLR,
                                       CLSYS_DRAW_MAIN,
                                       ONE_PALETTE_SIZE*4, 0, 2, 
                                       heapID );
  GFL_ARC_CloseDataHandle( arcHandle );

  // �o�^�C���f�b�N�X���L��
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ]  = character;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ]    = palette;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_COMMON_PALETTE ]    = commonPalette;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] = cellAnime;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: register MAIN-OBJ resources\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RESEARCH_CHECK_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_COMMON_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release MAIN-OBJ resources\n" );
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �f�t�H���g�����������ɂ��, �r�b�g�}�b�v�f�[�^�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forDefault( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup bitmap data for Default\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�ق��������݂�v�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forANALYZE_BUTTON( RESEARCH_CHECK_WORK* work )
{
  int x, y;
  GFL_BMP_DATA* srcBMP; 
  GFL_MSGDATA* msgData;
  STRBUF* strbuf;
  PRINTSYS_LSB color;
  const BITMAP_INIT_DATA* data;

  // �R�s�[����L�����N�^�ԍ�
  int charaNo[ BMPOAM_ACTOR_ANALYZE_BUTTON_CHARA_SIZE_Y ][ BMPOAM_ACTOR_ANALYZE_BUTTON_CHARA_SIZE_X ] = 
  {
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5},
    {6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8},
  };

  // �������f�[�^�擾
  data = &BitmapInitData[ BMPOAM_ACTOR_ANALYZE_BUTTON ]; 

  // �L�����N�^�f�[�^��ǂݍ���
  srcBMP = GFL_BMP_LoadCharacter( data->charaDataArcID, 
                                  data->charaDataArcDatID,
                                  FALSE, 
                                  work->heapID ); 
  // �L�����f�[�^���R�s�[����
  for( y=0; y < BMPOAM_ACTOR_ANALYZE_BUTTON_CHARA_SIZE_Y; y++)
  {
    for(x=0; x < BMPOAM_ACTOR_ANALYZE_BUTTON_CHARA_SIZE_X; x++)
    {
      GFL_BMP_Print( srcBMP, work->BmpData[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 
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

  PRINTSYS_PrintColor( work->BmpData[ BMPOAM_ACTOR_ANALYZE_BUTTON ],
                       data->stringDrawPosX, data->stringDrawPosY,
                       strbuf, work->font, color ); 

  GFL_HEAP_FreeMemory( strbuf );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-SELECT: setup bitmap data for ANALYZE_BUTTON\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̏������s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void SetupBmpOamSystem( RESEARCH_CHECK_WORK* work )
{
  // BMP-OAM �V�X�e�����쐬
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup BMP-OAM system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RESEARCH_CHECK_WORK* work )
{
  // BMP-OAM �V�X�e����j��
  BmpOam_Exit( work->BmpOamSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up BMP-OAM system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreatePaletteAnime( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL ); // ���d����

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: create palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeletePaletteAnime( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Delete( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: delete palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupPaletteAnime( RESEARCH_CHECK_WORK* work )
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
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: setup palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: clean up palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RESEARCH_CHECK_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       PaletteAnimeData[ index ].animeType,
                       PaletteAnimeData[ index ].fadeColor );
  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: start palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StopPaletteAnime( RESEARCH_CHECK_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: stop palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RESEARCH_CHECK_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Update( work->paletteAnime[ idx ] );
  }
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


//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterVBlankTask( RESEARCH_CHECK_WORK* work )
{
  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: register VBlank task\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RESEARCH_CHECK_WORK* work )
{ 
  GFL_TCB_DeleteTask( work->VBlankTask );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: release VBlank task\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RESEARCH_CHECK_WORK* work )
{ 
  // ������
  work->paletteFadeSystem = NULL;

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: init palette fade system\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���̌�Еt�����s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̒�������ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̒�������ID
 */
//-----------------------------------------------------------------------------------------
static u8 GetTopicID( const RESEARCH_CHECK_WORK* work )
{
  return work->researchData.topicID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ID
 */
//-----------------------------------------------------------------------------------------
static u8 GetQuestionID( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  qIdx = work->questionIdx;
  return work->researchData.questionData[ qIdx ].ID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���񓚑I�����̐� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �񓚂̑I�����̐�
 */
//------------------------------------------------------------------------------------------
static u8 GetAnswerNum( const RESEARCH_CHECK_WORK* work )
{
  u8 questionIdx;

  questionIdx = work->questionIdx;

  return work->researchData.questionData[ questionIdx ].answerNum;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉�ID���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̉�ID
 */
//-----------------------------------------------------------------------------------------
static u16 GetAnswerID( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, �\���̉񓚐l�� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �\���̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
static u16 GetCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  u16 count = 0;

  switch( work->dispType ) {
  case DATA_DISP_TYPE_TODAY:  count = GetTodayCountOfQuestion( work ); break;
  case DATA_DISP_TYPE_TOTAL:  count = GetTotalCountOfQuestion( work ); break;
  default: GF_ASSERT(0);
  }

  return count;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, �����̉񓚐l�����擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, �����̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
static u16 GetTodayCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;

  qIdx = work->questionIdx;

  return work->researchData.questionData[ qIdx ].todayCount;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
static u16 GetTotalCountOfQuestion( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;

  qIdx = work->questionIdx;

  return work->researchData.questionData[ qIdx ].totalCount;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, �\���̉񓚐l�� ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, �\���̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
static u16 GetTodayCountOfAnswer( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].todayCount;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
 *
 * @param work
 *
 * @return ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
 */
//-----------------------------------------------------------------------------------------
static u16 GetTotalCountOfAnswer( const RESEARCH_CHECK_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̃��C���~�O���t ���擾����
 *
 * @param work
 *
 * @return ���ݕ\�����̃��C���~�O���t
 */
//-----------------------------------------------------------------------------------------
static CIRCLE_GRAPH* GetMainGraph( const RESEARCH_CHECK_WORK* work )
{
  return work->mainGraph[ work->dispType ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݕ\�����̃T�u�~�O���t
 *
 * @param work
 *
 * @return ���ݕ\�����̃T�u�~�O���t
 */
//-----------------------------------------------------------------------------------------
static CIRCLE_GRAPH* GetSubGraph ( const RESEARCH_CHECK_WORK* work )
{
  return work->subGraph[ work->dispType ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������̉~�O���t�����邩�ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �A�j���[�V�������Ă���O���t������ꍇ FALSE
 *         ���ׂẴO���t�̃A�j���[�V�������I�����Ă���ꍇ TRUE
 */
//-----------------------------------------------------------------------------------------
static BOOL CheckAllGraphAnimeEnd( const RESEARCH_CHECK_WORK* work )
{
  if( CIRCLE_GRAPH_IsAnime( work->mainGraph[ DATA_DISP_TYPE_TODAY ] ) ||
      CIRCLE_GRAPH_IsAnime( work->mainGraph[ DATA_DISP_TYPE_TOTAL ] ) ||
      CIRCLE_GRAPH_IsAnime( work->subGraph[ DATA_DISP_TYPE_TODAY ] ) ||
      CIRCLE_GRAPH_IsAnime( work->subGraph[ DATA_DISP_TYPE_TOTAL ] ) ) { return FALSE; }

  return TRUE;
}

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
static u32 GetObjResourceRegisterIndex( const RESEARCH_CHECK_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RESEARCH_CHECK_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RESEARCH_CHECK_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�C�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ɂ��t���b�V�� ( �A�E�g ) ���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeFlashOut( RESEARCH_CHECK_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_FLASH_OUT_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_FLASH_OUT_WAIT,
                  MAIN_BG_PALETTE_FADE_FLASH_OUT_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASH_OUT_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASH_OUT_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_FLASH_OUT_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_FLASH_OUT_WAIT,
                  MAIN_OBJ_PALETTE_FADE_FLASH_OUT_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASH_OUT_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASH_OUT_COLOR,
                  work->VBlankTCBSystem );
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: start palette fade flash out\n" );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ɂ��t���b�V�� ( �C�� ) ���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeFlashIn( RESEARCH_CHECK_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_FLASH_IN_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_FLASH_IN_WAIT,
                  MAIN_BG_PALETTE_FADE_FLASH_IN_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASH_IN_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASH_IN_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_FLASH_IN_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_FLASH_IN_WAIT,
                  MAIN_OBJ_PALETTE_FADE_FLASH_IN_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASH_IN_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASH_IN_COLOR,
                  work->VBlankTCBSystem );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: start palette fade flash in\n" );
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
static BOOL IsPaletteFadeEnd( RESEARCH_CHECK_WORK* work )
{
  return (PaletteFadeCheck( work->paletteFadeSystem ) == 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief ���C���~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param dispType �\���^�C�v
 */
//-----------------------------------------------------------------------------------------
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
    SetupCenterColorOfGraphComponent( &components[ aIdx ] );
    switch( dispType ) {
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param dispType �\���^�C�v
 */
//-----------------------------------------------------------------------------------------
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
    SetupCenterColorOfGraphComponent( &components[ aIdx ] );
    switch( dispType ) {
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

//-----------------------------------------------------------------------------------------
/**
 * @breif �O���t�\���v�f�̒��S�_�J���[���Z�b�g�A�b�v����
 *
 * @param component �O���t�\���v�f�̒ǉ��f�[�^
 */
//-----------------------------------------------------------------------------------------
static void SetupCenterColorOfGraphComponent( GRAPH_COMPONENT_ADD_DATA* component )
{
  const int min = 0; // �J���[�����̍ŏ��l
  const int max = 31; // �J���[�����̍ő�l
  const int brightness = 3; // ���S�_�J���[�̖��邳
  int R, G, B;
  int cR, cG, cB;

  // �O���̐F���擾
  R = component->outerColorR;
  G = component->outerColorG;
  B = component->outerColorB;

  // �l��`�F�b�N
  GF_ASSERT( min<=R && R<=max );
  GF_ASSERT( min<=G && G<=max );
  GF_ASSERT( min<=B && B<=max );

  // ���S�_�̐F������
  cR = (R + max * (brightness - 1)) / brightness;
  cG = (G + max * (brightness - 1)) / brightness;
  cB = (B + max * (brightness - 1)) / brightness;

  // �l��`�F�b�N
  GF_ASSERT( min<=cR && cR<=max );
  GF_ASSERT( min<=cG && cG<=max );
  GF_ASSERT( min<=cB && cB<=max );

  // ���S�̐F���Z�b�g
  component->centerColorR = cR;
  component->centerColorG = cG;
  component->centerColorB = cB;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���C���~�O���t, �T�u�~�O���t�̏d�Ȃ���𒲐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustCircleGraphLayer( RESEARCH_CHECK_WORK* work )
{
  // �T�u�~�O���t����O�ɕ\�������悤��z���W��ݒ肷��
  CIRCLE_GRAPH_SetCenterZ( GetMainGraph(work), FX16_CONST(-2.0f) );
  CIRCLE_GRAPH_SetCenterZ( GetSubGraph(work), FX16_CONST(0.0f) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-CHECK: adjust circle graph layer\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
    case RESEARCH_CHECK_SEQ_SETUP:      OS_TFPrintf( PRINT_TARGET, "SETUP " );      break;
    case RESEARCH_CHECK_SEQ_STANDBY:    OS_TFPrintf( PRINT_TARGET, "STANDBY " );    break;
    case RESEARCH_CHECK_SEQ_KEY_WAIT:   OS_TFPrintf( PRINT_TARGET, "KEY-WAIT " );   break;
    case RESEARCH_CHECK_SEQ_ANALYZE:    OS_TFPrintf( PRINT_TARGET, "ANALYZE " );    break;
    case RESEARCH_CHECK_SEQ_PERCENTAGE: OS_TFPrintf( PRINT_TARGET, "PERCENTAGE " ); break;
    case RESEARCH_CHECK_SEQ_FLASH_OUT:  OS_TFPrintf( PRINT_TARGET, "FLASH_OUT " );  break;
    case RESEARCH_CHECK_SEQ_FLASH_IN:   OS_TFPrintf( PRINT_TARGET, "FLASH_IN " );   break;
    case RESEARCH_CHECK_SEQ_UPDATE:     OS_TFPrintf( PRINT_TARGET, "UPDATE " );     break;
    case RESEARCH_CHECK_SEQ_FADE_OUT:   OS_TFPrintf( PRINT_TARGET, "FADE_OUT " );   break;
    case RESEARCH_CHECK_SEQ_CLEAN_UP:   OS_TFPrintf( PRINT_TARGET, "CLEAN-UP " );   break;
    case RESEARCH_CHECK_SEQ_FINISH:     OS_TFPrintf( PRINT_TARGET, "FINISH " );     break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @breif �����f�[�^��ݒ肷��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
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
