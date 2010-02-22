//============================================================================================
/**
 * @file		time_icon.h
 * @brief		�Z�[�u���Ȃǂɕ\�������^�C�}�[�A�C�R������
 * @author	Hiroyuki Nakamura
 * @date		10.02.22
 *
 *	���W���[�����FTIMEICON
 */
//============================================================================================
#pragma	once



// �^�C�}�[�A�C�R�����[�N
typedef struct _TIMEICON_WORK	TIMEICON_WORK;

#define	TIMEICON_DEFAULT_WAIT		( 16 )



extern TIMEICON_WORK * TIMEICON_Create(
												GFL_TCBSYS * tcbsys, GFL_BMPWIN * msg_win, u8 clear_color, u8 wait, HEAPID heapID );

extern void TILEICON_Exit( TIMEICON_WORK * wk );
