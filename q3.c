#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
void *allocateMemory(int size){
    void *p = malloc(size);
    if(p==NULL){
        printf("Failed to allocate memory\n");
        exit(-1);
    }
    
}

typedef pthread_mutex_t Mutex;

Mutex *createMutex(){
    Mutex *mutex  = allocateMemory(sizeof(Mutex));
    int n = pthread_mutex_init(mutex, NULL);
    if (n != 0){
        printf("Failed to init mutex\n");
        exit(-1);
    }
    return mutex;
}

void lockMutex (Mutex *mutex){
    int n = pthread_mutex_lock (mutex);
    if (n != 0){
        printf("Failed to lock mutex\n");
        exit(-1);
    }
}

void unlockMutex (Mutex *mutex)
{
  int n = pthread_mutex_unlock (mutex);
    if (n != 0){
        printf("Failed to unlock mutex\n");
        exit(-1);
    }
}

typedef pthread_cond_t Cond;

Cond *createCondition(){
    Cond *cond = allocateMemory (sizeof(Cond)); 
    int n = pthread_cond_init (cond, NULL);
    if (n != 0){
        printf("Failed to create condition\n");
        exit(-1);
    }
    return cond;
}

void wait(Cond *cond, Mutex *mutex){
    int n = pthread_cond_wait (cond, mutex);
    if (n != 0){
        printf("Failed to wait on condition\n");
        exit(-1);
    }
}

void signal (Cond *cond){
    int n = pthread_cond_signal (cond);
    if (n != 0){
        printf("Failed to signal on condition\n");
        exit(-1);
    }
}


typedef struct {
    int value;
    int wakeups;
    Cond* cond;
    Mutex* mutex;
} Semaphore;

Semaphore *createSemaphore(int value){
    Semaphore *semaphore = allocateMemory(sizeof(Semaphore));
    semaphore->value = value;
    semaphore->wakeups = 0;
    semaphore->mutex = createMutex();
    semaphore->cond = createCondition();
    return semaphore;
}

void semWait(Semaphore *semaphore){
    lockMutex(semaphore->mutex);
    semaphore->value--;
    if (semaphore->value < 0) {
    do {
        wait(semaphore->cond, semaphore->mutex);
    } while (semaphore->wakeups < 1);
    semaphore->wakeups--;
    }
    unlockMutex(semaphore->mutex);
}

void semSignal(Semaphore *semaphore){
    lockMutex(semaphore->mutex);
    semaphore->value++;

    if (semaphore->value <= 0) {
    semaphore->wakeups++;
    signal(semaphore->cond);
    }
    unlockMutex(semaphore->mutex);
}
Semaphore* Failed;
Semaphore* mtx;
void *student_thread(void *arg);

struct student{
	int roll_no;
	int branch;
	int preffered_course[8];
	int assigned_course[4];
	int course_allocated;
};

struct course{
	int cid;
	int kSpectrum;
	int stuEnrolled;	
	int stu_branch[4];
};

struct course *c;
struct student *s;
void struct_allocation(int std_count,int crs_count)
{
    c = (struct course *) malloc(sizeof(struct course) * crs_count);
	s = (struct student *) malloc(sizeof(struct student) * std_count);
    
}
void course_intilization(int std_count,int crs_count)
{
    //knowledge 0:commerce 1:humanities 2:management 3:arts
    for(int i=0;i<crs_count;i++)
    {
        c[i].cid=i;
        c[i].kSpectrum=(i%4);
        c[i].stuEnrolled=0;
        c[i].stu_branch[0]=12;    //m comm
        c[i].stu_branch[1]=12;        //phd
        c[i].stu_branch[2]=24;      //b comm  
        c[i].stu_branch[3]=12;    //arts

    }
}
void *student_thread(void *arg1){
	int studentid = (int *)arg1;
	
    semWait(mtx);
	// printf("records Active :%d\n",studentid);
	int i;
	int m = 4;
	for(i = 0 ;i < 8 ; i++){
		int s_course_id = s[studentid].preffered_course[i];	
		int s_branch = s[studentid].branch;
		int stu_branch_course = c[s_course_id].stu_branch[s_branch];
        //  printf("temp2 %d %d %d\n",s_course_id,s_branch,stu_branch_course);
        //  printf("%d %d\n",s[studentid].course_assigned,stu_branch_course);
		// int stu_branch_course = c[s_course_id].stu_branch[s_branch];
		// printf(" %d %d %d \n",s_course_id,s_branch,c[s_course_id].stu_branch[s_branch]);
		if(stu_branch_course != 0){
			if(s[studentid].course_allocated > 0){
				int x = m - s[studentid].course_allocated;
				s[studentid].assigned_course[x] = s_course_id;
				s[studentid].course_allocated--;
				c[s_course_id].stu_branch[s_branch]--;
				c[s_course_id].stuEnrolled++;
			}	
		}
	}	
    semSignal(mtx);
}


int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        printf("Invalid Execution Format\n");
        exit(1);
    }
    int i,k,j,p,amount;
    int tstudents = atoi(argv[1]);	
	int tcourses = atoi(argv[2]);
	int carray[tcourses];
    
	for(i = 0 ; i < tcourses ; i++){
		carray[i] = 0;
	}
    struct_allocation(tstudents,tcourses);
    course_intilization(tstudents,tcourses);
    mtx = createSemaphore(1);	
    pthread_t s_thread[tstudents];
    FILE *file=NULL;
    file = fopen("allocation.txt", "w");
	if(file == NULL){
		perror("Unable to create file");
		exit(1);
	}
    for(j = 0 ; j < tstudents ; j++)
    {
		s[j].roll_no = j;
		int hash1 = rand() % 4;
		s[j].branch = hash1;	
		k = 0;
        // printf("new check 3: %d\n",j);
		while(1){
			int hash2 = rand() % tcourses;
			if(carray[hash2] == 0){	
				if(k < 8 ){
					s[j].preffered_course[k++] = hash2; 
				}else{
                    // carray[pi] = 1;
					break;
				}	
			}   
		}
        for(i = 0 ; i < tcourses ; i++){
                	carray[i] = 0;
   		}		

		for(k = 0 ; k < 4 ; k++){
			s[j].assigned_course[k] = 0;
		}
		s[j].course_allocated = 4;
        // printf("new check 7\n");
	}	
    for(p = 0 ; p < tstudents ; p++ ){
		amount = pthread_create(&(s_thread[p]), NULL, student_thread , (void *) p);
	}	
	//printf("Waiting for the thread to finish");
	for(p = 0 ; p < tstudents ; p++ )
    {
        pthread_join(s_thread[p], NULL);
    }
     printf("Students with unassigned Courses=>\n");
	for(i = 0 ; i < tstudents ;i++){
		if(s[i].course_allocated > 0){
		printf("student of student id:%d \n", i+1);
		}
	}	
    fprintf(file, " Course Allocation Table \n");
    fprintf(file,"Course No \t \t No of Students\n");
	for(i = 0 ; i < tcourses ; i++){
        fprintf(file,"%d \t \t \t\t\t %d\n",c[i].cid+1,c[i].stuEnrolled);
		}
		
	fclose(file);
    exit(0);
// return 0;
}