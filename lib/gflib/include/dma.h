
//=============================================================================================
/**
 * @file	dma.h
 * @brief	DMA�V�X�e���v���O����
 * @author	Hisashi Sogabe
 * @date	2007/04/26
 */
//=============================================================================================

#ifndef __DMA_H_
#define __DMA_H_

#ifdef __cplusplus
extern "C" {
#endif


//DMA���g�����
typedef enum {
  GFL_DMA_NET_NO = 1,
  GFL_DMA_GX_NO= 3,
} GFL_DMA_NO ;




//DMA�C�l�[�u���t���O�i0:disable 1:enable�j
#define	GFL_DMA_DISABLE				(0)
#define	GFL_DMA_ENABLE				(1)

//���荞�ݗv�������C�l�[�u���i0:disable 1:enable�j
#define	GFL_DMA_INTR_DISABLE		(0)
#define	GFL_DMA_INTR_ENABLE			(1)

/*
DMA�N�����[�h(000:�����ɋN�� 001:VBlank 010:HBlank 011:�\������
			  100:���C���������\�� 101:NITRO�J�[�h 110:�J�[�g���b�W
			  111:�W�I���g���R�}���hFIFO�j
*/
#define	GFL_DMA_MODE_SOON			(0)
#define	GFL_DMA_MODE_VBLANK			(1)
#define	GFL_DMA_MODE_HBLANK			(2)
#define	GFL_DMA_MODE_DISPLAY		(3)
#define	GFL_DMA_MODE_MAINMEMORY		(4)
#define	GFL_DMA_MODE_NITROCARD		(5)
#define	GFL_DMA_MODE_CARTRIDGE		(6)
#define	GFL_DMA_MODE_GEOMETRY		(7)

//�]���r�b�g�i0:16bit 1:32bit�j
#define	GFL_DMA_SEND_BIT_16			(0)
#define	GFL_DMA_SEND_BIT_32			(1)

//�J��Ԃ����[�h�I��(0:OFF 1:ON�j
#define	GFL_DMA_CONTINUE_MODE_OFF	(0)
#define	GFL_DMA_CONTINUE_MODE_ON	(1)

//�]�����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�ݒ�֎~�j
#define	GFL_DMA_SAR_INC				(0)
#define	GFL_DMA_SAR_DEC				(1)
#define	GFL_DMA_SAR_FIX				(2)

//�]����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�C���N/�����[�h�j
#define	GFL_DMA_DAR_INC				(0)
#define	GFL_DMA_DAR_DEC				(1)
#define	GFL_DMA_DAR_FIX				(2)
#define	GFL_DMA_DAR_RELOAD			(3)
 
//--------------------------------------------------------------------------------------------
/**
 * DMA������
 *
 * @param	heapID	���[�N�f�[�^���擾����q�[�vID
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_DMA_Init(HEAPID heapID);

//--------------------------------------------------------------------------------------------
/**
 * DMA�ݒ菈���iVBlank�̍Ō�ŌĂԂ��Ƃ𐄏��j
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_DMA_Main(void);

//--------------------------------------------------------------------------------------------
/**
 * DMA�I��
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_DMA_Exit(void);

//--------------------------------------------------------------------------------------------
/**
 * DMA�J�n
 *
 * @param	ch				�g�pDMAch
 * @param	sad				�]�����A�h���X
 * @param	dad				�]����A�h���X
 * @param	dma_enable		DMA�C�l�[�u���t���O�i0:disable 1:enable�j
 * @param	intr_enable		���荞�ݗv�������C�l�[�u���i0:disable 1:enable�j
 * @param	dma_mode		DMA�N�����[�h(000:�����ɋN�� 001:VBlank 010:HBlank 011:�\������
 *										  100:���C���������\�� 101:NITRO�J�[�h 110:�J�[�g���b�W
 *										  111:�W�I���g���R�}���hFIFO�j
 * @param	send_bit		�]���r�b�g�i0:16bit 1:32bit�j
 * @param	continue_mode	�J��Ԃ����[�h�I��(0:OFF 1:ON�j
 * @param	sar				�]�����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�ݒ�֎~�j
 * @param	dar				�]����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�C���N/�����[�h�j
 * @param	word_count		�]����
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_DMA_Start(int ch,u32 sad,u32 dad,int dma_enable,int intr_enable,int dma_mode,int send_bit,int continue_mode,int sar,int	dar,int word_count);

//--------------------------------------------------------------------------------------------
/**
 * DMA��~
 *
 * @param	ch	��~��������DMAch
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_DMA_Stop(int ch);

//--------------------------------------------------------------------------------------------
/**
 * GX��DMA���g��or�g��Ȃ���؂�ւ���
 *
 * @param	bEnable	�g����TRUE �g��Ȃ���FALSE
 */
//--------------------------------------------------------------------------------------------

extern void GFL_DMA_GXDmaEnable(BOOL bEnable);

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif __DMA_H_

