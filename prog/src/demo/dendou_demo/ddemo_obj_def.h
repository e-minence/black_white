//============================================================================================
/**
 * @file		ddemo_obj_def.h
 * @brief		殿堂入りデモ画面 ＯＢＪ関連
 * @author	Hiroyuki Nakamura
 * @date		10.04.07
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

// OBJ ID
enum {
	DDEMOOBJ_ID_POKE = 0,			// ポケモン（正面）
	DDEMOOBJ_ID_POKE_B,				// ポケモン（背面）
	DDEMOOBJ_ID_MES,
	DDEMOOBJ_ID_INFO,

	DDEMOOBJ_ID_EFF,

	DDEMOOBJ_ID_MAX
};

// キャラリソース
enum {
	DDEMOOBJ_CHRRES_POKE = 0,		// ポケモン（正面）
	DDEMOOBJ_CHRRES_POKE_B,			// ポケモン（背面）
	DDEMOOBJ_CHRRES_ETC,
	DDEMOOBJ_CHRRES_MAX
};

// パレットリソース
enum {
	DDEMOOBJ_PALRES_POKE = 0,		// ポケモン（正面）
	DDEMOOBJ_PALRES_POKE_B,			// ポケモン（背面）
	DDEMOOBJ_PALRES_ETC,
	DDEMOOBJ_PALRES_FOAM,
	DDEMOOBJ_PALRES_MAX
};

// セルリソース
enum {
	DDEMOOBJ_CELRES_POKE = 0,		// ポケモン（正面）
	DDEMOOBJ_CELRES_POKE_B,			// ポケモン（背面）
	DDEMOOBJ_CELRES_ETC,
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
