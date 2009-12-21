//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_sidebar.c
 *	@brief  �T�C�h�o�[����
 *	@author	Toru=Nagihashi
 *	@data		2009.12.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include <calctool.h>

//�o�g�����R�[�_�[�����W���[��
#include "br_res.h"
#include "br_fade.h"

//�O�����J
#include "br_sidebar.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	����
//=====================================
#define BR_SIDEBAR_DISPLAY_MAX  (2)
#define BR_SIDEBAR_MAX          (6*BR_SIDEBAR_DISPLAY_MAX)
#define BR_SIDEBAR_MOVE_F       (10)			///< �T�C�h�o�[�̈ړ��t���[��

//-------------------------------------
///	�T�C�h�o�[�P��
//=====================================
typedef enum
{ 
  BR_SIDEBAR_ONE_MOVE_MODE_NONE,  //���삵�Ȃ�
  BR_SIDEBAR_ONE_MOVE_MODE_BOOT,  //�N�����̓���
  BR_SIDEBAR_ONE_MOVE_MODE_SHAKE, //���ł����h��Ă��铮��
  BR_SIDEBAR_ONE_MOVE_MODE_END,   //�I�����̓���
}BR_SIDEBAR_ONE_MOVE_MODE;

//-------------------------------------
///	�T�C�h�o�[�̕���
//=====================================
typedef enum
{ 
  BR_SIDEBAR_DIR_LEFT,
  BR_SIDEBAR_DIR_RIGHT,
}BR_SIDEBAR_DIR;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///�T�C�h�o�[�̃f�[�^	
//=====================================
typedef struct 
{
  s16 boot_init_x;            //�����l
  s16 boot_init_y;            //�����l
  s16 boot_scale_x;           //�������X�P�[��
  s16 boot_move_f;            //�ړ��t���[��
  s16 boot_end_x;             //�ړI�l
  s16 boot_end_y;             //�ړI�l
  s16 shake_init_deg;         //�����l
  s16 shake_deg_dir;          //�h������
  CLSYS_DRAW_TYPE draw_type;  //�`��^�C�v
  BR_SIDEBAR_DIR  dir;        //����
} BR_SIDEBAR_DATA;

//-------------------------------------
///	�T�C�h�o�[�P��
//=====================================
typedef struct _BR_SIDEBAR_ONE BR_SIDEBAR_ONE;
typedef void (*BR_SIDEBAR_MOVE_FUNCTION)( BR_SIDEBAR_ONE *p_wk );
struct _BR_SIDEBAR_ONE 
{
  BOOL  is_exist;
  GFL_CLWK  *p_clwk;
  fx32  x;
  fx32  y;
  fx32  init_x;
  fx32  scale;
  BR_SIDEBAR_MOVE_FUNCTION  move_function;
  u32 state;
  s32 deg;
  s32 wait;
  s32 vconst;
  s32 val_x;
  fx32 val_scale;
  BR_SIDEBAR_DATA data;
} ;

//-------------------------------------
///	�T�C�h�o�[����
//=====================================
struct _BR_SIDEBAR_WORK
{ 
  BR_SIDEBAR_ONE  sidebar[ BR_SIDEBAR_MAX ];
};

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	BR_SIDEBAR_ONE
//=====================================
static void BR_SIDEBAR_ONE_Init( BR_SIDEBAR_ONE *p_wk, GFL_CLUNIT *p_clunit, const BR_RES_WORK *cp_res, const BR_SIDEBAR_DATA *cp_data, HEAPID heapID );
static void BR_SIDEBAR_ONE_Exit( BR_SIDEBAR_ONE *p_wk );
static void BR_SIDEBAR_ONE_Main( BR_SIDEBAR_ONE *p_wk );
static BOOL BR_SIDEBAR_ONE_IsExist( const BR_SIDEBAR_ONE *cp_wk );
static void BR_SIDEBAR_ONE_SetMoveMode( BR_SIDEBAR_ONE *p_wk, BR_SIDEBAR_ONE_MOVE_MODE mode );
static BOOL BR_SIDEBAR_ONE_IsMoveNone( const BR_SIDEBAR_ONE *cp_wk );
static void Br_SideBar_One_Move_Boot( BR_SIDEBAR_ONE *p_wk );
static void Br_SideBar_One_Move_Shake( BR_SIDEBAR_ONE *p_wk );
static void Br_SideBar_One_Move_End( BR_SIDEBAR_ONE *p_wk );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
/// �T�C�h�o�[�̃f�[�^	
//=====================================
static const BR_SIDEBAR_DATA sc_sidebar_data[ BR_SIDEBAR_MAX ] =
{ 
  //------------ ���� ---------------
  { 
    82,
    96,
    FX32_CONST(1.4f),
    BR_SIDEBAR_MOVE_F -2 * 2,
    8 -2,
    96,
    90,
    +4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    178,
    96,
    FX32_CONST(1.8f),
    BR_SIDEBAR_MOVE_F -3 * 2,
    8 - 3,
    96,
    135,
    -4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    118,
    96,
    FX32_CONST(2.0f),
    BR_SIDEBAR_MOVE_F +2 * 2,
    8 + 2,
    96,
    270,
    +4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    42,
    96,
    FX32_CONST(1.6f),
    BR_SIDEBAR_MOVE_F -1 * 2,
    255- 8 - 1,
    96,
    90,
    +4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_RIGHT,
  },
  { 
    150,
    96,
    FX32_CONST(1.4f),
    BR_SIDEBAR_MOVE_F -3 * 2,
    255- 8 - 3,
    96,
    135,
    -4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_RIGHT,
  },
  { 
    210,
    96,
    FX32_CONST(1.6f),
    BR_SIDEBAR_MOVE_F +2 * 2,
    255- 8 + 2,
    96,
    270,
    +4,
    CLSYS_DRAW_MAIN,
    BR_SIDEBAR_DIR_RIGHT,
  },

  //------------ ����� ---------------
  { 
    82,
    96,
    FX32_CONST(1.4f),
    BR_SIDEBAR_MOVE_F -2 * 2,
    8 - 2,
    96,
    90,
    +4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    178,
    96,
    FX32_CONST(1.8f),
    BR_SIDEBAR_MOVE_F -3 * 2,
    8 - 3,
    96,
    135,
    -4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    118,
    96,
    FX32_CONST(2.0f),
    BR_SIDEBAR_MOVE_F +2 * 2,
    8 + 2,
    96,
    270,
    +4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_LEFT,
  },
  { 
    42,
    96,
    FX32_CONST(1.6f),
    BR_SIDEBAR_MOVE_F -1 * 2,
    255- 8 - 1,
    96,
    90,
    +4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_RIGHT,
  },
  { 
    150,
    96,
    FX32_CONST(1.4f),
    BR_SIDEBAR_MOVE_F -3 * 2,
    255- 8 - 3,
    96,
    135,
    -4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_RIGHT,
  },
  { 
    210,
    96,
    FX32_CONST(1.6f),
    BR_SIDEBAR_MOVE_F +2 * 2,
    255- 8 + 2,
    96,270,
    +4,
    CLSYS_DRAW_SUB,
    BR_SIDEBAR_DIR_RIGHT,
  },
};


//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[  ������
 *
 *	@param	GFL_CLUNIT *p_clunit  OBJ�����p���j�b�g
 *	@param	*p_fade               �t�F�[�h���W���[��
 *	@param	*p_res                ���\�[�X�Ǘ�
 *	@param	heapID                �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_SIDEBAR_WORK * BR_SIDEBAR_Init( GFL_CLUNIT *p_clunit, BR_FADE_WORK *p_fade, BR_RES_WORK *p_res, HEAPID heapID )
{ 
  BR_SIDEBAR_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_SIDEBAR_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_SIDEBAR_WORK));

  //���\�[�X�ǂݍ���
  BR_RES_LoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_M, heapID );
  BR_RES_LoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_S, heapID );

  //�T�C�h�o�[�̏�����
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    { 
      BR_SIDEBAR_ONE_Init( &p_wk->sidebar[i], p_clunit, p_res, &sc_sidebar_data[i], heapID );
    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[  �j��
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_Exit( BR_SIDEBAR_WORK *p_wk, BR_RES_WORK *p_res )
{ 
  //�T�C�h�o�[�̔j��
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( BR_SIDEBAR_ONE_IsExist( &p_wk->sidebar[i]) )
      { 
        BR_SIDEBAR_ONE_Exit( &p_wk->sidebar[i] );
      }
    }
  }

  //���\�[�X�J��
  BR_RES_UnLoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_M );
  BR_RES_UnLoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_S );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[  ���C������
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_Main( BR_SIDEBAR_WORK *p_wk )
{ 
  //�T�C�h�o�[���C������
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( BR_SIDEBAR_ONE_IsExist( &p_wk->sidebar[i]) )
      { 
        BR_SIDEBAR_ONE_Main( &p_wk->sidebar[i] );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �N�����̓����J�n���N�G�X�g
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_StartBoot( BR_SIDEBAR_WORK *p_wk )
{ 
 //�T�C�h�o�[�N�����[�h��
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( BR_SIDEBAR_ONE_IsExist( &p_wk->sidebar[i]) )
      { 
        BR_SIDEBAR_ONE_SetMoveMode( &p_wk->sidebar[i], BR_SIDEBAR_ONE_MOVE_MODE_BOOT );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ펞�̓����J�n���N�G�X�g
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_StartShake( BR_SIDEBAR_WORK *p_wk )
{ 
 //�T�C�h�o�[�N�����[�h��
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( BR_SIDEBAR_ONE_IsExist( &p_wk->sidebar[i]) )
      { 
        BR_SIDEBAR_ONE_SetMoveMode( &p_wk->sidebar[i], BR_SIDEBAR_ONE_MOVE_MODE_SHAKE );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �I�����N�G�X�g
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_StartClose( BR_SIDEBAR_WORK *p_wk )
{ 
 //�T�C�h�o�[�N�����[�h��
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( BR_SIDEBAR_ONE_IsExist( &p_wk->sidebar[i]) )
      { 
        BR_SIDEBAR_ONE_SetMoveMode( &p_wk->sidebar[i], BR_SIDEBAR_ONE_MOVE_MODE_END );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C����ʂ̃��\�[�X�j��
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_UnLoadMain( BR_SIDEBAR_WORK *p_wk, BR_RES_WORK *p_res )
{
  //�T�C�h�o�[�̔j��
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( sc_sidebar_data[i].draw_type == CLSYS_DRAW_MAIN )
      { 
        BR_SIDEBAR_ONE_Exit( &p_wk->sidebar[i] );
      }
    }
  }

  //���\�[�X�j��
  BR_RES_UnLoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_M );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C����ʂ̃��\�[�X�ǂݍ���
 *
 *	@param	BR_SIDEBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SIDEBAR_LoadMain( BR_SIDEBAR_WORK *p_wk, GFL_CLUNIT *p_clunit, BR_FADE_WORK *p_fade, BR_RES_WORK *p_res, HEAPID heapID )
{ 
  //���\�[�X�ǂݍ���
  BR_RES_LoadOBJ( p_res, BR_RES_OBJ_SIDEBAR_M, heapID );

  //�T�C�h�o�[�̍쐬
  { 
    int i;
    for( i = 0; i < BR_SIDEBAR_MAX; i++ )
    {
      if( sc_sidebar_data[i].draw_type == CLSYS_DRAW_MAIN )
      { 
        BR_SIDEBAR_ONE_Init( &p_wk->sidebar[i], p_clunit, p_res, &sc_sidebar_data[i], heapID );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���슮���`�F�b�N
 *
 *	@param	const BR_SIDEBAR_WORK *cp_wk  ���[�N
 *
 *	@return TRUE�œ���I��  FALSE�œ��쒆
 */
//-----------------------------------------------------------------------------
BOOL BR_SIDEBAR_IsMoveEnd( const BR_SIDEBAR_WORK *cp_wk )
{ 
  int i;
  BOOL is_end  = TRUE;

  for( i = 0; i < BR_SIDEBAR_MAX; i++ )
  {
    if( BR_SIDEBAR_ONE_IsExist( &cp_wk->sidebar[i]) )
    { 
      is_end &= BR_SIDEBAR_ONE_IsMoveNone( &cp_wk->sidebar[i]);
    }
  }

  return is_end;
}

//=============================================================================
/**
 *    �T�C�h�o�[�P��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P�@������
 *
 *	@param	BR_SIDEBAR_ONE *p_wk  ���[�N
 *	@param	*p_clunit             �Z���쐬�p���j�b�g
 *	@param	*p_res                ���\�[�X�Ǘ�
 *	@param	BR_SIDEBAR_DATA *cp_data  �f�[�^
 *	@param	heapID                    �q�[�v
 */
//-----------------------------------------------------------------------------
static void BR_SIDEBAR_ONE_Init( BR_SIDEBAR_ONE *p_wk, GFL_CLUNIT *p_clunit, const BR_RES_WORK *cp_res, const BR_SIDEBAR_DATA *cp_data, HEAPID heapID )
{ 
  //������
  GFL_STD_MemClear( p_wk, sizeof(BR_SIDEBAR_ONE) );
  p_wk->data  = *cp_data;

  //CLWK�쐬
  { 
    BR_RES_OBJ_DATA res;

    //���\�[�X�擾
    { 
      BOOL ret;
      BR_RES_OBJID objID;

      if( cp_data->draw_type == CLSYS_DRAW_MAIN )
      { 
        objID = BR_RES_OBJ_SIDEBAR_M;
      }
      else
      { 
        objID = BR_RES_OBJ_SIDEBAR_S;
      }
      ret = BR_RES_GetOBJRes( cp_res, objID, &res );
      GF_ASSERT( ret );
    }

    //CLWK�쐬
    { 
      GFL_CLWK_DATA data;
      GFL_STD_MemClear( &data, sizeof(GFL_CLWK_DATA) );
      data.pos_x  = cp_data->boot_init_x;
      data.pos_y  = cp_data->boot_init_y;
      p_wk->x = FX32_CONST( data.pos_x );
      p_wk->y = FX32_CONST( data.pos_y );
      p_wk->scale = cp_data->boot_scale_x;

      p_wk->p_clwk  = GFL_CLACT_WK_Create( p_clunit,
          res.ncg, res.ncl, res.nce,
          &data, p_wk->data.draw_type, heapID );

      GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk, CLSYS_AFFINETYPE_DOUBLE );
      GFL_CLACT_WK_SetTypeScale( p_wk->p_clwk, p_wk->scale, CLSYS_MAT_X );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
    }

  }

  p_wk->is_exist  = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  �j��
 *
 *	@param	BR_SIDEBAR_ONE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_SIDEBAR_ONE_Exit( BR_SIDEBAR_ONE *p_wk )
{ 

  GFL_CLACT_WK_Remove( p_wk->p_clwk );
  GFL_STD_MemClear( p_wk, sizeof(BR_SIDEBAR_ONE) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  ���C������
 *
 *	@param	BR_SIDEBAR_ONE *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_SIDEBAR_ONE_Main( BR_SIDEBAR_ONE *p_wk )
{ 
  if( p_wk->move_function )
  { 
    p_wk->move_function( p_wk );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���݂��Ă��邩�`�F�b�N
 *
 *	@param	const BR_SIDEBAR_ONE *cp_wk ���[�N
 *
 *	@return TRUE�Ȃ�Α���  FALSE�Ȃ�Α��݂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BR_SIDEBAR_ONE_IsExist( const BR_SIDEBAR_ONE *cp_wk )
{ 
  return cp_wk->is_exist;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���삵�Ă��邩�`�F�b�N
 *
 *	@param	const BR_SIDEBAR_ONE *cp_wk   ���[�N
 *
 *	@return TRUE�œ��삵�Ă��Ȃ�  FALSE�œ��쒆
 */
//-----------------------------------------------------------------------------
static BOOL BR_SIDEBAR_ONE_IsMoveNone( const BR_SIDEBAR_ONE *cp_wk )
{ 
  return cp_wk->move_function == NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  �ړ����[�h�Z�b�g
 *
 *	@param	BR_SIDEBAR_ONE *p_wk  ���[�N
 *	@param	mode                  �ړ����[�h
 */
//-----------------------------------------------------------------------------
static void BR_SIDEBAR_ONE_SetMoveMode( BR_SIDEBAR_ONE *p_wk, BR_SIDEBAR_ONE_MOVE_MODE mode )
{ 
  switch( mode )
  { 
  case BR_SIDEBAR_ONE_MOVE_MODE_NONE: //�����s��Ȃ�
    p_wk->move_function = NULL;
    break;

  case BR_SIDEBAR_ONE_MOVE_MODE_BOOT:  //�N�����̓���
    p_wk->move_function = Br_SideBar_One_Move_Boot;
    break;

  case BR_SIDEBAR_ONE_MOVE_MODE_SHAKE: //���ł����h��Ă��铮��
    p_wk->move_function = Br_SideBar_One_Move_Shake;
    break;

  case BR_SIDEBAR_ONE_MOVE_MODE_END:  //�I����
    p_wk->move_function = Br_SideBar_One_Move_End;
    break;
  }

  p_wk->state = 0;
}
//-------------------------------------
///	�ړ��֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  �N�����̈ړ�
 *
 *	@param	BR_SIDEBAR_ONE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_SideBar_One_Move_Boot( BR_SIDEBAR_ONE *p_wk )
{ 
  enum 
  { 
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_MOVE1,
    SEQ_MOVE2,
    SEQ_END,
  };

  static const int SIDE_BAR_WAIT        =  3;
  static const int SIDE_BAR_SIN_MOVE_F  = 4;

  p_wk->vconst++;
  p_wk->vconst %=2;
  if( p_wk->vconst )
  { 
    return;
  }

  switch( p_wk->state )
  { 
  case SEQ_INIT:
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
    p_wk->wait  = 0;
    p_wk->scale = p_wk->data.boot_scale_x;
    p_wk->state = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    if ( p_wk->wait == SIDE_BAR_WAIT ){
      p_wk->state   = SEQ_MOVE1;
      p_wk->init_x  = FX32_CONST( GFL_CLACT_WK_GetTypePos( p_wk->p_clwk, p_wk->data.draw_type, CLSYS_MAT_X ) );
      p_wk->wait  = 0;
    }
    else {
      p_wk->wait++;
    }
    break;

	case SEQ_MOVE1:
		{	
      const s8 move = (p_wk->data.dir == BR_SIDEBAR_DIR_LEFT ) ? -1: +1;

			if ( p_wk->wait == SIDE_BAR_SIN_MOVE_F ){
				p_wk->state = SEQ_MOVE2;
				p_wk->wait  = 0;
			}
			else {				
				
        p_wk->init_x = p_wk->init_x + ( ( GFL_CALC_Sin360R( GFL_CALC_GET_ROTA_NUM( p_wk->deg ) ) * 1 ) * ( move * -1 ) );


				
				p_wk->deg += 32;
				p_wk->deg %= 360;
				
        GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, p_wk->init_x >> FX32_SHIFT, p_wk->data.draw_type, CLSYS_MAT_X );
				
				p_wk->wait++;
			}	
		}
		break;
	
	case SEQ_MOVE2:
		{
			s16 x;

      x = GFL_CLACT_WK_GetTypePos( p_wk->p_clwk, p_wk->data.draw_type, CLSYS_MAT_X );
			
			if ( p_wk->wait == 0 ){
				
				p_wk->val_x  = p_wk->data.boot_end_x - x;
				p_wk->val_x /= p_wk->data.boot_move_f;
				p_wk->wait++;
			}
			else if ( p_wk->wait == p_wk->data.boot_move_f + 1 ){
				x = p_wk->data.boot_end_x;
        p_wk->state = SEQ_END;
			}
			else {
				x += p_wk->val_x;
				p_wk->wait++;
			}
			
      GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, x, p_wk->data.draw_type, CLSYS_MAT_X );
		}	
		break;
  case SEQ_END:
    BR_SIDEBAR_ONE_SetMoveMode( p_wk, BR_SIDEBAR_ONE_MOVE_MODE_SHAKE );
    break;
  }

  if ( p_wk->state == SEQ_MOVE2 && p_wk->scale > FX32_CONST(0.1f) ){
		 p_wk->scale -= FX32_CONST(0.10f);
		 
     GFL_CLACT_WK_SetTypeScale( p_wk->p_clwk, p_wk->scale, CLSYS_MAT_X );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  �悱�ł����h���ړ�
 *
 *	@param	BR_SIDEBAR_ONE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_SideBar_One_Move_Shake( BR_SIDEBAR_ONE *p_wk )
{ 
  enum 
  { 
    SEQ_INIT,
    SEQ_MAIN,
  };

  p_wk->vconst++;
  p_wk->vconst %=2;
  if( p_wk->vconst )
  { 
    return;
  }

  switch( p_wk->state )
  { 
  case SEQ_INIT:
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
    GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, p_wk->data.boot_end_x, p_wk->data.draw_type, CLSYS_MAT_X );
    //GFL_CLACT_WK_SetTypeScale( p_wk->p_clwk, FX32_CONST(0.1f), CLSYS_MAT_X );
    p_wk->init_x  = FX32_CONST( p_wk->data.boot_end_x );
    p_wk->deg     = p_wk->data.shake_init_deg;
    p_wk->state   = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    { 
      const s8 move = (p_wk->data.dir == BR_SIDEBAR_DIR_LEFT ) ? -1: +1;
      fx32 now;

      //�Ȃ����A�x���烉�W�A���ɂ����l���A����Ƀ��W�A���ɕϊ����鎮��p���āASin�l�����o���Ă��邪�A
      //���Ƃ��Ƃ̃o�g�����R�[�_�[�ł������Ȃ��Ă����B�Č��̂��߁A���̂܂܁B
      now = p_wk->init_x + ( ( GFL_CALC_Sin360R( GFL_CALC_GET_ROTA_NUM( p_wk->deg ) ) * 3 ) * ( move * -1 ) );
      //NAGI_Printf( "0x%x\n", ( ( GFL_CALC_Sin360R( GFL_CALC_GET_ROTA_NUM( p_wk->deg ) ) * 3 ) * ( move * -1 ) ) );

      p_wk->deg += p_wk->data.shake_deg_dir;
      p_wk->deg %= 360;

      GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, now >> FX32_SHIFT, p_wk->data.draw_type, CLSYS_MAT_X );
    }
    break;
  }

  if (  p_wk->scale > FX32_CONST(0.1f) ){
		 p_wk->scale -= FX32_CONST(0.10f);
		 
     GFL_CLACT_WK_SetTypeScale( p_wk->p_clwk, p_wk->scale, CLSYS_MAT_X );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�C�h�o�[�P��  �I����
 *
 *	@param	BR_SIDEBAR_ONE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_SideBar_One_Move_End( BR_SIDEBAR_ONE *p_wk )
{ 
 enum 
  { 
    SEQ_INIT,
    SEQ_MOVE,
    SEQ_END,
  };

  static const int SIDE_BAR_WAIT        = 3;
  static const int SIDE_BAR_SIN_MOVE_F  = 4;

  p_wk->vconst++;
  p_wk->vconst %=2;
  if( p_wk->vconst )
  { 
    return;
  }

  switch( p_wk->state )
  { 
  case SEQ_INIT:
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
    p_wk->wait  = 0;
    p_wk->state = SEQ_MOVE;
    break;

	case SEQ_MOVE:
    {
      s16 x;
      fx32 scale;

      x     = GFL_CLACT_WK_GetTypePos( p_wk->p_clwk, p_wk->data.draw_type, CLSYS_MAT_X );
      scale = GFL_CLACT_WK_GetTypeScale( p_wk->p_clwk, CLSYS_MAT_X );

      if ( p_wk->wait == 0 ){

        p_wk->val_x  = p_wk->data.boot_init_x - x;
        p_wk->val_x /= p_wk->data.boot_move_f;
        p_wk->wait++;

        p_wk->val_scale = p_wk->data.boot_scale_x - scale;
        p_wk->val_scale /= p_wk->data.boot_move_f;
      }
      else if ( p_wk->wait == p_wk->data.boot_move_f + 1 ){
        x = p_wk->data.boot_init_x;
        scale = p_wk->data.boot_scale_x;
        p_wk->state = SEQ_END;
      }
      else {
				x += p_wk->val_x;
        scale += p_wk->val_scale;
        p_wk->wait++;
      }
			
      GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, x, p_wk->data.draw_type, CLSYS_MAT_X );
      GFL_CLACT_WK_SetTypeScale( p_wk->p_clwk, scale, CLSYS_MAT_X );
		}	
		break;
	
	case SEQ_END:
    BR_SIDEBAR_ONE_SetMoveMode( p_wk, BR_SIDEBAR_ONE_MOVE_MODE_NONE );
		break;
  }

}
