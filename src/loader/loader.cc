#include "basic/basictypes.h"
#include "basic/native_library.h"
#include "log/mig_log.h"
#include <stdio.h>
#include <signal.h>
//#include "client/linux/handler/exception_handler.h"


typedef int32 (*CorePlugin)(int32 argc,char* argv[]);


basic::libhandle  handle_lancher = NULL;

static bool DumpCallBack(const char* dump_path,const char* minidump_id,
						 void* contect,bool succeeded){

    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);

    return succeeded;
}


int main (int32 argc,char* argv[]){
#ifdef SIGPIPE
	signal (SIGPIPE, SIG_IGN);
#endif
//	google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,NULL,true);
	int32 rc;
	//init 
	handle_lancher = basic::load_native_library("./plugins/core/core.so");
	if (handle_lancher==NULL){
		MIG_ERROR(USER_LEVEL,"Can't load path robot.so\n");
		return true;
	}

	CorePlugin entry_point = (CorePlugin)
		basic::get_function_pointer(handle_lancher,"core_main");
	
	if (entry_point==NULL){
		MIG_ERROR(USER_LEVEL,"Can't get function::core_main");
		return true;
	}

	rc = entry_point(argc,argv);

	basic::unload_native_library(handle_lancher);
}
