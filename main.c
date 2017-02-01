/*
   Author:  Travis Hoover
   Date:  January 29, 2017
   Purpose:  To calculate the area under the curve
              sqrt(4-x*x)
             from 0 to 2
             Using the trapezoidal rule
   Remember to include -lm in the compile line.
*/

/* need the thread library for threads and the math
  library for the sin function */
#include <stdio.h>
#include <pthread.h>
#include <math.h>


float width;                      /* the width of a segment */
float rsum = 0.0;                 /* the trapezoidal result */
pthread_mutex_t mutex1;           /* mutual exclusion variable for simpsons */
float heightSum = 0;

/*Calculate one segment of the area using the trapezoidal rule*/
void *trapezoidal(void *mine) {
    float partial_sum = 0.0;      /* area under my segment*/
    int me = *((int *)mine);      /* which segment is mine */
    float heightRight;		      /* height of right side of trapezoid */
    float heightLeft;             /* height of the left side of trapezoid */
    float mid;		              /* midpoint of my segment */

    mid = me * width;

    /* calculate the right-side height of the curve at each x value */
    heightRight = sqrt(4.0 - (mid + width) * (mid + width));

    /* calculate the left-side height of the curve at each x value */
    heightLeft = sqrt(4.0 - mid * mid);

    /* calculate the area in my segment */
    partial_sum = (heightRight + heightLeft) * width / 2;
    heightSum += (heightLeft + heightRight);

    /* increment the global area */
    pthread_mutex_lock(&mutex1);
    rsum += partial_sum;
    pthread_mutex_unlock(&mutex1);

    return 0;
}

int main()
{
    int no_processes;            /* number of threads/segments */
    int i;                       /* loop variable */
    int is[50];                  /* array for designating segment numbers */
    pthread_t threadr[50];       /* thread id array */

    /* initialize mutual exclusion variables */
    pthread_mutex_init(&mutex1,NULL);

    /* determine the number of partitions...which is the number of threads */
    printf("input the number of partitions\n");
    scanf("%d",&no_processes);

    /* the width is the same for each segment so make it global */
    width = 2.0/no_processes;

    /* create threads */
    for(i=0;i<no_processes;i++) {
        is[i]=i;
        if(pthread_create(&threadr[i],NULL,trapezoidal,(void *)&is[i]) != 0)
            perror("Pthread_create fails");
    }

    /* wait for all threads */
    for(i=0;i<no_processes; i++) {
        if (pthread_join(threadr[i], NULL) != 0)
            perror("Pthread_join fails");
    }

    /* print the area for each */
    printf("The area under the curve is %f\n",rsum);

    return(0);
}
