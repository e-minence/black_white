//=============================================================================
/**
 * @file	wm_icon.c
 * @brief	�ʐM���ɕ\�������A�C�R��
 * @author	gamefreak
 * @date    2007.3.1
 */
//=============================================================================

#include "gflib.h"
#include "net.h"
#include "wm_icon.h"
#include "device/net_wireless.h"

// �p���b�g�t�@�C���̑傫��
#define WM_ICON_PALFILE_SIZE	( 600 )
#define WM_ICON_CHARFILE_SIZE	( 600 )

// �ʐM�A�C�R���������߂ɓ]������PAL_VRAM�̈ʒu�E�傫��(14�ԃp���b�g�g�p�j
#define WM_ICON_PAL_POS		( 14 )
#define WM_ICON_PAL_OFFSET	( 16 * 2 * WM_ICON_PAL_POS )
#define WM_ICON_PAL_SIZE	( 16 * 2 )


// �]���T�C�Y
#define WM_ICON_CHAR_SIZE    ( 4*4*32  )		

// �A�j���p�^�[���̐�
#define WM_ICON_ANIME_MAX	 ( 4 )




#define WM_ICON_MAIN_OAM_ADRS	(GXOamAttr*)(HW_OAM)
#define WM_ICON_SUB_OAM_ADRS	(GXOamAttr*)(HW_OAM_END)

// ���C����ʁE�T�u��ʂ̐؂�ւ��ɍ��킹�铮�샂�[�h
enum {
	MAIN_SUB_MODE_DEFAULT,	///< �������Ȃ��i���C���ɏo������j
	MAIN_SUB_MODE_TOP,		///< ���ʂɏo������
	MAIN_SUB_MODE_BOTTOM,	///< ����ʂɏo������
};

//----------------------------------------------
/**
 *	�Ǘ��p�\���̒�`
 */
//----------------------------------------------
typedef struct _VINTR_WIRELESS_ICON		VINTR_WIRELESS_ICON;

struct _VINTR_WIRELESS_ICON {
	int		anime_seq;
	int     anime;
	u16     x,y;
    u8      bWifi;
    u8      main_sub_mode;
    u8      sub_wrote_flag;
	const 	VOamAnm *table;
	GXOamAttr*  prevOamPtr;
};



//==============================================================
// Prototype
//==============================================================
static void WirelessIconAnimeFunc( void *work );
static void trans_palette_data( int vramType, BOOL bWifi, u32 offset, HEAPID heapID );
static void trans_cgx_data( int vramType, BOOL bWifi, HEAPID heapID );
static int inline get_target_vram( VINTR_WIRELESS_ICON* vwi );
static int inline calc_anime_index( int targetVram, int anime_ptn );


//==============================================================================
/**
 * Vblank�ʐM�A�C�R���A�j���J�n�֐�
 *
 * @param   objVRAM		
 * @param   HeapId		�q�[�v
 *
 * @retval  VINTR_WIRELESS_ICON *		
 */
//==============================================================================
//==============================================================================
/**
 * $brief   Vblank�ʐM�A�C�R���A�j���J�n�֐�
 *
 * @param   objVRAM		���g�p�i�����܂��j
 * @param   HeapId		
 * @param   x			�\���ʒuX
 * @param   y			�\���ʒuY
 * @param   bWifi		DS�ʐM�A�C�R�����Hwifi�ʐM�A�C�R�����H
 * @param   tbl[]		�A�j���e�[�u���i���������j
 *
 * @retval  VINTR_WIRELESS_ICON *		
 */
//==============================================================================
static VINTR_WIRELESS_ICON *AddWirelessIconOAM(u32 objVRAM, u32 HeapId, int x, int y, BOOL bWifi, const VOamAnm *tbl)
{
	VINTR_WIRELESS_ICON *vwi;

	// �p���b�g�ǂݍ��݃��C��14�Ԃɓ]��
	trans_palette_data( NNS_G2D_VRAM_TYPE_2DMAIN, bWifi, WM_ICON_PAL_OFFSET, HeapId );

	// VRAM�̍Ō��CGX�f�[�^�]��
	trans_cgx_data( NNS_G2D_VRAM_TYPE_2DMAIN, bWifi, HeapId );

	// �^�X�N�o�^�E���[�N������
	vwi        = (VINTR_WIRELESS_ICON*)GFL_HEAP_AllocMemoryLow(HeapId, sizeof(VINTR_WIRELESS_ICON));

	vwi->x     = x;
	vwi->y     = y;
	vwi->anime_seq = 0;
    vwi->anime     = 3;		// 0:3�{ 1:2�{ 2:1�{ 0:�ؒf (�̒ʐM�A�C�R���j
//	vwi->table     = tbl;
    vwi->bWifi     = bWifi;
    vwi->sub_wrote_flag = FALSE;
    vwi->main_sub_mode = MAIN_SUB_MODE_DEFAULT;
    vwi->prevOamPtr = (GXOamAttr *)HW_OAM;

	return vwi;
}


//------------------------------------------------------------------
/**
 * �ʐM�A�C�R��TCB���C���y�^�X�N�z
 *
 * @param   tcb		
 * @param   work	
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void WirelessIconAnimeFunc( void *work )
{
	VINTR_WIRELESS_ICON *vwi = (VINTR_WIRELESS_ICON *)work;

	int targetVram, anime_index;
	GXOamAttr* pOam;

	targetVram = get_target_vram( vwi );
	anime_index = calc_anime_index( targetVram, vwi->anime );
	pOam = ( targetVram == NNS_G2D_VRAM_TYPE_2DMAIN )? WM_ICON_MAIN_OAM_ADRS : WM_ICON_SUB_OAM_ADRS;

	G2_SetOBJAttr(
//    	(GXOamAttr *)(HW_OAM + sizeof(GXOamAttr)*127),		
		pOam,
    	vwi->x,
    	vwi->y,
    	0,
    	GX_OAM_MODE_NORMAL,
    	0,
    	GX_OAM_EFFECT_NONE,
    	GX_OAM_SHAPE_16x16,
	    GX_OAM_COLORMODE_16,
	    anime_index,
	    WM_ICON_PAL_POS,
	    0
	);

	if( pOam != vwi->prevOamPtr )
	{
		G2_SetOBJAttr(
			vwi->prevOamPtr,
	    	0,
	    	0,
	    	0,
	    	GX_OAM_MODE_NORMAL,
	    	0,
	    	GX_OAM_EFFECT_NODISPLAY,
	    	GX_OAM_SHAPE_16x16,
		    GX_OAM_COLORMODE_16,
		    0,
		    0,
		    0
		);
		vwi->prevOamPtr = pOam;
	}

}
//------------------------------------------------------------------
/**
 * ���݂̃^�[�Q�b�gVRAM���擾
 *
 * @param   vwi		
 *
 * @retval  int 	NNS_G2D_VRAM_TYPE_2DMAIN or NNS_G2D_VRAM_TYPE_2DSUB
 */
//------------------------------------------------------------------
static int inline get_target_vram( VINTR_WIRELESS_ICON* vwi )
{
	switch( vwi->main_sub_mode ){
	case MAIN_SUB_MODE_TOP:
		return (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB )? NNS_G2D_VRAM_TYPE_2DMAIN : NNS_G2D_VRAM_TYPE_2DSUB;

	case MAIN_SUB_MODE_BOTTOM:
		return (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB )? NNS_G2D_VRAM_TYPE_2DSUB : NNS_G2D_VRAM_TYPE_2DMAIN;

	default:
		return NNS_G2D_VRAM_TYPE_2DMAIN;
	}
}

//------------------------------------------------------------------
/**
 * �A�j���C���f�b�N�X�v�Z
 *
 * @param   targetVram		
 * @param   anime_ptn		
 *
 * @retval  int 			�A�j���C���f�b�N�X
 */
//------------------------------------------------------------------
static int inline calc_anime_index( int targetVram, int anime_ptn )
{
	int vramMode, bank;

	if( targetVram == NNS_G2D_VRAM_TYPE_2DMAIN )
	{
		vramMode = GX_GetOBJVRamModeChar();
		bank = GX_GetBankForOBJ();
	}
	else
	{
		vramMode = GXS_GetOBJVRamModeChar();
		bank = GX_GetBankForSubOBJ();
	}

	switch( vramMode ){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		if( (bank == GX_VRAM_OBJ_16_G) || (bank == GX_VRAM_OBJ_16_F) )
		{
			return 512-16+4*anime_ptn;
		}
		else
		{
			return 1024-16+4*anime_ptn;
		}
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		if( (bank == GX_VRAM_OBJ_80_EF) || (bank == GX_VRAM_OBJ_80_EG) )
		{
			return 640-4+1*anime_ptn;
		}
		else
		{
			return 1024-4+1*anime_ptn;
		}
	default:
		return 1024-8+2*anime_ptn;
	}
}
//------------------------------------------------------------------
/**
 * �オ�c��Ȃ��悤�ɁA�A�C�R������������Ԃ�OAM�𒼏������Ă���
 *
 * @param   oam		�������ݐ�A�h���X
 *
 */
//------------------------------------------------------------------
static inline void clear_oam( GXOamAttr* oam )
{
	G2_SetOBJAttr(
		oam,
		0,
		0,
		0,
		GX_OAM_MODE_NORMAL,
		0,
		GX_OAM_EFFECT_NODISPLAY,
		GX_OAM_SHAPE_16x16,
		GX_OAM_COLORMODE_16,
		0,
		0,
		0
	);
}



//==============================================================================
/**
 * �ʐM�A�C�R���A�j���`�F���W
 *
 * @param   vwi		
 * @param   anime		
 *
 * @retval  none		
 */
//==============================================================================
static void WirelessIconAnimeChange(VINTR_WIRELESS_ICON *vwi, int anime)
{
	// �A�j���`�F���W
	if(anime<WM_ICON_ANIME_MAX)
	{
		vwi->anime = anime;
	}
}

//==============================================================================
/**
 * Vblank�ʐM�A�C�R���\�������I��
 *
 * @param   vwi		
 *
 * @retval  none		
 */
//==============================================================================
static void WirelessIconEnd(VINTR_WIRELESS_ICON *vwi)
{
	clear_oam( WM_ICON_MAIN_OAM_ADRS );
	if( vwi->sub_wrote_flag )
	{
		clear_oam( WM_ICON_SUB_OAM_ADRS );
	}
	// ���[�N���
	GFL_HEAP_FreeMemory( vwi );
}


//==============================================================================
/**
 * �ʐM�A�C�R���̕\��LCD������ or ����ʂɌŒ肷��
 *
 * @param   vwi			�A�C�R�����[�N�|�C���^
 * @param   bTop		TRUE���Ə��ʂɌŒ�^FALSE���Ɖ���ʂɌŒ�
 * @param   heapID		�e���|�����g�p�q�[�vID
 *
 */
//==============================================================================
static void WirelessIconHoldLCD( VINTR_WIRELESS_ICON *vwi, BOOL bTop, HEAPID heapID )
{
	trans_palette_data( NNS_G2D_VRAM_TYPE_2DSUB, vwi->bWifi, WM_ICON_PAL_OFFSET, heapID );
	trans_cgx_data( NNS_G2D_VRAM_TYPE_2DSUB, vwi->bWifi, heapID );
	vwi->main_sub_mode = (bTop)? MAIN_SUB_MODE_TOP : MAIN_SUB_MODE_BOTTOM;
	vwi->sub_wrote_flag = TRUE;
}


//==============================================================================
/**
 * �ʐM�A�C�R���ʒu�̕\��LCD�Œ����߂ăf�t�H���g�̋����ɖ߂�
 *
 * @param   vwi			�A�C�R�����[�N�|�C���^
 *
 */
//==============================================================================
static void WirelessIconDefaultLCD( VINTR_WIRELESS_ICON* vwi )
{
	if( vwi->main_sub_mode != MAIN_SUB_MODE_DEFAULT )
	{
		vwi->main_sub_mode = MAIN_SUB_MODE_DEFAULT;
		vwi->sub_wrote_flag = FALSE;
		clear_oam( WM_ICON_SUB_OAM_ADRS );
	}
}






//======================================================================================================
//======================================================================================================

//------------------------------------------------------------------
/**
 * �p���b�g�f�[�^�]��
 *
 * @param   vramType	VRAM�^�C�v�iMAIN or SUB�j
 * @param   bWifi		TRUE�Ȃ�Wi-Fi�ʐM�A�C�R���^FALSE�Ȃ�DS�ʐM�A�C�R��
 * @param   offset		�]���ʒu�o�C�g�I�t�Z�b�g
 * @param   HeapId		�e���|�����q�[�vID
 */
//------------------------------------------------------------------
static void trans_palette_data( int vramType, BOOL bWifi, u32 offset, HEAPID heapID )
{
	void* pal = GFL_HEAP_AllocMemoryLow( heapID, WM_ICON_PALFILE_SIZE );
	if( pal )
	{
		NNSG2dPaletteData* palData;

		GF_ReadFile( "src/gfl_graphic/wm.nclr", pal );
		DC_FlushRange( pal, WM_ICON_PALFILE_SIZE );
		NNS_G2dGetUnpackedPaletteData( pal, &palData );    // �W�J

		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			GX_LoadOBJPltt( palData->pRawData, offset, WM_ICON_PAL_SIZE );
		}
		else
		{
			GXS_LoadOBJPltt( palData->pRawData, offset, WM_ICON_PAL_SIZE );
		}

		GFL_HEAP_FreeMemory( pal );
	}
}


//------------------------------------------------------------------
/**
 * �A�C�R��CGX�f�[�^�]��
 *
 * @param   vramType	VRAM�^�C�v�iMAIN or SUB�j
 * @param   bWifi		TRUE�Ȃ�Wi-Fi�ʐM�A�C�R���^FALSE�Ȃ�DS�ʐM�A�C�R��
 * @param   HeapId		�e���|�����q�[�vID
 */
//------------------------------------------------------------------
static void trans_cgx_data( int vramType, BOOL bWifi, HEAPID heapID )
{
	void *ncg = GFL_HEAP_AllocMemoryLow( heapID, WM_ICON_CHARFILE_SIZE );

	if( ncg )
	{
		NNSG2dCharacterData *ppCharData;
		int offset, vramMode, objBank;

		// DS�ʐM�A�C�R�����Awifi�A�C�R����ǂݍ���
	    if(bWifi)
	    {
            GF_ReadFile( "src/gfl_graphic/wifi.ncgr", ncg );
	    }
	    else
	    {
            GF_ReadFile( "src/gfl_graphic/wm.ncgr", ncg );
	    }
		DC_FlushRange( ncg, WM_ICON_CHARFILE_SIZE );
		NNS_G2dGetUnpackedCharacterData( ncg, &ppCharData );   // �W�J

		// VRAM�ݒ�ɍ��킹�ē]���ʒu������
		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			vramMode = GX_GetOBJVRamModeChar();
			objBank = GX_GetBankForOBJ();
		}
		else
		{
			vramMode = GX_GetOBJVRamModeChar();
			objBank = GX_GetBankForOBJ();
		}
		switch( vramMode ){
		case GX_OBJVRAMMODE_CHAR_1D_32K:
			if( objBank==GX_VRAM_OBJ_16_G || objBank==GX_VRAM_OBJ_16_F ) 
			{
				offset = WM_ICON_CHAR_OFFSET16;
			}
			else
			{
				offset = WM_ICON_CHAR_OFFSET32;
			}
			break;
		case GX_OBJVRAMMODE_CHAR_1D_128K:
			if( objBank==GX_VRAM_OBJ_80_EF || objBank==GX_VRAM_OBJ_80_EG ) 
			{
				offset = WM_ICON_CHAR_OFFSET80;
			}
			else
			{
				offset = WM_ICON_CHAR_OFFSET128;
			}
			break;
		default:
			offset = WM_ICON_CHAR_OFFSET64;
			break;
		}

		// OBJ�L������VRAM�֓]��
		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			GX_LoadOBJ( ppCharData->pRawData, offset, WM_ICON_CHAR_SIZE );
		}
		else
		{
			GXS_LoadOBJ( ppCharData->pRawData, offset, WM_ICON_CHAR_SIZE );
		}

        GFL_HEAP_FreeMemory( ncg );
	}
}



//�A�C�R���̈ʒu
static const VOamAnm WM_IconAnimTbl[]={
	{512-16   },
	{512-16+4 },
	{512-16+8 },
    {512-16+12},
};
static VINTR_WIRELESS_ICON *VintrWirelessIconPtr = NULL;


//==============================================================================
/**
 * $brief   ����炭����ʐM�A�C�R��
 *
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasy(BOOL bWifi, HEAPID heapID)
{
    WirelessIconEasyXY(240, 0, bWifi, heapID);
}

//==============================================================================
/**
 * $brief   ����炭����ʐM�A�C�R�� ���������
 *
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasyXY(int x,int y, BOOL bWifi,HEAPID heapID)
{
    if(!GFL_NET_WLIsVRAMDStart()){  // �C�N�j���[�����������ꍇ�ʐM���ĂȂ�
        return ;
    }
    if(VintrWirelessIconPtr){
        WirelessIconEasyEnd();
    }
    VintrWirelessIconPtr = AddWirelessIconOAM(0,heapID, x, y, bWifi, WM_IconAnimTbl);
}


//==============================================================================
/**
 * $brief   ����炭����ʐM�A�C�R���폜
 *
 * @param   none		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasyEnd(void)
{
    if(VintrWirelessIconPtr){
        WirelessIconEnd(VintrWirelessIconPtr);
        VintrWirelessIconPtr = NULL;
    }
}

//==============================================================================
/**
 * $brief   �ʐM���x�����Z�b�g����
 * @param   level
 * @retval  none
 */
//==============================================================================
void WirelessIconEasy_SetLevel(int level)
{
    if(VintrWirelessIconPtr){
        WirelessIconAnimeChange(VintrWirelessIconPtr, level);
    }
}

//------------------------------------------------------------------
/**
 * ����炭����ʐM�A�C�R���F�\��LCD������or����ʂɌŒ肷��
 *
 * @param   bTop		TRUE���Ə��ʁ^FALSE���Ɖ����
 * @param   heapID		�e���|�����g�p�q�[�vID
 *
 */
//------------------------------------------------------------------
void WirelessIconEasy_HoldLCD( BOOL bTop, HEAPID heapID )
{
    if(VintrWirelessIconPtr)
    {
		WirelessIconHoldLCD(VintrWirelessIconPtr, bTop, heapID );
	}
}

//------------------------------------------------------------------
/**
 * ����炭����ʐM�A�C�R���F�\��LCD�̌Œ���~�߁A�f�t�H���g�̋����ɖ߂�
 */
//------------------------------------------------------------------
void WirelessIconEasy_DefaultLCD( void )
{
    if(VintrWirelessIconPtr){
		WirelessIconDefaultLCD( VintrWirelessIconPtr );
	}
}


//==============================================================================
/**
 * $brief   �ʐM�A�C�R�����s�֐�
 * @retval  none
 */
//==============================================================================
void WirelessIconEasyFunc(void)
{
    if(VintrWirelessIconPtr){
        WirelessIconAnimeFunc( VintrWirelessIconPtr );
    }
}

