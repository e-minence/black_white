PATH=%PATH%;c:\NitroSDK\tools\bin;c:\NitroSystem\tools\win\bin;

copy ..\app_menu_common\task_menu.ncl .\
copy ..\app_menu_common\task_menu.ncg .\

g2dcvtr back_bg_up.ncl
g2dcvtr back_bg_down.ncl 
g2dcvtr back_bg_up.nsc
g2dcvtr back_bg_down.nsc
g2dcvtr obj_gage.ncl
g2dcvtr obj_gage.nce
g2dcvtr obj_pokemon.ncl
g2dcvtr obj_pokemon.nce
g2dcvtr obj_note.ncl
g2dcvtr obj_note.nce
g2dcvtr obj_button.ncl

@echo off
REM �{�^��OBJ��\resource\app_menu_common\task_menu.ncg task_menu.ncl���R�s�[���Ă��č쐬���Ă��܂��B
REM ncg���X�V���ꂽ���ɂ͔��f������K�v������܂��B
REM ���̂��߃R���o�[�g���Ă����̃t�H���_��obj_button.ncg�͔��f���Ă��܂���B
@echo on

g2dcvtr obj_button.nce

@if errorlevel 1 goto err

echo �A�[�J�C�u�̍쐬
nnsarc -c -i -n mictest.narc -S arc_list.txt

@if errorlevel 1 goto err

echo �j��
del *.NCLR
del *.NCGR
del *.NCER
del *.NANR
del *.NSCR


@if errorlevel 1 goto err

@goto ok

:ok
   @echo =======================
   @echo  Convert is SUCCESS !!!
   @echo =======================
   @goto end

:err
   @echo ***********************
   @echo        Error !!! 
   @echo **********************
   @goto end
:end

REM Pause
