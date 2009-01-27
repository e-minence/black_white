//===================================================================
/**
 * @file	debug_print.h
 * @brief	���胆�[�U�̊��ŃR���p�C�������ꍇ�̂ݓ��삷��Printf�֐�
 * @author	taya
 * @date	07.02.13
 */
//===================================================================
#ifndef __DEBUG_PRINT_H__
#define __DEBUG_PRINT_H__


typedef enum {
	AUTHOR_FREE = 0,	///< ������w�肷��ƑS���ŏo�͂����

	AUTHOR_WATANABE,
	AUTHOR_TAMADA,
	AUTHOR_SOGABE,
	AUTHOR_OHNO,
	AUTHOR_MATSUDA,
	AUTHOR_KAGAYA,
	AUTHOR_ARIIZUMI,
	AUTHOR_TAYA,

	AUTHOR_IGNORE,		///< ������w�肷��Əo�͂���Ȃ�
}AUTHOR_ID;


#if defined DEBUG_ONLY_FOR_taya
#define DEFINED_AUTHOR_ID	(AUTHOR_TAYA)
#elif defined DEBUG_ONLY_FOR_hiro_nakamura
#define DEFINED_AUTHOR_ID	(AUTHOR_NAKAMURA)
#elif defined DEBUG_ONLY_FOR_genya_hosaka
#define DEFINED_AUTHOR_ID	(AUTHOR_HOSAKA)
#elif defined DEBUG_ONLY_FOR_toru_nagihashi
#define DEFINED_AUTHOR_ID	(AUTHOR_NAGI)
#elif defined DEBUG_ONLY_FOR_watanabe
#define DEFINED_AUTHOR_ID	(AUTHOR_WATANABE)
#elif defined DEBUG_ONLY_FOR_tamada
#define DEFINED_AUTHOR_ID	(AUTHOR_TAMADA)
#elif defined DEBUG_ONLY_FOR_sogabe
#define DEFINED_AUTHOR_ID	(AUTHOR_SOGABE)
#elif defined DEBUG_ONLY_FOR_ohno
#define DEFINED_AUTHOR_ID	(AUTHOR_OHNO)
#elif defined DEBUG_ONLY_FOR_matsuda
#define DEFINED_AUTHOR_ID	(AUTHOR_MATSUDA)
#elif defined DEBUG_ONLY_FOR_kagaya
#define DEFINED_AUTHOR_ID	(AUTHOR_KAGAYA)
#elif defined DEBUG_ONLY_FOR_ariizumi_nobuhiko
#define DEFINED_AUTHOR_ID	(AUTHOR_ARIIZUMI)
#else
#define DEFINED_AUTHOR_ID	(AUTHOR_FREE)
#endif


// �f�o�b�O�v�����g�L���t���O�� PM_DEBUG �Ƃ͕ʂɂ��Ă���
#ifdef PM_DEBUG
#define DEBUG_PRINT_ENABLE
#endif


#ifdef DEBUG_PRINT_ENABLE

//==============================================================================================
/**
 * ���胆�[�U�̊��ŃR���p�C�������ꍇ�̂ݓ��삷��Printf�֐�
 *
 * @param   author_id		���[�UID�idebug_print.h�ɋL�q�j
 * @param   fmt				OS_TPrintf�ɓn�������t��������
 *
 */
//==============================================================================================
extern void DEBUG_Printf( AUTHOR_ID author_id, const char * fmt, ... );

#define		DEBUG_PrintfEx( flg, id, ... )		{  if(flg){ DEBUG_Printf(id, __VA_ARGS__); } }

#else

#define		DEBUG_Printf( id, ... )				((void)0)
#define		DEBUG_PrintfEx( flg, id, ... )		((void)0)

#endif


//-----------------------------------------------------------
// ���񂽂�L�q�p�}�N��
//-----------------------------------------------------------
#define		TAYA_Printf( ... )			DEBUG_Printf( AUTHOR_TAYA, __VA_ARGS__ )
#define		WATANABE_Printf( ... )	DEBUG_Printf( AUTHOR_WATANABE, __VA_ARGS__ )
#define		TAMADA_Printf( ...)			DEBUG_Printf( AUTHOR_TAMADA, __VA_ARGS__ )
#define		SOGABE_Printf( ...)			DEBUG_Printf( AUTHOR_SOGABE, __VA_ARGS__ )
#define		OHNO_Printf( ...)				DEBUG_Printf( AUTHOR_OHNO, __VA_ARGS__ )
#define		MATSUDA_Printf( ...)		DEBUG_Printf( AUTHOR_MATSUDA, __VA_ARGS__ )
#define		KAGAYA_Printf( ...)			DEBUG_Printf( AUTHOR_KAGAYA, __VA_ARGS__ )
#define		ARIIZUMI_Printf( ...)		DEBUG_Printf( AUTHOR_ARIIZUMI, __VA_ARGS__ )


#endif
