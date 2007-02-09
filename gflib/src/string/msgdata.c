//============================================================================================
/**
 * @file	msgdata.c
 * @brief	�e�탁�b�Z�[�W�f�[�^��舵��
 * @author	taya
 */
//============================================================================================
#include "gflib.h"

// ����define��L���ɂ���ƁA�Í������b�Z�[�W�Ή�
//#define  MSGDATA_CODED	

/*------------------------------------------------------------*/
/** ������p�����[�^                                          */
/*------------------------------------------------------------*/
typedef struct {
	u32  offset;	///< �t�@�C���擪����̃o�C�g�I�t�Z�b�g
	u32  len;		///< �������iEOM���܂ށj
}MSG_PARAM_BLOCK;

/*------------------------------------------------------------*/
/** ������f�[�^�w�b�_                                        */
/*------------------------------------------------------------*/
struct _MSGDATA_HEADER{
	#ifndef MSGDATA_CODED
	u16					numMsgs;
	u16					randValue;
	#else
	union {
		u32				numMsgs;
		u32				randValue;
	};
	#endif
	MSG_PARAM_BLOCK		params[];
};


//===============================================================================================
// �C�����C���֐�
//===============================================================================================
//------------------------------------------------------------------
/**
 * �����񂲂Ƃ̃p�����[�^�\���̃I�t�Z�b�g���v�Z
 *
 * @param   strID		
 *
 * @retval  inline int		
 */
//------------------------------------------------------------------
static inline int CalcParamBlockOfs( u32 strID )
{
	return sizeof(MSGDATA_HEADER) + sizeof(MSG_PARAM_BLOCK) * strID;
}


//------------------------------------------------------------------
/**
 * �Í������ꂽ������p�����[�^�\���̂𕜍�����
 *
 * @param   param		�p�����[�^�\���̂ւ̃|�C���^
 * @param   strID		������ID
 * @param   rand		�Í��L�[����
 */
//------------------------------------------------------------------
static inline void DecodeParam( MSG_PARAM_BLOCK* param, u32 strID, u32 rand )
{
#ifdef MSGDATA_CODED
	rand = (rand * 765 * (strID+1)) & 0xffff;
	rand |= (rand << 16);

	param->offset ^= rand;
	param->len ^= rand;
#endif
}


//------------------------------------------------------------------
/**
 * �Í������ꂽ������𕜍�����
 *
 * @param   str			������|�C���^
 * @param   len			������
 * @param   strID		������ID
 * @param   rand		�Í��L�[����
 *
 */
//------------------------------------------------------------------
static inline void DecodeStr( STRCODE* str, u32 len, u32 strID, u16 rand )
{
#ifdef MSGDATA_CODED
	rand = (strID + 1) * 596947;
	while(len--)
	{
		*str ^= rand;
		str++;
		rand += 18749;
	}
#endif
}


/*============================================================================================*/
/*                                                                                            */
/*                                                                                            */
/*  ���b�Z�[�W�f�[�^�̃R���g���[��                                                            */
/*                                                                                            */
/*                                                                                            */
/*============================================================================================*/
//------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^���A�[�J�C�u���烍�[�h����
 *
 * @param   arcID		�A�[�J�C�u�t�@�C���h�c
 * @param   datID		�A�[�J�C�u�t�@�C�����̃��b�Z�[�W�f�[�^�h�c
 * @param   heapID		�q�[�v�h�c
 *
 * @retval  MSGDATA_HEADER*		���b�Z�[�W�f�[�^�|�C���^
 */
//------------------------------------------------------------------
MSGDATA_HEADER*
	GFL_MSG_DataLoad
		( u32 arcID, u32 datID, HEAPID heapID )
{
	return GFL_ARC_DataLoadMalloc( arcID, datID, heapID );
}


//------------------------------------------------------------------
/**
 * ���[�h�������b�Z�[�W�f�[�^���A�����[�h����
 *
 * @param   msgdat		���b�Z�[�W�f�[�^�|�C���^
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_DataUnload
		( MSGDATA_HEADER* msgdat )
{
	GFL_HEAP_FreeMemory( msgdat );
}


//------------------------------------------------------------------
/**
 * ������o�b�t�@�^�I�u�W�F�N�g�ɁA�w�蕶������擾
 *
 * @param   msgdat		���b�Z�[�W�f�[�^�|�C���^
 * @param   strID		������ID
 * @param   dst			������o�b�t�@�^
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_GetStr
		( const MSGDATA_HEADER* msgdat, u32 strID, STRBUF* dst )
{
	if( strID < msgdat->numMsgs )
	{
		MSG_PARAM_BLOCK param;
		STRCODE* str;
		u32 size;

		param = msgdat->params[strID];
		DecodeParam( &param, strID, msgdat->randValue );

		size = param.len * sizeof(STRCODE);
		str = GFL_HEAP_AllocMemory( HeapGetLow( GFL_HEAPID_SYSTEM ), size );

		//�R���o�[�g����Ă��镶������w��̃��[�J���o�b�t�@�ɃR�s�[
		GFL_STD_MemCopy16( (((u8*)msgdat) + param.offset), str, size );

		DecodeStr( str, param.len, strID, msgdat->randValue );
		GFL_STR_SetStringCodeOrderLength( dst, str, param.len );
		GFL_HEAP_FreeMemory( str );
	}
	else
	{
		GF_ASSERT_MSG(0, "strID:%d", strID);
		GFL_STR_BufferClear( dst );
	}
}


//------------------------------------------------------------------
/**
 * ������o�b�t�@�^�I�u�W�F�N�g���������m�ۂ��A�w�蕶������擾
 *
 * @param   msgdat		���b�Z�[�W�f�[�^�|�C���^
 * @param   strID		������ID
 * @param   heapID		�������m�ۗp�̃q�[�vID
 *
 * @retval  �����񂪃R�s�[���ꂽ������o�b�t�@�^�I�u�W�F�N�g�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
STRBUF*
	GFL_MSG_GetStrAlloc
	( const MSGDATA_HEADER* msgdat, u32 strID, HEAPID heapID )
{
	if( strID < msgdat->numMsgs )
	{
		MSG_PARAM_BLOCK param;
		STRCODE* str;
		STRBUF* dst;
		u32 size;

		param = msgdat->params[strID];
		DecodeParam( &param, strID, msgdat->randValue );
		size = param.len * sizeof(STRCODE);
		str = GFL_HEAP_AllocMemory( HeapGetLow( heapID) , size );

		//�R���o�[�g����Ă��镶������w��̃��[�J���o�b�t�@�ɃR�s�[
		GFL_STD_MemCopy16( (((u8*)msgdat) + param.offset), str, size );

		DecodeStr( str, param.len, strID, msgdat->randValue );

		dst = GFL_STR_BufferCreate( param.len, heapID );
		GFL_STR_SetStringCodeOrderLength( dst, str, param.len );

		GFL_HEAP_FreeMemory( str );
		return dst;
	}
	else
	{
		// ���݂��Ȃ��C���f�b�N�X�̗v����������󕶎����Ԃ��Ă���
		GF_ASSERT_MSG(0, "strID:%d", strID);
		return NULL;
	}
}


//------------------------------------------------------------------
/**
 * �w�蕶����݂̂����[�h���w��o�b�t�@�ɃR�s�[�i�A�[�J�C�uID�w��j
 *
 * @param   arcID		�A�[�J�C�u�t�@�C���h�c
 * @param   datID		�A�[�J�C�u�t�@�C�����̃��b�Z�[�W�f�[�^�h�c
 * @param   msgID		���b�Z�[�W�f�[�^���̕�����h�c
 * @param   heapID		�ꎞ�������m�ۗp�̃q�[�v�h�c
 * @param   dst			������R�s�[��o�b�t�@
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_GetStrDirect
		( u32 arcID, u32 datID, u32 strID, HEAPID heapID, STRBUF* dst )
{
	ARCHANDLE*  arcHandle = GFL_ARC_DataHandleOpen( arcID, heapID );
	GFL_MSG_GetStrDirectByHandle( arcHandle, datID, strID, heapID, dst );
	GFL_ARC_DataHandleClose( arcHandle );
}


//------------------------------------------------------------------
/**
 * �w�蕶����݂̂����[�h���w��o�b�t�@�ɃR�s�[�i�A�[�J�C�u�n���h���g�p�j
 *
 * @param   arcHandle	�A�[�J�C�u�n���h��
 * @param   datID		�A�[�J�C�u�t�@�C�����̃��b�Z�[�W�f�[�^�h�c
 * @param   msgID		���b�Z�[�W�f�[�^���̕�����h�c
 * @param   heapID		�ꎞ�������m�ۗp�̃q�[�v�h�c
 * @param   dst			������R�s�[��o�b�t�@
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_GetStrDirectByHandle
		( ARCHANDLE* arcHandle, u32 datID, u32 strID, HEAPID heapID, STRBUF* dst )
{
	MSGDATA_HEADER  header;
	MSG_PARAM_BLOCK  param;
	STRCODE*  str;
	u32 size;

	GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, 0, sizeof(MSGDATA_HEADER), &header );

	if( strID < header.numMsgs )
	{
		GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, CalcParamBlockOfs(strID),
									sizeof(MSG_PARAM_BLOCK), &param );
		DecodeParam( &param, strID, header.randValue );

		size = param.len * sizeof(STRCODE);
		str = GFL_HEAP_AllocMemory( HeapGetLow( heapID ), size );

		GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, param.offset, size, str );
		DecodeStr( str, param.len, strID, header.randValue );
		GFL_STR_SetStringCodeOrderLength( dst, str, param.len );
		GFL_HEAP_FreeMemory( str );
	}
	else
	{
		GF_ASSERT_MSG(0, "datID:%d strID:%d", datID, strID);
	}
}


//------------------------------------------------------------------
/**
 * �w�蕶����݂̂����[�h��A�����ŃA���P�[�g�����������ɃR�s�[���ĕԂ�
 *�i�A�[�J�C�uID�w��j
 *
 * @param   arcID		�A�[�J�C�u�t�@�C���h�c
 * @param   datID		�A�[�J�C�u�t�@�C�����̃��b�Z�[�W�f�[�^�h�c
 * @param   msgID		���b�Z�[�W�f�[�^���̕�����h�c
 * @param   heapID		�ꎞ�������m�ۗp�̃q�[�v�h�c
 *
 * @retval  �����񂪃R�s�[���ꂽ������o�b�t�@�^�I�u�W�F�N�g�ւ̃|�C���^
 */
//------------------------------------------------------------------
STRBUF*
	GFL_MSG_GetStrDirectAlloc
		( u32 arcID, u32 datID, u32 strID, HEAPID heapID )
{
	ARCHANDLE*  arcHandle = GFL_ARC_DataHandleOpen( arcID, heapID );
	STRBUF* ret = GFL_MSG_GetStrDirectAllocByHandle( arcHandle, datID, strID, heapID );
	GFL_ARC_DataHandleClose( arcHandle );

	return ret;
}


//------------------------------------------------------------------
/**
 * �w�蕶����݂̂����[�h��A�����ŃA���P�[�g�����������ɃR�s�[���ĕԂ�
 *�i�A�[�J�C�u�n���h���g�p�j
 *
 * @param   arcHandle	�A�[�J�C�u�n���h��
 * @param   datID		�A�[�J�C�u�t�@�C�����̃��b�Z�[�W�f�[�^�h�c
 * @param   msgID		���b�Z�[�W�f�[�^���̕�����h�c
 * @param   heapID		�ꎞ�������m�ۗp�̃q�[�v�h�c
 *
 * @retval  �����񂪃R�s�[���ꂽ������o�b�t�@�^�I�u�W�F�N�g�ւ̃|�C���^
 */
//------------------------------------------------------------------
STRBUF*
	GFL_MSG_GetStrDirectAllocByHandle
		( ARCHANDLE* arcHandle, u32 datID, u32 strID, HEAPID heapID )
{
	MSGDATA_HEADER header;
	STRCODE* str;
	u32	size;

	GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, 0, sizeof(MSGDATA_HEADER), &header );

	if( strID < header.numMsgs )
	{
		MSG_PARAM_BLOCK  param;
		STRBUF*   dst;

		GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, CalcParamBlockOfs(strID),
									sizeof(MSG_PARAM_BLOCK), &param );
		DecodeParam( &param, strID, header.randValue );

		dst = GFL_STR_BufferCreate( param.len, heapID );
		size = param.len * sizeof(STRCODE);
		str = GFL_HEAP_AllocMemory( HeapGetLow( heapID ), size );

		GFL_ARC_DataLoadOfsByHandle( arcHandle, datID, param.offset, size, str );
		DecodeStr( str, param.len, strID, header.randValue );
		GFL_STR_SetStringCodeOrderLength( dst, str, param.len );
		GFL_HEAP_FreeMemory( str );
		return dst;
	}
	else
	{
		GF_ASSERT_MSG(0, "datID:%d strID:%d", datID, strID);
		return NULL;
	}
}


//------------------------------------------------------------------
/**
 * ���[�h�������b�Z�[�W�f�[�^����ɁA�i�[����Ă��郁�b�Z�[�W�̐���Ԃ�
 *
 * @param   msgdat		���b�Z�[�W�f�[�^�|�C���^
 *
 * @retval  u32			���b�Z�[�W��
 */
//------------------------------------------------------------------
u32
	GFL_MSG_GetMessageCount
		( const MSGDATA_HEADER* msgdat )
{
	return msgdat->numMsgs;
}


//------------------------------------------------------------------
/**
 * �A�[�J�C�u���f�[�^ID����ɁA�i�[����Ă��郁�b�Z�[�W�̐���Ԃ�
 *
 * @param   arcID		�A�[�J�C�uID
 * @param   datID		�f�[�^ID
 *
 * @retval  u32			���b�Z�[�W��
 */
//------------------------------------------------------------------
u32
	GFL_MSG_GetMessageCountDirect
		( u32 arcID, u32 datID )
{
	MSGDATA_HEADER  header;
	GFL_ARC_DataLoadOfs( &header, arcID, datID, 0, sizeof(MSGDATA_HEADER) );

	return header.numMsgs;
}


/*============================================================================================*/
/*                                                                                            */
/*                                                                                            */
/*  ���b�Z�[�W�f�[�^�}�l�[�W��                                                                */
/*                                                                                            */
/*                                                                                            */
/*============================================================================================*/
//----------------------------------------------------
/**
 *  �}�l�[�W�����[�N�\����
 */
//----------------------------------------------------
struct _MSGDATA_MANAGER {

	u16		type;
	HEAPID	heapID;
	u16		arcID;
	u16		datID;

	union {
		MSGDATA_HEADER*  msgData;
		ARCHANDLE*       arcHandle;
	};
};


//------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^�}�l�[�W���쐬
 *
 * @param   type		�}�l�[�W���^�C�v�w��
 * @param   arcID		���b�Z�[�W�f�[�^�̊܂܂��A�[�J�C�u�h�c
 * @param   datID		���b�Z�[�W�f�[�^�{�̂̃A�[�J�C�u���h�c
 * @param   heapID		�������m�ۗp�q�[�v�h�c
 *
 * @retval  MSGDATA_MANAGER*		�쐬�����}�l�[�W�����[�N�|�C���^
 */
//------------------------------------------------------------------
MSGDATA_MANAGER*
	GFL_MSG_ManagerCreate
		( MSGMAN_TYPE type, u32 arcID, u32 datID, HEAPID heapID )
{
	// �}�l�[�W���쐬��������擾���}�l�[�W���p���c�̗��ꂪ�v������葽�����Ȃ̂�
	// �}�l�[�W�����[�N�̓������u���b�N�̌������擾����
	MSGDATA_MANAGER* man = GFL_HEAP_AllocMemory( HeapGetLow( heapID ), sizeof(MSGDATA_MANAGER) );

	if( type == MSGMAN_TYPE_NORMAL )
	{
		man->msgData = GFL_MSG_DataLoad( arcID, datID, heapID );
	}
	else
	{
		man->arcHandle = GFL_ARC_DataHandleOpen(arcID, heapID);
	}
	man->type = type;
	man->arcID = arcID;
	man->datID = datID;
	man->heapID = heapID;

	return man;
}


//------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^�}�l�[�W���̔j��
 *
 * @param   man		�}�l�[�W�����[�N�|�C���^
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_ManagerDelete
		( MSGDATA_MANAGER* man )
{
	if( man )
	{
		switch( man->type ){
		case MSGMAN_TYPE_NORMAL:
			GFL_MSG_DataUnload( man->msgData );
			break;
		case MSGMAN_TYPE_DIRECT:
			GFL_ARC_DataHandleClose( man->arcHandle );
			break;
		}
		GFL_HEAP_FreeMemory( man );
	}
}


//------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^�}�l�[�W�����g���ĕ�������w��o�b�t�@�ɃR�s�[
 *
 * @param   man			�}�l�[�W�����[�N�|�C���^
 * @param   strID		������h�c
 * @param   dst			�R�s�[��o�b�t�@�|�C���^
 *
 */
//------------------------------------------------------------------
void
	GFL_MSG_ManagerGetString
		( const MSGDATA_MANAGER* man, u32 strID, STRBUF* dst )
{
	switch( man->type ){
	case MSGMAN_TYPE_NORMAL:
		GFL_MSG_GetStr( man->msgData, strID, dst );
		break;

	case MSGMAN_TYPE_DIRECT:
		GFL_MSG_GetStrDirectByHandle( man->arcHandle, man->datID, strID, man->heapID, dst );
		break;
	}
}


//------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^�}�l�[�W�����g���ĕ�������擾
 *�i������STRBUF���쐬���ĕԂ��B�쐬���ꂽSTRBUF�̔j���͊e���ōs���j
 *
 * @param   man			�}�l�[�W�����[�N�|�C���^
 * @param   strID		������ID
 *
 * @retval  STRBUF*		�R�s�[��o�b�t�@�|�C���^
 */
//------------------------------------------------------------------
STRBUF*
	GFL_MSG_ManagerAllocString
		( const MSGDATA_MANAGER* man, u32 strID )
{
	switch( man->type ){
	case MSGMAN_TYPE_NORMAL:
		return GFL_MSG_GetStrAlloc( man->msgData, strID, man->heapID );

	case MSGMAN_TYPE_DIRECT:
		return GFL_MSG_GetStrDirectAllocByHandle( man->arcHandle, man->datID, strID, man->heapID );
	}
	return NULL;
}


//------------------------------------------------------------------
/**
 * �i�[����Ă��郁�b�Z�[�W����Ԃ�
 *
 * @param   man		�}�l�[�W�����[�N�|�C���^
 *
 * @retval  u32		���b�Z�[�W��
 */
//------------------------------------------------------------------
u32
	GFL_MSG_ManagerGetMessageCount
		( const MSGDATA_MANAGER* man )
{
	switch( man->type ){
	case MSGMAN_TYPE_NORMAL:
		return GFL_MSG_GetMessageCount( man->msgData );

	case MSGMAN_TYPE_DIRECT:
		return GFL_MSG_GetMessageCountDirect( man->arcID, man->datID );
	}
	return 0;
}


