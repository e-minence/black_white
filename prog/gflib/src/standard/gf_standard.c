//============================================================================================
/**
 * @file	gf_standard.c
 * @brief	�b�W���֐� + �����֐�
 * @author	GAME FREAK inc.
 * @date	2006.11.29
 */
//============================================================================================
#include "gflib.h"
#include "gf_standard.h"



/*-----------------------------------------------------------------------------
 *					�萔�錾
 ----------------------------------------------------------------------------*/
#define N 624						///< �����z��̗v�f��
#define M 397						///< ���R��
#define MATRIX_A 0x9908b0dfUL		///< ���߂��l�̂P�r�b�g�ڂ��P�̎���XOR����l 
#define UPPER_MASK 0x80000000UL		///< �r�b�g��w-r�r�b�g���擾����}�N��	����w = 32 r = 31�ł��B
#define LOWER_MASK 0x7fffffffUL		///< �r�b�g��r�r�b�g���擾����}�N��		��������ƂR�Q�r�b�g�����Ȃ��̗������擾�ł��܂��B

/*-----------------------------------------------------------------------------
 *					���[�N
 ----------------------------------------------------------------------------*/

struct _GF_STD_WORK{
    u32 mt[N];						///< MT�̎��̌��ʂ̓���z��
    int mti;						///< �z��v�f�ԍ�(N+1�̎��͏���������Ă��Ȃ�)
    u32 mag01[2];	///< ���̌��ʂP�r�b�g�ڂ�Y�����ԍ��Ƃ��ē����Xor������e�[�u��
    MATHCRC16Table crcTable;   ///< CRC�e�[�u��
};

typedef struct _GF_STD_WORK GF_STD_WORK;


static GF_STD_WORK* _pSTDSys = NULL; // �X�^���_�[�h���C�u�����̃������[���Ǘ�����static�ϐ�

//----------------------------------------------------------------------------
/**
 *	@brief	�W�����C�u�����̏������֐�
 *	@retval	�Ȃ�
 */
//-----------------------------------------------------------------------------

void GFL_STD_Init(HEAPID heapID)
{
    GF_ASSERT(_pSTDSys==NULL);
    _pSTDSys = GFL_HEAP_AllocMemory(heapID, sizeof(GF_STD_WORK));
    _pSTDSys->mti = N + 1;			///< �z��v�f�ԍ�(N+1�̎��͏���������Ă��Ȃ�)
    _pSTDSys->mag01[0] = 0x0UL;
    _pSTDSys->mag01[1] = MATRIX_A;	///< ���̌��ʂP�r�b�g�ڂ�Y�����ԍ��Ƃ��ē����Xor������e�[�u��
    MATH_CRC16CCITTInitTable(&_pSTDSys->crcTable);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�W�����C�u�����̏I���֐�
 *	@retval	�Ȃ�
 */
//-----------------------------------------------------------------------------

void GFL_STD_Exit(void)
{
    GFL_HEAP_FreeMemory(_pSTDSys);
    _pSTDSys=NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������̈���r����
 *	@param	s1	��r�������[1
 *	@param	s2	��r�������[2
 *	@param	size	��r�T�C�Y
 *	@retval	�O   ����
 *	@retval	�I�O �قȂ�
 */
//-----------------------------------------------------------------------------
int GFL_STD_MemComp(const void *s1, const void *s2, const u32 size)
{
    const unsigned char  *p1 = (const unsigned char *)s1;
    const unsigned char  *p2 = (const unsigned char *)s2;
    u32 n = size;

    while (n-- > 0) {
        if (*p1 != *p2)
            return (*p1 - *p2);
        p1++;
        p2++;
    }
    return (0);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O�������܂ł̃o�C�g�����v��
            �����̈����ɂ�STRBUF���g������  ����͒ʐM��SSID�Ȃ� ASCII������Ɏg�p����p�r�ɂ������鎖
 *	@param	s	    �f�[�^��
 *	@return	�T�C�Y������
 */
//-----------------------------------------------------------------------------
int GFL_STD_StrLen( const void *s )
{
    const unsigned char  *p1 = (const unsigned char *)s;
    int n = 0;

    while( 1 ){
        if(*p1 == 0){
            return n;
        }
        p1++;
        n++;
    }
    return 0;
}


/*-----------------------------------------------------------------------------
 *
 *	H16/11/10		Tomoya Takahashi
 *	
 *				MT���ł̗��������֐�
 *
 ----------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------
 *
 *	@brief   �����Z���k�c�C�X�^�[�������֐� RTC��vsyncnum�ŏ��������Ă�������
 *	@param   s  �V�[�h�l(���̒l���珉�����l�����`�����@�Ő�������܂�)
 *  @return  none
 * 
 ----------------------------------------------------------------------------*/
void GFL_STD_MtRandInit(u32 s)
{
//    mt[0]= s & 0xffffffff;			// �V�[�h�l��v�f�O�ɑ��
    _pSTDSys->mt[0]= s ;			// �V�[�h�l��v�f�O�ɑ��

	//
	// ���`�����@�ŗ����𐶐�
	//
    for (_pSTDSys->mti=1; _pSTDSys->mti<N; _pSTDSys->mti++) {
        _pSTDSys->mt[_pSTDSys->mti] = 
	    (1812433253UL * (_pSTDSys->mt[_pSTDSys->mti-1] ^ (_pSTDSys->mt[_pSTDSys->mti-1] >> 30)) + _pSTDSys->mti); 
//        mt[mti] &= 0xffffffff;
    }
}

/**-----------------------------------------------------------------------------
 *  @brief   �����Z���k�c�C�X�^�[�ŕ����Ȃ��R�Q�r�b�g���̗������擾
 *	@return  �������ꂽ����
------------------------------------------------------------------------------*/
u32 __GFL_STD_MtRand(void)
{
    u32 y;
    
	//
	// �����̐���
	//
	// �����痐���𐶐����邩�̃`�F�b�N
    if (_pSTDSys->mti >= N) {		// �����Ƀ��[�v������
		int kk;			// ���̌v�Z���[�v�p

		// mti��N+1�̂Ƃ��͏�����
        if (_pSTDSys->mti == N+1)   
		{
			GFL_STD_MtRandInit(5489UL);		// �K���Ȓl�ŏ�����
		}

		//
		// mti��N�ɂȂ����̂�
		// �����痐���𐶐��i�l���킩��܂���j
		// 
        for (kk=0;kk<N-M;kk++) {
            y = (_pSTDSys->mt[kk]&UPPER_MASK)|(_pSTDSys->mt[kk+1]&LOWER_MASK);
            _pSTDSys->mt[kk] = _pSTDSys->mt[kk+M] ^ (y >> 1) ^ _pSTDSys->mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (_pSTDSys->mt[kk]&UPPER_MASK)|(_pSTDSys->mt[kk+1]&LOWER_MASK);
            _pSTDSys->mt[kk] = _pSTDSys->mt[kk+(M-N)] ^ (y >> 1) ^ _pSTDSys->mag01[y & 0x1UL];
        }
        y = (_pSTDSys->mt[N-1]&UPPER_MASK)|(_pSTDSys->mt[0]&LOWER_MASK);
        _pSTDSys->mt[N-1] = _pSTDSys->mt[M-1] ^ (y >> 1) ^ _pSTDSys->mag01[y & 0x1UL];

        _pSTDSys->mti = 0;		// �擪�ɖ߂�
    }
  
	//
	// ���̗������擾
	// 
    y = _pSTDSys->mt[_pSTDSys->mti++];

	// ���̉��̏����ł����ƒl�𕪕z�����Ă��܂��B
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	CRC���v�Z����
 *	@param	pData	    �v�Z����̈�
 *	@param	dataLength  �f�[�^�̒���
 *	@return	CRC�l
 */
//-----------------------------------------------------------------------------

u16 GFL_STD_CrcCalc( const void* pData, u32 dataLength )
{
    if(dataLength < sizeof(u16)){
        u8 dummyData[2];
        u8* pD = (u8*)pData;
        dummyData[0]=*pD;
        dummyData[1]=dummyData[0];
        return MATH_CalcCRC16CCITT(&_pSTDSys->crcTable, &dummyData, sizeof(u16));
    }
    return MATH_CalcCRC16CCITT(&_pSTDSys->crcTable, pData, dataLength);
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���`�����@�ɂ��32bit�����R���e�L�X�g�����������܂��B
            �������V�[�h��RTC�ɂ����������ł�
 *  @param  context �����\���̂̃|�C���^
 *  @return none
 */
//----------------------------------------------------------------------------

void GFL_STD_RandGeneralInit(GFL_STD_RandContext *context)
{
    RTCDate date;
    RTCTime time;
    u32 seed;
    u64 seed64;
    RTC_GetDateTime(&date, &time);
    seed = date.year + date.month * 0x100 * date.day * 0x10000
        + time.hour * 0x10000 + (time.minute + time.second) * 0x1000000;

    GFL_STD_MtRandInit(seed);
    seed64 = GFL_STD_MtRand( 0 );
    seed64 = (seed64 << 32) + GFL_STD_MtRand( 0 );
    GFL_STD_RandInit( context, seed64 );
}
