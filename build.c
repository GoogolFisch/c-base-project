#include"./builder/util.h"


void bmain_rebuild_self(int32_t argc,char **argv,struct Tasks *t){
	argc = argc;
	task_set_file(t,"gcc");
	task_add_args_many(t,"./build.c","-o","boot","-g","-Wall","-Wextra",NULL);
	task_exec(t);
	task_clear_args(t);
	if(!string_end_cmp(argv[0],"/boot")){
		if(remove(argv[0]) == 0)
			printf("./boot\n");
		else
			fprintf(stdout,"Error: %s couldn't be deleted",argv[0]);
	}
}
int isForTestingFile(struct dirent *dp){
	if(string_end_cmp(dp->d_name,".c"))
		return 1;
	return 0;
}
void bmain_test_execute(int32_t argc,char **argv,char *path,struct Tasks *t){
	argc = argc;
	argv = argv;
	char *copyName = string_copy_margin(path,4);
}

int32_t main(int32_t argc,char **argv){
	struct Strings *flags = NULL;
	struct Tasks t = {0};
	if(task_needs_rebuild_many("./boot","./build.c",NULL)){
		bmain_rebuild_self(argc,argv,&t);
	}
	int isDebugMode = 1;
	int isTestingMode = 0;
	if(argc > 1){
		if(strcmp(argv[1],"release")){
			isDebugMode = 0;
		}
		if(strcmp(argv[1],"test")){
			isTestingMode = 1;
		}
	}
	if(isDebugMode)
		strings_append_many(&flags,"-g","-fsanitize=address,unreachable,undefined","-Wall","-Wextra",NULL);
	if(isTestingMode){
		struct Strings *foundStr = task_walk_direntry_filtered("./test/",&isForTestingFile);
		char *path = strings_pop(&foundStr);
		while(path){
			bmain_test_execute(argc,argv,path,&t);
			path = strings_pop(&foundStr);
		}
	}
	//
	task_await(&t);
	return 0;
}
