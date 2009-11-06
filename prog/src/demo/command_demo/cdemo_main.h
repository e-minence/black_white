//============================================================================================
/**
 * @file		cdemo_main.h
 * @brief		�R�}���h�f����� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
/*
#ifndef LD_MAIN_H
#define	LD_MAIN_H

#include "demo/legend_demo.h"
//#include "../../field/motion_bl.h"
*/

#pragma	once

#include "demo/command_demo.h"

//============================================================================================
//	�萔��`
//============================================================================================


/*
// �A�j���e�[�u���C���f�b�N�X
enum {
	LD_SCENE_ANM_ICA = 0,
	LD_SCENE_ANM_IMA,
	LD_SCENE_ANM_ITA,
	LD_SCENE_ANM_ITP,
	LD_SCENE_ANM_MAX,
};

// �V�[���f�[�^
typedef struct {
	NNSG3dRenderObj	rendObj;
	NNSG3dResMdl * mdl;
	NNSG3dResFileHeader * rh;
	void * anmMem[LD_SCENE_ANM_MAX];
	NNSG3dAnmObj * anmObj[LD_SCENE_ANM_MAX];
}LD_SCENE_DATA;

// ���[�V�����u���[�������p�����[�^
typedef struct {
	// �O���t�B�b�N���[�h
	GXDispMode dispMode;	// �f�B�X�v���C���[�h
	GXBGMode bgMode;		// BG���[�h	
	GXBG0As bg0_2d3d;		// BG0��3d�Ɏg�p���邩

	// �L���v�`��
	GXCaptureSize sz;		// �L���v�`���T�C�Y
    GXCaptureMode mode;		// �L���v�`�����[�h
    GXCaptureSrcA a;		// �L���v�`����A
    GXCaptureSrcB b;		// �L���v�`����B
    GXCaptureDest dest;		// �L���v�`���f�[�^�]��Vram
    int eva;				// �u�����h�W��A
    int evb;				// �u�����h�W��B

	int heap_id;			// �g�p����q�[�vID
}LDMAIN_MBL_PARAM;

// ���[�V�����u���[�^�X�N���[�N
typedef struct {
	GXVRamLCDC			lcdc;		// ����LCDC
	LDMAIN_MBL_PARAM	data;		// ���[�V�����u���[�������p�����[�^
	BOOL				init_flg;
	TCB_PTR				tcb;	
}LDMAIN_MBL_WORK;

// �`���f�����[�N
typedef struct {
	LEGEND_DEMO_DATA * dat;

	LDMAIN_MBL_WORK * mb;

	NNSFndAllocator	allocator;
	GF_CAMERA_PTR	camera;		// �J����
	LD_SCENE_DATA	scene[8];	// �V�[���f�[�^
	u32	scene_max;				// �V�[���f�[�^��

	VecFx32 target;

	int	main_seq;
	int	next_seq;

	u32	cnt;		// �J�E���^

	void * work;	// �e�A�v���Ŏg�p���郏�[�N�i�m�ہE����͊e�A�v���Łj

}LEGEND_DEMO_WORK;

typedef int (*pLegendDemoFunc)(LEGEND_DEMO_WORK*);

// ���\�[�X�f�[�^�e�[�u��
typedef struct {
	u32	imd;
	u8	anm[4];
	BOOL	glstFlag;
}LD_RES_TBL;

// �J�����f�[�^
typedef struct {
	GF_CAMERA_PARAM	prm;
	VecFx32	target;
	fx32	near;
	fx32	far;
}LD_CAMERA_DATA;

#define	LD_RES_DUMMY	( 0xff )		// ���\�[�X�_�~�[�f�[�^
*/

typedef struct {
	u16	buff[256*16];
	u32	size;
}CDEMO_PALETTE;

typedef struct {
	COMMANDDEMO_DATA * dat;

	ARCHANDLE * gra_ah;

	CDEMO_PALETTE	pltt[3];

	GFL_TCB * vtask;					// TCB ( VBLANK )

	int * commSrc;
	int * commPos;

	u32	cnt;		// �J�E���^

	// �A���t�@�u�����h
	int	alpha_plane1;		// �ΏۖʂP
	int	alpha_ev1;			// �ΏۖʂP�̃u�����h�W��
	int	alpha_plane2;		// �ΏۖʂQ
	int	alpha_ev2;			// �ΏۖʂQ�̃u�����h�W��
	int	alpha_end_ev;		// �I���u�����h�W��
	int	alpha_mv_frm;		// �I���܂ł̃t���[����

	// �a�f�X�N���[���؂�ւ��A�j��
	int	bgsa_chr;
	int	bgsa_pal;
	int	bgsa_scr;
	u16	bgsa_max;
	u16	bgsa_num;
	u8	bgsa_wait;
	u8	bgsa_cnt;
	u16	bgsa_load;
	u16	bgsa_seq;

	int	main_seq;
	int	next_seq;

#ifdef PM_DEBUG
	u32	debug_count_frm;
	u32	debug_count;

	OSTick	stick;
	OSTick	etick;

#endif	// PM_DEBUG

}CDEMO_WORK;

typedef int (*pCommDemoFunc)(CDEMO_WORK*);

#define	HEAPID_COMMAND_DEMO		( HEAPID_BOX_SYS )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

extern void CDEMOMAIN_VramBankSet(void);
extern void CDEMOMAIN_BgInit( CDEMO_WORK * wk );
extern void CDEMOMAIN_BgExit(void);
extern void CDEMOMAIN_CommDataLoad( CDEMO_WORK * wk );
extern void CDEMOMAIN_CommDataDelete( CDEMO_WORK * wk );
extern void CDEMOMAIN_InitVBlank( CDEMO_WORK * wk );
extern void CDEMOMAIN_ExitVBlank( CDEMO_WORK * wk );

extern void CDEMOMAIN_FadeInSet( CDEMO_WORK * wk, int div, int sync, int next );
extern void CDEMOMAIN_FadeOutSet( CDEMO_WORK * wk, int div, int sync, int next );
extern void CDEMOMAIN_WhiteInSet( CDEMO_WORK * wk, int div, int sync, int next );
extern void CDEMOMAIN_WhiteOutSet( CDEMO_WORK * wk, int div, int sync, int next );

extern void CDEMOMAIN_LoadPaletteRequest( CDEMO_WORK * wk, u32 frm, u32 id );

extern void CDEMOMAIN_LoadBgGraphic( CDEMO_WORK * wk, u32 chr, u32 pal, u32 scrn, u32 frm );
























#if 0
//--------------------------------------------------------------------------------------------
/**
 * ���ʏ�����
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_CommInit( LEGEND_DEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ���ʉ������
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_CommRelease( LEGEND_DEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�C���Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_FadeInSet( LEGEND_DEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�A�E�g�Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_FadeOutSet( LEGEND_DEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * �z���C�g�C���Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_WhiteInSet( LEGEND_DEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * �z���C�g�A�E�g�Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_WhiteOutSet( LEGEND_DEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * �J�����ݒ�
 *
 * @param	wk		�`���f�����[�N
 * @param	dat		�J�����f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_InitCamera( LEGEND_DEMO_WORK * wk, const LD_CAMERA_DATA * dat );

//--------------------------------------------------------------------------------------------
/**
 * ���C�g������
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_InitLight(void);

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�ݒ�
 *
 * @param	gst		���C�g�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_SetLight( GLST_DATA * gst );

//--------------------------------------------------------------------------------------------
/**
 * �G�b�W�}�[�L���O�ݒ�
 *
 * @param	rgb		�J���[�e�[�u��
 *
 * @return	none
 *
 * @li	rgb = NULL �ŃG�b�W�}�[�L���O����
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_EdgeMarkingSet( const GXRgb * rgb );

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���f�[�^�ǂݍ���
 *
 * @param	wk			�`���f�����[�N
 * @param	arcIndex	�A�[�N�C���f�b�N�X
 * @param	tbl			�V�[�����\�[�X�f�[�^�e�[�u��
 * @param	max			�V�[�����\�[�X�f�[�^��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_Load3DData( LEGEND_DEMO_WORK * wk, u32 arcIndex, const LD_RES_TBL * tbl, u32 max );

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���f�[�^�폜
 *
 * @param	wk			�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_Delete3DData( LEGEND_DEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���\��
 *
 * @param	wk			�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_Draw3DMain( LEGEND_DEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���A�j���`�F�b�N�i�ʁj
 *
 * @param	wk		�`���f�����[�N
 * @param	id		���f���h�c
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL LDMAIN_ObjAnmCheck( LEGEND_DEMO_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���A�j���`�F�b�N�i�S�́j
 *
 * @param	wk		�`���f�����[�N
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL LDMAIN_AllObjAnmCheck( LEGEND_DEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[������
 *
 * @param	eva		�u�����h�W��A
 * @param	evb		�u�����h�W��B
 *
 * @return	FLD_MOTION_BL_PTR	���[�V�����u���[�|�C���^
 */
//--------------------------------------------------------------------------------------------
extern LDMAIN_MBL_WORK * LDMAIN_MotionBlInit( int eva, int evb );

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[�폜
 *
 * @param	mbl		���[�V�����u���[�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void LDMAIN_MotionBlExit( LDMAIN_MBL_WORK * mb );

#endif
