//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   mictest.c
 *  @brief    �}�C�N�e�X�g���
 *  @author   Toru=Nagihashi / genya hosaka(GS����WB�ɈڐA)
 *  @data   2008.07.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//  �V�X�e��
#include <gflib.h>
#include "calctool.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "sound/pm_sndsys.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "sound/snd_mic.h"
#include "app/app_menu_common.h"
#include "app_menu_common.naix"


#include "arc_def.h"

//  �O�����J
#include "app/mictest.h"

//  ���\�[�X
#include "mictest.naix"     //  �A�[�J�C�u
#include "message.naix"// GMM
#include "msg/msg_mictest.h"//  GMM

// ===== @TODO �����������̉���` =====

// BGM�֘A
static void Snd_BgmFadeOut( int a, int b ){}
static void Snd_BgmFadeIn( int a, int b, int c){}
#define BGM_VOL_MIN (0)
#define BGM_VOL_MAX (127)
#define BGM_FADEIN_START_VOL_NOW (0)

// ===== �������������̉���`�� =====


/*
 *  �}�C�N�e�X�g�C�V���K��
 *
 *  �E�}�C�N�e�X�g���A�}�C�N�̓��̓��x����5 �i�K�Ŋm�F�ł���悤�ɂ��Ă��������B
 *  �E�Q�C����PM_SetAmpGain���g�p���āAPM_AMPGAIN_80�i80�{�j�ɐݒ�
 *    ��snd_system�̏������ōs���Ă���B
 *  �Eunsigned char�^��8bit�T���v�����O�o�b�t�@��0����255�͈̔͂�L���l�Ƃ��āA128�����̃f�[�^��256���猸�Z���i�g�`�̃��x�����]�j�A�ő�l��T���܂��B
 *    �����P
 *    ����ɂ���ē���ꂽ�f�[�^���ȉ��̂悤�ȋ�؂�ő��肵�Ă��������B
 *  �ELEVEL4  188 �` 255
 *  �ELEVEL3  167 �` 187
 *  �ELEVEL2  153 �` 166
 *  �ELEVEL1  141 �` 152
 *  �ELEVEL0  0 �` 140
 *  �E�T���v�����O���g���͔C��
 *
 *  �ENTSC-ONLINE��DS�}�j���A����S�W�A�S������������ŁA�u�}�C�N�v�ƌ�������ƃK�C�h���C�����łĂ��܂��B
 */

//=============================================================================
/**
 *          �}�N���X�C�b�`
*/
//=============================================================================
//#define MIC_FORMAT_PCM16  //  ��`���O����8bit
//#define TWL_FREQENCY      // DSi�̎��̃T���v�����O���g���𒲍����鏈�����L���ɂȂ�



//=============================================================================
// TWLSDK����Ȃ��Ƒ��݂��Ȃ��֐����������̂œ���͂��Ȃ�����`���Ă���
//=============================================================================
#ifndef SDK_TWL
#define MIC_SAMPLING_RATE_8180  (8180)
extern int MIC_StartLimitedSampling( MICAutoParam *param);
extern void   MIC_StopLimitedSampling(void);

int MIC_StartLimitedSampling( MICAutoParam *param)
{
  return 1;
}
void   MIC_StopLimitedSampling(void)
{
  return;
}
#endif

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
///   �}�N���֐�
//=====================================
//  0�`����-1��Ԃ�Rand
#define MICTEST_RAND_MAX( m ) ( GFUser_GetPublicRand(m) )
//#define MICTEST_RAND_MAX( m ) (gf_rand() * (m+1) / (GF_RAND_MAX + 1))

//--------------------------------------------------------------
/// �Z���A�N�^�[�̃��\�[�XID
//==============================================================
//�� GFL_CLACT_WK_Create �ŘA�ԎQ�Ƃ��Ă��邽�߁A�ύX�s��
enum
{ 
  // �Q�[�W
  MICTEST_RES_M_GAGE_CHAR = 0,
  MICTEST_RES_M_GAGE_PLTT,
  MICTEST_RES_M_GAGE_ANIM,
  // �|�P����
  MICTEST_RES_M_POKE_CHAR,
  MICTEST_RES_M_POKE_PLTT,
  MICTEST_RES_M_POKE_ANIM,
  // ����
  MICTEST_RES_M_NOTE_CHAR,
  MICTEST_RES_M_NOTE_PLTT,
  MICTEST_RES_M_NOTE_ANIM,
  // �{�^��
  MICTEST_RES_S_BTN_CHAR,
  MICTEST_RES_S_BTN_PLTT,
  MICTEST_RES_S_BTN_ANIM,
  // ����
  MICTEST_RES_MAX,
};

#define OBJ_WKNUM   (32)  ///< CLACT_UNIT�ŊǗ�����OBJ��

//-------------------------------------
///   OBJ�̌ʌ�
//=====================================
#define OBJ_NOTE_MAX  (3)

//-------------------------------------
///   OBJ����
//=====================================
enum {
  MICTEST_OBJ_GAGE_L, //�Q�[�W��
  MICTEST_OBJ_GAGE_R, //�Q�[�W�E
  MICTEST_OBJ_POKEMON,//�|�P����
  MICTEST_OBJ_BUTTON, //�{�^��
  MICTEST_OBJ_NOTE, //����
  MICTEST_OBJ_NOTE_END  = MICTEST_OBJ_NOTE + OBJ_NOTE_MAX - 1,
  MICTEST_OBJ_MAX,
};

//-------------------------------------
///   BWPWIN
//=====================================
enum {
  MICTEST_BMPWIN_TITLE,
  MICTEST_BMPWIN_INFO,
  MICTEST_BMPWIN_RETBTN,
  MICTEST_BMPWIN_MAX,
};

//-------------------------------------
///   OBJ�̏����ʒu
//=====================================
#define OBJ_GAGE_L_POS_X    (128-64)
#define OBJ_GAGE_L_POS_Y    (96)
#define OBJ_GAGE_R_POS_X    (128+64)
#define OBJ_GAGE_R_POS_Y    (96)
#define OBJ_POKEMON_POS_X   (128)
#define OBJ_POKEMON_POS_Y   (96)
#define OBJ_NOTE_POS_X      (OBJ_POKEMON_POS_X)
#define OBJ_NOTE_POS_Y      (OBJ_POKEMON_POS_Y-32)
#define OBJ_BUTTON_POS_X    (27*8)
#define OBJ_BUTTON_POS_Y    (22*8)

//-------------------------------------
///   �}�C�N�̃T���v�����O���[�g���̏���
//=====================================
#define MIC_SAMPLING_RATE   (8000)
#define MIC_SAMPLING_TIME   (32) // msec
#define MIC_SAMPLING_LENGTH (MIC_SAMPLING_RATE * MIC_SAMPLING_TIME / 1000)

#ifdef MIC_FORMAT_PCM16
#define MIC_SAMPLING_TYPE   MIC_SAMPLING_TYPE_12BIT
typedef u16 MIC_BUF_SIZE;
#else
#define MIC_SAMPLING_TYPE    MIC_SAMPLING_TYPE_8BIT; //MIC_SAMPLING_TYPE_SIGNED_8BIT;
typedef u8 MIC_BUF_SIZE;
#endif  // MIC_FORMAT_PCM16

//-------------------------------------
///   �}�C�N�K��ɂ�郌�x�����
//=====================================
#define IS_MICINPUT_LEVEL0( n ) (  0<=n && n<=140)
#define IS_MICINPUT_LEVEL1( n ) (141<=n && n<=152)
#define IS_MICINPUT_LEVEL2( n ) (153<=n && n<=166)
#define IS_MICINPUT_LEVEL3( n ) (167<=n && n<=187)
#define IS_MICINPUT_LEVEL4( n ) (188<=n && n<=255)

//-------------------------------------
///   �������h�ꓮ�������̒萔
//=====================================
//  �����_����
#define NOTE_SPEED_MIN      FX32_CONST(1.2f)
#define NOTE_SPEED_MAX      FX32_CONST(2.2f)
#define NOTE_ANGLE_MIN      (60)
#define NOTE_ANGLE_MAX      (120)
#define NOTE_WIDTH_MIN      (6)
#define NOTE_WIDTH_MAX      (20)
/*#define NOTE_WAIT_MIN     (1)
#define NOTE_WAIT_MAX     (5)
#define NOTE_WAIT_SUM     (6)
*/
#define NOTE_SPEED_RANGE    (NOTE_SPEED_MAX - NOTE_SPEED_MIN + 1)
#define NOTE_ANGLE_RANGE    (NOTE_ANGLE_MAX - NOTE_ANGLE_MIN + 1)
#define NOTE_WIDTH_RANGE    (NOTE_WIDTH_MAX - NOTE_WIDTH_MIN + 1)
//#define NOTE_WAIT_RANGE     (NOTE_WAIT_MAX - NOTE_WAIT_MIN + 1)

//-------------------------------------
///   BMPWIN�̈ʒu
//=====================================
#define BMPWIN_TITLE_FRAME    (GFL_BG_FRAME0_M)
#define BMPWIN_TITLE_X      (0)
#define BMPWIN_TITLE_X_DOT  (5)
#define BMPWIN_TITLE_Y      (1)
#define BMPWIN_TITLE_W      (10)
#define BMPWIN_TITLE_H      (3)
#define BMPWIN_TITLE_PLT    (13)
#define BMPWIN_TITLE_CHR_OFS  (1)
#define BMPWIN_INFO_FRAME   (GFL_BG_FRAME0_S)
#define BMPWIN_INFO_X     (5)
#define BMPWIN_INFO_Y     (8)
#define BMPWIN_INFO_W     (22)
#define BMPWIN_INFO_H     (5)
#define BMPWIN_INFO_PLT     (13)
#define BMPWIN_INFO_CHR_OFS   (1)
#define BMPWIN_RETBTN_FRAME   (GFL_BG_FRAME0_S)
#define BMPWIN_RETBTN_X     (23)
#define BMPWIN_RETBTN_Y     (21)
#define BMPWIN_RETBTN_W     (6)
#define BMPWIN_RETBTN_H     (2)
#define BMPWIN_RETBTN_PLT   (13)
#define BMPWIN_RETBTN_CHR_OFS (1+BMPWIN_INFO_W*BMPWIN_INFO_H)

//-------------------------------------
///   �J�E���g
//=====================================
#define GAGE_DECREASE     (2)
#define NOTE_ENTER_CNT_MAX    (10)
#define BGM_FADE_CNT_MAX    (10)

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
///   �V�[�P���X�Ǘ�
//=====================================
typedef struct _MICTEST_SEQ_WORK MICTEST_SEQ_WORK;
typedef void (*SEQFUNC)( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
struct _MICTEST_SEQ_WORK{
  void        *p_wk_adrs;
  SEQFUNC       SeqFunction;
  u32         seq;
  BOOL        is_end;
};

//-------------------------------------
///   �������������߂̃��[�N�G���A
//=====================================
typedef struct {
  GFL_CLWK*   p_act;
  u32         wait;
  VecFx32       pos;
  VecFx32       speed;
  fx32        shake_pt;
  fx32        shake_speed;
  fx32        shake_width;
}NOTE_MOVE_WORK;

//-------------------------------------
//  �}�C�N�e�X�gBG�p���[�N
//=====================================
typedef struct {
  GFL_BMPWIN    *bmpwin[MICTEST_BMPWIN_MAX];
  GFL_MSGDATA   *p_msg_mng; //< MSG�`���BMPWIN�Ȃ̂ł������
  GFL_FONT      *fontHandle;
}MICTEST_BG_WORK;

//-------------------------------------
///   �^�X�N�Ǘ��\����
//=====================================
typedef struct {
  NOTE_MOVE_WORK    note_wk;
  GFL_TCB     *tcb_ptr;
} NOTE_TASK_CTRL;

//-------------------------------------
//  �}�C�N�e�X�gOBJ�p���[�N
//=====================================
typedef struct {
  GFL_CLWK    *p_act[MICTEST_OBJ_MAX];
  GFL_CLUNIT  *p_cell_unit;
  u32 resObjTbl[ MICTEST_RES_MAX ];

  //  �����p
  NOTE_TASK_CTRL  note_task_ctrl[OBJ_NOTE_MAX];
  u32             note_cnt;
}MICTEST_OBJ_WORK;

//-------------------------------------
//  �}�C�N�e�X�g�}�C�N�p���[�N
//=====================================
typedef struct {
  MICAutoParam    param;
  void        *p_buf_adrs;
  s32         prev_volume;  //< �}�C�N���ʌ����p
  BOOL        anm_set_flg;
  BOOL        is_start;
  BOOL        pre_start_sampling;
  BOOL        mic_use_flag;
  BOOL        pre_mic_use;
}MICTEST_MIC_WORK;

//-------------------------------------
//  �}�C�N�e�X�g���C�����[�N
//=====================================
typedef struct {
  MICTEST_OBJ_WORK  obj;
  MICTEST_BG_WORK   bg;
  MICTEST_MIC_WORK  mic;
  MICTEST_SEQ_WORK  seq;
  void* tcbWork;
  GFL_TCBSYS* tcbSys;
  GFL_TCB*    tcbVBlank;

  int heap_id;
}MICTEST_MAIN_WORK;


//=============================================================================
/**
 *          �v���g�^�C�v�錾
*/
//=============================================================================

static GFL_PROC_RESULT MicTestProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT MicTestProc_Exit( GFL_PROC *proc,int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT MicTestProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *mywk );

//-------------------------------------
///   �A�v���P�[�V����
//=====================================
static void MicTest_InitApplication( u32 heap_id );
static void MicTest_ExitApplication( void );
static void MicTest_SetVramBank( void );
static void MicTest_VBlankFunction( GFL_TCB * tcb, void * p_wk_adrs );

//-------------------------------------
///   OBJ�Ǘ�
//=====================================
static void MicTest_OBJ_Init( MICTEST_OBJ_WORK *p_obj, u32 heap_id );
static void MicTest_OBJ_Exit( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_Main( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_SetClact( MICTEST_OBJ_WORK *p_obj, HEAPID heap_id );
static void MicTest_OBJ_DeleteClact( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_VBlancFunction( MICTEST_OBJ_WORK *p_obj );

//-------------------------------------
///   BG�Ǘ�
//=====================================
static void MicTest_BG_Init( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTest_BG_Exit( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_Main( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_LoadBg( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTest_BG_UnLoadBg( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_VBlancFunction( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_CreateMsg( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTest_BG_DeleteMsg( MICTEST_BG_WORK *p_bg );

//-------------------------------------
///   �}�C�N�Ǘ�
//=====================================
static void MicTest_MIC_Init( MICTEST_MIC_WORK *p_mic, u32 heap_id, MICCallback MicCallback, void*p_mic_arg );
static void MicTest_MIC_Exit( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_Main( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_StartSampling( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_StopSampling( MICTEST_MIC_WORK *p_mic );

static void ChangeObjAnmMicCallback( MICResult result, void *p_arg );
static int CalcAverageVolumeMicBuffer( MICAutoParam * p_mic_param );
static u8 GetMicLevelToAnmSeqNum( u8 volume );

//-------------------------------------
///   TASK�֌W
//=====================================
//  Note
static BOOL MoveStartNoteCtrl( MICTEST_OBJ_WORK *p_wk, u32 heap_id, s16 x, s16 y, GFL_TCBSYS* tcbSys );
static void MoveStartNoteTask( GFL_TCBSYS* tcbSys, GFL_CLWK*  p_act, NOTE_TASK_CTRL *p_task_wk, /*u32 heap_id,*/ s16 x, s16 y, fx32 speed, u32 angle, u32 width, u32 wait );
static void MoveNoteTask( GFL_TCB* p_tcb, void *p_wk_adrs );
static void CleanNoteTask( MICTEST_OBJ_WORK *p_obj );
static BOOL IsEmptyNoteTask( const MICTEST_OBJ_WORK *cp_obj );

//-------------------------------------
///   SEQ
//=====================================
static void MicTest_SEQ_Init( MICTEST_SEQ_WORK *p_seq, void * p_wk_adrs, SEQFUNC SeqFunction );
static void MicTest_SEQ_Main( MICTEST_SEQ_WORK *p_seq );
static void MicTest_SEQ_ChangeSeq( MICTEST_SEQ_WORK *p_seq, SEQFUNC SeqFunction );
static void* MicTest_SEQ_GetWk( const MICTEST_SEQ_WORK *cp_seq );
static BOOL MicTest_SEQ_IsEnd( const MICTEST_SEQ_WORK *cp_seq );
static void MicTest_SEQ_End( MICTEST_SEQ_WORK *p_seq );

//-------------------------------------
///   SEQ�֐�
//=====================================
static void SEQFUNC_FadeIn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_Main( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_RetBtn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_FadeOut( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_End( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );

//-------------------------------------
///   ���̑�
//=====================================
static int GetHitTrg( void ); //  Trg���m�i���W�e�[�u������j
static BOOL IsProcEndTrg( void );
#ifdef TWL_FREQENCY
static SNDEXFrequency GetI2SFrequency(void);
#endif

//=============================================================================
/**
 *          �f�[�^�̈�
*/
//=============================================================================

//�I�[�o�[���C�v���Z�X��`�f�[�^
const GFL_PROC_DATA TitleMicTestProcData = {
  MicTestProc_Init,
  MicTestProc_Main,
  MicTestProc_Exit,
//  FS_OVERLAY_ID(mictest),
//  NO_OVERLAY_ID,
};

//-------------------------------------
///   BG�ʐݒ�f�[�^  �i�{��MicTest_BG_Init�����ɂ��������悢��
//              MicTest_BG_Exit�ŉ���Ɏg�����ߊO���ɂ����j
//=====================================
static const struct {
  GFL_BG_BGCNT_HEADER header;
  u8 frame;
    u8 mode;
} sc_bg_cnt_data[] = {
  {
    { 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000, 0x4000, GX_BG_EXTPLTT_01,
      0,0,0,FALSE },
    GFL_BG_FRAME0_M,
    GFL_BG_MODE_TEXT,
  },
  {
    { 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000,GX_BG_CHARBASE_0x04000, 0x4000, GX_BG_EXTPLTT_01,
      1,0,0,FALSE },
    GFL_BG_FRAME1_M,
    GFL_BG_MODE_TEXT,
  },
  {
    { 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800,GX_BG_CHARBASE_0x08000, 0x4000, GX_BG_EXTPLTT_01,
      2,0,0,FALSE },
    GFL_BG_FRAME2_M,
    GFL_BG_MODE_TEXT,
  },
  {
    { 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000, 0x4000, GX_BG_EXTPLTT_01,
      0,0,0,FALSE },
    GFL_BG_FRAME0_S,
    GFL_BG_MODE_TEXT,
  },
  {
    { 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000,GX_BG_CHARBASE_0x04000, 0x4000, GX_BG_EXTPLTT_01,
      1,0,0,FALSE },
    GFL_BG_FRAME1_S,
    GFL_BG_MODE_TEXT,
  },
};
  
//--------------------------------------------------------------
/// VRAM BANK
//==============================================================
static const GFL_DISP_VRAM c_vram_bank = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_32K, // Main Mapping Mode
  GX_OBJVRAMMODE_CHAR_1D_32K, // Sub Mapping Mode
};

//=============================================================================
/**
 *          gloval function
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �v���b�N�J�n����
 *
 *  @param  GFL_PROC *proc  �v���b�N���[�N
 *  @param  *seq    �V�[�P���X�ԍ�
 *  @param  *pwk
 *  @param  *mywk 
 *
 *  @return �v���b�N���
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MicTestProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
  MICTEST_MAIN_WORK *p_wk;

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MICTEST, 0x70000 );
  
  p_wk = GFL_PROC_AllocWork( proc,sizeof(MICTEST_MAIN_WORK),HEAPID_MICTEST );
  MI_CpuClear8( p_wk, sizeof(MICTEST_MAIN_WORK) );

  p_wk->heap_id = HEAPID_MICTEST;

  //  appli
  MicTest_InitApplication( p_wk->heap_id );
  MicTest_SetVramBank();

  // TCB
  p_wk->tcbWork = GFL_HEAP_AllocMemory( p_wk->heap_id , GFL_TCB_CalcSystemWorkSize( 5 ) );
  p_wk->tcbSys = GFL_TCB_Init( 5 , p_wk->tcbWork );
  
  p_wk->tcbVBlank = GFUser_VIntr_CreateTCB( MicTest_VBlankFunction, p_wk, 1 );

  //  BG
  MicTest_BG_Init( &p_wk->bg, p_wk->heap_id );
  MicTest_BG_LoadBg( &p_wk->bg, p_wk->heap_id );
  MicTest_BG_CreateMsg( &p_wk->bg, p_wk->heap_id );

  //  OBJ
  MicTest_OBJ_Init( &p_wk->obj, p_wk->heap_id );
  MicTest_OBJ_SetClact( &p_wk->obj, p_wk->heap_id );

  //  MIC
  MicTest_MIC_Init( &p_wk->mic, p_wk->heap_id, ChangeObjAnmMicCallback, p_wk );

  //  SEQ
  MicTest_SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeIn );

  //  �Ȃ̓t�F�[�h�A�E�g
  Snd_BgmFadeOut( BGM_VOL_MIN, BGM_FADE_CNT_MAX );

  return GFL_PROC_RES_FINISH;
}


// �^�C�g���֔��
extern const GFL_PROC_DATA TitleProcData;
FS_EXTERN_OVERLAY( title );

//----------------------------------------------------------------------------
/**
 *  @brief  �v���b�N�I������
 *
 *  @param  GFL_PROC *proc  �v���b�N���[�N
 *  @param  *seq    �V�[�P���X�ԍ�
 *  @param  *pwk
 *  @param  *mywk 
 *
 *  @return �v���b�N���
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MicTestProc_Exit( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
  MICTEST_MAIN_WORK* p_wk = mywk;

  GFL_TCB_DeleteTask( p_wk->tcbVBlank );

  GFL_TCB_Exit( p_wk->tcbSys );
  GFL_HEAP_FreeMemory( p_wk->tcbWork );

  //  MIC
  MicTest_MIC_Exit( &p_wk->mic );

  //  OBJ
  MicTest_OBJ_DeleteClact( &p_wk->obj );
  MicTest_OBJ_Exit( &p_wk->obj );

  //  BG
  MicTest_BG_DeleteMsg( &p_wk->bg );
  MicTest_BG_Exit( &p_wk->bg );

  //  appli
  MicTest_ExitApplication();

  //���[�N�G���A���
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( p_wk->heap_id );

  //  �Ȃ̓t�F�[�h�C��
  Snd_BgmFadeIn( BGM_VOL_MAX, BGM_FADE_CNT_MAX, BGM_FADEIN_START_VOL_NOW );

  GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
//  Main_SetNextProc( FS_OVERLAY_ID(title), &TitleProcData );

  return GFL_PROC_RES_FINISH;
}

#include "sound/pm_wb_voice.h"
#include "poke_tool/monsno_def.h"
//-----------------------------------------------------------------------------
/**
 *  @brief  �v���b�N���C������
 *
 *  @param  GFL_PROC *proc  �v���b�N���[�N
 *  @param  *seq    �V�[�P���X�ԍ�
 *  @param  *pwk
 *  @param  *mywk 
 *
 *  @return �v���b�N���
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MicTestProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
  MICTEST_MAIN_WORK* p_wk = mywk;

  if( MicTest_SEQ_IsEnd( &p_wk->seq ) ) {
    return GFL_PROC_RES_FINISH;
  }
  MicTest_SEQ_Main( &p_wk->seq );

  MicTest_OBJ_Main( &p_wk->obj );
  MicTest_BG_Main( &p_wk->bg );
  MicTest_MIC_Main( &p_wk->mic );

  GFL_TCB_Main( p_wk->tcbSys );
  
  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *        �V�[�P���X  seq
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X�V�X�e��������
 *
 *  @param  MICTEST_SEQ_WORK *p_seq �V�[�P���X�Ǘ����[�N
 *  @param  * p_wk_adrs       �V�[�P���X�֐��ɓn�����[�N
 *  @param  SeqFunction       �ŏ��Ɏ��s����V�[�P���X�֐�
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_Init( MICTEST_SEQ_WORK *p_seq, void * p_wk_adrs, SEQFUNC SeqFunction )
{
  p_seq->p_wk_adrs  = p_wk_adrs;
  p_seq->is_end   = FALSE;

  MicTest_SEQ_ChangeSeq( p_seq, SeqFunction );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X���C������
 *
 *  @param  MICTEST_SEQ_WORK *p_seq   �V�[�P���X�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_Main( MICTEST_SEQ_WORK *p_seq )
{
  if( MicTest_SEQ_IsEnd( p_seq ) == FALSE ) {
    p_seq->SeqFunction( p_seq, &p_seq->seq );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X�؂�ւ�
 *
 *  @param  MICTEST_SEQ_WORK *p_seq �V�[�P���X�Ǘ����[�N
 *  @param  SeqFunction       �؂�ւ���V�[�P���X�֐�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_ChangeSeq( MICTEST_SEQ_WORK *p_seq, SEQFUNC SeqFunction )
{
  p_seq->SeqFunction  = SeqFunction;
  p_seq->seq      = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X�֐��ɓn�����[�N�擾
 *
 *  @param  const MICTEST_SEQ_WORK *cp_seq  �V�[�P���X�Ǘ����[�N 
 *
 *  @return �V�[�P���X�֐��ɓn�����[�N�̃A�h���X
 */
//-----------------------------------------------------------------------------
static void* MicTest_SEQ_GetWk( const MICTEST_SEQ_WORK *cp_seq )
{
  return cp_seq->p_wk_adrs;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X���I���������ǂ���
 *
 *  @param  const MICTEST_SEQ_WORK *cp_seq  �V�[�P���X�Ǘ����[�N
 *
 *  @return �V�[�P���X���I��������
 */
//-----------------------------------------------------------------------------
static BOOL MicTest_SEQ_IsEnd( const MICTEST_SEQ_WORK *cp_seq )
{
  return cp_seq->is_end;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X�V�X�e���I��
 *
 *  @param  MICTEST_SEQ_WORK *p_seq 
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_End( MICTEST_SEQ_WORK *p_seq )
{
  p_seq->is_end = TRUE;
}

//=============================================================================
/**
 *        �V�[�P���X�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�h�C��
 *
 *  @param  MICTEST_SEQ_WORK *p_seq_wk  �V�[�P���X�Ǘ����[�N
 *  @param  *p_seq          �V�[�P���X�ԍ�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
  enum {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
  };
  MICTEST_MAIN_WORK *p_wk = MicTest_SEQ_GetWk( p_seq_wk );

  switch( *p_seq ) {
  case SEQ_INIT :
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
        0x0000, COMM_BRIGHTNESS_SYNC, 1, p_wk->heap_id );
    *p_seq = SEQ_MAIN;
    break;
  
  case SEQ_MAIN:
    if( WIPE_SYS_EndCheck() ){
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_Main );
    break;

  default :
    GF_ASSERT( 0 );
  };
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N�e�X�g���C������
 *
 *  @param  MICTEST_SEQ_WORK *p_seq_wk  �V�[�P���X�Ǘ����[�N
 *  @param  *p_seq          �V�[�P���X�ԍ�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
  enum {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
    SEQ_END,
  };
  MICTEST_MAIN_WORK *p_wk = MicTest_SEQ_GetWk( p_seq_wk );

  switch( *p_seq ) {
  case SEQ_INIT :
    MicTest_MIC_StartSampling( &p_wk->mic );
    *p_seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    if( IsProcEndTrg() ) {
      PMSND_PlaySE( SEQ_SE_CANCEL1 );
      *p_seq = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    MicTest_MIC_StopSampling( &p_wk->mic );
    *p_seq = SEQ_END;
    break;

  case SEQ_END:
    MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_RetBtn );
    break;

  default :
    GF_ASSERT( 0 );
  };
}

//----------------------------------------------------------------------------
/**
 *  @brief  �߂�{�^�����������Ƃ��̏���
 *
 *  @param  MICTEST_SEQ_WORK *p_seq_wkq �V�[�P���X�Ǘ����[�N
 *  @param  *p_seq          �V�[�P���X�ԍ�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RetBtn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
  enum {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
  };
  MICTEST_MAIN_WORK *p_wk = MicTest_SEQ_GetWk( p_seq_wk );

  switch( *p_seq ) {
  case SEQ_INIT :
    GFL_CLACT_WK_SetAnmSeq( p_wk->obj.p_act[MICTEST_OBJ_BUTTON], 1 );
    *p_seq = SEQ_MAIN;
    break;

  case SEQ_MAIN :
    if( GFL_CLACT_WK_CheckAnmActive( p_wk->obj.p_act[MICTEST_OBJ_BUTTON] ) == FALSE ) {
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_FadeOut );
    break;

  default :
    GF_ASSERT( 0 );
  };
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�h�A�E�g
 *
 *  @param  MICTEST_SEQ_WORK *p_seq_wk  �V�[�P���X�Ǘ����[�N
 *  @param  *p_seq          �V�[�P���X�ԍ�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
  enum {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
  };
  MICTEST_MAIN_WORK *p_wk = MicTest_SEQ_GetWk( p_seq_wk );

  switch( *p_seq ) {
  case SEQ_INIT :
    WIPE_SYS_Start(WIPE_PATTERN_WMS,
          WIPE_TYPE_FADEOUT,
          WIPE_TYPE_FADEOUT, 0x0000, COMM_BRIGHTNESS_SYNC, 1, p_wk->heap_id
          );
    *p_seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    if( WIPE_SYS_EndCheck() ){
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_End );
    break;

  default :
    GF_ASSERT( 0 );
  };
}

//----------------------------------------------------------------------------
/**
 *  @brief  �I������
 *
 *  @param  MICTEST_SEQ_WORK *p_seq_wk  �V�[�P���X�Ǘ����[�N
 *  @param  *p_seq          �V�[�P���X�ԍ�
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
  MICTEST_MAIN_WORK *p_wk = MicTest_SEQ_GetWk( p_seq_wk );
  CleanNoteTask( &p_wk->obj );
  MicTest_SEQ_End( p_seq_wk );
}

//=============================================================================
/**
 *          system
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �A�v���P�[�V����������
 *
 *  @param  u32 heap_id   �q�[�vID
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_InitApplication( u32 heap_id )
{
  //  �\���ݒ����
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );

  //  Wnd�ݒ����
  WIPE_ResetWndMask( WIPE_DISP_MAIN );
  WIPE_ResetWndMask( WIPE_DISP_SUB ); 
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�v���P�[�V�����I��
 *
 *  @param  void 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_ExitApplication( void )
{
  //  ��\��
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();  
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 ); 
}


//----------------------------------------------------------------------------
/**
 *  @brief  �o���N�e�[�u���Z�b�g
 *
 *  @param  void 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_SetVramBank( void )
{
  GFL_DISP_SetBank( &c_vram_bank );
}

//----------------------------------------------------------------------------
/**
 *  @brief  VBlank�֐�
 *
 *  @param  GFL_TCB * tcb
 *  @param  void * work 
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static void MicTest_VBlankFunction( GFL_TCB * tcb, void * p_wk_adrs )
{
  MICTEST_MAIN_WORK *p_wk = p_wk_adrs;

  NNS_GfdDoVramTransfer();  //VRam�]���}�l�[�W�����s
  MicTest_OBJ_VBlancFunction( &p_wk->obj );
  MicTest_BG_VBlancFunction( &p_wk->bg );

  OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}


//=============================================================================
/**
 *    OBJ�Ǘ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�V�X�e��������
 *
 *  @param  MicTest_OBJ_WORK *p_obj OBJ�p���[�N
 *  @param  heap_id         �q�[�vID
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Init( MICTEST_OBJ_WORK *p_obj, u32 heap_id )
{
  //  �V�X�e���쐬
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &c_vram_bank, heap_id );

  //  �\��
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�V�X�e���j��
 *
 *  @param  MicTest_OBJ_WORK *p_obj OBJ�p���[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Exit( MICTEST_OBJ_WORK *p_obj )
{
  GFL_CLACT_SYS_Delete();
}

//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�V�X�e�����C������
 *
 *  @param  MICTEST_OBJ_WORK *p_obj 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Main( MICTEST_OBJ_WORK *p_obj )
{
  u16 i;
  for( i = 0; i < MICTEST_OBJ_MAX; i++ ){
    GFL_CLACT_WK_AddAnmFrame( p_obj->p_act[i], FX32_ONE );
  }

  GFL_CLACT_SYS_Main();
}

//----------------------------------------------------------------------------
/**
 *  @brief  OBJCLACT����
 *
 *  @param  MICTEST_OBJ_WORK *p_obj 
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_SetClact( MICTEST_OBJ_WORK *p_obj, HEAPID heap_id )
{

  ARCHANDLE* arcHandle, *c_arcHandle;
  u16 i, param_no;

  // �Z���A�N�^�[�Z�b�g����
  p_obj->p_cell_unit = GFL_CLACT_UNIT_Create( OBJ_WKNUM, 0, heap_id );

  //==========================================================================
  // �����\�[�X�W�J
  //==========================================================================
  
  //�@�n���h������
  arcHandle   = GFL_ARC_OpenDataHandle( ARCID_MICTEST_GRA, heap_id );
  c_arcHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heap_id );

  // ----- LCD MAIN -----
  
  // �Q�[�W
  p_obj->resObjTbl[MICTEST_RES_M_GAGE_PLTT] =
    GFL_CLGRP_PLTT_Register( arcHandle, NARC_mictest_obj_gage_NCLR, CLSYS_DRAW_MAIN, 0, heap_id );

  p_obj->resObjTbl[MICTEST_RES_M_GAGE_CHAR] = 
    GFL_CLGRP_CGR_Register( arcHandle, NARC_mictest_obj_gage_NCGR, 0, CLSYS_DRAW_MAIN, heap_id );

  p_obj->resObjTbl[MICTEST_RES_M_GAGE_ANIM] =
    GFL_CLGRP_CELLANIM_Register( arcHandle, NARC_mictest_obj_gage_NCER, NARC_mictest_obj_gage_NANR, heap_id );
  
  // �|�P����
  p_obj->resObjTbl[MICTEST_RES_M_POKE_PLTT] =
    GFL_CLGRP_PLTT_Register( arcHandle, NARC_mictest_obj_pokemon_NCLR, CLSYS_DRAW_MAIN, 0x20, heap_id );
  
  p_obj->resObjTbl[MICTEST_RES_M_POKE_CHAR] = 
    GFL_CLGRP_CGR_Register( arcHandle, NARC_mictest_obj_pokemon_NCGR, 0, CLSYS_DRAW_MAIN, heap_id );

  p_obj->resObjTbl[MICTEST_RES_M_POKE_ANIM] =
    GFL_CLGRP_CELLANIM_Register( arcHandle, NARC_mictest_obj_pokemon_NCER, NARC_mictest_obj_pokemon_NANR, heap_id );
  
  //  ����
  p_obj->resObjTbl[MICTEST_RES_M_NOTE_PLTT] =
    GFL_CLGRP_PLTT_Register( arcHandle, NARC_mictest_obj_note_NCLR, CLSYS_DRAW_MAIN, 0x40, heap_id );

  p_obj->resObjTbl[MICTEST_RES_M_NOTE_CHAR] = 
    GFL_CLGRP_CGR_Register( arcHandle, NARC_mictest_obj_note_NCGR, 0, CLSYS_DRAW_MAIN, heap_id );

  p_obj->resObjTbl[MICTEST_RES_M_NOTE_ANIM] =
    GFL_CLGRP_CELLANIM_Register( arcHandle, NARC_mictest_obj_note_NCER, NARC_mictest_obj_note_NANR, heap_id );


  // ----- LCD SUB -----
  
  //  �{�^��
  p_obj->resObjTbl[MICTEST_RES_S_BTN_PLTT] =
    GFL_CLGRP_PLTT_Register( c_arcHandle, NARC_app_menu_common_task_menu_NCLR, CLSYS_DRAW_SUB, 0, heap_id );

  p_obj->resObjTbl[MICTEST_RES_S_BTN_CHAR] = 
    GFL_CLGRP_CGR_Register( c_arcHandle, NARC_app_menu_common_task_menu_NCGR, 0, CLSYS_DRAW_SUB, heap_id );

  p_obj->resObjTbl[MICTEST_RES_S_BTN_ANIM] =
    GFL_CLGRP_CELLANIM_Register( arcHandle, NARC_mictest_obj_button_NCER, NARC_mictest_obj_button_NANR, heap_id );
  
  //�@�n���h�������
  GFL_ARC_CloseDataHandle( c_arcHandle );
  GFL_ARC_CloseDataHandle( arcHandle );
  
  //==========================================================================
  // ���A�N�^�[����
  //==========================================================================
  { 
    static const GFL_CLWK_DATA sc_obj_add_param[] = 
    {
      //  s16 pos_x;        // �����W
      //  s16 pos_y;        // �����W
      //  u16 anmseq;       // �A�j���[�V�����V�[�P���X
      //  u8  softpri;      // �\�t�g�D�揇�� 0>0xff
      //  u8  bgpri;        // bg�D�揇��

      //  GAGE_L
      {
        OBJ_GAGE_L_POS_X, OBJ_GAGE_L_POS_Y, 0, 0, 0, 
      },
      //  GAGE_R
      {
        OBJ_GAGE_R_POS_X, OBJ_GAGE_R_POS_Y, 0, 0, 0,
      },
      //  POKEMON
      {
        OBJ_POKEMON_POS_X, OBJ_POKEMON_POS_Y, 0, 0, 0,
      },
      //  BUTTON
      {
        OBJ_BUTTON_POS_X, OBJ_BUTTON_POS_Y, 0, 0, 1,
      },
      //  NOTE
      {
        0, 0, 0, 0, 0,
      },
    };

    // 0=���\�[�XID�̐擪�e�[�u��, 1=�T�[�t�F�[�X�w��, 2=�Q�Ƃ���GFL_CLWK_DATA��IDX
    static const u32 sc_res_data_tbl[MICTEST_OBJ_MAX][3]  = { 
        MICTEST_RES_M_GAGE_CHAR,  CLSYS_DEFREND_MAIN, 0,
        MICTEST_RES_M_GAGE_CHAR,  CLSYS_DEFREND_MAIN, 1,
        MICTEST_RES_M_POKE_CHAR,  CLSYS_DEFREND_MAIN, 2,
        MICTEST_RES_S_BTN_CHAR,   CLSYS_DEFREND_SUB,  3,
        MICTEST_RES_M_NOTE_CHAR,  CLSYS_DEFREND_MAIN, 4,
        MICTEST_RES_M_NOTE_CHAR,  CLSYS_DEFREND_MAIN, 4,
        MICTEST_RES_M_NOTE_CHAR,  CLSYS_DEFREND_MAIN, 4,
    };
    
    //  �A�N�^�[����
    for( i = 0; i < MICTEST_OBJ_MAX; i++ ) {
          
      GF_ASSERT( sc_res_data_tbl[i][0]+2 < MICTEST_RES_MAX );

      p_obj->p_act[i] = GFL_CLACT_WK_Create( p_obj->p_cell_unit,
          p_obj->resObjTbl[ sc_res_data_tbl[i][0] ],
          p_obj->resObjTbl[ sc_res_data_tbl[i][0]+1 ],
          p_obj->resObjTbl[ sc_res_data_tbl[i][0]+2 ],
          &sc_obj_add_param[ sc_res_data_tbl[i][2] ], sc_res_data_tbl[i][1], heap_id );

      //�@�I�[�g�A�j���t���O�Z�b�g
      GFL_CLACT_WK_SetAutoAnmFlag( p_obj->p_act[i], TRUE );
    }
  }
    
  //  �����͔�\��
  for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
    GFL_CLACT_WK_SetDrawEnable( p_obj->p_act[ MICTEST_OBJ_NOTE + i ], FALSE );
  }
  
  //  �ʐݒ�
  GFL_CLACT_WK_SetAffineParam( p_obj->p_act[MICTEST_OBJ_GAGE_L], CLSYS_AFFINETYPE_NONE );
  GFL_CLACT_WK_SetFlip( p_obj->p_act[MICTEST_OBJ_GAGE_L], CLWK_FLIP_H, TRUE );

  // �����̐���ێ�
  p_obj->note_cnt = NOTE_ENTER_CNT_MAX;
}

//----------------------------------------------------------------------------
/**
 *  @brief  clact�I��
 *
 *  @param  mictest_obj_work *p_obj 
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_DeleteClact( MICTEST_OBJ_WORK *p_obj )
{
  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete( p_obj->p_cell_unit );
}

//----------------------------------------------------------------------------
/**
 *  @brief  OBJVBlanc������
 *
 *  @param  MICTEST_OBJ_WORK *p_obj OBJ�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_VBlancFunction( MICTEST_OBJ_WORK *p_obj )
{
  GFL_CLACT_SYS_VBlankFunc();
}


//=============================================================================
/**
 *    BG�Ǘ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  BG�V�X�e��������
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *  @param  heap_id         �Ǘ��p���[�N�A���b�N�p�q�[�v
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Init( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
  //  �V�X�e��
  GFL_BG_Init( heap_id );
  
  // BMPWIN
  GFL_BMPWIN_Init( heap_id );

  //  �V�X�e��������
  {
    static const GFL_BG_SYS_HEADER bg_sys_header = {
      GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
    };
    GFL_BG_SetBGMode( &bg_sys_header );
  }
  
  // �^�b�`�t�H���g���[�h
  p_bg->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heap_id );

  //  BG�ʐݒ�
  {
    /* BG�ʐݒ�f�[�^���O���ֈړ� */
    int i;
    for( i = 0; i < NELEMS(sc_bg_cnt_data); i++ ){
      
      GFL_BG_SetBGControl( sc_bg_cnt_data[i].frame, &sc_bg_cnt_data[i].header, sc_bg_cnt_data[i].mode );
      GFL_BG_ClearScreen( sc_bg_cnt_data[i].frame );
      GFL_BG_SetClearCharacter( sc_bg_cnt_data[i].frame, 32, 0, heap_id );
      GFL_BG_SetVisible( sc_bg_cnt_data[i].frame, VISIBLE_ON );
    }
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  BG�V�X�e�����
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Exit( MICTEST_BG_WORK *p_bg )
{
  int i;
  for( i = 0; i < NELEMS(sc_bg_cnt_data); i++ ){
    GFL_BG_FreeBGControl( sc_bg_cnt_data[i].frame );
  }

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG�V�X�e�����C������
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Main( MICTEST_BG_WORK *p_bg )
{

}

//----------------------------------------------------------------------------
/**
 *  @brief  BG���\�[�X�ǂݍ���
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *  @param  heap_id         ���\�[�X�ǂݍ��ݗp�q�[�v
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_LoadBg( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, 0, 0x20, heap_id );
  GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, 0, 0x20, heap_id );

  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 13*0x20, 16*2, heap_id );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 13*0x20, 16*2, heap_id );

  //  ----- ����� -----
  GFL_ARC_UTIL_TransVramBgCharacter(  ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCGR,
            GFL_BG_FRAME1_S, 0, 0, 0, heap_id );
  GFL_ARC_UTIL_TransVramScreen( ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NSCR,
            GFL_BG_FRAME1_S, 0, 0, 0, heap_id );  

  //  ----- ���� -----
  GFL_ARC_UTIL_TransVramBgCharacter(  ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCGR,
            GFL_BG_FRAME1_M, 0, 0, 0, heap_id );
  GFL_ARC_UTIL_TransVramScreen( ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NSCR,
            GFL_BG_FRAME1_M, 0, 0, 0, heap_id );    
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG���\�[�X���
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_UnLoadBg( MICTEST_BG_WORK *p_bg )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG�V�X�e��VBlank������
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_VBlancFunction( MICTEST_BG_WORK *p_bg )
{
  GFL_BG_VBlankFunc();
}


static const PRINTSYS_LSB string_col_tbl[]={
  PRINTSYS_MACRO_LSB(  1, 2, 15),    //�u�}�C�N�e�X�g�v�̐F
  PRINTSYS_MACRO_LSB(  1, 2, 15),    // �����������͂̐F
  PRINTSYS_MACRO_LSB( 15, 2,  0),    // �u���ǂ�v�̐F
};

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�쐬
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *  @param  heap_id         BMPWIN�p�q�[�vID
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_CreateMsg( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
  static const struct {
    u8 frame;
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u8 plt_num;
    u16 chr_ofs;
    u8 x_dot;
    u8 padding[3];
  } sc_bmp_setup_data[] = {
    //  TITLE
    {
      BMPWIN_TITLE_FRAME, BMPWIN_TITLE_X, BMPWIN_TITLE_Y, BMPWIN_TITLE_W, BMPWIN_TITLE_H,
      BMPWIN_TITLE_PLT, BMPWIN_TITLE_CHR_OFS, BMPWIN_TITLE_X_DOT,
    },
    //  INFO
    {
      BMPWIN_INFO_FRAME, BMPWIN_INFO_X, BMPWIN_INFO_Y, BMPWIN_INFO_W, BMPWIN_INFO_H,
      BMPWIN_INFO_PLT, BMPWIN_INFO_CHR_OFS, 0,
    },
    //  RETBTN
    {
      BMPWIN_RETBTN_FRAME, BMPWIN_RETBTN_X, BMPWIN_RETBTN_Y, BMPWIN_RETBTN_W, BMPWIN_RETBTN_H,
      BMPWIN_RETBTN_PLT, BMPWIN_RETBTN_CHR_OFS, 0,
    },
  };
  int i;
  STRBUF  *p_buf;

  p_bg->p_msg_mng = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_mictest_dat, heap_id );

  for( i = 0; i < MICTEST_BMPWIN_MAX; i++ ) {
    GFL_BMP_DATA* bmp;

    p_bg->bmpwin[i] = GFL_BMPWIN_CreateFixPos( sc_bmp_setup_data[i].frame,
        sc_bmp_setup_data[i].x, sc_bmp_setup_data[i].y, sc_bmp_setup_data[i].w, 
        sc_bmp_setup_data[i].h, sc_bmp_setup_data[i].plt_num, sc_bmp_setup_data[i].chr_ofs );

    bmp = GFL_BMPWIN_GetBmp( p_bg->bmpwin[i] );

    GFL_BMP_Clear( bmp, 0 );
    GFL_BMPWIN_ClearScreen( p_bg->bmpwin[i] );

    p_buf = GFL_MSG_CreateString( p_bg->p_msg_mng, i );
    PRINTSYS_PrintColor( bmp, sc_bmp_setup_data[i].x_dot, 0, p_buf, p_bg->fontHandle, string_col_tbl[i] );
    GFL_STR_DeleteBuffer( p_buf );
    GFL_BMPWIN_MakeTransWindow( p_bg->bmpwin[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�j��
 *
 *  @param  MICTEST_BG_WORK *p_bg BG�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_DeleteMsg( MICTEST_BG_WORK *p_bg )
{
  int i;
  for( i = 0; i < MICTEST_BMPWIN_MAX; i++ )  {
    GFL_BMP_DATA* bmp;

    bmp = GFL_BMPWIN_GetBmp( p_bg->bmpwin[i] );

//  GF_BGL_BmpWinOff( p_bg->bmpwin[i] );
    GFL_BMP_Clear( bmp, 0 );
    GFL_BMPWIN_ClearScreen( p_bg->bmpwin[i] ) ;
    GFL_BMPWIN_Delete( p_bg->bmpwin[i] ); 
  }

  GFL_MSG_Delete( p_bg->p_msg_mng );

  // �^�b�`�t�H���g���
  GFL_FONT_Delete( p_bg->fontHandle );
}

//=============================================================================
/**
 *  �}�C�N�Ǘ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N�����A������
 *
 *  @param  MICTEST_MIC_WORK *p_mic �}�C�N�Ǘ����[�N
 *  @param  heap_id         �o�b�t�@�p�q�[�v
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Init( MICTEST_MIC_WORK *p_mic, u32 heap_id, MICCallback MicCallback, void*p_mic_arg )
{
  SND_MIC_Init( heap_id );

  //�}�C�N���X���[�v���֎~��
  GFL_UI_SleepDisable( GFL_UI_SLEEP_MIC );
  
  //  �o�b�t�@�쐬
  { 
    u32 size    = sizeof(MIC_BUF_SIZE)*MIC_SAMPLING_LENGTH + 32;  //  ��Ƀo�b�t�@�A�h���X��32�o�C�g�A���C�����邽��+32
    MIC_BUF_SIZE  *p_buf  = GFL_HEAP_AllocMemory( heap_id, size );
    MI_CpuClear8( p_buf, size );
    p_mic->p_buf_adrs   = p_buf;

    //  �}�C�N�p����������
    p_mic->param.type   = MIC_SAMPLING_TYPE;
    p_mic->param.buffer = (void*)MATH_ROUNDUP32( (int)p_buf );
    p_mic->param.size   = sizeof(MIC_BUF_SIZE)*MIC_SAMPLING_LENGTH;
    p_mic->param.rate   = MIC_SAMPLING_RATE_8180; //NNS_SND_STRM_TIMER_CLOCK / MIC_SAMPLING_RATE * 64;
    p_mic->param.loop_enable   = TRUE;
    p_mic->param.full_callback = MicCallback;
    p_mic->param.full_arg      = p_mic_arg;
  }

  p_mic->pre_mic_use  = SND_MIC_IsAmpOnWaitFlag();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N�����@���
 *
 *  @param  MICTEST_MIC_WORK *p_mic �}�C�N�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Exit( MICTEST_MIC_WORK *p_mic )
{
  GFL_HEAP_FreeMemory( p_mic->p_buf_adrs );
  SND_MIC_Exit();
  //�}�C�N���X���[�v������
  GFL_UI_SleepEnable( GFL_UI_SLEEP_MIC );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N����  ���C������
 *
 *  @param  MICTEST_MIC_WORK *p_mic   �}�C�N�Ǘ����[�N
 *
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Main( MICTEST_MIC_WORK *p_mic )
{ 
  BOOL now_mic_use;

  SND_MIC_Main();
  
  now_mic_use = SND_MIC_IsAmpOnWaitFlag();

  //�����}�C�N�g�p�s��Ԃ�������A
  //���݂̃o�b�t�@���N���A����
  if( !now_mic_use || !p_mic->pre_mic_use )
  { 
    if( !p_mic->mic_use_flag )
    { 
      if( p_mic->is_start )
      { 
        //start���Ă���STOP
        MicTest_MIC_StopSampling( p_mic );
        p_mic->pre_start_sampling = TRUE;
      }
      //����Ƀo�b�t�@���N���A
      { 
        //int size  = p_mic->param.size / sizeof(MIC_BUF_SIZE) - 1;
        //MIC_BUF_SIZE  *p_mic_buffer = p_mic->param.buffer;
        //MI_CpuClear8( p_mic_buffer, size );
        OS_Printf( "buff�g�p�s��ԁ@����\n" );
      }
      p_mic->mic_use_flag = TRUE;
    }
    //�g�p�J�n�ɂȂ���
    if( now_mic_use && !p_mic->pre_mic_use )
    { 
      p_mic->mic_use_flag = FALSE;
      if( p_mic->pre_start_sampling )
      { 
        //STOP���Ă���Start
        MicTest_MIC_StartSampling( p_mic );
        OS_Printf( "AutoStart\n" );
        p_mic->pre_start_sampling = FALSE;
      }
      OS_Printf( "buff�g�p�\��ԁ@����\n" );
    }
  }

  p_mic->pre_mic_use  = now_mic_use;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N����  �T���v�����O�X�^�[�g
 *
 *  @param  MICTEST_MIC_WORK *p_mic �}�C�N�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_StartSampling( MICTEST_MIC_WORK *p_mic )
{
  if( p_mic->mic_use_flag == FALSE )
  { 
//    SND_MIC_StartAutoSampling( &p_mic->param );
      MICResult   result;
#ifdef TWL_FREQENCY
      if(OS_IsRunOnTwl()){
        OS_Printf("SNDEX Frequency=%d\n", GetI2SFrequency());
      }
#endif
      result=MIC_StartLimitedSampling( &p_mic->param );
      OS_Printf("mic start result=%d\n", result);
      p_mic->is_start = TRUE;
  }
  else
  { 
    p_mic->pre_start_sampling = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N����  �T���v�����O��~
 *
 *  @param  MICTEST_MIC_WORK *p_mic �}�C�N�Ǘ����[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_StopSampling( MICTEST_MIC_WORK *p_mic )
{
//  SND_MIC_StopAutoSampling();
  MIC_StopLimitedSampling();
  p_mic->is_start = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N�o�b�t�@�����āA���ω��ʂ��o������
 *
 *  @param  MICAutoParam * p_mic_param  �}�C�N�̏��
 *
 *  @return ���ω��ʒl
 */
//-----------------------------------------------------------------------------
static int CalcAverageVolumeMicBuffer( MICAutoParam * p_mic_param )
{
  int i;
  int volume  = 0;
  int size  = p_mic_param->size / sizeof(MIC_BUF_SIZE) - 1;
  MIC_BUF_SIZE  *p_mic_buffer = p_mic_param->buffer;
  for( i = 0; i < size; i++ ) {
    if( p_mic_buffer[i] < 128 ) {
      volume  += (256 - p_mic_buffer[i]);   //  ���P
    }else{
      volume  += p_mic_buffer[i];
    }
  }

  return volume / size;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N�o�b�t�@����t�ɂȂ����Ƃ��ɌĂ΂��R�[���o�b�N
 *      �����ł͉��L���s���Ă���
 *      �P�D���ʃQ�[�W�̑�������
 *      �Q�D�|�P�����̃A�j���J�n�A��~
 *      �R�D���ʂ����̂Ƃ������^�X�NON
 *
 *  @param  MICResult result  �}�C�N�^���I���p���
 *  @param  *p_arg        �n���ꂽ�A�h���X(MICTEST_MAIN_WORK��n���Ă���)
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void ChangeObjAnmMicCallback( MICResult result, void *p_arg )
{   
  if( result == MIC_RESULT_SUCCESS ) {
    MICTEST_MAIN_WORK *p_wk = p_arg;
    MICTEST_MIC_WORK  *p_mic  = &p_wk->mic;
    MICTEST_OBJ_WORK  *p_obj  = &p_wk->obj;
    int now_volume; 
    int anm_seq;
    static u32 cnt = 0;

    now_volume  = CalcAverageVolumeMicBuffer( &p_mic->param );
  
    //  �Q�[�W��������
    //  �����l��茻�݂̓��͂������Ȃ猸���l�Ɍ��݂̒l���Z�b�g
    if( p_mic->prev_volume < now_volume)  {
      anm_seq = GetMicLevelToAnmSeqNum( now_volume );
      OS_Printf("VOLUME=%d GAGE=%d\n", now_volume, anm_seq );
      p_mic->prev_volume  = now_volume;
      p_mic->anm_set_flg  = TRUE;
    } else {
      anm_seq = GetMicLevelToAnmSeqNum( p_mic->prev_volume );
      p_mic->prev_volume  -= GAGE_DECREASE;
    }

    //  �A�j���V�[�P���X�X�V
    if( anm_seq != 0 ) {
      //  �|�P�����A�j���J�n
      if( p_mic->anm_set_flg ) {
        GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_POKEMON], 1 );
        p_mic->anm_set_flg  = FALSE;
      }

      if( IsEmptyNoteTask( p_obj ) )
      {
        MoveStartNoteCtrl( p_obj, p_wk->heap_id, OBJ_NOTE_POS_X, OBJ_NOTE_POS_Y, p_wk->tcbSys );
      }else{
        //  �n�[�g����^�X�N�J�n
        if( --p_obj->note_cnt == 0 ) {
          if( MoveStartNoteCtrl( p_obj, p_wk->heap_id, OBJ_NOTE_POS_X, OBJ_NOTE_POS_Y, p_wk->tcbSys ) ) {
            OS_Printf( "NOTE_START:%d\n",cnt );
          }
          p_obj->note_cnt = NOTE_ENTER_CNT_MAX;
        }
      }

      GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_L], anm_seq );
      GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_R], anm_seq );
    }else if( anm_seq == 0 ) {
      //  �A�j����~
      GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_POKEMON], 0 );
      GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_L], 0 );
      GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_R], 0 );
    }

    cnt++;
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief  �}�C�N���̓��x������A�Q�[�W�A�j���V�[�P���X���擾����
 *
 *  @param  u8 volume ����
 *
 *  @return �A�j���V�[�P���X
 */
//-----------------------------------------------------------------------------
static u8 GetMicLevelToAnmSeqNum( u8 volume )
{
  u8 anm_seq;
  if( IS_MICINPUT_LEVEL0( volume ) )  {
    anm_seq = 0;
  }else if( IS_MICINPUT_LEVEL1( volume ) ) {
    anm_seq = 1;
  }else if( IS_MICINPUT_LEVEL2( volume ) ) {
    anm_seq = 2;
  }else if( IS_MICINPUT_LEVEL3( volume ) ) {
    anm_seq = 3;
  }else if( IS_MICINPUT_LEVEL4( volume ) ){
    anm_seq = 4;
  }else {
    GF_ASSERT( 0 );
  }

  return anm_seq;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �����^�X�N���󂩂ǂ���
 *
 *  @param  const MICTEST_OBJ_WORK *cp_obj OBJ�Ǘ����[�N
 *
 *  @return TRUE�Ȃ��
 */
//-----------------------------------------------------------------------------
static BOOL IsEmptyNoteTask( const MICTEST_OBJ_WORK *cp_obj )
{
  int i;
  for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
    if( cp_obj->note_task_ctrl[i].tcb_ptr != NULL ) {
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �g���Ă��Ȃ��A�N�^�[����������ɂ���瓮���^�X�N�ɓn������
 *
 *  @param  MICTEST_OBJ_WORK *p_wk  OBJ�������[�N
 *  @param  heap_id         �^�X�N���[�N�p�q�[�v
 *  @param  x           �������W
 *  @param  y           �������W
 *  @param  is_start        �J�n
 *
 *  @return TRUE �Ȃ珈���J�n�AFALSE�Ȃ���s���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MoveStartNoteCtrl( MICTEST_OBJ_WORK *p_wk, u32 heap_id, s16 x, s16 y, GFL_TCBSYS* tcbSys )
{
  int i;
  GFL_CLWK* p_note      = NULL;
  NOTE_TASK_CTRL  *p_task_ctrl  = NULL;

  //  �󂢂Ă���ӏ�������
  for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
    if( p_wk->note_task_ctrl[i].tcb_ptr == NULL ) {
      p_task_ctrl = &p_wk->note_task_ctrl[i];
      p_note    = p_wk->p_act[ MICTEST_OBJ_NOTE+i ];
      break;
    }
  }
  
  //  ������������s
  if( p_task_ctrl != NULL ) {
    fx32 speed  = NOTE_SPEED_MIN + MICTEST_RAND_MAX( NOTE_SPEED_RANGE );
    u32 angle = NOTE_ANGLE_MIN + MICTEST_RAND_MAX( NOTE_ANGLE_RANGE );
    u32 width = NOTE_WIDTH_MIN + MICTEST_RAND_MAX( NOTE_WIDTH_RANGE );
  //  u32 wait  = NOTE_WAIT_MIN  + MICTEST_RAND_MAX( NOTE_WAIT_RANGE  );
  //  wait  *=  NOTE_WAIT_SUM;  // �ł������o���������邽�߁A�����_���̒l�������č��������Ă���
    MoveStartNoteTask( tcbSys, p_note, /*heap_id*/p_task_ctrl, x, y, speed, angle, width, 1 );
    return TRUE;
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ��������ɂ���瓮���^�X�N�A�J�n
 *
 *  @param  GFL_CLWK* p_act �����̃A�N�^�[
 *  @param  NOTE_MOVE_WORK *p_task_wk �^�X�N���[�N
 *  //      @param  heap_id     �^�X�N���[�N�p�q�[�v
 *  @param  x       �������W
 *  @param  y       �������W
 *  @param  speed     �㏸�X�s�[�h
 *  @param  angle     �㏸�A���O��
 *  @param  width     ��ꕝ
 *  @param  wait      �J�n�ҋ@�V���N
 *  @param  p_is_end    �I���t���O�iNULL���Ɩ����j
 *
 *  @return tcb�̃|�C���^
 */
//-----------------------------------------------------------------------------
static void MoveStartNoteTask( GFL_TCBSYS* tcbSys, GFL_CLWK*  p_act, NOTE_TASK_CTRL *p_task_wk, /*u32 heap_id,*/ s16 x, s16 y, fx32 speed, u32 angle, u32 width, u32 wait )
{
  NOTE_MOVE_WORK  *p_wk;

  /* ���荞�ݒ��ɃA���P�[�g����ƃA�T�[�g�̂��߃A���P�[�g�ł��Ȃ��̂ŁA���̂�������Ă��� */
//  p_tcb = PMDS_taskAdd( MoveNoteTask, sizeof(NOTE_MOVE_WORK), 0, heap_id ); //  (�o�X���L���邩��@�}�C�N�̃o�b�t�@�I���R�[���o�b�N�͊��荞�݂炵��)
  p_task_wk->tcb_ptr  = GFL_TCB_AddTask( tcbSys, MoveNoteTask, p_task_wk, 0 );

  p_wk  = GFL_TCB_GetWork( p_task_wk->tcb_ptr );
  p_wk->p_act = p_act;
  p_wk->wait  = wait;
  p_wk->pos.x = FX32_CONST(x);
  p_wk->pos.y = FX32_CONST(y);
  p_wk->pos.z = 0;
  p_wk->speed.x =  FX_Mul( speed, GFL_CALC_Sin360(90+angle));
  p_wk->speed.y = -FX_Mul( speed, GFL_CALC_Sin360(angle) );
  p_wk->speed.z = 0;
  p_wk->shake_pt    = 0;
  p_wk->shake_speed = FX32_CONST(6.0f);
  p_wk->shake_width = width;

  GFL_CLACT_WK_SetWldTypePos( p_wk->p_act, x, CLSYS_MAT_X );
  GFL_CLACT_WK_SetWldTypePos( p_wk->p_act, y, CLSYS_MAT_Y );

}

//----------------------------------------------------------------------------
/**
 *  @brief  ��������ɂ���瓮���^�X�N
 *
 *  @param  GFL_TCB* p_tcb  �^�X�N�|�C���^
 *  @param  *p_wk_adrs    �n���ꂽ�A�h���X
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
static void MoveNoteTask( GFL_TCB* p_tcb, void *p_wk_adrs )
{
  NOTE_TASK_CTRL  *p_ctrl = p_wk_adrs;
  NOTE_MOVE_WORK  *p_wk = &p_ctrl->note_wk;

  if( p_wk->wait ) {
    p_wk->wait--;
    if( p_wk->wait == 0 ) {
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_act, TRUE );
    }
  }else{  
    int shake;
    s16 x, y;

    VEC_Add( &p_wk->pos, &p_wk->speed, &p_wk->pos );
    x = p_wk->pos.x >> FX32_SHIFT;
    y = p_wk->pos.y >> FX32_SHIFT;
    p_wk->shake_pt  += p_wk->shake_speed;
    while( p_wk->shake_pt > FX32_CONST(360) ) {
      p_wk->shake_pt -= FX32_CONST(360);
    }
    shake = p_wk->shake_pt >> FX32_SHIFT;
    x   += (p_wk->shake_width * GFL_CALC_Sin360( shake )) >> FX32_SHIFT;

    GFL_CLACT_WK_SetWldTypePos( p_wk->p_act, x, CLSYS_MAT_X );
    GFL_CLACT_WK_SetWldTypePos( p_wk->p_act, y, CLSYS_MAT_Y );

    if( y < - 16 ) {
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_act, FALSE );
      GFL_TCB_DeleteTask( p_tcb );
      p_ctrl->tcb_ptr = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����^�X�N�A�S�č폜
 *
 *  @param  MICTEST_OBJ_WORK *p_obj OBJ�Ǘ����[�N
 *
 *  @return note
 */ 
//-----------------------------------------------------------------------------
static void CleanNoteTask( MICTEST_OBJ_WORK *p_obj )
{
  int i;
  for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
    if( p_obj->note_task_ctrl[i].tcb_ptr != NULL ) {
      GFL_TCB_DeleteTask( p_obj->note_task_ctrl[i].tcb_ptr );
      p_obj->note_task_ctrl[i].tcb_ptr  = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�q�b�g�󋵊m�F
 *
 *  @param  none
 *
 *  @return �q�b�g�����e�[�u���̔ԍ��@�q�b�g���Ȃ��� GFL_UI_TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
static int GetHitTrg()
{
  static const GFL_UI_TP_HITTBL sc_hit_tbl[]  = {
    { OBJ_BUTTON_POS_Y - 16, OBJ_BUTTON_POS_Y + 16,
      OBJ_BUTTON_POS_X - 40, OBJ_BUTTON_POS_X + 40},
    { GFL_UI_TP_HIT_END,0,0,0 },    // �I���f�[�^ 
  };

  return GFL_UI_TP_HitTrg( sc_hit_tbl );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���Z�X�I���L�[
 *
 *  @param  none
 *
 *  @return TRUE�Ńv���Z�X�I���@FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
static BOOL IsProcEndTrg()
{
  return  GetHitTrg() == 0 || GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL;
}


#ifdef TWL_FREQENCY
#define     RETRY_MAX_COUNT     8

/*---------------------------------------------------------------------------*
  Name:         GetI2SFrequency

  Description:  I2S ������g�����擾����B

  Arguments:    None.

  Returns:      SDNEXFrequency  -   I2S ������g����Ԃ��B
 *---------------------------------------------------------------------------*/
static SNDEXFrequency GetI2SFrequency(void)
{
    SNDEXResult     result;
    SNDEXFrequency  freq;
    s32             retry   =   0;

    while (TRUE)
    {
        result  =   SNDEX_GetI2SFrequency(&freq);
        switch (result)
        {
        case SNDEX_RESULT_SUCCESS:          // ����
            return freq;
        case SNDEX_RESULT_EXCLUSIVE:        // �r���G���[
        case SNDEX_RESULT_PXI_SEND_ERROR:   // PXI �L���[����t
            if (++ retry <= RETRY_MAX_COUNT)
            {
                OS_Sleep(1);
                continue;
            }
            OS_TWarning("Retry count overflow.\n");
            break;
        case SNDEX_RESULT_BEFORE_INIT:      // �������O
        case SNDEX_RESULT_ILLEGAL_STATE:    // ��Ԉُ�
            OS_TWarning("Illegal state to get I2S frequency.\n");
            break;
        case SNDEX_RESULT_FATAL_ERROR:      // �v���I�ȃG���[
        default:
            OS_Panic("Fatal error (%d).\n", result);
            /* never return */
        }
    }
    return SNDEX_FREQUENCY_32730;
}
#endif