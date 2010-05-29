//=============================================================================================
/**
 * @file  btl_tables.h
 * @brief �|�P����WB �Ă��Ƃ��ȃT�C�Y�̃e�[�u����VRAM_H�ɒu���ăA�N�Z�X����
 * @author  taya
 *
 * @date  2010.05.27  �쐬
 */
//=============================================================================================
#pragma once


/**
 *  �A���R�[�����O�Ώۂ̃��U������
 */
extern BOOL BTL_TABLES_IsMatchEncoreFail( WazaID waza );

/**
 *  ���̂܂ˎ��s�Ώۂ̃��U����
 */
extern BOOL BTL_TABLES_IsMatchMonomaneFail( WazaID waza );



extern const WazaID* BTL_TABLES_GetYubiFuruOmmitTable( u32* elems );
extern BOOL BTL_TABLES_IsYubiFuruOmmit( WazaID waza );


#ifdef PM_DEBUG
extern int GYubiFuruDebugNumber[ BTL_POS_MAX ];

extern void BTL_TABLES_YubifuruDebugReset( void );
extern void BTL_TABLES_YubifuruDebugSetEnd( void );
extern void BTL_TABLES_YubifuruDebugInc( u8 pos );
#endif
