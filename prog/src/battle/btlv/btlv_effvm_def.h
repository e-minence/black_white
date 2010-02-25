//======================================================================
/**
 * @file	btlv_effvm_def.h
 * @brief	�G�t�F�N�g�V�[�P���X�p�̃}�N����`�t�@�C��
 * @author	HisashiSogabe
 * @date	2009.02.12
 */
//======================================================================

//====================================================================================
/**
 *	�萔��`
 */
//====================================================================================
#ifndef __BTLV_EFFVM_DEF_H_
#define __BTLV_EFFVM_DEF_H_

#define	BTLV_EFFVM_STACK_SIZE	( 16 )	///<�g�p����X�^�b�N�̃T�C�Y
#define	BTLV_EFFVM_REG_SIZE		( 8 )		///<�g�p���郌�W�X�^�̐�

//�X�N���v�g�A�h���X�e�[�u���p��`
#define	BTLEFF_AA_SAME			( 0xffffffff )		//�����ʒuAA�Ɠ���
#define	BTLEFF_AA_REVERSE		( 0xfffffffe )		//�����ʒuAA�̔��]
#define	BTLEFF_BB_SAME			( 0xfffffffd )		//�����ʒuBB�Ɠ���
#define	BTLEFF_BB_REVERSE		( 0xfffffffc )		//�����ʒuBB�̔��]
#define	BTLEFF_A_SAME				( 0xfffffffb )		//�����ʒuA�Ɠ���
#define	BTLEFF_A_REVERSE		( 0xfffffffa )		//�����ʒuA�̔��]
#define	BTLEFF_B_SAME				( 0xfffffff9 )		//�����ʒuB�Ɠ���
#define	BTLEFF_B_REVERSE		( 0xfffffff8 )		//�����ʒuB�̔��]
#define	BTLEFF_C_SAME				( 0xfffffff7 )		//�����ʒuC�Ɠ���
#define	BTLEFF_C_REVERSE		( 0xfffffff6 )		//�����ʒuC�̔��]
#define	BTLEFF_D_SAME				( 0xfffffff5 )		//�����ʒuD�Ɠ���
#define	BTLEFF_D_REVERSE		( 0xfffffff4 )		//�����ʒuD�̔��]

//�J�����ړ��^�C�v
#define	BTLEFF_CAMERA_MOVE_DIRECT										( 0 )		//�_�C���N�g
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION						( 1 )		//�Ǐ]
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	( 2 )		//�Ǐ]�i���Ύw��j

//�J�����ړ���
#define	BTLEFF_CAMERA_POS_AA						( 0 )
#define	BTLEFF_CAMERA_POS_BB						( 1 )
#define	BTLEFF_CAMERA_POS_A							( 2 )
#define	BTLEFF_CAMERA_POS_B							( 3 )
#define	BTLEFF_CAMERA_POS_C							( 4 )
#define	BTLEFF_CAMERA_POS_D							( 5 )
#define	BTLEFF_CAMERA_POS_E							( 6 )
#define	BTLEFF_CAMERA_POS_F							( 7 )
#define	BTLEFF_CAMERA_POS_INIT					( 8 )
#define	BTLEFF_CAMERA_POS_ATTACK				( 9 )
#define	BTLEFF_CAMERA_POS_ATTACK_PAIR		( 10 )
#define	BTLEFF_CAMERA_POS_DEFENCE				( 11 )
#define	BTLEFF_CAMERA_POS_DEFENCE_PAIR	( 12 )

#define	BTLEFF_CAMERA_POS_NONE					( 0xffffffff )

//�J�������
#define	BTLEFF_CAMERA_SHAKE_VERTICAL		( 0 )
#define	BTLEFF_CAMERA_SHAKE_HORIZON		  ( 1 )

//�ˉe���[�h
#define	BTLEFF_CAMERA_PROJECTION_ORTHO					( 0 )
#define	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE		( 1 )

//�p�[�e�B�N���Đ�
#define	BTLEFF_PARTICLE_PLAY_POS_AA				      ( 0 )
#define	BTLEFF_PARTICLE_PLAY_POS_BB				      ( 1 )
#define	BTLEFF_PARTICLE_PLAY_POS_A				      ( 2 )
#define	BTLEFF_PARTICLE_PLAY_POS_B				      ( 3 )
#define	BTLEFF_PARTICLE_PLAY_POS_C				      ( 4 )
#define	BTLEFF_PARTICLE_PLAY_POS_D				      ( 5 )
#define	BTLEFF_PARTICLE_PLAY_POS_E				      ( 6 )
#define	BTLEFF_PARTICLE_PLAY_POS_F				      ( 7 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_NONE		      ( 8 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	      ( 9 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK_MINUS	( 10 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	      ( 11 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS	( 12 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACKOFS	    ( 13 )

//�G�~�b�^�ړ�
#define	BTLEFF_EMITTER_MOVE_NONE							( 0 )
#define	BTLEFF_EMITTER_MOVE_STRAIGHT					( 1 )
#define	BTLEFF_EMITTER_MOVE_CURVE							( 2 )
#define	BTLEFF_EMITTER_MOVE_CURVE_HALF				( 3 )

//�G�~�b�^�~�ړ�
#define BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_L   ( 0 )
#define BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_R   ( 1 )
#define BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_L  ( 2 )
#define BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_R  ( 3 )
#define BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_L   ( 4 )
#define BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_R   ( 5 )

//�|�P��������
#define	BTLEFF_POKEMON_POS_AA							( 0 )
#define	BTLEFF_POKEMON_POS_BB							( 1 )
#define	BTLEFF_POKEMON_POS_A							( 2 )
#define	BTLEFF_POKEMON_POS_B							( 3 )
#define	BTLEFF_POKEMON_POS_C							( 4 )
#define	BTLEFF_POKEMON_POS_D							( 5 )
#define	BTLEFF_POKEMON_POS_E					    ( 6 )
#define	BTLEFF_POKEMON_POS_F							( 7 )
#define	BTLEFF_TRAINER_POS_AA					    ( 8 )
#define	BTLEFF_TRAINER_POS_BB					    ( 9 )
#define	BTLEFF_TRAINER_POS_A					    ( 10 )
#define	BTLEFF_TRAINER_POS_B					    ( 11 )
#define	BTLEFF_TRAINER_POS_C					    ( 12 )
#define	BTLEFF_TRAINER_POS_D					    ( 13 )
#define	BTLEFF_POKEMON_SIDE_ATTACK				( 14 )		//�U����
#define	BTLEFF_POKEMON_SIDE_ATTACK_PAIR		( 15 )		//�U�����y�A
#define	BTLEFF_POKEMON_SIDE_DEFENCE				( 16 )		//�h�䑤
#define	BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	( 17 )		//�h�䑤�y�A

#define	BTLEFF_POKEMON_MOVE_DIRECT						    ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_MOVE_INTERPOLATION			    ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP					    ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG		    ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_MOVE_INTERPOLATION_DIRECT	( EFFTOOL_CALCTYPE_INTERPOLATION_DIRECT )
#define	BTLEFF_POKEMON_MOVE_INIT		              ( BTLEFF_POKEMON_MOVE_INTERPOLATION_DIRECT + 1 )

#define	BTLEFF_POKEMON_SCALE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_SCALE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

#define	BTLEFF_POKEMON_ROTATE_DIRECT					( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ROTATE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG	( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

#define	BTLEFF_POKEMON_ALPHA_DIRECT					  ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ALPHA_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG	  ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

#define	BTLEFF_MEPACHI_ON											( MCSS_MEPACHI_ON )
#define	BTLEFF_MEPACHI_OFF										( MCSS_MEPACHI_OFF )
#define	BTLEFF_MEPACHI_MABATAKI								( 2 )
#define	BTLEFF_ANM_STOP												( MCSS_ANM_STOP_ON )
#define	BTLEFF_ANM_START											( MCSS_ANM_STOP_OFF )

//�|�P�����~�ړ�
#define BTLEFF_AXIS_X_L                       ( 0 )     //�w����
#define BTLEFF_AXIS_X_R                       ( 1 )     //�w���E
#define BTLEFF_AXIS_Y_L                       ( 2 )     //�x����
#define BTLEFF_AXIS_Y_R                       ( 3 )     //�x���E
#define BTLEFF_AXIS_Z_L                       ( 4 )     //�y����
#define BTLEFF_AXIS_Z_R                       ( 5 )     //�y���E

#define BTLEFF_SHIFT_H_P                      ( 0 )     //�V�t�g�g�{
#define BTLEFF_SHIFT_H_M                      ( 1 )     //�V�t�g�g�|
#define BTLEFF_SHIFT_V_P                      ( 2 )     //�V�t�g�u�{
#define BTLEFF_SHIFT_V_M                      ( 3 )     //�V�t�g�u�|

//�g���[�i�[����
#define	BTLEFF_TRAINER_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_TRAINER_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

#define	BTLEFF_TRTYPE_FROM_WORK		            ( -1 )    //�g���[�i�[�Z�b�g�𒼒l�ł͂Ȃ�sequence_work�̒��g�ōs��


//SE�Đ��v���[���[��`
#define BTLEFF_SEPLAY_SYSTEM                  ( 0 )
#define BTLEFF_SEPLAY_SE1                     ( 1 )
#define BTLEFF_SEPLAY_SE2                     ( 2 )
#define BTLEFF_SEPLAY_PSG                     ( 3 )
#define BTLEFF_SEPLAY_SE3                     ( 4 )
#define BTLEFF_SEPLAY_DEFAULT                 ( 5 )

//SE�X�g�b�v�v���[���[��`
#define BTLEFF_SESTOP_SYSTEM                  ( 0 )
#define BTLEFF_SESTOP_SE1                     ( 1 )
#define BTLEFF_SESTOP_SE2                     ( 2 )
#define BTLEFF_SESTOP_PSG                     ( 3 )
#define BTLEFF_SESTOP_SE3                     ( 4 )
#define BTLEFF_SESTOP_ALL                     ( 5 )

//SE�p��
#define BTLEFF_SEPAN_INTERPOLATION            ( 0 )
#define BTLEFF_SEPAN_ROUNDTRIP                ( 1 )

#define BTLEFF_SEPAN_ATTACK                   ( BTLEFF_POKEMON_SIDE_ATTACK )
#define BTLEFF_SEPAN_DEFENCE                  ( BTLEFF_POKEMON_SIDE_DEFENCE )

//SE�G�t�F�N�g
#define BTLEFF_SEEFFECT_INTERPOLATION         ( 0 )
#define BTLEFF_SEEFFECT_ROUNDTRIP             ( 1 )

#define BTLEFF_SEEFFECT_PITCH                 ( 0 )
#define BTLEFF_SEEFFECT_VOLUME                ( 1 )
#define BTLEFF_SEEFFECT_PAN                   ( 2 )

//�G�t�F�N�g�I���҂�
#define	BTLEFF_EFFENDWAIT_ALL									( 0 )
#define	BTLEFF_EFFENDWAIT_CAMERA							( 1 )
#define	BTLEFF_EFFENDWAIT_PARTICLE						( 2 )
#define	BTLEFF_EFFENDWAIT_POKEMON							( 3 )
#define	BTLEFF_EFFENDWAIT_TRAINER							( 4 )
#define	BTLEFF_EFFENDWAIT_BG							    ( 5 )
#define	BTLEFF_EFFENDWAIT_PALFADE_STAGE		  	( 6 )
#define	BTLEFF_EFFENDWAIT_PALFADE_FIELD		  	( 7 )
#define	BTLEFF_EFFENDWAIT_PALFADE_3D			    ( 8 )
#define	BTLEFF_EFFENDWAIT_PALFADE_EFFECT	    ( 9 )
#define BTLEFF_EFFENDWAIT_SEALL               ( 10 )
#define BTLEFF_EFFENDWAIT_SE1                 ( 11 )
#define BTLEFF_EFFENDWAIT_SE2                 ( 12 )
#define BTLEFF_EFFENDWAIT_SE3                 ( 13 )
#define BTLEFF_EFFENDWAIT_PSG                 ( 14 )
#define BTLEFF_EFFENDWAIT_SYSTEM              ( 15 )

//���䃂�[�h
#define	BTLEFF_CONTROL_MODE_CONTINUE					( 0 )
#define	BTLEFF_CONTROL_MODE_SUSPEND						( 1 )

//��������
#define BTLEFF_COND_NO_EXIST                  ( 0 )
#define BTLEFF_COND_EXIST                     ( 1 )

#define BTLEFF_FX32_SHIFT											( 12 )

//BG�p���b�g�t�F�[�h�Ώے�`
#define BTLEFF_PAL_FADE_STAGE                 ( 0 )
#define BTLEFF_PAL_FADE_FIELD                 ( 1 )
#define BTLEFF_PAL_FADE_3D                    ( 2 )
#define BTLEFF_PAL_FADE_EFFECT                ( 3 )
#define BTLEFF_PAL_FADE_ALL                   ( 4 )

//�w�i�Ŏg�p����Ă���p���b�g���r�b�g�ŕ\��
#define BTLEFF_PAL_FADE_EFFECT_BIT            ( 0xff00 )

//BG�v���C�I���e�B�ݒ�
#define BTLEFF_BG_PRI_FRONT                   ( 0 )
#define BTLEFF_BG_PRI_BACK                    ( 1 )

//BG�X�N���[��
#define	BTLEFF_BG_SCROLL_DIRECT						    ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_BG_SCROLL_INTERPOLATION			  ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_BG_SCROLL_ROUNDTRIP					  ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_BG_SCROLL_ROUNDTRIP_LONG		    ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_BG_SCROLL_EVERY		            ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG + 1 )

//BG���X�^�[�X�N���[��
#define	BTLEFF_BG_RASTER_SCROLL_H						  ( 0 )
#define	BTLEFF_BG_RASTER_SCROLL_V						  ( 1 )

//BG���l
#define	BTLEFF_BG_ALPHA_DIRECT					      ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_BG_ALPHA_INTERPOLATION		      ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_BG_ALPHA_ROUNDTRIP				      ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_BG_ALPHA_ROUNDTRIP_LONG	      ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

//BG�\��/��\��
#define	BTLEFF_STAGE			( 0 )
#define	BTLEFF_FIELD			( 1 )
#define	BTLEFF_EFFECT			( 2 )

#define	BTLEFF_VANISH_OFF	( 0 )
#define	BTLEFF_VANISH_ON	( 1 )

//OBJ����
#define	BTLEFF_OBJ_MOVE_DIRECT						    ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_OBJ_MOVE_INTERPOLATION			    ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_OBJ_MOVE_ROUNDTRIP					    ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_OBJ_MOVE_ROUNDTRIP_LONG		    ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

#define	BTLEFF_OBJ_SCALE_DIRECT						    ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_OBJ_SCALE_INTERPOLATION			    ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_OBJ_SCALE_ROUNDTRIP					    ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_OBJ_SCALE_ROUNDTRIP_LONG		    ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

//�w�胏�[�N��`
#define BTLEFF_WORK_POS_AA_WEIGHT         (  0 )   ///<POS_AA�̑̏d
#define BTLEFF_WORK_POS_BB_WEIGHT         (  1 )   ///<POS_BB�̑̏d
#define BTLEFF_WORK_POS_A_WEIGHT          (  2 )   ///<POS_A�̑̏d
#define BTLEFF_WORK_POS_B_WEIGHT          (  3 )   ///<POS_B�̑̏d
#define BTLEFF_WORK_POS_C_WEIGHT          (  4 )   ///<POS_C�̑̏d
#define BTLEFF_WORK_POS_D_WEIGHT          (  5 )   ///<POS_D�̑̏d
#define BTLEFF_WORK_POS_E_WEIGHT          (  6 )   ///<POS_E�̑̏d
#define BTLEFF_WORK_POS_F_WEIGHT          (  7 )   ///<POS_F�̑̏d
#define BTLEFF_WORK_ATTACK_WEIGHT         (  8 )   ///<�U�����̑̏d
#define BTLEFF_WORK_WAZA_RANGE            (  9 )   ///<�Z�̌��ʔ͈�
#define BTLEFF_WORK_TURN_COUNT            ( 10 )   ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
#define BTLEFF_WORK_CONTINUE_COUNT        ( 11 )  ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
#define BTLEFF_WORK_YURE_CNT              ( 12 )  ///<�{�[������J�E���g
#define BTLEFF_WORK_GET_SUCCESS           ( 13 )  ///<�ߊl�������ǂ���
#define BTLEFF_WORK_ITEM_NO               ( 14 )  ///<�{�[���̃A�C�e���i���o�[
#define BTLEFF_WORK_SEQUENCE_WORK         ( 15 )  ///<�ėp���[�N
#define BTLEFF_WORK_ATTACK_POKEMON_VANISH ( 16 )  ///<�U�����̃|�P�����������Ă��邩
#define BTLEFF_WORK_ATTACK_POKEMON_DIR    ( 17 )  ///<�U�����̃|�P�����̌���
#define BTLEFF_WORK_POS_AA_RARE           ( 18 )   ///<POS_AA�����A���H
#define BTLEFF_WORK_POS_BB_RARE           ( 19 )   ///<POS_BB�����A���H
#define BTLEFF_WORK_POS_A_RARE            ( 20 )   ///<POS_A�����A���H
#define BTLEFF_WORK_POS_B_RARE            ( 21 )   ///<POS_B�����A���H
#define BTLEFF_WORK_POS_C_RARE            ( 22 )   ///<POS_C�����A���H
#define BTLEFF_WORK_POS_D_RARE            ( 23 )   ///<POS_D�����A���H
#define BTLEFF_WORK_POS_E_RARE            ( 24 )   ///<POS_E�����A���H
#define BTLEFF_WORK_POS_F_RARE            ( 25 )   ///<POS_F�����A���H
#define BTLEFF_WORK_ATTACK_RARE           ( 26 )   ///<�U���������A���H
#define BTLEFF_WORK_POS_AA_JUMP           ( 27 )   ///<POS_AA�����˂�̉H
#define BTLEFF_WORK_POS_BB_JUMP           ( 28 )   ///<POS_BB�����˂�̉H
#define BTLEFF_WORK_POS_A_JUMP            ( 29 )   ///<POS_A�����˂�̉H
#define BTLEFF_WORK_POS_B_JUMP            ( 30 )   ///<POS_B�����˂�̉H
#define BTLEFF_WORK_POS_C_JUMP            ( 31 )   ///<POS_C�����˂�̉H
#define BTLEFF_WORK_POS_D_JUMP            ( 32 )   ///<POS_D�����˂�̉H
#define BTLEFF_WORK_POS_E_JUMP            ( 33 )   ///<POS_E�����˂�̉H
#define BTLEFF_WORK_POS_F_JUMP            ( 34 )   ///<POS_F�����˂�̉H
#define BTLEFF_WORK_ATTACK_JUMP           ( 35 )   ///<�U���������˂�̉H
#define BTLEFF_WORK_MULTI                 ( 36 )   ///<�}���`�ΐ�H
#define BTLEFF_WORK_TRTYPE_A              ( 37 )   ///<�g���[�i�[�^�C�v
#define BTLEFF_WORK_TRTYPE_B              ( 38 )   ///<�g���[�i�[�^�C�v
#define BTLEFF_WORK_TRTYPE_C              ( 39 )   ///<�g���[�i�[�^�C�v
#define BTLEFF_WORK_TRTYPE_D              ( 40 )   ///<�g���[�i�[�^�C�v

//������
#define BTLEFF_COND_EQUAL       ( 0 )   // ==
#define BTLEFF_COND_NOT_EQUAL   ( 1 )   // !=
#define BTLEFF_COND_MIMAN       ( 2 )   // <
#define BTLEFF_COND_KOERU       ( 3 )   // >
#define BTLEFF_COND_IKA         ( 4 )   // <=
#define BTLEFF_COND_IJOU        ( 5 )   // >=

//�݂����
#define BTLEFF_MIGAWARI_OFF     ( 0 )   // �݂����OFF
#define BTLEFF_MIGAWARI_ON      ( 1 )   // �݂����ON

//�{�[�����[�h
#define BTLEFF_CAPTURE_BALL_POS_AA  ( 0 )
#define BTLEFF_CAPTURE_BALL_POS_BB  ( 1 )
#define BTLEFF_CAPTURE_BALL_POS_A   ( 2 )
#define BTLEFF_CAPTURE_BALL_POS_B   ( 3 )
#define BTLEFF_CAPTURE_BALL_POS_C   ( 4 )
#define BTLEFF_CAPTURE_BALL_POS_D   ( 5 )
#define BTLEFF_CAPTURE_BALL_POS_E   ( 6 )
#define BTLEFF_CAPTURE_BALL_POS_F   ( 7 )
#define BTLEFF_USE_BALL             ( 8 )
#define BTLEFF_CAPTURE_BALL_ATTACK  ( 9 )

#endif //__BTLV_EFFVM_DEF_H_

//====================================================================================
/**
 *	VisualStudio�ō쐬�����G�t�F�N�g�G�f�B�^�c�[���ł��g�p�����t�@�C���ł� 
 *	�e���߃}�N����`�̃R�����g����c�[���ŕ\�������R�}���h��������̌^�Ȃǂ𔻒f���܂�
 */
//====================================================================================

#if 0

ex)
 * #param_num	5
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_DIRECT	BTLEFF_CAMERA_MOVE_INTERPOLATION
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D	�����ʒu	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D	BTLEFF_CAMERA_POS_INIT BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_DEFENCE
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT

#param_num�������āA�R�}���h�̈����̐���錾�����܂�(@param�̑����ɂȂ�܂��j
doxygen������@param�͂��̂܂܃c�[����̃R�����g�Ƃ��ĕ\������܂�
#param���������ƂŁA�c�[���œǂݍ��ރp�����[�^�ł��邱�Ƃ̐錾�����܂�
�p�����[�^�̌^���L�q���܂�
�ECOMBOBOX_TEXT
�@�R���{�{�b�N�X�^�ɂ��āA�R���{�{�b�N�X�ŕ\�����郊�X�g�f�[�^���L���܂�

�ECOMBOBOX_VALUE
�@��ɐݒ肵���R���{�{�b�N�X�^�̃C���f�b�N�X�ɑΉ����Ď��ۂ̃V�[�P���X�\�[�X�ɏ����o����郉�x����`�Ȃǂ��L�q���܂�

�EVALUE_FX32
�@FX32�^�̒l�i�c�[����ł́A��������̐��l���͂��\�ɂ��܂��j
	�^�̐錾�̌��ɁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�

�EVALUE_INT
	INT�^�̒l�i�c�[����ł́A�����Ȃ��̐��l���͂ɂȂ�܂��j
	�^�̐錾�̌��ɁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�

�EVALUE_VECFX32
	VecFx32�^�̒l�@�^�̐錾�̌��Ƀ^�u��؂�ŁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�
ex)
	VALUE_VECFX32	camPosX	camPosY	camPosZ
	
	�������邱�Ƃɂ���āA�c�[���̃_�C�A���O�ɁA���͂��邽�߂̒l�̈Ӗ���\�������邱�Ƃ��ł��܂�
 
�EVALUE_ANGLE
	�p�x���͗p�̒l�i0�`360�j
  �R���o�[�^�Łi0�`0xffff�j�ɕϊ����܂�

�EFILE_DIALOG
�@�t�@�C���_�C�A���O��\�����ăt�@�C���I���𑣂��@�I�����i��g���q���w��ł��܂�

�EFILE_DIALOG_COMBOBOX
�@�t�@�C���_�C�A���O�őI�������t�@�C�����R���{�{�b�N�X�ŕ\�����܂��@�I�����i��g���q���w��ł��܂�

�ECOMBOBOX_HEADER
�@FILE_DIALOG_COMBOBOX�őI�������t�@�C���̊g���q.h�t�@�C����ǂݍ���ŃR���{�{�b�N�X��\�����܂�

#endif

//���߃V���{���錾
//COMMAND_START
#define	EC_CAMERA_MOVE							  ( 0 )
#define	EC_CAMERA_MOVE_COORDINATE		  ( 1 )
#define	EC_CAMERA_MOVE_ANGLE				  ( 2 )
#define	EC_CAMERA_SHAKE				        ( 3 )
#define	EC_CAMERA_PROJECTION				  ( 4 )
#define	EC_PARTICLE_LOAD						  ( 5 )
#define	EC_PARTICLE_PLAY						  ( 6 )
#define	EC_PARTICLE_PLAY_COORDINATE	  ( 7 )
#define	EC_PARTICLE_PLAY_ORTHO			  ( 8 )
#define	EC_PARTICLE_DELETE					  ( 9 )
#define	EC_EMITTER_MOVE							  ( 10 )
#define	EC_EMITTER_MOVE_COORDINATE	  ( 11 )
#define	EC_EMITTER_CIRCLE_MOVE        ( 12 )
#define	EC_EMITTER_CIRCLE_MOVE_ORTHO  ( 13 )
#define	EC_POKEMON_MOVE							  ( 14 )
#define	EC_POKEMON_CIRCLE_MOVE			  ( 15 )
#define	EC_POKEMON_SCALE						  ( 16 )
#define	EC_POKEMON_ROTATE						  ( 17 )
#define	EC_POKEMON_ALPHA						  ( 18 )
#define	EC_POKEMON_MOSAIC						  ( 19 )
#define	EC_POKEMON_SET_MEPACHI_FLAG	  ( 20 )
#define	EC_POKEMON_SET_ANM_FLAG			  ( 21 )
#define	EC_POKEMON_PAL_FADE					  ( 22 )
#define	EC_POKEMON_VANISH					    ( 23 )
#define	EC_POKEMON_SHADOW_VANISH	    ( 24 )
#define	EC_POKEMON_DEL	              ( 25 )
#define	EC_TRAINER_SET							  ( 26 )
#define	EC_TRAINER_MOVE							  ( 27 )
#define	EC_TRAINER_ANIME_SET				  ( 28 )
#define	EC_TRAINER_DEL							  ( 29 )
#define	EC_BG_LOAD	   				        ( 30 )
#define	EC_BG_SCROLL		   				    ( 31 )
#define	EC_BG_RASTER_SCROLL		   	    ( 32 )
#define	EC_BG_PAL_ANM		   				    ( 33 )
#define	EC_BG_PRIORITY		   				  ( 34 )
#define	EC_BG_ALPHA		   				      ( 35 )
#define	EC_BG_PAL_FADE		   				  ( 36 )
#define	EC_BG_VANISH								  ( 37 )
#define	EC_OBJ_SET	   				        ( 38 )
#define	EC_OBJ_MOVE	   				        ( 39 )
#define	EC_OBJ_SCALE	   				      ( 40 )
#define	EC_OBJ_ANIME_SET	   				  ( 41 )
#define	EC_OBJ_PAL_FADE		   				  ( 42 )
#define	EC_OBJ_DEL	   				        ( 43 )
#define	EC_SE_PLAY									  ( 44 )
#define	EC_SE_STOP									  ( 45 )
#define	EC_SE_PAN								      ( 46 )
#define	EC_SE_EFFECT								  ( 47 )
#define	EC_EFFECT_END_WAIT					  ( 48 )
#define	EC_WAIT											  ( 49 )
#define	EC_CONTROL_MODE							  ( 50 )
#define	EC_IF						              ( 51 )
#define	EC_MCSS_POS_CHECK						  ( 52 )
#define	EC_SET_WORK						        ( 53 )
#define	EC_GET_WORK						        ( 54 )
#define	EC_MIGAWARI						        ( 55 )
#define	EC_HENSHIN						        ( 56 )
#define	EC_NAKIGOE						        ( 57 )
#define	EC_BALL_MODE						      ( 58 )
#define	EC_BALLOBJ_SET						    ( 59 )
#define	EC_CALL						            ( 60 )
#define	EC_RETURN						          ( 61 )
#define	EC_JUMP						            ( 62 )
#define	EC_PAUSE						          ( 63 )

//�I���R�}���h�͕K����ԉ��ɂȂ�悤�ɂ���
#define	EC_SEQ_END									  ( 64 )

#ifndef __C_NO_DEF_

//����Ƀp�f�B���O�������Ȃ��悤�ɂ���
	.option	alignment off

//���߃}�N����`
//======================================================================
/**
 * @brief		�J�����ړ�
 *
 * #param_num	5
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	�����ʒu	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_ATTACK_PAIR	BTLEFF_CAMERA_POS_DEFENCE BTLEFF_CAMERA_POS_DEFENCE_PAIR	BTLEFF_CAMERA_POS_INIT	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE	type, move_pos, frame, wait, brake
	.short	EC_CAMERA_MOVE
	.long		\type
	.long		\move_pos
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�J�����ړ��i���W�w��j
 *
 * #param_num	6
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���ʒu
 * @param	move_tar	�ړ���^�[�Q�b�g
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�Ǐ]�i���Ύw��j	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_VECFX32	CamPosX	CamPosY	CamPosZ
 * #param	VALUE_VECFX32	CamTarX	CamTarY	CamTarZ
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE_COODINATE	type, pos_x, pos_y, pos_z, tar_x, tar_y, tar_z, frame, wait, brake
	.short	EC_CAMERA_MOVE_COORDINATE
	.long		\type
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
	.long		\tar_x
	.long		\tar_y
	.long		\tar_z
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�J�����ړ��i�p�x�w��j
 *
 * #param_num	6
 * @param	type				�J�����ړ��^�C�v
 * @param	angle_phi		�ړ���Ӂi�Ǐ]�̂Ƃ��́A���ݒl����̑��΁j
 * @param	angle_theta	�ړ���Ɓi�Ǐ]�̂Ƃ��́A���ݒl����̑��΁j
 * @param	frame				�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait				�ړ��E�G�C�g
 * @param	brake				�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_INT	�ړ����
 * #param	VALUE_INT	�ړ����
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE_ANGLE	type, angle_phi, angle_theta, frame, wait, brake
	.short	EC_CAMERA_MOVE_ANGLE
	.long		\type
	.long		\angle_phi
	.long		\angle_theta
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�J�������
 *
 * #param_num	6
 * @param	dir     ������
 * @param	value   ��ꕝ
 * @param	offset	��ꕝ�I�t�Z�b�g
 * @param	frame   �P���ɂ�����t���[��
 * @param	wait    �P�t���[�����̃E�G�C�g
 * @param	count   ����
 *
 * #param	COMBOBOX_TEXT	�c���	�����
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_SHAKE_VERTICAL	BTLEFF_CAMERA_SHAKE_HORIZON
 * #param	VALUE_FX32	��ꕝ
 * #param	VALUE_FX32	��ꕝ�I�t�Z�b�g�i�P��ꂲ�Ɓj
 * #param	VALUE_INT	�P���ɂ�����t���[��
 * #param	VALUE_INT	�P�t���[�����̃E�G�C�g
 * #param	VALUE_INT	����
 */
//======================================================================
	.macro	CAMERA_SHAKE	dir, value, offset, frame, wait, count
	.short	EC_CAMERA_SHAKE
	.long		\dir
	.long		\value
	.long		\offset
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief		�ˉe���[�h
 *
 * #param_num	1
 * @param	type				�ˉe�^�C�v
 *
 * #param	COMBOBOX_TEXT	���ˉe	�����ˉe
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_PROJECTION_ORTHO	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE
 */
//======================================================================
	.macro	CAMERA_PROJECTION	type
	.short	EC_CAMERA_PROJECTION
	.long		\type
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���f�[�^���[�h
 *
 * #param_num	1
 * @param	datID	�A�[�J�C�u�f�[�^��datID
 *
 * #param	FILE_DIALOG_WITH_ADD	.spa	�p�[�e�B�N���Đ�	�p�[�e�B�N���Đ��i���W�w��j	�G�~�b�^�ړ�
 */
//======================================================================
	.macro	PARTICLE_LOAD	datID
	.short	EC_PARTICLE_LOAD
	.long		\datID
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ�
 *
 * #param_num	7
 * @param	num				�Đ��p�[�e�B�N���i���o�[
 * @param	index			spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ����������ʒu
 * @param param1    ofs_y:�p�[�e�B�N���Đ�Y�����I�t�Z�b�g	dir_angle:�p�[�e�B�N���Đ�����Y�p�x dummy:���g�p
 * @param	param2		�Đ�����Ƃ��̕␳�l
 * @param	speed			�Đ�����Ƃ��̈ړ����x�␳�l
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB	BTLEFF_PARTICLE_PLAY_POS_A  BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C  BTLEFF_PARTICLE_PLAY_POS_D  BTLEFF_PARTICLE_PLAY_POS_E  BTLEFF_PARTICLE_PLAY_POS_F
 * #param	COMBOBOX_TEXT	��������	�U����	�h�䑤	�U�����|  �h�䑤�|  POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_NONE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK_MINUS  BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB BTLEFF_PARTICLE_PLAY_POS_A	BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C	BTLEFF_PARTICLE_PLAY_POS_D BTLEFF_PARTICLE_PLAY_POS_E BTLEFF_PARTICLE_PLAY_POS_F
 * #param VALUE_VECFX32 �p�[�e�B�N���Đ�Y�����I�t�Z�b�g �p�[�e�B�N���Đ�����Y�p�x ���g�p
 * #param	VALUE_VECFX32 �G�~�b�^���a �G�~�b�^���� �G�~�b�^�X�P�[��
 * #param VALUE_INIT  1:1:1
 * #param	VALUE_FX32 �G�~�b�^���x
 * #param VALUE_INIT  1
 */
//======================================================================
	.macro	PARTICLE_PLAY	num, index, start_pos, dir_pos, ofs_y, dir_angle, dummy, radius, life, scale, speed
	.short	EC_PARTICLE_PLAY
	.long		\num
	.long		\index
	.long		\start_pos
	.long		\dir_pos
	.long		\ofs_y
	.long		\dir_angle
	.long		\dummy
	.long		\radius
	.long		\life
	.long		\scale
	.long		\speed
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ��i���W�w��j
 *
 * #param_num	7
 * @param	num				�Đ��p�[�e�B�N���i���o�[
 * @param	index			spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ������ʒu
 * @param param1    ofs_y:�p�[�e�B�N���Đ�Y�����I�t�Z�b�g	dir_angle:�p�[�e�B�N���Đ�����Y�p�x dummy:���g�p
 * @param	param2		�Đ�����Ƃ��̕␳�l
 * @param	speed			�Đ�����Ƃ��̈ړ����x�␳�l
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	VALUE_VECFX32	POS_X	POS_Y	POS_Z
 * #param	VALUE_VECFX32	DIR_X	DIR_Y	DIR_Z
 * #param VALUE_VECFX32 �p�[�e�B�N���Đ�Y�����I�t�Z�b�g �p�[�e�B�N���Đ�����Y�p�x ���g�p
 * #param	VALUE_VECFX32 �G�~�b�^���a �G�~�b�^���� �G�~�b�^�X�P�[��
 * #param VALUE_INIT  1:1:1
 * #param	VALUE_FX32 �G�~�b�^���x
 * #param VALUE_INIT  1
 */
//======================================================================
	.macro	PARTICLE_PLAY_COORDINATE	num, index, start_pos_x, start_pos_y, start_pos_z, dir_pos_x, dir_pos_y, dir_pos_z, ofs_y, dir_angle, dummy, radius, life, scale, speed
	.short	EC_PARTICLE_PLAY_COORDINATE
	.long		\num
	.long		\index
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\dir_pos_x
	.long		\dir_pos_y
	.long		\dir_pos_z
	.long		\ofs_y
	.long		\dir_angle
	.long		\dummy
	.long		\radius
	.long		\life
	.long		\scale
	.long		\speed
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ��i���ˉe�j
 *
 * #param_num	7
 * @param	num				�Đ��p�[�e�B�N���i���o�[
 * @param	index			spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 * @param	dir	      �p�[�e�B�N���̌���
 * @param	ofs			  �p�[�e�B�N���Đ��I�t�Z�b�g
 * @param	param			�����ōĐ�����Ƃ��̕␳�l
 * @param	speed			�����ōĐ�����Ƃ��̈ړ����x�␳�l
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F �U����+�␳
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB	BTLEFF_PARTICLE_PLAY_POS_A  BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C  BTLEFF_PARTICLE_PLAY_POS_D  BTLEFF_PARTICLE_PLAY_POS_E  BTLEFF_PARTICLE_PLAY_POS_F  BTLEFF_PARTICLE_PLAY_SIDE_ATTACKOFS
 * #param	COMBOBOX_TEXT	��������	�U����	�h�䑤	�U�����|  �h�䑤�|  POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_NONE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK_MINUS  BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB BTLEFF_PARTICLE_PLAY_POS_A	BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C	BTLEFF_PARTICLE_PLAY_POS_D BTLEFF_PARTICLE_PLAY_POS_E BTLEFF_PARTICLE_PLAY_POS_F
 * #param	VALUE_VECFX32 ofs_x ofs_y ofs_z
 * #param	VALUE_VECFX32 �G�~�b�^���a �G�~�b�^���� �G�~�b�^�X�P�[��
 * #param VALUE_INIT  0.5:1:0.5
 * #param	VALUE_FX32 �G�~�b�^���x
 * #param VALUE_INIT  1
 */
//======================================================================
	.macro	PARTICLE_PLAY_ORTHO	num, index, start_pos, dir, ofs_x, ofs_y, ofs_z, radius, life, scale, speed
	.short	EC_PARTICLE_PLAY_ORTHO
	.long		\num
	.long		\index
	.long		\start_pos
	.long		\dir
	.long		\ofs_x
	.long		\ofs_y
	.long		\ofs_z
	.long		\radius
	.long		\life
	.long		\scale
	.long		\speed
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���f�[�^�폜
 *
 * #param_num	1
 * @param	num			�폜����p�[�e�B�N���i���o�[
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 */
//======================================================================
	.macro	PARTICLE_DELETE	num
	.short	EC_PARTICLE_DELETE
	.long		\num
	.endm

//======================================================================
/**
 * @brief	�G�~�b�^�ړ�
 *
 * #param_num	7
 * @param	num					�Đ��p�[�e�B�N���i���o�[
 * @param	index				spa���C���f�b�N�X�i���o�[
 * @param	move_type		�ړ��^�C�v�i�����A�������j
 * @param	start_pos		�ړ��J�n�����ʒu
 * @param	end_pos			�ړ��I�������ʒu
 * @param	move_param	�����ʒuY�����I�t�Z�b�g(ofs_y)	�ړ��t���[��(move_frame)	���������_�i���������̂݁j(top)
 * @param	param		    �Đ�����Ƃ��̕␳�l  life speed
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	����	������  �������i�����j
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	VALUE_VECFX32	�����ʒuY�����I�t�Z�b�g	�ړ��t���[��	���������_�i���������̂݁j
 * #param VALUE_VECFX32 �G�~�b�^�����@�G�~�b�^�X�s�[�h  ���g�p
 * #param VALUE_INIT  1:1:0
 */
//======================================================================
	.macro	EMITTER_MOVE	num, index, move_type, start_pos, end_pos, ofs_y, move_frame, top, life, speed, dummy
	.short	EC_EMITTER_MOVE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.long		\life
	.long		\speed
	.long		\dummy
	.endm

//======================================================================
/**
 * @brief	�G�~�b�^�ړ��i���W�w��j
 *
 * #param_num	7
 * @param	num					�Đ��p�[�e�B�N���i���o�[
 * @param	index				spa���C���f�b�N�X�i���o�[
 * @param	move_type		�ړ��^�C�v�i�����A�������j
 * @param	start_pos		�ړ��J�n���W
 * @param	end_pos			�ړ��I�������ʒu
 * @param	move_param	�����ʒuY�����I�t�Z�b�g(ofs_y)	�ړ��t���[��(move_frame)	���������_�i���������̂݁j(top)
 * @param	param		    �Đ�����Ƃ��̕␳�l  life speed
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	����	������  �������i�����j
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
 * #param	VALUE_VECFX32	X���W	Y���W	Z���W
 * #param	COMBOBOX_TEXT	�U����	�h�䑤 POS_AA POS_BB POS_A POS_B POS_C POS_D  POS_E POS_F
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB	BTLEFF_PARTICLE_PLAY_POS_A	BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C	BTLEFF_PARTICLE_PLAY_POS_D BTLEFF_PARTICLE_PLAY_POS_E BTLEFF_PARTICLE_PLAY_POS_F
 * #param	VALUE_VECFX32	�����ʒuY�����I�t�Z�b�g	�ړ��t���[��	���������_�i���������̂݁j
 * #param VALUE_VECFX32 �G�~�b�^�����@�G�~�b�^�X�s�[�h  ���g�p
 * #param VALUE_INIT  1:1:0
 */
//======================================================================
	.macro	EMITTER_MOVE_COORDINATE	num, index, move_type, start_pos_x, start_pos_y, start_pos_z, end_pos, ofs_y, move_frame, top, life, speed, dummy
	.short	EC_EMITTER_MOVE_COORDINATE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.long		\life
	.long		\speed
	.long		\dummy
	.endm

//======================================================================
/**
 * @brief	�G�~�b�^�~�ړ�
 *
 * #param_num	6
 * @param	num					      �Đ��p�[�e�B�N���i���o�[
 * @param	index				      spa���C���f�b�N�X�i���o�[
 * @param center_pos        �~�^���̒��S
 * @param param             ���������a:�c�������a:Y�����I�t�Z�b�g
 * @param	rotate_param			��]�t���[�����i1��]���t���[���ł��邩�j:�ړ��E�G�C�g:��]�J�E���g
 * @param	rotate_after_wait 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param COMBOBOX_TEXT �U������  �U�����E  �h�䑤��  �h�䑤�E  ��ʒ�����  ��ʒ����E
 * #param COMBOBOX_VALUE  BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_L BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_R BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_L BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_R BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_L BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_R
 * #param VALUE_VECFX32  ���������a �c�������a Y�����I�t�Z�b�g
 * #param	VALUE_VECINT		��]�t���[����  �ړ��E�G�C�g  ��]�J�E���g
 * #param	VALUE_INT 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 */
//======================================================================
  .macro  EMITTER_CIRCLE_MOVE num, index, center_pos, radius_h, radius_v, offset_y, frame, wait, count, rotate_after_wait
  .short  EC_EMITTER_CIRCLE_MOVE
  .long   \num
  .long   \index
  .long   \center_pos
  .long   \radius_h
  .long   \radius_v
  .long   \offset_y
  .long   \frame
  .long   \wait
  .long   \count
  .long   \rotate_after_wait
  .endm

//======================================================================
/**
 * @brief	�G�~�b�^�~�ړ��i���ˉe�j
 *
 * #param_num	6
 * @param	num					      �Đ��p�[�e�B�N���i���o�[
 * @param	index				      spa���C���f�b�N�X�i���o�[
 * @param center_pos        �~�^���̒��S
 * @param param             ���������a:�c�������a:Y�����I�t�Z�b�g
 * @param	rotate_param			��]�t���[�����i1��]���t���[���ł��邩�j:�ړ��E�G�C�g:��]�J�E���g
 * @param	rotate_after_wait 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param COMBOBOX_TEXT �U������  �U�����E  �h�䑤��  �h�䑤�E  ��ʒ�����  ��ʒ����E
 * #param COMBOBOX_VALUE  BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_L BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_R BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_L BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_R BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_L BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_R
 * #param VALUE_VECFX32  ���������a �c�������a Y�����I�t�Z�b�g
 * #param	VALUE_VECINT		��]�t���[����  �ړ��E�G�C�g  ��]�J�E���g
 * #param	VALUE_INT 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 */
//======================================================================
  .macro  EMITTER_CIRCLE_MOVE_ORTHO num, index, center_pos, radius_h, radius_v, offset_y, frame, wait, count, rotate_after_wait
  .short  EC_EMITTER_CIRCLE_MOVE_ORTHO
  .long   \num
  .long   \index
  .long   \center_pos
  .long   \radius_h
  .long   \radius_v
  .long   \offset_y
  .long   \frame
  .long   \wait
  .long   \count
  .long   \rotate_after_wait
  .endm

//======================================================================
/**
 * @brief	�|�P�����ړ�
 *
 * #param_num	7
 * @param	pos					�ړ�������|�P�����̗����ʒu
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O  �����ʒu
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_MOVE_DIRECT	BTLEFF_POKEMON_MOVE_INTERPOLATION	BTLEFF_POKEMON_MOVE_ROUNDTRIP	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG  BTLEFF_POKEMON_MOVE_INIT
 * #param	VALUE_FX32	�ړ���X���W
 * #param	VALUE_FX32	�ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_POKEMON_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����~�ړ�
 *
 * #param_num	7
 * @param	pos				      	�~�ړ�������|�P�����̗����ʒu
 * @param	axis			      	��]��
 * @param	shift			      	��]�V�t�g
 * @param	radius_h	        ���������a
 * @param	radius_v	        �c�������a
 * @param	rotate_param			��]�t���[�����i1��]���t���[���ł��邩�j:�ړ��E�G�C�g:��]�J�E���g
 * @param	rotate_after_wait 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�w����  �w���E  �x����  �x���E  �y����  �y���E
 * #param	COMBOBOX_VALUE	BTLEFF_AXIS_X_L BTLEFF_AXIS_X_R BTLEFF_AXIS_Y_L BTLEFF_AXIS_Y_R BTLEFF_AXIS_Z_L BTLEFF_AXIS_Z_R
 * #param	COMBOBOX_TEXT �V�t�g�g�{  �V�t�g�g�|  �V�t�g�u�{  �V�t�g�u�|
 * #param	COMBOBOX_VALUE	BTLEFF_SHIFT_H_P BTLEFF_SHIFT_H_M BTLEFF_SHIFT_V_P BTLEFF_SHIFT_V_M
 * #param	VALUE_FX32	���������a
 * #param	VALUE_FX32	�c�������a
 * #param	VALUE_VECFX32		��]�t���[����  �ړ��E�G�C�g  ��]�J�E���g
 * #param	VALUE_INT 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 */
//======================================================================
	.macro	POKEMON_CIRCLE_MOVE	pos, axis, shift, radius_h, radius_v, frame, rotate_wait, count, rotate_after_wait
	.short	EC_POKEMON_CIRCLE_MOVE
	.long		\pos
	.long		\axis
	.long		\shift
	.long		\radius_h
	.long		\radius_v
	.long		\frame
	.long		\rotate_wait
	.long		\count
	.long		\rotate_after_wait
	.endm

//======================================================================
/**
 * @brief	�|�P�����g�k
 *
 * #param_num	7
 * @param	pos			�g�k������|�P�����̗����ʒu
 * @param	type		�g�k�^�C�v
 * @param	scale_x	X�����g�k��
 * @param	scale_y	Y�����g�k��
 * @param	frame		�g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�g�k�E�G�C�g
 * @param	count		�����J�E���g�i�g�k�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SCALE_DIRECT	BTLEFF_POKEMON_SCALE_INTERPOLATION	BTLEFF_POKEMON_SCALE_ROUNDTRIP	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	X�����g�k��
 * #param	VALUE_FX32	Y�����g�k��
 * #param	VALUE_INT		�g�k�t���[����
 * #param	VALUE_INT		�g�k�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_SCALE	pos, type, scale_x, scale_y, frame, wait, count
	.short	EC_POKEMON_SCALE
	.long		\pos
	.long		\type
	.long		\scale_x
	.long		\scale_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P������]
 *
 * #param_num	6
 * @param	pos			��]������|�P�����̗����ʒu
 * @param	type		��]�^�C�v
 * @param	rotate	��]�p�x
 * @param	frame		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		��]�E�G�C�g
 * @param	count		�����J�E���g�i��]�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ROTATE_DIRECT	BTLEFF_POKEMON_ROTATE_INTERPOLATION	BTLEFF_POKEMON_ROTATE_ROUNDTRIP	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG
 * #param	VALUE_ANGLE	��]�p�x
 * #param	VALUE_INT		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		��]�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_ROTATE	pos, type, rotate, frame, wait, count
	.short	EC_POKEMON_ROTATE
	.long		\pos
	.long		\type
	.long		\rotate
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�������l
 *
 * #param_num	6
 * @param	pos			���l�Z�b�g����|�P�����̗����ʒu
 * @param	type		���l�^�C�v
 * @param	alpha	  ���l
 * @param	frame		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�E�G�C�g
 * @param	count		�����J�E���g�i���l�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ALPHA_DIRECT	BTLEFF_POKEMON_ALPHA_INTERPOLATION	BTLEFF_POKEMON_ALPHA_ROUNDTRIP	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG
 * #param	VALUE_INT	  ���l
 * #param	VALUE_INT		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_ALPHA	pos, type, alpha, frame, wait, count
	.short	EC_POKEMON_ALPHA
	.long		\pos
	.long		\type
	.long		\alpha
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�������U�C�N
 *
 * #param_num	6
 * @param	pos			���U�C�N����|�P�����̗����ʒu
 * @param	type		���U�C�N�^�C�v
 * @param	alpha	  ���U�C�N�l
 * @param	frame		�t���[�����i�ݒ肵�����U�C�N�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�E�G�C�g
 * @param	count		�����J�E���g�i���U�C�N�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ALPHA_DIRECT	BTLEFF_POKEMON_ALPHA_INTERPOLATION	BTLEFF_POKEMON_ALPHA_ROUNDTRIP	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG
 * #param	VALUE_INT	  ���U�C�N�l
 * #param	VALUE_INT		�t���[�����i�ݒ肵�����U�C�N�l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		�E�G�C�g
 * #param VALUE_INIT  1
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_MOSAIC	pos, type, mosaic, frame, wait, count
	.short	EC_POKEMON_MOSAIC
	.long		\pos
	.long		\type
	.long		\mosaic
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�������p�`
 *
 * #param_num	4
 * @param	pos			���p�`������|�P�����̗����ʒu
 * @param	type		���p�`�^�C�v
 * @param	wait		�܂΂����E�G�C�g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 * @param	count		�܂΂����J�E���g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	����	�J����	�܂΂���
 * #param	COMBOBOX_VALUE	BTLEFF_MEPACHI_ON	BTLEFF_MEPACHI_OFF	BTLEFF_MEPACHI_MABATAKI
 * #param	VALUE_INT	�܂΂����E�G�C�g�i�܂΂������L���j
 * #param	VALUE_INT	�܂΂����J�E���g�i�܂΂������L���j
 */
//======================================================================
	.macro	POKEMON_SET_MEPACHI_FLAG	pos, type, wait, count
	.short	EC_POKEMON_SET_MEPACHI_FLAG
	.long		\pos
	.long		\type
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����A�j��
 *
 * #param_num	2
 * @param	pos			�A�j�����삷��|�P�����̗����ʒu
 * @param	flag		�A�j���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�A�j���X�g�b�v	�A�j���X�^�[�g
 * #param	COMBOBOX_VALUE	BTLEFF_ANM_STOP	BTLEFF_ANM_START
 */
//======================================================================
	.macro	POKEMON_SET_ANM_FLAG	pos, flag
	.short	EC_POKEMON_SET_ANM_FLAG
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�|�P�����p���b�g�t�F�[�h
 *
 * #param_num	5
 * @param	pos				�p���b�g�t�F�[�h����|�P�����̗����ʒu
 * @param	start_evy	START_EVY�l
 * @param	end_evy		END_EVY�l
 * @param	wait			�E�G�C�g
 * @param	rgb				�J�nor�I�����̐F
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	VALUE_INT	START_EVY�l
 * #param	VALUE_INT	END_EVY�l
 * #param	VALUE_INT	�E�G�C�g
 * #param	VALUE_VECFX32	R�l(0-31)	G�l(0-31)	B�l(0-31)
 */
//======================================================================
	.macro	POKEMON_PAL_FADE	pos,	start_evy,	end_evy,	wait,	r, g, b
	.short	EC_POKEMON_PAL_FADE
	.long		\pos
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	�|�P�����o�j�b�V��
 *
 * #param_num	2
 * @param	pos			�o�j�b�V�����삷��|�P�����̗����ʒu
 * @param	flag		�o�j�b�V���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�o�j�b�V���I��	�o�j�b�V���I�t
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_VANISH	pos, flag
	.short	EC_POKEMON_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�|�P�����e�o�j�b�V��
 *
 * #param_num	2
 * @param	pos			�o�j�b�V�����삷��|�P�����̗����ʒu
 * @param	flag		�o�j�b�V���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�o�j�b�V���I��	�o�j�b�V���I�t
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_SHADOW_VANISH	pos, flag
	.short	EC_POKEMON_SHADOW_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�|�P�����폜
 *
 * #param_num	1
 * @param	index		�폜����|�P�����̗����ʒu
 *
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE
 */
//======================================================================
	.macro	POKEMON_DEL	pos
	.short	EC_POKEMON_DEL
	.long		\pos
	.endm


//======================================================================
/**
 * @brief	�g���[�i�[�Z�b�g
 *
 * #param_num	3
 * @param	trtype		�Z�b�g����g���[�i�[�^�C�v
 * @param	position	�Z�b�g����g���[�i�[�̗����ʒu
 * @param	pos			  �Z�b�g������W(X:Y:Z)
 *
 * #param	VALUE_INT	�g���[�i�[�^�C�v
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	VALUE_VECFX32	X���W Y���W Z���W
 */
//======================================================================
	.macro	TRAINER_SET	index, position, pos_x, pos_y, pos_z
	.short	EC_TRAINER_SET
	.long		\index
	.long		\position
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�ړ�
 *
 * #param_num	7
 * @param	index				�ړ�������g���[�i�[�̗����ʒu
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_TRAINER_MOVE_DIRECT	BTLEFF_TRAINER_MOVE_INTERPOLATION	BTLEFF_TRAINER_MOVE_ROUNDTRIP	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_INT		�ړ���X���W
 * #param	VALUE_INT		�ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	TRAINER_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_TRAINER_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�A�j���Z�b�g
 *
 * #param_num	2
 * @param	pos			�A�j���Z�b�g����g���[�i�[�̗����ʒu
 * @param	anm_no	�Z�b�g����A�j���i���o�[
 *
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	VALUE_INT		�A�j���i���o�[
 */
//======================================================================
	.macro	TRAINER_ANIME_SET	pos, anm_no
	.short	EC_TRAINER_ANIME_SET
	.long		\pos
	.long		\anm_no
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�폜
 *
 * #param_num	1
 * @param	index		�폜����g���[�i�[�̗����ʒu
 *
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 */
//======================================================================
	.macro	TRAINER_DEL	pos
	.short	EC_TRAINER_DEL
	.long		\pos
	.endm

//======================================================================
/**
 * @brief	BG�̃��[�h
 *
 * #param_num	1
 * @param	datID	�A�[�J�C�u�f�[�^��datID
 *
 * #param	FILE_DIALOG	.NSCR
 */
//======================================================================
	.macro	BG_LOAD	datID
	.short	EC_BG_LOAD
	.long		\datID
	.endm

//======================================================================
/**
 * @brief	BG�̃X�N���[��
 *
 * #param_num	6
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	���  �_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_BG_SCROLL_EVERY  BTLEFF_BG_SCROLL_DIRECT	BTLEFF_BG_SCROLL_INTERPOLATION	BTLEFF_BG_SCROLL_ROUNDTRIP	BTLEFF_BG_SCROLL_ROUNDTRIP_LONG
 * #param	VALUE_INT		�ړ���X���W
 * #param	VALUE_INT		�ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	BG_SCROLL	type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_BG_SCROLL
	.long   \type
  .long   \move_pos_x
  .long   \move_pos_y
  .long   \frame
  .long   \wait
  .long   \count
	.endm

//======================================================================
/**
 * @brief	BG�̃��X�^�[�X�N���[��
 *
 * #param_num	4
 * @param	type		���X�^�[�^�C�v
 * @param	radius  �T�C���J�[�u�̔��a
 * @param	line	  �T�C���J�[�u1���������C���ɂ��邩
 * @param	wait		�E�G�C�g
 *
 * #param	COMBOBOX_TEXT ��  �c
 * #param	COMBOBOX_VALUE  BTLEFF_BG_RASTER_SCROLL_H BTLEFF_BG_RASTER_SCROLL_V
 * #param	VALUE_INT �T�C���J�[�u�̔��a
 * #param	VALUE_INT �T�C���J�[�u1���������C���ɂ��邩
 * #param	VALUE_INT �E�G�C�g
 */
//======================================================================
  .macro  BG_RASTER_SCROLL  type, radius, line, wait
  .short  EC_BG_RASTER_SCROLL
  .long   \type
  .long   \radius
  .long   \line
  .long   \wait
  .endm

//======================================================================
/**
 * @brief	BG�̃p���b�g�A�j��
 *
 * #param_num	5
 * @param	datID     �A�j���p�p���b�g�̃A�[�J�C�u�f�[�^ID
 * @param	trans_pal �����p���b�g
 * @param	col_count ����J���[��
 * @param	offset		���茳�p���b�g�̍X�V�I�t�Z�b�g
 * @param	wait		  �E�G�C�g
 *
 * #param	VALUE_INT �A�j���p�p���b�g�̃A�[�J�C�u�f�[�^ID
 * #param	VALUE_INT �����p���b�g
 * #param	VALUE_INT ����J���[��
 * #param	VALUE_INT ���茳�p���b�g�̍X�V�I�t�Z�b�g
 * #param	VALUE_INT �E�G�C�g
 */
//======================================================================
  .macro  BG_PAL_ANM  datID, trans_pal, col_count, offset, wait
  .short  EC_BG_PAL_ANM
  .long   \datID
  .long   \trans_pal
  .long   \col_count
  .long   \offset
  .long   \wait
  .endm

//======================================================================
/**
 * @brief	BG�̃v���C�I���e�B
 *
 * #param_num	1
 * @param	pri	BG�̃v���C�I���e�B
 *
 * #param	COMBOBOX_TEXT �O��  �w��
 * #param	COMBOBOX_VALUE BTLEFF_BG_PRI_FRONT  BTLEFF_BG_PRI_BACK
 */
//======================================================================
  .macro  BG_PRIORITY pri
  .short  EC_BG_PRIORITY
  .long   \pri
  .endm

//======================================================================
/**
 * @brief	BG���l
 *
 * #param_num	6
 * @param	bg_num	���l�Z�b�g����BG�̎��
 * @param	type		���l�^�C�v
 * @param	alpha	  ���l
 * @param	frame		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�E�G�C�g
 * @param	count		�����J�E���g�i���l�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT ���~  �t�B�[���h  ���~�{�t�B�[���h  �G�t�F�N�g��  ���ׂ�
 * #param	COMBOBOX_VALUE  BTLEFF_PAL_FADE_STAGE BTLEFF_PAL_FADE_FIELD BTLEFF_PAL_FADE_3D  BTLEFF_PAL_FADE_EFFECT BTLEFF_PAL_FADE_ALL
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_BG_ALPHA_DIRECT	BTLEFF_BG_ALPHA_INTERPOLATION	BTLEFF_BG_ALPHA_ROUNDTRIP	BTLEFF_BG_ALPHA_ROUNDTRIP_LONG
 * #param	VALUE_INT	  ���l
 * #param	VALUE_INT		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
  .macro  BG_ALPHA  bg_num, type, alpha, frame, wait, count
  .short  EC_BG_ALPHA
  .long   \bg_num
  .long   \type
  .long   \alpha
  .long   \frame
  .long   \wait
  .long   \count
  .endm

//======================================================================
/**
 * @brief	BG�̃p���b�g�t�F�[�h
 *
 * #param_num	5
 * @param	bg_num    ���삷��w�i
 * @param	start_evy	START_EVY�l
 * @param	end_evy		END_EVY�l
 * @param	wait			�E�G�C�g
 * @param	rgb				�J�nor�I�����̐F
 *
 * #param	COMBOBOX_TEXT ���~  �t�B�[���h  ���~�{�t�B�[���h  �G�t�F�N�g��  ���ׂ�
 * #param	COMBOBOX_VALUE  BTLEFF_PAL_FADE_STAGE BTLEFF_PAL_FADE_FIELD BTLEFF_PAL_FADE_3D  BTLEFF_PAL_FADE_EFFECT BTLEFF_PAL_FADE_ALL
 * #param	VALUE_INT	START_EVY�l
 * #param	VALUE_INT	END_EVY�l
 * #param	VALUE_INT	�E�G�C�g
 * #param	VALUE_VECFX32	R�l(0-31)	G�l(0-31)	B�l(0-31)
 */
//======================================================================
	.macro	BG_PAL_FADE bg_num, start_evy, end_evy, wait, r, g, b
	.short	EC_BG_PAL_FADE
	.long		\bg_num
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	BG�̕\��/��\��
 *
 * #param_num	2
 * @param	bg_num	���삷��w�i
 * @param	sw			�\��/��\�����w��
 *
 * #param	COMBOBOX_TEXT	���~  �t�B�[���h  �G�t�F�N�g
 * #param	COMBOBOX_VALUE  BTLEFF_STAGE  BTLEFF_FIELD  BTLEFF_EFFECT
 * #param	COMBOBOX_TEXT ��\��  �\��
 * #param	COMBOBOX_VALUE  BTLEFF_VANISH_ON BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	BG_VISIBLE	bg_num, sw
	.short	EC_BG_VANISH
	.long		\bg_num
	.long		\sw
	.endm

//======================================================================
/**
 * @brief	OBJ�̃Z�b�g
 *
 * #param_num	7
 * @param	index   �Ǘ��p�C���f�b�N�X�i0-3�j
 * @param	datID   �A�[�J�C�u�f�[�^��datID
 * @param pos     �\��������W
 * @param ofs_x   �I�t�Z�b�gX���W
 * @param ofs_y   �I�t�Z�b�gY���W
 * @param scale_x �I�t�Z�b�gX���W
 * @param scale_y �I�t�Z�b�gY���W
 *
 * #param	VALUE_INT �C���f�b�N�X
 * #param	FILE_DIALOG .NCGR
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param VALUE_FX32  �I�t�Z�b�gX���W
 * #param VALUE_FX32  �I�t�Z�b�gY���W
 * #param VALUE_FX32  �X�P�[��X�l
 * #param VALUE_FX32  �X�P�[��Y�l
 */
//======================================================================
  .macro  OBJ_SET index, datID, pos, ofs_x, ofs_y, scale_x, scale_y
  .short  EC_OBJ_SET
  .long   \index
  .long   \datID
  .long   \pos
  .long   \ofs_x
  .long   \ofs_y
  .long   \scale_x
  .long   \scale_y
  .endm

//======================================================================
/**
 * @brief	OBJ�ړ�
 *
 * #param_num	7
 * @param	index				�ړ�������OBJ�̊Ǘ��C���f�b�N�X
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	VALUE_INT		OBJ�̊Ǘ��C���f�b�N�X
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_OBJ_MOVE_DIRECT	BTLEFF_OBJ_MOVE_INTERPOLATION	BTLEFF_OBJ_MOVE_ROUNDTRIP	BTLEFF_OBJ_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_INT   �ړ���X���W
 * #param	VALUE_INT   �ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	OBJ_MOVE	index, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_OBJ_MOVE
	.long		\index
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	OBJ�g�k
 *
 * #param_num	7
 * @param	index		�g�k������OBJ�̊Ǘ��C���f�b�N�X
 * @param	type		�g�k�^�C�v
 * @param	scale_x	X�����g�k��
 * @param	scale_y	Y�����g�k��
 * @param	frame		�g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�g�k�E�G�C�g
 * @param	count		�����J�E���g�i�g�k�^�C�v�������̂Ƃ������L���j
 *
 * #param	VALUE_INT		OBJ�̊Ǘ��C���f�b�N�X
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_OBJ_SCALE_DIRECT	BTLEFF_OBJ_SCALE_INTERPOLATION	BTLEFF_OBJ_SCALE_ROUNDTRIP	BTLEFF_OBJ_SCALE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	X�����g�k��
 * #param	VALUE_FX32	Y�����g�k��
 * #param	VALUE_INT		�g�k�t���[����
 * #param	VALUE_INT		�g�k�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	OBJ_SCALE	index, type, scale_x, scale_y, frame, wait, count
	.short	EC_OBJ_SCALE
	.long		\index
	.long		\type
	.long		\scale_x
	.long		\scale_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	OBJ�A�j���Z�b�g
 *
 * #param_num	2
 * @param	index		�A�j���Z�b�g����OBJ�̊Ǘ��C���f�b�N�X
 * @param	anm_no	�Z�b�g����A�j���i���o�[
 *
 * #param	VALUE_INT		OBJ�̊Ǘ��C���f�b�N�X
 * #param	VALUE_INT		�A�j���i���o�[
 */
//======================================================================
	.macro	OBJ_ANIME_SET	pos, anm_no
	.short	EC_OBJ_ANIME_SET
	.long		\pos
	.long		\anm_no
	.endm

//======================================================================
/**
 * @brief	OBJ�p���b�g�t�F�[�h
 *
 * #param_num	5
 * @param	index     ���삷��OBJ�̊Ǘ��C���f�b�N�X
 * @param	start_evy	START_EVY�l
 * @param	end_evy		END_EVY�l
 * @param	wait			�E�G�C�g
 * @param	rgb				�J�nor�I�����̐F
 *
 * #param	VALUE_INT		OBJ�̊Ǘ��C���f�b�N�X
 * #param	VALUE_INT	START_EVY�l
 * #param	VALUE_INT	END_EVY�l
 * #param	VALUE_INT	�E�G�C�g
 * #param	VALUE_VECFX32	R�l(0-31)	G�l(0-31)	B�l(0-31)
 */
//======================================================================
	.macro	OBJ_PAL_FADE index, start_evy, end_evy, wait, r, g, b
	.short	EC_OBJ_PAL_FADE
	.long		\index
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	OBJ�폜
 *
 * #param_num	1
 * @param	index		�폜����OBJ�̊Ǘ��C���f�b�N�X
 *
 * #param	VALUE_INT		OBJ�̊Ǘ��C���f�b�N�X
 */
//======================================================================
	.macro	OBJ_DEL	pos
	.short	EC_OBJ_DEL
	.long		\pos
	.endm

//======================================================================
/**
 * @brief	SE�Đ�
 *
 * #param_num	7
 * @param	se_no	      �Đ�����SE�i���o�[
 * @param player      �Đ�����PlayerNo
 * @param wait        �Đ��܂ł̃E�G�C�g
 * @param pitch       �Đ��s�b�`
 * @param vol         �Đ��{�����[��
 * @param mod_depth   �Đ����W�����[�V�����f�v�X
 * @param mod_speed   �Đ����W�����[�V�����X�s�[�h
 *
 * #param	VALUE_INT   �Đ�����SE�i���o�[
 * #param COMBOBOX_TEXT �f�t�H���g  SE1 SE2 SE3 PSG SYSTEM
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_DEFAULT  BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2 BTLEFF_SEPLAY_SE3 BTLEFF_SEPLAY_PSG BTLEFF_SEPLAY_SYSTEM
 * #param VALUE_INT   �Đ��܂ł̃E�G�C�g
 * #param VALUE_INT   �Đ��s�b�`
 * #param VALUE_INT   �Đ��{�����[��
 * #param VALUE_INIT  127
 * #param VALUE_INT   �Đ����W�����[�V�����f�v�X
 * #param VALUE_INT   �Đ����W�����[�V�����X�s�[�h
 */
//======================================================================
	.macro	SE_PLAY	se_no, player, wait, pitch, vol, mod_depth, mod_speed
	.short	EC_SE_PLAY
	.long		\se_no
	.long   \player
	.long   \wait
  .long   \pitch
  .long   \vol
  .long   \mod_depth
  .long   \mod_speed
	.endm

//======================================================================
/**
 * @brief	SE�X�g�b�v
 *
 * #param_num	1
 * @param player  �X�g�b�v����PlayerNo
 *
 * #param COMBOBOX_TEXT ���ׂ�  SE1 SE2 SE3 PSG SYSTEM
 * #param COMBOBOX_VALUE BTLEFF_SESTOP_ALL  BTLEFF_SESTOP_SE1 BTLEFF_SESTOP_SE2 BTLEFF_SESTOP_SE3 BTLEFF_SESTOP_PSG BTLEFF_SESTOP_SYSTEM
 */
//======================================================================
	.macro	SE_STOP player
	.short	EC_SE_STOP
  .long   \player
	.endm

//======================================================================
/**
 * @brief	SE�p��
 *
 * #param_num	6
 * @param player      �Z�b�g����PlayerNo
 * @param type        �ω��^�C�v
 * @param start       �J�n�l
 * @param end         �I���l
 * @param start_wait  �J�n�E�G�C�g
 * @param para        start����end�܂ł̃t���[����  �t���[���Ԃ̃E�G�C�g  �������̉�
 *
 * #param COMBOBOX_TEXT SE1 SE2 SE3 PSG SYSTEM
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2 BTLEFF_SEPLAY_SE3  BTLEFF_SEPLAY_PSG  BTLEFF_SEPLAY_SYSTEM
 * #param COMBOBOX_TEXT �Ǐ]  ����
 * #param COMBOBOX_VALUE  BTLEFF_SEPAN_INTERPOLATION BTLEFF_SEPAN_ROUNDTRIP
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_SEPAN_ATTACK BTLEFF_SEPAN_DEFENCE
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_SEPAN_ATTACK BTLEFF_SEPAN_DEFENCE
 * #param VALUE_INT �J�n�E�G�C�g
 * #param VALUE_VECINT start����end�܂ł̃t���[����  �t���[���Ԃ̃E�G�C�g  ������
 * #param VALUE_INIT  60:0:0
 */
//======================================================================
	.macro	SE_PAN player, type, start, end, start_wait, frame, wait, count
	.short	EC_SE_PAN
  .long   \player
  .long   \type
  .long   \start
  .long   \end
  .long   \start_wait
  .long   \frame
  .long   \wait
  .long   \count
	.endm

//======================================================================
/**
 * @brief	SE���I�ω�
 *
 * #param_num	7
 * @param player      �ύX����PlayerNo
 * @param type        �ω��^�C�v
 * @param param       �ω�������p�����[�^
 * @param start       �J�n�l
 * @param end         �I���l
 * @param start_wait  �J�n�E�G�C�g
 * @param para        start����end�܂ł̃t���[����  �t���[���Ԃ̃E�G�C�g  �������̉�
 *
 * #param COMBOBOX_TEXT SE1 SE2 SE3 PSG SYSTEM
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2 BTLEFF_SEPLAY_SE3  BTLEFF_SEPLAY_PSG  BTLEFF_SEPLAY_SYSTEM
 * #param COMBOBOX_TEXT �Ǐ]  ����
 * #param COMBOBOX_VALUE BTLEFF_SEEFFECT_INTERPOLATION BTLEFF_SEEFFECT_ROUNDTRIP
 * #param COMBOBOX_TEXT �s�b�`  �{�����[��  �p��
 * #param COMBOBOX_VALUE BTLEFF_SEEFFECT_PITCH  BTLEFF_SEEFFECT_VOLUME  BTLEFF_SEEFFECT_PAN
 * #param VALUE_INT �J�n�l
 * #param VALUE_INT �I���l
 * #param VALUE_INT �J�n�E�G�C�g
 * #param VALUE_VECINT start����end�܂ł̃t���[����  �t���[���Ԃ̃E�G�C�g  ������
 *
 */
//======================================================================
	.macro	SE_EFFECT player, type, param, start, end, start_wait, frame, wait, count
	.short	EC_SE_EFFECT
  .long   \player
  .long   \type
  .long   \param
  .long   \start
  .long   \end
  .long   \start_wait
  .long   \frame
  .long   \wait
  .long   \count
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�I���҂�
 *
 * #param_num	1
 * @param	kind	�I���҂�������
 *
 * #param	COMBOBOX_TEXT	���ׂ�	�J����	�p�[�e�B�N��	�|�P����	�g���[�i�[  BG  PALFADE���~  PALFADE�t�B�[���h PALFADE���~�{�t�B�[���h  PALFADE�G�t�F�N�g SEPLAY���ׂ�  SEPLAY1 SEPLAY2 SEPLAY3 SEPLAY_PSG SEPLAY_SYSTEM
 * #param COMBOBOX_VALUE	BTLEFF_EFFENDWAIT_ALL	BTLEFF_EFFENDWAIT_CAMERA	BTLEFF_EFFENDWAIT_PARTICLE	BTLEFF_EFFENDWAIT_POKEMON	BTLEFF_EFFENDWAIT_TRAINER BTLEFF_EFFENDWAIT_BG  BTLEFF_EFFENDWAIT_PALFADE_STAGE  BTLEFF_EFFENDWAIT_PALFADE_FIELD  BTLEFF_EFFENDWAIT_PALFADE_3D BTLEFF_EFFENDWAIT_PALFADE_EFFECT BTLEFF_EFFENDWAIT_SEALL  BTLEFF_EFFENDWAIT_SE1  BTLEFF_EFFENDWAIT_SE2 BTLEFF_EFFENDWAIT_SE3 BTLEFF_EFFENDWAIT_PSG BTLEFF_EFFENDWAIT_SYSTEM
 */
//======================================================================
	.macro	EFFECT_END_WAIT	kind
	.short	EC_EFFECT_END_WAIT
	.long		\kind
	.endm

//======================================================================
/**
 * @brief	�^�C�}�[�ɂ��E�G�C�g
 *
 * #param_num	1
 * @param		wait	�E�G�C�g����
 *
 * #param	VALUE_INT	�E�G�C�g����
 */
//======================================================================
	.macro	WAIT	wait
	.short	EC_WAIT
	.long		\wait
	.endm

//======================================================================
/**
 * @brief	���䃂�[�h�ύX
 *
 * #param_num	1
 * @param		mode	���䃂�[�h�i�p���F���[�h���؂�ւ�邩�A�G�t�F�N�g�I���܂��܂ŏ������p�����Ď��s�@�����F1�s���ƂɎ��s�j
 *
 * #param	COMBOBOX_TEXT	�p��	����
 * #param	COMBOBOX_VALUE	BTLEFF_CONTROL_MODE_CONTINUE	BTLEFF_CONTROL_MODE_SUSPEND
 */
//======================================================================
	.macro	CONTROL_MODE	mode
	.short	EC_CONTROL_MODE
	.long		\mode
	.endm

//======================================================================
/**
 * @brief	�w�肳�ꂽ���[�N�����ď�������
 *
 * #param_num	4
 * @param		work  ��������Ɏg�����[�N
 * @param		cond  ������
 * @param		value ��r����l
 * @param		adrs  ��ѐ�
 *
 * #param COMBOBOX_TEXT WAZA_RANGE TURN_COUNT CONTINUE_COUNT YURE_CNT GET_SUCCESS ITEM_NO POS_AA_WEIGHT POS_BB_WEIGHT POS_A_WEIGHT POS_B_WEIGHT POS_C_WEIGHT POS_D_WEIGHT POS_E_WEIGHT POS_F_WEIGHT �U�����̏d  SEQUENCE_WORK ATTACK_POKEMON_VANISH �U��������  POS_AA_RARE POS_BB_RARE POS_A_RARE POS_B_RARE POS_C_RARE POS_D_RARE POS_E_RARE POS_F_RARE �U�������A  POS_AA_JUMP POS_BB_JUMP POS_A_JUMP  POS_B_JUMP  POS_C_JUMP  POS_D_JUMP  POS_E_JUMP  POS_F_JUMP  �U�������˂�  �}���`  TRTYPE_A  TRTYPE_B  TRTYPE_C  TRTYPE_D
 * #param COMBOBOX_VALUE BTLEFF_WORK_WAZA_RANGE BTLEFF_WORK_TURN_COUNT BTLEFF_WORK_CONTINUE_COUNT BTLEFF_WORK_YURE_CNT BTLEFF_WORK_GET_SUCCESS BTLEFF_WORK_ITEM_NO BTLEFF_WORK_POS_AA_WEIGHT  BTLEFF_WORK_POS_BB_WEIGHT BTLEFF_WORK_POS_A_WEIGHT BTLEFF_WORK_POS_B_WEIGHT BTLEFF_WORK_POS_C_WEIGHT BTLEFF_WORK_POS_D_WEIGHT BTLEFF_WORK_POS_E_WEIGHT BTLEFF_WORK_POS_F_WEIGHT BTLEFF_WORK_ATTACK_WEIGHT BTLEFF_WORK_SEQUENCE_WORK BTLEFF_WORK_ATTACK_POKEMON_VANISH BTLEFF_WORK_ATTACK_POKEMON_DIR BTLEFF_WORK_POS_AA_RARE BTLEFF_WORK_POS_BB_RARE BTLEFF_WORK_POS_A_RARE BTLEFF_WORK_POS_B_RARE BTLEFF_WORK_POS_C_RARE BTLEFF_WORK_POS_D_RARE BTLEFF_WORK_POS_E_RARE BTLEFF_WORK_POS_F_RARE  BTLEFF_WORK_ATTACK_RARE BTLEFF_WORK_POS_AA_JUMP BTLEFF_WORK_POS_BB_JUMP BTLEFF_WORK_POS_A_JUMP  BTLEFF_WORK_POS_B_JUMP  BTLEFF_WORK_POS_C_JUMP  BTLEFF_WORK_POS_D_JUMP  BTLEFF_WORK_POS_E_JUMP  BTLEFF_WORK_POS_F_JUMP  BTLEFF_WORK_ATTACK_JUMP BTLEFF_WORK_MULTI BTLEFF_WORK_TRTYPE_A BTLEFF_WORK_TRTYPE_B BTLEFF_WORK_TRTYPE_C BTLEFF_WORK_TRTYPE_D
 * #param COMBOBOX_TEXT ==  !=  <  >  <=  >=
 * #param COMBOBOX_VALUE  BTLEFF_COND_EQUAL BTLEFF_COND_NOT_EQUAL BTLEFF_COND_MIMAN BTLEFF_COND_KOERU  BTLEFF_COND_IKA BTLEFF_COND_IJOU
 * #param VALUE_INT ��r����l
 * #param VALUE_INT ��ѐ惉�x��
 */
//======================================================================
	.macro	IF  work, cond, value, adrs
	.short  EC_IF
	.long   \work
	.long   \cond
	.long   \value
	.long		( \adrs - . ) - 4
	.endm

//======================================================================
/**
 * @brief	�����ʒu�`�F�b�N
 *
 * #param_num	3
 * @param		pos   �����ʒu
 * @param	  cond  �`�F�b�N����
 * @param		adrs  �`�F�b�N����TRUE���̔�ѐ�
 *
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT ���Ȃ�  ����
 * #param	COMBOBOX_VALUE BTLEFF_COND_NO_EXIST  BTLEFF_COND_EXIST
 * #param	VALUE_INT ��ѐ惉�x��
 */
//======================================================================
	.macro	MCSS_POS_CHECK	pos, cond, adrs
  .short  EC_MCSS_POS_CHECK
	.long		\pos
	.long		\cond
	.long		( \adrs - . ) - 4
	.endm

//======================================================================
/**
 * @brief	�ėp���[�N�ɒl���Z�b�g
 *
 * #param_num	1
 * @param		value	�Z�b�g����l
 *
 * #param	VALUE_INT �Z�b�g����l
 */
//======================================================================
	.macro	SET_WORK	value
  .short  EC_SET_WORK
	.long		\value
	.endm

//======================================================================
/**
 * @brief	�w�肳�ꂽ���[�N����l���Q�b�g
 *
 * #param_num	1
 * @param		work	�擾���郏�[�N
 *
 * #param COMBOBOX_TEXT WAZA_RANGE TURN_COUNT CONTINUE_COUNT YURE_CNT GET_SUCCESS ITEM_NO POS_AA_WEIGHT POS_BB_WEIGHT POS_A_WEIGHT POS_B_WEIGHT POS_C_WEIGHT POS_D_WEIGHT POS_E_WEIGHT POS_F_WEIGHT �U�����̏d  SEQUENCE_WORK ATTACK_POKEMON_VANISH �U��������  POS_AA_RARE POS_BB_RARE POS_A_RARE POS_B_RARE POS_C_RARE POS_D_RARE POS_E_RARE POS_F_RARE �U�������A  POS_AA_JUMP POS_BB_JUMP POS_A_JUMP  POS_B_JUMP  POS_C_JUMP  POS_D_JUMP  POS_E_JUMP  POS_F_JUMP  �U�������˂�  �}���`  TRTYPE_A  TRTYPE_B  TRTYPE_C  TRTYPE_D
 * #param COMBOBOX_VALUE BTLEFF_WORK_WAZA_RANGE BTLEFF_WORK_TURN_COUNT BTLEFF_WORK_CONTINUE_COUNT BTLEFF_WORK_YURE_CNT BTLEFF_WORK_GET_SUCCESS BTLEFF_WORK_ITEM_NO BTLEFF_WORK_POS_AA_WEIGHT  BTLEFF_WORK_POS_BB_WEIGHT BTLEFF_WORK_POS_A_WEIGHT BTLEFF_WORK_POS_B_WEIGHT BTLEFF_WORK_POS_C_WEIGHT BTLEFF_WORK_POS_D_WEIGHT BTLEFF_WORK_POS_E_WEIGHT BTLEFF_WORK_POS_F_WEIGHT BTLEFF_WORK_ATTACK_WEIGHT BTLEFF_WORK_SEQUENCE_WORK BTLEFF_WORK_ATTACK_POKEMON_VANISH BTLEFF_WORK_ATTACK_POKEMON_DIR BTLEFF_WORK_POS_AA_RARE BTLEFF_WORK_POS_BB_RARE BTLEFF_WORK_POS_A_RARE BTLEFF_WORK_POS_B_RARE BTLEFF_WORK_POS_C_RARE BTLEFF_WORK_POS_D_RARE BTLEFF_WORK_POS_E_RARE BTLEFF_WORK_POS_F_RARE  BTLEFF_WORK_ATTACK_RARE BTLEFF_WORK_POS_AA_JUMP BTLEFF_WORK_POS_BB_JUMP BTLEFF_WORK_POS_A_JUMP  BTLEFF_WORK_POS_B_JUMP  BTLEFF_WORK_POS_C_JUMP  BTLEFF_WORK_POS_D_JUMP  BTLEFF_WORK_POS_E_JUMP  BTLEFF_WORK_POS_F_JUMP  BTLEFF_WORK_ATTACK_JUMP BTLEFF_WORK_MULTI BTLEFF_WORK_TRTYPE_A BTLEFF_WORK_TRTYPE_B BTLEFF_WORK_TRTYPE_C BTLEFF_WORK_TRTYPE_D
 */
//======================================================================
	.macro	GET_WORK	work
  .short  EC_GET_WORK
	.long		\work
	.endm

//======================================================================
/**
 * @brief	�݂���菈��
 *
 * #param_num	3
 * @param sw	  ON/OFF
 * @param pos   ��������|�W�V����
 * @param flag  �����ŕێ�����t���O�̗��������𐧌�
 *
 * #param COMBOBOX_TEXT ON  OFF
 * #param COMBOBOX_VALUE BTLEFF_MIGAWARI_ON BTLEFF_MIGAWARI_OFF
 * #param	COMBOBOX_TEXT	�U����  �h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE
 * #param	VALUE_INT 0
 *
 */
//======================================================================
	.macro	MIGAWARI	sw, pos, flag
  .short  EC_MIGAWARI
	.long		\sw
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�ւ񂵂񏈗�
 *
 * #param_num	0
 */
//======================================================================
	.macro	HENSHIN
  .short  EC_HENSHIN
	.endm

//======================================================================
/**
 * @brief	�|�P��������
 *
 * #param_num	1
 * @param pos �������o���|�P�����̗����ʒu
 *
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK	BTLEFF_POKEMON_SIDE_DEFENCE	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F
 *
 */
//======================================================================
	.macro	NAKIGOE pos
  .short  EC_NAKIGOE
	.long		\pos
	.endm

//======================================================================
/**
 * @brief �{�[�����[�h
 *
 * �p�[�e�B�N��orOBJ�Ń{�[�����o���Ƃ��ɃI�t�Z�b�g�ɂ���p�����[�^���w��
 *
 * #param_num	1
 * @param type  �I�t�Z�b�g�^�C�v
 *
 * #param	COMBOBOX_TEXT �g�p�A�C�e�� �ߊl�{�[��AA �ߊl�{�[��BB �ߊl�{�[��A �ߊl�{�[��B �ߊl�{�[��C �ߊl�{�[��D �ߊl�{�[��E �ߊl�{�[��F �ߊl�{�[���U����
 * #param	COMBOBOX_VALUE	BTLEFF_USE_BALL BTLEFF_CAPTURE_BALL_POS_AA BTLEFF_CAPTURE_BALL_POS_BB BTLEFF_CAPTURE_BALL_POS_A BTLEFF_CAPTURE_BALL_POS_B BTLEFF_CAPTURE_BALL_POS_C BTLEFF_CAPTURE_BALL_POS_D BTLEFF_CAPTURE_BALL_POS_E BTLEFF_CAPTURE_BALL_POS_F BTLEFF_CAPTURE_BALL_ATTACK
 *
 */
//======================================================================
	.macro	BALL_MODE pos
  .short  EC_BALL_MODE
	.long		\pos
	.endm

//======================================================================
/**
 * @brief	�{�[��OBJ�̃Z�b�g
 *
 * #param_num	6
 * @param	index   �Ǘ��p�C���f�b�N�X�i0-3�j
 * @param pos     �\��������W
 * @param ofs_x   �I�t�Z�b�gX���W
 * @param ofs_y   �I�t�Z�b�gY���W
 * @param scale_x �I�t�Z�b�gX���W
 * @param scale_y �I�t�Z�b�gY���W
 *
 * #param	VALUE_INT �C���f�b�N�X
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param VALUE_FX32  �I�t�Z�b�gX���W
 * #param VALUE_FX32  �I�t�Z�b�gY���W
 * #param VALUE_FX32  �X�P�[��X�l
 * #param VALUE_FX32  �X�P�[��Y�l
 */
//======================================================================
  .macro  BALLOBJ_SET index, pos, ofs_x, ofs_y, scale_x, scale_y
  .short  EC_BALLOBJ_SET
  .long   \index
  .long   \pos
  .long   \ofs_x
  .long   \ofs_y
  .long   \scale_x
  .long   \scale_y
  .endm

//======================================================================
/**
 * @brief	�T�u���[�`���R�[��
 *
 * #param_num	3
 * @param	seq_no  �T�u���[�`���V�[�P���X�i���o�[
 * @param	atk_pos attack_pos
 * @param	def_pos def_pos
 *
 * #param	VALUE_INT �V�[�P���X�i���o�[
 * #param	COMBOBOX_TEXT POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F �U����  �h�䑤
 * #param	COMBOBOX_VALUE  BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE
 * #param	COMBOBOX_TEXT POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F �U����  �h�䑤
 * #param	COMBOBOX_VALUE  BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_DEFENCE
 */
//======================================================================
  .macro  CALL seq_no, atk_pos, def_pos
  .short  EC_CALL
  .long   \seq_no
  .long   \atk_pos
  .long   \def_pos
  .endm

//======================================================================
/**
 * @brief	�T�u���[�`�����^�[��
 *
 * #param_num	0
 */
//======================================================================
  .macro  RETURN
  .short  EC_RETURN
  .endm

//======================================================================
/**
 * @brief	�W�����v
 *
 * #param_num	1
 *
 * @param adrs  ��ѐ惉�x��
 *
 * #param VALUE_INT ��ѐ惉�x��
 */
//======================================================================
  .macro  JUMP  adrs
  .short  EC_JUMP
	.long		( \adrs - . ) - 4
  .endm

//======================================================================
/**
 * @brief	�V�[�P���X�ꎞ��~
 *
 * #param_num	0
 */
//======================================================================
  .macro  PAUSE
  .short  EC_PAUSE
  .endm

//======================================================================
/**
 * @brief	���x��
 *
 * #param_num	1
 * @param		label	���x��
 *
 * #param	VALUE_INT ���x��
 */
//======================================================================
	.macro	LABEL	label
\label:
	.endm

//======================================================================
/**
 * 	�G�t�F�N�g�I���͂��Ȃ炸�Ō�ɂȂ�悤�ɂ���
 */
//======================================================================
//======================================================================
/**
 * @brief	�G�t�F�N�g�I��
 *
 * #param_num	0
 */
//======================================================================
	.macro	SEQ_END
	.short	EC_SEQ_END
	.endm

#endif
