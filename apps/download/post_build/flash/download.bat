@echo off  
  
@echo *** AC692X Loader *************************************************************
@echo  SDK VER    - 
@echo  SDK PROJ   -
@echo  Build Time -%time%
@echo  Build Date - %date%
@echo off
@echo *** BY BIOS - mail.scmc.tech@gmail.com  ***************************************


cd %~dp0
REM  echo %1
REM  echo %2
REM  echo %3
if exist uboot.boot del uboot.boot
type uboot.bin > uboot.boot

cd ui_resource
copy *.* ..\
cd ..

cd tone_resource
copy *.mp3 ..\
cd ..
isd_download.exe -tonorflash -dev br21 -boot 0x2000 -div6 -wait 300 -format cfg -format vm -f uboot.boot sdk.app

if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist jl_692x.bin del jl_692x.bin
if exist jl_cfg.bin del jl_cfg.bin


rename jl_isd.bin jl_692x.bin
bfumake.exe -fi jl_692x.bin -ld 0x0000 -rd 0x0000 -fo updata.bfu

@rem rename jl_isd.bin jl_cfg.bin
@rem bfumake.exe -fi jl_cfg.bin -ld 0x0000 -rd 0x0000 -fo jl_692x.bfu
@rem copy /b jl_692x.bfu+jl_flash_cfg.bin  jl_692x.bfu

IF EXIST no_isd_file del jl_692x.bin jl_cfg.bin


@rem format vm        //����VM 68K����
@rem format cfg       //����BT CFG 4K����
@rem format 0x3f0-2  //��ʾ�ӵ� 0x3f0 �� sector ��ʼ�������� 2 �� sector(��һ������Ϊ16���ƻ�10���ƶ��ɣ��ڶ�������������10����)


