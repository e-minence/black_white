//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_snow.c
 *	@brief		�V�C	��
 *	@author		tomoya takahashi
 *	@date		2009.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc/arc_def.h"
#include "arc/field_weather.naix"
#include "arc/field_weather_light.naix"


#include "weather_snow.h"

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
//	��Ǘ��֐��萔
//=====================================
#define	WEATHER_SNOW_TIMING_MIN		(10)				// ����o���^�C�~���O�ŏ�
#define WEATHER_SNOW_TIMING_MAX		(14)				// ����o���^�C�~���O�ő�
#define WEATHER_SNOW_TIMING_ADD		(5)					// �^�C�~���O�����炷��
#define	WEATHER_SNOW_ADD_TIMING		(1)					// �P�x�̃I�u�W�F�o�^���𑝂₷�^�C�~���O
#define	WEATHER_SNOW_ADD			(0)					// �I�u�W�F�o�^���𑫂��l
#define WEATHER_SNOW_MAIN			(1)					// ���C���ł͂��ꂾ���o�^

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_SNOW_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_SNOW_NOFADE_OBJ_START_DUST_NUM	( 2 )			// �������炷��
#define	WEATHER_SNOW_NOFADE_OBJ_START_DUST_MOVE	( 24 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_SNOW_FOG_TIMING		(200)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_SNOW_FOG_TIMING_END	(150)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_START		(8)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_START_END	(16)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_OFS		(0x300)

/*== ��I�u�W�F�N�g ==*/
#define	WEATHER_SNOW_ADD_TMG_X_BASE	(2)					// ��̃X�s�[�h�𑫂��^�C�~���O
#define	WEATHER_SNOW_ADD_TMG_X_RAN	(2)					// ��̃X�s�[�h�𑫂��^�C�~���O�����l
#define	WEATHER_SNOW_ADD_TMG_Y_BASE	(2)					// ��̃X�s�[�h�𑫂��^�C�~���O
#define	WEATHER_SNOW_ADD_TMG_Y_RAN	(1)					// ��̃X�s�[�h�𑫂��^�C�~���O�����l

#define	WEATHER_SSNOWTART_X_BASE	(-32)						// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_SSNOWTART_X_MAX	(414)						// X�J�n���W�����̍ő�l
#define WEATHER_SNOW_OBJ_NUM		(4)					// �I�u�W�F���

#define WEATHER_SNOW_OBJ_AUTODEST_TIMING_MAX ( 30 )	// ���Ń^�C���@�ő�
#define WEATHER_SNOW_OBJ_AUTODEST_TIMING_MIN ( 2 )	// ���Ń^�C���@�ŏ�

/*== ��O���t�B�b�N�\���f�[�^ ==*/
#define WEATHER_SNOW_GRAPHIC_CELL	( 3 )
#define WEATHER_SNOW_GRAPHIC_BG		( WEATHER_GRAPHIC_NONE )



//-------------------------------------
//	����Ǘ��֐��萔
//=====================================
#define	WEATHER_SSNOW_TIMING_MIN		(1)							// �J���o���^�C�~���O�ŏ�
#define WEATHER_SSNOW_TIMING_MAX		(6)						// �J���o���^�C�~���O�ő�
#define WEATHER_SSNOW_TIMING_ADD		(3)							// �^�C�~���O�����炷��
#define WEATHER_SSNOW_ADD_START			(1)							// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_SSNOW_ADD_TIMING		(2)							// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_SSNOW_ADD				(1)							// �o�^���鐔�𑝂₷��
#define WEATHER_SSNOW_ADD_END			(-1)							// �o�^���鐔�𑝂₷��
#define	WEATHER_SSNOW_ADD_MAIN			(1)							// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_SSNOW_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_SSNOW_NOFADE_OBJ_START_DUST_NUM		( 2 )			// �������炷��
#define	WEATHER_SSNOW_NOFADE_OBJ_START_DUST_MOVE	( 3 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_SSNOW_FOG_TIMING		(400)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_SSNOW_FOG_TIMING_END	(300)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_SSNOW_FOG_START			(8)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SSNOW_FOG_START_END		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SSNOW_FOG_OFS			(0x100)

/*== ��I�u�W�F�N�g ==*/
#define WEATHER_SSNOW_SPEED_X		(-1)						// ���ɐi�ރX�s�[�h
#define	WEATHER_SSNOW_END_MIN		(2)							// �I���J�E���^�ŏ�
#define WEATHER_SSNOW_END_MAX		(23)						// �I���J�E���^�ő�
#define WEATHER_SSNOW_END_NUM		(WEATHER_SSNOW_END_MAX - WEATHER_SSNOW_END_MIN)	// �J�E���^�̒l�̐�
#define	WEATHER_SSNOW_END_DIV		((WEATHER_SSNOW_END_NUM / 3)+1)	// �I�u�W�F�N�g�̎�ނ��v�Z����Ƃ�����l
#define	WEATHER_SSNOW_START_X_BASE	(-20)						// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_SSNOW_MUL_X			(20)						// �x�[�X�ɐ�̎�ޕ������l
#define	WEATHER_SSNOW_START_X_MAX	(420)						// X�J�n���W�����̍ő�l
#define	WEATHER_SSNOW_START_Y		(-8)						// Y�J�n���W

#define	WEATHER_SSNOW_ADD_SP_TMG_NUM (4)						// �X�s�[�h��ύX����^�C�~���O�̐�
#define	WEATHER_SSNOW_ADD_SP_TMG_TMG (100)						// �X�s�[�h��ύX����^�C�~���O�̃^�C�~���O

/*== ����O���t�B�b�N�\���f�[�^ ==*/
#define WEATHER_SSNOW_GRAPHIC_CELL	( 1 )
#define WEATHER_SSNOW_GRAPHIC_BG	( WEATHER_GRAPHIC_NONE )

//-----------------------
// �k�C����
//=======================
#define	WEATHER_SNOW_S_TIMING_MIN		(2)							// ����o���^�C�~���O�ŏ�
#define WEATHER_SNOW_S_TIMING_MAX		(3)						// ����o���^�C�~���O�ő�
#define WEATHER_SNOW_S_TIMING_ADD		(2)							// �^�C�~���O�����炷��
#define WEATHER_SNOW_S_ADD_START		(1)							// �ŏ��̓����ɐ��o�^���鐔
#define WEATHER_SNOW_S_ADD_TIMING		(2)							// ��̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_SNOW_S_ADD				(1)							// �o�^���鐔�𑝂₷��
#define WEATHER_SNOW_S_ADD_END			(-2)						// �o�^���鐔�𑝂₷��
#define	WEATHER_SNOW_S_ADD_MAIN			(1)						// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_SNOW_S_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_SNOW_S_NOFADE_OBJ_START_DUST_NUM	( 2 )		// �P�����炵�ē��삳����
#define	WEATHER_SNOW_S_NOFADE_OBJ_START_DUST_MOVE	( 2 )		// �P�����炵�ē��삳����

/*== �t�H�O ==*/
#define	WEATHER_SNOW_S_FOG_TIMING		(200)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_SNOW_S_FOG_TIMING_END	(150)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_S_FOG_START		(8)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_S_FOG_START_END	(10)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_S_FOG_OFS			(0x100)

/*== BG ==*/
#define WEATHER_SNOW_S_BG_ALP_TIM_S	(6)		// �A���t�@���P�グ��^�C�~���O
#define WEATHER_SNOW_S_BG_ALP_TIM_E	(8)		// �A���t�@���P������^�C�~���O

// �I�u�W�F 
#define	WEATHER_SNOW_S_END_MIN		(9)						// �I���J�E���^�ŏ�
#define WEATHER_SNOW_S_END_MAX		(12)						// �I���J�E���^�ő�
#define WEATHER_SNOW_S_START_X		(256)						// ������o���Ƃ���X���W
#define WEATHER_SNOW_S_START_X_RAN	(24)						// ������o���Ƃ���X���W
#define	WEATHER_SNOW_S_START_Y_BASE	(-32)							// �x�[�X�ɂȂ�Y�J�n���W
#define	WEATHER_SNOW_S_START_Y_MAX	(168)						// X�J�n���W�����̍ő�l


#define	WEATHER_SNOW_S_ADD_SP_TMG_NUM (4)						// �X�s�[�h��ύX����^�C�~���O�̐�
#define	WEATHER_SNOW_S_ADD_SP_CHG_TMG (256)						// �X�s�[�h��ύX����^�C�~���O�̃^�C�~���O

#define WEATHER_SNOW_S_ADD_SP_TMG		(4)						// �X�s�[�h��ύX����^�C�~���O
#define WEATHER_SNOW_S_SP_UP			(8)						// ���̒l�ȏ�̉��X�s�[�h�̂Ƃ��͔h��ɂ���
static const int WEATHER_SSNOW_SPEED_X_SML[WEATHER_SNOW_S_ADD_SP_TMG_NUM] = {-7,-11,-15,-19};
static const int WEATHER_SSNOW_SPEED_Y_SML[WEATHER_SNOW_S_ADD_SP_TMG_NUM] = {3,6,3,6};	// ���Ăɐi�ރX�s�[�h



//-------------------------------------
//	�����Ǘ��֐��萔
//=====================================
#define	WEATHER_ARARE_TIMING_MIN		(1)							// �J���o���^�C�~���O�ŏ�
#define WEATHER_ARARE_TIMING_MAX		(8)						// �J���o���^�C�~���O�ő�
#define WEATHER_ARARE_TIMING_ADD		(4)							// �^�C�~���O�����炷��
#define WEATHER_ARARE_ADD_START		(0)							// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_ARARE_ADD_TIMING		(2)							// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_ARARE_ADD			(1)							// �o�^���鐔�𑝂₷��
#define	WEATHER_ARARE_ADD_MAIN		(1)							// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_ARARE_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_ARARE_NOFADE_OBJ_START_DUST_NUM	( 10 )			// ���P�ʂŕω������邩
#define	WEATHER_ARARE_NOFADE_OBJ_START_DUST_MOVE	( 1 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_ARARE_FOG_TIMING		(150)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_ARARE_FOG_TIMING_END	(50)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_ARARE_FOG_START		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_ARARE_FOG_START_END	(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_ARARE_FOG_OFS		(0x300)

/*== �����I�u�W�F�N�g ==*/
#define WEATHER_ARARE_SPEED_X		(-4)						// ���ɐi�ރX�s�[�h
#define WEATHER_ARARE_SPEED_Y		(10)						// ���Ăɐi�ރX�s�[�h�x�[�X
#define WEATHER_ARARE_ADDSPEED_X	(2)						// ���ɐi�ރX�s�[�h�ɑ���������
#define WEATHER_ARARE_ADDSPEED_Y	(3)						// ���Ăɐi�ރX�s�[�h�ɑ���������
#define	WEATHER_ARARE_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_ARARE_END_MAX		(3)							// �I���J�E���^�ő�
#define	WEATHER_ARARE_START_X_BASE	(0)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_ARARE_MUL_X			(15)						// �x�[�X�ɉJ�̎�ޕ������l
#define	WEATHER_ARARE_START_X_MAX	(270)						// X�J�n���W�����̍ő�l
#define	WEATHER_ARARE_START_Y		(0)						// Y�J�n���W
#define	WEATHER_ARARE_SPEED_ERR		(23)						// �X�s�[�h�␳�l

#define WEATHER_ARARE_GROUND_SPEED_X		(-2)					// ���n��̉��ɐi�ރX�s�[�h
#define WEATHER_ARARE_GROUND_SPEED_Y		(2)						// ���n��̉��ɐi�ރX�s�[�h

// �����m��
#define WEATHER_ARARE_RAND_MAX		(100)
#define WEATHER_ARARE_RAND_SMALL	(30)
#define WEATHER_ARARE_RAND_MIDDLE	(95)
#define WEATHER_ARARE_RAND_BIG		(100)


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���ʂ̐჏�[�N
//=====================================
typedef struct {
	s32 work[10];
} WEATHER_SNOW_WORK;


//-------------------------------------
///	���჏�[�N
//=====================================
typedef struct {
	s32 work[10];
} WEATHER_SSNOW_WORK;

//-------------------------------------
///	�����Ɛ��჏�[�N
//=====================================
typedef struct {
	s32 work[10];
} WEATHER_SNOW_S_WORK;


//-------------------------------------
///	����ꃏ�[�N
//=====================================
typedef struct {
	s32 work[10];
} WEATHER_ARARE_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�ʏ�̐�
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_SNOW_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SNOW_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 





//-------------------------------------
///	����
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_SSNOW_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SSNOW_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 
static void WEATHER_SSNOW_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_SSNOW_WORK* p_wk );



//-------------------------------------
///	����������
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_SNOW_S_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SNOW_S_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 
static void WEATHER_SNOW_S_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_SNOW_S_WORK* p_wk );


//-------------------------------------
///	�����
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_ARARE_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_ARARE_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 

//-----------------------------------------------------------------------------
/**
 *			�V�C�f�[�^
 */
//-----------------------------------------------------------------------------
// �ʏ�̐�
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_SNOW = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	NARC_field_weather_snow_NCGR,			// OAM CG
	NARC_field_weather_snow_NCLR,			// OAM PLTT
	NARC_field_weather_snow_NCER,			// OAM CELL
	NARC_field_weather_snow_NANR,			// OAM CELLANM
	0,		// BGTEX
	0,		// GXTexSizeS
	0,		// GXTexSizeT
	0,		// GXTexRepeat
	0,		// GXTexFlip
	0,		// GXTexFmt
	0,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_SNOW_WORK),

	// �Ǘ��֐�
	WEATHER_SNOW_Init,		// ������
	WEATHER_SNOW_FadeIn,		// �t�F�[�h�C��
	WEATHER_SNOW_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_SNOW_Main,		// ���C������
	WEATHER_SNOW_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_SNOW_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SNOW_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_SNOW_OBJ_Move,
};



// ����
#if 1

WEATHER_TASK_DATA c_WEATHER_TASK_DATA_SNOWSTORM = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	TRUE,		// BG���g�p���邩�H
	NARC_field_weather_snow_s_NCGR,			// OAM CG
	NARC_field_weather_snow_NCLR,			// OAM PLTT
	NARC_field_weather_snow_s_NCER,			// OAM CELL
	NARC_field_weather_snow_s_NANR,			// OAM CELLANM
	NARC_field_weather_snow_storm_nsbtx,		// BGTEX
	GX_TEXSIZE_S32,		// GXTexSizeS
	GX_TEXSIZE_T32,		// GXTexSizeT
	GX_TEXREPEAT_ST,		// GXTexRepeat
	GX_TEXFLIP_NONE,		// GXTexFlip
	GX_TEXFMT_PLTT4,		// GXTexFmt
	GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_SNOW_S_WORK),

	// �Ǘ��֐�
	WEATHER_SNOW_S_Init,		// ������
	WEATHER_SNOW_S_FadeIn,		// �t�F�[�h�C��
	WEATHER_SNOW_S_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_SNOW_S_Main,		// ���C������
	WEATHER_SNOW_S_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_SNOW_S_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SNOW_S_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_SNOW_S_OBJ_Move,
};

#else

WEATHER_TASK_DATA c_WEATHER_TASK_DATA_SNOWSTORM = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	NARC_field_weather_snow_s_NCGR,			// OAM CG
	NARC_field_weather_snow_NCLR,			// OAM PLTT
	NARC_field_weather_snow_s_NCER,			// OAM CELL
	NARC_field_weather_snow_s_NANR,			// OAM CELLANM
	0,		// BGTEX
	0,		// GXTexSizeS
	0,		// GXTexSizeT
	0,		// GXTexRepeat
	0,		// GXTexFlip
	0,		// GXTexFmt
	0,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_SNOW_WORK),

	// �Ǘ��֐�
	WEATHER_SSNOW_Init,		// ������
	WEATHER_SSNOW_FadeIn,		// �t�F�[�h�C��
	WEATHER_SSNOW_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_SSNOW_Main,		// ���C������
	WEATHER_SSNOW_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_SSNOW_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SSNOW_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_SSNOW_OBJ_Move,
};

#endif


// �����
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_ARARE = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	NARC_field_weather_arare_NCGR,			// OAM CG
	NARC_field_weather_arare_NCLR,			// OAM PLTT
	NARC_field_weather_arare_NCER,			// OAM CELL
	NARC_field_weather_arare_NANR,			// OAM CELLANM
	0,		// BGTEX
	0,		// GXTexSizeS
	0,		// GXTexSizeT
	0,		// GXTexRepeat
	0,		// GXTexFlip
	0,		// GXTexFmt
	0,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_ARARE_WORK),

	// �Ǘ��֐�
	WEATHER_ARARE_Init,		// ������
	WEATHER_ARARE_FadeIn,		// �t�F�[�h�C��
	WEATHER_ARARE_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_ARARE_Main,		// ���C������
	WEATHER_ARARE_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_ARARE_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_ARARE_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_ARARE_OBJ_Move,
};




//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	���ʂ̐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	����������
 *
 *	@param	p_wk			���[�N
 *	@param	fog_cont		�t�H�O���L�����H
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	
	// ��I�u�W�F�̓o�^�t�F�[�h������
	WEATHER_TASK_ObjFade_Init( p_wk,
		1,
		WEATHER_SNOW_TIMING_MAX,
		WEATHER_SNOW_MAIN,
		WEATHER_SNOW_TIMING_MIN,
		-WEATHER_SNOW_TIMING_ADD,
		WEATHER_SNOW_ADD_TIMING,
		WEATHER_SNOW_ADD,
		WEATHER_SNOW_OBJ_Add );
	
	p_local_wk->work[1] = 0;							// �Ⴊ�P�񉺂܂ōs�������̃t���O
	
	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );
	
	p_local_wk->work[0] = WEATHER_SNOW_FOG_START;	// �������t�H�O�p
	
	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C������
 *
 *	@param	p_wk		���[�N
 *	@param	fog_cont	�t�H�O�Ǘ��t���O
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	BOOL result;
	BOOL fog_result;
	WEATHER_SNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeIn_Init( p_wk,
					WEATHER_FOG_SLOPE_DEFAULT, 
					WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_FOG_OFS, 
					WEATHER_SNOW_FOG_TIMING, fog_cont );


      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );
		}
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
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
 *	@brief	�t�F�[�h�Ȃ�������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_SNOW_MAIN,
		WEATHER_SNOW_TIMING_MIN,
		WEATHER_SNOW_MAIN,
		WEATHER_SNOW_TIMING_MIN,
		-WEATHER_SNOW_TIMING_ADD,
		WEATHER_SNOW_ADD_TIMING,
		WEATHER_SNOW_ADD,
		WEATHER_SNOW_OBJ_Add );
	
	p_local_wk->work[1] = 0;							// �Ⴊ�P�񉺂܂ōs�������̃t���O
	p_local_wk->work[2] = 0;							// �����j���t���O
	
	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_FOG_OFS, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_SNOW_OBJ_Add, WEATHER_SNOW_NOFADE_OBJ_START_NUM, WEATHER_SNOW_NOFADE_OBJ_START_DUST_NUM, WEATHER_SNOW_NOFADE_OBJ_START_DUST_MOVE, heapID );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Ⴡ�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
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
 *	@brief	�t�F�[�h�A�E�g�J�n
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// obj
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_SNOW_TIMING_MAX,
			WEATHER_SNOW_TIMING_ADD,
			-1 );
	// fog
	p_local_wk->work[0] = WEATHER_SNOW_FOG_START_END;	// �������t�H�O�p

	// �X�N���[������
	{
		int x, y;
		WEATHER_TASK_GetScrollDist( p_wk, &x, &y );
		WEATHER_TASK_ScrollObj( p_wk, x, y );
	}

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_WORK* p_local_wk;
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
					WEATHER_SNOW_FOG_TIMING_END, fog_cont );
		}
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result && result ){

			// �I�u�W�F�j���t���OON
			p_local_wk->work[2] = 1;
			
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
 *	@brief	�j������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );


	
	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��I�u�W�F����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_SNOW_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	GFL_CLACTPOS pos;
	GFL_CLWK* p_clwk;
	s32*	p_local_wk;
	s32*	p_flg;
	s32*	p_exitflg;

	// ���[�U���[�N�擾
	p_local_wk = WEATHER_OBJ_WORK_GetWork( p_wk );

	// �A�N�^�[���[�N�擾
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	// �S�j���v��
	p_exitflg = (s32*)p_local_wk[7];
	switch( p_local_wk[8] ){
	case 0:
		// �����@�j���t���O�������Ă����玩���j��������
		if( *p_exitflg ){
			p_local_wk[8] ++;
		}
		break;

	case 1:	// ������0�ɂȂ�����j��
		p_local_wk[9] --;
		if( p_local_wk[9] < 0 ){
			WEATHER_TASK_DeleteObj( p_wk );
			return ;
		}
		break;
	}
	

	
	WEATHER_OBJ_WORK_GetPos( p_wk, &pos );

	// ������
	// X���W�ړ�
	if(((p_local_wk[5] & 0xffff) >= p_local_wk[2])){
		pos.x	+= p_local_wk[1];
		p_local_wk[4]++;
		p_local_wk[5] &= 0xffff0000;
		// �X�s�[�h�`�F���W���[
		if(p_local_wk[4] < 10){
			p_local_wk[2]--;
		}else{
			p_local_wk[2]++;
		}
		if(p_local_wk[4] >= 20){	// ����ō��E�ɂ����
			p_local_wk[4] = 0;
			p_local_wk[1]*=-1;
		}//*/
	}
	// Y���W�ړ�
	if(((p_local_wk[5] >> 16) >= p_local_wk[3])){
		pos.y += 1;
		p_local_wk[5] &= 0x0000ffff;
	} 

	// ���W�ݒ�
	WEATHER_OBJ_WORK_SetPos( p_wk, &pos );
	
	// �J�E���g�ݒ�
	p_local_wk[6] = (p_local_wk[6] + 1) % 100;
	p_local_wk[5]++;
	p_local_wk[5]+=0x10000;

	// �j���]�[���ɂ�����j��  
	if(((pos.y < -284) && (pos.y > -296)) ||
		((pos.y > 212) && (pos.y < 232))){
		p_flg = (s32*)(p_local_wk[0]);
		*p_flg = 1;
		WEATHER_TASK_DeleteObj( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�N�g�o�^����
 *
 *	@param	p_wk	���[�N
 *	@param	num		��
 */
//-----------------------------------------------------------------------------
static void WEATHER_SNOW_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	int rand;	// ����
	WEATHER_OBJ_WORK* p_addobj;		// �o�^�I�u�W�F
	WEATHER_SNOW_WORK*	p_local_wk;	// �V�X�e�����[�N
	s32*	p_obj_wk;	// �I�u�W�F�N�g���[�N
	int	frame;		// �t���[��
	GFL_CLWK* p_clwk;
	GFL_CLACTPOS pos;	// ���W

	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
	
	if(p_local_wk->work[1] == 1){
		num *= 2;
	}
	
	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		p_addobj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(p_addobj == NULL){			// ���s������I��
			break;
		}
		p_obj_wk	= WEATHER_OBJ_WORK_GetWork( p_addobj );
		p_clwk		= WEATHER_OBJ_WORK_GetClWk( p_addobj );
		
		frame = GFUser_GetPublicRand( WEATHER_SNOW_OBJ_NUM );
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );
		
		p_obj_wk[4] = 10;	// ���̓��������ύX�J�E���^	
		p_obj_wk[5] = 0;
		p_obj_wk[6] = 0;	// �P���J�E���^
		p_obj_wk[8] = 0;							// �V�[�P���X
		
		rand = GFUser_GetPublicRand(0);

		p_obj_wk[9] = WEATHER_SNOW_OBJ_AUTODEST_TIMING_MIN + (rand % WEATHER_SNOW_OBJ_AUTODEST_TIMING_MAX);		// ���Ŷ���
	
		// �X�s�[�h
		if((rand % 2) == 0){
			p_obj_wk[1] = 1;
		}else{
			p_obj_wk[1] = -1;
		}
		p_obj_wk[2] = WEATHER_SNOW_ADD_TMG_X_BASE + (GFUser_GetPublicRand(WEATHER_SNOW_ADD_TMG_X_RAN));
		p_obj_wk[3] = WEATHER_SNOW_ADD_TMG_Y_BASE + (GFUser_GetPublicRand(WEATHER_SNOW_ADD_TMG_Y_RAN));

		p_obj_wk[0] = (s32)&p_local_wk->work[1];		// ���܂ōs������t���O�𗧂Ă�|�C���^��ۑ�
		p_obj_wk[7] = (s32)&p_local_wk->work[2];		// �j���t���O�|�C���^ON
		
		// ���W��ݒ�
		pos.x = ( WEATHER_SSNOWTART_X_BASE + (GFUser_GetPublicRand(WEATHER_SSNOWTART_X_MAX)) );
		
		if((p_local_wk->work[1] == 1) &&
			(i >= (num /2)) ){	// �����o��
			pos.y = ( -40 - GFUser_GetPublicRand(20));
		}else{
			pos.y = ( -8 - GFUser_GetPublicRand(20));
		}
		WEATHER_OBJ_WORK_SetPos( p_addobj, &pos );
	}

}




//----------------------------------------------------------------------------
/**
 *	@brief	���Ꮙ����
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_SSNOW_ADD_START,
		WEATHER_SSNOW_TIMING_MAX,
		WEATHER_SSNOW_ADD_MAIN,
		WEATHER_SSNOW_TIMING_MIN,
		-WEATHER_SSNOW_TIMING_ADD,
		WEATHER_SSNOW_ADD_TIMING,
		WEATHER_SSNOW_ADD,
		WEATHER_SSNOW_OBJ_Add );

	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	p_local_wk->work[0] = WEATHER_SNOW_S_FOG_START;	// �������t�H�O�p

	
	p_local_wk->work[1] = 0;					// �I�u�W�F���J�E���^
	p_local_wk->work[2] = 0;					// BG�J�E���^


	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );


  // SE
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_SNOWSTORM );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	BOOL result;
	BOOL fog_result;
	WEATHER_SSNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeIn_Init( p_wk,
					WEATHER_FOG_SLOPE_DEFAULT, 
					WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_S_FOG_OFS,
					WEATHER_SSNOW_FOG_TIMING, fog_cont );
      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );
		}
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����

			// BGON
			WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );

			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	// �X�N���[������
	WEATHER_SSNOW_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_SSNOW_ADD_MAIN,  
		WEATHER_SSNOW_TIMING_MIN,
		WEATHER_SSNOW_ADD_MAIN,
		WEATHER_SSNOW_TIMING_MIN,
		-WEATHER_SSNOW_TIMING_ADD,
		WEATHER_SSNOW_ADD_TIMING,
		WEATHER_SSNOW_ADD,
		WEATHER_SSNOW_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SSNOW_FOG_OFS, fog_cont );

	
	p_local_wk->work[1] = 0;					// �I�u�W�F���J�E���^
	p_local_wk->work[2] = 0;					// BG�J�E���^

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_SSNOW_OBJ_Add, WEATHER_SSNOW_NOFADE_OBJ_START_NUM, WEATHER_SSNOW_NOFADE_OBJ_START_DUST_NUM, WEATHER_SSNOW_NOFADE_OBJ_START_DUST_MOVE, heapID );


	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );

	// �X�N���[������
	WEATHER_SSNOW_SCROLL_Main( p_wk, p_local_wk );


	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_SSNOW_TIMING_MAX,
			WEATHER_SSNOW_TIMING_ADD,
			WEATHER_SSNOW_ADD_END );
	
	// fog
	p_local_wk->work[0] = WEATHER_SSNOW_FOG_START_END;	// �������t�H�O�p

	// �X�N���[������
	WEATHER_SSNOW_SCROLL_Main( p_wk, p_local_wk );

	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE );


  // SE
  WEATHER_TASK_StopLoopSnd( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	
	// �t�H�O����
	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeOut_Init( p_wk,
					WEATHER_FOG_DEPTH_DEFAULT_START, 
					WEATHER_SSNOW_FOG_TIMING_END, fog_cont );
		}
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result && result ){

			// �I�u�W�F�j���t���OON
			p_local_wk->work[2] = 1;
			
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	// �X�N���[������
	WEATHER_SSNOW_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SSNOW_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SSNOW_WORK* p_local_wk;

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
 *	@brief	����	�I�u�W�F����
 */
//-----------------------------------------------------------------------------
static void WEATHER_SSNOW_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	s32* obj_w;
	GFL_CLACTPOS mat;	// �ݒ���W
	GFL_CLWK* p_clwk;

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	GFL_CLACT_WK_GetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	
	// ����t���O���`�F�b�N
	switch(obj_w[3]){
	case 0:		// ����

		// ������
		mat.x += obj_w[4];
		mat.y += obj_w[2];

		// �j�����邩�`�F�b�N
		if(obj_w[0]++ > obj_w[1]){
			// �j��
			obj_w[3] = 1;
		}
		// �J�E���^�œ������l��ω�
		if((obj_w[0] % obj_w[5]) == 0){
			obj_w[4]--;
			
			
			if(obj_w[2] > 1){
				obj_w[2]--;
			}//*/
		}

		GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	
		break;
	case 1:		// �j��
		WEATHER_TASK_DeleteObj( p_wk );
		break;
	}		

}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�I�u�W�F�ǉ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_SSNOW_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	int tbl_num;
	WEATHER_SSNOW_WORK* sys_w;		// �V�X�e�����[�N
	s32* obj_w;			// �I�u�W�F�N�g���[�N
	static int WEATHER_SSNOW_ADD_SP_TMG[WEATHER_SSNOW_ADD_SP_TMG_NUM] = {8,16,8,5};
	static int WEATHER_SSNOW_SPEED_Y[WEATHER_SSNOW_ADD_SP_TMG_NUM] = {4,4,4,4};	// ���Ăɐi�ރX�s�[�h
	int frame;	// �t���[����
	GFL_CLACTPOS mat;	// �ݒ���W
	GFL_CLWK* p_clwk;
	
	sys_w = WEATHER_TASK_GetWorkData( p_wk );
	
	

	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		add_obj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(add_obj == NULL){			// ���s������I��
			break;
		}

		obj_w = WEATHER_OBJ_WORK_GetWork( add_obj );		// �I�u�W�F���[�N�쐬
		p_clwk	= WEATHER_OBJ_WORK_GetClWk( add_obj );

		// ���̒l�����߂�
		sys_w->work[1]++;
		if(sys_w->work[1] >= (WEATHER_SSNOW_ADD_SP_TMG_TMG * WEATHER_SSNOW_ADD_SP_TMG_NUM)){
			sys_w->work[1] = 0;
		}
		tbl_num = (sys_w->work[1] / WEATHER_SSNOW_ADD_SP_TMG_TMG);
		obj_w[5] = WEATHER_SSNOW_ADD_SP_TMG[ tbl_num ];
		
	
		
		// �̈��������
		obj_w[0] = 0;			// �J�E���^
		obj_w[1] = WEATHER_SSNOW_END_MIN + GFUser_GetPublicRand((WEATHER_SSNOW_END_MAX - WEATHER_SSNOW_END_MIN));	// �I���J�E���^
		frame = (obj_w[1] - WEATHER_SSNOW_END_MIN) / WEATHER_SSNOW_END_DIV;	// ����̎��
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );
		
		obj_w[4] = WEATHER_SSNOW_SPEED_X * (frame+1);
		obj_w[2] = WEATHER_SSNOW_SPEED_Y[tbl_num] * (frame+1);	// ���ăX�s�[�h
		obj_w[3] = 0;			// �j���A�j���t���O
		
		// ���W��ݒ�
		{
			mat.x = WEATHER_SSNOW_START_X_BASE + (frame * WEATHER_SSNOW_MUL_X) + GFUser_GetPublicRand(WEATHER_SSNOW_START_X_MAX);
			mat.y = WEATHER_SSNOW_START_Y;
			
			GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
		}
	
	}

}





//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[������
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_SSNOW_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_SSNOW_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
	p_wk->work[2] = (p_wk->work[2] + 6) % 256;
	WEATHER_TASK_3DBG_SetScrollX( p_sys, (p_wk->work[2]*2) - x );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->work[2]*2) + y );
}







//----------------------------------------------------------------------------
/**
 *	@brief	���Ꮙ����
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_SSNOW_ADD_START,	// obj�o�^��
		WEATHER_SSNOW_TIMING_MAX,	// �o�^�^�C�~���O
		WEATHER_SNOW_S_ADD_MAIN,
		WEATHER_SNOW_S_TIMING_MIN,
		-WEATHER_SNOW_S_TIMING_ADD,
		WEATHER_SNOW_S_ADD_TIMING,
		WEATHER_SNOW_S_ADD,
		WEATHER_SNOW_S_OBJ_Add );

	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	p_local_wk->work[0] = WEATHER_SNOW_S_FOG_START;	// �������t�H�O�p

	
	p_local_wk->work[1] = 0;					// �I�u�W�F���J�E���^
	p_local_wk->work[2] = 0;					// BG�J�E���^

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );


  // SE
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_SNOWSTORM );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	BOOL result;
	BOOL fog_result;
	WEATHER_SNOW_S_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){

			WEATHER_TASK_FogFadeIn_Init( p_wk,
					WEATHER_FOG_SLOPE_DEFAULT, 
					WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_S_FOG_OFS,
					WEATHER_SNOW_S_FOG_TIMING, fog_cont );

      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );
		}
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����

			// BGON
			WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );

			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	// �X�N���[������
	WEATHER_SNOW_S_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_SNOW_S_ADD_MAIN,	// obj�o�^��
		WEATHER_SNOW_S_TIMING_MIN,	// �o�^�^�C�~���O
		WEATHER_SNOW_S_ADD_MAIN,
		WEATHER_SNOW_S_TIMING_MIN,
		-WEATHER_SNOW_S_TIMING_ADD,
		WEATHER_SNOW_S_ADD_TIMING,
		WEATHER_SNOW_S_ADD,
		WEATHER_SNOW_S_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_S_FOG_OFS, fog_cont );

	
	p_local_wk->work[1] = 0;					// �I�u�W�F���J�E���^
	p_local_wk->work[2] = 0;					// BG�J�E���^

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_SNOW_S_OBJ_Add, WEATHER_SNOW_S_NOFADE_OBJ_START_NUM, WEATHER_SNOW_S_NOFADE_OBJ_START_DUST_NUM, WEATHER_SNOW_S_NOFADE_OBJ_START_DUST_MOVE, heapID );


	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_show_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;
	s32 wind;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	if( WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID ) ){

		wind = p_local_wk->work[1] / WEATHER_SNOW_S_ADD_SP_CHG_TMG;
		wind = WEATHER_SSNOW_SPEED_X_SML[wind];
		
		if(wind <= -WEATHER_SNOW_S_SP_UP){
			// �����o�^
			WEATHER_SNOW_S_OBJ_Add( p_wk, WEATHER_SNOW_S_ADD_MAIN, heapID );
		}
	}

	// �X�N���[������
	WEATHER_SNOW_S_SCROLL_Main( p_wk, p_local_wk );


	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_SNOW_S_TIMING_MAX,
			WEATHER_SNOW_S_TIMING_ADD,
			WEATHER_SNOW_S_ADD_END );
	
	// fog
	p_local_wk->work[0] = WEATHER_SNOW_S_FOG_START_END;	// �������t�H�O�p

	// �X�N���[������
	WEATHER_SNOW_S_SCROLL_Main( p_wk, p_local_wk );

	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE );

  // SE
  WEATHER_TASK_StopLoopSnd( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;
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
					WEATHER_SNOW_S_FOG_TIMING_END, fog_cont );
		}
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		}else{
			fog_result = TRUE;
		}
	
		if( fog_result && result ){

			// �I�u�W�F�j���t���OON
			p_local_wk->work[2] = 1;
			
			// �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
			// �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
			if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/
				
				return WEATHER_TASK_FUNC_RESULT_FINISH;
			}
		}
	}

	// �X�N���[������
	WEATHER_SNOW_S_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_S_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_SNOW_S_WORK* p_local_wk;

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
 *	@brief	����	�I�u�W�F����
 */
//-----------------------------------------------------------------------------
static void WEATHER_SNOW_S_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	s32* obj_w;
	GFL_CLACTPOS mat;	// �ݒ���W
	GFL_CLWK* p_clwk;

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	GFL_CLACT_WK_GetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	
	// ����t���O���`�F�b�N
	switch(obj_w[3]){
	case 0:		// ����

		// ������
		mat.x += obj_w[4];
		mat.y += obj_w[2];

		// �j�����邩�`�F�b�N
		if(obj_w[0]++ > obj_w[1]){
			// �j��
			obj_w[3] = 1;
		}
		// �J�E���^�œ������l��ω�
		if((obj_w[0] % WEATHER_SNOW_S_ADD_SP_TMG) == 0){
			obj_w[4] += obj_w[5];
			
			if(obj_w[2] > 1){
				obj_w[2]--;
			}
		}
	
		GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
		break;
	case 1:		// �j��
		WEATHER_TASK_DeleteObj( p_wk );
		break;
	}		

}

//----------------------------------------------------------------------------
/**
 *	@brief	����	�I�u�W�F�ǉ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_SNOW_S_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	int tbl_num;
	WEATHER_SNOW_S_WORK* sys_w;	// �V�X�e�����[�N
	s32* obj_w;	// �I�u�W�F�N�g���[�N
	int		frame;		// �t���[����
	GFL_CLWK* p_clwk;
	GFL_CLACTPOS mat;
	
	sys_w = WEATHER_TASK_GetWorkData( p_wk );

		
	// ���̒l�����߂�
	sys_w->work[1]++;
	if(sys_w->work[1] >= (WEATHER_SNOW_S_ADD_SP_CHG_TMG  * WEATHER_SNOW_S_ADD_SP_TMG_NUM)){
		sys_w->work[1] = 0;
	}
	tbl_num = (sys_w->work[1] / WEATHER_SNOW_S_ADD_SP_CHG_TMG );

	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		add_obj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(add_obj == NULL){			// ���s������I��
			break;
		}

		// �I�u�W�F���[�N
		obj_w = WEATHER_OBJ_WORK_GetWork( add_obj );
		p_clwk	= WEATHER_OBJ_WORK_GetClWk( add_obj );
		
		// �̈��������
		obj_w[0] = 0;			// �J�E���^
		obj_w[1] = WEATHER_SNOW_S_END_MIN + GFUser_GetPublicRand((WEATHER_SNOW_S_END_MAX - WEATHER_SNOW_S_END_MIN) );	// �I���J�E���^
		frame = GFUser_GetPublicRand(4);	// ����̎��
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );
		
		obj_w[4] = WEATHER_SSNOW_SPEED_X_SML[tbl_num] * ((frame)+1);
		obj_w[2] = WEATHER_SSNOW_SPEED_Y_SML[tbl_num] * ((frame)+1);	// ���ăX�s�[�h
		obj_w[3] = 0;			// �j���A�j���t���O
		if(frame == 3){
			obj_w[4] += WEATHER_SSNOW_SPEED_X_SML[tbl_num];
			obj_w[2] += WEATHER_SSNOW_SPEED_Y_SML[tbl_num];
		}
		obj_w[5] = WEATHER_SSNOW_SPEED_X_SML[tbl_num];
		
		// ���W��ݒ�
		mat.x = WEATHER_SNOW_S_START_X + GFUser_GetPublicRand(WEATHER_SNOW_S_START_X_RAN);
		mat.y = WEATHER_SNOW_S_START_Y_BASE + GFUser_GetPublicRand(WEATHER_SNOW_S_START_Y_MAX);

		GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	}

}





//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[������
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_SNOW_S_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_SNOW_S_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
	p_wk->work[2] = (p_wk->work[2] + 6) % 256;
	WEATHER_TASK_3DBG_SetScrollX( p_sys, (p_wk->work[2]*2) - x );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->work[2]*2) + y );
}






//----------------------------------------------------------------------------
/**
 *	@brief	������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_ARARE_ADD_START,
		WEATHER_ARARE_TIMING_MAX,
		WEATHER_ARARE_ADD_MAIN,
		WEATHER_ARARE_TIMING_MIN,
		-WEATHER_ARARE_TIMING_ADD,
		WEATHER_ARARE_ADD_TIMING,
		WEATHER_ARARE_ADD,
		WEATHER_ARARE_OBJ_Add );

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );



	p_local_wk->work[0] = WEATHER_ARARE_FOG_START;	// �������t�H�O�p

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

  // SE
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_ARARE );
	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
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
			WEATHER_FOG_DEPTH_DEFAULT + WEATHER_ARARE_FOG_OFS, 
			WEATHER_ARARE_FOG_TIMING,
			fog_cont );


      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_rain_dat, heapID );
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
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_ARARE_ADD_MAIN,
		WEATHER_ARARE_TIMING_MIN,
		WEATHER_ARARE_ADD_MAIN,
		WEATHER_ARARE_TIMING_MIN,
		-WEATHER_ARARE_TIMING_ADD,
		WEATHER_ARARE_ADD_TIMING,
		WEATHER_ARARE_ADD,
		WEATHER_ARARE_OBJ_Add );


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_ARARE_FOG_OFS, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_ARARE_OBJ_Add, WEATHER_ARARE_NOFADE_OBJ_START_NUM, WEATHER_ARARE_NOFADE_OBJ_START_DUST_NUM, WEATHER_ARARE_NOFADE_OBJ_START_DUST_MOVE, heapID );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_rain_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
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
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_ARARE_TIMING_MAX,
			WEATHER_ARARE_TIMING_ADD,
			-WEATHER_ARARE_ADD );
	
	// �t�H�O
	p_local_wk->work[0] = WEATHER_ARARE_FOG_START_END;	// �������t�H�O�p


  // SE
  WEATHER_TASK_StopLoopSnd( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
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
					WEATHER_ARARE_FOG_TIMING_END, fog_cont );
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
static WEATHER_TASK_FUNC_RESULT WEATHER_ARARE_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_ARARE_WORK* p_local_wk;
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
static void WEATHER_ARARE_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
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
	
		// �j�����邩�`�F�b�N
		obj_w[0] += 1;
		if(obj_w[0] > obj_w[1]){
			
			// �j���A�j��
			obj_w[3] = 1;		
			obj_w[0] = 8 + (obj_w[6] % 4) * 2;			// �j���A�j���J�E���^

			obj_w[4] = WEATHER_ARARE_GROUND_SPEED_X;
			obj_w[2] = WEATHER_ARARE_GROUND_SPEED_Y;

			GFL_CLACT_WK_SetAnmIndex( p_clwk, GFL_CLACT_WK_GetAnmIndex( p_clwk ) + 3 );
		}else{

			// ������
			mat.x += (obj_w[4]);
			mat.y += (obj_w[2]);

			// ���W�ݒ�
			WEATHER_OBJ_WORK_SetPos( p_wk, &mat );
		}

		break;
	case 1:		// �j���A�j��
		// ���W�ݒ�
		mat.x += obj_w[4];
		if( (obj_w[0] % 2) == 0 ){
			mat.y += obj_w[2];
		}else{
			mat.y -= obj_w[2];
		}

		if( (obj_w[0] % 4) == 0 ){

			if( (obj_w[2] - 1) >= 0 ){
				obj_w[2] --;
			}
			if( (obj_w[4] + 1) <= 0 ){
				obj_w[4] ++;
			}
		}
		WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &mat );

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
static void WEATHER_ARARE_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_ARARE_WORK*	p_local_wk;	// �V�X�e�����[�N
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
		if( (rand % WEATHER_ARARE_RAND_MAX) < WEATHER_ARARE_RAND_SMALL ){
			frame = 0;
		}else if( (rand % WEATHER_ARARE_RAND_MAX) < WEATHER_ARARE_RAND_MIDDLE ){
			frame = 1;
		}else if( (rand % WEATHER_ARARE_RAND_MAX) < WEATHER_ARARE_RAND_BIG ){
			frame = 2;
		}
		GFL_CLACT_WK_SetAnmIndex( p_clwk, 2-frame );

		err = (rand % WEATHER_ARARE_SPEED_ERR);
		obj_w[2] = WEATHER_ARARE_SPEED_Y * (frame+1) + err;
		if(frame == 2){		// ��ԑO�̉J�̓X�s�[�h���P�i�K�グ��
			obj_w[2] += WEATHER_ARARE_ADDSPEED_Y;
		}
		
		err /= WEATHER_ARARE_SPEED_X;
		obj_w[4] = WEATHER_ARARE_SPEED_X * (frame+1) + err;
		if(frame == 2){		// ��ԑO�̉J�̓X�s�[�h���P�i�K�グ��
			obj_w[4] += WEATHER_ARARE_ADDSPEED_X;
		}

		obj_w[3] = 0;			// �j���A�j���t���O
		
		// �j���^�C�~���O	�S�����ʂ͈̔͂̒l
		obj_w[1] = WEATHER_ARARE_END_MIN  + (rand % WEATHER_ARARE_END_MAX);
		
		// ���W��ݒ�
		mat.x = ( WEATHER_ARARE_START_X_BASE + (frame * WEATHER_ARARE_MUL_X) + (rand % WEATHER_ARARE_START_X_MAX) );
		mat.y = WEATHER_ARARE_START_Y;
		WEATHER_OBJ_WORK_SetPos( add_obj, &mat );

		obj_w[6] = rand;			// �����ۑ�
	}
}

