///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F���
 * @file   research_graph.c
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
#include "research_graph.h"
#include "research_graph_index.h"
#include "research_graph_def.h"
#include "research_graph_data.cdat"
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
struct _RESEARCH_RADAR_GRAPH_WORK
{ 
  RRC_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID heapID;

  GFL_FONT*    font;                   // �t�H���g
  GFL_MSGDATA* message[ MESSAGE_NUM ]; // ���b�Z�[�W
  WORDSET*     wordset;                // ���[�h�Z�b�g

  // ���
  QUEUE*    stateQueue; // ��ԃL���[
  RRG_STATE state;      // ���݂̏��
  u32       stateSeq;   // ��ԓ��V�[�P���X�ԍ�
  u32       stateCount; // ��ԃJ�E���^
  u32       waitFrame;  // WAIT ��Ԃ̑҂�����

  // �J�[�\��
  MENU_ITEM cursorPos; // �J�[�\���ʒu

  // �����f�[�^
  RESEARCH_DATA researchData;
  u8            questionIdx;  // �\�����̎���C���f�b�N�X
  u8            answerIdx;    // �I�𒆂̉񓚃C���f�b�N�X

  // �~�O���t
  CIRCLE_GRAPH*  mainGraph[ GRAPH_DISP_MODE_NUM ]; // �ʏ펞�ɕ\������O���t
  CIRCLE_GRAPH*  subGraph [ GRAPH_DISP_MODE_NUM ]; // �X�V���ɕ\������O���t
  GRAPH_DISP_MODE graphMode;      // ���� or ���v �̂ǂ���̃f�[�^��\������̂�

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

  // �t���O
  BOOL analyzeFlag;        // ��͂��ς�ł��邩�ǂ���
  BOOL analyzeByTouchFlag; // �^�b�`�ɂ���͂��ǂ���
  BOOL updateFlag;         // �X�V�����ǂ���
  BOOL stateEndFlag;       // ���݂̏�Ԃ��I���������ǂ���
  BOOL finishFlag;         // �O���t��ʂ̏I���t���O
  RRG_RESULT finishResult; // �O���t��ʂ̏I������
};



//=========================================================================================
// ���֐��C���f�b�N�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
// ��LAYER 6 ���
//-----------------------------------------------------------------------------------------
// ��ԏ���
static void MainSeq_SETUP( RRG_WORK* work ); // RRG_STATE_SETUP
static void MainSeq_STANDBY( RRG_WORK* work ); // RRG_STATE_STANDBY
static void MainSeq_KEYWAIT( RRG_WORK* work ); // RRG_STATE_KEYWAIT
static void MainSeq_ANALYZE( RRG_WORK* work ); // RRG_STATE_ANALYZE
static void MainSeq_PERCENTAGE( RRG_WORK* work ); // RRG_STATE_PERCENTAGE
static void MainSeq_FLASHOUT( RRG_WORK* work ); // RRG_STATE_FLASHOUT
static void MainSeq_FLASHIN( RRG_WORK* work ); // RRG_STATE_FLASHIN
static void MainSeq_UPDATE( RRG_WORK* work ); // RRG_STATE_UPDATE
static void MainSeq_FADEOUT( RRG_WORK* work ); // RRG_STATE_FADEOUT
static void MainSeq_WAIT( RRG_WORK* work ); // RRG_STATE_WAIT
static void MainSeq_CLEANUP( RRG_WORK* work ); // RRG_STATE_CLEANUP 
// ��Ԑ���
static void CountUpStateCount( RRG_WORK* work ); // ��ԃJ�E���^���X�V����
static void RegisterNextState( RRG_WORK* work, RRG_STATE nextSeq ); // ���̏�Ԃ��L���[�ɓo�^����
static void FinishCurrentState( RRG_WORK* work ); // ���݂̏�Ԃ��I������
static void SwitchState( RRG_WORK* work ); // ������Ԃ�ύX����
static void SetState( RRG_WORK* work, RRG_STATE nextSeq ); // ������Ԃ�ݒ肷��
static u32 GetStateSeq( const RRG_WORK* work ); // ��ԓ��V�[�P���X�ԍ����擾����
static void IncStateSeq( RRG_WORK* work ); // ��ԓ��V�[�P���X�ԍ����C���N�������g����
static void ResetStateSeq( RRG_WORK* work ); // ��ԓ��V�[�P���X�ԍ������Z�b�g����
static void SetFinishReason( RRG_WORK* work, SEQ_CHANGE_TRIG reason ); // �O���t��ʂ̏I�����R��o�^����
static void SetFinishResult( RRG_WORK* work, RRG_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
static void SetWaitFrame( RRG_WORK* work, u32 frame ); // �t���[���o�ߑ҂���Ԃ̑҂����Ԃ�ݒ肷��
static u32 GetWaitFrame( const RRG_WORK* work ); // �t���[���o�ߑ҂���Ԃ̑҂����Ԃ��擾����
RRG_STATE GetFirstState( const RRG_WORK* work ); // �ŏ��̏�Ԃ��擾����    
//-----------------------------------------------------------------------------------------
// ��LAYER 5 �@�\
//-----------------------------------------------------------------------------------------
// �J�[�\��
static void MoveMenuCursorUp( RRG_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RRG_WORK* work ); // ���ֈړ�����
static void MoveMenuCursorDirect( RRG_WORK* work, MENU_ITEM menuItem ); // ���ڈړ�����
static void MoveMenuCursorSilent( RRG_WORK* work, MENU_ITEM menuItem ); // ���ڈړ����� ( �_�ŁESE�Ȃ� )
// �\�����鎿��
static void ChangeQuestionToNext( RRG_WORK* work ); // ���̎���ɕύX����
static void ChangeQuestionToPrev( RRG_WORK* work ); // �O�̎���ɕύX����
// �\�������
static void ChangeAnswerToNext( RRG_WORK* work ); // ���̉񓚂ɕύX����
static void ChangeAnswerToPrev( RRG_WORK* work ); // �O�̉񓚂ɕύX����
static void ChangeAnswerToTop( RRG_WORK* work ); // �擪�̉񓚂ɕύX����
// �����f�[�^�̕\���^�C�v
static void SwitchDataDisplayType( RRG_WORK* work ); // �����f�[�^�̕\���^�C�v��؂�ւ���
// ��ʂ̃t�F�[�h�C���E�t�F�[�h�A�E�g
static void StartFadeIn( void ); // ��ʂ̃t�F�[�h�C�����J�n����
static void StartFadeOut( void ); // ��ʂ̃t�F�[�h�A�E�g���J�n����
//-----------------------------------------------------------------------------------------
// ��LAYER 4 �ʑ���
//-----------------------------------------------------------------------------------------
// ���j���[���ڃJ�[�\��
static void ShiftMenuCursorPos( RRG_WORK* work, int stride ); // �J�[�\���ʒu��ύX����
static void SetMenuCursorPos( RRG_WORK* work, MENU_ITEM menuItem ); // �J�[�\���ʒu��ύX����
// ���j���[���ڂ̕\��
static void SetMenuCursorOn( RRG_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuCursorOff( RRG_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
// �\�����鎿��C���f�b�N�X
static void ShiftQuestionIdx( RRG_WORK* work, int stride ); // �\�����鎿��̃C���f�b�N�X��ύX����
// �I�𒆂̉񓚃C���f�b�N�X
static void ShiftAnswerIdx( RRG_WORK* work, int stride ); // �I�𒆂̉񓚃C���f�b�N�X��ύX����
static void ResetAnswerIdx( RRG_WORK* work );             // �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����
//�w�񍐂�����x�{�^��
static void UpdateAnalyzeButton( RRG_WORK* work ); //�w�񍐂�����x�{�^�����X�V����
static void BlinkAnalyzeButton( RRG_WORK* work ); //�w�񍐂�����x�{�^����_�ł�����
static void SetAnalyzeButtonCursorOn( RRG_WORK* work ); //�w�񍐂�����x�{�^�����J�[�\��������Ă����Ԃɂ���
static void SetAnalyzeButtonCursorOff( RRG_WORK* work ); //�w�񍐂�����x�{�^�����J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetAnalyzeButtonCursorSet( RRG_WORK* work ); //�w�񍐂�����x�{�^�����J�[�\�����Z�b�g������Ԃɂ���
static void SetAnalyzeButtonActive( RRG_WORK* work ); //�w�񍐂�����x�{�^�����A�N�e�B�u��Ԃɂ���
static void SetAnalyzeButtonNotActive( RRG_WORK* work ); //�w�񍐂�����x�{�^�����A�N�e�B�u��Ԃɂ���
//�u�߂�v�{�^��
static void BlinkReturnButton( RRG_WORK* work ); //�u�߂�v�{�^����_�ł�����
// �~�O���t
static void UpdateCircleGraphs( RRG_WORK* work ); // ���ׂẲ~�O���t���X�V����
static void DrawCircleGraphs( const RRG_WORK* work ); // ���ׂẲ~�O���t��`�悷��
static void SetupMainCircleGraph( RRG_WORK* work, GRAPH_DISP_MODE graphMode ); // ���C���~�O���t�����݂̒����f�[�^�ō\������
static void SetupSubCircleGraph ( RRG_WORK* work, GRAPH_DISP_MODE graphMode ); // �T�u�~�O���t�����݂̒����f�[�^�ō\������
static void InterchangeCircleGraph( RRG_WORK* work ); // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
static void SetupCenterColorOfGraphComponent( GRAPH_COMPONENT_ADD_DATA* component ); // �O���t�\���v�f�̒��S�_�J���[���Z�b�g�A�b�v����
static void AdjustCircleGraphLayer( RRG_WORK* work ); // ���C���~�O���t, �T�u�~�O���t�̏d�Ȃ���𒲐�����
// �񓚃}�[�J�[
static void DrawAnswerMarker( const RRG_WORK* work ); // �񓚃}�[�J�[��`�悷��
// ���
static void UpdateArrow( RRG_WORK* work ); // ���̕\�����X�V����
// % �\��
static void SetupPercentages( RRG_WORK* work ); // ���݂̒����f�[�^�ɂ��킹�ăZ�b�g�A�b�v����
static void VanishAllPercentage( RRG_WORK* work ); // �S�Ắ��\������������
static void DispPercentage( RRG_WORK* work, u8 index ); // ���I�u�W�F�N�g��\������
static void DispAllPercentage( RRG_WORK* work ); // �S�Ắ��I�u�W�F�N�g��\������
// �^�b�`�͈�
static void UpdateTouchArea( RRG_WORK* work ); // �^�b�`�͈͂��X�V����
// BG
static void UpdateMainBG_WINDOW( RRG_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ���X�V����
// BGFont
static void UpdateBGFont_TopicTitle( RRG_WORK* work ); // �������ږ����X�V����
static void UpdateBGFont_QuestionCaption( RRG_WORK* work ); // ����̕⑫�����X�V����
static void UpdateBGFont_Question( RRG_WORK* work ); // ������X�V���� 
static void UpdateBGFont_Answer( RRG_WORK* work ); // �񓚂��X�V����
static void UpdateBGFont_MyAnswer( RRG_WORK* work ); // �����̉񓚂��X�V����
static void UpdateBGFont_Count( RRG_WORK* work ); // �񓚐l�����X�V����
static void UpdateBGFont_NoData( RRG_WORK* work ); //�u�������� ���傤�����イ�v�̕\�����X�V����
static void UpdateBGFont_DataReceiving( RRG_WORK* work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
// OBJ
static void UpdateControlCursor( RRG_WORK* work ); // ����J�[�\���̕\�����X�V����
static void UpdateMyAnswerIconOnButton( RRG_WORK* work ); // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����
static void UpdateMyAnswerIconOnGraph( RRG_WORK* work ); // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
// BMP-OAM
static void ShowAnalyzeButton( RRG_WORK* work ); //�w�񍐂�����x�{�^����\������
static void ShowAnalyzeMessage( RRG_WORK* work ); //�u�c�����������イ�c�v��\������
static void HideAnalyzeMessage( RRG_WORK* work ); //�u�c�����������イ�c�v���\���ɂ���
static void BmpOamSetDrawEnable( RRG_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable );  // �\�����邩�ǂ�����ݒ肷��
// �p���b�g�A�j���[�V����
static void StartPaletteAnime( RRG_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RRG_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static BOOL CheckPaletteAnime( const RRG_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V���������ǂ������`�F�b�N����
static void UpdatePaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
static void UpdateCommonPaletteAnime( RRG_WORK* work ); // ���ʃp���b�g�A�j���[�V�������X�V����
// �p���b�g�t�F�[�h
static void StartPaletteFadeFlashOut( RRG_WORK* work ); // �p���b�g�A�j���ɂ��t���b�V�� ( �A�E�g ) ���J�n����
static void StartPaletteFadeFlashIn ( RRG_WORK* work ); // �p���b�g�A�j���ɂ��t���b�V�� ( �C�� ) ���J�n����
static BOOL IsPaletteFadeEnd( RRG_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
// VBlank�^�X�N
static void VBlankFunc( GFL_TCB* tcb, void* wk );  // VBlank���̏���
//-----------------------------------------------------------------------------------------
// ��LAYER 3 �擾�E�ݒ�E����
//-----------------------------------------------------------------------------------------
// �V�X�e���E���[�N
static GAMESYS_WORK* GetGameSystem( const RRG_WORK* work ); // �Q�[���V�X�e�����擾����
static GAMEDATA* GetGameData( const RRG_WORK* work ); // �Q�[���f�[�^���擾����
static HEAPID GetHeapID( const RRG_WORK* work ); // �q�[�vID���擾����
static void SetHeapID( RRG_WORK* work, HEAPID heapID );// �q�[�vID��ݒ肷��
static RRC_WORK* GetCommonWork( const RRG_WORK* work ); // �S��ʋ��ʃ��[�N���擾����
static void SetCommonWork( RRG_WORK* work, RRC_WORK* commonWork ); // �S��ʋ��ʃ��[�N��ݒ肷��
// �����f�[�^�\���^�C�v
static void SetDataDisplayType( RRG_WORK* work, GRAPH_DISP_MODE graphMode ); // �����f�[�^�̕\���^�C�v��ݒ肷��
// �~�O���t
static CIRCLE_GRAPH* GetMainGraph( const RRG_WORK* work ); // ���ݕ\�����̃��C���~�O���t���擾����
static CIRCLE_GRAPH* GetSubGraph( const RRG_WORK* work ); // ���ݕ\�����̃T�u�~�O���t���擾����
static BOOL CheckAllGraphAnimeEnd( const RRG_WORK* work ); // �A�j���[�V�������̉~�O���t�����邩�ǂ����𔻒肷��
// �����f�[�^
static u8 GetTopicID( const RRG_WORK* work ); // ���ݕ\�����̒�������ID
static u8 GetQuestionID( const RRG_WORK* work ); // ���ݕ\�����̎���ID
static u8 GetAnswerNum( const RRG_WORK* work ); // ���ݕ\�����̎���ɑ΂���񓚑I�����̐�
static u16 GetAnswerID( const RRG_WORK* work ); // ���ݕ\�����̉�ID
static u16 GetCountOfQuestion( const RRG_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfQuestion( const RRG_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfQuestion( const RRG_WORK* work ); // ���ݕ\�����̎���ɑ΂���, ���v�̉񓚐l��
static u16 GetCountOfAnswer( const RRG_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �\�����̉񓚐l��
static u16 GetTodayCountOfAnswer( const RRG_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, �����̉񓚐l��
static u16 GetTotalCountOfAnswer( const RRG_WORK* work ); // ���ݕ\�����̉񓚂ɑ΂���, ���v�̉񓚐l��
static u8 GetInvestigatingTopicID( const RRG_WORK* work ); // ���ݒ������̒�������ID���擾����
static u8 GetMyAnswerID( const RRG_WORK* work ); // ���ݕ\�����̎���ɑ΂���, �����̉�ID���擾����
static u8 GetMyAnswerID_PlayTime( const RRG_WORK* work ); // ����w�v���C���Ԃ́H�x�ɑ΂��鎩���̉�ID���擾����
// OBJ
static u32 GetObjResourceRegisterIndex( const RRG_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
static GFL_CLUNIT* GetClactUnit( const RRG_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RRG_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
//-----------------------------------------------------------------------------------------
// ��LAYER 2 �����E�������E�j��
//-----------------------------------------------------------------------------------------
static RRG_WORK* CreateGraphWork( HEAPID heapID ); // �O���t��ʊǗ����[�N ����
static void InitGraphWork( RRG_WORK* work ); // �O���t��ʊǗ����[�N ������
static void DeleteGraphWork( RRG_WORK* work ); // �O���t��ʊǗ����[�N �j��
static void InitStateQueue( RRG_WORK* work ); // ��ԃL���[ ������
static void CreateStateQueue( RRG_WORK* work ); // ��ԃL���[ �쐬
static void DeleteStateQueue( RRG_WORK* work ); // ��ԃL���[ �j��
static void InitFont( RRG_WORK* work ); // �t�H���g ������
static void CreateFont( RRG_WORK* work ); // �t�H���g ����
static void DeleteFont( RRG_WORK* work ); // �t�H���g �j��
static void InitMessages( RRG_WORK* work ); // ���b�Z�[�W ������
static void CreateMessages( RRG_WORK* work ); // ���b�Z�[�W ����
static void DeleteMessages( RRG_WORK* work ); // ���b�Z�[�W �j��
static void InitWordset( RRG_WORK* work ); // ���[�h�Z�b�g ������
static void CreateWordset( RRG_WORK* work ); // ���[�h�Z�b�g ����
static void DeleteWordset( RRG_WORK* work ); // ���[�h�Z�b�g �j��
static void InitCircleGraphs( RRG_WORK* work ); // �~�O���t ������
static void CreateCircleGraph( RRG_WORK* work ); // �~�O���t ����
static void DeleteCircleGraph( RRG_WORK* work ); // �~�O���t �j��
static void InitResearchData( RRG_WORK* work ); // �����f�[�^ ������
static void SetupResearchData( RRG_WORK* work ); // �����f�[�^ �擾
static void SetupTouchArea( RRG_WORK* work ); // �^�b�`�̈� ����
static void InitArrow( RRG_WORK* work ); // ��� ������
static void CreateArrow( RRG_WORK* work ); // ��� ����
static void DeleteArrow( RRG_WORK* work ); // ��� �j��
static void InitPercentage( RRG_WORK* work ); // % �\���I�u�W�F�N�g ������
static void CreatePercentage( RRG_WORK* work ); // % �\���I�u�W�F�N�g ����
static void DeletePercentage( RRG_WORK* work ); // % �\���I�u�W�F�N�g �j��
static void Setup3D( void ); // 3D �`�� ����
static void SetupBG( RRG_WORK* work ); // BG �S�� ����
static void SetupSubBG_WINDOW( RRG_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ����
static void SetupSubBG_FONT( RRG_WORK* work ); // SUB-BG ( �t�H���g�� ) ����
static void SetupMainBG_WINDOW( RRG_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ����
static void SetupMainBG_FONT( RRG_WORK* work ); // MAIN-BG ( �t�H���g�� ) ����
static void CleanUpBG( RRG_WORK* work ); // BG �S�� ��Еt��
static void CleanUpSubBG_WINDOW( RRG_WORK* work ); // SUB-BG ( �E�B���h�E�� ) ��Еt��
static void CleanUpSubBG_FONT( RRG_WORK* work ); // SUB-BG ( �t�H���g�� ) ��Еt��
static void CleanUpMainBG_WINDOW( RRG_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) ��Еt��
static void CleanUpMainBG_FONT( RRG_WORK* work ); // MAIN-BG ( �t�H���g�� ) ��Еt��
static void InitBGFonts( RRG_WORK* work ); // ������`��I�u�W�F�N�g ������
static void CreateBGFonts( RRG_WORK* work ); // ������`��I�u�W�F�N�g ����
static void DeleteBGFonts( RRG_WORK* work ); // ������`��I�u�W�F�N�g �j��
static void RegisterSubObjResources( RRG_WORK* work ); // SUB-OBJ ���\�[�X �o�^
static void ReleaseSubObjResources( RRG_WORK* work ); // SUB-OBJ ���\�[�X ���
static void RegisterMainObjResources( RRG_WORK* work ); // MAIN-OBJ ���\�[�X �o�^
static void ReleaseMainObjResources( RRG_WORK* work ); // MAIN-OBJ ���\�[�X ���
static void InitClactUnits( RRG_WORK* work ); // �Z���A�N�^�[���j�b�g ������
static void CreateClactUnits( RRG_WORK* work ); // �Z���A�N�^�[���j�b�g ����
static void DeleteClactUnits( RRG_WORK* work ); // �Z���A�N�^�[���j�b�g �j��
static void InitClactWorks( RRG_WORK* work ); // �Z���A�N�^�[���[�N ������
static void CreateClactWorks( RRG_WORK* work ); // �Z���A�N�^�[���[�N ����
static void DeleteClactWorks( RRG_WORK* work ); // �Z���A�N�^�[���[�N �j��
static void InitBitmapDatas( RRG_WORK* work ); // �r�b�g�}�b�v�f�[�^ ������
static void CreateBitmapDatas( RRG_WORK* work ); // �r�b�g�}�b�v�f�[�^ ����
static void SetupBitmapData_forDefault( RRG_WORK* work ); // �r�b�g�}�b�v�f�[�^ ���� ( �f�t�H���g ) 
static void SetupBitmapData_forANALYZE_BUTTON( RRG_WORK* work ); // �r�b�g�}�b�v�f�[�^ ���� ( �w�񍐂�����x�{�^�� ) 
static void DeleteBitmapDatas( RRG_WORK* work ); // �r�b�g�}�b�v�f�[�^ �j��
static void SetupBmpOamSystem( RRG_WORK* work ); // BMP-OAM �V�X�e�� ����
static void CleanUpBmpOamSystem( RRG_WORK* work ); // BMP-OAM �V�X�e�� ��Еt��
static void CreateBmpOamActors( RRG_WORK* work ); // BMP-OAM �A�N�^�[ ����
static void DeleteBmpOamActors( RRG_WORK* work ); // BMP-OAM �A�N�^�[ �j��
static void InitPaletteFadeSystem( RRG_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ������
static void SetupPaletteFadeSystem( RRG_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ����
static void CleanUpPaletteFadeSystem( RRG_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��
static void InitPaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������[�N ������
static void CreatePaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������[�N ����
static void DeletePaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������[�N �j��
static void SetupPaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������[�N ����
static void CleanUpPaletteAnime( RRG_WORK* work ); // �p���b�g�A�j���[�V�������[�N ��Еt��
static void SetupWirelessIcon( const RRG_WORK* work ); // �ʐM�A�C�R�� ����
static void RegisterVBlankTask( RRG_WORK* work ); // VBlank �^�X�N �o�^
static void ReleaseVBlankTask( RRG_WORK* work ); // VBlank �^�X�N ����
//-----------------------------------------------------------------------------------------
// ��LAYER 1 ���[�e�B���e�B
//-----------------------------------------------------------------------------------------
static u8 Bind_u8( int num ); // u8 �Ɏ��܂�悤�Ɋۂ߂�




//=========================================================================================
// ��public functions
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
RRG_WORK* RRG_CreateWork( RRC_WORK* commonWork )
{
  RRG_WORK* work;
  HEAPID heapID;

  heapID = RRC_GetHeapID( commonWork );

  // ���[�N�𐶐�
  work = CreateGraphWork( heapID );

  // ���[�N��������
  InitGraphWork( work );
  SetHeapID( work, heapID );
  SetCommonWork( work, commonWork );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
void RRG_DeleteWork( RRG_WORK* work )
{
  DeleteStateQueue( work );
  DeleteGraphWork( work );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �����񍐊m�F��� ���C������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
void RRG_Main( RRG_WORK* work )
{
  // ��Ԃ��Ƃ̏���
  switch( work->state ) {
  case RRG_STATE_SETUP:       MainSeq_SETUP( work );       break;
  case RRG_STATE_STANDBY:     MainSeq_STANDBY( work );     break;
  case RRG_STATE_KEYWAIT:     MainSeq_KEYWAIT( work );     break;
  case RRG_STATE_ANALYZE:     MainSeq_ANALYZE( work );     break;
  case RRG_STATE_PERCENTAGE:  MainSeq_PERCENTAGE( work );  break;
  case RRG_STATE_FLASHOUT:    MainSeq_FLASHOUT( work );    break;
  case RRG_STATE_FLASHIN:     MainSeq_FLASHIN( work );     break;
  case RRG_STATE_UPDATE:      MainSeq_UPDATE( work );      break;
  case RRG_STATE_FADEOUT:     MainSeq_FADEOUT( work );     break;
  case RRG_STATE_WAIT:        MainSeq_WAIT( work );        break;
  case RRG_STATE_CLEANUP:     MainSeq_CLEANUP( work );     break;
  case RRG_STATE_FINISH:      return;
  default: GF_ASSERT(0);
  }

  // 2D �`��
  GFL_CLACT_SYS_Main();             // �Z���A�N�^�[�V�X�e�� ���C������
  UpdateCircleGraphs( work );       // �~�O���t ���C������
  ARROW_Main( work->arrow );        // ��� ���C������
  UpdateCommonPaletteAnime( work ); // ���ʃp���b�g�A�j���[�V�������X�V
  UpdatePaletteAnime( work );       // �p���b�g�A�j���[�V�����X�V

  // 3D �`��
  DrawCircleGraphs( work ); // �~�O���t��`��
  DrawAnswerMarker( work ); // �񓚃}�[�J�[��`��
  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );

  // ��Ԃ��X�V
  CountUpStateCount( work ); // ��ԃJ�E���^�X�V
  SwitchState( work );       // ��ԍX�V
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I������
 *
 * @param work
 *
 * @return �O���t��ʂ��I�������ꍇ TRUE
 */
//-----------------------------------------------------------------------------------------
BOOL RRG_IsFinished( const RRG_WORK* work )
{
  return work->finishFlag;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�����ʂ̎擾
 *
 * @param work
 *
 * @return �O���t��ʂ̏I������
 */
//-----------------------------------------------------------------------------------------
RRG_RESULT RRG_GetResult( const RRG_WORK* work )
{
  return work->finishResult;
}



//=========================================================================================
// ��private functions
//=========================================================================================

//=========================================================================================
// ��LAYER 6 ��ԓ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ������� ( RRG_STATE_SETUP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_SETUP( RRG_WORK* work )
{
  CreateStateQueue( work );
  CreateFont( work );
  CreateMessages( work );
  CreateWordset( work );
  SetupTouchArea( work );
  UpdateTouchArea( work );

  // 3D �����ݒ�
  Setup3D();

  // BG ����
  SetupBG( work );
  SetupSubBG_WINDOW ( work );
  SetupSubBG_FONT( work );
  SetupMainBG_WINDOW( work );
  SetupMainBG_FONT( work );

  // ������`��I�u�W�F�N�g ����
  CreateBGFonts( work );

  // OBJ ����
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

  SetupResearchData( work );  // �����f�[�^���擾
  CreateCircleGraph( work );  // �~�O���t �쐬
  CreateArrow( work );        // ��� �쐬
  CreatePercentage( work );   // % �\���I�u�W�F�N�g����

  SetupPaletteFadeSystem( work ); // �p���b�g�t�F�[�h�V�X�e�� ����
  CreatePaletteAnime( work );     // �p���b�g�A�j���[�V�������[�N�𐶐�
  SetupPaletteAnime( work );      // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v
  RegisterVBlankTask( work );     // VBkank�^�X�N�o�^

  // �ʐM�A�C�R��
  SetupWirelessIcon( work );

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
  ShowAnalyzeButton( work );            //�w�񍐂�����x�{�^����\��
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_RECEIVE_BUTTON ); //�u�f�[�^��M���v�{�^���̃p���b�g�A�j�����J�n
  UpdateAnalyzeButton( work ); //�w�񍐂�����x�{�^�����X�V����

  // ��ʃt�F�[�h�C���J�n
  StartFadeIn();

  // ���̏�Ԃ��Z�b�g
  {
    RRG_STATE next_state;

    next_state = GetFirstState( work ); 
    RegisterNextState( work, next_state ); // ���̏�Ԃ��Z�b�g

    if( next_state == RRG_STATE_KEYWAIT ) {
      SetMenuCursorOn( work ); // �J�[�\��������Ă����Ԃɂ���
    } 
  }

  // ��ԏI��
  FinishCurrentState( work ); 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������� ( RRG_STATE_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_STANDBY( RRG_WORK* work )
{
  int key;
  int trg;
  int touch;
  int commonTouch;

  key = GFL_UI_KEY_GetCont();
  trg = GFL_UI_KEY_GetTrg(); 
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RRC_GetHitTable(work->commonWork) );

  //-----------------------
  //�w�߂�x�{�^�����^�b�`
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
    BlinkReturnButton( work );                    //�w�߂�x�{�^���𖾖ł�����
    PMSND_PlaySE( SEQ_SE_CANCEL1 );               // �L�����Z����
    FinishCurrentState( work );                   // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_WAIT );    // => RRG_STATE_WAIT
    RegisterNextState( work, RRG_STATE_FADEOUT ); // ==> RRG_STATE_FADEOUT
    RegisterNextState( work, RRG_STATE_CLEANUP ); // ===> RRG_STATE_CLEANUP
    return;
  }

  //----------
  // �L�[����
  if( (trg & PAD_KEY_UP)   || (trg & PAD_KEY_DOWN)  ||
      (trg & PAD_KEY_LEFT) || (trg & PAD_KEY_RIGHT) || (trg & PAD_BUTTON_A) ) {
    SetMenuCursorOn( work );                      // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                  //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                   // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT ); // => RRG_STATE_KEYWAIT
    return;
  }

  //-------------------
  //�u�~�O���t�v�^�b�`
  if( touch == TOUCH_AREA_GRAPH ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      work->analyzeByTouchFlag = TRUE;                 // �^�b�`�ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      UpdateBGFont_DataReceiving( work );              //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
      UpdateBGFont_Answer( work );                     //�u�񓚁v������̕\�����X�V����
      FinishCurrentState( work );                      // RRG_STATE_STANDBY ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_STANDBY );    // =====> RRG_STATE_STANDBY 
      return;
    }
  }

  //---------------------
  //�w�񍐂�����x�{�^�����^�b�`
  if( touch == TOUCH_AREA_ANALYZE_BUTTON ) {
    if( (work->analyzeFlag == FALSE) && (GetCountOfQuestion(work) != 0) ) {
      work->analyzeByTouchFlag = TRUE;                 // �^�b�`�ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      UpdateBGFont_DataReceiving( work );              //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
      UpdateBGFont_Answer( work );                     //�u�񓚁v������̕\�����X�V����
      FinishCurrentState( work );                      // RRG_STATE_STANDBY ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_STANDBY );    // =====> RRG_STATE_STANDBY 
      return;
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
      return;
    }
  }

  //-----------------------
  //�u����v�{�^�����^�b�`
  if( touch == TOUCH_AREA_QUESTION ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      work->analyzeByTouchFlag = TRUE;                 // �^�b�`�ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      MoveMenuCursorDirect( work, MENU_ITEM_ANSWER );  // �J�[�\���ʒu��ݒ�
      UpdateBGFont_DataReceiving( work );              //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
      UpdateBGFont_Answer( work );                     //�u�񓚁v������̕\�����X�V����
      FinishCurrentState( work );                      // RRG_STATE_STANDBY ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_STANDBY );    // =====> RRG_STATE_STANDBY 
      return;
    }
    else {
      MoveMenuCursorDirect( work, MENU_ITEM_QUESTION ); // �J�[�\���ʒu��ݒ�
      return;
    }
  }

  //-----------------------
  //�u�񓚁v�{�^�����^�b�`
  if( touch == TOUCH_AREA_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_ANSWER ); // �J�[�\���ʒu��ݒ�
    SetMenuCursorOn( work );                        // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                    //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                     // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT );   // => RRG_STATE_KEYWAIT
    return;
  } 
  //-----------------------------
  //�u�����̉񓚁v�{�^�����^�b�`
  if( touch == TOUCH_AREA_MY_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_MY_ANSWER ); // �J�[�\���ʒu��ݒ�
    SetMenuCursorOn( work );                           // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                       //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                        // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT );      // => RRG_STATE_KEYWAIT
    return;
  } 
  //-----------------------------
  //�u�񓚐l���v�{�^�����^�b�`
  if( touch == TOUCH_AREA_COUNT ) {
    MoveMenuCursorDirect( work, MENU_ITEM_COUNT ); // �J�[�\���ʒu��ݒ�
    SetMenuCursorOn( work );                       // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                   //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                    // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT );  // => RRG_STATE_KEYWAIT
    return;
  }

  //---------------------
  // ���J�[�\�����^�b�`
  if( touch == TOUCH_AREA_CONTROL_CURSOR_L ) {
    SetMenuCursorOn( work );                      // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                  //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                   // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT ); // => RRG_STATE_KEYWAIT
    // �J�[�\���ʒu�ɉ���������
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToPrev( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToPrev( work );    break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    }
    return;
  } 
  //---------------------
  // �E�J�[�\�����^�b�`
  if( touch == TOUCH_AREA_CONTROL_CURSOR_R ) {
    SetMenuCursorOn( work );                      // �J�[�\��������Ă����Ԃɂ���
    UpdateAnalyzeButton( work );                  //�w�񍐂�����x�{�^�����X�V����
    FinishCurrentState( work );                   // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_KEYWAIT ); // => RRG_STATE_KEYWAIT
    // �J�[�\���ʒu�ɉ���������
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToNext( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToNext( work );    break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    }
    return;
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON ); // ��ʑJ�ڂ̃g���K��o�^
    BlinkReturnButton( work );                     //�w�߂�x�{�^���𖾖ł�����
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                // �L�����Z����
    FinishCurrentState( work );                    // RRG_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_WAIT );     // => RRG_STATE_WAIT
    RegisterNextState( work, RRG_STATE_FADEOUT );  // ==> RRG_STATE_FADEOUT
    RegisterNextState( work, RRG_STATE_CLEANUP );  // ===> RRG_STATE_CLEANUP
    return;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂���� ( RRG_STATE_KEYWAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_KEYWAIT( RRG_WORK* work )
{
  int key;
  int trg;
  int touch;
  int commonTouch;

  key = GFL_UI_KEY_GetCont();
  trg = GFL_UI_KEY_GetTrg(); 
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RRC_GetHitTable(work->commonWork) );

  //------------------------
  // �����f�[�^�̍X�V�����o
  if( (work->analyzeFlag == TRUE) && (GAMEBEACON_Get_NewEntry() == TRUE) ) {
    FinishCurrentState( work );                      // RRG_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_UPDATE );     // => RRG_STATE_UPDATE
    RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT
    RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN
    RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE
    RegisterNextState( work, RRG_STATE_KEYWAIT );    // =====> RRG_STATE_KEYWAIT
    return;
  } 

  // TEST: �X�V����
#ifdef PM_DEBUG
  if( trg & PAD_BUTTON_DEBUG ) {
    FinishCurrentState( work );                      // RRG_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_UPDATE );     // => RRG_STATE_UPDATE
    RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT
    RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN
    RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE
    RegisterNextState( work, RRG_STATE_KEYWAIT );    // =====> RRG_STATE_KEYWAIT
    return;
  }
#endif

  //-----------------------
  //�w�߂�x�{�^�����^�b�`
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );  // ��ʑJ�ڂ̃g���K��o�^
    BlinkReturnButton( work );                     //�w�߂�x�{�^���𖾖ł�����
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                // �L�����Z����
    FinishCurrentState( work );                    // RRG_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_WAIT );     // => RRG_STATE_WAIT 
    RegisterNextState( work, RRG_STATE_FADEOUT );  // ==> RRG_STATE_FADEOUT 
    RegisterNextState( work, RRG_STATE_CLEANUP );  // ===> RRG_STATE_CLEANUP 
    return;
  }

  //--------
  // ���L�[
  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work ); // �J�[�\���ړ�
    return;
  } 
  //--------
  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work ); // �J�[�\���ړ�
    return;
  } 

  //-------------------
  //�u�~�O���t�v�^�b�`
  if( touch == TOUCH_AREA_GRAPH ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      work->analyzeByTouchFlag = TRUE;                 // �^�b�`�ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      FinishCurrentState( work );                      // RRG_STATE_KEYWAIT ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_STANDBY );    // =====> RRG_STATE_STANDBY 
      return;
    }
  } 
  //----------------------------
  //�w�񍐂�����x�{�^�����^�b�`
  if( touch == TOUCH_AREA_ANALYZE_BUTTON ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) ) {
      work->analyzeByTouchFlag = TRUE;                 // �^�b�`�ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      FinishCurrentState( work );                      // RRG_STATE_KEYWAIT ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_STANDBY );    // =====> RRG_STATE_STANDBY 
      return;
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
      return;
    }
  }

  //-----------------------
  //�u����v�{�^�����^�b�`
  if( touch == TOUCH_AREA_QUESTION ) {
    MoveMenuCursorDirect( work, MENU_ITEM_QUESTION ); // �J�[�\���ړ�
    return;
  }
  //-----------------------
  //�u�񓚁v�{�^�����^�b�`
  if( touch == TOUCH_AREA_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_ANSWER ); // �J�[�\���ړ�
    return;
  } 
  //-----------------------------
  //�u�����̉񓚁v�{�^�����^�b�`
  if( touch == TOUCH_AREA_MY_ANSWER ) {
    MoveMenuCursorDirect( work, MENU_ITEM_MY_ANSWER ); // �J�[�\���ړ�
    return;
  } 
  //-----------------------------
  //�u�񓚐l���v�{�^�����^�b�`
  if( touch == TOUCH_AREA_COUNT ) {
    MoveMenuCursorDirect( work, MENU_ITEM_COUNT ); // �J�[�\���ړ�
    return;
  }

  //-----------------------------
  // ���L�[ or ���J�[�\�����^�b�`
  if( (trg & PAD_KEY_LEFT) || (touch == TOUCH_AREA_CONTROL_CURSOR_L) ) {
    // �J�[�\���ʒu�ɉ���������
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToPrev( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToPrev( work );    break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    }
  } 
  //-----------------------------
  // ���L�[ or �E�J�[�\�����^�b�`
  if( (trg & PAD_KEY_RIGHT) || (touch == TOUCH_AREA_CONTROL_CURSOR_R) ) {
    // �J�[�\���ʒu�ɉ���������
    switch( work->cursorPos ) {
    case MENU_ITEM_QUESTION:  ChangeQuestionToNext( work );  break;
    case MENU_ITEM_ANSWER:    ChangeAnswerToNext( work );    break;
    case MENU_ITEM_COUNT:     SwitchDataDisplayType( work ); break;
    }
  }

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    if( (work->analyzeFlag == FALSE ) && (GetCountOfQuestion(work) != 0) && 
        ( (work->cursorPos == MENU_ITEM_QUESTION) || (work->cursorPos == MENU_ITEM_ANALYZE) ) ) {
      work->analyzeByTouchFlag = FALSE;                // �{�^���ŉ��
      BlinkAnalyzeButton( work );                      //�w�񍐂�����x�{�^���𖾖ł�����
      FinishCurrentState( work );                      // RRG_STATE_KEYWAIT ��Ԃ��I��
      RegisterNextState( work, RRG_STATE_ANALYZE );    // => RRG_STATE_ANALYZE 
      RegisterNextState( work, RRG_STATE_FLASHOUT );   // ==> RRG_STATE_FLASHOUT 
      RegisterNextState( work, RRG_STATE_FLASHIN );    // ===> RRG_STATE_FLASHIN 
      RegisterNextState( work, RRG_STATE_PERCENTAGE ); // ====> RRG_STATE_PERCENTAGE 
      RegisterNextState( work, RRG_STATE_KEYWAIT );    // =====> RRG_STATE_KEYWAIT 
      return;
    }
  } 

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON ); // ��ʑJ�ڂ̃g���K��o�^
    BlinkReturnButton( work );                     //�w�߂�x�{�^���𖾖ł�����
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                // �L�����Z����
    FinishCurrentState( work );                    // RRG_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRG_STATE_WAIT );     // => RRG_STATE_WAIT 
    RegisterNextState( work, RRG_STATE_FADEOUT );  // ==> RRG_STATE_FADEOUT 
    RegisterNextState( work, RRG_STATE_CLEANUP );  // ===> RRG_STATE_CLEANUP 
    return;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��͏�� ( RRG_STATE_ANALYZE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_ANALYZE( RRG_WORK* work )
{ 
  switch( GetStateSeq(work) ) {
  case 0:
    SetupResearchData( work );                              // �����f�[�^���ăZ�b�g�A�b�v
    ShowAnalyzeMessage( work );                             //�u�c�����������イ�c�v��\��
    SetupMainCircleGraph( work, GRAPH_DISP_MODE_TODAY );    // �~�O���t�쐬
    SetupMainCircleGraph( work, GRAPH_DISP_MODE_TOTAL );    // �~�O���t�쐬
    CIRCLE_GRAPH_SetDrawEnable( GetMainGraph(work), TRUE ); // �~�O���t�\���J�n
    CIRCLE_GRAPH_AnalyzeReq( GetMainGraph(work) );          // �~�O���t�\���J�n

    IncStateSeq( work );
    break;

  case 1:
    // SE ����~���Ă���
    if( PMSND_CheckPlaySE() == FALSE ) { 
      // ��莞�Ԃ��o��
      if( STATE_ANALYZE_FRAMES <= work->stateCount ) {
        IncStateSeq( work );
      } 
      else { 
        PMSND_PlaySE( SEQ_SE_SYS_81 ); // �f�[�^��͒�SE�����[�v������
      }
    }
    break;

  case 2:
    // ��͍ς݃t���O�𗧂Ă�
    work->analyzeFlag = TRUE;

    // �^�b�`�ŉ��
    if( work->analyzeByTouchFlag == FALSE ) {
      MoveMenuCursorSilent( work, MENU_ITEM_ANSWER ); // �J�[�\���ʒu���w�񓚁x�ɍ��킹��
    }
    // �{�^���ŉ��
    else {
      SetMenuCursorPos( work, MENU_ITEM_ANSWER ); // �J�[�\���ʒu���w�񓚁x�ɍ��킹��
      UpdateControlCursor( work );                // ���E�J�[�\�����X�V
      UpdateTouchArea( work );                    // �^�b�`�͈͂��X�V
    }

    // �\�����X�V
    ChangeAnswerToTop( work );          // �擪�̉񓚂�\��
    HideAnalyzeMessage( work );         //�u�c�����������イ�c�v������
    UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
    UpdateBGFont_Answer( work );        // �񓚂��X�V����
    UpdateBGFont_MyAnswer( work );      // �����̉񓚂��X�V����
    UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
    UpdateBGFont_Count( work );         // �񓚐l�����X�V����
    UpdateArrow( work );                // �����X�V����
    UpdateMyAnswerIconOnButton( work ); // �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����

    // �������ʕ\��SE
    PMSND_PlaySE( SEQ_SE_SYS_82 );

    FinishCurrentState( work );
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��͏�� ( RRG_STATE_PERCENTAGE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_PERCENTAGE( RRG_WORK* work )
{ 
  switch( GetStateSeq(work) ) {
  case 0:
    SetupPercentages( work ); // ���\���I�u�W�F�N�g���Z�b�g�A�b�v
    IncStateSeq( work );
    break;

  case 1:
    // �V���ȁ���\��
    if( work->stateCount % 10 == 0 ) {
      DispPercentage( work, work->percentageDispNum++ );
    }

    // �S�Ă̐��l��\�������玟�̏�Ԃ�
    if( work->percentageNum <= work->percentageDispNum ) {
      FinishCurrentState( work );
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʃt���b�V���V�[�P���X ( RRG_STATE_FLASHOUT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FLASHOUT( RRG_WORK* work )
{
  switch( GetStateSeq(work) ) {
  case 0:
    StopPaletteAnime( work, PALETTE_ANIME_HOLD ); //�w�񍐂�����x�{�^���̈Ó]�A�j�����~
    StartPaletteFadeFlashOut( work ); // �p���b�g�t�F�[�h�J�n
    IncStateSeq( work );
    break;

  case 1:
    // �p���b�g�t�F�[�h�I��
    if( IsPaletteFadeEnd( work ) ) {
      FinishCurrentState( work );
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʃt���b�V���V�[�P���X ( RRG_STATE_FLASHIN ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FLASHIN( RRG_WORK* work )
{
  switch( GetStateSeq(work) ) {
  case 0:
    // �p���b�g�t�F�[�h�J�n
    StartPaletteFadeFlashIn( work );
    IncStateSeq( work );
    break;

  case 1:
    // �p���b�g�t�F�[�h�I��
    if( IsPaletteFadeEnd( work ) ) {
      UpdateAnalyzeButton( work ); //�w�񍐂�����x�{�^�����X�V����
      FinishCurrentState( work );
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��Ԃւ̏������ ( RRG_STATE_UPDATE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_UPDATE( RRG_WORK* work )
{
  switch( GetStateSeq(work) ) {
  case 0:
    // �X�V�J�n
    work->updateFlag = TRUE;

    // �����f�[�^���ăZ�b�g�A�b�v
    SetupResearchData( work );

    // �\�����X�V
    SetMenuCursorOff( work );           // �J�[�\��������Ă��Ȃ���Ԃɂ���
    UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
    UpdateBGFont_Answer( work );        //�u�񓚁v������̕\�����X�V����
    UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
    UpdateArrow( work );                // ���̕\�����X�V����
    UpdateControlCursor( work );        // ���E�̃J�[�\���\�����X�V����
    VanishAllPercentage( work );        // ���\������������

    // �T�u�~�O���t�쐬
    SetupSubCircleGraph( work, GRAPH_DISP_MODE_TODAY );
    SetupSubCircleGraph( work, GRAPH_DISP_MODE_TOTAL );

    // �~�O���t�̏d�Ȃ���𒲐�
    AdjustCircleGraphLayer( work );

    // �~�O���t�\���J�n
    CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), TRUE );
    CIRCLE_GRAPH_UpdateReq( GetSubGraph(work) );

    IncStateSeq( work );
    break;

  case 1:
    // SE ����~���Ă���
    if( PMSND_CheckPlaySE() == FALSE ) { 
      // ��莞�Ԃ��o��
      if( STATE_UPDATE_FRAMES <= work->stateCount ) {
        IncStateSeq( work );
      } 
      else { 
        PMSND_PlaySE( SEQ_SE_SYS_81 ); // �f�[�^��M��SE�����[�v������
      }
    }
    break;

  case 2:
    // �X�V�I��
    work->updateFlag = FALSE;

    // �\�����X�V
    InterchangeCircleGraph( work ); // �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
    CIRCLE_GRAPH_SetDrawEnable( GetSubGraph(work), FALSE ); // �T�u�~�O���t ( �����C�� ) �\���I��
    UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V����
    SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
    UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
    UpdateBGFont_Answer( work );        // �񓚂��X�V����
    UpdateBGFont_Count( work );         // �񓚐l�����X�V����
    UpdateArrow( work );                // �����X�V����
    UpdateControlCursor( work );        // ���E�̃J�[�\���\�����X�V����
    DispAllPercentage( work );          // ���\�L��\������
    UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����

    // �X�V����SE
    PMSND_PlaySE( SEQ_SE_SYS_82 );

    FinishCurrentState( work );
    break;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��Ԃւ̏������ ( RRG_STATE_FADEOUT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_FADEOUT( RRG_WORK* work )
{
  switch( GetStateSeq(work) ) {
  case 0:
    // �t�F�[�h�A�E�g�J�n
    StartFadeOut();
    IncStateSeq( work );
    break; 

  case 1:
    // �t�F�[�h���I��
    if( GFL_FADE_CheckFade() == FALSE ) {
      FinishCurrentState( work );
    } 
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��Ԃւ̏������ ( RRG_STATE_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_WAIT( RRG_WORK* work )
{
  // �҂����Ԃ��o��
  if( GetWaitFrame(work) < work->stateCount ) {
    FinishCurrentState( work );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Еt����� ( RRG_STATE_CLEANUP ) �̏���
 *
 * @param work
 *
 * @return ��Ԃ��ω�����ꍇ ���̏�Ԕԍ�
 *         ��Ԃ��p������ꍇ ���݂̏�Ԕԍ�
 */
//-----------------------------------------------------------------------------------------
static void MainSeq_CLEANUP( RRG_WORK* work )
{ 
  // �p���b�g�A�j���[�V�������[�N
  CleanUpPaletteAnime( work );
  DeletePaletteAnime( work );

  // ���ʃp���b�g�A�j���[�V����
  RRC_StopAllPaletteAnime( work->commonWork ); // ��~����, 
  RRC_ResetAllPalette( work->commonWork );     // �p���b�g�����ɖ߂�

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
  SetFinishResult( work, RRG_RESULT_TO_TOP );  

  // ��ԏI��
  RegisterNextState( work, RRG_STATE_FINISH );
  FinishCurrentState( work );

  // �O���t��ʏI��
  work->finishFlag = TRUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���݂̏�Ԃ��I������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishCurrentState( RRG_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->stateEndFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->stateEndFlag = TRUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �O���t��ʂ̏I�����R��o�^����
 *
 * @param work
 * @param reason �I���̗��R
 */
//-----------------------------------------------------------------------------------------
static void SetFinishReason( RRG_WORK* work, SEQ_CHANGE_TRIG reason )
{
  RRC_SetSeqChangeTrig( work->commonWork, reason );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//-----------------------------------------------------------------------------------------
static void SetFinishResult( RRG_WORK* work, RRG_RESULT result )
{
  work->finishResult = result;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂���Ԃ̑҂����Ԃ�ݒ肷��
 *
 * @param work
 * @param frame �ݒ肷��҂��t���[����
 */
//-----------------------------------------------------------------------------------------
static void SetWaitFrame( RRG_WORK* work, u32 frame )
{
  work->waitFrame = frame;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂���Ԃ̑҂����Ԃ��擾����
 *
 * @param work
 *
 * @return �҂��t���[����
 */
//-----------------------------------------------------------------------------------------
static u32 GetWaitFrame( const RRG_WORK* work )
{
  return work->waitFrame;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̏�Ԃ��擾����    
 *
 * @param work
 *
 * @return �Z�b�g�A�b�v��̍ŏ��̏��
 */
//-----------------------------------------------------------------------------------------
RRG_STATE GetFirstState( const RRG_WORK* work )
{
  RRC_WORK* commonWork;
  RADAR_SEQ prev_seq;
  SEQ_CHANGE_TRIG trig;

  commonWork = work->commonWork;
  prev_seq   = RRC_GetPrevSeq( commonWork );
  trig       = RRC_GetSeqChangeTrig( commonWork );

  // �O�̉�ʂ��{�^���ŏI��
  if( (prev_seq != RADAR_SEQ_NULL) && (trig == SEQ_CHANGE_BY_BUTTON) ) {
    return RRG_STATE_KEYWAIT;
  }
  else {
    return RRG_STATE_STANDBY;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���X�V����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CountUpStateCount( RRG_WORK* work )
{
  (work->stateCount)++;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̏�Ԃ�o�^����
 *
 * @param work
 * @param nextSeq �o�^������
 */
//-----------------------------------------------------------------------------------------
static void RegisterNextState( RRG_WORK* work, RRG_STATE nextSeq )
{
  // ��ԃL���[�ɒǉ�����
  QUEUE_EnQueue( work->stateQueue, nextSeq );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SwitchState( RRG_WORK* work )
{
  RRG_STATE nextSeq;

  if( work->stateEndFlag == FALSE ){ return; }  // ���݂̏�Ԃ��I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->stateQueue ) ){ return; } // ��ԃL���[�ɓo�^����Ă��Ȃ�

  // ��Ԃ�ύX
  nextSeq = QUEUE_DeQueue( work->stateQueue );
  SetState( work, nextSeq ); 
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ݒ肷��
 *
 * @param work
 * @parma nextSeq �ݒ肷����
 */
//-----------------------------------------------------------------------------------------
static void SetState( RRG_WORK* work, RRG_STATE nextSeq )
{ 
  work->state        = nextSeq;
  work->stateSeq     = 0;
  work->stateCount   = 0;
  work->stateEndFlag = FALSE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�ԍ����擾����
 *
 * @param work
 *
 * @return ��ԓ��V�[�P���X�ԍ�
 */
//-----------------------------------------------------------------------------------------
static u32 GetStateSeq( const RRG_WORK* work )
{
  return work->stateSeq;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�ԍ����C���N�������g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void IncStateSeq( RRG_WORK* work )
{
  (work->stateSeq)++;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�ԍ������Z�b�g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ResetStateSeq( RRG_WORK* work )
{
  work->stateSeq = 0;
}


//=========================================================================================
// ��LAYER 5 �@�\
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorUp( RRG_WORK* work )
{ 
  BOOL loop = TRUE;

  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, -1 );

  // �J�[�\���ʒu�𒲐�
  while( loop )
  {
    //�u�񓚁vor�u�����̉񓚁v
    if( (work->cursorPos == MENU_ITEM_ANSWER) || (work->cursorPos == MENU_ITEM_MY_ANSWER) ) {
      // ����� or�u�������ܒ������v
      if( (work->analyzeFlag == FALSE) || (GetCountOfQuestion(work) == 0) ) {
        ShiftMenuCursorPos( work, -1 ); 
      }
      else {
        loop = FALSE;
      }
    }
    //�w�񍐂�����x�{�^��
    else if( work->cursorPos == MENU_ITEM_ANALYZE ) {
      // ��͍ς� or�u�������ܒ������v
      if( (work->analyzeFlag == TRUE) || (GetCountOfQuestion(work) == 0) ) {
        ShiftMenuCursorPos( work, -1 ); 
      }
      else {
        loop = FALSE;
      }
    }
    else {
      loop = FALSE;
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V
  UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����

  if( work->cursorPos == MENU_ITEM_ANALYZE ) {
    SetAnalyzeButtonCursorSet( work ); //�w�񍐂�����x�{�^���̃J�[�\���Z�b�g�A�j�����J�n����
  }

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
static void MoveMenuCursorDown( RRG_WORK* work )
{
  BOOL loop = TRUE;

  // �\�����X�V
  SetMenuCursorOff( work );  // �J�[�\��������Ă��Ȃ���Ԃɂ���

  // �J�[�\���ړ�
  ShiftMenuCursorPos( work, 1 );

  // �J�[�\���ʒu�𒲐�
  while( loop )
  {
    //�u�񓚁vor�u�����̉񓚁v
    if( (work->cursorPos == MENU_ITEM_ANSWER) || (work->cursorPos == MENU_ITEM_MY_ANSWER) ) {
      // ����� or�u�������ܒ������v
      if( (work->analyzeFlag == FALSE) || (GetCountOfQuestion(work) == 0) ) {
        ShiftMenuCursorPos( work, 1 ); 
      }
      else {
        loop = FALSE;
      }
    }
    //�w�񍐂�����x�{�^��
    else if( work->cursorPos == MENU_ITEM_ANALYZE ) {
      // ��͍ς� or�u�������ܒ������v
      if( (work->analyzeFlag == TRUE) || (GetCountOfQuestion(work) == 0) ) {
        ShiftMenuCursorPos( work, 1 ); 
      }
      else {
        loop = FALSE;
      }
    }
    else {
      loop = FALSE;
    }
  }

  // �\�����X�V
  SetMenuCursorOn( work );      // �J�[�\��������Ă����Ԃɂ���
  UpdateControlCursor( work );  // ���E�J�[�\�����X�V
  UpdateMyAnswerIconOnGraph( work );  // �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����

  if( work->cursorPos == MENU_ITEM_ANALYZE ) {
    SetAnalyzeButtonCursorSet( work ); //�w�񍐂�����x�{�^���̃J�[�\���Z�b�g�A�j�����J�n����
  }

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
static void MoveMenuCursorDirect( RRG_WORK* work, MENU_ITEM menuItem )
{
  MoveMenuCursorSilent( work, menuItem );

  // �J�[�\���Z�b�g�̃p���b�g�A�j�����J�n����
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET ); 

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 ); 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���𒼐ڈړ����� ( �_�ŁESE�Ȃ� )
 *
 * @param work
 * @param menuItem �ړ���̃��j���[����
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorSilent( RRG_WORK* work, MENU_ITEM menuItem )
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
static void ChangeQuestionToNext( RRG_WORK* work )
{
  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
  }

  // �f�[�^���X�V
  SetupResearchData( work );   // �����f�[�^���ăZ�b�g�A�b�v
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
  UpdateAnalyzeButton( work );          //�w�񍐂�����x�{�^�����X�V����

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
static void ChangeQuestionToPrev( RRG_WORK* work )
{
  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
  }

  // �f�[�^���X�V
  SetupResearchData( work );    // �����f�[�^���ăZ�b�g�A�b�v
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
  UpdateAnalyzeButton( work );          //�w�񍐂�����x�{�^�����X�V����

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
static void ChangeAnswerToNext( RRG_WORK* work )
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
static void ChangeAnswerToPrev( RRG_WORK* work )
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
static void ChangeAnswerToTop( RRG_WORK* work )
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
static void SwitchDataDisplayType( RRG_WORK* work )
{
  CIRCLE_GRAPH* graph;
  GRAPH_DISP_MODE nextType;
  BOOL newGraphWait = FALSE; // �V�����\������~�O���t��, �\������܂ł̑҂����Ԃ��K�v���ǂ���

  // �~�O���t�̃A�j���[�V�������͎󂯕t���Ȃ�
  if( CheckAllGraphAnimeEnd(work) == FALSE ) { return; }

  // �\�����̉~�O���t����������
  if( work->analyzeFlag && GetCountOfQuestion(work) != 0 ) {
    CIRCLE_GRAPH_DisappearReq( GetMainGraph(work) );
    newGraphWait = TRUE;
  }

  // �\���^�C�v��ύX
  switch( work->graphMode ) {
  case GRAPH_DISP_MODE_TODAY: nextType = GRAPH_DISP_MODE_TOTAL; break;
  case GRAPH_DISP_MODE_TOTAL: nextType = GRAPH_DISP_MODE_TODAY; break;
  default: GF_ASSERT(0);
  } 
  SetDataDisplayType( work, nextType );

  // �\�����X�V
  UpdateMainBG_WINDOW( work );        // MAIN-BG ( �E�B���h�E�� ) ���X�V
  SetMenuCursorOn( work );            // �J�[�\��������Ă����Ԃɂ���
  UpdateBGFont_Answer( work );        // �񓚂��X�V����
  UpdateBGFont_Count( work );         // �񓚐l�����X�V����
  UpdateBGFont_NoData( work );        //�u�������� ���傤�����イ�v�̕\�����X�V����
  UpdateBGFont_DataReceiving( work ); //�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
  UpdateArrow( work );                // �����X�V����

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

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�C�����J�n����
 */
//-----------------------------------------------------------------------------------------
static void StartFadeIn( void )
{
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, 0);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�A�E�g���J�n����
 */
//-----------------------------------------------------------------------------------------
static void StartFadeOut( void )
{
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      0, 16, 0);
}


//=========================================================================================
// ��LAYER 4 �ʑ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�����ړ�����
 *
 * @parma work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void ShiftMenuCursorPos( RRG_WORK* work, int stride )
{
  int nowPos;
  int nextPos;

  // �J�[�\���ʒu���X�V
  nowPos  = work->cursorPos;
  nextPos = (nowPos + stride + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  work->cursorPos = nextPos; 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���𒼐ڎw�肵�ĕύX����
 *
 * @param work
 * @param menuItem �ύX��̃J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorPos( RRG_WORK* work, MENU_ITEM menuItem )
{
  // �J�[�\���ʒu���X�V
  work->cursorPos = menuItem;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorOn( RRG_WORK* work )
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

  //�w�񍐂�����x�{�^��
  case MENU_ITEM_ANALYZE:
    SetAnalyzeButtonCursorOn( work );
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���]�����N�G�X�g
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW ); 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��郁�j���[���ڂ�, ���j���[���ڂ��J�[�\��������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorOff( RRG_WORK* work )
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

  //�w�񍐂�����x�{�^��
  case MENU_ITEM_ANALYZE:
    SetAnalyzeButtonCursorOff( work );
    break;

  // �G���[
  default:
    GF_ASSERT(0);
  }

  // �X�N���[���]�����N�G�X�g
  GFL_BG_LoadScreenReq( MAIN_BG_WINDOW );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\�����鎿��̃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void ShiftQuestionIdx( RRG_WORK* work, int stride )
{
  int nowIdx;
  int nextIdx;

  // �C���f�b�N�X��ύX
  nowIdx  = work->questionIdx;
  nextIdx = ( nowIdx + stride + QUESTION_NUM_PER_TOPIC ) % QUESTION_NUM_PER_TOPIC;
  work->questionIdx = nextIdx;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X��ύX����
 *
 * @param work
 * @param stride �����N�ύX��
 */
//-----------------------------------------------------------------------------------------
static void ShiftAnswerIdx( RRG_WORK* work, int stride )
{
  int questionID;
  int answerID, nextAnswerID;
  int answerRank, nextAnswerRank;
  int answerNum;
  int nextAnswerIdx;
  CIRCLE_GRAPH* graph;
  
  // ��{�����擾
  graph      = GetMainGraph( work );
  questionID = GetQuestionID( work );
  answerNum  = GetAnswerNum( work );

  // �\�����̉񓚂̃����N���擾
  answerID   = GetAnswerID( work );
  answerRank = CIRCLE_GRAPH_GetComponentRank_byID( graph, answerID );

  // �X�V��̃����N�̉񓚃C���f�b�N�X���擾
  nextAnswerRank = (answerRank + stride + answerNum) % answerNum;
  nextAnswerID   = CIRCLE_GRAPH_GetComponentID_byRank( graph, nextAnswerRank );
  nextAnswerIdx  = RESEARCH_DATA_GetAnswerIndex_byID( &(work->researchData), questionID, nextAnswerID );

  // �C���f�b�N�X��ύX
  work->answerIdx = nextAnswerIdx;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�𒆂̉񓚃C���f�b�N�X�����Z�b�g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ResetAnswerIdx( RRG_WORK* work )
{
  work->answerIdx = 0;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateAnalyzeButton( RRG_WORK* work )
{
  // �{�^���������Ȃ�
  if( (work->analyzeFlag == TRUE) || // ��͍ς�
      (work->updateFlag == TRUE) || // �X�V��
      (GetCountOfQuestion(work) == 0) ) { //�u�������ܒ������v
    // �Ó]���Ă��Ȃ�
    if( CheckPaletteAnime( work, PALETTE_ANIME_HOLD ) == FALSE ) {
      SetAnalyzeButtonNotActive( work );
    }
  }
  // �{�^����������
  else {
    // �Ó]���Ă���
    if( CheckPaletteAnime( work, PALETTE_ANIME_HOLD ) == TRUE ) {
      SetAnalyzeButtonActive( work );
    }
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^����_�ł����� 
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void BlinkAnalyzeButton( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 0 );

  StopPaletteAnime( work, PALETTE_ANIME_HOLD );
  StopPaletteAnime( work, PALETTE_ANIME_RECOVER );
  StartPaletteAnime( work, PALETTE_ANIME_SELECT );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetAnalyzeButtonCursorOn( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 0 );

  // �p���b�g�A�j���[�V�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_ANALYZE_CURSOR_ON );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����J�[�\��������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetAnalyzeButtonCursorOff( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 1 );

  // �p���b�g�A�j���[�V������~
  StopPaletteAnime( work, PALETTE_ANIME_ANALYZE_CURSOR_ON );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����J�[�\�����Z�b�g������Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetAnalyzeButtonCursorSet( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 0 );

  // �p���b�g�A�j���[�V�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_ANALYZE_CURSOR_SET );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����A�N�e�B�u��Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetAnalyzeButtonActive( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 1 );

  // �Ó]���I��
  StopPaletteAnime( work, PALETTE_ANIME_HOLD ); 

  // ���A������
  StartPaletteAnime( work, PALETTE_ANIME_RECOVER ); 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^�����A�N�e�B�u��Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetAnalyzeButtonNotActive( RRG_WORK* work )
{
  // �K�p����p���b�g�ԍ���ύX
  BmpOam_ActorSetPaletteOffset( work->BmpOamActor[ BMPOAM_ACTOR_ANALYZE_BUTTON ], 1 );

  // �Ó]������
  StartPaletteAnime( work, PALETTE_ANIME_HOLD ); 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�߂�v�{�^����_�ł����� 
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void BlinkReturnButton( RRG_WORK* work )
{
  RRC_StartPaletteAnime( work->commonWork, COMMON_PALETTE_ANIME_RETURN );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�ƃ��C���~�O���t�����ւ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InterchangeCircleGraph( RRG_WORK* work )
{
  int typeIdx;
  CIRCLE_GRAPH* temp;

  for( typeIdx=0; typeIdx < GRAPH_DISP_MODE_NUM; typeIdx++ )
  {
    CIRCLE_GRAPH* temp         = work->mainGraph[ typeIdx ];
    work->mainGraph[ typeIdx ] = work->subGraph[ typeIdx ];
    work->subGraph[ typeIdx ]  = temp;
  }
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
static void AdjustCircleGraphLayer( RRG_WORK* work )
{
  // �T�u�~�O���t����O�ɕ\�������悤��z���W��ݒ肷��
  CIRCLE_GRAPH_SetCenterZ( GetMainGraph(work), FX16_CONST(-2.0f) );
  CIRCLE_GRAPH_SetCenterZ( GetSubGraph(work), FX16_CONST(0.0f) );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���C���~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param graphMode �\���^�C�v
 */
//-----------------------------------------------------------------------------------------
static void SetupMainCircleGraph( RRG_WORK* work, GRAPH_DISP_MODE graphMode )
{
  int aIdx;
  CIRCLE_GRAPH* graph;
  GRAPH_COMPONENT_ADD_DATA components[ MAX_ANSWER_NUM_PER_QUESTION ];
  u8 answerNum;
  const QUESTION_DATA* questionData;
  const ANSWER_DATA* answerData;

  graph        = work->mainGraph[ graphMode ];
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
    switch( graphMode ) {
    case GRAPH_DISP_MODE_TODAY: components[ aIdx ].value = answerData->todayCount; break;
    case GRAPH_DISP_MODE_TOTAL: components[ aIdx ].value = answerData->totalCount; break;
    default: GF_ASSERT(0);
    }
  }

  // �O���t�̍\���v�f���Z�b�g
  CIRCLE_GRAPH_SetupComponents( graph, components, answerNum );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �T�u�~�O���t�����݂̃f�[�^�ō\������
 *
 * @param work
 * @param graphMode �\���^�C�v
 */
//-----------------------------------------------------------------------------------------
static void SetupSubCircleGraph( RRG_WORK* work, GRAPH_DISP_MODE graphMode )
{
  int aIdx;
  CIRCLE_GRAPH* graph;
  GRAPH_COMPONENT_ADD_DATA components[ MAX_ANSWER_NUM_PER_QUESTION ];
  u8 answerNum;
  const QUESTION_DATA* questionData;
  const ANSWER_DATA* answerData;

  graph        = work->subGraph[ graphMode ];
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
    switch( graphMode ) {
    case GRAPH_DISP_MODE_TODAY: components[ aIdx ].value = answerData->todayCount; break;
    case GRAPH_DISP_MODE_TOTAL: components[ aIdx ].value = answerData->totalCount; break;
    default: GF_ASSERT(0);
    }
  }

  // �O���t�̍\���v�f���Z�b�g
  CIRCLE_GRAPH_SetupComponents( graph, components, answerNum );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ׂẲ~�O���t���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateCircleGraphs( RRG_WORK* work )
{
  int typeIdx;

  for( typeIdx=0; typeIdx < GRAPH_DISP_MODE_NUM; typeIdx++ )
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
static void DrawCircleGraphs( const RRG_WORK* work )
{
  int typeIdx;

  for( typeIdx=0; typeIdx < GRAPH_DISP_MODE_NUM; typeIdx++ )
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
static void DrawAnswerMarker( const RRG_WORK* work )
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
 * @brief ���̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateArrow( RRG_WORK* work )
{
  // ����
  ARROW_Vanish( work->arrow );

  // �X�V���͕\�����Ȃ�
  if( work->updateFlag == TRUE ) { return; }

  // ����͂Ȃ�\�����Ȃ�
  if( !work->analyzeFlag ) { return; }

  // �Ώۂ�0%�Ȃ�\�����Ȃ�
  if( CIRCLE_GRAPH_GetComponentPercentage_byID( GetMainGraph(work), GetAnswerID(work) ) == 0 ) { return; }

  // �\���J�n
  if( work->analyzeFlag ) {
    int endX, endY;
    CIRCLE_GRAPH_GetComponentPointPos_byID( GetMainGraph(work), GetAnswerID(work), &endX, &endY );
    ARROW_Setup( work->arrow, ARROW_START_X, ARROW_START_Y, endX, endY );
    ARROW_StartAnime( work->arrow );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���\���I�u�W�F�N�g��, ���݂̒����f�[�^�ɂ��킹�ăZ�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupPercentages( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�Ắ��\������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void VanishAllPercentage( RRG_WORK* work )
{
  int idx;

  // ���ׂĔ�\���ɂ���
  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    PERCENTAGE_SetDrawEnable( work->percentage[ idx ], FALSE );
  }
  work->percentageDispNum = 0;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���I�u�W�F�N�g��\������
 *
 * @param work
 * @param index �\�����道�I�u�W�F�N�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
static void DispPercentage( RRG_WORK* work, u8 index )
{
  PERCENTAGE_SetDrawEnable( work->percentage[ index ], TRUE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�Ắ��I�u�W�F�N�g��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DispAllPercentage( RRG_WORK* work )
{
  int idx;
  int num;

  num = work->percentageNum;

  for( idx=0; idx < num; idx++ )
  {
    DispPercentage( work, idx );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �^�b�`�͈͂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTouchArea( RRG_WORK* work )
{
  const MENU_ITEM_DRAW_PARAM* menu;
  GFL_UI_TP_HITTBL* area;

  // ���J�[�\��
  menu = &( MenuItemDrawParam[ work->cursorPos ] );
  area = &( work->touchHitTable[ TOUCH_AREA_CONTROL_CURSOR_L ] );
  area->rect.left   = Bind_u8( menu->left_dot + menu->leftCursorOffsetX - CLWK_CONTROL_CURSOR_L_WIDTH/2 );
  area->rect.top    = Bind_u8( menu->top_dot  + menu->leftCursorOffsetY - CLWK_CONTROL_CURSOR_L_HEIGHT/2 );
  area->rect.right  = Bind_u8( area->rect.left + TOUCH_AREA_CONTROL_CURSOR_WIDTH );
  area->rect.bottom = Bind_u8( area->rect.top  + TOUCH_AREA_CONTROL_CURSOR_HEIGHT );

  // �E�J�[�\��
  menu = &( MenuItemDrawParam[ work->cursorPos ] );
  area = &( work->touchHitTable[ TOUCH_AREA_CONTROL_CURSOR_R ] );
  area->rect.left   = Bind_u8( menu->left_dot + menu->rightCursorOffsetX - CLWK_CONTROL_CURSOR_R_WIDTH/2 );
  area->rect.top    = Bind_u8( menu->top_dot  + menu->rightCursorOffsetY - CLWK_CONTROL_CURSOR_R_HEIGHT/2 );
  area->rect.right  = Bind_u8( area->rect.left + TOUCH_AREA_CONTROL_CURSOR_WIDTH );
  area->rect.bottom = Bind_u8( area->rect.top  + TOUCH_AREA_CONTROL_CURSOR_HEIGHT );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-BG ( �E�B���h�E�� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateMainBG_WINDOW( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ږ����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_TopicTitle( RRG_WORK* work )
{
  u32 topicID;
  u32 strID;

  // �������ڂ̕⑫���̕�����ID���擾
  topicID = work->researchData.topicID;
  strID = StringID_topicTitle[ topicID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( work->BGFont[ SUB_BG_FONT_TOPIC_TITLE ], strID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����̕⑫�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_QuestionCaption( RRG_WORK* work )
{
  u32 questionID;
  u32 strID;

  // ����̕⑫���̕�����ID���擾
  questionID  = GetQuestionID( work );
  strID       = CaptionStringID_question[ questionID ];

  // BG ( �t�H���g�� ) �ɑ΂�, ���������������
  BG_FONT_SetMessage( work->BGFont[ SUB_BG_FONT_QUESTION_CAPTION ], strID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������X�V���� 
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_Question( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �񓚂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_Answer( RRG_WORK* work )
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
  BG_FONT_SetDrawEnable( BGFont, TRUE );

  GFL_STR_DeleteBuffer( strbuf_plain );
  GFL_STR_DeleteBuffer( strbuf_expand );
  GFL_STR_DeleteBuffer( strbuf_answer );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_MyAnswer( RRG_WORK* work )
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
  BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_MY_ANSWER ], TRUE );
  BG_FONT_SetString( work->BGFont[ MAIN_BG_FONT_MY_ANSWER ], strbuf_expand );

  GFL_STR_DeleteBuffer( strbuf_plain );
  GFL_STR_DeleteBuffer( strbuf_expand );
  GFL_STR_DeleteBuffer( strbuf_myAnswer );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �񓚐l�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_Count( RRG_WORK* work )
{
  u32 strID; 
  STRBUF* strbuf_plain;  // �W�J�O�̕�����
  STRBUF* strbuf_expand; // �W�J��̕�����

  // ������ID������
  if( work->analyzeFlag ) { // ��͍ς�
    switch( work->graphMode ) {
    case GRAPH_DISP_MODE_TODAY: strID = str_check_today_count; break;
    case GRAPH_DISP_MODE_TOTAL: strID = str_check_total_count; break;
    default: GF_ASSERT(0);
    }
  }
  else { // �����
    switch( work->graphMode ) {
    case GRAPH_DISP_MODE_TODAY: strID = str_check_today_count01; break;
    case GRAPH_DISP_MODE_TOTAL: strID = str_check_total_count01; break;
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�������� ���傤�����イ�v�̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_NoData( RRG_WORK* work )
{
  if( GetCountOfQuestion(work) == 0 ) { 
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_NO_DATA ], TRUE ); // �\��
  }
  else { 
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_NO_DATA ], FALSE ); // �N���A
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @breif�u�f�[�^����Ƃ����イ�v�̕\�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGFont_DataReceiving( RRG_WORK* work )
{
  if( work->updateFlag ) {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_DATA_RECEIVING ], TRUE ); // �\��  
  }
  else {
    BG_FONT_SetDrawEnable( work->BGFont[ MAIN_BG_FONT_DATA_RECEIVING ], FALSE ); // �N���A
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����J�[�\���̕\�����X�V����
 *
 * @param work 
 */
//-----------------------------------------------------------------------------------------
static void UpdateControlCursor( RRG_WORK* work )
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

  // �X�V�� or�u�����̉񓚁vor �w�񍐂�����x�{�^����I�����Ă���ꍇ�͕\�����Ȃ�
  if( (work->updateFlag == TRUE ) || 
      (work->cursorPos == MENU_ITEM_MY_ANSWER) ||
      (work->cursorPos == MENU_ITEM_ANALYZE) ) {
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚃A�C�R�� ( �{�^���� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateMyAnswerIconOnButton( RRG_WORK* work )
{
  // ����͂Ȃ�\�����Ȃ�
  if( work->analyzeFlag == FALSE ) {
    GFL_CLACT_WK_SetDrawEnable( work->clactWork[ CLWK_MY_ANSWER_ICON_ON_BUTTON ], FALSE );
    return;
  }

  // �\��
  GFL_CLACT_WK_SetDrawEnable( work->clactWork[ CLWK_MY_ANSWER_ICON_ON_BUTTON ], TRUE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����̉񓚃A�C�R�� ( �O���t�� ) ���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
void UpdateMyAnswerIconOnGraph( RRG_WORK* work )
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

  // �Ώۂ�0%�Ȃ�\�����Ȃ�
  if( CIRCLE_GRAPH_GetComponentPercentage_byID( GetMainGraph(work), answerID ) == 0 ) { 
    GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
    return; 
  }

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

  // �A�j���[�V�����J�n
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, TRUE );
  GFL_CLACT_WK_SetAnmMode( clwk, CLSYS_ANMPM_FORWARD_L );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�w�񍐂�����x�{�^����\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ShowAnalyzeButton( RRG_WORK* work )
{
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZE_BUTTON, TRUE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�c�����������イ�c�v��\������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ShowAnalyzeMessage( RRG_WORK* work )
{
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, TRUE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u�c�����������イ�c�v���\���ɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void HideAnalyzeMessage( RRG_WORK* work )
{
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_ANALYZING, FALSE );
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
static void BmpOamSetDrawEnable( RRG_WORK* work, BMPOAM_ACTOR_INDEX actorIdx, BOOL enable )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( actorIdx < BMPOAM_ACTOR_NUM );

  // �\����Ԃ�ύX
  BmpOam_ActorSetDrawEnable( work->BmpOamActor[ actorIdx ], enable );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RRG_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       PaletteAnimeData[ index ].animeType,
                       PaletteAnimeData[ index ].fadeColor );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 * @param index ��~����A�j���[�V�������w��
 */
//------------------------------------------------------------------------------------
static void StopPaletteAnime( RRG_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V���������ǂ������`�F�b�N����
 *
 * @param work
 * @param index �`�F�b�N����A�j���[�V�������w��
 *
 * @return �A�j���[�V�������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
static BOOL CheckPaletteAnime( const RRG_WORK* work, PALETTE_ANIME_INDEX index )
{
  return PALETTE_ANIME_CheckAnime( work->paletteAnime[ index ] );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Update( work->paletteAnime[ idx ] );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ���ʃp���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdateCommonPaletteAnime( RRG_WORK* work )
{
  RRC_UpdatePaletteAnime( work->commonWork );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ɂ��t���b�V�� ( �A�E�g ) ���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeFlashOut( RRG_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_FLASHOUT_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_FLASHOUT_WAIT,
                  MAIN_BG_PALETTE_FADE_FLASHOUT_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASHOUT_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASHOUT_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_FLASHOUT_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_FLASHOUT_WAIT,
                  MAIN_OBJ_PALETTE_FADE_FLASHOUT_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASHOUT_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASHOUT_COLOR,
                  work->VBlankTCBSystem );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ɂ��t���b�V�� ( �C�� ) ���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeFlashIn( RRG_WORK* work )
{
  // MAIN-BG
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_BG,
                  MAIN_BG_PALETTE_FADE_FLASHIN_TARGET_BITMASK,
                  MAIN_BG_PALETTE_FADE_FLASHIN_WAIT,
                  MAIN_BG_PALETTE_FADE_FLASHIN_START_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASHIN_END_STRENGTH,
                  MAIN_BG_PALETTE_FADE_FLASHIN_COLOR,
                  work->VBlankTCBSystem );

  // MAIN-OBJ
  PaletteFadeReq( work->paletteFadeSystem, 
                  PF_BIT_MAIN_OBJ,
                  MAIN_OBJ_PALETTE_FADE_FLASHIN_TARGET_BITMASK,
                  MAIN_OBJ_PALETTE_FADE_FLASHIN_WAIT,
                  MAIN_OBJ_PALETTE_FADE_FLASHIN_START_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASHIN_END_STRENGTH,
                  MAIN_OBJ_PALETTE_FADE_FLASHIN_COLOR,
                  work->VBlankTCBSystem );
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
static BOOL IsPaletteFadeEnd( RRG_WORK* work )
{
  return (PaletteFadeCheck( work->paletteFadeSystem ) == 0 );
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
  RRG_WORK* work = (RRG_WORK*)wk;

  GFL_BG_VBlankFunc(); // BG
  GFL_CLACT_SYS_VBlankFunc(); // OBJ
  PaletteFadeTrans( work->paletteFadeSystem ); // �p���b�g�t�F�[�h
}


//=========================================================================================
// ��LAYER 3 �擾�E�ݒ�E����
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �Q�[���V�X�e�����擾����
 *
 * @param work
 *
 * @return �Q�[���V�X�e��
 */
//-----------------------------------------------------------------------------------------
static GAMESYS_WORK* GetGameSystem( const RRG_WORK* work )
{
  return RRC_GetGameSystem( work->commonWork );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Q�[���f�[�^���擾����
 *
 * @param work
 *
 * @return �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------------------
static GAMEDATA* GetGameData( const RRG_WORK* work )
{
  return RRC_GetGameData( work->commonWork );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �q�[�vID���擾����
 *
 * @param work
 *
 * @return �q�[�vID
 */
//-----------------------------------------------------------------------------------------
static HEAPID GetHeapID( const RRG_WORK* work )
{
  return work->heapID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �q�[�vID��ݒ肷��
 *
 * @param work
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
static void SetHeapID( RRG_WORK* work, HEAPID heapID )
{
  work->heapID = heapID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N���擾����
 *
 * @param work
 *
 * @return �S��ʋ��ʃ��[�N
 */
//-----------------------------------------------------------------------------------------
static RRC_WORK* GetCommonWork( const RRG_WORK* work )
{
  return work->commonWork;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N��ݒ肷��
 *
 * @param work
 * @param commonWork
 */
//-----------------------------------------------------------------------------------------
static void SetCommonWork( RRG_WORK* work, RRC_WORK* commonWork )
{
  work->commonWork = commonWork;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^�̕\���^�C�v��ݒ肷��
 *
 * @param work
 * @param graphMode �ݒ肷��\���^�C�v
 */
//-----------------------------------------------------------------------------------------
static void SetDataDisplayType( RRG_WORK* work, GRAPH_DISP_MODE graphMode )
{
  work->graphMode = graphMode;
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
static CIRCLE_GRAPH* GetMainGraph( const RRG_WORK* work )
{
  return work->mainGraph[ work->graphMode ];
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
static CIRCLE_GRAPH* GetSubGraph ( const RRG_WORK* work )
{
  return work->subGraph[ work->graphMode ];
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
static BOOL CheckAllGraphAnimeEnd( const RRG_WORK* work )
{
  if( CIRCLE_GRAPH_IsAnime( work->mainGraph[ GRAPH_DISP_MODE_TODAY ] ) ||
      CIRCLE_GRAPH_IsAnime( work->mainGraph[ GRAPH_DISP_MODE_TOTAL ] ) ||
      CIRCLE_GRAPH_IsAnime( work->subGraph[ GRAPH_DISP_MODE_TODAY ] ) ||
      CIRCLE_GRAPH_IsAnime( work->subGraph[ GRAPH_DISP_MODE_TOTAL ] ) ) { return FALSE; }

  return TRUE;
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
static u8 GetTopicID( const RRG_WORK* work )
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
static u8 GetQuestionID( const RRG_WORK* work )
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
static u8 GetAnswerNum( const RRG_WORK* work )
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
static u16 GetAnswerID( const RRG_WORK* work )
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
static u16 GetCountOfQuestion( const RRG_WORK* work )
{
  u16 count = 0;

  switch( work->graphMode ) {
  case GRAPH_DISP_MODE_TODAY:  count = GetTodayCountOfQuestion( work ); break;
  case GRAPH_DISP_MODE_TOTAL:  count = GetTotalCountOfQuestion( work ); break;
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
static u16 GetTodayCountOfQuestion( const RRG_WORK* work )
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
static u16 GetTotalCountOfQuestion( const RRG_WORK* work )
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
static u16 GetCountOfAnswer( const RRG_WORK* work )
{
  switch( work->graphMode ) {
  case GRAPH_DISP_MODE_TODAY: return GetTodayCountOfAnswer( work ); break;
  case GRAPH_DISP_MODE_TOTAL: return GetTotalCountOfAnswer( work ); break;
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
static u16 GetTodayCountOfAnswer( const RRG_WORK* work )
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
static u16 GetTotalCountOfAnswer( const RRG_WORK* work )
{
  u8 qIdx;
  u8 aIdx;

  qIdx = work->questionIdx;
  aIdx = work->answerIdx;

  return work->researchData.questionData[ qIdx ].answerData[ aIdx ].totalCount;
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
static u8 GetInvestigatingTopicID( const RRG_WORK* work )
{
  int qIdx;
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u8 topicID;

  gameData = GetGameData( work );
  save  = GAMEDATA_GetSaveControlWork( gameData );
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
u8 GetMyAnswerID( const RRG_WORK* work )
{
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  QUESTIONNAIRE_ANSWER_WORK* myAnswer;
  const RESEARCH_DATA* researchData;
  u8 questionID, questionIdx;
  u16 answerID, answerIdx;

  // �Z�[�u�f�[�^���擾
  gameData = GetGameData( work );
  save     = GAMEDATA_GetSaveControlWork( gameData );
  QSave    = SaveData_GetQuestionnaire( save );
  myAnswer = Questionnaire_GetAnswerWork( QSave );

  // ����ID���擾
  questionID = GetQuestionID( work );

  // ���₪�w�v���C���Ԃ́H�x�Ȃ�ʑΏ�
  if( questionID == QUESTION_ID_PLAY_TIME ) {
    return GetMyAnswerID_PlayTime( work );
  }

  // ���ݕ\�����̎���ɑ΂���, �����̉񓚃C���f�b�N�X���擾
  answerIdx = QuestionnaireAnswer_ReadBit( myAnswer, questionID );

  // ���񓚂��l������
  if( answerIdx == 0 ) {
    return ANSWER_ID_000;
  }
  else {
    answerIdx--;
  }

  // ��ID���擾
  researchData = &( work->researchData );
  questionIdx  = work->questionIdx;
  answerID     = RESEARCH_DATA_GetAnswerID_byIndex( researchData, questionIdx, answerIdx );

  return answerID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����w�v���C���Ԃ́H�x�ɑ΂��鎩���̉�ID���擾����
 *
 * @param work
 *
 * @return ����w�v���C���Ԃ́H�x�ɑ΂��鎩���̉�ID ( ANSWER_ID_xxxx )
 */
//-----------------------------------------------------------------------------------------
static u8 GetMyAnswerID_PlayTime( const RRG_WORK* work )
{
  GAMEDATA* gameData;
  PLAYTIME* time;
  u16 hour;

  gameData = GetGameData( work );
  time     = GAMEDATA_GetPlayTimeWork( gameData );
  hour     = PLAYTIME_GetHour( time );

  if( hour <=10 ) { return ANSWER_ID_135; }
  else if( hour <=  20 ) { return ANSWER_ID_136; }
  else if( hour <=  30 ) { return ANSWER_ID_137; }
  else if( hour <=  40 ) { return ANSWER_ID_138; }
  else if( hour <=  50 ) { return ANSWER_ID_139; }
  else if( hour <=  60 ) { return ANSWER_ID_140; }
  else if( hour <=  70 ) { return ANSWER_ID_141; }
  else if( hour <=  80 ) { return ANSWER_ID_142; }
  else if( hour <=  90 ) { return ANSWER_ID_143; }
  else if( hour <= 100 ) { return ANSWER_ID_144; }
  else { return ANSWER_ID_145; }
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
static u32 GetObjResourceRegisterIndex( const RRG_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RRG_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RRG_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}


//=========================================================================================
// ��LAYER 2 �����E�������E�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �O���t��ʊǗ����[�N ����
 *
 * @param work
 *
 * @return �O���t��ʊǗ����[�N
 */
//-----------------------------------------------------------------------------------------
static RRG_WORK* CreateGraphWork( HEAPID heapID )
{
  RRG_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(RRG_WORK) );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �O���t��ʊǗ����[�N ������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitGraphWork( RRG_WORK* work )
{
  int i;

  work->state              = RRG_STATE_SETUP;
  work->stateEndFlag       = FALSE;
  work->stateCount         = 0;
  work->waitFrame          = WAIT_FRAME_BUTTON;
  work->cursorPos          = MENU_ITEM_QUESTION;
  work->analyzeFlag        = FALSE;
  work->analyzeByTouchFlag = FALSE;
  work->updateFlag         = FALSE;
  work->questionIdx        = 0;
  work->answerIdx          = 0;
  work->graphMode          = GRAPH_DISP_MODE_TODAY;
  work->VBlankTCBSystem    = GFUser_VIntr_GetTCBSYS();
  work->percentageNum      = 0;
  work->percentageDispNum  = 0;
  work->finishResult       = RRG_RESULT_TO_TOP;
  work->finishFlag         = FALSE;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ ){ work->objResRegisterIdx[i] = 0; }

  InitResearchData( work );
  InitStateQueue( work );
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �O���t��ʊǗ����[�N �j��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteGraphWork( RRG_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitStateQueue( RRG_WORK* work )
{
  work->stateQueue = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateStateQueue( RRG_WORK* work )
{
  GF_ASSERT( work->stateQueue == NULL );

  // �쐬
  work->stateQueue = QUEUE_Create( STATE_QUEUE_SIZE, work->heapID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteStateQueue( RRG_WORK* work )
{
  GF_ASSERT( work->stateQueue );

  // �폜
  QUEUE_Delete( work->stateQueue );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitFont( RRG_WORK* work )
{
  // ������
  work->font = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateFont( RRG_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, FALSE, work->heapID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteFont( RRG_WORK* work )
{
  GF_ASSERT( work->font );

  // �폜
  GFL_FONT_Delete( work->font );
}


//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitMessages( RRG_WORK* work )
{
  int msgIdx;

  // ������
  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    work->message[ msgIdx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateMessages( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteMessages( RRG_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] );

    // �폜
    GFL_MSG_Delete( work->message[ msgIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitWordset( RRG_WORK* work )
{
  // ������
  work->wordset = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateWordset( RRG_WORK* work )
{
  // ���d����
  GF_ASSERT( work->wordset == NULL );

  // �쐬
  work->wordset = WORDSET_Create( work->heapID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���[�h�Z�b�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteWordset( RRG_WORK* work )
{
  // �폜
  WORDSET_Delete( work->wordset );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitCircleGraphs( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < GRAPH_DISP_MODE_NUM; idx++ )
  {
    work->mainGraph[ idx ] = NULL;
    work->subGraph[ idx ]  = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t�𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateCircleGraph( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < GRAPH_DISP_MODE_NUM; idx++ )
  {
    // ���d����
    GF_ASSERT( work->mainGraph[ idx ] == NULL );
    GF_ASSERT( work->subGraph[ idx ] == NULL );

    work->mainGraph[ idx ] = CIRCLE_GRAPH_Create( work->heapID );
    work->subGraph[ idx ]  = CIRCLE_GRAPH_Create( work->heapID );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteCircleGraph( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < GRAPH_DISP_MODE_NUM; idx++ )
  {
    // ������
    GF_ASSERT( work->mainGraph[ idx ] != NULL );
    GF_ASSERT( work->subGraph[ idx ] != NULL );

    CIRCLE_GRAPH_Delete( work->mainGraph[ idx ] );
    CIRCLE_GRAPH_Delete( work->subGraph[ idx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitResearchData( RRG_WORK* work )
{
  // 0�N���A
  GFL_STD_MemClear( &( work->researchData ), sizeof(RESEARCH_DATA) );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^���擾����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void SetupResearchData( RRG_WORK* work )
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
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;

  // �Z�[�u�f�[�^�擾
  gameData = GetGameData( work );
  save = GAMEDATA_GetSaveControlWork( gameData );
  QSave = SaveData_GetQuestionnaire( save );
  topicID = GetInvestigatingTopicID( work ); // ��������ID
  questionID[0] = QuestionnaireWork_GetInvestigatingQuestion( QSave, 0 ); // ����ID
  questionID[1] = QuestionnaireWork_GetInvestigatingQuestion( QSave, 1 ); // ����ID
  questionID[2] = QuestionnaireWork_GetInvestigatingQuestion( QSave, 2 ); // ����ID
  answerNumOfQuestion[0] = AnswerNum_question[ questionID[0] ]; // ����ID ==> �񓚑I�����̐�
  answerNumOfQuestion[1] = AnswerNum_question[ questionID[1] ]; // ����ID ==> �񓚑I�����̐�
  answerNumOfQuestion[2] = AnswerNum_question[ questionID[2] ]; // ����ID ==> �񓚑I�����̐�
  todayCountOfQuestion[0] = QuestionnaireWork_GetTodayCount( QSave, questionID[0] ); // ����ɑ΂���, �����̉񓚐l��
  todayCountOfQuestion[1] = QuestionnaireWork_GetTodayCount( QSave, questionID[1] ); // ����ɑ΂���, �����̉񓚐l��
  todayCountOfQuestion[2] = QuestionnaireWork_GetTodayCount( QSave, questionID[2] ); // ����ɑ΂���, �����̉񓚐l��
  totalCountOfQuestion[0] = QuestionnaireWork_GetTotalCount( QSave, questionID[0] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  totalCountOfQuestion[1] = QuestionnaireWork_GetTotalCount( QSave, questionID[1] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  totalCountOfQuestion[2] = QuestionnaireWork_GetTotalCount( QSave, questionID[2] ); // ����ɑ΂���, ���܂܂ł̉񓚐l��
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    u8 qID = questionID[ qIdx ];

    for( aIdx=0; aIdx < answerNumOfQuestion[ qIdx ]; aIdx++ )
    {
      answerID[ qIdx ][ aIdx ] = AnswerID_question[ qID ][ aIdx ]; // ��ID
      todayCountOfAnswer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTodayAnswerNum( QSave, qID, aIdx + 1 ); // �񓚂ɑ΂���, �����̉񓚐l��
      totalCountOfAnswer[ qIdx ][ aIdx ] = QuestionnaireWork_GetTotalAnswerNum( QSave, qID, aIdx + 1 ); // �񓚂ɑ΂���, ���܂܂ł̉񓚐l�� ( �����̐l�������� )
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
      int aID = answerID[ qIdx ][ aIdx ];
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].ID = aID; // ��ID
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorR = ColorR_answer[ aID ]; // �\���J���[(R)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorG = ColorG_answer[ aID ]; // �\���J���[(G)
      work->researchData.questionData[ qIdx ].answerData[ aIdx ].colorB = ColorB_answer[ aID ]; // �\���J���[(B)
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
static void SetupTouchArea( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < TOUCH_AREA_NUM; idx++ )
  {
    work->touchHitTable[ idx ].rect.left   = TouchAreaInitData[ idx ].left;
    work->touchHitTable[ idx ].rect.right  = TouchAreaInitData[ idx ].right;
    work->touchHitTable[ idx ].rect.top    = TouchAreaInitData[ idx ].top;
    work->touchHitTable[ idx ].rect.bottom = TouchAreaInitData[ idx ].bottom;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitArrow( RRG_WORK* work )
{
  work->arrow = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateArrow( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteArrow( RRG_WORK* work )
{
  GF_ASSERT( work->arrow );

  // �j��
  ARROW_Delete( work->arrow );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitPercentage( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    work->percentage[ idx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g�𐶐�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreatePercentage( RRG_WORK* work )
{
  int idx; 
  PERCENTAGE_DISP_PARAM param;

  // �\���p�����[�^��ݒ�
  param.cgrIndex      = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CHARACTER );
  param.plttIndex     = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_PALETTE );
  param.cellAnimIndex = GetObjResourceRegisterIndex( work, OBJ_RESOURCE_MAIN_CELL_ANIME );
  param.setSerface    = CLSYS_DEFREND_MAIN;
  param.anmseqNumber  = NANR_obj_nom;
  param.anmseqBase    = NANR_obj_percent;

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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief % �\���I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeletePercentage( RRG_WORK* work )
{
  int idx;

  // �S�񓚗p�̃I�u�W�F�N�g��j��
  for( idx=0; idx < PERCENTAGE_NUM; idx++ )
  {
    GF_ASSERT( work->percentage[ idx ] );

    // �j��
    PERCENTAGE_Delete( work->percentage[ idx ] );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief 3D �����ݒ�
 */
//-------------------------------------------------------------------------------
static void Setup3D( void )
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
 * @brief BG �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBG( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBG( RRG_WORK* work )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_3D );
  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( SUB_BG_FONT );
  GFL_BG_FreeBGControl( SUB_BG_WINDOW );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSubBG_WINDOW( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RRG_WORK* work )
{
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RRG_WORK* work )
{
  // NULL�L�����m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RRG_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_WINDOW( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RRG_WORK* work )
{
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RRG_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RRG_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitBGFonts( RRG_WORK* work )
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
static void CreateBGFonts( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteBGFonts( RRG_WORK* work )
{
  int i;
  
  // �ʏ��BGFont
  for( i=0; i < BG_FONT_NUM; i++ )
  {
    GF_ASSERT( work->BGFont[i] );
    BG_FONT_Delete( work->BGFont[i] );
    work->BGFont[i] = NULL;
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterSubObjResources( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseSubObjResources( RRG_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterMainObjResources( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RRG_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_COMMON_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitClactUnits( RRG_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
static void CreateClactUnits( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
static void DeleteClactUnits( RRG_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitClactWorks( RRG_WORK* work )
{
  int wkIdx;

  // ������
  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    work->clactWork[ wkIdx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateClactWorks( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteClactWorks( RRG_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    // ��������Ă��Ȃ�
    GF_ASSERT( work->clactWork[ wkIdx ] );

    // �j��
    GFL_CLACT_WK_Remove( work->clactWork[ wkIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitBitmapDatas( RRG_WORK* work )
{
  int idx;

  // ������
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    work->BmpData[ idx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CreateBitmapDatas( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �f�t�H���g�����������ɂ��, �r�b�g�}�b�v�f�[�^�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forDefault( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�񍐂�����x�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forANALYZE_BUTTON( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void DeleteBitmapDatas( RRG_WORK* work )
{
  int idx;

  // �j��
  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    GFL_BMP_Delete( work->BmpData[ idx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̏������s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void SetupBmpOamSystem( RRG_WORK* work )
{
  // BMP-OAM �V�X�e�����쐬
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RRG_WORK* work )
{
  // BMP-OAM �V�X�e����j��
  BmpOam_Exit( work->BmpOamSystem );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CreateBmpOamActors( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void DeleteBmpOamActors( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < BMPOAM_ACTOR_NUM; idx++ )
  {
    BmpOam_ActorDel( work->BmpOamActor[ idx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RRG_WORK* work )
{ 
  // ������
  work->paletteFadeSystem = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RRG_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���̌�Еt�����s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpPaletteFadeSystem( RRG_WORK* work )
{ 
  // �p���b�g�t�F�[�h�̃��N�G�X�g���[�N��j��
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_OBJ );

  // �t�F�[�h�Ǘ��V�X�e����j��
  PaletteFadeFree( work->paletteFadeSystem );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreatePaletteAnime( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL ); // ���d����

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeletePaletteAnime( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Delete( work->paletteAnime[ idx ] );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupPaletteAnime( RRG_WORK* work )
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
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RRG_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �ʐM�A�C�R�����Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupWirelessIcon( const RRG_WORK* work )
{
  GFL_NET_ChangeIconPosition( WIRELESS_ICON_X, WIRELESS_ICON_Y );
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, work->heapID );
  GFL_NET_ReloadIcon();
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterVBlankTask( RRG_WORK* work )
{
  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RRG_WORK* work )
{ 
  GFL_TCB_DeleteTask( work->VBlankTask );
}


//=========================================================================================
// ��LAYER 1 ���[�e�B���e�B
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief u8 �Ɏ��܂�悤�Ɋۂ߂�
 *
 * @param num �ۂ߂鐔�l
 *
 * @return num �� [0, 255] �Ɋۂ߂��l
 */
//-----------------------------------------------------------------------------------------
static u8 Bind_u8( int num )
{
  if( num < 0 ) { return 0; }
  if( 0xff < num ) { return 0xff; } 
  return num;
}
