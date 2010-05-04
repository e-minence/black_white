//////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������� ( ���j���[��� )
 * @file   research_menu.c
 * @author obata
 * @date   2010.02.03
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "bg_font.h"
#include "queue.h"
#include "palette_anime.h"
#include "research_menu.h"
#include "research_menu_index.h"
#include "research_menu_def.h"
#include "research_menu_data.cdat"
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
// ������������� ���[�N
//====================================================================================
struct _RESEARCH_MENU_WORK
{ 
  RESEARCH_COMMON_WORK* commonWork; // �S��ʋ��ʃ��[�N
  HEAPID                heapID;
  GAMESYS_WORK*         gameSystem;
  GAMEDATA*             gameData;

  GFL_FONT*    font;
  GFL_MSGDATA* message[ MESSAGE_NUM ];

  QUEUE*               seqQueue;      // �V�[�P���X�L���[
  RESEARCH_MENU_SEQ    seq;           // �����V�[�P���X
  u32                  seqCount;      // �V�[�P���X�J�E���^
  BOOL                 seqFinishFlag; // ���݂̃V�[�P���X���I���������ǂ���
  RESEARCH_MENU_RESULT result;        // ��ʏI������
  u32                  waitFrame;     // FRAME_WAIT �V�[�P���X�ł̑҂��t���[����

  MENU_ITEM cursorPos;     // �J�[�\���ʒu
  BOOL      newEntryFlag;  // �V�����l���Ƃ����������ǂ���

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
};



//====================================================================================
// ���֐��C���f�b�N�X
//====================================================================================
//------------------------------------------------------------------------------------
// ��LAYER 3 �V�[�P���X
//------------------------------------------------------------------------------------
// �V�[�P���X����������
static void InitSeq_SETUP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void InitSeq_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void InitSeq_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_KEY_WAIT
static void InitSeq_FRAME_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FRAME_WAIT
static void InitSeq_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void InitSeq_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP 
// �V�[�P���X����
static void Main_SETUP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void Main_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void Main_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_KEY_WAIT
static void Main_FRAME_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FRAME_WAIT
static void Main_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void Main_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP 
// �V�[�P���X�I������
static void FinishSeq_SETUP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_SETUP
static void FinishSeq_STAND_BY( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_STAND_BY
static void FinishSeq_KEY_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_KEY_WAIT
static void FinishSeq_FRAME_WAIT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FRAME_WAIT
static void FinishSeq_FADE_OUT( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_FADE_OUT
static void FinishSeq_CLEAN_UP( RESEARCH_MENU_WORK* work ); // RESEARCH_MENU_SEQ_CLEAN_UP 
// �V�[�P���X����
static void CountUpSeqCount( RESEARCH_MENU_WORK* work ); // �V�[�P���X�J�E���^���X�V����
static void SetNextSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq ); // ���̃V�[�P���X���L���[�ɓo�^����
static void FinishCurrentSeq( RESEARCH_MENU_WORK* work ); // ���݂̃V�[�P���X���I������
static void SwitchSeq( RESEARCH_MENU_WORK* work ); // �����V�[�P���X��ύX����
static void SetSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq ); // �����V�[�P���X��ݒ肷��
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result ); // ��ʏI�����ʂ�ݒ肷��
static void SetWaitFrame( RESEARCH_MENU_WORK* work, u32 frame ); // FRAME_WAIT �V�[�P���X�̑҂����Ԃ�ݒ肷��
static u32 GetWaitFrame( const RESEARCH_MENU_WORK* work ); // FRAME_WAIT �V�[�P���X�̑҂����Ԃ��擾����
static RESEARCH_MENU_SEQ GetFirstSeq( const RESEARCH_MENU_WORK* work ); // �ŏ��̃V�[�P���X���擾����
//------------------------------------------------------------------------------------
// ��LAYER 2 �@�\
//------------------------------------------------------------------------------------
// �f�[�^�X�V
static void CheckNewEntry( RESEARCH_MENU_WORK* work ); // �V�����l���Ƃ����������ǂ������`�F�b�N����
// �J�[�\���ړ�
static void MoveCursorUp( RESEARCH_MENU_WORK* work ); // �J�[�\������Ɉړ�����
static void MoveCursorDown( RESEARCH_MENU_WORK* work ); // �J�[�\�������Ɉړ�����
static void MoveCursorDirect( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem ); // �J�[�\���𒼐ڈړ�����
// ���j���[���ڃ{�^��
static void MenuItemSetCursorOn( MENU_ITEM menuItem ); // �J�[�\��������Ă����Ԃɂ���
static void MenuItemSetCursorOff( MENU_ITEM menuItem ); // �J�[�\��������Ă��Ȃ���Ԃɂ���
static void SetChangeButtonNotActive( RESEARCH_MENU_WORK* work ); //�w���������߂�x�{�^�����A�N�e�B�u��Ԃɂ���
// "new" �A�C�R��
static void NewIconDispOn( const RESEARCH_MENU_WORK* work ); // "new" �A�C�R����\������
static void NewIconDispOff( const RESEARCH_MENU_WORK* work ); // "new" �A�C�R�����\���ɂ���
//------------------------------------------------------------------------------------
// ��LAYER 1 �ʑ���
//------------------------------------------------------------------------------------
// �J�[�\���ʒu
static void SetupCursorPos( RESEARCH_MENU_WORK* work ); // �J�[�\���ʒu���Z�b�g�A�b�v����
static void ShiftCursorPos( RESEARCH_MENU_WORK* work, int offset ); // �J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
static void SetCursorPos( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem ); // �J�[�\���ʒu��ύX���� ( ���l�w�� )
static BOOL CheckCursorSetEnable( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem ); // �J�[�\�����w��ʒu�ɃZ�b�g�ł��邩�ǂ������`�F�b�N����
// ����������ʃ��[�N
static void CreateMenuWork( HEAPID heapID ); // ����������ʃ��[�N�𐶐�����
static void DeleteMenuWork( RESEARCH_MENU_WORK* work ); // ����������ʃ��[�N��j������
static void InitMenuWork( RESEARCH_MENU_WORK* work ); // ����������ʃ��[�N������������
static void SetupMenuWork( RESEARCH_MENU_WORK* work ); // ����������ʃ��[�N���Z�b�g�A�b�v����
static void CleanUpMenuWork( RESEARCH_MENU_WORK* work ); // ����������ʃ��[�N���N���[���A�b�v����
// �t�H���g
static void InitFont( RESEARCH_MENU_WORK* work ); // �t�H���g�f�[�^������������
static void CreateFont( RESEARCH_MENU_WORK* work ); // �t�H���g�f�[�^�𐶐�����
static void DeleteFont( RESEARCH_MENU_WORK* work ); // �t�H���g�f�[�^��j������
// ���b�Z�[�W
static void InitMessages( RESEARCH_MENU_WORK* work ); // ���b�Z�[�W�f�[�^������������
static void CreateMessages( RESEARCH_MENU_WORK* work ); // ���b�Z�[�W�f�[�^�𐶐�����
static void DeleteMessages( RESEARCH_MENU_WORK* work ); // ���b�Z�[�W�f�[�^��j������
// �^�b�`�̈�
static void SetupTouchArea( RESEARCH_MENU_WORK* work ); // �^�b�`�̈���Z�b�g�A�b�v����
// �V�[�P���X�L���[
static void InitSeqQueue( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[������������
static void CreateSeqQueue( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[�𐶐�����
static void DeleteSeqQueue( RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[��j������
// BG
static void SetupBG( RESEARCH_MENU_WORK* work ); // BG�S�ʂ̃Z�b�g�A�b�v���s��
static void SetupSubBG_WINDOW( RESEARCH_MENU_WORK* work ); // SUB-BG ( �E�B���h�E�� ) �̃Z�b�g�A�b�v���s��
static void SetupSubBG_FONT( RESEARCH_MENU_WORK* work ); // SUB-BG ( �t�H���g�� ) �̃Z�b�g�A�b�v���s��
static void SetupMainBG_WINDOW( RESEARCH_MENU_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) �̃Z�b�g�A�b�v���s��
static void SetupMainBG_FONT( RESEARCH_MENU_WORK* work ); // MAIN-BG ( �t�H���g�� ) �̃Z�b�g�A�b�v���s��
static void CleanUpBG( RESEARCH_MENU_WORK* work ); // BG�S�ʂ̃N���[���A�b�v���s��
static void CleanUpSubBG_WINDOW( RESEARCH_MENU_WORK* work ); // SUB-BG ( �E�B���h�E�� ) �̃N���[���A�b�v���s��
static void CleanUpSubBG_FONT( RESEARCH_MENU_WORK* work ); // SUB-BG ( �t�H���g�� ) �̃N���[���A�b�v���s��
static void CleanUpMainBG_WINDOW( RESEARCH_MENU_WORK* work ); // MAIN-BG ( �E�B���h�E�� ) �̃N���[���A�b�v���s��
static void CleanUpMainBG_FONT( RESEARCH_MENU_WORK* work ); // MAIN-BG ( �t�H���g�� ) �̃N���[���A�b�v���s��
// ������`��I�u�W�F�N�g
static void InitBGFonts( RESEARCH_MENU_WORK* work ); // ������`��I�u�W�F�N�g������������
static void CreateBGFonts( RESEARCH_MENU_WORK* work ); // ������`��I�u�W�F�N�g�𐶐�����
static void DeleteBGFonts( RESEARCH_MENU_WORK* work ); // ������`��I�u�W�F�N�g��j������
// OBJ ���\�[�X
static void InitOBJResources( RESEARCH_MENU_WORK* work ); // OBJ���\�[�X������������
static void RegisterSubOBJResources( RESEARCH_MENU_WORK* work ); // OBJ���\�[�X��o�^���� ( SUB-OBJ )
static void RegisterMainOBJResources( RESEARCH_MENU_WORK* work ); // OBJ���\�[�X��o�^���� ( MAIN-OBJ )
static void ReleaseSubOBJResources( RESEARCH_MENU_WORK* work ); // OBJ���\�[�X�̓o�^���������� ( SUB-OBJ )
static void ReleaseMainOBJResources( RESEARCH_MENU_WORK* work ); // OBJ���\�[�X�̓o�^���������� ( MAIN-OBJ )
static u32 GetOBJResRegisterIndex( const RESEARCH_MENU_WORK* work, OBJ_RESOURCE_ID resID ); // OBJ���\�[�X�̓o�^�C���f�b�N�X���擾����
// �Z���A�N�^�[�V�X�e��
static void CreateClactSystem( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[�V�X�e���𐶐�����
static void DeleteClactSystem( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[�V�X�e����j������
// �Z���A�N�^�[���j�b�g
static void InitClactUnits( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���j�b�g��j������
static GFL_CLUNIT* GetClactUnit( const RESEARCH_MENU_WORK* work, CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
// �Z���A�N�^�[���[�N
static void InitClactWorks( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���[�N������������
static void CreateClactWorks( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RESEARCH_MENU_WORK* work ); // �Z���A�N�^�[���[�N��j������
static GFL_CLWK* GetClactWork( const RESEARCH_MENU_WORK* work, CLWK_INDEX wkIdx ); // �Z���A�N�^�[���[�N���擾����
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
static void StartPaletteAnime( RESEARCH_MENU_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RESEARCH_MENU_WORK* work, PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void UpdatePaletteAnime( RESEARCH_MENU_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
// �ʐM�A�C�R��
static void SetupWirelessIcon( const RESEARCH_MENU_WORK* work ); // �ʐM�A�C�R�����Z�b�g�A�b�v����
// ��������
static u8 GetSelectableTopicNum( const RESEARCH_MENU_WORK* work ); // �I���\�Ȓ������ڂ̐����擾����
//------------------------------------------------------------------------------------
// ��LAYER 0 �f�o�b�O
//------------------------------------------------------------------------------------
static void DebugPrint_seqQueue( const RESEARCH_MENU_WORK* work ); // �V�[�P���X�L���[���o�͂���




//====================================================================================
// ������������� ����֐�
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�𐶐�����
 *
 * @param commonWork �S��ʋ��ʃ��[�N
 *
 * @return ������������������ʃ��[�N
 */
//------------------------------------------------------------------------------------
RESEARCH_MENU_WORK* CreateResearchMenuWork( RESEARCH_COMMON_WORK* commonWork )
{
  int i;
  RESEARCH_MENU_WORK* work;
  HEAPID heapID;

  heapID = RESEARCH_COMMON_GetHeapID( commonWork );

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_MENU_WORK) );

  // ������
  work->commonWork   = commonWork;
  work->heapID       = heapID;
  work->gameSystem   = RESEARCH_COMMON_GetGameSystem( commonWork );
  work->gameData     = RESEARCH_COMMON_GetGameData( commonWork );
  work->seq          = RESEARCH_MENU_SEQ_SETUP;
  work->seqFinishFlag= FALSE;
  work->seqCount     = 0;
  work->result       = RESEARCH_MENU_RESULT_NONE;
  work->waitFrame    = WAIT_FRAME_BUTTON;
  work->cursorPos    = MENU_ITEM_CHANGE_RESEARCH;
  work->newEntryFlag = FALSE;

  InitOBJResources( work );
  InitSeqQueue( work );
  InitMessages( work );
  InitFont( work );
  InitBGFonts( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitPaletteAnime( work );

  CreateSeqQueue( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create work\n" );

  return work;
}

//------------------------------------------------------------------------------------
/**
 * @brief ����������ʃ��[�N�̔j��
 *
 * @param heapID
 */
//------------------------------------------------------------------------------------
void DeleteResearchMenuWork( RESEARCH_MENU_WORK* work )
{
  if( work == NULL ) {
    GF_ASSERT(0);
    return;
  }

  DeleteSeqQueue( work ); // �V�[�P���X�L���[
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete work\n" );
} 

//------------------------------------------------------------------------------------
/**
 * @brief ����������� ���C������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
RESEARCH_MENU_RESULT ResearchMenuMain( RESEARCH_MENU_WORK* work )
{
  // �V�[�P���X���Ƃ̏���
  switch( work->seq ) {
  case RESEARCH_MENU_SEQ_SETUP:      Main_SETUP   ( work );   break;
  case RESEARCH_MENU_SEQ_STAND_BY:   Main_STAND_BY( work );   break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:   Main_KEY_WAIT( work );   break;
  case RESEARCH_MENU_SEQ_FRAME_WAIT: Main_FRAME_WAIT( work ); break;
  case RESEARCH_MENU_SEQ_FADE_OUT:   Main_FADE_OUT( work );   break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:   Main_CLEAN_UP( work );   break;
  case RESEARCH_MENU_SEQ_FINISH:     return work->result;
  default:  GF_ASSERT(0);
  }

  CheckNewEntry( work );      // �f�[�^�X�V
  UpdatePaletteAnime( work ); // �p���b�g�A�j���[�V�����X�V
  RESEARCH_COMMON_UpdatePaletteAnime( work->commonWork ); // ���ʃp���b�g�A�j���[�V�������X�V
  GFL_CLACT_SYS_Main();       // �Z���A�N�^�[�V�X�e�� ���C������

  CountUpSeqCount( work ); // �V�[�P���X�J�E���^�X�V
  SwitchSeq( work );  // �V�[�P���X�X�V

  return RESEARCH_MENU_RESULT_CONTINUE;
}


//====================================================================================
// ���V�[�P���X����
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_SETUP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq SETUP\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[�҂��V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_STAND_BY( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq STAND_BY\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  MenuItemSetCursorOn( work->cursorPos ); // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
  StartPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j�����J�n

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq KEY_WAIT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂��V�[�P���X ( RESEARCH_MENU_SEQ_FRAME_WAIT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_FRAME_WAIT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq FRAME_WAIT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq CLEAN_UP\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeq_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // �t�F�[�h�A�E�g�J�n
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      0, 16, 0);

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq FADE_OUT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_SETUP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq SETUP\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_STAND_BY( RESEARCH_MENU_WORK* work )
{ 
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq STAND_BY\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_KEY_WAIT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq KEY_WAIT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂��V�[�P���X ( RESEARCH_MENU_SEQ_FRAME_WAIT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_FRAME_WAIT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq FRAME_WAIT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq CLEAN_UP\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishSeq_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish seq FADE_OUT\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X ( RESEARCH_MENU_SEQ_SETUP ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_SETUP( RESEARCH_MENU_WORK* work )
{
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
  CreateClactSystem( work );
  RegisterSubOBJResources( work );
  RegisterMainOBJResources( work );
  CreateClactUnits( work );
  CreateClactWorks( work );

  // �p���b�g�A�j���[�V����
  CreatePaletteAnime( work );
  SetupPaletteAnime( work );

  // �ʐM�A�C�R��
  SetupWirelessIcon( work );

  // ��ʃt�F�[�h�C��
  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, 0);

  //�w���������߂�x�{�^�����A�N�e�B�u��Ԃɂ���
  if( CheckCursorSetEnable( work, MENU_ITEM_CHANGE_RESEARCH ) == FALSE ) {
    SetChangeButtonNotActive( work );
  }

  SetNextSeq( work, GetFirstSeq(work) ); // ���̃V�[�P���X���Z�b�g
  FinishCurrentSeq( work ); // �V�[�P���X�I��
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃L�[�҂��V�[�P���X ( RESEARCH_MENU_SEQ_STAND_BY ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_STAND_BY( RESEARCH_MENU_WORK* work )
{
  int trg;
  int touch;
  int commonTouch;

  trg = GFL_UI_KEY_GetTrg();
  touch = GFL_UI_TP_HitTrg( work->touchHitTable );
  commonTouch = GFL_UI_TP_HitTrg( RESEARCH_COMMON_GetHitTable(work->commonWork) );

  //----------------------
  //�u���ǂ�v�{�^�� or B
  if( (commonTouch == COMMON_TOUCH_AREA_RETURN_BUTTON) || (trg & PAD_BUTTON_B) ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetResult( work, RESEARCH_MENU_RESULT_EXIT );        // ��ʏI�����ʂ�����
    SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );
    SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );
    SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );
    FinishCurrentSeq( work );
    return;
  }

  //----------------
  // �\���L�[ or A
  if( (trg & PAD_KEY_UP)   || (trg & PAD_KEY_DOWN)  ||
      (trg & PAD_KEY_LEFT) || (trg & PAD_KEY_RIGHT) || (trg & PAD_BUTTON_A) ) {
    MenuItemSetCursorOn( work->cursorPos ); // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
    SetNextSeq( work, RESEARCH_MENU_SEQ_KEY_WAIT );
    FinishCurrentSeq( work );
  }

  //-------------------------------------
  //�u�������e��ύX����v�{�^�����^�b�`
  if( touch == TOUCH_AREA_CHANGE_BUTTON ) {
    if( CheckCursorSetEnable(work, MENU_ITEM_CHANGE_RESEARCH) ) {
      RESEARCH_COMMON_SetSeqChangeTrig( 
          work->commonWork, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHANGE_RESEARCH ); // �J�[�\���ʒu���X�V
      MenuItemSetCursorOn( work->cursorPos );              // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );   // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );     // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                      // ���艹
      SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT );   // ��ʏI�����ʂ�����
      SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );
      SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );
      SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );
      FinishCurrentSeq( work );
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }
  //-------------------------------------
  //�u�����񍐂��m�F����v�{�^�����^�b�`
  if( touch == TOUCH_AREA_CHECK_BUTTON ) {
    if( CheckCursorSetEnable(work, MENU_ITEM_CHECK_RESEARCH) ) {
      RESEARCH_COMMON_SetSeqChangeTrig( 
          work->commonWork, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHECK_RESEARCH ); // �J�[�\���ʒu���X�V
      MenuItemSetCursorOn( work->cursorPos );             // �J�[�\���ʒu�̃��j���[���ڂ�I����Ԃɂ���
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );  // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );    // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                     // ���艹
      SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK );   // ��ʏI�����ʂ�����
      SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );
      SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );
      SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );
      FinishCurrentSeq( work );
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }

#ifdef PM_DEBUG
  if( trg & PAD_BUTTON_L ) {
    DEBUG_GAMEBEACON_Set_NewEntry();
  }
#endif
}

//------------------------------------------------------------------------------------
/**
 * @brief �L�[���͑҂��V�[�P���X ( RESEARCH_MENU_SEQ_KEY_WAIT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_KEY_WAIT( RESEARCH_MENU_WORK* work )
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
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetResult( work, RESEARCH_MENU_RESULT_EXIT );        // ��ʏI�����ʂ�����
    SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );    // �� �҂�
    SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );      // �� �t�F�[�h�A�E�g
    SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );      // �� �N���[���A�b�v
    FinishCurrentSeq( work );
    return;
  }

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

  //----------
  // A �{�^��
  if( trg & PAD_BUTTON_A ) {
    RESEARCH_COMMON_SetSeqChangeTrig( 
        work->commonWork, SEQ_CHANGE_BY_BUTTON ); // ��ʑJ�ڂ̃g���K��o�^
    StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON ); // �J�[�\��ON�p���b�g�A�j���I��
    StartPaletteAnime( work, PALETTE_ANIME_SELECT );   // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_DECIDE1 );                    // ���艹
    switch( work->cursorPos ) { // ��ʏI�����ʂ�����
    case MENU_ITEM_CHANGE_RESEARCH: SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT ); break;
    case MENU_ITEM_CHECK_RESEARCH:  SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK );  break;
    default: GF_ASSERT(0)
    }
    SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );  // �� �҂�
    SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );    // �� �t�F�[�h�A�E�g
    SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );    // �� �N���[���A�b�v
    FinishCurrentSeq( work );
    return;
  }

  //----------------------------
  //�u�������e��ύX����v�{�^��
  if( touch == TOUCH_AREA_CHANGE_BUTTON ) {
    if( CheckCursorSetEnable(work, MENU_ITEM_CHANGE_RESEARCH) ) {
      RESEARCH_COMMON_SetSeqChangeTrig( 
          work->commonWork, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHANGE_RESEARCH );  // �J�[�\���ʒu���X�V
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );    // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );      // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                       // ���艹
      SetResult( work, RESEARCH_MENU_RESULT_TO_SELECT );    // ��ʏI�����ʂ�����
      SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );     // �� �҂�
      SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );       // �� �t�F�[�h�A�E�g
      SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );       // �� �N���[���A�b�v
      FinishCurrentSeq( work );
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }
  //----------------------------
  //�u�����񍐂��m�F����v�{�^��
  if( touch == TOUCH_AREA_CHECK_BUTTON ) {
    if( CheckCursorSetEnable(work, MENU_ITEM_CHECK_RESEARCH) ) {
      RESEARCH_COMMON_SetSeqChangeTrig( 
          work->commonWork, SEQ_CHANGE_BY_TOUCH ); // ��ʑJ�ڂ̃g���K��o�^
      MoveCursorDirect( work, MENU_ITEM_CHECK_RESEARCH ); // �J�[�\���ʒu���X�V
      StopPaletteAnime( work, PALETTE_ANIME_CURSOR_ON );  // �J�[�\��ON�p���b�g�A�j���I��
      StartPaletteAnime( work, PALETTE_ANIME_SELECT );    // �I���p���b�g�A�j���J�n
      PMSND_PlaySE( SEQ_SE_DECIDE1 );                     // ���艹
      SetResult( work, RESEARCH_MENU_RESULT_TO_CHECK );   // ��ʏI�����ʂ�����
      SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );   // �� �҂�
      SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );     // �� �t�F�[�h�A�E�g
      SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );     // �� �N���[���A�b�v
      FinishCurrentSeq( work );
    }
    else {
      PMSND_PlaySE( SEQ_SE_BEEP );
    }
    return;
  }

  //----------
  // B �{�^��
  if( trg & PAD_BUTTON_B ) {
    RESEARCH_COMMON_StartPaletteAnime( 
        work->commonWork, COMMON_PALETTE_ANIME_RETURN ); // �I���p���b�g�A�j���J�n
    PMSND_PlaySE( SEQ_SE_CANCEL1 );                      // �L�����Z����
    SetResult( work, RESEARCH_MENU_RESULT_EXIT );        // ��ʏI�����ʂ�����
    SetNextSeq( work, RESEARCH_MENU_SEQ_FRAME_WAIT );    // �� �҂�
    SetNextSeq( work, RESEARCH_MENU_SEQ_FADE_OUT );      // �� �t�F�[�h�A�E�g
    SetNextSeq( work, RESEARCH_MENU_SEQ_CLEAN_UP );      // �� �N���[���A�b�v
    FinishCurrentSeq( work );
    return;
  }

#ifdef PM_DEBUG
  if( trg & PAD_BUTTON_L ) {
    DEBUG_GAMEBEACON_Set_NewEntry();
  }
#endif
}

//------------------------------------------------------------------------------------
/**
 * @brief �t���[���o�ߑ҂��V�[�P���X ( RESEARCH_MENU_SEQ_FRAME_WAIT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_FRAME_WAIT( RESEARCH_MENU_WORK* work )
{
  // �҂����Ԃ��o��
  if( GetWaitFrame(work) < work->seqCount ) {
    FinishCurrentSeq( work );
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_FADE_OUT ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_FADE_OUT( RESEARCH_MENU_WORK* work )
{
  // �t�F�[�h���I��
  if( GFL_FADE_CheckFade() == FALSE ) {
    FinishCurrentSeq( work );
  } 
}

//------------------------------------------------------------------------------------
/**
 * @brief ��Еt���V�[�P���X ( RESEARCH_MENU_SEQ_CLEAN_UP ) �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void Main_CLEAN_UP( RESEARCH_MENU_WORK* work )
{
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

  SetNextSeq( work, RESEARCH_MENU_SEQ_FINISH );
  FinishCurrentSeq( work );
}


//====================================================================================
// ���V�[�P���X����
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�J�E���^���X�V����
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void CountUpSeqCount( RESEARCH_MENU_WORK* work )
{
  u32 maxCount;

  // �C���N�������g
  work->seqCount++;

  // �ő�l������
  switch( work->seq ) {
  case RESEARCH_MENU_SEQ_SETUP:       maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_STAND_BY:    maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:    maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_FRAME_WAIT:  maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:    maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:    maxCount = 0xffffffff;  break;
  case RESEARCH_MENU_SEQ_FINISH:      maxCount = 0xffffffff;  break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < work->seqCount ) { 
    work->seqCount = maxCount;
  }
}

//------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X���L���[�ɓo�^����
 *
 * @param work
 * @param nextSeq �o�^����V�[�P���X
 */
//------------------------------------------------------------------------------------
static void SetNextSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq )
{
  // �V�[�P���X�L���[�ɒǉ�����
  QUEUE_EnQueue( work->seqQueue, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set next seq\n" );
  DebugPrint_seqQueue( work );
}

//------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void FinishCurrentSeq( RESEARCH_MENU_WORK* work )
{
  // ���łɏI���ς�
  GF_ASSERT( work->seqFinishFlag == FALSE );

  // �I���t���O�𗧂Ă�
  work->seqFinishFlag = TRUE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: finish current sequence\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SwitchSeq( RESEARCH_MENU_WORK* work )
{
  RESEARCH_MENU_SEQ nextSeq;

  if( work->seqFinishFlag == FALSE ){ return; }  // ���݂̃V�[�P���X���I�����Ă��Ȃ�
  if( QUEUE_IsEmpty( work->seqQueue ) ){ return; } // �V�[�P���X�L���[�ɓo�^����Ă��Ȃ�

  // �ύX
  nextSeq = QUEUE_DeQueue( work->seqQueue );
  SetSeq( work, nextSeq ); 

  // DEBUG: �V�[�P���X�L���[��\��
  DebugPrint_seqQueue( work );
} 

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work
 * @parma nextSeq �ݒ肷��V�[�P���X
 */
//------------------------------------------------------------------------------------
static void SetSeq( RESEARCH_MENU_WORK* work, RESEARCH_MENU_SEQ nextSeq )
{ 
  // �V�[�P���X�̏I������
  switch( work->seq ) {
  case RESEARCH_MENU_SEQ_SETUP:      FinishSeq_SETUP( work );       break;
  case RESEARCH_MENU_SEQ_STAND_BY:   FinishSeq_STAND_BY( work );    break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:   FinishSeq_KEY_WAIT( work );    break;
  case RESEARCH_MENU_SEQ_FRAME_WAIT: FinishSeq_FRAME_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:   FinishSeq_FADE_OUT( work );    break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:   FinishSeq_CLEAN_UP( work );    break;
  case RESEARCH_MENU_SEQ_FINISH:     break;                         
  default:  GF_ASSERT(0);
  }

  // �X�V
  work->seq           = nextSeq;
  work->seqCount      = 0;
  work->seqFinishFlag = FALSE;

  // �V�[�P���X�̏���������
  switch( nextSeq ) {
  case RESEARCH_MENU_SEQ_SETUP:      InitSeq_SETUP( work );       break;
  case RESEARCH_MENU_SEQ_STAND_BY:   InitSeq_STAND_BY( work );    break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:   InitSeq_KEY_WAIT( work );    break;
  case RESEARCH_MENU_SEQ_FRAME_WAIT: InitSeq_FRAME_WAIT( work );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:   InitSeq_FADE_OUT( work );    break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:   InitSeq_CLEAN_UP( work );    break;
  case RESEARCH_MENU_SEQ_FINISH:     break;                       
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set seq ==> " );
  switch( nextSeq ) {
  case RESEARCH_MENU_SEQ_SETUP:      OS_TFPrintf( PRINT_TARGET, "SETUP" );       break;
  case RESEARCH_MENU_SEQ_STAND_BY:   OS_TFPrintf( PRINT_TARGET, "STAND_BY" );    break;
  case RESEARCH_MENU_SEQ_KEY_WAIT:   OS_TFPrintf( PRINT_TARGET, "KEY_WAIT" );    break;
  case RESEARCH_MENU_SEQ_FRAME_WAIT: OS_TFPrintf( PRINT_TARGET, "FRAME_WAIT" );  break;
  case RESEARCH_MENU_SEQ_FADE_OUT:   OS_TFPrintf( PRINT_TARGET, "FADE_OUT" );    break;
  case RESEARCH_MENU_SEQ_CLEAN_UP:   OS_TFPrintf( PRINT_TARGET, "CLEAN_UP" );    break;
  case RESEARCH_MENU_SEQ_FINISH:     OS_TFPrintf( PRINT_TARGET, "FINISH" );      break;
  default:  GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//====================================================================================
// ���f�[�^�X�V
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �V�����l���Ƃ����������ǂ������`�F�b�N����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CheckNewEntry( RESEARCH_MENU_WORK* work )
{
  if( work->newEntryFlag == FALSE ) {
    // �V�����l���Ƃ�������
    if( GAMEBEACON_Get_NewEntry() == TRUE ) {
      work->newEntryFlag = TRUE; // �t���O�𗧂Ă�
      NewIconDispOn( work );     // "new" �A�C�R����\������
    }
  }
}

//====================================================================================
// ���J�[�\���ړ�
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �J�[�\������ֈړ�����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void MoveCursorUp( RESEARCH_MENU_WORK* work )
{
  int nowPos;
  int nextPos;

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
static void MoveCursorDown( RESEARCH_MENU_WORK* work )
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
static void MoveCursorDirect( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem )
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
 * @brief �J�[�\���ʒu���Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupCursorPos( RESEARCH_MENU_WORK* work )
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
 * @brief �J�[�\���ʒu��ύX���� ( �I�t�Z�b�g�w�� )
 *
 * @param work
 * @param offset �ړ���
 */
//------------------------------------------------------------------------------------
static void ShiftCursorPos( RESEARCH_MENU_WORK* work, int offset )
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
static void SetCursorPos( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem )
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
static BOOL CheckCursorSetEnable( RESEARCH_MENU_WORK* work, MENU_ITEM menuItem )
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

  return TRUE;
}


//====================================================================================
// ����ʏI������
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̏I�����ʂ����肷��
 *
 * @param work
 * @param result ����
 */
//------------------------------------------------------------------------------------
static void SetResult( RESEARCH_MENU_WORK* work, RESEARCH_MENU_RESULT result )
{
  // ���d�ݒ�
  GF_ASSERT( work->result == RESEARCH_MENU_RESULT_NONE );

  // �ݒ�
  work->result = result;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: set result (%d)\n", result );
}

//------------------------------------------------------------------------------------
/**
 * @brief FRAME_WAIT �V�[�P���X�̑҂����Ԃ�ݒ肷��
 *
 * @param work
 * @param frame �҂��t���[����
 */
//------------------------------------------------------------------------------------
static void SetWaitFrame( RESEARCH_MENU_WORK* work, u32 frame )
{
  work->waitFrame = frame;
}

//------------------------------------------------------------------------------------
/**
 * @brief FRAME_WAIT �V�[�P���X�̑҂����Ԃ��擾����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static u32 GetWaitFrame( const RESEARCH_MENU_WORK* work )
{
  return work->waitFrame;
}

//------------------------------------------------------------------------------------
/**
 * @brief �ŏ��̃V�[�P���X���擾����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static RESEARCH_MENU_SEQ GetFirstSeq( const RESEARCH_MENU_WORK* work )
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
    return RESEARCH_MENU_SEQ_KEY_WAIT;
  }
  else {
    return RESEARCH_MENU_SEQ_STAND_BY;
  }
}


//====================================================================================
// ���t�H���g
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �t�H���g�f�[�^������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitFont( RESEARCH_MENU_WORK* work )
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
static void CreateFont( RESEARCH_MENU_WORK* work )
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
static void DeleteFont( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->font ); // ��������Ă��Ȃ�

  GFL_FONT_Delete( work->font );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete font\n" );
}


//====================================================================================
// �����b�Z�[�W
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�f�[�^������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitMessages( RESEARCH_MENU_WORK* work )
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
static void CreateMessages( RESEARCH_MENU_WORK* work )
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
static void DeleteMessages( RESEARCH_MENU_WORK* work )
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
static void SetupTouchArea( RESEARCH_MENU_WORK* work )
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
 * @brief �V�[�P���X�L���[������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitSeqQueue( RESEARCH_MENU_WORK* work )
{
  work->seqQueue = NULL;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: init seq queue\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateSeqQueue( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->seqQueue == NULL ); // ����������Ă��Ȃ�

  work->seqQueue = QUEUE_Create( SEQ_QUEUE_SIZE, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create seq queue\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteSeqQueue( RESEARCH_MENU_WORK* work )
{
  GF_ASSERT( work->seqQueue ); // ��������Ă��Ȃ�

  QUEUE_Delete( work->seqQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete seq queue\n" );
}


//====================================================================================
// ��BG
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief BG �̏���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief BG �̌�Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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


//====================================================================================
// ������ �E�B���h�EBG��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief ���� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpSubBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-WINDOW\n" );
}


//====================================================================================
// ������ �t�H���gBG��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief SUB-BG �t�H���g�ʂ̏���
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupSubBG_FONT( RESEARCH_MENU_WORK* work )
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
static void CleanUpSubBG_FONT( RESEARCH_MENU_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( SUB_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up SUB-BG-FONT\n" );
}


//====================================================================================
// ������� �E�B���h�EBG��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief ����� �E�B���h�EBG�� ��Еt��
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CleanUpMainBG_WINDOW( RESEARCH_MENU_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-WINDOW\n" );
}


//====================================================================================
// ������� �t�H���gBG��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ����� �t�H���gBG�� ����
 * 
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupMainBG_FONT( RESEARCH_MENU_WORK* work )
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
static void CleanUpMainBG_FONT( RESEARCH_MENU_WORK* work )
{ 
  // NULL�L�������
  GFL_BG_FillCharacterRelease( MAIN_BG_FONT, 1, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: clean up MAIN-BG-FONT\n" );
}


//====================================================================================
// ��������`��I�u�W�F�N�g
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitBGFonts( RESEARCH_MENU_WORK* work )
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

//------------------------------------------------------------------------------------
/**
 * @brief ������`��I�u�W�F�N�g��j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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


//====================================================================================
// �����j���[���ڑ���
//====================================================================================

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
  //�u�������e��ύX����v
  case MENU_ITEM_CHANGE_RESEARCH:
    BGFrame   = MAIN_BG_WINDOW;
    x         = CHANGE_BUTTON_X;
    y         = CHANGE_BUTTON_Y;
    width     = CHANGE_BUTTON_WIDTH;
    height    = CHANGE_BUTTON_HEIGHT;
    paletteNo = MAIN_BG_PALETTE_WINDOW_OFF_CHANGE;
    break;

  //�u�����񍐂��m�F����v
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
static void SetChangeButtonNotActive( RESEARCH_MENU_WORK* work )
{
  BG_FONT_SetPalette( work->BGFont[ BG_FONT_CHANGE_BUTTON ], MAIN_BG_PALETTE_FONT_NOT_ACTIVE );
  StartPaletteAnime( work, PALETTE_ANIME_CHANGE_BUTTON_HOLD_WINDOW );
  StartPaletteAnime( work, PALETTE_ANIME_CHANGE_BUTTON_HOLD_FONT );
}

//====================================================================================
// ��OBJ
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e�����쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void CreateClactSystem( RESEARCH_MENU_WORK* work )
{
  // �V�X�e���쐬
  //GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // VBlank ���荞�݊֐���o�^
  GFUser_SetVIntrFunc( GFL_CLACT_SYS_VBlankFunc );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: create clact system\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_MENU_WORK* work )
{
  // VBkank ���荞�݊֐�������
  GFUser_ResetVIntrFunc();

  // �V�X�e���j��
  //GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: delete clact system\n" );
}


//====================================================================================
// ��SUB-OBJ ���\�[�X
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief OBJ �̃��\�[�X������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitOBJResources( RESEARCH_MENU_WORK* work )
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
static void RegisterSubOBJResources( RESEARCH_MENU_WORK* work )
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
static void ReleaseSubOBJResources( RESEARCH_MENU_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_SUB_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release SUB-OBJ resources\n" );
}


//====================================================================================
// ��MAIN-OBJ ���\�[�X
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief MAIN-OBJ ���\�[�X��o�^����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void RegisterMainOBJResources( RESEARCH_MENU_WORK* work )
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
static void ReleaseMainOBJResources( RESEARCH_MENU_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_PALETTE ] );
  GFL_CLGRP_CELLANIM_Release( work->objResRegisterIdx[ OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: release MAIN-OBJ resources\n" );
}


//====================================================================================
// ���Z���A�N�^�[���j�b�g
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���쐬����
 *
 * @param
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param
 */
//------------------------------------------------------------------------------------
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


//====================================================================================
// ���Z���A�N�^�[���[�N
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���쐬����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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


//====================================================================================
// ��OBJ �A�N�Z�X
//====================================================================================

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
static u32 GetOBJResRegisterIndex( const RESEARCH_MENU_WORK* work, OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RESEARCH_MENU_WORK* work, CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RESEARCH_MENU_WORK* work, CLWK_INDEX wkIdx )
{
  return work->clactWork[ wkIdx ];
}

//------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void InitPaletteAnime( RESEARCH_MENU_WORK* work )
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
static void CreatePaletteAnime( RESEARCH_MENU_WORK* work )
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
static void DeletePaletteAnime( RESEARCH_MENU_WORK* work )
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
static void SetupPaletteAnime( RESEARCH_MENU_WORK* work )
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
static void CleanUpPaletteAnime( RESEARCH_MENU_WORK* work )
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
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void StartPaletteAnime( RESEARCH_MENU_WORK* work, PALETTE_ANIME_INDEX index )
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
static void StopPaletteAnime( RESEARCH_MENU_WORK* work, PALETTE_ANIME_INDEX index )
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
static void UpdatePaletteAnime( RESEARCH_MENU_WORK* work )
{
  int i;

  for( i=0; i < PALETTE_ANIME_NUM; i++ )
  {
    GF_ASSERT( work->paletteAnime[i] );

    PALETTE_ANIME_Update( work->paletteAnime[i] );
  }
}



//====================================================================================
// ��"new" �A�C�R��
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * "new" �A�C�R����\������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
/**
 * "new" �A�C�R�����\���ɂ���
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void NewIconDispOff( const RESEARCH_MENU_WORK* work )
{
  GFL_CLWK* clactWork;

  clactWork = GetClactWork( work, CLWK_NEW_ICON );
  GFL_CLACT_WK_SetDrawEnable( clactWork, FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-MENU: new icon disp off\n" );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ʐM�A�C�R�����Z�b�g�A�b�v����
 *
 * @param work
 */
//------------------------------------------------------------------------------------
static void SetupWirelessIcon( const RESEARCH_MENU_WORK* work )
{
  GFL_NET_ChangeIconPosition( WIRELESS_ICON_X, WIRELESS_ICON_Y );
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, work->heapID );
  GFL_NET_ReloadIcon();
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
static u8 GetSelectableTopicNum( const RESEARCH_MENU_WORK* work )
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

  return num;
}

//------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�L���[�̒��g��\������
 *
 * @param work
 */
//------------------------------------------------------------------------------------
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
    
    switch( value ) {
    case RESEARCH_MENU_SEQ_SETUP:      OS_TFPrintf( PRINT_TARGET, "SETUP " );      break;
    case RESEARCH_MENU_SEQ_STAND_BY:   OS_TFPrintf( PRINT_TARGET, "STAND_BY " );   break;
    case RESEARCH_MENU_SEQ_KEY_WAIT:   OS_TFPrintf( PRINT_TARGET, "KEY-WAIT " );   break;
    case RESEARCH_MENU_SEQ_FRAME_WAIT: OS_TFPrintf( PRINT_TARGET, "FRAME-WAIT " ); break;
    case RESEARCH_MENU_SEQ_FADE_OUT:   OS_TFPrintf( PRINT_TARGET, "FADE_OUT " );   break;
    case RESEARCH_MENU_SEQ_CLEAN_UP:   OS_TFPrintf( PRINT_TARGET, "CLEAN-UP " );   break;
    case RESEARCH_MENU_SEQ_FINISH:     OS_TFPrintf( PRINT_TARGET, "FINISH " );     break;
    default: GF_ASSERT(0);
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
} 
