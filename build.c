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
int isForTestingFile(char *fpath,struct dirent *dp){
	if(string_end_cmp(fpath,".c")){
		//printf("%s\n",fpath);
		return 1;
	}
	return 0;
}
char *bmain_test_compile(int32_t argc,char **argv,char *path,struct Strings **flags,struct Tasks *t){
	argc = argc;
	argv = argv;
	char *copyName = string_copy_margin(path,4);
	int32_t ending = strlen(copyName);
	copyName[ending - 1] = 'o';
	copyName[ending    ] = 'u';
	copyName[ending + 1] = 't';
	copyName[ending + 2] =   0;
	task_clear_args(t);
	task_set_file(t,"gcc");
	task_add_args_merge(t,*flags);
	task_add_args_many(t,path,"-o",copyName,NULL);
	task_exec(t);
	return copyName;
}
void bmain_test_execute(int32_t argc,char **argv,char *path,struct Strings **flags,struct Tasks *t){
	argc = argc;argv = argv;flags = flags;
	task_set_file(t,path);
	task_exec(t);
}
void bmain_run_tests(int32_t argc,char **argv,struct Strings **flags,struct Tasks *t){
	struct Strings *foundStr = task_walk_direntry_filtered("./test/",&isForTestingFile);
	if(foundStr == NULL){
		printf("No tests found!\n");
		return;
	}
	for(int32_t idx = 0;idx < foundStr->count;idx++){
		char *path = foundStr->strs[idx];
		foundStr->strs[idx] = bmain_test_compile(argc,argv,path,flags,t);
		free(path);
	}
	task_await(t);
	task_clear_args(t);
	for(int32_t idx = 0;idx < foundStr->count;idx++){
		bmain_test_execute(argc,argv,foundStr->strs[idx],flags,t);
		free(foundStr->strs[idx]);
	}
	strings_free(&foundStr);
}

int32_t main(int32_t argc,char **argv){
	struct Strings *flags = NULL;
	struct Tasks t = {0};
	int isDebugMode = 1;
	int isTestingMode = 0;
	int isAllRebuild  = 0;
	if(argc > 1){
		if(strcmp(argv[1],"release") == 0){
			isDebugMode = 0;
		}
		if(strcmp(argv[1],"test") == 0){
			isTestingMode = 1;
		}
		if(strcmp(argv[1],"all") == 0){
			isAllRebuild = 1;
		}
	}
	if(isAllRebuild || task_needs_rebuild_many("./boot","./build.c","./builder/util.h",NULL)){
		bmain_rebuild_self(argc,argv,&t);
	}
	if(isDebugMode){
		//strings_append_many(&flags,"-g",
		//      "-fsanitize=address,unreachable,undefined",
		//      "-Wall","-Wextra",NULL);
		strings_append_many(&flags,"-g","-Wall","-Wextra",NULL);
	}
	if(isTestingMode){
		bmain_run_tests(argc,argv,&flags,&t);
		task_await(&t);
		return 0;
	}
	//
	task_await(&t);
	return 0;
}
