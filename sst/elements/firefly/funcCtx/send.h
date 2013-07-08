// Copyright 2013 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2013, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef COMPONENTS_FIREFLY_SENDFUNCCTX_H
#define COMPONENTS_FIREFLY_SENDFUNCCTX_H

#include "functionCtx.h"

namespace SST {
namespace Firefly {

class SendCtx : public FunctionCtx 
{
  public:
    SendCtx(Hermes::Addr            target,
            uint32_t                count,
            Hermes::PayloadDataType dtype,
            Hermes::RankID          source,
            uint32_t                tag,
            Hermes::Communicator    group,
            Hermes::MessageRequest* req, 
            Hermes::Functor*        retFunc,
            FunctionType            type,
            Hades*                  obj); 
    ~SendCtx();

    bool run();

    enum { RunProgress, PrePost, Wait } m_state;

  private:
    Hermes::Addr            m_target;
    uint32_t                m_count;
    Hermes::PayloadDataType m_dtype;
    Hermes::RankID          m_source;
    uint32_t                m_tag;
    Hermes::Communicator    m_group;
    Hermes::MessageRequest* m_req;
    SendEntry*              m_sendEntry;
};

}
}

#endif
