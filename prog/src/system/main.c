//===================================================================
/**
 * @file  main.c
 * @brief �A�v���P�[�V�����@�T���v���X�P���g��
 * @author  GAME FREAK Inc.
 * @date  06.11.08
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#include "playable_version.h"
#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"
#include "system\machine_use.h"
#include "system\gfl_use.h"
#include "system\main.h"
#include "savedata/save_control.h"
#include "net/wih_dwc.h"
#include "print/printsys.h"
#include "print/global_font.h"
#include "print/global_msg.h"
#include "poke_tool/poke_tool.h"
#include "system/net_err.h"
#include "savedata/config.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "gamesystem\msgspeed.h"
#include "gamesystem\game_beacon.h"
#include "gamesystem/g_power.h"
#include "savedata/config.h"

#include "sound\pm_sndsys.h"
#include "sound\pm_voice.h"
#include "sound\pm_wb_voice.h"
#ifdef PM_DEBUG
#include "debug/debug_sd_print.h"
#include "test/performance.h"
#include "test/debug_pause.h"
#include "debug/debugwin_sys.h"
#include "debug/debug_ui.h"
#include "debug/debug_ui_input.h"
#include "debug/debug_flg.h"
#endif //PM_DEBUG

#include "title/title.h"

// �T�E���h�ǂݍ��݃X���b�h
extern OSThread soundLoadThread;

static BOOL isARM9preference(void);
static  void  SkeltonHBlankFunc(void);
static  void  SkeltonVBlankFunc(void);
static  void  GameInit(void);
static  void  GameMain(void);
static  void  GameExit(void);
static  void  GameVBlankFunc(void);
static  void  _set_sound_mode( CONFIG *config );

#ifdef  PLAYABLE_VERSION
static void checkOnwerComments( void );
#endif

#ifdef PM_DEBUG
static void DEBUG_StackOverCheck(void);
static void DEBUG_UI_AutoKey(void);

#define DEBUG_MAIN_TIME_AVERAGE (0) // ����AVERAGE�@�\�� �P�FON�@�O�FOFF

#else

#define DEBUG_MAIN_TIME_AVERAGE (0) // ����AVERAGE�@�\�� �P�FON�@�O�FOFF

#endif


//------------------------------------------------------------------
//  ��������AVERAGE
//------------------------------------------------------------------
#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON

u8 DEBUG_MAIN_UPDATE_TYPE = 0;  ///<FIELDMAP TOP TAIL �t���[���`�F�b�N�p


OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now;    // ���݂̃`�b�N

static OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_Start;    // 1�t���[���J�n
static OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_End;    // 1�t���[���J�n
static OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave;      // AVERAGE
static int DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count = 0; // �J�E���^
static int DEBUG_DEBUG_MAIN_TIME_AVERAGE_OverCount =0;    // 16000Over
static OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_Max =0;    // 16000Over
#define DEBUG_DEBUG_MAIN_TIME_AVERAGE_AVE_COUNT_TIME  ( 120 )

#define DEBUG_DEBUG_MAIN_TIME_AVERAGE_NOW_TIME_DRAW_KEY  ( PAD_BUTTON_R )

static void DEBUG_MAIN_TIME_AVERAGE_StartFunc( void );
static void DEBUG_MAIN_TIME_AVERAGE_EndFunc( void );
#endif

//------------------------------------------------------------------
//  �O���֐��錾
//------------------------------------------------------------------
//extern const  GFL_PROC_DATA CorpProcData;
//FS_EXTERN_OVERLAY(title);
FS_EXTERN_OVERLAY(notwifi);
FS_EXTERN_OVERLAY(intrude_system);

//------------------------------------------------------------------
/**
 * @brief ���C������
 */
//------------------------------------------------------------------
void NitroMain(void)
{

  // �n�[�h���V�X�e���S������������֐�
  MachineSystem_Init();
  // �f�e�k������
  GFLUser_Init();

#ifdef  PLAYABLE_VERSION
  checkOnwerComments();
#endif

  //HBLANK���荞�݋���
  OS_SetIrqFunction(OS_IE_H_BLANK,SkeltonHBlankFunc);
  //VBLANK���荞�݋���
  OS_SetIrqFunction(OS_IE_V_BLANK,SkeltonVBlankFunc);

  (void)OS_EnableIrqMask(OS_IE_H_BLANK|OS_IE_V_BLANK);
  (void)OS_EnableIrq();

  (void)GX_HBlankIntr(TRUE);
  (void)GX_VBlankIntr(TRUE);
  (void)OS_EnableInterrupts();

  // �K�v��TCB�Ƃ��o�^���āc
  GameInit();

#ifdef PM_DEBUG
  DEB_SD_PRINT_InitSystem( GFL_HEAPID_APP );
  DEBUG_PerformanceInit();
  DEBUG_PAUSE_Init();
  {
    //�f�o�b�O���̂Ă̏������{�A�h���X��n��
    u8 *charArea;
    u16 *scrnArea,*plttArea;
    NetErr_GetTempArea( &charArea , &scrnArea , &plttArea );
    DEBUGWIN_InitSystem(charArea , scrnArea , plttArea);
  }
#endif

  OS_Printf( "argc=%d, argv[0]=%s,argv[1]=%s\n%d",OS_GetArgc(), OS_GetArgv(0), OS_GetArgv(1));

  while(TRUE){
#ifdef PM_DEBUG
    //  DEBUG_PerformanceStart();
    DEBUG_PerformanceMain();
    DEBUG_PerformanceStartLine(PERFORMANCE_ID_MAIN);

    DEBUG_PerformanceStartTick(PERFORMANCE_ID_MAIN);

#endif //PM_DEBUG

#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
    DEBUG_MAIN_TIME_AVERAGE_StartFunc();
#endif // DEBUG_MAIN_TIME_AVERAGE_MASTER_ON

#ifdef PM_DEBUG
    DEBUG_StackOverCheck();
#endif  //PM_DEBUG

#ifdef PM_DEBUG
    DEBUG_UI_AutoKey();
#endif  //PM_DEBUG


    MachineSystem_Main();
    // ���C���������āc
    GFLUser_Main();
    GameMain();


    // �`��ɕK�v�ȏ������āc
    GFLUser_Display();

    // �����_�����O�G���W�����Q�Ƃ���f�[�^�Q���X���b�v
    // ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
    //G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

#ifdef PM_DEBUG
    //DEBUG_PerformanceDisp();
    DEBUG_PerformanceEndTick(PERFORMANCE_ID_MAIN);

    DEBUG_PerformanceEndLine(PERFORMANCE_ID_MAIN);
    DEB_SD_PRINT_UpdateSystem();
#endif //PM_DEBUG

#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
    DEBUG_MAIN_TIME_AVERAGE_EndFunc();
#endif // DEBUG_MAIN_TIME_AVERAGE_MASTER_ON

    // VBLANK�҂�
    GFL_G3D_SwapBuffers();

    {
      //2010.06.16  tamada
      //Vblank�҂���Ԃ̂Ƃ��ɒʐM���E��ʐM����킸
      //�T�E���h�������[�h�X���b�h�͒�~����悤�ɂ���
      BOOL isARM9pref = isARM9preference();
      if( isARM9pref )
      { //��ʐM����ARM9�D��ɂ���
        MI_SetMainMemoryPriority( MI_PROCESSOR_ARM9 );
      }

      if( OS_IsThreadTerminated( &soundLoadThread ) == FALSE )
      { // �T�E���h�ǂݍ��݃X���b�h�x�~
        OS_SleepThreadDirect( &soundLoadThread, NULL );
      }

      OS_WaitIrq(TRUE, OS_IE_V_BLANK);
      GameVBlankFunc();

      if( OS_IsThreadTerminated( &soundLoadThread ) == FALSE )
      { // �T�E���h�ǂݍ��݃X���b�h�N��
        OS_WakeupThreadDirect( &soundLoadThread );
      }

      if( isARM9pref )
      { //��ʐM����ARM9�D���ARM7�D��ɖ߂�
        MI_SetMainMemoryPriority( MI_PROCESSOR_ARM7 );
      }
    }

    
  }//while

  GameExit();
}


//------------------------------------------------------------------
/**
 * brief  ARM9�ɐ؂�ւ��邩�ǂ���
 */
//------------------------------------------------------------------
static BOOL isARM9preference(void)
{
  if(GFL_NET_SystemGetConnectNum() > 1){  //�ڑ���
    return FALSE;
  }
  if(GFL_NET_IsIrcConnect()){   //�ԊO��
    return FALSE;
  }
  if(GFL_NET_IsWifiConnect()){  //WIFI�ڑ�
    return FALSE;
  }
  return TRUE;
}



//------------------------------------------------------------------
/**
 * brief  HBlank���荞�ݏ���
 */
//------------------------------------------------------------------
static  void  SkeltonHBlankFunc(void)
{
  GFLUser_HIntr();
}

//------------------------------------------------------------------
/**
 * brief  VBlank���荞�ݏ���
 */
//------------------------------------------------------------------
static  void  SkeltonVBlankFunc(void)
{
  GFLUser_VIntr();

  OS_SetIrqCheckFlag(OS_IE_V_BLANK);
  MI_WaitDma(GX_DEFAULT_DMAID);
}

//--------------------------------------------------------------
/**
 * VBlank���Ԓ��ɍs������
 */
//--------------------------------------------------------------
static void GameVBlankFunc(void)
{
#ifdef PM_DEBUG
  DEBUG_PerformanceStartLine(PERFORMANCE_ID_VBLANK);
#endif

  MachineSystem_VIntr();
  GFLUser_VTiming();

#ifdef PM_DEBUG
  DEBUG_PerformanceEndLine(PERFORMANCE_ID_VBLANK);
#endif
}

//------------------------------------------------------------------
/**
 * @brief   �Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static  void  GameInit(void)
{
  
  /* ���[�U�[���x���ŕK�v�ȏ������������ɋL�q���� */
  //WIFI�ŕK�v�Ȃ��v���O�����I�[�o�[���C�̍ŏ��̃��[�h
  GFL_OVERLAY_Load( FS_OVERLAY_ID( notwifi ) );
  GFL_OVERLAY_Load( FS_OVERLAY_ID( intrude_system ) );


  // �ʐM�̃f�o�b�O�v�����g���s����`
#ifdef PM_DEBUG
#if defined(DEBUG_ONLY_FOR_ohno)||defined(DEBUG_ONLY_FOR_matsuda)||defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)||defined(DEBUG_ONLY_FOR_none)
  GFL_NET_DebugPrintOn();
#endif
#endif  //PM_DEBUG

  /* �����`��V�X�e�������� */
  PRINTSYS_Init( GFL_HEAPID_SYSTEM );

  // �ʐM�u�[�g���� VBlank���荞�݌�ɍs�����߂����ɋL�q�A�������͕\���p�֐��|�C���^
  GFL_NET_Boot( GFL_HEAPID_APP, NULL, GFL_HEAPID_APP, HEAPID_NETWORK_FIX);  //�����ŃG���[�\�����鎖������̂ŕ����`��V�X�e���͎�O
  // AP���̎擾
  WIH_DWC_CreateCFG(HEAPID_NETWORK_FIX);
  WIH_DWC_ReloadCFG();

  /* �N���v���Z�X�̐ݒ� */
#if 0
  TestModeSet();  //���T���v���f�o�b�O���[�h
#else
  GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TitleControlProcData, NULL);
//  GFL_PROC_SysCallProc(FS_OVERLAY_ID(title), &CorpProcData, NULL);
#endif

  
  //�Z�[�u�֘A������(�t���b�V����F�����Ȃ��ꍇ�A�x����ʂ��o���ׁA�����`��V�X�e�����o���Ă���)
  SaveControl_SystemInit(HEAPID_SAVE);
  CONFIG_ApplyMojiMode( SaveData_GetConfig( SaveControl_GetPointer() ) );
  MSGSPEED_InitSystem( SaveControl_GetPointer() );

  //�ʐM�G���[��ʊǗ��V�X�e��������
  NetErr_SystemInit();
  NetErr_SystemCreate(HEAPID_NET_ERR);

  /* �O���[�o�����b�Z�[�W�f�[�^���t�H���g������ */
  GLOBALMSG_Init( GFL_HEAPID_SYSTEM );

  /* poketool�V�X�e�������� */
  POKETOOL_InitSystem( GFL_HEAPID_SYSTEM );

  //�T�E���h�̐ݒ�
  PMSND_Init();
  //�����V�X�e���̐ݒ�
  PMVOICE_Init(GFL_HEAPID_APP, PMV_GetVoiceWaveIdx, PMV_CustomVoiceWave);

  // �T�E���h���[�h�i�X�e���I�E���m�����j�ݒ�
  _set_sound_mode( SaveData_GetConfig( SaveControl_GetPointer()) );

  //�L�[�R���g���[�����[�h�ݒ�
  CONFIG_SYSTEM_KyeControlTblSetting();
  
  //�Q�[�����r�[�R���X�L��������
  GAMEBEACON_Init(GFL_HEAPID_APP);
  
  //G�p���[
  GPower_SYSTEM_Init();
}

//------------------------------------------------------------------
/**
 * @brief   �Q�[�����Ƃ̃��C������
 */
//------------------------------------------------------------------
static  void  GameMain(void)
{
  /* ���[�U�[���x���ŕK�v�ȃ��C�������������ɋL�q���� */

  BrightnessChgMain();
  WIPE_SYS_Main();

  NetErr_Main();
#if PM_DEBUG
  if((GFL_UI_KEY_GetCont() & PAD_BUTTON_L) && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG)){
    NetErr_DEBUG_ErrorSet();
  }
#endif
  PMSND_Main();
  PMVOICE_Main();

  GAMEBEACON_Update();
  GPOWER_SYSTEM_Update();

}

//------------------------------------------------------------------
/**
 * @brief   �Q�[�����Ƃ̏I������
 */
//------------------------------------------------------------------
static  void  GameExit(void)
{
#ifdef PM_DEBUG
  //�f�o�b�O�V�X�e��
  DEBUGWIN_ExitSystem();
#endif //PM_DEBUG

  GAMEBEACON_Exit();
  WIH_DWC_DeleteCFG();
  PMVOICE_Exit();
  PMSND_Exit();
}


//----------------------------------------------------------------------------------
/**
 * @brief �T�E���h�ݒ�
 *
 * @param   config    
 */
//----------------------------------------------------------------------------------
static void _set_sound_mode( CONFIG *config )
{
  if(CONFIG_GetSoundMode(config)==SOUNDMODE_STEREO)
  {
    PMSND_SetStereo(TRUE);  // �X�e���I
  }
  else
  {
    PMSND_SetStereo(FALSE); // ���m����
  }
}

//--------------------------------------------------------------
/**
 * @brief   �X�^�b�N���`�F�b�N
 */
//--------------------------------------------------------------
#ifdef PM_DEBUG
static void DEBUG_StackOverCheck(void)
{
  OSStackStatus stack_status;

  stack_status = OS_GetStackStatus(OS_GetCurrentThread());
  switch(stack_status){
  case OS_STACK_OVERFLOW:
    GF_ASSERT("�X�^�b�N�ŉ��ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
    break;
  case OS_STACK_UNDERFLOW:
    GF_ASSERT("�X�^�b�N�ŏ�ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
    break;
  case OS_STACK_ABOUT_TO_OVERFLOW:
    OS_TPrintf("�X�^�b�N���̌x�������ɒB���Ă��܂�\n");
    break;
  }

  stack_status = OS_GetIrqStackStatus();
  switch(stack_status){
  case OS_STACK_OVERFLOW:
    GF_ASSERT("IRQ�X�^�b�N�ŉ��ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
    break;
  case OS_STACK_UNDERFLOW:
    GF_ASSERT("IRQ�X�^�b�N�ŏ�ʂ̃}�W�b�N�i���o�[�������������Ă��܂�\n");
    break;
  case OS_STACK_ABOUT_TO_OVERFLOW:
    OS_TPrintf("IRQ�X�^�b�N���̌x�������ɒB���Ă��܂�\n");
    break;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�L�[�`�F�b�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_UI_AutoKey( void  )
{
  
  if( (OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER)) ){
    // ���� UPDOWN
    if( DEBUG_FLG_GetFlg( DEBUG_FLG_AutoUpDown ) ){
      DEBUG_UI_SetUp( DEBUG_UI_AUTO_UPDOWN, DEBUG_UI_PLAY_LOOP );
    }else if( DEBUG_FLG_GetFlg( DEBUG_FLG_AutoLeftRight ) ){
      DEBUG_UI_SetUp( DEBUG_UI_AUTO_LEFTRIGHT, DEBUG_UI_PLAY_LOOP );
    }else if( DEBUG_FLG_GetFlg( DEBUG_FLG_AutoInputData ) ){
      DEBUG_UI_SetUp( DEBUG_UI_AUTO_USER_INPUT, DEBUG_UI_PLAY_LOOP );
    }else{
      if( (DEBUG_UI_GetType() == DEBUG_UI_AUTO_UPDOWN) ||
          (DEBUG_UI_GetType() == DEBUG_UI_AUTO_LEFTRIGHT) ||
          (DEBUG_UI_GetType() == DEBUG_UI_AUTO_USER_INPUT) ){
        DEBUG_UI_SetUp( DEBUG_UI_NONE, 0 );
      }
    }

    // Input�T���v�����O
    if( DEBUG_FLG_GetFlg( DEBUG_FLG_SampleInputData ) ){
      DEBUG_UI_INPUT_Start();
    }else{
      DEBUG_UI_INPUT_End();
    }
    DEBUG_UI_INPUT_Update();
  }

}

#endif  //PM_DEBUG



#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON

static void DEBUG_MAIN_TIME_AVERAGE_StartFunc( void )
{
  DEBUG_DEBUG_MAIN_TIME_AVERAGE_Start = OS_GetTick(); 
}

static void DEBUG_MAIN_TIME_AVERAGE_EndFunc( void )
{
  OSTick now_time;
  DEBUG_DEBUG_MAIN_TIME_AVERAGE_End = OS_GetTick(); 

  now_time = OS_TicksToMicroSeconds(DEBUG_DEBUG_MAIN_TIME_AVERAGE_End - DEBUG_DEBUG_MAIN_TIME_AVERAGE_Start);

  DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now = now_time;

  DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave += now_time;
  DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count ++;
  if( now_time > 16000 ){
    DEBUG_DEBUG_MAIN_TIME_AVERAGE_OverCount ++;
#if DEBUG_MAIN_TIME_AVERAGE
    OS_TPrintf( " update %d", DEBUG_MAIN_UPDATE_TYPE );
    OS_TPrintf( " over tick %ld", now_time );
    OS_TPrintf( " ave tick %d\n", (DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave / DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count) );
#endif
  }
  if( DEBUG_DEBUG_MAIN_TIME_AVERAGE_Max < now_time ){
    DEBUG_DEBUG_MAIN_TIME_AVERAGE_Max = now_time;
  }
  if( DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count >= DEBUG_DEBUG_MAIN_TIME_AVERAGE_AVE_COUNT_TIME ){

#if DEBUG_MAIN_TIME_AVERAGE
    OS_TPrintf( " ave tick %d", 
        DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave / DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count );
    OS_TPrintf( " Over 120::%d",
        DEBUG_DEBUG_MAIN_TIME_AVERAGE_OverCount );
    OS_TPrintf( " Max %d\n",
        DEBUG_DEBUG_MAIN_TIME_AVERAGE_Max );
#endif

    DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave = 0;
    DEBUG_DEBUG_MAIN_TIME_AVERAGE_Ave_Count = 0;
    DEBUG_DEBUG_MAIN_TIME_AVERAGE_OverCount = 0;
    DEBUG_DEBUG_MAIN_TIME_AVERAGE_Max       = 0;
  }

#if DEBUG_MAIN_TIME_AVERAGE
  if( DEBUG_DEBUG_MAIN_TIME_AVERAGE_NOW_TIME_DRAW_KEY & GFL_UI_KEY_GetCont() ){
    OS_TPrintf( " update[%d] tick %ld\n", DEBUG_MAIN_UPDATE_TYPE, now_time );
  }
#endif

}

#endif // DEBUG_MAIN_TIME_AVERAGE_MASTER_ON

#ifdef  PLAYABLE_VERSION
//�ۂ�����ځ[���Ԃ��
static const u16 passward[11] = {
  0x307d,   //��
  0x3051,   //��
  0x3082,   //��
  0x3093,   //��
  0x307c,   //��
  0x30fc,   //�[
  0x3060,   //��
  0x3076,   //��
  0x308a,   //��
  0x3086,   //��
  0x0000,   //�I�[����
};

//------------------------------------------------------------------
/**
 * @brief �w��p�X���[�h��{�̃R�����g�ɋL�����Ȃ��ƋN�����Ȃ�
 */
//------------------------------------------------------------------
static void checkOnwerComments( void )
{
  int i;
  BOOL result = TRUE;
  OSOwnerInfo owner_info;
  OS_GetOwnerInfo( &owner_info );

  for ( i = 0; i < NELEMS(passward); i ++ )
  {
    OS_Printf("%04x : %04x\n", owner_info.nickName[i], passward[i] );
    if ( owner_info.nickName[ i ] == passward[ i ] )
    {
      if ( passward[ i ] == 0x0000 )
      {
        break;  //�I�[�����Ȃ̂Ń`�F�b�N�ł��؂�
      }
      continue; //���̕������`�F�b�N
    }
    result = FALSE;
  }
  if ( owner_info.nickNameLength != i )
  {
    result = FALSE;
  }
  if ( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  { //�f�o�b�K��œ��삵�Ă���Ƃ��̓`�F�b�N����
    return;
  }
  if ( result == FALSE )
  {
    GF_PANIC( "password error!\n" );
  }
}

#endif  //  PLAYABLE_VERSION



