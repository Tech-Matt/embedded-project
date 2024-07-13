# Todo

## Current tasks
- [ ] Make also semaphore init atomic (?)
- [ ] Test if semaphores are actually running
- [ ] Is there a different way to make semaphores atomic (swap? test and set?)
- [ ] Pay attention to interrupts, they could make your system non deterministic. Consider the option of disabling them
- [ ] Investigate what other things you could implement as a real time os
- [ ] think about what possible tasks you could implement and which system to simulate

## TASKS
- [ ] How is the OS made?
- [ ] study the scheduling of the tasks, assure there are no possible deadlocks and decide scheduling algorithm
- [ ] Divide tasks in hard real time, soft real time, anytime
- [ ] Decide which action to perform in case of time infringments
- [ ] Search / calculate tasks funcion calls required time to execute 
- [ ] Think about a cool name with "---RTOS" (LOL)
- [ ] Read about the clock Subsystem and debug clock init


## Done
- [x] Make the code a bit more readable moving functions, creating .h and etc.
- [x] Implement lcd graphics
- [x] Make the semaphores atomic



