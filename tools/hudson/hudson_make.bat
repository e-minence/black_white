@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

SET IPMSG_BAT=tools/hudson/hudson_ipmsg.bat

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM ================================
REM ���� make
REM ================================
@echo ***** make *****
make

REM ����I������
if %ERRORLEVEL% == 0 goto END:

REM ================================
REM �G���[����
REM ================================
REM clean���Ă���
REM make clean

REM IPMSG�Œʒm(�G���[���x�����㏑�����Ă��܂�)
REM %IPMSG_BAT% 1
REM �Ȃ��������g���ƁA���ɗ���Ă��Ȃ�

@echo errorlevel = %ERROR_LEVEL%
exit %ERROR_LEVEL%

REM ================================
REM ����I��
REM ================================
:END
  REM %IPMSG_BAT% 0
  @echo hudson make end


