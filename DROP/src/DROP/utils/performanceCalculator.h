#pragma once

#include <deque>
#include <iostream>

class PerformanceCalculator
{
private:
    unsigned int windowSize;
    unsigned int overlap;
    unsigned int iter;
    std::deque<double> measurements; 

    double CalculateMeanMeasurements(std::deque<double> queue) const {
        double mean = 0.0f;
        for(size_t i = 0; i < queue.size(); i++){
            mean += queue[i];
        }
        mean /= queue.size();
        return mean;
    };

public:
    double framerate;

    PerformanceCalculator(const uint32_t windowSize_val, const uint32_t overlap_val):
        windowSize(windowSize_val), overlap(overlap_val), iter(0), framerate(0) {};

    ~PerformanceCalculator(){
        // delete &this->measurements;
    };

    void Step(const double deltaTime){
        this->measurements.push_back(deltaTime);

        size_t vectorDim = this->measurements.size();
        if(vectorDim >= windowSize){
            this->measurements.pop_front();
        }

        this->iter++;

        if(this->iter >= overlap){
            this->framerate = 1 / CalculateMeanMeasurements(this->measurements);
            this->iter = 0;
        }
    }

    void LogStatus() const {
        LOG_CORE_TRACE(
            "---START Performance Calculator LOG STATUS---"
            "\n\tSize: {0}"
            "\n\tFramerate: {1}"
            , measurements.size()
            , framerate
        );
        for(size_t i = 0; i < this->measurements.size(); i++){
            LOG_CORE_TRACE("\n\tElm {0}: {1}", i, measurements[i]);
        }

        LOG_CORE_TRACE("---END Performance Calculator LOG STATUS---");
    }
};
