#include <cstdio>
#include <iostream>
#include "MotionState.h"

MotionState::MotionState()
{
}

MotionState::~MotionState()
{
    delete mm;
    delete trajectory;
}

// Lifted from CMotionMatchingDoc::OnNewDocument()
void MotionState::init()
{
    mm         = new CMotionMatching();
    trajectory = new Trajectory();
    mm->trajectory = trajectory;

    frameNum    = 0;
    currentTime = 0;
    lastTime    = 0;
}

// Lifted from CMotionMatchingDoc::forwardFrame()
void MotionState::forwardFrame(int df)
{
    frameNum    += df;
    currentTime += df;
    trajectory->setCurrentTime(frameNum);
}

// Lifted from CMotionMatchingDoc::applyMotionMatching()
void MotionState::applyMotionMatching()
{
    mm->applyMotionMatching();
}

// Resets mm internal state and MotionState frame counters.
// CMotionMatching::reset() clears postures and cost accumulators;
// trajectory goal index is reset on the next forwardFrame() call.
void MotionState::reset()
{
    mm->reset();
    frameNum    = 0;
    currentTime = 0;
    lastTime    = 0;
}

// Lifted from CMotionMatchingDoc::OnMotionmatchingDoexperiment(),
// file-path argument replaces CFileDialog/CString.
// UpdateAllViews() is omitted — the caller handles any repaint.
void MotionState::runExperiment(const std::string& outputPath)
{
    FILE* file = fopen(outputPath.c_str(), "w");
    if (file == nullptr)
    {
        std::cout << "Error: cannot open output file: " << outputPath << std::endl;
        return;
    }

    reset();

    mm->dstMotion.m_pSkeleton->root->setGlobalTransform();
    mm->dstMotion.m_pSkeleton->root->printGlobalPos();

    double height = mm->dstMotion.m_pSkeleton->getHeight();
    printf("err\n");

    for (int i = 0; i < trajectory->m_numSample; i++)
    {
        mm->applyMotionMatching();
        double err = mm->computeErr();
        err /= height;
        err *= 1.8;
        forwardFrame();
        fprintf(file, "%lf\n", err);
    }

    fclose(file);
    std::cout << "Done!" << std::endl;
}
