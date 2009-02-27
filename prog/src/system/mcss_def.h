
//============================================================================================
/**
 * @file	mcss_def.h
 * @brief	�}���`�Z���\�t�g�E�G�A�X�v���C�g�`��p�֐�
 * @author	soga
 * @date	2008.11.17
 */
//============================================================================================

//--------------------------------------------------------------------------
/**
 * �萔��`
 */
//--------------------------------------------------------------------------

#define SCREEN_WIDTH		(256)			// ��ʕ�
#define SCREEN_HEIGHT		(192)			// ��ʍ���

#define SIZE_OF_RES_POOL	(1000)			// ���\�[�X�v�[���̃T�C�Y

#define	DEFAULT_Z_OFFSET	(-0x100)		//NNS_G2dSetRendererSpriteZoffset�ɓn���Z���`�斈�ɍX�V����Z�l

#define	DOT_LENGTH			(FX32_HALF >> 5)	//�Z���f�[�^1�h�b�g�ɑ΂���|���S���̒����̊

//--------------------------------------------------------------------------
/**
 * �\���̒�`
 */
//--------------------------------------------------------------------------
struct _MCSS_NCEC
{

	fx32	pos_x;		//�Z���`��X���W
	fx32	pos_y;		//�Z���`��Y���W
	fx32	size_x;		//�Z���T�C�YX
	fx32	size_y;		//�Z���T�C�YY
	fx32	tex_s;		//�e�N�X�`��s�l
	fx32	tex_t;		//�e�N�X�`��t�l

	//�ȉ��A���p�`�p�L����
	fx32	mepachi_pos_x;		//�Z���`��X���W
	fx32	mepachi_pos_y;		//�Z���`��Y���W
	fx32	mepachi_size_x;		//�Z���T�C�YX
	fx32	mepachi_size_y;		//�Z���T�C�YY
	fx32	mepachi_tex_s;		//�e�N�X�`��s�l
	fx32	mepachi_tex_t;		//�e�N�X�`��t�l
};

struct _MCSS_NCEC_WORK
{
	u32			cells;		//�Z������
	MCSS_NCEC	ncec[1];	//�Z�����i�ςȂ̂ŁA1�������m�ہj
};

struct _MCSS_WORK
{
	void						*mcss_ncer_buf;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellDataBank			*mcss_ncer;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����ncer�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmcr_buf;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellDataBank		*mcss_nmcr;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmcr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nanr_buf;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellAnimBankData		*mcss_nanr;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nanr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmar_buf;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellAnimBankData	*mcss_nmar;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmar�f�[�^�𒊏o�����f�[�^
	MCSS_NCEC_WORK				*mcss_ncec;				//1���̔|���ŕ\�����邽�߂̏�񂪊i�[���ꂽ�Ǝ��t�H�[�}�b�g�f�[�^
	NNSG2dMultiCellAnimation	mcss_mcanim;			//�}���`�Z���A�j���[�V�����̎���
	void						*mcss_mcanim_buf;		//�}���`�Z���A�j���[�V�����̎��̂̓����Ŏg�p���郏�[�N�̈�
	NNSG2dImageProxy			mcss_image_proxy;		//�e�N�X�`���v���L�V
	NNSG2dImagePaletteProxy		mcss_palette_proxy;		//�p���b�g�v���L�V
	VecFx32						pos;					//�}���`�Z���̃|�W�V����
	VecFx32						scale;					//�}���`�Z���̃X�P�[��
	VecFx32						shadow_scale;			//�e�̃X�P�[��
	u32							mepachi_flag	:1;		//���p�`�t���O
	u32							anm_stop_flag	:1;		//�A�j���X�g�b�v�t���O
	u32							vanish_flag		:1;		//�o�j�b�V���t���O
	u32											:29;
	int							index;					//�o�^INDEX
	int							heapID;					//�g�p����q�[�vID
};

struct _MCSS_SYS_WORK
{
	int						mcss_max;			//�o�^��MAX
	MCSS_WORK				**mcss;				//�o�^�}���`�Z���\����
#ifdef USE_RENDER
	NNSG2dRendererInstance	mcss_render;		//�`��p Render�@
	NNSG2dRenderSurface		mcss_surface;		//���C����� Surface
#endif //USE_RENDER
	u32						mcss_ortho_mode	:1;	//���ˉe�`�惂�[�h�t���O
	u32										:31;
	int						texAdrs;			//�e�N�X�`���]���J�n�A�h���X
	int						palAdrs;			//�e�N�X�`���p���b�g�]���J�n�A�h���X
	int						heapID;				//�g�p����q�[�vID
};

