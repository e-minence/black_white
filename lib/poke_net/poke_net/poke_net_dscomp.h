//============================================================================
//                         Pokemon Network Library
//============================================================================
#ifndef ___POKE_NET_BUILD_DS___
typedef	char	s8;							// signed 8 bit
typedef	short	s16;						// signed 16 bit
typedef	long	s32;						// signed 32 bit
#ifdef ___POKE_NET_BUILD_WINDOWS___
typedef	__int64	s64;						// signed 64bit
#endif
#ifdef ___POKE_NET_BUILD_LINUX___
typedef	long long	s64;					// signed 64bit
#endif

typedef	unsigned char	u8;					// unsigned 8 bit
typedef	unsigned short	u16;				// unsigned 16 bit
typedef	unsigned long	u32;				// unsigned 32 bit
#ifdef ___POKE_NET_BUILD_WINDOWS___
typedef	unsigned __int64	u64;			// unsigned 64bit
#endif
#ifdef ___POKE_NET_BUILD_LINUX___
typedef	unsigned long long	u64;			// unsigned 64bit
#endif

#endif


// VisualStudio�ŃR���p�C�����̈ȉ��̌x����}������B
// �d�l����Ă���W���֐��ɑ΂���x���FC4996
// �z��[0]�ɑ΂���x��:C4200
#ifdef ___POKE_NET_BUILD_WINDOWS___
#pragma  warning(disable : 4996)
#pragma  warning(disable : 4200)
#endif


