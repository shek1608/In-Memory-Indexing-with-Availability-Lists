#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct 
{ 
	int siz; /* Hole's size */
	long off; /* Hole's offset in file */ 
} avail_S;


typedef struct
{
	int key;
	long off;
} index_S;

char *fb;
FILE *fileStudent;
FILE *fileIndex, *fileAvail;
index_S objIndex[10001];
avail_S objAvail[10001];
int countFileIndex, countFileAvail;
int intFit;

int funcFind(int intKey,int start, int end)
{
	int mid;
	if(start<=end)
	{
		mid=(start+end)/2;
		if(objIndex[mid].key==intKey)	return mid;
		else if(objIndex[mid].key>intKey)	return funcFind(intKey,start,mid-1);
		else return funcFind(intKey,mid+1,end);
	}
	else return -1;

}


int comparAsc(const void *a1, const void *a2)
{
	int temp3,a;
	avail_S *b1 = (avail_S *)a1;
	avail_S *b2 = (avail_S *)a2;
	temp3 = (int)(b1->siz - b2->siz);
	//int a;
	return (temp3==0)? (int)(b1->siz - b2->siz): temp3;
}

int comparDesc(const void *a1, const void *a2)
{
	int temp3,a;
	avail_S *b2 = (avail_S *)a1;
	avail_S *b1 = (avail_S *)a2;
	temp3 = (int)(b1->siz - b2->siz);
	//int a;
	return (temp3==0)? (int)(b1->siz - b2->siz): temp3;
}

int comparIndex(const void *a1, const void *a2)
{
index_S *b1 = (index_S *)a1;
index_S *b2 = (index_S *)a2;
return (int)(b1->key - b2->key);
}

void funcDel(int Sid)
{
	int exists = -1;
	exists = funcFind(Sid,0,countFileIndex-1);
	if(exists==-1)
	{
		printf("No record with SID=%d exists\n", Sid);
	}
	else
	{
		//printf("Exists and can be deleted\n");

		
		FILE *fileFind;
		int fileFindsize;
		fileFind = fopen(fb,"r+b");
		objAvail[countFileAvail].off=objIndex[exists].off;
		fseek(fileFind,objIndex[exists].off,SEEK_SET);
		fread(&fileFindsize,sizeof(int),1,fileFind);
		//printf("size of fileFindsize = %d\n",fileFindsize);
		objAvail[countFileAvail].siz= sizeof(int) + fileFindsize;
		countFileAvail++;
		int i;
		/*for(i=0;i<countFileAvail;i++)
		{
			printf("Availability index = %d Size = %d Offset = %ld\n",i,objAvail[i].siz,objAvail[i].off);
		}
		*/

		for(i=exists;i<countFileIndex-1;i++)
			objIndex[i]=objIndex[i+1];
		countFileIndex--;
		int xyz;
		/*for(xyz=0;xyz<countFileIndex;xyz++) //uncomment to see what is in index struc
		{
			printf("Index %d - %d - %ld\n",xyz,objIndex[xyz].key,objIndex[xyz].off);
		}*/

		if(intFit==0)
		{
			//printf("intfit is zero, no sorting necssary\n");
		}
		else if(intFit==1)	qsort(objAvail,countFileAvail,sizeof(avail_S),comparAsc);
		else if(intFit==2)	qsort(objAvail,countFileAvail,sizeof(avail_S),comparDesc);



	}//end else

}

void funcAdd(int Sid, char *temp, char *arg)
{
	//printf("Here1");
	int abc;
	int exists = -1;
	int holeLocation = -1;
	int tempsize;
	tempsize = strlen(temp);

//int exists;
exists = funcFind(Sid,0,countFileIndex-1);
	if(exists<0)
	{
		if((fileStudent = fopen(arg,"r+b")) == NULL)
		{
			fileStudent = fopen(arg, "w+b");
			//printf("Not existsingg\n");
		}

		for(abc=0;abc<countFileAvail;abc++)
			if(objAvail[abc].siz>=tempsize+sizeof(int))
			{
				holeLocation = abc;
				break;
			}
		if(holeLocation!=-1)
		{
			//printf("Hole location not null....found a hole!\n");
			fseek(fileStudent,objAvail[holeLocation].off,SEEK_SET);
		}	
		/*///////////LOOP THROUGH AVAILABILITY LIST AND GET LOCATION*/
		else
		{
			//printf("Hole not present...end mein daalo\n");
			fseek(fileStudent,0,SEEK_END);	
		} 

		objIndex[countFileIndex].key = Sid;
		objIndex[countFileIndex].off = ftell(fileStudent);
		//printf("index added\n");
		countFileIndex++;


		fwrite(&tempsize,sizeof(int),1,fileStudent); //writing size
		fwrite(temp,1,tempsize,fileStudent);	//writing the data


		//printf("Index size=%d\n",countFileIndex );
		int xyz=0;
		for(xyz=0;xyz<countFileIndex;xyz++) //uncomment to see what is in index struc
		{
			//printf("Index %d - %d - %ld\n",xyz,objIndex[xyz].key,objIndex[xyz].off);
		}
				/* //Uncomment to test data in file
				fseek(fileStudent,0,SEEK_END);
				int tempfilesize = ftell(fileStudent);
				fseek(fileStudent,0,SEEK_SET);

				char *tempfilestream;
				tempfilestream = (char *)malloc(tempfilesize);
				fread(tempfilestream,tempfilesize,1,fileStudent);
				printf("FileSize = %d, file data = \n%s\n",tempfilesize,tempfilestream);
				*/

		if(holeLocation!=-1)
		{
			//printf("Need to update hole status here\n");
			int newSize;
			newSize = objAvail[holeLocation].siz - (tempsize+sizeof(int));
			int i;
			for(i=holeLocation;i<countFileAvail-1;i++)
			{
				objAvail[i]=objAvail[i+1];
			}
			if(newSize==0)
				countFileAvail--;
			else
			{
				objAvail[countFileAvail-1].siz = newSize;
				objAvail[countFileAvail-1].off = ftell(fileStudent);
			}
			//int i;
			/*
			for(i=0;i<countFileAvail;i++)
			{
				printf("Availability index = %d Size = %d Offset = %ld\n",i,objAvail[i].siz,objAvail[i].off);
			}
			*/
		
			//if(intFit!=0){}	//funcSortAvail();////////////////NEED TO CALL SORT HERE
			if(intFit==0)
			{
			//	printf("intfit is zero, no sorting necssary\n");
			}
			else if(intFit==1)	qsort(objAvail,countFileAvail,sizeof(avail_S),comparAsc);
			else if(intFit==2)	qsort(objAvail,countFileAvail,sizeof(avail_S),comparDesc);

		}
//	else 		printf("No need to update hole status here\n");


		/*/////////////NEED TO SORT HERE*/
				qsort(objIndex,countFileIndex,sizeof(index_S),comparIndex);
		


		fclose(fileStudent);
	}//end of record not already existing
	else
	{
		printf("Record with SID=%d exists\n",Sid);

	}
}


int main(int parameterCount, char* argument[])
{
//	printf("\n");
	//int intFit; //moved to global variables
	char input[1024], *temp;
	int Sid;

	countFileIndex = 0;
	countFileAvail = 0;

	//Check what user wants to do
	if(parameterCount!=3)
	{
		//printf("\nInvalid input\n");
		return 0;
	}
	if(!strcmp(argument[1],"--first-fit"))	
		intFit = 0;

	else if(!strcmp(argument[1],"--best-fit"))	
		intFit = 1;

	else if(!strcmp(argument[1],"--worst-fit"))	
		intFit = 2;
	char *fn = argument[2];
	fb = argument[2];

	fileStudent = fopen(fn,"r+b");
	if(fileStudent!=NULL)
	{
		fileAvail = fopen("fileAvail.bin","r+b");
		fileIndex = fopen("fileIndex.bin","r+b");
		if(fileAvail!=NULL)
		{
			//printf("\nOpened availability file");
			fseek(fileAvail,0,SEEK_END);
			int sizeFileAvail;
			sizeFileAvail = ftell(fileAvail);
			countFileAvail = sizeFileAvail/sizeof(avail_S);
			fseek(fileAvail,0,SEEK_SET);
			fread(objAvail,sizeof(avail_S),countFileAvail,fileAvail);
			fclose(fileAvail);

		}
		if(fileIndex!=NULL)
		{
			//printf("\nOpened Index file");
			fseek(fileIndex,0,SEEK_END);
			int sizeFileIndex;
			sizeFileIndex = ftell(fileIndex);
			countFileIndex = sizeFileIndex/sizeof(index_S);
			fseek(fileIndex,0,SEEK_SET);
			fread(objIndex,sizeof(index_S),countFileIndex,fileIndex);
			fclose(fileIndex);
		}


	}

	//printf("Index:\n");
	int tempPrint;
	/*
	for(tempPrint=0;tempPrint<countFileIndex;tempPrint++)
		printf("key=%d: offset=%ld\n", objIndex[tempPrint].key, objIndex[tempPrint].off);
	
	printf("Availability:\n");
	for(tempPrint=0;tempPrint<countFileAvail;tempPrint++)
		printf("size=%d: offset=%ld\n", objAvail[tempPrint].siz, objAvail[tempPrint].off);
	*/

//	printf("%s\t%d\n",argument[1],intFit);

	//Get into loop of waiting for a command
	while(1)
	{
	//	printf("Hi\n");
		fgets(input,1024,stdin);
	//	printf("%s",input);
		//printf("%s", input);
		if(!strcmp(input,"end\n"))
		{
	//		printf("Ending\n");
			break;
		}
	//	printf("Not ending\n");
		temp = strtok(input," ");
		//printf("%s\n",temp);
		if(!strcmp(temp,"add"))
		{

	//		printf("User wants to add\n");
			temp = strtok(NULL," ");
			Sid = atoi(temp);
			//printf("%d\n",Sid);
			temp = strtok(NULL,"\n");
			//printf("%d",strlen(temp));
			//printf("%s\n",temp);
			
			//temp[strlen(temp)-1]='\0';
			//printf("%d",strlen(temp));
			//printf("%s\n",temp);
			funcAdd(Sid,temp,fn);
		}//end add
		else if(!strcmp(temp,"find"))
		{
			int exists=-1;
	//		printf("User wants to find\n");
			temp = strtok(NULL," ");
			Sid = atoi(temp);
			//printf("%d\n",Sid);

			exists=funcFind(Sid,0,countFileIndex-1);
			if(exists==-1)	printf("No record with SID=%d exists\n",Sid);
			else
			{
	//			printf("Found\n");
				FILE *fileFind;
				int fileFindsize;
				fileFind = fopen(fn,"r+b");
				fseek(fileFind,objIndex[exists].off,SEEK_SET);
				fread(&fileFindsize,sizeof(int),1,fileFind);
	//			printf("size of fileFindsize = %d\n",fileFindsize);
				char *fileFindStream;
				//fileFindsize++;
				fileFindStream = (char *)malloc(fileFindsize+1);
				fread(fileFindStream,fileFindsize,1,fileFind);
				fileFindStream[fileFindsize]='\0';
				printf("%s\n",fileFindStream);
				fclose(fileFind);

			}//end else
		}//end find
		else if(!strcmp(temp,"del"))
		{
	//		printf("User wants to delete\n");
			temp = strtok(NULL," ");
			Sid = atoi(temp);
			funcDel(Sid);
		}//end del
	}//end while	

	printf("Index:\n");
	int temp2;
	for(temp2=0;temp2<countFileIndex;temp2++)
		printf("key=%d: offset=%ld\n", objIndex[temp2].key, objIndex[temp2].off);
	
	printf("Availability:\n");
	for(temp2=0;temp2<countFileAvail;temp2++)
		printf("size=%d: offset=%ld\n", objAvail[temp2].siz, objAvail[temp2].off);

	printf("Number of holes: %d\n",countFileAvail);

	long holeSpace = 0;
	for(temp2=0;temp2<countFileAvail;temp2++)
		holeSpace+=objAvail[temp2].siz;

	printf("Hole space: %ld\n",holeSpace);

	fileIndex = fopen("fileIndex.bin","w+b");
	fwrite(objIndex,sizeof(index_S),countFileIndex,fileIndex);
	fclose(fileIndex);

	fileAvail = fopen("fileAvail.bin","w+b");
	fwrite(objAvail,sizeof(avail_S),countFileAvail,fileAvail);
	fclose(fileAvail);


	return 0;
}