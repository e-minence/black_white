//============================================================================================
/**
 * @file		ddemo_main.h
 * @brief		�a������f�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.06
 *
 *	���W���[�����FDDEMOMAIN
 */
//============================================================================================
#pragma	once

#include "system/main.h"
#include "system/bmp_oam.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "sound/sound_manager.h"
#include "demo/dendou_demo.h"

#include "ddemo_obj_def.h"


// ����m�ۗp�q�[�v�h�c
#define	HEAPID_DENDOU_DEMO_L		( GFL_HEAP_LOWID(HEAPID_DENDOU_DEMO) )

// �n�`�l�t�H���g���[�N
typedef struct {
	BMPOAM_ACT_PTR oam;
	GFL_BMP_DATA * bmp;
}DDEMO_FONTOAM;

typedef struct {
	DENDOUDEMO_PARAM * dat;

	GFL_TCB * vtask;		// TCB ( VBLANK )

	SOUNDMAN_PRESET_HANDLE * sndHandle;

	GFL_G3D_UTIL * g3d_util;
	GFL_G3D_SCENE * g3d_scene;
	GFL_G3D_CAMERA * g3d_camera[2];
	GFL_G3D_LIGHTSET * g3d_light;
//	u32	g3d_obj_id;
//	u32	box_anm;

	GFL_PTC_PTR	ptc;											// �p�[�e�B�N��
	u8	ptcWork[PARTICLE_LIB_HEAP_SIZE];	// �p�[�e�B�N���Ŏg�p���郏�[�N

	GFL_FONT * font;						// �ʏ�t�H���g
	GFL_FONT * nfnt;						// �����t�H���g
	GFL_MSGDATA * mman;					// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;							// �P��Z�b�g
	STRBUF * exp;								// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;						// �v�����g�L���[

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[DDEMOOBJ_ID_MAX];
	u32	chrRes[DDEMOOBJ_CHRRES_MAX];
	u32	palRes[DDEMOOBJ_PALRES_MAX];
	u32	celRes[DDEMOOBJ_CELRES_MAX];

	// �n�`�l�t�H���g
	BMPOAM_SYS_PTR	fntoam;
	DDEMO_FONTOAM	fobj[DDEMOOBJ_FOAM_MAX];

	u16	monsno;
	u16	type;
	u8	form;

	u8	pokePos;
	u8	pokeMax;

	u8	rndCnt;
	s16	rndDx;
	s16	rndDy;
	s16	rndMx[8];
	s16	rndMy[8];

	u32	voicePlayer;

	int	wait;

	int	mainSeq;
	int	subSeq;
	int	nextSeq;
}DDEMOMAIN_WORK;

typedef int (*pDDEMOMAIN_FUNC)(DDEMOMAIN_WORK*);




extern void DDEMOMAIN_InitVBlank( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_ExitVBlank( DDEMOMAIN_WORK * wk );

extern void DDEMOMAIN_InitVram(void);
extern const GFL_DISP_VRAM * DDEMOMAIN_GetVramBankData(void);

extern void DDEMOMAIN_InitBg(void);
extern void DDEMOMAIN_ExitBg(void);

extern void DDEMOMAIN_SetBlendAlpha(void);

extern void DDEMOMAIN_InitMsg( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_ExitMsg( DDEMOMAIN_WORK * wk );


extern void DDEMOMAIN_GetPokeMax( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_GetPokeData( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_LoadPokeVoice( DDEMOMAIN_WORK * wk );


extern void DDEMOMAIN_Init3D( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_Exit3D( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_Main3D( DDEMOMAIN_WORK * wk );

extern void DDEMOMAIN_InitParticle( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_ExitParticle( DDEMOMAIN_WORK * wk );
extern void DDEMOMAIN_SetEffectParticle( DDEMOMAIN_WORK * wk, u32 id );
