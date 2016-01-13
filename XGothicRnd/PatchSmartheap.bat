IF EXIST "%G2_SYSTEM_PATH%\ShW32_nop.dll" (
 echo Patching Smartheap...
 copy "%G2_SYSTEM_PATH%\ShW32_nop.dll" "%G2_SYSTEM_PATH%\ShW32.dll"
)
