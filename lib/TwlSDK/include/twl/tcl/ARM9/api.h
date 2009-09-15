/*---------------------------------------------------------------------------*
  Project:  TwlSDK - TCL - include
  File:     api.h

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_TCL_API_H_
#define TWL_TCL_API_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <nitro/fs.h>
#include <twl/tcl/ARM9/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
  Name: TCL_GetTableBufferSize

  Description: �Ǘ��t�@�C����ǂݍ��ނ̂ɕK�v�ȃo�b�t�@�T�C�Y

  Arguments: 

  Returns: �o�b�t�@�T�C�Y
*-------------------------------------------------------------------------*/
extern u32 TCL_GetTableBufferSize( void );

/*-------------------------------------------------------------------------*
  Name: TCL_GetWorkBufferSize

  Description: �Ǘ��t�@�C������ɕK�v�ȃ��[�N�T�C�Y

  Arguments: 

  Returns: �o�b�t�@�T�C�Y
*-------------------------------------------------------------------------*/
extern u32 TCL_GetWorkBufferSize( void );

/*-------------------------------------------------------------------------*
  Name: TCL_LoadTable

  Description: �Ǘ��e�[�u����ǂݍ���
               �o�b�t�@��32�o�C�g�A���C�����g�Ŋm�ۂ��ĉ�����

  Arguments: pAccessor       : �A�N�Z�T
             tableBuffer     : �ǂݍ��݃o�b�t�@
             tableBufferSize : �o�b�t�@�T�C�Y
                               ( TCL_GetTableBufferSize() �ŗ^������T�C�Y)
             workBuffer      : ���[�N�o�b�t�@
             workBufferSize  : ���[�N�o�b�t�@�T�C�Y
                               ( TCL_GetWorkBufferSize() �ŗ^������T�C�Y)
             pFSResult       : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_LoadTable( TCLAccessor* pAccessor ,
                                void* tableBuffer ,
                                u32 tableBufferSize ,
                                void* workBuffer ,
                                u32 workBufferSize ,
                                FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_CreateTable

  Description: �Ǘ��e�[�u�����\�z����
               �o�b�t�@��32�o�C�g�A���C�����g�Ŋm�ۂ��ĉ�����

  Arguments: pAccessor       : �A�N�Z�T
             tableBuffer     : �ǂݍ��݃o�b�t�@
             tableBufferSize : �o�b�t�@�T�C�Y
                               ( TCL_GetTableBufferSize() �ŗ^������T�C�Y)
             workBuffer      : ���[�N�o�b�t�@
             workBufferSize  : ���[�N�o�b�t�@�T�C�Y
                               ( TCL_GetWorkBufferSize() �ŗ^������T�C�Y)
             pFSResult       : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_CreateTable( TCLAccessor* pAccessor ,
                                  void* tableBuffer ,
                                  u32 tableBufferSize ,
                                  void* workBuffer ,
                                  u32 workBufferSize ,
                                  FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_RepairTable

  Description: �Ǘ��e�[�u�����C������

  Arguments: pAccessor : �A�N�Z�T
             pFSResult : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_RepairTable( TCLAccessor* pAccessor , FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_SortTable

  Description: �Ǘ��e�[�u�����\�[�g����

  Arguments: pAccessor : �A�N�Z�T
             sortType  : �\�[�g�^�C�v

  Returns: �Ȃ�
*-------------------------------------------------------------------------*/
extern void TCL_SortTable( TCLAccessor* pAccessor , TCLSortType sortType );

/*-------------------------------------------------------------------------*
  Name: TCL_InitSearchObject

  Description: �����I�u�W�F�N�g�̏�����

  Arguments: pSearchObj : �����I�u�W�F�N�g
             condition  : ��������

  Returns: �Ȃ�
*-------------------------------------------------------------------------*/
extern void TCL_InitSearchObject( TCLSearchObject* pSearchObj , u32 condition );

/*-------------------------------------------------------------------------*
  Name: TCL_GetNumPictures

  Description: ���������ɍ��v����ʐ^���������邩

  Arguments: pAccessor  : �A�N�Z�T
             pSearchObj : �����I�u�W�F�N�g

  Returns: ����
*-------------------------------------------------------------------------*/
extern int TCL_GetNumPictures( const TCLAccessor* pAccessor , const TCLSearchObject* pSearchObj );

/*-------------------------------------------------------------------------*
  Name: TCL_SearchNextPictureInfo

  Description: ���������ɍ��v����A���̎ʐ^�����擾����

  Arguments: pAccessor  : �A�N�Z�T
             ppPicInfo  : �ʐ^���i�[��
             pSearchObj : �����I�u�W�F�N�g

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_SearchNextPictureInfo( const TCLAccessor* pAccessor ,
                                            const TCLPictureInfo** ppPicInfo ,
                                            TCLSearchObject* pSearchObj );

/*-------------------------------------------------------------------------*
  Name: TCL_SearchPictureInfoByIndex

  Description: ���������ɍ��v����A
               �w�肳�ꂽ�C���f�N�X�Ԗڂ̎ʐ^�����擾����

  Arguments: pAccessor  : �A�N�Z�T
             ppPicInfo  : �ʐ^���i�[��
             pSearchObj : �����I�u�W�F�N�g
             index      : �C���f�N�X

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_SearchPictureInfoByIndex( const TCLAccessor* pAccessor ,
                                               const TCLPictureInfo** ppPicInfo ,
                                               const TCLSearchObject* pSearchObj ,
                                               int index );

/*-------------------------------------------------------------------------*
  Name: TCL_SearchNextPicturePath

  Description: ���������ɍ��v����A���̎ʐ^�̃p�X���擾����

  Arguments: pAccessor  : �A�N�Z�T
             path       : �p�X�i�[��
             len        : �p�X��
             pSearchObj : �����I�u�W�F�N�g

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_SearchNextPicturePath( const TCLAccessor* pAccessor ,
                                            char* path ,
                                            size_t len ,
                                            TCLSearchObject* pSearchObj );

/*-------------------------------------------------------------------------*
  Name: TCL_SearchPicturePathByIndex

  Description: ���������ɍ��v����A
               �w�肳�ꂽ�C���f�N�X�Ԗڂ̎ʐ^�̃p�X���擾����

  Arguments: pAccessor  : �A�N�Z�T
             path       : �p�X�i�[��
             len        : �p�X��
             pSearchObj : �����I�u�W�F�N�g
             index      : �C���f�N�X

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_SearchPicturePathByIndex( const TCLAccessor* pAccessor ,
                                               char* path ,
                                               size_t len ,
                                               const TCLSearchObject* pSearchObj ,
                                               int index );

/*-------------------------------------------------------------------------*
  Name: TCL_PrintPicturePath

  Description: �ʐ^�p�X�̎擾

  Arguments: path     : �p�X�i�[��
             len      : �p�X��
             pPicInfo : �ʐ^���

  Returns: STD_TSNPrintf �̕Ԃ�l
*-------------------------------------------------------------------------*/
extern int TCL_PrintPicturePath( char* path ,
                                 size_t len ,
                                 const TCLPictureInfo* pPicInfo );

/*-------------------------------------------------------------------------*
  Name: TCL_GetPictureInfoFromPath

  Description: �p�X����ʐ^�����擾

  Arguments: pAccessor  : �A�N�Z�T
             pPicInfo   : �ʐ^���
             path       : �p�X�i�[��
             len        : �p�X��

  Returns: �擾�ł����Ȃ�� TRUE ��Ԃ�
*-------------------------------------------------------------------------*/
extern BOOL TCL_GetPictureInfoFromPath( const TCLAccessor* pAccessor ,
                          TCLPictureInfo** ppPicInfo ,
                          const char* path ,
                          size_t len );

/*-------------------------------------------------------------------------*
  Name: TCL_CalcNumEnableToTakePictures

  Description: �ʐ^�B�e�\�����̌v�Z

  Arguments: pAccessor : �A�N�Z�T

  Returns: �B�e�\����(���̏ꍇ�͉��炩�̎��s)
*-------------------------------------------------------------------------*/
extern int TCL_CalcNumEnableToTakePictures( const TCLAccessor* pAccessor );

/*-------------------------------------------------------------------------*
  Name: TCL_GetJpegEncoderBufferSize

  Description: JPEG�G���R�[�h�ɕK�v�ȃo�b�t�@�T�C�Y��Ԃ��܂�
               ������ SSP_GetJpegEncoderBufferSize() ���Ăяo���܂�

  Arguments: option : SSP_GetJpegEncoderBufferSize() �ɓn���I�v�V����

  Returns: SSP_GetJpegEncoderBufferSize() �̕Ԃ�l
*-------------------------------------------------------------------------*/
extern u32 TCL_GetJpegEncoderBufferSize( u32 option );

/*-------------------------------------------------------------------------*
  Name: TCL_EncodeAndWritePicture

  Description: �摜��JPEG�G���R�[�h�ƕۑ��A�Ǘ��t�@�C���̍X�V
               ������ TCL_GetCurrentSecond() ���Ăяo���܂�

  Arguments: pAccessor      : �A�N�Z�T
             imageBuffer    : �摜�o�b�t�@
             jpegBuffer     : JPEG�o�b�t�@
             jpegBufferSize : JPEG�o�b�t�@�T�C�Y
             workBuffer     : JPEG�G���R�[�h�ɕK�v�ȃ��[�N�o�b�t�@
             quality        : JPEG�G���R�[�h�N�I���e�B
             option         : JPEG�G���R�[�h�I�v�V����
             pFSResult      : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_EncodeAndWritePicture( TCLAccessor* pAccessor ,
                                            const void* imageBuffer ,
                                            u8* jpegBuffer ,
                                            u32 jpegBufferSize ,
                                            u8* workBuffer ,
                                            u32 quality ,
                                            u32 option ,
                                            FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_EncodeAndWritePictureEx

  Description: �摜��JPEG�G���R�[�h(���[�U���[�J�[�m�[�g�̐ݒ�)�ƕۑ��A�Ǘ��t�@�C���̍X�V
               ������ TCL_GetCurrentSecond() ���Ăяo���܂�

  Arguments: pAccessor           : �A�N�Z�T
             imageBuffer         : �摜�o�b�t�@
             jpegBuffer          : JPEG�o�b�t�@
             jpegBufferSize      : JPEG�o�b�t�@�T�C�Y
             workBuffer          : JPEG�G���R�[�h�ɕK�v�ȃ��[�N�o�b�t�@
             quality             : JPEG�G���R�[�h�N�I���e�B
             option              : JPEG�G���R�[�h�I�v�V����
             makerNoteBuffer     : ���[�U���[�J�[�m�[�g�o�b�t�@
             makerNoteBufferSize : ���[�U���[�J�[�m�[�g�o�b�t�@�T�C�Y
             pFSResult           : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_EncodeAndWritePictureEx( TCLAccessor* pAccessor ,
                                              const void* imageBuffer ,
                                              u8* jpegBuffer ,
                                              u32 jpegBufferSize ,
                                              u8* workBuffer ,
                                              u32 quality ,
                                              u32 option ,
                                              u8* makerNoteBuffer ,
                                              u16 makerNoteBufferSize ,
                                              FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_GetLastWrittenPicturePath

  Description: �Ō�ɏ����o�����摜�̃p�X��Ԃ�

  Arguments:

  Returns: �Ō�ɏ����o�����摜�̃p�X
*-------------------------------------------------------------------------*/
extern const char* TCL_GetLastWrittenPicturePath( void );

/*-------------------------------------------------------------------------*
  Name: TCL_GetLastWrittenPictureInfo

  Description: �Ō�ɏ����o�����摜�� TCLPictureInfo ��Ԃ�

  Arguments:

  Returns: �Ō�ɏ����o�����摜�� TCLPictureInfo
*-------------------------------------------------------------------------*/
extern const TCLPictureInfo* TCL_GetLastWrittenPictureInfo( void );

/*-------------------------------------------------------------------------*
  Name: TCL_DecodePicture

  Description: JPEG�f�R�[�h���s��

  Arguments: fileBuffer     : �t�@�C���o�b�t�@
             fileBufferSize : �t�@�C���o�b�t�@�T�C�Y
             imageBuffer    : �摜�o�b�t�@
             width          : �f�R�[�h�摜����
             height         : �f�R�[�h�摜�c��
             decodeOption   : �f�R�[�h�I�v�V����

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_DecodePicture( u8* fileBuffer ,
                                    u32 fileBufferSize ,
                                    void* imageBuffer ,
                                    s16 width ,
                                    s16 height ,
                                    u32 decodeOption );

/*-------------------------------------------------------------------------*
  Name: TCL_DeletePicture

  Description: JPEG�t�@�C���̍폜���s��

  Arguments: pAccessor : �A�N�Z�T
             pPicInfo  : �ʐ^���
             pFSResult : FS�֐��ŃG���[�����������ꍇ�� FSResult �̊i�[��

  Returns: TCLResult
*-------------------------------------------------------------------------*/
extern TCLResult TCL_DeletePicture( TCLAccessor* pAccessor ,
                                    const TCLPictureInfo* pPicInfo ,
                                    FSResult* pFSResult );

/*-------------------------------------------------------------------------*
  Name: TCL_IsSameImageType

  Description: �摜�^�C�v�������Ƃ݂Ȃ��邩�ǂ���

  Arguments: pPicInfo   : �ʐ^���
             pMakerNote : ���[�J�[�m�[�g

  Returns: �����Ƃ݂Ȃ���� TRUE
*-------------------------------------------------------------------------*/
extern BOOL TCL_IsSameImageType( const TCLPictureInfo* pPicInfo , const TCLMakerNote* pMakerNote );

/*-------------------------------------------------------------------------*
  Name: TCL_GetCurrentSecond

  Description: ���ݎ���(�b)���擾����
               ������ RTC_GetDateTime() ���Ăяo���܂�
               ���̊֐��� weak �V���{���Œ�`����Ă��܂�
               RTC �ւ̔r���������K�v�ȏꍇ�Ȃǂ͍Ē�`���Ă�������

  Arguments:

  Returns: ���ݎ���(�b)
*-------------------------------------------------------------------------*/
extern s64 TCL_GetCurrentSecond( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_TCL_API_H_ */
#endif
