//======================================================================
/**
 * @file	script_hideitem.c
 * @brief	�X�N���v�g����F�B���A�C�e���֘A
 * @date	05.08.04
 *
 * 01.11.07	Sousuke Tamada
 * 03.04.15	Satoshi Nohara
 * 05.04.25 Hiroyuki Nakamura
 *
 * 09.09.13 tamada  �B���A�C�e���֘A��Ɨ�������
//
//	�E�X�N���v�gID����A�B���A�C�e���t���O���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlag( fsys, GetHideItemFlagNoByScriptId(scr_id) );
//
 */
//======================================================================

#include <gflib.h>
#include "script.h"

#include "script_hideitem.h"

//ID_HIDE_ITEM_OFFSET
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"
//FH_FLAG_START
//#include "../../../resource/fldmapdata/flagwork/flag_define.h"
#include "eventwork_def.h"


//�B���A�C�e��
#ifndef SCRIPT_PL_NULL
u16 GetHideItemFlagNoByScriptId( u16 scr_id );
u16 GetHideItemFlagIndexByScriptId( u16 scr_id );
u8 GetHideItemResponseByScriptId( u16 scr_id );
static BOOL HideItemParamSet( SCRIPT_WORK* sc, u16 scr_id );
void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys );
#endif

//======================================================================
//	�B���A�C�e���f�[�^
//======================================================================
#ifndef SCRIPT_PL_NULL
typedef struct{
	u16 itemno;									//�A�C�e���i���o�[
	u8	num;									//��
	u8	response;								//�����x
	u16	sp;										//����(���g�p)
	u16	index;									//�t���O�C���f�b�N
}HIDE_ITEM_DATA;

//#include "../fielddata/script/hide_item.dat"	//�B���A�C�e���f�[�^
#endif

//======================================================================
//	�B���A�C�e���֘A
//	�E�X�N���v�gID����A�B���A�C�e���t���O���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlag( fsys, GetHideItemFlagNoByScriptId(scr_id) );
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�i���o�[���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "�t���O�i���o�["
 */
//--------------------------------------------------------------
u16 SCRIPT_GetHideItemFlagNoByScriptID( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET + FH_FLAG_START);
}

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�C���f�b�N�X���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "�t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
u16 GetHideItemFlagIndexByScriptId( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET);
}
#endif

//--------------------------------------------------------------
/**
 * 0���ŉB���t���O����������
 * @param   fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @retval  none
 * �a�������̃`�F�b�N�������Ă��܂������A
 * ����̌`��OK�������ł��B(08.06.25)
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static u16 oneday_hide_item1[][2] = {		//�|�S��
	{ ZONE_ID_D24R0103, 52 },
	{ ZONE_ID_D24R0104, 53 },
	{ ZONE_ID_D24R0105, 54 },
	{ ZONE_ID_D24R0105, 55 },
};

#define ONEDAY_HIDE_ITEM1_MAX	( NELEMS(oneday_hide_item1) )

static u16 oneday_hide_item2[] = {			//�\�m�I�̉ԉ�
	58,59,219,220,221,222,
};

#define ONEDAY_HIDE_ITEM2_MAX	( NELEMS(oneday_hide_item2) )
#endif

#ifndef SCRIPT_PL_NULL
void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys )
{
	u8 index;

	//�|�S����2�ӏ��̉B���A�C�e�����������邱�Ƃ�����
	index = ( gf_rand() % ONEDAY_HIDE_ITEM1_MAX );
	OS_Printf( "0_0 index = %d\n", index );
	if( fsys->location->zone_id != oneday_hide_item1[index][0] ){
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item1[index][1]) );
	}

	index = ( gf_rand() % ONEDAY_HIDE_ITEM1_MAX );
	OS_Printf( "0_1 index = %d\n", index );
	if( fsys->location->zone_id != oneday_hide_item1[index][0] ){
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item1[index][1]) );
	}

	//�\�m�I�̉ԉ���2�ӏ��̉B���A�C�e�����������邱�Ƃ�����
	if( fsys->location->zone_id != ZONE_ID_D13R0101 ){
		index = ( gf_rand() % ONEDAY_HIDE_ITEM2_MAX );
		OS_Printf( "1_0 index = %d\n", index );
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item2[index]) );

		index = ( gf_rand() % ONEDAY_HIDE_ITEM2_MAX );
		OS_Printf( "1_1 index = %d\n", index );
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item2[index]) );
	}
	return;
}
#endif

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���̔������擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "����"
 * �B���A�C�e���̃X�N���v�g����������ĂԂ悤�ɂ���I
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
u8 GetHideItemResponseByScriptId( u16 scr_id )
{
	int i;
	u16	index;
	const HIDE_ITEM_DATA* data;

	data	= &hide_item_data[0];
	index	= GetHideItemFlagIndexByScriptId(scr_id);			//�t���O�C���f�b�N�X�擾

	//�T�[�`����
	for( i=0; i < HIDE_ITEM_DATA_MAX ;i++ ){
		if( data[i].index == index ){
			break;
		}
	}

	//������Ȃ�����
	if( i >= HIDE_ITEM_DATA_MAX ){
		GF_ASSERT( (0) && "�B���A�C�e���f�[�^�ɊY������f�[�^������܂���I" );
		return 0;
	}

	return data[i].response;
}
#endif

//--------------------------------------------------------------
/**
 * �B���A�C�e���p�����[�^�����[�N�ɃZ�b�g
 *
 * @param   sc			SCRIPT�^�̃|�C���^
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "0=�Z�b�g���s�A1=�Z�b�g����"
 *
 * �g�p���Ă���I
 * SCWK_PARAM0
 * SCWK_PARAM1
 * SCWK_PARAM2
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static BOOL HideItemParamSet( SCRIPT_WORK* sc, u16 scr_id )
{
	int i;
	u16	index;
	const HIDE_ITEM_DATA* data;
	u16* param0 = getTempWork( sc, SCWK_PARAM0 );
	u16* param1 = getTempWork( sc, SCWK_PARAM1 );
	u16* param2 = getTempWork( sc, SCWK_PARAM2 );

	data	= &hide_item_data[0];
	index	= GetHideItemFlagIndexByScriptId(scr_id);		//�t���O�C���f�b�N�X�擾

	//�T�[�`����
	for( i=0; i < HIDE_ITEM_DATA_MAX ;i++ ){
		if( data[i].index == index ){
			break;
		}
	}

	//������Ȃ�����
	if( i >= HIDE_ITEM_DATA_MAX ){
		GF_ASSERT( (0) && "�B���A�C�e���f�[�^�ɊY������f�[�^������܂���I" );
		return 0;
	}

	*param0 = data[i].itemno;						//�A�C�e���i���o�[
	*param1 = data[i].num;							//��
	*param2 = GetHideItemFlagNoByScriptId(scr_id);	//�t���O�i���o�[

	return 1;
}
#endif

#define DEBUG_HIDE_ITEM_LIST	//�f�o�b�N�L��
//--------------------------------------------------------------
/**
 * ��ʓ��ɂ���B���A�C�e�����������Ċm�ۂ������X�g�ɓo�^
 *
 * @param   fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param   heapid		�q�[�vID
 *
 * @retval  "���X�g�̃A�h���X"
 *
 * ���������Y�ꂸ�ɁI
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
HIDE_ITEM_LIST * HideItem_CreateList( FLDCOMMON_WORK * fsys, int heapid )
{
	HIDE_ITEM_LIST* list;
	const BG_TALK_DATA* bg;
	int	hero_gx,hero_gz,i,max,count,tmp;
	int l,r,u,d;

	count = 0;

	//BG�f�[�^���擾
	max = EventData_GetNowBgTalkDataSize( fsys );
	max++;		//�I���R�[�h������̂�+1

	//�������m��
	list = sys_AllocMemory( heapid, sizeof(HIDE_ITEM_LIST) * max );

	//BG�f�[�^�����݂��Ă��Ȃ�������
	if( max == 1 ){
		//�I���R�[�h�Z�b�g
		list[0].response= HIDE_LIST_RESPONSE_NONE;
		list[0].gx		= 0xffff;
		list[0].gz		= 0xffff;
		return list;
	}

	//BG�f�[�^�擾
	bg	= EventData_GetNowBgTalkData( fsys );
	if( bg == NULL ){
		//�I���R�[�h�Z�b�g
		list[0].response= HIDE_LIST_RESPONSE_NONE;
		list[0].gx		= 0xffff;
		list[0].gz		= 0xffff;
		return list;
	}

	//��l���̈ʒu�擾
	hero_gx = Player_NowGPosXGet( fsys->player );
	hero_gz = Player_NowGPosZGet( fsys->player );

	//�����͈͂��Z�b�g(3D�͈ӎ������̊ȈՔ�)
	l = hero_gx - HIDE_LIST_SX;
	r = hero_gx + HIDE_LIST_SX;
	u = hero_gz - HIDE_LIST_TOP;
	d = hero_gz + HIDE_LIST_BOTTOM;

	//�␳
	if( l < 0 ){ l = 0; }
	//if( r < 0 ){ 0 };
	if( u < 0 ){ u = 0; }
	//if( d < 0 ){ 0 };

#ifdef DEBUG_HIDE_ITEM_LIST
	OS_Printf( "\n�������͈́�\n" );
	OS_Printf( "l = %d\n", l );
	OS_Printf( "r = %d\n", r );
	OS_Printf( "u = %d\n", u );
	OS_Printf( "d = %d\n", d );
#endif

	//BG�f�[�^���T�[�`��������
	for( i=0; i < max ;i++ ){

		//�B���A�C�e���^�C�v�ŁA�܂����肵�Ă��Ȃ�������
		if( (bg[i].type == BG_TALK_TYPE_HIDE) &&
			(CheckEventFlag(fsys, GetHideItemFlagNoByScriptId(bg[i].id)) == 0) ){

			//�����͈͓��ɂ��邩�`�F�b�N
			if( (bg[i].gx >= l) &&
				(bg[i].gx <= r) &&
				(bg[i].gz >= u) &&
				(bg[i].gz <= d) ){

				//�X�N���v�gID����A�B���A�C�e���̔������擾
				list[count].response= GetHideItemResponseByScriptId( bg[i].id );

#if 0
				//���������W�����̂܂ܑ��
				list[count].gx		= bg[i].gx;
				list[count].gz		= bg[i].gz;

				//���[�J�����W
				list[count].gx		= (bg[i].gx % 32);
				list[count].gz		= (bg[i].gz % 32);
#endif	

				//�����0,0�Ƃ��Ď擾�������W
				//(��l���̈ʒu����̍��������߂�)
				tmp = ( hero_gx - bg[i].gx );
				list[count].gx = abs(HIDE_LIST_SX - tmp);
				tmp = ( hero_gz - bg[i].gz );
				list[count].gz = abs(HIDE_LIST_TOP - tmp);

#ifdef DEBUG_HIDE_ITEM_LIST
				OS_Printf( "\n����l���̈ʒu��\n" );
				OS_Printf( "hero_gx = %d\n", hero_gx );
				OS_Printf( "hero_gz = %d\n", hero_gz );

				OS_Printf( "\n���������B���A�C�e���̈ʒu��\n" );
				OS_Printf( "bg[i].gx = %d\n", bg[i].gx );
				OS_Printf( "bg[i].gz = %d\n", bg[i].gz );

				OS_Printf( "\n�������0,0�Ƃ��Ď擾�������W��\n" );
				OS_Printf( "list[count].gx = %d\n", list[count].gx );
				OS_Printf( "list[count].gz = %d\n", list[count].gz );
#endif

				count++;
			}
		}
	}

	//�I���R�[�h�Z�b�g
	list[count].response= HIDE_LIST_RESPONSE_NONE;
	list[count].gx		= 0xffff;
	list[count].gz		= 0xffff;

	return list;
}
#endif


