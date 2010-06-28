//=============================================================================================
/**
 * @file  gfl_use.c
 * @date  2006.12.06
 * @author  GAME FREAK inc.
 * @brief ���[�U�[��GF���C�u�����g�p�̂��߂̃X�P���g���\�[�X
 *
 */
//=============================================================================================

#include <nitro.h>

#include <gflib.h>
#include <assert_default.h>

#include <procsys.h>
#include <gf_overlay.h>
#include <tcb.h>
#include <backup_system.h>

//#include "gflib_inter.h"

#include "system\main.h"
#include "system\gfl_use.h"
#include "arc_def.h"
#include "arc_file.h"
//#include "net_old/net_old.h"
#include "savedata/save_control.h"
#include "net/net_dev.h"
#include "system/save_error_warning.h"

#include "textprint.h"

#ifdef PM_DEBUG
#include "test/debug_pause.h"
#include "debug/debugwin_sys.h"
#include "debug/gf_mcs.h"
#include "sound/snd_viewer_mcs.h"
#include "debug/mcs_resident.h"
#include "debug/mcs_mode.h"


#endif //PM_DEBUG

//=============================================================================================
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  TCB_HINTR_MAX = 4,
  TCB_VINTR_MAX = 16,
};

//------------------------------------------------------------------
//  �q�[�v�T�C�Y��`
//------------------------------------------------------------------
#define HEAPSIZE_SYSTEM (0x006000)
#define HEAPSIZE_APP    (0x180000)   // PL,GS�ł͖�0x13A000
#define HEAPSIZE_DSI    (0x486000)   // DSI��16M

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
    GFL_STD_RandContext publicRandContext;
  GFL_TCBSYS *  TCBSysHintr;
  void *    TCBMemHintr;
  GFL_TCBSYS *  TCBSysVintr;
  void *    TCBMemVintr;
}GFL_USE_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static const HEAP_INIT_HEADER hih[]={
  { HEAPSIZE_SYSTEM,  OS_ARENA_MAIN },
#ifdef DEBUG_ONLY_FOR_taya
  { 0x182000,   OS_ARENA_MAIN },
//  { HEAPSIZE_APP,   OS_ARENA_MAIN },
#else
  { HEAPSIZE_APP,   OS_ARENA_MAIN },
#endif
};

static const HEAP_INIT_HEADER hihDSi[]={  //DSI�p
  { HEAPSIZE_SYSTEM,  OS_ARENA_MAIN },
  { HEAPSIZE_DSI,   OS_ARENA_MAIN },
};


static GFL_USE_WORK * gfl_work = NULL;
static int        GFL_USE_VintrCounter;

static void GFUser_PublicRandInit(void);

#ifdef PM_DEBUG
FS_EXTERN_OVERLAY(mcs_lib);
BOOL  mcs_recv_auto;

static void mcsRecv( void );
#endif

static GFL_USE_VINTR_FUNC GflUseVintrFunc;
static void *extraHeapBuffer = NULL;
static OSHeapHandle extraHeapHandle;
//=============================================================================================
//
//      �֐�
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����F������
 *
 * InitSystem�̓����ŌĂ�ł���InitFileSystem��gflib�̊O�ł�
 * �ĂׂȂ����߁AInitSystem���Ă�ł���B
 */
//------------------------------------------------------------------
void GFLUser_Init(void)
{
  //InitSystem();
#if 0
#ifdef USE_MAINEXMEMORY
  OS_EnableMainExArena();
#endif

  // �V�X�e��������
  OS_Init();
  OS_InitTick();
  OS_InitAlarm();
  // RTC������
  RTC_Init();
  // GX������
  GX_Init();
  // �Œ菬���_API������
  FX_Init();
  // �t�@�C���V�X�e��������
  InitFileSystem();
#endif

#ifndef MULTI_BOOT_MAKE
  if ( OS_IsRunOnTwl() == FALSE )
  {
    void *nstart;
    nstart = OS_InitAlloc( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1 );
    OS_SetMainArenaLo( nstart );
    extraHeapHandle = OS_CreateExtraHeap( OS_ARENA_MAIN );
    extraHeapBuffer = OS_AllocFromHeap( OS_ARENA_MAIN, extraHeapHandle, 0x3F00 );
  }
  else
  {
  }
#endif

  //�q�[�v�V�X�e��������
  if( OS_IsRunOnTwl() ){//DSI�Ȃ�
    GFL_HEAP_Init(&hihDSi[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0); //���C���A���[�i
  }
  else{
    GFL_HEAP_Init(&hih[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0); //���C���A���[�i
  }


  GFL_HEAP_DTCM_Init( 0x2000 );       //�c�s�b�l�A���[�i

#ifndef MULTI_BOOT_MAKE
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SAVE, HEAPSIZE_SAVE );

  //����q�[�v�̈�m��
  //�n�C�u���b�h���[�h����NITRO�N�����Ɛ擪16kb���q�[�v�Ƃ��Ċm�ۉ\
  //TWL�̏ꍇ�͑������畁�ʂɂƂ�
  
  if ( OS_IsRunOnTwl() == FALSE )
  {
    GFL_HEAP_CreateHeapInBuffer( extraHeapBuffer , 0x3F00 , HEAPID_EXTRA );
  }
  else
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_EXTRA, 0x3F00 );
  }
  
#endif //MULTI_BOOT_MAKE

  //STD �W�����C�u�����������i������CRC�j
  GFL_STD_Init(GFL_HEAPID_SYSTEM);
  //�A�[�J�C�u�V�X�e��������
#ifndef MULTI_BOOT_MAKE
  GFL_ARC_Init(&ArchiveFileTable[0],ARCID_TABLE_MAX);
#else
  GFL_ARC_InitMultiBoot(&ArchiveFileTable[0],ARCID_TABLE_MAX);
#endif //MULTI_BOOT_MAKE

  //UI�V�X�e��������
#ifndef MULTI_BOOT_MAKE
  GFL_UI_Boot(GFL_HEAPID_SYSTEM,FALSE);
#else
  GFL_UI_Boot(GFL_HEAPID_SYSTEM,TRUE);
#endif //MULTI_BOOT_MAKE

  // �A�T�[�g��~�֐��̐ݒ�
  if( OS_GetConsoleType() & (OS_CONSOLE_NITRO|OS_CONSOLE_TWL) ){
    // ROM���s�p�i�\�Ȃ����LCD�\���j
    GFL_ASSERT_SetLCDMode();
  }else{
    // NITRO�f�o�b�K�p�i�R�[���X�^�b�N�\�����\�Ȃ珈�����s�̂��߁j
    GFL_ASSERT_SetDisplayFunc( NULL, NULL, GFUser_AssertFinish );
  }

  //OVERLAY�V�X�e��������
  GFL_OVERLAY_boot(GFL_HEAPID_SYSTEM, 16, 4, 4);

  //PROC�V�X�e��������
  GFL_PROC_boot(GFL_HEAPID_SYSTEM);
  gfl_work = GFL_HEAP_AllocMemory(GFL_HEAPID_SYSTEM, sizeof(GFL_USE_WORK));
  gfl_work->TCBMemHintr = GFL_HEAP_AllocMemory(
      GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_HINTR_MAX));
  gfl_work->TCBSysHintr = GFL_TCB_Init(TCB_HINTR_MAX, gfl_work->TCBMemHintr);
  gfl_work->TCBMemVintr = GFL_HEAP_AllocMemory(
      GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_VINTR_MAX));
  gfl_work->TCBSysVintr = GFL_TCB_Init(TCB_VINTR_MAX, gfl_work->TCBMemVintr);

  // �ėp����������
  GFUser_PublicRandInit();

  //FADE�V�X�e��������
  GFL_FADE_Init(GFL_HEAPID_SYSTEM);

  //�T�E���h�V�X�e��������
  //GFL_SOUND_Init(&sndHeap[0],SOUND_HEAP_SIZE);

  //�o�b�N�A�b�v�V�X�e��������
#ifndef MULTI_BOOT_MAKE
  GFL_BACKUP_Init(HEAPID_SAVE, SaveErrorCall_Load, SaveErrorCall_Save);
#endif //MULTI_BOOT_MAKE

  //�V�X�e���t�H���g������
  GFL_TEXT_CreateSystem( NULL );
  //RTC������
  GFL_RTC_Init();

  GFL_USE_VintrCounter = 0;

  //MCS���C�u�������g���������Ƀ��[�h
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID( mcs_lib ) );
    GFL_MCS_Init();
  }
  MCS_USEMODE_Set( MCS_USEMODE_NONE );
#endif //MULTI_BOOT_MAKE
#endif
  GFUser_ResetVIntrFunc();

  GFUser_SetSleepSoundFunc();

  GFL_FADE_SetFadeSpeed( 2 );
}


//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����F���C���Ăяo��
 *
 * ���t���[���Ăяo�����K�v�����郁�C���������L�q����
 */
//------------------------------------------------------------------
void GFLUser_Main(void)
{
  GFL_UI_Main();

  NET_DEV_Main();


#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //�f�o�b�O�V�X�e��
  DEBUGWIN_UpdateSystem();
  //L�������Ă���Ԓ�~�BL�����Ȃ���R��1�R�}����
  if( DEBUG_PAUSE_Update() == TRUE &&
      DEBUGWIN_IsActive() == FALSE )
#endif MULTI_BOOT_MAKE
#endif
  {
    GFL_PROC_Main();
  }
  GFL_RTC_Main();
  //GFL_SOUND_Main();
  //
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //MCS��M
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_Main();
    GFL_MCS_SNDVIEWER_Main();
    if ( MCS_USEMODE_Get() == MCS_USEMODE_RESIDENT )
    {
      GFL_MCS_Resident();
    }
  }
#endif MULTI_BOOT_MAKE
#endif
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����F�`�揈��
 *
 * ���t���[���Ăяo�����K�v������`�揈�����L�q����
 */
//------------------------------------------------------------------
void GFLUser_Display(void)
{
  //GFL_OAM_sysMain(...);
  //GFL_BG_sysMain(...);
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����F�I������
 */
//------------------------------------------------------------------
void GFLUser_Exit(void)
{
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_Exit();
  }
#endif //MULTI_BOOT_MAKE
#endif
  GFL_TEXT_DeleteSystem();
#ifndef MULTI_BOOT_MAKE
  GFL_BACKUP_Exit();
#endif //MULTI_BOOT_MAKE
  //GFL_SOUND_Exit();
  GFL_FADE_Exit();
  GFL_PROC_Exit();
  GFL_TCB_Exit(gfl_work->TCBSysVintr);
  GFL_HEAP_FreeMemory(gfl_work->TCBSysVintr);
  GFL_HEAP_FreeMemory(gfl_work->TCBMemVintr);
  GFL_OVERLAY_Exit();
  GFL_UI_Exit();
  GFL_HEAP_DeleteHeap( HEAPID_SAVE );
  GFL_HEAP_DTCM_Exit();
  GFL_HEAP_Exit();
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FHBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void GFLUser_HIntr(void)
{
  GFL_TCB_Main(gfl_work->TCBSysHintr);
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FHBlank��TCB�̓o�^����
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_HIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
  return GFL_TCB_AddTask(gfl_work->TCBSysHintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FVBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void GFLUser_VIntr(void)
{
  if(GflUseVintrFunc){ GflUseVintrFunc(); }
}

//VBlank���荞�݊֐��̓o�^
BOOL GFUser_SetVIntrFunc(GFL_USE_VINTR_FUNC func)
{
  if(GflUseVintrFunc){
    GF_ASSERT(0);
    return FALSE;
  }
  GflUseVintrFunc = func;
  return TRUE;
}

//VBlank���荞�݊֐��̉���
void GFUser_ResetVIntrFunc(void)
{
  GflUseVintrFunc = NULL;
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FVBlank�^�C�~���O����
 */
//------------------------------------------------------------------
void GFLUser_VTiming(void)
{
  GFL_TCB_Main(gfl_work->TCBSysVintr);
  GFL_BG_VBlankFunc();
  // V�u�����N���ԂŎ��s���܂��B
  // �����A���j�b�g�̕`�悪�s���Ă��Ȃ��̂�
  // ���̊֐������s����ƁA�`�悵�Ă���OBJ�������Ă��܂�����
  // ���荞�݂Ȃ��ŌĂ΂Ȃ��ق����ǂ���������܂���B
    //�J�����܂��� Ariizumi1031
  //GFL_CLACT_VBlankFunc();
    // �ʐM�A�C�R���̕`��̂��߂ɂ���܂��B�ʐM���͍̂s���Ă��܂���
    //GFL_NET_VBlankFunc();
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //MCS��M
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_VIntrFunc();
  }
#endif //MULTI_BOOT_MAKE
#endif
  GFL_DMA_Main();


#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //L�������Ă���Ԓ�~�BL�����Ȃ���R��1�R�}����
  if( DEBUG_PAUSE_Update() == TRUE &&
      DEBUGWIN_IsActive() == FALSE )
#endif MULTI_BOOT_MAKE
#endif
  {
    GFL_FADE_Main();  // �t�F�[�h���f
  }

  GFL_USE_VintrCounter++;
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FVBlank��TCB�̓o�^����
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
  return GFL_TCB_AddTask(gfl_work->TCBSysVintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief GF���C�u�������p�����FVBlank��TCBSYS�̎擾
 */
//------------------------------------------------------------------
GFL_TCBSYS * GFUser_VIntr_GetTCBSYS( void )
{
  return gfl_work->TCBSysVintr;
}

//------------------------------------------------------------------
/**
 * @brief �R���e�L�X�g�𐶐�����
 * @param context GFL_STD_RandContext�|�C���^
 */
//------------------------------------------------------------------
void GFUser_SetStartRandContext(GFL_STD_RandContext *context)
{
  GFL_STD_RandGeneralInit(context);
}

//------------------------------------------------------------------
/**
 * @brief GFUser �ėp�����쐬
 */
//------------------------------------------------------------------
static void GFUser_PublicRandInit(void)
{
  GFUser_SetStartRandContext( &gfl_work->publicRandContext );
  GFL_STD_MtRandInit(GFUser_GetPublicRand(0));
}

//------------------------------------------------------------------
/**
 * @brief  �ėp�����𓾂�
 * @param  range   �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                 GFL_STD_RAND_MAX = 0 ���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
 *                 % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 * @return 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 GFUser_GetPublicRand(u32 range)
{
  return GFL_STD_Rand( &gfl_work->publicRandContext, range );
}

//------------------------------------------------------------------
/**
 * @brief  �ėp�����𓾂�
 * @param  range   �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                  % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 * @return 32bit�̃����_���l
 */
//------------------------------------------------------------------
u32 GFUser_GetPublicRand0(u32 range)
{
  return GFL_STD_Rand0( &gfl_work->publicRandContext, range );
}

//------------------------------------------------------------------
/**
 * @brief VBlank�J�E���^�[�擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
void GFUser_VIntr_ResetVblankCounter( void )
{
  GFL_USE_VintrCounter = 0;
}

int GFUser_VIntr_GetVblankCounter( void )
{
  return GFL_USE_VintrCounter;
}

//--------------------------------------------------------------------------
/**
 * @brief ArchiveID�Ńt�@�C�������擾����
 */
//--------------------------------------------------------------------------
const char * GFUser_GetFileNameByArcID(ARCID arcID)
{
  GF_ASSERT(arcID < ARCID_TABLE_MAX);
  return ArchiveFileTable[arcID];
}

//--------------------------------------------------------------------------
/**
 * ASSERT��~��ɌĂяo����郆�[�U�֐�
 *
 * �� �f�t�H���g����OS_Halt�ɂ���~���Ă��܂��A
 *    �R�[���X�^�b�N�\��������Ȃ��Ȃǂ̖�肪���邽�ߗp�ӂ��Ă���܂�
 *
 */
//--------------------------------------------------------------------------
void GFUser_AssertFinish( void )
{
  int key_cont;

  while(1){

    GFL_UI_Main();
    key_cont = GFL_UI_KEY_GetCont();

    // �L�[���������܂Ń��[�v
    if( (key_cont & PAD_BUTTON_L) && (key_cont & PAD_BUTTON_R) &&
      (key_cont & PAD_BUTTON_X) && (key_cont & PAD_BUTTON_Y) ){
      return ;
    }

    // VBlank�҂�
    // (�����L���ɂ���ƁA�f�o�b�J�Œ�~�������Ƃ��ɃR�[���X�^�b�N���\������Ȃ�)
//    OS_WaitIrq(TRUE,OS_IE_V_BLANK);
  }
}

//--------------------------------------------------------------------------
/**
 * MCS�풓�A�v���ւ̃��N�G�X�g
 */
//--------------------------------------------------------------------------
BOOL GFUser_SendHeapStatus(void);

BOOL GFUser_SendHeapStatus(void)
{
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
#if PM_DEBUG
    GFL_MCS_Resident_SendHeapStatus();
#endif
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
//
//  DS�̃J�o�[���J�������̏���
//
//=============================================================================================
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
static void sleepSoundSetupDefault( void * work )
{
  NNS_SndSetMasterVolume( 0 );
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
static void sleepSoundReleaseDefault( void * work )
{
  NNS_SndSetMasterVolume( 127 );
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void GFUser_SetSleepSoundFunc( void )
{
  GFL_UI_SleepSoundSetFunc( sleepSoundSetupDefault, sleepSoundReleaseDefault, NULL );
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL GFUser_IsSleepSoundFunc( void )
{
  if( GFL_UI_SleepSoundGoGetFunc() == sleepSoundSetupDefault ){
    GF_ASSERT( GFL_UI_SleepSoundReleaseGetFunc() == sleepSoundReleaseDefault );
    return TRUE;
  }
  return FALSE;
}



