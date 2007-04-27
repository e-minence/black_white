
//=============================================================================================
/**
 * @file	dma.c
 * @brief	DMA�V�X�e���v���O����
 * @author	Hisashi Sogabe
 * @date	2007/04/24
 */
//=============================================================================================
#include "gflib.h"
#include "../build/libraries/mi/common/include/mi_dma.h"

//=============================================================================================
//	�萔�錾
//=============================================================================================

#define	GFL_DMA_MAX	(4)		//DMA�`�����l����MAX�l

//=============================================================================================
//	�^�錾
//=============================================================================================

typedef struct
{
	u32	sad;		//�]�����A�h���X
	u32	dad;		//�]����A�h���X
	union{
		u32	param;
		struct{
			u32	dma_enable		:1;		//DMA�C�l�[�u��
			u32	intr_enable		:1;		//���荞�ݗv�������C�l�[�u��
			u32	dma_mode		:3;		/*DMA�N�����[�h(000:�����ɋN�� 001:VBlank 010:HBlank 011:�\������
														100:���C���������\�� 101:NITRO�J�[�h 110:�J�[�g���b�W
														111:�W�I���g���R�}���hFIFO�j*/
			u32	send_bit		:1;		//�]���r�b�g�i0:16bit 1:32bit�j
			u32	continue_mode	:1;		//�J��Ԃ����[�h�I��(0:OFF 1:ON�j
			u32	sar				:2;		//�]�����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�ݒ�֎~�j
			u32	dar				:2;		//�]����A�h���X�X�V���@�i00:�C���N�������g 01:�f�N�������g 10:�Œ� 11:�C���N/�����[�h�j
			u32	word_count		:21;	//�]����
		};
	}cnt;
	u32	req_flag	:1;		//�ݒ�v���t���O
	u32	set_flag	:1;		//�ݒ�ς݃t���O
	u32				:31;
}GFL_DMA_PARAM;

typedef struct
{
	GFL_DMA_PARAM	gdp[GFL_DMA_MAX];
}GFL_DMA_WORK;

//=============================================================================================
//	���[�N
//=============================================================================================

static	GFL_DMA_WORK *gdw=NULL;

//=============================================================================================
//	�ݒ�֐�
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * DMA������
 *
 * @param	heapID	���[�N�f�[�^���擾����q�[�vID
 */
//--------------------------------------------------------------------------------------------
void	GFL_DMA_Init(HEAPID heapID)
{
	GF_ASSERT(gdw==NULL);

	gdw=GFL_HEAP_AllocClearMemory(heapID,sizeof(GFL_DMA_WORK));
}

//--------------------------------------------------------------------------------------------
/**
 * DMA�ݒ菈���iVBlank�̍Ō�ŌĂԂ��Ƃ𐄏��j
 */
//--------------------------------------------------------------------------------------------
void	GFL_DMA_Main(void)
{
	int	ch;

	if(gdw==NULL){
		return;
	}

	for(ch=0;ch<GFL_DMA_MAX;ch++){
		if(gdw->gdp[ch].req_flag){
			gdw->gdp[ch].req_flag=0;
			gdw->gdp[ch].set_flag=1;
			if(gdw->gdp[ch].cnt.word_count){
				GFL_DMA_Stop(ch);
				MIi_DmaSetParams(ch,(u32)gdw->gdp[ch].sad,(u32)gdw->gdp[ch].dad,gdw->gdp[ch].cnt.param);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * DMA�I��
 */
//--------------------------------------------------------------------------------------------
void	GFL_DMA_Exit(void)
{
	int	ch;

	GF_ASSERT(gdw!=NULL);

	for(ch=0;ch<GFL_DMA_MAX;ch++){
		GFL_DMA_Stop(ch);
	}

	GFL_HEAP_FreeMemory(gdw);

	gdw=NULL;
}

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
void	GFL_DMA_Start(int ch,u32 sad,u32 dad,int dma_enable,int intr_enable,int dma_mode,int send_bit,int continue_mode,int sar,int	dar,int word_count)
{
	GF_ASSERT(ch<GFL_DMA_MAX);

	gdw->gdp[ch].sad=sad;
	gdw->gdp[ch].dad=dad;
	gdw->gdp[ch].cnt.param=REG_MI_DMA0CNT_FIELD(dma_enable,intr_enable,dma_mode,send_bit,continue_mode,sar,dar,word_count);

	gdw->gdp[ch].req_flag=1;
}

//--------------------------------------------------------------------------------------------
/**
 * DMA��~
 *
 * @param	ch	��~��������DMAch
 */
//--------------------------------------------------------------------------------------------
void	GFL_DMA_Stop(int ch)
{
    vu32   *p = (vu32 *)((u32)REG_DMA0CNT_ADDR + ch * 12);

    *p = (vu32)REG_MI_DMA0CNT_FIELD(GFL_DMA_ENABLE,
									gdw->gdp[ch].cnt.intr_enable,
									GFL_DMA_MODE_SOON,
									gdw->gdp[ch].cnt.send_bit,
									GFL_DMA_CONTINUE_MODE_OFF,
									gdw->gdp[ch].cnt.sar,
									gdw->gdp[ch].cnt.dar,
									gdw->gdp[ch].cnt.word_count);

    // ARM7 must wait 2 cycle (load is 3 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }

    *p = (vu32)REG_MI_DMA0CNT_FIELD(GFL_DMA_DISABLE,
									gdw->gdp[ch].cnt.intr_enable,
									GFL_DMA_MODE_SOON,
									gdw->gdp[ch].cnt.send_bit,
									GFL_DMA_CONTINUE_MODE_OFF,
									gdw->gdp[ch].cnt.sar,
									gdw->gdp[ch].cnt.dar,
									gdw->gdp[ch].cnt.word_count);
	gdw->gdp[ch].set_flag=0;
}

