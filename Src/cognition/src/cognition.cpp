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
	data->cognition.writes = 0;
	memset(data, 0, sizeof(CognitionSharedMem));
}

int main(int argc, char** argv) {
	create_shared_memory();
	cout << "inited shared memory\n";
	if (argc > 1) {
		cout << "doing a one shot - should do the jesus move\n";
		{
			Locked(data->cognition);
			data->cognition.startTurn = true;
			data->cognition.writes++;
		}
		usleep(1000000);
		{
			Locked(data->cognition);
			data->cognition.reads = data->cognition.writes;
		}
		return 0;
	}
	// default - endlessly toggle something
	while (true) {
		usleep(1000000);
		// write
		if (data->cognition.reads < data->cognition.writes) {
			std::cout << "waiting for bhuman to catch up to cognition_writes\n";
		} else {
			{
				Locked(data->cognition);
				data->cognition.startTurn = not data->cognition.startTurn;
				data->cognition.writes++;
			}
			cout << "signaled start turn\n";
		}
		// and read
		if (data->bhuman.writes > data->bhuman.reads) {
			double ball_x, ball_y, ball_vx, ball_vy;
			int bhuman_writes;
			int ball_time_when_last_seen;
			{
				Locked(data->bhuman); // NOTE: as long as you only update writes *after* you are done, you can leave the comparison unlocked above.
				data->bhuman.reads = data->bhuman.writes;
				bhuman_writes = data->bhuman.writes;
				ball_x = data->bhuman.ball_position_estimate.x;
				ball_y = data->bhuman.ball_position_estimate.y;
				ball_vx = data->bhuman.ball_velocity_estimate.x;
				ball_vy = data->bhuman.ball_velocity_estimate.y;
				ball_time_when_last_seen = data->bhuman.ball_time_when_last_seen;
			}
			cout << "data->bhuman_writes" << bhuman_writes << "\n";
			cout << ball_time_when_last_seen << "\n";
			cout << ball_x << "," << ball_y << "\n";
			cout << ball_vx << "," << ball_vy << "\n";
		}
		std::cout << data->bhuman.writes << "\n"; // unlocked
	}
	return 0;
}
