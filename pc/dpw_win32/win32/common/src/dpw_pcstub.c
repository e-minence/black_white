#include <dwc.h>

#include "dpw_typedef.h"
#include "dpw_pcstub.h"

#include "dwc_ghttp.h"

#include <vector>
#include <algorithm>

static DWCError stDwcLastError = DWC_ERROR_NONE;  // �Ō�̃G���[
static int stDwcErrorCode = 0;  // �Ō�̃G���[�R�[�h
static std::vector<void*>	m_alloclist;				// �������[�A���P�[�V�������X�g

BOOL DWCi_IsError(void)
{

    if (stDwcLastError != DWC_ERROR_NONE) return TRUE;
    else return FALSE;
}


void DWCi_SetError(DWCError error, int errorCode)
{

    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERROR�͏㏑���֎~
        stDwcLastError = error;
        stDwcErrorCode = errorCode;
    }

}

void DWC_ClearError(void) {
	
    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERROR�̓N���A�֎~
        stDwcLastError = DWC_ERROR_NONE;
        stDwcErrorCode = 0;
    }
}

void* DWC_Alloc     ( DWCAllocType name, u32 size )
{
    void* mem = malloc( size );

	if( name == DPWI_ALLOC )
		m_alloclist.push_back( mem );

	return mem;
}


void DWC_Free      ( DWCAllocType name, void* ptr, u32 size )
{
	if( name != DPWI_ALLOC ){

		free( ptr );

		return;

	}

	std::vector<void*>::iterator it;

	it = std::find( m_alloclist.begin(), m_alloclist.end(), ptr );
	if( it != m_alloclist.end() ){

		// �������[�J��
		free( ptr );

		// ���X�g����폜
		m_alloclist.erase( it );
	
	}else{

		// �댯�ȊJ��
		OutputDebugString(L"[WARNING!!] illigal free\n");
		printf("[WARNING!!] memory leak at %d\n", (unsigned int)*it);

	}
	
}

void Debug_MemBrowse_begin()
{
	m_alloclist.clear();
}

void Debug_MemBrowse_end()
{

	for(std::vector<void*>::iterator it = m_alloclist.begin(); it != m_alloclist.end(); it++ ){
		OutputDebugString(L"[WARNING!!] memory leak\n");
		printf("[WARNING!!] memory leak at %d\n", (unsigned int)*it);
	}

}

void MI_CpuClear8(void* pt, int size)
{
	char* ptr = (char*)pt;

	while( size-- > 0 ){
		*ptr++ = 0;
	}
}


#define OS_Sleep(msec)	(Sleep(msec))


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand32

  Description:  ���`�����@�ɂ��32bit�����R���e�L�X�g�����������܂��B

  Arguments:    context �����R���e�L�X�g�ւ̃|�C���^
  
                seed    �����l�Ƃ��Đݒ肷�闐���̎�
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_InitRand32(MATHRandContext32 *context, u64 seed)
{
    context->x = seed;
    context->mul = (1566083941LL << 32) + 1812433253LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand32

  Description:  ���`�����@�ɂ��32bit�����擾�֐�

  Arguments:    context �����R���e�L�X�g�ւ̃|�C���^
  
                max     �擾���l�͈̔͂��w�� 0 �` max-1 �͈̔͂̒l���擾�ł��܂��B
                        0���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�32bit�l�ƂȂ�܂��B
  
  Returns:      32bit�̃����_���l
 *---------------------------------------------------------------------------*/
u32 MATH_Rand32(MATHRandContext32 *context, u32 max)
{
    context->x = context->mul * context->x + context->add;

    // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
    if (max == 0)
    {
        return (u32)(context->x >> 32);
    }
    else
    {
        return (u32)(((context->x >> 32) * max) >> 32);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand16

  Description:  ���`�����@�ɂ��16bit�����R���e�L�X�g�����������܂��B

  Arguments:    context �����R���e�L�X�g�ւ̃|�C���^
  
                seed    �����l�Ƃ��Đݒ肷�闐���̎�
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_InitRand16(MATHRandContext16 *context, u32 seed)
{
    context->x = seed;
    context->mul = 1566083941LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand16

  Description:  ���`�����@�ɂ��16bit�����擾�֐�

  Arguments:    context �����R���e�L�X�g�ւ̃|�C���^
  
                max     �擾���l�͈̔͂��w�� 0 �` max-1 �͈̔͂̒l���擾�ł��܂��B
                        0���w�肵���ꍇ�ɂ͂��ׂĂ͈̔͂�16bit�l�ƂȂ�܂��B
  
  Returns:      16bit�̃����_���l
 *---------------------------------------------------------------------------*/
u16 MATH_Rand16(MATHRandContext16 *context, u16 max)
{
    context->x = context->mul * context->x + context->add;

    // ����max���萔�Ȃ�΃R���p�C���ɂ��œK�������B
    if (max == 0)
    {
        return (u16)(context->x >> 16);
    }
    else
    {
        return (u16)(((context->x >> 16) * max) >> 16);
    }
}
