#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>


struct message
{
	int count;
	char m[1000];
	int exitStat;
	int users;
};

struct user
{
	struct message *global;
	char *name;
	int id;
};


void *inp(void *ptr)
{
	char *c;
	char *s=malloc(1000*sizeof(char));
	struct user *x;
	x=(struct user *)ptr;
	c=x->global->m;
	//sleep(0.6);
	while(1>0)
	{
		//printf("(Global Chat)You:");
		fgets(s,1000,stdin);
		//printf("%d\n", strcmp(s,"exit"));
		if(strcmp(s,"exit\n")==0)
		{
			x->global->exitStat=x->id;
			return NULL;
		}
		else if(s[0]=='$')
		{
			///printf("level1\n");
			char *reciever=malloc(1000*sizeof(char));
			strcpy(reciever,s+1);
			char *b=(char *)malloc(1000*sizeof(char));
			reciever[strlen(reciever)-1]='\0';
			getcwd(b,1000);
			strcat(b,"/");
			strcat(b,reciever);
			///printf("%s\n", b);
			int key2,shmidP;
			if((key2=ftok(b,'C'))!=-1)
			{
				//printf("level1\n");
				if((shmidP=shmget(key2,1024,0644))!=-1)
				{
					//printf("level1\n");
					char *data=(char *)malloc(1024*sizeof(char));
					data=shmat(shmidP,(void *)0, 0);
					if(data != (char *)(-1))
					{
						//printf("level1\n");
						printf("(Personal Chat with %s)You:",reciever);
						fgets(s,1000,stdin);
						strcpy(data,(char *)(x->name));
						strcpy(data+20,s);
						shmdt(data);
						continue;
					}	
				}
			}
		}
		strcpy(c,(char *)(x->name));
		strcpy(c+20,s);
		///strcat(c+20,"(Global Chat)You:");
		//sleep(1);
		//printf("%s", c+20);
		//printf("%d\n", strcmp(c,"exit"));
	}
}

int main(int argc, char const *argv[])
{
	key_t key,key2;
	int shmid,shmidP;
	//char *global;

	struct message *global;
	struct user arguments;

	char *username=malloc(20*sizeof(char));
	//printf("%s\n", username);
	while(1>0)
	{
		printf("Enter your username:");
		fgets(username,20,stdin);
		//printf("%c\n", username[strlen(username)-2]);
		if(username[strlen(username)-1]=='\n')
		{
			username[strlen(username)-1]='\0';
		}
		int status=mkdir(username,0777);
		if(status==-1)
		{
			printf("The chosen name already exists!\n");
		}
		else
		{
			break;
		}
	}
	arguments.name=username;

	char *b=(char *)malloc(1000*sizeof(char));
	getcwd(b,1000);
	if((key=ftok(b,'C'))==-1)
	{
		perror("shmget");
		exit(1);
	}
	//printf("%d\n",key );
	strcat(b,"/");
	strcat(b,username);
	//printf("%s\n", b);
	if((key2=ftok(b,'C'))==-1)
	{
		perror("shmget");
		exit(1);
	}
	if((shmidP=shmget(key2,1024,IPC_CREAT | 0644))==-1)
	{
		perror("shmget");
		exit(1);
	}
	char *data=(char *)malloc(1024*sizeof(char));
	data=shmat(shmidP,(void *)0, 0);
	if(data == (char *)(-1))
	{
		perror("shmat");
		exit(1);
	}

	// char data[3];
	// int a=12;
	// sprintf(data,"%d",a);
	// printf("%s\n", data);
	if((shmid=shmget(key,sizeof(global),0644))!=-1)
	{
		//printf("%s\n","idhar" );
		global=shmat(shmid,(void *)0, 0);
		if(global == (struct message *)(-1))
		{
			perror("shmat");
			exit(1);
		}

		global->count++;
		global->users++;
	}
	else if((shmid=shmget(key,sizeof(global),IPC_CREAT | 0644))!=-1)
	{
		//printf("%s\n","here" );
		global=shmat(shmid,(void *)0, 0);
		if(global == (struct message *)(-1))
		{
			perror("shmat");
			exit(1);
		}

		global->count=1;
		global->exitStat=0;
		global->users=1;
	}
	else
	{
		perror("shmget");
		exit(1);		
	}
	printf("No. of connected Users:%d\n",global->count );
	printf("Use \"$name\" to send personal message to \"name\"\n");
	printf("Else, all messages would be sent to global chat\n");
	printf("Type \"exit\" to escape\n");
	arguments.id=global->users;
	arguments.global=global;

	pthread_t t1;
	pthread_create(&t1,NULL,inp,(void *)(&arguments));
    //printf("here\n");

    int flag=0;
    while(1>0)
    {
    	/*if(flag==0)
    	{
    		flag=1;
    		printf("(Global Chat)You:");
    	}*/
      	if(strlen(global->m+20)!=0 && strcmp(global->m,username)!=0)
    	{
    		printf("\r(Global Chat)%s:%s", global->m,global->m+20);
    		sleep(1);
    		//printf("ih\n");
    		char c[1];
    		sprintf(global->m+20,"%s",c);
    		//flag=0;
    	}

    	if(strlen(data+20)!=0)
    	{
    		printf("(Personal Chat)%s:%s", data,data+20);
    		sleep(1);
    		//printf("ih\n");
    		char c[1];
    		sprintf(data+20,"%s",c);
    		//flag=0;
    	}
    	/*else if(strlen(global->m+20)!=0)
    	{
    		flag=0;
    		sleep(0.5);
    		//printf("ih");
    		char c[1];
    		sprintf(global->m+20,"%s",c);
    	}*/
    	if(global->exitStat==arguments.id)
    	{
    		break;
    	}
    }
    //printf("now\n");
    //pthread_join(t1,NULL);
    //printf("here\n");




	//fgets(global,10000,stdin);


	if(shmdt(data)==-1)
	{
		perror("shmdt");
		exit(1);
	}
	shmctl(shmidP, IPC_RMID,NULL);
	remove(username);


	global->count--;
	if(global->count==0)
	{
		if(shmdt(global)==-1)
		{
			perror("shmdt");
			exit(1);
		}
		shmctl(shmid, IPC_RMID,NULL);
	}
	else
	{
		if(shmdt(global)==-1)
		{
			perror("shmdt");
			exit(1);
		}
	}
	return 0;
}