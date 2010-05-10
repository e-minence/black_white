/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[
 * @file   research_main.c
 * @author obata
 * @date   2010.02.02
 */
///////////////////////////////////////////////////////////////////////////////// 
#include "app/research_radar.h"
#include "research_common.h"
#include "research_common_def.h"
#include "research_common_data.cdat"
#include "research_top.h"
#include "research_list.h"
#include "research_graph.h"

#include "system/main.h"                    // for HEAPID_xxxx
#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_xxxx



//===============================================================================
// ���萔
//===============================================================================

// SUB-BG �w�i��
static const GFL_BG_BGCNT_HEADER SubBGControl_BACK = 
{
  0, 0,                    // �����\���ʒu
  0x800,                   // �X�N���[���o�b�t�@�T�C�Y
  0,                       // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,   // �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,      // �J���[���[�h
  GX_BG_SCRBASE_0x0000,    // �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x04000,  // �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,   // �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,        // BG�g���p���b�g�X���b�g�I��
  SUB_BG_BACK_PRIORITY,    // �\���v���C�I���e�B�[
  0,                       // �G���A�I�[�o�[�t���O
  0,                       // DUMMY
  FALSE,                   // ���U�C�N�ݒ�
}; 

// SUB-BG ���[�_�[��
static const GFL_BG_BGCNT_HEADER SubBGControl_RADAR = 
{
  0, 0,                    // �����\���ʒu
  0x800,                   // �X�N���[���o�b�t�@�T�C�Y
  0,                       // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,   // �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,      // �J���[���[�h
  GX_BG_SCRBASE_0x0800,    // �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x04000,  // �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,   // �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,        // BG�g���p���b�g�X���b�g�I��
  SUB_BG_RADAR_PRIORITY,   // �\���v���C�I���e�B�[
  0,                       // �G���A�I�[�o�[�t���O
  0,                       // DUMMY
  FALSE,                   // ���U�C�N�ݒ�
}; 

// MAIN-BG �w�i��
static const GFL_BG_BGCNT_HEADER MainBGControl_BACK = 
{
  0, 0,                    // �����\���ʒu
  0x800,                   // �X�N���[���o�b�t�@�T�C�Y
  0,                       // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,   // �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,      // �J���[���[�h
  GX_BG_SCRBASE_0x0000,    // �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x04000,  // �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,   // �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,        // BG�g���p���b�g�X���b�g�I��
  MAIN_BG_BACK_PRIORITY,   // �\���v���C�I���e�B�[
  0,                       // �G���A�I�[�o�[�t���O
  0,                       // DUMMY
  FALSE,                   // ���U�C�N�ݒ�
};


//===============================================================================
// ���e��v���Z�X �V�[�P���X�ԍ�
//===============================================================================
// ���C���v���Z�X
typedef enum{
  PROC_MAIN_SEQ_SETUP,   // ������ʏ���
  PROC_MAIN_SEQ_MENU,    // �����������
  PROC_MAIN_SEQ_SELECT,  // �������e�ύX���
  PROC_MAIN_SEQ_CHECK,   // �����񍐊m�F���
  PROC_MAIN_SEQ_FINISH,  // �I��
} PROC_MAIN_SEQ;


//===============================================================================
// ���������[�_�[ �v���Z�X ���[�N
//===============================================================================
typedef struct {

  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;

  // �t���[���J�E���^
  u32 frameCount;

  // �S��ʋ��ʃ��[�N
  RRC_WORK* commonWork;

  // �e��ʐ�p���[�N
  RRT_WORK*   topWork;   // �g�b�v���
  RRL_WORK* listWork; // ���X�g���
  RRG_WORK*  graphWork;  // �����񍐊m�F���

} RESEARCH_WORK;


//===============================================================================
// ��prototype
//===============================================================================
// �v���Z�X����
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcEnd ( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk );
// ���C���v���Z�X �V�[�P���X����
static void ChangeMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq );
static void SetMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
// ���C���v���Z�X �V�[�P���X����
static PROC_MAIN_SEQ ProcMain_SETUP( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_MENU( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_CHECK( RESEARCH_WORK* work );
// �t���[���J�E���^
static u32 GetFrameCount( const RESEARCH_WORK* work );
static void CountUpFrame( RESEARCH_WORK* work );
// �v���Z�X���[�N�̐����E�j��
static void InitProcWork( RESEARCH_WORK* work, GAMESYS_WORK* gameSystem );
static void CreateCommonWork( RESEARCH_WORK* work );
static void DeleteCommonWork( RESEARCH_WORK* work );
// BG �̃Z�b�g�A�b�v�E�N���[���A�b�v
static void SetupBG( HEAPID heapID );
static void CleanUpBG( void );
static void LoadSubBGResources( HEAPID heapID );
static void LoadMainBGResources( HEAPID heapID );
static void UpdateRingAnime( const RESEARCH_WORK* work );


//===============================================================================
// ���v���Z�X ��`�f�[�^
//===============================================================================
GFL_PROC_DATA ResearchRadarProcData = 
{
  ResearchRadarProcInit,
  ResearchRadarProcMain,
  ResearchRadarProcEnd,
};


//===============================================================================
// ���v���Z�X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �v���Z�X�������֐�
 *
 * @param proc
 * @param seq  �V�[�P���X���[�N
 * @param prm  �v���Z�X�Ăяo���p�����[�^
 * @param wk   �v���Z�X���[�N
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk )
{
  RESEARCH_PARAM* param = prm;
  RESEARCH_WORK*  work  = wk;

  switch( *seq ) {
    case 0:
      // �v���Z�X���[�N�m��
      GFL_PROC_AllocWork( proc, sizeof(RESEARCH_WORK), GFL_HEAPID_APP ); 
      (*seq)++;
      break;

    case 1: 
      // �v���Z�X���[�N��������
      InitProcWork( work, param->gameSystem );

      // �\������
      GFL_DISP_SetBank( &VRAMBankSettings );         // VRAM-Bank ���蓖��
      GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );  // ����ʂ����C��

      // BG����
      SetupBG( work->heapID );
      LoadMainBGResources( work->heapID ); 
      LoadSubBGResources ( work->heapID ); 

      // �S��ʋ��ʃ��[�N�𐶐�
      CreateCommonWork( work );

      return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


//-------------------------------------------------------------------------------
/**
 * @brief �v���Z�X�I���֐�
 *
 * @param proc
 * @param seq  �V�[�P���X���[�N
 * @param prm  �v���Z�X�Ăяo���p�����[�^
 * @param wk   �v���Z�X���[�N
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcEnd( GFL_PROC* proc, int* seq, void* prm, void* wk )
{
  RESEARCH_WORK* work = wk;

  DeleteCommonWork( work ); // �S��ʋ��ʃ��[�N��j��
  CleanUpBG(); // BG ���N���[���A�b�v
  GFL_PROC_FreeWork( proc ); // �v���Z�X���[�N��j��

  return GFL_PROC_RES_FINISH;
}


//-------------------------------------------------------------------------------
/**
 * @brief �v���Z�X ���C���֐�
 *
 * @param proc
 * @param seq  �V�[�P���X���[�N
 * @param prm  �v���Z�X�Ăяo���p�����[�^
 * @param wk   �v���Z�X���[�N
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk )
{ 
  RESEARCH_WORK* work = wk;
  PROC_MAIN_SEQ  next_seq;

  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   next_seq = ProcMain_SETUP ( work );  break;
  case PROC_MAIN_SEQ_MENU:    next_seq = ProcMain_MENU  ( work );  break;
  case PROC_MAIN_SEQ_SELECT:  next_seq = ProcMain_SELECT( work );  break;
  case PROC_MAIN_SEQ_CHECK:   next_seq = ProcMain_CHECK ( work );  break;
  case PROC_MAIN_SEQ_FINISH:  return GFL_PROC_RES_FINISH;
  default:  GF_ASSERT(0);
  }

  UpdateRingAnime( work ); // ���ʂ̃����O�A�j���[�V�������X�V
  CountUpFrame( work ); // �t���[���J�E���^�X�V

  // �V�[�P���X���X�V
  if( *seq != next_seq ) {
    ChangeMainProcSeq( work, seq, next_seq );
  }

  return GFL_PROC_RES_CONTINUE;
}


//===============================================================================
// �����C���v���Z�X �V�[�P���X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work    �v���Z�X���[�N
 * @param seq     �V�[�P���X���[�N
 * @param nextSeq ���̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static void ChangeMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{ 
  // ���݂̃V�[�P���X���I��
  EndCurrentMainProcSeq( work, seq );

  // ���̃V�[�P���X���J�n
  SetMainProcSeq( work, seq, nextSeq );

  // ��ʂ̑J�ڂ�o�^����
  switch( *seq ) {
  case PROC_MAIN_SEQ_MENU:    RRC_SetNowSeq( work->commonWork, RADAR_SEQ_MENU ); break;
  case PROC_MAIN_SEQ_SELECT:  RRC_SetNowSeq( work->commonWork, RADAR_SEQ_SELECT ); break;
  case PROC_MAIN_SEQ_CHECK:   RRC_SetNowSeq( work->commonWork, RADAR_SEQ_CHECK ); break;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: chenge seq ==> " );
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   OS_TFPrintf( PRINT_TARGET, "SETUP(%d)\n",  *seq );  break;
  case PROC_MAIN_SEQ_MENU:    OS_TFPrintf( PRINT_TARGET, "MENU(%d)\n",   *seq );  break;
  case PROC_MAIN_SEQ_SELECT:  OS_TFPrintf( PRINT_TARGET, "SELECT(%d)\n", *seq );  break;
  case PROC_MAIN_SEQ_CHECK:   OS_TFPrintf( PRINT_TARGET, "CHECK(%d)\n",  *seq );  break;
  case PROC_MAIN_SEQ_FINISH:  OS_TFPrintf( PRINT_TARGET, "FINISH(%d)\n", *seq );  break;
  default:  GF_ASSERT(0);
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̃��C���v���Z�X �V�[�P���X���I������
 *
 * @param work �v���Z�X���[�N
 * @param seq  �V�[�P���X���[�N
 */
//-------------------------------------------------------------------------------
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq )
{
  // ���݂̃V�[�P���X�̐�p���[�N��j��
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:  break;
  case PROC_MAIN_SEQ_MENU:   RRT_DeleteWork( work->topWork );    break;
  case PROC_MAIN_SEQ_SELECT: RRL_DeleteWork( work->listWork );  break;
  case PROC_MAIN_SEQ_CHECK:  RRG_DeleteWork ( work->graphWork );   break;
  case PROC_MAIN_SEQ_FINISH: break;
  default:  GF_ASSERT(0);
  }

  // ��p���[�N�|�C���^���N���A
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_MENU:    work->topWork   = NULL;  break;
  case PROC_MAIN_SEQ_SELECT:  work->listWork = NULL;  break;
  case PROC_MAIN_SEQ_CHECK:   work->graphWork  = NULL;  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: end current seq(%d)\n", *seq );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ݒ肷��
 *
 * @param work    �v���Z�X���[�N
 * @param seq     �V�[�P���X���[�N
 * @param nextSeq �ݒ肷��V�[�P���X
 */
//-------------------------------------------------------------------------------
static void SetMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{
  // ���d�����`�F�b�N
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_MENU:    GF_ASSERT( work->topWork   == NULL );  break;
  case PROC_MAIN_SEQ_SELECT:  GF_ASSERT( work->listWork == NULL );  break;
  case PROC_MAIN_SEQ_CHECK:   GF_ASSERT( work->graphWork  == NULL );  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  } 

  // ���̃V�[�P���X�̐�p���[�N�𐶐�
  switch( nextSeq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_MENU:    work->topWork   = RRT_CreateWork( work->commonWork );  break;
  case PROC_MAIN_SEQ_SELECT:  work->listWork = RRL_CreateWork( work->commonWork );  break;
  case PROC_MAIN_SEQ_CHECK:   work->graphWork  = RRG_CreateWork ( work->commonWork );  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  } 

  // �V�[�P���X��ύX
  *seq = nextSeq; 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: set seq(%d)\n", *seq );
}


//===============================================================================
// �����C���v���Z�X �V�[�P���X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���C���v���Z�X ������ʏ���
 *
 * @param work �v���Z�X���[�N
 *
 * @return ���̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_SETUP( RESEARCH_WORK* work )
{
  return PROC_MAIN_SEQ_MENU;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���C���v���Z�X �������[�_�[�������
 *
 * @param work �v���Z�X���[�N
 *
 * @return �������[�_�[������ʂ��I�������ꍇ ���̃V�[�P���X
 *         �����łȂ���� ���݂̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_MENU( RESEARCH_WORK* work )
{ 
  RRT_Main( work->topWork );

  // �g�b�v��ʂ��I��
  if( RRT_IsFinished( work->topWork ) ) {

    RRT_RESULT result = RRT_GetResult( work->topWork );

    switch( result ) {
    case RRT_RESULT_TO_LIST:  return PROC_MAIN_SEQ_SELECT;
    case RRT_RESULT_TO_GRAPH: return PROC_MAIN_SEQ_CHECK;
    case RRT_RESULT_EXIT:     return PROC_MAIN_SEQ_FINISH;
    default:
      GF_ASSERT(0);
      return PROC_MAIN_SEQ_FINISH;
    }
  } 

  return PROC_MAIN_SEQ_MENU;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���C���v���Z�X �������ڕύX���
 *
 * @param work �v���Z�X���[�N
 *
 * @return �������ڕύX��ʂ��I�������ꍇ ���̃V�[�P���X
 *         �����łȂ���� ���݂̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work )
{
  PROC_MAIN_SEQ  nextSeq;

  RRL_Main( work->listWork );

  // ���X�g��ʂ��I��
  if( RRL_IsFinished( work->listWork ) ) {

    RRL_RESULT result = RRL_GetResult( work->listWork );

    switch( result ) {
    case RRL_RESULT_TO_TOP: return PROC_MAIN_SEQ_MENU;
    default:
      GF_ASSERT(0);
      return PROC_MAIN_SEQ_FINISH;
    }
  }

  return PROC_MAIN_SEQ_SELECT;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���C���v���Z�X �����񍐊m�F���
 *
 * @param work �v���Z�X���[�N
 *
 * @return �����񍐊m�F��ʂ��I�������ꍇ ���̃V�[�P���X
 *         �����łȂ���� ���݂̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_CHECK( RESEARCH_WORK* work )
{
  RRG_Main( work->graphWork );

  // �O���t��ʂ��I��
  if( RRG_IsFinished( work->graphWork ) ) {

    RRG_RESULT result = RRG_GetResult( work->graphWork );

    switch( result ) {
    case RRG_RESULT_TO_TOP: return PROC_MAIN_SEQ_MENU;
    default:  
      GF_ASSERT(0);
      return PROC_MAIN_SEQ_FINISH;
    } 
  }

  return PROC_MAIN_SEQ_CHECK;
} 

//-------------------------------------------------------------------------------
/**
 * @brief �t���[���J�E���^�̒l���擾����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static u32 GetFrameCount( const RESEARCH_WORK* work )
{
  return work->frameCount;
}

//-------------------------------------------------------------------------------
/**
 * @brief �t���[���J�E���^���C���N�������g����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CountUpFrame( RESEARCH_WORK* work )
{
  work->frameCount++;
}

//===============================================================================
// ���v���Z�X���[�N
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �v���Z�X���[�N������������
 *
 * @param work
 * @param gameSystem
 */
//-------------------------------------------------------------------------------
static void InitProcWork( RESEARCH_WORK* work, GAMESYS_WORK* gameSystem )
{
  work->heapID     = GFL_HEAPID_APP;
  work->gameSystem = gameSystem;
  work->frameCount = 0;
  work->commonWork = NULL;
  work->topWork   = NULL;
  work->listWork = NULL;
  work->graphWork  = NULL;
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N�𐶐�����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateCommonWork( RESEARCH_WORK* work )
{
  GF_ASSERT( work->commonWork == NULL );

  work->commonWork = RRC_CreateWork( work->heapID, work->gameSystem );
}

//-------------------------------------------------------------------------------
/**
 * @brief �S��ʋ��ʃ��[�N��j������
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteCommonWork( RESEARCH_WORK* work )
{
  GF_ASSERT( work->commonWork );

  RRC_DeleteWork( work->commonWork );
  work->commonWork = NULL;
}

//===============================================================================
// ��BG
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief BG ����
 * 
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void SetupBG( HEAPID heapID )
{ 
  GFL_BG_Init( heapID );

  // BG���[�h
  GFL_BG_SetBGMode( &BGSysHeader2D );

  // SUB-BG
  GFL_BG_SetBGControl( SUB_BG_BACK,  &SubBGControl_BACK,  GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_RADAR, &SubBGControl_RADAR, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( SUB_BG_BACK,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_RADAR, VISIBLE_ON );

  // MAIN-BG
  GFL_BG_SetBGControl( MAIN_BG_BACK, &MainBGControl_BACK, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( MAIN_BG_BACK, VISIBLE_ON );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: setup BG\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief BG ��Еt��
 */
//-------------------------------------------------------------------------------
static void CleanUpBG()
{
  GFL_BG_FreeBGControl( MAIN_BG_BACK );
  GFL_BG_FreeBGControl( SUB_BG_BACK );
  GFL_BG_Exit();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: clean up BG\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief SUB-BG �̃��\�[�X��ǂݍ���
 *
 * @param heapID �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------
static void LoadSubBGResources( HEAPID heapID )
{ 
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID ); 

    // �p���b�g�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgu_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( SUB_BG_BACK, data->pRawData, FULL_PALETTE_SIZE, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �L�����N�^�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_graphic_bgu_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( SUB_BG_BACK, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �X�N���[���f�[�^ ( �w�i�� )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_BACK, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_BACK );
      GFL_HEAP_FreeMemory( src );
    }
    // �X�N���[���f�[�^ ( ���[�_�[�� )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_ring_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_RADAR, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_RADAR );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: load SUB-BG resources\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief SUB-BG ���[�_�[�ʂ̃p���b�g�A�j���[�V�������X�V����
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void UpdateRingAnime( const RESEARCH_WORK* work )
{ 
  u32 frame;
  u8 paletteOffset;
  u8 paletteNo;

  // �ݒ肷��p���b�g�ԍ�������
  frame         = GetFrameCount( work );
  paletteOffset = ( (frame / SUB_BG_RADAR_PLT_ANIME_FRAME) % SUB_BG_RADAR_PLT_NUM );
  paletteNo     = SUB_BG_RADAR_FIRST_PLT_IDX + paletteOffset;

  // �X�N���[���f�[�^���X�V
  GFL_BG_ChangeScreenPalette( SUB_BG_RADAR, 0, 0, 32, 24, paletteNo );
  GFL_BG_LoadScreenReq( SUB_BG_RADAR );
}

//-------------------------------------------------------------------------------
/**
 * @brief MAIN-BG �̃��\�[�X��ǂݍ���
 *
 * @param heapID �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------
static void LoadMainBGResources( HEAPID heapID )
{ 
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID ); 

    // �p���b�g�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgd_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( MAIN_BG_BACK, data->pRawData, FULL_PALETTE_SIZE, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �L�����N�^�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_graphic_bgd_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( MAIN_BG_BACK, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �X�N���[���f�[�^ ( �w�i�� )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgd_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_BACK, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( MAIN_BG_BACK );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: load MAIN-BG resources\n" );
}
