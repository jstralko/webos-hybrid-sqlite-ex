include ../Makefile.inc

PALM_PDK_ROOT	:=	/opt/PalmPDK

PALM_PDK_INC	:= 	$(PALM_PDK_ROOT)/include

CC		:=  	arm-none-linux-gnueabi-gcc

CFLAGS		:=	-g $(DEVICEOPTS)

INCLUDES 	:= 	-I$(PALM_PDK_INC) \
			-I$(PALM_PDK_INC)/SDL \
			-Isqlite-3 

SYSROOT		:=	$(PALM_PDK_ROOT)/arm-gcc/sysroot

LIBDIRS		:= 	-L. -L$(PALM_PDK_ROOT)/device/lib

LDFLAGS		:= 	-Wl,--allow-shlib-undefined -Wl,-rpath-link,$(PALM_PDK_ROOT) -Wl,-rpath,-noinhibit-exec
#LDFLAGS		:= 	-Wl,--allow-shlib-undefined -Wl,-rpath-link,$(PALM_PDK_ROOT) -Wl,-rpath,.

LIBS		:=  	-lpdl -lSDL -lpthread -ldl

OBJECTS		:= 	list.o string_ops.o database.o main.o sqlite3.o

SQLITE_DIR	:=	sqlite-3

.PHONY		:	clean install package.properties

$(PLUGIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBDIRS) $(LDFLAGS) $(LIBS) $(OBJECTS) -o $(PLUGIN)

list.o: %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) --sysroot=$(SYSROOT) -c $< -o $@

string_ops.o: %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) --sysroot=$(SYSROOT) -c $< -o $@

database.o: %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) --sysroot=$(SYSROOT) -c $< -o $@

main.o: %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) --sysroot=$(SYSROOT) -c $< -o $@

sqlite3.o:
	cd $(SQLITE_DIR); ${MAKE}; cp sqlite3.o ../

clean:
	rm -f *~; rm -rf *.o $(PLUGIN) ../$(PLUGIN) ../package.properties

install: $(PLUGIN) package.properties
	cp $(PLUGIN) ../; rm $(PLUGIN)

package.properties:
	echo "filemode.755=$(PLUGIN)" > ../package.properties
