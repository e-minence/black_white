//=============================================================================================
/**
 * @file	assert.h
 * @date	2007.01.19
 * @author	GAME FREAK inc.
 */
//=============================================================================================

#ifndef	__ASSERT_H__
#define	__ASSERT_H__

#define	GF_ASSERT_ENABLE


#ifdef	GF_ASSERT_ENABLE

// ASSERT�L���̏ꍇ --------------------------------------------------------------------------
extern void GFL_Assert(const char * filename, unsigned int line_no, const char * exp);
extern void GFL_AssertMsg(const char * filename, unsigned int line_no, const char * fmt, ... );

#define GF_ASSERT(exp)		\
	(exp)? ((void)0): GFL_Assert(__FILE__, __LINE__, #exp);
#define GF_ASSERT_MSG(exp, ... )		\
	(exp)? ((void)0): GFL_AssertMsg(__FILE__, __LINE__, __VA_ARGS__ );
//#define GF_ASSERT_Printf( ... )		((exp)? ((void)0);


/* ASSERT���o�͂����[�U��`���邽�߂̊֐��^��` */

typedef void (*GFL_ASSERT_DispInitFunc)(void);
typedef void (*GFL_ASSERT_DispMainFunc)( const char* );
typedef void (*GFL_ASSERT_DispFinishFunc)(void);

extern void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc );


#else
// ASSERT�����̏ꍇ --------------------------------------------------------------------------

#define GF_ASSERT(exp)				((void)0);
#define GF_ASSERT_MSG(exp, ...)		((void)0);

#endif


#endif	/* __ASSERT_H__ */
