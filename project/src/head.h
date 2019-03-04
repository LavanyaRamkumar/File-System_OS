//11824
//i112
//d512
//s48
#include <fuse.h>	
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<stdbool.h>
#include<time.h>
#include <sys/types.h>
#include<unistd.h>
#include<errno.h>
//inodes- 4 per block
//inode size- 128
//128*4=512
//total size=12800
#define BLOCK_SIZE 512
#define NPB 4
#define T_BLOCKS 50
#define IMAX 32				//max files/dir that can be stored in a dir
#define IBLOCK 8
#define DP 8
//1 super
//8 inode blocks
//41 data blocks
#define DATA 41//41 data blocks

typedef struct dblock
{
	char data[BLOCK_SIZE-DP];
	int next;
	int prev;
}DBLOCK;

typedef struct inode
{
	char name[32];
	
	//STAT* info;
	int inum;
	mode_t mode;
	int link;
	int uid;
	int gid;
	int size;
	int blksize;
	int blkcnt;
	time_t acc_t;
	time_t mod_t;
	time_t cre_t;
	
	int db;
	int next;
	int prev;
	int in;
	unsigned int fd;
}INODE;

typedef struct super
{
	bool imap[IMAX];
	bool bmap[DATA];
}SUPER;

typedef struct fs
{
	SUPER sup;
	INODE inodes[IMAX];
	DBLOCK dbs[DATA];
}FS;

FS fs;
FS* ffss;
