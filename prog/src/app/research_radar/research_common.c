/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʒ�`
 * @file   research_common.c
 * @author obata
 * @date   2010.02.13
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "palette_anime.h"
#include "research_common.h"
#include "research_common_def.h"
#include "research_common_data.cdat"

#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "system/palanm.h"         // for PaletteFadeXxxx

#include "arc/arc_def.h"             // for ARCID_xxxx
#include "arc/app_menu_common.naix"  // for NARC_research_radar_xxxx


//===============================================================================
// ���������[�_�[ �S��ʋ��ʃ��[�N
//===============================================================================
struct _RESEARCH_COMMON_WORK
{
  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;
  GAMEDATA*     gameData;

  // �^�b�`�͈�
  GFL_UI_TP_HITTBL touchHitTable[ COMMON_TOUCH_AREA_NUM ]; 
  
  // �p���b�g�A�j���[�V����
  PALETTE_ANIME* paletteAnime[ COMMON_PALETTE_ANIME_NUM ];

  // OBJ
  u32         OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_NUM ]; // ���ʃ��\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ COMMON_CLUNIT_NUM ];               // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ COMMON_CLWK_NUM ];                 // �Z���A�N�^�[���[�N

  // �J���[�p���b�g
  PALETTE_FADE_PTR paletteFadeSystem; // �J���[�p���b�g �t�F�[�h�V�X�e��

  // ��ʑJ��
  RADAR_SEQ prevSeq; // ���O�̉��
  RADAR_SEQ nowSeq;  // ���݂̉��
  SEQ_CHANGE_TRIG seqTrig; // ��ʑJ�ڂ̈������ƂȂ����g���K
};

//-------------------------------------------------------------------------------
// ���������E�����E�j��
//-------------------------------------------------------------------------------
// �S��ʋ��ʃ��[�N
static void InitCommonWork( RESEARCH_COMMON_WORK* work ); // �S��ʋ��ʃ��[�N������������
static RESEARCH_COMMON_WORK* CreateCommonWork( HEAPID heapID ); // �S��ʋ��ʃ��[�N�𐶐�����
static void DeleteCommonWork( RESEARCH_COMMON_WORK* work ); // �S��ʋ��ʃ��[�N��j������
static void SetupCommonWork( RESEARCH_COMMON_WORK* work, GAMESYS_WORK* gameSystem, HEAPID heapID ); // �S��ʋ��ʃ��[�N���Z�b�g�A�b�v����
static void CleanUpCommonWork( RESEARCH_COMMON_WORK* work ); // �S��ʋ��ʃ��[�N���N���[���A�b�v����
// �Z���A�N�^�[�V�X�e��
static void CreateClactSystem( RESEARCH_COMMON_WORK* work ); // �A�N�^�[�V�X�e��������������
static void DeleteClactSystem( RESEARCH_COMMON_WORK* work ); // �A�N�^�[�V�X�e����j������
// OBJ ���\�[�X
static void InitOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X������������
static void RegisterOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X��o�^����
static void ReleaseOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X���������
// �Z���A�N�^�[���j�b�g
static void InitClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g��j������
// �Z���A�N�^�[���[�N
static void InitClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N������������
static void CreateClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N��j������
// �p���b�g�t�F�[�h�V�X�e��
static void InitPaletteFadeSystem( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e��������������
static void CreatePaletteFadeSystem( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e���𐶐�����
static void DeletePaletteFadeSystem( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e����j������
static void SetupPaletteFadeSystem( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����Z�b�g�A�b�v����
static void CleanUpPaletteFadeSystem( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����N���[���A�b�v����
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
static void StartPaletteAnime( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void ResetPalette( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index  ); // �p���b�g���A�j���J�n���̏�ԂɃ��Z�b�g����
static void UpdatePaletteAnime( RESEARCH_COMMON_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
// �^�b�`�͈�
static void SetupTouchArea( RESEARCH_COMMON_WORK* work ); // �^�b�`�͈͂��Z�b�g�A�b�v����
//-------------------------------------------------------------------------------
// ���擾
//-------------------------------------------------------------------------------
static u32 GetOBJResRegisterIndex( const RESEARCH_COMMON_WORK* work, COMMON_OBJ_RESOURCE_ID resID ); // ����OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx ); // ����OBJ�̃Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx ); // ����OBJ�̃Z���A�N�^�[���[�N���擾����





//===============================================================================
// ���O�����J�֐�
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���ʃ��[�N�𐶐�����
 *
 * @param heapID
 * @param gameSystem
 */
//-------------------------------------------------------------------------------
RESEARCH_COMMON_WORK* RESEARCH_COMMON_CreateWork( HEAPID heapID, GAMESYS_WORK* gameSystem )
{
  RESEARCH_COMMON_WORK* work;

  work = CreateCommonWork( heapID );           // ����
  InitCommonWork( work );                      // ������
  SetupCommonWork( work, gameSystem, heapID ); // �Z�b�g�A�b�v

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create work\n" );

  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ʃ��[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_DeleteWork( RESEARCH_COMMON_WORK* work )
{
  CleanUpCommonWork( work ); // �N���[���A�b�v
  DeleteCommonWork( work );  // �j��

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h ( �u���b�N�E�A�E�g ) ���J�n����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_StartPaletteFadeBlackOut( RESEARCH_COMMON_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: start palette fade black out\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h ( �u���b�N�E�C�� ) ���J�n����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_StartPaletteFadeBlackIn( RESEARCH_COMMON_WORK* work )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: start palette fade black in\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �q�[�vID���擾����
 *
 * @param work
 *
 * @return �q�[�vID
 */
//-------------------------------------------------------------------------------
HEAPID RESEARCH_COMMON_GetHeapID( const RESEARCH_COMMON_WORK* work )
{
  return work->heapID;
}

//-------------------------------------------------------------------------------
/**
 * @brief �Q�[���V�X�e�����擾����
 *
 * @param work
 *
 * @return �Q�[���V�X�e��
 */
//-------------------------------------------------------------------------------
GAMESYS_WORK* RESEARCH_COMMON_GetGameSystem( const RESEARCH_COMMON_WORK* work )
{
  return work->gameSystem;
}

//-------------------------------------------------------------------------------
/**
 * @brief �Q�[���f�[�^���擾����
 *
 * @param work
 *
 * @return �Q�[���f�[�^
 */
//-------------------------------------------------------------------------------
GAMEDATA* RESEARCH_COMMON_GetGameData( const RESEARCH_COMMON_WORK* work )
{
  return work->gameData;
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���j�b�g�̃C���f�b�N�X
 *
 * @return �w�肵���C���f�b�N�X�̃Z���A�N�^�[���j�b�g
 */
//-------------------------------------------------------------------------------
GFL_CLUNIT* RESEARCH_COMMON_GetClactUnit( 
    const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
{
  return GetClactUnit( work, unitIdx );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N���擾����
 *
 * @param work
 * @param workIdx �Z���A�N�^�[���[�N�̃C���f�b�N�X
 *
 * @return �w�肵���C���f�b�N�X�̃Z���A�N�^�[���[�N
 */
//-------------------------------------------------------------------------------
GFL_CLWK* RESEARCH_COMMON_GetClactWork( 
    const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return GetClactWork( work, workIdx );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ʃp���b�g�t�F�[�h�V�X�e�����擾����
 *
 * @param work
 *
 * @return ���ʃp���b�g�t�F�[�h�V�X�e��
 */
//-------------------------------------------------------------------------------
PALETTE_FADE_PTR RESEARCH_COMMON_GetPaletteFadeSystem( const RESEARCH_COMMON_WORK* work )
{
  return work->paletteFadeSystem;
} 

//-------------------------------------------------------------------------------
/**
 * @brief �q�b�g�e�[�u�����擾����
 *
 * @param work
 *
 * @return ���ʃ^�b�`�͈͂̃q�b�g�e�[�u��
 */
//-------------------------------------------------------------------------------
const GFL_UI_TP_HITTBL* RESEARCH_COMMON_GetHitTable( const RESEARCH_COMMON_WORK* work )
{
  return work->touchHitTable;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̉�ʂ�o�^����
 *
 * @param work
 * @param seq  �o�^�����ʂ̎��ʎq
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_SetNowSeq( RESEARCH_COMMON_WORK* work, RADAR_SEQ seq )
{
  // ���O�̉�ʂ��X�V
  work->prevSeq = work->nowSeq;

  // ���݂̉�ʂ��X�V
  work->nowSeq = seq;
}

//-------------------------------------------------------------------------------
/**
 * @brief ��ʑJ�ڂ̃g���K��o�^����
 *
 * @param work
 * @param trig
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_SetSeqChangeTrig( RESEARCH_COMMON_WORK* work, SEQ_CHANGE_TRIG trig )
{
  work->seqTrig = trig;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̉�ʂ��擾����
 *
 * @param work
 *
 * @return ���݂̉�ʂ�\�����ʎq
 */
//-------------------------------------------------------------------------------
RADAR_SEQ RESEARCH_COMMON_GetNowSeq( const RESEARCH_COMMON_WORK* work )
{
  return work->nowSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���O�̉�ʂ��擾����
 *
 * @param work
 *
 * @return ���O�̉�ʂ�\�����ʎq
 */
//-------------------------------------------------------------------------------
RADAR_SEQ RESEARCH_COMMON_GetPrevSeq( const RESEARCH_COMMON_WORK* work )
{
  return work->prevSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief ��ʑJ�ڂ̃g���K���擾����
 *
 * @paramw work
 *
 * @return ��ʑJ�ڂ̈������ƂȂ����g���K
 */
//-------------------------------------------------------------------------------
SEQ_CHANGE_TRIG RESEARCH_COMMON_GetSeqChangeTrig( const RESEARCH_COMMON_WORK* work )
{
  return work->seqTrig;
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_UpdatePaletteAnime( RESEARCH_COMMON_WORK* work )
{
  UpdatePaletteAnime( work );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 * @param index �J�n����A�j���[�V����
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_StartPaletteAnime( 
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  StartPaletteAnime( work, index );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~���� ( �ʎw�� ) 
 *
 * @param work
 * @param index ��~����A�j���[�V����
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_StopPaletteAnime( 
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{ 
  StopPaletteAnime( work, index );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~���� ( �S�w�� ) 
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_StopAllPaletteAnime( RESEARCH_COMMON_WORK* work )
{ 
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  { 
    StopPaletteAnime( work, idx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �ʎw�� )
 *
 * @param work
 * @param index ���Z�b�g�Ώۂ̃A�j���[�V����
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_ResetPalette(
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  ResetPalette( work, index );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �S�w�� )
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_ResetAllPalette( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  { 
    ResetPalette( work, idx );
  }
}




//===============================================================================
// ���������E�����E�j��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitCommonWork( RESEARCH_COMMON_WORK* work )
{
  // ������
  GFL_STD_MemClear( work, sizeof(RESEARCH_COMMON_WORK) );

  work->prevSeq = RADAR_SEQ_NULL;
  work->nowSeq = RADAR_SEQ_NULL;

  InitOBJResources( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitPaletteFadeSystem( work );
  InitPaletteAnime( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init common work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N�𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return ���������S��ʋ��ʃ��[�N
 */
//-------------------------------------------------------------------------------
static RESEARCH_COMMON_WORK* CreateCommonWork( HEAPID heapID )
{
  RESEARCH_COMMON_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_COMMON_WORK) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create common work\n" );

  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteCommonWork( RESEARCH_COMMON_WORK* work )
{
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete common work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N���Z�b�g�A�b�v����
 *
 * @param work
 * @param gameSystem
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void SetupCommonWork( RESEARCH_COMMON_WORK* work, GAMESYS_WORK* gameSystem, HEAPID heapID )
{
  work->heapID     = heapID;
  work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem );

  CreateClactSystem( work );       // �Z���A�N�^�[�V�X�e�����쐬
  RegisterOBJResources( work );    // OBJ���\�[�X��o�^
  CreateClactUnits( work );        // �Z���A�N�^�[���j�b�g�𐶐�
  CreateClactWorks( work );        // �Z���A�N�^�[���[�N�𐶐�
  CreatePaletteFadeSystem( work ); // �p���b�g�Ǘ��V�X�e����k����
  SetupPaletteFadeSystem( work );  // �p���b�g�Ǘ��V�X�e�����Z�b�g�A�b�v
  SetupTouchArea( work );          // �^�b�`�͈͂��Z�b�g�A�b�v
  CreatePaletteAnime( work );      // �p���b�g�A�j���[�V�������[�N�𐶐�
  SetupPaletteAnime( work );       // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: setup common work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N���N���[���A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpCommonWork( RESEARCH_COMMON_WORK* work )
{
  CleanUpPaletteAnime( work );      // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v
  DeletePaletteAnime( work );       // �p���b�g�A�j���[�V�������[�N��j��
  CleanUpPaletteFadeSystem( work ); // �p���b�g�Ǘ��V�X�e�����N���[���A�b�v
  DeletePaletteFadeSystem( work );  // �p���b�g�Ǘ��V�X�e����j��
  DeleteClactWorks( work );         // �Z���A�N�^�[���[�N��j��
  DeleteClactUnits( work );         // �Z���A�N�^�[���j�b�g��j��
  ReleaseOBJResources( work );      // OBJ���\�[�X�����
  DeleteClactSystem( work );        // �A�N�^�[�V�X�e����j��

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: clean up common work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �A�N�^�[�V�X�e��������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactSystem( RESEARCH_COMMON_WORK* work )
{
  // �V�X�e���쐬
  GFL_CLACT_SYS_Create( &CommonClactSystemInitData, &VRAMBankSettings, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create clact system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactSystem( RESEARCH_COMMON_WORK* work )
{ 
  // �V�X�e���j��
  GFL_CLACT_SYS_Delete();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete clact system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃��\�[�X������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitOBJResources( RESEARCH_COMMON_WORK* work )
{
  int i;

  for( i=0; i<COMMON_OBJ_RESOURCE_NUM; i++ )
  {
    work->OBJResRegisterIdx[i] = 0; 
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init OBJ resources\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃��\�[�X��o�^����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void RegisterOBJResources( RESEARCH_COMMON_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, iconPalette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, heapID );

  // ���\�[�X��o�^
  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_app_menu_common_bar_button_128k_NCGR, 
                                      FALSE, CLSYS_DRAW_MAIN, heapID ); 

  // ���ʑf�� ( �A�C�R�� ) �̃p���b�g
  iconPalette = GFL_CLGRP_PLTT_RegisterEx( arcHandle, 
                                       NARC_app_menu_common_bar_button_NCLR,
                                       CLSYS_DRAW_MAIN, 
                                       ONE_PALETTE_SIZE*0, 0, 4, 
                                       heapID );

  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_app_menu_common_bar_button_32k_NCER,
                                           NARC_app_menu_common_bar_button_32k_NANR,
                                           heapID ); 
  // �o�^�C���f�b�N�X���L��
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CHARACTER ]      = character;
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_PALETTE_ICON ]   = iconPalette;
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CELL_ANIME ]     = cellAnime;

  GFL_ARC_CloseDataHandle( arcHandle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: register OBJ resources\n" );
}

//-------------------------------------------------------------------------------
/**
 * @breif ����OBJ�̃��\�[�X���������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void ReleaseOBJResources( RESEARCH_COMMON_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_PALETTE_ICON ] );
  GFL_CLGRP_CELLANIM_Release( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CELL_ANIME ] );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: release OBJ resources\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitClactUnits( RESEARCH_COMMON_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init clact units\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g�𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactUnits( RESEARCH_COMMON_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    u16 workNum;
    u8 priority;

    // ���d����
    GF_ASSERT( work->clactUnit[ unitIdx ] == NULL );

    workNum  = CommonClactUnitWorkSize[ unitIdx ];
    priority = CommonClactUnitPriority[ unitIdx ];
    work->clactUnit[ unitIdx ] = GFL_CLACT_UNIT_Create( workNum, priority, work->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMON: create clact units\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactUnits( RESEARCH_COMMON_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete clact units\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitClactWorks( RESEARCH_COMMON_WORK* work )
{
  int workIdx;

  // ������
  for( workIdx=0; workIdx < COMMON_CLWK_NUM; workIdx++ )
  {
    work->clactWork[ workIdx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init clact works\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N�𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactWorks( RESEARCH_COMMON_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < COMMON_CLWK_NUM; wkIdx++ )
  {
    GFL_CLUNIT* unit;
    GFL_CLWK_DATA wkData;
    u32 charaIdx, paletteIdx, cellAnimeIdx;
    u16 surface;

    // ���d����
    GF_ASSERT( work->clactWork[ wkIdx ] == NULL );

    // �����p�����[�^�I��
    wkData.pos_x   = CommonClactWorkInitData[ wkIdx ].posX;
    wkData.pos_y   = CommonClactWorkInitData[ wkIdx ].posY;
    wkData.anmseq  = CommonClactWorkInitData[ wkIdx ].animeSeq;
    wkData.softpri = CommonClactWorkInitData[ wkIdx ].softPriority; 
    wkData.bgpri   = CommonClactWorkInitData[ wkIdx ].BGPriority; 
    unit           = GetClactUnit( work, CommonClactWorkInitData[ wkIdx ].unitIdx );
    charaIdx       = GetOBJResRegisterIndex( work, CommonClactWorkInitData[ wkIdx ].characterResID );
    paletteIdx     = GetOBJResRegisterIndex( work, CommonClactWorkInitData[ wkIdx ].paletteResID );
    cellAnimeIdx   = GetOBJResRegisterIndex( work, CommonClactWorkInitData[ wkIdx ].cellAnimeResID );
    surface        = CommonClactWorkInitData[ wkIdx ].setSurface;

    // ����
    work->clactWork[ wkIdx ] = GFL_CLACT_WK_Create( 
        unit, charaIdx, paletteIdx, cellAnimeIdx, &wkData, surface, work->heapID );

    // ��\���ɐݒ�
    //GFL_CLACT_WK_SetDrawEnable( work->clactWork[ wkIdx ], FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clactWork[ wkIdx ], TRUE );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create clact works\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactWorks( RESEARCH_COMMON_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < COMMON_CLWK_NUM; wkIdx++ )
  {
    // ��������Ă��Ȃ�
    GF_ASSERT( work->clactWork[ wkIdx ] );

    // �j��
    GFL_CLACT_WK_Remove( work->clactWork[ wkIdx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete clact works\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RESEARCH_COMMON_WORK* work )
{
  work->paletteFadeSystem = NULL;

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init palette fade system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreatePaletteFadeSystem( RESEARCH_COMMON_WORK* work )
{
  GF_ASSERT( work->paletteFadeSystem == NULL );

  work->paletteFadeSystem = PaletteFadeInit( work->heapID );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create palette fade system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e����j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeletePaletteFadeSystem( RESEARCH_COMMON_WORK* work )
{
  GF_ASSERT( work->paletteFadeSystem );

  PaletteFadeFree( work->paletteFadeSystem );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete palette fade system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e�����Z�b�g�A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RESEARCH_COMMON_WORK* work )
{
  PALETTE_FADE_PTR fadeSystem;
  
  fadeSystem = work->paletteFadeSystem;

  // ���[�N���m��
  PaletteFadeWorkAllocSet( fadeSystem, FADE_MAIN_BG,  FULL_PALETTE_SIZE, work->heapID );
  PaletteFadeWorkAllocSet( fadeSystem, FADE_MAIN_OBJ, FULL_PALETTE_SIZE, work->heapID );
  PaletteFadeWorkAllocSet( fadeSystem, FADE_SUB_BG,   FULL_PALETTE_SIZE, work->heapID );
  PaletteFadeWorkAllocSet( fadeSystem, FADE_SUB_OBJ,  FULL_PALETTE_SIZE, work->heapID );

  // ���[�N��������
  PaletteWorkSet_VramCopy( fadeSystem, FADE_MAIN_BG,  0, FULL_PALETTE_SIZE );
  PaletteWorkSet_VramCopy( fadeSystem, FADE_MAIN_OBJ, 0, FULL_PALETTE_SIZE );
  PaletteWorkSet_VramCopy( fadeSystem, FADE_SUB_BG,   0, FULL_PALETTE_SIZE );
  PaletteWorkSet_VramCopy( fadeSystem, FADE_SUB_OBJ,  0, FULL_PALETTE_SIZE );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: setup palette fade system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e�����N���[���A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpPaletteFadeSystem( RESEARCH_COMMON_WORK* work )
{
  PALETTE_FADE_PTR fadeSystem;
  
  fadeSystem = work->paletteFadeSystem;

  // ���[�N��j��
  PaletteFadeWorkAllocFree( fadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_MAIN_OBJ );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_SUB_BG );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_SUB_OBJ );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: clean up palette fade system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitPaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: init palette anime\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreatePaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL ); // ���d����

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create palette anime\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeletePaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Delete( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete palette anime\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupPaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Setup( work->paletteAnime[ idx ],
                         CommonPaletteAnimeData[ idx ].destAdrs,
                         CommonPaletteAnimeData[ idx ].srcAdrs,
                         CommonPaletteAnimeData[ idx ].colorNum);
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: setup palette anime\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: clean up palette anime\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 * @param index �J�n����A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteAnime( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       CommonPaletteAnimeData[ index ].animeType,
                       CommonPaletteAnimeData[ index ].fadeColor );
  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: start palette anime [%d]\n", index );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 * @param index ��~����A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void StopPaletteAnime( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: stop palette anime [%d]\n", index );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g���A�j���J�n���̏�ԂɃ��Z�b�g����
 *
 * @param work
 * @param index ���Z�b�g�Ώۂ̃A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void ResetPalette( RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{ 
  PALETTE_ANIME_Reset( work->paletteAnime[ index ] );

  // DEBUG;
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: reset palette [%d]\n", index );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Update( work->paletteAnime[ idx ] );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �^�b�`�͈͂��Z�b�g�A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupTouchArea( RESEARCH_COMMON_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_TOUCH_AREA_NUM; idx++ )
  {
    work->touchHitTable[ idx ].rect.left   = CommonTouchAreaInitData[ idx ].left;
    work->touchHitTable[ idx ].rect.right  = CommonTouchAreaInitData[ idx ].right;
    work->touchHitTable[ idx ].rect.top    = CommonTouchAreaInitData[ idx ].top;
    work->touchHitTable[ idx ].rect.bottom = CommonTouchAreaInitData[ idx ].bottom;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: setup touch area\n" );
}




//===============================================================================
// ���擾
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief OBJ ���\�[�X�̓o�^�C���f�b�N�X���擾����
 *
 * @param work
 * @param resID ���\�[�XID
 *
 * @return �w�肵�����\�[�X�̓o�^�C���f�b�N�X
 */
//-------------------------------------------------------------------------------
static u32 GetOBJResRegisterIndex( const RESEARCH_COMMON_WORK* work, COMMON_OBJ_RESOURCE_ID resID )
{
  return work->OBJResRegisterIdx[ resID ];
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g���擾����
 *
 * @param work
 * @param unitIdx �Z���A�N�^�[���j�b�g�̃C���f�b�N�X
 *  
 * @return �w�肵���Z���A�N�^�[���j�b�g
 */
//-------------------------------------------------------------------------------
static GFL_CLUNIT* GetClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
{
  return work->clactUnit[ unitIdx ];
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N���擾����
 *
 * @param work
 * @param workIdx �Z���A�N�^�[���[�N�̃C���f�b�N�X
 *  
 * @return �w�肵���Z���A�N�^�[���[�N
 */
//-------------------------------------------------------------------------------
static GFL_CLWK* GetClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return work->clactWork[ workIdx ];
}
