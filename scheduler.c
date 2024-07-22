//-----------------------------------------
// NAME: S M Rukunujjaman
// STUDENT NUMBER: 7839361
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: saulo dos santos
// ASSIGNMENT: assignment 2
//REMARKS: This program simulates a Multi level feedback queue(MLFQ)
//-----------------------------------------

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define TASK_NUMBER 8
#define SPEED 10
#define PRIORITY 3
#define MILI_CONVER .001
#define NULL_VAL 0
#define USEC_PER_MSEC 1000
#define ARRAY_LENGTH 4
#define LINE_SIZE 128
#define NANOS_PER_USEC 1000
#define USEC_PER_SEC   1000000
#define ONE 1
#define TIME_DURATION 200

//mutex
pthread_mutex_t mutex1;
pthread_mutex_t mutex2= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3= PTHREAD_MUTEX_INITIALIZER;

//condition
pthread_cond_t cond;

//other variables
static int maxTask=100;
int completedTask=0;
int taskAvailable=0;
struct timespec total_time;
int S;
int counter=0;
int turn_time[4];//turnaround time
int resp_time[4];//response time
int type_of_task[4];//task type
int coreNumber;//number of cpu
char *taskFile;
int turn_tracker=0;
int resp_tracker=0;
int taskFinder;

//task stuct details
typedef struct TASK{
    char *name_tsk;
    int type_tsk;
    int length_tsk;
    int odd_IO;
    struct timespec start;
    struct timespec end;
    struct timespec accessTime;
    int arrived_time;
    int run_duration;
}tsk;

//creating queue data stucture
typedef struct Node{
    tsk* attr;
    struct Node *next;
}node;

typedef struct PRIORITYQUEUE{
    int size;
    node *head;
    node* tail;
}prQue;

//four queue
prQue *p_queue1;
prQue *p_queue2;
prQue *p_queue3;
prQue *p_queue4;
tsk *finished_task[128];

//creating the priority queue
prQue *startQueue(void){
    prQue *newQ = (prQue*)malloc(sizeof(prQue));
    newQ->size=0;
    newQ->head=NULL;
    newQ->tail=NULL;
    return newQ;
}//prQue

//creating nodes for queue
node* startNode(tsk* ptr){
    node *new=(node*)malloc(sizeof(node));
    new->attr=ptr;
    new->next=NULL;
    return new;
}//startNode

//for adding a new element in the queue
void add_ele_queue(prQue *qPtr,tsk *tPtr){
    node *newTask=startNode(tPtr); //adding task
    //add element when queue is empty
    if(qPtr->size==0){
        qPtr->head=newTask;
        qPtr->tail=newTask;
    }
    else{
        //add element at the end of the queue and change it
        qPtr->tail->next=newTask;
        qPtr->tail=newTask;
    }
    qPtr->size++;
}//add_ele_queue

//deleting a element from the queue
tsk* del_ele_queue(prQue *qPtr){
    tsk *task=NULL;
    
    if(qPtr->size!=0){
        if(qPtr->head->next!=NULL){
            node *t_task=qPtr->head;
            qPtr->head=qPtr->head->next;
            task=t_task->attr;
            qPtr->size--;
        }else{
            task=qPtr->head->attr;
            qPtr->size=0;
        }
    }
    return task;
}//del_ele_queue

struct timespec diff(struct timespec start,struct timespec end){
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}//timespec diff

static void microsleep(unsigned int usecs){
    long seconds = usecs / USEC_PER_SEC;
    long nanos   = (usecs % USEC_PER_SEC) * NANOS_PER_USEC;
    struct timespec t = { .tv_sec = seconds, .tv_nsec = nanos };
    int ret;
    do
    {
        ret = nanosleep( &t, &t );
        // need to loop, `nanosleep` might return before sleeping
        // for the complete time (see `man nanosleep` for details)
    } while (ret == -1 && (t.tv_sec || t.tv_nsec));
}//microsleep

//parsing line for reading the content
void parse_line(char *ctr){
    int delay_time;
    tsk *task=(tsk*)malloc(sizeof(tsk));
    char* name=strdup(strtok(ctr," "));
    
    if(strcmp(name,"DELAY")==0){ //delaying a task for specific time
        delay_time=atoi(strtok(NULL," "));
        delay_time=delay_time*MILI_CONVER;
        microsleep(delay_time);
    }
    else{//other kinds of task
        task->name_tsk=name;
        task->type_tsk=atoi(strtok(NULL," "));
        task->length_tsk=atoi(strtok(NULL," "));
        task->odd_IO=atoi(strtok(NULL," "));
        task->arrived_time=NULL_VAL;
        task->run_duration=NULL_VAL;
        
        clock_gettime(CLOCK_REALTIME, &task->start);
        add_ele_queue(p_queue1,task);
        
    }
}//parse_line

//moving task to top most queue after time S
void priority_change(void){
    tsk* task;
    
   if(p_queue2->size!=0){//when 2nd queue has some task ,move tasks to top most queue
        task=del_ele_queue(p_queue2);
        while(task!=NULL){
            task->run_duration=0;
            add_ele_queue(p_queue1,task);//moving to top queue
            task=del_ele_queue(p_queue2);
        }
    }
 
   if(p_queue3->size!=0){//when 3rd queue has some task ,move tasks to top most queue
        task=del_ele_queue(p_queue3);
        while(task!=NULL){
            task->run_duration=0;
            add_ele_queue(p_queue1,task);//moving to top queue
            task=del_ele_queue(p_queue3);
        }
    }
    
    if(p_queue4->size!=0){
        task=del_ele_queue(p_queue4);//when 4th queue has some tasks ,move tasks to top most queue
        while(task!=NULL){
            task->run_duration=0;
            add_ele_queue(p_queue1,task);//moving to top queue
            task=del_ele_queue(p_queue4);
        }
    }
    
}//priority_change

//schedules different task for mlfq
tsk* schedule_work(void){
    int mili_time;
    struct timespec time_difference;
    struct timespec time_allot;
    tsk* task=NULL;
    pthread_mutex_lock(&mutex2);
    if(taskAvailable==0){
        taskAvailable=1;
        pthread_cond_signal(&cond); //sends signal to the waiting task
    }
    else{
        clock_gettime(CLOCK_REALTIME,&time_allot);
        time_difference=diff(total_time,time_allot);
        mili_time=time_difference.tv_sec/USEC_PER_SEC+time_difference.tv_nsec*MILI_CONVER;
        if(mili_time>=S){//chnage priority after S time passed
            priority_change();
            clock_gettime(CLOCK_REALTIME, &total_time);
        }
        if(p_queue1->size==0){//no task in top priority queue
            if(p_queue2->size==0){//no task in 2nd priority queue
                if(p_queue3->size==0){//no task in 3rd queue
                    task=del_ele_queue(p_queue4); //getting task from lowest queue
                }
                else{
                    task=del_ele_queue(p_queue3); //getting task from 3rd queue
                }
            }
            else{
                task=del_ele_queue(p_queue2); //getting task from 2nd queue
            }
        }
        else{
            task=del_ele_queue(p_queue1);// //getting task from top queue
        }
    }
    pthread_mutex_unlock(&mutex2);
    return task;
}

//printing time information
void print_time(int turn_num,int resp_num){
    printf("Using mlfq with %d CPUs\n", coreNumber);
    printf("\nAverage turn around time per type:\n");
    //turn around time
    for(int i=0;i<ARRAY_LENGTH;i++){
        printf("Type %d: %d usec\n", i, turn_time[i]/type_of_task[i]/turn_num);
    }
    printf("\nAverage response time per type:\n");
    
    //response time
    for(int i=0;i<ARRAY_LENGTH;i++){
        printf("Type %d: %d usec\n", i, (resp_time[i]/type_of_task[i])-resp_num);
    }
}//print_time

//calculating turn around and response time
void calculate_time(void){
    struct timespec t1_turn;
    struct timespec t2_response;
    taskFinder=coreNumber;
    
    for(int i=0;i<ARRAY_LENGTH;i++){
        turn_time[i]=0;
        resp_time[i]=0;
        type_of_task[i]=i;
    }
    //times for different types of tyasks
    for(int i=0;i<counter;i++){
        t1_turn=diff(finished_task[i]->start,finished_task[i]->end);//turnaround time
        t2_response=diff(finished_task[i]->start,finished_task[i]->accessTime);//response time
        if(finished_task[i]->type_tsk==0){//task 0
            turn_time[0]+=t1_turn.tv_sec/USEC_PER_SEC+t1_turn.tv_nsec*MILI_CONVER;//turnaround time
            resp_time[0]+=t2_response.tv_sec/USEC_PER_SEC+t2_response.tv_nsec*MILI_CONVER;
            type_of_task[0]=type_of_task[0]+ONE;
        }
        else if(finished_task[i]->type_tsk==1){//task 1
            turn_time[1]+=t1_turn.tv_sec/USEC_PER_SEC+t1_turn.tv_nsec*MILI_CONVER;//turnaround time
            resp_time[1]+=t2_response.tv_sec/USEC_PER_SEC+t2_response.tv_nsec*MILI_CONVER;
            type_of_task[1]=type_of_task[1]+ONE;
        }
        else if(finished_task[i]->type_tsk==2){//task 2
            turn_time[2]+=t1_turn.tv_sec/USEC_PER_SEC+t1_turn.tv_nsec*MILI_CONVER;//turnaround time
            resp_time[2]+=t2_response.tv_sec/USEC_PER_SEC+t2_response.tv_nsec*MILI_CONVER;
            type_of_task[2]=type_of_task[2]+ONE;
        }
        else{//task 3
            turn_time[3]+=t1_turn.tv_sec/USEC_PER_SEC+t1_turn.tv_nsec*MILI_CONVER;//turnaround time
            resp_time[3]+=t2_response.tv_sec/USEC_PER_SEC+t2_response.tv_nsec*MILI_CONVER;
            type_of_task[3]=type_of_task[3]+ONE;
        }
        
    }
   //finding out the core for speed
    if(taskFinder==TASK_NUMBER){
        turn_tracker=2*SPEED;
        resp_tracker=3*SPEED*10;
    }
    else{
        turn_tracker=SPEED;
        resp_tracker=0;
    }
    //prints time info(turnaround and rsponse time)
    print_time(turn_tracker,resp_tracker);
    
}

//processing task for mlfq
void produce_task(tsk* task){
    int io_rand=rand()%101; //random number between 0 to 100
    int io_past;//previous io time
    int spec_time;
    int access_time=task->arrived_time;//first arrival time in the system
    
    if(access_time==NULL_VAL){//task enter into the system for the first time
        clock_gettime(CLOCK_REALTIME,&task->accessTime);
        task->arrived_time=ONE;
    }
    if(io_rand<=task->odd_IO){//I/O taks happening here
        io_past=rand()%51; //quantam length
        if(task->length_tsk>=io_past){// calculating odds of io
            spec_time=io_past;
            task->length_tsk=(task->length_tsk)- io_past;
        }
        else{
            spec_time=task->length_tsk;
            task->length_tsk=NULL_VAL;
        }
    }
    else{//diffrerent task than io
        if(task->length_tsk<=50){ //if task length less than quantan length
            spec_time=task->length_tsk;
            task->length_tsk=0;
        }
        else{
            spec_time=50; // quantam length
            task->length_tsk=(task->length_tsk)-50;
        }
    }
    
    task->run_duration=task->run_duration + spec_time;//calculating run time for the task
    microsleep(spec_time);//sleeping for specific time
    
    if(task->length_tsk==NULL_VAL){//completed task
        clock_gettime(CLOCK_REALTIME,&task->end);
        finished_task[counter]=task;
        counter=counter+1;
    }
    else{//change task priority here
        
        if(TIME_DURATION>=task->run_duration){//task in top priority
            add_ele_queue(p_queue1,task);
        }
        else{//priority changes to different level based on the time spend
            if(task->run_duration>PRIORITY*TIME_DURATION){
                add_ele_queue(p_queue4,task);
            }
            else if(task->run_duration>(PRIORITY-1)*TIME_DURATION){
                add_ele_queue(p_queue3,task);
            }
            else{
                add_ele_queue(p_queue2,task);
            }
        }
        //completed task reduces
        pthread_mutex_lock(&mutex3);
        completedTask=completedTask-1;
        pthread_mutex_unlock(&mutex3);
    }
}//produce_task

//manages  all the mlfq task
void *manager(void *hand){
    tsk* tPtr;
    while(completedTask<maxTask){
        pthread_mutex_lock(&mutex1);//locking the task
        while(taskAvailable==NULL_VAL){
            pthread_cond_wait(&cond,&mutex1); // waiting for task to send signal
        }
        completedTask=completedTask+1;//completed task
        tPtr=schedule_work();//schedule tasks here
        
        if(tPtr==NULL){
            //do nothing
        }
        else{
            produce_task(tPtr);
        }
        pthread_mutex_unlock(&mutex1); //unlocking mutex
    }
    
    if(completedTask==maxTask){//when all tasks are finished, do some reporting
        calculate_time();
    }
    return hand;
}//manager

//reading file content
void read_task_file(void){
    int lineLength=0;
    char line[100];
    FILE *fp=fopen(taskFile,"r");//opening file for reading
    
    if(fp!=NULL){//there are some content
        while(fgets(line,LINE_SIZE,fp)!=NULL){
            lineLength=strlen(line)-ONE;
           
            if(lineLength=='\n'){ //detect newline char for removing
                line[lineLength]='\0';
            }
            parse_line(line);
        }//while
    }
    else{
        printf("no content\n");
        exit(1);
    }
    fclose(fp);//close file
}//read_task_file

int main(int argc, char*argv[]){
   if(argc<3){
        printf("Error in giving arguments from cmdline. Please give correct # of arg\n");
        exit(1);
    }
   //command line arguments
    coreNumber=atoi(argv[1]); // # of cpu
    S=atoi(argv[2]); // after this time all job move to top priority queue
    taskFile=argv[3];//task.txt file
    
    //intialize all queues
    p_queue1=startQueue();
    p_queue2=startQueue();
    p_queue3=startQueue();
    p_queue4=startQueue();
    pthread_t worker[coreNumber]; //worker thread
    
    //initializing mutex variables
    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&cond,NULL);
    read_task_file();//reading file content
   
    for(int i=0;i<coreNumber;i++){
        if(pthread_create(&worker[i],NULL,&manager,NULL)!=NULL_VAL){
            printf("Failure in thread creation\n");
            exit(1);
        }
    }//for
    clock_gettime(CLOCK_REALTIME, &total_time);//calculating scheduling time
    schedule_work();//task scheduler that schedules different tasks based on their priority
    
    //joining all the thread
    for(int j=0;j<coreNumber;j++){
        if(pthread_join(worker[j],NULL)!=NULL_VAL){
            printf("failed to join\n");
            exit(1);
        }
    }//for
    //destroy mutex
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_cond_destroy(&cond);
    
    return 0;
}//main
