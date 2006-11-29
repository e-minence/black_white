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
inline void GFL_STD_MemCopy16( const void* src, void* dest, u32 size ) { MI_CpuCopy8(src,dest,size); }

//--------------------------------------------------------------------
/**
 * @brief	�������[�R�s�[�֐� 32BIT�]����
 * @param   src   �R�s�[��
 * @param   dest  �R�s�[��
 * @param   size  �T�C�Y
 * @return  none
 */
//--------------------------------------------------------------------
inline void GFL_STD_MemCopy32( const void* src, void* dest, u32 size ) { MI_CpuCopy8(src,dest,size); }

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




#endif	/*	__GF_STANDARD_H__ */
