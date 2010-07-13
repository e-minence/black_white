//=============================================================================================
/**
 * @file	assert.h
 * @date	2007.01.19
 * @author	GAME FREAK inc.
 */
//=============================================================================================

#ifndef	__ASSERT_H__
#define	__ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

//�f�o�b�O��`
//#ifdef PM_DEBUG
#define	GF_ASSERT_ENABLE
//#endif//PM_DEBUG


#ifdef	GF_ASSERT_ENABLE

  //#define GF_ASSERT_Printf( ... )		((exp)? ((void)0);
  //

  #ifdef PM_DEBUG 
  // ASSERT�L���Ńf�o�b�OROM�̏ꍇ ------------------------------------------------------
    #define GF_ASSERT(exp)		\
    	(exp)? ((void)0): GFL_Assert(__FILE__, __LINE__, #exp);
    #define GF_ASSERT_MSG(exp, ... )		\
    	(exp)? ((void)0): GFL_AssertMsg(__FILE__, __LINE__, __VA_ARGS__ );
    #define GF_ASSERT_LIGHT(exp)		\
    	(exp)? ((void)0): GFL_Assert(__FILE__, __LINE__, #exp);
    #define GF_ASSERT_MSG_LIGHT(exp, ... )		\
    	(exp)? ((void)0): GFL_AssertMsg(__FILE__, __LINE__, __VA_ARGS__ );
    #define GF_ASSERT_HEAVY(exp)		\
    	(exp)? ((void)0): GFL_Assert(__FILE__, __LINE__, #exp);
    #define GF_ASSERT_MSG_HEAVY(exp, ... )		\
    	(exp)? ((void)0): GFL_AssertMsg(__FILE__, __LINE__, __VA_ARGS__ );
    #define GF_PANIC(...)         GF_ASSERT_MSG(0,__VA_ARGS__)
  #else
  // ASSERT�L���Ő��i��ROM�̏ꍇ ------------------------------------------------------
    #define GF_ASSERT(exp)				((void)0);
    #define GF_ASSERT_MSG(exp, ...)		((void)0);
    #define GF_ASSERT_LIGHT(exp)				((void)0);
    #define GF_ASSERT_MSG_LIGHT(exp, ...)		((void)0);
    #define GF_ASSERT_HEAVY(exp)		\
    	(exp)? ((void)0): GFL_Assert(__FILE__, __LINE__, #exp);
    #define GF_ASSERT_MSG_HEAVY(exp, ... )		\
    	(exp)? ((void)0): GFL_AssertMsg(__FILE__, __LINE__, __VA_ARGS__ );
    #define GF_PANIC(...)         OS_Panic(__VA_ARGS__)
  #endif

//���L�֐��𒼐ڎg�p���֎~  GF_ASSERT�}�N�����g�p���邱��
extern void GFL_Assert(const char * filename, unsigned int line_no, const char * exp);
extern void GFL_AssertMsg(const char * filename, unsigned int line_no, const char * fmt, ... );

/* ASSERT���o�͂����[�U��`���邽�߂̊֐��^��` */
typedef void (*GFL_ASSERT_DispInitFunc)(void);
typedef void (*GFL_ASSERT_DispMainFunc)( const char* );
typedef void (*GFL_ASSERT_DispFinishFunc)(void);

extern void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc );


#else
// ASSERT�����̏ꍇ --------------------------------------------------------------------------
#define GF_ASSERT(exp)				((void)0);
#define GF_ASSERT_MSG(exp, ...)		((void)0);

#define GF_PANIC(...)         OS_Panic(__VA_ARGS__)


#define GFL_ASSERT_SetDisplayFunc( ... )  ((void)0);

#endif

///���i��ROM��ASSERT���Ăяo���ꂽ�Ƃ��ɁA�R�[���o�b�N������֐��ւ̃|�C���^
typedef void (*GFL_ASSERT_RELEASEROM_CALLBACK_FUNC)( void );

extern void GFL_ASSERT_SetReleaseRomWarningFunc( GFL_ASSERT_RELEASEROM_CALLBACK_FUNC func );

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif	/* __ASSERT_H__ */
