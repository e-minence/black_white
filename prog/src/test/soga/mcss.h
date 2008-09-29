
//--------------------------------------------------------------------------
/**
 * �萔��`
 */
//--------------------------------------------------------------------------

#define SCREEN_WIDTH		(256)	// ��ʕ�
#define SCREEN_HEIGHT		(192)	// ��ʍ���

#define SIZE_OF_RES_POOL	(1000)	// ���\�[�X�v�[���̃T�C�Y

#define	DEFAULT_Z_OFFSET	(-0x100)	//NNS_G2dSetRendererSpriteZoffset�ɓn���Z���`�斈�ɍX�V����Z�l

//--------------------------------------------------------------------------
/**
 * �\���̒�`
 */
//--------------------------------------------------------------------------
typedef struct
{
	void						*mcss_ncer_buf;					//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellDataBank			*mcss_ncer;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����ncer�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmcr_buf;					//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellDataBank		*mcss_nmcr;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmcr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nanr_buf;					//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellAnimBankData		*mcss_nanr;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nanr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmar_buf;					//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellAnimBankData	*mcss_nmar;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmar�f�[�^�𒊏o�����f�[�^
	NNSG2dMultiCellAnimation	mcss_mcanim;					//�}���`�Z���A�j���[�V�����̎���
	void						*mcss_mcanim_buf;				//�}���`�Z���A�j���[�V�����̎��̂̓����Ŏg�p���郏�[�N�̈�
	NNSG2dImageProxy			mcss_image_proxy;				//�e�N�X�`���v���L�V
	NNSG2dImagePaletteProxy		mcss_palette_proxy;				//�p���b�g�v���L�V
	VecFx32						pos;							//�}���`�Z���̃|�W�V����
	int							index;							//�o�^INDEX
	int							heapID;							//�g�p����q�[�vID
}MCSS_WORK;

typedef struct
{
	int						max;				//�o�^��MAX
	MCSS_WORK				**mcss;				//�o�^�}���`�Z���\����
	NNSG2dRendererInstance	mcss_render;		//�`��p Render�@
	NNSG2dRenderSurface		mcss_surface;		//���C����� Surface
	VecFx32					*camPos;			// �J�����̈ʒu(�����_)
	VecFx32					*target;			// �J�����̏œ_(�������_)
	VecFx32					*camUp;				//
	int						heapID;				//�g�p����q�[�vID
}MCSS_SYS_WORK;

extern	MCSS_SYS_WORK*	MCSS_Init( int max, VecFx32 *camPos,VecFx32 *target, VecFx32 *camUp,HEAPID heapID );
extern	void	MCSS_Exit( MCSS_SYS_WORK *mcss_sys );
extern	void	MCSS_Main( MCSS_SYS_WORK *mcss_sys );
extern	void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys );
extern	MCSS_WORK*	MCSS_Add( MCSS_SYS_WORK *mcss_sys,
					  fx32			pos_x,
					  fx32			pos_y,
					  fx32			pos_z,
					  ARCID			arcID,
					  ARCDATID		ncbr,
					  ARCDATID		nclr,
					  ARCDATID		ncer,
					  ARCDATID		nanr,
					  ARCDATID		nmcr,
					  ARCDATID		nmar );
extern	void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );
