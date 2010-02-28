#ifndef __COGNITION_SHARED_MEM_H__
#define __COGNITION_SHARED_MEM_H__

#include <pthread.h>

#define COGNITION_MEM_NAME "burst_cognition"

/**
 * SharedMem is a shared memory area that has the logic to handle it's own mutex for locking. The users
 * must call lock and unlock at the right places (before and after accessing, either read or write).
 * The memory it self is mapped elsewhere.
 * The lock MUST only be initialized at one place, preferably where the memory map is created.
 * Behavior is undefined if pthread_mutex_init is called on already initialized memory (see man page, manpages-posix-dev).
 */
struct LockedMem {
	pthread_mutex_t m_mutex;
	/**
	 * do_init - pass true to initialize the mutex. Must be done
	 * in one process only. Currently we have no way 
	 */
	void init() {
		pthread_mutex_init(&m_mutex, NULL);
	}

	void lock() {
		pthread_mutex_lock(&m_mutex);
	}

	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}

	unsigned int writes; // incremented by 1 by Cognition process after write
	unsigned int reads;  // incremented by 1 by BHuman process <after/before> read
};

/*
 * Helper for LockedMem. Meant to be used like this:
 * {
 *    Locked(data->cognition); // data->cognition.lock() called by Locked constructor
 *    data->cognition.bla += 10;
 * } // destructor of Locked called here, which does the data->cognition.unlock()
 */
struct Locked {
private:
	LockedMem& m_mem;
public:
	Locked(LockedMem& mem) : m_mem(mem){
		m_mem.lock();
	}
	~Locked() {
		m_mem.unlock();
	}
};

struct CognitionWrittenLockedMem : public LockedMem {
	bool startTurn;
};

struct Vec2 {
	double x;
	double y;
};

struct BHumanWrittenLockedMem : public LockedMem {
	// TODO: we will need some way to make this easier to do. Will we? we can reuse the
	// Blackboard basically, and just have a copy of it shared.. But it has many pointers..
	Vec2 ball_position_estimate;
	Vec2 ball_velocity_estimate;
	unsigned ball_time_when_last_seen; // and now we need to synchronize the time too.. use some minimal delta.. argh!
};

// This is the memory layout at the mapped memory file

struct CognitionSharedMem {

	volatile bool inited; // must be a varialbe that when set to 0 will mean "not initialized"

	void init(bool in_cognition_process)
	{
		if (in_cognition_process) {
			cognition.init();
			bhuman.init();
			inited = true; // this is not protected by a mutex.
		} else {
			// busy loop on inited
			while (!inited) {
				usleep(100000); // 0.1 second
			}
		}
	}
	/**
	 * Call before exiting.
	 */
	void destroy() {
		inited = false;
		// we do not delete the memory at all;
	}
	CognitionWrittenLockedMem cognition;
	BHumanWrittenLockedMem bhuman;
};

#endif // __COGNITION_SHARED_MEM_H__
