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

#pragma once

#include "heap.h"

//�v�f���J�E���g C++���ƃe���v���[�g���g���炵��
#define elementof(ar) (sizeof(ar) / sizeof((ar)[0]))


///< �Q����������킷�ėp�\����
typedef struct {
  int x;
  int y;
} GFL_POINT;

///< �R����������킷�ėp�\����
typedef struct {
  int x;
  int y;
  int z;
} GFL_POINT3D;

///< �n�_�Ɣ͈͂�����킷�ėp�\����
typedef struct {
  int left;
  int top;
  int right;
  int bottom;
} GFL_RECT;


#define GFL_STD_RAND_MAX	(0)  ///< MtRand���œn���ő嗐���������o������

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
 *	@brief	��Βl�𓾂� ����  ���������Ɠ��Ă΂�邱�Ƃ�����܂�
 *	@param	a       ��Βl���ق����l
 *	@retval	        ��Βl
 */
//-----------------------------------------------------------------------------
inline int GFL_STD_Abs(int a){ return MATH_ABS(a); }

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
 *	@brief	������A��
            �����̈����ɂ�STRBUF���g������  ����͒ʐM��SSID�Ȃ� ASCII������Ɏg�p����p�r�ɂ������鎖
 *	@param	src	  �A���������̕�����
 *	@param	dist	�A��������̕�����
 *	@param	s	    �T�C�Y
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_STD_StrCat( void *src, const void *dist, int size );

//----------------------------------------------------------------------------
/**
 *	@brief	�������r \0'�ȍ~�̔�r�͍s���܂���B
            �����̈����ɂ�STRBUF���g������  ����͒ʐM��SSID�Ȃ� ASCII������Ɏg�p����p�r�ɂ������鎖
 *	@param	cmp1	  �A���������̕�����
 *	@param	cmp2	�A��������̕�����
 *	@param	size	    �T�C�Y
 *	@return	0�Ȃ瓯�� 0!=�Ȃ�قȂ�
 */
//-----------------------------------------------------------------------------
extern int GFL_STD_StrnCmp( const void *cmp1, const void *cmp2, int size );

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
 *  @param   range  �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                  GFL_STD_RAND_MAX=0���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
 *                  % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 *                  GFL_STD_RAND_MAX���g��Ȃ��ꍇ�͉��̊֐�
 *	@return  �������ꂽ����
 */
//----------------------------------------------------------------------------
extern u32 __GFL_STD_MtRand(void); ///< �g�p�֎~  ����inline�֐����g���Ă�������
static inline u32 GFL_STD_MtRand(u32 range)
{
  u64 x = (u64)__GFL_STD_MtRand();

  // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
  if (range == GFL_STD_RAND_MAX) {
    return (u32)x;
  }
  else {
    return (u32)((x * range) >> 32);
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief   �����Z���k�c�C�X�^�[�ŕ����Ȃ��R�Q�r�b�g���̗������擾
 *  @param   range    �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                    % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 *                  �S���Ƃ�ꍇ��̊֐�
 *	@return  �������ꂽ����
 */
//----------------------------------------------------------------------------
static inline u32 GFL_STD_MtRand0(u32 range)
{
  u64 x = (u64)__GFL_STD_MtRand();
  return (u32)((x * range) >> 32);
}

/**
 *  @brief   ���`�����@�����̍\����
 */
typedef struct
{
  u64     x;                         ///< �����l
  u64     mul;                       ///< �搔
  u64     add;                       ///< ���Z���鐔
} GFL_STD_RandContext;

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
 *  @param  range   �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                  GFL_STD_RAND_MAX = 0 ���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
 *                  % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 *                  GFL_STD_RAND_MAX���g��Ȃ��ꍇ�͉��̊֐�
 *  @return 32bit�̃����_���l
 */
//----------------------------------------------------------------------------
static inline u32 GFL_STD_Rand(GFL_STD_RandContext *context, u32 range)
{
  context->x = context->mul * context->x + context->add;

  // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
  if (range == GFL_STD_RAND_MAX)
    {
      return (u32)(context->x >> 32);
    }
  else
    {
      return (u32)(((context->x >> 32) * range) >> 32);
    }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���`�����@�ɂ��32bit�����擾�֐�
 *  @param  context �����\���̂̃|�C���^
 *  @param  range   �擾���l�͈̔͂��w�� 0 �` range-1 �͈̔͂̒l���擾�ł��܂��B
 *                  % �� / ���g�p�����ɂ�����range�l��ύX���Ă�������
 *                  �S���Ƃ�ꍇ��̊֐�
 *  @return 32bit�̃����_���l
 */
//----------------------------------------------------------------------------
static inline u32 GFL_STD_Rand0(GFL_STD_RandContext *context, u32 range)
{
  context->x = context->mul * context->x + context->add;
  return (u32)(((context->x >> 32) * range) >> 32);
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

//----------------------------------------------------------------------------------------------
//	�Í����n�ڐA
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
/**
 * �`�F�b�N�T���̎Z�o���[�`��
 *
 * @param	adrs	�J�n�A�h���X
 * @param	size	�f�[�^�T�C�Y
 *
 * @retval	"�Z�o�����l"
 */
//----------------------------------------------------------------------------------------------
extern u32 GFL_STD_CODED_CheckSum(const void * adrs, u32 size);

extern void	GFL_STD_CODED_Coded(void *data,u32 size,u32 code);
extern void	GFL_STD_CODED_Decoded(void *data,u32 size,u32 code);


#ifdef __cplusplus
} /* extern "C" */
#endif
