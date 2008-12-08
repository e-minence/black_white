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
#include "device/net_whpipe.h"

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


//�A�C�R���̈ʒu
static VINTR_WIRELESS_ICON *VintrWirelessIconPtr = NULL;


//==============================================================
// Prototype
//==============================================================
static void WirelessIconAnimeFunc( void *work );
static void transIconData(int vramType,BOOL bWifi, int heapID);
static int get_target_vram( VINTR_WIRELESS_ICON* vwi );
static int calc_anime_index( int targetVram, int anime_ptn );


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
 *
 * @retval  VINTR_WIRELESS_ICON *		
 */
//==============================================================================
static VINTR_WIRELESS_ICON *AddWirelessIconOAM(u32 objVRAM, u32 HeapId, int x, int y, BOOL bWifi,VINTR_WIRELESS_ICON* pVwi)
{
	VINTR_WIRELESS_ICON *vwi;

//    GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    transIconData(NNS_G2D_VRAM_TYPE_2DMAIN, bWifi, HeapId);
    
    vwi = pVwi;
    if(vwi==NULL){
        // �^�X�N�o�^�E���[�N������
        vwi        = (VINTR_WIRELESS_ICON*)GFL_HEAP_AllocMemoryLo(HeapId, sizeof(VINTR_WIRELESS_ICON));
    }

	vwi->x     = x;
	vwi->y     = y;
	vwi->anime_seq = 0;
    vwi->anime     = 3;		// 0:3�{ 1:2�{ 2:1�{ 0:�ؒf (�̒ʐM�A�C�R���j
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
static int get_target_vram( VINTR_WIRELESS_ICON* vwi )
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
static int calc_anime_index( int targetVram, int anime_ptn )
{
	int vramMode, bank, mapping;

	if( targetVram == NNS_G2D_VRAM_TYPE_2DMAIN )
	{
		bank = GX_GetBankForOBJ();
		mapping = GX_GetOBJVRamModeChar();
	}
	else
	{
		bank = GX_GetBankForSubOBJ();
		mapping = GXS_GetOBJVRamModeChar();
	}

#if 1
    //return 4*anime_ptn;
    switch(mapping){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return 4*anime_ptn;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return 2*anime_ptn;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return anime_ptn;
	case GX_OBJVRAMMODE_CHAR_1D_256K:
	default:
		GF_ASSERT(0 && "��Ή���OBJ�}�b�s���O���[�h");
		return anime_ptn;
	}
#else
    switch( bank ){
      case GX_VRAM_OBJ_16_F:  	//OBJ��16KBytes�m�ۂ��܂��BVRAM-F�����蓖�Ă܂��B
      case GX_VRAM_OBJ_16_G: 	//OBJ��16KBytes�m�ۂ��܂��BVRAM-G�����蓖�Ă܂��B
        return 512-16+4*anime_ptn;
      case GX_VRAM_OBJ_32_FG: 	//OBJ��32KBytes�m�ۂ��܂��BVRAM-F,G�����蓖�Ă܂��B
        return 1024-16+4*anime_ptn;
      case GX_VRAM_OBJ_64_E: 	//OBJ��64KBytes�m�ۂ��܂��BVRAM-E�����蓖�Ă܂��B
		return 1024-16+4*anime_ptn;
      case GX_VRAM_OBJ_80_EF: 	//OBJ��80KBytes�m�ۂ��܂��BVRAM-E,F�����蓖�Ă܂��B
      case GX_VRAM_OBJ_80_EG: 	//OBJ��80KBytes�m�ۂ��܂��BVRAM-E,G�����蓖�Ă܂��B
        return 640-4+1*anime_ptn;
      case GX_VRAM_OBJ_96_EFG: 	//OBJ��96KBytes�m�ۂ��܂��BVRAM-E,F,G�����蓖�Ă܂��B
      case GX_VRAM_OBJ_128_A: 	//OBJ��128KBytes�m�ۂ��܂��BVRAM-A�����蓖�Ă܂��B
      case GX_VRAM_OBJ_128_B: 	//OBJ��128KBytes�m�ۂ��܂��BVRAM-B�����蓖�Ă܂��B
        return 1024-4+1*anime_ptn;
      case GX_VRAM_OBJ_256_AB: 	//OBJ��256KBytes�m�ۂ��܂��BVRAM-A,B�����蓖�Ă܂��B
      default:
		return 1024-8+2*anime_ptn;
    }
#endif
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
    transIconData(NNS_G2D_VRAM_TYPE_2DSUB, vwi->bWifi, heapID);

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

//==============================================================================
/**
 * @brief   vramType�ɂ����� �L�����N�^�[�I�t�Z�b�g��Ԃ�
 * @retval   none
 */
//==============================================================================

static int _getCharOffset(int vramType)
{
    int vramMode,objBank,offset;
    
    // VRAM�ݒ�ɍ��킹�ē]���ʒu������
    if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN ){
        objBank = GX_GetBankForOBJ();
    }
    else{
        objBank = GX_GetBankForSubOBJ();
    }
#if 1
    return 0;
#else
	switch( objBank ){
      case GX_VRAM_OBJ_16_F:  	//OBJ��16KBytes�m�ۂ��܂��BVRAM-F�����蓖�Ă܂��B
      case GX_VRAM_OBJ_16_G: 	//OBJ��16KBytes�m�ۂ��܂��BVRAM-G�����蓖�Ă܂��B
        offset = WM_ICON_CHAR_OFFSET16;
        break;
      case GX_VRAM_OBJ_32_FG: 	//OBJ��32KBytes�m�ۂ��܂��BVRAM-F,G�����蓖�Ă܂��B
        offset = WM_ICON_CHAR_OFFSET32;
        break;
      case GX_VRAM_OBJ_64_E: 	//OBJ��64KBytes�m�ۂ��܂��BVRAM-E�����蓖�Ă܂��B
        offset = WM_ICON_CHAR_OFFSET64;
        break;
      case GX_VRAM_OBJ_80_EF: 	//OBJ��80KBytes�m�ۂ��܂��BVRAM-E,F�����蓖�Ă܂��B
      case GX_VRAM_OBJ_80_EG: 	//OBJ��80KBytes�m�ۂ��܂��BVRAM-E,G�����蓖�Ă܂��B
        offset = WM_ICON_CHAR_OFFSET80;
        break;
      case GX_VRAM_OBJ_96_EFG: 	//OBJ��96KBytes�m�ۂ��܂��BVRAM-E,F,G�����蓖�Ă܂��B
      case GX_VRAM_OBJ_128_A: 	//OBJ��128KBytes�m�ۂ��܂��BVRAM-A�����蓖�Ă܂��B
      case GX_VRAM_OBJ_128_B: 	//OBJ��128KBytes�m�ۂ��܂��BVRAM-B�����蓖�Ă܂��B
        offset = WM_ICON_CHAR_OFFSET128;
        break;
      case GX_VRAM_OBJ_256_AB: 	//OBJ��256KBytes�m�ۂ��܂��BVRAM-A,B�����蓖�Ă܂��B
      default:
        offset = WM_ICON_CHAR_OFFSET64;
    }
    return offset;
#endif
}

//==============================================================================
/**
 * @brief    �p���b�g�ƃL������VRAM�֓]������
 * @retval   none
 */
//==============================================================================

static void transIconData(int vramType,BOOL bWifi, int heapID)
{
    int aNoBuff[3];
    int palType,charNo,charType,vramMode;

    NET_ICONDATA_GetNoBuff(aNoBuff);
    if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN ){
        palType = PALTYPE_MAIN_OBJ;
        charType = OBJTYPE_MAIN;
    }
    else{
        palType = PALTYPE_SUB_OBJ;
        charType = OBJTYPE_SUB;
    }
    OS_TPrintf("ICONPAL %d %d %d \n",NET_ICONDATA_GetTableID(), aNoBuff[GFL_NET_ICON_WMNCLR],palType);
    GFL_ARC_UTIL_TransVramPalette(NET_ICONDATA_GetTableID(), aNoBuff[GFL_NET_ICON_WMNCLR],
                                  palType, WM_ICON_PAL_OFFSET, WM_ICON_PAL_SIZE, heapID);

    if(bWifi){
        charNo = GFL_NET_ICON_WIFINCGR;
    }
    else{
        charNo = GFL_NET_ICON_WMNCGR;
    }
    GFL_ARC_UTIL_TransVramObjCharacter(NET_ICONDATA_GetTableID(),aNoBuff[charNo],charType,
                                       _getCharOffset(vramType),
                                       0,FALSE,heapID);
}

//==============================================================================
/**
 * $brief   ����炭����ʐM�A�C�R��
 *
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void GFL_NET_WirelessIconEasy(BOOL bWifi, HEAPID heapID)
{
    GFL_NET_WirelessIconEasyXY(GFL_WICON_POSX,GFL_WICON_POSY, bWifi, heapID);
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
void GFL_NET_WirelessIconEasyXY(int x,int y, BOOL bWifi,HEAPID heapID)
{
    if(!GFL_NET_IsIchneumon()){  // �C�N�j���[�����������ꍇ�ʐM���ĂȂ�
        return ;
    }
    GFL_NET_WirelessIconEasyEnd();
    VintrWirelessIconPtr = AddWirelessIconOAM(0,heapID, x, y, bWifi, VintrWirelessIconPtr);
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
void GFL_NET_WirelessIconEasyEnd(void)
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
void GFL_NET_WirelessIconEasy_SetLevel(int level)
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
void GFL_NET_WirelessIconEasy_HoldLCD( BOOL bTop, HEAPID heapID )
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
void GFL_NET_WirelessIconEasy_DefaultLCD( void )
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
void GFL_NET_WirelessIconEasyFunc(void)
{
 
    if(VintrWirelessIconPtr){
        WirelessIconAnimeFunc( VintrWirelessIconPtr );
    }
}

