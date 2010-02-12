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
#include "research_test.h"
#include "research_menu.h"
#include "research_select.h"
#include "research_check.h"

#include "system/main.h"            // for HEAPID_xxxx
#include "arc/arc_def.h"            // for ARCID_xxxx
#include "arc/research_radar.naix"  // for NARC_xxxx


//===============================================================================
// ���e��v���Z�X �V�[�P���X�ԍ�
//===============================================================================
// �������v���Z�X
typedef enum{
  PROC_INIT_SEQ_ALLOC_WORK,     // �v���Z�X���[�N�m��
  PROC_INIT_SEQ_INIT_WORK,      // �v���Z�X���[�N������
  PROC_INIT_SEQ_SETUP_DISPLAY,  // �\������
  PROC_INIT_SEQ_SETUP_BG,       // BG ����
  PROC_INIT_SEQ_FINISH,         // �I��
} PROC_INIT_SEQ;

// �I���v���Z�X
typedef enum{
  PROC_END_SEQ_CLEAN_UP_BG, // BG��Еt��
  PROC_END_SEQ_FREE_WORK,   // �v���Z�X���[�N���
  PROC_END_SEQ_FINISH,      // �I��
} PROC_END_SEQ;

// ���C���v���Z�X
typedef enum{
  PROC_MAIN_SEQ_SETUP,   // ������ʏ���
  PROC_MAIN_SEQ_TEST,    // �e�X�g���
  PROC_MAIN_SEQ_MENU,    // �����������
  PROC_MAIN_SEQ_SELECT,  // �������e�ύX���
  PROC_MAIN_SEQ_CHECK,   // �����񍐊m�F���
  PROC_MAIN_SEQ_FINISH,  // �I��
} PROC_MAIN_SEQ;


//===============================================================================
// ���������[�_�[ �v���Z�X ���[�N
//===============================================================================
typedef struct
{
  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;

  // �t���[���J�E���^
  u32 frameCount;

  // �e��ʐ�p���[�N
  RESEARCH_TEST_WORK*   testWork;   // �e�X�g���
  RESEARCH_MENU_WORK*   menuWork;   // ����������� ( ���j���[��� )
  RESEARCH_SELECT_WORK* selectWork; // �������e�ύX��� 
  RESEARCH_CHECK_WORK*  checkWork;  // �����񍐊m�F���

} RESEARCH_WORK;


//===============================================================================
// ������J�֐�
//===============================================================================
// �v���Z�X����
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcEnd ( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk );
// ���C���v���Z�X �V�[�P���X����
static void SwitchMainProcSeq    ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void ChangeMainProcSeq    ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq );
static void SetMainProcSeq       ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
// ���C���v���Z�X �V�[�P���X����
static PROC_MAIN_SEQ ProcMain_SETUP ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_TEST  ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_MENU  ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_CHECK ( RESEARCH_WORK* work );
// VRAM-Bank
static void SetupVRAMBank();
// BG
static void SetupBG  ( HEAPID heapID );
static void CleanUpBG( void );
// ���� �w�iBG��
static void SubBG_BackGround_Setup( HEAPID heapID );
static void SubBG_BackGround_PaletteAnime( const RESEARCH_WORK* work );
// ����� �w�iBG��
static void MainBG_BackGround_Setup( HEAPID heapID );
static void MainBG_BackGround_PaletteAnime( const RESEARCH_WORK* work );


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

  switch( *seq )
  {
  // �v���Z�X���[�N�m��
  case PROC_INIT_SEQ_ALLOC_WORK:
    GFL_PROC_AllocWork( proc, sizeof(RESEARCH_WORK), HEAPID_PROC );
    (*seq)++;
    break;

  // �v���Z�X���[�N������
  case PROC_INIT_SEQ_INIT_WORK:
    OBATA_Printf( "HEAPID_PROC = %d\n", HEAPID_PROC );
    OBATA_Printf( "HEAPID_SAVE = %d\n", HEAPID_SAVE );
    work->heapID     = HEAPID_PROC;
    work->gameSystem = param->gameSystem;
    work->frameCount = 0;
    work->testWork   = NULL;
    work->menuWork   = NULL;
    work->selectWork = NULL;
    work->checkWork  = NULL;
    (*seq)++;
    break;

  // �\������
  case PROC_INIT_SEQ_SETUP_DISPLAY:
    SetupVRAMBank(); // VRAM-Bank ���蓖��
    GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );  // ����ʂ����C��
    (*seq)++;
    break;

  // BG����
  case PROC_INIT_SEQ_SETUP_BG:
    SetupBG( work->heapID );
    MainBG_BackGround_Setup( work->heapID ); 
    SubBG_BackGround_Setup( work->heapID ); 
    (*seq)++;
    break;

  // �I��
  case PROC_INIT_SEQ_FINISH:
    return GFL_PROC_RES_FINISH;

  // �G���[
  default:
    GF_ASSERT(0);
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

  switch( *seq )
  {
  // BG��Еt��
  case PROC_END_SEQ_CLEAN_UP_BG:
    CleanUpBG();
    (*seq)++;
    break;

  // �v���Z�X���[�N���
  case PROC_END_SEQ_FREE_WORK:
    GFL_PROC_FreeWork( proc );
    (*seq)++;
    break;

  // �I��
  case PROC_END_SEQ_FINISH:
    return GFL_PROC_RES_FINISH;

  // �G���[
  default:
    GF_ASSERT(0);
  }
  return GFL_PROC_RES_CONTINUE;
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
  PROC_MAIN_SEQ  nextSeq;

  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   nextSeq = ProcMain_SETUP ( work );  break;
  case PROC_MAIN_SEQ_TEST:    nextSeq = ProcMain_TEST  ( work );  break;
  case PROC_MAIN_SEQ_MENU:    nextSeq = ProcMain_MENU  ( work );  break;
  case PROC_MAIN_SEQ_SELECT:  nextSeq = ProcMain_SELECT( work );  break;
  case PROC_MAIN_SEQ_CHECK:   nextSeq = ProcMain_CHECK ( work );  break;
  case PROC_MAIN_SEQ_FINISH:  return GFL_PROC_RES_FINISH;
  default:  GF_ASSERT(0);
  }

  // �w�iBG�̃p���b�g�A�j���[�V����
  //MainBG_BackGround_PaletteAnime( work );
  SubBG_BackGround_PaletteAnime( work );

  // �t���[���J�E���^�X�V
  work->frameCount++; 

  // �V�[�P���X�̍X�V
  SwitchMainProcSeq( work, seq, nextSeq );

  // ���C���v���Z�X�p��
  return GFL_PROC_RES_CONTINUE;
}


//===============================================================================
// �����C���v���Z�X �V�[�P���X����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�̃X�C�b�`������s��
 *
 * @param work    �v���Z�X���[�N
 * @param seq     �V�[�P���X���[�N
 * @param nextSeq ���̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static void SwitchMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{
  // �ύX�Ȃ�
  if( *seq == nextSeq ){ return; }

  // �ύX
  ChangeMainProcSeq( work, seq, nextSeq );
}

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

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: chenge seq ==> " );
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   OS_TFPrintf( PRINT_TARGET, "SETUP(%d)\n",  *seq );  break;
  case PROC_MAIN_SEQ_TEST:    OS_TFPrintf( PRINT_TARGET, "TEST(%d)\n",   *seq );  break;
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
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:  break;
  case PROC_MAIN_SEQ_TEST:   DeleteResearchTestWork  ( work->testWork );    break;
  case PROC_MAIN_SEQ_MENU:   DeleteResearchMenuWork  ( work->menuWork );    break;
  case PROC_MAIN_SEQ_SELECT: DeleteResearchSelectWork( work->selectWork );  break;
  case PROC_MAIN_SEQ_CHECK:  DeleteResearchCheckWork ( work->checkWork );   break;
  case PROC_MAIN_SEQ_FINISH: break;
  default:  GF_ASSERT(0);
  }

  // ��p���[�N�|�C���^���N���A
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_TEST:    work->testWork   = NULL;  break;
  case PROC_MAIN_SEQ_MENU:    work->menuWork   = NULL;  break;
  case PROC_MAIN_SEQ_SELECT:  work->selectWork = NULL;  break;
  case PROC_MAIN_SEQ_CHECK:   work->checkWork  = NULL;  break;
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
  case PROC_MAIN_SEQ_TEST:    GF_ASSERT( work->testWork   == NULL );  break;
  case PROC_MAIN_SEQ_MENU:    GF_ASSERT( work->menuWork   == NULL );  break;
  case PROC_MAIN_SEQ_SELECT:  GF_ASSERT( work->selectWork == NULL );  break;
  case PROC_MAIN_SEQ_CHECK:   GF_ASSERT( work->checkWork  == NULL );  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  } 

  // ���̃V�[�P���X�̐�p���[�N�𐶐�
  switch( nextSeq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_TEST:    work->testWork   = CreateResearchTestWork  ( work->heapID );  break;
  case PROC_MAIN_SEQ_MENU:    work->menuWork   = CreateResearchMenuWork  ( work->heapID );  break;
  case PROC_MAIN_SEQ_SELECT:  work->selectWork = CreateResearchSelectWork( work->heapID );  break;
  case PROC_MAIN_SEQ_CHECK:   work->checkWork  = CreateResearchCheckWork ( work->heapID );  break;
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
  //return PROC_MAIN_SEQ_TEST;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���C���v���Z�X �e�X�g���
 *
 * @param work �v���Z�X���[�N
 *
 * @return �e�X�g��ʂ��I�������ꍇ ���̃V�[�P���X
 *         �����łȂ���� ���݂̃V�[�P���X
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_TEST( RESEARCH_WORK* work )
{ 
  RESEARCH_TEST_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // �e�X�g��ʃ��C������
  result = ResearchTestMain( work->testWork );
  
  // ���̃V�[�P���X������
  switch( result )
  {
  case RESEARCH_TEST_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_TEST;  break;
  case RESEARCH_TEST_RESULT_END:       nextSeq = PROC_MAIN_SEQ_MENU;  break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
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
  RESEARCH_MENU_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // �������[�_�[������ʃ��C������
  result = ResearchMenuMain( work->menuWork );
  
  // ���̃V�[�P���X������
  switch( result )
  {
  case RESEARCH_MENU_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_MENU;    break;
  case RESEARCH_MENU_RESULT_TO_SELECT: nextSeq = PROC_MAIN_SEQ_SELECT;  break;
  case RESEARCH_MENU_RESULT_TO_CHECK:  nextSeq = PROC_MAIN_SEQ_CHECK;   break;
  case RESEARCH_MENU_RESULT_EXIT:      nextSeq = PROC_MAIN_SEQ_FINISH;  break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
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
  RESEARCH_SELECT_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // �������ڕύX��ʃ��C������
  result = ResearchSelectMain( work->selectWork );
  
  // ���̃V�[�P���X������
  switch( result )
  {
  case RESEARCH_SELECT_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_SELECT;  break;
  case RESEARCH_SELECT_RESULT_TO_MENU:   nextSeq = PROC_MAIN_SEQ_MENU;    break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
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
  RESEARCH_CHECK_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // �����񍐊m�F��ʃ��C������
  result = ResearchCheckMain( work->checkWork );
  
  // ���̃V�[�P���X������
  switch( result )
  {
  case RESEARCH_CHECK_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_CHECK;  break;
  case RESEARCH_CHECK_RESULT_TO_MENU:   nextSeq = PROC_MAIN_SEQ_MENU;   break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
} 


//===============================================================================
// ��VRAM-Bank
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @biref VRAM-Bank �̊��蓖�Ă�ݒ肷��
 */
//-------------------------------------------------------------------------------
static void SetupVRAMBank()
{
  const GFL_DISP_VRAM VRAMBankSettings =
  {
    GX_VRAM_BG_128_A,            // MAIN-BG
    GX_VRAM_BGEXTPLTT_NONE,      // MAIN-BG-EXP-PLT
    GX_VRAM_SUB_BG_128_C,        // SUB--BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,  // SUB--BG-EXP-PLT
    GX_VRAM_OBJ_64_E,            // MAIN-OBJ
    GX_VRAM_OBJEXTPLTT_NONE,     // MAIN-OBJ-EXP-PLT
    GX_VRAM_SUB_OBJ_16_I,        // SUB--OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE, // SUB--OBJ-EXP-PLT
    GX_VRAM_TEX_0_B,             // TEX-IMG
    GX_VRAM_TEXPLTT_0_G,         // TEX-PLT
    GX_OBJVRAMMODE_CHAR_1D_64K,  // MAIN-OBJ-MAPPING-MODE
    GX_OBJVRAMMODE_CHAR_1D_32K,  // SUB--OBJ-MAPPING-MODE
  }; 
  GFL_DISP_SetBank( &VRAMBankSettings );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: setup VRAM-Bank\n" );
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
  {
    static const GFL_BG_SYS_HEADER BGSysHeader = 
    {
      GX_DISPMODE_GRAPHICS,   // �\�����[�h�w��
      GX_BGMODE_0,            // �a�f���[�h�w��(���C���X�N���[��)
      GX_BGMODE_0,            // �a�f���[�h�w��(�T�u�X�N���[��)
      GX_BG0_AS_2D            // �a�f�O�̂Q�c�A�R�c���[�h�I��
    };
    GFL_BG_SetBGMode( &BGSysHeader );
  } 

  // SUB-BG
  {
    const GFL_BG_BGCNT_HEADER BGCntHeader = 
    {
      0, 0,                         // �����\���ʒu
      0x800,                        // �X�N���[���o�b�t�@�T�C�Y
      0,                            // �X�N���[���o�b�t�@�I�t�Z�b�g
      GFL_BG_SCRSIZ_256x256,        // �X�N���[���T�C�Y
      GX_BG_COLORMODE_16,           // �J���[���[�h
      GX_BG_SCRBASE_0x0000,         // �X�N���[���x�[�X�u���b�N
      GX_BG_CHARBASE_0x04000,       // �L�����N�^�x�[�X�u���b�N
      GFL_BG_CHRSIZ_256x256,        // �L�����N�^�G���A�T�C�Y
      GX_BG_EXTPLTT_01,             // BG�g���p���b�g�X���b�g�I��
      SUB_BG_BACK_GROUND_PRIORITY,  // �\���v���C�I���e�B�[
      0,                            // �G���A�I�[�o�[�t���O
      0,                            // DUMMY
      FALSE,                        // ���U�C�N�ݒ�
    }; 
    GFL_BG_SetBGControl( SUB_BG_BACK_GROUND, &BGCntHeader, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( SUB_BG_BACK_GROUND, VISIBLE_ON );
  }

  // MAIN-BG
  {
    const GFL_BG_BGCNT_HEADER BGCntHeader = 
    {
      0, 0,                         // �����\���ʒu
      0x800,                        // �X�N���[���o�b�t�@�T�C�Y
      0,                            // �X�N���[���o�b�t�@�I�t�Z�b�g
      GFL_BG_SCRSIZ_256x256,        // �X�N���[���T�C�Y
      GX_BG_COLORMODE_16,           // �J���[���[�h
      GX_BG_SCRBASE_0x0000,         // �X�N���[���x�[�X�u���b�N
      GX_BG_CHARBASE_0x04000,       // �L�����N�^�x�[�X�u���b�N
      GFL_BG_CHRSIZ_256x256,        // �L�����N�^�G���A�T�C�Y
      GX_BG_EXTPLTT_01,             // BG�g���p���b�g�X���b�g�I��
      MAIN_BG_BACK_GROUND_PRIORITY, // �\���v���C�I���e�B�[
      0,                            // �G���A�I�[�o�[�t���O
      0,                            // DUMMY
      FALSE,                        // ���U�C�N�ݒ�
    };
    GFL_BG_SetBGControl( MAIN_BG_BACK_GROUND, &BGCntHeader, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( MAIN_BG_BACK_GROUND, TRUE );
  }

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
  GFL_BG_FreeBGControl( MAIN_BG_BACK_GROUND );
  GFL_BG_FreeBGControl( SUB_BG_BACK_GROUND );
  GFL_BG_Exit();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: clean up BG\n" );
}


//===============================================================================
// ������ �w�iBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���� �w�iBG�ʂ̏���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void SubBG_BackGround_Setup( HEAPID heapID )
{ 
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR, heapID ); 

    // �p���b�g�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgu_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( SUB_BG_BACK_GROUND, data->pRawData, FULL_PALETTE_SIZE, 0 );
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
      GFL_BG_LoadCharacter( SUB_BG_BACK_GROUND, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �X�N���[���f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_BACK_GROUND, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_BACK_GROUND );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: setup SUB BG BACK GROUND\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���� �w�iBG�ʂ̃p���b�g�A�j���[�V�����X�V
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SubBG_BackGround_PaletteAnime( const RESEARCH_WORK* work )
{ 
  u8 paletteNo;

  paletteNo = SUB_BG_BACK_GROUND_FIRST_PLT_IDX + 
              ( (work->frameCount / 40) % SUB_BG_BACK_GROUND_PLT_NUM );

  GFL_BG_ChangeScreenPalette( SUB_BG_BACK_GROUND, 0, 0, 32, 24, paletteNo );
  GFL_BG_LoadScreenReq( SUB_BG_BACK_GROUND );
}


//===============================================================================
// ������� �w�iBG��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ����� �w�iBG�ʂ̏���
 *
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void MainBG_BackGround_Setup( HEAPID heapID )
{ 
  // �f�[�^�ǂݍ���
  {
    ARCHANDLE* handle;

    // �n���h���I�[�v��
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR, heapID ); 

    // �p���b�g�f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgd_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( MAIN_BG_BACK_GROUND, data->pRawData, FULL_PALETTE_SIZE, 0 );
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
      GFL_BG_LoadCharacter( MAIN_BG_BACK_GROUND, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // �X�N���[���f�[�^
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgd_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_BACK_GROUND, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( MAIN_BG_BACK_GROUND );
      GFL_HEAP_FreeMemory( src );
    }

    // �n���h���N���[�Y
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: setup MAIN BG BACK GROUND\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ����� �w�iBG�ʂ̃p���b�g�A�j���[�V�����X�V
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void MainBG_BackGround_PaletteAnime( const RESEARCH_WORK* work )
{ 
  u8 paletteNo;

  paletteNo = MAIN_BG_BACK_GROUND_FIRST_PLT_IDX + 
              ( (work->frameCount / 40) % MAIN_BG_BACK_GROUND_PLT_NUM );

  GFL_BG_ChangeScreenPalette( MAIN_BG_BACK_GROUND, 0, 0, 32, 24, paletteNo );
  GFL_BG_LoadScreenReq( MAIN_BG_BACK_GROUND );
}
