
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
	u32       cells;		//�Z������
  u16       size_x;   //
  u16       size_y;
  s16       ofs_x;   //
  s16       ofs_y;
	MCSS_NCEC	ncec[1];	//�Z�����i�ςȂ̂ŁA1�������m�ہj
};

struct _MCSS_NCEN_WORK
{
  u8    stop_cellanms;    //�Î~�A�j���ł�����������Z���A�j���̖���
  u8    fly_flag;         //�����Ă���}���`�Z�����t���O
  u8    stop_node[2];     //�Z���A�j���m�[�h���i�ςȂ̂�1�������m�ہj
};

struct _MCSS_WORK
{
  GFL_TCB*                      tcb_load_resource;      //���\�[�X�ǂݍ���tcb
  GFL_TCB*                      tcb_load_palette;       //���\�[�X�ǂݍ���tcb
  GFL_TCB*                      tcb_load_base_palette;  //���\�[�X�ǂݍ���tcb
	void*													mcss_ncer_buf;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellDataBank*						mcss_ncer;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����ncer�f�[�^�𒊏o�����f�[�^
	void*													mcss_nmcr_buf;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellDataBank*			mcss_nmcr;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmcr�f�[�^�𒊏o�����f�[�^
	void*													mcss_nanr_buf;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dCellAnimBankData*				mcss_nanr;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nanr�f�[�^�𒊏o�����f�[�^
	void*													mcss_nmar_buf;				//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@
	NNSG2dMultiCellAnimBankData*	mcss_nmar;						//�t�@�C���f�[�^�ǂݍ��݃o�b�t�@����nmar�f�[�^�𒊏o�����f�[�^
	MCSS_NCEC_WORK*								mcss_ncec;						//1���̔|���ŕ\�����邽�߂̏�񂪊i�[���ꂽ�Ǝ��t�H�[�}�b�g�f�[�^
  MCSS_NCEN_WORK*               mcss_ncen;            //�Î~�A�j���p�m�[�h���
	NNSG2dMultiCellAnimation			mcss_mcanim;					//�}���`�Z���A�j���[�V�����̎���
	void*													mcss_mcanim_buf;			//�}���`�Z���A�j���[�V�����̎��̂̓����Ŏg�p���郏�[�N�̈�
	NNSG2dImageProxy							mcss_image_proxy;			//�e�N�X�`���v���L�V
	NNSG2dImagePaletteProxy				mcss_palette_proxy;		//�p���b�g�v���L�V
	u16														*base_pltt_data;			//�p���b�g�f�[�^�i�p���b�g�t�F�[�h�̃x�[�X�J���[�j
	u16														*fade_pltt_data;			//�p���b�g�f�[�^�i�ʂ̃p���b�g�t�F�[�h��K�����ăx�[�X�J���[�ɂ���j
	int														pltt_data_size;				//�p���b�g�f�[�^�T�C�Y
	VecFx32												pos;									//�}���`�Z���̃|�W�V����
	VecFx32												scale;								//�}���`�Z���̃X�P�[��
	VecFx32												rotate;								//�}���`�Z���̉�]
	VecFx32												shadow_scale;					//�e�̃X�P�[��
	VecFx32												ofs_pos;							//�|�W�V�����I�t�Z�b�g
	VecFx32												ofs_scale;						//�X�P�[���I�t�Z�b�g
	s8														pal_fade_start_evy;		//�p���b�g�t�F�[�h�@START_EVY�l
	s8														pal_fade_end_evy;			//�p���b�g�t�F�[�h�@END_EVY�l
	s8														pal_fade_wait;				//�p���b�g�t�F�[�h�@wait�l
	s8														pal_fade_wait_tmp;		//�p���b�g�t�F�[�h�@wait_tmp�l
	int														pal_fade_value;			  //�p���b�g�t�F�[�h�@�t�F�[�h�l
	u32														pal_fade_rgb;					//�p���b�g�t�F�[�h�@end_evy����rgb�l
	u32														alpha					      :8; //alpha�l
	u32														mepachi_flag	      :1;	//���p�`�t���O
	u32														anm_stop_flag	      :2;	//�A�j���X�g�b�v�t���O
	u32														vanish_flag		      :1;	//�o�j�b�V���t���O
	u32														pal_fade_flag	      :1;	//�p���b�g�t�F�[�h�t���O
	u32                           is_load_resource    :1; //���\�[�X�ǂݍ��݊����t���O
  u32                           shadow_alpha        :9; //�e�A���t�@(32�Œʏ�̔�����
	u32														shadow_vanish_flag  :1;    
	u32														mosaic				      :4; //���U�C�N    
	u32														fade_pltt_data_flag :1;    
	u32														ortho_mode          :1;    
	u32														reverse_draw        :1; //�t����`��    
	u32														                    :1;    
	int														index;								//�o�^INDEX
	int														heapID;								//�g�p����q�[�vID
  MCSS_ADD_WORK                 maw;

  fx32                          mcss_anm_frame;       //�A�j���[�V������i�߂�t���[����

  u16                           shadow_rotate;
  VecFx32                       shadow_offset;
};

struct _MCSS_SYS_WORK
{
  ARCHANDLE*              handle;               //���\�[�X��ARCHANDLE
  ARCID                   arcID;                //���\�[�X��ARCHANDLE���I�[�v������ARCID
  GFL_TCBSYS*             tcb_sys;              //���\�[�X�ǂݍ���tcbsys�iVBlank�O�ł̓]���j
  GFL_TCB*                tcb_load_shadow;      //�e���\�[�X�ǂݍ���tcb
	int											mcss_max;							//�o�^��MAX
	MCSS_WORK**							mcss;									//�o�^�}���`�Z���\����
#ifdef USE_RENDER
	NNSG2dRendererInstance	mcss_render;					//�`��p Render�@
	NNSG2dRenderSurface			mcss_surface;					//���C����� Surface
#endif //USE_RENDER
	NNSG2dImagePaletteProxy	shadow_palette_proxy;   	  //�p���b�g�v���L�V�i�e�p�j
  u32                     mcss_ortho_mode       :1;   //���ˉe�`�惂�[�h�t���O
	u32                     perspective_far_flag  :1;   //�����ˉeFAR�t���O
	u32                     ortho_far_flag        :1;   //���ˉeFAR�t���O
  u32                                           :29;
	u32											texAdrs;							//�e�N�X�`���]���J�n�A�h���X
	u32											palAdrs;							//�e�N�X�`���p���b�g�]���J�n�A�h���X
  MCSS_CALLBACK_FUNC*     load_resource_callback;   //LoadResource�̑O�ɌĂ΂��R�[���o�b�N�i�L�����f�[�^����Ɏg�p�j
  u32                     callback_work;        //�R�[���o�b�N�֐����ĂԎ��̈����̃|�C���^
	int											heapID;								//�g�p����q�[�vID
	
	fx32                    mcAnimRate; //�}���`�Z���A�j���{��
  fx32                    scale_offset;
  fx32                    scale_offset_work;
};

