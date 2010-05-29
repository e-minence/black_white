//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn.c
 *	@brief	�o�g�����R�[�_�[�{�^���Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�����\��
#include "print/printsys.h"

//�����̃��W���[��
#include "br_util.h"
#include "br_btn_data.h"
#include "br_snd.h"

//�O�����J
#include "br_btn.h"

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_MOVESYNC_MODIFY
#endif //PM_DEBUG

//�萔���f�o�b�O����ŕύX�\�ɂ��邽�߂̃}�N��
#ifdef DEBUG_MOVESYNC_MODIFY
#define VALUE_CONST( name, value )  static int name = value;
#else
#define VALUE_CONST( name, value )  static const int name = value;
#endif //DEBUG_MOVESYNC_MODIFY

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���ړ��^�C�v
//=====================================
typedef enum
{
	BR_BTN_MOVE_HIDE,		//�����ꂽ�{�^���̊i�[���鏈��	target= need
	BR_BTN_MOVE_APPEAR,	//�{�^���o������								target= NULL
	BR_BTN_MOVE_UP_TAG,		  //�^�O�ʒu�֓o��                target= need
  BR_BTN_MOVE_DOWN_TAG,     //�^�O�ʒu����{�^���ʒu�։�����target= need
  BR_BTN_MOVE_NEXT_TARGET,  //�^�[�Q�b�g�̍��W�ւ���  
} BR_BTN_MOVE;

VALUE_CONST( BR_BTN_MOVE_DEFAULT_SYNC, 26 )
VALUE_CONST( BR_BTN_MOVE_HIDE_SYNC, 10 )
VALUE_CONST( BR_BTN_MOVE_APPEAR_SYNC, 16 )
VALUE_CONST( BR_BTN_MOVE_UPTAG_M_SYNC, 14 )
VALUE_CONST( BR_BTN_MOVE_UPTAG_S_SYNC, 14 )

//-------------------------------------
///	�{�^�����J���
//=====================================
typedef enum
{
	BR_BTN_PARAM_TYPE,				//�{�^���̃^�C�v	�iBR_BTN_TYPE�j
	BR_BTN_PARAM_DATA1,				//�^�C�v�ɂ�����f�[�^
	BR_BTN_PARAM_DATA2,				//�^�C�v�ɂ�����f�[�^
	BR_BTN_PARAM_VALID,				//�����邩�ǂ���
  BR_BTN_PARAM_MENUID,      //�����̃��j���[ID
  BR_BTN_PARAM_NONE_PROC,   //�����Ȃ��Ƃ��̓��� BR_BTN_NONEPROC��
  BR_BTN_PARAM_NONE_DATA,   //�����Ȃ��Ƃ��̓���̃f�[�^�iBR_BTN_NONEPROC�񋓂̃R�����g�Q�Ɓj
  BR_BTN_PARAM_UNIQUE,      //�ŗL�{�^��

  BR_BTN_PARAM_X,
  BR_BTN_PARAM_Y,
} BR_BTN_PARAM;

//-------------------------------------
///	
//=====================================
#define BR_BTN_OAMFONT_OFS_X	(4*8)
#define BR_BTN_OAMFONT_OFS_Y	(1*8)

//-------------------------------------
///	�{�^���Ǘ��V�X�e��
//=====================================
//���ʂɃ{�^�����������Ƃ��̍��W
//#define TAG_INIT_POS_UP( n )		( 25 + ( ( 36 ) * ( 4-n ) ) )
#define TAG_INIT_POS_UP( n )		( 169 - n * 16 )

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ėp�{�^�����[�N
//=====================================
struct _BR_BTN_WORK
{
	GFL_CLWK					*p_clwk;		//CLWK
	BMPOAM_ACT_PTR		p_oamfont;	//OAM�t�H���g
	GFL_BMP_DATA			*p_bmp;			//�t�H���g
	const BR_BTN_DATA *cp_data;		//�{�^���̃f�[�^
	CLSYS_DRAW_TYPE		display;		//�ǂ���ɕ`�悷�邩
  u32               w;          //��
};

//-------------------------------------
///	�{�^���V�X�e���ŊǗ������p�̃{�^�����[�N
//=====================================
typedef struct _BR_BTNEX_WORK BR_BTNEX_WORK;
typedef BOOL (*BR_BTNEX_MOVEFUNCTION)( BR_BTNEX_WORK *p_wk );
struct _BR_BTNEX_WORK
{
	BOOL							is_use;			//�g�p�t���O
  CLSYS_DRAW_TYPE   display;    //�ǂ���̉�ʂ�
	BR_BTN_WORK				*p_btn;			//�{�^�����[�N
	const BR_BTN_DATA *cp_data;		//�{�^���̃f�[�^
  BR_BTNEX_MOVEFUNCTION move_function; //�ړ��֐�
  GFL_POINT           target; //�ړ��̃^�[�Q�b�g
  u32                 move_seq;   //�ړ��̃V�[�P���X
  s16                 start;
  s16                 end;
  s16                 sync_now;
  s16                 sync_max;
  BR_RES_OBJID        use_resID;

  GFL_POINT           *p_follow_pos;

  //��ʐ؂�ւ��p�ɍ��Ȃ����ەK�v�ȃ����o
  const BR_RES_WORK   *cp_res;
  HEAPID              heapID;
  GFL_CLUNIT          *p_unit;
  BMPOAM_SYS_PTR		  p_bmpoam;
  STRBUF              *p_strbuf;
};

//-------------------------------------
///	�o�g�����R�[�_�[�@�{�^���Ǘ�
//=====================================
struct _BR_BTN_SYS_WORK
{
	HEAPID						heapID;       //�q�[�vID
	BR_RES_WORK				*p_res;		    //���\�[�X
	GFL_CLUNIT				*p_unit;	    //�Z�����j�b�g
  BR_SEQ_WORK       *p_seq;       //�V�[�P���X
  BR_RES_OBJID      use_resID;    //�{�^���Ɏg�p���郊�\�[�XID

	BR_BTN_SYS_STATE	state;	      //�{�^���Ǘ��̏��
	BR_BTN_SYS_INPUT	input;	      //�{�^�����͏��
	BR_BTN_TYPE				next_type;    //���̓���
	BOOL							next_valid;   //���̃{�^���������邩�ǂ���
	u32								next_proc;    //���̃v���Z�X
	u32								next_mode;    //���̃v���Z�X���[�h

	u32								trg_btn;			//�������{�^��
  GFL_POINT         ball_folow_pos; //�{�[�����ǂ����W

	u8								btn_now_max;	//���݂̃{�^���o�b�t�@�ő�
	u8								btn_now_num;	//���݂̃{�^����
	u8								btn_stack_max;//�{�^���X�^�b�N�o�b�t�@�ő�
	u8								btn_stack_num;//�{�^���X�^�b�N���ݐ�
	BR_BTNEX_WORK			*p_btn_stack;	//�X�^�b�N�ɐςރ{�^���o�b�t�@
	BR_BTNEX_WORK			*p_btn_now;		//���݂̃{�^���o�b�t�@

	BR_BTN_DATA_SYS		*p_btn_data;	//�{�^���f�[�^�o�b�t�@
	BMPOAM_SYS_PTR		p_bmpoam;	    //BMPOAM�V�X�e��
  PRINT_QUE         *p_que;       //�v�����g�L���[ 

  BR_TEXT_WORK      *p_text;      //�����\��

  BR_EXIT_TYPE      exit_type;    //������
  BR_BALLEFF_WORK   *p_balleff;

  BR_BTN_SYS_RECOVERY_DATA  *p_recovery;  //���A�f�[�^
};
//=============================================================================
/**
 *					PRIVATE�֐�
*/
//=============================================================================
//-------------------------------------
///	�{�^���V�X�e���̃v���C�x�[�g
//=====================================
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn, CLSYS_DRAW_TYPE display );
static BOOL Br_Btn_Sys_PopStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn );
static void Br_Btn_Sys_ReLoadBtn( BR_BTN_SYS_WORK *p_wk, BR_MENUID menuID, const GFL_POINT *cp_init_pos );

static void Br_Btn_Sys_PushRecoveryData( BR_BTN_SYS_RECOVERY_DATA *p_wk, u16 menuID, u16 btnID );
static void Br_Btn_Sys_PopRecoveryData( BR_BTN_SYS_RECOVERY_DATA *p_wk );

typedef enum
{ 
  BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_UP,
  BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_DOWN,
}BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE;
static void Br_Btn_Sys_ChangePalleteOffsetStack( BR_BTN_SYS_WORK *p_wk, BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE mode );

//-------------------------------------
///	SEQFUNC
//    ���̃{�^���ւ����Ƃ�
//      Touch -> HideBtn -> ChangePage -> UpTag -> AppearBtn
//
//    �߂�Ƃ�
//      Touch -> LeaveBtn -> DownTag -> ChangePage -> AppearBtn
//
//    ����PROC�ւ����Ƃ�
//      Touch -> HideBtn -> ChangePage -> UpTag
//
//=====================================
static void SEQFUNC_Start( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Touch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_HideBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_HideBtn2( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ChangePage( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_AppearBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_LeaveBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_UpTag( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_DownTag( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Decide( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_NotPushMessage( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_Open( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Close( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	BTN�̏���
//=====================================
static void BR_BTNEX_Init( BR_BTNEX_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, const BR_RES_WORK *cp_res, const STRBUF *cp_strbuf, BR_RES_OBJID use_resID, HEAPID heapID );
static void BR_BTNEX_Exit( BR_BTNEX_WORK *p_wk );
static BOOL BR_BTNEX_GetTrg( const BR_BTNEX_WORK *cp_wk, u32 x, u32 y );
static void BR_BTNEX_StartMove( BR_BTNEX_WORK *p_wk, BR_BTN_MOVE move, const GFL_POINT *cp_target );
static BOOL BR_BTNEX_MainMove( BR_BTNEX_WORK *p_wk );
static void BR_BTNEX_ChangeDisplay( BR_BTNEX_WORK *p_wk, CLSYS_DRAW_TYPE display );
static void BR_BTNEX_Copy( const BR_BTN_SYS_WORK *cp_wk, BR_BTNEX_WORK *p_dst, const BR_BTNEX_WORK *cp_src, CLSYS_DRAW_TYPE	display );
static void BR_BTNEX_SetStackPos( BR_BTNEX_WORK *p_wk, u16 stack_num );
static void BR_BTNEX_GetPos( const BR_BTNEX_WORK *cp_wk, GFL_POINT *p_pos );
static void BR_BTNEX_SetPosY( BR_BTNEX_WORK *p_wk, s16 y );
static u32 BR_BTNEX_GetParam( const BR_BTNEX_WORK *cp_wk, BR_BTN_PARAM param );
static void BR_BTNEX_SetSoftPriority( BR_BTNEX_WORK *p_wk, u16 soft_pri );
static void BR_BTNEX_SetBgPriority( BR_BTNEX_WORK *p_wk, u16 bg_pri );
static void BR_BTNEX_SetPalletOffset( BR_BTNEX_WORK *p_wk, u16 ofs );
static void BR_BTNEX_SetFollowPtr( BR_BTNEX_WORK *p_wk, GFL_POINT *p_follow_pos );

//typedef BOOL (*BR_BTNEX_MOVEFUNCTION)( BR_BTNEX_WORK *p_wk );
static BOOL Br_BtnEx_Move_Hide( BR_BTNEX_WORK *p_wk );
static BOOL Br_BtnEx_Move_Appear( BR_BTNEX_WORK *p_wk );
static BOOL Br_BtnEx_Move_UpTag( BR_BTNEX_WORK *p_wk );
static BOOL Br_BtnEx_Move_DownTag( BR_BTNEX_WORK *p_wk );
static BOOL Br_BtnEx_Move_NextTarget( BR_BTNEX_WORK *p_wk );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�^�O�̈ʒu
//=====================================
static const GFL_POINT  sc_tag_pos[]  =
{ 
  //��ԉ�
  { 
    BR_BTN_POS_X, TAG_INIT_POS_UP(0)
  },
  { 
    BR_BTN_POS_X, TAG_INIT_POS_UP(1)
  },
  { 
    BR_BTN_POS_X, TAG_INIT_POS_UP(2)
  },
  //��ԏ�
  { 
    BR_BTN_POS_X, TAG_INIT_POS_UP(3)
  },
};


//=============================================================================
/**
 *					PUBILIC�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�������
 *
 *  @param	mode						�N�����[�h
 *  @param	GFL_CLUNIT			���j�b�g
 *  @param	BR_RES_WORK			���\�[�X
 *  @param	BR_SAVE_INFO		�o�g���^����
 *  @param  BR_BTN_SYS_RECOVERY_DATA  ���A�f�[�^
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_WORK *BR_BTN_SYS_Init( BR_MENUID menuID, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, const BR_SAVE_INFO  *cp_saveinfo, BR_BTN_SYS_RECOVERY_DATA *p_recovery, BR_BALLEFF_WORK *p_balleff, HEAPID heapID )
{	
	BR_BTN_SYS_WORK *p_wk;

	GF_ASSERT( menuID < BR_MENUID_MAX );

	//���[�N�쐬
	{	
		p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_BTN_SYS_WORK) );
		GFL_STD_MemClear( p_wk, sizeof(BR_BTN_SYS_WORK) );
		p_wk->p_res				= p_res;
		p_wk->p_unit			= p_unit;
		p_wk->heapID			= heapID;
    p_wk->p_recovery  = p_recovery;
		p_wk->p_bmpoam		= BmpOam_Init( heapID, p_unit);
    p_wk->p_que       = PRINTSYS_QUE_Create( heapID );
    p_wk->p_balleff   = p_balleff;
    p_wk->state = BR_BTN_SYS_STATE_BTN_MOVE;

    {
      BR_BTN_DATA_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(BR_BTN_DATA_SETUP) );
      setup.cp_saveinfo = cp_saveinfo;
      p_wk->p_btn_data	= BR_BTN_DATA_SYS_Init( &setup, heapID );
    }
	}

	//���\�[�X�ǂݍ���
	{	
    if( menuID >= BR_MUSICAL_MENUID_TOP )
    {
      p_wk->use_resID = BR_RES_OBJ_MUSICAL_BTN_M;
    }
    else
    { 
      p_wk->use_resID = BR_RES_OBJ_BROWSE_BTN_M;
    }

    BR_RES_LoadOBJ( p_wk->p_res, p_wk->use_resID + CLSYS_DRAW_MAIN, heapID );
    BR_RES_LoadOBJ( p_wk->p_res, p_wk->use_resID + CLSYS_DRAW_SUB, heapID );
	}

	//�{�^���o�b�t�@���쐬
	{	
		u32 size;
		
		p_wk->btn_now_max	= BR_BTN_DATA_SYS_GetDataMax(p_wk->p_btn_data);
		size = sizeof(BR_BTNEX_WORK) * p_wk->btn_now_max;
		p_wk->p_btn_now	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_now, size );
	}

	//�X�^�b�N�o�b�t�@���쐬
	{	
		u32 size;

		p_wk->btn_stack_max	= BR_BTN_SYS_STACK_MAX;
		size	= sizeof(BR_BTNEX_WORK) * BR_BTN_SYS_STACK_MAX;
		p_wk->p_btn_stack	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_stack, size );
	}

  //���W���[���쐬
  p_wk->p_seq = BR_SEQ_Init( p_wk, SEQFUNC_Start, heapID );


	//�ŏ��ɓǂݍ��ރ{�^����������
  if( p_wk->p_recovery->stack_num != 0 )
  { 
    //�߂�̏ꍇ

    GFL_MSGDATA *p_msg		= BR_RES_GetMsgData( p_res );

    STRBUF  *p_strbuf;
    const BR_BTN_DATA *	cp_data;
    BR_BTNEX_WORK	btn;

    int i;
    //�X�^�b�N
    for( i = 0; i < p_wk->p_recovery->stack_num; i++ )
    { 
      cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, 
          p_wk->p_recovery->stack[ i ].menuID, 
          p_wk->p_recovery->stack[ i ].btnID );

      p_strbuf  = BR_BTN_DATA_CreateString( p_wk->p_btn_data, cp_data, p_msg, GFL_HEAP_LOWID(heapID) );

      GFL_STD_MemClear( &btn, sizeof(BR_BTNEX_WORK) );
      BR_BTNEX_Init( &btn, cp_data, p_unit, p_wk->p_bmpoam, p_res, p_strbuf, p_wk->use_resID, heapID );
      Br_Btn_Sys_PushStack( p_wk, &btn, CLSYS_DRAW_MAIN );

      GFL_STR_DeleteBuffer( p_strbuf );
    }
    //���W��ݒ�
    for( i = 0; i < p_wk->btn_stack_num; i++ )
    {	
      BR_BTNEX_SetStackPos( &p_wk->p_btn_stack[p_wk->btn_stack_num - i - 1], i );
    }

    //�㕔�ɂ���^�O�������鉉�o
    BR_SEQ_SetNext( p_wk->p_seq, SEQFUNC_DownTag );
    p_wk->next_type = BR_BTN_TYPE_RETURN;
  }
  else
  { 
    //�J�n�̏ꍇ
    int i;
    STRBUF  *p_strbuf;
    const BR_BTN_DATA *	cp_data;
    GFL_MSGDATA *p_msg;

    p_msg		= BR_RES_GetMsgData( p_res );
    p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( p_wk->p_btn_data, menuID );

    for( i = 0; i < p_wk->btn_now_num; i++ )
    {	
      cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, menuID, i );
      p_strbuf  = BR_BTN_DATA_CreateString( p_wk->p_btn_data, cp_data, p_msg, GFL_HEAP_LOWID(heapID) );

      BR_BTNEX_Init( &p_wk->p_btn_now[i], cp_data, p_unit, p_wk->p_bmpoam, p_res, p_strbuf, p_wk->use_resID, heapID );
      GFL_STR_DeleteBuffer( p_strbuf );
    }
    //���W��ݒ�
    for( i = 0; i < p_wk->btn_now_num; i++ )
    {	
      BR_BTNEX_SetPosY( &p_wk->p_btn_now[i], 192+32 );
    }

    BR_SEQ_SetNext( p_wk->p_seq, SEQFUNC_Open );
  }

#if 0
	{	
		//�ǂݍ��ރ{�^���̃����N�������m���A�r���̂��̂ł���΃X�^�b�N�ɐς�
		{	
			u32 num	= BR_BTN_DATA_SYS_GetLinkMax( p_wk->p_btn_data, menuID );

			if( num != 0 )
			{	
        //�q���肪����Ƃ�

				int i;
				const BR_BTN_DATA *	cp_data;
				BR_MENUID	preID;
				u32 btnID;
				BR_BTNEX_WORK	btn;
        STRBUF  *p_strbuf;
        GFL_MSGDATA *p_msg;

        p_msg		= BR_RES_GetMsgData( p_res );
				for( i = 0; i < num; i++ )
				{	
          GFL_STD_MemClear( &btn, sizeof(BR_BTNEX_WORK) );
					BR_BTN_DATA_SYS_GetLink( p_wk->p_btn_data, menuID, i, &preID, &btnID );

					cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, preID, btnID );
          p_strbuf  = BR_BTN_DATA_CreateString( p_wk->p_btn_data, cp_data, p_msg, GFL_HEAP_LOWID(heapID) );
					BR_BTNEX_Init( &btn, cp_data, p_unit, p_wk->p_bmpoam, p_res, p_strbuf, p_wk->use_resID, heapID );
					Br_Btn_Sys_PushStack( p_wk, &btn, CLSYS_DRAW_MAIN );

          GFL_STR_DeleteBuffer( p_strbuf );
				}
        //���W��ݒ�
        for( i = 0; i < p_wk->btn_stack_num; i++ )
        {	
          BR_BTNEX_SetStackPos( &p_wk->p_btn_stack[p_wk->btn_stack_num - i - 1], i );
        }
      }
      { 
        int i;
        STRBUF  *p_strbuf;
        const BR_BTN_DATA *	cp_data;
        GFL_MSGDATA *p_msg;

        p_msg		= BR_RES_GetMsgData( p_res );
        p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( p_wk->p_btn_data, menuID );

        for( i = 0; i < p_wk->btn_now_num; i++ )
        {	
          cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, menuID, i );
          p_strbuf  = BR_BTN_DATA_CreateString( p_wk->p_btn_data, cp_data, p_msg, GFL_HEAP_LOWID(heapID) );

          BR_BTNEX_Init( &p_wk->p_btn_now[i], cp_data, p_unit, p_wk->p_bmpoam, p_res, p_strbuf, p_wk->use_resID, heapID );
          GFL_STR_DeleteBuffer( p_strbuf );
        }

        
      }
		}
	}
#endif

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ��j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_SYS_Exit( BR_BTN_SYS_WORK *p_wk )
{	
  //���W���[���j��
  BR_SEQ_Exit( p_wk->p_seq );

  //�e�L�X�g�ʔj��
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_wk->p_res );
    p_wk->p_text  = NULL;
  }

	//�X�^�b�N�o�b�t�@�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_stack_num; i++ )
		{	
			BR_BTNEX_Exit( &p_wk->p_btn_stack[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_stack );

	//�{�^���o�b�t�@�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			BR_BTNEX_Exit( &p_wk->p_btn_now[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_now );
	
	//���\�[�X�j��
	{	
    BR_RES_UnLoadOBJ( p_wk->p_res, p_wk->use_resID + CLSYS_DRAW_MAIN );
    BR_RES_UnLoadOBJ( p_wk->p_res, p_wk->use_resID + CLSYS_DRAW_SUB );
	}

	BR_BTN_DATA_SYS_Exit( p_wk->p_btn_data );
	BmpOam_Exit( p_wk->p_bmpoam );
  PRINTSYS_QUE_Delete( p_wk->p_que );

	//���[�N�j��
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ����C������
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_SYS_Main( BR_BTN_SYS_WORK *p_wk )
{
  //�V�[�P���X
	BR_SEQ_Main( p_wk->p_seq );

  //�v�����g
  PRINTSYS_QUE_Main( p_wk->p_que );

  if( p_wk->p_text )
  { 
    BR_TEXT_PrintMain( p_wk->p_text );
  }


#ifdef DEBUG_MOVESYNC_MODIFY
  {
    static int s_modify_mode  = 0;
    static const char * scp_name_tbl[]  =
    {
      "�f�t�H���g",
      "�B���Ƃ�",
      "�o����",
      "�㏸����",
      "�㏸����",
    };
    int *p_value_tbl[]  =
    {
      &BR_BTN_MOVE_DEFAULT_SYNC,
      &BR_BTN_MOVE_HIDE_SYNC,
      &BR_BTN_MOVE_APPEAR_SYNC,
      &BR_BTN_MOVE_UPTAG_M_SYNC,
      &BR_BTN_MOVE_UPTAG_S_SYNC,
    };
    const int trg = GFL_UI_KEY_GetTrg();
    BOOL is_update  = FALSE;

    if( trg & PAD_BUTTON_L )
    {
      if( s_modify_mode > 0 )
      {
        s_modify_mode--;
      }
      is_update = TRUE;
    }
    else if( trg & PAD_BUTTON_R )
    {
      if( s_modify_mode < NELEMS(scp_name_tbl )-1 )
      {
        s_modify_mode++;
      }
      is_update = TRUE;
    }
    else if( trg & PAD_KEY_UP )
    {
      (*p_value_tbl[ s_modify_mode])++;
      is_update = TRUE;
    }
    else if( trg & PAD_KEY_DOWN )
    {
      (*p_value_tbl[ s_modify_mode])--;
      is_update = TRUE;
    }

    if( is_update )
    {
      OS_TPrintf( "%s=%d\n", scp_name_tbl[ s_modify_mode ], *p_value_tbl[ s_modify_mode] );
    }
  }
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�	���͏�Ԏ擾
 *
 *	@param	cp_wk	���[�N
 *	@param	p_seq	BR_BTN_SYS_INPUT_CHANGESEQ�̂Ƃ��A���̃V�[�P���X
 *	@param	p_param	BR_BTN_SYS_INPUT_CHANGESEQ�̂Ƃ��A���̃V�[�P���X�̃��[�h
 *
 *	@return	���͂̎��
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq, u32 *p_mode )
{	
	if( p_seq )
	{	
		*p_seq	= cp_wk->next_proc;
	}
	if( p_mode )
	{	
		*p_mode	= cp_wk->next_mode;
	}

	return cp_wk->input;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�	��Ԏ擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	���݂̏��
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_STATE BR_BTN_SYS_GetState( const BR_BTN_SYS_WORK *cp_wk )
{	
	return cp_wk->state;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�I���^�C�v���擾
 *
 *	@param	const BR_BTN_SYS_WORK *cp_wk  ���[�N
 *
 *	@return �I���^�C�v���擾
 */
//-----------------------------------------------------------------------------
BR_EXIT_TYPE BR_BTN_SYS_GetExitType( const BR_BTN_SYS_WORK *cp_wk )
{ 
  return cp_wk->exit_type;
}
//=============================================================================
/**
 *		�{�^���Ǘ��V�X�e���̃v���C�x�[�g
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���Ǘ��V�X�e���Ƀ{�^����ς�
 *
 *	@param	BR_BTN_SYS_WORK *p_wk		���[�N
 *	@param	*p_btn									�X�^�b�N�ɐςރ{�^��
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn, CLSYS_DRAW_TYPE display )
{	
	GF_ASSERT( p_wk->btn_stack_num < p_wk->btn_stack_max );

	//�ς�
	BR_BTNEX_Copy( p_wk, &p_wk->p_btn_stack[p_wk->btn_stack_num], p_btn, display );
	p_wk->btn_stack_num++;

  //�D��x��ݒ�
  {
    int i;
    for( i = 0; i < p_wk->btn_stack_num; i++ )
    { 
      //������OBJ�ƕ���OAM�̗����ɐݒ肵�Ă���̂� i*2+1
      BR_BTNEX_SetSoftPriority( &p_wk->p_btn_stack[i], (p_wk->btn_stack_num-i)*2+1 );

      //���b�Z�[�W�\�����邽��
      BR_BTNEX_SetBgPriority( &p_wk->p_btn_stack[i], 1 );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���Ǘ��V�X�e������{�^����j������
 *
 *	@param	BR_BTN_SYS_WORK *p_wk	���[�N
 *	@param	*p_btn								�X�^�b�N����󂯎��{�^��
 *
 *	@retval	TRUE�j�������@FALSE�擪�������̂ŉ������Ȃ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Btn_Sys_PopStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn )
{	
	if( p_wk->btn_stack_num != 0 )
	{	
		p_wk->btn_stack_num--;
		*p_btn	= p_wk->p_btn_stack[p_wk->btn_stack_num];
		GFL_STD_MemClear( &p_wk->p_btn_stack[p_wk->btn_stack_num], sizeof(BR_BTNEX_WORK) );

		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^����j�����A�ۓǂݍ��݂���
 *
 *	@param	BR_BTN_SYS_WORK *p_wk ���[�N
 *	@param	BR_MENUID							�ǂݍ��ރ{�^���̃��j���[ID
 *	@param  cp_init_pos           NULL�ŏ����l�A  �w�肷��ƁA���ׂẴ{�^�������̈ʒu�ɃZ�b�g
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_ReLoadBtn( BR_BTN_SYS_WORK *p_wk, BR_MENUID menuID, const GFL_POINT *cp_init_pos )
{	
	int i;
	for( i = 0; i < p_wk->btn_now_max; i++ )
	{	
		BR_BTNEX_Exit( &p_wk->p_btn_now[i] );
	}

	{	
		int i;
		const BR_BTN_DATA *	cp_data;
    STRBUF  *p_strbuf;
    GFL_MSGDATA *p_msg;

    p_msg		= BR_RES_GetMsgData( p_wk->p_res );

		p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( p_wk->p_btn_data, menuID );

		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, menuID, i );
      p_strbuf  = BR_BTN_DATA_CreateString( p_wk->p_btn_data, cp_data, p_msg, GFL_HEAP_LOWID(p_wk->heapID) );

			BR_BTNEX_Init( &p_wk->p_btn_now[i], cp_data, p_wk->p_unit, p_wk->p_bmpoam, p_wk->p_res, p_strbuf, p_wk->use_resID, p_wk->heapID );

      GFL_STR_DeleteBuffer( p_strbuf );

      //�D��x�ݒ�
      BR_BTNEX_SetSoftPriority( &p_wk->p_btn_now[i], 3 );

      if( cp_init_pos )
      { 
        BR_BTN_SetPos( p_wk->p_btn_now[i].p_btn, cp_init_pos->x, cp_init_pos->y );
      }
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A�f�[�^�ɐݒ�
 *
 *	@param	BR_BTN_SYS_RECOVERY_DATA *p_wk  ���A�f�[�^
 *	@param	menuID  ���j���[ID
 *	@param	btnID   �{�^��ID
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_PushRecoveryData( BR_BTN_SYS_RECOVERY_DATA *p_wk, u16 menuID, u16 btnID )
{ 
  GF_ASSERT( p_wk->stack_num < BR_BTN_SYS_STACK_MAX );

  p_wk->stack[ p_wk->stack_num ].menuID = menuID;
  p_wk->stack[ p_wk->stack_num ].btnID = btnID;
  p_wk->stack_num++;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���A�f�[�^���珜��
 *
 *	@param	BR_BTN_SYS_RECOVERY_DATA *p_wk ���A�f�[�^
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_PopRecoveryData( BR_BTN_SYS_RECOVERY_DATA *p_wk )
{ 
	if( p_wk->stack_num != 0 )
	{	
    p_wk->stack_num--;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�^�b�N�̃{�^�����ʒu�ɂ��p���b�g�I�t�Z�b�g��ς���
 *
 *	@param	BR_BTN_SYS_WORK *p_wk ���[�N
 *	@param  ���[�h
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_ChangePalleteOffsetStack( BR_BTN_SYS_WORK *p_wk, BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE mode )
{
  int i;
  BOOL is_on;
  for( i = 0; i < p_wk->btn_stack_num; i++ )
  {
    switch( mode )
    {
    case BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_UP:
      is_on = (i == p_wk->btn_stack_num - 1);
      break;
    case BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_DOWN:
      is_on = (i >= p_wk->btn_stack_num - 2);
      break;
    }

    if( is_on )
    { 
      BR_BTNEX_SetPalletOffset( &p_wk->p_btn_stack[i], 0 );
    }
    else
    { 
      BR_BTNEX_SetPalletOffset( &p_wk->p_btn_stack[i], 1 );
    }
  }
}

//=============================================================================
/**
 *  SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 

  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;
  BR_SEQ_SetNext( p_seqwk, SEQFUNC_Touch );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`���C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Touch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  int i;
  u32 x, y;
  BOOL is_trg	= FALSE;

  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  p_wk->input	= BR_BTN_SYS_INPUT_NONE;
  p_wk->state = BR_BTN_SYS_STATE_WAIT;

  //�{�^���������m
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  {	
    for( i = 0; i < p_wk->btn_now_max; i++ )
    {	
      if( BR_BTNEX_GetTrg( &p_wk->p_btn_now[i], x, y ) )
      {	
        //�����邩�ǂ���
        BOOL is_valid = BR_BTNEX_GetParam(&p_wk->p_btn_now[i],BR_BTN_PARAM_VALID );
        if( is_valid )
        { 
          if( p_wk->p_text )
          { 
            BR_TEXT_Exit( p_wk->p_text, p_wk->p_res );
            p_wk->p_text  = NULL;
          }
          PMSND_PlaySE( BR_SND_SE_OK );
          //������̂ł�����
          p_wk->trg_btn	= i;
          is_trg				= TRUE;
          {
            p_wk->ball_folow_pos.x  = x;
            p_wk->ball_folow_pos.y  = y;
            BR_BALLEFF_StartMove( p_wk->p_balleff, BR_BALLEFF_MOVE_EMIT_FOLLOW, &p_wk->ball_folow_pos );

            BR_BTNEX_SetFollowPtr( &p_wk->p_btn_now[p_wk->trg_btn], &p_wk->ball_folow_pos );
          }
          p_wk->exit_type = BR_EXIT_TYPE_RETURN;
          break;
        }
        else
        { 
          //�����Ȃ��Ƃ��̓���
          BR_BTN_NONEPROC proc  = BR_BTNEX_GetParam(&p_wk->p_btn_now[i],BR_BTN_PARAM_NONE_PROC );
          u16 proc_data  = BR_BTNEX_GetParam(&p_wk->p_btn_now[i],BR_BTN_PARAM_NONE_DATA );
          PMSND_PlaySE( BR_SND_SE_NG );
          switch( proc )
          { 
          case BR_BTN_NONEPROC_NOPUSH:
            p_wk->trg_btn	= i;
            BR_SEQ_SetNext( p_seqwk, SEQFUNC_NotPushMessage );
            break;
          }
          break;
        }
      }
    }
  }

  //�{�^������J�n
  if( is_trg )
  {	

    p_wk->state = BR_BTN_SYS_STATE_BTN_MOVE;

    //�������{�^���̏��
    p_wk->next_type	= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_TYPE );
    p_wk->next_valid= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_VALID );
    p_wk->next_proc	= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_DATA1 );
    p_wk->next_mode	= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_DATA2 );

    //�D��x�ݒ�
    {
      int i;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {
        if( i == p_wk->trg_btn )
        { 
          BR_BTNEX_SetSoftPriority( &p_wk->p_btn_now[i], 1 );
        }
        else
        { 
          BR_BTNEX_SetSoftPriority( &p_wk->p_btn_now[i], 3 );
        }
      }
    }

    switch( p_wk->next_type )
    {
    case BR_BTN_TYPE_RETURN:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_LeaveBtn );
      break;

    case BR_BTN_TYPE_SELECT_MSG:
      /* fallthrough */
    case BR_BTN_TYPE_SELECT:
      /* fallthrough */
    case BR_BTN_TYPE_CHANGESEQ:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_HideBtn );
      break;

    case BR_BTN_TYPE_EXIT:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_Close );
      break;

    default:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����B���
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_HideBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_HIDE_START,   //�{�^�����B��鏈��
    SEQ_HIDE_WAIT,    //�{�^�����B���ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

 switch( *p_seq )
  { 
  case SEQ_HIDE_START:
    //�{�^�����B��铮��J�n
    { 
      int i;
      GFL_POINT pos;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        if( p_wk->trg_btn != i )
        {	
          BR_BTNEX_GetPos( &p_wk->p_btn_now[p_wk->trg_btn], &pos );
          BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_HIDE, &pos );
        }
      }
    }
    *p_seq  = SEQ_HIDE_WAIT;
    break;
  
  case SEQ_HIDE_WAIT:
    //�{�^�����B���܂ł̓���
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;

  case SEQ_END:

    switch( p_wk->next_type )
    {
    case BR_BTN_TYPE_CHANGESEQ:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_ChangePage );
      break;

    default:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_ChangePage );
      break;
    }
    break;
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����B���  �^�[�Q�b�g�͍~��Ă�������
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_HideBtn2( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_HIDE_START,   //�{�^�����B��鏈��
    SEQ_HIDE_WAIT,    //�{�^�����B���ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

 switch( *p_seq )
  { 
  case SEQ_HIDE_START:
    //�{�^�����B��铮��J�n
    { 
      int i;
      GFL_POINT pos;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        pos.x    = BR_BTN_DATA_GetParam( p_wk->p_btn_stack[p_wk->btn_stack_num - 1].cp_data, BR_BTN_DATA_PARAM_X );
        pos.y    = BR_BTN_DATA_GetParam( p_wk->p_btn_stack[p_wk->btn_stack_num - 1].cp_data, BR_BTN_DATA_PARAM_Y );
        BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_HIDE, &pos );
      }
    }
    *p_seq  = SEQ_HIDE_WAIT;
    break;
  
  case SEQ_HIDE_WAIT:
    //�{�^�����B���܂ł̓���
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;

  case SEQ_END:

    switch( p_wk->next_type )
    {
    case BR_BTN_TYPE_CHANGESEQ:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_ChangePage );
      break;

    default:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_ChangePage );
      break;
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�y�[�W�؂�ւ����o
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_ChangePage( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  //�ǂݑւ�
  if( p_wk->next_type == BR_BTN_TYPE_SELECT || p_wk->next_type == BR_BTN_TYPE_SELECT_MSG )
  {
    //���֍s���ꍇ�A�������{�^�����玟�ւ̏��𓾂�
    u32 nextID					= p_wk->next_proc; 
    GFL_POINT pos;

    //�����O�ɍ��W�����炤
    BR_BTNEX_GetPos( &p_wk->p_btn_now[p_wk->trg_btn], &pos );

    //���肵���{�^�����X�^�b�N�ɐς�
    Br_Btn_Sys_PushStack( p_wk, &p_wk->p_btn_now[p_wk->trg_btn], CLSYS_DRAW_SUB );
    Br_Btn_Sys_PushRecoveryData( p_wk->p_recovery, 
        BR_BTNEX_GetParam( &p_wk->p_btn_now[p_wk->trg_btn], BR_BTN_PARAM_MENUID ),
        p_wk->trg_btn );

    //�ς񂾂̂ŁA���̏ꏊ�̏�������(�R�s�[���Ă�̂ŊJ���͂��Ȃ�)
    GFL_STD_MemClear( &p_wk->p_btn_now[p_wk->trg_btn], sizeof(BR_BTNEX_WORK) );

    //���̃{�^����ǂݕς���
    pos.y = -32;
    Br_Btn_Sys_ReLoadBtn( p_wk, nextID, &pos );

    BR_SEQ_SetNext( p_seqwk, SEQFUNC_UpTag );
  }
  else if( p_wk->next_type == BR_BTN_TYPE_RETURN )
  { 
    //�߂�ꍇ�A�^�O���玟�ւ̏��𓾂�
    BR_BTNEX_WORK	btn;

    Br_Btn_Sys_PopRecoveryData( p_wk->p_recovery );
    if( Br_Btn_Sys_PopStack( p_wk, &btn ) )
    {	
      u32 menuID  = BR_BTNEX_GetParam( &btn, BR_BTN_PARAM_MENUID );

      GFL_POINT pos;
      GF_ASSERT( btn.is_use );
      //
      pos.x    = BR_BTN_DATA_GetParam( btn.cp_data, BR_BTN_DATA_PARAM_X );
      pos.y    = BR_BTN_DATA_GetParam( btn.cp_data, BR_BTN_DATA_PARAM_Y );

      //���̃{�^����ǂݕς���
      Br_Btn_Sys_ReLoadBtn( p_wk, menuID, &pos );

      //�����Ɠ����{�^���͗D��x����������
      {
        int i;
        u32 y = BR_BTN_DATA_GetParam( btn.cp_data, BR_BTN_DATA_PARAM_Y );
        for( i = 0; i < p_wk->btn_now_num; i++ )
        { 
          if( y == BR_BTN_DATA_GetParam( p_wk->p_btn_now[i].cp_data, BR_BTN_DATA_PARAM_Y ) )
          { 
            BR_BTNEX_SetSoftPriority( &p_wk->p_btn_now[i], 1 );
          }
        }
      }

      //�폜
      BR_BTNEX_Exit( &btn );

      BR_SEQ_SetNext( p_seqwk, SEQFUNC_AppearBtn );
    }
  } 
  else
  { 
    //���肵���{�^�����X�^�b�N�ɐς�
    Br_Btn_Sys_PushStack( p_wk, &p_wk->p_btn_now[p_wk->trg_btn], CLSYS_DRAW_SUB );
    Br_Btn_Sys_PushRecoveryData( p_wk->p_recovery, 
        BR_BTNEX_GetParam( &p_wk->p_btn_now[p_wk->trg_btn], BR_BTN_PARAM_MENUID ),
        p_wk->trg_btn );

    //�ς񂾂̂ŁA���̏ꏊ�̏�������(�R�s�[���Ă�̂ŊJ���͂��Ȃ�)
    GFL_STD_MemClear( &p_wk->p_btn_now[p_wk->trg_btn], sizeof(BR_BTNEX_WORK) );

    //���̃{�^��������
    { 
      int i;
			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				BR_BTNEX_Exit( &p_wk->p_btn_now[i] );
			}

    }


    BR_SEQ_SetNext( p_seqwk, SEQFUNC_UpTag );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_AppearBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_APPEAR_START, //�{�^���o������
    SEQ_APPEAR_WAIT,  //�{�^���o���ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_APPEAR_START:
    //�{�^���o������
    {	
      int i;
      //�{�^���o������
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_APPEAR, NULL );
      }

      *p_seq  = SEQ_APPEAR_WAIT;
    }
    break;

  case SEQ_APPEAR_WAIT:
    //�{�^���o���ҋ@����
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}
			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;
    
  case SEQ_END:

    switch( p_wk->next_type )
    {
    case BR_BTN_TYPE_SELECT_MSG:
      /* fallthrough */
    case BR_BTN_TYPE_SELECT:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
      break;

    case BR_BTN_TYPE_CHANGESEQ:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_UpTag ); 
      break;

    default:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
      break;
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�������֋���
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_LeaveBtn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_LEAVE_START,   //�{�^���������Ă����J�n
    SEQ_LEAVE_WAIT,    //�{�^���������Ă����ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

 switch( *p_seq )
  { 
  case SEQ_LEAVE_START:
    //�{�^�������铮��J�n
    { 
      int i;
      GFL_POINT pos;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        BR_BTNEX_GetPos( &p_wk->p_btn_now[i], &pos );
        pos.y = 192 + 32;
        BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_NEXT_TARGET, &pos );
      }
    }
    *p_seq  = SEQ_LEAVE_WAIT;
    break;
  
  case SEQ_LEAVE_WAIT:
    //�{�^��������܂ł̓���
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;

  case SEQ_END:

    BR_SEQ_SetNext( p_seqwk, SEQFUNC_DownTag );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�O�����
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_UpTag( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_UP_START, //�{�^���o������
    SEQ_UP_WAIT,  //�{�^���o���ҋ@
    SEQ_MSG_PRINT,  //�e�L�X�g�\���i�{�^���ɂ��j
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_UP_START:
    //�{�^���o������
    {	
      int i;
      //�X�^�b�N�ɐς񂾁A����{�^���͏㕔�ֈړ�
			for( i = 0; i < p_wk->btn_stack_num; i++ )
      { 
        if( i == p_wk->btn_stack_num - 1 )
        {	
          BR_BTNEX_StartMove( &p_wk->p_btn_stack[ i ], BR_BTN_MOVE_UP_TAG, &sc_tag_pos[0] );
        }
        else
        { 
          BR_BTNEX_StartMove( &p_wk->p_btn_stack[ i ], BR_BTN_MOVE_NEXT_TARGET, &sc_tag_pos[p_wk->btn_stack_num-i-1] );
        }
      }

      *p_seq  = SEQ_UP_WAIT;
    }
    break;

  case SEQ_UP_WAIT:
    //�{�^���o���ҋ@����
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_stack_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_stack[i] );
			}
			if( is_end )
			{
        Br_Btn_Sys_ChangePalleteOffsetStack( p_wk, BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_UP );
        *p_seq  = SEQ_MSG_PRINT;
			}
		}
    break;
    
  case SEQ_MSG_PRINT:  //�e�L�X�g�\���i�{�^���ɂ��j
    { 
      const BR_BTNEX_WORK *cp_target_btn  = &p_wk->p_btn_stack[ p_wk->btn_stack_num - 1 ];
      BR_BTN_TYPE  type = BR_BTNEX_GetParam( cp_target_btn, BR_BTN_PARAM_TYPE );
      u16   strID       = BR_BTNEX_GetParam( cp_target_btn, BR_BTN_PARAM_DATA2 );
      if( type == BR_BTN_TYPE_SELECT_MSG )
      { 
        if( p_wk->p_text == NULL )
        { 
          p_wk->p_text  = BR_TEXT_Init( p_wk->p_res, p_wk->p_que, p_wk->heapID );
        }
        BR_TEXT_Print( p_wk->p_text, p_wk->p_res, strID );
      }
    }

    *p_seq  = SEQ_END;
    break;

  case SEQ_END:
    switch( p_wk->next_type )
    {
    case BR_BTN_TYPE_SELECT_MSG:
      /* fallthrough */
    case BR_BTN_TYPE_SELECT:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_AppearBtn ); 
      break;
    default:
      BR_SEQ_SetNext( p_seqwk, SEQFUNC_Decide ); 
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�O�����Ɉړ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DownTag( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_DOWN_START, //�{�^�����~�J�n
    SEQ_DOWN_WAIT,  //�{�^�����~�ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_DOWN_START:
    //�{�^���o������
    {	
      int i;
      GFL_POINT pos;
      //�X�^�b�N�ɐς񂾈�Ԃ܂��̃{�^���͉����ֈړ�
      for( i = 0; i < p_wk->btn_stack_num; i++ )
      { 
        if( i == p_wk->btn_stack_num - 1  )
        {	
          pos.x = BR_BTN_DATA_GetParam( p_wk->p_btn_stack[i].cp_data, BR_BTN_DATA_PARAM_X );
          pos.y = BR_BTN_DATA_GetParam( p_wk->p_btn_stack[i].cp_data, BR_BTN_DATA_PARAM_Y );
          BR_BTNEX_StartMove( &p_wk->p_btn_stack[i], BR_BTN_MOVE_DOWN_TAG, &pos );
          BR_BTNEX_SetSoftPriority( &p_wk->p_btn_stack[i], 1 );
        }
        else
        { 
          BR_BTNEX_StartMove( &p_wk->p_btn_stack[i], BR_BTN_MOVE_NEXT_TARGET, &sc_tag_pos[p_wk->btn_stack_num-i-2] );
        }
      }

      //�D��x�ݒ�
      {
        int i;
        for( i = 0; i < p_wk->btn_now_num; i++ )
        {
          BR_BTNEX_SetSoftPriority( &p_wk->p_btn_now[i], 3 );
        }
      }

      *p_seq  = SEQ_DOWN_WAIT;
    }
    break;

  case SEQ_DOWN_WAIT:
    //�{�^���o���ҋ@����
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_stack_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_stack[i] );
			}
			if( is_end )
			{
        Br_Btn_Sys_ChangePalleteOffsetStack( p_wk, BR_BTN_SYS_CHANGE_PALLETE_OFFSET_STACK_MODE_DOWN );
        *p_seq  = SEQ_END;
			}
		}
    break;
    
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, SEQFUNC_ChangePage ); 
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���͌���
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Decide( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_END:
    if( p_wk->next_valid )
		{	
      switch( p_wk->next_type )
      {	
      case BR_BTN_TYPE_SELECT_MSG:
      /* fallthrough */
      case BR_BTN_TYPE_SELECT:				//�I��p�{�^��
        /* fallthrough */
      case BR_BTN_TYPE_RETURN:				//1�O�̃��j���[�֖߂�p�{�^��
        break;

      case BR_BTN_TYPE_EXIT:					//�o�g�����R�[�_�[�I���p�{�^��
        p_wk->input	= BR_BTN_SYS_INPUT_EXIT;
        break;

			case BR_BTN_TYPE_CHANGESEQ:		//�V�[�P���X�ύX�{�^��			
        p_wk->input	= BR_BTN_SYS_INPUT_CHANGESEQ;
        break;
      }
    }

    BR_SEQ_SetNext( p_seqwk, SEQFUNC_Touch ); 
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����Ȃ����b�Z�[�W�\��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					  �V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_NotPushMessage( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_TOUCH,
    SEQ_END,
  };

  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      u32 strID = BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_NONE_DATA );

      if( p_wk->p_text == NULL )
      { 
          p_wk->p_text  = BR_TEXT_Init( p_wk->p_res, p_wk->p_que, p_wk->heapID );
      }
      BR_TEXT_Print( p_wk->p_text, p_wk->p_res, strID );
    }
    *p_seq  = SEQ_TOUCH;
    break;

  case SEQ_TOUCH:
    *p_seq  = SEQ_END;
    break;

  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, SEQFUNC_Touch );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					  �V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Open( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_OPEN_START,   //�{�^���������Ă����J�n
    SEQ_OPEN_WAIT,    //�{�^���������Ă����ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_OPEN_START:
    //�{�^�����o�铮��J�n
    { 
      int i;
      GFL_POINT pos;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        pos.x    = BR_BTNEX_GetParam( &p_wk->p_btn_now[i], BR_BTN_PARAM_X );
        pos.y    = BR_BTNEX_GetParam( &p_wk->p_btn_now[i], BR_BTN_PARAM_Y );
        BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_NEXT_TARGET, &pos );
      }
    }
    *p_seq  = SEQ_OPEN_WAIT;
    break;
  
  case SEQ_OPEN_WAIT:
    //�{�^�����o��܂ł̓���
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;

  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, SEQFUNC_Start );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					  �V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Close( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CLOSE_START,   //�{�^���������Ă����J�n
    SEQ_CLOSE_WAIT,    //�{�^���������Ă����ҋ@
    SEQ_END,
  };
  BR_BTN_SYS_WORK *p_wk = p_wk_adrs;

 switch( *p_seq )
  { 
  case SEQ_CLOSE_START:
    //�{�^�������铮��J�n
    { 
      int i;
      GFL_POINT pos;
      for( i = 0; i < p_wk->btn_now_num; i++ )
      {	
        BR_BTNEX_GetPos( &p_wk->p_btn_now[i], &pos );
        pos.y = 192 + 32;
        BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_NEXT_TARGET, &pos );
      }
    }
    *p_seq  = SEQ_CLOSE_WAIT;
    break;
  
  case SEQ_CLOSE_WAIT:
    //�{�^��������܂ł̓���
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
        *p_seq  = SEQ_END;
			}
		}
    break;

  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
    break;
  }
}
//=============================================================================
/**
 *		�{�^���P�P�̏���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������
 *
 *	@param	BR_BTNEX_WORK *p_wk			���[�N
 *	@param	BR_BTN_DATA *cp_data	�{�^�����
 *	@param	*p_unit								OBJ�쐬�p���j�b�g
 *	@param	*p_bmpoam							BMPOAM�V�X�e��
 *	@param	p_res									���\�[�X
 *	@param  use_resID             �g�pOBJ�ԍ�
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Init( BR_BTNEX_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, const BR_RES_WORK *cp_res, const STRBUF *cp_strbuf, BR_RES_OBJID use_resID, HEAPID heapID )
{	
	u32 plt;

	GF_ASSERT( p_wk->is_use == FALSE );

	GFL_STD_MemClear( p_wk, sizeof(BR_BTNEX_WORK) );
	p_wk->is_use	= TRUE;
	p_wk->cp_data	= cp_data;
  p_wk->display = CLSYS_DRAW_SUB;

  p_wk->use_resID = use_resID;
  p_wk->cp_res  = cp_res;
  p_wk->heapID  = heapID;
  p_wk->p_unit  = p_unit;
  p_wk->p_bmpoam  = p_bmpoam;
  p_wk->p_strbuf = GFL_STR_CreateCopyBuffer( cp_strbuf, heapID );

	//�{�^���쐬
	{	
		BR_RES_OBJ_DATA	res;
		GFL_FONT *p_font;
		GFL_CLWK_DATA	cldata;
    STRBUF  *p_strbuf;

    //�f�[�^�ݒ�
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x    = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_X );
    cldata.pos_y    = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_Y );
    cldata.anmseq   = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_ANMSEQ );
    cldata.softpri  = 1;

		//���\�[�X�ǂݍ���()
		BR_RES_GetOBJRes( cp_res, p_wk->use_resID + CLSYS_DRAW_SUB, &res );
		p_font	= BR_RES_GetFont( cp_res );

		//�쐬
		p_wk->p_btn		= BR_BTN_InitEx( &cldata, p_wk->p_strbuf, BR_BTN_DATA_WIDTH, p_wk->display, p_unit, 
				p_bmpoam, p_font, &res, GetHeapLowID( heapID ) );

	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BR_BTNEX_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Exit( BR_BTNEX_WORK *p_wk )
{	
	if( p_wk->is_use )
	{	
    GFL_STR_DeleteBuffer( p_wk->p_strbuf );
    BR_BTN_Exit( p_wk->p_btn );
		GFL_STD_MemClear( p_wk, sizeof(BR_BTNEX_WORK) );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����^�b�`�������ǂ���
 *
 *	@param	const BR_BTNEX_WORK *cp_wk	���[�N
 *	@param	�ʒu���W
 *
 *	@return	TRUE�Ȃ�Ή�����	FALSE�Ȃ�Ή����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTNEX_GetTrg( const BR_BTNEX_WORK *cp_wk, u32 x, u32 y )
{	
  
  if( !cp_wk->is_use )
	{	
		return FALSE;
	}

  return BR_BTN_GetHit( cp_wk->p_btn, x, y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̓���J�n
 *
 *	@param	BR_BTNEX_WORK *p_wk				���[�N
 *	@param	move										����
 *	@param	BR_BTNEX_WORK *cp_target	����ɕK�v�ȃ^�[�Q���g
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_StartMove( BR_BTNEX_WORK *p_wk, BR_BTN_MOVE move, const GFL_POINT *cp_target )
{	
  p_wk->move_seq  = 0;

  if( cp_target )
  { 
    p_wk->target = *cp_target;
  }

  switch( move )
  { 
  case BR_BTN_MOVE_HIDE:
    p_wk->move_function = Br_BtnEx_Move_Hide;
    break;

  case BR_BTN_MOVE_APPEAR:
    p_wk->move_function = Br_BtnEx_Move_Appear;
    break;

  case BR_BTN_MOVE_UP_TAG:
    p_wk->move_function = Br_BtnEx_Move_UpTag;
    break;

  case BR_BTN_MOVE_DOWN_TAG:
    p_wk->move_function = Br_BtnEx_Move_DownTag;
    break;
    
  case BR_BTN_MOVE_NEXT_TARGET:
    p_wk->move_function = Br_BtnEx_Move_NextTarget;
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̃��C������
 *
 *	@param	BR_BTNEX_WORK *p_wk		���[�N
 *
 *	@return	TRUE�Ȃ�΃{�^���̓����I��	FALSE�Ȃ�Γ�����
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTNEX_MainMove( BR_BTNEX_WORK *p_wk )
{	
  if( p_wk->move_function )
  { 
    BOOL ret;
    ret = p_wk->move_function( p_wk );

    //�I��������ړ��֐������
    if( ret )
    { 
      p_wk->move_function = NULL;
    }

    //�ǔ��p�̕ϐ��֍��W��ݒ肷��
    if( p_wk->p_follow_pos )
    { 
      s16 x, y;
      BR_BTN_GetPos( p_wk->p_btn, &x, &y );

      p_wk->p_follow_pos->x = x + BR_BTN_DATA_WIDTH/2;
      p_wk->p_follow_pos->y = y;
    }

    return ret;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���̃f�B�X�v���C��؂�ւ�
 *
 *	@param	BR_BTNEX_WORK *cp_wk  ���[�N
 *	@param	�ǂ���̉�ʂɐ؂�ւ��邩
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_ChangeDisplay( BR_BTNEX_WORK *p_wk, CLSYS_DRAW_TYPE display )
{ 
  if( p_wk->display != display )
  { 
    s16 x, y;
    u16 soft_pri, bg_pri;

    p_wk->display = display;

    //�V������蒼�����߁A����
		{	
      BR_BTN_GetPos( p_wk->p_btn, &x, &y );
      soft_pri  = BR_BTN_GetSoftPriority( p_wk->p_btn );
      bg_pri    = BR_BTN_GetBgPriority( p_wk->p_btn );
			BR_BTN_Exit( p_wk->p_btn );
		}

    //����Ȃ��f�[�^��j��
    {
      p_wk->p_follow_pos  = NULL;
    }

    //�{�^���쐬
    {	
      BR_RES_OBJ_DATA	res;
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg;
      GFL_CLWK_DATA	cldata;
      u32 msgID;

      s16 y_ofs;
      if( display == CLSYS_DRAW_MAIN )
      { 
        y_ofs = +512;
      }
      else
      { 

        y_ofs = -512;
      }

      //�f�[�^�ݒ�
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x    = x;
      cldata.pos_y    = y + y_ofs;
      cldata.anmseq   = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_ANMSEQ );
      cldata.softpri  = soft_pri;
      cldata.bgpri    = bg_pri;

      //���\�[�X�ǂݍ���
      BR_RES_GetOBJRes( p_wk->cp_res, p_wk->use_resID + display, &res );
      p_font	= BR_RES_GetFont( p_wk->cp_res );
      p_msg		= BR_RES_GetMsgData( p_wk->cp_res );
      msgID		= BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_MSGID );


      //�쐬
      p_wk->p_btn		= BR_BTN_InitEx( &cldata, p_wk->p_strbuf, BR_BTN_DATA_WIDTH, display, p_wk->p_unit, 
          p_wk->p_bmpoam, p_font, &res, p_wk->heapID );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����R�s�[
 *
 *	@param	p_wk		���[�N					
 *	@param	p_dst		�R�s�[��@�T�u��ʂ̃{�^��
 *	@param	p_src		�R�s�[��	���C����ʂ̃{�^��
 *	@param	display	�ǂ���̉�ʂɃR�s�[���邩
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Copy( const BR_BTN_SYS_WORK *cp_wk, BR_BTNEX_WORK *p_dst, const BR_BTNEX_WORK *cp_src, CLSYS_DRAW_TYPE display )
{
	u32 plt;	

	GF_ASSERT( p_dst->is_use == FALSE );
	GF_ASSERT( cp_src->is_use == TRUE );
		
	//��U�R�s�[
	{
    *p_dst  = *cp_src;
	}

	//�Ⴄ�Ȃ�΍�蒼��
	if( cp_src->display	!= display )
	{	
    BR_BTNEX_ChangeDisplay( p_dst, display );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̍��W��ݒ�
 *
 *	@param	BR_BTNEX_WORK *p_wk	���[�N
 *	@param	x									X���W
 *	@param	y									Y���W
 *
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetStackPos( BR_BTNEX_WORK *p_wk, u16 stack_num )
{	
	GFL_CLACTPOS	pos;
	pos.x	= BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_X );
	pos.y	= TAG_INIT_POS_UP( stack_num );

  BR_BTN_SetPos( p_wk->p_btn, pos.x, pos.y );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���̈ʒu���擾
 *
 *	@param	const BR_BTNEX_WORK *cp_wk  ���[�N
 *	@param	*p_pos ���W
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_GetPos( const BR_BTNEX_WORK *cp_wk, GFL_POINT *p_pos )
{ 
  s16 x, y;
  BR_BTN_GetPos( cp_wk->p_btn, &x, &y );
  p_pos->x  = x;
  p_pos->y  = y;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^����Y�ʒu��ݒ�
 *
 *	@param	const BR_BTNEX_WORK *cp_wk  ���[�N
 *	@param	y Y���W
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetPosY( BR_BTNEX_WORK *p_wk, s16 y )
{ 
  s16 x;
	x	= BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_X );
  BR_BTN_SetPos( p_wk->p_btn, x, y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̏����󂯎��
 *
 *	@param	const BR_BTNEX_WORK *cp_wk	���[�N
 *	@param	param		�{�^���̏��C���f�b�N�X
 *
 *	@return	�{�^�����
 */
//-----------------------------------------------------------------------------
static u32 BR_BTNEX_GetParam( const BR_BTNEX_WORK *cp_wk, BR_BTN_PARAM param )
{	
	u32 ret;
	switch( param )
	{	
	case BR_BTN_PARAM_TYPE:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_TYPE );
		break;

	case BR_BTN_PARAM_DATA1:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_DATA1 );
		break;

	case BR_BTN_PARAM_DATA2:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_DATA2 );
		break;


  case BR_BTN_PARAM_NONE_PROC:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_NONE_PROC );
    break;

  case BR_BTN_PARAM_NONE_DATA:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_NONE_DATA );
    break;
	
	case BR_BTN_PARAM_VALID:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_VALID );
		break;

	case BR_BTN_PARAM_MENUID:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_MENUID );
		break;

  case BR_BTN_PARAM_UNIQUE:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_UNIQUE );
    break;

  case BR_BTN_PARAM_X:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_X );
    break;

  case BR_BTN_PARAM_Y:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_Y );
    break;

	default:
		ret	= 0;
		GF_ASSERT(0);
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v���C�I���e�B�ݒ�
 *
 *	@param	BR_BTNEX_WORK *p_wk ���[�N
 *	@param	soft_pri            �D��x
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetSoftPriority( BR_BTNEX_WORK *p_wk, u16 soft_pri )
{ 
  BR_BTN_SetSoftPriority( p_wk->p_btn, soft_pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BG�v���C�I���e�B�ݒ�
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *	@param	bg_pri                BG�D��x
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetBgPriority( BR_BTNEX_WORK *p_wk, u16 bg_pri )
{ 
  BR_BTN_SetBgPriority( p_wk->p_btn, bg_pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�ݒ�
 *
 *	@param	BR_BTNEX_WORK *p_wk ���[�N  
 *	@param	ofs                 �p���b�g�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetPalletOffset( BR_BTNEX_WORK *p_wk, u16 ofs )
{ 
  BR_BTN_SetPalleteOffset( p_wk->p_btn, ofs );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ǔ����邽�߂̍��W���i�[����ϐ���ݒ肷��
 *
 *	@param	BR_BTNEX_WORK *p_wk ���[�N
 *	@param	*p_follow_pos       �ǔ����W�i�[�ϐ�
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetFollowPtr( BR_BTNEX_WORK *p_wk, GFL_POINT *p_follow_pos )
{ 
  p_wk->p_follow_pos  = p_follow_pos;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �i�[����ړ�
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtnEx_Move_Hide( BR_BTNEX_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  switch( p_wk->move_seq )
  { 
  case SEQ_INIT:
    {
      GFL_POINT pos;

      BR_BTNEX_GetPos( p_wk, &pos );
      p_wk->start       = pos.y;
      p_wk->end         = p_wk->target.y;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_HIDE_SYNC;
    }
    p_wk->move_seq  = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o������
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtnEx_Move_Appear( BR_BTNEX_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  switch( p_wk->move_seq )
  { 
  case SEQ_INIT:
    {
      GFL_POINT pos;
      u16 end_y;

      BR_BTNEX_GetPos( p_wk, &pos );
      end_y  = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_Y ); 

      p_wk->start       = pos.y;
      p_wk->end         = end_y;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_APPEAR_SYNC;
    }
    p_wk->move_seq  = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    { 
    }
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^�O�����
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtnEx_Move_UpTag( BR_BTNEX_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT_S,
    SEQ_MAIN_S,
    SEQ_CHANGE,
    SEQ_INIT_M,
    SEQ_MAIN_M,
    SEQ_EXIT,
  };

  switch( p_wk->move_seq )
  { 
  case SEQ_INIT_S:
    {
      GFL_POINT pos;

      BR_BTNEX_GetPos( p_wk, &pos );
      //end_y  = p_wk->target.y;

      p_wk->start       = pos.y;
      p_wk->end         = -BR_BTN_DATA_HEIGHT;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_UPTAG_M_SYNC;

    }
    p_wk->move_seq  = SEQ_MAIN_S;
    break;

  case SEQ_MAIN_S:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_CHANGE;
      }
    }
    break;

  case SEQ_CHANGE:
    { 
      BR_BTNEX_ChangeDisplay( p_wk, CLSYS_DRAW_MAIN );
      p_wk->move_seq  = SEQ_INIT_M;
    }
    break;

  case SEQ_INIT_M:
    {
      GFL_POINT pos;
      BR_BTNEX_GetPos( p_wk, &pos );

      p_wk->start       = pos.y;
      p_wk->end         = p_wk->target.y;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_UPTAG_M_SYNC;

    }
    p_wk->move_seq  = SEQ_MAIN_M;
    break;

  case SEQ_MAIN_M:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    { 
    }
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^�O������
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtnEx_Move_DownTag( BR_BTNEX_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT_M,
    SEQ_MAIN_M,
    SEQ_CHANGE,
    SEQ_INIT_S,
    SEQ_MAIN_S,
    SEQ_EXIT,
  };

  switch( p_wk->move_seq )
  { 
  case SEQ_INIT_M:
    {
      GFL_POINT pos;
      BR_BTNEX_GetPos( p_wk, &pos );

      p_wk->start       = pos.y;
      p_wk->end         = 192+BR_BTN_DATA_HEIGHT;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_UPTAG_M_SYNC;

    }
    p_wk->move_seq  = SEQ_MAIN_M;
    break;

  case SEQ_MAIN_M:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_CHANGE;
      }
    }
    break;

  case SEQ_CHANGE:
    { 
      BR_BTNEX_ChangeDisplay( p_wk, CLSYS_DRAW_SUB );
      p_wk->move_seq  = SEQ_INIT_S;
    }
    break;

  case SEQ_INIT_S:
    { 
      GFL_POINT pos;

      BR_BTNEX_GetPos( p_wk, &pos );

      p_wk->start       = pos.y;
      p_wk->end         = p_wk->target.y;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_UPTAG_M_SYNC;

    }
    p_wk->move_seq  = SEQ_MAIN_S;
    break;

  case SEQ_MAIN_S:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ړI�̍��W�ւ���
 *
 *	@param	BR_BTNEX_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ňړ��I��  FALSE�ňړ���
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtnEx_Move_NextTarget( BR_BTNEX_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  switch( p_wk->move_seq )
  { 
  case SEQ_INIT:
    {
      GFL_POINT pos;

      BR_BTNEX_GetPos( p_wk, &pos );
      p_wk->start       = pos.y;
      p_wk->end         = p_wk->target.y;
      p_wk->sync_now    = 0;
      p_wk->sync_max    = BR_BTN_MOVE_DEFAULT_SYNC;
    }
    p_wk->move_seq  = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      s16 x;
      s16 now_y;
      now_y = p_wk->start + (p_wk->end - p_wk->start) * p_wk->sync_now / p_wk->sync_max;
      
      BR_BTN_GetPos( p_wk->p_btn, &x, NULL);
      BR_BTN_SetPos( p_wk->p_btn, x, now_y );

      if( p_wk->sync_now++ >= p_wk->sync_max )
      { 
        p_wk->move_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    return TRUE;
  }

  return FALSE;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�{�^���P�̍쐬�֐�(�e�v���b�N�Ŏg�p����)
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������
 *
 *	@param	const GFL_CLWK_DATA *cp_cldata	�ݒ���
 *	@param	msgID														�{�^���ɍڂ��镶����
 *	@param  w                               ��
 *	@param	display													�\����
 *	@param	*p_unit													�A�N�^�[�o�^���j�b�g
 *	@param	p_bmpoam												BMPOAM�o�^�V�X�e��
 *	@param	*p_font													�t�H���g
 *	@param	*p_msg													���b�Z�[�W
 *	@param	BR_RES_OBJ_DATA *cp_res					���\�[�X
 *	@param	heapID													�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_WORK * BR_BTN_Init( const GFL_CLWK_DATA *cp_cldata, u16 msgID, u16 w, CLSYS_DRAW_TYPE display, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, GFL_FONT *p_font, GFL_MSGDATA *p_msg, const BR_RES_OBJ_DATA *cp_res, HEAPID heapID )
{	
	BR_BTN_WORK *p_wk;
  STRBUF  *p_strbuf;

  //������쐬
  p_strbuf	= GFL_MSG_CreateString( p_msg, msgID );

  //�쐬�R�A
  p_wk  = BR_BTN_InitEx( cp_cldata, p_strbuf, w, display, p_unit, p_bmpoam, p_font, cp_res, heapID );

	//�t�H���g
  GFL_STR_DeleteBuffer( p_strbuf );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������EX msgID�ł͂Ȃ�STRBUF���當�����쐬�����
 *
 *	@param	const GFL_CLWK_DATA *cp_cldata	�ݒ���
 *	@param	msgID														�{�^���ɍڂ��镶����
 *	@param  w                               ��
 *	@param	display													�\����
 *	@param	*p_unit													�A�N�^�[�o�^���j�b�g
 *	@param	p_bmpoam												BMPOAM�o�^�V�X�e��
 *	@param	*p_font													�t�H���g
 *	@param	*p_msg													���b�Z�[�W
 *	@param	BR_RES_OBJ_DATA *cp_res					���\�[�X
 *	@param	heapID													�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_WORK * BR_BTN_InitEx( const GFL_CLWK_DATA *cp_cldata, const STRBUF *cp_strbuf, u16 w, CLSYS_DRAW_TYPE display, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, GFL_FONT *p_font, const BR_RES_OBJ_DATA *cp_res, HEAPID heapID )
{	
	BR_BTN_WORK *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_BTN_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BTN_WORK) );
  p_wk->w       = w;
  p_wk->display = display;

	//CLWK�쐬
	{	
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit, 
				cp_res->ncg, cp_res->ncl, cp_res->nce, 
				cp_cldata, display, heapID );
    GFL_CLACT_WK_SetObjMode( p_wk->p_clwk, GX_OAM_MODE_XLU );
	}

	//�t�H���g
	{	
		STRBUF			*p_str;

		p_wk->p_bmp	= GFL_BMP_Create( p_wk->w/8, 2, GFL_BMP_16_COLOR, heapID );
		PRINTSYS_PrintColor( p_wk->p_bmp, 0, 0, cp_strbuf, p_font, PRINTSYS_LSB_Make( 0xF, 0xE, 0 ) );
	}


	//OAM�̕������쐬
	{	
		BMPOAM_ACT_DATA	actdata;
		
		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= cp_cldata->pos_x + BR_BTN_OAMFONT_OFS_X;
		actdata.y		= cp_cldata->pos_y - BR_BTN_DATA_HEIGHT/2 + BR_BTN_OAMFONT_OFS_Y;
		actdata.pltt_index	= cp_res->ncl;
		actdata.soft_pri		= cp_cldata->softpri - 1;
		actdata.setSerface	= display;
		actdata.draw_type		= display;
		actdata.bg_pri			= cp_cldata->bgpri;
    actdata.pal_offset  = 1;
		p_wk->p_oamfont	= BmpOam_ActorAdd( p_bmpoam, &actdata );
    BmpOam_ActorBmpTrans(p_wk->p_oamfont);
    BmpOam_ActorSetObjMode( p_wk->p_oamfont, GX_OAM_MODE_XLU );
	}

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_Exit( BR_BTN_WORK *p_wk )
{	
	BmpOam_ActorDel( p_wk->p_oamfont );
	GFL_BMP_Delete( p_wk->p_bmp );
	GFL_CLACT_WK_Remove( p_wk->p_clwk );

	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̃^�b�`�����m
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *	@param	x													�^�b�`���WX
 *	@param	y													�^�b�`���WY
 *
 *	@return	TRUE�Ń{�^���g���K	FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_BTN_GetTrg( const BR_BTN_WORK *cp_wk, u32 x, u32 y )
{
  BOOL ret;
  ret = BR_BTN_GetHit( cp_wk, x, y );

  if( ret )
  { 
    PMSND_PlaySE( BR_SND_SE_OK );
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���Ɠ_�̏Փ˔���  (GetTrg�Ƃ̈Ⴂ��SE���Ȃ邩�Ȃ�Ȃ����ł�)
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *	@param	x													�^�b�`���WX
 *	@param	y													�^�b�`���WY
 *
 *	@return	TRUE�ŏՓ�	FALSE�ŏՓ˂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_BTN_GetHit( const BR_BTN_WORK *cp_wk, u32 x, u32 y )
{	
	GFL_RECT rect;
	GFL_CLACTPOS	pos;
  BOOL ret;

	GFL_CLACT_WK_GetPos( cp_wk->p_clwk, &pos, cp_wk->display );

	rect.left		= pos.x;
	rect.right	= pos.x + cp_wk->w;
	rect.top		= pos.y - BR_BTN_DATA_HEIGHT/2;
	rect.bottom	= pos.y + BR_BTN_DATA_HEIGHT/2;

  ret = ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̈ʒu��ݒ�
 *
 *	@param	BR_BTN_WORK *p_wk		���[�N
 *	@param	x										���WX
 *	@param	y										���WY
 */
//-----------------------------------------------------------------------------
void BR_BTN_SetPos( BR_BTN_WORK *p_wk, s16 x, s16 y )
{	
	GFL_CLACTPOS	pos;
	pos.x	= x;
	pos.y	= y;

	GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, p_wk->display );
	BmpOam_ActorSetPos( p_wk->p_oamfont,	pos.x + BR_BTN_OAMFONT_OFS_X,
																				pos.y - BR_BTN_DATA_HEIGHT/2 + BR_BTN_OAMFONT_OFS_Y );

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̈ʒu���擾
 *
 *	@param	BR_BTN_WORK *p_wk		���[�N
 *	@param	x										���WX�󂯎��
 *	@param	y										���WY�󂯎��
 */
//-----------------------------------------------------------------------------
void BR_BTN_GetPos( const BR_BTN_WORK *cp_wk, s16 *p_x, s16 *p_y )
{ 

	GFL_CLACTPOS	pos;
	GFL_CLACT_WK_GetPos( cp_wk->p_clwk, &pos, cp_wk->display );

  if( p_x )
  { 
    *p_x  = pos.x;
  }
  if( p_y )
  { 
    *p_y  = pos.y;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ���m�̃v���C�I���e�B��ݒ�
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 *	@param	soft_pri          �v���C�I���e�B
 */
//-----------------------------------------------------------------------------
void BR_BTN_SetSoftPriority( BR_BTN_WORK *p_wk, u8 soft_pri )
{ 
  GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, soft_pri );
  BmpOam_ActorSetSoftPriprity( p_wk->p_oamfont, soft_pri-1 );

}

//----------------------------------------------------------------------------
/**
 *	@brief  OBJ���m�̃v���C�I���e�B�擾
 *
 *	@param	const BR_BTN_WORK *cp_wk  ���[�N
 *
 *	@return �v���C�I���e�B
 */
//-----------------------------------------------------------------------------
u8 BR_BTN_GetSoftPriority( const BR_BTN_WORK *cp_wk )
{ 
  return GFL_CLACT_WK_GetSoftPri( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�Ƃ̃v���C�I���e�B��ݒ�
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 *	@param	bg_pri          �v���C�I���e�B
 */
//-----------------------------------------------------------------------------
void BR_BTN_SetBgPriority( BR_BTN_WORK *p_wk, u8 bg_pri )
{ 
  GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, bg_pri );
  BmpOam_ActorSetBgPriority( p_wk->p_oamfont, bg_pri );

}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�Ƃ̃v���C�I���e�B�擾
 *
 *	@param	const BR_BTN_WORK *cp_wk  ���[�N
 *
 *	@return �v���C�I���e�B
 */
//-----------------------------------------------------------------------------
u8 BR_BTN_GetBgPriority( const BR_BTN_WORK *cp_wk )
{ 
  return GFL_CLACT_WK_GetBgPri( cp_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�`�F���W
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 *	@param	ofs               �I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
void BR_BTN_SetPalleteOffset( BR_BTN_WORK *p_wk, u16 ofs )
{ 
  GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, ofs, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  BmpOam_ActorSetPaletteOffset( p_wk->p_oamfont, ofs + 1 );
}
