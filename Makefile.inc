PLUGIN			:=	db
MEM_USAGE		:=	20

PLUGIN_DIR		:= 	src

APPINFO			:=	appinfo.json

ifeq ($(DEVICE),pre)
	DEVICEOPTS	:= -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
	PARCH		:=	armv7
else
ifeq ($(DEVICE),pixi)
	DEVICEOPTS	:= -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
	PARCH		:=	armv6
else
	PARCH		:=	i686
endif
endif
