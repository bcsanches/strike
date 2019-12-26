rmdir /s /q c:\temp\release_strike\
mkdir c:\temp\release_strike\resources

pushd %PHOBOS_HOME%\scripts\
call build_release_sdl_binary_only.cmd c:\temp\release_strike\ 
popd

copy ..\bin\strike.dll c:\temp\release_strike\

copy ..\work\autoexec.cfg c:\temp\release_strike\
copy ..\work\strike.cfg c:\temp\release_strike\
xcopy /s ..\work\resources\*.* c:\temp\release_strike\resources\
                                          