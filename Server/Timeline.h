#ifndef H_TIMELINE
#define H_TIMELINE
#include "shapes.h"
#include <thread>
#include <chrono>
#include <mutex>

class Timeline
{
private:
    std::mutex m; // thread safe

    double startTime; // time of underlying measure of time at creation of this timeline
    double tic;       // # of time units of the underlying measure of time that correspond to one time step of this timeline

    bool paused;              // is this timeline paused?
    double elapsed_pause_time; // how much time has elapsed since this timeline was paused?
    double elapsed_bool_time; // what was the time when this was paused?

    Timeline *anchor; // pass as null for getting real time

public:
    Timeline(Timeline *anchor, double tic);
    double virtual getTime();
    void virtual pause();
    void virtual unpause();
    void virtual changeTic(double newTic);
    bool virtual isPaused();
    double virtual getTic();
};

#endif