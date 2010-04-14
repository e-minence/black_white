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

#include "savedata/save_outside.h"

FS_EXTERN_OVERLAY(outside_save);

//============================================================================================
//============================================================================================


#define MYSTERY_DATA_NO_USED		0x00000000
#define MYSTERY_MENU_FLAG		(MYSTERY_DATA_MAX_EVENT - 1)

//-------------------------------------
/// MYSTERY_ORIGINAL_DATA��OUTSIDE_MYSTERY�����ʂŎg�����߂̃A�N�Z�T
//=====================================
struct _MYSTERY_DATA
{
  void *p_data_adrs;
  MYSTERY_DATA_TYPE type;
  OUTSIDE_SAVE_CONTROL *p_out_sv;
};

//=============================================================================
/**
 *  MYSTERY_DATA�̃A�N�Z�X�֐�
 *    MYSTERY_DATA��MYSTERY_ORIGINAL_DATA��OUTSIDE_MYSTERY�����ʂɈ�����悤�ɂ��邽��
 *    ����MYSTERY_ORIGINAL_DATA��OUTSIDE_MYSTERY�̕ϐ����g�킸�A
 *    �S�Ċ֐��Ŏ擾���Ă���g���悤�ɂ��Ă�������
 */
//=============================================================================
static void MysteryData_Init( MYSTERY_DATA *p_wk, SAVE_CONTROL_WORK *p_sv, MYSTERYDATA_LOADTYPE type, HEAPID heapID );
static void MysteryData_Exit( MYSTERY_DATA *p_wk );
static MYSTERY_DATA_TYPE MysteryData_GetDataType( const MYSTERY_DATA *cp_wk );
static u32 MysteryData_GetWorkSize( const MYSTERY_DATA *cp_wk );
static u32 MysteryData_GetGiftMax( const MYSTERY_DATA *cp_wk );
static void* MysteryData_GetDataPtr( const MYSTERY_DATA *cp_wk );
static u8* MysteryData_GetRecvFlag( MYSTERY_DATA *p_wk );
static GIFT_PACK_DATA* MysteryData_GetGiftPackData( const MYSTERY_DATA *cp_wk, u32 index );
static RECORD_CRC* MysteryData_GetCrc( MYSTERY_DATA *p_wk );
static void MysteryData_Coded( MYSTERY_DATA *p_wk );
static void MysteryData_Decoded( MYSTERY_DATA *p_wk );

//============================================================================================
//
//		��ɃZ�[�u�V�X�e������Ă΂��֐�
//		�����͊Ǘ����Z�[�u�Ŏd�l����ӏ��Ȃ̂ŁA���ۂɂ��Ȃ�
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
  return sizeof(MYSTERY_ORIGINAL_DATA);
}
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^������
 * @param	fd		�ӂ����Z�[�u�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYSTERYDATA_Init(void * fd)
{
  //�Z�[�u�f�[�^����̂݃A�N�Z�X�����̂�
  //�����Z�[�u�p�ɊǗ��f�[�^�����O�ō쐬
  MYSTERY_DATA  data;
  GFL_STD_MemClear( &data, sizeof(MYSTERY_DATA) );
  data.p_data_adrs  = fd;
  data.type         = MYSTERY_DATA_TYPE_ORIGINAL;

  GFL_STD_MemClear(fd, sizeof(MYSTERY_ORIGINAL_DATA));

  MysteryData_Coded( &data );
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
  GIFT_PACK_DATA* data;
  
  u32 max = MysteryData_GetGiftMax( fd );

  for(i = 0; i < max; i++){
    data  = MysteryData_GetGiftPackData( fd, i );
    if(!MYSTERYDATA_IsIn(data->gift_type)){
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
  GIFT_PACK_DATA* data  = MysteryData_GetGiftPackData( fd, index );
  u32 max = MysteryData_GetGiftMax( fd );

  GF_ASSERT(index < GIFT_DATA_MAX);

  if(index < GIFT_DATA_MAX){
    if(MYSTERYDATA_IsIn(data->gift_type)){
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
  GIFT_PACK_DATA* src;
  GIFT_PACK_DATA* dst;
  u32 max = MysteryData_GetGiftMax( fd );

  if( !_IsExistsCard( fd, cardindex ) )
  { 
    int i;
    for( i = cardindex; i < max-1; i++ )
    { 
      src = MysteryData_GetGiftPackData( fd, i );
      dst = MysteryData_GetGiftPackData( fd, i+1 );
      *src  = *dst;
    }
    src = MysteryData_GetGiftPackData( fd, i );
    GFL_STD_MemClear( src, sizeof(GIFT_PACK_DATA) );
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
  GIFT_PACK_DATA* data  = MysteryData_GetGiftPackData( fd, index );
  u32 max = MysteryData_GetGiftMax( fd );

  GF_ASSERT(index < max);
  
  if(index < max){
    if(MYSTERYDATA_IsIn(data->gift_type)){
      return data;
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

  u32 max = MysteryData_GetGiftMax( fd );

  GF_ASSERT(index < max);
  
  MysteryData_Decoded(fd);
  px = _GetCardData(fd,index);
  if(px){
    GFL_STD_MemCopy(px, pData, sizeof(GIFT_PACK_DATA));
    bRet=TRUE;
  }
  MysteryData_Coded(fd);
  return bRet;
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
  u32 max = MysteryData_GetGiftMax( fd );

  MysteryData_Decoded(fd);
  // �Z�[�u�ł���̈悪������΃Z�[�u���s
  if(_CheckCardDataSpace(fd) == TRUE)
  {
    GIFT_PACK_DATA* src;

    // �J�[�h���Z�[�u����
    for(i = 0; i < max; i++){
      src = MysteryData_GetGiftPackData( fd, i );

      if(!MYSTERYDATA_IsIn(src->gift_type)){
        GFL_STD_MemCopy(gc, src, sizeof(GIFT_PACK_DATA));
        bRet = TRUE;
        break;
      }
    }
  }
  MysteryData_Coded(fd);
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
  u32 max = MysteryData_GetGiftMax( fd );

  GF_ASSERT(index < max);
  MysteryData_Decoded(fd);
  if(index < max){
    //����
    GIFT_PACK_DATA* data = MysteryData_GetGiftPackData( fd, index );
    data->gift_type = MYSTERYGIFT_TYPE_NONE;
    //�󂢂������l�߂�
    _FillSpaceCard( fd, index );
  }
  MysteryData_Coded(fd);
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

  MysteryData_Decoded(fd);
  bChk = _CheckCardDataSpace(fd);
  MysteryData_Coded(fd);

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
  MysteryData_Decoded(fd);
  bChk = _IsExistsCard(fd,index);
  MysteryData_Coded(fd);
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
  u32 max = MysteryData_GetGiftMax( fd );

  GF_ASSERT(index < max);

  MysteryData_Decoded(fd);
  if(index < max){
    GIFT_PACK_DATA* data  = MysteryData_GetGiftPackData( fd, index );
    if(data->have){
      bChk = TRUE;
    }
  }
  MysteryData_Coded(fd);
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
  u32 max = MysteryData_GetGiftMax( fd );
  GF_ASSERT(index < max);
  MysteryData_Decoded(fd);

  if(index < max){
    GIFT_PACK_DATA* data  = MysteryData_GetGiftPackData( fd, index );
    data->have = TRUE;
  }
  MysteryData_Coded(fd);
  
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
  u32 max = MysteryData_GetGiftMax( fd );

  MysteryData_Decoded(fd);
  for(i = 0; i < max; i++){
    if(_IsExistsCard(fd, i) == TRUE){
      bRet = TRUE;
    }
  }
  MysteryData_Coded(fd);
  
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
  u8* recv_flag = MysteryData_GetRecvFlag(fd);

  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);

  MysteryData_Decoded(fd);
  if(num < MYSTERY_DATA_MAX_EVENT){
    if(recv_flag[num / 8] & (1 << (num & 7))){
      bRet = TRUE;
    }
  }
  MysteryData_Coded(fd);

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
  u8* recv_flag = MysteryData_GetRecvFlag(fd);
  GF_ASSERT(num < MYSTERY_DATA_MAX_EVENT);

  MysteryData_Decoded(fd);

  if(num < MYSTERY_DATA_MAX_EVENT){
    recv_flag[num / 8] |= (1 << (num & 7));
  }

  MysteryData_Coded(fd);
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
  MysteryData_Decoded(fd);

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

  MysteryData_Coded(fd);
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u�J�n
 *
 *	@param	MYSTERY_DATA * fd   �s�v�c�Z�[�u�f�[�^
 *	@param	*p_gamedata         �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
void MYSTERYDATA_SaveAsyncStart( MYSTERY_DATA * fd, GAMEDATA *p_gamedata )
{ 
  if( MYSTERY_DATA_TYPE_OUTSIDE == MYSTERY_DATA_GetDataType( fd ) )
  { 
    OutsideSave_SaveAsyncInit( fd->p_out_sv );
  }
  else
  {
    GAMEDATA_SaveAsyncStart(p_gamedata);
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u���C������
 *
 *	@param	MYSTERY_DATA * fd   �s�v�c�Z�[�u�f�[�^
 *	@param	*p_gamedata         �Q�[���f�[�^
 *
 *	@return �Z�[�u����
 */
//-----------------------------------------------------------------------------
SAVE_RESULT MYSTERYDATA_SaveAsyncMain( MYSTERY_DATA * fd, GAMEDATA *p_gamedata )
{ 

  if( MYSTERY_DATA_TYPE_OUTSIDE == MYSTERY_DATA_GetDataType( fd ) )
  { 
    BOOL is_ret;
    is_ret  = OutsideSave_SaveAsyncMain( fd->p_out_sv );

    if( is_ret )
    { 
      return SAVE_RESULT_OK;
    }
    else
    { 
      return SAVE_RESULT_CONTINUE;
    }
  }
  else
  {
    return GAMEDATA_SaveAsyncMain(p_gamedata);
  }
}

//------------------------------------------------------------------
/**
 * @brief	  �Z�[�u�f�[�^����s�v�c�\���̃|�C���^��Ԃ�
 * @param	  sv		�Z�[�u�R���g���[�����[�N
 * @return	MYSTERY_DATA�|�C���^
 */
//------------------------------------------------------------------
#if 0
MYSTERY_DATA * SaveData_GetMysteryData(SAVE_CONTROL_WORK * sv)
{
	return SaveControl_DataPtrGet(sv, GMDATA_ID_MYSTERYDATA);
}
#endif


//----------------------------------------------------------------------------
/**
 *	@brief  �s�v�c�ȑ��蕨�Z�[�u�f�[�^���쐬
 *
 *	@param	SAVE_CONTROL_WORK *p_sv   �Z�[�u�R���g���[�����[�N
 *	@param  type                      �ǂݍ��݃^�C�v
 *	@param	heapID                    �q�[�vID
 *
 *	@return MYSTERY_DATA
 */
//-----------------------------------------------------------------------------
MYSTERY_DATA * MYSTERY_DATA_Load( SAVE_CONTROL_WORK *p_sv, MYSTERYDATA_LOADTYPE type, HEAPID heapID )
{ 
  MYSTERY_DATA *p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_DATA) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DATA) );

  MysteryData_Init( p_wk, p_sv, type, heapID );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s�v�c�ȑ��蕨�Z�[�u�f�[�^��j��
 *
 *	@param	MYSTERY_DATA *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_DATA_UnLoad( MYSTERY_DATA *p_wk )
{ 
  MysteryData_Exit( p_wk );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ő�ۑ��������擾
 *
 *	@param	const MYSTERY_DATA *fd  ���[�N
 *
 *	@return �ő�ۑ�����
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_DATA_GetCardMax( const MYSTERY_DATA *fd )
{ 
  return MysteryData_GetGiftMax( fd );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u�f�[�^�^�C�v���擾
 *
 *	@param	const MYSTERY_DATA *fd ���[�N
 *
 *	@return MYSTERY_DATA_TYPE�񋓂��Q��
 */
//-----------------------------------------------------------------------------
MYSTERY_DATA_TYPE MYSTERY_DATA_GetDataType( const MYSTERY_DATA *fd )
{ 
  return MysteryData_GetDataType( fd );
}
//=============================================================================
/**
 *  MYSTERY_DATA�̃f�[�^�擾
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  MYSTERY_DATA������
 *
 *	@param	MYSTERY_DATA *p_wk  ���[�N
 *	@param	*p_sv               �Z�[�u�f�[�^
 *	@param  type                �ǂݍ��݃^�C�v
 *	@param  HEAPID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MysteryData_Init( MYSTERY_DATA *p_wk, SAVE_CONTROL_WORK *p_sv, MYSTERYDATA_LOADTYPE type, HEAPID heapID )
{ 
  BOOL is_normal_sv_use;

  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DATA) );

  //���[�h�^�C�v
  switch( type )
  { 
  case MYSTERYDATA_LOADTYPE_NORMAL:  //�ʏ�Z�[�u
    is_normal_sv_use  = TRUE;
    break;
  case MYSTERYDATA_LOADTYPE_AUTO:    //�ʏ�Z�[�u�ƊǗ��O�Z�[�u�̎������ʃ��[�h
    is_normal_sv_use  = SaveData_GetExistFlag( p_sv);
    break;
  default:
    is_normal_sv_use  = TRUE;
    GF_ASSERT(0)
  }

  //�ʏ�Z�[�u�f�[�^�����邩�ǂ���
  if( is_normal_sv_use )
  {
    OS_TPrintf( "!!! �s�v�c�ȑ��蕨�ʏ�Z�[�u�f�[�^�ŋN�� !!!\n" );

    //�ʏ�Z�[�u
    p_wk->p_data_adrs = SaveControl_DataPtrGet(p_sv, GMDATA_ID_MYSTERYDATA);
    p_wk->type        = MYSTERY_DATA_TYPE_ORIGINAL;
  }
  else
  {
    OS_TPrintf( "!!! �s�v�c�ȑ��蕨�Ǘ��O�Z�[�u�f�[�^�ŋN�� !!!\n" );

    //�Ǘ��O�Z�[�u
    GFL_OVERLAY_Load( FS_OVERLAY_ID(outside_save) );

    p_wk->p_out_sv    = OutsideSave_SystemLoad( heapID );
    p_wk->p_data_adrs = OutsideSave_GetMysterPtr( p_wk->p_out_sv );
    p_wk->type        = MYSTERY_DATA_TYPE_OUTSIDE;

    //�����f�[�^���Ȃ������玩�O�ŏ���������
    //(MYSTERYDATA_Init�Ɠ����悤�ȏ��������Ǘ��O�Z�[�u�ɂ������čs��)
    if( !OutsideSave_GetExistFlag(p_wk->p_out_sv) )
    { 
      GFL_STD_MemClear( p_wk->p_data_adrs, sizeof(OUTSIDE_MYSTERY) );
      MysteryData_Coded( p_wk );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	MYSTERY_DATA *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MysteryData_Exit( MYSTERY_DATA *p_wk )
{ 
  switch( p_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    OutsideSave_SystemUnload(p_wk->p_out_sv);
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(outside_save) );
    break;
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    break;
  }

  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̎�ނ��擾
 *
 *	@param	const MYSTERY_DATA *cp_wk   ���[�N
 *
 *	@return �f�[�^�̎��
 */
//-----------------------------------------------------------------------------
static MYSTERY_DATA_TYPE MysteryData_GetDataType( const MYSTERY_DATA *cp_wk )
{ 
  return cp_wk->type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�T�C�Y���擾
 *
 *	@param	const MYSTERY_DATA *cp_wk MYSTERY_DATA
 *
 *	@return ���[�N�T�C�Y
 */
//-----------------------------------------------------------------------------
static u32 MysteryData_GetWorkSize( const MYSTERY_DATA *cp_wk )
{ 
  switch( cp_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    return sizeof( OUTSIDE_MYSTERY );
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    return sizeof( MYSTERY_ORIGINAL_DATA );
  default:
    GF_ASSERT(0);
    return 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̍ő�l�擾
 *
 *	@param	const MYSTERY_DATA *cp_wk   ���[�N
 *
 *	@return �f�[�^�̍ő�l
 */
//-----------------------------------------------------------------------------
static u32 MysteryData_GetGiftMax( const MYSTERY_DATA *cp_wk )
{ 
  switch( cp_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    return OUTSIDE_MYSTERY_MAX;
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    return GIFT_DATA_MAX;
  default:
    GF_ASSERT(0);
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԃ��擾
 *
 *	@param	const MYSTERY_DATA *cp_wk MYSTERY_DATA
 *
 *	@return ����
 */
//-----------------------------------------------------------------------------
static void* MysteryData_GetDataPtr( const MYSTERY_DATA *cp_wk )
{
  return cp_wk->p_data_adrs;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���V�[�u�t���O�A�h���X�擾
 *
 *	@param	const MYSTERY_DATA *cp_wk ���[�N
 *
 *	@return ���[�N���V�[�u�t���O
 */
//-----------------------------------------------------------------------------
static u8* MysteryData_GetRecvFlag( MYSTERY_DATA *p_wk )
{ 
  switch( p_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    { 
      OUTSIDE_MYSTERY * p_data  = MysteryData_GetDataPtr( p_wk );
      return p_data->recv_flag;
    }
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    { 
      MYSTERY_ORIGINAL_DATA * p_data  = MysteryData_GetDataPtr( p_wk );
      return p_data->recv_flag;
    }
  default:
    GF_ASSERT(0);
    return NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s�v�c�Ȃ�������̃f�[�^���擾
 *
 *	@param	const MYSTERY_DATA *cp_wk ���[�N
 *	@param	index �C���f�b�N�X
 *
 *	@return �s�v�c�Ȃ�������̃f�[�^
 */
//-----------------------------------------------------------------------------
static GIFT_PACK_DATA* MysteryData_GetGiftPackData( const MYSTERY_DATA *cp_wk, u32 index )
{ 
  u32 max = MysteryData_GetGiftMax( cp_wk );

  GF_ASSERT( index < max );

  switch( cp_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    { 
      OUTSIDE_MYSTERY * p_data  = MysteryData_GetDataPtr( cp_wk );
      return &p_data->card[ index ];
    }
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    { 
      MYSTERY_ORIGINAL_DATA * p_data  = MysteryData_GetDataPtr( cp_wk );
      return &p_data->card[ index ];
    }
  default:
    GF_ASSERT(0);
    return NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  CRC�̎擾
 *
 *	@param	const MYSTERY_DATA *cp_wk   ���[�N
 *
 *	@return CRC
 */
//-----------------------------------------------------------------------------
static RECORD_CRC* MysteryData_GetCrc( MYSTERY_DATA *p_wk )
{ 
  switch( p_wk->type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    { 
      OUTSIDE_MYSTERY * p_data  = MysteryData_GetDataPtr( p_wk );
      return &p_data->crc;
    }
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    { 
      MYSTERY_ORIGINAL_DATA * p_data  = MysteryData_GetDataPtr( p_wk );
      return &p_data->crc;
    }
  default:
    GF_ASSERT(0);
    return NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Í���
 *
 *	@param	MYSTERY_DATA *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MysteryData_Coded( MYSTERY_DATA *p_wk )
{ 
  void  *p_data = MysteryData_GetDataPtr( p_wk );
  MYSTERY_DATA_TYPE type  = MysteryData_GetDataType( p_wk );

  MYSTERYDATA_Coded( p_data, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	MYSTERY_DATA *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MysteryData_Decoded( MYSTERY_DATA *p_wk )
{ 
  void  *p_data = MysteryData_GetDataPtr( p_wk );
  MYSTERY_DATA_TYPE type  = MysteryData_GetDataType( p_wk );

  MYSTERYDATA_Decoded( p_data, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Í���
 *
 *	@param	void *p_data_adrs �f�[�^�̃A�h���X
 *	@param	type              �f�[�^�^�C�v
 */
//-----------------------------------------------------------------------------
void MYSTERYDATA_Coded( void *p_data_adrs, MYSTERY_DATA_TYPE type )
{ 
  u32   size;
  RECORD_CRC* p_crc;

  switch( type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    size  = sizeof( OUTSIDE_MYSTERY );
    p_crc = &(((OUTSIDE_MYSTERY*)p_data_adrs)->crc);
    break;
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    size  = sizeof( MYSTERY_ORIGINAL_DATA );
    p_crc = &(((MYSTERY_ORIGINAL_DATA*)p_data_adrs)->crc);
    break;
  }

  GF_ASSERT( p_crc->coded_number == 0 );

  p_crc->coded_number = OS_GetVBlankCount() | (OS_GetVBlankCount() << 8);
  if(p_crc->coded_number==0){
    p_crc->coded_number = 1;
  }
  p_crc->crc16ccitt_hash = GFL_STD_CODED_CheckSum( p_data_adrs, size - sizeof(RECORD_CRC));
  GFL_STD_CODED_Coded( p_data_adrs, size - sizeof(RECORD_CRC), 
      p_crc->crc16ccitt_hash + (p_crc->coded_number << 16));
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	void *p_data_adrs �f�[�^�̃A�h���X
 *	@param	type              �f�[�^�^�C�v
 */
//-----------------------------------------------------------------------------
void MYSTERYDATA_Decoded( void *p_data_adrs, MYSTERY_DATA_TYPE type )
{
  u32   size;
  RECORD_CRC* p_crc;

  switch( type )
  { 
  case MYSTERY_DATA_TYPE_OUTSIDE:  //�Ǘ��O�Z�[�u
    size  = sizeof( OUTSIDE_MYSTERY );
    p_crc = &(((OUTSIDE_MYSTERY*)p_data_adrs)->crc);
    break;
  case MYSTERY_DATA_TYPE_ORIGINAL: //�ʏ�̃Z�[�u
    size  = sizeof( MYSTERY_ORIGINAL_DATA );
    p_crc = &(((MYSTERY_ORIGINAL_DATA*)p_data_adrs)->crc);
    break;
  }

  GF_ASSERT(p_crc->coded_number!=0);

  GFL_STD_CODED_Decoded( p_data_adrs, size - sizeof(RECORD_CRC), 
    p_crc->crc16ccitt_hash + (p_crc->coded_number << 16));

  p_crc->coded_number = 0;

}
