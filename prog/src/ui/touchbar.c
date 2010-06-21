//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		touchbar.c
 *	@brief	�A�v���p����ʃ^�b�`�o�[
 *	@author	Toru=Nagihashi
 *	@data		2009.09.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//sound
#include "sound/pm_sndsys.h"

//mine
#include "ui/touchbar.h"
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
	TOUCHBAR_RES_COMMON_PLT,
	TOUCHBAR_RES_COMMON_CHR,
	TOUCHBAR_RES_COMMON_CEL,

	TOUCHBAR_RES_MAX
}RES_TYPE;

//-------------------------------------
///	�V�[�P���X
//=====================================
enum
{
	TOUCHBAR_SEQ_MAIN,	//�^�b�`�҂�
	TOUCHBAR_SEQ_ANM_TRG,		//�{�^���A�j��
	TOUCHBAR_SEQ_ANM,		//�{�^���A�j��
	TOUCHBAR_SEQ_TRG,		//�g���K
} ;

//-------------------------------------
///	����֐��^�C�v
//=====================================
typedef enum
{
	TOUCHBAR_ICON_MOVETYPE_PUSH,	//�����ꂽ��A�������A�j���ɂȂ�{�^��
	TOUCHBAR_ICON_MOVETYPE_FLIP,	//�����ꂽ��AON��OFF���؂�ւ�{�^��
} TOUCHBAR_ICON_MOVETYPE;


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
typedef void (*ACTIVE_FUNC)( ICON_WORK *p_wk, BOOL is_active );
struct _ICON_WORK
{
	GFL_CLWK						*p_clwk;		
	BOOL								is_active;		//�A�N�e�B�u��
	u32									now_anmseq;		//���݂̃A�j��
	PUSH_FUNC						push_func;		//�����ꂽ���̓���
  ACTIVE_FUNC         active_func;  //�A�N�e�B�u�A�p�b�V�u�ؑ֎�����
	TOUCHBAR_ICON_MOVETYPE	movetype;	//����^�C�v
	TOUCHBAR_ITEM_ICON			data;			//�{�^�����
} ;
//-------------------------------------
///	���ʃ��\�[�X
//=====================================
typedef struct 
{
	u32				res[TOUCHBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
} RES_WORK;

//-------------------------------------
///	����ʃo�[���C�����[�N
//=====================================
struct _TOUCHBAR_WORK
{
	s32				trg;			//�������A�C�R���̃C���f�b�N�X
	u32				tbl_len;	//�ő�A�C�R���̃e�[�u����
	u32				seq;			//�V�[�P���X

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
static ICON_WORK * Touchbar_Search( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON type );
static const ICON_WORK * Touchbar_SearchConst( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON type );

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
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const TOUCHBAR_ITEM_ICON *cp_setup,CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID );
static void ICON_Exit( ICON_WORK *p_wk );
static BOOL ICON_Main( ICON_WORK *p_wk );
static BOOL ICON_IsMoveEnd( const ICON_WORK *cp_wk );
static TOUCHBAR_ICON ICON_GetType( const ICON_WORK *cp_wk );
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
static void ICON_SetSoftPriority( ICON_WORK *p_wk, u8 pri );
static u8 ICON_GetSoftPriority( const ICON_WORK *p_wk );
static void ICON_SetBGPriority( ICON_WORK *p_wk, u8 bg_prio );
static u8 ICON_GetBGPriority( const ICON_WORK *p_wk );
//�ȉ����[�h�ʊ֐�
static void Icon_PushFuncNormal( ICON_WORK *p_wk );
static void Icon_PushFuncFlip( ICON_WORK *p_wk );
static void Icon_ActiveFuncNormal( ICON_WORK *p_wk, BOOL is_active );
static void Icon_ActiveFuncFlip( ICON_WORK *p_wk, BOOL is_active );
//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�A�C�R�����
//	TOUCHBAR_ICON�ƑΉ������A�N�e�B�u�e�[�u��
//=====================================
static const struct
{	
  union
  { 
    struct
    { 
      u16 active_anmseq;
      u16 noactive_anmseq;
      u16 push_anmseq;
      u16 dummy_anmseq;
    };
    struct
    { 
      u16 off_active_anmseq;
      u16 off_noactive_anmseq;
      u16 on_active_anmseq;
      u16 on_noactive_anmseq;
    };
  };
	u16 se;
	u32	key;
	TOUCHBAR_ICON_MOVETYPE	movetype;
} sc_common_icon_data[TOUCHBAR_ICON_MAX]	=
{	
	//TOUCHBAR_ICON_CLOSE,	//�~�{�^��
	{	
    { 
      APP_COMMON_BARICON_EXIT,
      APP_COMMON_BARICON_EXIT_OFF,
      APP_COMMON_BARICON_EXIT_ON,
      0,
    },
    TOUCHBAR_SE_CLOSE,
		PAD_BUTTON_X,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_RETURN,	//�����{�^��
	{	
    { 
      APP_COMMON_BARICON_RETURN,
      APP_COMMON_BARICON_RETURN_OFF,
      APP_COMMON_BARICON_RETURN_ON,
      0,
    },
		TOUCHBAR_SE_CANCEL,
		PAD_BUTTON_B,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_D,	//���{�^��
	{	
    { 
      APP_COMMON_BARICON_CURSOR_DOWN,
      APP_COMMON_BARICON_CURSOR_DOWN_OFF,
      APP_COMMON_BARICON_CURSOR_DOWN_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_DOWN,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_U,	//���{�^��
	{	
    { 
      APP_COMMON_BARICON_CURSOR_UP,
      APP_COMMON_BARICON_CURSOR_UP_OFF,
      APP_COMMON_BARICON_CURSOR_UP_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_UP,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_L,	//���{�^��
	{	
    { 
      APP_COMMON_BARICON_CURSOR_LEFT,
      APP_COMMON_BARICON_CURSOR_LEFT_OFF,
      APP_COMMON_BARICON_CURSOR_LEFT_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_LEFT,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_R,	//���{�^��
	{	
    { 
      APP_COMMON_BARICON_CURSOR_RIGHT,
      APP_COMMON_BARICON_CURSOR_RIGHT_OFF,
      APP_COMMON_BARICON_CURSOR_RIGHT_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_RIGHT,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CHECK,	//���������{�^��
	{	
    { 
      APP_COMMON_BARICON_CHECK_OFF,
      APP_COMMON_BARICON_CHECK_OFF_PASSIVE,
      APP_COMMON_BARICON_CHECK_ON,
      APP_COMMON_BARICON_CHECK_ON_PASSIVE,
    },
		TOUCHBAR_SE_Y_REG,
		PAD_BUTTON_Y,
		TOUCHBAR_ICON_MOVETYPE_FLIP,
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
 *	@param	const TOUCHBAR_SETUP *cp_setup	�ݒ���
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
extern TOUCHBAR_WORK * TOUCHBAR_Init( TOUCHBAR_SETUP *p_setup, HEAPID heapID )
{	
	TOUCHBAR_WORK	*p_wk;


	//���[�N�쐬�A������
	{	
		u32 size;

		size	= sizeof(TOUCHBAR_WORK) + sizeof(ICON_WORK) * p_setup->item_num;
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );

		p_wk->tbl_len	= p_setup->item_num;
		p_wk->seq			= TOUCHBAR_SEQ_MAIN;
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
 *	@brief	TOUCHBAR	�j��
 *
 *	@param	TOUCHBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Exit( TOUCHBAR_WORK *p_wk )
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
 *	@brief	TOUCHBAR	���C������
 *
 *	@param	TOUCHBAR_WORK *p_wk			���[�N
 *	@param	CURSOR_WORK	cp_cursor	�J�[�\���Ń{�^���������̂����������邽��
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Main( TOUCHBAR_WORK *p_wk )
{	
	switch( p_wk->seq )
	{	
	case TOUCHBAR_SEQ_MAIN:
		//�^�b�`���o
		{	
			int i;
			p_wk->trg		= TOUCHBAR_SELECT_NONE;
			for( i = 0; i < p_wk->tbl_len; i++ )
			{
				if( ICON_Main( &p_wk->icon[ i ] ) )
				{	
					p_wk->trg	= i;
					p_wk->seq	= TOUCHBAR_SEQ_ANM_TRG;
					break;
				}
			}
		}
		break;

	case TOUCHBAR_SEQ_ANM_TRG:	//�A�j���O�̂PF�𓾂邽��
			p_wk->seq	 = TOUCHBAR_SEQ_ANM;
		/* fallthrough */
	
	case TOUCHBAR_SEQ_ANM:
		//����
		if( ICON_IsMoveEnd( &p_wk->icon[ p_wk->trg ] ) )
		{
			p_wk->seq	 = TOUCHBAR_SEQ_TRG;
		}
		break;

	case TOUCHBAR_SEQ_TRG:
		//�g���K��1�t���[�������`����
		p_wk->seq	 = TOUCHBAR_SEQ_MAIN;
		break;
	}
	
	}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�I�����ꂽ���̂��擾	�A�j����̂PF
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ���[�N
 *
 *	@return	TOUCHBAR_SELECT��
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetTrg( const TOUCHBAR_WORK *cp_wk )
{	

	if( cp_wk->seq	== TOUCHBAR_SEQ_TRG && cp_wk->trg != TOUCHBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return TOUCHBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�I�����ꂽ���̂��擾	�A�j���O�̂PF
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ���[�N
 *
 *	@return	TOUCHBAR_SELECT��
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetTouch( const TOUCHBAR_WORK *cp_wk )
{	
	if( cp_wk->seq	== TOUCHBAR_SEQ_ANM_TRG && cp_wk->trg != TOUCHBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return TOUCHBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�S�ẴA�C�R����ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	is_active						�A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetActiveAll( TOUCHBAR_WORK *p_wk, BOOL is_active )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetActive( &p_wk->icon[ i ], is_active );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�S�ẴA�C�R����\���ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	is_active						�\��
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetVisibleAll( TOUCHBAR_WORK *p_wk, BOOL is_visible )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetVisible( &p_wk->icon[ i ], is_visible );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�S�ẴA�C�R����BG�v���C�I���e�B��ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	bg_prio						BG�v���C�I���e�B
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetBGPriorityAll( TOUCHBAR_WORK* p_wk, u8 bg_prio )
{
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetBGPriority( &p_wk->icon[ i ], bg_prio );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�S�ẴA�C�R����OBJ�v���C�I���e�B��ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	pri						OBJ�v���C�I���e�B
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSoftPriorityAll( TOUCHBAR_WORK* p_wk, u8 pri )
{
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetSoftPriority( &p_wk->icon[ i ], pri );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R���̃A�N�e�B�u�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon								�ݒ肷��A�C�R��
 *	@param	is_active						TRUE�ŃA�N�e�B�u	FALSE�Ńm���A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetActive( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_active )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetActive( p_icon, is_active );
}
//----------------------------------------------------------------------------
/**	
 *	@brief	TOUCHBAR	�A�C�R���̃A�N�e�B�u���擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	���[�N
 *	@param	icon												�擾����A�C�R��
 *
 *	@return	TRUE�ŃA�N�e�B�u	FALSE�Ńm���A�N�e�B�u
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetActive( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetActive( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R���̕\���ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon								�ݒ肷��A�C�R��
 *	@param	is_visible					TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetVisible( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_visible )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetVisible( p_icon, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R���̕\���擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	���[�N
 *	@param	icon												�擾����A�C�R��
 *
 *	@return	TRUE�ŕ\��	FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetVisible( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetVisible( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R����SE��ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon								�A�C�R��
 *	@param	se									SE
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSE( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 se )
{	

	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetSE( p_icon, se );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R����SE���擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	���[�N
 *	@param	icon	�A�C�R��
 *
 *	@return	SE
 */
//-----------------------------------------------------------------------------
u32 TOUCHBAR_GetSE( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetSE( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R���ɃL�[���͂�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon								�A�C�R��
 *	@param	key									�L�[����	PAD_KEY_UP��PAD_BUTTON_A��
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetUseKey( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 key )
{	

	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetKey( p_icon, key );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�A�C�R������L�[���͂��擾
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	���[�N
 *	@param	icon												�A�C�R��
 *
 *	@return	�L�[����
 */
//-----------------------------------------------------------------------------
u32 TOUCHBAR_GetUseKey( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetKey( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ�v���C�I���e�B�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 *	@param	icon                �A�C�R��
 *	@param	pri                 �D�揇��
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSoftPriority( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u8 pri )
{ 

	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetSoftPriority( p_icon, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ�v���C�I���e�B�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 *	@param	icon                �A�C�R��
 *	@param	pri                 �D�揇��
 */
//-----------------------------------------------------------------------------
u8 TOUCHBAR_GetSoftPriority( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{ 
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�ݒ�
	return ICON_GetSoftPriority( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BG�v���C�I���e�B�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 *	@param	icon                �A�C�R��
 *	@param	pri                 �D�揇��
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetBgPriority( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u8 pri )
{ 
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetBGPriority( p_icon, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ�v���C�I���e�B�ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 *	@param	icon                �A�C�R��
 *	@param	pri                 �D�揇��
 */
//-----------------------------------------------------------------------------
u8 TOUCHBAR_GetBgPriority( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{ 
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�ݒ�
	return ICON_GetBGPriority( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�t���b�v���{�^���ւ̐ݒ�
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon			�A�C�R��
 *	@param	is_flip		TRUE�Ńt���b�vON�i�`�F�b�N����Ă���jFALSE��OFF
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetFlip( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, BOOL is_flip )
{	
	ICON_WORK * p_icon;
		
	//�w��A�C�R��������
	p_icon	= Touchbar_Search( p_wk, icon );

	//�ݒ�
	ICON_SetFlip( p_icon, is_flip );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	�t���b�v���{�^���̎擾
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	icon			�A�C�R��
 *	@retval	is_flip		TRUE�Ńt���b�vON�i�`�F�b�N����Ă���jFALSE��OFF
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetFlip( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//�w��A�C�R��������
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//�擾
	return ICON_GetFlip( cp_icon );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`�o�[���Z�b�g
 *
 *	@param	TOUCHBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_ReStart( TOUCHBAR_WORK *p_wk )
{
  p_wk->seq = TOUCHBAR_SEQ_MAIN;
  p_wk->trg = TOUCHBAR_SELECT_NONE;

  TOUCHBAR_SetActiveAll( p_wk, TRUE );
}

//=============================================================================
/**
 *	TOUCHBAR private
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�R���̎�ނ���I�u�W�F�N�g��Search
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	type								�T���A�C�R���̃^�C�v
 *
 *	@return	��������ICON
 */
//-----------------------------------------------------------------------------
static ICON_WORK * Touchbar_Search( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON type )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		if( ICON_GetType( &p_wk->icon[ i ] ) == type )
		{	
			return &p_wk->icon[ i ];
		}
	}

	GF_ASSERT_MSG( 0,"TOUCHBAR:�w�肵���A�C�R����������܂���ł���%d", type );
	return NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�R���̎�ނ���I�u�W�F�N�g��Search	CONST��
 *
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
 *	@param	type								�T���A�C�R���̃^�C�v
 *
 *	@return	��������ICON
 */
//-----------------------------------------------------------------------------
static const ICON_WORK * Touchbar_SearchConst( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON type )
{	
	return Touchbar_Search( (TOUCHBAR_WORK*)cp_wk, type );
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
 *	@param	TOUCHBAR_WORK *p_wk	���[�N
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
					bgplttype, bg_plt*0x20, TOUCHBAR_BG_PLT_NUM*0x20, heapID );
			//�m�ۂ����̈�ɓǂݍ���
			p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, TOUCHBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
			GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "TOUCHBAR:BG�L�����̈悪����܂���ł���\n" );
			//�X�N���[���̓�������ɒu���āA����32*3������������
			RES_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, 32, 24, 
					TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, TOUCHBAR_MENUBAR_W, TOUCHBAR_MENUBAR_H,
					bg_plt, FALSE, heapID );
		}


		//OBJ
		//���ʃA�C�R�����\�[�X	
		p_wk->res[TOUCHBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, TOUCHBAR_OBJ_PLT_NUM, heapID );	
		p_wk->res[TOUCHBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

		p_wk->res[TOUCHBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
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
	GFL_CLGRP_CELLANIM_Release( p_wk->res[TOUCHBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[TOUCHBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[TOUCHBAR_RES_COMMON_PLT] );

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
 *	@param	TOUCHBAR_ITEM_ICON *cp_setup	�Z�b�g�A�b�v�f�[�^
 *	@param	HEAPID heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const TOUCHBAR_ITEM_ICON *cp_setup, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID )
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
		if( cp_setup->icon >= TOUCHBAR_ICON_CUTSOM1 )
		{	
			//�J�X�^���A�C�R��
			cg		= cp_setup->cg_idx;
			plt		= cp_setup->plt_idx;
			cell	= cp_setup->cell_idx;
			p_wk->movetype	= TOUCHBAR_ICON_MOVETYPE_PUSH;
		}
		else
		{	
			//���ʃA�C�R��
			cg		= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_CHR );
			plt		= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_PLT );
			cell	= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_CEL );

			p_wk->data.active_anmseq		= sc_common_icon_data[ cp_setup->icon ].active_anmseq;
			p_wk->data.noactive_anmseq	= sc_common_icon_data[ cp_setup->icon ].noactive_anmseq;
			p_wk->data.push_anmseq			= sc_common_icon_data[ cp_setup->icon ].push_anmseq;
			p_wk->data.dummy_anmseq			= sc_common_icon_data[ cp_setup->icon ].dummy_anmseq;
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
		case TOUCHBAR_ICON_MOVETYPE_PUSH:	//�����ꂽ��A�������A�j���ɂȂ�{�^��
			p_wk->push_func	= Icon_PushFuncNormal;
      p_wk->active_func = Icon_ActiveFuncNormal;
			break;
		case TOUCHBAR_ICON_MOVETYPE_FLIP:	//�����ꂽ��AON��OFF���؂�ւ�{�^��
			p_wk->push_func	  = Icon_PushFuncFlip;
      p_wk->active_func = Icon_ActiveFuncFlip;
			break;
		default:
			GF_ASSERT_MSG( 0, "TOUCHBAR:����^�C�v���s�K�؂ł�%d", p_wk->movetype );
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
			if( ((u32)( x - p_wk->data.pos.x) <= (u32)(TOUCHBAR_ICON_WIDTH))
					&	((u32)( y - p_wk->data.pos.y) <= (u32)(TOUCHBAR_ICON_HEIGHT)))
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
static TOUCHBAR_ICON ICON_GetType( const ICON_WORK *cp_wk )
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
  p_wk->active_func( p_wk, is_active );
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
	return cp_wk->now_anmseq	== cp_wk->data.push_anmseq ||
          cp_wk->now_anmseq == cp_wk->data.dummy_anmseq;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	OBJ�v���C�I���e�B�ݒ�
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	pri					OBJ�v���C�I���e�B
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetSoftPriority( ICON_WORK *p_wk, u8 pri )
{
  GF_ASSERT(p_wk);
  GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	OBJ�v���C�I���e�B�擾
 *
 *	@param	const ICON_WORK *cp_wk ���[�N
 *
 *	@return	OBJ�v���C�I���e�B
 */
//-----------------------------------------------------------------------------
static u8 ICON_GetSoftPriority( const ICON_WORK *p_wk )
{
  GF_ASSERT(p_wk);
  return GFL_CLACT_WK_GetSoftPri( p_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	BG�v���C�I���e�B�ݒ�
 *
 *	@param	ICON_WORK *p_wk	���[�N
 *	@param	bg_prio						BG�v���C�I���e�B
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetBGPriority( ICON_WORK *p_wk, u8 bg_prio )
{
  GF_ASSERT(p_wk);
  GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, bg_prio );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	BG�v���C�I���e�B�擾
 *
 *	@param	const ICON_WORK *cp_wk ���[�N
 *
 *	@return	BG�v���C�I���e�B
 */
//-----------------------------------------------------------------------------
static u8 ICON_GetBGPriority( const ICON_WORK *p_wk )
{
  GF_ASSERT(p_wk);
  return GFL_CLACT_WK_GetBgPri( p_wk->p_clwk );
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
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���������ꂽ���̓���֐�	�ʏ��
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Icon_ActiveFuncNormal( ICON_WORK *p_wk, BOOL is_active )
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
 *	@brief	�{�^���������ꂽ���̓���֐�	�t���b�v��
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Icon_ActiveFuncFlip( ICON_WORK *p_wk, BOOL is_active )
{	
	p_wk->is_active	= is_active;

	if( is_active )
	{	
		if( p_wk->now_anmseq == p_wk->data.push_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.dummy_anmseq;
		}	
    else if( p_wk->now_anmseq == p_wk->data.noactive_anmseq )
    { 
			p_wk->now_anmseq	= p_wk->data.active_anmseq;
    }
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
	}
	else
	{	
		if( p_wk->now_anmseq == p_wk->data.dummy_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.push_anmseq;
		}	
    else if( p_wk->now_anmseq == p_wk->data.active_anmseq )
    { 
			p_wk->now_anmseq	= p_wk->data.noactive_anmseq;
    }
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->data.noactive_anmseq );
	}
}
