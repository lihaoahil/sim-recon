// $Id$
//
//    File: JFactoryGenerator_DAQ.h
// Created: Thu Aug  9 12:40:08 EDT 2012
// Creator: davidl (on Darwin harriet.jlab.org 11.4.0 i386)
//

#ifndef _JFactoryGenerator_DAQ_
#define _JFactoryGenerator_DAQ_

#include <JANA/jerror.h>
#include <JANA/JFactory.h>
#include <JANA/JFactoryGenerator.h>
#include <JANA/JEventLoop.h>
using namespace jana;

#include "Df250PulseIntegral.h"
#include "Df250StreamingRawData.h"
#include "Df250WindowSum.h"
#include "Df250PulseRawData.h"
#include "Df250TriggerTime.h"
#include "Df250PulseTime.h"
#include "Df250WindowRawData.h"
#include "Df125WindowRawData.h"
#include "Df125PulseRawData.h"
#include "DF1TDCHit.h"
#include "DF1TDCTriggerTime.h"
#include "DCAEN1290TDCHit.h"


class JFactoryGenerator_DAQ: public jana::JFactoryGenerator{
	public:
		JFactoryGenerator_DAQ(){}
		virtual ~JFactoryGenerator_DAQ(){}
		virtual const char* className(void){return static_className();}
		static const char* static_className(void){return "JFactoryGenerator_DAQ";}
		
		jerror_t GenerateFactories(jana::JEventLoop *loop){
			loop->AddFactory(new JFactory<Df250PulseIntegral>());
			loop->AddFactory(new JFactory<Df250StreamingRawData>());
			loop->AddFactory(new JFactory<Df250WindowSum>());
			loop->AddFactory(new JFactory<Df250PulseRawData>());
			loop->AddFactory(new JFactory<Df250TriggerTime>());
			loop->AddFactory(new JFactory<Df250PulseTime>());
			loop->AddFactory(new JFactory<Df250WindowRawData>());
			loop->AddFactory(new JFactory<Df125PulseIntegral>());
			loop->AddFactory(new JFactory<Df125TriggerTime>());
			loop->AddFactory(new JFactory<Df125PulseTime>());
			loop->AddFactory(new JFactory<Df125WindowRawData>());
			loop->AddFactory(new JFactory<Df125PulseRawData>());
			loop->AddFactory(new JFactory<DF1TDCHit>());
			loop->AddFactory(new JFactory<DF1TDCTriggerTime>());
			loop->AddFactory(new JFactory<DCAEN1290TDCHit>());
			return NOERROR;
		}

};

#endif // _JFactoryGenerator_DAQ_

