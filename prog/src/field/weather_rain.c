//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_rain.c
 *	@brief		�V�C�J
 *	@author		tomoya takahashi
 *	@data		2009.03.27
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
#define WEATHER_RAIN_TIMING_ADD		(1)							// �^�C�~���O�����炷��
#define WEATHER_RAIN_ADD_START		(1)							// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_RAIN_ADD_TIMING		(1)							// �J�̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_RAIN_ADD			(1)							// �o�^���鐔�𑝂₷��
#define	WEATHER_RAIN_ADD_MAIN		(2)							// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_RAIN_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_RAIN_NOFADE_OBJ_START_DUST_NUM	( 10 )			// ���P�ʂŕω������邩
#define	WEATHER_RAIN_NOFADE_OBJ_START_DUST_MOVE	( 1 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_RAIN_FOG_TIMING		(300)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_RAIN_FOG_TIMING_END	(300)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_START_END	(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_RAIN_FOG_OFS		(0x300)
#define WEATHER_RAIN_FOG_OFS_START	(0x800)

/*== �J�I�u�W�F�N�g ==*/
#define WEATHER_RAIN_SPEED_X		(-5)						// ���ɐi�ރX�s�[�h
#define WEATHER_RAIN_SPEED_Y		(10)						// ���Ăɐi�ރX�s�[�h�x�[�X
#define	WEATHER_RAIN_END_MIN		(1)							// �I���J�E���^�ŏ�
#define WEATHER_RAIN_END_MAX		(3)							// �I���J�E���^�ő�
#define	WEATHER_RAIN_START_X_BASE	(0)							// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_RAIN_MUL_X			(15)						// �x�[�X�ɉJ�̎�ޕ������l
#define	WEATHER_RAIN_START_X_MAX	(270)						// X�J�n���W�����̍ő�l
#define	WEATHER_RAIN_START_Y		(-96)						// Y�J�n���W
#define	WEATHER_RAIN_SPEED_ERR		(20)						// �X�s�[�h�␳�l


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
	0,			// BG CG
	0,			// BG PLTT
	0,			// BG SCRN

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
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_RAIN_FOG_OFS_START, fog_cont );



	p_local_wk->work[0] = WEATHER_RAIN_FOG_START;	// �������t�H�O�p

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

	// ��
//	WeatherLoopSndPlay( sys_work, SEQ_SE_DP_T_AME );	

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
//	WeatherLoopSndStop( sys_work );

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
					WEATHER_FOG_DEPTH_DEFAULT + WEATHER_RAIN_FOG_OFS_START, 
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

	GFL_CLACT_WK_GetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	
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
				GFL_CLACT_WK_SetAnmFrame( p_clwk, 3 );
			}
		}
		// ���W�ݒ�
		GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );

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
		GFL_CLACT_WK_SetAnmFrame( p_clwk, frame );

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
		GFL_CLACT_WK_SetPos( p_clwk, &mat, CLSYS_DEFREND_MAIN );
	}
}

