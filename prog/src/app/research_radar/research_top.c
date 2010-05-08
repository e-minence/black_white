//////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �g�b�v���
 * @file   research_top.c
 * @author obata
 * @date   2010.02.03
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "bg_font.h"
#include "queue.h"
#include "palette_anime.h"
#include "research_top.h"
#include "research_top_index.h"
#include "research_top_def.h"
#include "research_top_data.cdat"
#include "research_common.h"
#include "research_common_data.cdat"

#include "system/main.h"            // for HEAPID_xxxx
#include "system/gfl_use.h"         // for GFUser_xxxx
#include "print/gf_font.h"          // for GFL_FONT_xxxx
#include "print/printsys.h"         // for PRINTSYS_xxxx
#include "gamesystem/game_beacon.h" // for GAMEBEACON_xxxx
#include "sound/pm_sndsys.h"        // for PMSND_xxxx
#include "sound/wb_sound_data.sadl" // for SEQ_SE_XXXX

#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_research_radar_xxxx
#include "arc/font/font.naix"               // for NARC_font_xxxx
#include "arc/message.naix"                 // for NARC_message_xxxx
#include "msg/msg_research_radar.h"         // for str_xxxx
#include "obj_NANR_LBLDEFS.h"               // for NANR_obj_xxxx

#include "../../../../resource/fldmapdata/flagwork/flag_define.h" // for FE_xxxx


//====================================================================================
// ���g�b�v��ʊǗ����[�N
//====================================================================================
struct _RESEARCH_RADAR_TOP_WORK { 

  RESEARCH_COMMON_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID heapID;

  GFL_FONT*    font;
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  QUEUE*    stateQueue; // ��ԃL���[
  RRT_STATE state;      // �������
  u32       stateSeq;   // ��ԓ��V�[�P���X�ԍ�
  u32       stateCount; // ��ԃJ�E���^
  u32       waitFrame;  // WAIT ��Ԃł̑҂��t���[����

  MENU_ITEM cursorPos; // �J�[�\���ʒu

  GFL_UI_TP_HITTBL touchHitTable[ TOUCH_AREA_NUM ]; // �^�b�`�̈�
  PALETTE_ANIME*   paletteAnime[ PALETTE_ANIME_NUM ]; // �p���b�g�A�j���[�V����
  BG_FONT*         BGFont[ BG_FONT_NUM ]; // ������`��I�u�W�F�N�g

  // OBJ
  u32         objResRegisterIdx[ OBJ_RESOURCE_NUM ];  // ���\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ CLUNIT_NUM ];                // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ];                  // �Z���A�N�^�[���[�N

  // �t���O
  BOOL nowStateEndFlag; // ���݂̏�Ԃ��I���������ǂ���
  BOOL newEntryFlag;  // �V�����l���Ƃ����������ǂ���
  BOOL finishFlag;    // �g�b�v��ʏI���t���O
  RRT_RESULT finishResult; // �I������
};


//====================================================================================
// ���֐��C���f�b�N�X
//====================================================================================
//------------------------------------------------------------------------------------
// ��LAYER 4 ���
//------------------------------------------------------------------------------------
// ��ԏ���
static void Main_SETUP( RRT_WORK* work ); // RRT_STATE_SETUP
static void Main_STANDBY( RRT_WORK* work ); // RRT_STATE_STANDBY
static void Main_KEYWAIT( RRT_WORK* work ); // RRT_STATE_KEYWAIT
static void Main_WAIT( RRT_WORK* work ); // RRT_STATE_WAIT
static void Main_FADEOUT( RRT_WORK* work ); // RRT_STATE_FADEOUT
static void Main_CLEANUP( RRT_WORK* work ); // RRT_STATE_CLEANUP 
// ��Ԑ���
static void RegisterNextState( RRT_WORK* work, RRT_STATE nextState ); // ���̏�Ԃ��L���[�ɓo�^����
static void FinishNowState( RRT_WORK* work ); // ���݂̏�Ԃ��I������
static void SwitchState( RRT_WORK* work ); // ������Ԃ�ύX����
static void SetState( RRT_WORK* work, RRT_STATE nextState ); // ������Ԃ�ݒ肷��
static RRT_STATE GetFirstState( const RRT_WORK* work ); // �ŏ��̏�Ԃ��擾����
static u32 GetStateSeq( const RRT_WORK* work ); // ��ԓ��V�[�P���X�ԍ����擾����
static void IncStateSeq( RRT_WORK* work ); // ��ԓ��V�[�P���X�ԍ����C���N�������g����
static void SetFinishReason( const RRT_WORK* work, SEQ_CHANGE_TRIG reason ); // �g�b�v��ʂ̏I�����R��o�^����
static void SetFinishResult( RRT_WORK* work, RRT_RESULT result ); // �I�����ʂ�ݒ肷��
//------------------------------------------------------------------------------------
// ��LAYER 3 �@�\
//------------------------------------------------------------------------------------
// �f�[�^�X�V
static void CheckNewEntry( RRT_WORK* work ); // �V�����l���Ƃ����������ǂ������`�F�b�N����
// �J�[�\���ړ�
static void MoveCursorUp( RRT_WORK* work ); // �J�[�\������Ɉړ�����
static void MoveCursorDown( RRT_WORK* work ); // �J�[�\�������Ɉړ�����
static void MoveCursorDirect( RRT_WORK* work, MENU_ITEM menuItem ); // �J�[�\���𒼐ڈړ�����
// ���j���[���ڃ{�^��
static void MenuItemSetCursorOn( MENU_ITEM menuItem ); // �J�[�\��������Ă����Ԃɂ���
static void MenuItemSetCursorOff( MENU_ITEM menuItem ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetChangeButtonNotActive( RRT_WORK* work ); //�w���������߂�x�{�^�����A�N�e�B�u��Ԃɂ���
//�w�߂�x�{�^��
static void BlinkReturnButton( RRT_WORK* work ); //�w�߂�x�{�^���𖾖ł�����
// "new" �A�C�R��
static void NewIconDispOn( const RRT_WORK* work ); // "new" �A�C�R����\������
static void NewIconDispOff( const RRT_WORK* work ); // "new" �A�C�R�����\���ɂ���
// ��ʂ̃t�F�[�h�C���E�t�F�[�h�A�E�g
static void StartFadeIn( void ); // �t�F�[�h�C�����J�n����
static void StartFadeOut( void ); // �t�F�[�h�A�E�g���J�n����
//------------------------------------------------------------------------------------
// ��LAYER 2 �擾�E�ݒ�E����
//------------------------------------------------------------------------------------
// �e��J�E���^
static void CountUpStateCount( RRT_WORK* work ); // ��ԃJ�E���^���X�V����
static u32 GetStateCount( const RRT_WORK* work ); // ��ԃJ�E���^�̒l���擾����
static void ResetStateCount( RRT_WORK* work ); // ��ԃJ�E���^�����Z�b�g����
static void SetWaitFrame( RRT_WORK* work, u32 frame ); // WAIT ��Ԃ̑҂����Ԃ�ݒ肷��
static u32 GetWaitFrame( const RRT_WORK* work ); // WAIT ��Ԃ̑҂����Ԃ��擾����
// �J�[�\���ʒu
static void ShiftCursorPos( RRT_WORK* work, int offset ); // �J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetCursorPos( RRT_WORK* work, MENU_ITEM menuItem ); // �J�[�\���ʒu��ύX���� ( ���l�w�� )
static BOOL CheckCursorSetEnable( RRT_WORK* work, MENU_ITEM menuItem ); // �J�[�\�����w��ʒu�ɃZ�b�g�ł��邩�ǂ������`�F�b�N����
// OBJ
static u32 GetOBJResRegisterIndex( const RRT_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
static GFL_CLUNIT* GetClactUnit( const RRT_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RRT_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
// �p���b�g�A�j���[�V����
static void StartPaletteAnime( RRT_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RRT_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void UpdatePaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
// ��������
static u8 GetSelectableTopicNum( const RRT_WORK* work ); // �I���\�Ȓ������ڂ̐����擾����
// �S��ʋ��ʃ��[�N
static RESEARCH_COMMON_WORK* GetCommonWork( const RRT_WORK* work ); // �S��ʋ��ʃ��[�N���擾����
static void SetCommonWork( RRT_WORK* work, RESEARCH_COMMON_WORK* commonWork ); // �S��ʋ��ʃ��[�N��ݒ肷��
// �q�[�vID
static HEAPID GetHeapID( const RRT_WORK* work ); // �q�[�vID���擾����
static void SetHeapID( RRT_WORK* work, HEAPID heapID ); // �q�[�vID��ݒ肷��
//------------------------------------------------------------------------------------
// ��LAYER 1 �����E�������E�j��
//------------------------------------------------------------------------------------
// �g�b�v��ʃ��[�N
static RRT_WORK* CreateTopWork( HEAPID heapID ); // �g�b�v��ʃ��[�N�𐶐�����
static void DeleteTopWork( RRT_WORK* work ); // �g�b�v��ʃ��[�N��j������
static void InitTopWork( RRT_WORK* work ); // �g�b�v��ʃ��[�N������������
// �J�[�\���ʒu
static void SetupCursorPos( RRT_WORK* work ); // �J�[�\���ʒu���Z�b�g�A�b�v����
// �t�H���g
static void InitFont( RRT_WORK* work ); // �t�H���g�f�[�^������������
static void CreateFont( RRT_WORK* work ); // �t�H���g�f�[�^�𐶐�����
static void DeleteFont( RRT_WORK* work ); // �t�H���g�f�[�^��j������
// ���b�Z�[�W
static void InitMessages( RRT_WORK* work ); // ���b�Z�[�W�f�[�^������������
static void CreateMessages( RRT_WORK* work ); // ���b�Z�[�W�f�[�^�𐶐�����
static void DeleteMessages( RRT_WORK* work ); // ���b�Z�[�W�f�[�^��j������
// �^�b�`�̈�
static void SetupTouchArea( RRT_WORK* work ); // �^�b�`�̈���Z�b�g�A�b�v����
// ��ԃL���[
static void InitStateQueue( RRT_WORK* work ); // ��ԃL���[������������
static void CreateStateQueue( RRT_WORK* work ); // ��ԃL���[�𐶐�����
static void DeleteStateQueue( RRT_WORK* work ); // ��ԃL���[��j������
// BG
static void SetupBG( RRT_WORK* work ); // BG�S�ʂ̃Z�b�g�A�b�v���s��
static void SetupSubBG_WINDOW( RRT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) �̃Z�b�g�A�b�v���s��
static void SetupSubBG_FONT( RRT_WORK* work ); // SUB-BG ( �t�H���g�� ) �̃Z�b�g�A�b�v���s��
static void SetupMainBG_WINDOW( RRT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) �̃Z�b�g�A�b�v���s��
static void SetupMainBG_FONT( RRT_WORK* work ); // MAIN-BG ( �t�H���g�� ) �̃Z�b�g�A�b�v���s��
static void CleanUpBG( RRT_WORK* work ); // BG�S�ʂ̃N���[���A�b�v���s��
static void CleanUpSubBG_WINDOW( RRT_WORK* work ); // SUB-BG ( �E�B���h�E�� ) �̃N���[���A�b�v���s��
static void CleanUpSubBG_FONT( RRT_WORK* work ); // SUB-BG ( �t�H���g�� ) �̃N���[���A�b�v���s��
static void CleanUpMainBG_WINDOW( RRT_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) �̃N���[���A�b�v���s��
static void CleanUpMainBG_FONT( RRT_WORK* work ); // MAIN-BG ( �t�H���g�� ) �̃N���[���A�b�v���s��
// ������`��I�u�W�F�N�g
static void InitBGFonts( RRT_WORK* work ); // ������`��I�u�W�F�N�g������������
static void CreateBGFonts( RRT_WORK* work ); // ������`��I�u�W�F�N�g�𐶐�����
static void DeleteBGFonts( RRT_WORK* work ); // ������`��I�u�W�F�N�g��j������
// OBJ ���\�[�X
static void InitOBJResources( RRT_WORK* work ); // OBJ���\�[�X������������
static void RegisterSubOBJResources( RRT_WORK* work ); // OBJ���\�[�X��o�^���� ( SUB-OBJ )
static void RegisterMainOBJResources( RRT_WORK* work ); // OBJ���\�[�X��o�^���� ( MAIN-OBJ )
static void ReleaseSubOBJResources( RRT_WORK* work ); // OBJ���\�[�X�̓o�^���������� ( SUB-OBJ )
static void ReleaseMainOBJResources( RRT_WORK* work ); // OBJ���\�[�X�̓o�^���������� ( MAIN-OBJ )
// �Z���A�N�^�[���j�b�g
static void InitClactUnits( RRT_WORK* work ); // �Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RRT_WORK* work ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RRT_WORK* work ); // �Z���A�N�^�[���j�b�g��j������
// �Z���A�N�^�[���[�N
static void InitClactWorks( RRT_WORK* work ); // �Z���A�N�^�[���[�N������������
static void CreateClactWorks( RRT_WORK* work ); // �Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RRT_WORK* work ); // �Z���A�N�^�[���[�N��j������
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RRT_WORK* work ); // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
// VBlank ���荞��
static void SetVBlankFunc( RRT_WORK* work ); // VBlank ���荞�݂�ݒ�
static void ResetVBlankFunc( RRT_WORK* work ); // VBlank ���荞�݂�����
// �ʐM�A�C�R��
static void SetupWirelessIcon( const RRT_WORK* work ); // �ʐM�A�C�R�����Z�b�g�A�b�v����
//------------------------------------------------------------------------------------
// ��LAYER 0 �f�o�b�O
//------------------------------------------------------------------------------------
static void DebugPrint_stateQueue( const RRT_WORK* work ); // ��ԃL���[���o�͂���



//====================================================================================
// ��public functions
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʃ��[�N�𐶐�����
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return ���������g�b�v��ʃ��[�N
 */
//------------------------------------------------------------------------------------
RRT_WORK* RRT_CreateWork( RESEARCH_COMMON_WORK* commonWork )
{
  RRT_WORK* work;
  HEAPID heapID;

  heapID = RESEARCH_COMMON_GetHeapID( commonWork );

  // ���[�N�𐶐�
  work = CreateTopWork( heapID );

  // ���[�N��������
  InitTopWork( work ); 
  SetHeapID( work, heapID );
  SetCommonWork( work, commonWork );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: RRT_CreateWork\n" );

  return work;
}

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʃ��[�N�̔j��
 *
 * @param heapID
 */
//------------------------------------------------------------------------------------
void RRT_DeleteWork( RRT_WORK* work )
{
  DeleteStateQueue( work );
  DeleteTopWork( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: RRT_DeleteWork\n" );
} 

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��� ���C������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
void RRT_Main( RRT_WORK* work )
{
  // ��Ԃ��Ƃ̏���
  switch( work->state ) {
  case RRT_STATE_SETUP:    Main_SETUP( work );    break;
  case RRT_STATE_STANDBY:  Main_STANDBY( work );  break;
  case RRT_STATE_KEYWAIT:  Main_KEYWAIT( work );  break;
  case RRT_STATE_WAIT:     Main_WAIT( work );     break;
  case RRT_STATE_FADEOUT:  Main_FADEOUT( work );  break;
  case RRT_STATE_CLEANUP:  Main_CLEANUP( work );  break;
  case RRT_STATE_FINISH:   return;
  default:  GF_ASSERT(0);
  }

  CheckNewEntry( work );
  UpdatePaletteAnime( work );
  RESEARCH_COMMON_UpdatePaletteAnime( work->commonWork );
  GFL_CLACT_SYS_Main();

  // ��Ԃ̍X�V
  CountUpStateCount( work );
  SwitchState( work );
}

//------------------------------------------------------------------------------------
/**
 * @breif �I������
 *
 * @param work
 *
 * @return �g�b�v��ʂ��I�������ꍇ TRUE
 */
//------------------------------------------------------------------------------------
BOOL RRT_IsFinished( const RRT_WORK* work )
{
  if( work->finishFlag ) {
    return TRUE;
  }
  else { 
    return FALSE;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �I�����ʂ̎擾
 *
 * @param work
 *
 * @return �g�b�v��ʂ̏I������
 */
//------------------------------------------------------------------------------------
RRT_RESULT RRT_GetResult( const RRT_WORK* work )
{
  GF_ASSERT( work->finishFlag );

  return work->finishResult;
}


//====================================================================================
// ��private functions
//====================================================================================

//====================================================================================
// ��LAYER 4 ���
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ������� ( RRT_STATE_SETUP ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_SETUP( RRT_WORK* work )
{
  // �f�[�^����
  CreateStateQueue( work );
  CreateFont( work );
  CreateMessages( work );
  SetupTouchArea( work );
  SetupCursorPos( work );

  // BG ����
  SetupBG           ( work );
  SetupSubBG_WINDOW ( work );
  SetupSubBG_FONT   ( work );
  SetupMainBG_WINDOW( work );
  SetupMainBG_FONT  ( work );

  // ������`��I�u�W�F�N�g ����
  CreateBGFonts( work );

  // OBJ ����
  RegisterSubOBJResources( work );
  RegisterMainOBJResources( work );
  CreateClactUnits( work );
  CreateClactWorks( work );

  // �p���b�g�A�j���[�V����
  CreatePaletteAnime( work );
  SetupPaletteAnime( work );

  // �ʐM�A�C�R��
  SetupWirelessIcon( work );

  // VBlank ���荞��
  SetVBlankFunc( work );

  //�w���������߂�x�{�^�����A�N�e�B�u��Ԃɂ���
  if( CheckCursorSetEnable( work, MENU_ITEM_CHANGE_RESEARCH ) == FALSE ) {
    SetChangeButtonNotActive( work );
  }

  // ���̏�Ԃ�ݒ�
  {
    RRT_STATE next_state;

    next_state = GetFirstState( work );
    RegisterNextState( work, GetFirstState(work) ); // ���̏�Ԃ��Z�b�g

    if( next_state == RRT_STATE_KEYWAIT ) {
      MenuItemSetCursorOn( work->cursorPos ); // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
      StartPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j�����J�n
    } 
  }

  // ��ʃt�F�[�h�C���J�n
  StartFadeIn();

  // ��ԏI��
  FinishNowState( work ); 
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[�҂���� ( RRT_STATE_STANDBY ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_STANDBY( RRT_WORK* work )
{
  int trg;
  int touch;
  int commonTouch;

  trg = GFL_UI_KEY_GetTrg();
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //-----------------
  //�u���ǂ�v�{�^�� 
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );  // ��ʑJ�ڂ̃g���K��o�^
    SetFinishResult( work, RRT_RESULT_EXIT );      // ��ʏI�����ʂ�����
    BlinkReturnButton( work );                     //�w�߂�x�{�^�������点��
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                // �L�����Z����
    SetWaitFrame( work, WAIT_FRAME_BUTTON );       // �҂����Ԃ�ݒ�
    FinishNowState( work );                        // RRT_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_WAIT );     // => RRT_STATE_WAIT
    RegisterNextState( work, RRT_STATE_FADEOUT );  // ==> RRT_STATE_FADEOUT
    RegisterNextState( work, RRT_STATE_CLEANUP );  // ===> RRT_STATE_CLEANUP
    return;
  }

  //----
  // B
  if( trg & PAD_BUTTON_B ) {
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON ); // ��ʑJ�ڂ̃g���K��o�^
    SetFinishResult( work, RRT_RESULT_EXIT );      // ��ʏI�����ʂ�����
    BlinkReturnButton( work );                     //�w�߂�x�{�^�������点��
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                // �L�����Z����
    SetWaitFrame( work, WAIT_FRAME_BUTTON );       // �҂����Ԃ�ݒ�
    FinishNowState( work );                        // RRT_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_WAIT );     // => RRT_STATE_WAIT
    RegisterNextState( work, RRT_STATE_FADEOUT );  // ==> RRT_STATE_FADEOUT
    RegisterNextState( work, RRT_STATE_CLEANUP );  // ===> RRT_STATE_CLEANUP
    return;
  }

  //----------------
  // �\���L�[ or A
  if( (trg & PAD_KEY_UP)   || (trg & PAD_KEY_DOWN)  ||
      (trg & PAD_KEY_LEFT) || (trg & PAD_KEY_RIGHT) || (trg & PAD_BUTTON_A) ) {
    MenuItemSetCursorOn( work->cursorPos );             // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
    StartPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j�����J�n
    FinishNowState( work );                             // RRT_STATE_STANDBY ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_KEYWAIT );       // => RRT_STATE_KEYWAIT
    return;
  }

  //--------------------------------
  //�w���������߂�x�{�^�����^�b�`
  if( touch == TOUCH_AREA_CHANGE_BUTTON ) {
    // �I����
    if( CheckCursorSetEnable(work, MENU_ITEM_CHANGE_RESEARCH) ) {
      SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );        // ��ʑJ�ڂ̃g���K��o�^
      SetFinishResult( work, RRT_RESULT_TO_LIST );         // ��ʏI�����ʂ�����
      MoveCursorDirect( work, MENU_ITEM_CHANGE_RESEARCH ); // �J�[�\���ʒu���X�V
      MenuItemSetCursorOn( work->cursorPos );              // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );   // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );     // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                      // ���艹
      SetWaitFrame( work, WAIT_FRAME_BUTTON );             // �҂����Ԃ�ݒ�
      FinishNowState( work );                              // RRT_STATE_STANDBY ��Ԃ��I��
      RegisterNextState( work, RRT_STATE_WAIT );           // => RRT_STATE_WAIT
      RegisterNextState( work, RRT_STATE_FADEOUT );        // ==> RRT_STATE_FADEOUT
      RegisterNextState( work, RRT_STATE_CLEANUP );        // ===> RRT_STATE_CLEANUP
      return;
    }
    // �I��s��
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }
  //------------------------------
  //�w����������x�{�^�����^�b�`
  if( touch == TOUCH_AREA_CHECK_BUTTON ) {
    // �I����
    if( CheckCursorSetEnable(work, MENU_ITEM_CHECK_RESEARCH) ) {
      SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );       // ��ʑJ�ڂ̃g���K��o�^
      SetFinishResult( work, RRT_RESULT_TO_GRAPH );       // ��ʏI�����ʂ�����
      MoveCursorDirect( work, MENU_ITEM_CHECK_RESEARCH ); // �J�[�\���ʒu���X�V
      MenuItemSetCursorOn( work->cursorPos );             // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );  // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );    // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                     // ���艹
      SetWaitFrame( work, WAIT_FRAME_BUTTON );            // �҂����Ԃ�ݒ�
      FinishNowState( work );                             // RRT_STATE_STANDBY ��Ԃ��I��
      RegisterNextState( work, RRT_STATE_WAIT );          // => RRT_STATE_WAIT
      RegisterNextState( work, RRT_STATE_FADEOUT );       // ==> RRT_STATE_FADEOUT
      RegisterNextState( work, RRT_STATE_CLEANUP );       // ===> RRT_STATE_CLEANUP
      return;
    }
    // �I��s��
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂���� ( RRT_STATE_KEYWAIT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_KEYWAIT( RRT_WORK* work )
{
  int trg;
  int touch;
  int commonTouch;

  trg = GFL_UI_KEY_GetTrg();
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //-----------------
  //�u���ǂ�v�{�^��
  if( commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON ) {
    SetFinishReason( work, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
    SetFinishResult( work, RRT_RESULT_EXIT );     // ��ʏI�����ʂ�����
    BlinkReturnButton( work );                    //�w�߂�x�{�^�������点��
    PMSND_PlaySE( SEQ_SE_CANCEL1 );               // �L�����Z����
    SetWaitFrame( work, WAIT_FRAME_BUTTON );      // �҂����Ԃ�ݒ�
    FinishNowState( work );                       // RRT_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_WAIT );    // => RRT_STATE_WAIT
    RegisterNextState( work, RRT_STATE_FADEOUT ); // ==> RRT_STATE_FADEOUT
    RegisterNextState( work, RRT_STATE_CLEANUP ); // ===> RRT_STATE_CLEANUP
    return;
  }

  //--------
  // �� �L�[
  if( trg & PAD_KEY_UP ) {
    MoveCursorUp( work ); // �J�[�\�����ړ�
  } 
  //--------
  // �� �L�[
  if( trg & PAD_KEY_DOWN ) {
    MoveCursorDown( work ); // �J�[�\�����ړ�
  }

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    SetFinishReason( work, SEQ_CHANGE_BY_BUTTON );     // ��ʑJ�ڂ̃g���K��o�^
    StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j���I��
    StartPaletteAnime( work, PALETTE_ANIME_SELECT );   // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                    // ���艹
    SetWaitFrame( work, WAIT_FRAME_BUTTON );           // �҂����Ԃ�ݒ�
    FinishNowState( work );                            // RRT_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_WAIT );         // => RRT_STATE_WAIT
    RegisterNextState( work, RRT_STATE_FADEOUT );      // ==> RRT_STATE_FADEOUT
    RegisterNextState( work, RRT_STATE_CLEANUP );      // ===> RRT_STATE_CLEANUP
    // ��ʏI�����ʂ�����
    switch( work->cursorPos ) { 
    case MENU_ITEM_CHANGE_RESEARCH: SetFinishResult( work, RRT_RESULT_TO_LIST );  break;
    case MENU_ITEM_CHECK_RESEARCH:  SetFinishResult( work, RRT_RESULT_TO_GRAPH ); break;
    default:                        SetFinishResult( work, RRT_RESULT_EXIT );     break;
    }
    return;
  }

  //-----------------------
  //�w���������߂�x�{�^��
  if( touch == TOUCH_AREA_CHANGE_BUTTON ) {
    // �I����
    if( CheckCursorSetEnable(work, MENU_ITEM_CHANGE_RESEARCH) ) {
      SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );         // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHANGE_RESEARCH );  // �J�[�\���ʒu���X�V
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );    // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );      // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                       // ���艹
      SetFinishResult( work, RRT_RESULT_TO_LIST );          // ��ʏI�����ʂ�����
      SetWaitFrame( work, WAIT_FRAME_BUTTON );              // �҂����Ԃ�ݒ�
      FinishNowState( work );                               // RRT_STATE_KEYWAIT ��Ԃ��I��
      RegisterNextState( work, RRT_STATE_WAIT );            // => RRT_STATE_WAIT
      RegisterNextState( work, RRT_STATE_FADEOUT );         // ==> RRT_STATE_FADEOUT       
      RegisterNextState( work, RRT_STATE_CLEANUP );         // ===> RRT_STATE_CLEANUP      
    }
    // �I��s��
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }

  //----------------------
  //�w����������x�{�^��
  if( touch == TOUCH_AREA_CHECK_BUTTON ) {
    // �I����
    if( CheckCursorSetEnable(work, MENU_ITEM_CHECK_RESEARCH) ) {
      SetFinishReason( work, SEQ_CHANGE_BY_TOUCH );       // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHECK_RESEARCH ); // �J�[�\���ʒu���X�V
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );  // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );    // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                     // ���艹
      SetFinishResult( work, RRT_RESULT_TO_GRAPH );       // ��ʏI�����ʂ�����
      SetWaitFrame( work, WAIT_FRAME_BUTTON );            // �҂����Ԃ�ݒ�
      FinishNowState( work );                             // RRT_STATE_KEYWAIT ��Ԃ��I��
      RegisterNextState( work, RRT_STATE_WAIT );          // => RRT_STATE_WAIT
      RegisterNextState( work, RRT_STATE_FADEOUT );       // ==> RRT_STATE_FADEOUT       
      RegisterNextState( work, RRT_STATE_CLEANUP );       // ===> RRT_STATE_CLEANUP      
    }
    // �I��s��
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    BlinkReturnButton( work );                    //�w�߂�x�{�^�������点��
    PMSND_PlaySE( SEQ_SE_CANCEL1 );               // �L�����Z����
    SetFinishResult( work, RRT_RESULT_EXIT );     // ��ʏI�����ʂ�����
    SetWaitFrame( work, WAIT_FRAME_BUTTON );      // �҂����Ԃ�ݒ�
    FinishNowState( work );                       // RRT_STATE_KEYWAIT ��Ԃ��I��
    RegisterNextState( work, RRT_STATE_WAIT );    // => RRT_STATE_WAIT
    RegisterNextState( work, RRT_STATE_FADEOUT ); // ==> RRT_STATE_FADEOUT       
    RegisterNextState( work, RRT_STATE_CLEANUP ); // ===> RRT_STATE_CLEANUP      
    return;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂���� ( RRT_STATE_WAIT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_WAIT( RRT_WORK* work )
{
  // �҂����Ԃ��o��
  if( GetWaitFrame(work) < GetStateCount(work) ) {
    FinishNowState( work );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt����� ( RRT_STATE_FADEOUT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_FADEOUT( RRT_WORK* work )
{
  switch( GetStateSeq(work) ) {
  // �t�F�[�h�A�E�g�J�n
  case 0:
    StartFadeOut(); 
    IncStateSeq( work );
    break;

  // �t�F�[�h�����҂�
  case 1:
    if( GFL_FADE_CheckFade() == FALSE ) {
      FinishNowState( work );
    } 
    break;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt����� ( RRT_STATE_CLEANUP ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_CLEANUP( RRT_WORK* work )
{
  ResetVBlankFunc ( work );

  // ���ʃp���b�g�A�j���[�V����
  RESEARCH_COMMON_StopAllPaletteAnime( work->commonWork ); // ��~����, 
  RESEARCH_COMMON_ResetAllPalette( work->commonWork );     // �p���b�g�����ɖ߂�

  // �p���b�g�A�j���[�V����
  CleanUpPaletteAnime( work );
  DeletePaletteAnime( work );

  // OBJ ��Еt��
  DeleteClactWorks( work );
  DeleteClactUnits( work );
  ReleaseSubOBJResources ( work );
  ReleaseMainOBJResources( work );

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

  work->finishFlag = TRUE;
  RegisterNextState( work, RRT_STATE_FINISH );
  FinishNowState( work );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���̏�Ԃ��L���[�ɓo�^����
 *
 * @param work
 * @param nextState �o�^������
 */
//------------------------------------------------------------------------------------
static void RegisterNextState( RRT_WORK* work, RRT_STATE nextState )
{
  // ��ԃL���[�ɒǉ�����
  QUEUE_EnQueue( work->stateQueue, nextState );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set next seq\n" );
  DebugPrint_stateQueue( work );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���݂̏�Ԃ��I������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishNowState( RRT_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->nowStateEndFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->nowStateEndFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish current sequence\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SwitchState( RRT_WORK* work )
{
  RRT_STATE now_seq, next_seq;

  if( work->nowStateEndFlag == FALSE ){ return; }  // ���݂̏�Ԃ��I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->stateQueue ) ){ return; } // ��ԃL���[�ɓo�^����Ă��Ȃ�

  now_seq  = work->state;
  next_seq = QUEUE_DeQueue( work->stateQueue );

  // ��Ԃ��X�V
  SetState( work, next_seq ); 

  // DEBUG: ��ԃL���[��\��
  DebugPrint_stateQueue( work );
} 

//------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ݒ肷��
 *
 * @param work
 * @parma nextState �ݒ肷����
 */
//------------------------------------------------------------------------------------
static void SetState( RRT_WORK* work, RRT_STATE nextState )
{ 
  // �X�V
  work->state           = nextState;
  work->stateSeq        = 0;
  work->stateCount      = 0;
  work->nowStateEndFlag = FALSE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set seq ==> " );
  switch( nextState ) {
  case RRT_STATE_SETUP:   OS_TFPrintf( PRINT_TARGET, "SETUP" );    break;
  case RRT_STATE_STANDBY: OS_TFPrintf( PRINT_TARGET, "STANDBY" );  break;
  case RRT_STATE_KEYWAIT: OS_TFPrintf( PRINT_TARGET, "KEYWAIT" );  break;
  case RRT_STATE_WAIT:    OS_TFPrintf( PRINT_TARGET, "WAIT" );     break;
  case RRT_STATE_FADEOUT: OS_TFPrintf( PRINT_TARGET, "FADEOUT" );  break;
  case RRT_STATE_CLEANUP: OS_TFPrintf( PRINT_TARGET, "CLEANUP" );  break;
  case RRT_STATE_FINISH:  OS_TFPrintf( PRINT_TARGET, "FINISH" );   break;
  default:  GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�ԍ����擾����
 *
 * @param work
 *
 * @return ��ԓ��V�[�P���X�ԍ�
 */
//------------------------------------------------------------------------------------
static u32 GetStateSeq( const RRT_WORK* work )
{
  return work->stateSeq;
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԓ��V�[�P���X�ԍ����C���N�������g����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void IncStateSeq( RRT_WORK* work )
{
  (work->stateSeq)++;
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̏�Ԃ��擾����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static RRT_STATE GetFirstState( const RRT_WORK* work )
{
  RESEARCH_COMMON_WORK* commonWork;
  RADAR_SEQ prev_seq;
  SEQ_CHANGE_TRIG trig;
  MENU_ITEM cursor_pos;

  commonWork = work->commonWork;
  prev_seq   = RESEARCH_COMMON_GetPrevSeq( commonWork );
  trig       = RESEARCH_COMMON_GetSeqChangeTrig( commonWork );

  // �O�̉�ʂ��{�^���ŏI��
  if( (prev_seq != RADAR_SEQ_NULL) && (trig == SEQ_CHANGE_BY_BUTTON) ) {
    return RRT_STATE_KEYWAIT;
  }
  else {
    return RRT_STATE_STANDBY;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʂ̏I�����R��o�^����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetFinishReason( const RRT_WORK* work, SEQ_CHANGE_TRIG reason )
{
  RESEARCH_COMMON_SetSeqChangeTrig( work->commonWork, reason );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result �I������
 */
//------------------------------------------------------------------------------------
static void SetFinishResult( RRT_WORK* work, RRT_RESULT result )
{
  work->finishResult = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish! result = %d\n", result );
}


//==================================================================================== 
// ��LAYER 3 �@�\
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �V�����l���Ƃ����������ǂ������`�F�b�N����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CheckNewEntry( RRT_WORK* work )
{
  if( work->newEntryFlag == FALSE ) {
    // �V�����l���Ƃ�������
    if( GAMEBEACON_Get_NewEntry() == TRUE ) {
      work->newEntryFlag = TRUE; // �t���O�𗧂Ă�
      NewIconDispOn( work );     // "new" �A�C�R����\������
    }
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\������ֈړ�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void MoveCursorUp( RRT_WORK* work )
{
  // ���ڂ̕\�����X�V
  MenuItemSetCursorOff( work->cursorPos );

  // �J�[�\���ʒu���X�V
  ShiftCursorPos( work, -1 );

  // ���ڂ̕\�����X�V
  MenuItemSetCursorOn( work->cursorPos );

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �p���b�g�A�j���[�V�������J�n
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor up\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\�������ֈړ�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void MoveCursorDown( RRT_WORK* work )
{
  // ���ڂ̕\�����X�V
  MenuItemSetCursorOff( work->cursorPos );

  // �J�[�\���ʒu���X�V
  ShiftCursorPos( work, 1 );

  // ���ڂ̕\�����X�V
  MenuItemSetCursorOn( work->cursorPos );

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �p���b�g�A�j���[�V�������J�n
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor down\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���𒼐ڈړ�����
 *
 * @param work
 * @param menuItem �ړ���̃��j���[����
 */
//------------------------------------------------------------------------------------
static void MoveCursorDirect( RRT_WORK* work, MENU_ITEM menuItem )
{
  // ���ڂ̕\�����X�V
  MenuItemSetCursorOff( work->cursorPos );

  // �J�[�\���ʒu���X�V
  SetCursorPos( work, menuItem );

  // ���ڂ̕\�����X�V
  MenuItemSetCursorOn( work->cursorPos );

  // �J�[�\���ړ���
  PMSND_PlaySE( SEQ_SE_SELECT1 );

  // �p���b�g�A�j���[�V�������J�n
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_SET );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: move cursor direct\n" );
}

//------------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ�I����Ԃɂ���
 *
 * @param menuItem �I����Ԃɂ��郁�j���[���ڂ��w��
 */
//------------------------------------------------------------------------------------
static void MenuItemSetCursorOn( MENU_ITEM menuItem )
{
  u8 BGFrame;
  u8 x, y, width, height;
  u8 paletteNo;

  // �p�����[�^������
  switch( menuItem ) {
  //�w���������߂�x
  case MENU_ITEM_CHANGE_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHANGE_BUTTON_X;
    y         = CHANGE_BUTTON_Y;
    width     = CHANGE_BUTTON_WIDTH;
    height    = CHANGE_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_ON;
    break;

  //�w����������x
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
    return;
  }

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, x, y, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//------------------------------------------------------------------------------------
/**
 * @biref ���j���[���ڂ��I����Ԃɂ���
 *
 * @param menuItem ��I����Ԃɂ��郁�j���[���ڂ��w��
 */
//------------------------------------------------------------------------------------
static void MenuItemSetCursorOff( MENU_ITEM menuItem )
{
  u8 BGFrame;
  u8 x, y, width, height;
  u8 paletteNo;

  // �p�����[�^������
  switch( menuItem ) {
  //�w���������߂�x
  case MENU_ITEM_CHANGE_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHANGE_BUTTON_X;
    y         = CHANGE_BUTTON_Y;
    width     = CHANGE_BUTTON_WIDTH;
    height    = CHANGE_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_OFF_CHANGE;
    break;

  //�w����������x
  case MENU_ITEM_CHECK_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHECK_BUTTON_X;
    y         = CHECK_BUTTON_Y;
    width     = CHECK_BUTTON_WIDTH;
    height    = CHECK_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_OFF_CHECK;
    break;

  // �G���[
  default:
    GF_ASSERT(0);
    return;
  }

  // �X�N���[���X�V
  GFL_BG_ChangeScreenPalette( BGFrame, x, y, width, height, paletteNo );
  GFL_BG_LoadScreenReq( BGFrame );
}

//------------------------------------------------------------------------------------
/**
 * @brief�w���������߂�x�{�^�����A�N�e�B�u��Ԃɂ���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetChangeButtonNotActive( RRT_WORK* work )
{
  BG_FONT_SetPalette( work->BGFont[ BG_FONT_CHANGE_BUTTON ], MAIN_BG_PALETTE_FONT_NOT_ACTIVE );
  StartPaletteAnime( work, PALETTE_ANIME_CHANGE_BUTTON_HOLD_WINDOW );
  StartPaletteAnime( work, PALETTE_ANIME_CHANGE_BUTTON_HOLD_FONT );
} 

//------------------------------------------------------------------------------------
/**
 * @brief�w�߂�x�{�^���𖾖ł����� 
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void BlinkReturnButton( RRT_WORK* work )
{
  // �p���b�g�A�j���J�n
  RESEARCH_COMMON_StartPaletteAnime( work->commonWork, COMMON_PALETTE_ANIME_RETURN );
}

//------------------------------------------------------------------------------------
/**
 * "new" �A�C�R����\������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void NewIconDispOn( const RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * "new" �A�C�R�����\���ɂ���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void NewIconDispOff( const RRT_WORK* work )
{
  GFL_CLWK* clactWork;

  clactWork = GetClactWork( work, CLWK_NEW_ICON );
  GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: new icon disp off\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�C�����J�n����
 */
//------------------------------------------------------------------------------------
static void StartFadeIn( void )
{
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, 0);
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�A�E�g���J�n����
 */
//------------------------------------------------------------------------------------
static void StartFadeOut( void )
{
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      0, 16, 0);
}


//====================================================================================
// ��LAYER 2 �擾�E�ݒ�E����
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���X�V����
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void CountUpStateCount( RRT_WORK* work )
{
  work->stateCount++;
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^�̒l���擾����
 *
 * @param work
 *
 * @return ��ԃJ�E���^�̒l
 */
//------------------------------------------------------------------------------------
static u32 GetStateCount( const RRT_WORK* work )
{
  return work->stateCount;
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^�����Z�b�g����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void ResetStateCount( RRT_WORK* work )
{
  work->stateCount = 0;
}

//------------------------------------------------------------------------------------
/**
 * @brief WAIT ��Ԃ̑҂����Ԃ�ݒ肷��
 *
 * @param work
 * @param frame �҂��t���[����
 */
//------------------------------------------------------------------------------------
static void SetWaitFrame( RRT_WORK* work, u32 frame )
{
  work->waitFrame = frame;
}

//------------------------------------------------------------------------------------
/**
 * @brief WAIT ��Ԃ̑҂����Ԃ��擾����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static u32 GetWaitFrame( const RRT_WORK* work )
{
  return work->waitFrame;
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
 *
 * @param work
 * @param offset �ړ���
 */
//------------------------------------------------------------------------------------
static void ShiftCursorPos( RRT_WORK* work, int offset )
{
  int nowPos;
  int nextPos;

  nowPos  = work->cursorPos;
  nextPos = nowPos;

  // �X�V��̃J�[�\���ʒu������
  do {
    nextPos = (nextPos + offset + MENU_ITEM_NUM) % MENU_ITEM_NUM;
  } while( CheckCursorSetEnable( work, nextPos ) == FALSE );

  // �J�[�\���ʒu���X�V
  work->cursorPos = nextPos;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: shift cursor pos ==> %d\n", nextPos );
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu��ύX���� ( ���l�w�� )
 *
 * @param work
 * @param menuItem �ړ���
 */
//------------------------------------------------------------------------------------
static void SetCursorPos( RRT_WORK* work, MENU_ITEM menuItem )
{
  // �J�[�\���ʒu���X�V
  work->cursorPos = menuItem;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set cursor pos ==> %d\n", menuItem );
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\�����w��ʒu�ɃZ�b�g�ł��邩�ǂ������`�F�b�N����
 *
 * @param work
 * @param menuItem �`�F�b�N�Ώۂ̍���
 *
 * @return �w�肵�����j���[���ڂɃJ�[�\�����Z�b�g�ł���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
static BOOL CheckCursorSetEnable( RRT_WORK* work, MENU_ITEM menuItem )
{
  //�w���������߂�x
  if( menuItem == MENU_ITEM_CHANGE_RESEARCH ) {
    // �I���\�ȍ��ڂ��Q�ȏ�
    if( 1 < GetSelectableTopicNum(work) ) {
      return TRUE;
    }
    // �I���\�ȍ��ڂ͂P����
    else {
      return FALSE;
    }
  }

  //�w���������߂�x�ȊO�͖������ŃZ�b�g�\
  return TRUE;
}

//------------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X�̓o�^�C���f�b�N�X���擾����
 *
 * @param work
 * @param resID ���\�[�XID
 *
 * @return �w�肵�����\�[�X�̓o�^�C���f�b�N�X
 */
//------------------------------------------------------------------------------------
static u32 GetOBJResRegisterIndex( const RRT_WORK* work, OBJ_RESOURCE_ID resID )
{
  return work->objResRegisterIdx[ resID ];
}

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���j�b�g�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���j�b�g
 */
//------------------------------------------------------------------------------------
static GFL_CLUNIT* GetClactUnit( const RRT_WORK* work, CLUNIT_INDEX unitIdx )
{
  return work->clactUnit[ unitIdx ];
}

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���[�N�̃C���f�b�N�X
 *
 * @return �w�肵���Z���A�N�^�[���[�N
 */
//------------------------------------------------------------------------------------
static GFL_CLWK* GetClactWork( const RRT_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RRT_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       PaletteAnimeData[ index ].animeType,
                       PaletteAnimeData[ index ].fadeColor );
  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: start palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StopPaletteAnime( RRT_WORK* work, PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: stop palette anime [%d]\n", index );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] );

    PALETTE_ANIME_Update( work->paletteAnime[i] );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �I���\�Ȓ������ڂ̐����擾����
 *
 * @param work
 *
 * @return �I���\�Ȓ������ڂ̐�
 */
//------------------------------------------------------------------------------------
static u8 GetSelectableTopicNum( const RRT_WORK* work )
{
  GAMEDATA* gameData;
  EVENTWORK* evwork;
  int num;

  gameData = RESEARCH_COMMON_GetGameData( work->commonWork );
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

  return num;
}

//------------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N���擾����
 *
 * @param work
 *
 * @return �S��ʋ��ʃ��[�N
 */
//------------------------------------------------------------------------------------
static RESEARCH_COMMON_WORK* GetCommonWork( const RRT_WORK* work )
{
  return work->commonWork;
}

//------------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N��ݒ肷��
 *
 * @param work
 * @param commonWork
 */
//------------------------------------------------------------------------------------
static void SetCommonWork( RRT_WORK* work, RESEARCH_COMMON_WORK* commonWork )
{
  work->commonWork = commonWork;
}

//------------------------------------------------------------------------------------
/**
 * @brief �q�[�vID���擾����
 *
 * @param work
 *
 * @return �q�[�vID
 */
//------------------------------------------------------------------------------------
static HEAPID GetHeapID( const RRT_WORK* work )
{
  return work->heapID;
}

//------------------------------------------------------------------------------------
/**
 * @brief �q�[�vID��ݒ肷��
 *
 * @param work
 * @param heapID
 */
//------------------------------------------------------------------------------------
static void SetHeapID( RRT_WORK* work, HEAPID heapID )
{
  work->heapID = heapID;
}


//====================================================================================
// ��LAYER 1 �����E�������E�j��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʃ��[�N�𐶐�����
 *
 * @param heapID
 *
 * @return �g�b�v��ʃ��[�N
 */
//------------------------------------------------------------------------------------
static RRT_WORK* CreateTopWork( HEAPID heapID )
{
  RRT_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(RRT_WORK) );

  return work;
}

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʃ��[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteTopWork( RRT_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
}

//------------------------------------------------------------------------------------
/**
 * @brief �g�b�v��ʃ��[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitTopWork( RRT_WORK* work )
{
  work->state           = RRT_STATE_SETUP;
  work->nowStateEndFlag = FALSE;
  work->stateCount      = 0;
  work->waitFrame       = 0;
  work->cursorPos       = MENU_ITEM_CHANGE_RESEARCH;
  work->newEntryFlag    = FALSE;
  work->finishFlag      = FALSE;
  work->finishResult    = RRT_RESULT_EXIT;
  work->stateSeq        = 0;

  InitOBJResources( work );
  InitStateQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitPaletteAnime( work );
}

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\���ʒu���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupCursorPos( RRT_WORK* work )
{
  RESEARCH_COMMON_WORK* commonWork;
  RADAR_SEQ prev_seq;
  SEQ_CHANGE_TRIG trig;
  MENU_ITEM cursor_pos;

  commonWork = work->commonWork;
  prev_seq   = RESEARCH_COMMON_GetPrevSeq( commonWork );
  trig       = RESEARCH_COMMON_GetSeqChangeTrig( commonWork );

  // �O�̉�ʂ��{�^���ŏI��
  if( (prev_seq != RADAR_SEQ_NULL) && (trig == SEQ_CHANGE_BY_BUTTON) ) {
    // �J�[�\���ʒu��������
    switch( prev_seq ) {
    default: GF_ASSERT(0);
    case RADAR_SEQ_SELECT: cursor_pos = MENU_ITEM_CHANGE_RESEARCH; break;
    case RADAR_SEQ_CHECK:  cursor_pos = MENU_ITEM_CHECK_RESEARCH;  break;
    }
    SetCursorPos( work, cursor_pos );
  }
  else {
    //�w���������߂�x��I����
    if( CheckCursorSetEnable( work, MENU_ITEM_CHANGE_RESEARCH ) ) {
      SetCursorPos( work, MENU_ITEM_CHANGE_RESEARCH );
    }
    //�w���������߂�x��I��s��
    else {
      SetCursorPos( work, MENU_ITEM_CHECK_RESEARCH );
    }
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�f�[�^������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitFont( RRT_WORK* work )
{
  work->font = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init font\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�f�[�^�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateFont( RRT_WORK* work )
{
  GF_ASSERT( work->font == NULL ); // ����������Ă��Ȃ�

  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
                                GFL_FONT_LOADTYPE_FILE, FALSE, work->heapID ); 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create font\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�f�[�^��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteFont( RRT_WORK* work )
{
  GF_ASSERT( work->font ); // ��������Ă��Ȃ�

  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete font\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitMessages( RRT_WORK* work )
{
  int i;

  for( i=0; i < MESSAGE_NUM; i++ )
  {
    work->message[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init messages\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateMessages( RRT_WORK* work )
{
  int i;

  for( i=0; i < MESSAGE_NUM; i++ )
  {
    GF_ASSERT( work->message[i] == NULL ); // ����������Ă��Ȃ�

    work->message[i] = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                       ARCID_MESSAGE, 
                                       MessageDataID[i],
                                       work->heapID ); 
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create messages\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteMessages( RRT_WORK* work )
{
  int i;

  for( i=0; i < MESSAGE_NUM; i++ )
  {
    GF_ASSERT( work->message[i] ); // ��������ĂȂ�

    GFL_MSG_Delete( work->message[i] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete messages\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �^�b�`�̈���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupTouchArea( RRT_WORK* work )
{
  int i;

  for( i=0; i < TOUCH_AREA_NUM; i++ )
  {
    work->touchHitTable[i].rect.left   = TouchAreaInitData[i].left;
    work->touchHitTable[i].rect.right  = TouchAreaInitData[i].right;
    work->touchHitTable[i].rect.top    = TouchAreaInitData[i].top;
    work->touchHitTable[i].rect.bottom = TouchAreaInitData[i].bottom;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create touch hit table\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitStateQueue( RRT_WORK* work )
{
  work->stateQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq queue\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateStateQueue( RRT_WORK* work )
{
  GF_ASSERT( work->stateQueue == NULL ); // ����������Ă��Ȃ�

  work->stateQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create seq queue\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteStateQueue( RRT_WORK* work )
{
  GF_ASSERT( work->stateQueue ); // ��������Ă��Ȃ�

  QUEUE_Delete( work->stateQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete seq queue\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupBG( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpBG( RRT_WORK* work )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl( MAIN_BG_FONT );
  GFL_BG_FreeBGControl( MAIN_BG_WINDOW );
  GFL_BG_FreeBGControl( SUB_BG_FONT );
  GFL_BG_FreeBGControl( SUB_BG_WINDOW );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up BG\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupSubBG_WINDOW( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RRT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-WINDOW\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RRT_WORK* work )
{
  // NULL�L�������m��
  GFL_BG_FillCharacter( SUB_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( SUB_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup SUB-BG-FONT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̌�Еt��
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpSubBG_FONT( RRT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-FONT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupMainBG_WINDOW( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RRT_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-WINDOW\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RRT_WORK* work )
{ 
  // NULL�L�����m��
  GFL_BG_FillCharacter( MAIN_BG_FONT, 0, 1, 0 );

  // �N���A
  GFL_BG_ClearScreen( MAIN_BG_FONT );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup MAIN-BG-FONT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ��Еt��
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpMainBG_FONT( RRT_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-FONT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitBGFonts( RRT_WORK* work )
{
  int i;

  for( i=0; i < BG_FONT_NUM; i++ )
  {
    work->BGFont[i] = NULL; 
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateBGFonts( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteBGFonts( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief OBJ �̃��\�[�X������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitOBJResources( RRT_WORK* work )
{
  int i;

  for( i=0; i<OBJ_RESOURCE_NUM; i++ )
  {
    work->objResRegisterIdx[i] = 0;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init OBJ resources\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void RegisterSubOBJResources( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief SUB-OBJ ���\�[�X���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void ReleaseSubOBJResources( RRT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release SUB-OBJ resources\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void RegisterMainOBJResources( RRT_WORK* work )
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

  palette = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_research_radar_graphic_obj_NCLR,
                                       CLSYS_DRAW_MAIN, 
                                       ONE_PALETTE_SIZE*6, 0, 4, 
                                       heapID );

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

//------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void ReleaseMainOBJResources( RRT_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release MAIN-OBJ resources\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitClactUnits( RRT_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init clact units\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//------------------------------------------------------------------------------------
static void CreateClactUnits( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//------------------------------------------------------------------------------------
static void DeleteClactUnits( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitClactWorks( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateClactWorks( RRT_WORK* work )
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
    charaIdx       = GetOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].characterResID );
    paletteIdx     = GetOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].paletteResID );
    cellAnimeIdx   = GetOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].cellAnimeResID );
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteClactWorks( RRT_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    work->paletteAnime[i] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreatePaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] == NULL ); // ����������Ă��Ȃ�

    work->paletteAnime[i] = PALETTE_ANIME_Create( work->heapID );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeletePaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] );

    PALETTE_ANIME_Delete( work->paletteAnime[i] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupPaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] );

    PALETTE_ANIME_Setup( work->paletteAnime[i],
                         PaletteAnimeData[i].destAdrs,
                         PaletteAnimeData[i].srcAdrs,
                         PaletteAnimeData[i].colorNum);
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: setup palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RRT_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[i] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up palette anime\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief VBlank ���荞�݂�ݒ�
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetVBlankFunc( RRT_WORK* work )
{
  GFUser_SetVIntrFunc( GFL_CLACT_SYS_VBlankFunc );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: SetVBlankFunc\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief VBlank ���荞�݂�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void ResetVBlankFunc( RRT_WORK* work )
{
  GFUser_ResetVIntrFunc();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: ResetVBlankFunc\n" );
}


//------------------------------------------------------------------------------------
/**
 * @brief �ʐM�A�C�R�����Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupWirelessIcon( const RRT_WORK* work )
{
  GFL_NET_ChangeIconPosition( WIRELESS_ICON_X, WIRELESS_ICON_Y );
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, work->heapID );
  GFL_NET_ReloadIcon();
}

//====================================================================================
// ��LAYER 0 �f�o�b�O
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[�̒��g��\������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DebugPrint_stateQueue( const RRT_WORK* work )
{
  int i;
  int dataNum;
  int value;

  // �L���[���̃f�[�^�̌����擾
  dataNum = QUEUE_GetDataNum( work->stateQueue );

  // �S�Ẵf�[�^���o��
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: seq queue = " );
  for( i=0; i < dataNum; i++ )
  { 
    value = QUEUE_PeekData( work->stateQueue, i );
    
    switch( value ) {
    case RRT_STATE_SETUP:    OS_TFPrintf( PRINT_TARGET, "SETUP " );      break;
    case RRT_STATE_STANDBY:  OS_TFPrintf( PRINT_TARGET, "STANDBY " );    break;
    case RRT_STATE_KEYWAIT:  OS_TFPrintf( PRINT_TARGET, "KEY-WAIT " );   break;
    case RRT_STATE_WAIT:     OS_TFPrintf( PRINT_TARGET, "FRAME-WAIT " ); break;
    case RRT_STATE_FADEOUT:  OS_TFPrintf( PRINT_TARGET, "FADEOUT " );    break;
    case RRT_STATE_CLEANUP:  OS_TFPrintf( PRINT_TARGET, "CLEAN-UP " );   break;
    case RRT_STATE_FINISH:   OS_TFPrintf( PRINT_TARGET, "FINISH " );     break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 
