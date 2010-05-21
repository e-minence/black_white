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
struct _RESEARCH_RADAR_COMMON_WORK {

  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;
  GAMEDATA*     gameData;

  // �^�b�`�͈�
  GFL_UI_TP_HITTBL touchHitTable[ COMMON_TOUCH_AREA_NUM ]; 
  
  // OBJ
  u32         OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_NUM ]; // ���ʃ��\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ COMMON_CLUNIT_NUM ];               // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ COMMON_CLWK_NUM ];                 // �Z���A�N�^�[���[�N

  // �J���[�p���b�g
  PALETTE_FADE_PTR paletteFadeSystem; // �J���[�p���b�g �t�F�[�h�V�X�e��

  // �p���b�g�A�j���[�V����
  PALETTE_ANIME* paletteAnime[ COMMON_PALETTE_ANIME_NUM ];

  // ��ʑJ��
  RADAR_SEQ prevSeq; // ���O�̉��
  RADAR_SEQ nowSeq;  // ���݂̉��
  SEQ_CHANGE_TRIG seqTrig; // ��ʑJ�ڂ̈������ƂȂ����g���K

  // ��ʂ̕��A�f�[�^
};

//-------------------------------------------------------------------------------
// ���������E�����E�j��
//-------------------------------------------------------------------------------
// �S��ʋ��ʃ��[�N
static void InitCommonWork( RRC_WORK* work ); // �S��ʋ��ʃ��[�N������������
static RRC_WORK* CreateCommonWork( HEAPID heapID ); // �S��ʋ��ʃ��[�N�𐶐�����
static void DeleteCommonWork( RRC_WORK* work ); // �S��ʋ��ʃ��[�N��j������
static void SetupCommonWork( RRC_WORK* work, GAMESYS_WORK* gameSystem, HEAPID heapID ); // �S��ʋ��ʃ��[�N���Z�b�g�A�b�v����
static void CleanUpCommonWork( RRC_WORK* work ); // �S��ʋ��ʃ��[�N���N���[���A�b�v����
// �Z���A�N�^�[�V�X�e��
static void CreateClactSystem( RRC_WORK* work ); // �A�N�^�[�V�X�e��������������
static void DeleteClactSystem( RRC_WORK* work ); // �A�N�^�[�V�X�e����j������
// OBJ ���\�[�X
static void InitOBJResources( RRC_WORK* work ); // ����OBJ�̃��\�[�X������������
static void RegisterOBJResources( RRC_WORK* work ); // ����OBJ�̃��\�[�X��o�^����
static void ReleaseOBJResources( RRC_WORK* work ); // ����OBJ�̃��\�[�X���������
// �Z���A�N�^�[���j�b�g
static void InitClactUnits( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g������������
static void CreateClactUnits( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteClactUnits( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g��j������
// �Z���A�N�^�[���[�N
static void InitClactWorks( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N������������
static void CreateClactWorks( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N�𐶐�����
static void DeleteClactWorks( RRC_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N��j������
// �p���b�g�t�F�[�h�V�X�e��
static void InitPaletteFadeSystem( RRC_WORK* work ); // �p���b�g�t�F�[�h�V�X�e��������������
static void CreatePaletteFadeSystem( RRC_WORK* work ); // �p���b�g�t�F�[�h�V�X�e���𐶐�����
static void DeletePaletteFadeSystem( RRC_WORK* work ); // �p���b�g�t�F�[�h�V�X�e����j������
static void SetupPaletteFadeSystem( RRC_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����Z�b�g�A�b�v����
static void CleanUpPaletteFadeSystem( RRC_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����N���[���A�b�v����
// �p���b�g�A�j���[�V����
static void InitPaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������[�N������������
static void CreatePaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������[�N�𐶐�����
static void DeletePaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������[�N��j������
static void SetupPaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
static void CleanUpPaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
static void StartPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������J�n����
static void StopPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index ); // �p���b�g�A�j���[�V�������~����
static void ResetPalette( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index  ); // �p���b�g���A�j���J�n���̏�ԂɃ��Z�b�g����
static void UpdatePaletteAnime( RRC_WORK* work ); // �p���b�g�A�j���[�V�������X�V����
// �^�b�`�͈�
static void SetupTouchArea( RRC_WORK* work ); // �^�b�`�͈͂��Z�b�g�A�b�v����
//-------------------------------------------------------------------------------
// ���擾
//-------------------------------------------------------------------------------
static u32 GetOBJResRegisterIndex( const RRC_WORK* work, COMMON_OBJ_RESOURCE_ID resID ); // ����OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetClactUnit( const RRC_WORK* work, COMMON_CLUNIT_INDEX unitIdx ); // ����OBJ�̃Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetClactWork( const RRC_WORK* work, COMMON_CLWK_INDEX workIdx ); // ����OBJ�̃Z���A�N�^�[���[�N���擾����



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
RRC_WORK* RRC_CreateWork( HEAPID heapID, GAMESYS_WORK* gameSystem )
{
  RRC_WORK* work;

  work = CreateCommonWork( heapID );           // ����
  InitCommonWork( work );                      // ������
  SetupCommonWork( work, gameSystem, heapID ); // �Z�b�g�A�b�v

  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ʃ��[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RRC_DeleteWork( RRC_WORK* work )
{
  CleanUpCommonWork( work ); // �N���[���A�b�v
  DeleteCommonWork( work );  // �j��
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h ( �u���b�N�E�A�E�g ) ���J�n����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RRC_StartPaletteFadeBlackOut( RRC_WORK* work )
{
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h ( �u���b�N�E�C�� ) ���J�n����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RRC_StartPaletteFadeBlackIn( RRC_WORK* work )
{
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
HEAPID RRC_GetHeapID( const RRC_WORK* work )
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
GAMESYS_WORK* RRC_GetGameSystem( const RRC_WORK* work )
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
GAMEDATA* RRC_GetGameData( const RRC_WORK* work )
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
GFL_CLUNIT* RRC_GetClactUnit( const RRC_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
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
GFL_CLWK* RRC_GetClactWork( const RRC_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return GetClactWork( work, workIdx );
}

//-------------------------------------------------------------------------------
/** * @brief ���ʃp���b�g�t�F�[�h�V�X�e�����擾����
 *
 * @param work
 *
 * @return ���ʃp���b�g�t�F�[�h�V�X�e��
 */
//-------------------------------------------------------------------------------
PALETTE_FADE_PTR RRC_GetPaletteFadeSystem( const RRC_WORK* work )
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
const GFL_UI_TP_HITTBL* RRC_GetHitTable( const RRC_WORK* work )
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
void RRC_SetNowSeq( RRC_WORK* work, RADAR_SEQ seq )
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
void RRC_SetSeqChangeTrig( RRC_WORK* work, SEQ_CHANGE_TRIG trig )
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
RADAR_SEQ RRC_GetNowSeq( const RRC_WORK* work )
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
RADAR_SEQ RRC_GetPrevSeq( const RRC_WORK* work )
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
SEQ_CHANGE_TRIG RRC_GetSeqChangeTrig( const RRC_WORK* work )
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
void RRC_UpdatePaletteAnime( RRC_WORK* work )
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
void RRC_StartPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
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
void RRC_StopPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
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
void RRC_StopAllPaletteAnime( RRC_WORK* work )
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
void RRC_ResetPalette(
    RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
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
void RRC_ResetAllPalette( RRC_WORK* work )
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
static void InitCommonWork( RRC_WORK* work )
{
  // ������
  GFL_STD_MemClear( work, sizeof(RRC_WORK) );

  work->prevSeq = RADAR_SEQ_NULL;
  work->nowSeq = RADAR_SEQ_NULL;

  InitOBJResources( work );
  InitClactUnits( work );
  InitClactWorks( work );
  InitPaletteFadeSystem( work );
  InitPaletteAnime( work );
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
static RRC_WORK* CreateCommonWork( HEAPID heapID )
{
  RRC_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(RRC_WORK) );

  return work;
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteCommonWork( RRC_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
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
static void SetupCommonWork( RRC_WORK* work, GAMESYS_WORK* gameSystem, HEAPID heapID )
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
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N������������S��ʋ��ʃ��[�N���N���[���A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpCommonWork( RRC_WORK* work )
{
  CleanUpPaletteAnime( work );      // �p���b�g�A�j���[�V�������[�N���N���[���A�b�v
  DeletePaletteAnime( work );       // �p���b�g�A�j���[�V�������[�N��j��
  CleanUpPaletteFadeSystem( work ); // �p���b�g�Ǘ��V�X�e�����N���[���A�b�v
  DeletePaletteFadeSystem( work );  // �p���b�g�Ǘ��V�X�e����j��
  DeleteClactWorks( work );         // �Z���A�N�^�[���[�N��j��
  DeleteClactUnits( work );         // �Z���A�N�^�[���j�b�g��j��
  ReleaseOBJResources( work );      // OBJ���\�[�X�����
  DeleteClactSystem( work );        // �A�N�^�[�V�X�e����j��
}

//-------------------------------------------------------------------------------
/**
 * @brief �A�N�^�[�V�X�e��������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactSystem( RRC_WORK* work )
{
  // �V�X�e���쐬
  GFL_CLACT_SYS_Create( &CommonClactSystemInitData, &VRAMBankSettings, work->heapID );
}

//-------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactSystem( RRC_WORK* work )
{ 
  // �V�X�e���j��
  GFL_CLACT_SYS_Delete();
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃��\�[�X������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitOBJResources( RRC_WORK* work )
{
  int i;

  for( i=0; i<COMMON_OBJ_RESOURCE_NUM; i++ )
  {
    work->OBJResRegisterIdx[i] = 0; 
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃��\�[�X��o�^����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void RegisterOBJResources( RRC_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, iconPalette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GetHeapLowID(heapID) );

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
}

//-------------------------------------------------------------------------------
/**
 * @breif ����OBJ�̃��\�[�X���������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void ReleaseOBJResources( RRC_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_PALETTE_ICON ] );
  GFL_CLGRP_CELLANIM_Release( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CELL_ANIME ] );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitClactUnits( RRC_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    work->clactUnit[ unitIdx ] = NULL;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g�𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactUnits( RRC_WORK* work )
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
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���j�b�g��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactUnits( RRC_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    GF_ASSERT( work->clactUnit[ unitIdx ] );
    GFL_CLACT_UNIT_Delete( work->clactUnit[ unitIdx ] );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitClactWorks( RRC_WORK* work )
{
  int workIdx;

  // ������
  for( workIdx=0; workIdx < COMMON_CLWK_NUM; workIdx++ )
  {
    work->clactWork[ workIdx ] = NULL;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N�𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateClactWorks( RRC_WORK* work )
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

    GFL_CLACT_WK_SetDrawEnable( work->clactWork[ wkIdx ], TRUE );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ�̃Z���A�N�^�[���[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteClactWorks( RRC_WORK* work )
{
  int wkIdx;

  for( wkIdx=0; wkIdx < COMMON_CLWK_NUM; wkIdx++ )
  {
    // ��������Ă��Ȃ�
    GF_ASSERT( work->clactWork[ wkIdx ] );

    // �j��
    GFL_CLACT_WK_Remove( work->clactWork[ wkIdx ] );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e��������������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void InitPaletteFadeSystem( RRC_WORK* work )
{
  work->paletteFadeSystem = NULL;
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e���𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreatePaletteFadeSystem( RRC_WORK* work )
{
  GF_ASSERT( work->paletteFadeSystem == NULL );

  work->paletteFadeSystem = PaletteFadeInit( work->heapID );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e����j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeletePaletteFadeSystem( RRC_WORK* work )
{
  GF_ASSERT( work->paletteFadeSystem );

  PaletteFadeFree( work->paletteFadeSystem );
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e�����Z�b�g�A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SetupPaletteFadeSystem( RRC_WORK* work )
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
}

//-------------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�V�X�e�����N���[���A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CleanUpPaletteFadeSystem( RRC_WORK* work )
{
  PALETTE_FADE_PTR fadeSystem;
  
  fadeSystem = work->paletteFadeSystem;

  // ���[�N��j��
  PaletteFadeWorkAllocFree( fadeSystem, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_MAIN_OBJ );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_SUB_BG );
  PaletteFadeWorkAllocFree( fadeSystem, FADE_SUB_OBJ );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void InitPaletteAnime( RRC_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    work->paletteAnime[ idx ] = NULL;
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CreatePaletteAnime( RRC_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] == NULL ); // ���d����

    work->paletteAnime[ idx ] = PALETTE_ANIME_Create( work->heapID );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void DeletePaletteAnime( RRC_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    PALETTE_ANIME_Delete( work->paletteAnime[ idx ] );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void SetupPaletteAnime( RRC_WORK* work )
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
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void CleanUpPaletteAnime( RRC_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_PALETTE_ANIME_NUM; idx++ )
  {
    GF_ASSERT( work->paletteAnime[ idx ] );

    // ���삵�Ă����p���b�g�����ɖ߂�
    PALETTE_ANIME_Reset( work->paletteAnime[ idx ] );
  }
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������J�n����
 *
 * @param work
 * @param index �J�n����A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void StartPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Start( work->paletteAnime[ index ], 
                       CommonPaletteAnimeData[ index ].animeType,
                       CommonPaletteAnimeData[ index ].fadeColor );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������~����
 *
 * @param work
 * @param index ��~����A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void StopPaletteAnime( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{
  PALETTE_ANIME_Stop( work->paletteAnime[ index ] );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g���A�j���J�n���̏�ԂɃ��Z�b�g����
 *
 * @param work
 * @param index ���Z�b�g�Ώۂ̃A�j���[�V����
 */
//----------------------------------------------------------------------------------------------
static void ResetPalette( RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index )
{ 
  PALETTE_ANIME_Reset( work->paletteAnime[ index ] );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
static void UpdatePaletteAnime( RRC_WORK* work )
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
static void SetupTouchArea( RRC_WORK* work )
{
  int idx;

  for( idx=0; idx < COMMON_TOUCH_AREA_NUM; idx++ )
  {
    work->touchHitTable[ idx ].rect.left   = CommonTouchAreaInitData[ idx ].left;
    work->touchHitTable[ idx ].rect.right  = CommonTouchAreaInitData[ idx ].right;
    work->touchHitTable[ idx ].rect.top    = CommonTouchAreaInitData[ idx ].top;
    work->touchHitTable[ idx ].rect.bottom = CommonTouchAreaInitData[ idx ].bottom;
  }
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
static u32 GetOBJResRegisterIndex( const RRC_WORK* work, COMMON_OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetClactUnit( const RRC_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetClactWork( const RRC_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return work->clactWork[ workIdx ];
}
