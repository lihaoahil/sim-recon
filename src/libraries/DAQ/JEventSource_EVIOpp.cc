// $Id$
//
//    File: JEventSource_EVIOpp.cc
// Created: Tue Mar 29 08:14:42 EDT 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <forward_list>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace std::chrono;


#include <TTAB/DTranslationTable_factory.h>
#include <DAQ/Df250EmulatorAlgorithm_v1.h>
#include <DAQ/Df125EmulatorAlgorithm_v2.h>


#include "JEventSource_EVIOpp.h"
#include "LinkAssociations.h"
using namespace jana;

//----------------
// Constructor
//----------------
JEventSource_EVIOpp::JEventSource_EVIOpp(const char* source_name):JEventSource(source_name)
{
	DONE = false;
	NEVENTS_PROCESSED = 0;
	NWAITS_FOR_THREAD = 0;
	NWAITS_FOR_PARSED_EVENT = 0;
	

	// Initialize dedicated JStreamLog used for debugging messages
	evioout.SetTag("--- EVIO ---: ");
	evioout.SetTimestampFlag();
	evioout.SetThreadstampFlag();
	
	// Define base set of status bits
	if(japp) DStatusBits::SetStatusBitDescriptions(japp);

	// Get configuration parameters
	VERBOSE = 0;
	NTHREADS = 2;
	MAX_PARSED_EVENTS = 128;
	MAX_EVENT_RECYCLES = 1000;
	MAX_OBJECT_RECYCLES = 1000;
	LOOP_FOREVER = false;
	USER_RUN_NUMBER = 0;
	ET_STATION_NEVENTS = 10;
	ET_STATION_CREATE_BLOCKING = false;
	PRINT_STATS = true;
	SWAP = true;
	LINK = true;
	PARSE = true;
	PARSE_F250 = true;
	PARSE_F125 = true;
	PARSE_F1TDC = true;
	PARSE_CAEN1290TDC = true;
	PARSE_CONFIG = true;
	PARSE_BOR = true;
	PARSE_EPICS = true;
	PARSE_EVENTTAG = true;
	PARSE_TRIGGER = true;
	F250_EMULATION_MODE = kEmulationAuto;
	F125_EMULATION_MODE = kEmulationAuto;


	gPARMS->SetDefaultParameter("EVIO:VERBOSE", VERBOSE, "Set verbosity level for processing and debugging statements while parsing. 0=no debugging messages. 10=all messages");
	gPARMS->SetDefaultParameter("EVIO:NTHREADS", NTHREADS, "Set the number of worker threads to use for parsing the EVIO data");
	gPARMS->SetDefaultParameter("EVIO:MAX_PARSED_EVENTS", MAX_PARSED_EVENTS, "Set maximum number of events to allow in EVIO parsed events queue");
	gPARMS->SetDefaultParameter("EVIO:MAX_EVENT_RECYCLES", MAX_EVENT_RECYCLES, "Set maximum number of EVIO (i.e. block of) events  a worker thread should process before pruning excess DParsedEvent objects from its pool");
	gPARMS->SetDefaultParameter("EVIO:MAX_OBJECT_RECYCLES", MAX_OBJECT_RECYCLES, "Set maximum number of events a DParsedEvent should be used for before pruning excess hit objects from its pools");
	gPARMS->SetDefaultParameter("EVIO:LOOP_FOREVER", LOOP_FOREVER, "If reading from EVIO file, keep re-opening file and re-reading events forever (only useful for debugging) If reading from ET, this is ignored.");
	gPARMS->SetDefaultParameter("EVIO:RUN_NUMBER", USER_RUN_NUMBER, "User-supplied run number. Override run number from other sources with this.(will be ignored if set to zero)");
	gPARMS->SetDefaultParameter("EVIO:ET_STATION_NEVENTS", ET_STATION_NEVENTS, "Number of events to use if we have to create the ET station. Ignored if station already exists.");
	gPARMS->SetDefaultParameter("EVIO:ET_STATION_CREATE_BLOCKING", ET_STATION_CREATE_BLOCKING, "Set this to 0 to create station in non-blocking mode (default is to create it in blocking mode). Ignored if station already exists.");
	gPARMS->SetDefaultParameter("EVIO:PRINT_STATS", PRINT_STATS, "Print some additional stats from event source when it's finished processing events");

	gPARMS->SetDefaultParameter("EVIO:SWAP", SWAP, "Allow swapping automatic swapping. Turning this off should only be used for debugging.");
	gPARMS->SetDefaultParameter("EVIO:LINK", LINK, "Link associated objects. Turning this off should only be used for debugging.");

	gPARMS->SetDefaultParameter("EVIO:PARSE", PARSE, "Set this to 0 to disable parsing of event buffers and generation of any objects (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_F250", PARSE_F250, "Set this to 0 to disable parsing of data from F250 ADC modules (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_F125", PARSE_F125, "Set this to 0 to disable parsing of data from F125 ADC modules (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_F1TDC", PARSE_F1TDC, "Set this to 0 to disable parsing of data from F1TDC modules (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_CAEN1290TDC", PARSE_CAEN1290TDC, "Set this to 0 to disable parsing of data from CAEN 1290 TDC modules (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_CONFIG", PARSE_CONFIG, "Set this to 0 to disable parsing of ROC configuration data in the data stream (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_BOR", PARSE_BOR, "Set this to 0 to disable parsing of BOR events from the data stream (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_EPICS", PARSE_EPICS, "Set this to 0 to disable parsing of EPICS events from the data stream (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_EVENTTAG", PARSE_EVENTTAG, "Set this to 0 to disable parsing of event tag data in the data stream (for benchmarking/debugging)");
	gPARMS->SetDefaultParameter("EVIO:PARSE_TRIGGER", PARSE_TRIGGER, "Set this to 0 to disable parsing of the built trigger bank from CODA (for benchmarking/debugging)");

	gPARMS->SetDefaultParameter("EVIO:F250_EMULATION_MODE", F250_EMULATION_MODE, "Set f250 emulation mode. 0=no emulation, 1=always, 2=auto. Default is 2 (auto).");
	gPARMS->SetDefaultParameter("EVIO:F125_EMULATION_MODE", F125_EMULATION_MODE, "Set f125 emulation mode. 0=no emulation, 1=always, 2=auto. Default is 2 (auto).");

	jobtype = DEVIOWorkerThread::JOB_NONE;
	if(PARSE) jobtype |= DEVIOWorkerThread::JOB_FULL_PARSE;
	if(LINK ) jobtype |= DEVIOWorkerThread::JOB_ASSOCIATE;
	
	source_type          = kNoSource;
	hdevio               = NULL;
	hdet                 = NULL;
	et_quit_next_timeout = false;

	uint64_t run_number_seed = 0;

	// Open either ET system or file
	if(this->source_name.find("ET:") == 0){

		// Try to open ET system
		if(VERBOSE>0) evioout << "Attempting to open \""<<this->source_name<<"\" as ET (network) source..." <<endl;

		hdet = new HDET(this->source_name, ET_STATION_NEVENTS, ET_STATION_CREATE_BLOCKING);
		if( ! hdet->is_connected ){
			cerr << hdet->err_mess.str() << endl;
			throw JException("Failed to open ET system: " + this->source_name, __FILE__, __LINE__);
		}
		source_type = kETSource;

	}else{

		// Try to open the file.
		if(VERBOSE>0) evioout << "Attempting to open \""<<this->source_name<<"\" as EVIO file..." <<endl;

		hdevio = new HDEVIO(this->source_name);
		if( ! hdevio->is_open ){
			cerr << hdevio->err_mess.str() << endl;
			throw JException("Failed to open EVIO file: " + this->source_name, __FILE__, __LINE__); // throw exception indicating error
		}
		source_type = kFileSource;
		
		run_number_seed = SearchFileForRunNumber(); // try and dig out run number from file
	}

	if(VERBOSE>0) evioout << "Success opening event source \"" << this->source_name << "\"!" <<endl;
	
	// Create dispatcher thread
	dispatcher_thread = new thread(&JEventSource_EVIOpp::Dispatcher, this);

	// Create worker threads
	for(uint32_t i=0; i<NTHREADS; i++){
		DEVIOWorkerThread *w = new DEVIOWorkerThread(this, parsed_events, MAX_PARSED_EVENTS, PARSED_EVENTS_MUTEX, PARSED_EVENTS_CV);
		w->MAX_EVENT_RECYCLES  = MAX_EVENT_RECYCLES;
		w->MAX_OBJECT_RECYCLES = MAX_OBJECT_RECYCLES;
		w->PARSE_F250          = PARSE_F250;
		w->PARSE_F125          = PARSE_F125;
		w->PARSE_F1TDC         = PARSE_F1TDC;
		w->PARSE_CAEN1290TDC   = PARSE_CAEN1290TDC;
		w->PARSE_CONFIG        = PARSE_CONFIG;
		w->PARSE_BOR           = PARSE_BOR;
		w->PARSE_EPICS         = PARSE_EPICS;
		w->PARSE_EVENTTAG      = PARSE_EVENTTAG;
		w->PARSE_TRIGGER       = PARSE_TRIGGER;
		w->run_number_seed     = run_number_seed;
		worker_threads.push_back(w);
	}
	
	// Create emulator objects
	f250Emulator = new Df250EmulatorAlgorithm_v1(NULL);
	f125Emulator = new Df125EmulatorAlgorithm_v2();

	// Record start time
	tstart = high_resolution_clock::now();

}

//----------------
// Destructor
//----------------
JEventSource_EVIOpp::~JEventSource_EVIOpp()
{
	// Set DONE flag to tell dispatcher thread to quit
	// as well as anyone in a wait state
	DONE = true;
	PARSED_EVENTS_CV.notify_all();
	
	// Wait for dispatcher to complete
	if(dispatcher_thread){
		dispatcher_thread->join();
		delete dispatcher_thread;
	}

	// Wait for all worker threads to end and destroy them all
	for(uint32_t i=0; i<worker_threads.size(); i++){
		worker_threads[i]->Finish();
		delete worker_threads[i];
	}
	
	// Delete emulator objects
	if(f250Emulator) delete f250Emulator;
	if(f125Emulator) delete f125Emulator;
	
	if(VERBOSE>0) evioout << "Closing hdevio event source \"" << this->source_name << "\"" <<endl;
	if(PRINT_STATS){
		auto tdiff = duration_cast<duration<double>>(tend - tstart);
		double rate = (double)NEVENTS_PROCESSED/tdiff.count();

		cout << endl;
		cout << "   EVIO Processing rate = " << rate << " Hz" << endl;
		cout << "      NWAITS_FOR_THREAD = " << NWAITS_FOR_THREAD << endl;
		cout << "NWAITS_FOR_PARSED_EVENT = " << NWAITS_FOR_PARSED_EVENT << endl;
	}
	
	// Delete all BOR objects
	for(auto p : borptrs_list) delete p;

	// Delete HDEVIO and print stats
	if(hdevio){
		hdevio->PrintStats();
		delete hdevio;
	}

	// Delete HDET and print stats
	if(hdet){
		hdet->PrintStats();
		delete hdet;
	}
}

//----------------
// Dispatcher
//----------------
void JEventSource_EVIOpp::Dispatcher(void)
{
	/// This is run in a dedicated thread created by the constructor.
	/// It's job is to read in events and dispatch the processing of
	/// them to worker threads. When a worker thread is done, it adds
	/// the event to the parsed_events list and clears its own "in_use"
	/// flag thereby, marking itself as available for another job.
	/// The worker threads will stall if adding the event(s) it produced
	/// would make parsed_events contain more than MAX_PARSED_EVENTS.
	/// This creates backpressure here by having no worker threads
	/// available.
	
	bool allow_swap = false; // Defer swapping to DEVIOWorkerThread
	uint64_t istreamorder = 0;
	while(true){
	
		if(japp->GetQuittingStatus()) break;

		// Get worker thread to handle this
		DEVIOWorkerThread *thr = NULL;
		while( !thr){
			for(uint32_t i=0; i<worker_threads.size(); i++){
				if(worker_threads[i]->in_use) continue;
				thr = worker_threads[i];
				break;
			}
			if(!thr) {
				NWAITS_FOR_THREAD++;
				this_thread::sleep_for(milliseconds(1));
			}
			if(DONE) break;
		}
		if(DONE) break;
		
		// Reduce average memory usage.
		if(++thr->Nrecycled%thr->MAX_EVENT_RECYCLES == 0) thr->Prune();
		
		uint32_t* &buff     = thr->buff;
		uint32_t  &buff_len = thr->buff_len;
		
		bool swap_needed = false;

		if(source_type==kFileSource){
			// ---- Read From File ----
//			hdevio->read(buff, buff_len, allow_swap);
//			hdevio->readSparse(buff, buff_len, allow_swap);
			hdevio->readNoFileBuff(buff, buff_len, allow_swap);
			thr->pos = hdevio->last_event_pos;
			if(hdevio->err_code == HDEVIO::HDEVIO_USER_BUFFER_TOO_SMALL){
				delete[] buff;
				buff_len = hdevio->last_event_len;
				buff = new uint32_t[buff_len];
				continue;
			}else if(hdevio->err_code!=HDEVIO::HDEVIO_OK){
				if(LOOP_FOREVER && NEVENTS_PROCESSED>=1){
					if(hdevio){
						hdevio->rewind();
						continue;
					}
				}else{
					cout << hdevio->err_mess.str() << endl;
				}
				break;
			}else{
				// HDEVIO_OK
				swap_needed = hdevio->swap_needed;
			}
		}else{
			// ---- Read From ET ----
			hdet->read(buff, buff_len, allow_swap);
			thr->pos = 0;
			if(hdet->err_code == HDET::HDET_TIMEOUT){
				if(et_quit_next_timeout) break;
				this_thread::sleep_for(milliseconds(1));
				continue;
			}else if(hdet->err_code != HDET::HDET_OK){
				cout << hdet->err_mess.str() << endl;
				break;
			}else{
				// HDET_OK
				swap_needed = hdet->swap_needed;
			}
		}

		uint32_t myjobtype = jobtype;
		if(swap_needed && SWAP) myjobtype |= DEVIOWorkerThread::JOB_SWAP;
		
		// Wake up worker thread to handle event
		thr->in_use = true;
		thr->jobtype = (DEVIOWorkerThread::JOBTYPE)myjobtype;
		thr->istreamorder = istreamorder++;

		thr->cv.notify_all();
	}
	
	// If we get here then there are no more events in the source.
	// Wait for all worker threads to become available so we know 
	// the system is drained of events. Then set the DONE flag so
	// GetEvent will properly return NO_MORE_EVENTS_IN_SOURCE.
	for(uint32_t i=0; i<worker_threads.size(); i++){
		worker_threads[i]->done = true;
		while(worker_threads[i]->in_use){
			this_thread::sleep_for(milliseconds(10));
		}
	}
	
	tend = std::chrono::high_resolution_clock::now();
	DONE = true;
}

//----------------
// GetEvent
//----------------
jerror_t JEventSource_EVIOpp::GetEvent(JEvent &event)
{
	// Get next event from list, waiting if necessary
	unique_lock<std::mutex> lck(PARSED_EVENTS_MUTEX);
	while(parsed_events.empty()){
		if(DONE) return NO_MORE_EVENTS_IN_SOURCE;
		NWAITS_FOR_PARSED_EVENT++;
		PARSED_EVENTS_CV.wait_for(lck,std::chrono::milliseconds(1));
	}

	DParsedEvent *pe = parsed_events.front();
	parsed_events.pop_front();
	
	// Release mutex and notify workers they can use it again
	lck.unlock();
	PARSED_EVENTS_CV.notify_all();
	
	// If this is a BOR event, then take ownership of
	// the DBORptrs object. If not, then copy a pointer
	// to the latest DBORptrs object into the event.
	if(pe->borptrs) borptrs_list.push_front(pe->borptrs);

	// Copy info for this parsed event into the JEvent
	event.SetJEventSource(this);
	event.SetEventNumber(pe->event_number);
	event.SetRunNumber(USER_RUN_NUMBER>0 ? USER_RUN_NUMBER:pe->run_number);
	event.SetRef(pe);

	// Set event status bits
	event.SetStatus(pe->event_status_bits);
	event.SetStatusBit(kSTATUS_EVIO);
	if( source_type == kFileSource ) event.SetStatusBit(kSTATUS_FROM_FILE);
	if( source_type == kETSource   ) event.SetStatusBit(kSTATUS_FROM_ET);

	// EPICS and BOR events are barrier events
	if(event.GetStatusBit(kSTATUS_EPICS_EVENT)) event.SetSequential();
	if(event.GetStatusBit(kSTATUS_BOR_EVENT  )) event.SetSequential();
	
	// Only add BOR events to physics events
	if(pe->borptrs==NULL)
		if(!borptrs_list.empty()) pe->borptrs = borptrs_list.front();
	
	return NOERROR;
}

//----------------
// FreeEvent
//----------------
void JEventSource_EVIOpp::FreeEvent(JEvent &event)
{
	// CAUTION: This is called by the EventBufferThread in JANA which
	// effectively serializes everything done here. (Don't delete all
	// objects in pe which can be slow.)
	DParsedEvent *pe = (DParsedEvent*)event.GetRef();
	pe->in_use = false; // return pe to pool
	
	NEVENTS_PROCESSED++;
}

//----------------
// GetObjects
//----------------
jerror_t JEventSource_EVIOpp::GetObjects(JEvent &event, JFactory_base *factory)
{
	// This callback is called to extract objects of a specific type from
	// an event and store them in the factory pointed to by JFactory_base.
	// The data type desired can be obtained via factory->GetDataClassName()
	// and the tag via factory->Tag().
	//
	// If the object is not one of a type this source can provide, then
	// it should return OBJECT_NOT_AVAILABLE. Otherwise, it should return
	// NOERROR;
	
	// When first called, the data objects will still be in the DParsedEvent
	// object and not yet copied into their respective factories. This will
	// do that copy of the pointers and set the "copied_to_factories" flag
	// in DParsedEvent indicating this has been done. It will then check the
	// name of the class being requested and return the appropriate value
	// depending on whether we supply that class or not.
	
	// We must have a factory to hold the data
	if(!factory)throw RESOURCE_UNAVAILABLE;

	// Get name of data class we're trying to extract and the factory tag
	string dataClassName = factory->GetDataClassName();
	string tag = factory->Tag();
	if(tag.length()!=0) return OBJECT_NOT_AVAILABLE; // can't provide tagged factory objects
	
	// Get any translation tables we'll need to apply
	JEventLoop *loop = event.GetJEventLoop();
	vector<const DTranslationTable*> translationTables;
	DTranslationTable_factory *ttfac = static_cast<DTranslationTable_factory*>(loop->GetFactory("DTranslationTable"));
	if(ttfac) ttfac->Get(translationTables);
	
	// Copy pointers to all hits to appropriate factories.
	// Link BORconfig objects and apply translation tables if appropriate.
	DParsedEvent *pe = (DParsedEvent*)event.GetRef();
	if(!pe->copied_to_factories){

		// Optionally link BOR object associations
		if(LINK && pe->borptrs) LinkBORassociations(pe);
		
		// Optionally emulate flash firmware
		if(!pe->vDf250WindowRawData.empty()) EmulateDf250Firmware(pe);
		if(!pe->vDf125WindowRawData.empty()) EmulateDf125Firmware(pe);

		// Copy all low-level hits to appropriate factories
		pe->CopyToFactories(loop);

		// Apply translation tables to create DigiHit objects
		for(auto tt : translationTables){
			tt->ApplyTranslationTable(loop);
		}
	}

	// Decide whether this is a data type the source supplies
	bool isSuppliedType = pe->IsParsedDataType(dataClassName);
	for(auto tt : translationTables){
		if(isSuppliedType) break;  // once this is set, it's set
		isSuppliedType = tt->IsSuppliedType(dataClassName);
	}

	// Check if this is a class we provide and return appropriate value
	if( isSuppliedType ){
		// We do produce this type
		return NOERROR;
	}else{
		// We do not produce this type
		return OBJECT_NOT_AVAILABLE;
	}	
}

//----------------
// LinkBORassociations
//----------------
void JEventSource_EVIOpp::LinkBORassociations(DParsedEvent *pe)
{
	/// Add BORConfig objects as associated objects
	/// to selected hit objects. Most other object associations
	/// are made in DEVIOWorkerThread::LinkAllAssociations. The
	/// BORConfig objects however, are not available when that
	/// is called.
	/// This is called from GetObjects() which is called from
	/// one of the processing threads.
	
	// n.b. all of the BORConfig object vectors will already
	// be sorted by rocid, then slot when the data was parsed.
	// The other hit objects are also already sorted (in
	// DEVIOWorkerThread::LinkAllAssociations).

	DBORptrs *borptrs = pe->borptrs;

	LinkModule(borptrs->vDf250BORConfig, pe->vDf250WindowRawData);
	LinkModule(borptrs->vDf250BORConfig, pe->vDf250PulseIntegral);

	LinkModule(borptrs->vDf125BORConfig, pe->vDf125WindowRawData);
	LinkModule(borptrs->vDf125BORConfig, pe->vDf125PulseIntegral);
	LinkModule(borptrs->vDf125BORConfig, pe->vDf125CDCPulse);
	LinkModule(borptrs->vDf125BORConfig, pe->vDf125FDCPulse);

	LinkModule(borptrs->vDF1TDCBORConfig, pe->vDF1TDCHit);

	LinkModule(borptrs->vDCAEN1290TDCBORConfig, pe->vDCAEN1290TDCHit);

}

////----------------
//// Cleanup
////----------------
//void JEventSource_EVIOpp::Cleanup(void)
//{
//	/// This is called internally by the JEventSource_EVIO class
//	/// once all events have been read in. Its purpose is to
//	/// free the hidden memory in all of the container class
//	/// members of the JEventSource_EVIO class. This is needed
//	/// for jobs that process a lot of input files and therefore
//	/// create a lot JEventSource_EVIO objects. JANA does not delete
//	/// these objects until the end of the job so this tends to
//	/// act like a memory leak. The data used can be substantial
//	/// (nearly 1GB per JEventSource_EVIO object).
//
//	if(hdevio) delete hdevio;
//	hdevio = NULL;
//
//	if(hdet) delete hdet;
//	hdet = NULL;
//
//}

//----------------
// SearchFileForRunNumber
//----------------
uint64_t JEventSource_EVIOpp::SearchFileForRunNumber(void)
{
	/// This is called from the constructor when reading
	/// from a file to seed the default run number. This
	/// is needed because the first event is probably a BOR
	/// event which does not contain the run number. The run
	/// number would then be reported as "0" and incorrect 
	/// calibrations and field maps would be read in. This 
	/// will try searching past the first (BOR) event for 
	/// either a physics event, or an EPICS event that has
	/// the run number defined in it.
	///
	/// The value returned from this will be copied into each
	/// DEVIOWorkerThread object when it is created. It will
	/// be used to initialize the run number for each event
	/// the worker thread processes. If the worker thread 
	/// processes a physics event containing a run number,
	/// then that will overwrite the run number in the
	/// DParsedEvent. Finally, if the user specified a run
	/// number, then that will be reported to JANA in lieu of
	/// the one in DParsedEvent.

	uint32_t buff_len = 4000000;
	uint32_t *buff = new uint32_t[buff_len];
	HDEVIO *hdevio = new HDEVIO(source_name);
	while(hdevio->read(buff, buff_len)){

		// Assume first word is number of words in bank
		uint32_t *iptr = buff;
		uint32_t *iend = &iptr[*iptr - 1];
		if(*iptr > 2048) iend = &iptr[2048];
		bool has_timestamps = false;
		while(iptr<iend){
			iptr++;

			// EPICS event
			if( (*iptr & 0xff000f) ==  0x600001){
				if(VERBOSE>2) evioout << "     Found EPICS header. Looking for HD:coda:daq:run_number ..." << endl;
				const char *cptr = (const char*)&iptr[1];
				const char *cend = (const char*)iend;
				const char *needle = "HD:coda:daq:run_number=";
				while(cptr<cend){
					if(VERBOSE>4) evioout << "       \""<<cptr<<"\"" << endl;
					if(!strncmp(cptr, needle, strlen(needle))){
					if(VERBOSE>2) evioout << "     Found it!" << endl;
					uint64_t run_number_seed = atoi(&cptr[strlen(needle)]);
					if(hdevio) delete hdevio;
					if(buff) delete[] buff;
					return run_number_seed;
				}
				cptr+=4; // should only start on 4-byte boundary!
			}
		}

		// BOR event
		if( (*iptr & 0xffffffff) ==  0x00700E01) continue;

		// PHYSICS event
		bool not_in_this_buffer = false;
		switch((*iptr)>>16){
			case 0xFF10:
			case 0xFF11:
			case 0xFF20:
			case 0xFF21:
			case 0xFF24:
			case 0xFF25:
			case 0xFF30:
				not_in_this_buffer = true;
				break;
			case 0xFF23:
			case 0xFF27:
				has_timestamps = true;
			case 0xFF22:
			case 0xFF26:
				break;
			default:
				continue;
		}

		if(not_in_this_buffer) break; // go to next EVIO buffer

		iptr++;
		if( ((*iptr)&0x00FF0000) != 0x000A0000) { iptr--; continue; }
		uint32_t M = iptr[-3] & 0x000000FF; // Number of events from Physics Event header
		if(VERBOSE>2) evioout << " ...(epic quest) Trigger bank " << (has_timestamps ? "does":"doesn't") << " have timestamps. Nevents in block M=" << M <<endl;
		iptr++;
		uint64_t *iptr64 = (uint64_t*)iptr;

		uint64_t event_num = *iptr64;
		if(VERBOSE>3) evioout << " ....(epic quest) Event num: " << event_num <<endl;
		iptr64++;
		if(has_timestamps) iptr64 = &iptr64[M]; // advance past timestamps

		uint64_t run_number_seed = (*iptr64)>>32;
		if(VERBOSE>1) evioout << " .. (epic quest) Found run number: " << run_number_seed <<endl;

		if(hdevio) delete hdevio;
		if(buff) delete[] buff;
		return run_number_seed;

	} // while(iptr<iend)

	if(hdevio->Nevents > 500) break;
	} // while(hdevio->read(buff, buff_len))

	if(hdevio) delete hdevio;
	if(buff) delete[] buff;

	return 0;
}

//----------------
// EmulateDf250Firmware
//----------------
void JEventSource_EVIOpp::EmulateDf250Firmware(DParsedEvent *pe)
{
	/// This is called from GetObjects, but only if window raw
	/// data objects exist. It will emulate the firmware, producing
	/// hit objects from the sample data.
	///
	/// There are some complications here worth noting regarding the
	/// behavior of the firmware used for Spring 2016 and earlier
	/// data. The firmware will actually look for a pulse in the
	/// window and if found, emit pulse time and pulse pedestal
	/// words for it. It will NOT emit a pulse integral word. Further,
	/// it will produce those words for at most one pulse. If running
	/// in mode 7 however, (i.e. no window raw data) it will produce
	/// multiple pulses. Thus, to mimic the behavior of mode 7, the
	/// emulator will produce multiple pulses. 
	///
	/// The exact behavior of this is determined by the F250_EMULATION_MODE
	/// parameter which may be one of 3 values. The default is
	/// kEmulationAuto which does the following:
	/// 1. Produce hit objects for all pulses found in window
	/// 2. If pulse time and pedestal objects exist from firmware
	///    then copy the first found hit into the "*_emulated"
	///    fields of those existing objects and discard the emulated
	///    hit.
	/// 3. Add the remaining emulated hits to the event.
	///
	/// The end result is that the first pulse time and pulse pedestal
	/// objects may contain some combination of emulated and firmware
	/// generated information. All pulse integral objects will be
	/// emulated. It may also be worth noting that this is done in such
	/// a way that the data may contain a mixture of mode 8 data 
	/// from some boards and mode 7 from others. It will not affect
	/// those boards producing mode 7 (e.g. won't delete their hits
	/// even if F250_EMULATION_MODE is set to kEmulationAlways).

	if(F250_EMULATION_MODE == kEmulationNone) return;

	for(auto wrd : pe->vDf250WindowRawData){
		const Df250PulseTime     *cf250PulseTime     = NULL;
		const Df250PulsePedestal *cf250PulsePedestal = NULL;

		try{ wrd->GetSingle(cf250PulseTime);     }catch(...){}
		try{ wrd->GetSingle(cf250PulsePedestal); }catch(...){}

		Df250PulseTime     *f250PulseTime     = (Df250PulseTime*    )cf250PulseTime;
		Df250PulsePedestal *f250PulsePedestal = (Df250PulsePedestal*)cf250PulsePedestal;

		// Emulate firmware
		vector<Df250PulseTime*>     em_pts;
		vector<Df250PulsePedestal*> em_pps;
		vector<Df250PulseIntegral*> em_pis;
		f250Emulator->EmulateFirmware(wrd, em_pts, em_pps, em_pis);
		
		// Spring 2016 and earlier data may have one pulse time and
		// one pedestal object in mode 8 data. Emulation mimics mode
		// 7 which may have more. If there are firmware generated 
		// pulse time/pedestal objects, copy the first emulated hit's
		// info into them and then the rest of the emulated hits into
		// appropriate lists.
		if(!em_pts.empty() && f250PulseTime){
			Df250PulseTime *em_pt = em_pts[0];
			f250PulseTime->time_emulated = em_pt->time_emulated;
			f250PulseTime->quality_factor_emulated = em_pt->quality_factor_emulated;
			if(F250_EMULATION_MODE == kEmulationAlways){
				f250PulseTime->time = em_pt->time;
				f250PulseTime->quality_factor = em_pt->quality_factor;
				f250PulseTime->emulated = true;
			}
			delete em_pt;
			em_pts.erase(em_pts.begin());
		}

		if(!em_pps.empty() && f250PulsePedestal){
			Df250PulsePedestal *em_pp = em_pps[0];
			f250PulsePedestal->pedestal_emulated   = em_pp->pedestal_emulated;
			f250PulsePedestal->pulse_peak_emulated = em_pp->pulse_peak_emulated;
			if(F250_EMULATION_MODE == kEmulationAlways){
				f250PulsePedestal->pedestal   = em_pp->pedestal_emulated;
				f250PulsePedestal->pulse_peak = em_pp->pulse_peak_emulated;
				f250PulsePedestal->emulated = true;
			}
			delete em_pp;
			em_pps.erase(em_pps.begin());
		}

		pe->vDf250PulseTime.insert(pe->vDf250PulseTime.end(),         em_pts.begin(), em_pts.end());
		pe->vDf250PulsePedestal.insert(pe->vDf250PulsePedestal.end(), em_pps.begin(), em_pps.end());
		pe->vDf250PulseIntegral.insert(pe->vDf250PulseIntegral.end(), em_pis.begin(), em_pis.end());
	}
	
	// One last bit of nastiness here. Because the emulator creates
	// new objects rather than pulling them from the DParsedEvent pools,
	// the pools will tend to get filled with them. Delete any pool
	// objects for the types the emulator creates to prevent memory-
	// leak-like behavior.
	for(auto p : pe->vDf250PulseTime_pool    ) delete p;
	for(auto p : pe->vDf250PulsePedestal_pool) delete p;
	for(auto p : pe->vDf250PulseIntegral_pool) delete p;
	pe->vDf250PulseTime_pool.clear();
	pe->vDf250PulsePedestal_pool.clear();
	pe->vDf250PulseIntegral_pool.clear();
	
}

//----------------
// EmulateDf125Firmware
//----------------
void JEventSource_EVIOpp::EmulateDf125Firmware(DParsedEvent *pe)
{
	/// This is called from GetObjects, but only if window raw
	/// data objects exist. It will emulate the firmware, producing
	/// hit objects from the sample data.

	if(F125_EMULATION_MODE == kEmulationNone) return;

	for(auto wrd : pe->vDf125WindowRawData){
		const Df125CDCPulse *cf125CDCPulse = NULL;
		const Df125FDCPulse *cf125FDCPulse = NULL;

		try{ wrd->GetSingle(cf125CDCPulse); }catch(...){}
		try{ wrd->GetSingle(cf125FDCPulse); }catch(...){}

		Df125CDCPulse *f125CDCPulse = (Df125CDCPulse*)cf125CDCPulse;
		Df125FDCPulse *f125FDCPulse = (Df125FDCPulse*)cf125FDCPulse;

		// If the the pulse objects do not exist, create new ones to go with our raw data
		// This should rarely happen since CDC_long and FDC_long have the raw data
		// along with the calculated quantities in a pulse word. Pure raw mode would be the only time
		// when this would not be the case. Since this is so infrequently used (if ever), 
		// the ROCID check for CDC/FDC determination is hard coded...
		// ROCID CDC: 25-28
		// ROCID FDC Cathode: 52,53,55-62

		if(f125CDCPulse == NULL && ( wrd->rocid < 30 ) ){
			f125CDCPulse           = pe->NEW_Df125CDCPulse();
			f125CDCPulse->rocid    = wrd->rocid;
			f125CDCPulse->slot     = wrd->slot;
			f125CDCPulse->channel  = wrd->channel;
			f125CDCPulse->emulated = true;
			f125CDCPulse->AddAssociatedObject(wrd);
		}

		else if(f125FDCPulse == NULL && ( wrd->rocid > 30 ) ){
			f125FDCPulse           = pe->NEW_Df125FDCPulse();
			f125FDCPulse->rocid    = wrd->rocid;
			f125FDCPulse->slot     = wrd->slot;
			f125FDCPulse->channel  = wrd->channel;
			f125FDCPulse->emulated = true;
			f125FDCPulse->AddAssociatedObject(wrd);
		}

		// Flag all objects as emulated and their values will be replaced with emulated quantities
		if (F125_EMULATION_MODE == kEmulationAlways){
			if(f125CDCPulse!=NULL) f125CDCPulse->emulated = 1;
			if(f125FDCPulse!=NULL) f125FDCPulse->emulated = 1;
		}

		// Perform the emulation
		f125Emulator->EmulateFirmware(wrd, f125CDCPulse, f125FDCPulse);
	}
}
