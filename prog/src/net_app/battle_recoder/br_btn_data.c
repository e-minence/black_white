//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn_data.c
 *	@brief	�{�^�����
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "msg/msg_battle_rec.h"
#include "br_btn_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���̍��W
//=====================================
#define BR_BTN_POS_X					(42)
#define BR_BTN_POS_START_Y		(25)	//�J�n�I�t�Z�b�g
#define BR_BTN_POS_OFFSET_Y		(36)	//�Ԋu�I�t�Z�b�g
#define BR_BTN_POS_NUMDIF_Y		(16)	//���ł̈ʒu�����I�t�Z�b�g
//���W�擾�}�N��
#define BR_BTN_DATA_GET_Y( n,m )	(BR_BTN_POS_START_Y+BR_BTN_POS_OFFSET_Y*n+BR_BTN_POS_NUMDIF_Y*(5-m))


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^�����
//=====================================
struct _BR_BTN_DATA
{	
	u16	param[BR_BTN_DATA_PARAM];
};

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�u���E�U���[�h	�������j���[
//=====================================
#define BTN_DATA_BROWSE_TOP_MAX	(4)
static const sc_btn_data_browse_top[BTN_DATA_BROWSE_TOP_MAX] =
{	
	//�o�g���r�f�I
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_TOP_MAX),
		BR_BROWSE_MENUID_TOP,
		msg_01,
		0,
		BR_BTN_TYPE_SELECT,
		BR_BROWSE_MENUID_BTLVIDEO,

	},
	//�o�g���T�u�E�F�C
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_TOP_MAX),
		BR_BROWSE_MENUID_TOP,
		msg_02,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		0,

	},
	//�����_���}�b�`
	{
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_TOP_MAX),
		BR_BROWSE_MENUID_TOP,
		msg_03,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		0,

	},
	//��߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BROWSE_TOP_MAX),
		BR_BROWSE_MENUID_TOP,
		msg_04,
		4,
		BR_BTN_TYPE_EXIT,
		0,
	},
};
//-------------------------------------
///	�u���E�U���[�h	�o�g���r�f�I���j���[
//=====================================
#define BTN_DATA_BROWSE_BTLVIDEO_MAX	(4)
static const sc_btn_data_browse_btlviedeo[BTN_DATA_BROWSE_BTLVIDEO_MAX] =
{	
	//�����̋L�^
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_BTLVIDEO_MAX),
		BR_BROWSE_MENUID_BTLVIDEO,
		msg_06,
		6,
		BR_BTN_TYPE_MYRECORD,
		0,
	},
	//�N���̋L�^
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_BTLVIDEO_MAX),
		BR_BROWSE_MENUID_BTLVIDEO,
		msg_07,
		7,
		BR_BTN_TYPE_SELECT,
		BR_BROWSE_MENUID_OTHER_RECORD,
	},
	//�L�^������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_BTLVIDEO_MAX),
		BR_BROWSE_MENUID_BTLVIDEO,
		msg_08,
		1,
		BR_BTN_TYPE_SELECT,
		BR_BROWSE_MENUID_DELETE_RECORD,
	},
	//���ǂ�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BROWSE_BTLVIDEO_MAX),
		BR_BROWSE_MENUID_BTLVIDEO,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BROWSE_MENUID_TOP,
	},
};

//-------------------------------------
///	�u���E�U���[�h	�N���̋L�^���j���[
//=====================================
#define BTN_DATA_BROWSE_OTHER_RECORD_MAX	(4)
static const sc_btn_data_browse_other_record[BTN_DATA_BROWSE_OTHER_RECORD_MAX] =
{	
	//�N���̋L�^�P
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		0,
	},
	//�N���̋L�^�Q
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		1,
	},
	//�N���̋L�^�R
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		2,
	},
	//�߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BROWSE_MENUID_BTLVIDEO,
	},
};
//-------------------------------------
///	�u���E�U���[�h	�L�^���������j���[
//=====================================
#define BTN_DATA_BROWSE_DELETE_RECORD_MAX	(3)
static const sc_btn_data_browse_delete_record[BTN_DATA_BROWSE_DELETE_RECORD_MAX] =
{	
	//�����̋L�^
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_DELETE_RECORD_MAX),
		BR_BROWSE_MENUID_DELETE_RECORD,
		msg_100,
		6,
		BR_BTN_TYPE_DELETE_MY,
		0,
	},
	//�N���̋L�^
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_DELETE_RECORD_MAX),
		BR_BROWSE_MENUID_DELETE_RECORD,
		msg_101,
		7,
		BR_BTN_TYPE_SELECT,
		BR_BROWSE_MENUID_DELETE_OTHER,
	},
	//���ǂ�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_DELETE_RECORD_MAX),
		BR_BROWSE_MENUID_DELETE_RECORD,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BROWSE_MENUID_BTLVIDEO,
	},
};
//-------------------------------------
///	�u���E�U���[�h	�N���̋L�^���������j���[
//=====================================
#define BTN_DATA_BROWSE_DELETE_OTHER_MAX	(4)
static const sc_btn_data_browse_delete_other[BTN_DATA_BROWSE_DELETE_OTHER_MAX] =
{	
	//�N���̋L�^�P
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_10,
		7,
		BR_BTN_TYPE_DELETE_OTHER,
		0,
	},
	//�N���̋L�^�Q
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_10,
		7,
		BR_BTN_TYPE_DELETE_OTHER,
		1,
	},
	//�N���̋L�^�R
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_10,
		7,
		BR_BTN_TYPE_DELETE_OTHER,
		2,
	},
	//�߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BROWSE_MENUID_DELETE_RECORD,
	},
};
//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	�������j���[
//=====================================
#define BTN_DATA_BTLVIDEO_TOP_MAX	(3)
static const sc_btn_data_btlvideo_top[BTN_DATA_BTLVIDEO_TOP_MAX] =
{	
	//�o�g���r�f�I������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BTLVIDEO_TOP_MAX),
		BR_BTLVIDEO_MENUID_TOP,
		msg_700,
		2,
		BR_BTN_TYPE_SELECT,
		BR_BTLVIDEO_MENUID_LOOK,
	},
	//�o�g���r�f�I�𑗂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BTLVIDEO_TOP_MAX),
		BR_BTLVIDEO_MENUID_TOP,
		msg_700,
		9,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//��߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BTLVIDEO_TOP_MAX),
		BR_BTLVIDEO_MENUID_TOP,
		msg_700,
		14,
		BR_BTN_TYPE_SELECT,
		BR_BTLVIDEO_MENUID_LOOK,
	},
};
//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	���郁�j���[
//=====================================
#define BTN_DATA_BTLVIDEO_LOOK_MAX	(4)
static const sc_btn_data_btlvideo_look[BTN_DATA_BTLVIDEO_LOOK_MAX] =
{	
	//�����L���O�ŒT��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_200,
		16,
		BR_BTN_TYPE_SELECT,
		BR_BTLVIDEO_MENUID_RANK,
	},
	//�ڂ����T��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_203,
		17,
		BR_BTN_TYPE_SELECT,
		BR_BTLVIDEO_MENUID_RANK,
	},
	//�i���o�[�ŒT��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_204,
		11,
		BR_BTN_TYPE_SELECT,
		BR_BTLVIDEO_MENUID_RANK,
	},
	//���ǂ�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BTLVIDEO_MENUID_TOP,
	},
};
//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	�����L���O�ŒT�����j���[
//=====================================
#define BTN_DATA_BTLVIDEO_RANK_MAX	(4)
static const sc_btn_data_btlvideo_rank[BTN_DATA_BTLVIDEO_RANK_MAX] =
{	
	//�ŐV30��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BTLVIDEO_RANK_MAX),
		BR_BTLVIDEO_MENUID_RANK,
		msg_202,
		10,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//�ʐM�ΐ탉���L���O
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BTLVIDEO_RANK_MAX),
		BR_BTLVIDEO_MENUID_RANK,
		msg_202,
		10,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//�T�u�E�F�C�����L���O
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BTLVIDEO_RANK_MAX),
		BR_BTLVIDEO_MENUID_RANK,
		msg_202,
		10,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//���ǂ�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(3,BTN_DATA_BTLVIDEO_RANK_MAX),
		BR_BTLVIDEO_MENUID_RANK,
		msg_05,
		5,
		BR_BTN_TYPE_RETURN,
		BR_BTLVIDEO_MENUID_LOOK,
	},
};
//-------------------------------------
///	�O���[�o���o�g���~���[�W�J���V���b�g	�������j���[
//=====================================
#define BTN_DATA_MUSICAL_TOP_MAX	(3)
static const sc_btn_data_musical_top[BTN_DATA_MUSICAL_TOP_MAX] =
{	
	//�ʐ^������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//�ʐ^�𑗂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_300,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		0,
	},
	//��߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_04,
		4,
		BR_BTN_TYPE_EXIT,
		0,
	},
};

//-------------------------------------
///	�ėp	�͂��A������
//=====================================
#define BTN_DATA_YESNO_MAX	(2)
static const sc_btn_data_yesno[BTN_DATA_YESNO_MAX] =
{	
	//�͂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		0,
		0
	},
	//������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		0,
		0
	},

};

//-------------------------------------
///	���j���[���Ƃ̃e�[�u��
//=====================================
static const struct 
{	
	u32					max;
	const BR_BTN_DATA *cp_data;
} scp_btn_data_tbl[BR_MENUID_MAX]	=
{	
	{	
		BTN_DATA_BROWSE_TOP_MAX,
		sc_btn_data_browse_top,
	},
	{	
		BTN_DATA_BROWSE_BTLVIDEO_MAX,
		sc_btn_data_browse_btlviedeo,
	},
	{	
		BTN_DATA_BROWSE_OTHER_RECORD_MAX,
		sc_btn_data_browse_other_record,
	},
	{	
		BTN_DATA_BROWSE_DELETE_RECORD_MAX,
		sc_btn_data_browse_delete_record,
	},
	{	
		BTN_DATA_BROWSE_DELETE_OTHER_MAX,
		sc_btn_data_browse_delete_other,
	},
	{	
		BTN_DATA_BTLVIDEO_TOP_MAX,
		sc_btn_data_btlvideo_top,
	},
	{	
		BTN_DATA_BTLVIDEO_LOOK_MAX,
		sc_btn_data_btlvideo_look,
	},
	{	
		BTN_DATA_BTLVIDEO_RANK_MAX,
		sc_btn_data_btlvideo_rank,
	},
	{	
		BTN_DATA_MUSICAL_TOP_MAX,
		sc_btn_data_musical_top,
	},
	{	
		BTN_DATA_YESNO_MAX,
		sc_btn_data_yesno,
	},
};

//=============================================================================
/**
 *					PUBLIC
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^���擾
 *
 *	@param	BR_MENUID menuID	�������郁�j���[ID
 *	@param	btnID							�{�^��ID
 *
 *	@return	�{�^���f�[�^
 */
//-----------------------------------------------------------------------------
const BR_BTN_DATA * BR_BTN_DATA_SYS_GetData( BR_MENUID menuID, u16 btnID )
{	
	GF_ASSERT( menuID < BR_MENUID_MAX );
	GF_ASSERT( btnID  < scp_btn_data_tbl[menuID].max );
	return scp_btn_data_tbl[menuID].cp_data[ btnID ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���Ƃ̃{�^���ő吔���擾
 *
 *	@param	BR_MENUID menuID	���j���[ID
 *
 *	@return	�{�^���̐�
 */
//-----------------------------------------------------------------------------
u32 BR_BTN_DATA_SYS_GetDataNum( BR_MENUID menuID )
{	
	GF_ASSERT( menuID < BR_MENUID_MAX );
	return scp_btn_data_tbl[menuID].max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ꂼ��̃��j���[�����������j���[�̃{�^���ő吔���擾
 *
 *	@return	�{�^���̐�
 */
//-----------------------------------------------------------------------------
u32 BR_BTN_DATA_SYS_GetDataMax( void )
{	
	int i;
	int max;

	max	= 0;
	for( i = 0; i < BR_MENUID_MAX; i++ )
	{	
		if( max	< BR_BTN_DATA_SYS_GetDataNum( i ) )
		{	
			max	= BR_BTN_DATA_SYS_GetDataNum( i );
		}

	}
	return max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�h�ʂ̏������j���[�擾
 *
 *	@param	BR_MODE mode ���[�h
 *
 *	@return	�������j���[
 */
//-----------------------------------------------------------------------------
BR_MENUID BR_BTN_DATA_SYS_GetStartMenuID( BR_MODE mode )
{	
	BR_MENUID	startID;

	switch( mode )
	{	
	case BR_MODE_BROWSE:
		startID	= BR_BROWSE_MENUID_TOP;
		break;
	case BR_MODE_GLOBAL_BV:
		startID	= BR_BTLVIDEO_MENUID_TOP;
		break;
	case BR_MODE_GLOBAL_MUSICAL:
		startID	= BR_MUSICAL_MENUID_TOP;
		break;
	default:
		GF_ASSERT(0);
	}

	return startID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̌��J�����擾
 *
 *	@param	const BR_BTN_DATA *cp_data	�f�[�^
 *	@param	paramID											���J���C���f�b�N�X
 *
 *	@return	�p�����[�^
 */
//-----------------------------------------------------------------------------
u16 BR_BTN_DATA_GetParam( const BR_BTN_DATA *cp_data, BR_BTN_DATA_PARAM paramID )
{	
	GF_ASSERT( paramID < BR_BTN_DATA_PARAM_MAX );
	return cp_data->param[paramID];
}
