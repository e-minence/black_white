//============================================================================================
/**
 * @file		ddemo_seq.h
 * @brief		�a������f�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.06
 *
 *	���W���[�����FDDEMOSEQ
 */
//============================================================================================
#pragma	once


enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_WIPE,

	MAINSEQ_MAIN,

	MAINSEQ_END
};


extern BOOL DDEMOSEQ_MainSeq( DDEMOMAIN_WORK * wk );
