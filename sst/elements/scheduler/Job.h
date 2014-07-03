// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef SST_SCHEDULER_JOB_H__
#define SST_SCHEDULER_JOB_H__

#include "sst/core/serialization.h"

#include <string>
#include <sstream>

#include "Statistics.h"  //needed for friend declaration
#include "TaskCommInfo.h"


namespace SST {
    namespace Scheduler {

        class AllocInfo;
        class Machine;

        class Job {
            public:
                Job(unsigned long arrivalTime, int procsNeeded, unsigned long actualRunningTime,
                    unsigned long estRunningTime);
                Job(long arrivalTime, int procsNeeded, long actualRunningTime,
                    long estRunningTime, std::string ID );
                Job(const Job &job);
                
                ~Job();

                unsigned long getStartTime() const { return startTime; }
                unsigned long getArrivalTime() const { return arrivalTime; }
                unsigned long getActualTime() const { return actualRunningTime; }
                int getProcsNeeded() const { return procsNeeded; }
                long getJobNum() const { return jobNum; }
                bool hasStarted() const { return started; }
                //assumes that allocation is not considered:
                unsigned long getEstimatedRunningTime() const { return estRunningTime; }
                
                std::string * getID() {
                    if (0 == ID.length()) {
                        std::ostringstream jobNumStr;
                        jobNumStr << jobNum;
                        ID = std::string( jobNumStr.str() );
                    }
                    return & ID;
                }
                
                void reset();
                
                std::string toString();

                void start(unsigned long time);
                void setFST(unsigned long FST);
                unsigned long getFST();
                void startsAtTime(unsigned long time);
                TaskCommInfo* taskCommInfo;
                
                //these are used to prevent high memory usage:
                //the communication files are read only when the job starts
                TaskCommInfo::commType commType;
                std::string commFile;
                std::string coordFile;
                int meshx, meshy, meshz;

            private:
                unsigned long arrivalTime;        //when the job arrived
                int procsNeeded;         //how many processors it uses
                unsigned long actualRunningTime;  //how long it runs
                unsigned long estRunningTime;     //user estimated running time
                unsigned long startTime;	     //when the job started (-1 if not running)
                //unsigned long jobFST;           //FST value for job
                bool hasRun;
                bool started;

                long jobNum;             //ID number unique to this job

                std::string ID;       // alternate ID not used by SST

                //helper for constructors:
                void initialize(unsigned long arrivalTime, int procsNeeded,
                                unsigned long actualRunningTime, unsigned long estRunningTime);

                friend class Statistics;
                friend class schedComponent;
                //so statistics and schedComponent can use actual job times

                friend class boost::serialization::access;
                template<class Archive>
                    void serialize(Archive & ar, const unsigned int version )
                    {
                        ar & BOOST_SERIALIZATION_NVP(arrivalTime);
                        ar & BOOST_SERIALIZATION_NVP(procsNeeded);
                        ar & BOOST_SERIALIZATION_NVP(actualRunningTime);
                        ar & BOOST_SERIALIZATION_NVP(estRunningTime);
                        ar & BOOST_SERIALIZATION_NVP(startTime);
                        ar & BOOST_SERIALIZATION_NVP(jobNum);
                        ar & BOOST_SERIALIZATION_NVP(ID);
                    }
        };
    }
}
#endif
