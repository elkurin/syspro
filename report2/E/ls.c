#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

void IntToString(char *str, int number)
{
	sprintf(str, "%d", number);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Path is not set.\n");
		exit(EXIT_FAILURE);
	}
	DIR *dir;
	char *path = argv[1];
	if ((dir = opendir(path)) == NULL) {
		perror(NULL);
		printf("ERROR : Failed to open directory.\n");
		return - 1;
	}
	struct dirent *get;
	get = (struct dirent *)malloc(sizeof(struct dirent));
	for (get = readdir(dir); get != NULL; get = readdir(dir)) {
		if (get->d_type == DT_DIR) continue;
		struct stat *buf;
		buf = (struct stat *)malloc(sizeof(struct stat));
		char pathname[1000];
		strcpy(pathname, argv[1]);
		strcat(pathname, get->d_name);
		if (stat(pathname, buf) == - 1) {
			perror(NULL);
			printf("ERROR : Failed to get status of \"%s\".\n", pathname);
			return - 1;
		}
		int m = buf->st_mode;
		int owner = m / 64 % 8;
		int group = m / 8 % 8;
		int other = m % 8;
		if (get->d_type == DT_DIR) printf("d");
		else printf("-");
		switch (owner) {
			case 0: printf("---"); break;
			case 1: printf("--x"); break;
			case 2: printf("-w-"); break;
			case 3: printf("-wx"); break;
			case 4: printf("r--"); break;
			case 5: printf("r-x"); break;
			case 6: printf("rw-"); break;
			case 7: printf("rwx"); break;
			default:
				 printf("ERROR : Failed to get permission status of \"%s\".\n", get->d_name);
				 return - 1;
		}
		switch (group) {
			case 0: printf("---"); break;
			case 1: printf("--x"); break;
			case 2: printf("-w-"); break;
			case 3: printf("-wx"); break;
			case 4: printf("r--"); break;
			case 5: printf("r-x"); break;
			case 6: printf("rw-"); break;
			case 7: printf("rwx"); break;
			default:
				 printf("ERROR : Failed to get permission status of \"%s\".\n", get->d_name);
				 return - 1;
		}
		switch (other) {
			case 0: printf("---"); break;
			case 1: printf("--x"); break;
			case 2: printf("-w-"); break;
			case 3: printf("-wx"); break;
			case 4: printf("r--"); break;
			case 5: printf("r-x"); break;
			case 6: printf("rw-"); break;
			case 7: printf("rwx"); break;
			default:
				 printf("ERROR : Failed to get permission status of \"%s\".\n", get->d_name);
				 return - 1;
		}
		printf(" ");
		
		printf("%ld %lld ", (long) buf->st_nlink, (long long) buf->st_size);

		int uid = buf->st_uid;
		int gid = buf->st_gid;
		FILE *fp;
		if ((fp = fopen("/etc/passwd", "r")) == NULL) {
			printf("ERROR : Failed to open \"/etc/passwd\".\n");
			return - 1;
		}
		char stat_uid_name[100], stat_gid_name[100];
		char stat_uid_num[100], stat_gid_num[100];
		IntToString(stat_uid_num, uid);
		IntToString(stat_gid_num, gid);
		char read_line[1000] = "\0";
		while(fgets(read_line, 10000, fp) != NULL) {
			char id_name[100];
			char id_num[100];
			int i;
			for (i = 0; read_line[i] != ':'; i++) id_name[i] = read_line[i];
			id_name[i] = '\0';
			int j;
			for (j = i + 3; isdigit(read_line[j]); j++) id_num[j - i - 3] = read_line[j];
			id_num[j - i - 3] = '\0';
			if (!strcmp(id_num, stat_uid_num)) strcpy(stat_uid_name, id_name);
			if (!strcmp(id_num, stat_gid_num)) strcpy(stat_gid_name, id_name);
		}
		printf("%s %s ", stat_uid_name, stat_gid_name);
		
		printf("%s %s\n", ctime(&buf->st_mtime), get->d_name);
	} 
	return 0;
}
