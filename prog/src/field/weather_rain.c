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
#define	WEATHER_RAIN_FOG_TIMING		(90)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_RAIN_FOG_TIMING_END	(50)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START_END	(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_OFS		(0)

/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_RAIN_SPEED_X		(8)						// ���ɐi�ރX�s�[�h
#define WEATHER_RAIN_SPEED_Y		(16)						// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_RAIN_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_RAIN_END_MAX		(2)							// �I���J�E���^�ő�
#define	WEATHER_RAIN_START_X_BASE	(-72)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_RAIN_MUL_X			(15)						// �x�[�X�ɉJ�̎�ޕ������l
#define	WEATHER_RAIN_START_X_MAX	(256)						// X�J�n���W�����̍ő�l
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
#define	WEATHER_STRAIN_ADD_MAIN		(1)					// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_STRAIN_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_STRAIN_NOFADE_OBJ_START_DUST_NUM	( 5 )		// �������쐔��ύX���邩
#define	WEATHER_STRAIN_NOFADE_OBJ_START_DUST_MOVE	( 1 )		// ���炷���쐔�̒l

/*== �t�H�O ==*/
#define	WEATHER_STRAIN_FOG_TIMING		(90)						// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_STRAIN_FOG_TIMING_END	(50)						// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_START		(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_START_END	(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STRAIN_FOG_OFS			(0x0)


/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_STRAIN_SPEED_X		(20)						// ���ɐi�ރX�s�[�h
#define WEATHER_STRAIN_SPEED_Y		(20)							// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_STRAIN_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_STRAIN_END_MAX		(2)							// �I���J�E���^�ő�
#define	WEATHER_STRAIN_START_X		(-256)							// �x�[�X�ɂȂ�X�J�n���W
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
	WEATHER_SPARK_SPARK_DARK,		// ��
	WEATHER_SPARK_SPARK_DARK_WAIT,		// ��
	WEATHER_SPARK_SPARK,		// ��
	WEATHER_SPARK_SPARK_WAIT,	// ��
	WEATHER_SPARK_WAIT,
};

// �����[�h
enum
{
  WEATHER_SPARK_MODE_EASY,
  WEATHER_SPARK_MODE_NORMAL,
  WEATHER_SPARK_MODE_RAIKAMI,
  WEATHER_SPARK_MODE_MAX,
};

// ���C�J�~�@��
#define WEATHER_SPARK_DARK_COLOR   ( GX_RGB(6,6,10) )
#define WEATHER_SPARK_DARK_SYNC   ( 4 )
#define WEATHER_SPARK_DARK_WAIT   ( 10 )

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



//-------------------------------------
///	���C�J�~�@�V�C
//=====================================
#define	WEATHER_RAIKAMI_TIMING_MIN		(0)					// �J���o���^�C�~���O�ŏ�
#define WEATHER_RAIKAMI_TIMING_MAX		(15)				// �J���o���^�C�~���O�ő�
#define WEATHER_RAIKAMI_TIMING_ADD		(3)					// �^�C�~���O�����炷��
#define WEATHER_RAIKAMI_ADD_START		(1)					// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_RAIKAMI_ADD_TIMING		(1)					// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_RAIKAMI_ADD			(1)						// �o�^���鐔�𑝂₷��
#define WEATHER_RAIKAMI_ADD_END		(-1)					// �o�^���鐔�𑝂₷��
#define	WEATHER_RAIKAMI_ADD_MAIN		(2)					// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_RAIKAMI_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_RAIKAMI_NOFADE_OBJ_START_DUST_NUM	( 5 )		// �������쐔��ύX���邩
#define	WEATHER_RAIKAMI_NOFADE_OBJ_START_DUST_MOVE	( 1 )		// ���炷���쐔�̒l

/*== �t�H�O ==*/
#define	WEATHER_RAIKAMI_FOG_TIMING		(80)						// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_RAIKAMI_FOG_TIMING_END	(50)						// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_RAIKAMI_FOG_START_END	(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIKAMI_FOG_OFS			(0x0)

/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_RAIKAMI_SPPED_RND (16)
#define WEATHER_RAIKAMI_SPEED_X		(14)						// ���ɐi�ރX�s�[�h
#define WEATHER_RAIKAMI_SPEED_Y		(14)							// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_RAIKAMI_END_MIN		(10)							// �I���J�E���^�ŏ�
#define WEATHER_RAIKAMI_END_MAX		(10)							// �I���J�E���^�ő�
#define	WEATHER_RAIKAMI_START_X		(-128)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_RAIKAMI_START_X_MAX	(384)						// X�J�n���W�����l
#define	WEATHER_RAIKAMI_START_Y		(-80)						// Y�J�n���W
#define	WEATHER_RAIKAMI_START_Y_MAX	(40)						// Y�J�n���W�����l

#define WEATHER_RAIKAMI_OBJ_MUL_NUM	(5)							// �I�u�W�F�̃X�s�[�h��ω�������l
#define WEATHER_RAIKAMI_OBJ_MUL_CHG	(60)						// �ύX�^�C�~���O
#define WEATHER_RAIKAMI_OBJ_MUL_ONE ( 10 )

#define WEATHER_RAIKAMI_OBJ_END_Y ( 192 )

//  �t�F�[�h�C���̏���
enum
{
  WEATHER_RAIKAMI_FADEIN_SEQ_INIT,
  WEATHER_RAIKAMI_FADEIN_SEQ_LIGHT_DARK,
  WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_00,
  WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_01,
  WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_02,
  WEATHER_RAIKAMI_FADEIN_SEQ_OBJADD,

};

#define WEATHER_RAIKAMI_SPARK_DARK_COLOR   ( GX_RGB(4,4,8) )
#define WEATHER_RAIKAMI_SPARK_DARK_SYNC   ( 8 )
#define WEATHER_RAIKAMI_SPARK_COLOR ( GX_RGB( 30,30,8 ) )
#define WEATHER_RAIKAMI_SPARK_INSYNC ( 4 )
#define WEATHER_RAIKAMI_SPARK_OUTSYNC ( 60 )

#define WEATHER_RAIKAMI_LIGHT_SPARK_WAIT ( 90 )
#define WEATHER_RAIKAMI_LIGHT_DARKSPARK_WAIT ( 16 )

#define WEATHER_RAIKAMI_LIGHT_FULL_RAIN_WAIT ( 48 )





//-------------------------------------
///	�J�U�J�~�@�V�C
//=====================================
// �t�F�[�h�C���̗���
enum
{
  KAZAKAMI_FADEIN_SEQ_INIT,
  KAZAKAMI_FADEIN_SEQ_LIGHT_DARK,
  KAZAKAMI_FADEIN_SEQ_WIND02,
};
#define KAZAKAMI_FADEIN_WIND_WAIT ( WEATHER_RAIKAMI_LIGHT_SPARK_WAIT ) // �Â��Ȃ肾���Ă��畗���o�Ă���܂ł̊�
#define KAZAKAMI_FADEIN_BGSTART_WAIT (8)


#define	WEATHER_KAZAKAMI_TIMING_MIN		(1)					// �J���o���^�C�~���O�ŏ�
#define WEATHER_KAZAKAMI_TIMING_MAX		(4)				// �J���o���^�C�~���O�ő�
#define WEATHER_KAZAKAMI_TIMING_ADD		(4)					// �^�C�~���O�����炷��
#define WEATHER_KAZAKAMI_ADD_START		(1)					// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_KAZAKAMI_ADD_TIMING		(1)					// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_KAZAKAMI_ADD			(1)						// �o�^���鐔�𑝂₷��
#define WEATHER_KAZAKAMI_ADD_END		(-1)					// �o�^���鐔�𑝂₷��
#define	WEATHER_KAZAKAMI_ADD_MAIN		(1)					// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_KAZAKAMI_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_KAZAKAMI_NOFADE_OBJ_START_DUST_NUM	( 5 )		// �������쐔��ύX���邩
#define	WEATHER_KAZAKAMI_NOFADE_OBJ_START_DUST_MOVE	( 1 )		// ���炷���쐔�̒l

/*== �t�H�O ==*/
#define	WEATHER_KAZAKAMI_FOG_TIMING		(10)						// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_KAZAKAMI_FOG_TIMING_END	(50)						// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_KAZAKAMI_FOG_OFS			(-16)
#define WEATHER_KAZAKAMI_FOG_OFS_NORMAL			(0x0)

enum
{
  KAZAKAMI_WIND_SCENE_NONE,
  KAZAKAMI_WIND_SCENE_RAIN,
  KAZAKAMI_WIND_SCENE_WINDRAIN,
  
  KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN,
  KAZAKAMI_WIND_SCENE_WINDRAIN_TO_RAIN,

  KAZAKAMI_WIND_SCENE_MAX,
};
#define KAZAKAMI_WIND_WINDRAIN_PARCENT  ( 50 )
#define KAZAKAMI_RAIN_RAIN_TO_WINDRAIN_PARCENT  ( 80 )

#define KAZAKAMI_FIRST_RAIN_TIME  ( 120 )

/*== �J�I�u�W�F�N�g ==*/
enum
{
  KAZAKAMI_OBJ_WIND,      // ���\���I�u�W�F�N�g
  KAZAKAMI_OBJ_RAIN,      // �J�\���I�u�W�F�N�g
  KAZAKAMI_OBJ_WIND_RAIN, // ���J�\���I�u�W�F�N�g

  KAZAKAMI_OBJ_TYPE_NUM,  // �I�u�W�F�N�g�^�C�v��
};

// ���X�s�[�h�@��l
#define KAZAKAMI_OBJ_WIND_SPEED_DEF ( 8 ) // 8=1
#define KAZAKAMI_OBJ_WIND_SPEED_GET( x ) ((x) / KAZAKAMI_OBJ_WIND_SPEED_DEF)
#define KAZAKAMI_OBJ_WIND_SPEED_SET( x ) ((x) * KAZAKAMI_OBJ_WIND_SPEED_DEF)

// �؂�ւ�蕗SE�Đ��^�C�~���O
#define KAZAKAMI_WIND_SCENE_CHANGE_SE_PLAY_TIMING ( 24 )

// �\���̕����[�v�^�C�~���O
#define KAZAKAMI_WIND_SE_LOOP_TIME_MIN  (50)
#define KAZAKAMI_WIND_SE_LOOP_TIME_DIF  (16)
#define KAZAKAMI_WIND_SE_LOOP_TIME      (KAZAKAMI_WIND_SE_LOOP_TIME_MIN)

#define KAZAKAMI_WIND_SE_LOOP_NEED_TIME (10)

#define KAZAKAMI_WIND_SE_LOOP_VOLUME_MIN  ( 100 )
#define KAZAKAMI_WIND_SE_LOOP_VOLUME_DIF  ( 127 - KAZAKAMI_WIND_SE_LOOP_VOLUME_MIN )

// ���ڂ������
#define KAZAKAMI_WIND_SCENE_RAIN_TIME_MIN ( 60 )
#define KAZAKAMI_WIND_SCENE_RAIN_TIME_RND ( 200 )

#define KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_MIN ( 60 )
#define KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_RND ( 400 )
#define KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_TIME_MIN ( 16 )

#define KAZAKAMI_WIND_SCENE_RAIN_WIND_SPEED_X (0)
#define KAZAKAMI_WIND_SCENE_RAIN_WIND_SPEED_Y (0)
#define KAZAKAMI_WIND_SCENE_RAIN_WINDON_SPEED_X (KAZAKAMI_OBJ_WIND_SPEED_SET(20))
#define KAZAKAMI_WIND_SCENE_RAIN_WINDON_SPEED_Y (KAZAKAMI_OBJ_WIND_SPEED_SET(2))
#define KAZAKAMI_WIND_SCENE_RAIN_WIND_ON_TIME ( 9 )

#define KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SPEED_X (KAZAKAMI_OBJ_WIND_SPEED_SET(20))
#define KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SPEED_Y (KAZAKAMI_OBJ_WIND_SPEED_SET(2))
#define KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SCALE_X (0x800)

#define KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_TBL_NUM (6)
#define KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_CHANGE_TIME (12 * KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_TBL_NUM) // �e�[�u�����ׂĂ�ύX�����
static const VecFx32 KAZAKAMI_WIND_SCENE_RAIN_RAINWIND_SPEED_TBL[ KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_TBL_NUM ] = 
{
  { KAZAKAMI_OBJ_WIND_SPEED_SET(26), KAZAKAMI_OBJ_WIND_SPEED_SET(3),}, 
  { KAZAKAMI_OBJ_WIND_SPEED_SET(36), KAZAKAMI_OBJ_WIND_SPEED_SET(2),},
  { KAZAKAMI_OBJ_WIND_SPEED_SET(30), KAZAKAMI_OBJ_WIND_SPEED_SET(3),},
  { KAZAKAMI_OBJ_WIND_SPEED_SET(38), KAZAKAMI_OBJ_WIND_SPEED_SET(3),},
  { KAZAKAMI_OBJ_WIND_SPEED_SET(24), KAZAKAMI_OBJ_WIND_SPEED_SET(3),},
  { KAZAKAMI_OBJ_WIND_SPEED_SET(32), KAZAKAMI_OBJ_WIND_SPEED_SET(2),},
};

// �w�i�̃X�P�[���l
static const fx32 KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_TBL[] = 
{
  0x1100, 
  0x1200, 
  0x1300, 
  0x1300, 
  0x1300, 
  0x1200, 
  0x1100, 
  0x1000, 
  0x1000, 
  0x1200, 
  0x1300, 
  0x1200, 
  0x1200, 
  0x1100, 
};
#define KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_NUM ( NELEMS(KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_TBL) )
#define KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_CHANGE_TIME ( 16*KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_NUM )


// WINDRAIN�̎��ɂ́AOBJ�̓o�^������ύX����
#define KAZAKAMI_WIND_SCENE_WIND_ADD_CUT_NUM ( 2 )


// ���\���I�u�W�F
#define KAZAKAMI_OBJ_WIND_DELETE_X  ( 256 )
#define KAZAKAMI_OBJ_WIND_DELETE_Y  ( 192 )

#define KAZAKAMI_OBJ_WIND_SPEED_X_MIN ( 16 )
#define KAZAKAMI_OBJ_WIND_SPEED_X_RND ( 24 )
#define KAZAKAMI_OBJ_WIND_SPEED_Y_MIN ( 8 )
#define KAZAKAMI_OBJ_WIND_SPEED_Y_RND ( 24 )

#define KAZAKAMI_OBJ_WIND_ADD_X ( -8 )
#define KAZAKAMI_OBJ_WIND_ADD_X_RND ( 8 )
#define KAZAKAMI_OBJ_WIND_ADD_Y_MIN ( -16 )
#define KAZAKAMI_OBJ_WIND_ADD_Y_RND ( 160 )


// ���J�\���I�u�W�F
#define KAZAKAMI_OBJ_WINDRAIN_SPEED_X_MIN ( 80 )
#define KAZAKAMI_OBJ_WINDRAIN_SPEED_X_RND ( 24 )
#define KAZAKAMI_OBJ_WINDRAIN_SPEED_Y_MIN ( 14 )
#define KAZAKAMI_OBJ_WINDRAIN_SPEED_Y_RND ( 24 )
#define KAZAKAMI_OBJ_WINDRAIN_ADD_X_MIN ( -8 )
#define KAZAKAMI_OBJ_WINDRAIN_ADD_X_RND ( 8 )
#define KAZAKAMI_OBJ_WINDRAIN_ADD_Y_MIN ( -16 )
#define KAZAKAMI_OBJ_WINDRAIN_ADD_Y_RND ( 160 )

// �J�\���I�u�W�F
#define KAZAKAMI_OBJ_RAIN_SPEED_X_MIN ( 86 ) // 8=1
#define KAZAKAMI_OBJ_RAIN_SPEED_RND ( 43 ) // 8=1
#define KAZAKAMI_OBJ_RAIN_SPEED_Y_MIN ( 258 )
#define KAZAKAMI_OBJ_RAIN_SPEED_Y_RND_MUL ( 2 )
#define KAZAKAMI_OBJ_RAIN_ADD_X_MIN ( -80 )
#define KAZAKAMI_OBJ_RAIN_ADD_X_RND ( 336 )
#define KAZAKAMI_OBJ_RAIN_ADD_Y_MIN ( -80 )
#define KAZAKAMI_OBJ_RAIN_ADD_Y_RND ( 40 )

#define KAZAKAMI_OBJ_RAIN_DELETE_X  ( 256 )
#define KAZAKAMI_OBJ_RAIN_DELETE_Y  ( 160 )


//-------------------------------------
//	�[��
//
//	���ԑтɂ��A������ς���B
//
//	�[���i17����2���ԑO�ɔ����j
//
//=====================================

#define EVENING_RAIN_TIME_AREA_MIN  ( 7140 )  // �[�����N���鎞�ԁ@�i�[���܂ł̎��ԁj
#define EVENING_RAIN_TIME_AREA  ( 7200 )  // �[�����N���鎞�ԁ@�i�[���܂ł̎��ԁj
#define EVENING_RAIN_TIME_KIRAKIRA_AREA     ( 7080 )  // ���炫�炪�I���܂ł̒�` �[���ɂȂ��Ă���1����
enum
{
  /* �V�C��ތS */
  EVENING_RAIN_TYPE_NONE = 0,
  EVENING_RAIN_TYPE_RAIN,
  EVENING_RAIN_TYPE_KIRAKIRA,
  EVENING_RAIN_TYPE_MAX,

  
  /* �V�[�P���X�S */
  EVENING_RAIN_SEQ_NONE = 0,
  
  // ���V�[�P���X
  EVENING_RAIN_SEQ_SPARKSOUND_FADEIN,
  EVENING_RAIN_SEQ_SPARKSOUND_ON,
  EVENING_RAIN_SEQ_SPARKSOUND_FADEOUT,
  
  // ���J�V�[�P���X
  EVENING_RAIN_SEQ_FADEIN,
  EVENING_RAIN_SEQ_ON,
  EVENING_RAIN_SEQ_FADEOUT,

  // ���炫��V�[�P���X
  EVENING_RAIN_SEQ_KIRAKIRA_FADEIN,
  EVENING_RAIN_SEQ_KIRAKIRA_ON,
  EVENING_RAIN_SEQ_KIRAKIRA_FADEOUT,


  /* ����I�u�W�F�^�C�v */
  EVENING_RAIN_OBJ_TYPE_NORMAL = 0,
  EVENING_RAIN_OBJ_TYPE_KIRAKIRA,

  /* ���炫�瓮��I�u�W�F�p�����[�^ */
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_TIME_MIN = 4,
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_TIME_RAND = 32,
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_X_MIN = -64,
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_X_RAND = 384,
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_Y_MIN = -32,
  EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_Y_RAND = 256,
};


#define	EVENING_RAIN_TIMING_MIN		(0)					// �J���o���^�C�~���O�ŏ�
#define EVENING_RAIN_TIMING_MAX		(6)				// �J���o���^�C�~���O�ő�
#define EVENING_RAIN_TIMING_ADD		(3)					// �^�C�~���O�����炷��
#define EVENING_RAIN_ADD_START		(1)					// �ŏ��̓����ɉJ��o�^���鐔
#define EVENING_RAIN_ADD_TIMING		(2)					// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define EVENING_RAIN_ADD			(1)						// �o�^���鐔�𑝂₷��
#define EVENING_RAIN_ADD_END		(-1)					// �o�^���鐔�𑝂₷��
#define	EVENING_RAIN_ADD_MAIN		(1)					// ���C���V�[�P���X�ł̓o�^���鐔


#define	EVENING_RAIN_KIRAKIRA_TIMING_MIN		(4)					// �J���o���^�C�~���O�ŏ�
#define EVENING_RAIN_KIRAKIRA_TIMING_MAX		(10)				// �J���o���^�C�~���O�ő�
#define EVENING_RAIN_KIRAKIRA_TIMING_ADD		(3)					// �^�C�~���O�����炷��
#define EVENING_RAIN_KIRAKIRA_ADD_START		(1)					// �ŏ��̓����ɉJ��o�^���鐔
#define EVENING_RAIN_KIRAKIRA_ADD_TIMING		(2)					// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define EVENING_RAIN_KIRAKIRA_ADD			(1)						// �o�^���鐔�𑝂₷��
#define EVENING_RAIN_KIRAKIRA_ADD_END		(-1)					// �o�^���鐔�𑝂₷��
#define	EVENING_RAIN_KIRAKIRA_ADD_MAIN		(1)					// ���C���V�[�P���X�ł̓o�^���鐔


/*== �t�H�O ==*/
#define	EVENING_RAIN_KIRAKIRA_FOG_TIMING		(90)						// �ɂP��t�H�O�e�[�u���𑀍�
#define	EVENING_RAIN_KIRAKIRA_FOG_TIMING_END	(50)						// �ɂP��t�H�O�e�[�u���𑀍�
#define EVENING_RAIN_KIRAKIRA_FOG_START		(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define EVENING_RAIN_KIRAKIRA_FOG_START_END	(1)					// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define EVENING_RAIN_KIRAKIRA_FOG_OFS			(32576)
#define EVENING_RAIN_KIRAKIRA_FOG_SLOPE			(8)

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

  s16 snd_wait;
  u16 snd_se;
} SPARK_DATA;


//-------------------------------------
///	��
//=====================================
typedef struct {
	u8 seq;
  u8 mode;
	u16 spark_type;
	u16	spark_tbl_num;
	u16 spark_tbl_count;
	
	SPARK_DATA	spark_data[ WEATHER_SPARK_TBL_MAX ];

	s32	wait;
  s16 snd_wait;
  s16 snd_se;
} WEATHER_SPARK_WORK;

//-------------------------------------
///	���J
//=====================================
typedef struct {
	s32 work[10];

	WEATHER_SPARK_WORK	spark;
} WEATHER_SPARKRAIN_WORK;



//-------------------------------------
///	���C�J�~
//=====================================
typedef struct {
  s16 seq;
  s16 wait;

  s32 scroll;
  s32 obj_speed_change;
  

	WEATHER_SPARK_WORK	spark;
} WEATHER_RAIKAMI_WORK;




//-------------------------------------
///	�J�U�J�~
//=====================================
typedef struct {
  s16 seq;
  s16 wait;

  s16 scroll_x;
  s16 scroll_y;
  s16 scroll_back_x;
  s16 scroll_back_y;
  s16 wind_scene_count;
  s16 wind_scene_count_max;
  s16 wind_scene_type;
  s16 wind_se_count;
  s16 wind_se_count_max;

  s16 wind_add_x; // ���悳�i�����l�j
  s16 wind_add_y;
  fx32 wind_bg_back_scale_x;

} WEATHER_KAZAKAMI_WORK;


//-------------------------------------
/// �J�U�J�~�@���\���@�I�u�W�F	
//=====================================
typedef struct 
{
  s16 pos_x;
  s16 pos_y;
  s16 speed_x;
  s16 speed_y;
  s16 last_speed_x;
  s16 last_speed_y;
} KAZAKAMI_OBJ_WIND_WK;

//-------------------------------------
/// �J�U�J�~�@�J�\���@�I�u�W�F	
//=====================================
typedef struct 
{
  s16 pos_x;
  s16 pos_y;
  s16 speed_x;
  s16 speed_y;
} KAZAKAMI_OBJ_RAIN_WK;

//-------------------------------------
/// �J�U�J�~�@���J�\���@�I�u�W�F	
//=====================================
typedef struct 
{
  s16 pos_x;
  s16 pos_y;
  s16 speed_x;
  s16 speed_y;
  s16 last_speed_x;
  s16 last_speed_y;
} KAZAKAMI_OBJ_WINDRAIN_WK;

//-------------------------------------
///	�J�U�J�~�@�I�u�W�F���[�N
//=====================================
typedef struct 
{
  u32 type;
  const WEATHER_KAZAKAMI_WORK* cp_parent;

  union
  {
    KAZAKAMI_OBJ_WIND_WK      wind; 
    KAZAKAMI_OBJ_RAIN_WK      rain; 
    KAZAKAMI_OBJ_WINDRAIN_WK  windrain; 
  }objwk;
} KAZAKAMI_OBJ_WK;




//-------------------------------------
///	�[��
//=====================================
typedef struct {
	s32 work[10];

  u16 time_in;  // EVENING_RAIN_TYPE_
  s16 seq;

  s32 scroll;

	WEATHER_SPARK_WORK	spark;
  
} WEATHER_EVENING_RAIN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�ʏ�̉J
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_RAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 


//-------------------------------------
///	���J
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_SPARKRAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SPARKRAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 



//-------------------------------------
///	�J�~�V���[�Y�@���C�J�~
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_RAIKAMI_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_RAIKAMI_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 

static void WEATHER_RAIKAMI_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_RAIKAMI_WORK* p_wk );


//-------------------------------------
///	�J�~�V���[�Y�@�J�U�J�~
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_KAZAKAMI_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_KAZAKAMI_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 

static void WEATHER_KAZAKAMI_OBJ_WIND_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 
static void WEATHER_KAZAKAMI_OBJ_WIND_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 
static void WEATHER_KAZAKAMI_OBJ_RAIN_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 
static void WEATHER_KAZAKAMI_OBJ_RAIN_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 
static void WEATHER_KAZAKAMI_OBJ_WINDRAIN_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 
static void WEATHER_KAZAKAMI_OBJ_WINDRAIN_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ); 


static void WEATHER_KAZAKAMI_WindStartRain( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, BOOL fadeout );
static void WEATHER_KAZAKAMI_WindControl( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, BOOL fadeout );
static void WEATHER_KAZAKAMI_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk );
static void WEATHER_KAZAKAMI_WindSePlay( WEATHER_TASK* p_sys, const WEATHER_KAZAKAMI_WORK* cp_wk, BOOL fog_cont, u16 se_no );
static void WEATHER_KAZAKAMI_WINDRAIN_SCENE_InitWindSePlay( WEATHER_KAZAKAMI_WORK* p_wk );
static void WEATHER_KAZAKAMI_WINDRAIN_SCENE_WindSePlay( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, BOOL fog_cont );


//-------------------------------------
///	��
//=====================================
static void WEATHER_PARK_Init( WEATHER_SPARK_WORK* p_wk, u8 mode );
static void WEATHER_PARK_Exit( WEATHER_SPARK_WORK* p_wk );
static void WEATHER_PARK_Main( WEATHER_SPARK_WORK* p_wk, WEATHER_TASK* p_sys );

static void WEATEHR_SPARK_SetUp_Light( WEATHER_SPARK_WORK* p_wk );
static void WEATEHR_SPARK_SetUp_Hard( WEATHER_SPARK_WORK* p_wk );
static void WEATEHR_SPARK_SetUp_LightAndHard( WEATHER_SPARK_WORK* p_wk );

static GXRgb WEATHER_SPARK_GetColor( const WEATHER_SPARK_WORK* cp_wk );


//-------------------------------------
///	�[��
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static void WEATHER_EVENING_RAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_EVENING_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 
static void WEATHER_EVENING_RAIN_OBJ_AddKirakira( WEATHER_TASK* p_wk, int num, HEAPID heapID ); 

static void WEATHER_EVENING_RAIN_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_EVENING_RAIN_WORK* p_wk );

static u32 WEATHER_EVENING_RAIN_CheckTimeType_Init( const WEATHER_TASK* cp_wk );
static u32 WEATHER_EVENING_RAIN_CheckTimeType_Main( const WEATHER_TASK* cp_wk, u32 now_evening_rain_type );

// �[���@�J
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 

// �[���@���炫��
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ); 


static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_Init[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_Init,
  WEATHER_EVENING_RAIN_KIRAKIRA_Init,
};
static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_FadeIn[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_FadeIn,
  WEATHER_EVENING_RAIN_KIRAKIRA_FadeIn,
};
static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_NoFade[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_NoFade,
  WEATHER_EVENING_RAIN_KIRAKIRA_NoFade,
};
static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_Main[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_Main,
  WEATHER_EVENING_RAIN_KIRAKIRA_Main,
};
static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_InitFadeOut[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_InitFadeOut,
  WEATHER_EVENING_RAIN_KIRAKIRA_InitFadeOut,
};
static WEATHER_TASK_FUNC_RESULT (*pEVENING_RAIN_TYPE_FadeOut[ EVENING_RAIN_TYPE_MAX ])( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID ) = 
{
  NULL,
  WEATHER_EVENING_RAIN_RAIN_FadeOut,
  WEATHER_EVENING_RAIN_KIRAKIRA_FadeOut,
};


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
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	NARC_field_weather_rain_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_NCER,			// OAM CELL
	NARC_field_weather_rain_NANR,			// OAM CELLANM
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
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	NARC_field_weather_rain_st_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_st_NCER,			// OAM CELL
	NARC_field_weather_rain_st_NANR,			// OAM CELLANM
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

// ���C�J�~
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_RAIKAMI = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_FRONT,		// BG���g�p���邩�H
	NARC_field_weather_rain_raikami_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_raikami_NCER,			// OAM CELL
	NARC_field_weather_rain_raikami_NANR,			// OAM CELLANM
  {
    {
      NARC_field_weather_rain_bg_nsbtx,		// BGTEX
      GX_TEXSIZE_S32,		// GXTexSizeS
      GX_TEXSIZE_T32,		// GXTexSizeT
      GX_TEXREPEAT_ST,		// GXTexRepeat
      GX_TEXFLIP_NONE,		// GXTexFlip
      GX_TEXFMT_PLTT4,		// GXTexFmt
      GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0
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
	sizeof(WEATHER_RAIKAMI_WORK),

	// �Ǘ��֐�
	WEATHER_RAIKAMI_Init,		// ������
	WEATHER_RAIKAMI_FadeIn,		// �t�F�[�h�C��
	WEATHER_RAIKAMI_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_RAIKAMI_Main,		// ���C������
	WEATHER_RAIKAMI_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_RAIKAMI_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_RAIKAMI_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_RAIKAMI_OBJ_Move,
};

// �����J�~
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_KAZAKAMI = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_ALL,		// BG���g�p���邩�H
	NARC_field_weather_kazakami_NCGR,			// OAM CG
	NARC_field_weather_kazakami_NCLR,			// OAM PLTT
	NARC_field_weather_kazakami_NCER,			// OAM CELL
	NARC_field_weather_kazakami_NANR,			// OAM CELLANM
  {
    {
      NARC_field_weather_kazakami_rain_nsbtx,		// BGTEX
      GX_TEXSIZE_S32,		// GXTexSizeS
      GX_TEXSIZE_T32,		// GXTexSizeT
      GX_TEXREPEAT_ST,		// GXTexRepeat
      GX_TEXFLIP_NONE,		// GXTexFlip
      GX_TEXFMT_PLTT4,		// GXTexFmt
      GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0
    },
    {
	    NARC_field_weather_kazakami_nsbtx,		// BGTEX
      GX_TEXSIZE_S32,		// GXTexSizeS
      GX_TEXSIZE_T32,		// GXTexSizeT
      GX_TEXREPEAT_ST,		// GXTexRepeat
      GX_TEXFLIP_NONE,		// GXTexFlip
      GX_TEXFMT_PLTT4,		// GXTexFmt
      GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_KAZAKAMI_WORK),

	// �Ǘ��֐�
	WEATHER_KAZAKAMI_Init,		// ������
	WEATHER_KAZAKAMI_FadeIn,		// �t�F�[�h�C��
	WEATHER_KAZAKAMI_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_KAZAKAMI_Main,		// ���C������
	WEATHER_KAZAKAMI_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_KAZAKAMI_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_KAZAKAMI_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_KAZAKAMI_OBJ_Move,
};


// �[��
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_EVENING_RAIN = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_FRONT,		// BG���g�p���邩�H
	NARC_field_weather_rain_st_NCGR,			// OAM CG
	NARC_field_weather_rain_NCLR,			// OAM PLTT
	NARC_field_weather_rain_st_NCER,			// OAM CELL
	NARC_field_weather_rain_st_NANR,			// OAM CELLANM
  {
    {
      NARC_field_weather_rain_bg_nsbtx,		// BGTEX
      GX_TEXSIZE_S32,		// GXTexSizeS
      GX_TEXSIZE_T32,		// GXTexSizeT
      GX_TEXREPEAT_ST,		// GXTexRepeat
      GX_TEXFLIP_NONE,		// GXTexFlip
      GX_TEXFMT_PLTT4,		// GXTexFmt
      GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0
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
	sizeof(WEATHER_EVENING_RAIN_WORK),

	// �Ǘ��֐�
	WEATHER_EVENING_RAIN_Init,		// ������
	WEATHER_EVENING_RAIN_FadeIn,		// �t�F�[�h�C��
	WEATHER_EVENING_RAIN_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_EVENING_RAIN_Main,		// ���C������
	WEATHER_EVENING_RAIN_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_EVENING_RAIN_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_EVENING_RAIN_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_EVENING_RAIN_OBJ_Move,
};




//----------------------------------------------------------------------------
/**
 *	@brief	������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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

      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_RAIN ) );

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
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_RAIN ) );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk );

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
static void WEATHER_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �X�p�[�N������
	WEATHER_PARK_Init( &p_local_wk->spark, WEATHER_SPARK_MODE_NORMAL );

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


	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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

      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_SPARK ) );

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
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_SPARK ) );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		���J���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SPARKRAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_SPARKRAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk );

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
static void WEATHER_SPARKRAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID )
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
 *	@brief  ���C�J�~�@�V�C  ������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �X�p�[�N������
	WEATHER_PARK_Init( &p_local_wk->spark, WEATHER_SPARK_MODE_RAIKAMI );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk, 
		WEATHER_RAIKAMI_ADD_START,	// obj�o�^��
		WEATHER_RAIKAMI_TIMING_MAX,// �o�^�^�C�~���O
		WEATHER_RAIKAMI_ADD_MAIN,
		WEATHER_RAIKAMI_TIMING_MIN,
		-WEATHER_RAIKAMI_TIMING_ADD,
		WEATHER_RAIKAMI_ADD_TIMING,
		WEATHER_RAIKAMI_ADD,
		WEATHER_RAIKAMI_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

  p_local_wk->wait      = 0;
  p_local_wk->seq      = 0;

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );


	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�J�~�@�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;
  BOOL result;
  BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );
  
  switch( p_local_wk->seq )
  {
  case WEATHER_RAIKAMI_FADEIN_SEQ_INIT:
    // ���C�g�ύX
    WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_RAIKAMI ) );

    p_local_wk->seq++;
    p_local_wk->wait = WEATHER_RAIKAMI_LIGHT_SPARK_WAIT;
    break;

  case WEATHER_RAIKAMI_FADEIN_SEQ_LIGHT_DARK:
    p_local_wk->wait--;
    if(p_local_wk->wait < 0)
    {
      p_local_wk->seq++;
    }
    break;

  case WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_00:
    // �h�f�J�C����炷�B�iLIGHT���Â����Ă���j
		WEATHER_TASK_LIGHT_StartColorFadeOneWay( p_wk, WEATHER_RAIKAMI_SPARK_DARK_COLOR, WEATHER_RAIKAMI_SPARK_DARK_SYNC );
    p_local_wk->wait = WEATHER_RAIKAMI_LIGHT_DARKSPARK_WAIT;
    p_local_wk->seq++;
    break;

  case WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_01:
    p_local_wk->wait --;
    if( p_local_wk->wait < 0 )
    {
      // �h�J�[���Ƃ���
		  WEATHER_TASK_LIGHT_StartColorFade( p_wk, WEATHER_RAIKAMI_SPARK_COLOR, WEATHER_RAIKAMI_SPARK_INSYNC, WEATHER_RAIKAMI_SPARK_OUTSYNC );
      
      // ��SE�Đ�
      WEATHER_TASK_PlaySnd( p_wk, WEATHER_SND_SE_SPARK );
      p_local_wk->seq++;
    }
    break;

  case WEATHER_RAIKAMI_FADEIN_SEQ_SPARK_02:
    if( WEATHER_TASK_LIGHT_IsColorFade( p_wk ) == FALSE )
    {
      WEATHER_TASK_FogFadeIn_Init( p_wk,
        WEATHER_FOG_SLOPE_DEFAULT, 
        WEATHER_FOG_DEPTH_DEFAULT + WEATHER_RAIKAMI_FOG_OFS, 
        WEATHER_RAIKAMI_FOG_TIMING,
        fog_cont );

      p_local_wk->seq++;
      p_local_wk->wait = WEATHER_RAIKAMI_LIGHT_FULL_RAIN_WAIT;
    }
    break;

  case WEATHER_RAIKAMI_FADEIN_SEQ_OBJADD:
    p_local_wk->wait--;
    if( p_local_wk->wait == 0 )
    {
      // ��
      WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	
      // BGON
      WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );
    }

	  result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s
	  fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );	// ���s
    if( result && fog_result )
    {
			return WEATHER_TASK_FUNC_RESULT_FINISH;
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  // �X�N���[������
  WEATHER_RAIKAMI_SCROLL_Main( p_wk, p_local_wk );

  return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�Ȃ�������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;
  BOOL result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_RAIKAMI_ADD_MAIN,	// obj�o�^��
		WEATHER_RAIKAMI_TIMING_MIN,// �o�^�^�C�~���O
		WEATHER_RAIKAMI_ADD_MAIN,
		WEATHER_RAIKAMI_TIMING_MIN,
		-WEATHER_RAIKAMI_TIMING_ADD,
		WEATHER_RAIKAMI_ADD_TIMING,
		WEATHER_RAIKAMI_ADD,
		WEATHER_RAIKAMI_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_RAIKAMI_OBJ_Add, WEATHER_RAIKAMI_NOFADE_OBJ_START_NUM, WEATHER_RAIKAMI_NOFADE_OBJ_START_DUST_NUM, WEATHER_RAIKAMI_NOFADE_OBJ_START_DUST_MOVE, heapID );

  // ��
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_RAIKAMI ) );

  // BGON
  WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	p_local_wk->obj_speed_change = (p_local_wk->obj_speed_change + 1) % (WEATHER_RAIKAMI_OBJ_MUL_CHG*WEATHER_RAIKAMI_OBJ_MUL_NUM);		// �J�o�^���ύX�J�E���^

	// �J�E���^��0�����ɂȂ�����J�o�^
	WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );


	// �X�p�[�N���C��
	WEATHER_PARK_Main( &p_local_wk->spark, p_wk );

  // �X�N���[������
  WEATHER_RAIKAMI_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g����
 */ 
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_RAIKAMI_TIMING_MAX,
			WEATHER_RAIKAMI_TIMING_ADD,
			WEATHER_RAIKAMI_ADD_END );
	
	p_local_wk->wait = WEATHER_RAIKAMI_FOG_START_END;	// �������t�H�O�p


	// ��
	WEATHER_TASK_StopLoopSnd( p_wk );	

  // BGOFF
  WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE, WEATHER_TASK_3DBG_FRONT );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g����
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_RAIKAMI_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	// �t�H�O����
	if(p_local_wk->wait > 0){
		p_local_wk->wait--;			// ���[�N���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->wait == 0 ){

			WEATHER_TASK_FogFadeOut_Init( p_wk,
					WEATHER_FOG_DEPTH_DEFAULT_START, 
					WEATHER_RAIKAMI_FOG_TIMING_END, fog_cont );
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
  WEATHER_RAIKAMI_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I������
 *
 *	@param	p_wk
 *	@param	fog_cont
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_RAIKAMI_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_RAIKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk );

	// �X�p�[�N������
	WEATHER_PARK_Exit( &p_local_wk->spark );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�J�~�@�I�u�W�F�@����
 */
//-----------------------------------------------------------------------------
static void WEATHER_RAIKAMI_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
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
		mat.x += obj_w[4];
		mat.y += obj_w[2];
	
		// �j�����邩�`�F�b�N
		if( (mat.y >= WEATHER_RAIKAMI_OBJ_END_Y) ){
      // �j��
      obj_w[3] = 1;
		}
		// ���W�ݒ�
		WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &mat );
		break;
	case 1:		// �j��
		WEATHER_TASK_DeleteObj( p_wk );
		break;
	}		

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�J�~�I�u�W�F�@�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_RAIKAMI_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_RAIKAMI_WORK*	p_local_wk;	// �V�X�e�����[�N
	int	err;	// �␳�l
	s32* obj_w;	// �I�u�W�F�N�g���[�N
	int speed_m;	// �X�s�[�h�ɂ�����l�e�[�u���̗v�f��
	int frame;		// �t���[����
	static const int WEATHER_RAIKAMI_OBJ_MUL[ WEATHER_RAIKAMI_OBJ_MUL_NUM ] = {10, 10, 13, 10, 16};
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
		frame = rand%3;	// �J�̎��
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );

    GF_ASSERT( (p_local_wk->obj_speed_change /WEATHER_RAIKAMI_OBJ_MUL_CHG) < WEATHER_RAIKAMI_OBJ_MUL_NUM );

		obj_w[4] = WEATHER_RAIKAMI_SPEED_X * (frame + 1) + ((rand % WEATHER_RAIKAMI_SPPED_RND));
		obj_w[2] = WEATHER_RAIKAMI_SPEED_Y * (frame + 1) + ((rand % WEATHER_RAIKAMI_SPPED_RND));
		obj_w[4] = (obj_w[4] * WEATHER_RAIKAMI_OBJ_MUL[p_local_wk->obj_speed_change /WEATHER_RAIKAMI_OBJ_MUL_CHG]) / WEATHER_RAIKAMI_OBJ_MUL_ONE;
		obj_w[2] = (obj_w[2] * WEATHER_RAIKAMI_OBJ_MUL[p_local_wk->obj_speed_change /WEATHER_RAIKAMI_OBJ_MUL_CHG]) / WEATHER_RAIKAMI_OBJ_MUL_ONE;
		
		obj_w[3] = 0;			// �j���A�j���t���O
		
		// ���W��ݒ�
		{
			mat.x = WEATHER_RAIKAMI_START_X + (rand % WEATHER_RAIKAMI_START_X_MAX);
			mat.y = WEATHER_RAIKAMI_START_Y + (rand % WEATHER_RAIKAMI_START_Y_MAX);

			WEATHER_OBJ_WORK_SetPosNoTurn( add_obj, &mat );
		}
		
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  �炢���݁@�X�N���[������
 */
//-----------------------------------------------------------------------------
static void WEATHER_RAIKAMI_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_RAIKAMI_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
	p_wk->scroll = (p_wk->scroll + 9) % 256;
	WEATHER_TASK_3DBG_SetScrollX( p_sys, (-p_wk->scroll) - x, WEATHER_TASK_3DBG_FRONT );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->scroll) + y, WEATHER_TASK_3DBG_FRONT );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk, 
		WEATHER_KAZAKAMI_ADD_START,	// obj�o�^��
		WEATHER_KAZAKAMI_TIMING_MAX,// �o�^�^�C�~���O
		WEATHER_KAZAKAMI_ADD_MAIN,
		WEATHER_KAZAKAMI_TIMING_MIN,
		-WEATHER_KAZAKAMI_TIMING_ADD,
		WEATHER_KAZAKAMI_ADD_TIMING,
		WEATHER_KAZAKAMI_ADD,
		WEATHER_KAZAKAMI_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

  // ��SE�Đ�����������
  WEATHER_KAZAKAMI_WINDRAIN_SCENE_InitWindSePlay( p_local_wk );
  
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;
  BOOL result;
  BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  switch( p_local_wk->seq )
  {
  case KAZAKAMI_FADEIN_SEQ_INIT:
    // ���C�g�ύX
    WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_KAZAKAMI ) );

    p_local_wk->wait = KAZAKAMI_FADEIN_WIND_WAIT;
    p_local_wk->seq++;
    break;
    
  case KAZAKAMI_FADEIN_SEQ_LIGHT_DARK:
    p_local_wk->wait--;
    if(p_local_wk->wait <= 0)
    {
      // �J���J�n����
      WEATHER_KAZAKAMI_WindStartRain( p_wk, p_local_wk, fog_cont, TRUE );

      // ���������A�J�̎��ԌŒ�
      p_local_wk->wind_scene_count_max = KAZAKAMI_FIRST_RAIN_TIME;
        
      // BG�J�n�܂ł̃E�G�C�g
      p_local_wk->wait = KAZAKAMI_FADEIN_BGSTART_WAIT;
      p_local_wk->seq++;
    }
    break;

  case KAZAKAMI_FADEIN_SEQ_WIND02:
    // �����Ď�
    p_local_wk->wait --;
    if( p_local_wk->wait == 0 )
    {
      // ��
      WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_KAZAKAMI_NORMAL_RAIN );	
      // BGON
      WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );
      //FOGON
      WEATHER_TASK_FogFadeIn_Init( p_wk,
        WEATHER_FOG_SLOPE_DEFAULT, 
        WEATHER_FOG_DEPTH_DEFAULT + WEATHER_KAZAKAMI_FOG_OFS_NORMAL, 
        WEATHER_KAZAKAMI_FOG_TIMING,
        fog_cont );
    }
    
	  result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s
	  fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );	// ���s
    if( (result==TRUE) && (fog_result==TRUE) && (p_local_wk->wait <= 0) )
    {
			return WEATHER_TASK_FUNC_RESULT_FINISH;
    }
    break;
    
  default:
    GF_ASSERT(0);
    break;
  }
  
  // ���̊Ǘ�
  WEATHER_KAZAKAMI_WindControl( p_wk, p_local_wk, fog_cont, FALSE );
  
  // �X�N���[������
  WEATHER_KAZAKAMI_SCROLL_Main( p_wk, p_local_wk );

  return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_KAZAKAMI_ADD_MAIN,	// obj�o�^��
		WEATHER_KAZAKAMI_TIMING_MIN,// �o�^�^�C�~���O
		WEATHER_KAZAKAMI_ADD_MAIN,
		WEATHER_KAZAKAMI_TIMING_MIN,
		-WEATHER_KAZAKAMI_TIMING_ADD,
		WEATHER_KAZAKAMI_ADD_TIMING,
		WEATHER_KAZAKAMI_ADD,
		WEATHER_KAZAKAMI_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_KAZAKAMI_OBJ_Add, WEATHER_KAZAKAMI_NOFADE_OBJ_START_NUM, WEATHER_KAZAKAMI_NOFADE_OBJ_START_DUST_NUM, WEATHER_KAZAKAMI_NOFADE_OBJ_START_DUST_MOVE, heapID );

  // ��
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_KAZAKAMI_NORMAL_RAIN );	
  WEATHER_KAZAKAMI_WindStartRain( p_wk, p_local_wk, fog_cont, FALSE );

  // BGON
  WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );

  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_KAZAKAMI ) );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �J�E���^��0�����ɂȂ�����J�o�^
	WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );

  // ���̊Ǘ�
  WEATHER_KAZAKAMI_WindControl( p_wk, p_local_wk, fog_cont, FALSE );

  // �X�N���[������
  WEATHER_KAZAKAMI_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_KAZAKAMI_TIMING_MAX,
			WEATHER_KAZAKAMI_TIMING_ADD,
			WEATHER_KAZAKAMI_ADD_END );
	
	// ��
	WEATHER_TASK_StopLoopSnd( p_wk );	

  WEATHER_TASK_FogFadeOut_Init( p_wk,
      WEATHER_FOG_DEPTH_DEFAULT_START, 
      WEATHER_KAZAKAMI_FOG_TIMING_END, fog_cont );

  // BGOFF
  WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE, WEATHER_TASK_3DBG_FRONT );
  WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE, WEATHER_TASK_3DBG_BACK );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
  WEATHER_KAZAKAMI_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// �I�u�W�F�N�g�t�F�[�h
	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	
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

  // ���̊Ǘ�
  WEATHER_KAZAKAMI_WindControl( p_wk, p_local_wk, fog_cont, TRUE  );

  // �X�N���[������
  WEATHER_KAZAKAMI_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�I��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_KAZAKAMI_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_KAZAKAMI_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�I�u�W�F����
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	GFL_CLWK* p_clwk;
  KAZAKAMI_OBJ_WK* p_obj_wk;

  static void (*WEATHER_KAZAKAMI_OBJ_MOVE_TBL[KAZAKAMI_OBJ_TYPE_NUM])( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ) = 
  {
    WEATHER_KAZAKAMI_OBJ_WIND_Move,
    WEATHER_KAZAKAMI_OBJ_RAIN_Move,
    WEATHER_KAZAKAMI_OBJ_WINDRAIN_Move,
  };

	p_obj_wk = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

  WEATHER_KAZAKAMI_OBJ_MOVE_TBL[ p_obj_wk->type ]( p_wk, p_obj_wk->cp_parent, (void*)&p_obj_wk->objwk, p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�I�u�W�F�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_KAZAKAMI_WORK*	p_local_wk;	// �V�X�e�����[�N
  KAZAKAMI_OBJ_WK* p_obj_wk;
	GFL_CLWK* p_clwk;

  static void (*WEATHER_KAZAKAMI_OBJ_ADD_TBL[KAZAKAMI_OBJ_TYPE_NUM])( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk ) = 
  {
    WEATHER_KAZAKAMI_OBJ_WIND_Add,
    WEATHER_KAZAKAMI_OBJ_RAIN_Add,
    WEATHER_KAZAKAMI_OBJ_WINDRAIN_Add,
  };
  u32 rand = GFUser_GetPublicRand(0);

	// ���[�U���[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  if( (p_local_wk->wind_scene_type == KAZAKAMI_WIND_SCENE_NONE) )
  {
    return ;
  }

  // �o�^���𒲐�
  if( (p_local_wk->wind_scene_type == KAZAKAMI_WIND_SCENE_WINDRAIN) )
  {
    if( (rand % KAZAKAMI_WIND_SCENE_WIND_ADD_CUT_NUM) != 0 )
    {
      return ;
    }
  }
	
	// num���I�u�W�F�N�g��o�^
	for(i=0;i<num;i++){

		add_obj = WEATHER_TASK_CreateObj( p_wk, heapID );		// �o�^
		if(add_obj == NULL){			// ���s������I��
			break;
		}
		p_obj_wk = WEATHER_OBJ_WORK_GetWork( add_obj );		// �I�u�W�F���[�N�쐬
		p_clwk	= WEATHER_OBJ_WORK_GetClWk( add_obj );

    GF_ASSERT( sizeof(KAZAKAMI_OBJ_WK) < WEATHER_OBJ_WORK_USE_WORKSIZE );

    p_obj_wk->cp_parent = p_local_wk;
    switch( p_local_wk->wind_scene_type )
    {
    case KAZAKAMI_WIND_SCENE_RAIN:
      p_obj_wk->type = KAZAKAMI_OBJ_RAIN;
      break;
    case KAZAKAMI_WIND_SCENE_WINDRAIN:
      if( (rand % 100) <= KAZAKAMI_WIND_WINDRAIN_PARCENT )
      {
        p_obj_wk->type = KAZAKAMI_OBJ_WIND_RAIN;
      }
      else
      {
        p_obj_wk->type = KAZAKAMI_OBJ_WIND;
      }
      break;

    case KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN:
    case KAZAKAMI_WIND_SCENE_WINDRAIN_TO_RAIN:
      if( (rand % 100) <= KAZAKAMI_RAIN_RAIN_TO_WINDRAIN_PARCENT )
      {
        p_obj_wk->type = KAZAKAMI_OBJ_RAIN;
      }
      else
      {
        p_obj_wk->type = KAZAKAMI_OBJ_WIND_RAIN;
      }
      break;

    default:
      GF_ASSERT(0);
      break;
    }

    // �o�^���
    WEATHER_KAZAKAMI_OBJ_ADD_TBL[ p_obj_wk->type ]( add_obj, p_obj_wk->cp_parent, (void*)&p_obj_wk->objwk, p_clwk );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���\���I�u�W�F�̓���
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_WIND_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_WIND_WK* p_objwk = p_obj;
  fx32 last_move_x, last_move_y;
  fx32 move_x, move_y;
  u16 rotate;
  s32 wind_add_x, wind_add_y;

  // ���������Ȃ��Ȃ�����A�O�̃X�s�[�h�Ői��
  if( cp_parent->wind_add_x == 0 )
  {
    wind_add_x = p_objwk->last_speed_x;
    wind_add_y = p_objwk->last_speed_y;
  }
  else
  {
    wind_add_x = cp_parent->wind_add_x;
    wind_add_y = cp_parent->wind_add_y;
    p_objwk->last_speed_x = wind_add_x;
    p_objwk->last_speed_y = wind_add_y;
  }

  // ���W�擾
	WEATHER_OBJ_WORK_GetPos( p_wk, &pos );

  // �ʏ�X�s�[�h�{��
  p_objwk->pos_x += p_objwk->speed_x + wind_add_x;
  p_objwk->pos_y += p_objwk->speed_y + wind_add_y;
  pos.x = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_x);
  pos.y = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_y);

  // �j���Ď�
  if( (pos.x >= KAZAKAMI_OBJ_WIND_DELETE_X) || (pos.y >= KAZAKAMI_OBJ_WIND_DELETE_Y) )
  {
    WEATHER_TASK_DeleteObj( p_wk );
    return ;
  }


  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���\���I�u�W�F�̒ǉ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_WIND_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_WIND_WK* p_objwk = p_obj;
  u32 rand = GFUser_GetPublicRand(0);

  p_objwk->speed_x = KAZAKAMI_OBJ_WIND_SPEED_X_MIN + (rand % KAZAKAMI_OBJ_WIND_SPEED_X_RND);
  p_objwk->speed_y = KAZAKAMI_OBJ_WIND_SPEED_Y_MIN + (rand % KAZAKAMI_OBJ_WIND_SPEED_Y_RND);

  p_objwk->last_speed_x = cp_parent->wind_add_x;
  p_objwk->last_speed_y = cp_parent->wind_add_y;

  // ���W
  pos.x = KAZAKAMI_OBJ_WIND_ADD_X + (rand % KAZAKAMI_OBJ_WIND_ADD_X_RND);
  pos.y = KAZAKAMI_OBJ_WIND_ADD_Y_MIN + (rand % KAZAKAMI_OBJ_WIND_ADD_Y_RND);
  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );

  p_objwk->pos_x = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.x );
  p_objwk->pos_y = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.y );

  GFL_CLACT_WK_SetAnmSeq( p_clwk, 0 );

  GFL_CLACT_WK_SetAutoAnmFlag( p_clwk, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( p_clwk, 2<<FX32_SHIFT );

}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�\���I�u�W�F�̓���
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_RAIN_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_RAIN_WK* p_objwk = p_obj;

  // ���W�擾
	WEATHER_OBJ_WORK_GetPos( p_wk, &pos );

  // �ʏ�X�s�[�h�{��
  p_objwk->pos_x += p_objwk->speed_x + cp_parent->wind_add_x;
  p_objwk->pos_y += p_objwk->speed_y + cp_parent->wind_add_y;

    
  pos.x = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_x);
  pos.y = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_y);

  // �j���Ď�
  if( (pos.x >= KAZAKAMI_OBJ_RAIN_DELETE_X) || (pos.y >= KAZAKAMI_OBJ_RAIN_DELETE_Y) )
  {
    WEATHER_TASK_DeleteObj( p_wk );
    return ;
  }


  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�\���I�u�W�F�̒ǉ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_RAIN_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_RAIN_WK* p_objwk = p_obj;
  u32 rand = GFUser_GetPublicRand(0);

  p_objwk->speed_x = KAZAKAMI_OBJ_RAIN_SPEED_X_MIN + (rand % KAZAKAMI_OBJ_RAIN_SPEED_RND);
  p_objwk->speed_y = KAZAKAMI_OBJ_RAIN_SPEED_Y_MIN + ((rand % KAZAKAMI_OBJ_RAIN_SPEED_RND) * KAZAKAMI_OBJ_RAIN_SPEED_Y_RND_MUL);

  // ���W
  pos.x = KAZAKAMI_OBJ_RAIN_ADD_X_MIN + (rand % KAZAKAMI_OBJ_RAIN_ADD_X_RND);
  pos.y = KAZAKAMI_OBJ_RAIN_ADD_Y_MIN + (rand % KAZAKAMI_OBJ_RAIN_ADD_Y_RND);
  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );

  p_objwk->pos_x = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.x );
  p_objwk->pos_y = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.y );

  GFL_CLACT_WK_SetAnmSeq( p_clwk, 1 );

  GFL_CLACT_WK_SetAutoAnmFlag( p_clwk, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( p_clwk, 2<<FX32_SHIFT );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���J�\���I�u�W�F�̓���
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_WINDRAIN_Move( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_WINDRAIN_WK* p_objwk = p_obj;

  // ���W�擾
	WEATHER_OBJ_WORK_GetPos( p_wk, &pos );

  // �ʏ�X�s�[�h�{��
  if( cp_parent->wind_add_x != 0 )
  {
    p_objwk->pos_x += p_objwk->speed_x + cp_parent->wind_add_x;
    p_objwk->pos_y += p_objwk->speed_y + cp_parent->wind_add_y;
    pos.x = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_x);
    pos.y = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_y);
    
    p_objwk->last_speed_x = p_objwk->speed_x + cp_parent->wind_add_x;
    p_objwk->last_speed_y = p_objwk->speed_y + cp_parent->wind_add_y;
  }
  else
  {
    p_objwk->pos_x += p_objwk->last_speed_x;
    p_objwk->pos_y += p_objwk->last_speed_y;
    pos.x = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_x);
    pos.y = KAZAKAMI_OBJ_WIND_SPEED_GET(p_objwk->pos_y);
  }

  // �j���Ď�
  if( (pos.x >= KAZAKAMI_OBJ_WIND_DELETE_X) || (pos.y >= KAZAKAMI_OBJ_WIND_DELETE_Y) )
  {
    WEATHER_TASK_DeleteObj( p_wk );
    return ;
  }

  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���J�\���I�u�W�F�̒ǉ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_OBJ_WINDRAIN_Add( WEATHER_OBJ_WORK* p_wk, const WEATHER_KAZAKAMI_WORK* cp_parent, void* p_obj, GFL_CLWK* p_clwk )
{
  GFL_CLACTPOS pos;
  KAZAKAMI_OBJ_WINDRAIN_WK* p_objwk = p_obj;
  u32 rand = GFUser_GetPublicRand(0);

  p_objwk->speed_x = KAZAKAMI_OBJ_WINDRAIN_SPEED_X_MIN + (rand % KAZAKAMI_OBJ_WINDRAIN_SPEED_X_RND);
  p_objwk->speed_y = KAZAKAMI_OBJ_WINDRAIN_SPEED_Y_MIN + (rand % KAZAKAMI_OBJ_WINDRAIN_SPEED_Y_RND);

  p_objwk->last_speed_x = p_objwk->speed_x + cp_parent->wind_add_x;
  p_objwk->last_speed_y = p_objwk->speed_y + cp_parent->wind_add_y;

  // ���W
  pos.x = KAZAKAMI_OBJ_WINDRAIN_ADD_X_MIN + (rand % KAZAKAMI_OBJ_WINDRAIN_ADD_X_RND);
  pos.y = KAZAKAMI_OBJ_WINDRAIN_ADD_Y_MIN + (rand % KAZAKAMI_OBJ_WINDRAIN_ADD_Y_RND);
  WEATHER_OBJ_WORK_SetPosNoTurn( p_wk, &pos );

  p_objwk->pos_x = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.x );
  p_objwk->pos_y = KAZAKAMI_OBJ_WIND_SPEED_SET( pos.y );

  GFL_CLACT_WK_SetAnmSeq( p_clwk, 2 );

  GFL_CLACT_WK_SetAutoAnmFlag( p_clwk, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( p_clwk, 2<<FX32_SHIFT );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����̊J�n
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_WindStartRain( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, BOOL fadeout )
{
  u32 rand = GFUser_GetPublicRand(0);
  // WINDRAIN��
  p_wk->wind_scene_type   = KAZAKAMI_WIND_SCENE_RAIN;
  p_wk->wind_scene_count_max  = KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_MIN + (rand % KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_RND);
  p_wk->wind_scene_count = 0;

  // FOG�𓮂���
  if( fadeout == FALSE )
  {
    WEATHER_TASK_FogFadeIn_Init( p_sys,
      WEATHER_FOG_SLOPE_DEFAULT, 
      WEATHER_FOG_DEPTH_DEFAULT + WEATHER_KAZAKAMI_FOG_OFS_NORMAL, 
      WEATHER_KAZAKAMI_FOG_TIMING,
      fog_cont );

    WEATHER_TASK_3DBG_SetVisible( p_sys, FALSE, WEATHER_TASK_3DBG_BACK );

    // SE�̕ύX
    //PMSND_PlaySE( WEATHER_SND_SE_KAZAKAMI_NORMAL_MIDDLE_WIND );
    WEATHER_TASK_PlayLoopSnd( p_sys, WEATHER_SND_SE_KAZAKAMI_NORMAL_RAIN );	
  }


}


//----------------------------------------------------------------------------
/**
 *	@brief  ���Ǘ�
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_WindControl( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, BOOL fadeout )
{
  // scene_type�̕ύX
  switch( p_wk->wind_scene_type )
  {
  case  KAZAKAMI_WIND_SCENE_RAIN:
    p_wk->wind_scene_count++;
    WEATHER_KAZAKAMI_WindSePlay( p_sys, p_wk, fog_cont, WEATHER_SND_SE_KAZAKAMI_NORMAL_MIDDLE_WIND );
    if( p_wk->wind_scene_count >= p_wk->wind_scene_count_max )
    {
        // RAIN_TO_WINDRAIN��
        p_wk->wind_scene_type   = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN;
        p_wk->wind_scene_count_max  = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_TIME_MIN;
        p_wk->wind_scene_count = 0;

        // FOG�𓮂���
        if( fadeout == FALSE )
        {
          WEATHER_TASK_FogFadeIn_Init( p_sys,
            WEATHER_FOG_SLOPE_DEFAULT, 
            WEATHER_FOG_DEPTH_DEFAULT + WEATHER_KAZAKAMI_FOG_OFS, 
            WEATHER_KAZAKAMI_FOG_TIMING,
            fog_cont );

          // SE�̕ύX
          //PMSND_PlaySE( WEATHER_SND_SE_KAZAKAMI_NORMAL_MIDDLE_WIND );
          WEATHER_TASK_PlayLoopSnd( p_sys, WEATHER_SND_SE_KAZAKAMI_HIGH_RAIN );	
          
        }
    }
    break;
    
  case  KAZAKAMI_WIND_SCENE_WINDRAIN:
    {
      p_wk->wind_scene_count++;
      //WEATHER_KAZAKAMI_WindSePlay( p_sys, p_wk, fog_cont, WEATHER_SND_SE_KAZAKAMI_MIDDLE_HIGH_WIND );
      WEATHER_KAZAKAMI_WindSePlay( p_sys, p_wk, fog_cont, WEATHER_SND_SE_KAZAKAMI_NORMAL_MIDDLE_WIND );
      WEATHER_KAZAKAMI_WINDRAIN_SCENE_WindSePlay( p_sys, p_wk, fog_cont );
      if( p_wk->wind_scene_count >= p_wk->wind_scene_count_max )
      {
        // WINDRAIN_TO_RAIN��
        p_wk->wind_scene_type   = KAZAKAMI_WIND_SCENE_WINDRAIN_TO_RAIN;
        p_wk->wind_scene_count_max  = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_TIME_MIN;
        p_wk->wind_scene_count = 0;

        // BG������
        if( fadeout == FALSE )
        {
          // 3DBG�̊Ǘ�
          WEATHER_TASK_3DBG_SetVisible( p_sys, FALSE, WEATHER_TASK_3DBG_BACK );
          WEATHER_TASK_3DBG_SetScaleX( p_sys, FX32_ONE, WEATHER_TASK_3DBG_FRONT );
          WEATHER_TASK_3DBG_SetScaleY( p_sys, FX32_ONE, WEATHER_TASK_3DBG_FRONT );

          // SE�̕ύX
          //PMSND_PlaySE( WEATHER_SND_SE_KAZAKAMI_MIDDLE_HIGH_WIND );
          //WEATHER_TASK_PlayLoopSnd( p_sys, WEATHER_SND_SE_KAZAKAMI_MIDDLE_RAIN );	
        
        }

      }
    }
    break;

  case KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN:
    {
      p_wk->wind_scene_count++;
      //WEATHER_KAZAKAMI_WindSePlay( p_wk, fog_cont, WEATHER_SND_SE_KAZAKAMI_MIDDLE_HIGH_WIND );
      if( p_wk->wind_scene_count >= p_wk->wind_scene_count_max )
      {
        u32 rand = GFUser_GetPublicRand(0);
        // WINDRAIN��
        p_wk->wind_scene_type   = KAZAKAMI_WIND_SCENE_WINDRAIN;
        p_wk->wind_scene_count_max  = KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_MIN + (rand % KAZAKAMI_WIND_SCENE_WINDRAIN_TIME_RND);
        p_wk->wind_scene_count = 0;

        // BG���o��
        if( fadeout == FALSE )
        {
          // 3DBG�̊Ǘ�
          WEATHER_TASK_3DBG_SetVisible( p_sys, TRUE, WEATHER_TASK_3DBG_BACK );
          WEATHER_TASK_3DBG_SetScaleX( p_sys, FX32_HALF, WEATHER_TASK_3DBG_FRONT );
          WEATHER_TASK_3DBG_SetScaleY( p_sys, FX32_ONE + FX32_HALF, WEATHER_TASK_3DBG_FRONT );

          // SE�̕ύX
          //PMSND_PlaySE( WEATHER_SND_SE_KAZAKAMI_MIDDLE_HIGH_WIND );
          //WEATHER_TASK_PlayLoopSnd( p_sys, WEATHER_SND_SE_KAZAKAMI_HIGH_RAIN );	
        }
      }
    }
    break;
    
  case KAZAKAMI_WIND_SCENE_WINDRAIN_TO_RAIN:
    {
      p_wk->wind_scene_count++;
      if( p_wk->wind_scene_count >= p_wk->wind_scene_count_max )
      {
        WEATHER_KAZAKAMI_WindStartRain( p_sys, p_wk, fog_cont, fadeout );
      }
    }
    break;

  case KAZAKAMI_WIND_SCENE_NONE:
    break;
    
  default:
    GF_ASSERT(0);
    break;
  }
  
  
  // ���̐ݒ�
  switch( p_wk->wind_scene_type )
  {
  case  KAZAKAMI_WIND_SCENE_RAIN:
    if( (p_wk->wind_scene_count_max - p_wk->wind_scene_count) >= KAZAKAMI_WIND_SCENE_RAIN_WIND_ON_TIME )
    {
      p_wk->wind_add_x = KAZAKAMI_WIND_SCENE_RAIN_WIND_SPEED_X;
      p_wk->wind_add_y = KAZAKAMI_WIND_SCENE_RAIN_WIND_SPEED_Y;
    }
    else
    {
      p_wk->wind_add_x = KAZAKAMI_WIND_SCENE_RAIN_WINDON_SPEED_X;
      p_wk->wind_add_y = KAZAKAMI_WIND_SCENE_RAIN_WINDON_SPEED_Y;
    }
    break;
    
  case  KAZAKAMI_WIND_SCENE_WINDRAIN:
    {
      u32 idx;

      idx = ((p_wk->wind_scene_count%KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_CHANGE_TIME) * KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_TBL_NUM) / KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_CHANGE_TIME;
      GF_ASSERT( idx < KAZAKAMI_WIND_SCENE_WINDRAIN_WIND_SPEED_TBL_NUM );
      p_wk->wind_add_x = KAZAKAMI_WIND_SCENE_RAIN_RAINWIND_SPEED_TBL[idx].x;
      p_wk->wind_add_y = KAZAKAMI_WIND_SCENE_RAIN_RAINWIND_SPEED_TBL[idx].y;

      idx = ((p_wk->wind_scene_count%KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_CHANGE_TIME) * KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_NUM) / KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_CHANGE_TIME;
      GF_ASSERT( idx < KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_NUM );
      p_wk->wind_bg_back_scale_x = KAZAKAMI_WIND_SCENE_RAIN_WIND_BG_BACK_SCALE_TBL[idx];
    }
    break;


  case KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN:
  case KAZAKAMI_WIND_SCENE_WINDRAIN_TO_RAIN:
    p_wk->wind_add_x = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SPEED_X;
    p_wk->wind_add_y = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SPEED_Y;
    p_wk->wind_bg_back_scale_x = KAZAKAMI_WIND_SCENE_RAIN_TO_WINDRAIN_WIND_SCALE_X;
    break;

  case KAZAKAMI_WIND_SCENE_NONE:
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�U�J�~�@�X�N���[������
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
  if(p_wk->wind_add_x != 0 )
  {
  	p_wk->scroll_x = (p_wk->scroll_x + 9) % 256;
	  p_wk->scroll_y = (p_wk->scroll_y + 3) % 256;
  	p_wk->scroll_back_x = (p_wk->scroll_back_x + 9) % 256;
	  p_wk->scroll_back_y = (p_wk->scroll_back_y + 2) % 256;
  }
  else
  {
  	p_wk->scroll_x = (p_wk->scroll_x + 3) % 256;
	  p_wk->scroll_y = (p_wk->scroll_y + 8) % 256;
  }

	WEATHER_TASK_3DBG_SetScrollX( p_sys, (-p_wk->scroll_x) - x, WEATHER_TASK_3DBG_FRONT );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->scroll_y) + y, WEATHER_TASK_3DBG_FRONT );

	WEATHER_TASK_3DBG_SetScrollX( p_sys, (-p_wk->scroll_back_x) - x, WEATHER_TASK_3DBG_BACK );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->scroll_back_y) + y, WEATHER_TASK_3DBG_BACK );

  {
    WEATHER_TASK_3DBG_SetScaleX( p_sys, p_wk->wind_bg_back_scale_x, WEATHER_TASK_3DBG_BACK );
    WEATHER_TASK_3DBG_SetScaleY( p_sys, p_wk->wind_bg_back_scale_x, WEATHER_TASK_3DBG_BACK );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�؂�ւ������܂������߂̕�SE���Đ�
 *
 *	@param	cp_wk
 *	@param	fog_cont
 *	@param	se_no 
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_WindSePlay( WEATHER_TASK* p_sys, const WEATHER_KAZAKAMI_WORK* cp_wk, BOOL fog_cont, u16 se_no )
{
  if( fog_cont )
  {
    if( cp_wk->wind_scene_count == (cp_wk->wind_scene_count_max - KAZAKAMI_WIND_SCENE_CHANGE_SE_PLAY_TIMING) )
    {
      WEATHER_TASK_PlaySnd( p_sys, se_no );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��SE�Đ������@������
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_WINDRAIN_SCENE_InitWindSePlay( WEATHER_KAZAKAMI_WORK* p_wk )
{
  p_wk->wind_se_count = 0;
  p_wk->wind_se_count_max = KAZAKAMI_WIND_SE_LOOP_TIME_MIN + GFUser_GetPublicRand( KAZAKAMI_WIND_SE_LOOP_TIME_DIF );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �\�����̕��Đ�����
 *
 *	@param	p_wk
 *	@param	fog_cont
 *	@param	se_no 
 */
//-----------------------------------------------------------------------------
static void WEATHER_KAZAKAMI_WINDRAIN_SCENE_WindSePlay( WEATHER_TASK* p_sys, WEATHER_KAZAKAMI_WORK* p_wk, BOOL fog_cont )
{
  if( fog_cont )
  {
    if( p_wk->wind_scene_count <= (p_wk->wind_scene_count_max - KAZAKAMI_WIND_SE_LOOP_TIME - KAZAKAMI_WIND_SCENE_CHANGE_SE_PLAY_TIMING) )
    {
      p_wk->wind_se_count ++;
      if( p_wk->wind_se_count >= p_wk->wind_se_count_max )
      {
        
        // ���Ă��Ȃ��Ƃ��ɖ炷
        if( PMSND_CheckPlaySE_byPlayerID( PMSND_GetSE_DefaultPlayerID(WEATHER_SND_SE_KAZAKAMI_NORMAL_MIDDLE_WIND) ) == FALSE )
        {
          u32 vol = KAZAKAMI_WIND_SE_LOOP_VOLUME_MIN + GFUser_GetPublicRand( KAZAKAMI_WIND_SE_LOOP_VOLUME_DIF );
        
          WEATHER_TASK_PlaySndVol( p_sys, WEATHER_SND_SE_KAZAKAMI_MIDDLE_HIGH_WIND, vol );
          WEATHER_KAZAKAMI_WINDRAIN_SCENE_InitWindSePlay( p_wk );
        }
        else
        {
          // ������SE�Đ�����ƁA�����`�����l�������I�[�o�[����̂ŁA
          // �J�E���^�̒l�𒲐�
          p_wk->wind_se_count = p_wk->wind_se_count_max - KAZAKAMI_WIND_SE_LOOP_NEED_TIME;
        }
      }
    }
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �[��  ������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
  
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // ���������̎��ԓ��`�F�b�N
  p_local_wk->time_in = WEATHER_EVENING_RAIN_CheckTimeType_Init( p_wk );

  if( pEVENING_RAIN_TYPE_Init[ p_local_wk->time_in ] ){
    return pEVENING_RAIN_TYPE_Init[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[��  �t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // �t�F�[�h�C��
  if( pEVENING_RAIN_TYPE_FadeIn[ p_local_wk->time_in ] ){
    return pEVENING_RAIN_TYPE_FadeIn[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }

	return WEATHER_TASK_FUNC_RESULT_FINISH;

}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  if( pEVENING_RAIN_TYPE_NoFade[ p_local_wk->time_in ] ){
    return pEVENING_RAIN_TYPE_NoFade[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
  int time_type;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // ���ԑт̕ω��`�F�b�N
  time_type = WEATHER_EVENING_RAIN_CheckTimeType_Main( p_wk, p_local_wk->time_in );

  if( p_local_wk->time_in != time_type ){
    // �܂��A���̂��̂��I��点��B
    if( (p_local_wk->seq == EVENING_RAIN_SEQ_ON) ||
        (p_local_wk->seq == EVENING_RAIN_SEQ_KIRAKIRA_ON) ){
      if( pEVENING_RAIN_TYPE_InitFadeOut[p_local_wk->time_in] ){
        pEVENING_RAIN_TYPE_InitFadeOut[p_local_wk->time_in]( p_wk, fog_cont, heapID );
        TOMOYA_Printf( "evening rain fade out %d\n", p_local_wk->time_in );
      }
    }

    // �I����Ă�����A���̎��ԑѓV�C�ɂ���
    if( p_local_wk->seq == EVENING_RAIN_SEQ_NONE ){
      p_local_wk->time_in = time_type;
    
      if( pEVENING_RAIN_TYPE_Init[p_local_wk->time_in] ){
        pEVENING_RAIN_TYPE_Init[p_local_wk->time_in]( p_wk, fog_cont, heapID );
        TOMOYA_Printf( "evening rain init %d\n", p_local_wk->time_in );
      }
    }
  }

  // ���̎��ԑт̃��C�����񂷁B
  if( pEVENING_RAIN_TYPE_Main[ p_local_wk->time_in ] ){
    pEVENING_RAIN_TYPE_Main[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }
  
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  if( pEVENING_RAIN_TYPE_InitFadeOut[ p_local_wk->time_in ] ){
    return pEVENING_RAIN_TYPE_InitFadeOut[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  if( pEVENING_RAIN_TYPE_FadeOut[ p_local_wk->time_in ] ){
    return pEVENING_RAIN_TYPE_FadeOut[ p_local_wk->time_in ]( p_wk, fog_cont, heapID );
  }
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	// ���C�g����
	WEATHER_TASK_LIGHT_Back( p_wk );

	// �X�p�[�N������
	WEATHER_PARK_Exit( &p_local_wk->spark );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�I�u�W�F�N�g����
 */
//-----------------------------------------------------------------------------
static void WEATHER_EVENING_RAIN_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	int i;
	GFL_CLACTPOS mat;
	GFL_CLWK* p_clwk;
	s32* obj_w;

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	WEATHER_OBJ_WORK_GetPos( p_wk, &mat );

  if(obj_w[5] == EVENING_RAIN_OBJ_TYPE_NORMAL){
    
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
  else
  {
    // ���ԕ������������
    obj_w[0]--;
    if(obj_w[0] <= 0){
      WEATHER_TASK_DeleteObj( p_wk );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �[�� �@�I�u�W�F�N�g�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_EVENING_RAIN_OBJ_Add( WEATHER_TASK* p_wk, int num, HEAPID heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_EVENING_RAIN_WORK*	p_local_wk;	// �V�X�e�����[�N
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

    // �I�u�W�F�^�C�v
    obj_w[5] = EVENING_RAIN_OBJ_TYPE_NORMAL;
		
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���炫��I�u�W�F�̓o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_EVENING_RAIN_OBJ_AddKirakira( WEATHER_TASK* p_wk, int num, HEAPID heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	WEATHER_EVENING_RAIN_WORK*	p_local_wk;	// �V�X�e�����[�N
	s32* obj_w;	// �I�u�W�F�N�g���[�N
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

		GFL_CLACT_WK_SetAnmSeq( p_clwk, 1 );
    GFL_CLACT_WK_SetAutoAnmFlag( p_clwk, TRUE );
		// ���W��ݒ�
		{
			mat.x = EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_X_MIN + GFUser_GetPublicRand(EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_X_RAND);
			mat.y = EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_Y_MIN + GFUser_GetPublicRand(EVENING_RAIN_OBJ_KIRAKIRA_MOVE_POS_Y_RAND);

			WEATHER_OBJ_WORK_SetPosNoTurn( add_obj, &mat );
		}

    // ���Ŏ���
    obj_w[0] = EVENING_RAIN_OBJ_KIRAKIRA_MOVE_TIME_MIN + GFUser_GetPublicRand(EVENING_RAIN_OBJ_KIRAKIRA_MOVE_TIME_RAND);

    // �I�u�W�F�^�C�v
    obj_w[5] = EVENING_RAIN_OBJ_TYPE_KIRAKIRA;
		
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  �[���X�N���[��
 */
//-----------------------------------------------------------------------------
static void WEATHER_EVENING_RAIN_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_EVENING_RAIN_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
	p_wk->scroll = (p_wk->scroll + 9) % 256;
	WEATHER_TASK_3DBG_SetScrollX( p_sys, (-p_wk->scroll) - x, WEATHER_TASK_3DBG_FRONT );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->scroll) + y, WEATHER_TASK_3DBG_FRONT );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԃ��獡�̓���^�C�v���擾
 *
 *	@param	p_wk  �V�C�^�X�N���[�N
 */
//-----------------------------------------------------------------------------
static u32 WEATHER_EVENING_RAIN_CheckTimeType_Init( const WEATHER_TASK* cp_wk )
{
  int time_zone_change_time;
  int time_zone_change_time_night;
  time_zone_change_time = WEATHER_TASK_GetTimeZoneChangeTime( cp_wk, TIMEZONE_EVENING );

  // ���J�]�[��
  if( (time_zone_change_time > EVENING_RAIN_TIME_AREA_MIN) && 
      (time_zone_change_time < EVENING_RAIN_TIME_AREA) ){
    return EVENING_RAIN_TYPE_RAIN;
  }

  return EVENING_RAIN_TYPE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԃ��獡�̓���^�C�v���擾  ���C���p
 *
 *	@param	p_wk  �V�C�^�X�N���[�N
 */
//-----------------------------------------------------------------------------
static u32 WEATHER_EVENING_RAIN_CheckTimeType_Main( const WEATHER_TASK* cp_wk, u32 now_evening_rain_type )
{
  int time_zone_change_time;
  int time_zone_change_time_night;
  time_zone_change_time = WEATHER_TASK_GetTimeZoneChangeTime( cp_wk, TIMEZONE_EVENING );

  // ���J�]�[��
  if( (time_zone_change_time > EVENING_RAIN_TIME_AREA_MIN) && 
      (time_zone_change_time < EVENING_RAIN_TIME_AREA) ){
    return EVENING_RAIN_TYPE_RAIN;
  }

  // ���炫��]�[��
  if( (now_evening_rain_type == EVENING_RAIN_TYPE_RAIN) || (now_evening_rain_type == EVENING_RAIN_TYPE_KIRAKIRA) ){
    if( (time_zone_change_time < EVENING_RAIN_TIME_AREA_MIN) && 
        (time_zone_change_time > EVENING_RAIN_TIME_KIRAKIRA_AREA) ){
      return EVENING_RAIN_TYPE_KIRAKIRA;
    }
  }

  return EVENING_RAIN_TYPE_NONE;
}




//----------------------------------------------------------------------------
/**
 *	@brief �[���@�J
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// �X�p�[�N������
	WEATHER_PARK_Init( &p_local_wk->spark, WEATHER_SPARK_MODE_EASY );

	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk, 
		EVENING_RAIN_ADD_START,	// obj�o�^��
		EVENING_RAIN_TIMING_MAX,// �o�^�^�C�~���O
		EVENING_RAIN_ADD_MAIN,
		EVENING_RAIN_TIMING_MIN,
		-EVENING_RAIN_TIMING_ADD,
		EVENING_RAIN_ADD_TIMING,
		EVENING_RAIN_ADD,
		WEATHER_EVENING_RAIN_OBJ_Add );
	

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, EVENING_RAIN_KIRAKIRA_FOG_SLOPE, WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );

	p_local_wk->work[0] = WEATHER_STRAIN_FOG_START;	// �������t�H�O�p
	p_local_wk->work[1] = 0;
	p_local_wk->work[2] = 0;							// ���J�E���^

	p_local_wk->work[4] = 39;							// ���J�E���^

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );


  p_local_wk->seq     = EVENING_RAIN_SEQ_FADEIN;

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // �����A���ԓ��Ȃ�A�t�F�[�h�C��
  {
    BOOL result;
    BOOL fog_result;

    // �I�u�W�F�N�g�t�F�[�h
    result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

    if( p_local_wk->work[4] > 0 )
    {
      p_local_wk->work[4]--;
      if( p_local_wk->work[4] == 0 )
      {
        // ��
        WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	
        // BGON
        WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );
      }
    }

    if(p_local_wk->work[0] > 0){
      p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
      if( p_local_wk->work[0] == 0 ){
        WEATHER_TASK_FogFadeIn_Init( p_wk,
        EVENING_RAIN_KIRAKIRA_FOG_SLOPE, 
        EVENING_RAIN_KIRAKIRA_FOG_OFS, 
        EVENING_RAIN_KIRAKIRA_FOG_TIMING,
        fog_cont );

        // ���C�g�ύX
        WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_EVENING_RAIN ) );

      }
    }else{
      fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
      

      // �^�C�~���O���ŏ��ɂȂ����烁�C����
      if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����

        // �V�[�P���X�ύX
        p_local_wk->seq     = EVENING_RAIN_SEQ_ON;
        return WEATHER_TASK_FUNC_RESULT_FINISH;
      }
    }

    // �X�N���[������
    WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
  }

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  
  // �����A���ԓ��Ȃ�A�ݒ�
  {

    // ��Ɨ̈�̏�����
    WEATHER_TASK_ObjFade_Init( p_wk,
      EVENING_RAIN_ADD_MAIN,	// obj�o�^��
      EVENING_RAIN_TIMING_MIN,// �o�^�^�C�~���O
      EVENING_RAIN_ADD_MAIN,
      EVENING_RAIN_TIMING_MIN,
      -EVENING_RAIN_TIMING_ADD,
      EVENING_RAIN_ADD_TIMING,
      EVENING_RAIN_ADD,
      WEATHER_EVENING_RAIN_OBJ_Add );
    

    // �t�H�O�̐ݒ�
    WEATHER_TASK_FogSet( p_wk, EVENING_RAIN_KIRAKIRA_FOG_SLOPE, EVENING_RAIN_KIRAKIRA_FOG_OFS, fog_cont );

    // �I�u�W�F�N�g���U��΂�
    WEATHER_TASK_DustObj( p_wk, WEATHER_EVENING_RAIN_OBJ_Add, WEATHER_STRAIN_NOFADE_OBJ_START_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_MOVE, heapID );

    p_local_wk->work[1] = 0;
    p_local_wk->work[2] = 0;							// ���J�E���^

    // ��
    WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_HIGHRAIN );	

    // BGON
    WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE, WEATHER_TASK_3DBG_FRONT );

    // ���C�g�ύX
    WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( WEATHER_NO_EVENING_RAIN ) );

    // �[���J�n
    p_local_wk->seq     = EVENING_RAIN_SEQ_ON;

  }

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
  int time;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // �t�F�[�h�C���E�l�`�h�m�E�t�F�[�h�A�E�g
  {
    WEATHER_TASK_FUNC_RESULT  result;
    
    switch( p_local_wk->seq ){
    case EVENING_RAIN_SEQ_NONE:
      // �I��
      break;
      
    case EVENING_RAIN_SEQ_FADEIN:
      WEATHER_EVENING_RAIN_RAIN_FadeIn( p_wk, fog_cont, heapID );
      break;
      
    case EVENING_RAIN_SEQ_ON:

      //���C������
      {
        p_local_wk->work[2] = (p_local_wk->work[2] + 1) % (WEATHER_STRAIN_OBJ_MUL_CHG*WEATHER_STRAIN_OBJ_MUL_NUM);		// �J�o�^���ύX�J�E���^
        // �J�E���^��0�����ɂȂ�����J�o�^
        WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );

        // �X�p�[�N���C��
        WEATHER_PARK_Main( &p_local_wk->spark, p_wk );

        // �X�N���[������
        WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
      }
      break;
      
    case EVENING_RAIN_SEQ_FADEOUT:
      WEATHER_EVENING_RAIN_RAIN_FadeOut( p_wk, fog_cont, heapID );
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  }
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // ���łɃt�F�[�h�����ƒ��`�F�b�N
  if( p_local_wk->seq == EVENING_RAIN_SEQ_FADEOUT ){
	  return WEATHER_TASK_FUNC_RESULT_FINISH;
  }
  
	// obj
	// �t�F�[�h�A�E�g�ݒ�
  {
    WEATHER_TASK_ObjFade_SetOut( p_wk,
        0,
        EVENING_RAIN_TIMING_MAX,
        EVENING_RAIN_TIMING_ADD,
        EVENING_RAIN_ADD_END );
    
    p_local_wk->work[0] = WEATHER_STRAIN_FOG_START_END;	// �������t�H�O�p


    // ��
    WEATHER_TASK_StopLoopSnd( p_wk );	
    // BGOFF
    WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE, WEATHER_TASK_3DBG_FRONT );

    p_local_wk->seq = EVENING_RAIN_SEQ_FADEOUT;
  }
  
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_RAIN_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  {
    BOOL result;
    BOOL fog_result;

    // �I�u�W�F�N�g�t�F�[�h
    result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

    // �t�H�O����
    if(p_local_wk->work[0] > 0){
      p_local_wk->work[0]--;			// ���[�N���O�ɂȂ�����t�H�O�𓮂���
      if( p_local_wk->work[0] == 0 ){

        WEATHER_TASK_FogFadeOut_Init( p_wk,
            WEATHER_FOG_DEPTH_DEFAULT_START, 
            EVENING_RAIN_KIRAKIRA_FOG_TIMING_END, fog_cont );
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

          p_local_wk->seq = EVENING_RAIN_SEQ_NONE;
          
          return WEATHER_TASK_FUNC_RESULT_FINISH;
        }
      }
    }

    // �X�N���[������
    WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
  }
  
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}


// �[���@���炫��
static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// ��Ɨ̈�̏�����
	WEATHER_TASK_ObjFade_Init( p_wk, 
		EVENING_RAIN_KIRAKIRA_ADD_START,	// obj�o�^��
		EVENING_RAIN_KIRAKIRA_TIMING_MAX,// �o�^�^�C�~���O
		EVENING_RAIN_KIRAKIRA_ADD_MAIN,
		EVENING_RAIN_KIRAKIRA_TIMING_MIN,
		-EVENING_RAIN_KIRAKIRA_TIMING_ADD,
		EVENING_RAIN_KIRAKIRA_ADD_TIMING,
		EVENING_RAIN_KIRAKIRA_ADD,
		WEATHER_EVENING_RAIN_OBJ_AddKirakira );
	

  p_local_wk->seq     = EVENING_RAIN_SEQ_KIRAKIRA_FADEIN;

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // �����A���ԓ��Ȃ�A�t�F�[�h�C��
  {
    BOOL result;

    // �I�u�W�F�N�g�t�F�[�h
    result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s
    // �^�C�~���O���ŏ��ɂȂ����烁�C����
    if( result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
      // �V�[�P���X�ύX
      p_local_wk->seq     = EVENING_RAIN_SEQ_KIRAKIRA_ON;
      return WEATHER_TASK_FUNC_RESULT_FINISH;
    }

    // �X�N���[������
    WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
  }

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  
  // �����A���ԓ��Ȃ�A�ݒ�
  {

    // ��Ɨ̈�̏�����
    WEATHER_TASK_ObjFade_Init( p_wk,
      EVENING_RAIN_KIRAKIRA_ADD_MAIN,	// obj�o�^��
      EVENING_RAIN_KIRAKIRA_TIMING_MIN,// �o�^�^�C�~���O
      EVENING_RAIN_KIRAKIRA_ADD_MAIN,
      EVENING_RAIN_KIRAKIRA_TIMING_MIN,
      -EVENING_RAIN_KIRAKIRA_TIMING_ADD,
      EVENING_RAIN_KIRAKIRA_ADD_TIMING,
      EVENING_RAIN_KIRAKIRA_ADD,
      WEATHER_EVENING_RAIN_OBJ_AddKirakira );
    

    // �I�u�W�F�N�g���U��΂�
    WEATHER_TASK_DustObj( p_wk, WEATHER_EVENING_RAIN_OBJ_AddKirakira, WEATHER_STRAIN_NOFADE_OBJ_START_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_NUM, WEATHER_STRAIN_NOFADE_OBJ_START_DUST_MOVE, heapID );

    // �[���J�n
    p_local_wk->seq     = EVENING_RAIN_SEQ_KIRAKIRA_ON;

  }

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;
  int time;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // �t�F�[�h�C���E�l�`�h�m�E�t�F�[�h�A�E�g
  {
    WEATHER_TASK_FUNC_RESULT  result;
    
    switch( p_local_wk->seq ){
    case EVENING_RAIN_SEQ_NONE:
      // �I��
      break;
      
    case EVENING_RAIN_SEQ_KIRAKIRA_FADEIN:
      WEATHER_EVENING_RAIN_KIRAKIRA_FadeIn( p_wk, fog_cont, heapID );
      break;
      
    case EVENING_RAIN_SEQ_KIRAKIRA_ON:

      //���C������
      {
        // �J�E���^��0�����ɂȂ�����J�o�^
        WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID );

        // �X�N���[������
        WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
      }
      break;
      
    case EVENING_RAIN_SEQ_KIRAKIRA_FADEOUT:
      WEATHER_EVENING_RAIN_KIRAKIRA_FadeOut( p_wk, fog_cont, heapID );
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  }
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  // ���łɃt�F�[�h�����ƒ��`�F�b�N
  if( p_local_wk->seq == EVENING_RAIN_SEQ_KIRAKIRA_FADEOUT ){
	  return WEATHER_TASK_FUNC_RESULT_FINISH;
  }
  
	// obj
	// �t�F�[�h�A�E�g�ݒ�
  {
    WEATHER_TASK_ObjFade_SetOut( p_wk,
        0,
        EVENING_RAIN_KIRAKIRA_TIMING_MAX,
        EVENING_RAIN_KIRAKIRA_TIMING_ADD,
        EVENING_RAIN_KIRAKIRA_ADD_END );
    
    p_local_wk->seq = EVENING_RAIN_SEQ_KIRAKIRA_FADEOUT;
  }
  
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

static WEATHER_TASK_FUNC_RESULT WEATHER_EVENING_RAIN_KIRAKIRA_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, HEAPID heapID )
{
	WEATHER_EVENING_RAIN_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

  {
    BOOL result;

    // �I�u�W�F�N�g�t�F�[�h
    result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

    if( result ){
      // �o�^�����O�ɂȂ�����I�����邩�`�F�b�N
      // �����̊Ǘ����邠�߂��S�Ĕj�����ꂽ��I��
      if( WEATHER_TASK_GetActiveObjNum( p_wk ) == 0 ){//*/

        p_local_wk->seq = EVENING_RAIN_SEQ_NONE;
        
        return WEATHER_TASK_FUNC_RESULT_FINISH;
      }
    }

    // �X�N���[������
    WEATHER_EVENING_RAIN_SCROLL_Main( p_wk, p_local_wk );
  }
  
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}





//----------------------------------------------------------------------------
/**
 *	@brief	������	������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_PARK_Init( WEATHER_SPARK_WORK* p_wk, u8 mode )
{
	GFL_STD_MemClear( p_wk, sizeof(WEATHER_SPARK_WORK) );

	p_wk->wait = GFUser_GetPublicRand( WEATHER_SPARK_INIT_RAND );

  p_wk->mode = mode;
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
 *
 *	EASY    ���̂ݕ\��
 *	NORMAL  �ʏ�
 *	RAIKAMI light�ȗ��Ȃ�
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
      if( p_wk->mode == WEATHER_SPARK_MODE_EASY )
      {
        WEATEHR_SPARK_SetUp_Light( p_wk );
      }
      else
      {
        switch( i ){
        case WEATHER_SPARK_TYPE_LIGHT:
          if( p_wk->mode == WEATHER_SPARK_MODE_NORMAL ){
            WEATEHR_SPARK_SetUp_Light( p_wk );
          }else{
            WEATEHR_SPARK_SetUp_Hard( p_wk );
          }
          break;
        case WEATHER_SPARK_TYPE_HARD:
          WEATEHR_SPARK_SetUp_Hard( p_wk );
          break;
        case WEATHER_SPARK_TYPE_LIGHT_HARD:
          WEATEHR_SPARK_SetUp_LightAndHard( p_wk );
          break;
        }
      }

			p_wk->spark_tbl_count	= 0;
			p_wk->seq				= WEATHER_SPARK_SPARK_DARK;
		}
		break;

  case WEATHER_SPARK_SPARK_DARK:
    if( p_wk->mode != WEATHER_SPARK_MODE_RAIKAMI )
    {
			p_wk->seq = WEATHER_SPARK_SPARK;
    }
    else
    {
      // �h�f�J�C����炷�B�iLIGHT���Â����Ă���j
	  	WEATHER_TASK_LIGHT_StartColorFadeOneWay( p_sys, WEATHER_SPARK_DARK_COLOR, WEATHER_SPARK_DARK_SYNC );
      p_wk->wait = WEATHER_SPARK_DARK_WAIT;
			p_wk->seq				= WEATHER_SPARK_SPARK_DARK_WAIT;
    }
    break;

  case WEATHER_SPARK_SPARK_DARK_WAIT:
    p_wk->wait --;
    if( p_wk->wait <= 0 )
    {
			p_wk->seq = WEATHER_SPARK_SPARK;
    }
    break;

	case WEATHER_SPARK_SPARK:	// ��
    
    if( p_wk->mode != WEATHER_SPARK_MODE_EASY ) //Easy�͉��̂�
    {
		  WEATHER_TASK_LIGHT_StartColorFade( p_sys, WEATHER_SPARK_GetColor(p_wk), p_wk->spark_data[ p_wk->spark_tbl_count ].insync, p_wk->spark_data[ p_wk->spark_tbl_count ].outsync );
    }

    
		// LIGHT->HARD�̏ꍇ�A�t�F�[�h���ɁA�����˂��Ă��܂�
		if( p_wk->spark_type == WEATHER_SPARK_TYPE_LIGHT_HARD ){
			p_wk->wait = (p_wk->spark_data[ p_wk->spark_tbl_count ].insync/2) + GFUser_GetPublicRand( p_wk->spark_data[ p_wk->spark_tbl_count ].outsync );
		}

    p_wk->snd_wait = p_wk->spark_data[ p_wk->spark_tbl_count ].snd_wait;
    p_wk->snd_se = p_wk->spark_data[ p_wk->spark_tbl_count ].snd_se;

    // ���̗��܂ł�wait
    // �ŏI������I�������A�E�G�C�g����ύX����
    // (1/2)�ɂ���
    if( p_wk->mode == WEATHER_SPARK_MODE_RAIKAMI )
    {
      if( (p_wk->spark_tbl_count+1) == p_wk->spark_tbl_num )
      {
        p_wk->spark_data[ p_wk->spark_tbl_count ].wait = (p_wk->spark_data[ p_wk->spark_tbl_count ].wait * 1) / 3;
      }
    }
    // EASY�Ȃ�A�E�G�C�g�̐���2�{�ɂ���
    else if( p_wk->mode == WEATHER_SPARK_MODE_EASY )
    {
      p_wk->spark_data[ p_wk->spark_tbl_count ].wait = (p_wk->spark_data[ p_wk->spark_tbl_count ].wait * 2);
    }
    
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
        WEATHER_TASK_PlaySnd( p_sys, p_wk->snd_se );
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
        WEATHER_TASK_PlaySnd( p_sys, p_wk->snd_se );
      }
    }
		
    // ���[�h�ɂ��p�x���ς��
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
    p_wk->spark_data[i].snd_se    = WEATHER_SND_SE_SPARK_SML;
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
  p_wk->spark_data[0].snd_se    = WEATHER_SND_SE_SPARK;
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
    p_wk->spark_data[i].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_LIGHT_HARD ];
    p_wk->spark_data[i].snd_se    = WEATHER_SND_SE_SPARK_SML;
	}

	p_wk->spark_data[i].spark_power = sc_WEATHER_SPARK_STRONG_RAND_MIN[ WEATHER_SPARK_TYPE_LIGHT_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_STRONG_RAND_MAX[ WEATHER_SPARK_TYPE_LIGHT_HARD ] );
	p_wk->spark_data[i].insync		= WEATHER_SPARK_FLASHIN_SYNC;
	p_wk->spark_data[i].outsync		= sc_WEATHER_SPARK_FLASHOUT_SYNC[ p_wk->spark_data[i].spark_power ];
	p_wk->spark_data[i].wait		= sc_WEATHER_SPARK_TYPE_RAND_WAIT_MIN[ WEATHER_SPARK_TYPE_LIGHT_HARD ] + GFUser_GetPublicRand( sc_WEATHER_SPARK_TYPE_RAND_WAIT_MAX[ WEATHER_SPARK_TYPE_LIGHT_HARD ] );
  p_wk->spark_data[i].snd_wait = sc_WEATHER_SPARK_SND_WAIT[ WEATHER_SPARK_TYPE_LIGHT_HARD ];
  p_wk->spark_data[i].snd_se    = WEATHER_SND_SE_SPARK;
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


