#include "head.h"
void disk_save()
{
	FILE *fd;
	fd=fopen("/home/lavanya/Desktop/project/bin.bin","wb+");
	fseek(fd,0,SEEK_SET);
	fwrite(ffss,sizeof(fs),1,fd);
	fclose(fd);
}
char* extract(const char* pa)
{
	if(strcmp(pa,"/")==0)
		return NULL;
	char path[strlen(pa)];
   strcpy(path,pa);
   const char s[2] = "/";
   char *token;
   int i=0,cnt=0,j=1;
   for(i=0;i<strlen(path);i++)
   {
   	if(path[i]=='/')	cnt++;
   }
   token = strtok(path, s);
   while( j<cnt) {
      token = strtok(NULL, s);
      j++;
   }
   return strtok(token, s);
}

int got(char* token,int start)
{
	int point=start;
	while(point!=-1)
   {
   	if(strcmp(fs.inodes[point].name,token)!=0)
   		point=fs.inodes[point].next;
   	else break;
   }
   return point;
}

int traverse_to(const char* pa,int* parent)
{
	printf("Traverse_to(%s)\n",pa);
	char path[strlen(pa)];
   strcpy(path,pa);
	if(strcmp(path,"/")==0)
	{
		*parent=-1;
		return 0;
	}
	int point=0;	
	int i=0,cnt=0;
	
	for(i=0;i<strlen(path);i++)
		if(path[i]=='/')
			cnt++;
			
	char* list[cnt];
	char s[2]="/";
	char* token= strtok(path, s);
	i=0;
	while( i<cnt) {
		list[i]=token;
      token = strtok(NULL, s);
      i++;
   }
   i=0;
   if(cnt==1)
	{
		(*parent)=0;
		return got(list[i],fs.inodes[point].in);
	}
   while(i<cnt)
   {
		point=fs.inodes[point].in;
		point=got(list[i],point);
		if(point==-1)
			return -1;
		i++;
		if(i==cnt)//cnt-1
		{
			return point;
		}
		if(i==cnt-1)//cnt-2
		{
			*parent=point;
		}
	}
	return -1;
}

int read_block(int dblock, char *buf,int new_off,int off,int n)
{
	printf("read_block\n");
	if(dblock==-1)
		return -1;
	if(n>BLOCK_SIZE-DP)
		return -1;	

	memcpy(buf+off,(fs.dbs[dblock].data)+new_off,n);
	return 0;
}

int write_block(int dblock,const char *buf,int new_off,int off,int n)
{
	printf("write_block\n");
	if(dblock==-1)
		return -1;
	if(n>BLOCK_SIZE-DP)
		return -1;
	memcpy((fs.dbs[dblock].data)+new_off, buf+off,n);
	return 0;
}

//initializes superblocks and blocks
static void *file_init(struct fuse_conn_info *conn)
{
	printf("INIT\n");
	ffss=&fs;
	FILE *fd=fopen("/home/lavanya/Desktop/project/bin.bin","rb+");
	fseek(fd,0,SEEK_END);
	long int p=ftell(fd);
	if(p!=0)
	{
		fseek(fd,0,SEEK_SET);
		fread(ffss,sizeof(fs),1,fd);
		fclose(fd);
		return 0;
	}
	fclose(fd);
	int i=0;
	for(i=0;i<IMAX;i++)
	{
		fs.sup.imap[i]=0;
		fs.inodes[i].db=-1;
		fs.inodes[i].next=-1;
		fs.inodes[i].prev=-1;
		fs.inodes[i].in=-1;
	}
	int j=0;
	for(j=0;j<DATA;j++)
	{
		fs.sup.bmap[j]=0;
		fs.dbs[j].next=-1;
		fs.dbs[j].prev=-1;
	}
	strcpy(fs.inodes[0].name,"root");
	fs.inodes[0].inum=0;
	fs.inodes[0].mode=S_IFDIR | 0775;
	fs.inodes[0].link=2;
	fs.inodes[0].uid=getuid();	//we need unistd.h for this
	fs.inodes[0].gid=getgid();
	fs.inodes[0].size=0;
	fs.inodes[0].blksize=0;
	fs.inodes[0].blkcnt=0;
	fs.inodes[0].cre_t=time(NULL);
	fs.inodes[0].mod_t=time(NULL);
	fs.inodes[0].acc_t=time(NULL);
	fs.inodes[0].db=-1;
	fs.inodes[0].next=-1;
	fs.inodes[0].prev=-1;
	fs.inodes[0].in=-1;
	fs.inodes[0].fd=0;
	fs.sup.imap[0]=1;
	return  0;
	
}

int file_create(const char *path, mode_t mode,struct fuse_file_info * fi)
{  
		printf("CREATE(%s)\n",path);
		if(strcmp(path,"/")==0)
			return -EEXIST;
		int par;
		int iaddr = traverse_to(path,&par);
		if(iaddr!=-1)
			return -EEXIST;
		if(par==-1)
			return -ENOENT;
		int n=IMAX;
		int i=0;
		for (i=0;i<n;i++){
			if(fs.sup.imap[i]==0)
				break;
		}
		int c=fs.inodes[par].in;
		strcpy(fs.inodes[i].name,extract(path));
		fs.inodes[i].inum=i;
		fs.inodes[i].mode=S_IFREG | 0664;
		fs.inodes[i].link=1;
		fs.inodes[i].uid=getuid();	//we need unistd.h for this
		fs.inodes[i].gid=getgid();
		fs.inodes[i].size=0;
		fs.inodes[i].blksize=0;
		fs.inodes[i].blkcnt=0;
		fs.inodes[i].cre_t=time(NULL);
		fs.inodes[i].mod_t=time(NULL);
		fs.inodes[i].acc_t=time(NULL);
		fs.inodes[i].db=-1;
		fs.inodes[i].next=-1;
		fs.inodes[i].prev=-1;
		fs.inodes[i].in=-1;
		fs.inodes[i].fd=0;
		fs.sup.imap[i]=1;
		
		if(c==-1)
			fs.inodes[par].in=i;
		else{
			while(fs.inodes[c].next!=-1){
				c=fs.inodes[c].next;
			}
			fs.inodes[c].next=i;
			fs.inodes[i].prev=c;}
	disk_save();
	return 0;
}
 

static int file_getattr(const char *path, struct stat *stbuf)
{	
	printf("GETATTR(%s)\n",path);
	int parent;
	int file=traverse_to(path,&parent);
	memset(stbuf,0,sizeof(struct stat));
	if(file!=-1)
		
	{
	stbuf->st_ino=fs.inodes[file].inum;
	stbuf->st_mode=fs.inodes[file].mode;	
	stbuf->st_nlink=fs.inodes[file].link;
	stbuf->st_uid=fs.inodes[file].uid;
	stbuf->st_gid=fs.inodes[file].gid;
	stbuf->st_size=fs.inodes[file].size;	
	stbuf->st_blksize=fs.inodes[file].blksize;
	stbuf->st_blocks=fs.inodes[file].blkcnt;
	stbuf->st_atime=fs.inodes[file].acc_t;
	stbuf->st_mtime=fs.inodes[file].mod_t;
	stbuf->st_ctime=fs.inodes[file].cre_t;
	}
	else 
		return -ENOENT;
	return 0;

}

static int file_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	filler(buf, ".", NULL,  0);
	filler(buf, "..", NULL,  0);
	int parent;
	int file=traverse_to(path,&parent);
	if(file==-1)
		return -ENOENT;
	int point=fs.inodes[file].in;
	while(point!=-1)
	{
		filler(buf,fs.inodes[point].name,NULL, 0);
		point=fs.inodes[point].next;
	}
	return 0;
}

static int file_mkdir(const char *path, mode_t mode)
{
		printf("CREATE(%s)\n",path);
		if(strcmp(path,"/")==0)
			return -EEXIST;
		int par;
		int iaddr = traverse_to(path,&par);
		if(iaddr!=-1)
			return -EEXIST;
		if(par==-1)
			return -ENOENT;
		int n=IMAX;
		int i=0;
		for (i=0;i<n;i++){
			if(fs.sup.imap[i]==0)
				break;
		}
		int c=fs.inodes[par].in;
		strcpy(fs.inodes[i].name,extract(path));
		fs.inodes[i].inum=i;
		fs.inodes[i].mode=S_IFDIR | 0775;
		fs.inodes[i].link=2;
		fs.inodes[i].uid=getuid();	//we need unistd.h for this
		fs.inodes[i].gid=getgid();
		fs.inodes[i].size=0;
		fs.inodes[i].blksize=0;
		fs.inodes[i].blkcnt=0;
		fs.inodes[i].cre_t=time(NULL);
		fs.inodes[i].mod_t=time(NULL);
		fs.inodes[i].acc_t=time(NULL);
		fs.inodes[i].db=-1;
		fs.inodes[i].next=-1;
		fs.inodes[i].prev=-1;
		fs.inodes[i].in=-1;
		fs.inodes[i].fd=0;
		fs.sup.imap[i]=1;
		
		if(c==-1)
			fs.inodes[par].in=i;
		else{
			while(fs.inodes[c].next!=-1){
				c=fs.inodes[c].next;
			}
			fs.inodes[c].next=i;
			fs.inodes[i].prev=c;}
	disk_save();
	return 0;
}


int file_open(const char * path, struct fuse_file_info * fi)
{
	int parent;
	int file=traverse_to(path,&parent);
	if(file!=-1)
	{
		fs.inodes[file].fd++;
		return 0;
	}
	return -ENOENT;
}

int file_read(const char * path, char * buf, size_t size, off_t off,struct fuse_file_info * fi)
{
	printf("READ(%s,%d,%d)\n",path,size,off);
	int parent;
	int file=traverse_to(path,&parent);
	if(file==-1)
		return -ENOENT;
	int block=fs.inodes[file].db;
	if(off>fs.inodes[file].size || size==0 ||block==-1)
		return 0;
	int new_off=(int)off%(BLOCK_SIZE-DP);
	int goblk=(int)off/(BLOCK_SIZE-DP);
	int i=0;
	for(i=0;i<goblk;i++)
		block=fs.dbs[block].next;
	int boff=0;
	int avail=BLOCK_SIZE-DP-new_off;
	int n = (size < avail) ? size : avail; 	
	int cnt=0;
	while(size>0)
	{
		printf("here");
		int k=read_block(block,buf,new_off,boff, n);
		if(k==0)	cnt=cnt+n;
		printf("block:%d\n",block);
		printf("------------------------------------------------------------------");
		printf(fs.dbs[block].data);
		block=fs.dbs[block].next;
		boff=boff+n;
		size=size-n;
		avail=BLOCK_SIZE-DP;
		n = (size > avail) ? avail : size;
		new_off=0;
 	}
 	printf("cnt:%d\n",cnt);
 	printf("READ(%s,%s,%d)\n",path,buf,size);
	return cnt;
}

int allocate_new_block(int block,int file)
{
	int i=0;
	for (i=0;i<DATA;i++){
		if(fs.sup.bmap[i]==0)
			break;
	}
	int new_block=i;
	fs.dbs[block].next=new_block;
	fs.dbs[new_block].next=-1;
	fs.dbs[new_block].prev=block;
	fs.sup.bmap[i]=1;
	fs.inodes[file].blkcnt=fs.inodes[file].blkcnt+1;
	return new_block;
}

int file_write(const char * path, const char * buf, size_t size, off_t off,struct fuse_file_info * fi)
{
	
	printf("WRITE(%s,%s,%d,%d)\n",path,buf,size,off);
	int parent;
	int file=traverse_to(path,&parent);
	if(file==-1)
		return -ENOENT;
	int block=fs.inodes[file].db;
	
	if(off>fs.inodes[file].size || size==0)
		return -errno;	
	if(fs.inodes[file].db==-1)
	{
		int i=0;
		for (i=0;i<DATA;i++){
			if(fs.sup.bmap[i]==0)
				break;
		}
		fs.inodes[file].db=i;
		block=i;
		fs.dbs[block].next=-1;
		fs.dbs[block].prev=-1;
		fs.sup.bmap[i]=1;
		fs.inodes[file].blkcnt=1;
	}
	int new_off=(int)off%(BLOCK_SIZE-DP);
	int goblk=(int)off/(BLOCK_SIZE-DP);
	int j=0;
	for(j=0;j<goblk;j++)
	{
		if(fs.dbs[block].next==-1) 
			block=allocate_new_block(block,file);
		else
			block=fs.dbs[block].next;
	}
	int boff=0;
	int avail=BLOCK_SIZE-DP-new_off;
	int n = (size < avail) ? size : avail; 	
	int cnt=0;
 	if(off+size>fs.inodes[file].size)
 	{
			fs.inodes[file].size=off+size;
	}
	while(size>0)
	{
		int k=write_block(block,buf,new_off,boff, n);
		if(k==0)	cnt=cnt+n;
		boff=boff+n;
		size=size-n;
		avail=BLOCK_SIZE-DP;
		n = (size > avail) ? avail : size;
		new_off=0;
		if(fs.dbs[block].next==-1 && size>0) 
			block=allocate_new_block(block,file);
		else
			block=fs.dbs[block].next;
 	}
 	printf("cnt:%d\n",cnt);
 	printf("file size:%d\n",fs.inodes[file].size);
 	disk_save();
 	return cnt;
}

int file_rmdir(const char * path)
{
	printf("RMDIR(%s)\n",path);
	int parent;
	int file=traverse_to(path,&parent);
	if(fs.inodes[file].in!=-1)
		return -ENOTEMPTY;
	if(fs.inodes[parent].in==file)
	{
		int next=fs.inodes[file].next;
		fs.inodes[parent].in=next;
		if(next!=-1)
			fs.inodes[next].prev=-1;
	}
	else if(fs.inodes[file].next!=-1)
	{
		fs.inodes[fs.inodes[file].prev].next=fs.inodes[file].next;
		fs.inodes[fs.inodes[file].next].prev=fs.inodes[file].prev;
	}
	else
	{
		fs.inodes[fs.inodes[file].prev].next=-1;
	}
	fs.sup.imap[file]=0;
	disk_save();
	return 0;
}

int file_unlink(const char * path)
{
	printf("UNLINK(%s)\n",path);
	int parent;
	int file=traverse_to(path,&parent);
	if(fs.inodes[file].in!=-1)
		return -ENOTEMPTY;
	if(fs.inodes[parent].in==file)
	{
		int next=fs.inodes[file].next;
		fs.inodes[parent].in=next;
		if(next!=-1)
			fs.inodes[next].prev=-1;
	}
	else if(fs.inodes[file].next!=-1)
	{
		fs.inodes[fs.inodes[file].prev].next=fs.inodes[file].next;
		fs.inodes[fs.inodes[file].next].prev=fs.inodes[file].prev;
	}
	else
	{
		fs.inodes[fs.inodes[file].prev].next=-1;
	}
	if(fs.inodes[file].db!=-1)
	{
		int start=fs.inodes[file].db;
		while(start!=-1)
		{
			fs.sup.bmap[start]=0;
			fs.dbs[start].prev=-1;
			start=fs.dbs[start].next;
			fs.dbs[start].next=-1;
		}
	}
	fs.inodes[file].db=-1;
	fs.sup.imap[file]=0;
	disk_save();
	return 0;
}

void file_destroy(void *private_data)
{	
	printf("DESTROY\n");
	disk_save();
	return;
}
int file_utime(const char * path, struct utimbuf *buf)
{
	printf("UTIME(%s)\n",path);
	int parent;
	int file=traverse_to(path,&parent);
	printf("%l",buf->actime);
	fs.inodes[file].acc_t=buf->actime;
	fs.inodes[file].mod_t=buf->modtime;
	return 0;
}
int file_rename(const char *old, const char *new)
{
	printf("RENAME(%s,%s)\n",old,new);
	int parent;
	int file=traverse_to(old,&parent);
	int filenew=traverse_to(new,&parent);
	if(filenew!=-1)
		return -EEXIST;
	if(file!=-1)
	{
		strcpy(fs.inodes[file].name,extract(new));
	}
	disk_save();
	return 0;
}

int file_truncate(const char * path, off_t off)
{
	printf("TRUNCATE(%s)\n",path);
	return 0;
}

int file_chmod(const char* path, mode_t mode)
{
	int parent;
	int file=traverse_to(path,&parent);
	fs.inodes[file].mode=mode;
	return 0;
}
static struct fuse_operations operations = {
  .create    	=file_create,
  .init		=file_init,
  .getattr = file_getattr,
  .utime		=file_utime,
  .mkdir        = file_mkdir,
  .readdir	= file_readdir,
  .rmdir		=file_rmdir,
  .unlink	=file_unlink,
  .open		= file_open,
  .read		=file_read,
  .write    =file_write,
  .rename	=file_rename,
  .destroy  =file_destroy,
  .truncate =file_truncate,
  .chmod= file_chmod
};

int main(int argc, char *argv[]) 
{
	return fuse_main(argc, argv, &operations);
}

