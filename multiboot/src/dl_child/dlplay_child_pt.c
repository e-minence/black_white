//======================================================================
/**
 *
 * @file	dlplay_child_pt.c	
 * @brief	wb�_�E�����[�h�v���C�@�v���`�i�p�@��ʃR�[�h
 * @author	ariizumi
 * @data	08/10/23
 */
//======================================================================
#include <gflib.h>
#include <string.h>
#include "system/gfl_use.h"
#include "pt_save.h"
#include "arc_def.h"

#include "../../pokemon_wb/prog/src/test/ariizumi/ari_debug.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_func.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_comm_func.h"

#include "dlplay_data_main.h"
#include "dlplay_child_common.h"

#include "pt_save.dat"	//dlplay_child_pt.c �ȊO�œǂނ�
#include "dp_save.dat"	//dlplay_child_pt.c �ȊO�œǂނ�

#define PT_SAVE_SIZE (SECTOR_SIZE * SECTOR_MAX)
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
enum PT_SAVE_TYPE
{
	PT_MAIN_FIRST	= 0,
	PT_BOX_FIRST	= 1,
	PT_MAIN_SECOND = 2,
	PT_BOX_SECOND	= 3,
};

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

BOOL	DLPlayData_PT_LoadData( DLPLAY_DATA_DATA *d_data );
BOOL	DLPlayData_PT_SaveData( DLPLAY_DATA_DATA *d_data );
void	DLPlayData_PT_SetBoxIndex( DLPLAY_DATA_DATA *d_data , DLPLAY_BOX_INDEX *idxData );

u32		DLPlayData_PT_GetStartAddress( const PT_GMDATA_ID id );
u32		DLPlayData_DP_GetStartAddress( const PT_GMDATA_ID id );
u32		DLPlayData_GetStartAddress( const PT_GMDATA_ID id , DLPLAY_CARD_TYPE type );
static	BOOL DLPlayData_CheckDataCorrect( SAVE_FOOTER **pFooterArr , DLPLAY_DATA_DATA *d_data );
static u8	DLPlayData_PT_CompareFooterData( SAVE_FOOTER *fData , BOOL fCorr ,
											 SAVE_FOOTER *sData , BOOL sCorr ,
											 u8	*pos );

void	PT_CalcTool_Decoded(void *data,u32 size,u32 code);
void	PT_CalcTool_Coded(void *data,u32 size,u32 code);
static	u16 PT_CodeRand(u32 *code);
static	u16	PT_PokeParaCheckSum(void *data,u32	size);
static	void	*PokeParaAdrsGet(PT_POKEMON_PARAM *ppp,u32 rnd,u8 id);
static	u8		PokeRareGetPara(u32 id,u32 rnd);
void	PokePasoParaInit(PT_POKEMON_PARAM *ppp);

const u32 DLPlayData_GetSavedataSize( const DLPLAY_CARD_TYPE type );
const u32 DLPlayData_GetBoxDataSize( const DLPLAY_CARD_TYPE type );
const u32 DLPlayData_GetBoxDataStartAddress( const DLPLAY_CARD_TYPE type );


//	�f�[�^�̓ǂݍ���
BOOL	DLPlayData_PT_LoadData( DLPLAY_DATA_DATA *d_data )
{
	const u32 saveSize = DLPlayData_GetSavedataSize( d_data->cardType_ );
	switch( d_data->subSeq_ )
	{
	case 0:	//����������уf�[�^�ǂݍ���
		{
#if DLPLAY_FUNC_USE_PRINT
		char str[256];
		sprintf(str,"Start load data. size:[%d]", saveSize );
		DLPlayFunc_PutString(str,d_data->msgSys_ );
#endif
		d_data->pData_		= GFL_HEAP_AllocClearMemory(  d_data->heapID_, saveSize );
		d_data->pDataMirror_= GFL_HEAP_AllocClearMemory(  d_data->heapID_, saveSize );
	
		d_data->lockID_ = OS_GetLockID();
		GF_ASSERT( d_data->lockID_ != OS_LOCK_ID_ERROR );
		//�v���`�i��4MB�t���b�V��
		CARD_IdentifyBackup( CARD_BACKUP_TYPE_FLASH_4MBITS );
		CARD_LockBackup( (u16)d_data->lockID_ );
		CARD_ReadFlashAsync( 0x0000 , d_data->pData_ , saveSize , NULL , NULL );
		d_data->subSeq_++;
		}
		break;
	case 1:	//�~���[�����O�f�[�^�ǂݍ���
		//�ǂݍ��݂̊�����҂�
		if( CARD_TryWaitBackupAsync() == TRUE )
		{
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
			CARD_ReadFlashAsync( 0x40000 , d_data->pDataMirror_ , saveSize , NULL , NULL );
			d_data->subSeq_++;
			
			DLPlayFunc_PutString("Data1 load complete.",d_data->msgSys_ );
		}

		break;
	case 2:	//�ǂݍ��݂̊�����҂�
		if( CARD_TryWaitBackupAsync() == TRUE )
		{
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
			CARD_UnlockBackup( (u16)d_data->lockID_ );
			OS_ReleaseLockID( (u16)d_data->lockID_ );
			d_data->subSeq_++;
			
			DLPlayFunc_PutString("Data2 load complete.",d_data->msgSys_ );
		}
		break;
	case 3:	//�e�u���b�N��CRC�`�F�b�N
		{
			//�����炭GS�ł͏������ς��
			//�e�u���b�N��CRC���`�F�b�N(�ꉞ�ʏ�f�[�^�����Ă���
			u8 i;
			const u32 boxStartAdd = DLPlayData_GetStartAddress(GMDATA_ID_BOXDATA , d_data->cardType_ );
			SAVE_FOOTER *footer[4];
			footer[PT_MAIN_FIRST]	= (SAVE_FOOTER*)&d_data->pData_[ DLPlayData_GetStartAddress(GMDATA_NORMAL_FOOTER , d_data->cardType_ ) ];
			footer[PT_BOX_FIRST]	= (SAVE_FOOTER*)&d_data->pData_[ DLPlayData_GetStartAddress(GMDATA_BOX_FOOTER , d_data->cardType_ ) ];
			footer[PT_MAIN_SECOND]  = (SAVE_FOOTER*)&d_data->pDataMirror_[ DLPlayData_GetStartAddress(GMDATA_NORMAL_FOOTER , d_data->cardType_ ) ];
			footer[PT_BOX_SECOND]	= (SAVE_FOOTER*)&d_data->pDataMirror_[ DLPlayData_GetStartAddress(GMDATA_BOX_FOOTER , d_data->cardType_ ) ];
#if DEB_ARI
			for( i=0;i<4;i++ ){
				OS_TPrintf("footer[%d] g_count[%2d] b_count[%2d] size[%5d] MGNo[%d] blkID[%02x] crc[%d]\n"
							,i ,footer[i]->g_count ,footer[i]->b_count ,footer[i]->size
							,footer[i]->magic_number ,footer[i]->blk_id ,footer[i]->crc);
			}
#endif
			{
				const BOOL isCorrectData = DLPlayData_CheckDataCorrect( &footer[0] , d_data );
				if( isCorrectData == TRUE )
				{
					if( d_data->boxSavePos_ == DDS_FIRST )
					{
						d_data->pBoxData_ = d_data->pData_ + boxStartAdd;
					}
					else
					{
						d_data->pBoxData_ = d_data->pDataMirror_ + boxStartAdd;
					}
				}
				else
				{
					d_data->errorState_ = DES_MISS_LOAD_BACKUP;
				}

			}

		}
		//GF_ASSERT( d_data->pBoxData_ != NULL );
		d_data->subSeq_++;
		break;
	
 	case 4:	//BOX�̉��(�{���͐e�ł��H
		d_data->subSeq_++;
		break;
	case 5:
		d_data->subSeq_ = 0;
		return TRUE;
		break;
	}
	return FALSE;
}

//�Z�[�u�͂P�̊֐��ŁA�Z�[�u�������҂������X�g�Z�[�u���s���B
BOOL	DLPlayData_PT_SaveData( DLPLAY_DATA_DATA *d_data )
{
	switch( d_data->subSeq_ )
	{
	case 0:	//�f�[�^�̉��H
		{
			u8 i,bi;
			PT_BOX_DATA *pBox = (PT_BOX_DATA*)d_data->pBoxData_;
			//�|�P�����f�[�^�̈Í���
			for( bi=0;bi<BOX_MAX_TRAY;bi++)
			{
				for( i=0;i<BOX_MAX_POS;i++ )
				{
					PT_POKEMON_PARAM *param = &pBox->ppp[bi][i];
					u16 sum;
					sum = PT_PokeParaCheckSum( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 );
					PT_CalcTool_Coded( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 , param->checksum );
				}
			}
			
			//�|�P����������(��������ɈÍ���������̂ŁA�Í����̌�
			GF_ASSERT( d_data->selectBoxNumber_ != SELECT_BOX_INVALID );
			for( i=0;i<BOX_MAX_POS;i++ )
			{
				PT_POKEMON_PARAM *param = &pBox->ppp[d_data->selectBoxNumber_][i];
				PokePasoParaInit( param );
			}
			//footer�̉��H
			//BOX
			{
				SAVE_FOOTER *boxFooter,*mainFooter;
				if( d_data->boxSavePos_ == DDS_FIRST ){
					boxFooter = (SAVE_FOOTER*)&d_data->pData_[ DLPlayData_GetStartAddress(GMDATA_BOX_FOOTER,d_data->cardType_) ];
				}
				else{
					boxFooter = (SAVE_FOOTER*)&d_data->pDataMirror_[ DLPlayData_GetStartAddress(GMDATA_BOX_FOOTER,d_data->cardType_) ];
				}

				if( d_data->mainSavePos_ == DDS_FIRST ){
					mainFooter = (SAVE_FOOTER*)&d_data->pData_[ DLPlayData_GetStartAddress(GMDATA_NORMAL_FOOTER,d_data->cardType_) ];
					mainFooter->crc = MATH_CalcCRC16CCITT( &d_data->crcTable_, d_data->pData_, mainFooter->size - sizeof(SAVE_FOOTER) );
				}
				else{
					mainFooter = (SAVE_FOOTER*)&d_data->pDataMirror_[ DLPlayData_GetStartAddress(GMDATA_NORMAL_FOOTER,d_data->cardType_) ];
					mainFooter->crc = MATH_CalcCRC16CCITT( &d_data->crcTable_, d_data->pDataMirror_, mainFooter->size - sizeof(SAVE_FOOTER) );
				}

				boxFooter->crc = MATH_CalcCRC16CCITT( &d_data->crcTable_, d_data->pBoxData_, boxFooter->size - sizeof(SAVE_FOOTER) );
				GF_ASSERT( boxFooter->g_count == mainFooter->g_count );
				boxFooter->g_count++;
				mainFooter->g_count++;
				boxFooter->b_count++;
				mainFooter->b_count++;
			}
			
			//�J�[�h�̃��b�N
			d_data->lockID_ = OS_GetLockID();
			GF_ASSERT( d_data->lockID_ != OS_LOCK_ID_ERROR );
			CARD_LockBackup( (u16)d_data->lockID_ );
			d_data->subSeq_++;
			
		}
		break;

	case 1:	//�i�s�f�[�^�̃Z�[�u
		{
			//�ǂݍ��񂾂ق��Ƃ͔��Α��ɏ���
			const u32 saveAddress = ( d_data->mainSavePos_ == DDS_FIRST ? 0x40000 : 0x00000 );
			const u32 saveSize = DLPlayData_GetStartAddress( GMDATA_NORMAL_FOOTER+1 , d_data->cardType_ );
			void *pData;
			if( d_data->mainSavePos_ == DDS_FIRST ){
				pData = d_data->pData_;
			}
			else{
				pData = d_data->pDataMirror_;
			}
			//�Z�[�u�J�n�I

			CARD_WriteAndVerifyFlashAsync( saveAddress , pData , saveSize , NULL , NULL );
			
			DLPlayFunc_PutString("saveMain......",d_data->msgSys_ );
			d_data->subSeq_++;
		}

		break;

	case 2:	//BOX�f�[�^��CRC�Z�b�g����Z�[�u(footer�Ƃ��ꏏ�Ɏg������
		if( CARD_TryWaitBackupAsync() == TRUE )
		{
			u32 saveAddress;
			const u32 saveSize = DLPlayData_GetBoxDataSize( d_data->cardType_ )+sizeof( SAVE_FOOTER );
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );

			//�Z�[�u�J�n�I
			//�ǂݍ��񂾂ق��Ƃ͔��Α��ɏ���
			saveAddress = ( d_data->boxSavePos_ == DDS_FIRST ? 0x40000 : 0x00000 );
			saveAddress += DLPlayData_GetStartAddress(GMDATA_ID_BOXDATA,d_data->cardType_);

			CARD_WriteAndVerifyFlashAsync( saveAddress , d_data->pBoxData_ , saveSize-1 , NULL , NULL );
			
			OS_TPrintf("[[%d]]\n",saveSize);
			DLPlayFunc_PutString("saveBox......",d_data->msgSys_);
			d_data->subSeq_++;

		}
		break;
	case 3:	//�������݂̊�����҂�
		if( CARD_TryWaitBackupAsync() == TRUE )
		{
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
			DLPlayFunc_PutString("Save complete.",d_data->msgSys_ );
			//������Lock�͕ێ������܂�
//			CARD_UnlockBackup( (u16)d_data->lockID_ );
//			OS_ReleaseLockID( (u16)d_data->lockID_ );
			d_data->isFinishSaveFirst_ = TRUE;
			d_data->subSeq_++;
		}
		break;
	case 4:	//�Ō�̓�����҂�
		if( d_data->permitLastSaveFirst_ == TRUE )
		{
			d_data->subSeq_++;
		}
		break;
	case 5:	//�Ō�̃Z�[�u
		{
			u32 saveAddress;
			const u32 saveSize = DLPlayData_GetBoxDataSize( d_data->cardType_ )+sizeof( SAVE_FOOTER );
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );

			//�Z�[�u�J�n�I
			//�ǂݍ��񂾂ق��Ƃ͔��Α��ɏ���
			saveAddress = ( d_data->boxSavePos_ == DDS_FIRST ? 0x40000 : 0x00000 );
			saveAddress += DLPlayData_GetStartAddress(GMDATA_ID_BOXDATA,d_data->cardType_);
			saveAddress += saveSize-1;

			CARD_WriteAndVerifyFlashAsync( saveAddress , d_data->pBoxData_+saveSize-1 , 1 , NULL , NULL );
			
			DLPlayFunc_PutString("saveBox......",d_data->msgSys_ );
			d_data->subSeq_++;
		}
		break;

	case 6:	//�ŏI�m�F
		if( CARD_TryWaitBackupAsync() == TRUE )
		{
			GF_ASSERT( CARD_GetResultCode() == CARD_RESULT_SUCCESS );
			DLPlayFunc_PutString("Save complete!!.",d_data->msgSys_ );
			//������Lock�͕ێ������܂�
			CARD_UnlockBackup( (u16)d_data->lockID_ );
			OS_ReleaseLockID( (u16)d_data->lockID_ );
			d_data->subSeq_++;
			d_data->isFinishSaveSecond_ = TRUE;	//2�Ԗڂ͖���
			d_data->isFinishSaveAll_ = TRUE;
		}
		break;
	}
	return FALSE;
}

void	DLPlayData_PT_SetBoxIndex( DLPLAY_DATA_DATA *d_data , DLPLAY_BOX_INDEX *idxData )
{
	u8 i,bi;
	PT_BOX_DATA *pBox = (PT_BOX_DATA*)d_data->pBoxData_;
	char name_str[256];
	u8 num=0;

	//���x����Ǘp�Ƀp�[�\�i���f�[�^�ƃ��x���e�[�u��
	POKEMON_PERSONAL_DATA* perData;
	u32	expTbl[101];
	//�������̎擾�J���͂܂Ƃ߂Ă�����Ⴄ
	perData = GFL_HEAP_AllocMemory( d_data->heapID_ , sizeof(POKEMON_PERSONAL_DATA));

	OS_TPrintf("POKEMON PARAM DECODE!!!\n");
	for( bi=0;bi<BOX_MAX_TRAY;bi++)
	{
		DLPlayFunc_DPTStrCode_To_UTF16( pBox->trayName[bi] , idxData->boxName_[bi] , BOX_MONS_NAME_LEN );
		OS_TPrintf("------BOX[%d]------\n",bi);
		for( i=0;i<BOX_MAX_POS;i++ )
		{
			DLPLAY_MONS_INDEX *pokeData = &idxData->pokeData_[bi][i];
			PT_POKEMON_PARAM *param = &pBox->ppp[bi][i];
			u16 sum;
			PT_CalcTool_Decoded( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 , param->checksum );
			sum = PT_PokeParaCheckSum( &param->paradata , sizeof(POKEMON_PASO_PARAM1)*4 );
			//FIXME:�T���`�F�G�G�G�b�N
			OS_TPrintf("[%2d-%2d] sum[%5d][%5d] ",bi,i,sum ,param->checksum);
			{
				POKEMON_PASO_PARAM1	*ppp1 = (POKEMON_PASO_PARAM1 *)PokeParaAdrsGet(param,param->personal_rnd,ID_POKEPARA1);
				POKEMON_PASO_PARAM2	*ppp2 = (POKEMON_PASO_PARAM2 *)PokeParaAdrsGet(param,param->personal_rnd,ID_POKEPARA2);
				POKEMON_PASO_PARAM3	*ppp3 = (POKEMON_PASO_PARAM3 *)PokeParaAdrsGet(param,param->personal_rnd,ID_POKEPARA3);
				POKEMON_PASO_PARAM4	*ppp4 = (POKEMON_PASO_PARAM4 *)PokeParaAdrsGet(param,param->personal_rnd,ID_POKEPARA4);
				if( ppp1->monsno == 0 ){
					pokeData->name_[0] = 0xffff;
					pokeData->pokeNo_ = 0;
					pokeData->lv_ = 0;
					pokeData->sex_ = 0;
					pokeData->form_ = 0;
					pokeData->rare_ = 0;
					OS_TPrintf("None");
				}
				else
				{
					u8 lv;
					pokeData->pokeNo_	= ppp1->monsno;
					pokeData->sex_		= ppp2->sex;
					pokeData->form_		= ppp2->form_no;
					DLPlayFunc_DPTStrCode_To_UTF16( ppp3->nickname , pokeData->name_ , MONS_NAME_SIZE+EOM_SIZE );
					OS_TPrintf("No[%3d]",ppp1->monsno);
					//���x���v�Z
					GFL_ARC_LoadData( perData , ARCID_POKE_PERSONAL , ppp1->monsno );
					GFL_ARC_LoadData( expTbl , ARCID_GRAWTBL , perData->grow );
					for( lv=1;lv<101;lv++ ){
						if( expTbl[lv] > ppp1->exp){break;}
					}
					pokeData->lv_ = lv-1;
					OS_TPrintf("lv[%2d]",lv-1);
					//���A�`�F�b�N
					if( PokeRareGetPara(ppp1->id_no,param->personal_rnd) == TRUE ){
						pokeData->rare_		= 1;
						OS_TPrintf("Rare!! ");
					}
					else{
						pokeData->rare_		= 0;
					}
					//�^�}�S�`�F�b�N
					if( ppp2->tamago_flag == 1 ||
						param->fusei_tamago_flag == 1 ){
						pokeData->isEgg_ = 1;
						OS_TPrintf("Egg!! ");
					}
					else{
						pokeData->isEgg_ = 0;
					}

					
				}
				OS_TPrintf("\n");
			}
		}
	}
	DLPlayFunc_PutString("Data load & decode is complete.",d_data->msgSys_ );
	
	GFL_HEAP_FreeMemory( perData );
}

//�f�[�^�̊֘A���A���������`�F�b�N����
static	BOOL DLPlayData_CheckDataCorrect( SAVE_FOOTER **pFooterArr , DLPLAY_DATA_DATA *d_data )
{
	u8 i;
	u8	mainDataNum,boxDataNum;
	BOOL isCorrect[4];	//�f�[�^���ۃ`�F�b�N
	//�܂��f�[�^�P�̂̃`�F�b�N
	for( i=0;i<4;i++ )
	{
		const u32 mainEndAdd = DLPlayData_GetStartAddress( GMDATA_NORMAL_FOOTER+1 , d_data->cardType_ );
		const u32 boxEndAdd = DLPlayData_GetStartAddress( GMDATA_BOX_FOOTER+1 , d_data->cardType_ );
		const u32 saveSize = ( i%2==0 ? mainEndAdd : boxEndAdd - mainEndAdd );
		const u32 addOfs = ( i%2==0 ? 0 : mainEndAdd );
		//�f�[�^�t�b�^���琮�������`�F�b�N
		//�}�W�b�N�i���o�[
		if( pFooterArr[i]->magic_number == -1 ){
			OS_TPrintf("Data[%d] is blank\n");
			isCorrect[i] = FALSE;
			continue;
		}
		else if( pFooterArr[i]->magic_number != MAGIC_NUMBER_PT ){
			OS_TPrintf("Data[%d] is invalid MAGIC_NUMBER[%x]\n",pFooterArr[i]->magic_number);
			isCorrect[i] = FALSE;
			continue;
		}
		//�u���b�NID(MAIN��0 BOX��1
		if( pFooterArr[i]->blk_id != ( i%2==0 ? 0 : 1 ) )
		{
			OS_TPrintf(" BlockID is NG!!\n");
			isCorrect[i] = FALSE;
			continue;
		}

		//�T�C�Y�̃`�F�b�N
		if( pFooterArr[i]->size != saveSize )
		{
			OS_TPrintf(" Datasize is NG!!\n");
			isCorrect[i] = FALSE;
			continue;
		}
		//CRC�`�F�b�N
		{
			//�T�C�Y���������̂��m�F���Ă�����Ȃ��ƃ}�Y�C
			const u16 crc = MATH_CalcCRC16CCITT( &d_data->crcTable_ 
					, ( i<2 ? d_data->pData_+addOfs : d_data->pDataMirror_+addOfs )
					, pFooterArr[i]->size - sizeof(SAVE_FOOTER) );
			OS_TPrintf("Data[%d] crc[%d]:[%d]",i,pFooterArr[i]->crc,crc);
			if( pFooterArr[i]->crc != crc ){
				OS_TPrintf(" crc check is NG!!\n");
				isCorrect[i] = FALSE;
				continue;
			}
		}

		isCorrect[i] = TRUE;
	}
	mainDataNum = DLPlayData_PT_CompareFooterData( pFooterArr[ PT_MAIN_FIRST ] ,isCorrect[ PT_MAIN_FIRST ],
												   pFooterArr[ PT_MAIN_SECOND ] ,isCorrect[ PT_MAIN_SECOND ] ,
												   &d_data->mainSavePos_ );
	boxDataNum = DLPlayData_PT_CompareFooterData(  pFooterArr[ PT_BOX_FIRST ] ,isCorrect[ PT_BOX_FIRST ],
												   pFooterArr[ PT_BOX_SECOND ] ,isCorrect[ PT_BOX_SECOND ] ,
												   &d_data->boxSavePos_ );
	if( mainDataNum == 0 || boxDataNum == 0 )
	{
		return FALSE;
	}
	else if( mainDataNum == 1 && boxDataNum == 1 )
	{
		if( d_data->mainSavePos_ == d_data->boxSavePos_ )
		{
			//����Z�[�u
			return TRUE;
		}
		else
		{
			//�Б��j��
			return FALSE;
		}
	}
	else if( mainDataNum == 1 && boxDataNum == 2 )
	{
		//�i�s�f�[�^�j��
		return FALSE;
	}
	else if( mainDataNum == 2 && boxDataNum == 1 )
	{
		if( pFooterArr[ d_data->mainSavePos_*2 ]->g_count ==
			pFooterArr[ d_data->boxSavePos_*2+1]->g_count )
		{
			//����Z�[�u��A�S�̃Z�[�u�Ȃ�
			return TRUE;
		}
		else
		{
			//BOX�f�[�^�j��
			return FALSE;
		}
	}
	else
	{
		if( pFooterArr[ d_data->mainSavePos_*2 ]->g_count ==
			pFooterArr[ d_data->boxSavePos_*2+1]->g_count )
		{
			//���Ȃ�
			return TRUE;
		}
		else
		{
			//�i�s��BOX�̊ԂŃZ�[�u���f
			return FALSE;
		}
	}
	return TRUE;
}

//�f�[�^�̐V�����ق��ƁA�����������擾
static u8	DLPlayData_PT_CompareFooterData( SAVE_FOOTER *fData , BOOL fCorr ,
											 SAVE_FOOTER *sData , BOOL sCorr ,
											 u8 *pos )
{
	if( fCorr == TRUE && sCorr == TRUE )
	{
		//����������
		if( fData->g_count != sData->g_count )
		{
			//�O���[�o�����Ⴄ�̂őS�̃Z�[�u��
			if( fData->b_count > sData->b_count )
			{
				GF_ASSERT( fData->g_count > sData->g_count );
				*pos = DDS_FIRST;
			}
			else if( fData->b_count < sData->b_count )
			{
				GF_ASSERT( fData->g_count < sData->g_count );
				*pos = DDS_SECOND;
			}
			else
			{
				//���肦�Ȃ��G���[(�ꉞ2M�t���b�V��������炵���E�E�E
				GF_ASSERT( FALSE );
				*pos = DDS_FIRST;
				return 0;
			}
		}
		else
		{
			//�O���[�o�����ꏏ�Ȏ��͕����Z�[�u
			if( fData->b_count > sData->b_count )
			{
				*pos = DDS_FIRST;
			}
			else if( fData->b_count < sData->b_count )
			{
				*pos = DDS_SECOND;
			}
			else
			{
				//���肦�Ȃ��G���[(�ꉞ2M�t���b�V��������炵���E�E�E
				GF_ASSERT( FALSE );
				*pos = DDS_FIRST;
				return 0;
			}
		}
		return 2;
	}
	else
	if( fCorr == TRUE && sCorr == FALSE )
	{
		//�Е����������Ă�
		*pos = DDS_FIRST;
		return 1;
	}
	else if( fCorr == FALSE && sCorr == TRUE )
	{
		//�Е����������Ă�
		*pos = DDS_SECOND;
		return 1;
	}
	else
	{
		//�����A�E�g
		*pos = DDS_ERROR;
		return 0;
	}
}

//�e�f�[�^�u���b�N�̊J�n�ʒu�����߂�BPT�ƈ���ăt�b�^����̃u���b�N�Ƃ��Čv�Z����
u32		DLPlayData_PT_GetStartAddress( const PT_GMDATA_ID id )
{
	u32 temp = 0;
	u8 i;
	for( i=0;i<id;i++ ){
		temp += DLPLAY_PT_SAVESIZETABLE[i];
	}
	return temp;
}

//�e�f�[�^�u���b�N�̊J�n�ʒu�����߂�BPT�ƈ���ăt�b�^����̃u���b�N�Ƃ��Čv�Z����
u32		DLPlayData_DP_GetStartAddress( const PT_GMDATA_ID id )
{
	u32 temp = 0;
	u8 i;
	//EMAIL�ȍ~�ɂȂ��PT��p��ID��2�����Ă���̂ŁA�����Œ���
	PT_GMDATA_ID max = id;
	if( max >= GMDATA_ID_EMAIL ){ max-=2; }
	for( i=0;i<max;i++ ){
		temp += DLPLAY_DP_SAVESIZETABLE[i];
	}
	return temp;
}

//�_�C�p�v���`�i�ŋ��ʉ����邽�߂̊֐�
u32		DLPlayData_GetStartAddress( const PT_GMDATA_ID id , DLPLAY_CARD_TYPE type )
{
	if( type == CARD_TYPE_DP ){
		return DLPlayData_DP_GetStartAddress( id );
	}
	else if( type == CARD_TYPE_PT ){
		return DLPlayData_PT_GetStartAddress( id );
	}
	
	GF_ASSERT( NULL );
	return 0;
}

//�p�����[�^�Í����n
//============================================================================================
/**
 *	�Í�����
 *
 * @param[in]	data	�Í�������f�[�^�̃|�C���^
 * @param[in]	size	�Í�������f�[�^�̃T�C�Y
 * @param[in]	code	�Í����L�[�̏����l
 */
//============================================================================================
void	PT_CalcTool_Coded(void *data,u32 size,u32 code)
{
	int	i;
	u16	*data_p=(u16 *)data;

	//�Í��́A�����Í��L�[�Ń}�X�N

	for(i=0;i<size/2;i++){
		data_p[i]^=PT_CodeRand(&code);
	}
}

//============================================================================================
/**
 *	��������
 *
 * @param[in]	data	��������f�[�^�̃|�C���^
 * @param[in]	size	��������f�[�^�̃T�C�Y
 * @param[in]	code	�Í����L�[�̏����l
 */
//============================================================================================
void	PT_CalcTool_Decoded(void *data,u32 size,u32 code)
{
	PT_CalcTool_Coded(data,size,code);
}

//============================================================================================
/**
 *	�����Í��L�[�������[�`��
 *
 * @param[in,out]	code	�Í��L�[�i�[���[�N�̃|�C���^
 *
 * @return	�Í��L�[�i�[���[�N�̏��2�o�C�g���Í��L�[�Ƃ��ĕԂ�
 */
//============================================================================================
static	u16 PT_CodeRand(u32 *code)
{
    code[0] = code[0] *1103515245L + 24691;
    return (u16)(code[0] / 65536L) ;
}
//============================================================================================
/**
 *	�|�P�����p�����[�^�̃`�F�b�N�T���𐶐�
 *
 * @param[in]	data	�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	size	�`�F�b�N�T���𐶐�����f�[�^�̃T�C�Y
 *
 * @return	���������`�F�b�N�T��
 */
//============================================================================================
static	u16	PT_PokeParaCheckSum(void *data,u32	size)
{
	int	i;
	u16	*data_p=(u16 *)data;
	u16	sum=0;

	for(i=0;i<size/2;i++){
		sum+=data_p[i];
	}

	return sum;
}

//ID(pokeno����Ȃ��I)�Ɨ����Ń��A���ǂ������߂�
static u8		PokeRareGetPara(u32 id,u32 rnd)
{
	return((((id&0xffff0000)>>16)^(id&0xffff)^((rnd&0xffff0000)>>16)^(rnd&0xffff))<8);
}

//============================================================================================
/**
 *	�{�b�N�X�|�P�����p�����[�^�\���̓��̃����o�̃A�h���X���擾
 *
 * @param[in]	ppp	�擾�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	rnd	�\���̓���ւ��̃L�[
 * @param[in]	id	���o�����������o�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return	�擾�����A�h���X
 */
//============================================================================================
static	void	*PokeParaAdrsGet( PT_POKEMON_PARAM *ppp,u32 rnd,u8 id)
{
	void	*ret;
	rnd=(rnd&0x0003e000)>>13;

	GF_ASSERT_MSG(rnd<=ID_POKEPARADATA62,"PokeParaAdrsGet:Index Over!");

	switch(rnd){
	case ID_POKEPARADATA11:
	case ID_POKEPARADATA51:
		{
			POKEMON_PARA_DATA11 *PPD11=(POKEMON_PARA_DATA11 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD11->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD11->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD11->ppp3;
					break;

				case ID_POKEPARA4:
					ret=&PPD11->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA12:
	case ID_POKEPARADATA52:
		{
			POKEMON_PARA_DATA12 *PPD12=(POKEMON_PARA_DATA12 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD12->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD12->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD12->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD12->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA13:
	case ID_POKEPARADATA53:
		{
			POKEMON_PARA_DATA13 *PPD13=(POKEMON_PARA_DATA13 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD13->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD13->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD13->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD13->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA14:
	case ID_POKEPARADATA54:
		{
			POKEMON_PARA_DATA14 *PPD14=(POKEMON_PARA_DATA14 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD14->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD14->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD14->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD14->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA15:
	case ID_POKEPARADATA55:
		{
			POKEMON_PARA_DATA15 *PPD15=(POKEMON_PARA_DATA15 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD15->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD15->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD15->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD15->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA16:
	case ID_POKEPARADATA56:
		{
			POKEMON_PARA_DATA16 *PPD16=(POKEMON_PARA_DATA16 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD16->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD16->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD16->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD16->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA21:
	case ID_POKEPARADATA61:
		{
			POKEMON_PARA_DATA21 *PPD21=(POKEMON_PARA_DATA21 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD21->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD21->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD21->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD21->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA22:
	case ID_POKEPARADATA62:
		{
			POKEMON_PARA_DATA22 *PPD22=(POKEMON_PARA_DATA22 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD22->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD22->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD22->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD22->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA23:
		{
			POKEMON_PARA_DATA23 *PPD23=(POKEMON_PARA_DATA23 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD23->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD23->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD23->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD23->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA24:
		{
			POKEMON_PARA_DATA24 *PPD24=(POKEMON_PARA_DATA24 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD24->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD24->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD24->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD24->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA25:
		{
			POKEMON_PARA_DATA25 *PPD25=(POKEMON_PARA_DATA25 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD25->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD25->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD25->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD25->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA26:
		{
			POKEMON_PARA_DATA26 *PPD26=(POKEMON_PARA_DATA26 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD26->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD26->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD26->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD26->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA31:
		{
			POKEMON_PARA_DATA31 *PPD31=(POKEMON_PARA_DATA31 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD31->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD31->ppp2;
					break;

				case ID_POKEPARA3:
					ret=&PPD31->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD31->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA32:
		{
			POKEMON_PARA_DATA32 *PPD32=(POKEMON_PARA_DATA32 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD32->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD32->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD32->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD32->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA33:
		{
			POKEMON_PARA_DATA33 *PPD33=(POKEMON_PARA_DATA33 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD33->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD33->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD33->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD33->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA34:
		{
			POKEMON_PARA_DATA34 *PPD34=(POKEMON_PARA_DATA34 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD34->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD34->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD34->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD34->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA35:
		{
			POKEMON_PARA_DATA35 *PPD35=(POKEMON_PARA_DATA35 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD35->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD35->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD35->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD35->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA36:
		{
			POKEMON_PARA_DATA36 *PPD36=(POKEMON_PARA_DATA36 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD36->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD36->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD36->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD36->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA41:
		{
			POKEMON_PARA_DATA41 *PPD41=(POKEMON_PARA_DATA41 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD41->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD41->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD41->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD41->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA42:
		{
			POKEMON_PARA_DATA42 *PPD42=(POKEMON_PARA_DATA42 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD42->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD42->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD42->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD42->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA43:
		{
			POKEMON_PARA_DATA43 *PPD43=(POKEMON_PARA_DATA43 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD43->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD43->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD43->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD43->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA44:
		{
			POKEMON_PARA_DATA44 *PPD44=(POKEMON_PARA_DATA44 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD44->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD44->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD44->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD44->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA45:
		{
			POKEMON_PARA_DATA45 *PPD45=(POKEMON_PARA_DATA45 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD45->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD45->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD45->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD45->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA46:
		{
			POKEMON_PARA_DATA46 *PPD46=(POKEMON_PARA_DATA46 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD46->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD46->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD46->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD46->ppp4;
					break;
			}
			break;
		}
	}
	return	ret;
}
//============================================================================================
/**
 *	�{�b�N�X�|�P�����p�����[�^�\���̂�����������
 *
 * @param[in]	ppp	����������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 */
//============================================================================================
void	PokePasoParaInit(PT_POKEMON_PARAM *ppp)
{
	MI_CpuClearFast(ppp,sizeof(PT_POKEMON_PARAM));
//�������Z�b�g�����Ƃ���ňÍ���
	PT_CalcTool_Coded(ppp->paradata,sizeof(POKEMON_PASO_PARAM1)*4,ppp->checksum);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	�e��ROM���Ƃ̐��l�擾�֐�
//////////////////////////////////////////////////////////////////////////////////////////////
const u32 DLPlayData_GetSavedataSize( const DLPLAY_CARD_TYPE type )
{
	GF_ASSERT( type < CARD_TYPE_INVALID );
	{
		const sizeArr[] = { PT_SAVE_SIZE , PT_SAVE_SIZE , PT_SAVE_SIZE };
		return sizeArr[type];
	}
}

const u32 DLPlayData_GetBoxDataSize( const DLPLAY_CARD_TYPE type )
{
	GF_ASSERT( type < CARD_TYPE_INVALID );
	switch( type )
	{
	case CARD_TYPE_DP:
		return DLPlayData_DP_GetStartAddress( GMDATA_BOX_FOOTER ) - DLPlayData_DP_GetStartAddress( GMDATA_ID_BOXDATA );
		break;
	case CARD_TYPE_PT:
		return DLPlayData_PT_GetStartAddress( GMDATA_BOX_FOOTER ) - DLPlayData_PT_GetStartAddress( GMDATA_ID_BOXDATA );
		break;
	case CARD_TYPE_GS:
		return 0;
		break;
	}
	return 0;
}

const u32 DLPlayData_GetBoxDataStartAddress( const DLPLAY_CARD_TYPE type )
{
	GF_ASSERT( type < CARD_TYPE_INVALID );
	switch( type )
	{
	case CARD_TYPE_DP:
		return DLPlayData_DP_GetStartAddress( GMDATA_ID_BOXDATA );
		break;
	case CARD_TYPE_PT:
		return DLPlayData_PT_GetStartAddress( GMDATA_ID_BOXDATA );
		break;
	case CARD_TYPE_GS:
		return 0;
		break;
	}
	return 0;
}

