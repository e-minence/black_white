@REM %ENV:A=B%は、環境変数ENVのAをBに置き換える、という意味
pushd %PROJECT_ROOT:/=\\%
svn up lib\gflib prog resource tools multiboot multiboot2
popd
