//============================================================================
// Name        : cognition.cpp
// Author      : Alon Levy
// Version     :
// Copyright   : Copyright Bar Ilan University 2010
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cunistd>
#include <cstring>
#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#include "cognition_shared_mem.h"

// Shared memory
CognitionSharedMem* data;
int fd;

using namespace std;

void close()
{
	close(fd);
}

void create_shared_memory()
{
	int fd;
	// created shared memory
	if((fd = shm_open(COGNITION_MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
	{
	    perror("cognition: shm_open");
	    close();
	    return;
	}

	if(ftruncate(fd, sizeof(CognitionSharedMem)) == -1)
	{
	    perror("cognition: ftruncate");
	    close();
	    return;
	}

	// map the shared memory
	if((data = (CognitionSharedMem*)mmap(NULL, sizeof(CognitionSharedMem), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
	    perror("cognition: mmap");
	    close();
	    return;
	}
	// try to be a little careful here, since we will restart cognition while Simulator/bhuman
	// are still running. So zero the cognition_writes first, then the rest;
	data->cognition_writes = 0;
	memset(data, 0, sizeof(CognitionSharedMem));
}

int main(int argc, char** argv) {
	create_shared_memory();
	cout << "inited shared memory\n";
	if (argc > 1) {
		cout << "doing a one shot - should do the jesus move\n";
		data->cognition.startTurn = true;
		data->cognition_writes++;
		usleep(1000000);
		data->cognition_reads = data->cognition_writes; // yeah, we shouldn't. And this is not atomic and bad bad bad.
		return 0;
	}
	// default - endlessly toggle something
	while (true) {
		usleep(1000000);
		// write
		if (data->cognition_reads < data->cognition_writes) {
			std::cout << "waiting for bhuman to catch up to cognition_writes\n";
		} else {
			data->cognition.startTurn = not data->cognition.startTurn;
			data->cognition_writes++;
			cout << "signaled start turn\n";
		}
		// and read
		if (data->bhuman_writes > data->bhuman_reads) {
			data->bhuman_reads = data->bhuman_writes; // TODO: again, locking required if I want to skip writes without missing any. (and for reading without the 'is being updated in the middle' effect)
			cout << "data->bhuman_writes" << data->bhuman_writes << "\n";
			cout << data->bhuman.ball_time_when_last_seen << "\n";
			cout << data->bhuman.ball_position_estimate.x << "," << data->bhuman.ball_position_estimate.y << "\n";
			cout << data->bhuman.ball_velocity_estimate.x << "," << data->bhuman.ball_velocity_estimate.y << "\n";
		}
		std::cout << data->bhuman_writes << "\n";
	}
	return 0;
}
