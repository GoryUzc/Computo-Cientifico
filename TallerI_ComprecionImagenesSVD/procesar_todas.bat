@echo off
REM Script para procesar todas las 8 imágenes con SVD

echo ========================================
echo PROCESAMIENTO SVD - TODAS LAS IMAGENES
echo ========================================
echo.

setlocal
cd /d "%~dp0build\Release"

REM Variable del directorio raíz
set "RAIZ=%~dp0"

echo Procesando 8 imagenes...
echo.

REM Imagen 1 - Gris
echo Procesando: edificio_gris_1.png (Gris)
svd_compresion.exe --imagen "%RAIZ%assets\grises\edificio_gris_1.png" --modo gris --id 1
echo.

REM Imagen 2 - Gris
echo Procesando: geometria_gris_2.png (Gris)
svd_compresion.exe --imagen "%RAIZ%assets\grises\geometria_gris_2.png" --modo gris --id 2
echo.

REM Imagen 3 - Gris
echo Procesando: lena_gris_3.png (Gris)
svd_compresion.exe --imagen "%RAIZ%assets\grises\lena_gris_3.png" --modo gris --id 3
echo.

REM Imagen 4 - Gris
echo Procesando: mandril_gris_4.png (Gris)
svd_compresion.exe --imagen "%RAIZ%assets\grises\mandril_gris_4.png" --modo gris --id 4
echo.

REM Imagen 5 - Color
echo Procesando: edificio_rgb_5.png (Color)
svd_compresion.exe --imagen "%RAIZ%assets\rgb\edificio_rgb_5.png" --modo color --id 5
echo.

REM Imagen 6 - Color
echo Procesando: lena_rgb_6.png (Color)
svd_compresion.exe --imagen "%RAIZ%assets\rgb\lena_rgb_6.png" --modo color --id 6
echo.

REM Imagen 7 - Color
echo Procesando: mandril_rgb_7.png (Color)
svd_compresion.exe --imagen "%RAIZ%assets\rgb\mandril_rgb_7.png" --modo color --id 7
echo.

REM Imagen 8 - Color
echo Procesando: pimiento_rgb_8.png (Color)
svd_compresion.exe --imagen "%RAIZ%assets\rgb\pimiento_rgb_8.png" --modo color --id 8
echo.

echo ========================================
echo PROCESO COMPLETADO
echo ========================================
echo Resultados en: %RAIZ%resultados\
echo   - Reconstrucciones: resultados\reconstrucciones\
echo   - Datos CSV: resultados\datos\
echo   - Graficos: resultados\graficos\
echo ========================================

endlocal