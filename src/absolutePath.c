#include "absolutePath.h"

char * give_relative_or_absolute_path(char * curr_dir,char* home_dir)
{
	int curr_dir_len = strlen(curr_dir);
	int home_dir_len = strlen(home_dir);
	int curr_dir_ind = 0;
	int not_found = false;
	if(curr_dir_len < home_dir_len)
	{
		return curr_dir;
		 
	}
	for(int i=0;i<home_dir_len;i++)
	{
		if(home_dir[i] != curr_dir[curr_dir_ind])
		{
			not_found = true;
			break;
		}
		curr_dir_ind++;
	}
	if(not_found)
		return curr_dir;
	else if(curr_dir_len == home_dir_len)
	{
		char *str = (char *)malloc(4);
		strcpy(str,"~");
		return str;
	}
	else
	{
		char * str = (char* )malloc(curr_dir_len - home_dir_len + 3);
		int j = home_dir_len+1;
		for(int i=0;i<curr_dir_len-home_dir_len;i++)
		{
			str[i] = curr_dir[j];
			j++;
		}
		return str;
	}
	return curr_dir;
}
char* giveAbsolutePath(char *path)
{
    char *build_path  = (char *)malloc(255); 
    //char buf[255];
    if(path[0]=='/')
    {
        return path;
    }
   	getcwd(build_path,255);
    strcat(build_path,"/");
    strcat(build_path,path);
    return build_path;

}