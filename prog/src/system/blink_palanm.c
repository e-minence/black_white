//============================================================================================
/**
 * @file		blink_palanm.c
 * @brief		�_�Ńp���b�g�A�j������
 * @author	Hiroyuki Nakamura
 * @date		10.01.18
 *
 *	���W���[�����FBLINKPALANM
 *
 *	�w��J���[����w��J���[�ւ̃p���b�g�A�j���ł��B
 *	�J�[�\���A�j���Ŏg�p���Ă��܂��B
 */
//============================================================================================
#include <gflib.h>

#include "system/blink_palanm.h"


//============================================================================================
//	�萔��`
//============================================================================================
struct _BLINK_PALANM_WORK {
	u16 * startPal;
	u16 * endPal;
	u16 * transPal;
	u16	offs;
	u16	size;
	u16	cnt;
	u16	seq;
	NNS_GFD_DST_TYPE	type;
	HEAPID	heapID;
};

#define ANM_CNT_VALUE		( 0x400 )			// �A�j���J�E���g
#define ANM_CNT_MAX			( 0x10000 )		// �A�j���J�E���g�ő�l

#define	COL_R(a)	( ( a >> GX_RGB_R_SHIFT ) & 0x1f )	// �w��J���[����Ԃ𔲂��o��
#define	COL_G(a)	( ( a >> GX_RGB_G_SHIFT ) & 0x1f )	// �w��J���[����΂𔲂��o��
#define	COL_B(a)	( ( a >> GX_RGB_B_SHIFT ) & 0x1f )	// �w��J���[����𔲂��o��



//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����[�N�쐬
 *
 * @param		offs		�A�j��������p���b�g�ʒu�i�J���[�P�ʁj
 * @param		size		�A�j��������p���b�g�̃T�C�Y�i�J���[�P�ʁj
 * @param		bgfrm		�a�f�t���[��
 * @param		hapID		�q�[�v�h�c
 *
 * @return	�A�j�����[�N
 */
//--------------------------------------------------------------------------------------------
BLINKPALANM_WORK * BLINKPALANM_Create( u16 offs, u16 size, u16 bgfrm, HEAPID heapID )
{
	BLINKPALANM_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(BLINKPALANM_WORK) );
	wk->transPal = GFL_HEAP_AllocMemory( heapID, sizeof(u16) * size );
	wk->offs = offs;
	wk->size = size;
	wk->cnt = 0;
	wk->seq = 0;
	wk->heapID = heapID;

	if( bgfrm <= GFL_BG_FRAME3_M ){
		wk->type = NNS_GFD_DST_2D_BG_PLTT_MAIN;
	}else{
		wk->type = NNS_GFD_DST_2D_BG_PLTT_SUB;
	}

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ݒ�
 *
 * @param		wk				�p���b�g�A�j�����[�N
 * @param		arcID			�A�[�J�C�u�h�c
 * @param		datID			�f�[�^�h�c
 * @param		startPos	�J�n��p���b�g�ʒu�i�J���[�P�ʁj
 * @param		endPos		�I����p���b�g�ʒu�i�J���[�P�ʁj
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BLINKPALANM_SetPalBufferArc( BLINKPALANM_WORK * wk, ARCID arcID, ARCDATID datID, u32 startPos, u32 endPos )
{
	NNSG2dPaletteData * pal;
	void * buf;
	u16 * dat;

	wk->startPal = GFL_HEAP_AllocMemory( wk->heapID, sizeof(u16) * wk->size );
	wk->endPal   = GFL_HEAP_AllocMemory( wk->heapID, sizeof(u16) * wk->size );

	buf = GFL_ARC_UTIL_LoadPalette( arcID, datID, &pal, wk->heapID );
	dat = (u16 *)pal->pRawData;

	GFL_STD_MemCopy( &dat[startPos], wk->startPal, sizeof(u16) * wk->size );
	GFL_STD_MemCopy( &dat[endPos], wk->endPal, sizeof(u16) * wk->size );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ݒ�i�n���h���w��j
 *
 * @param		wk				�p���b�g�A�j�����[�N
 * @param		ah				�A�[�N�n���h��
 * @param		datID			�f�[�^�h�c
 * @param		startPos	�J�n��p���b�g�ʒu�i�J���[�P�ʁj
 * @param		endPos		�I����p���b�g�ʒu�i�J���[�P�ʁj
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BLINKPALANM_SetPalBufferArcHDL( BLINKPALANM_WORK * wk, ARCHANDLE * ah, ARCDATID datID, u32 startPos, u32 endPos )
{
	NNSG2dPaletteData * pal;
	void * buf;
	u16 * dat;

	wk->startPal = GFL_HEAP_AllocMemory( wk->heapID, sizeof(u16) * wk->size );
	wk->endPal   = GFL_HEAP_AllocMemory( wk->heapID, sizeof(u16) * wk->size );

	buf = GFL_ARCHDL_UTIL_LoadPalette( ah, datID, &pal, wk->heapID );
	dat = (u16 *)pal->pRawData;

	GFL_STD_MemCopy( &dat[startPos], wk->startPal, sizeof(u16) * wk->size );
	GFL_STD_MemCopy( &dat[endPos], wk->endPal, sizeof(u16) * wk->size );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����[�N�폜
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BLINKPALANM_Exit( BLINKPALANM_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->endPal );
	GFL_HEAP_FreeMemory( wk->startPal );
	GFL_HEAP_FreeMemory( wk->transPal );
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����상�C��
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BLINKPALANM_Main( BLINKPALANM_WORK * wk )
{
	fx32	cos;
	u32	i;
	u8	r, g, b;

	if( ( wk->cnt + ANM_CNT_VALUE ) >= ANM_CNT_MAX ){
		wk->cnt = wk->cnt + ANM_CNT_VALUE - ANM_CNT_MAX;
	}else{
		wk->cnt += ANM_CNT_VALUE;
	}

	cos = ( FX_CosIdx(wk->cnt) + FX16_ONE ) / 2;

	for( i=0; i<wk->size; i++ ){
		r = COL_R(wk->startPal[i]) + (((COL_R(wk->endPal[i])-COL_R(wk->startPal[i]))*cos)>>FX16_SHIFT);
		g = COL_G(wk->startPal[i]) + (((COL_G(wk->endPal[i])-COL_G(wk->startPal[i]))*cos)>>FX16_SHIFT);
		b = COL_B(wk->startPal[i]) + (((COL_B(wk->endPal[i])-COL_B(wk->startPal[i]))*cos)>>FX16_SHIFT);

		wk->transPal[i] = GX_RGB( r, g, b );
	}

	NNS_GfdRegisterNewVramTransferTask( wk->type, wk->offs * 2, wk->transPal, wk->size * 2 );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�j������J�E���^�[��������
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BLINKPALANM_InitAnimeCount( BLINKPALANM_WORK * wk )
{
	wk->cnt = ANM_CNT_MAX;
}
