
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

typedef struct
{

	u16		char_no;	//�Z���Ŏg�p����L�����N�^No
	u8		size_x;		//�Z���T�C�YX
	u8		size_y;		//�Z���T�C�YY
	s32		pos_x;		//�Z���`��X���W
	s32		pos_y;		//�Z���`��Y���W

	//�ȉ��A���p�`�p�L����
	u16		mepachi_char_no;	//�Z���Ŏg�p����L�����N�^No
	u8		mepachi_size_x;		//�Z���T�C�YX
	u8		mepachi_size_y;		//�Z���T�C�YY
	s32		mepachi_pos_x;		//�Z���`��X���W
	s32		mepachi_pos_y;		//�Z���`��Y���W

}MCSS_NCEC;

typedef struct
{
	u32			cells;		//�Z������
	MCSS_NCEC	ncec[1];	//�Z�����i�ςȂ̂ŁA1�������m�ہj
}MCSS_NCEC_WORK;

typedef struct
{
	void						*mcss_ncer_buf;		//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellDataBank			*mcss_ncer;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����ncer�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmcr_buf;		//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellDataBank		*mcss_nmcr;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmcr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nanr_buf;		//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellAnimBankData		*mcss_nanr;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nanr�f�[�^�𒊏o�����f�[�^
	void						*mcss_nmar_buf;		//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellAnimBankData	*mcss_nmar;			//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmar�f�[�^�𒊏o�����f�[�^
	MCSS_NCEC_WORK				*mcss_ncec;			//1���̔|���ŕ\�����邽�߂̏�񂪊i�[���ꂽ�Ǝ��t�H�[�}�b�g�f�[�^
	NNSG2dMultiCellAnimation	mcss_mcanim;		//�}���`�Z���A�j���[�V�����̎���
	void						*mcss_mcanim_buf;	//�}���`�Z���A�j���[�V�����̎��̂̓����Ŏg�p���郏�[�N�̈�
	NNSG2dImageProxy			mcss_image_proxy;	//�e�N�X�`���v���L�V
	NNSG2dImagePaletteProxy		mcss_palette_proxy;	//�p���b�g�v���L�V
	VecFx32						pos;				//�}���`�Z���̃|�W�V����
	fx32						scale_x;
	fx32						scale_y;
	int							mepachi_flag;		//���p�`�t���O�i�b��j
	int							index;				//�o�^INDEX
	int							heapID;				//�g�p����q�[�vID
}MCSS_WORK;

typedef struct
{
	int						max;				//�o�^��MAX
	MCSS_WORK				**mcss;				//�o�^�}���`�Z���\����
#ifdef USE_RENDER
	NNSG2dRendererInstance	mcss_render;		//�`��p Render�@
	NNSG2dRenderSurface		mcss_surface;		//���C����� Surface
#endif //USE_RENDER
	GFL_G3D_CAMERA			*camera;			//�J�����ւ̃|�C���^
	int						texAdrs;			//�e�N�X�`���]���J�n�A�h���X
	int						palAdrs;			//�e�N�X�`���p���b�g�]���J�n�A�h���X
	int						heapID;				//�g�p����q�[�vID
}MCSS_SYS_WORK;

extern	MCSS_SYS_WORK*	MCSS_Init( int max, GFL_G3D_CAMERA *camera, HEAPID heapID );
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
					  ARCDATID		nmar,
					  ARCDATID		ncec);
extern	void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );
extern	void	MCSS_SetScaleX( MCSS_WORK *mcss, fx32 scale_x );
extern	void	MCSS_SetScaleY( MCSS_WORK *mcss, fx32 scale_y );

