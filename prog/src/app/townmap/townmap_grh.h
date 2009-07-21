//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_grh.h
 *	@brief	�^�E���}�b�v�p��{�O���t�B�b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.07.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	// ���C�����BG(�����)
	TOWNMAP_BG_PAL_M_00 = 0,//	256plt��������
	TOWNMAP_BG_PAL_M_01,		//   
	TOWNMAP_BG_PAL_M_02,		//  
	TOWNMAP_BG_PAL_M_03,		//   
	TOWNMAP_BG_PAL_M_04,		//   
	TOWNMAP_BG_PAL_M_05,		//   
	TOWNMAP_BG_PAL_M_06,		//   
	TOWNMAP_BG_PAL_M_07,		//   
	TOWNMAP_BG_PAL_M_08,		//  
	TOWNMAP_BG_PAL_M_09,		//  
	TOWNMAP_BG_PAL_M_10,		//  
	TOWNMAP_BG_PAL_M_11,		//  
	TOWNMAP_BG_PAL_M_12,		//  
	TOWNMAP_BG_PAL_M_13,		//  256plt�����܂�
	TOWNMAP_BG_PAL_M_14,		// �t�H���g
	TOWNMAP_BG_PAL_M_15,		//


	// �T�u���BG�i���ʁj
	TOWNMAP_BG_PAL_S_00 = 0,	//  �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_01,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_02,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_03,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_04,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_05,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_06,		//   �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_07,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_08,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_09,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_10,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_11,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_12,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_13,		// �g�p���ĂȂ�
	TOWNMAP_BG_PAL_S_14,		// �t�H���g
	TOWNMAP_BG_PAL_S_15,		//	�g�p���ĂȂ�
};

//-------------------------------------
///	BG�t���[��
//	�\�[�X��sc_bgsetup�ƑΉ�
//=====================================
typedef enum
{
	TOWNMAP_BG_FRAME_BAR_M,		//INFOWIN�≺�̃o�[
	TOWNMAP_BG_FRAME_FONT_M,	//������
	TOWNMAP_BG_FRAME_ROAD_M,	//��
	TOWNMAP_BG_FRAME_MAP_M,		//�n�}

	TOWNMAP_BG_FRAME_FONT_S,	//����
	TOWNMAP_BG_FRAME_WND_S,		//�����̃E�B���h�E

	TOWNMAP_BG_FRAME_MAX,			//�\�[�X���Ŏg�p
}TOWNMAP_GRAPHIC_BG_FRAME;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�^�E���}�b�v�p��{�O���t�B�b�N���[�N
//=====================================
typedef struct _TOWNMAP_GRAPHIC_SYS TOWNMAP_GRAPHIC_SYS;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern TOWNMAP_GRAPHIC_SYS * TOWNMAP_GRAPHIC_Init( HEAPID heapID );
extern void TOWNMAP_GRAPHIC_Exit( TOWNMAP_GRAPHIC_SYS *p_wk );
extern void TOWNMAP_GRAPHIC_Draw( TOWNMAP_GRAPHIC_SYS *p_wk );

extern u8 TOWNMAP_GRAPHIC_GetFrame( const TOWNMAP_GRAPHIC_SYS *cp_wk, TOWNMAP_GRAPHIC_BG_FRAME frame );
