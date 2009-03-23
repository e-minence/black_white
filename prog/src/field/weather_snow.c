//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_snow.c
 *	@brief		�V�C	��
 *	@author		tomoya takahashi
 *	@data		2009.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc/arc_def.h"
#include "arc/field_weather.naix"


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
#define	WEATHER_SNOW_TIMING_MIN		(14)				// ����o���^�C�~���O�ŏ�
#define WEATHER_SNOW_TIMING_MAX		(24)				// ����o���^�C�~���O�ő�
#define WEATHER_SNOW_TIMING_ADD		(5)					// �^�C�~���O�����炷��
#define	WEATHER_SNOW_ADD_TIMING		(1)					// �P�x�̃I�u�W�F�o�^���𑝂₷�^�C�~���O
#define	WEATHER_SNOW_ADD			(0)					// �I�u�W�F�o�^���𑫂��l
#define WEATHER_SNOW_MAIN			(1)					// ���C���ł͂��ꂾ���o�^

/*== �t�F�[�h�����J�n�̎� ==*/
#define WEATHER_SNOW_NOFADE_OBJ_START_NUM	( 20 )				// �J�n���̎U�z����I�u�W�F�N�g�̐�
#define	WEATHER_SNOW_NOFADE_OBJ_START_DUST_NUM	( 2 )			// �������炷��
#define	WEATHER_SNOW_NOFADE_OBJ_START_DUST_MOVE	( 24 )			// ���炵�ē��삳����l

/*== �t�H�O ==*/
#define	WEATHER_SNOW_FOG_TIMING		(1)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_SNOW_FOG_TIMING_END	(2)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_START		(16)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_START_END	(32)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_SNOW_FOG_OFS		(0x300)

/*== ��I�u�W�F�N�g ==*/
#define	WEATHER_SNOW_ADD_TMG_X_BASE	(4)					// ��̃X�s�[�h�𑫂��^�C�~���O
#define	WEATHER_SNOW_ADD_TMG_X_RAN	(2)					// ��̃X�s�[�h�𑫂��^�C�~���O�����l
#define	WEATHER_SNOW_ADD_TMG_Y_BASE	(1)					// ��̃X�s�[�h�𑫂��^�C�~���O
#define	WEATHER_SNOW_ADD_TMG_Y_RAN	(2)					// ��̃X�s�[�h�𑫂��^�C�~���O�����l

#define	WEATHER_SNOW_START_X_BASE	(-32)						// �x�[�X�ɂȂ�X�J�n���W
#define	WEATHER_SNOW_START_X_MAX	(414)						// X�J�n���W�����̍ő�l
#define WEATHER_SNOW_OBJ_NUM		(4)					// �I�u�W�F���

#define WEATHER_SNOW_OBJ_AUTODEST_TIMING_MAX ( 60 )	// ���Ń^�C���@�ő�
#define WEATHER_SNOW_OBJ_AUTODEST_TIMING_MIN ( 4 )	// ���Ń^�C���@�ŏ�

/*== ��O���t�B�b�N�\���f�[�^ ==*/
#define WEATHER_SNOW_GRAPHIC_CELL	( 3 )
#define WEATHER_SNOW_GRAPHIC_BG		( WEATHER_GRAPHIC_NONE )



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


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�ʏ�̐�
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Init( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeIn( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_NoFade( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Main( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_InitFadeOut( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeOut( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Exit( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID ); 
static void WEATHER_SNOW_OBJ_Move( WEATHER_OBJ_WORK* p_wk ); 
static void WEATHER_SNOW_OBJ_Add( WEATHER_TASK* p_wk, int num, u32 heapID ); 



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
	0,			// BG CG
	0,			// BG PLTT
	0,			// BG SCRN

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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Init( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
	if( fog_cont ){
		WEATHER_TASK_FogFade_Init( p_wk,
				WEATHER_FOG_SLOPE_DEFAULT, 
				WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_FOG_OFS, 
				GX_RGB(26,26,26),
				WEATHER_SNOW_FOG_TIMING );
	}
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeIn( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
	}else{
		
		fog_result = WEATHER_TASK_FogFade_Main( p_wk );
		
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_NoFade( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
	if(fog_cont){
		/*
		// �f�[�^��ݒ�
		sys_w->fogFade.Fog = sys_work->pWSysCont->fsys->fog_data;		// �t�H�O�ۑ�����
		weatherSysFogParamSet( sys_w->fogFade.Fog, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_SNOW_FOG_OFS, GX_RGB(26,26,26) );

		// �e�[�u���f�[�^���쐬���Ĕ��f
		weatherSysFogSet( &sys_w->fogFade );
		//*/
	}

	// �I�u�W�F�N�g���U��΂�
	WEATHER_TASK_DustObj( p_wk, WEATHER_SNOW_OBJ_Add, WEATHER_SNOW_NOFADE_OBJ_START_NUM, WEATHER_SNOW_NOFADE_OBJ_START_DUST_NUM, WEATHER_SNOW_NOFADE_OBJ_START_DUST_MOVE, heapID );


	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Ⴡ�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Main( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_InitFadeOut( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
	if( fog_cont ){
//		weatherSysFogFadeInit( &sys_w->fogFade, WEATHER_SNOW_FOG_TIMING_END, FALSE );
	}
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_FadeOut( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
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
	}else{
	
		if( fog_cont ){
			fog_result = WEATHER_TASK_FogFade_Main( p_wk );
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
static WEATHER_TASK_FUNC_RESULT WEATHER_SNOW_Exit( WEATHER_TASK* p_wk, BOOL fog_cont, u32 heapID )
{
	// FOG�I��
	

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
	

	
	GFL_CLACT_WK_GetPos( p_clwk, &pos, CLSYS_DEFREND_MAIN );

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
	GFL_CLACT_WK_SetPos( p_clwk, &pos, CLSYS_DEFREND_MAIN );
	
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
		GFL_CLACT_WK_SetAnmFrame( p_clwk, frame );
		
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
		pos.x = ( WEATHER_SNOW_START_X_BASE + (GFUser_GetPublicRand(WEATHER_SNOW_START_X_MAX)) );
		
		if((p_local_wk->work[1] == 1) &&
			(i >= (num /2)) ){	// �����o��
			pos.y = ( -40 - GFUser_GetPublicRand(20));
		}else{
			pos.y = ( -8 - GFUser_GetPublicRand(20));
		}
		GFL_CLACT_WK_SetPos( p_clwk, &pos, CLSYS_DEFREND_MAIN );
	}

}


