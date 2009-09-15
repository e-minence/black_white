/*---------------------------------------------------------------------------*
  Project:  TwlSDK - TCL - include
  File:     types.h

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
#ifndef TWL_TCL_TYPES_H_
#define TWL_TCL_TYPES_H_

#include <twl/misc.h>
#include <twl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief �����萔
 */
#define TCL_ARCHIVE_NAND   "photo:"

#define TCL_DIR_SUFFIX     "NIN02"
#define TCL_FILE_PREFIX    "HNI_"

#define TCL_ROOT_DIR       "DCIM"
#define TCL_PRIVATE_DIR    "private/ds/app/484E494A"
#define TCL_FRAME_ROOT_DIR TCL_PRIVATE_DIR "/" TCL_ROOT_DIR

/*!
  @brief �萔
 */
enum
{
    TCL_MAX_NUM_NAND_PICTURES =  500 ,

    TCL_MAX_NUM_DIRS           = 900 ,
    TCL_MAX_NUM_FILES_IN_A_DIR = 100 ,

    TCL_AVARAGE_SIZE  =  80 * 1024 , // �ʐ^�̕��σT�C�Y
    TCL_MARGIN_SIZE   = 350 * 1024 , // �ʐ^�ۑ��������v�Z����Ƃ��̃}�[�W���T�C�Y
    TCL_MAX_JPEG_SIZE = 200 * 1024 , // TCL���C�u���������߂�ő��jpeg�T�C�Y

    TCL_DIR_OFFSET  = 100 ,
    TCL_FILE_OFFSET =   1 ,

    TCL_JPEG_WIDTH  = 640 , // TCL���C�u���������߂�摜����
    TCL_JPEG_HEIGHT = 480 , // TCL���C�u���������߂�摜�c��

    TCL_JPEG_DEFAULT_QUALITY  = 80 , // SSP�G���R�[�h���̐����N�I���e�B�l

    // �G���R�[�h���ʂ̃t�@�C���T�C�Y���傫�������ꍇ�̍ăG���R�[�h���̃N�I���e�B�l�̈���������
    TCL_JPEG_QUALITY_STEP = 10 ,

    TCL_PATH_LEN = 64 , // �p�X��

    TCL_CONST_END
};

/*!
  @brief �ΏۃA�[�J�C�u
 */
typedef enum
{
    TCL_TARGET_ARCHIVE_NAND = 0 ,
    TCL_TARGET_ARCHIVE_SD , // ����ł�SD�J�[�h�͎g�p�ł��܂���
    TCL_TARGET_ARCHIVE_MAX
} TCLTargetArchive;

/*!
  @brief ���C�ɓ���^�C�v
 */
typedef enum
{
    TCL_FAVORITE_TYPE_NONE = 0 ,
    TCL_FAVORITE_TYPE_1 ,
    TCL_FAVORITE_TYPE_2 ,
    TCL_FAVORITE_TYPE_3 ,
    TCL_FAVORITE_TYPE_ALL ,
    TCL_FAVORITE_TYPE_MAX = TCL_FAVORITE_TYPE_ALL
} TCLFavoriteType;

/*!
  @brief �摜�^�C�v
 */
typedef enum
{
    TCL_IMAGE_TYPE_PICTURE = 0 ,
    TCL_IMAGE_TYPE_FRAME ,
    TCL_IMAGE_TYPE_COMPOSITE ,
    TCL_IMAGE_TYPE_UNKNOWN ,
    TCL_IMAGE_TYPE_MAX
} TCLImageType;

/*!
  @brief ���^�C�v
 */
typedef enum
{
    TCL_KEY_TYPE_LAUNCHER = 0 ,
    TCL_KEY_TYPE_APPLICATION ,
    TCL_KEY_TYPE_UNKNOWN ,
    TCL_KEY_TYPE_MAX
} TCLKeyType;

/*!
  @brief ��������
 */
typedef enum
{
    TCL_SEARCH_CONDITION_NONE          = 0         ,
    TCL_SEARCH_CONDITION_PICTURE       = 1 << 0x00 ,
    TCL_SEARCH_CONDITION_FRAME         = 1 << 0x01 ,
    TCL_SEARCH_CONDITION_FAVORITE_NONE = 1 << 0x07 ,
    TCL_SEARCH_CONDITION_FAVORITE_1    = 1 << 0x08 ,
    TCL_SEARCH_CONDITION_FAVORITE_2    = 1 << 0x09 ,
    TCL_SEARCH_CONDITION_FAVORITE_3    = 1 << 0x0a ,
    TCL_SEARCH_CONDITION_FAVORITE_ALL  = 1 << 0x0b ,
    TCL_SEARCH_CONDITION_LAUNCHER      = 1 << 0x0c ,
    TCL_SEARCH_CONDITION_APPLICATION   = 1 << 0x0d ,
    TCL_SEARCH_CONDITION_FREE          = 1 << 0x10 , // �󂫃X�y�[�X��Ԃ�������Ɠ���ȏ����Ȃ̂ő��Ƃ͕��p�s��
    TCL_SEARCH_CONDITION_MAX
} TCLSearchCondition;

/*!
  @brief �\�[�g�^�C�v
 */
typedef enum
{
    TCL_SORT_TYPE_DATE = 0 ,
    TCL_SORT_TYPE_FAVORITE_1 ,
    TCL_SORT_TYPE_FAVORITE_2 ,
    TCL_SORT_TYPE_FAVORITE_3 ,
    TCL_SORT_TYPE_FAVORITE_ALL ,
    TCL_SORT_TYPE_FRAME ,
    TCL_SORT_TYPE_MAX
} TCLSortType;

/*!
  @brief �e��֐��̖߂�l
 */
typedef enum
{
    TCL_RESULT_SUCCESS = 0 ,
    TCL_RESULT_ERROR_FS_OPEN_FILE_EX ,
    TCL_RESULT_ERROR_FS_CLOSE_FILE ,
    TCL_RESULT_ERROR_FS_CREATE_FILE_AUTO ,
    TCL_RESULT_ERROR_FS_GET_FILE_LENGTH ,
    TCL_RESULT_ERROR_FS_READ_FILE ,
    TCL_RESULT_ERROR_FS_WRITE_FILE ,
    TCL_RESULT_ERROR_FS_DELETE_FILE ,
    TCL_RESULT_ERROR_FS_OPEN_DIRECTORY ,
    TCL_RESULT_ERROR_FS_CLOSE_DIRECTORY ,
    TCL_RESULT_ERROR_FS_GET_ARCHIVE_RESOURCE ,

    TCL_RESULT_ERROR_OVER_NUM_PICTURES , // �Ǘ��e�[�u���ɕۑ��\�ȗʂ𒴂��Ă���
    TCL_RESULT_ERROR_NO_TABLE_FILE ,     // �e�[�u����������Ȃ�����
    TCL_RESULT_ERROR_BROKEN_TABLE_FILE , // �Ǘ��t�@�C�������Ă�
    TCL_RESULT_ERROR_NO_NEXT_INDEX ,     // �ʐ^�����ɕۑ����ׂ��ꏊ��������Ȃ�����
    TCL_RESULT_ERROR_EXIST_OTHER_FILE ,  // ����ۑ���ɉ����ʂ̃t�@�C����������
    TCL_RESULT_ERROR_ALREADY_MANAGED ,   // �w��C���f�N�X�͊��ɊǗ����ɂ���
    TCL_RESULT_ERROR_NO_FIND_PICTURE ,   // �p�X�����Ŏw��C���f�N�X�ł͎ʐ^��������Ȃ�����

    TCL_RESULT_ERROR_SSP_START_JPEG_DECODER        , // �f�R�[�h�G���[
    TCL_RESULT_ERROR_SSP_EXTRACT_JPEG_DECODER_EXIF , // exif�ǂݍ��݃G���[

    TCL_RESULT_ERROR_OTHER ,                  // ���̑��̃G���[
    TCL_RESULT_MAX
} TCLResult;

/*!
  @brief �ʐ^�ۑ���C���f�N�X�ێ�
 */
typedef struct
{
    u16 dirIdx;
    u16 fileIdx;
} TCLIndexInfo;

/*!
  @brief �Ǘ��t�@�C���A�N�Z�T
 */
typedef struct
{
    void* tableBuffer;
    u32   tableBufferSize;
    void* workBuffer;
    u32   workBufferSize;
} TCLAccessor;

/*!
  @brief �Ǘ��e�[�u���w�b�_
 */
typedef struct
{
    u32 signature;
    u32 version;
    u16 numElements;
    u8  targetArchive;
    u8  reserved0;
    TCLIndexInfo picIdxInfo;
    TCLIndexInfo frmIdxInfo;
    u16 crc;
    u16 offset;
} TCLTableHeader;

/*!
  @brief �ʐ^���\����
 */
typedef struct
{
    s64 second;
    int favoriteOrder;

    union
    {
        u32 bitField;
        struct
        {
            u32 validity     :  1;
            u32 dirIdx       : 10;
            u32 fileIdx      :  7;
            u32 favoriteType :  2;
            u32 imageType    :  2;
            u32 keyType      :  2;
            u32 defaultFrame :  1;
            u32 reserved     :  7;
        };
    };
} TCLPictureInfo;

/*!
  @brief ���[�J�[�m�[�g�Ɋi�[����l
 */
typedef struct
{
    union
    {
        u32 bitField;
        struct
        {
            u32 imageType :  2;
            u32 reserved0 : 30;
        };
    };
    u32 reserved1;
} TCLMakerNote;

/*!
  @brief �����I�u�W�F�N�g
 */
typedef struct
{
    int startIdx;
    u32 condition;
} TCLSearchObject;

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_TCL_TYPES_H_ */
#endif
