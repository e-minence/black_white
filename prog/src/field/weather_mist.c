//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_mist.c
 *	@brief  �V�C�F��
 *	@author	tomoya takahashi
 *	@date		2010.02.11
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc/arc_def.h"
#include "arc/field_weather.naix"
#include "arc/field_weather_light.naix"


#include "weather_mist.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

/*== �� ==*/
#define	WEATHER_MIST_FOG_TIMING		(90)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_MIST_FOG_TIMING_END	(60)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_MIST_FOG_OFS			(-8)

#if 0 // �ʏ�FOG�ł��A�����ȃJ�����A���O���ɑΉ�����K�v���o����B
static const u8 sc_MIST_FOG_SLOPE[] = {
  WEATHER_FOG_SLOPE_DEFAULT,    // �ʏ�
  WEATHER_FOG_SLOPE_DEFAULT,    // H01
  WEATHER_FOG_SLOPE_DEFAULT,    // C03
  WEATHER_FOG_SLOPE_DEFAULT,    // �|�P�Z��
  WEATHER_FOG_SLOPE_DEFAULT,    // H01�Q�[�g�O
  WEATHER_FOG_SLOPE_DEFAULT,    // �|�P�������[�O�S�i
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�H�n�i���H�n�j
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�����L��
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�H�n�i�W���j
  WEATHER_FOG_SLOPE_DEFAULT,    // H02
  WEATHER_FOG_SLOPE_DEFAULT,    // H03
  WEATHER_FOG_SLOPE_DEFAULT,    // �`�����s�I�����[�h
  WEATHER_FOG_SLOPE_DEFAULT,    // C05�n�ʃW��
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�H�n�i�^�񒆁j
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�H�n�i�|�P�Z���j
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�H�n�i�A�g���G�j
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�S�[�X�g�j
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�����j
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�i���j
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�G�X�p�[�j
  WEATHER_FOG_SLOPE_DEFAULT,    // �|�P���[�O�����i���t�g��j
  WEATHER_FOG_SLOPE_DEFAULT,    // �|�P���[�O�����i���t�g���j
  WEATHER_FOG_SLOPE_DEFAULT,    // WFBC�����P
  WEATHER_FOG_SLOPE_DEFAULT,    // WFBC����2
  WEATHER_FOG_SLOPE_DEFAULT,    // WFBC����3
  WEATHER_FOG_SLOPE_DEFAULT,    // C03�x�C�T�C�h
  WEATHER_FOG_SLOPE_DEFAULT,    // H05
  WEATHER_FOG_SLOPE_DEFAULT,    // C07�X�W��
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�S�[�X�g�j����
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�����j����
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�i���j����
  WEATHER_FOG_SLOPE_DEFAULT,    // �l�V�������i�G�X�p�[�j����
  WEATHER_FOG_SLOPE_DEFAULT,    // C08�h���S���W��
  WEATHER_FOG_SLOPE_DEFAULT,    // �^���[�I�u�w�u���PF�`�SF
};

static const u32 sc_MIST_FOG_OFFS[] = {
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �ʏ�
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // H01
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �|�P�Z��
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // H01�Q�[�g�O
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �|�P�������[�O�S�i
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�H�n�i���H�n�j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�����L��
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�H�n�i�W���j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // H02
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // H03
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �`�����s�I�����[�h
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C05�n�ʃW��
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�H�n�i�^�񒆁j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�H�n�i�|�P�Z���j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�H�n�i�A�g���G�j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�S�[�X�g�j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�����j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�i���j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�G�X�p�[�j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �|�P���[�O�����i���t�g��j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �|�P���[�O�����i���t�g���j
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // WFBC�����P
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // WFBC����2
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // WFBC����3
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C03�x�C�T�C�h
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // H05
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C07�X�W��
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�S�[�X�g�j����
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�����j����
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�i���j����
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �l�V�������i�G�X�p�[�j����
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // C08�h���S���W��
  WEATHER_FOG_DEPTH_DEFAULT_START + WEATHER_MIST_FOG_OFS,    // �^���[�I�u�w�u���PF�`�SF
};
#endif


/*== �� ==*/
#define	WEATHER_PALACE_MIST_FOG_TIMING		(90)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_PALACE_MIST_FOG_TIMING_END	(60)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_PALACE_MIST_FOG_OFS			(32607)
#define WEATHER_PALACE_MIST_FOG_SLOPE		(8)

static const u8 sc_PALACE_MIST_FOG_SLOPE[] = {
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �ʏ�
  WEATHER_PALACE_MIST_FOG_SLOPE,    // H01
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �|�P�Z��
  WEATHER_PALACE_MIST_FOG_SLOPE,    // H01�Q�[�g�O
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �|�P�������[�O�S�i
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�H�n�i���H�n�j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�����L��
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�H�n�i�W���j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // H02
  1,    // H03
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �`�����s�I�����[�h
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C05�n�ʃW��
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�H�n�i�^�񒆁j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�H�n�i�|�P�Z���j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�H�n�i�A�g���G�j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�S�[�X�g�j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�����j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�i���j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�G�X�p�[�j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �|�P���[�O�����i���t�g��j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �|�P���[�O�����i���t�g���j
  WEATHER_PALACE_MIST_FOG_SLOPE,    // WFBC�����P
  WEATHER_PALACE_MIST_FOG_SLOPE,    // WFBC����2
  4,    // WFBC����3
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C03�x�C�T�C�h
  8,    // H05
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C07�X�W��
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�S�[�X�g�j����
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�����j����
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�i���j����
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �l�V�������i�G�X�p�[�j����
  WEATHER_PALACE_MIST_FOG_SLOPE,    // C08�h���S���W��
  WEATHER_PALACE_MIST_FOG_SLOPE,    // �^���[�I�u�w�u���PF�`�SF
};

static const u32 sc_PALACE_MIST_FOG_OFFS[] = {
  WEATHER_PALACE_MIST_FOG_OFS,    // �ʏ�
  WEATHER_PALACE_MIST_FOG_OFS,    // H01
  WEATHER_PALACE_MIST_FOG_OFS,    // C03
  WEATHER_PALACE_MIST_FOG_OFS,    // �|�P�Z��
  WEATHER_PALACE_MIST_FOG_OFS,    // H01�Q�[�g�O
  WEATHER_PALACE_MIST_FOG_OFS,    // �|�P�������[�O�S�i
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�H�n�i���H�n�j
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�����L��
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�H�n�i�W���j
  WEATHER_PALACE_MIST_FOG_OFS,    // H02
  0,    // H03
  WEATHER_PALACE_MIST_FOG_OFS,    // �`�����s�I�����[�h
  WEATHER_PALACE_MIST_FOG_OFS,    // C05�n�ʃW��
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�H�n�i�^�񒆁j
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�H�n�i�|�P�Z���j
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�H�n�i�A�g���G�j
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�S�[�X�g�j
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�����j
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�i���j
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�G�X�p�[�j
  WEATHER_PALACE_MIST_FOG_OFS,    // �|�P���[�O�����i���t�g��j
  WEATHER_PALACE_MIST_FOG_OFS,    // �|�P���[�O�����i���t�g���j
  WEATHER_PALACE_MIST_FOG_OFS,    // WFBC�����P
  WEATHER_PALACE_MIST_FOG_OFS,    // WFBC����2
  2568,    // WFBC����3
  WEATHER_PALACE_MIST_FOG_OFS,    // C03�x�C�T�C�h
  32623,    // H05
  WEATHER_PALACE_MIST_FOG_OFS,    // C07�X�W��
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�S�[�X�g�j����
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�����j����
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�i���j����
  WEATHER_PALACE_MIST_FOG_OFS,    // �l�V�������i�G�X�p�[�j����
  WEATHER_PALACE_MIST_FOG_OFS,    // C08�h���S���W��
  WEATHER_PALACE_MIST_FOG_OFS,    // �^���[�I�u�w�u���PF�`�SF
};



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���჏�[�N
//=====================================
typedef struct {
	s32 work[10];

  u32 slope;
  u32 offs;
} WEATHER_MIST_WORK;

//-------------------------------------
///	���჏�[�N
//=====================================
typedef struct {
  s32 weather_no;
	s32 work[10];

  u32 slope;
  u32 offs;
} WEATHER_PALACE_MIST_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	��
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_MIST_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_MIST_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 



//-------------------------------------
///	�p���X��
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_WHITE_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_BLACK_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_WHITE_MIST_HIGH_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_BLACK_MIST_HIGH_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_PALACE_MIST_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_PALACE_MIST_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 


//-----------------------------------------------------------------------------
/**
 *			�V�C�f�[�^
 */
//-----------------------------------------------------------------------------
// ��
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_MIST = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_MIST_WORK),

	// �Ǘ��֐�
	WEATHER_MIST_Init,		// ������
	WEATHER_MIST_FadeIn,		// �t�F�[�h�C��
	WEATHER_MIST_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_MIST_Main,		// ���C������
	WEATHER_MIST_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_MIST_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_MIST_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_MIST_OBJ_Move,
};

// �p���X��
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_WHITE_MIST = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_PALACE_MIST_WORK),

	// �Ǘ��֐�
	WEATHER_PALACE_WHITE_MIST_Init,		// ������
	WEATHER_PALACE_MIST_FadeIn,		// �t�F�[�h�C��
	WEATHER_PALACE_MIST_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_PALACE_MIST_Main,		// ���C������
	WEATHER_PALACE_MIST_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_PALACE_MIST_OBJ_Move,
};

WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_BLACK_MIST = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_PALACE_MIST_WORK),

	// �Ǘ��֐�
	WEATHER_PALACE_BLACK_MIST_Init,		// ������
	WEATHER_PALACE_MIST_FadeIn,		// �t�F�[�h�C��
	WEATHER_PALACE_MIST_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_PALACE_MIST_Main,		// ���C������
	WEATHER_PALACE_MIST_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_PALACE_MIST_OBJ_Move,
};


// �p���X��
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_WHITE_MIST_HIGH = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_PALACE_MIST_WORK),

	// �Ǘ��֐�
	WEATHER_PALACE_WHITE_MIST_HIGH_Init,		// ������
	WEATHER_PALACE_MIST_FadeIn,		// �t�F�[�h�C��
	WEATHER_PALACE_MIST_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_PALACE_MIST_Main,		// ���C������
	WEATHER_PALACE_MIST_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_PALACE_MIST_OBJ_Move,
};

WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_BLACK_MIST_HIGH = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_PALACE_MIST_WORK),

	// �Ǘ��֐�
	WEATHER_PALACE_BLACK_MIST_HIGH_Init,		// ������
	WEATHER_PALACE_MIST_FadeIn,		// �t�F�[�h�C��
	WEATHER_PALACE_MIST_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_PALACE_MIST_Main,		// ���C������
	WEATHER_PALACE_MIST_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_PALACE_MIST_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_PALACE_MIST_OBJ_Move,
};




//----------------------------------------------------------------------------
/**
 *	@brief  ��  ������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	p_local_wk->work[0] = 0;	// �������t�H�O�p

  p_local_wk->slope = WEATHER_FOG_SLOPE_DEFAULT;
  p_local_wk->offs = WEATHER_FOG_DEPTH_DEFAULT + WEATHER_MIST_FOG_OFS;

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	BOOL fog_result;
	WEATHER_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
  if( p_local_wk->work[0] == 0 ){

    WEATHER_TASK_FogFadeIn_Init( p_wk,
        p_local_wk->slope, 
        p_local_wk->offs, 
        WEATHER_MIST_FOG_TIMING, fog_cont );

      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_MIST ) );

    p_local_wk->work[0]--;
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, p_local_wk->offs, fog_cont );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_MIST ) );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  ���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// fog
	p_local_wk->work[0] = 0;	// �������t�H�O�p

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_MIST_WORK* p_local_wk;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// �t�H�O����
  if( p_local_wk->work[0] == 0 ){
    p_local_wk->work[0] --;

    WEATHER_TASK_FogFadeOut_Init( p_wk,
        WEATHER_FOG_DEPTH_DEFAULT_START, 
        WEATHER_MIST_FOG_TIMING_END, fog_cont );
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result ){
			
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIST_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  ����
 */
//-----------------------------------------------------------------------------
static void WEATHER_MIST_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
}








//----------------------------------------------------------------------------
/**
 *	@brief  ��  ������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_WHITE_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;
  u32 camera_type = WEATHER_TASK_CAMERA_GetType( p_wk );

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->work[0] = 0;	// �������t�H�O�p

  p_local_wk->weather_no = WEATHER_NO_PALACE_WHITE_MIST;


  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_SLOPE) );
  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_OFFS) );

  // slope �� offs�����߂�
  p_local_wk->slope = sc_PALACE_MIST_FOG_SLOPE[ camera_type ];
  p_local_wk->offs  = sc_PALACE_MIST_FOG_OFFS[ camera_type ];

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_BLACK_MIST_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;
  u32 camera_type = WEATHER_TASK_CAMERA_GetType( p_wk );

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->work[0] = 0;	// �������t�H�O�p

  p_local_wk->weather_no = WEATHER_NO_PALACE_BLACK_MIST;

  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_SLOPE) );
  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_OFFS) );

  // slope �� offs�����߂�
  p_local_wk->slope = sc_PALACE_MIST_FOG_SLOPE[ camera_type ];
  p_local_wk->offs  = sc_PALACE_MIST_FOG_OFFS[ camera_type ];


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_WHITE_MIST_HIGH_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;
  u32 camera_type = WEATHER_TASK_CAMERA_GetType( p_wk );

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->work[0] = 0;	// �������t�H�O�p

  p_local_wk->weather_no = WEATHER_NO_PALACE_WHITE_MIST_HIGH;

  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_SLOPE) );
  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_OFFS) );

  // slope �� offs�����߂�
  p_local_wk->slope = sc_PALACE_MIST_FOG_SLOPE[ camera_type ];
  p_local_wk->offs  = sc_PALACE_MIST_FOG_OFFS[ camera_type ];


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_BLACK_MIST_HIGH_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;
  u32 camera_type = WEATHER_TASK_CAMERA_GetType( p_wk );

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->work[0] = 0;	// �������t�H�O�p

  p_local_wk->weather_no = WEATHER_NO_PALACE_BLACK_MIST_HIGH;

  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_SLOPE) );
  GF_ASSERT( camera_type < NELEMS(sc_PALACE_MIST_FOG_OFFS) );

  // slope �� offs�����߂�
  p_local_wk->slope = sc_PALACE_MIST_FOG_SLOPE[ camera_type ];
  p_local_wk->offs  = sc_PALACE_MIST_FOG_OFFS[ camera_type ];


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	BOOL fog_result;
	WEATHER_PALACE_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
  if( p_local_wk->work[0] == 0 ){

    WEATHER_TASK_FogFadeIn_Init( p_wk,
        p_local_wk->slope, 
        p_local_wk->offs, 
        WEATHER_PALACE_MIST_FOG_TIMING, fog_cont );

      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( p_local_wk->weather_no ) );

    p_local_wk->work[0]--;
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, p_local_wk->slope, p_local_wk->offs, fog_cont );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( p_local_wk->weather_no ) );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  ���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// fog
	p_local_wk->work[0] = 0;	// �������t�H�O�p

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_PALACE_MIST_WORK* p_local_wk;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// �t�H�O����
  if( p_local_wk->work[0] == 0 ){
    p_local_wk->work[0] --;

    WEATHER_TASK_FogFadeOut_Init( p_wk,
        WEATHER_FOG_DEPTH_DEFAULT_START, 
        WEATHER_PALACE_MIST_FOG_TIMING_END, fog_cont );
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result ){
			
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  �j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_PALACE_MIST_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��  ����
 */
//-----------------------------------------------------------------------------
static void WEATHER_PALACE_MIST_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
}


