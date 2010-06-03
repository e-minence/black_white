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

/**
 *  �v���b�V���[�Ώۃ��U����
 */
extern BOOL BTL_TABLES_IsPressureEffectiveWaza( WazaID waza );


/**
 *  �����ǂ莸�s�Ώۃ��U����
 */
extern BOOL BTL_TABLES_IsSakidoriFailWaza( WazaID waza );

/**
 *  �g���[�X�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckTraceFailTokusei( u16 tokuseiID );

/**
 *  �u�Ȃ肫��v�ł��Ȃ��Ƃ���������
 */
extern BOOL BTL_TABLES_CheckNarikiriFailTokusei( u16 tokuseiID );



extern const WazaID* BTL_TABLES_GetYubiFuruOmmitTable( u32* elems );
extern BOOL BTL_TABLES_IsYubiFuruOmmit( WazaID waza );
extern BOOL BTL_TABLES_CheckItemCallNoEffect( u16 itemID );


/**
 *  �����^���n�[�u�Ή��̏�Ԉُ�`�F�b�N
 */
extern WazaSick  BTL_TABLES_GetMentalSickID( u32 idx );

/**
 *  �g�p����Ώۃ|�P�����̑I�����s���K�v�������A�C�e��������
 */
extern BOOL BTL_TABLES_IsNoTargetItem( u16 itemNo );


#ifdef PM_DEBUG
extern int GYubiFuruDebugNumber[ BTL_POS_MAX ];

extern void BTL_TABLES_YubifuruDebugReset( void );
extern void BTL_TABLES_YubifuruDebugSetEnd( void );
extern void BTL_TABLES_YubifuruDebugInc( u8 pos );
#endif
