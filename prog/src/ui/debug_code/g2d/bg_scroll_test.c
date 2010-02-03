//=============================================================================
/**
 *
 *	@file		bg_scroll_test.c
 *	@brief  �L�[����ɂ��BG�X�N���[���̃e�X�g
 *	@author	hosaka genya
 *	@data		2010.02.03
 *
 *	<�R�[�h����>
 *	�����[�v�ĂԂ��ƂŃL�[�����BG���X�N���[���ł��܂��B
 *
 *	<�������>
 *    �����Fy+,y-;
 *�@  �����Fx+,x-;
 */
//=============================================================================

#if PM_DEBUG

static void debug_bg_scroll_test( int frame )
{
  int cnt;
  
  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
  {
    cnt = GFL_BG_GetScrollY( frame );
    GFL_BG_SetScrollReq( frame, GFL_BG_SCROLL_Y_SET, cnt+1 );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
  {
    cnt = GFL_BG_GetScrollY( frame );
    GFL_BG_SetScrollReq( frame, GFL_BG_SCROLL_Y_SET, cnt-1 );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    cnt = GFL_BG_GetScrollX( frame );
    GFL_BG_SetScrollReq( frame, GFL_BG_SCROLL_X_SET, cnt+1 );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    cnt = GFL_BG_GetScrollX( frame );
    GFL_BG_SetScrollReq( frame, GFL_BG_SCROLL_X_SET, cnt-1 );
  }
}


#endif
