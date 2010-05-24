//============================================================================================
/**
 * @file		ddemo_obj_def.h
 * @brief		�a������f����� �n�a�i�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.04.07
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

#define	DDEMOOBJ_ID_2ND_EFF_MAX		( 24 )		// �V�[���Q�ŕ\���ł�����n�a�i�̍ő吔

// OBJ ID
enum {
	DDEMOOBJ_ID_POKE = 0,			// �|�P�����i���ʁj
	DDEMOOBJ_ID_POKE_B,				// �|�P�����i�w�ʁj
	DDEMOOBJ_ID_MES,
	DDEMOOBJ_ID_INFO,

	DDEMOOBJ_ID_EFF,

	DDEMOOBJ_ID_PLAYER_M,			// �v���C���[�i���C����ʁj
	DDEMOOBJ_ID_PLAYER_S,			// �v���C���[�i�T�u��ʁj
	DDEMOOBJ_ID_2ND_MES,
	DDEMOOBJ_ID_2ND_INFO,

	DDEMOOBJ_ID_2ND_EFF,

	DDEMOOBJ_ID_MAX = DDEMOOBJ_ID_2ND_EFF + DDEMOOBJ_ID_2ND_EFF_MAX
};

// �L�������\�[�X
enum {
	DDEMOOBJ_CHRRES_POKE = 0,		// �|�P�����i���ʁj
	DDEMOOBJ_CHRRES_POKE_B,			// �|�P�����i�w�ʁj
	DDEMOOBJ_CHRRES_ETC,
	DDEMOOBJ_CHRRES_PLAYER_M,		// �v���C���[�i���C����ʁj
	DDEMOOBJ_CHRRES_PLAYER_S,		// �v���C���[�i�T�u��ʁj
	DDEMOOBJ_CHRRES_ETC_S,
	DDEMOOBJ_CHRRES_MAX
};

// �p���b�g���\�[�X
enum {
	DDEMOOBJ_PALRES_POKE = 0,		// �|�P�����i���ʁj
	DDEMOOBJ_PALRES_POKE_B,			// �|�P�����i�w�ʁj
	DDEMOOBJ_PALRES_ETC,
	DDEMOOBJ_PALRES_FOAM,
	DDEMOOBJ_PALRES_PLAYER_M,		// �v���C���[�i���C����ʁj
	DDEMOOBJ_PALRES_PLAYER_S,		// �v���C���[�i�T�u��ʁj
	DDEMOOBJ_PALRES_ETC_S,
	DDEMOOBJ_PALRES_MAX
};

// �Z�����\�[�X
enum {
	DDEMOOBJ_CELRES_POKE = 0,		// �|�P�����i���ʁj
	DDEMOOBJ_CELRES_POKE_B,			// �|�P�����i�w�ʁj
	DDEMOOBJ_CELRES_ETC,
	DDEMOOBJ_CELRES_PLAYER_M,		// �v���C���[�i���C����ʁj
	DDEMOOBJ_CELRES_PLAYER_S,		// �v���C���[�i�T�u��ʁj
	DDEMOOBJ_CELRES_ETC_S,
	DDEMOOBJ_CELRES_MAX
};

// font OAM
enum {
	DDEMOOBJ_FOAM_MES = 0,
	DDEMOOBJ_FOAM_INFO,
	DDEMOOBJ_FOAM_MES2,
	DDEMOOBJ_FOAM_PLAYER,
	DDEMOOBJ_FOAM_MAX,
};
