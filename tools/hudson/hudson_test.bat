@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM ================================
REM �u�[�g
REM ================================
nitrorun %PROJECT_PROGDIR%/bin/ARM9-TS/Release/main.srl

REM �I������
if %ERRORLEVEL% == 0 goto END:

REM �G���[�R�[�h���ہX�Ԃ�
exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:END
  @echo hudson make end


