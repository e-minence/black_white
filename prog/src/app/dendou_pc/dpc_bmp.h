//============================================================================================
/**
 * @file		dpc_bmp.h
 * @brief		�a������m�F��� �a�l�o�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	���W���[�����FDPCBMP
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

extern void DPCBMP_Init( DPCMAIN_WORK * wk );
extern void DPCBMP_Exit( DPCMAIN_WORK * wk );
extern void DPCBMP_PrintUtilTrans( DPCMAIN_WORK * wk );

extern void DPCBMP_PutTitle( DPCMAIN_WORK * wk );
extern void DPCBMP_PutPage( DPCMAIN_WORK * wk );
extern void DPCBMP_PutInfo( DPCMAIN_WORK * wk );
