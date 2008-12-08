//=============================================================================
/**
 * @file	comm_sharing_system.h
 * @brief	�f�[�^���L���s���ꍇ�̒ʐM�V�X�e��
 * @author	Katsumi Ohno
 * @date    2005.07.08
 */
//=============================================================================

#ifndef _WM_ICON_H_
#define _WM_ICON_H_


// �ʐM�A�C�R���p���b�g�f�[�^���]�������ʒu
#define WM_ICON_PALETTE_NO	( 14 )

// �ʐM�A�C�R���L�����f�[�^��]������VRAM�̈ʒu
// �ʐM�A�C�R���L�����f�[�^��Ҳ�OBJVRAM�̈�ԍŌ��
// �P�U�L�����]������܂��B

//OBJ16K���[�h�̎�
#define WM_ICON_CHAR_OFFSET16  ( (512-16)*32 )	
//OBJ32K���[�h�̎�
#define WM_ICON_CHAR_OFFSET32  ( (1024-16)*32 )	
//OBJ64K���[�h�̎�
#define WM_ICON_CHAR_OFFSET64 ( (1024-16)*32 )	
//OBJ80K���[�h�̎�
#define WM_ICON_CHAR_OFFSET80 ( (2560-16)*32 )	
//OBJ128K���[�h�̎�
#define WM_ICON_CHAR_OFFSET128 ( (4096-16)*32 )	

// -----------------------------------------------
// �Ǘ��p�\���̒�`
// -----------------------------------------------

typedef struct{
	u16 pat;
}VOamAnm;


extern void GFL_NET_WirelessIconEasy(BOOL bWifi, HEAPID heapID);
extern void GFL_NET_WirelessIconEasyXY(int x, int y, BOOL bWifi, HEAPID heapID);
extern void GFL_NET_WirelessIconEasyEnd(void);
extern void GFL_NET_WirelessIconEasy_SetLevel(int level);
extern void GFL_NET_WirelessIconEasy_HoldLCD( BOOL bTop, HEAPID heapID );
extern void GFL_NET_WirelessIconEasy_DefaultLCD( void );
extern void GFL_NET_WirelessIconEasyFunc(void);

// �ʐM�A�C�R���������߂ɓ]������PAL_VRAM�̈ʒu�E�傫��(14�ԃp���b�g�g�p�j
#define WM_ICON_PAL_POS		( 14 )
#define WM_ICON_PAL_OFFSET	( 16 * 2 * WM_ICON_PAL_POS )
#define WM_ICON_PAL_SIZE	( 16 * 2 )


#endif //_WM_ICON_H_
