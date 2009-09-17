// $Id: DEventProcessor_trackeff_hists.cc 2774 2007-07-19 15:59:02Z davidl $
//
//    File: DEventProcessor_trackeff_hists.cc
// Created: Sun Apr 24 06:45:21 EDT 2005
// Creator: davidl (on Darwin Harriet.local 7.8.0 powerpc)
//

#include <iostream>
#include <cmath>
using namespace std;

#include <TThread.h>

#include <TROOT.h>

#include "DEventProcessor_trackeff_hists.h"

#include <JANA/JApplication.h>
#include <JANA/JEventLoop.h>

#include <DANA/DApplication.h>
#include <TRACKING/DMCThrown.h>
#include <TRACKING/DTrackCandidate.h>
#include <TRACKING/DTrack.h>
#include <TRACKING/DMCTrajectoryPoint.h>
#include <PID/DParticle.h>
#include <DVector2.h>
#include <particleType.h>


// Routine used to create our DEventProcessor
extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new DEventProcessor_trackeff_hists());
}
} // "C"


//------------------
// DEventProcessor_trackeff_hists
//------------------
DEventProcessor_trackeff_hists::DEventProcessor_trackeff_hists()
{
	trk_ptr = &trk;
	MAX_TRACKS = 10;
	
	trkeff = NULL;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&rt_mutex, NULL);
}

//------------------
// ~DEventProcessor_trackeff_hists
//------------------
DEventProcessor_trackeff_hists::~DEventProcessor_trackeff_hists()
{

}

//------------------
// init
//------------------
jerror_t DEventProcessor_trackeff_hists::init(void)
{
	pthread_mutex_lock(&mutex);

	// Create TRACKING directory
	TDirectory *dir = (TDirectory*)gROOT->FindObject("TRACKING");
	if(!dir)dir = new TDirectoryFile("TRACKING","TRACKING");
	dir->cd();

	// Create Tree
	if(!trkeff){
		trkeff = new TTree("trkeff","Tracking Efficiency");
		trkeff->Branch("F","track",&trk_ptr);
	}

	dir->cd("../");

	pthread_mutex_unlock(&mutex);
		
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t DEventProcessor_trackeff_hists::brun(JEventLoop *loop, int runnumber)
{
	
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t DEventProcessor_trackeff_hists::erun(void)
{

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DEventProcessor_trackeff_hists::fini(void)
{
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DEventProcessor_trackeff_hists::evnt(JEventLoop *loop, int eventnumber)
{
	vector<const DCDCTrackHit*> cdctrackhits;
	vector<const DFDCPseudo*> fdcpseudos;
	vector<const DTrackCandidate*> trackcandidates;
	vector<const DTrack*> tracks;
	vector<const DParticle*> particles;
	vector<const DParticle*> throwns;
	vector<const DMCTrajectoryPoint*> mctraj;
	
	loop->Get(cdctrackhits);
	loop->Get(fdcpseudos);
	loop->Get(trackcandidates);
	loop->Get(tracks);
	loop->Get(particles);
	loop->Get(throwns, "THROWN");
	loop->Get(mctraj);

	// 1. Get number of CDC/FDC wires for each primary track
	// 2. Get number of CDC/FDC wires and track number for a given DKinematicData object
	// 3. Get number of DKinematicData objects of same type associated with each track number
	
	// Get track info and track number for each reconstructed track
	vector<track_info> ti_can(MAX_TRACKS);
	vector<track_info> ti_trk(MAX_TRACKS);
	vector<track_info> ti_prt(MAX_TRACKS);
	for(unsigned int i=0; i<trackcandidates.size(); i++)FillTrackInfo(trackcandidates[i], ti_can);
	for(unsigned int i=0; i<tracks.size(); i++)FillTrackInfo(tracks[i], ti_trk);
	for(unsigned int i=0; i<particles.size(); i++)FillTrackInfo(particles[i], ti_prt);

	// The highest (and therefore, most interesting) GEANT mechansim for each track in the
	// region before it gets to the BCAL.
	vector<double> dtheta_mech(MAX_TRACKS);
	vector<double> dp_mech(MAX_TRACKS);
	vector<TVector3> last_p(MAX_TRACKS, TVector3(0.0, 0.0, 0.0));
	vector<int> mech_max(MAX_TRACKS,0);
	for(unsigned int i=0; i<mctraj.size(); i++){
		int track = mctraj[i]->track;
		int mech = mctraj[i]->mech;
		double R = sqrt(pow((double)mctraj[i]->x, 2.0) + pow((double)mctraj[i]->y, 2.0));
		if(track<0 || track>=MAX_TRACKS)continue;
		if(R>60.0)continue;
		TVector3 p(mctraj[i]->px, mctraj[i]->py, mctraj[i]->pz);
		if(mech>mech_max[track]){
			mech_max[track] = mech;
			dtheta_mech[track] = p.Angle(last_p[track]);
			dp_mech[track] = (p - last_p[track]).Mag();
		}
		last_p[track] = p;
	}
	
	// Lock mutex
	pthread_mutex_lock(&mutex);

	// Loop over thrown tracks
	for(unsigned int i=0; i<throwns.size(); i++){
		const DParticle *thrown = throwns[i];

		trk.pthrown = thrown->momentum();

		// Get info for thrown track
		GetNhits(thrown, trk.Ncdc, trk.Nfdc, trk.track);
		if(trk.track<=0 || trk.track>=MAX_TRACKS)continue;

		// Copy best reconstructed track info
		trk.can  = ti_can[trk.track];
		trk.trk  = ti_trk[trk.track];
		trk.part = ti_prt[trk.track];

		// Fill tree
		trk.event = eventnumber;
		trk.mech = mech_max[trk.track];
		trk.dtheta_mech = dtheta_mech[trk.track];
		trk.dp_mech = dp_mech[trk.track];
		trkeff->Fill();
	}
	

	// Unlock mutex
	pthread_mutex_unlock(&mutex);
	
	return NOERROR;
}

//------------------
// FillTrackInfo
//------------------
void DEventProcessor_trackeff_hists::FillTrackInfo(const DKinematicData *kd,  vector<track_info> &vti)
{
	// Get track info and track number most closely matching this track
	int track_no;
	track_info ti;
	GetTrackInfo(kd, ti, track_no);
	if(track_no<0 || track_no>=MAX_TRACKS)return;
	
	// Check if this track has more wires hit than the existing track_info
	// and replace if needed.
	int Nwires = ti.Ncdc + ti.Nfdc;
	int Nwires_prev = vti[track_no].Ncdc + vti[track_no].Nfdc;
	if(Nwires > Nwires_prev)vti[track_no] = ti;
}

//------------------
// GetTrackInfo
//------------------
void DEventProcessor_trackeff_hists::GetTrackInfo(const DKinematicData *kd, track_info &ti, int &track_no)
{
	ti.p = kd->momentum();
	GetNhits(kd, ti.Ncdc, ti.Nfdc, track_no);
	
	// Try dynamic casting DKinematicData into something that can be used to get
	// at the chisq and Ndof.
	const DTrackCandidate *can = dynamic_cast<const DTrackCandidate*>(kd);
	const DTrack *track = dynamic_cast<const DTrack*>(kd);
	const DParticle *part = dynamic_cast<const DParticle*>(kd);
	if(can!=NULL){
		ti.trk_chisq = can->chisq;
		ti.trk_Ndof = can->Ndof;
	}else if(track!=NULL){
		ti.trk_chisq = track->chisq;
		ti.trk_Ndof = track->Ndof;
	}else if(part!=NULL){
		ti.trk_chisq = part->chisq;
		ti.trk_Ndof = part->Ndof;
	}else{
		ti.trk_chisq = 1.0E6;
		ti.trk_Ndof = -1;
	}
}

//------------------
// GetNhits
//------------------
void DEventProcessor_trackeff_hists::GetNhits(const DKinematicData *kd, int &Ncdc, int &Nfdc, int &track)
{
	vector<const DCDCTrackHit*> cdctrackhits;
	vector<const DFDCPseudo*> fdcpseudos;
	
	// The DKinematicData object should be a DTrackCandidate, DTrack, or DParticle which
	// has associated objects for the hits
	kd->Get(cdctrackhits);
	kd->Get(fdcpseudos);

	// The track number is buried in the truth hit objects of type DMCTrackHit. These should be 
	// associated objects for the individual hit objects. We need to loop through them and
	// keep track of how many hits for each track number we find

	// CDC hits
	vector<int> cdc_track_no(MAX_TRACKS, 0);
	for(unsigned int i=0; i<cdctrackhits.size(); i++){
		vector<const DMCTrackHit*> mctrackhits;
		cdctrackhits[i]->Get(mctrackhits);
		if(mctrackhits.size()==0)continue;
		if(!mctrackhits[0]->primary)continue;
		int track = mctrackhits[0]->track;
		if(track>=0 && track<MAX_TRACKS)cdc_track_no[track]++;
	}
	// FDC hits
	vector<int> fdc_track_no(MAX_TRACKS, 0);
	for(unsigned int i=0; i<fdcpseudos.size(); i++){
		vector<const DMCTrackHit*> mctrackhits;
		fdcpseudos[i]->Get(mctrackhits);
		if(mctrackhits.size()==0)continue;
		if(!mctrackhits[0]->primary)continue;
		int track = mctrackhits[0]->track;
		if(track>=0 && track<MAX_TRACKS)fdc_track_no[track]++;
	}
	
	// Find track number with most wires hit
	int track_with_max_hits = 0;
	int tot_hits_max = cdc_track_no[0] + fdc_track_no[0];
	for(int i=1; i<MAX_TRACKS; i++){
		int tot_hits = cdc_track_no[i] + fdc_track_no[i];
		if(tot_hits > tot_hits_max){
			track_with_max_hits=i;
			tot_hits_max = tot_hits;
		}
	}
	
	Ncdc = cdc_track_no[track_with_max_hits];
	Nfdc = fdc_track_no[track_with_max_hits];
	
	// If there are no hits on this track, then we really should report
	// a "non-track" (i.e. track=-1)
	track = tot_hits_max>0 ? track_with_max_hits:-1;
}

