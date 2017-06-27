
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../bin/strader.out
## 编译器
COMPILER = g++

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
##CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE
## for release
CPP_FLAG = -g -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./../../library/public -I../../../library/third_party/breakpad/src 

## 库
LINK_PATHS =  -L/usr/local/lib
LINK_LIBRARYS = -lpthread -lexpat -ldl -levent

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output: loader.o native_library.o mig_log.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
loader.o:
	$(COMPILE) ./loader.cc

native_library.o:
	$(COMPILE) ./../../library/public/basic/native_library.cc

mig_log.o:
	$(COMPILE) ./../../library/public/log/mig_log.cc
