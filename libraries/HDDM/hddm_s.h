/*
 * hddm_s.h - DO NOT EDIT THIS FILE
 *
 * This file was generated automatically by hddm-c from the file
 * event.xml

 * This header file defines the c structures that hold the data
 * described in the data model (from event.xml). 
 *
 * The hddm data model tool set was written by
 * Richard Jones, University of Connecticut.
 *
 * For more information see the following web site
 *
 * http://zeus.phys.uconn.edu/halld/datamodel/doc
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <rpc/rpc.h>
#include <string.h>
#include <strings.h>
#include <particleType.h>

/* Note to users: The option MALLOC_FREE_WITH_MEMCHECK
 * was created for debugging this hddm library, but it
 * is also useful for finding memory leaks in user
 * code.  To use it, replace malloc(n) everywhere in
 * your code with MALLOC(n,"some descriptive string")
 * and free(p) with FREE(p) and include this header
 * and compile with -DMALLOC_FREE_WITH_MEMCHECK set.
 * Any attempt to malloc memory already malloc'ed or
 * to free memory that has not yet been malloc'ed is
 * immediately flagged with an error message.  A call
 * to checkpoint() anywhere in the user code reports
 * any memory that has been malloc'ed not freed.
 */
#if defined MALLOC_FREE_WITH_MEMCHECK
#  include <memcheck.h>
#  define MALLOC(N,S) (checkin(malloc(N),S))
#  define FREE(P) (checkout(P),free(P))
#else
#  define MALLOC(N,S) malloc(N)
#  define FREE(P) free(P)
#endif

#ifndef SAW_s_Momentum_t
#define SAW_s_Momentum_t

typedef struct {
   float                E;
   float                px;
   float                py;
   float                pz;
} s_Momentum_t;
#endif /* s_Momentum_t */

#ifndef SAW_s_Properties_t
#define SAW_s_Properties_t

typedef struct {
   int32_t              charge;
   float                mass;
} s_Properties_t;
#endif /* s_Properties_t */

#ifndef SAW_s_Beam_t
#define SAW_s_Beam_t

typedef struct {
   Particle_t           type;
   s_Momentum_t*        momentum;
   s_Properties_t*      properties;
} s_Beam_t;
#endif /* s_Beam_t */

#ifndef SAW_s_Target_t
#define SAW_s_Target_t

typedef struct {
   Particle_t           type;
   s_Momentum_t*        momentum;
   s_Properties_t*      properties;
} s_Target_t;
#endif /* s_Target_t */

#ifndef SAW_s_Product_t
#define SAW_s_Product_t

typedef struct {
   int32_t              decayVertex;
   int32_t              id;
   int32_t              mech;
   int32_t              parentid;
   int32_t              pdgtype;
   Particle_t           type;
   s_Momentum_t*        momentum;
   s_Properties_t*      properties;
} s_Product_t;

typedef struct {
   unsigned int mult;
   s_Product_t in[1];
} s_Products_t;
#endif /* s_Product_t */

#ifndef SAW_s_Origin_t
#define SAW_s_Origin_t

typedef struct {
   float                t;
   float                vx;
   float                vy;
   float                vz;
} s_Origin_t;
#endif /* s_Origin_t */

#ifndef SAW_s_Vertex_t
#define SAW_s_Vertex_t

typedef struct {
   s_Products_t*        products;
   s_Origin_t*          origin;
} s_Vertex_t;

typedef struct {
   unsigned int mult;
   s_Vertex_t in[1];
} s_Vertices_t;
#endif /* s_Vertex_t */

#ifndef SAW_s_Reaction_t
#define SAW_s_Reaction_t

typedef struct {
   int32_t              type;
   float                weight;
   s_Beam_t*            beam;
   s_Target_t*          target;
   s_Vertices_t*        vertices;
} s_Reaction_t;

typedef struct {
   unsigned int mult;
   s_Reaction_t in[1];
} s_Reactions_t;
#endif /* s_Reaction_t */

#ifndef SAW_s_CdcStrawHit_t
#define SAW_s_CdcStrawHit_t

typedef struct {
   float                dE;
   float                t;
} s_CdcStrawHit_t;

typedef struct {
   unsigned int mult;
   s_CdcStrawHit_t in[1];
} s_CdcStrawHits_t;
#endif /* s_CdcStrawHit_t */

#ifndef SAW_s_CdcStraw_t
#define SAW_s_CdcStraw_t

typedef struct {
   int32_t              ring;
   int32_t              straw;
   s_CdcStrawHits_t*    cdcStrawHits;
} s_CdcStraw_t;

typedef struct {
   unsigned int mult;
   s_CdcStraw_t in[1];
} s_CdcStraws_t;
#endif /* s_CdcStraw_t */

#ifndef SAW_s_CdcTruthPoint_t
#define SAW_s_CdcTruthPoint_t

typedef struct {
   float                dEdx;
   float                dradius;
   float                phi;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                r;
   float                t;
   int32_t              track;
   float                z;
} s_CdcTruthPoint_t;

typedef struct {
   unsigned int mult;
   s_CdcTruthPoint_t in[1];
} s_CdcTruthPoints_t;
#endif /* s_CdcTruthPoint_t */

#ifndef SAW_s_CentralDC_t
#define SAW_s_CentralDC_t

typedef struct {
   s_CdcStraws_t*       cdcStraws;
   s_CdcTruthPoints_t*  cdcTruthPoints;
} s_CentralDC_t;
#endif /* s_CentralDC_t */

#ifndef SAW_s_FdcAnodeHit_t
#define SAW_s_FdcAnodeHit_t

typedef struct {
   float                dE;
   float                t;
} s_FdcAnodeHit_t;

typedef struct {
   unsigned int mult;
   s_FdcAnodeHit_t in[1];
} s_FdcAnodeHits_t;
#endif /* s_FdcAnodeHit_t */

#ifndef SAW_s_FdcAnodeWire_t
#define SAW_s_FdcAnodeWire_t

typedef struct {
   int32_t              wire;
   s_FdcAnodeHits_t*    fdcAnodeHits;
} s_FdcAnodeWire_t;

typedef struct {
   unsigned int mult;
   s_FdcAnodeWire_t in[1];
} s_FdcAnodeWires_t;
#endif /* s_FdcAnodeWire_t */

#ifndef SAW_s_FdcCathodeHit_t
#define SAW_s_FdcCathodeHit_t

typedef struct {
   float                q;
   float                t;
} s_FdcCathodeHit_t;

typedef struct {
   unsigned int mult;
   s_FdcCathodeHit_t in[1];
} s_FdcCathodeHits_t;
#endif /* s_FdcCathodeHit_t */

#ifndef SAW_s_FdcCathodeStrip_t
#define SAW_s_FdcCathodeStrip_t

typedef struct {
   int32_t              plane;
   int32_t              strip;
   s_FdcCathodeHits_t*  fdcCathodeHits;
} s_FdcCathodeStrip_t;

typedef struct {
   unsigned int mult;
   s_FdcCathodeStrip_t in[1];
} s_FdcCathodeStrips_t;
#endif /* s_FdcCathodeStrip_t */

#ifndef SAW_s_FdcTruthPoint_t
#define SAW_s_FdcTruthPoint_t

typedef struct {
   float                E;
   float                dEdx;
   float                dradius;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_FdcTruthPoint_t;

typedef struct {
   unsigned int mult;
   s_FdcTruthPoint_t in[1];
} s_FdcTruthPoints_t;
#endif /* s_FdcTruthPoint_t */

#ifndef SAW_s_FdcChamber_t
#define SAW_s_FdcChamber_t

typedef struct {
   int32_t              layer;
   int32_t              module;
   s_FdcAnodeWires_t*   fdcAnodeWires;
   s_FdcCathodeStrips_t* fdcCathodeStrips;
   s_FdcTruthPoints_t*  fdcTruthPoints;
} s_FdcChamber_t;

typedef struct {
   unsigned int mult;
   s_FdcChamber_t in[1];
} s_FdcChambers_t;
#endif /* s_FdcChamber_t */

#ifndef SAW_s_ForwardDC_t
#define SAW_s_ForwardDC_t

typedef struct {
   s_FdcChambers_t*     fdcChambers;
} s_ForwardDC_t;
#endif /* s_ForwardDC_t */

#ifndef SAW_s_StcHit_t
#define SAW_s_StcHit_t

typedef struct {
   float                dE;
   float                t;
} s_StcHit_t;

typedef struct {
   unsigned int mult;
   s_StcHit_t in[1];
} s_StcHits_t;
#endif /* s_StcHit_t */

#ifndef SAW_s_StcPaddle_t
#define SAW_s_StcPaddle_t

typedef struct {
   int32_t              sector;
   s_StcHits_t*         stcHits;
} s_StcPaddle_t;

typedef struct {
   unsigned int mult;
   s_StcPaddle_t in[1];
} s_StcPaddles_t;
#endif /* s_StcPaddle_t */

#ifndef SAW_s_StcTruthPoint_t
#define SAW_s_StcTruthPoint_t

typedef struct {
   float                E;
   float                dEdx;
   float                phi;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                r;
   float                t;
   int32_t              track;
   float                z;
} s_StcTruthPoint_t;

typedef struct {
   unsigned int mult;
   s_StcTruthPoint_t in[1];
} s_StcTruthPoints_t;
#endif /* s_StcTruthPoint_t */

#ifndef SAW_s_StartCntr_t
#define SAW_s_StartCntr_t

typedef struct {
   s_StcPaddles_t*      stcPaddles;
   s_StcTruthPoints_t*  stcTruthPoints;
} s_StartCntr_t;
#endif /* s_StartCntr_t */

#ifndef SAW_s_BcalHit_t
#define SAW_s_BcalHit_t

typedef struct {
   float                E;
   float                t;
   float                zLocal;
} s_BcalHit_t;

typedef struct {
   unsigned int mult;
   s_BcalHit_t in[1];
} s_BcalHits_t;
#endif /* s_BcalHit_t */

#ifndef SAW_s_BcalCell_t
#define SAW_s_BcalCell_t

typedef struct {
   int32_t              layer;
   int32_t              module;
   int32_t              sector;
   s_BcalHits_t*        bcalHits;
} s_BcalCell_t;

typedef struct {
   unsigned int mult;
   s_BcalCell_t in[1];
} s_BcalCells_t;
#endif /* s_BcalCell_t */

#ifndef SAW_s_BcalTruthShower_t
#define SAW_s_BcalTruthShower_t

typedef struct {
   float                E;
   float                phi;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                r;
   float                t;
   int32_t              track;
   float                z;
} s_BcalTruthShower_t;

typedef struct {
   unsigned int mult;
   s_BcalTruthShower_t in[1];
} s_BcalTruthShowers_t;
#endif /* s_BcalTruthShower_t */

#ifndef SAW_s_BarrelEMcal_t
#define SAW_s_BarrelEMcal_t

typedef struct {
   s_BcalCells_t*       bcalCells;
   s_BcalTruthShowers_t* bcalTruthShowers;
} s_BarrelEMcal_t;
#endif /* s_BarrelEMcal_t */

#ifndef SAW_s_CereHit_t
#define SAW_s_CereHit_t

typedef struct {
   float                pe;
   float                t;
} s_CereHit_t;

typedef struct {
   unsigned int mult;
   s_CereHit_t in[1];
} s_CereHits_t;
#endif /* s_CereHit_t */

#ifndef SAW_s_CereSection_t
#define SAW_s_CereSection_t

typedef struct {
   int32_t              sector;
   s_CereHits_t*        cereHits;
} s_CereSection_t;

typedef struct {
   unsigned int mult;
   s_CereSection_t in[1];
} s_CereSections_t;
#endif /* s_CereSection_t */

#ifndef SAW_s_CereTruthPoint_t
#define SAW_s_CereTruthPoint_t

typedef struct {
   float                E;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_CereTruthPoint_t;

typedef struct {
   unsigned int mult;
   s_CereTruthPoint_t in[1];
} s_CereTruthPoints_t;
#endif /* s_CereTruthPoint_t */

#ifndef SAW_s_Cerenkov_t
#define SAW_s_Cerenkov_t

typedef struct {
   s_CereSections_t*    cereSections;
   s_CereTruthPoints_t* cereTruthPoints;
} s_Cerenkov_t;
#endif /* s_Cerenkov_t */

#ifndef SAW_s_FtofNorthHit_t
#define SAW_s_FtofNorthHit_t

typedef struct {
   float                E;
   float                dE;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   float                x;
   float                y;
   float                z;
} s_FtofNorthHit_t;

typedef struct {
   unsigned int mult;
   s_FtofNorthHit_t in[1];
} s_FtofNorthHits_t;
#endif /* s_FtofNorthHit_t */

#ifndef SAW_s_FtofSouthHit_t
#define SAW_s_FtofSouthHit_t

typedef struct {
   float                E;
   float                dE;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   float                x;
   float                y;
   float                z;
} s_FtofSouthHit_t;

typedef struct {
   unsigned int mult;
   s_FtofSouthHit_t in[1];
} s_FtofSouthHits_t;
#endif /* s_FtofSouthHit_t */

#ifndef SAW_s_FtofCounter_t
#define SAW_s_FtofCounter_t

typedef struct {
   int32_t              bar;
   int32_t              plane;
   s_FtofNorthHits_t*   ftofNorthHits;
   s_FtofSouthHits_t*   ftofSouthHits;
} s_FtofCounter_t;

typedef struct {
   unsigned int mult;
   s_FtofCounter_t in[1];
} s_FtofCounters_t;
#endif /* s_FtofCounter_t */

#ifndef SAW_s_FtofTruthPoint_t
#define SAW_s_FtofTruthPoint_t

typedef struct {
   float                E;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_FtofTruthPoint_t;

typedef struct {
   unsigned int mult;
   s_FtofTruthPoint_t in[1];
} s_FtofTruthPoints_t;
#endif /* s_FtofTruthPoint_t */

#ifndef SAW_s_ForwardTOF_t
#define SAW_s_ForwardTOF_t

typedef struct {
   s_FtofCounters_t*    ftofCounters;
   s_FtofTruthPoints_t* ftofTruthPoints;
} s_ForwardTOF_t;
#endif /* s_ForwardTOF_t */

#ifndef SAW_s_FcalHit_t
#define SAW_s_FcalHit_t

typedef struct {
   float                E;
   float                t;
} s_FcalHit_t;

typedef struct {
   unsigned int mult;
   s_FcalHit_t in[1];
} s_FcalHits_t;
#endif /* s_FcalHit_t */

#ifndef SAW_s_FcalBlock_t
#define SAW_s_FcalBlock_t

typedef struct {
   int32_t              column;
   int32_t              row;
   s_FcalHits_t*        fcalHits;
} s_FcalBlock_t;

typedef struct {
   unsigned int mult;
   s_FcalBlock_t in[1];
} s_FcalBlocks_t;
#endif /* s_FcalBlock_t */

#ifndef SAW_s_FcalTruthShower_t
#define SAW_s_FcalTruthShower_t

typedef struct {
   float                E;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_FcalTruthShower_t;

typedef struct {
   unsigned int mult;
   s_FcalTruthShower_t in[1];
} s_FcalTruthShowers_t;
#endif /* s_FcalTruthShower_t */

#ifndef SAW_s_ForwardEMcal_t
#define SAW_s_ForwardEMcal_t

typedef struct {
   s_FcalBlocks_t*      fcalBlocks;
   s_FcalTruthShowers_t* fcalTruthShowers;
} s_ForwardEMcal_t;
#endif /* s_ForwardEMcal_t */

#ifndef SAW_s_UpvLeftHit_t
#define SAW_s_UpvLeftHit_t

typedef struct {
   float                E;
   float                t;
} s_UpvLeftHit_t;

typedef struct {
   unsigned int mult;
   s_UpvLeftHit_t in[1];
} s_UpvLeftHits_t;
#endif /* s_UpvLeftHit_t */

#ifndef SAW_s_UpvRightHit_t
#define SAW_s_UpvRightHit_t

typedef struct {
   float                E;
   float                t;
} s_UpvRightHit_t;

typedef struct {
   unsigned int mult;
   s_UpvRightHit_t in[1];
} s_UpvRightHits_t;
#endif /* s_UpvRightHit_t */

#ifndef SAW_s_UpvPaddle_t
#define SAW_s_UpvPaddle_t

typedef struct {
   int32_t              layer;
   int32_t              row;
   s_UpvLeftHits_t*     upvLeftHits;
   s_UpvRightHits_t*    upvRightHits;
} s_UpvPaddle_t;

typedef struct {
   unsigned int mult;
   s_UpvPaddle_t in[1];
} s_UpvPaddles_t;
#endif /* s_UpvPaddle_t */

#ifndef SAW_s_UpvTruthShower_t
#define SAW_s_UpvTruthShower_t

typedef struct {
   float                E;
   bool_t               primary;
   int32_t              ptype;
   float                px;
   float                py;
   float                pz;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_UpvTruthShower_t;

typedef struct {
   unsigned int mult;
   s_UpvTruthShower_t in[1];
} s_UpvTruthShowers_t;
#endif /* s_UpvTruthShower_t */

#ifndef SAW_s_UpstreamEMveto_t
#define SAW_s_UpstreamEMveto_t

typedef struct {
   s_UpvPaddles_t*      upvPaddles;
   s_UpvTruthShowers_t* upvTruthShowers;
} s_UpstreamEMveto_t;
#endif /* s_UpstreamEMveto_t */

#ifndef SAW_s_TaggerHit_t
#define SAW_s_TaggerHit_t

typedef struct {
   float                t;
} s_TaggerHit_t;

typedef struct {
   unsigned int mult;
   s_TaggerHit_t in[1];
} s_TaggerHits_t;
#endif /* s_TaggerHit_t */

#ifndef SAW_s_MicroChannel_t
#define SAW_s_MicroChannel_t

typedef struct {
   float                E;
   int32_t              column;
   int32_t              row;
   s_TaggerHits_t*      taggerHits;
} s_MicroChannel_t;

typedef struct {
   unsigned int mult;
   s_MicroChannel_t in[1];
} s_MicroChannels_t;
#endif /* s_MicroChannel_t */

#ifndef SAW_s_Tagger_t
#define SAW_s_Tagger_t

typedef struct {
   s_MicroChannels_t*   microChannels;
} s_Tagger_t;
#endif /* s_Tagger_t */

#ifndef SAW_s_McTrajectoryPoint_t
#define SAW_s_McTrajectoryPoint_t

typedef struct {
   float                E;
   float                dE;
   int32_t              mech;
   int32_t              part;
   int32_t              primary_track;
   float                px;
   float                py;
   float                pz;
   float                radlen;
   float                step;
   float                t;
   int32_t              track;
   float                x;
   float                y;
   float                z;
} s_McTrajectoryPoint_t;

typedef struct {
   unsigned int mult;
   s_McTrajectoryPoint_t in[1];
} s_McTrajectoryPoints_t;
#endif /* s_McTrajectoryPoint_t */

#ifndef SAW_s_McTrajectory_t
#define SAW_s_McTrajectory_t

typedef struct {
   s_McTrajectoryPoints_t* mcTrajectoryPoints;
} s_McTrajectory_t;
#endif /* s_McTrajectory_t */

#ifndef SAW_s_HitView_t
#define SAW_s_HitView_t

typedef struct {
   s_CentralDC_t*       centralDC;
   s_ForwardDC_t*       forwardDC;
   s_StartCntr_t*       startCntr;
   s_BarrelEMcal_t*     barrelEMcal;
   s_Cerenkov_t*        Cerenkov;
   s_ForwardTOF_t*      forwardTOF;
   s_ForwardEMcal_t*    forwardEMcal;
   s_UpstreamEMveto_t*  upstreamEMveto;
   s_Tagger_t*          tagger;
   s_McTrajectory_t*    mcTrajectory;
} s_HitView_t;
#endif /* s_HitView_t */

#ifndef SAW_s_PhysicsEvent_t
#define SAW_s_PhysicsEvent_t

typedef struct {
   int32_t              eventNo;
   int32_t              runNo;
   s_Reactions_t*       reactions;
   s_HitView_t*         hitView;
} s_PhysicsEvent_t;

typedef struct {
   unsigned int mult;
   s_PhysicsEvent_t in[1];
} s_PhysicsEvents_t;
#endif /* s_PhysicsEvent_t */

#ifndef SAW_s_HDDM_t
#define SAW_s_HDDM_t

typedef struct {
   s_PhysicsEvents_t*   physicsEvents;
} s_HDDM_t;
#endif /* s_HDDM_t */

#ifdef __cplusplus
extern "C" {
#endif

s_HDDM_t* make_s_HDDM();

s_PhysicsEvents_t* make_s_PhysicsEvents(int n);

s_Reactions_t* make_s_Reactions(int n);

s_Beam_t* make_s_Beam();

s_Momentum_t* make_s_Momentum();

s_Properties_t* make_s_Properties();

s_Target_t* make_s_Target();

s_Vertices_t* make_s_Vertices(int n);

s_Products_t* make_s_Products(int n);

s_Origin_t* make_s_Origin();

s_HitView_t* make_s_HitView();

s_CentralDC_t* make_s_CentralDC();

s_CdcStraws_t* make_s_CdcStraws(int n);

s_CdcStrawHits_t* make_s_CdcStrawHits(int n);

s_CdcTruthPoints_t* make_s_CdcTruthPoints(int n);

s_ForwardDC_t* make_s_ForwardDC();

s_FdcChambers_t* make_s_FdcChambers(int n);

s_FdcAnodeWires_t* make_s_FdcAnodeWires(int n);

s_FdcAnodeHits_t* make_s_FdcAnodeHits(int n);

s_FdcCathodeStrips_t* make_s_FdcCathodeStrips(int n);

s_FdcCathodeHits_t* make_s_FdcCathodeHits(int n);

s_FdcTruthPoints_t* make_s_FdcTruthPoints(int n);

s_StartCntr_t* make_s_StartCntr();

s_StcPaddles_t* make_s_StcPaddles(int n);

s_StcHits_t* make_s_StcHits(int n);

s_StcTruthPoints_t* make_s_StcTruthPoints(int n);

s_BarrelEMcal_t* make_s_BarrelEMcal();

s_BcalCells_t* make_s_BcalCells(int n);

s_BcalHits_t* make_s_BcalHits(int n);

s_BcalTruthShowers_t* make_s_BcalTruthShowers(int n);

s_Cerenkov_t* make_s_Cerenkov();

s_CereSections_t* make_s_CereSections(int n);

s_CereHits_t* make_s_CereHits(int n);

s_CereTruthPoints_t* make_s_CereTruthPoints(int n);

s_ForwardTOF_t* make_s_ForwardTOF();

s_FtofCounters_t* make_s_FtofCounters(int n);

s_FtofNorthHits_t* make_s_FtofNorthHits(int n);

s_FtofSouthHits_t* make_s_FtofSouthHits(int n);

s_FtofTruthPoints_t* make_s_FtofTruthPoints(int n);

s_ForwardEMcal_t* make_s_ForwardEMcal();

s_FcalBlocks_t* make_s_FcalBlocks(int n);

s_FcalHits_t* make_s_FcalHits(int n);

s_FcalTruthShowers_t* make_s_FcalTruthShowers(int n);

s_UpstreamEMveto_t* make_s_UpstreamEMveto();

s_UpvPaddles_t* make_s_UpvPaddles(int n);

s_UpvLeftHits_t* make_s_UpvLeftHits(int n);

s_UpvRightHits_t* make_s_UpvRightHits(int n);

s_UpvTruthShowers_t* make_s_UpvTruthShowers(int n);

s_Tagger_t* make_s_Tagger();

s_MicroChannels_t* make_s_MicroChannels(int n);

s_TaggerHits_t* make_s_TaggerHits(int n);

s_McTrajectory_t* make_s_McTrajectory();

s_McTrajectoryPoints_t* make_s_McTrajectoryPoints(int n);

#ifdef __cplusplus
}
#endif

#ifndef s_DocumentString
#define s_DocumentString

extern char HDDM_s_DocumentString[];

#ifdef INLINE_PREPEND_UNDERSCORES
#define inline __inline
#endif

#endif /* s_DocumentString */

#ifndef HDDM_STREAM_INPUT
#define HDDM_STREAM_INPUT -91
#define HDDM_STREAM_OUTPUT -92

struct popNode_s {
   void* (*unpacker)(XDR*, struct popNode_s*);
   int inParent;
   int popListLength;
   struct popNode_s* popList[99];
};
typedef struct popNode_s popNode;

typedef struct {
   FILE* fd;
   int iomode;
   char* filename;
   XDR* xdrs;
   popNode* popTop;
} s_iostream_t;

#endif /* HDDM_STREAM_INPUT */

#ifdef __cplusplus
extern "C" {
#endif

s_HDDM_t* read_s_HDDM(s_iostream_t* fp);

int skip_s_HDDM(s_iostream_t* fp, int nskip);

int flush_s_HDDM(s_HDDM_t* this1,s_iostream_t* fp);

s_iostream_t* open_s_HDDM(char* filename);

s_iostream_t* init_s_HDDM(char* filename);

void close_s_HDDM(s_iostream_t* fp);

#ifdef __cplusplus
}
#endif

#if !defined HDDM_NULL
extern int hddm_nullTarget;
# define HDDM_NULL (void*)&hddm_nullTarget
#endif
