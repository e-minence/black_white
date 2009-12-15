//============================================================================================
/**
 * @file		zknlist_obj_def.h
 * @brief		�}�Ӄ��X�g��� �n�a�i�֘A��`
 * @author	Hiroyuki Nakamura
 * @date		09.12.15
 *
 *	���W���[�����FZKNLISTOBJ
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

#define	ZKNLISTOBJ_POKEICON_MAX		( 16 )		// �|�P�����A�C�R���\����

// OBJ Index
enum {
	ZKNLISTOBJ_IDX_POKEICON = 0,
	ZKNLISTOBJ_IDX_POKEGRA = ZKNLISTOBJ_IDX_POKEICON + ZKNLISTOBJ_POKEICON_MAX,
	ZKNLISTOBJ_IDX_POKEGRA2,
	ZKNLISTOBJ_IDX_SCROLL_BAR,

	ZKNLISTOBJ_IDX_TB_RETURN,
	ZKNLISTOBJ_IDX_TB_EXIT,
	ZKNLISTOBJ_IDX_TB_Y_BUTTON,
	ZKNLISTOBJ_IDX_TB_LEFT,
	ZKNLISTOBJ_IDX_TB_RIGHT,
	ZKNLISTOBJ_IDX_TB_SELECT,
	ZKNLISTOBJ_IDX_TB_START,

	ZKNLISTOBJ_IDX_MAX
};

// �L�������\�[�XID
enum {
	ZKNLISTOBJ_CHRRES_POKEICON = 0,	// �|�P�����A�C�R��
	ZKNLISTOBJ_CHRRES_POKEGRA = ZKNLISTOBJ_CHRRES_POKEICON + ZKNLISTOBJ_POKEICON_MAX,
	ZKNLISTOBJ_CHRRES_POKEGRA2,
	ZKNLISTOBJ_CHRRES_TOUCH_BAR,
	ZKNLISTOBJ_CHRRES_ZKNOBJ,
	ZKNLISTOBJ_CHRRES_MAX
};

// �p���b�g���\�[�XID
enum {
	ZKNLISTOBJ_PALRES_POKEICON = 0,
//	ZKNLISTOBJ_PALRES_POKEICON_S,
	ZKNLISTOBJ_PALRES_POKEGRA,
	ZKNLISTOBJ_PALRES_POKEGRA2,
	ZKNLISTOBJ_PALRES_TOUCH_BAR,
	ZKNLISTOBJ_PALRES_ZKNOBJ,
	ZKNLISTOBJ_PALRES_MAX
};

// �Z�����\�[�XID
enum {
	ZKNLISTOBJ_CELRES_POKEICON = 0,
//	ZKNLISTOBJ_CELRES_POKEICON_S,
	ZKNLISTOBJ_CELRES_POKEGRA,
	ZKNLISTOBJ_CELRES_POKEGRA2,
	ZKNLISTOBJ_CELRES_TOUCH_BAR,
	ZKNLISTOBJ_CELRES_ZKNOBJ,
	ZKNLISTOBJ_CELRES_MAX
};
