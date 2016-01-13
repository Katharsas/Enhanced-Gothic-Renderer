IF EXIST "%G2_SYSTEM_PATH%\ShW32_org.dll" (
 echo Un-Patching Smartheap...
 copy "%G2_SYSTEM_PATH%\ShW32_org.dll" "%G2_SYSTEM_PATH%\ShW32.dll"
)