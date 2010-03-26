//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut.c
 *	@brief	Y�{�^���o�^�Z�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "savedata/shortcut.h"
#include "savedata/save_tbl.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_PRINT_ON
#endif //PM_DEBUG

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	Y�{�^���o�^�p�Z�[�u�f�[�^
//=====================================
struct _SHORTCUT
{	
	u8	data[SHORTCUT_SAVE_FLAG_MAX];
};

SDK_COMPILER_ASSERT( SHORTCUT_ID_MAX <= SHORTCUT_SAVE_FLAG_MAX );

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
static void ShortCut_SortNull( SHORTCUT *p_wk );


#ifdef DEBUG_PRINT_ON
static inline void DEBUG_PrintData( SHORTCUT *p_wk )
{	
	int i;
	for( i = 0; i < SHORTCUT_ID_MAX; i++ )
	{	
		NAGI_Printf( "data[%d]= %d \n", i, p_wk->data[ i ] );
	}
}
#else
#define DEBUG_PRINT_ON(x)	/*  */
#endif

//=============================================================================
/**
 *	�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SHORTCUT�\���̂̃T�C�Y�擾
 *
 *	@return	SHORTCUT�\���̂̃T�C�Y
 */
//-----------------------------------------------------------------------------
int SHORTCUT_GetWorkSize( void )
{	
	return sizeof(SHORTCUT);
}
//----------------------------------------------------------------------------
/**
 *	@brief	SHORTCUT�\���̏�����
 *
 *	@param	SHORTCUT *p_msc ���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUT_Init( SHORTCUT *p_wk )
{	
	GFL_STD_MemFill( p_wk->data, SHORTCUT_ID_NULL, sizeof(u8) * SHORTCUT_ID_MAX );
}

//=============================================================================
/**
 *	�Z�[�u�f�[�^�擾�̂��߂̊֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����ރf�[�^��CONST�Ń|�C���^�擾
 *
 *	@param	SAVE_CONTROL_WORK * cp_sv	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 *	@return	SHORTCUT
 */
//-----------------------------------------------------------------------------
const SHORTCUT * SaveData_GetShortCutConst( const SAVE_CONTROL_WORK * cp_sv)
{	
	return (const SHORTCUT *)SaveData_GetShortCut( (SAVE_CONTROL_WORK *)cp_sv);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����ރf�[�^�̃|�C���^�擾
 *
 *	@param	SAVE_CONTROL_WORK * p_sv	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 *	@return	SHORTCUT
 */
//-----------------------------------------------------------------------------
SHORTCUT * SaveData_GetShortCut( SAVE_CONTROL_WORK * p_sv)
{	
	SHORTCUT	*p_msc;
	p_msc = SaveControl_DataPtrGet(p_sv, GMDATA_ID_SHORTCUT);
	return p_msc;
}

//=============================================================================
/**
 *	�e�A�v�����ł́AY�{�^���o�^�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	Y�{�^���o�^�ݒ�
 *
 *	@param	SHORTCUT *p_wk	���[�N
 *	@param	shortcutID		�o�^�^�C�v
 *	@param	is_on		TRUE��Y�{�^���o�^	FALSE��Y�{�^���o�^����
 */
//-----------------------------------------------------------------------------
void SHORTCUT_SetRegister( SHORTCUT *p_wk, SHORTCUT_ID shortcutID, BOOL is_on )
{	
	int i;
	if( is_on )
	{	
		//�o�^���鏈��
		//�\�[�g�ς݂ƍl���ANULL������܂Ō��������
		for( i = 0; i < SHORTCUT_ID_MAX; i++ )
		{
			//���łɂ������牽�����Ȃ�
			if( p_wk->data[ i ] == shortcutID )
			{
				return;
			}

			//�������
			if( p_wk->data[ i ] == SHORTCUT_ID_NULL )
			{	
				break;
			}
		}
		GF_ASSERT_MSG( i != SHORTCUT_ID_MAX, "SHORTCUT_ID�ւ̓o�^���s�\��\n" );

		p_wk->data[ i ] = shortcutID;
	}
	else
	{	
		//��������
		for( i = 0; i < SHORTCUT_ID_MAX; i++ )
		{	
			if( p_wk->data[ i ] == shortcutID )
			{	
				p_wk->data[ i ] = SHORTCUT_ID_NULL;
			}
		}
	}


	//NULL���\�[�g
	ShortCut_SortNull( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	Y�{�^���o�^��Ԏ擾
 *	
 *	@param	const SHORTCUT *cp_wk	���[�N
 *	@param	shortcutID									�o�^�^�C�v
 *
 *	@return	TRUE�Ȃ�Γo�^����Ă���	FALSE�Ȃ�Γo�^����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL SHORTCUT_GetRegister( const SHORTCUT *cp_wk, SHORTCUT_ID shortcutID )
{	
	int i;
	//��������
	for( i = 0; i < SHORTCUT_ID_MAX; i++ )
	{	
		if( cp_wk->data[ i ] == shortcutID )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
/**
 *	Y�{�^�����j���[�Ŏ擾�A���삷��֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�C���f�b�N�X����o�^����Ă�����̂��擾
 *
 *	@param	const SHORTCUT *cp_wk		���[�N
 *	@param	idx											�C���f�b�N�X	
 *
 *	@return	�o�^����Ă���^�C�v	or SHORTCUT_ID_NULL�Ȃ�Γo�^����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
SHORTCUT_ID SHORTCUT_GetType( const SHORTCUT *cp_wk, u8 idx )
{	
	GF_ASSERT_MSG( idx < SHORTCUT_ID_MAX, "�C���f�b�N�X���ő�𒴂��Ă��܂�\n" );
	return cp_wk->data[ idx ];
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�^�����擾
 *
 *	@param	const SHORTCUT *cp_wk		���[�N
 *
 *	@return	�o�^��
 */
//-----------------------------------------------------------------------------
u16 SHORTCUT_GetMax( const SHORTCUT *cp_wk )
{	
	int i;
	int cnt = 0;
	for( i = 0 ; i < SHORTCUT_ID_MAX; i++ )
	{	
		if( cp_wk->data[ i ] != SHORTCUT_ID_NULL )
		{	
			cnt++;
		}
	}

	return cnt;
}
//----------------------------------------------------------------------------
/**
 *	@brief	����ւ��}��
 *					�w�肳�ꂽ�^�C�v���w�肵���C���f�b�N�X�Ɉڂ�����
 *
 *	@param	const SHORTCUT *cp_wk		���[�N
 *	@param	shortcutID										
 *	@param	insert_idx							�}�������C���f�b�N�X
 *
 *	@param	�}�����ꂽ�C���f�b�N�X�i�|1����Ă��邱�Ƃ�����j
 *
 */
//-----------------------------------------------------------------------------
u8 SHORTCUT_Insert( SHORTCUT *p_wk, SHORTCUT_ID shortcutID, u8 insert_idx )
{	
	s16 i;
	u16 next;
	s16 erase_idx;

	/*
	 *	����
	 *
	 *	�}���Ȃ̂ŁA��ɂ����Ƃ��͗ǂ����A
	 *	��[�����ꍇ�A���ւ����Ƃ��́A�����̕����l�����邱��
	 *
	 *		
	 *	AAA	��	�����ƃC���f�b�N�X���l�܂�̂ŁE�E�E
	 *			��	���ɂ����Ƃ���-1�C���f�b�N�X
	 *	BBB
	 *		
	 *	CCC
	 *
	 *
	 */


	GF_ASSERT_MSG( insert_idx < SHORTCUT_ID_MAX, "�C���f�b�N�X���ő�𒴂��Ă��܂�\n" );
	
	//�����̃C���f�b�N�X���擾
	for( erase_idx = 0 ; erase_idx < SHORTCUT_ID_MAX ; erase_idx++ )
	{
		if( p_wk->data[ erase_idx ] == shortcutID  )
		{	
			break;
		}
	}
	GF_ASSERT_MSG( erase_idx != SHORTCUT_ID_MAX, "������Ȃ�����\n" )


	//�^�C�v������
	SHORTCUT_SetRegister( p_wk, shortcutID, FALSE );
	
	NAGI_Printf( "��������\n" );
	DEBUG_PrintData( p_wk );
	
	//���������܂�̂�
	//�C���f�b�N�X�|1
	if( erase_idx < insert_idx )
	{	
		insert_idx--;
	}


	//�}���̈ȉ����P�����炷
	for( i = SHORTCUT_ID_MAX-1; i >= insert_idx ; i-- )
	{	
		p_wk->data[ i ] = p_wk->data[ i - 1 ];
	}

	//�}��
	p_wk->data[ insert_idx ] = shortcutID;


	NAGI_Printf( "�}������\n" );
	DEBUG_PrintData( p_wk );

	return insert_idx;
}
//=============================================================================
/**
 *		PRIVATE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SHORTCUT_ID_NULL������΃f�[�^���l�߂�
 *
 *	@param	SHORTCUT *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ShortCut_SortNull( SHORTCUT *p_wk )
{	
	u16 i;
	u16 next;
	u8 tmp;
	for( i = 0; i < SHORTCUT_ID_MAX; i++ )
	{
		next	= MATH_CLAMP( i+1, 1, SHORTCUT_ID_MAX-1 );
		if( p_wk->data[ i ] == SHORTCUT_ID_NULL )
		{	
			tmp	= p_wk->data[ i ];
			p_wk->data[ i ] = p_wk->data[ next ];
			p_wk->data[ next ] = tmp;
		}
	}
}
