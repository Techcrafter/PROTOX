@echo off
:AGAIN
cd src/tilemap
C:\CEdev\bin\convbin --iformat csv -i tilemap.csv -o tilemap.c --oformat c --name tilemapM
cd ../gfx
C:\CEdev\bin\convimg -i convimg.yaml
cd ../..
make -B
echo ------------------------------Done!------------------------------
pause > nul
goto AGAIN