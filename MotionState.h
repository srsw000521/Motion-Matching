#pragma once
#include "CMotionMatching.h"
#include "Trajectory.h"
#include <string>

class MotionState {
public:
    MotionState();
    ~MotionState();

    CMotionMatching* mm         = nullptr;
    Trajectory*      trajectory = nullptr;
    int frameNum    = 0;
    int currentTime = 0;
    int lastTime    = 0;

    void init();
    void forwardFrame(int df = 1);
    void applyMotionMatching();
    void reset();
    void runExperiment(const std::string& outputPath);
};
