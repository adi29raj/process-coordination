#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

void GeekArrives();
void NonGeekArrives();
void sArrives();

int BoardBridge();
int GoBridge();

typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Cond;

void *check_malloc(int size){
    void *p = malloc(size);
    if(p==NULL)
    {
        printf("failed to create memory\n");
        exit(-1);
    }
}

Mutex* make_mutex()
{
    Mutex *mutex = check_malloc(sizeof(Mutex));
    int n = pthread_mutex_init(mutex, NULL);
    if(n!=0)
    {
    printf("make_lock failed");
    exit(-1);
    }
    return mutex;
}

void mutex_lock(Mutex *mutex)
{
    int n = pthread_mutex_lock(mutex);
    if(n!=0)
    {
    printf("lock failed");
    exit(-1);
    }
}

void mutex_unlock(Mutex *mutex)
{
    int n = pthread_mutex_unlock(mutex);
    if(n!=0)
    {
    printf("unlock failed");
    exit(-1);
    }
}

Cond *make_cond()
{
    Cond *cond = check_malloc(sizeof(Cond));
    int n = pthread_cond_init(cond,NULL);
    if(n!=0)
    {
    printf("make_cond failed");
    exit(-1);
    }

    return cond;
    
}

void cond_wait(Cond *cond, Mutex *mutex)
{
    int n = pthread_cond_wait(cond,mutex);
    if(n!=0)
    {
    printf("cond_wait failed");
    exit(-1);
    }
}

void cond_signal(Cond *cond){
    int n = pthread_cond_signal(cond);
    if(n!=0)
    {
    printf("cond_signal failed");
    exit(-1);
    }
}

typedef struct{
    int value;
    int wakeups;
    Mutex *mutex;
    Cond * cond;
} sema;

sema * sCreate(int value)
{
    sema * sem = check_malloc(sizeof(sema));
    sem->value=value;
    sem->wakeups=0;
    sem->mutex=make_mutex();
    sem->cond=make_cond();
    return sem;
}

void sWait(sema *sem)
{
    mutex_lock(sem->mutex);
    sem->value--;

    if(sem->value<0)
    {
        do{
            cond_wait(sem->cond,sem->mutex);
        }while(sem->wakeups <1);
        sem->wakeups--;
    }

    mutex_unlock(sem->mutex);
}

void sSignal(sema *sem)
{
    mutex_lock(sem->mutex);
    sem->value++;

    if(sem->value<=0)
    {
        sem->wakeups++;
        cond_signal(sem->cond);

    }
    mutex_unlock(sem->mutex);
}

// typedef struct sema
// {
//     volatile int value;
//     pthread_mutex_t m;
//     pthread_cond_t c;
// }sema;

// sema *sCreate(int val)
// {
    
//     sema *sem = malloc(sizeof(sema));
//     if(!sem)
//     return NULL;

//     sem->value=val;

//     errno = pthread_mutex_init(&sem->m,NULL);
//     if(!errno)
//     free(sem);

//     errno = pthread_cond_init(&sem->c,NULL);
//     if(!errno)
//     pthread_mutex_destroy(&sem->m);

//     return sem;
    
// }

// void sSignal(sema *sem)
// {
//     pthread_mutex_lock(&sem->m);
//     ++sem->value;
//     if(sem->value>0)
//     pthread_cond_signal(&sem->c);

//     pthread_mutex_unlock(&sem->m);
// }

// void sWait(sema *sem)
// {
//     pthread_mutex_lock(&sem->m);
//     while(sem->value<1)
//     pthread_cond_wait(&sem->c,&sem->m);

//     --sem->value;

//     if(sem->value>0)
//     pthread_cond_signal(&sem->c);

//     pthread_mutex_unlock(&sem->m);
// }





int gc,ngc,sc;

int gccr=0,ngccr=0,sccr=0;
int bl=0;
int c=0;

sema *g;
sema *ng;
sema *m;
sema *s;

void GeekArrives()
    {
        sWait(m);
        if(gc>0)
        {
            gccr+=1;
            sWait(g);
            gc = g->value;
            // gc-=1;
            // printf("\n%dgc\n",gc);
        }
        sSignal(m);
    }

    void NonGeekArrives()
    {
        sWait(m);
        if(ngc>0)
        {
            ngccr+=1;
            sWait(ng);
            ngc=ng->value;
            // printf("\n%dngc\n",ngc);
            // ngc-=1;
        }
        sSignal(m);
    }

    void sArrives(){
        sWait(m);
        if(sc>0)
        {
            sccr+=1;
            sWait(s);
            sc=s->value;
            // printf("\n%dsc\n",sc);
            // sc-=1;
        }
        sSignal(m);
    }

int GoBridge()
{
    int s = BoardBridge();
    if(s==1)
    sleep(1);
    return s;
}    

int BoardBridge(){
    int f =0;
    sWait(m);

    if(gccr>=4 && ngccr>=0 && sccr>=0)
    {
        gccr-=4;
        printf("Geeks: 4\t Non Geeks: 0\t Singers : 0\t\n");
        f=1;
    }
    else if(gccr>=0 && ngccr>=4 && sccr>=0)
    {
        ngccr-=4;
        printf("Geeks: 0\t Non Geeks: 4\t Singers : 0\t\n");
        f=1;
    }
    else if(gccr>=2 && ngccr>=2 && sccr>=0)
    {
        gccr -=2;
        ngccr-=2;
        printf("Geeks: 2\t Non Geeks: 2\t Singers : 0\t\n");
        f=1;
    }
    else if(gccr>=2 && ngccr>=1 && sccr>=1)
    {
        gccr -=2;
        ngccr-=1;
        sccr-=1;
        printf("Geeks: 2\t Non Geeks: 1\t Singers : 1\t\n");
        f=1;
    }
    else if(gccr>=1 && ngccr>=2 && sccr>=1)
    {
        gccr -=1;
        ngccr-=2;
        sccr-=1;
        printf("Geeks: 1\t Non Geeks: 2\t Singers : 1\t\n");
        f=1;
    }
    else if(gccr>=3 && ngccr>=0 && sccr>=1)
    {
        gccr -=3;
        sccr-=1;
        printf("Geeks: 3\t Non Geeks: 0\t Singers : 1\t\n");
        f=1;
    }
    else if(gccr>=0 && ngccr>=3 && sccr>=1)
    {
        ngccr-=3;
        sccr-=1;
        printf("Geeks: 0\t Non Geeks: 3\t Singers : 1\t\n");
        f=1;
    }

    sSignal(m);
    return f;
    
}

int main(int argc, char *argv[]){
    if(argc!=4)
    {
        printf("Incorrect number of arguments\n");
        exit(1);
    }

    int gcs = atoi(argv[1]);
    int ngcs = atoi(argv[2]);
    int scs = atoi(argv[3]);

    gc =gcs;
    ngc = ngcs;
    sc = scs;

    //printf("hii\n");
    //printf("\n%d gc\n",gc);
    g = sCreate(gc);
    g->value=gc;
    //printf("\n%d g\n",g->value);
    ng = sCreate(ngc);
    ng->value=ngc;
    //printf("\n%d g\n",g->value);
    m = sCreate(1);
    m->value=1;

    s = sCreate(sc);
    s->value=sc;

    //printf("%d\n",g->value);
    pthread_t g_threads[gc],ng_threads[ngc],s_threads[sc];

    int r,r3;
    srand(time(NULL));

    while(1)
    {
        int r1 = rand()%3;
        //printf("\n%dr1\t",r1);
        if(r1==0)
        {
            if(gc>0)
            {
                r = pthread_create(&(g_threads[c]),NULL,GeekArrives,(void*)r1);
                // printf("gc\n");
                c++;
            }
        }
        else if(r1==1)
        {
            if(ngc>0)
            {
                r = pthread_create(&(ng_threads[c]),NULL,NonGeekArrives,(void*)r1);
                // printf("ngc\n");
                c++;
            }
        }
        else if(r1==2)
        {
            if(sc>0)
            {
                r = pthread_create(&(g_threads[c]),NULL,sArrives,(void*)r1);
                // printf("sc\n");
                c++;
            }
        }

        // printf("%dgc\t%dnc\t%dsc\n",gccr,ngccr,sccr);
		// printf("%d\t%d\t%d\n",gc,ngc,sc);
        if(gc>0 || ngc==0 || sc==0)
            r3 = GoBridge();

        sWait(m);

        if(gc==0 && ngc==0 && sc==0)
        break;

        sSignal(m);

    }
    printf("Persons left :\n");
    printf("Geeks: %d\t Non Geeks: %d\t Singers : %d\t\n",gccr,ngccr,sccr);

return 0;
}

