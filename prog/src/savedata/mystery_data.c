//============================================================================================
/**
 * @file	  mystery_data.c
 * @date	  2009.12.05
 * @author	k.ohno
 * @brief	  �ӂ����ʐM�p�Z�[�u�f�[�^�֘A
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/mystery_data.h"
#include "mystery_data_local.h"

//============================================================================================
//============================================================================================


#define MYSTERY_DATA_NO_USED		0x00000000
#define MYSTERY_MENU_FLAG		(MYSTERY_DATA_MAX_EVENT - 1)


//=============================================================================
/**
 *  �v���g�^�C�v
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief   �Í���
 * @param   rec   
 */
//--------------------------------------------------------------
static void _Coded(MYSTERY_DATA *pMysData)
{
  GF_ASSERT(pMysData->crc.coded_number==0);

  pMysData->crc.coded_number = OS_GetVBlankCount() | (OS_GetVBlankCount() << 8);
  if(pMysData->crc.coded_number==0){
    pMysData->crc.coded_number = 1;
  }
  pMysData->crc.crc16ccitt_hash = GFL_STD_CODED_CheckSum(pMysData, sizeof(MYSTERY_DATA) - sizeof(RECORD_CRC));
  GFL_STD_CODED_Coded(pMysData, sizeof(MYSTERY_DATA) - sizeof(RECORD_CRC), 
    pMysData->crc.crc16ccitt_hash + (pMysData->crc.coded_number << 16));
}

//--------------------------------------------------------------
/**
 * @brief   ������
 * @param   rec   
 */
//--------------------------------------------------------------
static void _Decoded(MYSTERY_DATA *pMysData)
{
  GF_ASSERT(pMysData->crc.coded_number!=0);

  GFL_STD_CODED_Decoded(pMysData, sizeof(MYSTERY_DATA) - sizeof(RECORD_CRC), 
    pMysData->crc.crc16ccitt_hash + (pMysData->crc.coded_number << 16));

  pMysData->crc.coded_number = 0;
}


//============================================================================================
//
//		��ɃZ�[�u�V�X�e������Ă΂��֐�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�T�C�Y�̎擾
 * @return	int		�ӂ����Z�[�u�f�[�^�̃T�C�Y
 */
//------------------------------------------------------------------
int MYSTERYDATA_GetWorkSize(void)
{
  return sizeof(MYSTERY_DATA);
}
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^������
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYSTERYDATA_Init(MYSTERY_DATA * fd)
{
  GFL_STD_MemClear(fd, sizeof(MYSTERY_DATA));

  _Coded( fd );


}

//============================================================================================
//
//		���p���邽�߂ɌĂ΂��A�N�Z�X�֐�
//
//============================================================================================


//------------------------------------------------------------------
/**
 * @brief	�f�[�^���L�����ǂ���
 * @param	gift_type
 * @return	BOOL =TRUE �L��
 */
//------------------------------------------------------------------

static BOOL MYSTERYDATA_IsIn(u16 gift_type)
{
  if((gift_type > MYSTERYGIFT_TYPE_NONE) &&
     (gift_type < MYSTERYGIFT_TYPE_MAX)){
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^���Z�[�u�ł��邩�`�F�b�N
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	size		�f�[�^�̃T�C�Y
 * @return	TRUE: �󂫂����� : FALSE: �󂫃X���b�g������
 */
//------------------------------------------------------------------
static BOOL _CheckCardDataSpace(MYSTERY_DATA *fd)
{
  int i;
  for(i = 0; i < GIFT_DATA_MAX; i++){
    if(!MYSTERYDATA_IsIn(fd->card[i].gift_type)){
      return TRUE;
    }
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�����݂��邩�Ԃ�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	BOOL	TRUE�̎��A���݂���
 */
//------------------------------------------------------------------
static BOOL _IsExistsCard(const MYSTERY_DATA * fd, u32 index)
{
  GF_ASSERT(index < GIFT_DATA_MAX);

  if(index < GIFT_DATA_MAX){
    if(MYSTERYDATA_IsIn(fd->card[index].gift_type)){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�h�f�[�^�̋󂫂��l�߂�
 *
 *	@param	MYSTERY_DATA * fd �ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 *	@param  cardindex         �󂫃J�[�h�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void _FillSpaceCard( MYSTERY_DATA * fd, u32 cardindex )
{ 
  if( !_IsExistsCard( fd, cardindex ) )
  { 
    int i;
    for( i = cardindex; i < GIFT_DATA_MAX-1; i++ )
    { 
      fd->card[i] = fd->card[i+1];
    }
    GFL_STD_MemClear( &fd->card[i], sizeof(GIFT_PACK_DATA) );
  }
}



//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�̎擾 
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		���蕨�f�[�^�̃C���f�b�N�X
 * @param	GIFT_PACK_DATA�̃|�C���^
 * @return	�f�[�^����ꂽ��TRUE
 */
//------------------------------------------------------------------
static GIFT_PACK_DATA* _GetCardData(MYSTERY_DATA *fd, u32 index)
{
  GF_ASSERT(index < GIFT_DATA_MAX);
  
  if(index < GIFT_DATA_MAX){
    if(MYSTERYDATA_IsIn(fd->card[index].gift_type)){
      return &fd->card[index];
    }
  }
  return NULL;
}



//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�̎擾 
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	index		���蕨�f�[�^�̃C���f�b�N�X
 * @param	GIFT_PACK_DATA�̃|�C���^
 * @return	�f�[�^����ꂽ��TRUE
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_GetCardData(MYSTERY_DATA *fd, u32 index,GIFT_PACK_DATA *pData)
{
  BOOL bRet=FALSE;
  GIFT_PACK_DATA* px;
  GF_ASSERT(index < GIFT_DATA_MAX);
  
  _Decoded(fd);
  px = _GetCardData(fd,index);
  if(px){
    GFL_STD_MemCopy(px, pData, sizeof(GIFT_PACK_DATA));
    bRet=TRUE;
  }
  _Coded(fd);
  return bRet;
}

GIFT_PACK_DATA *MYSTERYDATA_GetCardDataOld(MYSTERY_DATA *fd, u32 cardindex)
{
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^���Z�[�u�f�[�^�֓o�^    �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	p		�f�[�^�ւ̃|�C���^
 * @return	TRUE: �Z�[�u�ł��� : FALSE: �󂫃X���b�g����������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_SetCardData(MYSTERY_DATA *fd, const void *p)
{
  int i;
  GIFT_PACK_DATA *gc = (GIFT_PACK_DATA *)p;
  BOOL bRet = FALSE;

  _Decoded(fd);
  // �Z�[�u�ł���̈悪������΃Z�[�u���s
  if(_CheckCardDataSpace(fd) == TRUE)
  {
    // �J�[�h���Z�[�u����
    for(i = 0; i < GIFT_DATA_MAX; i++){
      if(!MYSTERYDATA_IsIn(fd->card[i].gift_type)){
        GFL_STD_MemCopy(gc, &fd->card[i], sizeof(GIFT_PACK_DATA));
        bRet = TRUE;
        break;
      }
    }
  }
  _Coded(fd);
  return bRet;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�𖕏�����  �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYSTERYDATA_RemoveCardData(MYSTERY_DATA *fd, u32 index)
{
  GF_ASSERT(index < GIFT_DATA_MAX);
  _Decoded(fd);
  if(index < GIFT_DATA_MAX){
    //����
    fd->card[index].gift_type = MYSTERYGIFT_TYPE_NONE;
    //�󂢂������l�߂�
    _FillSpaceCard( fd, index );
  }
  _Coded(fd);
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^���Z�[�u�ł��邩�`�F�b�N  �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	size		�f�[�^�̃T�C�Y
 * @return	TRUE: �󂫂����� : FALSE: �󂫃X���b�g������
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_CheckCardDataSpace(MYSTERY_DATA *fd)
{
  BOOL bChk;

  _Decoded(fd);
  bChk = _CheckCardDataSpace(fd);
  _Coded(fd);

  return bChk;
}

//------------------------------------------------------------------
/**
 * @brief	�J�[�h�f�[�^�����݂��邩�Ԃ�    �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @return	BOOL	TRUE�̎��A���݂���
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsExistsCard(MYSTERY_DATA * fd, u32 index)
{
  BOOL bChk;
  _Decoded(fd);
  bChk = _IsExistsCard(fd,index);
  _Coded(fd);
  return bChk;
}

//------------------------------------------------------------------
/**
 * @brief	 �w��̃J�[�h����f�[�^���󂯎��ς݂��Ԃ� �Í������ς�
 * @param	 fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	 index		�C�x���g�ԍ�
 * @return	BOOL	TRUE�̎��A�󂯎��ς�
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsHavePresent(MYSTERY_DATA * fd, u32 index)
{
  BOOL bChk=FALSE;
  GF_ASSERT(index < GIFT_DATA_MAX);

  _Decoded(fd);
  if(index < GIFT_DATA_MAX){
    if(fd->card[index].have){
      bChk = TRUE;
    }
  }
  _Coded(fd);
  return bChk;
}

//------------------------------------------------------------------
/**
 * @brief	 �w��̃J�[�h����f�[�^���󂯎�������Ƃɂ��� �Í������ς�
 * @param	 fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	 index		�C�x���g�ԍ�
 */
//------------------------------------------------------------------
void MYSTERYDATA_SetHavePresent(MYSTERY_DATA * fd, u32 index)
{
  GF_ASSERT(index < GIFT_DATA_MAX);
  _Decoded(fd);

  if(index < GIFT_DATA_MAX){
    fd->card[index].have = TRUE;
  }
  _Coded(fd);
  
}
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ɃJ�[�h�f�[�^�����݂��邩�Ԃ� �Í������ς�
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsExistsCardAll(MYSTERY_DATA *fd)
{
  int i;
  BOOL bRet=FALSE;

  _Decoded(fd);
  for(i = 0; i < GIFT_DATA_MAX; i++){
    if(_IsExistsCard(fd, i) == TRUE){
      bRet = TRUE;
    }
  }
  _Coded(fd);
  
  return bRet;
}


//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g�͂��łɂ���������Ԃ� �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	BOOL	TRUE�̎��A���łɂ�����Ă���
 */
//------------------------------------------------------------------
BOOL MYSTERYDATA_IsEventRecvFlag(MYSTERY_DATA * fd, u32 num)
{
  BOOL bRet=FALSE;
  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);

  _Decoded(fd);
  if(num < MYSTERY_DATA_MAX_EVENT){
    if(fd->recv_flag[num / 8] & (1 << (num & 7))){
      bRet = TRUE;
    }
  }
  _Coded(fd);

  return bRet;
}


//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g���������t���O�𗧂Ă� �Í������ς�
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	NONE
 */
//------------------------------------------------------------------
void MYSTERYDATA_SetEventRecvFlag(MYSTERY_DATA * fd, u32 num)
{
  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);

  _Decoded(fd);

  if(num < MYSTERY_DATA_MAX_EVENT){
    fd->recv_flag[num / 8] |= (1 << (num & 7));
  }

  _Coded(fd);

}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�h�f�[�^�����ւ��� �Í������ς�
 *
 *	@param	MYSTERY_DATA * fd �ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 *	@param	cardindex1        ����ւ���J�[�h�P
 *	@param	cardindex2        ����ւ���J�[�h�Q
 */
//-----------------------------------------------------------------------------
void MYSTERYDATA_SwapCard( MYSTERY_DATA * fd, u32 cardindex1, u32 cardindex2 )
{ 
  _Decoded(fd);

  if( _IsExistsCard( fd, cardindex1 )
    && _IsExistsCard( fd, cardindex2 ) )
  { 
    GIFT_PACK_DATA  temp;
    GIFT_PACK_DATA  *p_data1;
    GIFT_PACK_DATA  *p_data2;

    p_data1 = _GetCardData( fd, cardindex1 );
    p_data2 = _GetCardData( fd, cardindex2 );

    GFL_STD_MemCopy( p_data1, &temp, sizeof(GIFT_PACK_DATA) );
    GFL_STD_MemCopy( p_data2, p_data1, sizeof(GIFT_PACK_DATA) );
    GFL_STD_MemCopy( &temp, p_data2, sizeof(GIFT_PACK_DATA) );
  }

  _Coded(fd);

}

//------------------------------------------------------------------
/**
 * @brief	  �Z�[�u�f�[�^����s�v�c�\���̃|�C���^��Ԃ�
 * @param	  sv		�Z�[�u�R���g���[�����[�N
 * @return	MYSTERY_DATA�|�C���^
 */
//------------------------------------------------------------------
MYSTERY_DATA * SaveData_GetMysteryData(SAVE_CONTROL_WORK * sv)
{
	return SaveControl_DataPtrGet(sv, GMDATA_ID_MYSTERYDATA);
}


