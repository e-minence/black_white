@echo off
REM =================================================================
REM hudson�p��SLR�Ɉ����𖄂ߍ��ރo�b�`
REM �p�r    : prompt.bat cmd hudson_buryarg.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2010/05/12
REM =================================================================

SET PATH_TOOLS=%PROJECT_ROOT%\tools\hudson

REM �e�X�g�i���o�[ >> hudson�Ŏw��
SET TEST_NO=%1

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM �܂�make����
make

REM ================================
REM TLF�ɏ�������ł���SRL���Đ���
REM ================================
buryarg.TWL %PATH_MAIN_TLF% %TEST_NO%
touch %PATH_MAIN_SRL%
make

