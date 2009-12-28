//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_storm.c
 *	@brief		����
 *	@author		tomoya takahshi
 *	@date		2009.03.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc/arc_def.h"
#include "arc/field_weather.naix"
#include "arc/field_weather_light.naix"


#include "weather_storm.h"

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
//	�����Ǘ��֐��萔
//=====================================
#define	WEATHER_STORM_TIMING_MIN		(2)							// �����o���^�C�~���O�ŏ�
#define WEATHER_STORM_TIMING_MAX		(15)						// �����o���^�C�~���O�ő�
#define WEATHER_STORM_TIMING_ADD		(2)							// �^�C�~���O�����炷��
#define WEATHER_STORM_TIMING_ADD_END	(1)							// �^�C�~���O�����炷��
#define WEATHER_STORM_ADD_START			(1)							// �ŏ��̓����ɉJ��o�^���鐔
#define WEATHER_STORM_ADD_TIMING		(4)							// ���̃^�C�~���O�������ύX������P�񑝂₷
#define WEATHER_STORM_ADD				(1)							// �o�^���鐔�𑝂₷��
#define WEATHER_STORM_ADD_END			(-1)							// �o�^���鐔�𑝂₷��
#define	WEATHER_STORM_ADD_MAIN			(1)							// ���C���V�[�P���X�ł̓o�^���鐔

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_STORM_NOFADE_OBJ_START_NUM	( 24 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_STORM_NOFADE_OBJ_START_DUST_NUM		( 2 )			// �������炵�ē��삳���邩
#define	WEATHER_STORM_NOFADE_OBJ_START_DUST_MOVE	( 2 )			// �P�����炵�ē��삳����

/*== �t�H�O ==*/
#define	WEATHER_STORM_FOG_TIMING		(200)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_STORM_FOG_TIMING_END	(200)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_STORM_FOG_START			(1)							// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STORM_FOG_START_END		(31)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_STORM_FOG_OFS	( 0x100 )
#define WEATHER_STORM_FOG_SLOPE	( 4 )


/*== BG ==*/
#define WEATHER_STORM_BG_ALP_TIM_S	(32)		// �A���t�@���P�グ��^�C�~���O
#define WEATHER_STORM_BG_ALP_TIM_E	(24)		// �A���t�@���P������^�C�~���O
#define WEATHER_STORM_BG_ALP_S_END	( 3 )
#define WEATHER_STORM_BG_ALP_E_END	( 0 )
#define WEATHER_STORM_BG_ALP_DEF	( 16 )

#define WEATHER_STORM_BG_ALP_STRONG_WIND	(4)	// ���������̂Ƃ��̃��l


/*== ���I�u�W�F�N�g ==*/
#define	WEATHER_STORM_END_MIN		(15)						// �I���J�E���^�ŏ�
#define WEATHER_STORM_END_MAX		(35)						// �I���J�E���^�ő�
#define WEATHER_STORM_END_NUM		(WEATHER_STORM_END_MAX - WEATHER_STORM_END_MIN)	// �J�E���^�̒l�̐�
#define	WEATHER_STORM_END_DIV		((WEATHER_STORM_END_NUM / 4)+1)	// �I�u�W�F�N�g�̎�ނ��v�Z����Ƃ�����l
#define	WEATHER_STORM_START_X1		(262)						// �x�[�X�ɂȂ�X�J�n���W	�悱�p
#define	WEATHER_STORM_START_X1_RAN	(24)						// �x�[�X�ɂȂ�X�J�n���W	�悱�p
#define WEATHER_STORM_START_Y1_MIN	( -64 )						//  Y�J�n���W�ŏ�
#define	WEATHER_STORM_START_Y1_MAX	(192)						// Y�J�n���W�ő�
#define	WEATHER_STORM_SPEED_NUM		(8)							// �������̐�
#define	WEATHER_STORM_SPEED_CNG_NUM	(40)						// �ς���^�C�~���O
#define WEATHER_STORM_SPEED_ADD_TMG (5)						// ���̐�����ς���^�C�~���O

#define WEATHER_STORM_STRONG_CHG	(6)						// �������̒l�ȏ�ɂȂ�������������������

static const int STORM_SPEED_X_TBL[WEATHER_STORM_SPEED_NUM] = {-6,-10,-10,-6,-10,-12,-20,-12};
static const int STORM_SPEED_Y_TBL[WEATHER_STORM_SPEED_NUM] = { 4, 4, 4, 8, 8, 4, 4, 4};	

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
} WEATHER_STORM_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------



//-------------------------------------
///	����
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static void WEATHER_STORM_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_STORM_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 


static void WEATHER_STORM_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_STORM_WORK* p_wk );


//-----------------------------------------------------------------------------
/**
 *			�V�C�f�[�^
 */
//-----------------------------------------------------------------------------
// ����
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_STORM = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	TRUE,		// OAM���g�p���邩�H
	TRUE,		// BG���g�p���邩�H
	NARC_field_weather_storm_NCGR,			// OAM CG
	NARC_field_weather_storm_NCLR,			// OAM PLTT
	NARC_field_weather_storm_NCER,			// OAM CELL
	NARC_field_weather_storm_NANR,			// OAM CELLANM
	NARC_field_weather_storm_nsbtx,		// BGTEX
	GX_TEXSIZE_S32,		// GXTexSizeS
	GX_TEXSIZE_T32,		// GXTexSizeT
	GX_TEXREPEAT_ST,		// GXTexRepeat
	GX_TEXFLIP_NONE,		// GXTexFlip
	GX_TEXFMT_PLTT4,		// GXTexFmt
	GX_TEXPLTTCOLOR0_TRNS,		// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(WEATHER_STORM_WORK),

	// �Ǘ��֐�
	WEATHER_STORM_Init,		// ������
	WEATHER_STORM_FadeIn,		// �t�F�[�h�C��
	WEATHER_STORM_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_STORM_Main,		// ���C������
	WEATHER_STORM_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_STORM_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_STORM_Exit,		// �j��

	// �I�u�W�F����֐�
	WEATHER_STORM_OBJ_Move,
};




//----------------------------------------------------------------------------
/**
 *	@brief	������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_STORM_ADD_START,	// obj�o�^��
		WEATHER_STORM_TIMING_MAX,	// �o�^�^�C�~���O
		WEATHER_STORM_ADD_MAIN,
		WEATHER_STORM_TIMING_MIN,
		-WEATHER_STORM_TIMING_ADD,
		WEATHER_STORM_ADD_TIMING,
		WEATHER_STORM_ADD,
		WEATHER_STORM_OBJ_Add );

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_STORM_FOG_SLOPE, 
				WEATHER_FOG_DEPTH_DEFAULT_START, fog_cont );



	p_local_wk->work[0] = WEATHER_STORM_FOG_START;	// �������t�H�O�p
	p_local_wk->work[1] = 0;		// �I�u�W�F�N�g�ǉ����J�E���^
	p_local_wk->work[2] = 0;		// BG�J�E���^
	p_local_wk->work[3] = 0;		// ���̃X�s�[�h

	// �X�N���[�������̏�����
	WEATHER_TASK_InitScrollDist( p_wk );

  // SE
  WEATHER_TASK_PlayLoopSnd( p_wk, WEATHER_SND_SE_STORM );
	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
	BOOL result;
	BOOL fog_result;


	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	result = WEATHER_TASK_ObjFade_Main( p_wk, heapID );	// ���s

	if(p_local_wk->work[0] > 0){
		p_local_wk->work[0]--;			// ���[�N6���O�ɂȂ�����t�H�O�𓮂���
		if( p_local_wk->work[0] == 0 ){
			WEATHER_TASK_FogFadeIn_Init( p_wk,
			WEATHER_STORM_FOG_SLOPE, 
			WEATHER_FOG_DEPTH_DEFAULT + WEATHER_STORM_FOG_OFS, 
			WEATHER_STORM_FOG_TIMING,
			fog_cont );


      // ���C�g�ύX
      WEATHER_TASK_LIGHT_Change( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_storm_dat, heapID );
		}
	}else{
		
		fog_result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
		// �^�C�~���O���ŏ��ɂȂ����烁�C����
		if( fog_result && result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
			// BGON
			WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );

			// �V�[�P���X�ύX
			return WEATHER_TASK_FUNC_RESULT_FINISH;
		}
	}

	// �X�N���[������
	WEATHER_STORM_SCROLL_Main( p_wk, p_local_wk );


	return WEATHER_TASK_FUNC_RESULT_CONTINUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ��Ɨ̈�̏�����
	// �I�u�W�F�N�g�t�F�[�h�p
	WEATHER_TASK_ObjFade_Init( p_wk,
		WEATHER_STORM_ADD_MAIN,	// obj�o�^��
		WEATHER_STORM_TIMING_MIN,	// �o�^�^�C�~���O
		WEATHER_STORM_ADD_MAIN,
		WEATHER_STORM_TIMING_MIN,
		-WEATHER_STORM_TIMING_ADD,
		WEATHER_STORM_ADD_TIMING,
		WEATHER_STORM_ADD,
		WEATHER_STORM_OBJ_Add );


	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_STORM_FOG_SLOPE, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_STORM_FOG_OFS, fog_cont );

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_STORM_OBJ_Add, WEATHER_STORM_NOFADE_OBJ_START_NUM, WEATHER_STORM_NOFADE_OBJ_START_DUST_NUM, WEATHER_STORM_NOFADE_OBJ_START_DUST_MOVE, heapID );

	
	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, TRUE );


  // ���C�g�ύX
  WEATHER_TASK_LIGHT_Set( p_wk, ARCID_FIELD_WEATHER_LIGHT, NARC_field_weather_light_light_storm_dat, heapID );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
	u32 wind;
	
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	if( WEATHER_TASK_ObjFade_NoFadeMain( p_wk, heapID ) ){

		// ���e�[�u�����擾
		wind = p_local_wk->work[1] / WEATHER_STORM_SPEED_CNG_NUM;
		// ����ݒ肷��
		p_local_wk->work[3] = STORM_SPEED_X_TBL[wind];
		
		// ���l�ƃI�u�W�F�̏o���ʂ𕗂̋����ŕς���
		if( p_local_wk->work[3] <= -WEATHER_STORM_STRONG_CHG ){
			// �����o�^
			WEATHER_STORM_OBJ_Add( p_wk, WEATHER_STORM_ADD_MAIN, heapID );
		
		}
	}

	// �X�N���[������
	WEATHER_STORM_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// obj
	// �t�F�[�h�A�E�g�ݒ�
	WEATHER_TASK_ObjFade_SetOut( p_wk,
			0,
			WEATHER_STORM_TIMING_MAX,
			WEATHER_STORM_TIMING_ADD,
			WEATHER_STORM_ADD_END );
	
	// �t�H�O
	p_local_wk->work[0] = WEATHER_STORM_FOG_START_END;	// �������t�H�O�p


	// BGON
	WEATHER_TASK_3DBG_SetVisible( p_wk, FALSE );


  // SE
  WEATHER_TASK_StopLoopSnd( p_wk );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
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
					WEATHER_STORM_FOG_TIMING_END, fog_cont );
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
	WEATHER_STORM_SCROLL_Main( p_wk, p_local_wk );

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_STORM_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_STORM_WORK* p_local_wk;
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
static void WEATHER_STORM_OBJ_Move( WEATHER_OBJ_WORK* p_wk )
{
	GFL_CLACTPOS mat;
	GFL_CLWK* p_clwk;
	s32* obj_w;		// �I�u�W�F���[�N

	obj_w = WEATHER_OBJ_WORK_GetWork( p_wk );
	p_clwk = WEATHER_OBJ_WORK_GetClWk( p_wk );

	WEATHER_OBJ_WORK_GetPos( p_wk, &mat );
	
	// ����t���O���`�F�b�N
	if(!obj_w[3]){
		// ������
		mat.x += obj_w[4];
		mat.y += obj_w[2];

		if((obj_w[0] % WEATHER_STORM_SPEED_ADD_TMG) == 0){
			obj_w[4] += obj_w[5];
		}

		// �j�����邩�`�F�b�N
		if(obj_w[0]++ > obj_w[1]){
			// �j��
			obj_w[3] = 1;
		}

		WEATHER_OBJ_WORK_SetPos( p_wk, &mat );
	}else{
		WEATHER_TASK_DeleteObj( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�I�u�W�F�o�^
 */
//-----------------------------------------------------------------------------
static void WEATHER_STORM_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID )
{
	int i;		// ���[�v�p
	WEATHER_OBJ_WORK* add_obj;		// �o�^�I�u�W�F
	int	wind;	// ������
	int	rand;
	WEATHER_STORM_WORK* sys_w;	// �V�X�e�����[�N
	s32* obj_w;	// �I�u�W�F���[�N
	int frame;	// �t���[����
	GFL_CLWK* p_clwk;
	GFL_CLACTPOS mat;
	
	sys_w = WEATHER_TASK_GetWorkData( p_wk );

	
	
	// ���ύX�J�E���g
	sys_w->work[1] = (sys_w->work[1]+1) % (WEATHER_STORM_SPEED_CNG_NUM*WEATHER_STORM_SPEED_NUM);
		
	// �����������߂�
	wind = sys_w->work[1] / WEATHER_STORM_SPEED_CNG_NUM;
	
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
		obj_w[1] = WEATHER_STORM_END_MIN + GFUser_GetPublicRand(WEATHER_STORM_END_MAX - WEATHER_STORM_END_MIN);	// �I���J�E���^
		
		frame = 3 - ((obj_w[1] - WEATHER_STORM_END_MIN) / WEATHER_STORM_END_DIV);	// ����̎��
		
		obj_w[2] = (STORM_SPEED_Y_TBL[wind]) * (frame+1);
		obj_w[4] = (STORM_SPEED_X_TBL[wind]) * (frame+1);
		obj_w[3] = 0;			// �j���A�j���t���O
		obj_w[5] = STORM_SPEED_X_TBL[wind];	// �X�s�[�h���グ�Ă����l

		// �ق�Ƃɂ��܂ɃT�{�e��
		rand = GFUser_GetPublicRand(1000);
		if(rand == 777){
			frame = 4;
			obj_w[2] += obj_w[2] / 2;
		}
		
		GFL_CLACT_WK_SetAnmIndex( p_clwk, frame );
		
		mat.x = WEATHER_STORM_START_X1 + GFUser_GetPublicRand(WEATHER_STORM_START_X1_RAN);
		mat.y = WEATHER_STORM_START_Y1_MIN + GFUser_GetPublicRand(WEATHER_STORM_START_Y1_MAX);

		// ���W�ݒ�
		WEATHER_OBJ_WORK_SetPos( add_obj, &mat );
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[������
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_STORM_SCROLL_Main( WEATHER_TASK* p_sys, WEATHER_STORM_WORK* p_wk )
{
	int x, y;
	WEATHER_TASK_GetScrollDist( p_sys, &x, &y );
	WEATHER_TASK_ScrollObj( p_sys, x, y );

	// BG�ʂ��΂ߏ�ɓ�����
	p_wk->work[2] = (p_wk->work[2] + 6) % 256;
	WEATHER_TASK_3DBG_SetScrollX( p_sys, (p_wk->work[2]*2) - x );
	WEATHER_TASK_3DBG_SetScrollY( p_sys, (-p_wk->work[2]*2) + y );
}
