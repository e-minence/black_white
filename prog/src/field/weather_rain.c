//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_rain.c
 *	@brief		�V�C�J
 *	@author		tomoya takahashi
 *	@date		2009.03.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc/arc_def.h"
#include "arc/field_weather.naix"
#include "arc/field_weather_light.naix"

#include "weather_rain.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	�J�Ǘ��֐��萔
//=====================================
#define	WEATHER_RAIN_TIMING_MIN		(0)							// �J���o���^�C�~���O�ŏ�
#define WEATHER_RAIN_TIMING_MAX		(8)						// �J���o���^�C�~���O�ő�
#define WEATHER_RAIN_TIMING_ADD		(4)							// �^�C�~���O�����炷��
#define WEATHER_RAIN_ADD_START		(1)							// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_RAIN_ADD_TIMING		(2)							// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_RAIN_ADD			(1)							// �o�^���鐔�𑝂₷��
#define	WEATHER_RAIN_ADD_MAIN		(2)							// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_RAIN_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_RAIN_NOFADE_OBJ_START_DUST_NUM	( 10 )			// ���P�ʂŕω������邩
#define	WEATHER_RAIN_NOFADE_OBJ_START_DUST_MOVE	( 1 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_RAIN_FOG_TIMING		(150)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_RAIN_FOG_TIMING_END	(50)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START_END	(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_OFS		(0x300)

/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_RAIN_SPEED_X		(-8)						// ���ɐi�ރX�s�[�h
#define WEATHER_RAIN_SPEED_Y		(16)						// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_RAIN_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_RAIN_END_MAX		(2)							// �I���J�E���^�ő�
#define	WEATHER_RAIN_START_X_BASE	(0)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_RAIN_MUL_X			(15)						// �x�[�X�ɉJ�̎�ޕ������l
#define	WEATHER_RAIN_START_X_MAX	(270)						// X�J�n���W�����̍ő�l
#define	WEATHER_RAIN_START_Y		(-96)						// Y�J�n���W
#define	WEATHER_RAIN_SPEED_ERR		(20)						// �X�s�[�h�␳�l





//-------------------------------------
//	��J
//=====================================
#define	WEATHER_STRAIN_TIMING_MIN		(0)					// �J���o���^�C�~���O�ŏ�
#define WEATHER_STRAIN_TIMING_MAX		(15)				// �J���o���^�C�~���O�ő�
#define WEATHER_STRAIN_TIMING_ADD		(3)					// �^�C�~���O�����炷��
#define WEATHER_STRAIN_ADD_START		(1)					// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_STRAIN_ADD_TIMING		(2)					// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_STRAIN_ADD			(1)						// �o�^���鐔�𑝂₷��
#define WEATHER_STRAIN_ADD_END		(-1)					// �o�^���鐔�𑝂₷��
#define	WEATHER_STRAIN_ADD_MAIN		(2)					// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_STRAIN_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_STRAIN_NOFADE_OBJ_START_DUST_NUM	( 5 )		// �������쐔��ύX���邩
#define	WEATHER_STRAIN_NOFADE_OBJ_START_DUST_MOVE	( 1 )		// ���炷���쐔�̒l

/*== �t�H�O ==*/
#define	WEATHER_STRAIN_FOG_TIMING		(150)						// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_STRAIN_FOG_TIMING_END	(50)						// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_START		(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_START_END	(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_OFS			(0x200)


/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_STRAIN_SPEED_X		(-20)						// ���ɐi�ރX�s�[�h
#define WEATHER_STRAIN_SPEED_Y		(20)							// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_STRAIN_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_STRAIN_END_MAX		(2)							// �I���J�E���^�ő�
#define	WEATHER_STRAIN_START_X		(0)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_STRAIN_START_X_MAX	(512)						// X�J�n���W�����l
#define	WEATHER_STRAIN_START_Y		(-80)						// Y�J�n���W
#define	WEATHER_STRAIN_START_Y_MAX	(40)						// Y�J�n���W�����l

#define WEATHER_STRAIN_OBJ_MUL_NUM	(5)							// �I�u�W�F�̃X�s�[�h��ω�������l
#define WEATHER_STRAIN_OBJ_MUL_CHG	(60)						// �ύX�^�C�~���O



//-------------------------------------
///	���V�[�P���X
//=====================================
enum{
	WEATHER_SPARK_START_WAIT,
	WEATHER_SPARK_SETUP,
	WEATHER_SPARK_SPARK,		// ��
	WEATHER_SPARK_SPARK_WAIT,	// ��
	WEATHER_SPARK_WAIT,
};

// ���e�[�u����
#define WEATHER_SPARK_TBL_MAX	( 4 )	// 4��܂ŌJ��Ԃ����Ƃ��o����

// �������E�G�C�g
#define WEATHER_SPARK_INIT_RAND	(3)

// ���̐F�\����
#define WEATHER_SPARK_COLOR_BLUE	( 8 )	//  ������R�����g�A�E�g����Ɛ݂𒲐��ł���悤��
#ifndef WEATHER_SPARK_COLOR_BLUE
static int blue = 8;
#endif

// ���̃^�C�v				����
// 1.�y�����̂�				����
// 2.�d�����̂�				����
// 3.�y����->�d����			�������߂�
enum{
	WEATHER_SPARK_TYPE_LIGHT,
	WEATHER_SPARK_TYPE_HARD,
	WEATHER_SPARK_TYPE_LIGHT_HARD,

	WEATHER_SPARK_TYPE_MAX,
};

// �m��
#define WEATHER_SPARK_TYPE_RAND_MAX	( 100 )
static const u8 sc_WEATHER_SPARK_TYPE_RAND[WEATHER_SPARK_TYPE_MAX] = {
	30,
	75,
	100,
};

// ������������̃E�G�C�g�ŏ�
static const u16 sc_WEATHER_SPARK_TYPE_RAND_WAIT_MIN[WEATHER_SPARK_TYPE_MAX] = {
	120,
	240,
	240,
};

// ������������̃E�G�C�g�����ŕϓ�����l
static const u16 sc_WEATHER_SPARK_TYPE_RAND_WAIT_MAX[WEATHER_SPARK_TYPE_MAX] = {
	240,
	300,
	300,
};

// ���̋���	�ŏ�
#define WEATHER_SPARK_STRONG_MAX	(31)
static const u8 sc_WEATHER_SPARK_STRONG_RAND_MIN[WEATHER_SPARK_TYPE_MAX] = {
	18,
	24,
	30,
};

// ���̋���	�����ŕϓ�����l
static const u8 sc_WEATHER_SPARK_STRONG_RAND_MAX[WEATHER_SPARK_TYPE_MAX] = {
	3,
	3,
	1,
};

// ���ɂ���ẴT�E���h�N���E�G�C�g
static const s8 sc_WEATHER_SPARK_SND_WAIT[WEATHER_SPARK_TYPE_MAX] = 
{
  28,
  13,
  10,
};

// ���t���b�V���X�s�[�h
#define WEATHER_SPARK_FLASHIN_SYNC	( 4 )
static const u8 sc_WEATHER_SPARK_FLASHOUT_SYNC[32] = {
  15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,
	15,15,15,20,20,30,30,35,35,35,
	40,40,
	
};

// �y�����̌J��Ԃ���
#define WEATHER_SPARK_LIGHT_NUM_MIN	(1)
#define WEATHER_SPARK_LIGHT_NUM_MAX	(2) //

// �y��->�d���@�E�G�C�g�����l
#define WEATHER_SPARK_LIGHT_HARD_WAIT_RAND		(4)
#define WEATHER_SPARK_LIGHT_HARD_OUTSYNC		(8)



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���ʂ̉J
//=====================================
typedef struct {
	s32 work[10];
} WEATHER_RAIN_WORK;


//-------------------------------------
///	���P���
//=====================================
typedef struct {
	u16 insync;
	u16 outsync;
	u16 spark_power;
	s16 wait;

  s32 snd_wait;
} SPARK_DATA;


//-------------------------------------
///	��
//=====================================
typedef struct {
	u16 seq;
	u16 spark_type;
	u16	spark_tbl_num;
	u16 spark_tbl_count;
	
	SPARK_DATA	spark_data[ WEATHER_SPARK_TBL_MAX ];

	s32	wait;
  s32 snd_wait;
} WEATHER_SPARK_WORK;

//-------------------------------------
///	���J
//=====================================
typedef struct {
	s32 work[10];

	WEATHER_SPARK_WORK	spark;
} WEATHER_SPARKRAIN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�ʏ�̉J
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_RAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 


//-------------------------------------
///	���J
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_SPARKRAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SPARKRAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 



//-------------------------------------
///	��
//=====================================
static void WEATHER_PARK_Init( WEATHER_SPARK_WORK* p_wk );
static void WEATHER_PARK_Exit( WEATHER_SPARK_WORK* p_wk );
static void WEATHER_PARK_Main( WEATHER_SPARK_WORK* p_wk, WEATHER_TASK* p_sys );

static void WEATEHR_SPARK_SetUp_Light( WEATHER_SPARK_WORK* p_wk );
static void WEATEHR_SPARK_SetUp_Hard( WEATHER_SPARK_WORK* p_wk );
static void WEATEHR_SPARK_SetUp_LightAndHard( WEATHER_SPARK_WORK* p_wk );

static GXRgb WEATHER_SPARK_GetColor( const WEATHER_SPARK_WORK* cp_wk );



//-----------------------------------------------------------------------------
/**
 *			�V�C�f�[�^
 */
//-----------------------------------------------------------------------------
// �ʏ�̉J
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_RAIN = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	NARC_field_weather_rain_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_NCER,			// OAM CELL
	NARC_field_weather_rain_NANR,			// OAM CELLANM
	0,		// BGTEX
	0,		// GXTexSizeS
	0,		// GXTexSizeT
	0,		// GXTexRepeat
	0,		// GXTexFlip
	0,		// GXTexFmt
	0,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_RAIN_WORK),

	// �Ǘ��֐�
	WEATHER_RAIN_Init,		// ������
	WEATHER_RAIN_FadeIn,		// �t�F�[�h�C��
	WEATHER_RAIN_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_RAIN_Main,		// ���C������
	WEATHER_RAIN_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_RAIN_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_RAIN_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_RAIN_OBJ_Move,
};


// ���J
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_SPARKRAIN = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	NARC_field_weather_rain_st_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_st_NCER,			// OAM CELL
	NARC_field_weather_rain_st_NANR,			// OAM CELLANM
	0,		// BGTEX
	0,		// GXTexSizeS
	0,		// GXTexSizeT
	0,		// GXTexRepeat
	0,		// GXTexFlip
	0,		// GXTexFmt
	0,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_SPARKRAIN_WORK),

	// �Ǘ��֐�
	WEATHER_SPARKRAIN_Init,		// ������
	WEATHER_SPARKRAIN_FadeIn,		// �t�F�[�h�C��
	WEATHER_SPARKRAIN_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_SPARKRAIN_Main,		// ���C������
	WEATHER_SPARKRAIN_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_SPARKRAIN_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SPARKRAIN_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_SPARKRAIN_OBJ_Move,
};





//----------------------------------------------------------------------------
/**
 *	@brief	������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_RAIN_ADD_START,
		WEATHER_RAIN_TIMING_MAX,
		WEATHER_RAIN_ADD_MAIN,
		WEATHER_RAIN_TIMING_MIN,
		-WEATHER_RAIN_TIMING_ADD,
		WEATHER_RAIN_ADD_TIMING,
		WEATHER_RAIN_ADD,
		WEATHER_RAIN_OBJ_Add );

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );



	p_local_wk->work[0] = WEATHER_RAIN_FOG_START;	// �������t�H�O�p

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

	// ���C�g�ύX
	WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_rain_dat, heapID );
	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;


	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){
			WEATHER_TASK_FogFadeIn_Init( p_wk,
			WEATHER_FOG_SLOPE_DEFAULT, 
			WEATHER_FOG_DEPTH_DEFAULT + WEATHER_RAIN_FOG_OFS, 
			WEATHER_RAIN_FOG_TIMING,
			fog_cont );

      // ��
      WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_RAIN );	
		}
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
			// �V�[�P���X�ύX
			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_RAIN_ADD_MAIN,
		WEATHER_RAIN_TIMING_MIN,
		WEATHER_RAIN_ADD_MAIN,
		WEATHER_RAIN_TIMING_MIN,
		-WEATHER_RAIN_TIMING_ADD,
		WEATHER_RAIN_ADD_TIMING,
		WEATHER_RAIN_ADD,
		WEATHER_RAIN_OBJ_Add );


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_RAIN_FOG_OFS, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_RAIN_OBJ_Add, WEATHER_RAIN_NOFADE_OBJ_START_NUM, WEATHER_RAIN_NOFADE_OBJ_START_DUST_NUM, WEATHER_RAIN_NOFADE_OBJ_START_DUST_MOVE, heapID );

  // ��
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_RAIN );	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_RAIN_TIMING_MAX,
			WEATHER_RAIN_TIMING_ADD,
			-WEATHER_RAIN_ADD );
	
	// �t�H�O
	p_local_wk->work[0] = WEATHER_RAIN_FOG_START_END;	// �������t�H�O�p


	// ��
	WEATHER_TASK_StopLoopSnd( p_wk );	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	// �t�H�O����
	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeOut_Init( p_wk,
					WEATHER_FOG_DEPTH_DEFAULT_START, 
					WEATHER_RAIN_FOG_TIMING_END, fog_cont );
		}
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result && result ){
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�I�u�W�F����
 */
//-----------------------------------------------------------------------------
static void WEATHER_RAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	int i;
	GFL_CLACTPOS mat;
	GFL_CLWK* p_clwk;
	s32*	obj_w;

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	WEATHER_OBJ_WORK_GetPos( p_wk, &mat );
	
	// ����t���O���`�F�b�N
	switch(obj_w[3]){
	case 0:		// ����
		// ������
		mat.x += (obj_w[4]*2);
		mat.y += (obj_w[2]*2);
	
		// �j�����邩�`�F�b�N
		obj_w[0] += 2;
		if(obj_w[0] > obj_w[1]){
			// �A�j���[�V���������邩�`�F�b�N
			if(GFUser_GetPublicRand(10) < 7){		// 7/10�͔j��
				// �j��
				obj_w[3] = 2;
			}else{
			
				// �j���A�j��
				obj_w[3] = 1;		
				obj_w[0] = 4;			// �j���A�j���J�E���^
				GFL_CLACT_WK_SetAnmIndex( p_clwk, 3 );
			}
		}
		// ���W�ݒ�
		WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &mat );

		break;
	case 1:		// �j���A�j��
		if(obj_w[0]-- <= 0){
			obj_w[3] = 2;		// �j��
		}
		break;

	case 2:		// �j��
		WEATHER_TASK_DeleteObj( p_wk );
		break;
	}		
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�I�u�W�F�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_RAIN_WORK*	p_local_wk;	// �V�X�e�����[�N
	int	err;	// �␳�l
	s32* obj_w;	// �I�u�W�F�N�g���[�N
	int frame;	// �`��t���[��
	GFL_CLACTPOS mat;	// �ݒ���W
	GFL_CLWK* p_clwk;
	u32 rand;

	// ���[�U���[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		add_obj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(add_obj == NULL){			// ���s������I��
			break;
		}
		obj_w = WEATHER_OBJ_WORK_GetWork( add_obj );		// �I�u�W�F���[�N�쐬
		p_clwk	= WEATHER_OBJ_WORK_GetClWk( add_obj );

		// �����擾
		rand = GFUser_GetPublicRand(0);
		
		// �̈��������
		obj_w[0] = 0;			// �J�E���^
		frame = rand%3;	// �J�̎��
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );

		err = (rand % WEATHER_RAIN_SPEED_ERR);
		obj_w[2] = WEATHER_RAIN_SPEED_Y * (frame+1) + err;
		if(frame == 2){		// ��ԑO�̉J�̓X�s�[�h���P�i�K�グ��
			obj_w[2] += WEATHER_RAIN_SPEED_Y;
		}
		
		err /= WEATHER_RAIN_SPEED_X;
		obj_w[4] = WEATHER_RAIN_SPEED_X * (frame+1) + err;
		if(frame == 2){		// ��ԑO�̉J�̓X�s�[�h���P�i�K�グ��
			obj_w[4] += WEATHER_RAIN_SPEED_X;
		}
		obj_w[3] = 0;			// �j���A�j���t���O
		
		// �j���^�C�~���O	�S�����ʂ͈̔͂̒l
		obj_w[1] = WEATHER_RAIN_END_MIN  + (rand % WEATHER_RAIN_END_MAX);
		
		// ���W��ݒ�
		mat.x = ( WEATHER_RAIN_START_X_BASE + (frame * WEATHER_RAIN_MUL_X) + (rand % WEATHER_RAIN_START_X_MAX) );
		mat.y = WEATHER_RAIN_START_Y;
		WEATHER_OBJ_WORK_SetPosNoTurn( add_obj, &mat );
	}
}








//----------------------------------------------------------------------------
/**
 *	@brief		���J������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �X�p�[�N������
	WEATHER_PARK_Init( &p_local_wk->spark );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk, 
		WEATHER_STRAIN_ADD_START,	// obj�o�^��
		WEATHER_STRAIN_TIMING_MAX,// �o�^�^�C�~���O
		WEATHER_STRAIN_ADD_MAIN,
		WEATHER_STRAIN_TIMING_MIN,
		-WEATHER_STRAIN_TIMING_ADD,
		WEATHER_STRAIN_ADD_TIMING,
		WEATHER_STRAIN_ADD,
		WEATHER_SPARKRAIN_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	p_local_wk->work[0] = WEATHER_STRAIN_FOG_START;	// �������t�H�O�p
	p_local_wk->work[1] = 0;
	p_local_wk->work[2] = 0;							// ���J�E���^

	p_local_wk->work[4] = 39;							// ���J�E���^

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

	// ���C�g�ύX
	WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_rain_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

  if( p_local_wk->work[4] > 0 )
  {
    p_local_wk->work[4]--;
    if( p_local_wk->work[4] == 0 )
    {
      // ��
      WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	
    }
  }

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){
			WEATHER_TASK_FogFadeIn_Init( p_wk,
			WEATHER_FOG_SLOPE_DEFAULT, 
			WEATHER_FOG_DEPTH_DEFAULT + WEATHER_STRAIN_FOG_OFS, 
			WEATHER_STRAIN_FOG_TIMING,
			fog_cont );

		}
	}else{
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		

		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����

			// �V�[�P���X�ύX
			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );



	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_STRAIN_ADD_MAIN,	// obj�o�^��
		WEATHER_STRAIN_TIMING_MIN,// �o�^�^�C�~���O
		WEATHER_STRAIN_ADD_MAIN,
		WEATHER_STRAIN_TIMING_MIN,
		-WEATHER_STRAIN_TIMING_ADD,
		WEATHER_STRAIN_ADD_TIMING,
		WEATHER_STRAIN_ADD,
		WEATHER_SPARKRAIN_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_SPARKRAIN_OBJ_Add, WEATHER_STRAIN_NOFADE_OBJ_START_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_MOVE, heapID );

	p_local_wk->work[1] = 0;
	p_local_wk->work[2] = 0;							// ���J�E���^

  // ��
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->work[2] = (p_local_wk->work[2] + 1) % (WEATHER_STRAIN_OBJ_MUL_CHG*WEATHER_STRAIN_OBJ_MUL_NUM);		// �J�o�^���ύX�J�E���^
	// �J�E���^��0�����ɂȂ�����J�o�^
	WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );


	// �X�p�[�N���C��
	WEATHER_PARK_Main( &p_local_wk->spark, p_wk );

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�t�F�[�h�A�E�g�J�n
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_STRAIN_TIMING_MAX,
			WEATHER_STRAIN_TIMING_ADD,
			WEATHER_STRAIN_ADD_END );
	
	p_local_wk->work[0] = WEATHER_STRAIN_FOG_START_END;	// �������t�H�O�p


	// ��
	WEATHER_TASK_StopLoopSnd( p_wk );	
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	// �t�H�O����
	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeOut_Init( p_wk,
					WEATHER_FOG_DEPTH_DEFAULT_START, 
					WEATHER_STRAIN_FOG_TIMING_END, fog_cont );
		}
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result && result ){
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk, heapID );

	// �X�p�[�N������
	WEATHER_PARK_Exit( &p_local_wk->spark );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�I�u�W�F���C��
 */
//-----------------------------------------------------------------------------
static void WEATHER_SPARKRAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	int i;
	GFL_CLACTPOS mat;
	GFL_CLWK* p_clwk;
	s32* obj_w;

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	WEATHER_OBJ_WORK_GetPos( p_wk, &mat );
	
	// ����t���O���`�F�b�N
	switch(obj_w[3]){
	case 0:		// ����
		// ������
		mat.x += (obj_w[4]*2);
		mat.y += (obj_w[2]*2);
	
		// �j�����邩�`�F�b�N
		obj_w[0] += 2;
		if(obj_w[0] > obj_w[1]){
			// �A�j���[�V���������邩�`�F�b�N
			if(GFUser_GetPublicRand(10) < 5){		// 7/10�͔j��
				// �j��
				obj_w[3] = 2;
			}else{
			
				// �j���A�j��
				obj_w[3] = 1;		
				obj_w[0] = 4;			// �j���A�j���J�E���^
				GFL_CLACT_WK_SetAnmIndex( p_clwk, 3 );
			}
		}
		// ���W�ݒ�
		WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &mat );
		break;
	case 1:		// �j���A�j��
		if(obj_w[0]-- <= 0){
			obj_w[3] = 2;		// �j��
		}
		break;

	case 2:		// �j��
		WEATHER_TASK_DeleteObj( p_wk );
		break;
	}		

}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�I�u�W�F�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_SPARKRAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_SPARKRAIN_WORK*	p_local_wk;	// �V�X�e�����[�N
	int	err;	// �␳�l
	s32* obj_w;	// �I�u�W�F�N�g���[�N
	int speed_m;	// �X�s�[�h�ɂ�����l�e�[�u���̗v�f��
	int frame;		// �t���[����
	static const int WEATHER_STRAIN_OBJ_MUL[ WEATHER_STRAIN_OBJ_MUL_NUM ] = {1, 1, 2, 1, 3};
	u32 rand;
	GFL_CLACTPOS mat;	// �ݒ���W
	GFL_CLWK* p_clwk;

	// ���[�U���[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		add_obj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(add_obj == NULL){			// ���s������I��
			break;
		}
		obj_w = WEATHER_OBJ_WORK_GetWork( add_obj );		// �I�u�W�F���[�N�쐬
		p_clwk	= WEATHER_OBJ_WORK_GetClWk( add_obj );

		// �����擾
		rand = GFUser_GetPublicRand(0);
		
		// �̈��������
		obj_w[0] = 0;			// �J�E���^
		frame = rand%3;	// �J�̎��
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );

		obj_w[4] = WEATHER_STRAIN_SPEED_X * (frame+1);
		obj_w[2] = WEATHER_STRAIN_SPEED_Y * (frame+1);
		obj_w[4] *= WEATHER_STRAIN_OBJ_MUL[p_local_wk->work[2]/WEATHER_STRAIN_OBJ_MUL_CHG];
		obj_w[2] *= WEATHER_STRAIN_OBJ_MUL[p_local_wk->work[2]/WEATHER_STRAIN_OBJ_MUL_CHG];
		
		obj_w[3] = 0;			// �j���A�j���t���O
		
		// �j���^�C�~���O	�S�����ʂ͈̔͂̒l
		obj_w[1] = WEATHER_STRAIN_END_MIN  + (rand % WEATHER_STRAIN_END_MAX);
		obj_w[1] /= WEATHER_STRAIN_OBJ_MUL[p_local_wk->work[2]/WEATHER_STRAIN_OBJ_MUL_CHG];
		
		// ���W��ݒ�
		{
			mat.x = WEATHER_STRAIN_START_X + (rand % WEATHER_STRAIN_START_X_MAX);
			mat.y = WEATHER_STRAIN_START_Y + (rand % WEATHER_STRAIN_START_Y_MAX);

			WEATHER_OBJ_WORK_SetPosNoTurn( add_obj, &mat );
		}
		
	}

}



//----------------------------------------------------------------------------
/**
 *	@brief	������	������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_PARK_Init( WEATHER_SPARK_WORK* p_wk )
{
	GFL_STD_MemClear( p_wk, sizeof(WEATHER_SPARK_WORK) );

	p_wk->wait = GFUser_GetPublicRand( WEATHER_SPARK_INIT_RAND );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������@�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_PARK_Exit( WEATHER_SPARK_WORK* p_wk )
{
	GFL_STD_MemClear( p_wk, sizeof(WEATHER_SPARK_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	p_wk	���[�N
 *	@param	p_sys	�V�C�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_PARK_Main( WEATHER_SPARK_WORK* p_wk, WEATHER_TASK* p_sys )
{
#ifndef WEATHER_SPARK_COLOR_BLUE
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		blue --;
		OS_TPrintf( "blue %d\n", blue );
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		blue ++;
		OS_TPrintf( "blue %d\n", blue );
	}
#endif

	switch( p_wk->seq ){
	case WEATHER_SPARK_START_WAIT:
		p_wk->wait--;
		if( p_wk->wait < 0 ){
			p_wk->seq = WEATHER_SPARK_SETUP;
		}
		break;

	case WEATHER_SPARK_SETUP:
		{
			u32 rand;
			int i;

			rand = GFUser_GetPublicRand( WEATHER_SPARK_TYPE_RAND_MAX );
				
			// ���I��
			for( i=0; i<WEATHER_SPARK_TYPE_MAX; i++ ){
				if( sc_WEATHER_SPARK_TYPE_RAND[i] >= rand ){
					break;
				}
			}
			GF_ASSERT( i<WEATHER_SPARK_TYPE_MAX );

//			i = WEATHER_SPARK_TYPE_LIGHT_HARD;
//			OS_TPrintf( "spark_type %d\n", i );

			// ���ꂼ��̃^�C�v���Ƃɏ�����
			switch( i ){
			case WEATHER_SPARK_TYPE_LIGHT:
				WEATEHR_SPARK_SetUp_Light( p_wk );
				break;
			case WEATHER_SPARK_TYPE_HARD:
				WEATEHR_SPARK_SetUp_Hard( p_wk );
				break;
			case WEATHER_SPARK_TYPE_LIGHT_HARD:
				WEATEHR_SPARK_SetUp_LightAndHard( p_wk );
				break;
			}

			p_wk->spark_tbl_count	= 0;
			p_wk->seq				= WEATHER_SPARK_SPARK;
		}
		break;

	case WEATHER_SPARK_SPARK:	// ��
		WEATHER_TASK_LIGHT_StartColorFade( p_sys, WEATHER_SPARK_GetColor(p_wk), p_wk->spark_data[ p_wk->spark_tbl_count ].insync, p_wk->spark_data[ p_wk->spark_tbl_count ].outsync );
		// LIGHT->HARD�̏ꍇ�A�t�F�[�h���ɁA�����˂��Ă��܂�
		if( p_wk->spark_type == WEATHER_SPARK_TYPE_LIGHT_HARD ){
			p_wk->wait = (p_wk->spark_data[ p_wk->spark_tbl_count ].insync/2) + GFUser_GetPublicRand( p_wk->spark_data[ p_wk->spark_tbl_count ].outsync );
		}

    p_wk->snd_wait = p_wk->spark_data[ p_wk->spark_tbl_count ].snd_wait;

//		OS_TPrintf( "color %d\n", p_wk->spark_data[ p_wk->spark_tbl_count ].spark_power );
		
		p_wk->seq	= WEATHER_SPARK_SPARK_WAIT;
		break;

	case WEATHER_SPARK_SPARK_WAIT:	// ��

    // SE�̍Đ�
    if(p_wk->snd_wait > 0)
    {
      p_wk->snd_wait --;
      if( p_wk->snd_wait == 0 )
      {
        // ��SE�Đ�
        PMSND_PlaySE( WEATHER_SND_SE_SPARK );
      }
    }

		if( !WEATHER_TASK_LIGHT_IsColorFade( p_sys ) ){
			p_wk->seq	= WEATHER_SPARK_WAIT;
		}
		break;

	case WEATHER_SPARK_WAIT:
    // SE�̍Đ�
    if(p_wk->snd_wait > 0)
    {
      p_wk->snd_wait --;
      if( p_wk->snd_wait == 0 )
      {
        // ��SE�Đ�
        PMSND_PlaySE( WEATHER_SND_SE_SPARK );
      }
    }
		
		p_wk->spark_data[ p_wk->spark_tbl_count ].wait --;
		if( p_wk->spark_data[ p_wk->spark_tbl_count ].wait <= 0 ){
			// ���̃f�[�^������Ȃ玟�̃f�[�^��
			if( p_wk->spark_tbl_num > (p_wk->spark_tbl_count+1) ){
				p_wk->spark_tbl_count ++;
				p_wk->seq = WEATHER_SPARK_SPARK;
			}else{

				// �Z�b�g�A�b�v��
				p_wk->seq = WEATHER_SPARK_SETUP;
			}
		}
		
		break;

	default:
		break;
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	�y�����ݒ�
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATEHR_SPARK_SetUp_Light( WEATHER_SPARK_WORK* p_wk )
{
	int i;
	
	p_wk->spark_type		= WEATHER_SPARK_TYPE_LIGHT;
	p_wk->spark_tbl_num		= WEATHER_SPARK_LIGHT_NUM_MIN + GFUser_GetPublicRand( WEATHER_SPARK_LIGHT_NUM_MAX );

	GF_ASSERT( p_wk->spark_tbl_num < WEATHER_SPARK_TBL_MAX );

	for( i=0; i<p_wk->spark_tbl_num; i++ ){
		p_wk->spark_data[i].spark_power = sc_WEATHER_SPARK_STRONG_RAND_MIN[ WEATHER_SPARK_TYPE_LIGHT ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_STRONG_RAND_MAX[ WEATHER_SPARK_TYPE_LIGHT ] );
		p_wk->spark_data[i].insync		= WEATHER_SPARK_FLASHIN_SYNC;
		p_wk->spark_data[i].outsync		= sc_WEATHER_SPARK_FLASHOUT_SYNC[ p_wk->spark_data[i].spark_power ];
		p_wk->spark_data[i].wait		= sc_WEATHER_SPARK_TYPE_RAND_WAIT_MIN[ WEATHER_SPARK_TYPE_LIGHT ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_TYPE_RAND_WAIT_MAX[ WEATHER_SPARK_TYPE_LIGHT ] );
    p_wk->spark_data[i].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_LIGHT ];
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�d�����ݒ�
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATEHR_SPARK_SetUp_Hard( WEATHER_SPARK_WORK* p_wk )
{

	p_wk->spark_type		= WEATHER_SPARK_TYPE_HARD;
	p_wk->spark_tbl_num		= 1;

	p_wk->spark_data[0].spark_power = sc_WEATHER_SPARK_STRONG_RAND_MIN[ WEATHER_SPARK_TYPE_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_STRONG_RAND_MAX[ WEATHER_SPARK_TYPE_HARD ] );
	p_wk->spark_data[0].insync		= WEATHER_SPARK_FLASHIN_SYNC;
	p_wk->spark_data[0].outsync		= sc_WEATHER_SPARK_FLASHOUT_SYNC[ p_wk->spark_data[0].spark_power ];
	p_wk->spark_data[0].wait		= sc_WEATHER_SPARK_TYPE_RAND_WAIT_MIN[ WEATHER_SPARK_TYPE_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_TYPE_RAND_WAIT_MAX[ WEATHER_SPARK_TYPE_HARD ] );
  p_wk->spark_data[0].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_HARD ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�y��->�d�����ݒ�
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATEHR_SPARK_SetUp_LightAndHard( WEATHER_SPARK_WORK* p_wk )
{
	int i;
	
	p_wk->spark_type		= WEATHER_SPARK_TYPE_LIGHT_HARD;
	p_wk->spark_tbl_num		= WEATHER_SPARK_LIGHT_NUM_MIN + GFUser_GetPublicRand( WEATHER_SPARK_LIGHT_NUM_MAX );
	p_wk->spark_tbl_num		++;	// �Ō�̈ꔭ

	GF_ASSERT( p_wk->spark_tbl_num < WEATHER_SPARK_TBL_MAX );

	for( i=0; i<p_wk->spark_tbl_num-1; i++ ){
		p_wk->spark_data[i].spark_power = sc_WEATHER_SPARK_STRONG_RAND_MIN[ WEATHER_SPARK_TYPE_LIGHT ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_STRONG_RAND_MAX[ WEATHER_SPARK_TYPE_LIGHT ] );
		p_wk->spark_data[i].insync		= WEATHER_SPARK_FLASHIN_SYNC;
		p_wk->spark_data[i].outsync		= WEATHER_SPARK_LIGHT_HARD_OUTSYNC;
		p_wk->spark_data[i].wait		= GFUser_GetPublicRand( WEATHER_SPARK_LIGHT_HARD_WAIT_RAND );
    p_wk->spark_data[i].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_LIGHT ];
	}

	p_wk->spark_data[i].spark_power = sc_WEATHER_SPARK_STRONG_RAND_MIN[ WEATHER_SPARK_TYPE_LIGHT_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_STRONG_RAND_MAX[ WEATHER_SPARK_TYPE_LIGHT_HARD ] );
	p_wk->spark_data[i].insync		= WEATHER_SPARK_FLASHIN_SYNC;
	p_wk->spark_data[i].outsync		= sc_WEATHER_SPARK_FLASHOUT_SYNC[ p_wk->spark_data[i].spark_power ];
	p_wk->spark_data[i].wait		= sc_WEATHER_SPARK_TYPE_RAND_WAIT_MIN[ WEATHER_SPARK_TYPE_LIGHT_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_TYPE_RAND_WAIT_MAX[ WEATHER_SPARK_TYPE_LIGHT_HARD ] );
  p_wk->spark_data[i].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_LIGHT_HARD ];
}


//----------------------------------------------------------------------------
/**
 *	@brief	���F�̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	���F
 */
//-----------------------------------------------------------------------------
static GXRgb WEATHER_SPARK_GetColor( const WEATHER_SPARK_WORK* cp_wk )
{
	u32 power;

	power = cp_wk->spark_data[ cp_wk->spark_tbl_count ].spark_power;

#ifndef WEATHER_SPARK_COLOR_BLUE
	return GX_RGB( power, power, blue );
#else
	return GX_RGB( power, power, WEATHER_SPARK_COLOR_BLUE );
#endif
}


