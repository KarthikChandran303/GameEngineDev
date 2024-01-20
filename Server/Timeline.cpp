#include "Timeline.h"

Timeline::Timeline(Timeline *anchor, double tic)
{
    this->anchor = anchor;
    this->tic = tic;
    if (anchor == NULL)
    {
        startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    else
    {
        startTime = anchor->getTime();
    }
    this->paused = false;
    this->elapsed_pause_time = 0.0;
    this->elapsed_bool_time = 0.0;
}

void Timeline::changeTic(double newTic)
{
    this->tic = newTic;
}

double Timeline::getTime()
{
    if (anchor == NULL)
    {
        return ( std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime ) / 1000000; // get global time
    }
    else if (paused)
    {
        return (elapsed_bool_time - elapsed_pause_time) * tic - startTime;
    }
    else
    {
        return ( ( anchor->getTime() - elapsed_pause_time ) * tic ) - startTime;
    }
}

void Timeline::pause()
{
    if (anchor == NULL)
    {
        return; // can't pause global timeline
    }

    else if (!paused)
    {
        paused = true;
        elapsed_bool_time = anchor->getTime();
    }
}

void Timeline::unpause()
{
    if (anchor == NULL)
    {
        return; // can't unpause global timeline
    }

    else if (paused)
    {
        paused = false;
        elapsed_pause_time += ( anchor->getTime() - elapsed_bool_time );
    }
}

bool Timeline::isPaused()
{
    return paused;
}

double Timeline::getTic()
{
    return tic;
}