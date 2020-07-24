#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

bool read_directory(char* name, char* name_c, char path_e[50]) // "name_c" is the name of command given
{
	DIR* directory = opendir(name);
	struct dirent * diret;
	while ((diret = readdir (directory)) != NULL)
	{
		vector<string> v;
		v.push_back(diret->d_name);
		if (strcmp(name_c, v[0].c_str()) == 0)
		{
			strcpy(path_e, name);
			closedir(directory);
			return true;
		}	
	}
	closedir(directory);
	return false;
}
//system calls of opendir, readdir, and struct dirent href: cplusplus.com

void string_extra(char* arr, const int j, char* extra)
{
	int count = 0, i = 0, k = 0;
	for ( i = 0; count != j; i++)
	{
		if (arr[i] == '/')
			count++;
	}
	while (arr[i] != '\0')
		extra[k++] = arr[i++];
	extra[k] = '\0';
}

void string_tok_length(char* arr, int &j, char delim)
{
	for (int i = 0; arr[i] != '\0'; i++)
	{
		if (arr[i] == delim)
			j++;
	}
}

void remove_first_letter(char*& arr)
{
	char* p = nullptr;
	p = arr;
	p++;
	arr = p;
}

void remove_last_letter(char*&arr)
{
	arr[strlen(arr) - 1] = '\0';
}

void string_tok_pipe(char** &path, char* arr, const char* c, const int j, const int k) // modified funciton specially for pipes
{
	path[k] = strtok(arr, c);
	for (int i = k + 1; i <= j; i++)
		path[i] = strtok(NULL, c);
}

void string_tok(char** &path, char* arr, const char* c, const int j, const int k)
{
	path[k] = strtok(arr, c);
	for (int i = k + 1; i < j; i++)
		path[i] = strtok(NULL, c);
}

void De_Allocate_single(char* arr)
{
	delete[] arr;
	arr = NULL;
}

void De_Allocate_double(char** arr, const int j)
{
	delete[] arr;
}

bool check_cd(char* ar)
{
	if (strcmp(ar, "cd") == 0)
		return true;
	else 
		return false;
}

void exe_cd(char* path)
{
	char p[200] = {'\0'};
	int c = chdir(path);
	if (c != 0)
		perror("\nPath cannot be changed!");
	else
	{
		getcwd(p, 200);
		cout << "path has been changed to " << p << endl; 
	}
}

bool check_path(char* ar)
{
	for (int i = 0; ar[i] != '\0'; i++)
	{
		if (ar[i] == '/')
			return true;
	}
	return false;
}

int seperate_executer(char* ar, char* arg, char* arr)
{
	// arr for initial path, ar for command name, arg for arguments, extra for seperating command and path and path** for seperated paths by ':'
	char** path, **argv, extra[10] = {'\0'};
	int k = 1, j = 1, slash = 0;
	if (strcmp(ar, "exit") == 0)
		exit(0);
	if (check_cd(ar))
		exe_cd(arg);
	bool f1 = false;
	f1 = check_path(ar);
	if (f1)
	{	
		string_tok_length(ar, slash, '/');
		string_extra(ar, slash, extra);
	}
	string_tok_length(arg, k, ' ');
	string_tok_length(arr, j, ':');
	path = new char*[j];
	argv = new char*[k];
	string_tok(path,arr, ":",j, 0);
	// path** has successfully stored all the paths in "PATHS" variable
	if (k > 1) // given command has arguments
	{
		if (f1)
			argv[0] = extra;
		else
			argv[0] = ar;
		string_tok(argv, arg, " ", k, 1);
	}
	else if (k == 1) // if the command has no arguments
	{
		if (f1)
			argv[0] = extra;
		else
			argv[0] = ar;
		argv[1] = arg;
	} 
	// argv** has successfully stored all the commands + names in char *arg
		char path_e[50] = {'\0'};
		bool flag = false;
		if (!f1)
		{
			for (int i = 0; i < j && !flag; i++)
			{
				flag = read_directory(path[i], ar, path_e);
			}
			if (!flag)
			{	
				De_Allocate_double(path, j);
				De_Allocate_double(argv, k);
			}
			else if (flag)
			{
				strcat(path_e, "/");
				strcat(path_e, ar);
			}
		}
		pid_t pid1 = fork();
		if (pid1 == 0)
			execv(path_e, argv);
		else if (pid1 > 0)
			wait(NULL);
	return 0;
}


void rem_f_l(char**& arr, const int n)
{
	for (int i = 1; i < n + 1; i++)
		remove_first_letter(arr[i]);
	for (int i = 0; i < n; i++)
		remove_last_letter(arr[i]);
}

void store_file_names(char* arr, char*& f_ioc, const int index, const int j)
{
	int k = 0;
	for (int i = index; arr[i] != ' ' && arr[i] != '\0'; i++, k++)
			f_ioc[k] = arr[i];
	f_ioc[k] = '\0';		
}

//have taken double pointer of char**

int check_file_io(char* arr)
{
	int a = 0;
	for (int i = 0; arr[i] != '\0' ; i++)
	{
		if (arr[i] == '>' || arr[i] == '<')
			a++;
	}
	return a;	
}

void check_file(char* arr, int *f_io, int& j, char** &f_ioc, const int &iter, int& k)
{
	int index = 0;
	for (int i = 0; arr[i] != '\0'; i++)
	{
		if (arr[i] == '>') // input from command so write in file
		{
			f_io[k++] = 1;
			f_io[k++] = iter;
			index = i;
			i += 2;
			store_file_names(arr, f_ioc[j], i, j);
			arr[index - 1] = '\0';
			j++;
		}
		else if (arr[i] == '<') // input from file so read in file
		{
			f_io[k++] = 0;
			f_io[k++] = iter;
			index = i;
			i += 2;
			store_file_names(arr, f_ioc[j], i, j);
			arr[index - 1] = '\0';
			j++;
		}
	}	
}

void check_space(char* arr)
{
	for (int i = 1; arr[i] != '\0'; i++)
		{
			if (arr[i] == ' ' && arr[i + 1] == '\0')
				arr[i] = '\0';
		}
}

int main()
{
	int f_io_c2 = 0, f_io_c = 0, *f_io = nullptr, out = 0, n_p = 0, k = 0, **n_ps = nullptr; // out for storing fd_out of screen, n_p stores the number of pipes in a command while **n_ps has the pipes fd
	char **f_ioc = nullptr, arr[1000] = {'\0'}, arrr[50] = {'\0'}, arg[30] = {' '}, **ar = NULL, **argv = NULL, *arr_path = getenv("PATH"); // arr holds the whole command, ar holds each command seperately
	cout << "Enter Command: ";
	cin.getline(arr, 1000);
	int f_io_length = check_file_io(arr);
	f_io = new int[f_io_length * 2];
	f_ioc = new char*[f_io_length];
	out = dup(1); // out now has fd_out of screen
	getcwd(arrr, 50);
	strcat(arr_path, ":");
	strcat(arr_path, arrr);
	string_tok_length(arr, n_p, '|'); // detect the number of pipes
	n_ps = new int*[n_p];
	ar = new char*[n_p + 1];
	for (int i = 0; i < n_p; i++)
		n_ps[i] = new int[2];
	for (int i = 0; i < n_p + 1; i++)
		ar[i] = new char[100];
	for (int i = 0; i < f_io_length; i++)
		f_ioc[i] = new char[100];
	for (int i = 0; i < n_p; i++)
		pipe(n_ps[i]);
	string_tok_pipe(ar, arr, "|", n_p, 0);
	rem_f_l(ar, n_p);
	for (int i = 0; i <= n_p ; i++)
		check_file(ar[i], f_io, f_io_c, f_ioc, i, f_io_c2);
	check_space(ar[n_p]);
	cout << endl;
	int fd1 = 0, fd2 = 0, pc = 0; // fd for fd count, io for 1 or 0
	bool flagfff = false;
	for (int i = 0; i <= n_p; i++)
		{
			if (f_io_length != 0 && pc < f_io_length)
			{
				if (f_io[pc + 1] == i)
				{
					int fd_f = 0;
					if (f_io[pc] == 1)
						fd_f = creat(f_ioc[pc], S_IRWXU);
					else if (f_io[pc] == 0)
						fd_f = open(f_ioc[pc], O_RDONLY);	
					dup2(fd_f, f_io[pc]);
					pc++;
					if (i == n_p)
						flagfff = true;
				}
			}
			if (n_p != 0)
			{
				if (i == n_p && !flagfff)
				{
					dup2(out, 1);
				}
				if (i >= 0 && i < n_p) // for output
				{
					dup2(n_ps[fd1++][1], 1);
				}
				if (i > 0 && i <= n_p) // for input
				{
					dup2(n_ps[fd2++][0], 0);
				}
			}
			string_tok_length(ar[i], k, ' ');
			if (k != 0)
			{
				argv = new char*[k + 1];
				char cc[250] = {'\0'};
				string_tok(argv, ar[i], " ", k + 1, 0);
				if ((strcmp(argv[0], "echo") == 0) || argv[1][0] == '"' )
				{
					for (int i = 1; i <= k; i++)
					{
						strcat(cc, argv[i]);
						strcat(cc, {" "});
					}
					argv[1] = cc;	
				}
			}
			else
			{
				argv = new char*[2];
				argv[0] = ar[i];
				argv[1] = arg;
			}
			pid_t pid = fork();
			if (pid == 0)
			seperate_executer(argv[0], argv[1], arr_path);
			else if (pid > 0)
			{
				wait(NULL);
				if (i >= 0 && i < n_p) // for output
				{
					close(n_ps[fd1 - 1][1]);
				}
				if (i > 0 && i <= n_p) // for input
				{
					close(n_ps[fd2 - 1][0]);
				}
			}
			k = 0;
		}
	for (int i = 0; i < n_p; i++)
	{
		close(n_ps[i][0]);
		close(n_ps[i][1]);
	}
		
	if (f_ioc)
	{
		delete[] f_ioc;
		f_ioc = nullptr;
	}
	if (n_ps)
	{
		delete[] n_ps;
		n_ps = nullptr;
	}
	if (f_io)
	{
		delete[] f_io;
		f_io = nullptr;
	}
	return 0;
}
