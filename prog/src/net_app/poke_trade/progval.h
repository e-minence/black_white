//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		progval.h
 *	@brief		�v�Z���N���X
 *	@author		Toru=Nagihashi
 *	@data		2007.11.06
 *
 *  ������
 *	�����ⓙ�����x�^���Ȃǂɂ��Ă̌v�Z�����N���X���������́B
 *	�\���̂�Alloc���Ȃ��̂ŁA���[�N�G���A�ȂǂɎ����Ȃ���΂����Ȃ��B
 *
 *
 *	���菇
 *		�P�D���[�N�G���A�Ȃǂɍ\���̂��`
 *		�Q�D�\���̂�Init�֐��ŏ������B���̍ۂɏ����l��^����B
 *		�R�D�\���̂�Main�֐��ɂ킽�����t���[���Ă�
 *		�S�D�\���̂�now_val�ϐ��Ƀt���[�����Ƃ̌v�Z���ʂ������Ă���̂ŁA
 *			�ǂݎ���Ē��ڎg�p����B�i��{�A���������Ȃ����Ɓj
 *		�T�DMain�֐��̖߂�l��TRUE�ɂȂ�ƌv�Z�I��
 *
 *	���g�����C���[�W
 *		switch( seq ){
 *		case SEQ_INIT:
 *			PROGVAL_VEL_Init( &work->vel, 0, 100, 60 );
 *			seq=SEQ_MAIN;
 *			break;
 *
 *		case SEQ_MAIN:
 *			if( PROGVAL_VEL_Main( &work->vel ) ) {
 *				seq=SEQ_END;
 *			}
 *			SetObjPosX( work->vel.now_val );//����OBJ��X���W��ݒ肷��֐��Ƃ���
 *			break;
 *
 *		case SEQ_END:
 *			break;
 *		}
 *
 *	
 *	�������F�v�Z��
 *  1.���������^��
 *		���������Ԃɂ���ē������ړ�
 *  
 *		���x V = v0
 *		���� t
 *		�ʒu x = x0 + V*T
 *
 *		�E�O���t
 *		��
 *		||
 *		||
 *		||�\�\�\�\�\�\
 *		||
 *		||
 *		||
 *		 =============�@��
 *	
 * 
 *  2.�������x�����^��
 *		���������Ԃɂ���đ������Ă����ړ�
 *  
 *		���x v = v0 + at
 *		���� t
 *		�ʒu x = v0*t + 1/2*a*(t*t)
 *		���� a = 2(x - v0*t) / (t*t)
 *
 *		�ό`��	 v*v - v0*v0 = 2*a*x
 *
 *		�E�O���t
 *		��
 *		||
 *		||�@�@�@�@�^�@
 *		||�@�@�@�^�@
 *		||�@�@�^
 *		||�@�^
 *		||�^
 *		 =============�@��
 *
 *
 *	3.�R���Ȑ��i�G���~�[�g�Ȑ��j
 *	
 *		�R���Ȑ��͂S�̃p�����[�^�łł���i�Ƃ�����n�̃p�����[�^����n-1���Ȑ����o����j
 *		�G���~�[�g�Ȑ��Ƃ͈ȉ��̃p�����[�^���K�v�ȋȐ��̂���
 *		�Ȑ��J�n�ʒu	x0
 *		�Ȑ��I���ʒu	x1
 *		�J�n�����x�N�g��v0
 *		�I�������x�N�g��v1
 *
 *		�n�_����I�_���w�肵�āA�Q�̃x�N�g���ɂ��
 *		���̊Ԃ̋Ȑ���⊮���܂��B
 *		�l�b�g��Łu�G���~�[�g�Ȑ� java�v�Ɠ����ƐF�X���߂��܂��B
 *
 *
 *		��	X = x(t) = at^3 + bt^2 + ct + d
 *
 *		x(t)���O�ƂP�̂Ƃ�
 *			x(0) = d, x(1) = a + b + c + d;
 *
 *		���Ԃ���������ꍇ�̑��x��
 *			v(t) = dx(t)/dt = 3at^2 + 2bt * c
 *		�������Ƃ��Ĉȉ��̍s�񎮂ɂȂ�
 *
 *
 *								  [	 2 -2  1   1 ][	x0 ]
 *		x( t ) = [t^3, t^2, t, 1] [ -3  3 -2  -1 ][ x1 ]
 *								  [	 0  0  1   0 ][	v0 ]
 *								  [	 1  0  0   0 ][	v1 ]
 *		�E�O���t
 *		Y
 *		||
 *		||�@�@�@�@�@
 *		||�@�@�@�Q�Q�@
 *		||�@�@�^�@�@�_			���Ȑ��̂���
 *		||�@��v0	 �Ex1
 *		||�^�@�@�@�@ ��v1
 *		�Ex0=============�@X
 *
 *
 *	4.�R���Ȑ��i�x�W�F�Ȑ��j
 *		�x�W�F�Ȑ��͈ȉ��̃p�����[�^�ŏo����Ȑ��̂���
 *		�Ȑ��J�n�ʒu	p0
 *		�Ȑ�����_�P	p1
 *		�Ȑ�����_�Q	p2
 *		�Ȑ��I���ʒu	p3
 *		�i���_�㐧��_�͂����ł��悢���A���̌`�����g���₷���Ǝv����j
 *
 *		�G���~�[�g�Ȑ��͎n�_�ƏI�_�ɋȂ��鋭���̃x�N�g����^���邪�A
 *		����x�W�F�Ȑ���4�_��^���邱�Ƃŏo����B
 *
 *		�x�W�F��4�_����o����3�̐����ŁA�גʂ��̐����Q�ɂ����āA�����_���v�Z��
 *		�o�����Q�̓����_������ł����ƍ쐬���邱�Ƃ��o����B
 *		���̂��߁A�n�_�ƏI�_�͓_��ʉ߂��邪�A����_�͒ʉ߂��Ȃ��B
 *
 *		���̓G���~�[�g�Ȑ��̋�����_�ɕϊ�����ƑΓ��̊֌W�ɂȂ�B
 *		�x�W�F						�G���~�[�g
 *		p0	= x0					x0	= p0
 *		p1	= x0 + v0 / 3			x1	= p3
 *		p2	= x1 - v1 / 3			v0	= 3(p1 - p0)
 *		p3	= x1					v1	= 3(p3 - p2)
 *
 *		��L�̊֌W�ɂ��
 *
 *								  [	-1  3 -3 1 ][ p0 ]
 *		x( t ) = [t^3, t^2, t, 1] [  3 -6  3 0 ][ p1 ]
 *								  [	-3  3  0 0 ][ p2 ]
 *								  [	 1  0  0 0 ][ p3 ]
 *
 *		����ɕό`�����
 *												  [ p0 ]
 *		x( t ) = [(1-t)^3 3t(1-t)^2 3t^2(1-t) t^3][ p1 ]
 *												  [ p2 ]
 *												  [ p3 ]
 *
 *	5.�R���Ȑ��iB�X�v���C���Ȑ��j
 *
 *
 *
 *	6.�R���Ȑ��iCatmullROm�Ȑ��j
 *
 *		�x�W�F�͐���_���w��ł��܂����A����_�̈ʒu�ƋȐ��̊֌W��
 *		���܂����C���[�W���ɂ����ł��B
 *		Catmull�Ȑ��́A�n�_�A�I�_�ɉ����S�Ă̐���_��ʂ�Ȑ��ł��B
 *		
 *
 *
 * ����
 *		20080708	�S�ď�������
 *					pokemon encount_effect_def	��蔲���o���{����
 *
 *		20080710	vel, vel_fx, aclr, sin�e�^���̓������C���B
 *					�Estart<end����Ȃ��Ă����삷��悤�ɕύX�B
 *					�E�덷�����Ȃ�����悤�ɏC��
 *
 *		20080711	hermite�ǉ� pezier�ǉ� bspline�ǉ� catmullrom�ǉ�
 *
 *		20080918	SOCproject�ɒǉ��B�J�E���^�O���֐����쐬�B
 *		20080919	3���Ȑ���Z�l���v�Z����Ă��Ȃ������o�O�C��
 *		20081027	ACLR_FX�ŏ������傫���Ƃ��Ɍ����ړ��ɂȂ�Ȃ������̂��C��
 *
 *
 *	����̖ڕW
 *		fraction	�����֐�
 *	
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __PROGVAL_H__
#define __PROGVAL_H__

//-----------------------------------------------------------------------------
/**
 *					�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	���������^������
//=====================================
typedef struct {
	int now_val;		//���݂̒l
	int start_val;		//�J�n�̒l
	int end_val;		//�I���̒l
	fx32 add_val;		//���Z�l(�덷�������Ȃ��悤�ɂ�������fx)
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_VELOCITY_WORK;
extern void PROGVAL_VEL_Init( PROGVAL_VELOCITY_WORK* p_wk, int start, int end, int sync );
extern BOOL	PROGVAL_VEL_Main( PROGVAL_VELOCITY_WORK* p_wk );

//-------------------------------------
//	���������^������	�Œ菭������
//=====================================
typedef struct {
	fx32 now_val;		//���݂̒l
	fx32 start_val;		//�J�n�̒l
	fx32 end_val;		//�I���̒l
	fx32 add_val;		//���Z�l�iFX�Ԃ͂����ɒl�����Z���Ă����Ɠ������x�����^���ɂȂ�܂��j
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_VELOCITY_WORK_FX;
//�J�E���^�����
extern void PROGVAL_VEL_InitFx( PROGVAL_VELOCITY_WORK_FX* p_wk, fx32 start, fx32 end, int sync );
extern BOOL	PROGVAL_VEL_MainFx( PROGVAL_VELOCITY_WORK_FX* p_wk );
//�J�E���^�O����
extern void PROGVAL_VEL_InitFxN( PROGVAL_VELOCITY_WORK_FX* p_wk, fx32 start, fx32 end, int sync_max );
extern BOOL	PROGVAL_VEL_MainFxN( PROGVAL_VELOCITY_WORK_FX* p_wk, int sync_now );

//-------------------------------------
//	�������x�����^��	�V���N������
//=====================================
typedef struct {
	fx32 now_val;		//���݂̒l
	fx32 start_val;		//�J�n�̒l
	fx32 end_val;		//�I���̒l
	fx32 velocity;		//�����x
	fx32 aclr;			//�����x
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_ACLR_WORK_FX;
extern void PROGVAL_ACLR_InitFx( PROGVAL_ACLR_WORK_FX* p_wk, fx32 start, fx32 end, fx32 velocity, int sync );
extern BOOL	PROGVAL_ACLR_MainFx( PROGVAL_ACLR_WORK_FX* p_wk );

//-------------------------------------
//	�����^��	�V���N������
//=====================================
typedef struct {
	fx32 now_val;		//���݂̒l
	fx32 start_val;		//�J�n�̒l
	fx32 end_val;		//�I���̒l
	fx32 velocity;		//�����x
	fx32 deg;			//����
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_DEG_WORK_FX;
extern void PROGVAL_DEG_InitFx( PROGVAL_ACLR_WORK_FX* p_wk, fx32 start, fx32 end, fx32 velocity, int sync );
extern BOOL	PROGVAL_DEG_MainFx( PROGVAL_ACLR_WORK_FX* p_wk );


//-------------------------------------
//	�T�C��������
//=====================================
typedef struct {
	fx32 now_val;		//���݂̃T�C���l
	u16 rot_val;		//���݂̊p�x�l
	u16 start_val;		//�J�n�̊p�x
	u16 end_val;		//�I���̊p�x
	u16 add_val;		//�����p�x
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_SINMOVE_WORK;
extern void PROGVAL_SINMOVE_Init( PROGVAL_SINMOVE_WORK* p_wk, u16 start_rot, u16 end_rot, int sync );
extern BOOL	PROGVAL_SINMOVE_Main( PROGVAL_SINMOVE_WORK* p_wk );

//-------------------------------------
///	�R���Ȑ��G���~�[�g�Ȑ���
//		�n�_�A�I�_�A�J�n���x�x�N�g���A�I�����x�x�N�g����
//		�^���āA������Ȑ�
//=====================================
typedef struct {
	VecFx32 now_val;	//���݂̍��W
	VecFx32	start_pos;	//�J�n���W
	VecFx32 start_vec;	//�J�n�@���x�N�g��
	VecFx32 end_pos;	//�I�����W
	VecFx32 end_vec;	//�I���@���x�N�g��
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_HERMITE_WORK;
//	�J�E���^�����
extern void PROGVAL_HERMITE_Init( PROGVAL_HERMITE_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_start_vec,	const VecFx32 *cp_end_pos, const VecFx32 *cp_end_vec, int sync );
extern BOOL PROGVAL_HERMITE_Main( PROGVAL_HERMITE_WORK* p_wk );
//	�J�E���^�O����
extern void PROGVAL_HERMITE_InitN( PROGVAL_HERMITE_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_start_vec,	const VecFx32 *cp_end_pos, const VecFx32 *cp_end_vec, int sync_max );
extern BOOL PROGVAL_HERMITE_MainN( PROGVAL_HERMITE_WORK* p_wk, int sync_now );


//-------------------------------------
///	�R���Ȑ��x�W�F�Ȑ���
//		�n�_�A�I�_�A����_�O�C�P��^����
//		������Ȑ��B
//=====================================
typedef struct {
	VecFx32 now_val;	//���݂̍��W
	VecFx32	start_pos;	//�J�n���W
	VecFx32 ctrl_pos0;	//����_�O
	VecFx32 ctrl_pos1;	//����_�P
	VecFx32 end_pos;	//�I�����W
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_PEZIER_WORK;
extern void PROGVAL_PEZIER_Init( PROGVAL_PEZIER_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_ctrl_pos0, const VecFx32 *cp_ctrl_pos1, const VecFx32 *cp_end_pos, int sync );
extern BOOL PROGVAL_PEZIER_Main( PROGVAL_PEZIER_WORK* p_wk );

//-------------------------------------
///	�R���Ȑ�B�X�v���C���Ȑ���
//=====================================
typedef struct {
	VecFx32 now_val;	//���݂̍��W
	VecFx32	start_pos;	//�J�n���W
	VecFx32 ctrl_pos0;	//����_�O
	VecFx32 ctrl_pos1;	//����_�P
	VecFx32 end_pos;	//�I�����W
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_BSPLINE_WORK;
extern void PROGVAL_BSPLINE_Init( PROGVAL_BSPLINE_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_ctrl_pos0, const VecFx32 *cp_ctrl_pos1, const VecFx32 *cp_end_pos, int sync );
extern BOOL PROGVAL_BSPLINE_Main( PROGVAL_BSPLINE_WORK* p_wk );

//-------------------------------------
///	�R���Ȑ�CatmullROm�Ȑ���
//		�n�_�A�I�_�A����_�O�C�P��^����
//		�_���ʂ�Ȑ��B
//=====================================
typedef struct {
	VecFx32 now_val;	//���݂̍��W
	VecFx32	start_pos;	//�J�n���W
	VecFx32 ctrl_pos0;	//����_�O
	VecFx32 ctrl_pos1;	//����_�P
	VecFx32 end_pos;	//�I�����W
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_CATMULLROM_WORK;
extern void PROGVAL_CATMULLROM_Init( PROGVAL_CATMULLROM_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_ctrl_pos0, const VecFx32 *cp_ctrl_pos1, const VecFx32 *cp_end_pos, int sync );
extern BOOL PROGVAL_CATMULLROM_Main( PROGVAL_CATMULLROM_WORK* p_wk );


//-------------------------------------
//	���������^������	Vector��
//=====================================
typedef struct {
	PROGVAL_VELOCITY_WORK_FX	x;
	PROGVAL_VELOCITY_WORK_FX	y;
	PROGVAL_VELOCITY_WORK_FX	z;
	VecFx32						start_pos;
	VecFx32						end_pos;
	VecFx32						now_pos;
} PROGVAL_VELOCITY_WORK_VEC;
//�J�E���^�����
extern void PROGVAL_VEL_InitVec( PROGVAL_VELOCITY_WORK_VEC* p_wk, const VecFx32 *cp_start, const VecFx32 *cp_end, int sync );
extern BOOL	PROGVAL_VEL_MainVec( PROGVAL_VELOCITY_WORK_VEC* p_wk );
//�J�E���_�O����
extern void PROGVAL_VEL_InitVecN( PROGVAL_VELOCITY_WORK_VEC* p_wk, const VecFx32 *cp_start, const VecFx32 *cp_end, int sync_max );
extern BOOL	PROGVAL_VEL_MainVecN( PROGVAL_VELOCITY_WORK_VEC* p_wk, int sync_now );

//-------------------------------------
//	�������x�����^��	�V���N������
//=====================================
typedef struct {
	PROGVAL_ACLR_WORK_FX	aclr;
	VecFx32					now_pos;
	VecFx32					start_pos;
	VecFx32					direction;
} PROGVAL_ACLR_WORK_VEC;
//�J�E���^���e
extern void PROGVAL_ACLR_InitVec( PROGVAL_ACLR_WORK_VEC* p_wk, const VecFx32 *cp_start, const VecFx32 *cp_end, fx32 velocity, int sync );
extern BOOL	PROGVAL_ACLR_MainVec( PROGVAL_ACLR_WORK_VEC* p_wk );

//-------------------------------------
//	�������x�����^��	�V���N�񓯊� �ő�l��
//=====================================
typedef struct {
	VecFx32				now_pos;
	VecFx32				start_pos;
	VecFx32				end_pos;
	fx32				aclr;
	fx32				distance;
	fx32				aclr_add;
	fx32				aclr_dec;
	fx32				aclr_max;
} PROGVAL_ACLR_WORK_VEC2;
//�J�E���^���e
extern void PROGVAL_ACLR_InitVec2( PROGVAL_ACLR_WORK_VEC2* p_wk, const VecFx32 *cp_start, const VecFx32 *cp_end, fx32 aclr_init, fx32 aclr_add, fx32 aclr_max, fx32 aclr_dec );
extern BOOL	PROGVAL_ACLR_MainVec2( PROGVAL_ACLR_WORK_VEC2* p_wk );


//-------------------------------------
///	�h��鏈��
//=====================================
typedef struct {
	VecFx32				now_pos;
	VecFx32				start_pos;
	VecFx32				shake_min;
	VecFx32				shake_max;
	u16					space_now;
	u16					space_max;
	u16					sync_now;
	u16					sync_max;
	u16					seq;
} PROGVAL_SHAKE_WORK_VEC;
#define PROGVAL_SHAKE_LOOP	(0xFFFF)
extern void PROGVAL_SHAKE_InitVec( PROGVAL_SHAKE_WORK_VEC *p_wk, const VecFx32 *cp_start, const VecFx32 * cp_shake_min, const VecFx32 *cp_shake_max, u16 space, u16 sync );
extern BOOL PROGVAL_SHAKE_MainVec( PROGVAL_SHAKE_WORK_VEC *p_wk );

//-------------------------------------
///	��]����
//=====================================
typedef struct {
	MtxFx43	now_rot_mtx;

	u16	start_rot;
	u16	now_rot;
	s32	move_rot;

	VecFx32 center_pos;
	VecFx32 rot_vec;

	s32 count;
	s32 count_max;
} PROGVAL_ROT_WORK;
extern void PROGVAL_ROT_Init( PROGVAL_ROT_WORK* p_wk, const VecFx32* cp_center_ofs, u16 start_rot, s32 end_rot, const VecFx32* cp_rotvec, u32 count_max );
extern BOOL PROGVAL_ROT_Main( PROGVAL_ROT_WORK* p_wk );

#endif		// __PROGVAL_H__

