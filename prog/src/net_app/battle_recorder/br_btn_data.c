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
//���C�u����
#include <gflib.h>

#include "msg/msg_battle_rec.h"
#include "br_btn_data.h"
#include "br_core.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���̍��W
//=====================================
#define BR_BTN_POS_START_Y		(25)	//�J�n�I�t�Z�b�g
#define BR_BTN_POS_OFFSET_Y		(36)	//�Ԋu�I�t�Z�b�g
#define BR_BTN_POS_NUMDIF_Y		(16)	//���ł̈ʒu�����I�t�Z�b�g
//���W�擾�}�N��
#define BR_BTN_DATA_GET_Y( n,m )	(BR_BTN_POS_START_Y+BR_BTN_POS_OFFSET_Y*n+BR_BTN_POS_NUMDIF_Y*(5-m))

#define BR_BTN_LINK_END				(0xFFFFFFFF)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���q������
//=====================================
typedef struct 
{
	BR_MENUID	menuID;
	u32				btnID;
} BR_BTN_LINK;

//-------------------------------------
///	�{�^�����
//=====================================
struct _BR_BTN_DATA
{	
	u16			param[BR_BTN_DATA_PARAM_MAX];
};

//-------------------------------------
///	�{�^�����
//=====================================
struct _BR_BTN_DATA_SYS
{
	const BR_BTLREC_SET *cp_rec;
	BR_BTN_DATA					buff[0];
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
static const BR_BTN_DATA sc_btn_data_browse_top[BTN_DATA_BROWSE_TOP_MAX] =
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
		0,
		0,
		0,
		0,
	},
	//�o�g���T�u�E�F�C
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_TOP_MAX),
		BR_BROWSE_MENUID_TOP,
		msg_02,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_BTLSUBWAY,
		0,
		0,
		0,
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
		BR_PROCID_RNDMATCH,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_browse_top[] = 
{	
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	}
};

//-------------------------------------
///	�u���E�U���[�h	�o�g���r�f�I���j���[
//=====================================
#define BTN_DATA_BROWSE_BTLVIDEO_MAX	(4)
static const BR_BTN_DATA sc_btn_data_browse_btlvideo[BTN_DATA_BROWSE_BTLVIDEO_MAX] =
{	
	//�����̋L�^
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_BTLVIDEO_MAX),
		BR_BROWSE_MENUID_BTLVIDEO,
		msg_06,
		6,
		BR_BTN_TYPE_MYRECORD,
		BR_PROCID_RECORD,
		0,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
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
		0,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_browse_btlvideo[] = 
{	
	{	
		BR_BROWSE_MENUID_TOP,
		0,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	},
};

//-------------------------------------
///	�u���E�U���[�h	�N���̋L�^���j���[
//=====================================
#define BTN_DATA_BROWSE_OTHER_RECORD_MAX	(4)
static const BR_BTN_DATA sc_btn_data_browse_other_record[BTN_DATA_BROWSE_OTHER_RECORD_MAX] =
{	
	//�N���̋L�^�P
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		BR_PROCID_RECORD,
		1,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
	},
	//�N���̋L�^�Q
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		BR_PROCID_RECORD,
		2,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
	},
	//�N���̋L�^�R
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_OTHER_RECORD_MAX),
		BR_BROWSE_MENUID_OTHER_RECORD,
		msg_10,
		7,
		BR_BTN_TYPE_OTHERRECORD,
		BR_PROCID_RECORD,
		3,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_browse_other_record[] = 
{	
	{	
		BR_BROWSE_MENUID_TOP,
		0,
	},
	{	
		BR_BROWSE_MENUID_BTLVIDEO,
		1,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	},
};
//-------------------------------------
///	�u���E�U���[�h	�L�^���������j���[
//=====================================
#define BTN_DATA_BROWSE_DELETE_RECORD_MAX	(3)
static const BR_BTN_DATA sc_btn_data_browse_delete_record[BTN_DATA_BROWSE_DELETE_RECORD_MAX] =
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
		0,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
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
		0,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_browse_delete_record[] = 
{	
	{	
		BR_BROWSE_MENUID_TOP,
		0,
	},
	{	
		BR_BROWSE_MENUID_BTLVIDEO,
		2,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	},
};
//-------------------------------------
///	�u���E�U���[�h	�N���̋L�^���������j���[
//=====================================
#define BTN_DATA_BROWSE_DELETE_OTHER_MAX	(4)
static const BR_BTN_DATA sc_btn_data_browse_delete_other[BTN_DATA_BROWSE_DELETE_OTHER_MAX] =
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
		0,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
	},
	//�N���̋L�^�Q
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_10,
		7,
		BR_BTN_TYPE_DELETE_OTHER,
		0,
		1,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
	},
	//�N���̋L�^�R
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BROWSE_DELETE_OTHER_MAX),
		BR_BROWSE_MENUID_DELETE_OTHER,
		msg_10,
		7,
		BR_BTN_TYPE_DELETE_OTHER,
		0,
		2,
		0,
		BR_BTN_NONEPROC_NOPUSH,
		msg_info_000,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_browse_delete_other[] = 
{	
	{	
		BR_BROWSE_MENUID_TOP,
		0,
	},
	{	
		BR_BROWSE_MENUID_BTLVIDEO,
		2,
	},
	{	
		BR_BROWSE_MENUID_DELETE_RECORD,
		1,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	},
};
//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	�������j���[
//=====================================
#define BTN_DATA_BTLVIDEO_TOP_MAX	(3)
static const BR_BTN_DATA sc_btn_data_btlvideo_top[BTN_DATA_BTLVIDEO_TOP_MAX] =
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
		0,
		0,
		0,
		0,
	},
	//�o�g���r�f�I�𑗂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BTLVIDEO_TOP_MAX),
		BR_BTLVIDEO_MENUID_TOP,
		msg_701,
		9,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_BV_SEND,
		0,
		0,
		0,
		0,
	},
	//��߂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BTLVIDEO_TOP_MAX),
		BR_BTLVIDEO_MENUID_TOP,
		msg_04,
		4,
		BR_BTN_TYPE_EXIT,
		0,
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_btlvideo_top[] = 
{	
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	}
};

//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	���郁�j���[
//=====================================
#define BTN_DATA_BTLVIDEO_LOOK_MAX	(4)
static const BR_BTN_DATA sc_btn_data_btlvideo_look[BTN_DATA_BTLVIDEO_LOOK_MAX] =
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
		0,
		0,
		0,
		0,
	},
	//�ڂ����T��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_203,
		17,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_BV_SEARCH,
		0,
		0,
		0,
		0,
	},
	//�i���o�[�ŒT��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(2,BTN_DATA_BTLVIDEO_LOOK_MAX),
		BR_BTLVIDEO_MENUID_LOOK,
		msg_204,
		11,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_CODEIN,
		0,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_btlvideo_look[] = 
{	
	{	
		BR_BTLVIDEO_MENUID_TOP,
		0,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	}
};
//-------------------------------------
///	�O���[�o���o�g���r�f�I���[�h	�����L���O�ŒT�����j���[
//=====================================
#define BTN_DATA_BTLVIDEO_RANK_MAX	(4)
static const BR_BTN_DATA sc_btn_data_btlvideo_rank[BTN_DATA_BTLVIDEO_RANK_MAX] =
{	
	//�ŐV30��
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_BTLVIDEO_RANK_MAX),
		BR_BTLVIDEO_MENUID_RANK,
		msg_202,
		10,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_BV_RANK,
		0,
		0,
		0,
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
		BR_PROCID_BV_RANK,
		1,
		0,
		0,
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
		BR_PROCID_BV_RANK,
		2,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_btlvideo_rank[] = 
{	
	{	
		BR_BTLVIDEO_MENUID_TOP,
		0,
	},
	{	
		BR_BTLVIDEO_MENUID_LOOK,
		0,
	},
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	},
};
//-------------------------------------
///	�O���[�o���o�g���~���[�W�J���V���b�g	�������j���[
//=====================================
#define BTN_DATA_MUSICAL_TOP_MAX	(3)
static const BR_BTN_DATA sc_btn_data_musical_top[BTN_DATA_MUSICAL_TOP_MAX] =
{	
	//�ʐ^������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		BR_BTN_TYPE_CHANGESEQ,
		BR_PROCID_MUSICAL_LOOK,
		0,
		0,
		0,
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
		BR_PROCID_MUSICAL_SEND,
		0,
		0,
		0,
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
		0,
		0,
		0,
		0,
	},
};
static const BR_BTN_LINK sc_btn_link_musical_top[] = 
{	
	{	
		BR_BTN_LINK_END,
		BR_BTN_LINK_END,
	}
};

//-------------------------------------
///	�ėp	�͂��A������
//=====================================
#define BTN_DATA_YESNO_MAX	(2)
static const BR_BTN_DATA sc_btn_data_yesno[BTN_DATA_YESNO_MAX] =
{	
	//�͂�
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(0,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	},
	//������
	{	
		BR_BTN_POS_X,
		BR_BTN_DATA_GET_Y(1,BTN_DATA_MUSICAL_TOP_MAX),
		BR_MUSICAL_MENUID_TOP,
		msg_301,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	},

};

//-------------------------------------
///	���j���[���Ƃ̃e�[�u��
//=====================================
static const struct 
{	
	u16								max;			//�{�^���̐�
	const BR_BTN_DATA *cp_data;	//�{�^�����
	const BR_BTN_LINK	*cp_link;	//�{�^���q����
} sc_btn_data_tbl[BR_MENUID_MAX]	=
{	
	{	
		BTN_DATA_BROWSE_TOP_MAX,
		sc_btn_data_browse_top,
		sc_btn_link_browse_top,
	},
	{	
		BTN_DATA_BROWSE_BTLVIDEO_MAX,
		sc_btn_data_browse_btlvideo,
		sc_btn_link_browse_btlvideo,
	},
	{	
		BTN_DATA_BROWSE_OTHER_RECORD_MAX,
		sc_btn_data_browse_other_record,
		sc_btn_link_browse_other_record,
	},
	{	
		BTN_DATA_BROWSE_DELETE_RECORD_MAX,
		sc_btn_data_browse_delete_record,
		sc_btn_link_browse_delete_record,
	},
	{	
		BTN_DATA_BROWSE_DELETE_OTHER_MAX,
		sc_btn_data_browse_delete_other,
		sc_btn_link_browse_delete_other,
	},
	{	
		BTN_DATA_BTLVIDEO_TOP_MAX,
		sc_btn_data_btlvideo_top,
		sc_btn_link_btlvideo_top,
	},
	{	
		BTN_DATA_BTLVIDEO_LOOK_MAX,
		sc_btn_data_btlvideo_look,
		sc_btn_link_btlvideo_look,
	},
	{	
		BTN_DATA_BTLVIDEO_RANK_MAX,
		sc_btn_data_btlvideo_rank,
		sc_btn_link_btlvideo_rank,
	},
	{	
		BTN_DATA_MUSICAL_TOP_MAX,
		sc_btn_data_musical_top,
		sc_btn_link_musical_top,
	},
	{	
		BTN_DATA_YESNO_MAX,
		sc_btn_data_yesno,
		NULL,
	},
};

//=============================================================================
/**
 *					PUBLIC
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^�o�b�t�@�쐬
 *
 *  @param	�o�g�����R�[�_�[�Z�[�u�f�[�^
 *	@param	HEAPID heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_DATA_SYS * BR_BTN_DATA_SYS_Init( const BR_BTLREC_SET *cp_rec, HEAPID heapID )
{	
	BR_BTN_DATA_SYS	*p_wk;
	u32 size;

	//�w�b�_���o�b�t�@�̃T�C�Y�v�Z
	{	
		u32 buff_size;
		int i;

		//�o�b�t�@���𐔂���
		buff_size	= 0;
		for( i = 0; i < BR_MENUID_MAX; i++ )
		{
			buff_size	+= sc_btn_data_tbl[i].max;
		}
		size	= sizeof(BR_BTN_DATA_SYS) + sizeof(BR_BTN_DATA) * buff_size;
	}

	//�w�b�_���o�b�t�@�쐬
	p_wk	= GFL_HEAP_AllocMemory( heapID, size );
	GFL_STD_MemClear( p_wk, size );
	p_wk->cp_rec	= cp_rec;

	//�o�b�t�@�ɏ��ݒ�
	{	
		int i,j;
		u32 max;
		u32 cnt;
		const BR_BTN_DATA *cp_data;

		cnt	= 0;
		for( i = 0; i < BR_MENUID_MAX; i++ )
		{	
			max			= sc_btn_data_tbl[i].max;
			cp_data	= sc_btn_data_tbl[i].cp_data;
			for( j = 0; j < max; j++ )
			{	
				//�����f�[�^��ݒ�
				p_wk->buff[cnt]	= cp_data[j];


				//�O������̏��������
				switch( p_wk->buff[cnt].param[ BR_BTN_DATA_PARAM_TYPE ] )
				{	
				//�����̋L�^���Ȃ������牟���Ȃ�
				case BR_BTN_TYPE_MYRECORD:
					/* fallthrough */
				case BR_BTN_TYPE_DELETE_MY:
					p_wk->buff[cnt].param[BR_BTN_DATA_PARAM_VALID]	= cp_rec->my.is_valid;
					break;

				//�N���̋L�^���Ȃ������牟���Ȃ�
				//������Ȃ�Ε\�����u�����v���u�~�~�~�̂��낭�v�ɐ؂�ւ�
				case BR_BTN_TYPE_OTHERRECORD:
					/* fallthrough */
				case BR_BTN_TYPE_DELETE_OTHER:
					{	
						u32 id	= p_wk->buff[cnt].param[ BR_BTN_DATA_PARAM_DATA2 ];
						GF_ASSERT( id < 4 );
						p_wk->buff[cnt].param[BR_BTN_DATA_PARAM_VALID]	= cp_rec->other[ id-1 ].is_valid;
						if( cp_rec->other[ id-1 ].is_valid )
						{	
							p_wk->buff[cnt].param[BR_BTN_DATA_PARAM_MSGID]	= msg_09;
						}
					}
					break;

					//@todo �����_���}�b�`��o�g���T�u�E�F�C�͂����������牟���Ȃ��󋵂����肦�邩��

				default:	//���̑��͂ǂ�ȂƂ��ł�������
					p_wk->buff[cnt].param[BR_BTN_DATA_PARAM_VALID]	= TRUE;	
				}
				cnt++;


			}
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^�o�b�t�@�j��
 *
 *	@param	BR_BTN_DATA_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_DATA_SYS_Exit( BR_BTN_DATA_SYS *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^���擾
 *
 *  @param	p_wk							���[�N
 *	@param	BR_MENUID menuID	�������郁�j���[ID
 *	@param	btnID							�{�^��ID
 *
 *	@return	�{�^���f�[�^
 */
//-----------------------------------------------------------------------------
const BR_BTN_DATA * BR_BTN_DATA_SYS_GetData( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID, u16 btnID )
{	
	int i;
	u32 offset;

	GF_ASSERT( menuID < BR_MENUID_MAX );
	GF_ASSERT( btnID  < sc_btn_data_tbl[menuID].max );

	//�I�t�Z�b�g�v�Z
	offset	= 0;
	for( i = 0; i < menuID; i++ )
	{	
		offset	+= sc_btn_data_tbl[i].max;
	}

	return &(cp_wk->buff[offset + btnID]);
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���Ƃ̃{�^���ő吔���擾
 *
 *  @param	p_wk							���[�N
 *	@param	BR_MENUID menuID	���j���[ID
 *
 *	@return	�{�^���̐�
 */
//-----------------------------------------------------------------------------
u32 BR_BTN_DATA_SYS_GetDataNum( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID )
{	
	GF_ASSERT( menuID < BR_MENUID_MAX );
	return sc_btn_data_tbl[menuID].max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ꂼ��̃��j���[�����������j���[�̃{�^���ő吔���擾
 *  @param	p_wk							���[�N
 *	@return	�{�^���̐�
 */
//-----------------------------------------------------------------------------
u32 BR_BTN_DATA_SYS_GetDataMax( const BR_BTN_DATA_SYS *cp_wk )
{	
	int i;
	int max;

	max	= 0;
	for( i = 0; i < BR_MENUID_MAX; i++ )
	{	
		if( max	< BR_BTN_DATA_SYS_GetDataNum( cp_wk, i ) )
		{	
			max	= BR_BTN_DATA_SYS_GetDataNum( cp_wk, i );
		}

	}
	return max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����N���擾
 *
 *	@param	const BR_BTN_DATA_SYS *cp_wk	���[�N
 *	@param	menuID		���j���[ID
 *	@param	idx				�ǂ̂��炢�O�̃����N��	0��1�O�@1��2�O
 *
 *	@return	�����N
 */
//-----------------------------------------------------------------------------
void BR_BTN_DATA_SYS_GetLink( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID, u32 idx, BR_MENUID *p_preID, u32 *p_btnID )
{	
	u32 max;
	max	= BR_BTN_DATA_SYS_GetLinkMax( cp_wk, menuID );

	GF_ASSERT( idx < max );

	if( p_preID )
	{	
		*p_preID	= sc_btn_data_tbl[ menuID ].cp_link[idx].menuID;
	}

	if( p_btnID )
	{	
		*p_btnID	= sc_btn_data_tbl[ menuID ].cp_link[idx].btnID;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����N�����擾
 *
 *	@param	const BR_BTN_DATA_SYS *cp_wk	���[�N
 *	@param	menuID												���j���[ID
 *
 *	@return	�����N��
 */
//-----------------------------------------------------------------------------
u32 BR_BTN_DATA_SYS_GetLinkMax( const BR_BTN_DATA_SYS *cp_wk, BR_MENUID menuID )
{	
	int i;
	GF_ASSERT( menuID < BR_MENUID_MAX );

	for( i = 0; sc_btn_data_tbl[ menuID ].cp_link[i].menuID != BR_BTN_LINK_END; i++ )
	{
		/* nothing */
	}

	return i;
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
