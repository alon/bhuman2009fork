#ifndef __COGNITION_SHARED_MEM_H__
#define __COGNITION_SHARED_MEM_H__

#define COGNITION_MEM_NAME "burst_cognition"

struct CognitionWrittenSharedMem {
	bool startTurn;
};

struct Vec2 {
	double x;
	double y;
};

struct BHumanWrittenSharedMem {
	// TODO: we will need some way to make this easier to do. Will we? we can reuse the
	// Blackboard basically, and just have a copy of it shared.. But it has many pointers..
	Vec2 ball_position_estimate;
	Vec2 ball_velocity_estimate;
	unsigned ball_time_when_last_seen; // and now we need to synchronize the time too.. use some minimal delta.. argh!
};

// This is the memory layout at the mapped memory file
struct CognitionSharedMem {
	unsigned int cognition_writes; // incremented by 1 by Cognition process after write
	unsigned int cognition_reads;  // incremented by 1 by BHuman process <after/before> read
	unsigned int bhuman_writes;    // incremented by 1 by BHuman process adter write
	unsigned int bhuman_reads;     // incremented by 1 by Cognition process <after/before> read
	CognitionWrittenSharedMem cognition;
	BHumanWrittenSharedMem bhuman;
};

#endif // __COGNITION_SHARED_MEM_H__
