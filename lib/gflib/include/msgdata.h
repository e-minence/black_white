//============================================================================================
/**
 * @file	gfl_msgdata.h
 * @brief	��������ɑΉ��������b�Z�[�W�f�[�^
 * @author	taya GAME FREAK inc.
 */
//============================================================================================
#ifndef __GFL_MSGDATA_H__
#define __GFL_MSGDATA_H__

#include <strbuf.h>


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------
/**
 *  ���b�Z�[�W�f�[�^�n���h���^
 */
//----------------------------------------------------
typedef struct _GFL_MSGDATA		GFL_MSGDATA;

//----------------------------------------------------
/**
 *  ���b�Z�[�W�f�[�^�̃��[�h����
 */
//----------------------------------------------------
typedef enum {
	GFL_MSG_LOAD_NORMAL,	///< �Œ���̃w�b�_�������I���������ɂ��A�K�v�ɉ����ăt�@�C���A�N�Z�X����B
	GFL_MSG_LOAD_FAST,		///< �K�v�ȑS�Ẵf�[�^���I���������ɂ���B���������K�v�Ȍ��Ŏg�p���Ă��������B
}GflMsgLoadType;



extern GFL_MSGDATA* GFL_MSG_Create( GflMsgLoadType loadType, u16 ArcID, u16 datID, HEAPID heapID );
extern void GFL_MSG_Delete( GFL_MSGDATA* handle );
extern void GFL_MSG_GetString( const GFL_MSGDATA* handle, u32 strID, STRBUF* dst );
extern STRBUF* GFL_MSG_CreateString( const GFL_MSGDATA* handle, u32 strID );
extern u32 GFL_MSG_GetStrCount( const GFL_MSGDATA* handle );


/*============================================================================================*/
/*                                                                                            */
/*  ���[�h�Ώی���̐؂�ւ�����                                                              */
/*                                                                                            */
/*  ����̕�����ID���g���āA���ۂɓǂݏo����镶�����؂�ւ��邽�߂̎d�g�݁B                */
/*  ���{��ł́w�Ђ炪�ȁ|�����x�؂�ւ��Ƃ��A                                                */
/*  ���B�ł́w�p�E���E�ƁE�ɁE���x�T������؂�ւ��Ƃ��Ɏg���Ă��炤���Ƃ�z��B              */
/*                                                                                            */
/*============================================================================================*/

//--------------------------------------------------------------------------------------
/**
 * ���[�h�ΏۂƂȂ錾��ID��ݒ肷��B
 * �ȍ~�A������擾�֐�����Ď擾�ł��镶���񂪐؂�ւ��B
 *
 * @param   langID		����ID
 */
//--------------------------------------------------------------------------------------
extern void GFL_MSGSYS_SetLangID( u8 langID );

//--------------------------------------------------------------------------------------
/**
 * �ݒ肳�ꂽ���[�h�Ώۂ̌���ID���擾�B
 *
 * @retval  u8		�ݒ肳��Ă��錾��ID
 */
//--------------------------------------------------------------------------------------
extern u8 GFL_MSGSYS_GetLangID( void );


//---- �������́�  ������폜���܂� ----

static inline void GFL_MSG_SetLangID( u8 langID )
{
	GFL_MSGSYS_SetLangID( langID );
}








#ifdef __cplusplus
}/* extern "C" */
#endif


#endif	/* #ifndef __GFL_MSGDATA_H__ */

