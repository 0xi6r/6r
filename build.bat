@echo off
REM Build script for Windows (fallback if Makefile doesn't work)

echo Building 6r Text Editor for Windows...

REM Create directories if they don't exist
if not exist obj mkdir obj
if not exist bin mkdir bin

REM Compile source files
echo Compiling platform.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/platform.c -o obj/platform.o
if errorlevel 1 goto error

echo Compiling buffer.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/buffer.c -o obj/buffer.o
if errorlevel 1 goto error

echo Compiling tui.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/tui.c -o obj/tui.o
if errorlevel 1 goto error

echo Compiling input.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/input.c -o obj/input.o
if errorlevel 1 goto error

echo Compiling fileio.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/fileio.c -o obj/fileio.o
if errorlevel 1 goto error

echo Compiling file_ops.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/file_ops.c -o obj/file_ops.o
if errorlevel 1 goto error

echo Compiling editor.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/editor.c -o obj/editor.o
if errorlevel 1 goto error

echo Compiling main.c...
gcc -Wall -Wextra -std=c99 -O2 -DPLATFORM_WINDOWS -Iinclude -c src/main.c -o obj/main.o
if errorlevel 1 goto error

REM Link executable
echo Linking...
gcc obj/*.o -o bin/6r.exe
if errorlevel 1 goto error

echo Build successful!
echo Executable: bin\6r.exe
goto end

:error
echo Build failed!
exit /b 1

:clean
echo Cleaning build artifacts...
if exist obj del /Q obj\*
if exist bin del /Q bin\*
echo Cleaned.

:rebuild
call :clean
call :all

:run
if exist bin\6r.exe (
    echo Running 6r Text Editor...
    bin\6r.exe
) else (
    echo Please build first using: build.bat
)

:end
if "%1"=="clean" goto clean
if "%1"=="rebuild" goto rebuild  
if "%1"=="run" goto run