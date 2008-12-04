#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================================
/**
 * @file	gf_standard.h
 * @brief	�b�W���֐���`�p�w�b�_
 *          inline�ɂł��Ȃ����̂� gf_standard.c�ɋL�q���܂�
 * @author	GAME FREAK inc.
 * @date	2006.11.29
 */
//=============================================================================================

#ifndef	__GF_STANDARD_H__
#define	__GF_STANDARD_H__

#include "heap.h"

//----------------------------------------------------------------------------
/**
 *	@brief	�W�����C�u�����̏������֐�
 *	@retval	�Ȃ�
 */
//-----------------------------------------------------------------------------
extern void GFL_STD_Init(HEAPID heapID);

//----------------------------------------------------------------------------
/**
 *	@brief	�W�����C�u�����̏I���֐�
 *	@retval	�Ȃ�
 */
//-----------------------------------------------------------------------------
extern void GFL_STD_Exit(void);

//--------------------------------------------------------------------
/**
 * @brief	�������[�R�s�[�֐�
 * @param   src   �R�s�[��
 * @param   dest  �R�s�[��
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemCopy( const void* src, void* dest, u32 size ) { MI_CpuCopy8(src,dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	�������[�R�s�[�֐� 16BIT�]����
 * @param   src   �R�s�[��
 * @param   dest  �R�s�[��
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemCopy16( const void* src, void* dest, u32 size ) { MI_CpuCopy16(src,dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	�������[�R�s�[�֐� 32BIT�]����
 * @param   src   �R�s�[��
 * @param   dest  �R�s�[��
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemCopy32( const void* src, void* dest, u32 size ) { MI_CpuCopy32(src,dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������w��̃f�[�^�Ŗ��߂܂�
 * @param   dest   �]����A�h���X
 * @param   data  �]���f�[�^
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemFill( void* dest, u8 data, u32 size ) { MI_CpuFill8(dest,data,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������w��̃f�[�^�Ŗ��߂܂�(16BIT��)
 * @param   dest   �]����A�h���X
 * @param   data  �]���f�[�^
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemFill16( void* dest, u16 data, u32 size ) { MI_CpuFill16(dest,data,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������w��̃f�[�^�Ŗ��߂܂�(32BIT��)
 * @param   dest   �]����A�h���X
 * @param   data  �]���f�[�^
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemFill32( void* dest, u32 data, u32 size ) { MI_CpuFill32(dest,data,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������O�ŃN���A���܂�
 * @param   dest   �]����A�h���X
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemClear( void* dest, u32 size ) { MI_CpuClear8(dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������O�ŃN���A���܂�(16BIT��)
 * @param   dest   �]����A�h���X
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemClear16( void* dest, u32 size ) { MI_CpuClear16(dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	���������O�ŃN���A���܂�(32BIT��)
 * @param   dest   �]����A�h���X
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemClear32( void* dest, u32 size ) { MI_CpuClear32(dest,size); }

//----------------------------------------------------------------------------
/**
 *	@brief	�������̈���r����
 *	@param	s1	    ��r�������[1
 *	@param	s2	    ��r�������[2
 *	@param	size	��r�T�C�Y
 *	@retval	�O      ����
 *	@retval	�I�O    �قȂ�
 */
//-----------------------------------------------------------------------------
extern int GFL_STD_MemComp( const void *s1, const void *s2, const u32 size );

//----------------------------------------------------------------------------
/**
 *	@brief	�O�������܂ł̃o�C�g�����v��
            �����̈����ɂ�STRBUF���g������  ����͒ʐM��SSID�Ȃ� ASCII������Ɏg�p����p�r�ɂ������鎖
 *	@param	s	    �f�[�^��
 *	@return	�T�C�Y������
 */
//-----------------------------------------------------------------------------
extern int GFL_STD_StrLen( const void *s );

//----------------------------------------------------------------------------
/**
 *	@brief   �����Z���k�c�C�X�^�[�������֐� RTC��vsyncnum�ŏ��������Ă�������
 *	@param   s  �V�[�h�l(���̒l���珉�����l�����`�����@�Ő�������܂�)
 *  @return  none
 */
//----------------------------------------------------------------------------
extern void GFL_STD_MtRandInit(u32 s);

//----------------------------------------------------------------------------
/**
 *  @brief   �����Z���k�c�C�X�^�[�ŕ����Ȃ��R�Q�r�b�g���̗������擾
 *  @param   max    �擾���l�͈̔͂��w�� 0 �` max-1 �͈̔͂̒l���擾�ł��܂��B
 *                  0���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
 *                  % �� / ���g�p�����ɂ�����max�l��ύX���Ă�������
 *	@return  �������ꂽ����
 */
//----------------------------------------------------------------------------
extern u32 __GFL_STD_MtRand(void); ///< �g�p�֎~  ����inline�֐����g���Ă�������
static inline u32 GFL_STD_MtRand(u32 max)
{
    u64 x = (u64)__GFL_STD_MtRand();

    // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
    if (max == 0) {
        return (u32)x;
    }
    else {
        return (u32)((x * max) >> 32);
    }
}

#define GFL_STD_MTRAND_MAX	(0xffffffff)

/**
 *  @brief   ���`�����@�����̍\����
 */
typedef struct
{
    u64     x;                         ///< �����l
    u64     mul;                       ///< �搔
    u64     add;                       ///< ���Z���鐔
}
GFL_STD_RandContext;

//----------------------------------------------------------------------------
/**
 *  @brief  ���`�����@�ɂ��32bit�����R���e�L�X�g�����������܂��B
 *  @param  context �����\���̂̃|�C���^
 *  @param  seed    �����l�Ƃ��Đݒ肷�闐���̎�  �����Z���k�c�C�X�^�[��2��Ă�� (a1<<32)+a2�Ő������Ă�������
 *  @return none
 */
//----------------------------------------------------------------------------
static inline void GFL_STD_RandInit(GFL_STD_RandContext *context, u64 seed)
{
    context->x = seed;
    context->mul = (1566083941LL << 32) + 1812433253LL;
    context->add = 2531011;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���`�����@�ɂ��32bit�����擾�֐�
 *  @param  context �����\���̂̃|�C���^
 *  @param  max     �擾���l�͈̔͂��w�� 0 �` max-1 �͈̔͂̒l���擾�ł��܂��B
 *                  GFL_STD_MTRAND_MAX���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
 *                  % �� / ���g�p�����ɂ�����max�l��ύX���Ă�������
 *  @return 32bit�̃����_���l
 */
//----------------------------------------------------------------------------
static inline u32 GFL_STD_Rand(GFL_STD_RandContext *context, u32 max)
{
    context->x = context->mul * context->x + context->add;

    // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
    if (max == GFL_STD_MTRAND_MAX)
    {
        return (u32)(context->x >> 32);
    }
    else
    {
        return (u32)(((context->x >> 32) * max) >> 32);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	CRC���v�Z����
 *	@param	pData	    �v�Z����̈�
 *	@param	dataLength  �f�[�^�̒���
 *	@return	CRC�l
 */
//-----------------------------------------------------------------------------

extern u16 GFL_STD_CrcCalc( const void* pData, u32 dataLength );

//----------------------------------------------------------------------------
/**
 *  @brief  ���`�����@�ɂ��32bit�����R���e�L�X�g�����������܂��B
            �������V�[�h��RTC�ɂ����������ł�
 *  @param  context �����\���̂̃|�C���^
 *  @return none
 */
//----------------------------------------------------------------------------
extern void GFL_STD_RandGeneralInit(GFL_STD_RandContext *context);

#endif	/*	__GF_STANDARD_H__ */
#ifdef __cplusplus
} /* extern "C" */
#endif
