@echo off
REM =================================================================
REM hudson > make clean > make clean �p�o�b�`
REM �p�r    : prompt.bat cmd hudson_clean.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

SET PATH_TOOLS=C:\.hudson\jobs\wb_make\workspace\pokemon_wb\tools\hudson

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM ================================
REM  make clean
REM ================================
@echo ***** make clean *****
make clean

REM ����I������
if %ERRORLEVEL% == 0 goto END:

REM ================================
REM �G���[����
REM ================================
@echo errorlevel = %ERRORLEVEL%
exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:END
  @echo hudson make end




