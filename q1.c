#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <unistd.h> // for sleep()

void *allocateMemory(int size){
    void *p = malloc(size);
    if(p==NULL){
        printf("Faled to allocate memory\n");
        exit(-1);
    }
}

typedef pthread_mutex_t Mutex;

Mutex *createMutex(){
    Mutex *mutex  = allocateMemory(sizeof(Mutex));
    int n = pthread_mutex_init(mutex, NULL);
    if (n != 0){
        printf("Faled to init mutex\n");
        exit(-1);
    }
    return mutex;
}

void lockMutex (Mutex *mutex){
    int n = pthread_mutex_lock (mutex);
    if (n != 0){
        printf("Faled to lock mutex\n");
        exit(-1);
    }
}

void unlockMutex (Mutex *mutex)
{
  int n = pthread_mutex_unlock (mutex);
    if (n != 0){
        printf("Faled to unlock mutex\n");
        exit(-1);
    }
}

typedef pthread_cond_t Cond;

Cond *createCondition(){
    Cond *cond = allocateMemory (sizeof(Cond)); 
    int n = pthread_cond_init (cond, NULL);
    if (n != 0){
        printf("Faled to create condition\n");
        exit(-1);
    }
    return cond;
}

void wait(Cond *cond, Mutex *mutex){
    int n = pthread_cond_wait (cond, mutex);
    if (n != 0){
        printf("Faled to wait on condition\n");
        exit(-1);
    }
}

void signal (Cond *cond){
    int n = pthread_cond_signal (cond);
    if (n != 0){
        printf("Faled to signal on condition\n");
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

int *siteArray;
int curHydrogen;
int curOxygen;

Semaphore** sitesSem;
Semaphore* hydrogenSem;
Semaphore* oxygenSem;
Semaphore* thresholdSem;
Semaphore* totalEnergySem;
Semaphore* mtx;


void* handleSiteThread(void* id){
    int siteId = (int*) id;
    // printf("Thread Entry%d\n",siteId);
    // if(sitesSem[siteId]->value==0){
    //     return ;  
    // }
    semWait(sitesSem[siteId]);
    while(curHydrogen >=2 && curOxygen >=1){
        // printf("%d\t%d\t%d\n",siteId,curHydrogen,curOxygen);
        // printf("A%d\t%d\t%d\n",siteId, hydrogenSem->value,oxygenSem->value);
        if(curHydrogen >=2 && curOxygen >=1){
            // printf("Thread%d\n",siteId);

            semWait(hydrogenSem);
            semWait(hydrogenSem);
            semWait(oxygenSem);
            siteArray[siteId] = 1;
            semSignal(totalEnergySem);
            curHydrogen = hydrogenSem->value;
            curOxygen = oxygenSem->value;
            // curHydrogen -=2;
            // curOxygen -=1;
            printf("Site No %d , Hydrogen atoms left : %d Oxygen atom left : %d , Total Energy: %d E Mj\n",siteId, hydrogenSem->value, oxygenSem->value,totalEnergySem->value);
            siteArray[siteId] = 0;
            sleep(3);
        }
        else{
            break;
        }

    }
    // printf("Thread%d\n",siteId);
}

int main(int argc, char *argv[]){
    if(argc != 5){
		printf("Enter outputs like <No of H atoms> <No of O atoms> <No of sites> <Threshold Energy(in Units)>\n");
		exit(1);
	}
    int noOfHydrogen = atoi(argv[1]);
	int noOfOxygen = atoi(argv[2]);
	int noOfSite = atoi(argv[3]);
	int threshold = atoi(argv[4]);

    curHydrogen = noOfHydrogen;
    curOxygen = noOfHydrogen;

    pthread_t sites[noOfSite];
    siteArray = (int*)malloc(sizeof(int) * noOfSite);
    for(int i = 0 ; i < noOfSite ;i++){
		siteArray[i] = 0;
	}

    sitesSem = (Semaphore**)malloc(sizeof(Semaphore)* noOfSite );
    hydrogenSem = createSemaphore(noOfHydrogen);
    oxygenSem = createSemaphore(noOfOxygen);
    thresholdSem = createSemaphore(threshold);
    totalEnergySem = createSemaphore(0);
    mtx = createSemaphore(1);
    for(int i = 0 ; i < noOfSite; i++){
		sitesSem[i] = createSemaphore(0);
	}

    for(int k = 0 ; k < noOfSite ;k++){
        pthread_create(&(sites[k]), NULL, handleSiteThread, (void *) k);
	}

    int random;
    while(curHydrogen >=2 && curOxygen >=1){
        random = rand()% noOfSite;
        // printf("%d\t",random);
        if(random == 0){
            if( siteArray[random + 1] == 0 && siteArray[random] == 0){
				semSignal(sitesSem[random]);
			}
        }
        else if(random == noOfSite-1){
            if( siteArray[random - 1] == 0 && siteArray[random] == 0){
				semSignal(sitesSem[random]);
			}
        }
        else{
            if(siteArray[random - 1] == 0 && siteArray[random + 1] == 0 && siteArray[random] == 0){
				semSignal(sitesSem[random]);
			}
        }
        // printf("%d\t%d\t%d\n",random,curHydrogen,curOxygen);
        // printf("%d\t%d\t%d\n",random,curHydrogen,curOxygen);l
    }
    // printf("%d\t%d\t%d\n",random,curHydrogen,curOxygen);


    for(int k = 0 ; k < noOfSite ;k++){
        // printf("K: %d\t%d\n",k,sitesSem[k]->value);
        if(sitesSem[k]->value<0){
            // printf("W: %d\n",k);
            semSignal(sitesSem[k]);
        }
		int c = pthread_join(sites[k], NULL);
        // printf("C: %d\n",c);
	}
	printf("No of hydrogen left : %d, oxygen : %d\n",curHydrogen, curOxygen);
    return 0;
}

