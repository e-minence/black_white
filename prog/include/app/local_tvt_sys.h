//======================================================================
/**
 * @file	local_tvt_sys.h
 * @brief	��ʐM�e���r�g�����V�[�o�[ ���C��
 * @author	ariizumi
 * @data	09/11/02
 *
 * ���W���[�����FLOCAL_TVT
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define LOCAL_TVT_MEMBER_MAX (4)
FS_EXTERN_OVERLAY(local_tvt);

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  LTM_2_MEMBER = 2,
  LTM_4_MEMBER = 4,
  
  LTM_MAX,
}LOCAL_TVT_MODE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  LOCAL_TVT_MODE mode;
  u8             charaType[LOCAL_TVT_MEMBER_MAX];
  u8             bgType[LOCAL_TVT_MEMBER_MAX];
}LOCAL_TVT_INIT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

extern GFL_PROC_DATA LocalTvt_ProcData;
