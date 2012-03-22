#ifndef _memLink_h
#define _memLink_h

#include <sst_config.h>
#include <sst/core/component.h>

#include <mem/mem_object.hh>
#include <memEvent.h>

class M5;

struct MemLinkParams : public MemObjectParams
{
    M5*             m5Comp;
    Range< Addr >   range;
    std::string     linkName;
    bool            snoop;
    int             delay;
};

//class SST::Link;

class MemLink : public MemObject
{
    class LinkPort : public Port
    {
      public:
        LinkPort( const std::string &name, MemLink* obj ) :
            Port( name, obj )
        {
        }

        void sendFunctional(PacketPtr pkt)
        {
            DBGX_M5(3,"\n");
            Port::sendFunctional( pkt );
            delete pkt;
        }

        bool sendTiming(PacketPtr pkt)
        {
            DBGX_M5(3,"cmd=`%s`addr=%#lx\n",
                        pkt->cmdString().c_str(),pkt->getAddr());
            if ( pkt->memInhibitAsserted() ) {
                DBGX_M5(3,"memInhibitAsserted\n");
                delete pkt;
                return true;
            }
            if ( ! m_deferred.empty() || ! Port::sendTiming( pkt ) ) {
                DBGX_M5(3,"deffered\n");
                m_deferred.push_back( pkt );
            } else {
                DBGX_M5(3,"\n");
            }
            return true;
        }

      protected:
        virtual bool recvTiming(PacketPtr pkt) {
            DBGX_M5(3,"cmd=`%s`addr=%#lx first=%lu finish=%lu\n",
                        pkt->cmdString().c_str(),pkt->getAddr(),
                        pkt->firstWordTime,pkt->finishTime);
            return static_cast<MemLink*>(owner)->send( new MemEvent( pkt ) );
        }

        virtual Tick recvAtomic(PacketPtr pkt) { panic("??"); }

        virtual void recvFunctional(PacketPtr pkt)
        {
            DBGX_M5(2,"\n");

            if ( ! pkt->cmd.isWrite() ) { panic( "read is not supported" ); }

            static_cast<MemLink*>(owner)->send(
                    new MemEvent( pkt, MemEvent::Functional ) );
        }
        virtual void recvRetry()
        {
            while ( ! m_deferred.empty() ) {
                if ( ! Port::sendTiming( m_deferred.front() ) ) {
                    break;
                }
                DBGX_M5(3,"\n");
                m_deferred.pop_front();
            }
        }

        virtual void recvStatusChange(Status status) {}

        virtual void getDeviceAddressRanges(AddrRangeList &resp, bool &snoop)
        { static_cast<MemLink*>(owner)->getAddressRanges( resp, snoop ); }

      private:

        std::deque< PacketPtr > m_deferred;
    };

  public:
    MemLink( const MemLinkParams * );
    virtual Port *getPort(const std::string &if_name, int idx = -1);
    static MemLink* create( std::string name, M5*, Port*, const SST::Params& ); 

  private:
    void init( void );
    bool send( SST::Event* );
    void eventHandler( SST::Event* );
    void getAddressRanges(AddrRangeList &resp, bool &snoop );

    M5*                   m_comp;
    SST::Link*            m_link;
    LinkPort*             m_port;
    Range< Addr >         m_range;
    bool                  m_snoop;
    int                   m_delay;
};

#endif
