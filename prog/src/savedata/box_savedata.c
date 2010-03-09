//==============================================================================
/**
 * @file	contest_savedata.c
 * @brief	�{�b�N�X�Z�[�u�f�[�^
 * @author	ariizumi
 * @date	2008.11.07
 */
//==============================================================================

#include <gflib.h>

#include "savedata/box_savedata.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "arc_def.h"

#include "system/main.h"
#include "msg/msg_boxmenu.h"
#include "message.naix"
#include "savedata/save_tbl.h"


static void PPP_RecoverPP(const POKEMON_PASO_PARAM* pp){}  //�_�~�[�֐�
static BOX_TRAY_DATA* BOXTRAYDAT_GetTrayData( const BOX_MANAGER *boxData , const u32 trayNum );
static BOX_SAVEDATA* BOXDAT_GetBoxSaveData( const BOX_MANAGER *boxData );



//#define TRAY_ALL_USE_BIT	((1<<BOX_MAX_TRAY)-1)
//#define TRAY_ALL_USE_BIT	(0b111111111111111111)

//�t���b�V���̃y�[�W���܂����Ȃ��悤��1�g���[���̃f�[�^��256�o�C�g�A���C�����g����
struct _BOX_TRAY_DATA
{
	POKEMON_PASO_PARAM  ppp[BOX_MAX_POS];
//	u8 dummy[TRAY_DUMMY_NUM];
};

struct _BOX_SAVEDATA{
//	BOX_TRAY_DATA		btd[BOX_MAX_TRAY];  //WB���g���[���ʂ̃Z�[�u�ɂȂ�܂��B
	//������艺�̃f�[�^���Q�T�U�o�C�g�A���C�����g���ꂽ�Ƃ���Ƀ}�b�s���O�����
	u32			currentTrayNumber;								//4
//	u32			UseBoxBits;										//4
	STRCODE	trayName[BOX_MAX_TRAY][BOX_TRAYNAME_BUFSIZE];	//2*20*24 = 960
	u8			wallPaper[BOX_MAX_TRAY];						//24
	u8			daisukiBitFlag;									//1
	u8			trayMax;					// �J������Ă�g���C��
	u8			dummy[2];					// �A���C�����g�_�~�[			//2
	
	//�ȉ��̂̏��
	//�|�P�����f�[�^��0x1000*18	�o�C�g = 0x12000
	//�|�P�����f�[�^�ȊO��764�o�C�g = 0x2fc
	//�v0x122fc�o�C�g�@
	//����ɂb�q�b�S�o�C�g��ǉ�����ƁA0x12300�ƂȂ�A256�o�C�g�A���C�����g����A
	//�t�b�^�[���݂̂̏������݂̍ۂɁA�t���b�V���y�[�W���܂����Ȃ��Ȃ�
};


// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
#ifdef PM_DEBUG
#ifdef _NITRO
SDK_COMPILER_ASSERT(sizeof(BOX_TRAY_DATA) == 4080); //�w�b�_�����炵�܂���
SDK_COMPILER_ASSERT(sizeof(BOX_SAVEDATA) == 0x3e0);
#endif
#endif

struct _BOX_MANAGER{
	SAVE_CONTROL_WORK *sv;
};


//==============================================================
// Prototype
//==============================================================
static void boxdata_init( BOX_SAVEDATA* boxdat );


//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^������
 */
//------------------------------------------------------------------
void BOXDAT_Init( BOX_SAVEDATA* boxdat )
{
	boxdata_init(boxdat);
///	SaveData_RequestTotalSave();����ō폜
}
//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^�̑��T�C�Y��Ԃ�
 *
 * @retval  u32		�{�b�N�X�f�[�^�T�C�Y
 */
//------------------------------------------------------------------
u32 BOXDAT_GetTotalSize( void )
{
	OS_Printf("�{�b�N�X�f�[�^�T�C�Y%x\n",sizeof( BOX_SAVEDATA ));
	return sizeof( BOX_SAVEDATA );
}


//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^�̈揉����
 *
 * @param   boxdat		
 *
 */
//------------------------------------------------------------------
static void boxdata_init( BOX_SAVEDATA* boxdat )
{
	u32 i, p;
	GFL_MSGDATA*  msgman;
/*
	for(i = 0; i < BOX_MAX_TRAY; i++)
	{
		for(p = 0; p < BOX_MAX_POS; p++)
		{
			PPP_Clear( &(boxdat->btd[i].ppp[p]) );
		}
	}
*/

	// �ǎ��i���o�[�C���������N���u�ǎ��̎擾�t���O
	for(i = 0, p = 0; i < BOX_MAX_TRAY; i++)
	{
		boxdat->wallPaper[i] = p++;
		if(p >= BOX_NORMAL_WALLPAPER_MAX)
		{
			p = 0;
		}
	}
	boxdat->daisukiBitFlag = 0;


	// �f�t�H���g�{�b�N�X���Z�b�g
	msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_boxmenu_dat, GFL_HEAPID_APP );
	if( msgman )
	{
		for(i = 0 ; i < BOX_MAX_TRAY; i++)
		{
			GFL_MSG_GetStringRaw( msgman, mes_boxmenu_02_02+i, boxdat->trayName[i], BOX_TRAYNAME_BUFSIZE );
		}
		GFL_MSG_Delete( msgman );
	}

	boxdat->currentTrayNumber = 0;

	// �J������Ă�g���C��
	boxdat->trayMax = BOX_MIN_TRAY;
}

static BOX_SAVEDATA* BOXDAT_GetBoxSaveData( const BOX_MANAGER *boxData )
{
  return SaveControl_DataPtrGet(boxData->sv, GMDATA_ID_BOXDATA);
}

//------------------------------------------------------------------
/**
 * �{�b�N�X�S�̂��炩��󂫗̈��T���ă|�P�����f�[�^���i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
BOOL BOXDAT_PutPokemon( BOX_MANAGER* box, const POKEMON_PASO_PARAM* poke )
{
	u32 b;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);

	b = boxData->currentTrayNumber;
	do
	{
		PPP_RecoverPP(poke);

		if( BOXDAT_PutPokemonBox( box, b, poke ) )
		{
			//SaveData_RequestTotalSave();	//����ŕύX
//			BOXDAT_SetTrayUseBit( box, b);
			return TRUE;
		}

//		if( ++b >= BOX_MAX_TRAY )
		if( ++b >= boxData->trayMax )
		{
			b = 0;
		}

	}while( b != boxData->currentTrayNumber );

	// �J������Ă��Ȃ��g���C���`�F�b�N
	if( boxData->trayMax != BOX_MAX_TRAY ){
		BOXDAT_PutPokemonBox( box, boxData->trayMax, poke );
		//SaveData_RequestTotalSave();	//����ŕύX
//		BOXDAT_SetTrayUseBit( box, boxData->trayMax );
		boxData->trayMax += BOX_MIN_TRAY;
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
 * �{�b�N�X���w�肵�ă|�P�����f�[�^�i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		���Ԗڂ̃{�b�N�X�Ɋi�[���邩
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
BOOL BOXDAT_PutPokemonBox( BOX_MANAGER* box, u32 trayNum, const POKEMON_PASO_PARAM* poke )
{
	u32 i;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);

	PPP_RecoverPP(poke);

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

	for(i = 0; i < BOX_MAX_POS; i++)
	{
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
		if( PPP_Get( &(trayData->ppp[i]), ID_PARA_monsno, NULL  ) == 0 )
		{
			trayData->ppp[i] = *poke;
			//SaveData_RequestTotalSave();	//����ŕύX
//			BOXDAT_SetTrayUseBit( box, trayNum);
			return TRUE;
		}
	}

	return FALSE;
}
//------------------------------------------------------------------
/**
 * �{�b�N�X�A�ʒu���w�肵�ă|�P�����f�[�^�i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		���Ԗڂ̃{�b�N�X�Ɋi�[���邩
 * @param   pos			�{�b�N�X���̈ʒu
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
BOOL BOXDAT_PutPokemonPos( BOX_MANAGER* box, u32 trayNum, u32 pos, const POKEMON_PASO_PARAM* poke )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	PPP_RecoverPP(poke);

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

	if(	( trayNum < boxData->trayMax ) && ( pos < BOX_MAX_POS ) ){
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
		trayData->ppp[pos] = *poke;
		//SaveData_RequestTotalSave();	����ŕύX
//		BOXDAT_SetTrayUseBit( box, trayNum);	
		return TRUE;
	}else{
		GF_ASSERT(0);
	}
	return FALSE;
}


//------------------------------------------------------------------
/**
 * �w��ʒu�̃|�P���������ւ���
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		���Ԗڂ̃{�b�N�X��
 * @param   pos1		�{�b�N�X���̈ʒu
 * @param   pos2		�{�b�N�X���̈ʒu
 *
 */
//------------------------------------------------------------------
void BOXDAT_ChangePokeData( BOX_MANAGER * box, u32 tray1, u32 pos1, u32 tray2, u32 pos2 )
{
  BOX_TRAY_DATA * tray1Data = BOXTRAYDAT_GetTrayData( box, tray1 );
  BOX_TRAY_DATA * tray2Data = BOXTRAYDAT_GetTrayData( box, tray2 );

	POKEMON_PASO_PARAM	tmp = tray1Data->ppp[pos1];
	tray1Data->ppp[pos1] = tray2Data->ppp[pos2];
	tray2Data->ppp[pos2] = tmp;
	//SaveData_RequestTotalSave();	����ŕύX
//	BOXDAT_SetTrayUseBit( box, tray1 );
//	BOXDAT_SetTrayUseBit( box, tray2 );
}

//------------------------------------------------------------------
/**
 * �ʒu���w�肵�ă{�b�N�X�̃|�P�����f�[�^�N���A
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		���Ԗڂ̃{�b�N�X��
 * @param   pos			�{�b�N�X���̈ʒu
 *
 */
//------------------------------------------------------------------
void BOXDAT_ClearPokemon( BOX_MANAGER* box, u32 trayNum, u32 pos )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

	if( (pos < BOX_MAX_POS) && (trayNum < BOX_MAX_TRAY) )
	{
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
		PPP_Clear( &(trayData->ppp[pos]) );
		//SaveData_RequestTotalSave();	����ŕύX
//		BOXDAT_SetTrayUseBit( box, trayNum);
	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �J�����g�̃g���C�i���o�[��Ԃ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�J�����g�g���C�i���o�[
 */
//------------------------------------------------------------------
u32 BOXDAT_GetCureentTrayNumber( const BOX_MANAGER* box )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	return boxData->currentTrayNumber;
}

//------------------------------------------------------------------
/**
 * �J������Ă���g���C����Ԃ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�g���C��
 */
//------------------------------------------------------------------
u32 BOXDAT_GetTrayMax( const BOX_MANAGER * box )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	return boxData->trayMax;
}

//------------------------------------------------------------------
/**
 * �g���C���J������
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�g���C��
 */
//------------------------------------------------------------------
u32 BOXDAT_AddTrayMax( BOX_MANAGER * box )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);

	if( boxData->trayMax != BOX_MAX_TRAY ){
		boxData->trayMax += BOX_MIN_TRAY;
	}
	return boxData->trayMax;
}

//------------------------------------------------------------------
/**
 * �P�ł��󂫂̂���g���C�i���o�[��Ԃ��i�J�����g���猟���J�n����j
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�󂫂̂���g���C�i���o�[�^������Ȃ���� BOXDAT_TRAYNUM_ERROR
 */
//------------------------------------------------------------------
u32 BOXDAT_GetEmptyTrayNumber( const BOX_MANAGER* box )
{
	int tray, pos;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);

	tray = boxData->currentTrayNumber;

	while(1)
	{
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,tray);
		for(pos=0; pos<BOX_MAX_POS; pos++)
		{
			if( PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[pos])), ID_PARA_poke_exist, NULL  ) == 0 )
			{
				return tray;
			}
		}
		if( ++tray >= boxData->trayMax )
		{
			tray = 0;
		}
		if( tray == boxData->currentTrayNumber )
		{
			break;
		}
	}

	// �J������Ă��Ȃ��g���C���`�F�b�N
	if( boxData->trayMax != BOX_MAX_TRAY ){
		tray = boxData->trayMax;
		boxData->trayMax += BOX_MIN_TRAY;
		return tray;
	}

	return BOXDAT_TRAYNUM_ERROR;
}
//------------------------------------------------------------------
/**
 * �󂫂̂���g���C�i���o�[�ƃg���C���̈ʒu�����o
 *
 * @param   box			[in] �{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		[in|out] ���o�J�n�g���C�i���o�[���w�聨�ŏ��ɋ󂫂����������g���C�i���o�[������
 * @param   pos			[in|out] ���o�J�n�ʒu���w�聨�ŏ��ɋ󂫂����������g���C���̈ʒu������
 *
 * @retval	�󂫂�����������TRUE, ������Ȃ�������FALSE
 *
 */
//------------------------------------------------------------------
BOOL BOXDAT_GetEmptyTrayNumberAndPos( const BOX_MANAGER* box, int* trayNum, int* pos )
{
	int t, p;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);

	if( *trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		*trayNum = boxData->currentTrayNumber;
	}

	t = *trayNum;
	p = *pos;

	while(1)
	{
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,t);
		for( ; p<BOX_MAX_POS; p++)
		{
			if( PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[p])), ID_PARA_poke_exist, NULL  ) == 0 )
			{
				*trayNum = t;
				*pos = p;
				return TRUE;
			}
		}
		if( ++t >= boxData->trayMax )
		{
			t = 0;
		}
		if( t == (*trayNum) )
		{
			break;
		}
		p = 0;
	}

	// �J������Ă��Ȃ��g���C���`�F�b�N
	if( boxData->trayMax != BOX_MAX_TRAY ){
		*trayNum = boxData->trayMax;
		*pos = 0;
		boxData->trayMax += BOX_MIN_TRAY;
		return TRUE;
	}

	return BOXDAT_TRAYNUM_ERROR;
}

//------------------------------------------------------------------
/**
 * �S�̂���󂫂̐����J�E���g���ĕԂ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�󂫂̐�
 */
//------------------------------------------------------------------
u32 BOXDAT_GetEmptySpaceTotal( const BOX_MANAGER* box )
{
	int t, pos;
	u32 cnt;

	cnt = 0;

	for(t=0; t<BOX_MAX_TRAY; t++)
	{
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,t);
		for(pos=0; pos<BOX_MAX_POS; pos++)
		{
			if( PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[pos])), ID_PARA_poke_exist, NULL  ) == 0 )
			{
				cnt++;
			}
		}
	}

	return cnt;
}
//------------------------------------------------------------------
/**
 * �w��g���C����󂫂̐����J�E���g���ĕԂ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum	�g���C�i���o�[
 *
 * @retval  u32		�󂫂̐�
 */
//------------------------------------------------------------------
u32 BOXDAT_GetEmptySpaceTray( const BOX_MANAGER* box, u32 trayNum )
{
	int pos;
	u32 cnt;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
  BOX_TRAY_DATA *trayData;

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}
	GF_ASSERT( trayNum < BOX_MAX_TRAY );


	cnt = 0;

  trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
	for(pos=0; pos<BOX_MAX_POS; pos++)
	{
		if( PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[pos])), ID_PARA_poke_exist, NULL  ) == 0 )
		{
			cnt++;
		}
	}

	return cnt;
}
//------------------------------------------------------------------
/**
 * �J�����g�̃g���C�i���o�[���Z�b�g
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 * @param   num		�J�����g�g���C�i���o�[
 */
//------------------------------------------------------------------
void BOXDAT_SetCureentTrayNumber( BOX_MANAGER* box, u32 num )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( num < BOX_MAX_TRAY )
	{
		boxData->currentTrayNumber = num;
		//SaveData_RequestTotalSave();	//����ō폜�@�|�P�����ȊO�̃f�[�^�͂����Ȃ�ꍇ�������̂ł��̊֐��Ń��N�G�X�g���Ȃ��Ăn�j
	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �w��g���C�̕ǎ��i���o�[��Ԃ�
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber		�g���C�i���o�[
 *
 * @retval  u32		�ǎ��i���o�[
 */
//------------------------------------------------------------------
u32 BOXDAT_GetWallPaperNumber( const BOX_MANAGER* box, u32 trayNum )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNum < BOX_MAX_TRAY )
	{
		return boxData->wallPaper[trayNum];
	}
	else
	{
		GF_ASSERT(0);
		return 0;
	}
}

//------------------------------------------------------------------
/**
 * ����ŗL���ȕǎ��ԍ����`�F�b�N
 *
 * @param	num		�ǎ��ԍ�
 *
 * @retval	"TRUE = �L��"
 * @retval	"FALSE = ����"
 */
//------------------------------------------------------------------
/*
static BOOL WallPaperNumberCheck( u32 num )
{
	if( num < BOX_NORMAL_WALLPAPER_MAX ||
		( num >= BOX_TOTAL_WALLPAPER_MAX_PL && num < BOX_TOTAL_WALLPAPER_MAX_GS ) ){
		return TRUE;
	}
	return FALSE;
}
*/

//------------------------------------------------------------------
/**
 * �w��g���C�̕ǎ��i���o�[���Z�b�g
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum			�g���C�i���o�[
 * @param   wallPaperNumber	�ǎ��i���o�[
 *
 */
//------------------------------------------------------------------
void BOXDAT_SetWallPaperNumber( BOX_MANAGER* box, u32 trayNum, u32 wallPaperNumber )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

//	if(	trayNum < BOX_MAX_TRAY && WallPaperNumberCheck(wallPaperNumber) == TRUE ){
	if(	trayNum < BOX_MAX_TRAY ){
		boxData->wallPaper[trayNum] = wallPaperNumber;
		//SaveData_RequestTotalSave();	����ō폜�@�|�P�����ȊO�̃f�[�^�͂����Ȃ�ꍇ�������̂ł��̊֐��Ń��N�G�X�g���Ȃ��Ăn�j	
	}else{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �w��g���C�̖��O�擾
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber		�g���C�i���o�[
 * @param   buf				������R�s�[��o�b�t�@
 *
 */
//------------------------------------------------------------------
void BOXDAT_GetBoxName( const BOX_MANAGER* box, u32 trayNumber, STRBUF* buf )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNumber == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNumber = boxData->currentTrayNumber;
	}

	if( trayNumber < BOX_MAX_TRAY )
	{
		GFL_STR_SetStringCode( buf, boxData->trayName[trayNumber] );
	}
	else
	{
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * �w��g���C�̖��O�ݒ�
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber		�g���C�i���o�[
 * @param   src				���O
 *
 */
//------------------------------------------------------------------
void BOXDAT_SetBoxName( BOX_MANAGER* box, u32 trayNumber, const STRBUF* src )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNumber == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNumber = boxData->currentTrayNumber;
	}

	if( trayNumber < BOX_MAX_TRAY )
	{
		GFL_STR_GetStringCode( src, boxData->trayName[trayNumber], BOX_TRAYNAME_BUFSIZE );
		//SaveData_RequestTotalSave();	//����ō폜�@�|�P�����ȊO�̃f�[�^�͂����Ȃ�ꍇ�������̂ł��̊֐��Ń��N�G�X�g���Ȃ��Ăn�j
	}
}
//------------------------------------------------------------------
/**
 * �w��g���C�Ɋi�[����Ă���|�P��������Ԃ�
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber		�g���C�i���o�[
 *
 * @retval  u32		�g���C�Ɋi�[����Ă���|�P������
 */
//------------------------------------------------------------------
u32 BOXDAT_GetPokeExistCount( const BOX_MANAGER* box, u32 trayNum )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

	if( trayNum < BOX_MAX_TRAY )
	{
		int i;
		u32 cnt = 0;
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
		for(i=0; i<BOX_MAX_POS; i++)
		{
			if(PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[i])), ID_PARA_poke_exist, NULL ))
			{
				cnt++;
			}
		}
		return cnt;
	}
	else
	{
		GF_ASSERT(0);
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * �w��g���C�Ɋi�[����Ă���|�P��������Ԃ�(�^�}�S���O)
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber		�g���C�i���o�[
 *
 * @retval  u32		�g���C�Ɋi�[����Ă���|�P������
 */
//------------------------------------------------------------------
u32 BOXDAT_GetPokeExistCount2( const BOX_MANAGER* box, u32 trayNum )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

	if( trayNum < BOX_MAX_TRAY )
	{
		int i;
		u32 cnt = 0;
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
		for(i=0; i<BOX_MAX_POS; i++)
		{
			if(PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[i])), ID_PARA_poke_exist, NULL ))
			{
				if(PPP_Get( (POKEMON_PASO_PARAM*)(&(trayData->ppp[i])), ID_PARA_tamago_flag, NULL ) == 0)	//�^�}�S���O
				{
					cnt++;
				}
			}
		}
		return cnt;
	}
	else
	{
		GF_ASSERT(0);
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * �{�b�N�X�S�̂Ɋ܂܂��|�P��������Ԃ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
u32 BOXDAT_GetPokeExistCountTotal( const BOX_MANAGER* box )
{
	u32 cnt, i;

	for(i=0, cnt=0; i<BOX_MAX_TRAY; i++)
	{
		cnt += BOXDAT_GetPokeExistCount( box, i );
	}
	return cnt;
}

//------------------------------------------------------------------
/**
 * �{�b�N�X�S�̂Ɋ܂܂��|�P��������Ԃ�(�^�}�S���O�Łj
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
u32 BOXDAT_GetPokeExistCount2Total( const BOX_MANAGER* box )
{
	u32 cnt, i;

	for(i=0, cnt=0; i<BOX_MAX_TRAY; i++)
	{
		cnt += BOXDAT_GetPokeExistCount2( box, i );
	}
	return cnt;
}

//------------------------------------------------------------------
/**
 * �{�b�N�X�i�[�|�P�����ɑ΂���PPP_Get
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		�g���C�i���o�[
 * @param   pos			�i�[�ʒu
 * @param   param		PPPGet �p�����[�^
 * @param   buf			PPPGet �o�b�t�@
 *
 * @retval  u32			PPPGet �߂�l
 */
//------------------------------------------------------------------
u32 BOXDAT_PokeParaGet( const BOX_MANAGER* box, u32 trayNum, u32 pos, int param, void* buf )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	BOX_TRAY_DATA *trayData;
	GF_ASSERT((trayNum<BOX_MAX_TRAY)||(trayNum == BOXDAT_TRAYNUM_CURRENT));
	GF_ASSERT(pos<BOX_MAX_POS);

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}
  trayData = BOXTRAYDAT_GetTrayData(box,trayNum);

	return PPP_Get( (POKEMON_PASO_PARAM*)(&trayData->ppp[pos]), param, buf );
}


//------------------------------------------------------------------
/**
 * �{�b�N�X�i�[�|�P�����ɑ΂���PPPPut
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		�g���C�i���o�[
 * @param   pos			�i�[�ʒu
 * @param   param		PPPGet �p�����[�^
 * @param   buf			PPPGet �o�b�t�@
 *
 */
//------------------------------------------------------------------
void BOXDAT_PokeParaPut( BOX_MANAGER* box, u32 trayNum, u32 pos, int param, u32 arg )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
  BOX_TRAY_DATA *trayData;
	GF_ASSERT((trayNum<BOX_MAX_TRAY)||(trayNum == BOXDAT_TRAYNUM_CURRENT));
	GF_ASSERT(pos<BOX_MAX_POS);

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}

  trayData = BOXTRAYDAT_GetTrayData(box,trayNum);
	PPP_Put( (POKEMON_PASO_PARAM*)(&trayData->ppp[pos]), param, arg );
	//SaveData_RequestTotalSave();	//����ŕύX
//	BOXDAT_SetTrayUseBit( box, trayNum);

}










//------------------------------------------------------------------
/**
 * �w��ʒu�̃|�P�����f�[�^�ւ̃|�C���^��Ԃ�
 *�i���܂�g���Ăق����͂Ȃ��j
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   boxNum		�{�b�N�X�i���o�[
 * @param   pos			
 *
 * @retval  POKEMON_PASO_PARAM*		
 */
//------------------------------------------------------------------
POKEMON_PASO_PARAM* BOXDAT_GetPokeDataAddress( const BOX_MANAGER* box, u32 trayNum, u32 pos )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
  BOX_TRAY_DATA *trayData;
	GF_ASSERT( ((trayNum<BOX_MAX_TRAY)||(trayNum == BOXDAT_TRAYNUM_CURRENT)) );
	GF_ASSERT( (pos<BOX_MAX_POS) );

	if( trayNum == BOXDAT_TRAYNUM_CURRENT )
	{
		trayNum = boxData->currentTrayNumber;
	}
  trayData = BOXTRAYDAT_GetTrayData(box,trayNum);

	return (POKEMON_PASO_PARAM*) &(trayData->ppp[pos]);
}






//==============================================================================================
//	�ǉ��ǎ��i�����������N���u�ǎ��j
//==============================================================================================

//------------------------------------------------------------------
/**
 * �ǉ��ǎ���L���ɂ���
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   flag			�L���ɂ���ǎ��t���O
 *
 * @return	none
 */
//------------------------------------------------------------------
void BOXDAT_SetExWallPaperFlag( BOX_MANAGER * box, u32 flag )
{
  BOX_SAVEDATA * boxData = BOXDAT_GetBoxSaveData( box );

	GF_ASSERT( flag == BOX_EX_WALLPAPER_SET_FLAG_1 || flag == BOX_EX_WALLPAPER_SET_FLAG_2 );

	boxData->daisukiBitFlag |= flag;
}

//------------------------------------------------------------------
/**
 * �ǉ��ǎ����L����
 *
 * @param   box				�{�b�N�X�f�[�^�|�C���^
 * @param   flag			�ǎ��t���O
 *
 * @retval	"TURE = �L��"
 * @retval	"FALSE = ����ȊO"
 */
//------------------------------------------------------------------
BOOL BOXDAT_GetExWallPaperFlag( BOX_MANAGER * box, u32 flag )
{
  BOX_SAVEDATA * boxData = BOXDAT_GetBoxSaveData( box );

	GF_ASSERT( flag == BOX_EX_WALLPAPER_SET_FLAG_1 || flag == BOX_EX_WALLPAPER_SET_FLAG_2 );

	if( ( boxData->daisukiBitFlag & flag ) != 0 ){
		return TRUE;
	}
	return FALSE;
}


//==============================================================================================
// ���������N���u�ǎ�
//==============================================================================================

//------------------------------------------------------------------
/**
 * ���������N���u�ǎ����P���A�L���ɂ���
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   number		�L���ɂ���ǎ��i���o�[
 *
 */
//------------------------------------------------------------------
/*
void BOXDAT_SetDaisukiKabegamiFlag( BOX_MANAGER* box, u32 number )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	GF_ASSERT( number < BOX_EX_WALLPAPER_MAX );

	boxData->daisukiBitFlag |= (1 << number);
///	SaveData_RequestTotalSave();����ō폜�@�|�P�����ȊO�̃f�[�^�͂����Ȃ�ꍇ�������̂ł��̊֐��Ń��N�G�X�g���Ȃ��Ăn�j
}
*/

//------------------------------------------------------------------
/**
 * ���������N���u�ǎ����擾���Ă��邩�`�F�b�N
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   number		�`�F�b�N����ǎ��i���o�[
 *
 * @retval  BOOL		TRUE�Ŏ擾���Ă���
 */
//------------------------------------------------------------------
/*
BOOL BOXDAT_GetDaisukiKabegamiFlag( const BOX_MANAGER* box, u32 number )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	GF_ASSERT( number < BOX_EX_WALLPAPER_MAX );

	return (boxData->daisukiBitFlag & (1<<number)) != 0;
}
*/

//------------------------------------------------------------------
/**
 * �擾�������������N���u�ǎ��̑������擾
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
/*
u32 BOXDAT_GetDaiukiKabegamiCount( const BOX_MANAGER* box )
{
	u32 i, cnt;

	for(i=0, cnt=0; i<BOX_EX_WALLPAPER_MAX; i++)
	{
		if( BOXDAT_GetDaisukiKabegamiFlag(box, i) )
		{
			cnt++;
		}
	}
	return cnt;
}
*/

//------------------------------------------------------------------
/**
 * �ҏW���s�����g���[�̃r�b�g�𗧂Ă�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 * @param	inTrayIdx	�g���[�C���f�b�N�X
 *
 * @retval  none
 */
//------------------------------------------------------------------
/*
void BOXDAT_SetTrayUseBit(BOX_MANAGER* box, const u8 inTrayIdx)
{
	u8 bit;
	u32 data = 0;
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	if (inTrayIdx >= BOX_MAX_TRAY){
		GF_ASSERT_MSG(0,"�{�b�N�X�g���[�C���f�b�N�X�s��\n");
		return;
	}

	bit = (0b1);
	data = bit << inTrayIdx;

	boxData->UseBoxBits |= data;
}
*/

//------------------------------------------------------------------
/**
 * �g���[�̑S�r�b�g�𗧂Ă�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 * @param	inTrayIdx	�g���[�C���f�b�N�X
 *
 * @retval  none
 */
//------------------------------------------------------------------
/*
void BOXDAT_SetTrayUseBitAll(BOX_MANAGER* box)
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	boxData->UseBoxBits = TRAY_ALL_USE_BIT;
	OS_Printf("�S�r�b�g�I��%x\n",boxData->UseBoxBits);
}
*/

//------------------------------------------------------------------
/**
 * �g���[�̑S�r�b�g�𗧂Ă�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 * @param	inTrayIdx	�g���[�C���f�b�N�X
 *
 * @retval  none
 */
//------------------------------------------------------------------
/*
void BOXDAT_ClearTrayUseBits(BOX_MANAGER* box)
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	boxData->UseBoxBits = 0;
	OS_Printf("�g���[�ҏW�r�b�g�S�N���A\n");
}
*/

//------------------------------------------------------------------
/**
 * �g���[�̕ҏW�r�b�g�Q���擾
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�ҏW�r�b�g�Q
 */
//------------------------------------------------------------------
/*
u32 BOXDAT_GetTrayUseBits(const BOX_MANAGER* box)
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	return boxData->UseBoxBits;
}
*/

//------------------------------------------------------------------
/**
 * 1�{�b�N�X�̃|�P�����f�[�^�T�C�Y���擾
 *
 *
 * @retval  u32		�f�[�^�T�C�Y
 */
//------------------------------------------------------------------
u32 BOXDAT_GetOneBoxDataSize(void)
{
	return	sizeof(BOX_TRAY_DATA);
}

//------------------------------------------------------------------
/**
 * 18�{�b�N�X�̃|�P�����f�[�^�ȊO�̃_�~�[�P�U�o�C�g�̒��g���`�F�b�N����
 *
 *
 */
//------------------------------------------------------------------
void BOXDAT_CheckBoxDummyData(BOX_MANAGER* box)
{
  //�_�~�[�͖������܂����B
  /*
	u8 i,d;
	for(i = 0; i < BOX_MAX_TRAY; i++){
    BOX_TRAY_DATA *trayData = BOXTRAYDAT_GetTrayData(box,i);
		for(d = 0; d < TRAY_DUMMY_NUM; d++){
			GF_ASSERT( trayData->dummy[d] == 0 );
			trayData->dummy[d] = 0;
		}
	}
	*/

}

BOX_MANAGER * BOX_DAT_InitManager( const HEAPID heapId , SAVE_CONTROL_WORK * sv)
{
	BOX_MANAGER* pData;
	pData = GFL_HEAP_AllocMemory( heapId , sizeof( BOX_MANAGER ) );
	pData->sv = sv;
  {
    BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(pData);
    BOXDAT_Init(boxData);
  }
	return pData;

}
void BOX_DAT_ExitManager( BOX_MANAGER *box )
{
  GFL_HEAP_FreeMemory( box );
}


//---------------------------------------------------------------------------
/**
 * @brief	�{�b�N�X�f�[�^�̃Z�b�g(1�{�b�N�X�P��(30�C))
 * @param	trayIdx		�{�b�N�X�ԍ�
 * @param	dataPtr		�ۑ�����f�[�^�̃|�C���^
 * @param	boxData		�{�b�N�X�Z�[�u�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void BOXDAT_SetPPPData_Tray( u8 trayIdx , void *dataPtr , BOX_MANAGER *box )
{
  BOX_SAVEDATA *boxData = BOXDAT_GetBoxSaveData(box);
	//�ʐM�p�ɐ؂蕪���A��ǃV�[�P���X���͂��ނ��H
	// ������蒼���K�v������
//	GFL_STD_MemCopy( dataPtr , (void*)boxData->ppp[trayIdx] , 0x88*30 );

}

//==============================================================================================
// �{�b�N�X�f�[�^��������
//==============================================================================================

void BOXTRAYDAT_Init( BOX_TRAY_DATA* traydat )
{
  u32 p;
  for(p = 0; p < BOX_MAX_POS; p++)
  {
  	PPP_Clear( &(traydat->ppp[p]) );
  }
}

u32 BOXTRAYDAT_GetTotalSize( void )
{
  return sizeof( BOX_TRAY_DATA );
}

static BOX_TRAY_DATA* BOXTRAYDAT_GetTrayData( const BOX_MANAGER *boxData , const u32 trayNum )
{
  return SaveControl_DataPtrGet(boxData->sv, GMDATA_ID_BOXTRAY_01+trayNum);
}
