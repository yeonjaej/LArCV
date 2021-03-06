#ifndef __RUN3DTRACKER_CXX__
#define __RUN3DTRACKER_CXX__

#include "Run3DTracker.h"

#include "DataFormat/storage_manager.h"
#include "DataFormat/track.h"
#include "DataFormat/vertex.h"
#include "DataFormat/hit.h"
#include "DataFormat/Image2D.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/wire.h"
#include "DataFormat/EventROI.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/TimeService.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TLine.h"
#include "TF1.h"
#include "TVector3.h"
#include "TRandom3.h"
#include "TSpline.h"

//#include "LArCV/core/DataFormat/ChStatus.h"
#include "AStarUtils.h"

#include "AStar3DAlgo.h"
#include "AStar3DAlgoProton.h"

//#include "SCE/SpaceChargeMicroBooNE.h"
#include "AStarTracker.h"


//#include "../../core/DataFormat/EventPGraph.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

#include <cassert>

namespace larcv {

    static Run3DTrackerProcessFactory __global_Run3DTrackerProcessFactory__;

    Run3DTracker::Run3DTracker(const std::string name)
    : ProcessBase(name),
    _foutll("larlite_reco3D.root"),
    _spline_file("Proton_Muon_Range_dEdx_LAr_TSplines.root"),
    _recoTree(nullptr),
    _recoTree_SCEadded(nullptr)
    {}

    void Run3DTracker::configure(const PSet& cfg)
    {
        _input_pgraph_producer     = cfg.get<std::string>("InputPgraphProducer");
        _img2d_producer            = cfg.get<std::string>("Image2DProducer");
        _par_pix_producer          = cfg.get<std::string>("ParPixelProducer");
        _true_roi_producer         = cfg.get<std::string>("TrueROIProducer");
        _mask_shower               = cfg.get<bool>("MaskShower",false);
	_anatree_name              = cfg.get<std::string>("AnaTreeName","_recoTree");
	_anatree_sce_name          = cfg.get<std::string>("AnaTreeSCEAddedName","_recoTree_SCEadded");
	_out_track_prodname        = cfg.get<std::string>("OutputTrackProducerName", "trackReco");
	_out_vertex_prodname       = cfg.get<std::string>("OutputVertexProducerName","trackReco");
	_out_assoc_prodname        = cfg.get<std::string>("OutputAssocProducerName", "trackReco");
	_out_track_sce_prodname    = cfg.get<std::string>("OutputTrackSCEAddedProducerName", "trackReco_sceadded");
	_out_assoc_sce_prodname    = cfg.get<std::string>("OutputAssocSCEAddedProducerName", "trackReco_sceadded");

	if ( _out_track_prodname==_out_track_sce_prodname ) {
	  LARCV_CRITICAL() << "Cannot name normal and SCE-added track producer the same" << std::endl;
	  throw larbys("Cannot name normal and SCE-added track producer the same");
	}
	if ( _out_assoc_prodname==_out_assoc_sce_prodname ) {
	  LARCV_CRITICAL() << "Cannot name normal and SCE-added association producer the same" << std::endl;
	  throw larbys("Cannot name normal and SCE-added association producer the same");
	}
	if ( _anatree_name==_anatree_sce_name ) {
	  LARCV_CRITICAL() << "Cannot name normal and SCE-added ana tree the same" << std::endl;
	  throw larbys("Cannot name normal and SCE-added ana tree the same");
	}

    }

    void Run3DTracker::MakeTTree(){
        _recoTree = new TTree(_anatree_name.c_str(),"_recoTree");

        _recoTree->Branch("run"     , &_run   , "_run/I");
        _recoTree->Branch("subrun"  , &_subrun, "_subrun/I");
        _recoTree->Branch("event"   , &_event , "_event/I");
        _recoTree->Branch("entry"   , &_entry , "_entry/I");

        _recoTree->Branch("vtx_id", &_vtx_id , "vtx_id/I");
        _recoTree->Branch("vtx_x" , &_vtx_x , "vtx_x/F");
        _recoTree->Branch("vtx_y" , &_vtx_y , "vtx_y/F");
        _recoTree->Branch("vtx_z" , &_vtx_z , "vtx_z/F");

        _recoTree->Branch("randomSeed",&randomSeed);
        _recoTree->Branch("trk_id_v", &_trk_id_v);
        _recoTree->Branch("NtracksReco",&NtracksReco);
        _recoTree->Branch("MinLengthAllowed",&MinLengthAllowed);
        _recoTree->Branch("trackQ3_v",&_trackQ3_v);
        _recoTree->Branch("trackQ5_v",&_trackQ5_v);
        _recoTree->Branch("trackQ10_v",&_trackQ10_v);
        _recoTree->Branch("trackQ20_v",&_trackQ20_v);
        _recoTree->Branch("trackQ30_v",&_trackQ30_v);
        _recoTree->Branch("trackQ50_v",&_trackQ50_v);

        _recoTree->Branch("E_muon_v",&_E_muon_v);
        _recoTree->Branch("E_proton_v",&_E_proton_v);
        _recoTree->Branch("Length_v",&_Length_v);
        _recoTree->Branch("Avg_Ion_v",&_Avg_Ion_v);
        _recoTree->Branch("Avg_IonY_v",&_Avg_IonY_v);
        _recoTree->Branch("Ion_5cm_v",&_Ion_5cm_v);
        _recoTree->Branch("Ion_10cm_v",&_Ion_10cm_v);
        _recoTree->Branch("Ion_tot_v",&_Ion_tot_v);
        _recoTree->Branch("IonY_5cm_v",&_IonY_5cm_v);
        _recoTree->Branch("IonY_10cm_v",&_IonY_10cm_v);
        _recoTree->Branch("IonY_tot_v",&_IonY_tot_v);
        _recoTree->Branch("Truncated_dQdX1_v",&_Trunc_dQdX1_v);
        _recoTree->Branch("Truncated_dQdX3_v",&_Trunc_dQdX3_v);
        _recoTree->Branch("IondivLength_v",&_IondivLength_v);
        _recoTree->Branch("TotalADCvalues_v",&_TotalADCvalues_v);
        _recoTree->Branch("Angle_v",&_Angle_v);
        _recoTree->Branch("Reco_goodness_v",&_Reco_goodness_v);
        _recoTree->Branch("track_Goodness_v",&_track_Goodness_v);
        _recoTree->Branch("GoodVertex",&GoodVertex);
        _recoTree->Branch("missingTrack",&_missingTrack);
        _recoTree->Branch("nothingReconstructed",&_nothingReconstructed);
        _recoTree->Branch("tooShortDeadWire",&_tooShortDeadWire);
        _recoTree->Branch("tooShortFaintTrack",&_tooShortFaintTrack);
        _recoTree->Branch("tooManyTracksAtVertex",&_tooManyTracksAtVertex);
        _recoTree->Branch("possibleCosmic",&_possibleCosmic);
        _recoTree->Branch("possiblyCrossing",&_possiblyCrossing);
        _recoTree->Branch("branchingTracks",&_branchingTracks);
        _recoTree->Branch("jumpingTracks",&_jumpingTracks);
        _recoTree->Branch("RecoVertex",&_RecoVertex);
        _recoTree->Branch("vertexPhi",&_vertexPhi);
        _recoTree->Branch("vertexTheta",&_vertexTheta);

        _recoTree->Branch("vertexPhi_2cm",&_vertexPhi_2cm);
        _recoTree->Branch("vertexTheta_2cm",&_vertexTheta_2cm);
        _recoTree->Branch("vertexPhi_5cm",&_vertexPhi_5cm);
        _recoTree->Branch("vertexTheta_5cm",&_vertexTheta_5cm);
        _recoTree->Branch("vertexPhi_7cm",&_vertexPhi_7cm);
        _recoTree->Branch("vertexTheta_7cm",&_vertexTheta_7cm);
        _recoTree->Branch("vertexPhi_10cm",&_vertexPhi_10cm);
        _recoTree->Branch("vertexTheta_10cm",&_vertexTheta_10cm);
        _recoTree->Branch("vertexPhi_12cm",&_vertexPhi_12cm);
        _recoTree->Branch("vertexTheta_12cm",&_vertexTheta_12cm);
        _recoTree->Branch("vertexPhi_15cm",&_vertexPhi_15cm);
        _recoTree->Branch("vertexTheta_15cm",&_vertexTheta_15cm);
        _recoTree->Branch("vertexPhi_17cm",&_vertexPhi_17cm);
        _recoTree->Branch("vertexTheta_17cm",&_vertexTheta_17cm);
        _recoTree->Branch("vertexPhi_20cm",&_vertexPhi_20cm);
        _recoTree->Branch("vertexTheta_20cm",&_vertexTheta_20cm);
        _recoTree->Branch("vertexPhi_30cm",&_vertexPhi_30cm);
        _recoTree->Branch("vertexTheta_30cm",&_vertexTheta_30cm);
        _recoTree->Branch("trackAvg15cm_x",&trackAvg15cm_x);
        _recoTree->Branch("trackAvg15cm_y",&trackAvg15cm_y);
        _recoTree->Branch("trackAvg15cm_z",&trackAvg15cm_z);

        _recoTree->Branch("closestWall",&_closestWall);

        _recoTree->Branch("recoEndPoints_x",&recoEndPoints_x);
        _recoTree->Branch("recoEndPoints_y",&recoEndPoints_y);
        _recoTree->Branch("recoEndPoints_z",&recoEndPoints_z);

        _recoTree->Branch("MCvertex",&MCvertex);
        _recoTree->Branch("Ep_t"       , &_Ep_t       , "Ep_t/D");
        _recoTree->Branch("Em_t"       , &_Em_t       , "Em_t/D");
        _recoTree->Branch("Ee_t"       , &_Ee_t       , "Ee_t/D");

        _recoTree->Branch("MuonStartPoint_X", &_MuonStartPoint_X, "MuonStartPoint_X/D");
        _recoTree->Branch("ProtonStartPoint_X", &_ProtonStartPoint_X, "ProtonStartPoint_X/D");
        _recoTree->Branch("ElectronStartPoint_X", &_ElectronStartPoint_X, "ElectronStartPoint_X/D");

        _recoTree->Branch("MuonStartPoint_Y", &_MuonStartPoint_Y, "MuonStartPoint_Y/D");
        _recoTree->Branch("ProtonStartPoint_Y", &_ProtonStartPoint_Y, "ProtonStartPoint_Y/D");
        _recoTree->Branch("ElectronStartPoint_Y", &_ElectronStartPoint_Y, "ElectronStartPoint_Y/D");

        _recoTree->Branch("MuonStartPoint_Z", &_MuonStartPoint_Z, "MuonStartPoint_Z/D");
        _recoTree->Branch("ProtonStartPoint_Z", &_ProtonStartPoint_Z, "ProtonStartPoint_Z/D");
        _recoTree->Branch("ElectronStartPoint_Z", &_ElectronStartPoint_Z, "ElectronStartPoint_Z/D");

        _recoTree->Branch("MuonEndPoint_X", &_MuonEndPoint_X, "MuonEndPoint_X/D");
        _recoTree->Branch("ProtonEndPoint_X", &_ProtonEndPoint_X, "ProtonEndPoint_X/D");
        _recoTree->Branch("ElectronEndPoint_X", &_ElectronEndPoint_X, "ElectronEndPoint_X/D");

        _recoTree->Branch("MuonEndPoint_Y", &_MuonEndPoint_Y, "MuonEndPoint_Y/D");
        _recoTree->Branch("ProtonEndPoint_Y", &_ProtonEndPoint_Y, "ProtonEndPoint_Y/D");
        _recoTree->Branch("ElectronEndPoint_Y", &_ElectronEndPoint_Y, "ElectronEndPoint_Y/D");

        _recoTree->Branch("MuonEndPoint_Z", &_MuonEndPoint_Z, "MuonEndPoint_Z/D");
        _recoTree->Branch("ProtonEndPoint_Z", &_ProtonEndPoint_Z, "ProtonEndPoint_Z/D");
        _recoTree->Branch("ElectronEndPoint_Z", &_ElectronEndPoint_Z, "ElectronEndPoint_Z/D");
    }

    void Run3DTracker::MakeTTree_SCEadded(){
        _recoTree_SCEadded = new TTree(_anatree_sce_name.c_str(),"_recoTree_SCEadded");

        _recoTree_SCEadded->Branch("run"     , &_run   , "_run/I");
        _recoTree_SCEadded->Branch("subrun"  , &_subrun, "_subrun/I");
        _recoTree_SCEadded->Branch("event"   , &_event , "_event/I");
        _recoTree_SCEadded->Branch("entry"   , &_entry , "_entry/I");

        _recoTree_SCEadded->Branch("vtx_id", &_vtx_id , "vtx_id/I");
        _recoTree_SCEadded->Branch("vtx_x" , &_vtx_x_sceadded , "vtx_x_sceadded/F");
        _recoTree_SCEadded->Branch("vtx_y" , &_vtx_y_sceadded , "vtx_y_sceadded/F");
        _recoTree_SCEadded->Branch("vtx_z" , &_vtx_z_sceadded , "vtx_z_sceadded/F");

        _recoTree_SCEadded->Branch("randomSeed",&randomSeed);
        _recoTree_SCEadded->Branch("trk_id_v", &_trk_id_v);
        _recoTree_SCEadded->Branch("NtracksReco",&NtracksReco_sceadded);
        _recoTree_SCEadded->Branch("MinLengthAllowed",&MinLengthAllowed);
        _recoTree_SCEadded->Branch("trackQ3_v",&_trackQ3_v_sceadded);
        _recoTree_SCEadded->Branch("trackQ5_v",&_trackQ5_v_sceadded);
        _recoTree_SCEadded->Branch("trackQ10_v",&_trackQ10_v_sceadded);
        _recoTree_SCEadded->Branch("trackQ20_v",&_trackQ20_v_sceadded);
        _recoTree_SCEadded->Branch("trackQ30_v",&_trackQ30_v_sceadded);
        _recoTree_SCEadded->Branch("trackQ50_v",&_trackQ50_v_sceadded);

        _recoTree_SCEadded->Branch("E_muon_v",&_E_muon_v_sceadded);
        _recoTree_SCEadded->Branch("E_proton_v",&_E_proton_v_sceadded);
        _recoTree_SCEadded->Branch("Length_v",&_Length_v_sceadded);
        _recoTree_SCEadded->Branch("Avg_Ion_v",&_Avg_Ion_v_sceadded);
        _recoTree_SCEadded->Branch("Avg_IonY_v",&_Avg_IonY_v_sceadded);
        _recoTree_SCEadded->Branch("Ion_5cm_v",&_Ion_5cm_v_sceadded);
        _recoTree_SCEadded->Branch("Ion_10cm_v",&_Ion_10cm_v_sceadded);
        _recoTree_SCEadded->Branch("Ion_tot_v",&_Ion_tot_v_sceadded);
        _recoTree_SCEadded->Branch("IonY_5cm_v",&_IonY_5cm_v_sceadded);
        _recoTree_SCEadded->Branch("IonY_10cm_v",&_IonY_10cm_v_sceadded);
        _recoTree_SCEadded->Branch("IonY_tot_v",&_IonY_tot_v_sceadded);
        _recoTree_SCEadded->Branch("Truncated_dQdX1_v",&_Trunc_dQdX1_v_sceadded);
        _recoTree_SCEadded->Branch("Truncated_dQdX3_v",&_Trunc_dQdX3_v_sceadded);
        _recoTree_SCEadded->Branch("IondivLength_v",&_IondivLength_v_sceadded);
        _recoTree_SCEadded->Branch("TotalADCvalues_v",&_TotalADCvalues_v_sceadded);
        _recoTree_SCEadded->Branch("Angle_v",&_Angle_v_sceadded);
        _recoTree_SCEadded->Branch("Reco_goodness_v",&_Reco_goodness_v);
        _recoTree_SCEadded->Branch("track_Goodness_v",&_track_Goodness_v);
        _recoTree_SCEadded->Branch("GoodVertex",&GoodVertex);
        _recoTree_SCEadded->Branch("missingTrack",&_missingTrack);
        _recoTree_SCEadded->Branch("nothingReconstructed",&_nothingReconstructed);
        _recoTree_SCEadded->Branch("tooShortDeadWire",&_tooShortDeadWire);
        _recoTree_SCEadded->Branch("tooShortFaintTrack",&_tooShortFaintTrack);
        _recoTree_SCEadded->Branch("tooManyTracksAtVertex",&_tooManyTracksAtVertex);
        _recoTree_SCEadded->Branch("possibleCosmic",&_possibleCosmic);
        _recoTree_SCEadded->Branch("possiblyCrossing",&_possiblyCrossing);
        _recoTree_SCEadded->Branch("branchingTracks",&_branchingTracks);
        _recoTree_SCEadded->Branch("jumpingTracks",&_jumpingTracks);
        _recoTree_SCEadded->Branch("RecoVertex",&_RecoVertex_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi",&_vertexPhi_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta",&_vertexTheta_sceadded);

        _recoTree_SCEadded->Branch("vertexPhi_2cm",&_vertexPhi_2cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_2cm",&_vertexTheta_2cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_5cm",&_vertexPhi_5cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_5cm",&_vertexTheta_5cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_7cm",&_vertexPhi_7cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_7cm",&_vertexTheta_7cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_10cm",&_vertexPhi_10cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_10cm",&_vertexTheta_10cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_12cm",&_vertexPhi_12cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_12cm",&_vertexTheta_12cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_15cm",&_vertexPhi_15cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_15cm",&_vertexTheta_15cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_17cm",&_vertexPhi_17cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_17cm",&_vertexTheta_17cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_20cm",&_vertexPhi_20cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_20cm",&_vertexTheta_20cm_sceadded);
        _recoTree_SCEadded->Branch("vertexPhi_30cm",&_vertexPhi_30cm_sceadded);
        _recoTree_SCEadded->Branch("vertexTheta_30cm",&_vertexTheta_30cm_sceadded);

        _recoTree_SCEadded->Branch("trackAvg15cm_x_sceadded",&trackAvg15cm_x_sceadded);
        _recoTree_SCEadded->Branch("trackAvg15cm_y_sceadded",&trackAvg15cm_y_sceadded);
        _recoTree_SCEadded->Branch("trackAvg15cm_z_sceadded",&trackAvg15cm_z_sceadded);

        _recoTree_SCEadded->Branch("closestWall",&_closestWall_sceadded);

        _recoTree_SCEadded->Branch("recoEndPoints_x",&recoEndPoints_x_sceadded);
        _recoTree_SCEadded->Branch("recoEndPoints_y",&recoEndPoints_y_sceadded);
        _recoTree_SCEadded->Branch("recoEndPoints_z",&recoEndPoints_z_sceadded);

        _recoTree_SCEadded->Branch("MCvertex",&MCvertex);
        _recoTree_SCEadded->Branch("Ep_t"       , &_Ep_t       , "Ep_t/D");
        _recoTree_SCEadded->Branch("Em_t"       , &_Em_t       , "Em_t/D");
        _recoTree_SCEadded->Branch("Ee_t"       , &_Ee_t       , "Ee_t/D");

        _recoTree_SCEadded->Branch("MuonStartPoint_X", &_MuonStartPoint_X, "MuonStartPoint_X/D");
        _recoTree_SCEadded->Branch("ProtonStartPoint_X", &_ProtonStartPoint_X, "ProtonStartPoint_X/D");
        _recoTree_SCEadded->Branch("ElectronStartPoint_X", &_ElectronStartPoint_X, "ElectronStartPoint_X/D");

        _recoTree_SCEadded->Branch("MuonStartPoint_Y", &_MuonStartPoint_Y, "MuonStartPoint_Y/D");
        _recoTree_SCEadded->Branch("ProtonStartPoint_Y", &_ProtonStartPoint_Y, "ProtonStartPoint_Y/D");
        _recoTree_SCEadded->Branch("ElectronStartPoint_Y", &_ElectronStartPoint_Y, "ElectronStartPoint_Y/D");

        _recoTree_SCEadded->Branch("MuonStartPoint_Z", &_MuonStartPoint_Z, "MuonStartPoint_Z/D");
        _recoTree_SCEadded->Branch("ProtonStartPoint_Z", &_ProtonStartPoint_Z, "ProtonStartPoint_Z/D");
        _recoTree_SCEadded->Branch("ElectronStartPoint_Z", &_ElectronStartPoint_Z, "ElectronStartPoint_Z/D");

        _recoTree_SCEadded->Branch("MuonEndPoint_X", &_MuonEndPoint_X, "MuonEndPoint_X/D");
        _recoTree_SCEadded->Branch("ProtonEndPoint_X", &_ProtonEndPoint_X, "ProtonEndPoint_X/D");
        _recoTree_SCEadded->Branch("ElectronEndPoint_X", &_ElectronEndPoint_X, "ElectronEndPoint_X/D");

        _recoTree_SCEadded->Branch("MuonEndPoint_Y", &_MuonEndPoint_Y, "MuonEndPoint_Y/D");
        _recoTree_SCEadded->Branch("ProtonEndPoint_Y", &_ProtonEndPoint_Y, "ProtonEndPoint_Y/D");
        _recoTree_SCEadded->Branch("ElectronEndPoint_Y", &_ElectronEndPoint_Y, "ElectronEndPoint_Y/D");

        _recoTree_SCEadded->Branch("MuonEndPoint_Z", &_MuonEndPoint_Z, "MuonEndPoint_Z/D");
        _recoTree_SCEadded->Branch("ProtonEndPoint_Z", &_ProtonEndPoint_Z, "ProtonEndPoint_Z/D");
        _recoTree_SCEadded->Branch("ElectronEndPoint_Z", &_ElectronEndPoint_Z, "ElectronEndPoint_Z/D");
    }

    void Run3DTracker::initialize()
    {
        LARCV_INFO() << "[Run3DTracker]" << std::endl;
        assert(!_spline_file.empty());
        IsMCC9 = true;
        tracker.SetIsMCC9(IsMCC9);
        tracker.SetDrawOutputs(false);
        tracker.SetOutputDir(out_dir);
        tracker.SetSplineFile(_spline_file);
        tracker.initialize();
        tracker.SetRandomSeed(100);
        MinLengthAllowed = 3;
        tracker.SetMinLength(MinLengthAllowed);//cm
        tracker.SetVerbose(0);

        std::string filename;

        std::cout << filename << std::endl;

        // define trees & branches
        MakeTTree();
        MakeTTree_SCEadded();

        if (_foutll.empty()) throw larbys("specify larlite file output name");

        _storage.set_io_mode(larlite::storage_manager::kWRITE);
        _storage.set_out_filename(_foutll);

        if(!_storage.open()) {
            LARCV_CRITICAL() << "ERROR, larlite output file could not open" << std::endl;
            throw larbys("die");
        }

    }

    bool Run3DTracker::process(IOManager& mgr)
    {
        ClearEvent();
        std::cout << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << "Entry " << mgr.current_entry() << " / " << mgr.get_n_entries() << std::endl;
        std::cout << "============================================" << std::endl;
        gStyle->SetOptStat(0);

        TVector3 vertex(-1,-1,-1);

        auto ev_pgraph_v     = (EventPGraph*) mgr.get_data(kProductPGraph,_input_pgraph_producer);
        _run    = (int) ev_pgraph_v->run();
        _subrun = (int) ev_pgraph_v->subrun();
        _event  = (int) ev_pgraph_v->event();
        _entry  = (int) mgr.current_entry();

        auto ev_track           = (larlite::event_track*)  _storage.get_data(larlite::data::kTrack,  _out_track_prodname );
        auto ev_vertex          = (larlite::event_vertex*) _storage.get_data(larlite::data::kVertex, _out_vertex_prodname );
        auto ev_ass             = (larlite::event_ass*)    _storage.get_data(larlite::data::kAssociation, _out_assoc_prodname );
        auto ev_track_sceadded  = (larlite::event_track*)  _storage.get_data(larlite::data::kTrack,  _out_track_sce_prodname );
        auto ev_ass_scedradded  = (larlite::event_ass*)    _storage.get_data(larlite::data::kAssociation, _out_assoc_sce_prodname );


        if(ev_pgraph_v->PGraphArray().size()==0){advance_larlite();return true;}

        auto ev_img_v           = (EventImage2D*)mgr.get_data(kProductImage2D,_img2d_producer);
        //auto tag_img_thru_v     = (EventImage2D*)mgr.get_data(kProductImage2D,"thrumutags");
        //auto tag_img_stop_v     = (EventImage2D*)mgr.get_data(kProductImage2D,"stopmutags");

        EventPixel2D* ev_pix_v = nullptr;
        if (!_par_pix_producer.empty())ev_pix_v = (EventPixel2D*) mgr.get_data(kProductPixel2D,_par_pix_producer);


        //auto ev_pcluster_v = (EventPixel2D*)mgr.get_data(kProductPixel2D,"test_img");
        //auto ev_ctor_v     = (EventPixel2D*)mgr.get_data(kProductPixel2D,"test_ctor");


        // get the event clusters and full images
        //auto const& ctor_m = ev_ctor_v->Pixel2DClusterArray();
        auto full_adc_img_v = &(ev_img_v->Image2DArray());
        //auto full_tag_img_thru_v = &(tag_img_thru_v->Image2DArray());
        //auto full_tag_img_stop_v = &(tag_img_stop_v->Image2DArray());



        //
        // Fill MC if exists
        //
        EventROI* ev_partroi_v = nullptr;
        if (!_true_roi_producer.empty()){ev_partroi_v = (EventROI*) mgr.get_data(kProductROI,_true_roi_producer);}
        if (ev_partroi_v) {
            const auto& mc_roi_v = ev_partroi_v->ROIArray();
            FillMC(mc_roi_v);
        }


        // loop over found vertices
        //auto const& pcluster_m = ev_pcluster_v->Pixel2DClusterArray();

        std::vector<larcv::ImageMeta> Full_meta_v(3);
        std::vector<larcv::Image2D> Tagged_Image(3);
        std::vector<larcv::Image2D> Full_image_v(3);

        double wireRange[3] = {2500,2500,3500};
        double tickRange = 8502;
        //double tickRange = 6000;

        // Create base image2D with the full view, fill it with the input image 2D, we will crop it later
        for(size_t iPlane=0;iPlane<3;iPlane++){
            Full_meta_v[iPlane] = larcv::ImageMeta(wireRange[iPlane],tickRange,(int)(tickRange)/6,(int)(wireRange[iPlane]),0,tickRange);
            Full_image_v[iPlane] = larcv::Image2D(Full_meta_v[iPlane]);
            Tagged_Image[iPlane] = larcv::Image2D(Full_meta_v[iPlane]);
            if(full_adc_img_v->size() == 3)Full_image_v[iPlane].overlay( (*full_adc_img_v)[iPlane] );
            //if(full_tag_img_thru_v->size() == 3)Tagged_Image[iPlane].overlay( (*full_tag_img_thru_v)[iPlane] );
            //if(full_tag_img_stop_v->size() == 3)Tagged_Image[iPlane].overlay( (*full_tag_img_stop_v)[iPlane] );
        }
        tracker.SetOriginalImage(Full_image_v);
        tracker.SetTaggedImage(Tagged_Image);
        tracker.SetTrackInfo(_run, _subrun, _event, 0);

        std::cout << _run << " " << _subrun << " " << _event <<  std::endl;

        static std::vector<TVector3> vertex_v;
        static std::vector<TVector3> vertex_v_sceadded;
        if(vertex_v.size()!=0)vertex_v.clear();

        for(size_t pgraph_id = 0; pgraph_id < ev_pgraph_v->PGraphArray().size(); ++pgraph_id) {

            auto const& pgraph = ev_pgraph_v->PGraphArray().at(pgraph_id);
            auto const& roi_v         = pgraph.ParticleArray();
            auto const& cluster_idx_v = pgraph.ClusterIndexArray();

            RecoVertex.SetXYZ(pgraph.ParticleArray().front().X(),pgraph.ParticleArray().front().Y(),pgraph.ParticleArray().front().Z());
            vertex_v.push_back(RecoVertex);

            RecoVertex_sceadded.SetXYZ(pgraph.ParticleArray().front().X(),pgraph.ParticleArray().front().Y(),pgraph.ParticleArray().front().Z());
            vertex_v_sceadded.push_back(RecoVertex_sceadded);

            if (_mask_shower) {

                assert (ev_pix_v);
                const auto& pix_m          = ev_pix_v->Pixel2DClusterArray();
                const auto& pix_meta_m     = ev_pix_v->ClusterMetaArray();

                //
                // mask shower particle pixels
                // method : pixels stored via larbys image
                //
                for(size_t roid=0; roid < roi_v.size(); ++roid) {
                    const auto& roi = roi_v[roid];
                    auto cidx = cluster_idx_v.at(roid);

                    if (roi.Shape() == kShapeTrack) continue;

                    for(size_t plane=0; plane<3; ++plane) {

                        auto iter_pix = pix_m.find(plane);
                        if(iter_pix == pix_m.end())
                            continue;

                        auto iter_pix_meta = pix_meta_m.find(plane);
                        if(iter_pix_meta == pix_meta_m.end())
                            continue;

                        auto const& pix_v      = (*iter_pix).second;
                        auto const& pix_meta_v = (*iter_pix_meta).second;

                        auto const& pix      = pix_v.at(cidx);
                        auto const& pix_meta = pix_meta_v.at(cidx);

                        auto& plane_img = Full_image_v.at(plane);

                        for(const auto& px : pix) {
                            auto posx = pix_meta.pos_x(px.Y());
                            auto posy = pix_meta.pos_y(px.X());
                            auto row = plane_img.meta().row(posy);
                            auto col = plane_img.meta().col(posx);
                            plane_img.set_pixel(row,col,0);
                        }
                    } // end plane
                } // end ROI
            }//end mask Shower
        }

        tracker.SetOriginalImage(Full_image_v);
        tracker.SetTaggedImage(Tagged_Image);
        tracker.SetTrackInfo(_run, _subrun, _event, 0);
        tracker.ClearDeadWireList();
        std::vector<std::vector<unsigned> > ass_vertex_to_track_vv;
        ass_vertex_to_track_vv.resize(vertex_v.size());

        std::vector<std::vector<unsigned> > ass_vertex_to_track_sceadded_vv;
        ass_vertex_to_track_sceadded_vv.resize(vertex_v.size());

        //vertex_v = MCVertices;
        int TrackID = 0;

        if(vertex_v.size()!=0){
            for(size_t ivertex = 0;ivertex<vertex_v.size();ivertex++){

                NtracksReco = 0;
                if(_trk_id_v.size()!=0)_trk_id_v.clear();
                if(_IondivLength_v.size()!=0)_IondivLength_v.clear();
                if(_TotalADCvalues_v.size()!=0)_TotalADCvalues_v.clear();
                if(_IondivLength_v_sceadded.size()!=0)_IondivLength_v_sceadded.clear();
                if(_TotalADCvalues_v_sceadded.size()!=0)_TotalADCvalues_v_sceadded.clear();

                double xyz[3] = {vertex_v[ivertex].X(),vertex_v[ivertex].Y(),vertex_v[ivertex].Z()};
                double xyz_sceadded[3] = {vertex_v_sceadded[ivertex].X(),vertex_v_sceadded[ivertex].Y(),vertex_v_sceadded[ivertex].Z()};
                ev_vertex->push_back(larlite::vertex(xyz,ivertex));

                auto& ass_vertex_to_track_v          = ass_vertex_to_track_vv[ivertex];
                auto& ass_vertex_to_track_sceadded_v = ass_vertex_to_track_sceadded_vv[ivertex];

                tracker.SetSingleVertex(vertex_v[ivertex]);

                int Ntrials = 0;
                int NtotalTrials = 5;
                GoodVertex = false;
                while(GoodVertex == false && Ntrials < NtotalTrials){
                    GoodVertex = false;
                    tracker.SetRandomSeed(Ntrials+1);
                    tracker.ReconstructVertex();
                    GoodVertex = tracker.IsGoodVertex();
                    Ntrials++;
                    std::cout << "trial #" << Ntrials << "/"<< NtotalTrials <<" GoodVertex = " << GoodVertex << std::endl;
                    if(GoodVertex || Ntrials == NtotalTrials)break;
                }

                auto recoedVertex = tracker.GetReconstructedVertexTracks();

                NtracksReco = 0;
                for(size_t itrack = 0; itrack<recoedVertex.size();itrack++){
                    ass_vertex_to_track_v.push_back(ev_track->size());
                    recoedVertex[itrack].set_track_id(TrackID);
                    (*ev_track).push_back(recoedVertex[itrack]);
                    _trk_id_v.push_back(TrackID);
                    TrackID++;
                    if(recoedVertex[itrack].Length(0) > MinLengthAllowed){NtracksReco++;}
                }

                auto Energies_v = tracker.GetEnergies();
                _E_muon_v.resize(Energies_v.size());
                _E_proton_v.resize(Energies_v.size());
                for(size_t trackid=0; trackid<Energies_v.size(); ++trackid) {
                    _E_proton_v[trackid] = Energies_v[trackid].front();
                    _E_muon_v[trackid]   = Energies_v[trackid].back();
                }


                _vtx_id = (int) ivertex;
                _vtx_x  = (float) vertex_v[ivertex].X();
                _vtx_y  = (float) vertex_v[ivertex].Y();
                _vtx_z  = (float) vertex_v[ivertex].Z();



                _DeadWireList = tracker.GetDeadWireList();
                for(size_t iwire = 0;iwire<_DeadWireList[0].size();iwire++){
                    _DeadWireList_U.push_back((int)(_DeadWireList[0][iwire]));
                }
                for(size_t iwire = 0;iwire<_DeadWireList[1].size();iwire++){
                    _DeadWireList_V.push_back((int)(_DeadWireList[1][iwire]));
                }
                for(size_t iwire = 0;iwire<_DeadWireList[2].size();iwire++){
                    _DeadWireList_Y.push_back((int)(_DeadWireList[2][iwire]));
                }
                _Length_v        = tracker.GetVertexLength();
                _closestWall     = tracker.GetClosestWall();
                _trackQ3_v       = tracker.GetTotalPixADC(3.);
                _trackQ5_v       = tracker.GetTotalPixADC(5.);
                _trackQ10_v      = tracker.GetTotalPixADC(10.);
                _trackQ20_v      = tracker.GetTotalPixADC(20.);
                _trackQ30_v      = tracker.GetTotalPixADC(30.);
                _trackQ50_v      = tracker.GetTotalPixADC(50.);
                _Avg_Ion_v       = tracker.GetAverageIonization();
                _Ion_5cm_v       = tracker.GetTotalIonization(5);
                _Ion_10cm_v      = tracker.GetTotalIonization(10);
                _Ion_tot_v       = tracker.GetTotalIonization();
                _Avg_IonY_v      = tracker.GetAverageIonization_Yplane();
                _IonY_5cm_v      = tracker.GetTotalIonization_Yplane(5);
                _IonY_10cm_v     = tracker.GetTotalIonization_Yplane(10);
                _IonY_tot_v      = tracker.GetTotalIonization_Yplane();
                _Trunc_dQdX1_v   = tracker.ComputeTruncateddQdX(1.0);
                _Trunc_dQdX3_v   = tracker.ComputeTruncateddQdX(3.0);
                _TotalADCvalues_v = tracker.GetTotalPixADC();

                _Angle_v          = tracker.GetVertexAngle(2,0); // average over 2 cm to estimate the angles
                _vertexPhi_2cm    = tracker.GetVertexPhi();
                _vertexTheta_2cm  = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(5,2); // average over 5 cm to estimate the angles
                _vertexPhi_5cm    = tracker.GetVertexPhi();
                _vertexTheta_5cm  = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(7,2); // average over 7 cm to estimate the angles
                _vertexPhi_7cm    = tracker.GetVertexPhi();
                _vertexTheta_7cm  = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(10,2); // average over 10 cm to estimate the angles
                _vertexPhi_10cm   = tracker.GetVertexPhi();
                _vertexTheta_10cm = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(12,2); // average over 12 cm to estimate the angles
                _vertexPhi_12cm   = tracker.GetVertexPhi();
                _vertexTheta_12cm = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(17,2); // average over 10 cm to estimate the angles
                _vertexPhi_17cm   = tracker.GetVertexPhi();
                _vertexTheta_17cm = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(20,2); // average over 10 cm to estimate the angles
                _vertexPhi_20cm   = tracker.GetVertexPhi();
                _vertexTheta_20cm = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(30,2); // average over 10 cm to estimate the angles
                _vertexPhi_30cm   = tracker.GetVertexPhi();
                _vertexTheta_30cm = tracker.GetVertexTheta();
                _Angle_v          = tracker.GetVertexAngle(15,2); // average over 15 cm to estimate the angles
                _vertexPhi        = tracker.GetVertexPhi();
                _vertexTheta      = tracker.GetVertexTheta();
                _vertexPhi_15cm   = tracker.GetVertexPhi();
                _vertexTheta_15cm = tracker.GetVertexTheta();

                trackAvg15cm = tracker.GetTrack15cmAvg();
                for(size_t ii=0 ;ii < trackAvg15cm.size();ii++){
                    trackAvg15cm_x.push_back(trackAvg15cm.at(ii).X());
                    trackAvg15cm_y.push_back(trackAvg15cm.at(ii).Y());
                    trackAvg15cm_z.push_back(trackAvg15cm.at(ii).Z());
                }

                if(recoEndPoints.size()   !=0)recoEndPoints.clear();
                if(recoEndPoints_x.size() !=0)recoEndPoints_x.clear();
                if(recoEndPoints_y.size() !=0)recoEndPoints_y.clear();
                if(recoEndPoints_z.size() !=0)recoEndPoints_z.clear();
                recoEndPoints = tracker.GetEndPoints();
                for(int kk = 0;kk < recoEndPoints.size(); kk++){
                    recoEndPoints_x.push_back(recoEndPoints[kk].X());
                    recoEndPoints_y.push_back(recoEndPoints[kk].Y());
                    recoEndPoints_z.push_back(recoEndPoints[kk].Z());
                }
                for(size_t itrack = 0; itrack<_Length_v.size();itrack++){
                    _IondivLength_v.push_back(_Ion_tot_v[itrack]/_Length_v[itrack]);
                }
                _RecoVertex = vertex_v.at(ivertex);

                _Reco_goodness_v  = tracker.GetRecoGoodness();
                _track_Goodness_v = tracker.GetVtxQuality();


                assert (_Reco_goodness_v.size() == 9);
                _missingTrack          = _Reco_goodness_v.at(0);
                _nothingReconstructed  = _Reco_goodness_v.at(1);
                _tooShortDeadWire      = _Reco_goodness_v.at(2);
                _tooShortFaintTrack    = _Reco_goodness_v.at(3);
                _tooManyTracksAtVertex = _Reco_goodness_v.at(4);
                _possibleCosmic        = _Reco_goodness_v.at(5);
                _possiblyCrossing      = _Reco_goodness_v.at(6);
                _branchingTracks       = _Reco_goodness_v.at(7);
                _jumpingTracks         = _Reco_goodness_v.at(8);

                //tracker.DrawVertexVertical();


                /////////////////////////
                // sce added variables //
                /////////////////////////
                std::cout << "Adding SCE to reconstructed tracks" << std::endl;
                tracker.Add_SCE_to_Tracks();

                auto recoedVertex_sceadded = tracker.GetReconstructedVertexTracks();
                NtracksReco_sceadded = 0;
                for(size_t itrack = 0; itrack<recoedVertex_sceadded.size();itrack++){
                    ass_vertex_to_track_sceadded_v.push_back(ev_track_sceadded->size());
                    recoedVertex_sceadded[itrack].set_track_id(TrackID);
                    (*ev_track_sceadded).push_back(recoedVertex_sceadded[itrack]);
                    _trk_id_v.push_back(TrackID);
                    TrackID++;
                    if(recoedVertex_sceadded[itrack].Length(0) > MinLengthAllowed){NtracksReco_sceadded++;}
                }

                auto Energies_scedr_v = tracker.GetEnergies();
                _E_muon_v_sceadded.resize(Energies_scedr_v.size());
                _E_proton_v_sceadded.resize(Energies_scedr_v.size());
                for(size_t trackid=0; trackid<Energies_v.size(); ++trackid) {
                    _E_proton_v_sceadded[trackid] = Energies_scedr_v[trackid].front();
                    _E_muon_v_sceadded[trackid]   = Energies_scedr_v[trackid].back();
                }

                _vtx_x_sceadded  = (float) vertex_v_sceadded[ivertex].X();
                _vtx_y_sceadded  = (float) vertex_v_sceadded[ivertex].Y();
                _vtx_z_sceadded  = (float) vertex_v_sceadded[ivertex].Z();

                _Length_v_sceadded         = tracker.GetVertexLength();
                _closestWall_sceadded      = tracker.GetClosestWall();
                _trackQ3_v_sceadded        = tracker.GetTotalPixADC(3.);
                _trackQ5_v_sceadded        = tracker.GetTotalPixADC(5.);
                _trackQ10_v_sceadded       = tracker.GetTotalPixADC(10.);
                _trackQ20_v_sceadded       = tracker.GetTotalPixADC(20.);
                _trackQ30_v_sceadded       = tracker.GetTotalPixADC(30.);
                _trackQ50_v_sceadded       = tracker.GetTotalPixADC(50.);
                _Avg_Ion_v_sceadded        = tracker.GetAverageIonization();
                _Ion_5cm_v_sceadded        = tracker.GetTotalIonization(5);
                _Ion_10cm_v_sceadded       = tracker.GetTotalIonization(10);
                _Ion_tot_v_sceadded        = tracker.GetTotalIonization();
                _Avg_IonY_v_sceadded       = tracker.GetAverageIonization_Yplane();
                _IonY_5cm_v_sceadded       = tracker.GetTotalIonization_Yplane(5);
                _IonY_10cm_v_sceadded      = tracker.GetTotalIonization_Yplane(10);
                _IonY_tot_v_sceadded       = tracker.GetTotalIonization_Yplane();
                _Trunc_dQdX1_v_sceadded    = tracker.ComputeTruncateddQdX(1.0);
                _Trunc_dQdX3_v_sceadded    = tracker.ComputeTruncateddQdX(3.0);
                _TotalADCvalues_v_sceadded = tracker.GetTotalPixADC();

                _Angle_v_sceadded          = tracker.GetVertexAngle(2,0); // average over 2 cm to estimate the angles
                _vertexPhi_2cm_sceadded    = tracker.GetVertexPhi();
                _vertexTheta_2cm_sceadded  = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(5,2); // average over 5 cm to estimate the angles
                _vertexPhi_5cm_sceadded    = tracker.GetVertexPhi();
                _vertexTheta_5cm_sceadded  = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(7,2); // average over 7 cm to estimate the angles
                _vertexPhi_7cm_sceadded    = tracker.GetVertexPhi();
                _vertexTheta_7cm_sceadded  = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(10,2); // average over 10 cm to estimate the angles
                _vertexPhi_10cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_10cm_sceadded = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(12,2); // average over 12 cm to estimate the angles
                _vertexPhi_12cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_12cm_sceadded = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(17,2); // average over 10 cm to estimate the angles
                _vertexPhi_17cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_17cm_sceadded = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(20,2); // average over 10 cm to estimate the angles
                _vertexPhi_20cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_20cm_sceadded = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(30,2); // average over 10 cm to estimate the angles
                _vertexPhi_30cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_30cm_sceadded = tracker.GetVertexTheta();
                _Angle_v_sceadded          = tracker.GetVertexAngle(15,2); // average over 15 cm to estimate the angles
                _vertexPhi_sceadded        = tracker.GetVertexPhi();
                _vertexTheta_sceadded      = tracker.GetVertexTheta();
                _vertexPhi_15cm_sceadded   = tracker.GetVertexPhi();
                _vertexTheta_15cm_sceadded = tracker.GetVertexTheta();

                trackAvg15cm_sceadded = tracker.GetTrack15cmAvg();
                for(size_t ii=0 ;ii < trackAvg15cm_sceadded.size();ii++){
                    trackAvg15cm_x_sceadded.push_back(trackAvg15cm_sceadded.at(ii).X());
                    trackAvg15cm_y_sceadded.push_back(trackAvg15cm_sceadded.at(ii).Y());
                    trackAvg15cm_z_sceadded.push_back(trackAvg15cm_sceadded.at(ii).Z());
                }

                if(recoEndPoints_sceadded.size()   !=0)recoEndPoints_sceadded.clear();
                if(recoEndPoints_x_sceadded.size() !=0)recoEndPoints_x_sceadded.clear();
                if(recoEndPoints_y_sceadded.size() !=0)recoEndPoints_y_sceadded.clear();
                if(recoEndPoints_z_sceadded.size() !=0)recoEndPoints_z_sceadded.clear();
                recoEndPoints_sceadded = tracker.GetEndPoints();
                for(int kk = 0;kk < recoEndPoints_sceadded.size(); kk++){
                    recoEndPoints_x_sceadded.push_back(recoEndPoints_sceadded[kk].X());
                    recoEndPoints_y_sceadded.push_back(recoEndPoints_sceadded[kk].Y());
                    recoEndPoints_z_sceadded.push_back(recoEndPoints_sceadded[kk].Z());
                }
                for(size_t itrack = 0; itrack<_Length_v_sceadded.size();itrack++){
                    _IondivLength_v_sceadded.push_back(_Ion_tot_v_sceadded[itrack]/_Length_v_sceadded[itrack]);
                }
                _RecoVertex_sceadded = vertex_v_sceadded.at(ivertex);

                randomSeed = tracker.GetRandomSeed();
                //tracker.DrawVertexVertical();

                //______________________
                if(ev_partroi_v)MCevaluation();
                //----------------------

                _recoTree->Fill();
                _recoTree_SCEadded->Fill();

                ClearVertex();
                std::cout << std::endl << std::endl;

            }
        }

        // set the ass
        ev_ass->set_association(ev_vertex->id(),ev_track->id(), ass_vertex_to_track_vv);
        ev_ass_scedradded->set_association(ev_vertex->id(),ev_track_sceadded->id(), ass_vertex_to_track_sceadded_vv);

        advance_larlite();
        std::cout << "...Reconstruted..." << std::endl;

        return true;
    }

    void Run3DTracker::MCevaluation(){
        std::cout << "MCevaluation()" << std::endl;

        std::vector< std::vector<double> > Energies_v = tracker.GetEnergies();
        //_Length_v = tracker.GetVertexLength();

        if(!tracker.IsGoodVertex()){std::cout << "error: is not good vertex" << std::endl;return;}
        if(_Avg_Ion_v.size()!=2){std::cout << "error: _Avg_Ion_v.size() = " << _Avg_Ion_v.size() << std::endl;return;}

        std::cout << "in store energies : " << std::endl;
        std::cout << Energies_v[0][0] << ":" << Energies_v[0][1] << std::endl;
        std::cout << Energies_v[1][0] << ":" << Energies_v[1][1] << std::endl;

        double Epreco;
        double Emreco;

        if(_Avg_Ion_v[0] > _Avg_Ion_v[1]){
            Epreco = Energies_v[0][0];// first track is proton
            Emreco = Energies_v[1][1];// second track is muon
        }
        else{
            Epreco = Energies_v[1][0]; // second track is proton
            Emreco = Energies_v[0][1]; // first track is muon
        }

        if( std::abs((Epreco-Ep_t)/Ep_t) > 0.1 || std::abs((Emreco-Em_t)/Em_t) > 0.1 ){
            checkEvents.push_back(Form("%d_%d_%d",_run,_subrun,_event));
        }

        std::cout << "proton : Epreco = " << Epreco << " MeV, Epth : " << Ep_t << " MeV..." << 100*(Epreco-Ep_t)/Ep_t << " %" << std::endl;
        std::cout << "muon   : Emreco = " << Emreco << " MeV, Emth : " << Em_t << " MeV..." << 100*(Emreco-Em_t)/Em_t << " %" << std::endl;

        std::cout << "E nu th   : " << NeutrinoEnergyTh << " MeV" << std::endl;
        std::cout << "E nu Reco : " << Epreco+Emreco << " MeV" << std::endl;
        std::cout << "relative Enu diff: " << 100*((Epreco+Emreco+146)-NeutrinoEnergyTh)/NeutrinoEnergyTh << " %" << std::endl;
        std::cout << "relative Enu_p+m diff: " << 100*((Epreco+Emreco)-(Ep_t+Em_t))/(Ep_t+Em_t) << " %" << std::endl;
    }

    void Run3DTracker::finalize()
    {
        tracker.finalize();

        if(has_ana_file()) {
            ana_file().cd();
            _recoTree->Write();
            _recoTree_SCEadded->Write();
        }
        _storage.close();
        std::cout << "finalized tracker" << std::endl;
    }

    void Run3DTracker::SetSplineLocation(const std::string& fpath) {
        LARCV_INFO() << "setting spline loc @ " << fpath << std::endl;
        tracker.SetSplineFile(fpath);
        _spline_file = fpath;
        LARCV_DEBUG() << "end" << std::endl;
    }

    void Run3DTracker::advance_larlite() {
        _storage.set_id(_run,_subrun,_event);
        _storage.next_event(true);
    }

    void Run3DTracker::FillMC(const std::vector<ROI>& mc_roi_v) {
        bool found_muon = false;
        bool found_proton = false;
        bool found_electron = false;

        std::vector<TVector3> MuonVertices;
        std::vector<TVector3> ProtonVertices;
        std::vector<TVector3> ElectronVertices;
        std::vector<TVector3> MuonEndPoint;
        std::vector<TVector3> ProtonEndPoint;
        std::vector<TVector3> ElectronEndPoint;

        for(size_t iMC = 0;iMC<mc_roi_v.size();iMC++){
            if(mc_roi_v[iMC].PdgCode() == 13){
                if (found_muon) continue;
                LARCV_INFO() << "muon.....@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                _MuonStartPoint_X = mc_roi_v[iMC].X();
                _MuonStartPoint_Y = mc_roi_v[iMC].Y();
                _MuonStartPoint_Z = mc_roi_v[iMC].Z();

                _MuonEndPoint_X = mc_roi_v[iMC].EndPosition().X();
                _MuonEndPoint_Y = mc_roi_v[iMC].EndPosition().Y();
                _MuonEndPoint_Z = mc_roi_v[iMC].EndPosition().Z();
                _Em_t = mc_roi_v[iMC].EnergyDeposit();
                found_muon = true;
            }
            if(mc_roi_v[iMC].PdgCode() == 2212){
                if (found_proton) continue;
                LARCV_INFO() << "proton...@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                _ProtonStartPoint_X = mc_roi_v[iMC].X();
                _ProtonStartPoint_Y = mc_roi_v[iMC].Y();
                _ProtonStartPoint_Z = mc_roi_v[iMC].Z();

                _ProtonEndPoint_X = mc_roi_v[iMC].EndPosition().X();
                _ProtonEndPoint_Y = mc_roi_v[iMC].EndPosition().Y();
                _ProtonEndPoint_Z = mc_roi_v[iMC].EndPosition().Z();
                _Ep_t = mc_roi_v[iMC].EnergyDeposit();
                found_proton = true;
            }
            if(mc_roi_v[iMC].PdgCode() == 11){
                if(found_electron) continue;
                LARCV_INFO() << "electron.@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                _ElectronStartPoint_X = mc_roi_v[iMC].X();
                _ElectronStartPoint_Y = mc_roi_v[iMC].Y();
                _ElectronStartPoint_Z = mc_roi_v[iMC].Z();

                _ElectronEndPoint_X = mc_roi_v[iMC].EndPosition().X();
                _ElectronEndPoint_Y = mc_roi_v[iMC].EndPosition().Y();
                _ElectronEndPoint_Z = mc_roi_v[iMC].EndPosition().Z();
                _Ee_t = mc_roi_v[iMC].EnergyDeposit();
                found_electron = true;
            }

            // Adrien old version
            if(mc_roi_v[iMC].PdgCode() == 14){
                NeutrinoEnergyTh = mc_roi_v[iMC].EnergyDeposit();
                std::cout << "Neutrino : " << NeutrinoEnergyTh << " MeV" << std::endl;
            }
            if(mc_roi_v[iMC].PdgCode() == 13){
                std::cout << "muon.....@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                Em_t = mc_roi_v[iMC].EnergyDeposit();
                MuonVertices.push_back(TVector3(mc_roi_v[iMC].X(),mc_roi_v[iMC].Y(),mc_roi_v[iMC].Z()));
                MuonEndPoint.push_back(TVector3(mc_roi_v[iMC].EndPosition().X(), mc_roi_v[iMC].EndPosition().Y(), mc_roi_v[iMC].EndPosition().Z()));
            }
            if(mc_roi_v[iMC].PdgCode() == 2212){
                if(mc_roi_v[iMC].EnergyDeposit() < 60) continue;
                std::cout << "proton...@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                Ep_t = mc_roi_v[iMC].EnergyDeposit();
                ProtonVertices.push_back(TVector3(mc_roi_v[iMC].X(),mc_roi_v[iMC].Y(),mc_roi_v[iMC].Z()));
                ProtonEndPoint.push_back(TVector3(mc_roi_v[iMC].EndPosition().X(), mc_roi_v[iMC].EndPosition().Y(), mc_roi_v[iMC].EndPosition().Z()));
            }
            if(mc_roi_v[iMC].PdgCode() == 11){
                std::cout << "electron.@" << mc_roi_v[iMC].X() << ", " << mc_roi_v[iMC].Y() << ", " << mc_roi_v[iMC].Z() << " ... " << mc_roi_v[iMC].EnergyDeposit() << " MeV" << std::endl;
                ElectronVertices.push_back(TVector3(mc_roi_v[iMC].X(),mc_roi_v[iMC].Y(),mc_roi_v[iMC].Z()));
                ElectronEndPoint.push_back(TVector3(mc_roi_v[iMC].EndPosition().X(), mc_roi_v[iMC].EndPosition().Y(), mc_roi_v[iMC].EndPosition().Z()));
            }

            bool isVertex = false;
            bool isNumu = false;
            bool isNue = false;
            std::vector<int> goodMuon, goodElectron;
            for(size_t iProton = 0;iProton<ProtonVertices.size();iProton++){
                isVertex = false;
                isNumu = false;
                isNue = false;
                for(size_t iMuon = 0;iMuon<MuonVertices.size();iMuon++){
                    if(MuonVertices[iMuon] == ProtonVertices[iProton]){isVertex = true;isNumu = true;goodMuon.push_back(iMuon);}
                }
                for(size_t iElectron = 0;iElectron<ElectronVertices.size();iElectron++){
                    if(ProtonVertices[iProton] == ElectronVertices[iProton]){isVertex = true;isNue = true;goodElectron.push_back(iElectron);}
                }
                if(isVertex && MCVertices.size()!=0 && ProtonVertices[iProton] == MCVertices[MCVertices.size()-1])continue;
                if(isVertex){
                    MCvertex = ProtonVertices[iProton];
                    MCVertices.push_back(MCvertex);
                }
            }

            // end Adrien
        } // end rois
        return;
    } // end mc

    void Run3DTracker::ClearEvent() {
        _run    = kINVALID_INT;
        _subrun = kINVALID_INT;
        _event  = kINVALID_INT;
        _Nreco = 0;

        _MuonStartPoint_X = -1.0*kINVALID_DOUBLE;
        _ProtonStartPoint_X = -1.0*kINVALID_DOUBLE;
        _ElectronStartPoint_X = -1.0*kINVALID_DOUBLE;

        _MuonStartPoint_Y = -1.0*kINVALID_DOUBLE;
        _ProtonStartPoint_Y = -1.0*kINVALID_DOUBLE;
        _ElectronStartPoint_Y = -1.0*kINVALID_DOUBLE;

        _MuonStartPoint_Z = -1.0*kINVALID_DOUBLE;
        _ProtonStartPoint_Z = -1.0*kINVALID_DOUBLE;
        _ElectronStartPoint_Z = -1.0*kINVALID_DOUBLE;


        _MuonEndPoint_X = -1.0*kINVALID_DOUBLE;
        _ProtonEndPoint_X = -1.0*kINVALID_DOUBLE;
        _ElectronEndPoint_X = -1.0*kINVALID_DOUBLE;

        _MuonEndPoint_Y = -1.0*kINVALID_DOUBLE;
        _ProtonEndPoint_Y = -1.0*kINVALID_DOUBLE;
        _ElectronEndPoint_Y = -1.0*kINVALID_DOUBLE;

        _MuonEndPoint_Z = -1.0*kINVALID_DOUBLE;
        _ProtonEndPoint_Z = -1.0*kINVALID_DOUBLE;
        _ElectronEndPoint_Z = -1.0*kINVALID_DOUBLE;

        _Ep_t = -1.0*kINVALID_DOUBLE;
        _Em_t = -1.0*kINVALID_DOUBLE;
        _Ee_t = -1.0*kINVALID_DOUBLE;

        MCvertex.SetXYZ(-1,-1,-1);
        if(MCVertices.size()!=0)MCVertices.clear();

        ClearVertex();
    }

    void Run3DTracker::ClearVertex() {
        _vtx_id = -1.0 * kINVALID_INT;
        _vtx_x  = -1.0 * kINVALID_FLOAT;
        _vtx_y  = -1.0 * kINVALID_FLOAT;
        _vtx_z  = -1.0 * kINVALID_FLOAT;
        _vtx_x_sceadded  = -1.0 * kINVALID_FLOAT;
        _vtx_y_sceadded  = -1.0 * kINVALID_FLOAT;
        _vtx_z_sceadded  = -1.0 * kINVALID_FLOAT;
        _E_muon_v.clear();
        _E_muon_v_sceadded.clear();
        _E_proton_v.clear();
        _E_proton_v_sceadded.clear();
        _Length_v.clear();
        _Length_v_sceadded.clear();
        _Avg_Ion_v.clear();
        _Avg_Ion_v_sceadded.clear();
        _Angle_v.clear();
        _Angle_v_sceadded.clear();
        _DeadWireList.clear();
        _Reco_goodness_v.clear();
        GoodVertex = -1.0*kINVALID_INT;
        _missingTrack = -1.0*kINVALID_INT;
        _nothingReconstructed = -1.0*kINVALID_INT;
        _tooShortDeadWire = -1.0*kINVALID_INT;
        _tooShortFaintTrack = -1.0*kINVALID_INT;
        _tooManyTracksAtVertex = -1.0*kINVALID_INT;
        _possibleCosmic = -1.0*kINVALID_INT;
        _possiblyCrossing = -1.0*kINVALID_INT;
        _branchingTracks = -1.0*kINVALID_INT;
        _jumpingTracks = -1.0*kINVALID_INT;
        _trackQ50_v.clear();
        _trackQ50_v_sceadded.clear();
        _trackQ30_v.clear();
        _trackQ30_v_sceadded.clear();
        _trackQ20_v.clear();
        _trackQ20_v_sceadded.clear();
        _trackQ10_v.clear();
        _trackQ10_v_sceadded.clear();
        _trackQ5_v.clear();
        _trackQ5_v_sceadded.clear();
        _trackQ3_v.clear();
        _trackQ3_v_sceadded.clear();

    }

}
#endif
