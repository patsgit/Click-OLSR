/*
 * settxpower.{cc,hh} -- sets wifi txpower annotation on a packet
 * John Bicket
 *
 * Copyright (c) 2003 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/packet_anno.hh>
#include "settxpower.hh"
#include <clicknet/ether.h>
#include <clicknet/wifi.h>
#include <click/etheraddress.hh>
CLICK_DECLS

SetTXPower::SetTXPower()
  : Element(1, 1)
{
  MOD_INC_USE_COUNT;
}

SetTXPower::~SetTXPower()
{
  MOD_DEC_USE_COUNT;
}

int
SetTXPower::configure(Vector<String> &conf, ErrorHandler *errh)
{
  _power = 0;

  if (cp_va_parse(conf, this, errh,
		  cpKeywords, 
		  "POWER", cpUnsigned, "power", &_power, 
		  0) < 0) {
    return -1;
  }

  return 0;
}

Packet *
SetTXPower::simple_action(Packet *p_in)
{
  if (p_in) {
    struct click_wifi_extra *ceh = (struct click_wifi_extra *) p_in->all_user_anno();
    ceh->magic = WIFI_EXTRA_MAGIC;
    ceh->power = _power;
    return p_in;
  }
  return 0;
}

enum {H_POWER,

};
static String
SetTXPower_read_param(Element *e, void *thunk)
{
  SetTXPower *td = (SetTXPower *)e;
  switch ((uintptr_t) thunk) {
  case H_POWER:
    return String(td->_power) + "\n";
  default:
    return String();
  }
}
static int 
SetTXPower_write_param(const String &in_s, Element *e, void *vparam,
		      ErrorHandler *errh)
{
  SetTXPower *f = (SetTXPower *)e;
  String s = cp_uncomment(in_s);
  switch((int)vparam) {
  case H_POWER: {
    unsigned m;
    if (!cp_unsigned(s, &m)) 
      return errh->error("power parameter must be unsigned");
    f->_power = m;
    break;
  
  }
  }
  return 0;
}
void
SetTXPower::add_handlers()
  {
  add_default_handlers(true);
  add_read_handler("power", SetTXPower_read_param, (void *) H_POWER);
  add_write_handler("power", SetTXPower_write_param, (void *) H_POWER);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SetTXPower)

