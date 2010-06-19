/*-----------------------------------------------------------------*
  Project:  NetAgent AM - include
  File:     AMProtect.h

  Copyright NetAgent Co. Ltd., All rights reserved.

  Revision 1.27 2010/02/26
  �Í����ύX�̎��{
  ����������ύX
  
  Revision 1.26 2009/12/09
  �Í��������̉���
  ���o�ł��Ȃ��}�W�R�������o�ł���悤�ɏC��
  
  Revision 1.25 2009/11/18
  �f�o�b�O�E�����[�X�r���h���Ƀf�o�b�K���}�W�R�����肵�Ă��܂��s�s���ɑΉ�
  
  Revision 1.24 2009/10/09
  �Í��������̉���
  ����������ύX
  �኱�̍����������{
  �Í����ύX�̎��{
  
  Revision 1.23 2009/08/11
  �Í��������̕ύX
  
  Revision 1.22 2009/07/01
  Thumb��Nitro/TwlSDK���g�p�����ꍇ�Ɍ댟�m����o�O���C��
  
  Revision 1.20 2009/04/14
  TWL���[�h(HYBRID/LTD)�ɑΉ�
  ��������������
  �Í����ύX�̎��{
  
  Revision 1.10 2009/03/10
  ����������ŃN���b�V������\���������o�O���C��
  �Í����ύX�̎��{
  
  Revision 1.08 2008/12/25
  �Í����ύX�̎��{�i����܂ł����{����Ă�������A�i�E���X�j
  
  Revision 1.07 2008/12/14
  �w�b�_�t�@�C���ɃG���[�`�F�b�N��ǋL
  
  Revision 1.06 2008/11/12
  �֐��|�C���^�̕ێ����@����ѓ�������������

  Revision 1.05 2008/08/21
  �o�[�W�����̑ł����������{

  Revision 0.3  2008/07/25
  �G�~�����[�^���o�֐��̒ǉ�
  
  Revision 0.2  2008/04/26
  DEBUG, RELEASE���[�h���ł�ensata,IS-NITRO-EMULATOR,
  IS-NITRO-DEBUGGER�̌��o�}��
  
  Revision 0.1  2008/03/16
  �����o�[�W����
  
 *-----------------------------------------------------------------*/

 
#ifndef _AM_PROTECT_H_
#define _AM_PROTECT_H_

#include <nitro.h>
#include <nitro/types.h>
#include <nitro/os.h>
#include <nitro/card.h>

#define AM_VERSION (127)

/* -- �֐��|�C���^����p��` ------------------------------------- */
#define _ADD_VAL        ((u32)(8 * 100))
#define _SET_FUNC(x)    (((u32)(x)) + (_ADD_VAL))
#define _GET_FUNC(x)    (((u32)(x)) - (_ADD_VAL))

/* �����Ɏg�p����֐��|�C���^�̒�` */
typedef void (*AM_PTR_FUNC)(void);
typedef u32 (*AM_CALLBACK_PTR)(void*);

/* ���̊֐����G�N�X�|�[�g */
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __UTILS_H__
# define IMPORT_AM_API(__FUNC) Stub_##__FUNC
#endif /* __UTILS_H__ */


// API��`
extern u32 IMPORT_AM_API(ProtectB1)(AM_CALLBACK_PTR addr, void* param, u32 reserve);
extern u32 IMPORT_AM_API(NotProtectB1)(AM_CALLBACK_PTR addr, void* param, u32 reserve);
extern u32 IMPORT_AM_API(ProtectB2)(AM_CALLBACK_PTR addr, void* param, u32 reserve);
extern u32 IMPORT_AM_API(NotProtectB2)(AM_CALLBACK_PTR addr, void* param, u32 reserve);
extern u32 IMPORT_AM_API(ProtectB3)(AM_CALLBACK_PTR addr, void* param, u32 reserve);
extern u32 IMPORT_AM_API(NotProtectB3)(AM_CALLBACK_PTR addr, void* param, u32 reserve);


// ver 1.24 �ɂ����� ver 1.23�ȑO�Ƃ�API�݊������Ƃ邽�߂̊֐�
static u32 __wrap_func( void* param ){
	if( param ){
		(*((AM_PTR_FUNC)param))();
	}
	return ~(u32)param;
}


/*-----------------------------------------------------------------*
  undocumented
 *-----------------------------------------------------------------*/
#ifdef SDK_FINALROM
#define AM_IsMagiconB1(func,arg,reserve)	(IMPORT_AM_API(ProtectB1)(func,arg,reserve))
#define AM_IsMagiconB2(func,arg,reserve)	(IMPORT_AM_API(ProtectB2)(func,arg,reserve))
#define AM_IsMagiconB3(func,arg,reserve)	(IMPORT_AM_API(ProtectB3)(func,arg,reserve))
#else
/* DEBUG,RELEASE�łł͌����J�����ł̃`�F�b�N��}������ */
static u32 AM_IsMagiconB1(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	// LockID�̋󂫊m�F
	// LockID���g���؂��Ă���悤�ł����OS_Panic()
	s32 lockId = OS_GetLockID();
	if (OS_LOCK_ID_ERROR == lockId) {
		OS_Panic("lock ID get error.");
	}
	OS_ReleaseLockID((u16)lockId);
	
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		return (u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(ProtectB1)( func, arg, reserve );
	
}

static u32 AM_IsMagiconB2(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		return (u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(ProtectB2)( func, arg, reserve );
	
}

static u32 AM_IsMagiconB3(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		return (u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(ProtectB3)( func, arg, reserve );
	
}

#endif /* SDK_FINALROM */


/*-----------------------------------------------------------------*
  undocumented
 *-----------------------------------------------------------------*/
#ifdef SDK_FINALROM
#define AM_IsNotMagiconB1(func,arg,seed)	(IMPORT_AM_API(NotProtectB1)(func,arg,seed))
#define AM_IsNotMagiconB2(func,arg,seed)	(IMPORT_AM_API(NotProtectB2)(func,arg,seed))
#define AM_IsNotMagiconB3(func,arg,seed)	(IMPORT_AM_API(NotProtectB3)(func,arg,seed))
#else
/* DEBUG,RELEASE�łł͌����J�����ł̃`�F�b�N��}������ */
static u32 AM_IsNotMagiconB1(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	// LockID�̋󂫊m�F
	// LockID���g���؂��Ă���悤�ł����OS_Panic()
	s32 lockId = OS_GetLockID();
	if (OS_LOCK_ID_ERROR == lockId) {
		OS_Panic("lock ID get error.");
	}
	OS_ReleaseLockID((u16)lockId);
	
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		if (NULL != func) {
			(*func)( arg );
		}
		return ~(u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(NotProtectB1)( func, arg, reserve );
	
}

static u32 AM_IsNotMagiconB2(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		if (NULL != func) {
			(*func)( arg );
		}
		return ~(u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(NotProtectB2)( func, arg, reserve );
	
}


static u32 AM_IsNotMagiconB3(AM_CALLBACK_PTR func, void *arg, u32 reserve) {
	
	switch (OS_GetConsoleType() & OS_CONSOLE_MASK) {
	case OS_CONSOLE_ENSATA:
	case OS_CONSOLE_ISEMULATOR:
	case OS_CONSOLE_ISDEBUGGER:
#ifdef OS_CONSOLE_TWLDEBUGGER
	case OS_CONSOLE_TWLDEBUGGER:
#endif
		if (NULL != func) {
			(*func)( arg );
		}
		return ~(u32)arg;
	default:
		;
	}
	
	return IMPORT_AM_API(NotProtectB3)( func, arg, reserve );
	
}

#endif /* SDK_FINALROM */


/*
  version 1.23�ȑO��API�Ƃ̌݊����̂��߂̊֐���`
  �C�����C���֐���`���g�p���ă��[�U�[�R�[�h�G���A�ɓW�J������
*/
inline static u32 AM_IsMagiconA1( AM_PTR_FUNC func ){ return AM_IsMagiconB1(__wrap_func, func,0) == ~(u32)func; }
inline static u32 AM_IsMagiconA2( AM_PTR_FUNC func ){ return AM_IsMagiconB2(__wrap_func, func,0) == ~(u32)func; }
inline static u32 AM_IsMagiconA3( AM_PTR_FUNC func ){ return AM_IsMagiconB3(__wrap_func, func,0) == ~(u32)func; }
inline static u32 AM_IsNotMagiconA1( AM_PTR_FUNC func ){ return AM_IsNotMagiconB1(__wrap_func, func,0) == ~(u32)func; }
inline static u32 AM_IsNotMagiconA2( AM_PTR_FUNC func ){ return AM_IsNotMagiconB2(__wrap_func, func,0) == ~(u32)func; }
inline static u32 AM_IsNotMagiconA3( AM_PTR_FUNC func ){ return AM_IsNotMagiconB3(__wrap_func, func,0) == ~(u32)func; }

#ifdef __cplusplus
}
#endif


/* _AM_PROTECT_H_ */
#endif


/* --------------------------------------------------------- EOF --*/
