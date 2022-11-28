#Problem 1 :
We take 4 inputs from the user (no of Hydrogen Atoms, No of oxygen atoms, no of sites and Threshold respectively). We create a seperate thread for each site and picking a random site, checking the following conditions sequentially :
-> number of Hydrogen atoms>=2 and number of oxygen atoms>=1. 
-> no adjacent site should have an ongoing reaction and the current site should not have an ongoing process. 
This is then lead to 'signal' of the 'siteSem' semaphore (semaphore for each site) randomly.
After checking conditions, we output the current stats, then the siteArray is set back to '0' to indicate the site is available for next reaction and put the system to 'sleep' for 3 seconds. Then we 'signal' the'currthreshold' as the process is completed, energy is generated and thus, we decrement the current energy.
This process if repeated till either of the initial listed conditions turns to be false and thus the code comes out of the loop.

##Input format :
./a.out <No of H atoms> <No of O atoms> <No of sites> <Threshold Energy(in Units)>

##Output format :
Site No : x, Hydrogen atoms left : y Oxygen atom left : z , Total Energy: w E Mj

#Problem 2 :
Created 4 semaphore for geeks, non-geeks, singers and mutex.
Using random function to generate values randomly to choose value between 0 to 2 for thread creation for geeks, nongeeks and singers.
As person arrives, semaphore value decreases and value corresponding to people reaching bridge increases.
As the people gets in the safe pattern, they are sent over the bridge and thus the current people waiting to pass the bridge decreases.
Once the value of all the semaphore become 0 it terminates.

##Input format :
./a.out <geeks> <non-geeks> <singers>

##Output format :
Number of geeks,non-geeks and singer who have created the safe pattern and crossed the bridge.
Geek: Non-Geeks: Singers:
Last line is number of people left at end.

#Problem 3:
Two Structures are created i.e. Student, Courses
Structure student has properties rollno, branch, preferred course array, assigned course arra, course allocator
Structure course has properties like courses,knowledge, students enrolled in a course, student limit from the different branches
Input is given while executing 
Format for executing:"./a.out <#no of students> <#no of courses>".
Each student is allocated its branch randomly by hash.
For each student a list of preffered courses is created.
Semaphore are implemented and not used from semaphore.h library
Threads for all students works sequentely and are allocated the courses as per the condition
The result is stored is a text file "allocation.txt" which contains the information of the the courses and the no of students who have been allotted that course
For the students who could not be alloted course are diplayed in the command prompt/terminal window.
To Execute the file use command:
	gcc problem3.c
	./a.out <#no of students> <#no of courses>
