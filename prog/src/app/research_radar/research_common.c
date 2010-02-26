/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʒ�`
 * @file   research_common.c
 * @author obata
 * @date   2010.02.13
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "research_common.h"
#include "research_common_def.h"
#include "research_common_data.cdat"

#include "gamesystem/gamesystem.h"       // for GAMESYS_WORK

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

  u32         OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_NUM ]; // ���ʃ��\�[�X�̓o�^�C���f�b�N�X
  GFL_CLUNIT* clactUnit[ COMMON_CLUNIT_NUM ];               // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ COMMON_CLWK_NUM ];                 // �Z���A�N�^�[���[�N
};

//-------------------------------------------------------------------------------
// �������E�����E�j��
//-------------------------------------------------------------------------------
// OBJ
static void CreateClactSystem( RESEARCH_COMMON_WORK* work ); // �A�N�^�[�V�X�e��������������
static void DeleteClactSystem( RESEARCH_COMMON_WORK* work ); // �A�N�^�[�V�X�e����j������
static void InitCommonOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X������������
static void RegisterCommonOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X��o�^����
static void ReleaseCommonOBJResources( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃��\�[�X���������
static void InitCommonClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g������������
static void CreateCommonClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g�𐶐�����
static void DeleteCommonClactUnits( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���j�b�g��j������
static void InitCommonClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N������������
static void CreateCommonClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N�𐶐�����
static void DeleteCommonClactWorks( RESEARCH_COMMON_WORK* work ); // ����OBJ�̃Z���A�N�^�[���[�N��j������

//-------------------------------------------------------------------------------
// �擾
//-------------------------------------------------------------------------------
static u32 GetCommonOBJResRegisterIndex( const RESEARCH_COMMON_WORK* work, COMMON_OBJ_RESOURCE_ID resID ); // ����OBJ���\�[�X�̓o�^�C���f�b�N�X
static GFL_CLUNIT* GetCommonClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx ); // ����OBJ�̃Z���A�N�^�[���j�b�g���擾����
static GFL_CLWK* GetCommonClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx ); // ����OBJ�̃Z���A�N�^�[���[�N���擾����


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

  // ����
  work = GFL_HEAP_AllocMemory( heapID, sizeof(RESEARCH_COMMON_WORK) );

  // ������
  work->heapID     = heapID;
  work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem );
  InitCommonOBJResources( work );
  InitCommonClactUnits( work );
  InitCommonClactWorks( work );

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
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: delete work\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ���Z�b�g�A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_SetupCommonOBJ( RESEARCH_COMMON_WORK* work )
{
  CreateClactSystem( work );

  RegisterCommonOBJResources( work );
  CreateCommonClactUnits( work );
  CreateCommonClactWorks( work );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����OBJ���N���[���A�b�v����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
void RESEARCH_COMMON_CleanUpCommonOBJ( RESEARCH_COMMON_WORK* work )
{
  // �j��
  DeleteCommonClactWorks( work );
  DeleteCommonClactUnits( work );
  ReleaseCommonOBJResources( work );

  // ������
  InitCommonOBJResources( work );
  InitCommonClactUnits( work );
  InitCommonClactWorks( work );

  // �A�N�^�[�V�X�e���j��
  DeleteClactSystem( work );
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
GFL_CLUNIT* RESEARCH_COMMON_GetClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
{
  return GetCommonClactUnit( work, unitIdx );
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
GFL_CLWK* RESEARCH_COMMON_GetClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return GetCommonClactWork( work, workIdx );
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
  GFL_CLACT_SYS_Create( &ClactSystemInitData, &VRAMBankSettings, work->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-COMMON: create clact system\n" );
}

//----------------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e����j������
 *
 * @param work
 */
//----------------------------------------------------------------------------------------------
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
static void InitCommonOBJResources( RESEARCH_COMMON_WORK* work )
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
static void RegisterCommonOBJResources( RESEARCH_COMMON_WORK* work )
{
  HEAPID heapID;
  ARCHANDLE* arcHandle;
  u32 character, palette, cellAnime;

  heapID    = work->heapID;
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, heapID );

  // ���\�[�X��o�^
  character = GFL_CLGRP_CGR_Register( arcHandle, 
                                      NARC_app_menu_common_bar_button_128k_NCGR, 
                                      FALSE, CLSYS_DRAW_MAIN, heapID ); 

  palette = GFL_CLGRP_PLTT_Register( arcHandle, 
                                     NARC_app_menu_common_bar_button_NCLR,
                                     CLSYS_DRAW_MAIN, 0, heapID );

  cellAnime = GFL_CLGRP_CELLANIM_Register( arcHandle,
                                           NARC_app_menu_common_bar_button_32k_NCER,
                                           NARC_app_menu_common_bar_button_32k_NANR,
                                           heapID ); 
  // �o�^�C���f�b�N�X���L��
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CHARACTER ]  = character;
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_PALETTE ]    = palette;
  work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CELL_ANIME ] = cellAnime;

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
static void ReleaseCommonOBJResources( RESEARCH_COMMON_WORK* work )
{
  GFL_CLGRP_CGR_Release     ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_CHARACTER ] );
  GFL_CLGRP_PLTT_Release    ( work->OBJResRegisterIdx[ COMMON_OBJ_RESOURCE_MAIN_PALETTE ] );
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
static void InitCommonClactUnits( RESEARCH_COMMON_WORK* work )
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
static void CreateCommonClactUnits( RESEARCH_COMMON_WORK* work )
{
  int unitIdx;

  for( unitIdx=0; unitIdx < COMMON_CLUNIT_NUM; unitIdx++ )
  {
    u16 workNum;
    u8 priority;

    // ���d����
    GF_ASSERT( work->clactUnit[ unitIdx ] == NULL );

    workNum  = ClactUnitWorkSize[ unitIdx ];
    priority = ClactUnitPriority[ unitIdx ];
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
static void DeleteCommonClactUnits( RESEARCH_COMMON_WORK* work )
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
static void InitCommonClactWorks( RESEARCH_COMMON_WORK* work )
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
static void CreateCommonClactWorks( RESEARCH_COMMON_WORK* work )
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
    wkData.pos_x   = ClactWorkInitData[ wkIdx ].posX;
    wkData.pos_y   = ClactWorkInitData[ wkIdx ].posY;
    wkData.anmseq  = ClactWorkInitData[ wkIdx ].animeSeq;
    wkData.softpri = ClactWorkInitData[ wkIdx ].softPriority; 
    wkData.bgpri   = ClactWorkInitData[ wkIdx ].BGPriority; 
    unit           = GetCommonClactUnit( work, ClactWorkInitData[ wkIdx ].unitIdx );
    charaIdx       = GetCommonOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].characterResID );
    paletteIdx     = GetCommonOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].paletteResID );
    cellAnimeIdx   = GetCommonOBJResRegisterIndex( work, ClactWorkInitData[ wkIdx ].cellAnimeResID );
    surface        = ClactWorkInitData[ wkIdx ].setSurface;

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
static void DeleteCommonClactWorks( RESEARCH_COMMON_WORK* work )
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
static u32 GetCommonOBJResRegisterIndex( const RESEARCH_COMMON_WORK* work, COMMON_OBJ_RESOURCE_ID resID )
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
static GFL_CLUNIT* GetCommonClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx )
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
static GFL_CLWK* GetCommonClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx )
{
  return work->clactWork[ workIdx ];
}
