// -*- c-basic-offset: 4 -*-
#ifndef CLICK_LINKUNQUEUE_HH
#define CLICK_LINKUNQUEUE_HH
#include <click/element.hh>
#include <click/task.hh>
#include <click/timer.hh>
#include <click/notifier.hh>
#include <click/standard/storage.hh>
CLICK_DECLS

/*
=c

LinkUnqueue(LATENCY, BANDWIDTH)

=s packet scheduling

link emulator

=d

Pulls packets from the single input port, then delays them so as to emulate a
serial link with latency LATENCY (microsecond precision) and bandwidth
BANDWIDTH (in Kbits/second). Thus, every packet is delayed at least by
LATENCY, and additionally delayed by their packet size and BANDWIDTH, and by
any packets ahead of it in the virtual link. Packets are pushed out the single
output port when their time comes.

LinkUnqueue listens for upstream notification, such as that available from
Queue.

LinkUnqueue stores packets that are in transit over the virtual link. It
doesn't store any other packets, however; it doesn't pull from its input
unless there is room on the link. To emulate a link fed by a packet queue, use
a "Queue -> LinkUnqueue" combination.

LinkUnqueue uses its input packets' "extra length" annotations, and destroys
their "next packet" annotations.

=h latency read-only

The LATENCY argument.

=h bandwidth read-only

The BANDWIDTH argument.

=h size read-only

The number of packets currently in, or partially in, the emulated link.

=h reset write-only

When written, drops all packets in, or partially in, the emulated link.

=a DelayUnqueue, Queue, Unqueue, RatedUnqueue, BandwidthRatedUnqueue,
DelayShaper, SetTimestamp */

class LinkUnqueue : public Element, public Storage { public:
  
    LinkUnqueue();
    ~LinkUnqueue();

    const char *class_name() const	{ return "LinkUnqueue"; }
    const char *processing() const	{ return PULL_TO_PUSH; }
    void *cast(const char *);
    LinkUnqueue *clone() const		{ return new LinkUnqueue; }

    int configure(Vector<String> &, ErrorHandler *);
    int initialize(ErrorHandler *);
    void cleanup(CleanupStage);
    void add_handlers();
  
    bool run_task();

  private:

    Packet *_qhead;
    Packet *_qtail;
    struct timeval _latency;
    enum { S_TASK, S_TIMER, S_ASLEEP } _state;
    uint32_t _bandwidth;
    Task _task;
    Timer _timer;
    NotifierSignal _signal;

    void delay_by_bandwidth(Packet *, const struct timeval &) const;
    static String read_param(Element *, void *);
    static int write_handler(const String &, Element *, void *, ErrorHandler *);

};

CLICK_ENDDECLS
#endif