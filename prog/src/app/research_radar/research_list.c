///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���X�g���
 * @file   research_list.c
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <gflib.h>
#include "bg_font.h"
#include "queue.h"
#include "palette_anime.h"
#include "research_list.h"
#include "research_list_index.h"
#include "research_list_def.h"
#include "research_list_data.cdat"
#include "research_common.h"
#include "research_common_data.cdat"
#include "research_data.h"

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
// �����X�g��ʊǗ����[�N
//=========================================================================================
struct _RESEARCH_RADAR_LIST_WORK
{ 
  // �S��ʋ��ʃ��[�N
  RRC_WORK* commonWork; 

  HEAPID       heapID; 
  GFL_FONT*    font;
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  QUEUE*    stateQueue; // ��ԃL���[
  RRL_STATE state;      // ���݂̏��
  u32       stateSeq;   // ��ԓ��V�[�P���X�ԍ�
  u32       stateCount; // ��ԃJ�E���^
  u32       waitFrame;  // �t���[���o�ߑ҂���Ԃ̑҂�����

  // ���j���[����
  MENU_ITEM menuCursorPos; // �J�[�\���ʒu

  // ��������
  u8 selectableTopicNum; // �I���\�Ȓ������ڂ̐�
  u8 selectedTopicID;    // �I��������������ID 
  u8 topicCursorPos;     // �J�[�\���ʒu ( == ��������ID )
  u8 topicCursorNextPos; // �ړ���̃J�[�\���ʒu 

  // �X�N���[��
  int scrollCursorPos;      // �X�N���[���J�[�\���ʒu
  int scrollCursorPos_prev; // �O��̃X�N���[���J�[�\���ʒu
  int scrollStartPos;       // �X�N���[���J�n���̃J�[�\���ʒu
  int scrollEndPos;         // �X�N���[���I�����̃J�[�\���ʒu
  int scrollFrames;         // �X�N���[���t���[����
  int scrollFrameCount;     // �X�N���[���t���[�����J�E���^
  int sliderSpeed;          // �X���C�_�[�̑��x
  int sliderAccel;          // �X���C�_�[�̉����x

  // �^�b�`�̈�
  GFL_UI_TP_HITTBL menuTouchHitTable[ MENU_TOUCH_AREA_NUM ];
  GFL_UI_TP_HITTBL topicTouchHitTable[ TOPIC_TOUCH_AREA_NUM ];
  GFL_UI_TP_HITTBL scrollTouchHitTable[ SCROLL_TOUCH_AREA_NUM ];

  // �p���b�g�t�F�[�h�����V�X�e��
  PALETTE_FADE_PTR paletteFadeSystem; 

  // �p���b�g�A�j���[�V����
  PALETTE_ANIME* paletteAnime[ PALETTE_ANIME_NUM ];

  // ������`��I�u�W�F�N�g
  BG_FONT* BGFont_topic[ TOPIC_ID_NUM ]; // ��������
  BG_FONT* BGFont_string[ BG_FONT_NUM ]; // ������

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ]; // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];               // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                 // �Z���A�N�^�[���[�N

  // BMP-OAM
  BMPOAM_SYS_PTR BmpOamSystem;                    // BMP-OAM �V�X�e��
  BMPOAM_ACT_PTR BmpOamActor[ BMPOAM_ACTOR_NUM ]; // BMP-OAM�A�N�^�[
  GFL_BMP_DATA*  BmpData[ BMPOAM_ACTOR_NUM ];     // �e�A�N�^�[�ɑΉ�����r�b�g�}�b�v�f�[�^

  // VBlank
  GFL_TCBSYS* VBlankTCBSystem; // VBlank���Ԓ��̃^�X�N�Ǘ��V�X�e��
  GFL_TCB*    VBlankTask;      // VBlank�^�C�~���O���ɍs���^�X�N

  // �t���O
  BOOL stateEndFlag;   // ���݂̏�Ԃ��I���������ǂ���
  BOOL palFadeOutFlag; // �t�F�[�h�A�E�g�����ǂ���
  BOOL finishFlag;     // ���X�g��ʂ��I���������ǂ���
  RRL_RESULT finishResult; // ���X�g��ʂ̏I������
};



//=========================================================================================
// ���֐��C���f�b�N�X
//=========================================================================================

//-----------------------------------------------------------------------------------------
// ��LAYER 5 ���
//-----------------------------------------------------------------------------------------
// ��Ԃ��Ƃ̏���
static void MainState_SETUP( RRL_WORK* work ); // RRL_STATE_SETUP
static void MainState_STANDBY( RRL_WORK* work ); // RRL_STATE_STANDBY
static void MainState_KEY_WAIT( RRL_WORK* work ); // RRL_STATE_KEY_WAIT
static void MainState_AUTO_SCROLL( RRL_WORK* work ); // RRL_STATE_AUTO_SCROLL
static void MainState_SLIDE_CONTROL( RRL_WORK* work ); // RRL_STATE_SLIDE_CONTROL
static void MainState_RETURN_FROM_KEYWAIT( RRL_WORK* work ); // RRL_STATE_RETURN_FROM_KEYWAIT
static void MainState_RETURN_FROM_STANDBY( RRL_WORK* work ); // RRL_STATE_RETURN_FROM_STANDBY
static void MainState_TO_CONFIRM_STANDBY( RRL_WORK* work ); // RRL_STATE_TO_CONFIRM_STANDBY
static void MainState_CONFIRM_STANDBY( RRL_WORK* work ); // RRL_STATE_CONFIRM_STANDBY
static void MainState_TO_CONFIRM_KEY_WAIT( RRL_WORK* work ); // RRL_STATE_TO_CONFIRM_KEY_WAIT
static void MainState_CONFIRM_KEY_WAIT( RRL_WORK* work ); // RRL_STATE_CONFIRM_KEY_WAIT
static void MainState_CONFIRM_CANCEL( RRL_WORK* work ); // RRL_STATE_CONFIRM_CANCEL
static void MainState_CONFIRM_DETERMINE( RRL_WORK* work ); // RRL_STATE_CONFIRM_DETERMINE
static void MainState_CLEAN_UP( RRL_WORK* work ); // RRL_STATE_CLEAN_UP 
// ��Ԑ���
static void IncStateCount( RRL_WORK* work ); // ��ԃJ�E���^���C���N�������g����
static void RegisterNextState( RRL_WORK* work, RRL_STATE nextSeq ); // ���̏�Ԃ��L���[�ɓo�^����
static void FinishCurrentState( RRL_WORK* work ); // ���݂̏�Ԃ��I������
static void SwitchState( RRL_WORK* work ); // ������Ԃ�ύX����
static RRL_STATE GetState( const RRL_WORK* work ); // ��Ԃ��擾����
static void SetState( RRL_WORK* work, RRL_STATE nextSeq ); // ������Ԃ�ݒ肷��
static u32 GetStateSeq( const RRL_WORK* work ); // ��ԓ��V�[�P���X�ԍ����擾����
static void IncStateSeq( RRL_WORK* work ); // ��ԓ��V�[�P���X�ԍ����C���N�������g����
static void ResetStateSeq( RRL_WORK* work ); // ��ԓ��V�[�P���X�ԍ������Z�b�g����
static SEQ_CHANGE_TRIG GetFinishReason( RRL_WORK* work ); // ��ʑJ�ڂ̕��@���擾����
static void SetFinishReason( RRL_WORK* work, SEQ_CHANGE_TRIG reason ); // ���X�g��ʏI���̕��@��o�^����
static void SetFinishResult( RRL_WORK* work, RRL_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
static void SetWaitFrame( RRL_WORK* work, u32 frame ); // �t���[���o�ߑ҂���Ԃ̑҂����Ԃ�ݒ肷��
static u32 GetWaitFrame( const RRL_WORK* work ); // �t���[���o�ߑ҂���Ԃ̑҂����Ԃ��擾����
//-----------------------------------------------------------------------------------------
// ��LAYER 4 �@�\
//-----------------------------------------------------------------------------------------
// ���j���[���ڃJ�[�\��
static void MoveMenuCursorUp( RRL_WORK* work ); // ��ֈړ�����
static void MoveMenuCursorDown( RRL_WORK* work ); // ���ֈړ�����
static void MoveMenuCursorDirect( RRL_WORK* work, MENU_ITEM menuItem ); // ���ڈړ�����
// �������ڃJ�[�\��
static void AdjustTopicCursor( RRL_WORK* work ); // �J�[�\������ʓ��ɑ��݂���悤�ɒ�������
static void MoveTopicCursorDirect( RRL_WORK* work, u8 topicID ); // ���ڈړ�����
//�w�߂�x�{�^��
static void BlinkReturnButton( RRL_WORK* work ); //�w�߂�x�{�^���𖾖ł�����
// �I�[�g�X�N���[��
static void StartAutoScroll_to_Topic( RRL_WORK* work ); // �I�[�g�X�N���[�����J�n���� ( �������ڃJ�[�\���ʒu�� )
static void StartAutoScroll_for_Reset( RRL_WORK* work ); // �I�[�g�X�N���[�����J�n���� ( �͈͊O�X�N���[�������Z�b�g )
static void StartAutoScroll_to_Confirm( RRL_WORK* work ); // �I�[�g�X�N���[�����J�n���� ( �������ڊm��̊m�F�� )
//-----------------------------------------------------------------------------------------
// ��LAYER 3 �ʑ���
//-----------------------------------------------------------------------------------------
// �Z�[�u�f�[�^
static u8 GetInvestigatingTopicID( const RRL_WORK* work ); // ���ݒ������̒�������ID���擾����
static void UpdateInvestigatingTopicID( const RRL_WORK* work ); // �������鍀�ڂ��X�V����
// ���j���[����
static void ShiftMenuCursorPos( RRL_WORK* work, int stride ); // ���j���[���ڃJ�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetMenuCursorPos( RRL_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڃJ�[�\���ʒu��ύX���� ( ���l�w�� ) 
static void SetMenuItemCursorOn( RRL_WORK* work, MENU_ITEM menuItem ); // �J�[�\��������Ă����Ԃɂ���
static void SetMenuItemCursorOff( RRL_WORK* work, MENU_ITEM menuItem ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
// ��������
static void SetTopicCursorPosDirect( RRL_WORK* work, int topciID ); // �������ڃJ�[�\���̈ʒu��ݒ肷�� ( ���l�w�� ) 
static void SetTopicCursorNextPos( RRL_WORK* work, int stride ); // �������ڃJ�[�\���̈ړ����ݒ肷�� ( �I�t�Z�b�g�w�� )
static void SetTopicButtonCursorOn( const RRL_WORK* work ); // �J�[�\��������Ă����Ԃɂ���
static void SetTopicButtonCursorOff( const RRL_WORK* work ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetTopicButtonInvestigating( const RRL_WORK* work, u8 topicID ); // �������̏�Ԃɂ���
static void SetTopicButtonNotInvestigating( const RRL_WORK* work, u8 topicID ); // �������łȂ���Ԃɖ߂�
static u8 CalcScreenLeftOfTopicButton( const RRL_WORK* work, u8 topicID ); // �������ڂ̍���x���W���Z�o���� ( �X�N���[���P�� )
static u8 CalcScreenTopOfTopicButton( const RRL_WORK* work, u8 topicID ); // �������ڂ̍���y���W���Z�o���� ( �X�N���[���P�� )
static int CalcDisplayLeftOfTopicButton( const RRL_WORK* work, u8 topicID ); // �������ڃ{�^���̍���x���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� )
static int CalcDisplayTopOfTopicButton ( const RRL_WORK* work, u8 topicID ); // �������ڃ{�^���̍���y���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� ) 
static int CalcDisplayBottomOfTopicButton ( const RRL_WORK* work, u8 topicID ); // �������ڃ{�^���̉E��y���W���Z�o���� ( �f�B�X�v���C���W�n�E�h�b�g�P�� ) 
static void UpdateTopicButtonMask( const RRL_WORK* work ); // �������ڃ{�^���̃X�N���[����荞�݂��B�����߂̃E�B���h�E���X�V����
static void UpdateInvestigatingIcon( const RRL_WORK* work ); // �������A�C�R���̕\����Ԃ��X�V����
// ���ʂ̕\��
static void UpdateTopicDetailStrings_at_Now( RRL_WORK* work ); // ���ʂ̒������ڐ��������X�V���� ( ���݂̃J�[�\���ʒu�ɍ��킹�� )
static void UpdateTopicDetailStrings_at_Next( RRL_WORK* work ); // ���ʂ̒������ڐ��������X�V���� ( ���̃J�[�\���ʒu�ɍ��킹�� )
static void UpdateTopicDetailStrings_at( RRL_WORK* work, u8 topicID ); // ���ʂ̒������ڐ��������X�V���� ( �w��J�[�\���ʒu�ɍ��킹�� )
static void HideTopicDetailStrints( RRL_WORK* work ); // ���ʂ̒������ڐ������̕\�����B��
static void ShowTopicDetailStrings( RRL_WORK* work ); // ���ʂ̒������ڐ������̕\�����J�n����
// �X���C�_�[
static void UpdateSliderDisp( const RRL_WORK* work ); // ���݂̃X�N���[�������l�ɍ��킹��, �X���C�_�[�̕\���ʒu���X�V����
static int CalcSliderPos_byScrollValue( const RRL_WORK* work ); // �X�N���[�������l����, �X���C�_�[�̕\���ʒu���v�Z����
static int CalcScrollCursorPos_bySliderPos( const RRL_WORK* work, int controlPos ); // �X���C�_�[�̕\���ʒu����, �X�N���[���J�[�\���ʒu���Z�o����
static int GetSliderPos( const RRL_WORK* work ); // �X���C�_�[�̕\���ʒu���擾����
static void AccelerateSliderSpeed( RRL_WORK* work ); // �X���C�_�[�̈ړ����x���X�V����
static BOOL IsSliderMoving( const RRL_WORK* work ); // �X���C�_�[�������Ă��邩�ǂ������`�F�b�N����
static void ObserveSliderSpeed( RRL_WORK* work ); // �X���C�_�[�̈ړ����x���ϑ�����
static void AdjustSliderSpeed_forFreeMove( RRL_WORK* work ); // �X���C�_�[�̈ړ����x�𒲐����� ( ���R�ړ��p )
static void AdjustSliderAccel_forFreeMove( RRL_WORK* work ); // �X���C�_�[�̉����x�𒲐����� ( ���R�ړ��p )
// �X�N���[��
static void StartScroll( RRL_WORK* work, int startPos, int endPos, int frames ); // �X�N���[�����J�n����
static void UpdateScroll( RRL_WORK* work ); // �X�N���[�����X�V����
static BOOL CheckScrollEnd( RRL_WORK* work ); // �X�N���[�����I���������ǂ����𔻒肷��
static void ShiftScrollCursorPos( RRL_WORK* work, int offset ); // �X�N���[���J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetScrollCursorPos( RRL_WORK* work, int pos ); // �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w�� )
static void SetScrollCursorPosForce( RRL_WORK* work, int pos ); // �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w��E�͈͌���Ȃ� )
static void AdjustScrollCursor( RRL_WORK* work ); // �X�N���[�������l�ɍ��킹, �X�N���[���J�[�\���ʒu��␳����
static void UpdateScrollValue( const RRL_WORK* work ); // �X�N���[�������l���X�V����
static void AdjustScrollValue( const RRL_WORK* work ); // �X�N���[�������l���X�N���[���J�[�\���ʒu�ɍ��킹�čX�V����
static int GetScrollValue(); // �X�N���[�������l���擾����
static int GetMinScrollCursorMarginPos(); // �X�N���[���J�[�\���]���͈͂̍ŏ��l���擾����
static int GetMaxScrollCursorMarginPos(); // �X�N���[���J�[�\���]���͈͂̍ő�l���擾����
static int CalcTopicID_byScrollCursorPos( int pos ); // �w�肵���X�N���[���J�[�\���ʒu�ɂ���{�^���̒�������ID���v�Z����
static int CalcScrollCursorPosOfTopicButtonTop( int topicID ); // �w�肵���������ڃ{�^���̏�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
static int CalcScrollCursorPosOfTopicButtonBottom( int topicID ); // �w�肵���������ڃ{�^���̒�ӂɊY������X�N���[���J�[�\���ʒu���v�Z����
static int GetMaxScrollValue( const RRL_WORK* work ); // �ő�X�N���[���l���擾����
static int GetMaxScrollCursorPos( const RRL_WORK* work ); // �X�N���[���J�[�\���̍ő�l���擾����
static BOOL CheckScrollControlCan( const RRL_WORK* work ); // �X�N���[�����삪�\���ǂ����𔻒肷��
// �^�b�`�͈�
static void UpdateTopicTouchArea( RRL_WORK* work ); // �^�b�`�͈͂��X�V����
// ��ʂ̃t�F�[�h�A�E�g�E�t�F�[�h�C��
static void StartFadeIn( void ); // �t�F�[�h�C�����J�n����
static void StartFadeOut( void ); // �t�F�[�h�A�E�g���J�n����
static BOOL CheckFadeEnd( void ); // �t�F�[�h�̏I�����`�F�b�N����
// �p���b�g�t�F�[�h
static void StartPaletteFadeOut( RRL_WORK* work ); // �p���b�g�̃t�F�[�h�A�E�g���J�n����
static void StartPaletteFadeIn ( RRL_WORK* work ); // �p���b�g�̃t�F�[�h�C�����J�n����
static BOOL IsPaletteFadeEnd( RRL_WORK* work ); // �p���b�g�̃t�F�[�h�������������ǂ����𔻒肷��
// �p���b�g�A�j���[�V����
static void StartCommonPaletteAnime( RRL_WORK* work, COMMON_PALETTE_ANIME_INDEX index ); // ���ʃp���b�g�A�j���[�V�������J�n����
static void StartPaletteAnime( RRL_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RRL_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static BOOL CheckPaletteAnimeEnd( const RRL_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�����̏I�����`�F�b�N����
static void UpdatePaletteAnime( RRL_WORK* work );  // �p���b�g�A�j���[�V�������X�V����
static void UpdateCommonPaletteAnime( const RRL_WORK* work ); // ���ʃp���b�g�A�j���[�V�������X�V����
// VBlank
static void VBlankFunc( GFL_TCB* tcb, void* wk ); // VBlank ���̏���
//-----------------------------------------------------------------------------------------
// ��LAYER 2 �擾�E�ݒ�E����
//-----------------------------------------------------------------------------------------
// �V�X�e��
static GAMESYS_WORK* GetGameSystem( const RRL_WORK* work );
static GAMEDATA* GetGameData( const RRL_WORK* work );
static void SetHeapID( RRL_WORK* work, HEAPID heapID );
static void SetCommonWork( RRL_WORK* work, RRC_WORK* commonWork );
// ��������
static int GetNextTopicID( const RRL_WORK* work, int topicID ); // ���̒�������ID���擾����
static int GetPrevTopicID( const RRL_WORK* work, int topicID ); // �O�̒�������ID���擾����
static u8 GetSelectableTopicNum( const RRL_WORK* work ); // �I���\�Ȓ������ڂ̐����擾����
static u8 GetSelectedTopicID( const RRL_WORK* work ); // �I�𒆂̒�������ID���擾����
static void SetSelectedTopicID( RRL_WORK* work, u8 topicID ); // ��������ID��I������
static void ResetSelectedTopicID( RRL_WORK* work ); // ��������ID�̑I������������
static BOOL IsTopicIDSelected( const RRL_WORK* work ); // �I���ς݂��𔻒肷��
static BOOL CheckTopicCanSelect( const RRL_WORK* work, u8 topicID ); // �������ڂ�I���\���ǂ������`�F�b�N����
// OBJ
static u32 GetObjResourceRegisterIndex( const RRL_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
static GFL_CLUNIT* GetClactUnit( const RRL_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RRL_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
// BMP-OAM
static void BmpOamSetDrawEnable( RRL_WORK* work, BMPOAM_ACTOR_INDEX BmpOamActorIdx, BOOL enable ); // �\�����邩�ǂ�����ݒ肷��
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RRL_WORK* work, MENU_ITEM menuItem ); // ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[���擾����
//-----------------------------------------------------------------------------------------
// ��LAYER 1 �����E�������E�����E��Еt���E�j��
//-----------------------------------------------------------------------------------------
static RRL_WORK* CreateListWork( HEAPID heapID ); // ���X�g��ʊǗ����[�N ����
static void InitListWork( RRL_WORK* work ); // ���X�g��ʊǗ����[�N ������
static void DeleteListWork( RRL_WORK* work ); // ���X�g��ʊǗ����[�N �j��
static void SetupSelectableTopicNum( RRL_WORK* work ); // �I���\�Ȓ������ڂ̐� ����
static void CreateFont( RRL_WORK* work ); // �t�H���g�f�[�^ ����
static void InitFont( RRL_WORK* work ); // �t�H���g�f�[�^ ������
static void DeleteFont( RRL_WORK* work ); // �t�H���g�f�[�^ �j��
static void CreateMessages( RRL_WORK* work ); // ���b�Z�[�W�f�[�^ ����
static void InitMessages( RRL_WORK* work ); // ���b�Z�[�W�f�[�^ ������
static void DeleteMessages( RRL_WORK* work ); // ���b�Z�[�W�f�[�^ �j��
static void CreateStateQueue( RRL_WORK* work ); // ��ԃL���[ ����
static void InitStateQueue( RRL_WORK* work ); // ��ԃL���[ ������
static void DeleteStateQueue( RRL_WORK* work ); // ��ԃL���[ �j��
static void SetupTouchArea( RRL_WORK* work ); // �^�b�`�̈� ����
static void SetupBG( RRL_WORK* work ); // BG �S�� ����
static void SetupSubBG_WINDOW( RRL_WORK* work ); // SUB-BG ( �E�B���h�E�� )  ����
static void SetupSubBG_FONT( RRL_WORK* work ); // SUB-BG ( �t�H���g�� )  ����
static void SetupMainBG_BAR( RRL_WORK* work ); // MAIN-BG ( �o�[�� )  ����
static void SetupMainBG_WINDOW( RRL_WORK* work ); // MAIN-BG ( �E�B���h�E�� )  ����
static void SetupMainBG_FONT( RRL_WORK* work ); // MAIN-BG ( �t�H���g�� )  ����
static void CleanUpBG( RRL_WORK* work ); // BG �S�� ��Еt��
static void CleanUpSubBG_WINDOW( RRL_WORK* work ); // SUB-BG ( �E�B���h�E�� )  ��Еt��
static void CleanUpSubBG_FONT( RRL_WORK* work ); // SUB-BG ( �t�H���g�� )  ��Еt��
static void CleanUpMainBG_BAR( RRL_WORK* work ); // MAIN-BG ( �o�[�� )  ��Еt��
static void CleanUpMainBG_WINDOW( RRL_WORK* work ); // MAIN-BG ( �E�B���h�E�� )  ��Еt��
static void CleanUpMainBG_FONT( RRL_WORK* work ); // MAIN-BG ( �t�H���g�� )  ��Еt��
static void CreateBGFonts( RRL_WORK* work ); // ������`��I�u�W�F�N�g ����
static void InitBGFonts( RRL_WORK* work ); // ������`��I�u�W�F�N�g ������
static void DeleteBGFonts( RRL_WORK* work ); // ������`��I�u�W�F�N�g �j��
static void InitOBJResources( RRL_WORK* work ); // OBJ ���\�[�X ������
static void RegisterSubObjResources( RRL_WORK* work ); // SUB-OBJ ���\�[�X �o�^
static void ReleaseSubObjResources( RRL_WORK* work ); // SUB-OBJ ���\�[�X ���
static void RegisterMainObjResources( RRL_WORK* work ); // MAIN-OBJ ���\�[�X �o�^
static void ReleaseMainObjResources( RRL_WORK* work ); // MAIN-OBJ ���\�[�X ���
static void InitClactUnits( RRL_WORK* work ); // �Z���A�N�^�[���j�b�g ������
static void CreateClactUnits( RRL_WORK* work ); // �Z���A�N�^�[���j�b�g ����
static void DeleteClactUnits( RRL_WORK* work ); // �Z���A�N�^�[���j�b�g �j��
static void InitClactWorks( RRL_WORK* work ); // �Z���A�N�^�[���[�N ������
static void CreateClactWorks( RRL_WORK* work ); // �Z���A�N�^�[���[�N ����
static void DeleteClactWorks( RRL_WORK* work ); // �Z���A�N�^�[���[�N �j��
static void InitBitmapDatas( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ ������
static void CreateBitmapDatas( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ �쐬
static void SetupBitmapData_forDefault( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ ���� ( �f�t�H���g )
static void SetupBitmapData_forOK( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ ���� (�u�����Ă��v)
static void SetupBitmapData_forCANCEL( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ ���� (�u��߂�v)
static void DeleteBitmapDatas( RRL_WORK* work ); // �r�b�g�}�b�v�f�[�^ �j��
static void SetupBmpOamSystem( RRL_WORK* work ); // BMP-OAM �V�X�e�� ����
static void CleanUpBmpOamSystem( RRL_WORK* work ); // BMP-OAM �V�X�e�� ��Еt��
static void CreateBmpOamActors( RRL_WORK* work ); // BMP-OAM �A�N�^�[ �쐬
static void DeleteBmpOamActors( RRL_WORK* work ); // BMP-OAM �A�N�^�[ �j��
static void InitPaletteFadeSystem( RRL_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ������
static void SetupPaletteFadeSystem( RRL_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ����
static void CleanUpPaletteFadeSystem( RRL_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�� ��Еt��
static void CreatePaletteAnime( RRL_WORK* work ); // �p���b�g�A�j���[�V�������[�N ����
static void InitPaletteAnime( RRL_WORK* work ); // �p���b�g�A�j���[�V�������[�N ������
static void DeletePaletteAnime( RRL_WORK* work ); // �p���b�g�A�j���[�V�������[�N �j��
static void SetupPaletteAnime( RRL_WORK* work ); // �p���b�g�A�j���[�V�������[�N ����
static void CleanUpPaletteAnime( RRL_WORK* work );  // �p���b�g�A�j���[�V�������[�N ��Еt��
static void SetupScrollBar( RRL_WORK* work ); // �X�N���[���o�[ ����
static void SetupWirelessIcon( const RRL_WORK* work ); // �ʐM�A�C�R�� ����
static void RegisterVBlankTask( RRL_WORK* work ); // VBlank �^�X�N �o�^
static void ReleaseVBlankTask ( RRL_WORK* work ); // VBlank �^�X�N ����




//=========================================================================================
// ��public functions
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʊǗ����[�N�𐶐�����
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return ���X�g��ʊǗ����[�N
 */
//-----------------------------------------------------------------------------------------
RRL_WORK* RRL_CreateWork( RRC_WORK* commonWork )
{
  RRL_WORK* work;
  HEAPID heapID;

  heapID = RRC_GetHeapID( commonWork );

  // ���[�N�𐶐�
  work = CreateListWork( heapID );

  // ���[�N��������
  InitListWork( work );
  SetHeapID( work, heapID );
  SetCommonWork( work, commonWork );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
void RRL_DeleteWork( RRL_WORK* work )
{
  DeleteStateQueue( work );
  DeleteListWork( work );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
void RRL_Main( RRL_WORK* work )
{
  switch( GetState(work) ) {
  case RRL_STATE_SETUP:               MainState_SETUP( work );               break;
  case RRL_STATE_STANDBY:             MainState_STANDBY( work );             break;
  case RRL_STATE_KEY_WAIT:            MainState_KEY_WAIT( work );            break;
  case RRL_STATE_AUTO_SCROLL:         MainState_AUTO_SCROLL( work );         break;
  case RRL_STATE_SLIDE_CONTROL:       MainState_SLIDE_CONTROL( work );       break;
  case RRL_STATE_RETURN_FROM_KEYWAIT: MainState_RETURN_FROM_KEYWAIT( work ); break;
  case RRL_STATE_RETURN_FROM_STANDBY: MainState_RETURN_FROM_STANDBY( work ); break;
  case RRL_STATE_TO_CONFIRM_STANDBY:  MainState_TO_CONFIRM_STANDBY( work );  break;
  case RRL_STATE_CONFIRM_STANDBY:     MainState_CONFIRM_STANDBY( work );     break;
  case RRL_STATE_TO_CONFIRM_KEY_WAIT: MainState_TO_CONFIRM_KEY_WAIT( work ); break;
  case RRL_STATE_CONFIRM_KEY_WAIT:    MainState_CONFIRM_KEY_WAIT( work );    break;
  case RRL_STATE_CONFIRM_CANCEL:      MainState_CONFIRM_CANCEL( work );      break;
  case RRL_STATE_CONFIRM_DETERMINE:   MainState_CONFIRM_DETERMINE( work );   break;
  case RRL_STATE_CLEAN_UP:            MainState_CLEAN_UP( work );            break;
  case RRL_STATE_FINISH:              break;
  default:  GF_ASSERT(0);
  }

  UpdateCommonPaletteAnime( work ); // ���ʃp���b�g�A�j���[�V�������X�V
  UpdatePaletteAnime( work );       // �p���b�g�A�j���[�V�������X�V
  GFL_CLACT_SYS_Main();             // �Z���A�N�^�[�V�X�e�� ���C������
  IncStateCount( work );            // ��ԃJ�E���^���C���N�������g
  SwitchState( work );              // ��Ԃ��X�V
}


//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʂ̏I�����`�F�b�N����
 *
 * @param work
 *
 * @return ���X�g��ʂ��I�����Ă���ꍇ TRUE
 */
//-----------------------------------------------------------------------------------------
RRL_IsFinished( const RRL_WORK* work )
{
  return work->finishFlag;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʂ̏I�����ʂ��擾����
 *
 * @param work
 *
 * @return ���X�g��ʂ̏I������
 */
//-----------------------------------------------------------------------------------------
RRL_RESULT RRL_GetResult( const RRL_WORK* work )
{
  return work->finishResult;
}



//=========================================================================================
// ��LAYER 5 ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ������� ( RRL_STATE_SETUP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_SETUP( RRL_WORK* work )
{
  CreateStateQueue( work );
  CreateFont( work );
  CreateMessages( work );
  SetupTouchArea( work );
  UpdateTopicTouchArea( work );
  SetupSelectableTopicNum( work );

  // BG ����
  SetupBG( work );
  SetupSubBG_WINDOW( work );
  SetupSubBG_FONT( work );
  SetupMainBG_BAR( work );
  SetupMainBG_WINDOW( work );
  SetupMainBG_FONT( work );

  // ������`��I�u�W�F�N�g ����
  CreateBGFonts( work );
  UpdateTopicDetailStrings_at_Now( work );
  HideTopicDetailStrints( work );

  // OBJ ����
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

  RegisterVBlankTask( work ); // VBkank�^�X�N�o�^
  SetupWirelessIcon( work );  // �ʐM�A�C�R��
  SetupScrollBar( work );     // �X�N���[���o�[

  // �������̍��ڂ�����ꍇ
  if( GetInvestigatingTopicID( work ) != INVESTIGATING_QUESTION_NULL ) {
    // �������̍��ڂ�I����Ԃɂ���
    SetTopicButtonInvestigating( work, nowTopicID ); // �I�����Ă����Ԃɂ���
    SetSelectedTopicID( work, nowTopicID );          // �������̍��ڂ�I��
    UpdateInvestigatingIcon( work );                 // �������ڑI���A�C�R�����X�V
  }

  // �p���b�g�A�j���[�V�����J�n
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_ON ); 

  // �m�F���b�Z�[�W�ƑI�����ڂ�����
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, FALSE );
  BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, FALSE );

  // �I�[�g�X�N���[���J�n
  StartAutoScroll_to_Topic( work ); 
  while( CheckScrollEnd(work) == FALSE ) {
    UpdateScroll( work ); // �X�N���[�����X�V
  }
  // �X�N���[���I��
  UpdateScrollValue( work );       // �X�N���[�������l���X�V
  UpdateTopicTouchArea( work );    // �^�b�`�͈͂��X�V
  UpdateSliderDisp( work );        // �X���C�_�[
  UpdateInvestigatingIcon( work ); // �������ڑI���A�C�R�����X�V
  UpdateTopicButtonMask( work );   // �������ڂ̃}�X�N�E�B���h���X�V

  // �t�F�[�h�C���J�n
  StartFadeIn(); 

  // ���̏�ԑJ�ڂ��Z�b�g
  if( GetFinishReason( work ) == SEQ_CHANGE_BY_BUTTON ) {
    SetTopicButtonCursorOn( work );                // �J�[�\��������Ă����Ԃɂ���
    ShowTopicDetailStrings( work );                // ���ʂ̏ڍו\���J�n
    FinishCurrentState( work );                    // RRL_STATE_SETUP ��ԏI��
    RegisterNextState( work, RRL_STATE_KEY_WAIT ); // => RRL_STATE_KEY_WAIT 
  }
  else {
    SetTopicButtonCursorOff( work );               // �J�[�\��������Ă��Ȃ���Ԃɂ���
    FinishCurrentState( work );                    // RRL_STATE_SETUP ��ԏI��
    RegisterNextState( work, RRL_STATE_STANDBY );  // => RRL_STATE_STANDBY 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[���͑҂���� ( RRL_STATE_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_STANDBY( RRL_WORK* work )
{
  int trg;
  int touchTrg;
  int commonTouch;

  trg   = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->topicTouchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RRC_GetHitTable(work->commonWork) );

  //-------------------------
  //�u���ǂ�v�{�^�����^�b�`
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                           // �L�����Z����
    BlinkReturnButton( work );                                //�w�߂�x�{�^���𖾖ł�����
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );             // �^�b�`�ŏI��
    FinishCurrentState( work );                               // RRL_STATE_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_RETURN_FROM_STANDBY ); // => RRL_STATE_RETURN_FROM_STANDBY 
    return;
  }

  //-----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                           // �L�����Z����
    BlinkReturnButton( work );                                //�w�߂�x�{�^���𖾖ł�����
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON );            // �{�^���ŏI��
    FinishCurrentState( work );                               // RRL_STATE_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_RETURN_FROM_STANDBY ); // => RRL_STATE_RETURN_FROM_STANDBY 
    return;
  }

  //----------------------
  // �\���L�[ or A �{�^��
  if( (trg & PAD_KEY_UP) || (trg & PAD_KEY_DOWN) ||
      (trg & PAD_KEY_LEFT) || (trg & PAD_KEY_RIGHT) || (trg & PAD_BUTTON_A) ) {
    SetTopicButtonCursorOn( work );                 // �J�[�\��������Ă����Ԃɂ���
    ShowTopicDetailStrings( work );                 // ���ʂ̏ڍו\���J�n
    FinishCurrentState( work );                     // RRL_STATE_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_KEY_WAIT );  // => RRL_STATE_KEY_WAIT 
    return;
  }

  //----------------------------
  //�u�������ځv�{�^�����^�b�`
  if( (TOPIC_TOUCH_AREA_TOPIC_0 <= touchTrg) && (touchTrg <= TOPIC_TOUCH_AREA_TOPIC_9) ) {
    // �I���\
    if( CheckTopicCanSelect( work, touchTrg ) == TRUE ) {
      MoveTopicCursorDirect( work, touchTrg );                 // �J�[�\���ړ�
      SetSelectedTopicID( work, work->topicCursorPos );        // �J�[�\���ʒu�̒������ڂ�I��
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                          // ���艹
      StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );   // ���ڑI�����̃p���b�g�A�j���J�n
      UpdateTopicDetailStrings_at_Now( work );                 // ���ʂ̒������ڐ��������X�V
      ShowTopicDetailStrings( work );                          // ���ʂ̏ڍו\���J�n
      FinishCurrentState( work );                              // RRL_STATE_STANDBY ��ԏI��
      RegisterNextState( work, RRL_STATE_TO_CONFIRM_STANDBY ); // => RRL_STATE_TO_CONFIRM_STANDBY 
    }
    return;
  } 

  //----------------
  // �X�N���[���o�[
  if( ( GFL_UI_TP_HitCont( work->scrollTouchHitTable ) == SCROLL_TOUCH_AREA_BAR ) ||
      ( GFL_UI_TP_HitTrg( work->scrollTouchHitTable ) == SCROLL_TOUCH_AREA_BAR ) ) {
    // �X�N���[������\
    if( CheckScrollControlCan( work ) == TRUE ) {
      FinishCurrentState( work );                          // RRL_STATE_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_SLIDE_CONTROL );  // => RRL_STATE_SLIDE_CONTROL 
      RegisterNextState( work, RRL_STATE_KEY_WAIT );       // ==> RRL_STATE_KEY_WAIT 
    }
    return;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂���� ( RRL_STATE_KEY_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_KEY_WAIT( RRL_WORK* work )
{ 
  int trg;
  int touchTrg;
  int commonTouch;
  BOOL select = FALSE;

  trg   = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->topicTouchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RRC_GetHitTable(work->commonWork) );

  //-------------------------
  //�u���ǂ�v�{�^�����^�b�`
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                           // �L�����Z����
    BlinkReturnButton( work );                                //�w�߂�x�{�^���𖾖ł�����
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );             // �^�b�`�ŏI��
    FinishCurrentState( work );                               // RRL_STATE_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_RETURN_FROM_KEYWAIT ); // => RRL_STATE_RETURN_FROM_KEYWAIT 
    return;
  }

  //---------
  // ���L�[
  if( trg & PAD_KEY_UP ) {
    SetTopicButtonCursorOff( work );                   // �ړ��O�̍��ڂ����ɖ߂�
    SetTopicCursorNextPos( work, -1 );                 // �ړ����ݒ�
    FinishCurrentState( work );                        // RRL_STATE_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_AUTO_SCROLL );  // => RRL_STATE_AUTO_SCROLL 
    RegisterNextState( work, RRL_STATE_KEY_WAIT );     // ==> RRL_STATE_KEY_WAIT 
    return;
  } 
  //---------
  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    SetTopicButtonCursorOff( work );                   // �ړ��O�̍��ڂ����ɖ߂�
    SetTopicCursorNextPos( work, 1 );                  // �ړ����ݒ�
    FinishCurrentState( work );                        // RRL_STATE_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_AUTO_SCROLL );  // => RRL_STATE_AUTO_SCROLL 
    RegisterNextState( work, RRL_STATE_KEY_WAIT );     // ==> RRL_STATE_KEY_WAIT 
    return;
  } 

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );    // ���ڑI�����̃p���b�g�A�j���J�n
    SetSelectedTopicID( work, work->topicCursorPos );         // �J�[�\���ʒu�̒������ڂ�I��
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                           // ���艹
    RegisterNextState( work, RRL_STATE_TO_CONFIRM_KEY_WAIT ); // => RRL_STATE_TO_CONFIRM_KEY_WAIT 
    FinishCurrentState( work );                               // RRL_STATE_KEY_WAIT ��ԏI��
    return;
  } 

  //----------------------------
  //�u�������ځv�{�^�� ���^�b�`
  if( (TOPIC_TOUCH_AREA_TOPIC_0 <= touchTrg) && (touchTrg <= TOPIC_TOUCH_AREA_TOPIC_9) ) {
    // �I���\
    if( CheckTopicCanSelect( work, touchTrg ) == TRUE ) {
      MoveTopicCursorDirect( work, touchTrg );                 // �J�[�\���ړ�
      SetSelectedTopicID( work, work->topicCursorPos );        // �J�[�\���ʒu�̒������ڂ�I��
      StartPaletteAnime( work, PALETTE_ANIME_TOPIC_SELECT );   // ���ڑI�����̃p���b�g�A�j���J�n;
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                          // ���艹
      UpdateTopicDetailStrings_at_Now( work );                 // ���ʂ̒������ڐ��������X�V
      FinishCurrentState( work );                              // RRL_STATE_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_TO_CONFIRM_STANDBY ); // => RRL_STATE_TO_CONFIRM_STANDBY 
    }
    return;
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                           // �L�����Z����
    BlinkReturnButton( work );                                //�w�߂�x�{�^���𖾖ł�����
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON );            // �{�^���ŏI��
    FinishCurrentState( work );                               // RRL_STATE_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_RETURN_FROM_KEYWAIT ); // => RRL_STATE_RETURN_FROM_KEYWAIT 
    return;
  } 

  //----------------
  // �X�N���[���o�[
  if( ( GFL_UI_TP_HitCont( work->scrollTouchHitTable ) == SCROLL_TOUCH_AREA_BAR ) ||
      ( GFL_UI_TP_HitTrg( work->scrollTouchHitTable ) == SCROLL_TOUCH_AREA_BAR ) ) {
    // �X�N���[������\
    if( CheckScrollControlCan( work ) == TRUE ) {
      FinishCurrentState( work );                         // RRL_STATE_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_SLIDE_CONTROL ); // => RRL_STATE_SLIDE_CONTROL 
      RegisterNextState( work, RRL_STATE_KEY_WAIT );      // ==> RRL_STATE_KEY_WAIT 
    }
    return;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������҂���� ( RRL_STATE_AUTO_SCROLL ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_AUTO_SCROLL( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    StartAutoScroll_to_Topic( work );                         // �I�[�g�X�N���[���J�n
    MoveTopicCursorDirect( work, work->topicCursorNextPos );  // �J�[�\�����ړ�
    UpdateTopicDetailStrings_at_Now( work );                  // ���ʂ̒������ڐ��������X�V
    IncStateSeq( work );
    break;

  case 1:
    // �X�N���[������
    UpdateScroll( work );            // �X�N���[�����X�V
    UpdateScrollValue( work );       // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );    // �^�b�`�͈͂��X�V
    UpdateSliderDisp( work );        // �X���C�_�[
    UpdateInvestigatingIcon( work ); // �������ڑI���A�C�R�����X�V
    UpdateTopicButtonMask( work );   // �������ڂ̃}�X�N�E�B���h���X�V

    // �X�N���[���I��
    if( CheckScrollEnd(work) ) {
      UpdateTopicButtonMask( work ); // �E�B���h�E��؂�
      FinishCurrentState( work );    // RRL_STATE_AUTO_SCROLL ��ԏI��
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������� ( RRL_STATE_SLIDE_CONTROL ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_SLIDE_CONTROL( RRL_WORK* work )
{
  u32 x, y;
  BOOL touch;

  touch = GFL_UI_TP_GetPointCont( &x, &y );

  switch( GetStateSeq(work) ) {
  case 0: 
    // �^�b�`�������ꂽ
    if( touch == FALSE ) {
      AdjustSliderSpeed_forFreeMove( work ); // �X���C�_�[�̑��x�𒲐�
      AdjustSliderAccel_forFreeMove( work ); // �X���C�_�[�̉����x�𒲐�
      IncStateSeq( work ); 
      break;
    }

    // �X�N���[������
    {
      int scrollCursorPos;
      scrollCursorPos = CalcScrollCursorPos_bySliderPos( work, y ); // �^�b�`�ꏊ����, �X�N���[���J�[�\���ʒu���Z�o
      SetScrollCursorPos( work, scrollCursorPos );// �X�N���[���J�[�\���ʒu���X�V
      AdjustScrollValue( work );                  // �X�N���[�������l���X�V
      UpdateTopicTouchArea( work );               // �^�b�`�͈͂��X�V����
      UpdateSliderDisp( work );                   // �X���C�_�[���X�V
      UpdateInvestigatingIcon( work );            // �������ڑI���A�C�R�����X�V
      ObserveSliderSpeed( work );                 // �X���C�_�[�̈ړ����x���ϑ�
    }

    break;

  case 1:
    // �X�N���[������
    {
      int scrollCursorPos;
      scrollCursorPos = work->scrollCursorPos + work->sliderSpeed;
      SetScrollCursorPos( work, scrollCursorPos );// �X�N���[���J�[�\���ʒu���X�V
      AdjustScrollValue( work );                  // �X�N���[�������l���X�V
      UpdateTopicTouchArea( work );               // �^�b�`�͈͂��X�V����
      UpdateSliderDisp( work );                   // �X���C�_�[���X�V
      UpdateInvestigatingIcon( work );            // �������ڑI���A�C�R�����X�V
    } 

    // �X���C�_�[�̑��x���X�V
    AccelerateSliderSpeed( work );

    // �X���C�_�[����~
    if( IsSliderMoving( work ) == FALSE ) {
      UpdateTopicDetailStrings_at_Now( work ); // ���ʂ̒������ڐ��������X�V
      ShowTopicDetailStrings( work );          // ���ʂ̏ڍו\���J�n
      FinishCurrentState( work );
    }
    break;
  }

  // �J�[�\���ʒu����
  AdjustTopicCursor( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��� ( RRL_STATE_CONFIRM_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_CONFIRM_STANDBY( RRL_WORK* work )
{
  int trg;
  int touchTrg;

  trg = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->menuTouchHitTable );

  //----------------------
  // �\���L�[ or A �{�^��
  if( (trg & PAD_KEY_UP) || (trg & PAD_KEY_DOWN) ||
      (trg & PAD_KEY_LEFT) || (trg & PAD_KEY_RIGHT) || (trg & PAD_BUTTON_A) ) {
    FinishCurrentState( work );                               // RRL_STATE_CONFIRM_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_TO_CONFIRM_KEY_WAIT ); // => RRL_STATE_TO_CONFIRM_KEY_WAIT 
    return;
  }

  //---------------------------
  //�u�����Ă��v�{�^�����^�b�`
  if( touchTrg == MENU_TOUCH_AREA_OK_BUTTON ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_OK ); // �J�[�\���ړ�
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );   // �J�[�\��������Ă��鎞�̃p���b�g�A�j�����~
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );     // �I�����̃p���b�g�A�j�����J�n
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );             // �^�b�`�ŏI��
    FinishCurrentState( work );                               // RRL_STATE_CONFIRM_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_CONFIRM_DETERMINE );   // => RRL_STATE_CONFIRM_DETERMINE 
    return;
  } 

  //-----------------------------
  // B �{�^�� or �u��߂�v�{�^��
  if( (trg & PAD_BUTTON_B) || (touchTrg == MENU_TOUCH_AREA_CANCEL_BUTTON) ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_CANCEL ); // �J�[�\���ړ�
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );       // �J�[�\��������Ă��鎞�̃p���b�g�A�j�����~
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );         // �I�����̃p���b�g�A�j�����J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                               // �L�����Z����
    FinishCurrentState( work );                                   // RRL_STATE_CONFIRM_STANDBY ��ԏI��
    RegisterNextState( work, RRL_STATE_CONFIRM_CANCEL );          // => RRL_STATE_CONFIRM_CANCEL 
    RegisterNextState( work, RRL_STATE_KEY_WAIT );                // ==> RRL_STATE_KEY_WAIT 
    return;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��� ( RRL_STATE_CONFIRM_KEY_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_CONFIRM_KEY_WAIT( RRL_WORK* work )
{
  int trg;
  int touchTrg;

  trg = GFL_UI_KEY_GetTrg();
  touchTrg = GFL_UI_TP_HitTrg( work->menuTouchHitTable );

  //--------
  // ���L�[
  if( trg & PAD_KEY_UP ) {
    MoveMenuCursorUp( work ); // �J�[�\���ړ�
  }
  //--------
  // ���L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveMenuCursorDown( work ); // �J�[�\���ړ�
  } 

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON ); // �J�[�\��������Ă���ۂ̃p���b�g�A�j�����~
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );   // ���ڑI�����̃p���b�g�A�j�����J�n
    // �J�[�\���ʒu�ɉ���������
    switch( work->menuCursorPos ) {
    case MENU_ITEM_DETERMINATION_OK:
      SetFinishReason( work, SEQ_CHANGE_BY_BUTTON );          // �{�^���ŏI��
      FinishCurrentState( work );                             // RRL_STATE_CONFIRM_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_CONFIRM_DETERMINE ); // => RRL_STATE_CONFIRM_DETERMINE 
      break;
    case MENU_ITEM_DETERMINATION_CANCEL:
      PMSND_PlaySE( SEQ_SE_CANCEL1 );                       // �L�����Z����
      FinishCurrentState( work );                           // RRL_STATE_CONFIRM_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_CONFIRM_CANCEL );  // => RRL_STATE_CONFIRM_CANCEL 
      RegisterNextState( work, RRL_STATE_KEY_WAIT );        // ==> RRL_STATE_KEY_WAIT 
      break;
    default:
      GF_ASSERT(0);
    }
    return;
  } 

  //----------------------------
  //�u�����Ă��v�{�^�����^�b�`
  if( touchTrg == MENU_TOUCH_AREA_OK_BUTTON ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_OK ); // �J�[�\���ړ�
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );   // �J�[�\��������Ă���ۂ̃p���b�g�A�j�����~
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );     // ���ڑI�����̃p���b�g�A�j�����J�n
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );             // �^�b�`�ŏI��
    FinishCurrentState( work );                               // RRL_STATE_CONFIRM_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_CONFIRM_DETERMINE );   // => RRL_STATE_CONFIRM_DETERMINE 
    return;
  } 

  //-----------------------------
  // B �{�^�� or �u��߂�v�{�^��
  if( (trg & PAD_BUTTON_B) || (touchTrg == MENU_TOUCH_AREA_CANCEL_BUTTON) ) {
    MoveMenuCursorDirect( work, MENU_ITEM_DETERMINATION_CANCEL ); // �J�[�\���ړ�
    StopPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );       // �J�[�\��������Ă���ۂ̃p���b�g�A�j�����~
    StartPaletteAnime( work, PALETTE_ANIME_MENU_SELECT );         // ���ڑI�����̃p���b�g�A�j�����J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                               // �L�����Z����
    FinishCurrentState( work );                                   // RRL_STATE_CONFIRM_KEY_WAIT ��ԏI��
    RegisterNextState( work, RRL_STATE_CONFIRM_CANCEL );          // => RRL_STATE_CONFIRM_CANCEL 
    RegisterNextState( work, RRL_STATE_KEY_WAIT );                // ==> RRL_STATE_KEY_WAIT 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��� ( RRL_STATE_TO_CONFIRM_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_TO_CONFIRM_STANDBY( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    SetMenuItemCursorOff( work, MENU_ITEM_DETERMINATION_CANCEL ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
    SetMenuItemCursorOff( work, MENU_ITEM_DETERMINATION_OK );     // �J�[�\��������Ă��Ȃ���Ԃɂ���

    // �m�F���b�Z�[�W�ƑI�����ڂ�\��
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, TRUE );
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, TRUE );
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, TRUE );

    if( work->palFadeOutFlag == FALSE ) {
      BG_FONT_SetPalette( work->BGFont_topic[ work->topicCursorPos ], 0xe ); // �t�H���g�̃p���b�g��ύX ( �p���b�g�t�F�[�h�������ɂȂ�悤�� )
      StartPaletteFadeOut( work ); // �p���b�g�t�F�[�h�A�E�g�J�n
    }

    // �X�N���[���J�n
    StartAutoScroll_to_Confirm( work );

    IncStateSeq( work );
    break;

  case 1:
    // �X�N���[������
    UpdateScroll( work );             // �X�N���[�����X�V
    UpdateScrollValue( work );        // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );     // �^�b�`�͈͂��X�V
    UpdateSliderDisp( work );         // �X���C�_�[���X�V
    UpdateInvestigatingIcon( work );  // �������ڑI���A�C�R�����X�V
    UpdateTopicButtonMask( work );    // �������ڂ̃}�X�N�E�B���h���X�V

    // �X�N���[���I��
    if( CheckScrollEnd( work ) ) {
      FinishCurrentState( work );                           // RRL_STATE_TO_CONFIRM_STANDBY ��ԏI��
      RegisterNextState( work, RRL_STATE_CONFIRM_STANDBY ); // => RRL_STATE_CONFIRM_STANDBY
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm��̊m�F��� ( RRL_STATE_TO_CONFIRM_KEY_WAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_TO_CONFIRM_KEY_WAIT( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    SetMenuCursorPos( work, MENU_ITEM_DETERMINATION_OK );         // �J�[�\���ʒu��������
    SetMenuItemCursorOff( work, MENU_ITEM_DETERMINATION_CANCEL ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
    SetMenuItemCursorOn( work, MENU_ITEM_DETERMINATION_OK );      // �J�[�\��������Ă����Ԃɂ���
    StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_ON );      // �p���b�g�A�j���[�V�����J�n

    // �m�F���b�Z�[�W�ƑI�����ڂ�\��
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, TRUE );
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, TRUE );
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, TRUE );

    if( work->palFadeOutFlag == FALSE ) {
      BG_FONT_SetPalette( work->BGFont_topic[ work->topicCursorPos ], 0xe ); // �t�H���g�̃p���b�g��ύX ( �p���b�g�t�F�[�h�������ɂȂ�悤�� )
      StartPaletteFadeOut( work ); // �p���b�g�t�F�[�h�A�E�g�J�n
    }

    // �X�N���[���J�n
    StartAutoScroll_to_Confirm( work );

    IncStateSeq( work );
    break;

  case 1:
    // �X�N���[������
    UpdateScroll( work );             // �X�N���[�����X�V
    UpdateScrollValue( work );        // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );     // �^�b�`�͈͂��X�V
    UpdateSliderDisp( work );         // �X���C�_�[���X�V
    UpdateInvestigatingIcon( work );  // �������ڑI���A�C�R�����X�V
    UpdateTopicButtonMask( work );    // �������ڂ̃}�X�N�E�B���h���X�V

    // �X�N���[���I��
    if( CheckScrollEnd( work ) ) {
      FinishCurrentState( work );                            // RRL_STATE_TO_CONFIRM_KEY_WAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_CONFIRM_KEY_WAIT ); // => RRL_STATE_CONFIRM_KEY_WAIT
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڊm���� ( RRL_STATE_CONFIRM_DETERMINE ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_CONFIRM_DETERMINE( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    SetTopicButtonNotInvestigating( work, GetInvestigatingTopicID(work) ); // ���������������ڂ̃{�^����, �������Ă��Ȃ���Ԃɖ߂�
    UpdateInvestigatingTopicID( work );                                    // �������鍀�ڂ��X�V
    SetTopicButtonInvestigating( work, GetSelectedTopicID(work) );         // �V���ɒ������鍀�ڂ̃{�^����, �������̏�Ԃɂ���
    UpdateInvestigatingIcon( work );                                       // �������A�C�R�����X�V
    BmpOamSetDrawEnable( work, BMPOAM_ACTOR_DETERMINE, TRUE );             //�u���傤�����@���������܂��I�v��\��
    PMSND_PlaySE( SEQ_SE_SYS_80 );                                         // �����J�n SE 
    IncStateSeq( work );
    break;

  case 1:
    // ��莞�Ԃ��o��
    if( SEQ_DETERMINE_WAIT_FRAMES <= work->stateCount ) {
      StartFadeOut(); // �t�F�[�h�A�E�g�J�n
      IncStateSeq( work );
    }
    break;

  case 2:
    // �t�F�[�h���I��
    if( CheckFadeEnd() ) {
      StartPaletteFadeIn( work ); // �p���b�g�̃t�F�[�h�C�����J�n����
      IncStateSeq( work );
    } 
    break;

  case 3:
    // �p���b�g�t�F�[�h����
    if( IsPaletteFadeEnd( work ) ) {
      FinishCurrentState( work );                    // RRL_STATE_CONFIRM_DETERMINE ��ԏI��
      RegisterNextState( work, RRL_STATE_CLEAN_UP ); // ===> RRL_STATE_CLEAN_UP 
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g ��� ( RRL_STATE_RETURN_FROM_KEYWAIT ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_RETURN_FROM_KEYWAIT( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    // �҂����Ԃ��o��
    if( GetWaitFrame(work) < work->stateCount ) {
      StartFadeOut(); // �t�F�[�h�A�E�g�J�n
      IncStateSeq( work );
    }
    break;

  case 1:
    // �t�F�[�h���I��
    if( CheckFadeEnd() ) {
      StartPaletteFadeIn( work );    // �p���b�g�̃t�F�[�h�C�����J�n����
      IncStateSeq( work );
    } 
    break;

  case 2:
    // �p���b�g�t�F�[�h����
    if( IsPaletteFadeEnd( work ) ) {
      BG_FONT_SetPalette( work->BGFont_topic[ work->topicCursorPos ], 0xf); // �t�H���g�̃p���b�g�����ɖ߂�
      FinishCurrentState( work );                                           // RRL_STATE_RETURN_FROM_KEYWAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_CLEAN_UP );                        // => RRL_STATE_CLEAN_UP 
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂� ��� ( RRL_STATE_RETURN_FROM_STANDBY ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_RETURN_FROM_STANDBY( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    // �҂����Ԃ��o��
    if( GetWaitFrame(work) < work->stateCount ) {
      StartFadeOut(); // �t�F�[�h�A�E�g�J�n
      IncStateSeq( work );
    }
    break;

  case 1:
    // �t�F�[�h���I��
    if( CheckFadeEnd() ) {
      StartPaletteFadeIn( work );    // �p���b�g�̃t�F�[�h�C�����J�n����
      IncStateSeq( work );
    } 
    break;

  case 2:
    // �p���b�g�t�F�[�h����
    if( IsPaletteFadeEnd( work ) ) {
      BG_FONT_SetPalette( work->BGFont_topic[ work->topicCursorPos ], 0xf); // �t�H���g�̃p���b�g�����ɖ߂�
      FinishCurrentState( work );                                           // RRL_STATE_RETURN_FROM_KEYWAIT ��ԏI��
      RegisterNextState( work, RRL_STATE_CLEAN_UP );                        // => RRL_STATE_CLEAN_UP 
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g���A��� ( RRL_STATE_CONFIRM_CANCEL ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_CONFIRM_CANCEL( RRL_WORK* work )
{
  switch( GetStateSeq( work ) ) {
  case 0:
    StartAutoScroll_for_Reset( work ); // �X�N���[���J�n
    IncStateSeq( work );
    break;

  case 1:
    // �X�N���[������
    UpdateScroll( work );            // �X�N���[�����X�V
    UpdateScrollValue( work );       // �X�N���[�������l���X�V
    UpdateTopicTouchArea( work );    // �^�b�`�͈͂��X�V����
    UpdateSliderDisp( work );        // �X���C�_�[
    UpdateInvestigatingIcon( work ); // �������ڑI���A�C�R��
    UpdateTopicButtonMask( work );   // �������ڂ̃}�X�N�E�B���h���X�V����

    // �X�N���[�����I��
    if( CheckScrollEnd(work) ) {
      IncStateSeq( work );
    }
    break;

  case 2:
    // �p���b�g�A�j�����I��
    if( CheckPaletteAnimeEnd(work, PALETTE_ANIME_MENU_SELECT ) ) {
      // �m�F���b�Z�[�W�ƑI�����ڂ�����
      BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CONFIRM, FALSE );
      BmpOamSetDrawEnable( work, BMPOAM_ACTOR_OK, FALSE );
      BmpOamSetDrawEnable( work, BMPOAM_ACTOR_CANCEL, FALSE );
      UpdateTopicButtonMask( work ); // �E�B���h�E��؂�
      StartPaletteFadeIn( work );    // �p���b�g�̃t�F�[�h�C�����J�n����
      IncStateSeq( work );
    }
    break;

  case 3:
    // �p���b�g�t�F�[�h����
    if( IsPaletteFadeEnd( work ) ) {
      BG_FONT_SetPalette( work->BGFont_topic[ work->topicCursorPos ], 0xf); // �t�H���g�̃p���b�g�����ɖ߂�
      FinishCurrentState( work ); // ���̏�Ԃ�
    }
    break;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Еt����� ( RRL_STATE_CLEAN_UP ) �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MainState_CLEAN_UP( RRL_WORK* work )
{ 
  // VBlank�^�X�N������
  ReleaseVBlankTask( work );

  // �p���b�g�A�j���[�V���� 
  CleanUpPaletteAnime( work );
  DeletePaletteAnime( work );

  // ���ʃp���b�g�A�j���[�V����
  RRC_StopAllPaletteAnime( work->commonWork ); // ��~����, 
  RRC_ResetAllPalette( work->commonWork );     // �p���b�g�����ɖ߂�

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
  SetFinishResult( work, RRL_RESULT_TO_TOP );   // ���X�g��ʂ̏I�����ʂ�ݒ�
  FinishCurrentState( work );                   // RRL_STATE_CLEAN_UP ��ԏI��
  RegisterNextState( work, RRL_STATE_FINISH );  // => RRL_STATE_FINISH 
  work->finishFlag = TRUE;
  GX_SetVisibleWnd( GX_WNDMASK_NONE ); //�E�B���h�E�𖳌���
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���C���N�������g����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void IncStateCount( RRL_WORK* work )
{
  work->stateCount++;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���̏�Ԃ��L���[�ɓo�^����
 *
 * @param work
 * @param nextSeq �o�^������
 */
//-----------------------------------------------------------------------------------------
static void RegisterNextState( RRL_WORK* work, RRL_STATE nextSeq )
{
  QUEUE_EnQueue( work->stateQueue, nextSeq );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���݂̏�Ԃ��I������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void FinishCurrentState( RRL_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->stateEndFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->stateEndFlag = TRUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ʑJ�ڂ̕��@���擾����
 *
 * @param work
 *
 * @return ��ʑJ�ڂ̕��@
 */
//-----------------------------------------------------------------------------------------
static SEQ_CHANGE_TRIG GetFinishReason( RRL_WORK* work )
{
  return RRC_GetSeqChangeTrig( work->commonWork );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʏI���̕��@��o�^����
 *
 * @param work
 * @param reason �I�����R
 */
//-----------------------------------------------------------------------------------------
static void SetFinishReason( RRL_WORK* work, SEQ_CHANGE_TRIG reason )
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
static void SetFinishResult( RRL_WORK* work, RRL_RESULT result )
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
static void SetWaitFrame( RRL_WORK* work, u32 frame )
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
static u32 GetWaitFrame( const RRL_WORK* work )
{
  return work->waitFrame;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SwitchState( RRL_WORK* work )
{
  RRL_STATE nextSeq;

  if( work->stateEndFlag == FALSE ){ return; }  // ���݂̏�Ԃ��I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->stateQueue ) ){ return; } // ��ԃL���[�ɓo�^����Ă��Ȃ�

  // �ύX
  nextSeq = QUEUE_DeQueue( work->stateQueue );
  SetState( work, nextSeq ); 
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ��擾����
 *
 * @param work
 *
 * @return ���݂̏��
 */
//-----------------------------------------------------------------------------------------
static RRL_STATE GetState( const RRL_WORK* work )
{
  return work->state;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ݒ肷��
 *
 * @param work
 * @parma nextSeq �ݒ肷����
 */
//-----------------------------------------------------------------------------------------
static void SetState( RRL_WORK* work, RRL_STATE nextSeq )
{ 
  // �X�V
  work->state        = nextSeq;
  work->stateCount   = 0;
  work->stateEndFlag = FALSE;
  ResetStateSeq( work );
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
static u32 GetStateSeq( const RRL_WORK* work )
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
static void IncStateSeq( RRL_WORK* work )
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
static void ResetStateSeq( RRL_WORK* work )
{
  work->stateSeq = 0;
}



//=========================================================================================
// ��LAYER 4 �@�\
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorUp( RRL_WORK* work )
{ 
  SetMenuItemCursorOff( work, work->menuCursorPos );        // �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, -1 );                           // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos );         // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                           // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�������ֈړ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void MoveMenuCursorDown( RRL_WORK* work )
{
  SetMenuItemCursorOff( work, work->menuCursorPos );        // �J�[�\��������Ă��Ȃ���Ԃɂ���
  ShiftMenuCursorPos( work, 1 );                            // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos );         // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                           // �J�[�\���ړ���
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
static void MoveMenuCursorDirect( RRL_WORK* work, MENU_ITEM menuItem )
{
  SetMenuItemCursorOff( work, work->menuCursorPos );        // �J�[�\��������Ă��Ȃ���Ԃɂ���
  SetMenuCursorPos( work, menuItem );                       // �J�[�\���ړ�
  SetMenuItemCursorOn( work, work->menuCursorPos );         // �J�[�\��������Ă����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                           // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_MENU_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\������ʓ��ɑ��݂���悤�ɒ�������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustTopicCursor( RRL_WORK* work )
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param topicID �ړ���̒�������ID
 */
//-----------------------------------------------------------------------------------------
static void MoveTopicCursorDirect( RRL_WORK* work, u8 topicID )
{
  SetTopicButtonCursorOff( work );                           // �ړ��O�̍��ڂ����ɖ߂�
  SetTopicCursorPosDirect( work, topicID );                  // �J�[�\���ʒu���X�V
  SetTopicButtonCursorOn( work );                            // �ړ���̍��ڂ�I����Ԃɂ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );                            // �J�[�\���ړ���
  StartPaletteAnime( work, PALETTE_ANIME_TOPIC_CURSOR_SET ); // �p���b�g�A�j���J�n
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�w�߂�x�{�^���𖾖ł�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void BlinkReturnButton( RRL_WORK* work )
{
  StartCommonPaletteAnime( work, COMMON_PALETTE_ANIME_RETURN );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�[�g�X�N���[�����J�n���� ( �������ڃJ�[�\���ʒu�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartAutoScroll_to_Topic( RRL_WORK* work )
{
  int startPos = work->scrollCursorPos;
  int endPos;
  int frames;

  // ���֐i�ޏꍇ
  if( work->topicCursorPos < work->topicCursorNextPos ) {
    // �X�N���[���J�[�\���̏I�_�̓{�^���̒�ӈʒu + 1
    endPos = CalcScrollCursorPosOfTopicButtonBottom( work->topicCursorNextPos ) + 1;
  }
  // ��֐i�ޏꍇ
  else {
    // �X�N���[���J�[�\���̏I�_�̓{�^���̏�ӈʒu
    endPos = CalcScrollCursorPosOfTopicButtonTop( work->topicCursorNextPos );
  }

  // �X�N���[�������l�ɕύX������X�N���[���̏ꍇ,
  // �X�N���[���J�[�\���n�_���W��[�ɃZ�b�g����
  {
    int min = GetMinScrollCursorMarginPos();
    int max = GetMaxScrollCursorMarginPos();
    if( (endPos < min) || (max < endPos) ) {
      if( startPos < endPos ) {
        startPos = max;
      }
      else {
        startPos = min;
      }
    }
  }

  frames = ( MATH_MAX(startPos, endPos) - MATH_MIN(startPos, endPos) ) / 4;

  // �X�N���[�������l�ɕω������邩�ǂ���
  {
    int min = GetMinScrollCursorMarginPos();
    int max = GetMaxScrollCursorMarginPos();
    // �X�N���[�������l�ɕύX���Ȃ�
    if( ( min <= startPos) && (startPos <= max) && 
        ( min <= endPos) && (endPos <= max)  ) {
      frames = 0;
    }
  } 
  StartScroll( work, startPos, endPos, frames );  
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�[�g�X�N���[�����J�n���� ( �͈͊O�X�N���[�������Z�b�g )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartAutoScroll_for_Reset( RRL_WORK* work )
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �I�[�g�X�N���[�����J�n���� ( �������ڊm��̊m�F�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartAutoScroll_to_Confirm( RRL_WORK* work )
{
  int centerPos = ( GetMinScrollCursorMarginPos() + GetMaxScrollCursorMarginPos() ) / 2;
  int startPos  = work->scrollCursorPos;
  int endPos    = CalcScreenTopOfTopicButton( work, work->topicCursorPos ) * DOT_PER_CHARA;
  int frames;
  // �J�[�\���ʒu�̒������ڂ���ʂ̔C�ӂ̈ʒu�܂ŗ���悤�ɃX�N���[��������
  if( centerPos < endPos ) { 
    // �i�ޕ����ŃJ�[�\���ʒu�̎n�_�E�I�_��ς���
    startPos = GetMaxScrollCursorMarginPos();
    endPos   = GetMaxScrollCursorMarginPos() + (endPos - centerPos);
  }
  else {
    startPos = GetMinScrollCursorMarginPos();
    endPos   = GetMinScrollCursorMarginPos() - (centerPos - endPos);
  }
  frames = ( MATH_MAX(startPos, endPos) - MATH_MIN(startPos, endPos) ) / 3;
  StartScroll( work, startPos, endPos, frames  );  
}


//=========================================================================================
// ��LAYER 3 �ʑ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ݒ������̒�������ID���擾����
 *
 * @param work
 *
 * @return ���ݒ������̒�������ID
 */
//-----------------------------------------------------------------------------------------
static u8 GetInvestigatingTopicID( const RRL_WORK* work )
{
  int qIdx;
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  u8 questionID[ QUESTION_NUM_PER_TOPIC ];
  u8 topicID;

  gameData = GetGameData( work );
  save     = GAMEDATA_GetSaveControlWork( gameData );
  QSave    = SaveData_GetQuestionnaire( save );

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
 * @brief �������鍀�ڂ��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateInvestigatingTopicID( const RRL_WORK* work )
{
  GAMEDATA* gameData;
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* QSave;
  u8 topicID;

  // ���ڂ�I�����Ă��Ȃ�
  GF_ASSERT( IsTopicIDSelected(work) );

  topicID  = GetSelectedTopicID( work ); // �I��������������ID
  gameData = GetGameData( work );
  save     = GAMEDATA_GetSaveControlWork( gameData );
  QSave    = SaveData_GetQuestionnaire( save );
  
  // �Z�[�u�f�[�^���X�V
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question1_topic[ topicID ], 0 );
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question2_topic[ topicID ], 1 );
  QuestionnaireWork_SetInvestigatingQuestion( QSave, Question3_topic[ topicID ], 2 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\�����ړ�����
 *
 * @parma work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void ShiftMenuCursorPos( RRL_WORK* work, int stride )
{
  int nowPos;
  int nextPos;

  // �J�[�\���ʒu���X�V
  nowPos  = work->menuCursorPos;
  nextPos = (nowPos + stride + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  work->menuCursorPos = nextPos; 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڃJ�[�\���ʒu��ύX���� ( ���l�w�� )
 *
 * @param work
 * @param menuItem �ݒ肷�郁�j���[���ڈʒu
 */
//-----------------------------------------------------------------------------------------
static void SetMenuCursorPos( RRL_WORK* work, MENU_ITEM menuItem )
{
  // �J�[�\���ʒu���X�V
  work->menuCursorPos = menuItem;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����j���[���ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 * @param menuItem ���j���[����
 */
//-----------------------------------------------------------------------------------------
static void SetMenuItemCursorOn( RRL_WORK* work, MENU_ITEM menuItem )
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
static void SetMenuItemCursorOff( RRL_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACT_PTR BmpOamActor;

  // �J�[�\���ʒu�̃��j���[���ڂɑΉ�����BMP-OAM �A�N�^�[���擾
  BmpOamActor = GetBmpOamActorOfMenuItem( work, menuItem );

  // BMP-OAM �A�N�^�[�̃p���b�g�I�t�Z�b�g��ύX
  BmpOam_ActorSetPaletteOffset( BmpOamActor, 1 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̈ʒu��ݒ肷��
 *
 * @param work
 * @param topicID �ݒ肷�钲������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicCursorPosDirect( RRL_WORK* work, int topicID )
{
  // �J�[�\���ʒu���X�V
  work->topicCursorPos = topicID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������ڃJ�[�\���̈ړ����ݒ肷��
 *
 * @param work
 * @param stride �ړ���
 */
//-----------------------------------------------------------------------------------------
static void SetTopicCursorNextPos( RRL_WORK* work, int stride )
{
  int nowPos, nextPos;

  // �ړ���̃J�[�\���ʒu���Z�o
  nowPos  = work->topicCursorPos;
  nextPos = (nowPos + stride + GetSelectableTopicNum(work)) % GetSelectableTopicNum(work);

  // �J�[�\���̈ړ����ݒ�
  work->topicCursorNextPos = nextPos;
}

//-----------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă����Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonCursorOn( const RRL_WORK* work )
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
  GFL_BG_LoadScreenV_Req( BGFrame );
}

//-----------------------------------------------------------------------------------------
/**
 * @biref �J�[�\���ʒu�ɂ��钲�����ڂ�, �J�[�\��������Ă��Ȃ���Ԃɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonCursorOff( const RRL_WORK* work )
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
  GFL_BG_LoadScreenV_Req( BGFrame );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڂ̃{�^����, �������̏�Ԃɐݒ肷��
 *
 * @param work
 * @param topicID ��������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonInvestigating( const RRL_WORK* work, u8 topicID )
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
  GFL_BG_LoadScreenV_Req( MAIN_BG_WINDOW );
  GFL_BG_LoadScreenV_Req( MAIN_BG_FONT );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���������ڂ̃{�^����, �������łȂ���Ԃɖ߂�
 *
 * @param work
 * @param topicID ��������ID
 */
//-----------------------------------------------------------------------------------------
static void SetTopicButtonNotInvestigating( const RRL_WORK* work, u8 topicID )
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
  GFL_BG_LoadScreenV_Req( MAIN_BG_WINDOW );
  GFL_BG_LoadScreenV_Req( MAIN_BG_FONT );
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
static u8 CalcScreenLeftOfTopicButton( const RRL_WORK* work, u8 topicID )
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
static u8 CalcScreenTopOfTopicButton( const RRL_WORK* work, u8 topicID )
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
static int CalcDisplayLeftOfTopicButton( const RRL_WORK* work, u8 topicID )
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
static int CalcDisplayTopOfTopicButton( const RRL_WORK* work, u8 topicID )
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
static int CalcDisplayBottomOfTopicButton( const RRL_WORK* work, u8 topicID )
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
static void UpdateTopicButtonMask( const RRL_WORK* work )
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
 * @brief �������ڑI���A�C�R���̕\����Ԃ��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateInvestigatingIcon( const RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̒������ڐ��������X�V���� ( ���݂̃J�[�\���ʒu�ɍ��킹�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicDetailStrings_at_Now( RRL_WORK* work )
{
  UpdateTopicDetailStrings_at( work, work->topicCursorPos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̒������ڐ��������X�V���� ( ���̃J�[�\���ʒu�ɍ��킹�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicDetailStrings_at_Next( RRL_WORK* work )
{
  UpdateTopicDetailStrings_at( work, work->topicCursorNextPos );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̒������ڐ��������X�V���� ( �w��J�[�\���ʒu�ɍ��킹�� )
 *
 * @param work
 * @param topicID �J�[�\���ʒu ( ��������ID )
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicDetailStrings_at( RRL_WORK* work, u8 topicID )
{
  // �������� �薼/�⑫
  BG_FONT_SetMessage( work->BGFont_string[ BG_FONT_TOPIC_TITLE ],   StringID_topicTitle[ topicID ] );
  BG_FONT_SetMessage( work->BGFont_string[ BG_FONT_TOPIC_CAPTION ], StringID_topicCaption[ topicID ] );

  // ����
  BG_FONT_SetMessage( work->BGFont_string[ BG_FONT_QUESTION_1 ], StringID_question[ Question1_topic[ topicID ] ] );
  BG_FONT_SetMessage( work->BGFont_string[ BG_FONT_QUESTION_2 ], StringID_question[ Question2_topic[ topicID ] ] );
  BG_FONT_SetMessage( work->BGFont_string[ BG_FONT_QUESTION_3 ], StringID_question[ Question3_topic[ topicID ] ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̒������ڐ������̕\�����B��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void HideTopicDetailStrints( RRL_WORK* work )
{
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_TOPIC_TITLE ], FALSE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_TOPIC_CAPTION ], FALSE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_1 ], FALSE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_2 ], FALSE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_3 ], FALSE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ̒������ڐ������̕\�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ShowTopicDetailStrings( RRL_WORK* work )
{
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_TOPIC_TITLE ], TRUE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_TOPIC_CAPTION ], TRUE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_1 ], TRUE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_2 ], TRUE );
  BG_FONT_SetDrawEnable( work->BGFont_string[ BG_FONT_QUESTION_3 ], TRUE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�̕\���ʒu���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateSliderDisp( const RRL_WORK* work )
{
  GFL_CLWK* clactWork;

  // �Z���A�N�^�[���[�N���擾
  clactWork  = GetClactWork( work, CLWK_SCROLL_CONTROL ); 

  // �\�����Ȃ�
  if( CheckScrollControlCan( work ) == FALSE ) {
    GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );
  }
  // �\������
  else { 
    GFL_CLACTPOS pos;
    u16 setSurface;

    // �\���ʒu���Z�o
    pos.x = SCROLL_CONTROL_LEFT;
    pos.y = CalcSliderPos_byScrollValue( work );
    setSurface = ClactWorkInitData[ CLWK_SCROLL_CONTROL ].setSurface;

    // �\���ʒu��ύX
    GFL_CLACT_WK_SetPos( clactWork, &pos, setSurface );
    GFL_CLACT_WK_SetDrawEnable( clactWork, TRUE );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�̕\���ʒu���v�Z����
 *
 * @param work
 *
 * @return �w�肵���X�N���[�������l���狁�߂�, �X�N���[���o�[�܂ݕ�����y���W
 */
//-----------------------------------------------------------------------------------------
static int CalcSliderPos_byScrollValue( const RRL_WORK* work )
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
 * @brief �X���C�_�[�̕\���ʒu����, �X�N���[���J�[�\���ʒu���Z�o����
 *
 * @param work
 * @param controlPos �܂ݕ����̕\���ʒu
 *
 * @return �X�N���[���J�[�\���̈ʒu
 */
//-----------------------------------------------------------------------------------------
static int CalcScrollCursorPos_bySliderPos( const RRL_WORK* work, int controlPos )
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
 * @brief �X���C�_�[�̕\���ʒu���擾����
 *
 * @param work
 *
 * @return �X���C�_�[�̕\���ʒu
 */
//-----------------------------------------------------------------------------------------
static int GetSliderPos( const RRL_WORK* work )
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
 * @brief �X���C�_�[�̈ړ����x���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AccelerateSliderSpeed( RRL_WORK* work )
{
  int prev_speed;
  int accl_speed;

  // �����x��K�p
  prev_speed = work->sliderSpeed;
  accl_speed = work->sliderSpeed + work->sliderAccel;

  // �������ω�
  if( prev_speed * accl_speed <= 0 ) { accl_speed = 0; }

  // ���x���X�V
  work->sliderSpeed = accl_speed;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�������Ă��邩�ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return �X���C�_�[�������Ă���ꍇ TRUE
 */
//-----------------------------------------------------------------------------------------
static BOOL IsSliderMoving( const RRL_WORK* work )
{
  if( work->sliderSpeed == 0 ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�̈ړ����x���ϑ�����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ObserveSliderSpeed( RRL_WORK* work )
{
  // �X�N���[���J�[�\���̕ω��ʂő��x������
  work->sliderSpeed = work->scrollCursorPos - work->scrollCursorPos_prev;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�̈ړ����x�𒲐����� ( ���R�ړ��p )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustSliderSpeed_forFreeMove( RRL_WORK* work )
{
  // ����₷���悤�ɒ���
  work->sliderSpeed *= 1.5;

  // �ő�l�␳
  if( SLIDER_MAX_SPEED_AT_FREE_MOVE < work->sliderSpeed ) {
    work->sliderSpeed = SLIDER_MAX_SPEED_AT_FREE_MOVE;
  }
  // �ŏ��l�␳
  else if( work->sliderSpeed < SLIDER_MIN_SPEED_AT_FREE_MOVE ) {
    work->sliderSpeed = SLIDER_MIN_SPEED_AT_FREE_MOVE;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X���C�_�[�̉����x�𒲐����� ( ���R�ړ��p )
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustSliderAccel_forFreeMove( RRL_WORK* work )
{
  if( work->sliderSpeed < 0 ) {
    work->sliderAccel = SLIDER_ACCEL_AT_FREE_MOVE;
  }
  else {
    work->sliderAccel = -SLIDER_ACCEL_AT_FREE_MOVE;
  }
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
static void StartScroll( RRL_WORK* work, int startPos, int endPos, int frames )
{
  // �X�N���[���p�����[�^��ݒ�
  work->scrollStartPos   = startPos;
  work->scrollEndPos     = endPos;
  work->scrollFrames     = frames;
  work->scrollFrameCount = 0;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�����X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateScroll( RRL_WORK* work )
{
  int maxFrame, nowFrame;
  int startPos, endPos, nowPos;

  GF_ASSERT( work->scrollFrameCount <= work->scrollFrames );

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
static BOOL CheckScrollEnd( RRL_WORK* work )
{
  int maxFrame, nowFrame;

  maxFrame = work->scrollFrames;
  nowFrame = work->scrollFrameCount;

  // �I��
  if( maxFrame <= nowFrame ) { 
    return TRUE; 
  }
  else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
 *
 * @param work
 * @param offset �ړ��I�t�Z�b�g�l
 */
//-----------------------------------------------------------------------------------------
static void ShiftScrollCursorPos( RRL_WORK* work, int offset )
{
  int now;
  int next;

  // �ړ���̃J�[�\���ʒu������
  now  = work->scrollCursorPos;
  next = now + offset; 
  if( next < MIN_SCROLL_CURSOR_POS ) { next = MIN_SCROLL_CURSOR_POS; } // �ŏ��l�␳
  if( GetMaxScrollCursorPos(work) < next ) { next = GetMaxScrollCursorPos(work); } // �ő�l�␳

  // �J�[�\���ʒu���X�V
  work->scrollCursorPos_prev = now;
  work->scrollCursorPos      = next;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w�� )
 *
 * @param work
 * @param pos �ݒ肷��J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetScrollCursorPos( RRL_WORK* work, int pos )
{
  // ������␳
  if( pos < MIN_SCROLL_CURSOR_POS ) { pos = MIN_SCROLL_CURSOR_POS; } // �ŏ��l�␳
  if( GetMaxScrollCursorPos(work) < pos ) { pos = GetMaxScrollCursorPos(work); } // �ő�l�␳

  // �J�[�\���ʒu���X�V
  work->scrollCursorPos_prev = work->scrollCursorPos;
  work->scrollCursorPos      = pos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��ύX���� ( ���l�w��E�͈͌���Ȃ� )
 *
 * @param work
 * @param pos �ݒ肷��J�[�\���ʒu
 */
//-----------------------------------------------------------------------------------------
static void SetScrollCursorPosForce( RRL_WORK* work, int pos )
{
  // �J�[�\���ʒu���X�V
  work->scrollCursorPos_prev = pos;
  work->scrollCursorPos      = pos;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���ʒu��␳����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void AdjustScrollCursor( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[�������l���X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateScrollValue( const RRL_WORK* work )
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
static void AdjustScrollValue( const RRL_WORK* work )
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
static int GetMaxScrollValue( const RRL_WORK* work )
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
static int GetMaxScrollCursorPos( const RRL_WORK* work )
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
static BOOL CheckScrollControlCan( const RRL_WORK* work )
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
 * @brief �������ڂ̃^�b�`�͈͂��X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void UpdateTopicTouchArea( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�C�����J�n����
 *
 * @param work
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
 * @brief �t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartFadeOut( void )
{
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      0, 16, 0);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�̏I�����`�F�b�N����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static BOOL CheckFadeEnd( void )
{
  // �t�F�[�h�I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�A�E�g���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeOut( RRL_WORK* work )
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

  // �t���O�𗧂Ă�
  work->palFadeOutFlag = TRUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�̃t�F�[�h�C�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void StartPaletteFadeIn( RRL_WORK* work )
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

  // �t���O�𗎂Ƃ�
  work->palFadeOutFlag = FALSE;
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
static BOOL IsPaletteFadeEnd( RRL_WORK* work )
{
  return (PaletteFadeCheck( work->paletteFadeSystem ) == 0);
}

//------------------------------------------------------------------------------------
/**
 * @brief ���ʃp���b�g�A�j���[�V�������J�n����
 *
 * @param work
 * @param index �J�n����p���b�g�A�j���̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void StartCommonPaletteAnime( RRL_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  RRC_StartPaletteAnime( work->commonWork, index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 * @param index �J�n����p���b�g�A�j���̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RRL_WORK* work, PALETTE_ANIME_INDEX index )
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
 * @param index ��~����p���b�g�A�j���̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static void StopPaletteAnime( RRL_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�����̏I�����`�F�b�N����
 *
 * @param work
 * @param index ��~����p���b�g�A�j���̃C���f�b�N�X
 *
 * @return �p���b�g�A�j�����I�����Ă���ꍇ TRUE
 */
//------------------------------------------------------------------------------------
static BOOL CheckPaletteAnimeEnd( const RRL_WORK* work, PALETTE_ANIME_INDEX index )
{
  if( PALETTE_ANIME_CheckAnime( work->paletteAnime[ index ] ) ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RRL_WORK* work )
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
static void UpdateCommonPaletteAnime( const RRL_WORK* work )
{
  RRC_UpdatePaletteAnime( work->commonWork );
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
  RRL_WORK* work = (RRL_WORK*)wk;

  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( work->paletteFadeSystem );
}


//=========================================================================================
// ��LAYER 2 �擾�E�ݒ�E����
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
static GAMESYS_WORK* GetGameSystem( const RRL_WORK* work )
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
static GAMEDATA* GetGameData( const RRL_WORK* work )
{
  return RRC_GetGameData( work->commonWork );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �q�[�vID��ݒ肷��
 *
 * @param work
 * @param heapID
 */
//-----------------------------------------------------------------------------------------
static void SetHeapID( RRL_WORK* work, HEAPID heapID )
{
  work->heapID = heapID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N��ݒ肷��
 *
 * @param work
 * @param commonWork
 */
//-----------------------------------------------------------------------------------------
static void SetCommonWork( RRL_WORK* work, RRC_WORK* commonWork )
{
  work->commonWork = commonWork;
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
static int GetNextTopicID( const RRL_WORK* work, int topicID )
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
static int GetPrevTopicID( const RRL_WORK* work, int topicID )
{
  return (topicID - 1 + GetSelectableTopicNum(work)) % GetSelectableTopicNum(work);
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I���\�Ȓ������ڂ̐����擾����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static u8 GetSelectableTopicNum( const RRL_WORK* work )
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
static u8 GetSelectedTopicID( const RRL_WORK* work )
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
static void SetSelectedTopicID( RRL_WORK* work, u8 topicID )
{
  work->selectedTopicID = topicID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu�ɂ��钲�����ڂ̑I�����L�����Z������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ResetSelectedTopicID( RRL_WORK* work )
{
  work->selectedTopicID = TOPIC_ID_DUMMY;
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
static BOOL IsTopicIDSelected( const RRL_WORK* work )
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
static BOOL CheckTopicCanSelect( const RRL_WORK* work, u8 topicID )
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
 * @brief OBJ ���\�[�X�̓o�^�C���f�b�N�X���擾����
 *
 * @param work
 * @param resID ���\�[�XID
 *
 * @return �w�肵�����\�[�X�̓o�^�C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
static u32 GetObjResourceRegisterIndex( const RRL_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RRL_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RRL_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
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
static void BmpOamSetDrawEnable( RRL_WORK* work, BMPOAM_ACTOR_INDEX actorIdx, BOOL enable )
{
  GF_ASSERT( actorIdx < BMPOAM_ACTOR_NUM );

  // �\����Ԃ�ύX
  BmpOam_ActorSetDrawEnable( work->BmpOamActor[ actorIdx ], enable );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j���[���ڂɑΉ�����BMP-OAM�A�N�^�[���擾����
 * 
 * @param work
 * @param menuItem ���j���[���ڂ̃C���f�b�N�X
 *
 * @return �w�肵�����j���[���ڂɑΉ�����BMP-OAM �A�N�^�[
 */
//-----------------------------------------------------------------------------------------
static BMPOAM_ACT_PTR GetBmpOamActorOfMenuItem( const RRL_WORK* work, MENU_ITEM menuItem )
{
  BMPOAM_ACTOR_INDEX BmpOamActorIdx;

  BmpOamActorIdx = BmpOamIndexOfMenu[ menuItem ];
  return work->BmpOamActor[ BmpOamActorIdx ];
}


//=========================================================================================
// ��LAYER 1 �����E�������E�����E��Еt���E�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʊǗ����[�N�𐶐�����
 *
 * @param heapID
 *
 * @return ���X�g��ʊǗ����[�N
 */
//-----------------------------------------------------------------------------------------
static RRL_WORK* CreateListWork( HEAPID heapID )
{
  RRL_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(RRL_WORK) );

  return work;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʊǗ����[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitListWork( RRL_WORK* work )
{
  work->state                = RRL_STATE_SETUP;
  work->stateCount           = 0;
  work->stateEndFlag         = FALSE;
  work->waitFrame            = WAIT_FRAME_BUTTON;
  work->menuCursorPos        = MENU_ITEM_DETERMINATION_OK;
  work->topicCursorPos       = 0;
  work->topicCursorNextPos   = 0;
  work->selectableTopicNum   = 0;
  work->selectedTopicID      = TOPIC_ID_DUMMY;
  work->VBlankTCBSystem      = GFUser_VIntr_GetTCBSYS();
  work->scrollCursorPos      = MIN_SCROLL_CURSOR_POS;
  work->scrollCursorPos_prev = MIN_SCROLL_CURSOR_POS;
  work->scrollStartPos       = 0;
  work->scrollEndPos         = 0;
  work->scrollFrames         = 0;
  work->scrollFrameCount     = 0;
  work->palFadeOutFlag       = FALSE;
  work->finishResult         = RRL_RESULT_TO_TOP;
  work->finishFlag           = FALSE;
  work->VBlankTask           = NULL;

  InitStateQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitOBJResources( work );
  InitPaletteFadeSystem( work );
  InitBitmapDatas( work );
  InitPaletteAnime( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���X�g��ʊǗ����[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteListWork( RRL_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �I���\�Ȓ������ڂ̐����Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSelectableTopicNum( RRL_WORK* work )
{
  GAMEDATA* gameData;
  EVENTWORK* evwork;
  int num;

  gameData = GetGameData( work );
  evwork   = GAMEDATA_GetEventWork( gameData );

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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h�����쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateFont( RRL_WORK* work )
{
  GF_ASSERT( work->font == NULL );

  // ����
  work->font = GFL_FONT_Create( 
      ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heapID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h��������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitFont( RRL_WORK* work )
{
  work->font = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�n���h����j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteFont( RRL_WORK* work )
{
  GF_ASSERT( work->font );

  GFL_FONT_Delete( work->font );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateMessages( RRL_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] == NULL );

    work->message[ msgIdx ] = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, MessageDataID[ msgIdx ], work->heapID ); 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitMessages( RRL_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    work->message[ msgIdx ] = NULL;
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteMessages( RRL_WORK* work )
{
  int msgIdx;

  for( msgIdx=0; msgIdx < MESSAGE_NUM; msgIdx++ )
  {
    GF_ASSERT( work->message[ msgIdx ] );

    GFL_MSG_Delete( work->message[ msgIdx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateStateQueue( RRL_WORK* work )
{
  GF_ASSERT( work->stateQueue == NULL );

  work->stateQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitStateQueue( RRL_WORK* work )
{
  work->stateQueue = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteStateQueue( RRL_WORK* work )
{
  GF_ASSERT( work->stateQueue );

  QUEUE_Delete( work->stateQueue );
}

//-----------------------------------------------------------------------------------------
/**
 * @breif �^�b�`�̈�̏������s��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupTouchArea( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBG( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBG( RRL_WORK* work )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( MAIN_BG_BAR );
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
static void SetupSubBG_WINDOW( RRL_WORK* work )
{
  ARCHANDLE* handle;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, GetHeapLowID(work->heapID) ); 

  // �X�N���[���f�[�^
  {
    void* src;
    ARCDATID datID;
    NNSG2dScreenData* data;
    datID = NARC_research_radar_graphic_bgu_win1_NSCR;
    src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
    NNS_G2dGetUnpackedScreenData( src, &data );
    GFL_BG_WriteScreen( SUB_BG_WINDOW, data->rawData, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( SUB_BG_WINDOW );
    GFL_HEAP_FreeMemory( src );
  }

  // �n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RRL_WORK* work )
{
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RRL_WORK* work )
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
static void CleanUpSubBG_FONT( RRL_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_BAR( RRL_WORK* work )
{
  ARCHANDLE* handle;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, GetHeapLowID(work->heapID) ); 

  // �X�N���[���f�[�^
  {
    void* src;
    ARCDATID datID;
    NNSG2dScreenData* data;
    datID = NARC_research_radar_graphic_bgd_title_NSCR;
    src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, work->heapID );
    NNS_G2dGetUnpackedScreenData( src, &data );
    GFL_BG_WriteScreen( MAIN_BG_BAR, data->rawData, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( MAIN_BG_BAR );
    GFL_HEAP_FreeMemory( src );
  }

  // �n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �o�[BG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_BAR( RRL_WORK* work )
{
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_WINDOW( RRL_WORK* work )
{
  ARCHANDLE* handle;

  // �n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, GetHeapLowID(work->heapID) ); 

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
    GFL_BG_LoadScreenV_Req( MAIN_BG_WINDOW );
    GFL_HEAP_FreeMemory( src );
  }

  // �n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RRL_WORK* work )
{
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RRL_WORK* work )
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
static void CleanUpMainBG_FONT( RRL_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void CreateBGFonts( RRL_WORK* work )
{
  int i;

  // �ʏ��BGFont
  for( i=0; i<BG_FONT_NUM; i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->BGFont_string[i] == NULL ); 

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
    work->BGFont_string[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetMessage( work->BGFont_string[i], strID );
  } 

  // ��������BGFont
   for( i=0; i < GetSelectableTopicNum(work); i++ )
  {
    BG_FONT_PARAM param;
    GFL_MSGDATA* msgData;
    u32 strID;

    GF_ASSERT( work->BGFont_topic[i] == NULL ); 

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
    work->BGFont_topic[i] = BG_FONT_Create( &param, work->font, msgData, work->heapID );

    // �������ݒ�
    BG_FONT_SetMessage( work->BGFont_topic[i], strID );
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitBGFonts( RRL_WORK* work )
{
  int idx;

  // �ʏ��BGFont
  for( idx=0; idx < BG_FONT_NUM; idx++ )
  {
    work->BGFont_string[ idx ] = NULL; 
  }
  // ��������BGFont
  for( idx=0; idx < TOPIC_ID_NUM; idx++ )
  {
    work->BGFont_topic[ idx ] = NULL; 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void DeleteBGFonts( RRL_WORK* work )
{
  int i;
  
  // �ʏ��BGFont
  for( i=0; i < BG_FONT_NUM; i++ )
  {
    GF_ASSERT( work->BGFont_string[i] );
    BG_FONT_Delete( work->BGFont_string[i] );
    work->BGFont_string[i] = NULL;
  }
  // ��������BGFont
  for( i=0; i < GetSelectableTopicNum(work); i++ )
  {
    GF_ASSERT( work->BGFont_topic[i] );
    BG_FONT_Delete( work->BGFont_topic[i] );
    work->BGFont_topic[i] = NULL;
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitOBJResources( RRL_WORK* work )
{
  int i;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ )
  {
    work->objResRegisterIdx[i] = 0; 
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterSubObjResources( RRL_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, GetHeapLowID(heapID) );

  // ���\�[�X��o�^
  character = GFL_CLGRP_CGR_Register( 
      arcHandle, NARC_research_radar_graphic_obj_NCGR, FALSE, CLSYS_DRAW_SUB, heapID ); 

  palette = GFL_CLGRP_PLTT_Register( 
      arcHandle, NARC_research_radar_graphic_obj_NCLR, CLSYS_DRAW_SUB, 0, heapID );

  cellAnime = GFL_CLGRP_CELLANIM_Register( 
      arcHandle, NARC_research_radar_graphic_obj_NCER, NARC_research_radar_graphic_obj_NANR, heapID ); 

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
static void ReleaseSubObjResources( RRL_WORK* work )
{
  GFL_CLGRP_CGR_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void RegisterMainObjResources( RRL_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, palette3, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, GetHeapLowID(heapID) );

  character = GFL_CLGRP_CGR_Register( 
      arcHandle, NARC_research_radar_graphic_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID ); 

  palette = GFL_CLGRP_PLTT_RegisterEx( 
      arcHandle, NARC_research_radar_graphic_obj_NCLR, CLSYS_DRAW_MAIN, ONE_PALETTE_SIZE*6, 0, 4, heapID ); 

  cellAnime = GFL_CLGRP_CELLANIM_Register( 
      arcHandle, NARC_research_radar_graphic_obj_NCER, NARC_research_radar_graphic_obj_NANR, heapID ); 

  GFL_ARC_CloseDataHandle( arcHandle );

  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, heapID ); 

  palette3 = GFL_CLGRP_PLTT_RegisterEx( 
      arcHandle, NARC_app_menu_common_task_menu_NCLR, CLSYS_DRAW_MAIN, ONE_PALETTE_SIZE*4, 0, 2, heapID );

  GFL_ARC_CloseDataHandle( arcHandle );

  // �o�^�C���f�b�N�X���L��
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ]  = character;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ]    = palette;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] = cellAnime;
  work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON ] = palette3;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseMainObjResources( RRL_WORK* work )
{
  GFL_CLGRP_CGR_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );
  GFL_CLGRP_PLTT_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void InitClactUnits( RRL_WORK* work )
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
static void CreateClactUnits( RRL_WORK* work )
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
static void DeleteClactUnits( RRL_WORK* work )
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
static void InitClactWorks( RRL_WORK* work )
{
  int wkIdx;

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
static void CreateClactWorks( RRL_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    GFL_CLUNIT* unit;
    GFL_CLWK_DATA wkData;
    u32 charaIdx, paletteIdx, cellAnimeIdx;
    u16 surface;

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
static void DeleteClactWorks( RRL_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < CLWK_NUM; wkIdx++ )
  {
    GF_ASSERT( work->clactWork[ wkIdx ] );

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
static void InitBitmapDatas( RRL_WORK* work )
{
  int idx;

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
static void CreateBitmapDatas( RRL_WORK* work )
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
static void SetupBitmapData_forDefault( RRL_WORK* work )
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
 * @brief�u�����Ă��v�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forOK( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief�u��߂�v�{�^���p�̃r�b�g�}�b�v�f�[�^����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupBitmapData_forCANCEL( RRL_WORK* work )
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
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �r�b�g�}�b�v�f�[�^��j������
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void DeleteBitmapDatas( RRL_WORK* work )
{
  int idx;

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
static void SetupBmpOamSystem( RRL_WORK* work )
{
  work->BmpOamSystem = BmpOam_Init( work->heapID, work->clactUnit[ CLUNIT_BMPOAM ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �V�X�e���̌�Еt�����s��
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CleanUpBmpOamSystem( RRL_WORK* work )
{
  BmpOam_Exit( work->BmpOamSystem );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BMP-OAM �A�N�^�[���쐬����
 *
 * @parma work
 */
//-----------------------------------------------------------------------------------------
static void CreateBmpOamActors( RRL_WORK* work )
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
static void DeleteBmpOamActors( RRL_WORK* work )
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
static void InitPaletteFadeSystem( RRL_WORK* work )
{ 
  work->paletteFadeSystem = NULL;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RRL_WORK* work )
{
  u32 tcbWorkSize;

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
static void CleanUpPaletteFadeSystem( RRL_WORK* work )
{ 
  // �p���b�g�t�F�[�h�̃��N�G�X�g���[�N��j��
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->paletteFadeSystem, FADE_MAIN_OBJ );

  // �t�F�[�h�Ǘ��V�X�e����j��
  PaletteFadeFree( work->paletteFadeSystem );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreatePaletteAnime( RRL_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL );

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RRL_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeletePaletteAnime( RRL_WORK* work )
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
static void SetupPaletteAnime( RRL_WORK* work )
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
static void CleanUpPaletteAnime( RRL_WORK* work )
{
  int idx;

  for( idx=0; idx < PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�N���[���o�[���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupScrollBar( RRL_WORK* work )
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �ʐM�A�C�R�����Z�b�g�A�b�v����
 * 
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void SetupWirelessIcon( const RRL_WORK* work )
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
static void RegisterVBlankTask( RRL_WORK* work )
{
  GF_ASSERT( work->VBlankTask == NULL );

  work->VBlankTask = GFUser_VIntr_CreateTCB( VBlankFunc, work, 0 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief VBlank���Ԓ��̃^�X�N����������
 *
 * @param work
 */
//-----------------------------------------------------------------------------------------
static void ReleaseVBlankTask( RRL_WORK* work )
{ 
  GF_ASSERT( work->VBlankTask );

  GFL_TCB_DeleteTask( work->VBlankTask );
  work->VBlankTask = NULL;
} 
