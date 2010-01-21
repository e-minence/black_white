//============================================================================================
/**
 * @file		vs_multi_list.c
 * @brief		�퓬�|�P�������X�g ���ʒ�`
 * @author	Hiroyuki Nakamura
 * @date		10.01.15
 *
 *	���W���[�����FBPL_COMM
 */
//============================================================================================
#pragma	once

// �p���b�g��`
#define	BPL_COMM_PAL_NN_PLATE	( 0 )		///< �p���b�g00�F�v���[�g�i�ʏ�j
#define	BPL_COMM_PAL_MN_PLATE	( 5 )		///< �p���b�g01�F�v���[�g�i�}���`�j
#define	BPL_COMM_PAL_DN_PLATE	( 7 )		///< �p���b�g02�F�v���[�g�i�m���j
#define	BPL_COMM_PAL_HPGAGE		( 9 )		///< �p���b�g09�FHP�Q�[�W

#define	BPL_PAL_B_RED		( 10 )		///< �p���b�g09�F�{�^���i�ԁj
#define	BPL_PAL_B_GREEN		( 12 )		///< �p���b�g09�F�{�^���i�΁j

#define	BPL_PAL_SYS_FONT	( 13 )		///< �p���b�g13�F�V�X�e���t�H���g
#define	BPL_PAL_TALK_WIN	( 14 )		///< �p���b�g14�F��b�E�B���h�E
#define	BPL_PAL_TALK_FONT	( BPL_PAL_SYS_FONT )		///< �p���b�g15�F��b�t�H���g

// �v���[�g�T�C�Y
#define	BPL_COMM_BSX_PLATE		( 16 )
#define	BPL_COMM_BSY_PLATE		( 6 )

// �v���[�g�z�u�ʒu
#define	BPL_COMM_SCR_POKE1_PX		( 0 )
#define	BPL_COMM_SCR_POKE1_PY		( 0 )
#define	BPL_COMM_SCR_POKE2_PX		( 16 )
#define	BPL_COMM_SCR_POKE2_PY		( 1 )
#define	BPL_COMM_SCR_POKE3_PX		( 0 )
#define	BPL_COMM_SCR_POKE3_PY		( 6 )
#define	BPL_COMM_SCR_POKE4_PX		( 16 )
#define	BPL_COMM_SCR_POKE4_PY		( 7 )
#define	BPL_COMM_SCR_POKE5_PX		( 0 )
#define	BPL_COMM_SCR_POKE5_PY		( 12 )
#define	BPL_COMM_SCR_POKE6_PX		( 16 )
#define	BPL_COMM_SCR_POKE6_PY		( 13 )

// �a�l�o
// �|�P�����P
#define	BPL_COMM_WIN_P1_POKE1_PX		( 0 )
#define	BPL_COMM_WIN_P1_POKE1_PY		( 0 )
#define	BPL_COMM_WIN_P1_POKE1_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE1_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE1_PAL	( BPL_COMM_PAL_HPGAGE )
// �|�P�����Q
#define	BPL_COMM_WIN_P1_POKE2_PX		( 16 )
#define	BPL_COMM_WIN_P1_POKE2_PY		( 1 )
#define	BPL_COMM_WIN_P1_POKE2_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE2_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE2_PAL	( BPL_COMM_PAL_HPGAGE )
// �|�P�����R
#define	BPL_COMM_WIN_P1_POKE3_PX		( 0 )
#define	BPL_COMM_WIN_P1_POKE3_PY		( 6 )
#define	BPL_COMM_WIN_P1_POKE3_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE3_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE3_PAL	( BPL_COMM_PAL_HPGAGE )
// �|�P�����S
#define	BPL_COMM_WIN_P1_POKE4_PX		( 16 )
#define	BPL_COMM_WIN_P1_POKE4_PY		( 7 )
#define	BPL_COMM_WIN_P1_POKE4_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE4_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE4_PAL	( BPL_COMM_PAL_HPGAGE )
// �|�P�����T
#define	BPL_COMM_WIN_P1_POKE5_PX		( 0 )
#define	BPL_COMM_WIN_P1_POKE5_PY		( 12 )
#define	BPL_COMM_WIN_P1_POKE5_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE5_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE5_PAL	( BPL_COMM_PAL_HPGAGE )
// �|�P�����U
#define	BPL_COMM_WIN_P1_POKE6_PX		( 16 )
#define	BPL_COMM_WIN_P1_POKE6_PY		( 13 )
#define	BPL_COMM_WIN_P1_POKE6_SX		( 15 )
#define	BPL_COMM_WIN_P1_POKE6_SY		( 5 )
#define	BPL_COMM_WIN_P1_POKE6_PAL	( BPL_COMM_PAL_HPGAGE )

// ���O�\�����W
#define BPL_COMM_P1_NAME_PX		( 32 )
#define BPL_COMM_P1_NAME_PY		( 8-1 )
// �g�o�\�����W
#define BPL_COMM_P1_HP_PX			( 92 )
#define BPL_COMM_P1_HP_PY		  ( 32 )
#define BPL_COMM_P1_HP_SX			( 24 )
#define BPL_COMM_P1_HP_SY			( 8 )
// �g�o�Q�[�W�\�����W
#define BPL_COMM_P1_HPGAGE_PX	( 48+16 )
#define BPL_COMM_P1_HPGAGE_PY	( 24 )
#define BPL_COMM_P1_HPGAGE_SX	( 64 )
#define BPL_COMM_P1_HPGAGE_SY	( 8 )
// �k���\�����W
#define BPL_COMM_P1_LV_PX			( 8 )
#define BPL_COMM_P1_LV_PY			( 32 )

#define	BPL_COMM_HP_DOTTO_MAX				( 48 )		// HP�Q�[�W�̃h�b�g�T�C�Y

#define BPL_COMM_HP_GAGE_COL_G1  ( 1 ) // HP�Q�[�W�J���[�΂P
#define BPL_COMM_HP_GAGE_COL_G2  ( 2 ) // HP�Q�[�W�J���[�΂Q
#define BPL_COMM_HP_GAGE_COL_Y1  ( 3 ) // HP�Q�[�W�J���[���P
#define BPL_COMM_HP_GAGE_COL_Y2  ( 4 ) // HP�Q�[�W�J���[���Q
#define BPL_COMM_HP_GAGE_COL_R1  ( 5 ) // HP�Q�[�W�J���[�ԂP
#define BPL_COMM_HP_GAGE_COL_R2  ( 6 ) // HP�Q�[�W�J���[�ԂQ

// �n�a�i
// �|�P�����A�C�R��
#define	BPL_COMM_POKEICON_LPX		( 16 )
#define	BPL_COMM_POKEICON_RPX		( 144 )
#define	BPL_COMM_POKEICON1_PY		( 16 )
#define	BPL_COMM_POKEICON2_PY		( 24 )
#define	BPL_COMM_POKEICON3_PY		( 64 )
#define	BPL_COMM_POKEICON4_PY		( 72 )
#define	BPL_COMM_POKEICON5_PY		( 112 )
#define	BPL_COMM_POKEICON6_PY		( 120 )
// �A�C�e���A�C�R��
#define	BPL_COMM_ITEMICON_X			( 8 )
#define	BPL_COMM_ITEMICON_Y			( 8 )
// ��Ԉُ�A�C�R��
#define	BPL_COMM_STATUSICON_X		( 12 )
#define	BPL_COMM_STATUSICON_Y		( 24 )
// �g�o�Q�[�W�w�i
#define	BPL_COMM_HPGAUGE_X			( 71 )
#define	BPL_COMM_HPGAUGE_Y			( 12 )
