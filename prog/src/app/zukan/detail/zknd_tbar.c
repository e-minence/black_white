//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		zknd_tbar.c
 *	@brief	�A�v���p����ʃ^�b�`�o�[
 *	@author	Koji Kawada
 *	@data		2010.02.04
 *
 *  @note   touchbar.c���R�s�y���āA
 *          (1) TOUCHBAR��ZKND_TBAR�ɒu�����A
 *          (2) Touchbar��Zknd_Tbar�ɒu�����A
 *          (3) ui/touchbar��zknd_tbar�ɒu�����A
 *          �K�v�ȋ@�\��ǉ����������ł��B
 *          Revision 14819 Date 2009.12.16�����B
 *          �ύX�ӏ��ɂ�ZKND_Modified�Ƃ������t���Ă����܂����B
 *
 *  ���W���[�����FZKND_TBAR
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//sound
#include "sound/pm_sndsys.h"

//mine
#include "zknd_tbar.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���\�[�X
//=====================================
//���\�[�X
typedef enum
{	
	ZKND_TBAR_RES_COMMON_PLT,
	ZKND_TBAR_RES_COMMON_CHR,
	ZKND_TBAR_RES_COMMON_CEL,

	ZKND_TBAR_RES_MAX
}RES_TYPE;

//-------------------------------------
///	�V�[�P���X
//=====================================
enum
{
	ZKND_TBAR_SEQ_MAIN,	//�^�b�`�҂�
	ZKND_TBAR_SEQ_ANM_TRG,		//�{�^���A�j��
	ZKND_TBAR_SEQ_ANM,		//�{�^���A�j��
	ZKND_TBAR_SEQ_TRG,		//�g���K
} ;

//-------------------------------------
///	����֐��^�C�v
//=====================================
typedef enum
{
	ZKND_TBAR_ICON_MOVETYPE_PUSH,	//�����ꂽ��A�������A�j���ɂȂ�{�^��
	ZKND_TBAR_ICON_MOVETYPE_FLIP,	//�����ꂽ��AON��OFF���؂�ւ�{�^��
} ZKND_TBAR_ICON_MOVETYPE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�A�C�R�����[�N
//=====================================
typedef struct _ICON_WORK ICON_WORK;
typedef void (*PUSH_FUNC)( ICON_WORK *p_wk );
struct _ICON_WORK
{
	GFL_CLWK						*p_clwk;		
	BOOL								is_active;		//�A�N�e�B�u��
	u32									now_anmseq;		//���݂̃A�j��
	PUSH_FUNC						push_func;		//�����ꂽ���̓���
	ZKND_TBAR_ICON_MOVETYPE	movetype;	//����^�C�v
	ZKND_TBAR_ITEM_ICON			data;			//�{�^�����
} ;
//-------------------------------------
///	���ʃ��\�[�X
//=====================================
typedef struct 
{
	u32				res[ZKND_TBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
} RES_WORK;

//-------------------------------------
///	����ʃo�[���C�����[�N
//=====================================
struct _ZKND_TBAR_WORK
{
	s32				trg;			//�������A�C�R���̃C���f�b�N�X
	u32				tbl_len;	//�ő�A�C�R���̃e�[�u����
	u32				seq;			//�V�[�P���X
  
  BOOL      whole_is_active;  // �S�̐�p�̃A�N�e�B�u�t���O  // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B
  BOOL      whole_is_lock;    // �S�̐�p�̃��b�N            // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B

	RES_WORK	res;			//���ʃ��\�[�X
	ICON_WORK	icon[0];	//�A�C�R��
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�A�v���P�[�V����
//=====================================
static ICON_WORK * Zknd_Tbar_Search( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON type );
static const ICON_WORK * Zknd_Tbar_SearchConst( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON type );

//-------------------------------------
///	���\�[�X
//=====================================
static void RES_Init( RES_WORK *p_wk, u8 frm, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, BOOL is_notload_bg, HEAPID heapID );
static void RES_Exit( RES_WORK *p_wk );
static u32 RES_GetResource( const RES_WORK *cp_wk, RES_TYPE type );
static void RES_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );
//-------------------------------------
///	ICON
//=====================================
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const ZKND_TBAR_ITEM_ICON *cp_setup,CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID );
static void ICON_Exit( ICON_WORK *p_wk );
static BOOL ICON_Main( ICON_WORK *p_wk );
static BOOL ICON_IsMoveEnd( const ICON_WORK *cp_wk );
static ZKND_TBAR_ICON ICON_GetType( const ICON_WORK *cp_wk );
static void ICON_SetVisible( ICON_WORK *p_wk, BOOL is_visible );
static BOOL ICON_GetVisible( const ICON_WORK *cp_wk );
static void ICON_SetActive( ICON_WORK *p_wk, BOOL is_active );
static BOOL ICON_GetActive( const ICON_WORK *cp_wk );
static void ICON_SetSE( ICON_WORK *p_wk, u32 se );
static u32 ICON_GetSE( const ICON_WORK *cp_wk );
static void ICON_SetKey( ICON_WORK *p_wk, u32 key );
static u32 ICON_GetKey( const ICON_WORK *cp_wk );
static void ICON_SetFlip( ICON_WORK *p_wk, BOOL is_flip );
static BOOL ICON_GetFlip( const ICON_WORK *cp_wk );
static void Icon_PushFuncNormal( ICON_WORK *p_wk );
static void Icon_PushFuncFlip( ICON_WORK *p_wk );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�A�C�R�����
//	ZKND_TBAR_ICON�ƑΉ������A�N�e�B�u�e�[�u��
//=====================================
static const struct
{	
	u16 active_anmseq;
	u16 noactive_anmseq;
	u16 push_anmseq;
	u16 se;
	u32	key;
	ZKND_TBAR_ICON_MOVETYPE	movetype;
} sc_common_icon_data[ZKND_TBAR_ICON_MAX]	=
{	
	//ZKND_TBAR_ICON_CLOSE,	//�~�{�^��
	{	
		APP_COMMON_BARICON_EXIT,
		APP_COMMON_BARICON_EXIT_OFF,
		APP_COMMON_BARICON_EXIT_ON,
		ZKND_TBAR_SE_CLOSE,
		PAD_BUTTON_X,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_RETURN,	//�����{�^��
	{	
		APP_COMMON_BARICON_RETURN,
		APP_COMMON_BARICON_RETURN_OFF,
		APP_COMMON_BARICON_RETURN_ON,
		ZKND_TBAR_SE_CANCEL,
		PAD_BUTTON_B,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_CUR_D,	//���{�^��
	{	
		APP_COMMON_BARICON_CURSOR_DOWN,
		APP_COMMON_BARICON_CURSOR_DOWN_OFF,
		APP_COMMON_BARICON_CURSOR_DOWN_ON,
		ZKND_TBAR_SE_DECIDE,
		PAD_KEY_DOWN,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_CUR_U,	//���{�^��
	{	
		APP_COMMON_BARICON_CURSOR_UP,
		APP_COMMON_BARICON_CURSOR_UP_OFF,
		APP_COMMON_BARICON_CURSOR_UP_ON,
		ZKND_TBAR_SE_DECIDE,
		PAD_KEY_UP,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_CUR_L,	//���{�^��
	{	
		APP_COMMON_BARICON_CURSOR_LEFT,
		APP_COMMON_BARICON_CURSOR_LEFT_OFF,
		APP_COMMON_BARICON_CURSOR_LEFT_ON,
		ZKND_TBAR_SE_DECIDE,
		PAD_KEY_LEFT,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_CUR_R,	//���{�^
	{	
		APP_COMMON_BARICON_CURSOR_RIGHT,
		APP_COMMON_BARICON_CURSOR_RIGHT_OFF,
		APP_COMMON_BARICON_CURSOR_RIGHT_ON,
		ZKND_TBAR_SE_DECIDE,
		PAD_KEY_RIGHT,
		ZKND_TBAR_ICON_MOVETYPE_PUSH,
	},
	//ZKND_TBAR_ICON_CHECK,	//���������{�^��
	{	
		APP_COMMON_BARICON_CHECK_OFF,
		APP_COMMON_BARICON_CHECK_NONE,
		APP_COMMON_BARICON_CHECK_ON,
		ZKND_TBAR_SE_Y_REG,
		PAD_BUTTON_Y,
		ZKND_TBAR_ICON_MOVETYPE_FLIP,
	},
};


//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�������g���Łi�A�C�R���ʒu�w��Łj
 *
 *	@param	const ZKND_TBAR_SETUP *cp_setup	�ݒ���
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
extern ZKND_TBAR_WORK * ZKND_TBAR_Init( ZKND_TBAR_SETUP *p_setup, HEAPID heapID )
{	
	ZKND_TBAR_WORK	*p_wk;


	//���[�N�쐬�A������
	{	
		u32 size;

		size	= sizeof(ZKND_TBAR_WORK) + sizeof(ICON_WORK) * p_setup->item_num;
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );

		p_wk->tbl_len	= p_setup->item_num;
		p_wk->seq			= ZKND_TBAR_SEQ_MAIN;
    
    p_wk->whole_is_active = TRUE;   // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B
    p_wk->whole_is_lock   = FALSE;  // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B
	}

	//���\�[�X�ǂ݂���
	{	
		CLSYS_DRAW_TYPE			clsys_draw_type;
		CLSYS_DEFREND_TYPE	clsys_def_type;
		PALTYPE							bgplttype;

		//OBJ�ǂݍ��ޏꏊ���`�F�b�N
		if( p_setup->bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}

		//���\�[�X�ǂݍ���
		RES_Init( &p_wk->res, p_setup->bar_frm, clsys_draw_type, bgplttype, 
				p_setup->mapping, p_setup->bg_plt, p_setup->obj_plt, p_setup->is_notload_bg, heapID );
	
		//ICON�ݒ�
		{	
			int i;
			for( i = 0; i < p_wk->tbl_len; i++ )
			{	
				ICON_Init( &p_wk->icon[i], p_setup->p_unit, &p_wk->res, &p_setup->p_item[i], clsys_draw_type, heapID );
			}
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�j��
 *
 *	@param	ZKND_TBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_Exit( ZKND_TBAR_WORK *p_wk )
{	
	//ICON�j��
	{	
		int i;
		for( i = 0; i < p_wk->tbl_len; i++ )
		{	
			ICON_Exit( &p_wk->icon[i] );
		}
	}

	//���\�[�X�j��
	RES_Exit( &p_wk->res );
	
	//�j��
	GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	���C������
 *
 *	@param	ZKND_TBAR_WORK *p_wk			���[�N
 *	@param	CURSOR_WORK	cp_cursor	�J�[�\���Ń{�^���������̂����������邽��
 *
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_Main( ZKND_TBAR_WORK *p_wk )
{	
	switch( p_wk->seq )
	{	
	case ZKND_TBAR_SEQ_MAIN:
		//�^�b�`���o
		{	
			int i;
			p_wk->trg		= ZKND_TBAR_SELECT_NONE;

      if( (p_wk->whole_is_active) && (!p_wk->whole_is_lock) )  // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B 
      {
			  for( i = 0; i < p_wk->tbl_len; i++ )
			  {
				  if( ICON_Main( &p_wk->icon[ i ] ) )
				  {	
					  p_wk->trg	= i;
					  p_wk->seq	= ZKND_TBAR_SEQ_ANM_TRG;
            p_wk->whole_is_lock = TRUE;  // ZKND_Modified �S�̐�p�̐ݒ��ǉ��B
					  break;
				  }
			  }
      }
		}
		break;

	case ZKND_TBAR_SEQ_ANM_TRG:	//�A�j���O�̂PF�𓾂邽��
			p_wk->seq	 = ZKND_TBAR_SEQ_ANM;
		/* fallthrough */
	
	case ZKND_TBAR_SEQ_ANM:
		//����
		if( ICON_IsMoveEnd( &p_wk->icon[ p_wk->trg ] ) )
		{
			p_wk->seq	 = ZKND_TBAR_SEQ_TRG;
		}
		break;

	case ZKND_TBAR_SEQ_TRG:
		//�g���K��1�t���[�������`����
		p_wk->seq	 = ZKND_TBAR_SEQ_MAIN;
		break;
	}
	
	}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�I�����ꂽ���̂��擾	�A�j����̂PF
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk ���[�N
 *
 *	@return	ZKND_TBAR_SELECT��
 */
//-----------------------------------------------------------------------------
ZKND_TBAR_ICON ZKND_TBAR_GetTrg( const ZKND_TBAR_WORK *cp_wk )
{	

	if( cp_wk->seq	== ZKND_TBAR_SEQ_TRG && cp_wk->trg != ZKND_TBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return ZKND_TBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�I�����ꂽ���̂��擾	�A�j���O�̂PF
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk ���[�N
 *
 *	@return	ZKND_TBAR_SELECT��
 */
//-----------------------------------------------------------------------------
ZKND_TBAR_ICON ZKND_TBAR_GetTouch( const ZKND_TBAR_WORK *cp_wk )
{	
	if( cp_wk->seq	== ZKND_TBAR_SEQ_ANM_TRG && cp_wk->trg != ZKND_TBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return ZKND_TBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�S�ẴA�C�R����ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	is_active						�A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetActiveAll( ZKND_TBAR_WORK *p_wk, BOOL is_active )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetActive( &p_wk->icon[ i ], is_active );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�S�ẴA�C�R����\���ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	is_active						�\��
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetVisibleAll( ZKND_TBAR_WORK *p_wk, BOOL is_visible )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetVisible( &p_wk->icon[ i ], is_visible );
	}
}

// ZKND_Modified �S�̐�p�̐ݒ��ǉ��B��
//-------------------------------------
///	�S�̂̃A�N�e�B�u��؂�ւ���
// �����ڂ��A�N�e�B�u��Ԃɂ����܂܁A�S�̐�p�̃A�N�e�B�u�t���O�̐؂�ւ����s��
//=====================================
void ZKND_TBAR_SetActiveWhole( ZKND_TBAR_WORK *p_wk, BOOL is_active )
{
  p_wk->whole_is_active = is_active;
}

//-------------------------------------
///	�S�̂̃��b�N����������
// �����ڂ��A�N�e�B�u��Ԃɂ����܂܂������Ă���S�̐�p�̃��b�N����������
// �Q�x�������Ȃ��悤�ɁA�P�x�������牟���Ȃ��悤�Ƀ��b�N�������邱�Ƃɂ���
//=====================================
void ZKND_TBAR_UnlockWhole( ZKND_TBAR_WORK *p_wk )
{
  p_wk->whole_is_lock = FALSE;
}
// ZKND_Modified �S�̐�p�̐ݒ��ǉ��B��


//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R���̃A�N�e�B�u�ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon								�ݒ肷��A�C�R��
 *	@param	is_active						TRUE�ŃA�N�e�B�u	FALSE�Ńm���A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetActive( ZKND_TBAR_WORK *p_wk,  ZKND_TBAR_ICON icon, BOOL is_active )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Zknd_Tbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetActive( p_icon, is_active );
}
//----------------------------------------------------------------------------
/**	
 *	@brief	ZKND_TBAR	�A�C�R���̃A�N�e�B�u���擾
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk	���[�N
 *	@param	icon												�擾����A�C�R��
 *
 *	@return	TRUE�ŃA�N�e�B�u	FALSE�Ńm���A�N�e�B�u
 */
//-----------------------------------------------------------------------------
BOOL ZKND_TBAR_GetActive( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Zknd_Tbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetActive( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R���̕\���ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon								�ݒ肷��A�C�R��
 *	@param	is_visible					TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetVisible( ZKND_TBAR_WORK *p_wk,  ZKND_TBAR_ICON icon, BOOL is_visible )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Zknd_Tbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetVisible( p_icon, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R���̕\���擾
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk	���[�N
 *	@param	icon												�擾����A�C�R��
 *
 *	@return	TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
BOOL ZKND_TBAR_GetVisible( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Zknd_Tbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetVisible( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R����SE��ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon								�A�C�R��
 *	@param	se									SE
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetSE( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, u32 se )
{	

	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Zknd_Tbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetSE( p_icon, se );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R����SE���擾
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk	���[�N
 *	@param	icon	�A�C�R��
 *
 *	@return	SE
 */
//-----------------------------------------------------------------------------
u32 ZKND_TBAR_GetSE( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Zknd_Tbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetSE( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R���ɃL�[���͂�ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon								�A�C�R��
 *	@param	key									�L�[����	PAD_KEY_UP��PAD_BUTTON_A��
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetUseKey( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, u32 key )
{	

	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Zknd_Tbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetKey( p_icon, key );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�A�C�R������L�[���͂��擾
 *
 *	@param	const ZKND_TBAR_WORK *cp_wk	���[�N
 *	@param	icon												�A�C�R��
 *
 *	@return	�L�[����
 */
//-----------------------------------------------------------------------------
u32 ZKND_TBAR_GetUseKey( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Zknd_Tbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetKey( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�t���b�v���{�^���ւ̐ݒ�
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon			�A�C�R��
 *	@param	is_flip		TRUE�Ńt���b�vON�i�`�F�b�N����Ă���jFALSE��OFF
 */
//-----------------------------------------------------------------------------
void ZKND_TBAR_SetFlip( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, BOOL is_flip )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Zknd_Tbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetFlip( p_icon, is_flip );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ZKND_TBAR	�t���b�v���{�^���̎擾
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	icon			�A�C�R��
 *	@retval	is_flip		TRUE�Ńt���b�vON�i�`�F�b�N����Ă���jFALSE��OFF
 */
//-----------------------------------------------------------------------------
BOOL ZKND_TBAR_GetFlip( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Zknd_Tbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetFlip( cp_icon );
}

// ZKND_Modified �A�C�R���ʑ����ǉ��B��
//-------------------------------------
///	�A�C�R����GFL_CLWK���擾����
//=====================================
GFL_CLWK* ZKND_TBAR_GetClwk( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon )
{
	ICON_WORK* p_icon	= Zknd_Tbar_Search( p_wk, icon );
  return  p_icon->p_clwk;
}
// ZKND_Modified �A�C�R���ʑ����ǉ��B��


//=============================================================================
/**
 *	ZKND_TBAR private
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�R���̎�ނ���I�u�W�F�N�g��Search
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	type								�T���A�C�R���̃^�C�v
 *
 *	@return	��������ICON
 */
//-----------------------------------------------------------------------------
static ICON_WORK * Zknd_Tbar_Search( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON type )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		if( ICON_GetType( &p_wk->icon[ i ] ) == type )
		{	
			return &p_wk->icon[ i ];
		}
	}

	GF_ASSERT_MSG( 0,"ZKND_TBAR:�w�肵���A�C�R����������܂���ł���%d", type );
	return NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�R���̎�ނ���I�u�W�F�N�g��Search	CONST��
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	type								�T���A�C�R���̃^�C�v
 *
 *	@return	��������ICON
 */
//-----------------------------------------------------------------------------
static const ICON_WORK * Zknd_Tbar_SearchConst( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON type )
{	
	return Zknd_Tbar_Search( (ZKND_TBAR_WORK*)cp_wk, type );
}
//=============================================================================
/**
 *			RES
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�ǂ݂���
 *
 *	@param	ZKND_TBAR_WORK *p_wk	���[�N
 *	@param	frm								�t���[��
 *	@param	clsys_draw_type		�Z���ǂݍ��݃^�C�v
 *	@param	PALTYPE bgplttype	�p���b�g�ǂݍ��ݏꏊ
 *	@param	mapping						�Z���}�b�s���O���[�h
 *	@param	bg_plt						BG�̃p���b�g�ԍ�
 *	@patam	obj_plt						OBJ�̃p���b�g�ԍ�
 *	@param	is_notload_bg			TRU��BG�ǂݍ��܂Ȃ��t���O
 *	@param	heapID						�ǂݍ��݃e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void RES_Init( RES_WORK *p_wk, u8 frm, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, BOOL is_notload_bg, HEAPID heapID )
{	
	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(RES_WORK) );
	p_wk->bg_frm	= frm;

	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

		//BG
		if( !is_notload_bg )
		{	
			//�̈�̊m��
			GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
					bgplttype, bg_plt*0x20, ZKND_TBAR_BG_PLT_NUM*0x20, heapID );
			//�m�ۂ����̈�ɓǂݍ���
			p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, ZKND_TBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
			GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "ZKND_TBAR:BG�L�����̈悪����܂���ł���\n" );
			//�X�N���[���̓�������ɒu���āA����32*3������������
			RES_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), ZKND_TBAR_MENUBAR_X, ZKND_TBAR_MENUBAR_Y, 32, 24, 
					ZKND_TBAR_MENUBAR_X, ZKND_TBAR_MENUBAR_Y, ZKND_TBAR_MENUBAR_W, ZKND_TBAR_MENUBAR_H,
					bg_plt, FALSE, heapID );
		}


		//OBJ
		//���ʃA�C�R�����\�[�X	
		p_wk->res[ZKND_TBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, ZKND_TBAR_OBJ_PLT_NUM, heapID );	
		p_wk->res[ZKND_TBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

		p_wk->res[ZKND_TBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				APP_COMMON_GetBarIconCellArcIdx(mapping),
				APP_COMMON_GetBarIconAnimeArcIdx(mapping), heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *
 *	@param	RES_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void RES_Exit( RES_WORK *p_wk )
{	
	//OBJ
	GFL_CLGRP_CELLANIM_Release( p_wk->res[ZKND_TBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[ZKND_TBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[ZKND_TBAR_RES_COMMON_PLT] );

	//BG
	GFL_BG_FreeCharacterArea(p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));


	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(RES_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�擾
 *
 *	@param	const RES_WORK *cp_wk	���[�N
 *	@param	type									�^�C�v
 *
 *	@return	���\�[�X�擾
 */
//-----------------------------------------------------------------------------
static u32 RES_GetResource( const RES_WORK *cp_wk, RES_TYPE type )
{	
	return cp_wk->res[ type ];
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void RES_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	

//=============================================================================
/**
 *			ICON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ICON������
 *
 *	@param	ICON_WORK *p_wk								���[�N
 *	@param	p_unit												CLWK�쐬�pUNIT
 *	@param	RES_WORK											���ʃ��\�[�X
 *	@param	ZKND_TBAR_ITEM_ICON *cp_setup	�Z�b�g�A�b�v�f�[�^
 *	@param	HEAPID heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const ZKND_TBAR_ITEM_ICON *cp_setup, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk ,sizeof(ICON_WORK) );
	p_wk->data	= *cp_setup;
	p_wk->is_active	= TRUE;
	
	//CLWK�쐬
	{	
		u32 cg, plt, cell;
		GFL_CLWK_DATA				cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= cp_setup->pos.x;
		cldata.pos_y	= cp_setup->pos.y;

		//�^�C�v�ɂ���ă��\�[�X��f�[�^���Ⴄ
		if( cp_setup->icon >= ZKND_TBAR_ICON_CUSTOM )  // ZKND_Modified �J�X�^���̌������Ȃ��ɂ����B
		{	
			//�J�X�^���A�C�R��
			cg		= cp_setup->cg_idx;
			plt		= cp_setup->plt_idx;
			cell	= cp_setup->cell_idx;
			p_wk->movetype	= ZKND_TBAR_ICON_MOVETYPE_PUSH;
		}
		else
		{	
			//���ʃA�C�R��
			cg		= RES_GetResource( cp_res, ZKND_TBAR_RES_COMMON_CHR );
			plt		= RES_GetResource( cp_res, ZKND_TBAR_RES_COMMON_PLT );
			cell	= RES_GetResource( cp_res, ZKND_TBAR_RES_COMMON_CEL );

			p_wk->data.active_anmseq		= sc_common_icon_data[ cp_setup->icon ].active_anmseq;
			p_wk->data.noactive_anmseq	= sc_common_icon_data[ cp_setup->icon ].noactive_anmseq;
			p_wk->data.push_anmseq			= sc_common_icon_data[ cp_setup->icon ].push_anmseq;
			p_wk->data.key							= sc_common_icon_data[ cp_setup->icon ].key;
			p_wk->data.se								= sc_common_icon_data[ cp_setup->icon ].se;
			p_wk->movetype							= sc_common_icon_data[ cp_setup->icon ].movetype;
		}
		p_wk->now_anmseq	= p_wk->data.active_anmseq;
		cldata.anmseq	= p_wk->now_anmseq;

		//�쐬
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit, cg, plt, cell, &cldata, clsys_def_type, heapID );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk, TRUE );

	}

		//����^�C�v�ɂ���Ď��s�֐��؂�ւ�
	{	
		switch( p_wk->movetype )
		{	
		case ZKND_TBAR_ICON_MOVETYPE_PUSH:	//�����ꂽ��A�������A�j���ɂȂ�{�^��
			p_wk->push_func	= Icon_PushFuncNormal;
			break;
		case ZKND_TBAR_ICON_MOVETYPE_FLIP:	//�����ꂽ��AON��OFF���؂�ւ�{�^��
			p_wk->push_func	= Icon_PushFuncFlip;
			break;
		default:
			GF_ASSERT_MSG( 0, "ZKND_TBAR:����^�C�v���s�K�؂ł�%d", p_wk->movetype );
		}

	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON�j��
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ICON_Exit( ICON_WORK *p_wk )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( p_wk->p_clwk );
	//�N���A
	GFL_STD_MemClear( p_wk ,sizeof(ICON_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	���C������
 *
 *	@param	ICON_WORK *p_wk ���[�N
 *
 *	@return	TRUE����ON�@FALSE����OFF
 */
//-----------------------------------------------------------------------------
static BOOL ICON_Main( ICON_WORK *p_wk )
{	
				
	BOOL	is_visible;
	BOOL	is_update;
	u32 x, y;

	is_update		= FALSE;
	is_visible	= GFL_CLACT_WK_GetDrawEnable( p_wk->p_clwk );

	//����\�ȂƂ��̂�
	if( p_wk->is_active & is_visible )
	{	
		//�^�b�`����
		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
			if( ((u32)( x - p_wk->data.pos.x) <= (u32)(p_wk->data.width))  // ZKND_Modified �����w��ł���悤�ɂ����B
					&	((u32)( y - p_wk->data.pos.y) <= (u32)(ZKND_TBAR_ICON_HEIGHT)))
			{
				GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
				is_update	= TRUE;
			}
		}

		//�L�[����
		if( p_wk->data.key != 0 )
		{	
			if( GFL_UI_KEY_GetTrg() & p_wk->data.key )
			{	
				GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
				is_update	= TRUE;
			}
		}

		//����֐�
		if( is_update )
		{	
			p_wk->push_func( p_wk );
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�A�j���̓�����`�F�b�N
 *
 *	@param	const ICON_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�œ��삵�Ă��Ȃ�	FALSE�œ��쒆
 */
//-----------------------------------------------------------------------------
static BOOL ICON_IsMoveEnd( const ICON_WORK *cp_wk )
{	
	return !GFL_CLACT_WK_CheckAnmActive( cp_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�A�C�R���̎��
 *
 *	@param	const ICON_WORK *cp_wk ���[�N
 *
 *	@return	�A�C�R���̎��
 */
//-----------------------------------------------------------------------------
static ZKND_TBAR_ICON ICON_GetType( const ICON_WORK *cp_wk )
{	
	return	cp_wk->data.icon;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�\���ݒ�
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	is_visible			TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void ICON_SetVisible( ICON_WORK *p_wk, BOOL is_visible )
{	
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�\���擾
 *
 *	@param	const ICON_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�\��	FALSE��\��
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetVisible( const ICON_WORK *cp_wk )
{	
	return GFL_CLACT_WK_GetDrawEnable( cp_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�A�N�e�B�u��Ԑݒ�
 *
 *	@param	ICON_WORK *p_wk		���[�N
 *	@param	is_active					TRUE�ŃA�N�e�B�u���	FALSE�Ńm���A�N�e�B�u���
 */
//-----------------------------------------------------------------------------
static void ICON_SetActive( ICON_WORK *p_wk, BOOL is_active )
{	
	p_wk->is_active	= is_active;

	if( is_active )
	{	
		//�A�N�e�B�u�ɂȂ����u�ԂɃA�j���J�n�ɂȂ�Ȃ��悤�ɂ���
		if( p_wk->now_anmseq == p_wk->data.push_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.active_anmseq;
		}	
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
	}
	else
	{	
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->data.noactive_anmseq );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�A�N�e�B�u����擾
 *
 *	@param	const ICON_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�ŃA�N�e�B�u���	FALSE�Ńm���A�N�e�B�u���
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetActive( const ICON_WORK *cp_wk )
{	
	return cp_wk->is_active;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	SE��ݒ�
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	se							SE
 */
//-----------------------------------------------------------------------------
static void ICON_SetSE( ICON_WORK *p_wk, u32 se )
{	
	p_wk->data.se	= se;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	SE���擾
 *
 *	@param	const ICON_WORK *cp_wk	���[�N
 *
 *	@return	SE
 */
//-----------------------------------------------------------------------------
static u32 ICON_GetSE( const ICON_WORK *cp_wk )
{	
	return cp_wk->data.se;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�L�[���͂��Z�b�g
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	key							�L�[����	PAD_KEY_UP��
 */
//-----------------------------------------------------------------------------
static void ICON_SetKey( ICON_WORK *p_wk, u32 key )
{	
	p_wk->data.key	= key;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�L�[���͂��擾
 *
 *	@param	const ICON_WORK *cp_wk	���[�N
 *
 *	@return	�L�[����
 */
//-----------------------------------------------------------------------------
static u32 ICON_GetKey( const ICON_WORK *cp_wk )
{	
	return cp_wk->data.key;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�t���b�v�ݒ�
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	is_flip					TRUE��ON	FALSE��OFF
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetFlip( ICON_WORK *p_wk, BOOL is_flip )
{	
	if( is_flip )
	{	
		p_wk->now_anmseq	= p_wk->data.push_anmseq;
	}
	else
	{	
		p_wk->now_anmseq	= p_wk->data.active_anmseq;
	}	
	GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	�t���b�v�擾
 *
 *	@param	const ICON_WORK *cp_wk ���[�N
 *
 *	@return	TRUE��ON	FALSE��OFF
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetFlip( const ICON_WORK *cp_wk )
{	
	return cp_wk->now_anmseq	== cp_wk->data.push_anmseq;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���������ꂽ���̓���֐�	�ʏ��
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Icon_PushFuncNormal( ICON_WORK *p_wk )
{	
	//�A�j����ς��A����炷
	p_wk->now_anmseq	= p_wk->data.push_anmseq;

	GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );

	if( p_wk->data.se != 0 )
	{	
		PMSND_PlaySE( p_wk->data.se );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���������ꂽ���̓���֐�	�t���b�v��
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Icon_PushFuncFlip( ICON_WORK *p_wk )
{	
	BOOL now_flip	= ICON_GetFlip( p_wk );

	ICON_SetFlip( p_wk, !now_flip );

	if( p_wk->data.se != 0 )
	{	
		PMSND_PlaySE( p_wk->data.se );
	}
}
