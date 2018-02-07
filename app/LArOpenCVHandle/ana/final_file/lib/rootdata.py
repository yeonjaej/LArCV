import ROOT
from array import array

kINVALID_INT    = ROOT.std.numeric_limits("int")().lowest()
kINVALID_FLOAT  = ROOT.std.numeric_limits("float")().lowest()
kINVALID_DOUBLE = ROOT.std.numeric_limits("double")().lowest()

class ROOTData:
    def __init__(self):

        #
        # book keeping
        # 
        self.run    = array( 'i', [ kINVALID_INT ] )
        self.subrun = array( 'i', [ kINVALID_INT ] )
        self.event  = array( 'i', [ kINVALID_INT ] )

        self.num_croi   = array( 'i', [ kINVALID_INT ] )
        self.num_vertex = array( 'i', [ kINVALID_INT ] )
        self.vertex_id  = array( 'i', [ kINVALID_INT ] )

        #
        # is it selected
        #
        self.reco_selected = array( 'i', [ kINVALID_INT ] )
        self.reco_close    = array( 'i', [ kINVALID_INT ] )
        self.reco_on_nu    = array( 'i', [ kINVALID_INT ] )

        #
        # the LL
        #
        self.LL_dist = array( 'f', [ kINVALID_FLOAT ] )
        self.LLc_e   = array( 'f', [ kINVALID_FLOAT ] )
        self.LLc_p   = array( 'f', [ kINVALID_FLOAT ] )
        self.LLe_e   = array( 'f', [ kINVALID_FLOAT ] )
        self.LLe_p   = array( 'f', [ kINVALID_FLOAT ] )

        #
        # MC truth
        #
        self.selected1L1P   = array( 'i', [ kINVALID_INT   ] )
        self.scedr          = array( 'f', [ -1.0*kINVALID_FLOAT ] )
        self.nu_pdg         = array( 'i', [ kINVALID_INT   ] )
        self.inter_type     = array( 'i', [ kINVALID_INT   ] ) 
        self.inter_mode     = array( 'i', [ kINVALID_INT   ] )
        self.true_vertex    = array( 'f', [ kINVALID_FLOAT, kINVALID_FLOAT, kINVALID_FLOAT ] )

        self.true_proton_E    = array( 'f', [ kINVALID_FLOAT ] )
        self.true_lepton_E    = array( 'f', [ kINVALID_FLOAT ] )
        self.true_electron_E  = array( 'f', [ kINVALID_FLOAT ] )
        self.true_total_E     = array( 'f', [ kINVALID_FLOAT ] )

        self.true_proton_dR    = array( 'f', [ kINVALID_FLOAT, kINVALID_FLOAT, kINVALID_FLOAT ] )
        self.true_electron_dR  = array( 'f', [ kINVALID_FLOAT, kINVALID_FLOAT, kINVALID_FLOAT ] )

        self.true_proton_theta   = array( 'f', [ kINVALID_FLOAT ] )
        self.true_electron_theta = array( 'f', [ kINVALID_FLOAT ] )
        self.true_muon_theta     = array( 'f', [ kINVALID_FLOAT ] )

        self.true_proton_phi   = array( 'f', [ kINVALID_FLOAT ] )
        self.true_electron_phi = array( 'f', [ kINVALID_FLOAT ] )
        self.true_muon_phi     = array( 'f', [ kINVALID_FLOAT ] )

        self.true_proton_ylen   = array( 'f', [ kINVALID_FLOAT ] )
        self.true_opening_angle = array( 'f', [ kINVALID_FLOAT ] )

        self.true_nu_E = array( 'f', [ kINVALID_FLOAT ] )

        #
        # reco parameters
        #

        # truth
        self.reco_mc_proton_E    = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_mc_electron_E  = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_mc_total_E     = array( 'f', [ kINVALID_FLOAT ] )


        # proton and electron
        self.reco_proton_id    = array( 'i', [ kINVALID_INT   ] )
        self.reco_proton_E     = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_proton_theta = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_proton_phi   = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_proton_dEdx  = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_proton_len   = array( 'f', [ kINVALID_FLOAT ] )

        self.reco_electron_id    = array( 'i', [ kINVALID_INT]    )
        self.reco_electron_E     = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_electron_theta = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_electron_phi   = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_electron_dEdx  = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_electron_len   = array( 'f', [ kINVALID_FLOAT ] )

        # combined
        self.reco_total_E = array( 'f', [ kINVALID_FLOAT ] )
        self.reco_vertex  = array( 'f', [ kINVALID_FLOAT, kINVALID_FLOAT, kINVALID_FLOAT ] )


        #
        # numu related
        #
        self.CosmicLL      = array( 'f', [ kINVALID_FLOAT ])
        self.NuBkgLL       = array( 'f', [ kINVALID_FLOAT ])
        self.PassCuts      = array( 'i', [ kINVALID_INT   ])
        self.VtxAlgo       = array( 'i', [ kINVALID_INT   ])
        self.NTracks       = array( 'i', [ kINVALID_INT   ])
        self.N5cmTracks    = array( 'i', [ kINVALID_INT   ])
        self.InFiducial    = array( 'i', [ kINVALID_INT   ])
        self.Good3DReco    = array( 'i', [ kINVALID_INT   ])
        self.AnythingRecod = array( 'i', [ kINVALID_INT   ])

        self.Muon_id          = array( 'i', [ kINVALID_INT   ])
        self.Muon_PhiReco     = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_ThetaReco   = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_TrackLength = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_dQdx        = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_Trunc_dQdx1 = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_Trunc_dQdx3 = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_IonPerLen   = array( 'f', [ kINVALID_FLOAT ])
        self.Muon_Edep        = array( 'f', [ kINVALID_FLOAT ])

        self.Proton_id          = array( 'i', [ kINVALID_INT   ])
        self.Proton_PhiReco     = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_ThetaReco   = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_TrackLength = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_dQdx        = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_Trunc_dQdx1 = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_Trunc_dQdx3 = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_IonPerLen   = array( 'f', [ kINVALID_FLOAT ])
        self.Proton_Edep        = array( 'f', [ kINVALID_FLOAT ])

        #
        # pid related
        # 
        self.inferred     = array( 'i', [ kINVALID_INT   ])
        self.plane        = array( 'i', [ kINVALID_INT   ])
        self.eminus_score = array( 'f', [ kINVALID_FLOAT ])
        self.gamma_score  = array( 'f', [ kINVALID_FLOAT ])
        self.muon_score   = array( 'f', [ kINVALID_FLOAT ])
        self.pion_score   = array( 'f', [ kINVALID_FLOAT ])
        self.proton_score = array( 'f', [ kINVALID_FLOAT ])



    def reset(self):

        self.run[0]        = kINVALID_INT
        self.subrun[0]     = kINVALID_INT
        self.event[0]      = kINVALID_INT
   
        self.num_croi[0]   = kINVALID_INT
        self.num_vertex[0] = kINVALID_INT
        self.vertex_id[0]  = kINVALID_INT
        
        self.reco_selected[0] = kINVALID_INT
        self.reco_close[0] = kINVALID_INT
        self.reco_on_nu[0] = kINVALID_INT

        # LL
        self.LL_dist[0] = kINVALID_FLOAT
        self.LLc_e[0]   = kINVALID_FLOAT
        self.LLc_p[0]   = kINVALID_FLOAT
        self.LLe_e[0]   = kINVALID_FLOAT
        self.LLe_p[0]   = kINVALID_FLOAT

        # mc
        self.true_vertex[0]   = kINVALID_FLOAT
        self.true_vertex[1]   = kINVALID_FLOAT
        self.true_vertex[2]   = kINVALID_FLOAT
        self.selected1L1P[0]  = kINVALID_INT
        self.scedr[0]         = -1.0*kINVALID_FLOAT
        self.nu_pdg[0]        = kINVALID_INT
        self.true_nu_E[0]     = kINVALID_FLOAT

        self.inter_type[0]     = kINVALID_INT
        self.inter_mode[0]     = kINVALID_INT

        self.true_proton_E[0]   = kINVALID_FLOAT
        self.true_lepton_E[0]   = kINVALID_FLOAT
        self.true_electron_E[0] = kINVALID_FLOAT

        self.true_proton_dR[0]    = kINVALID_FLOAT
        self.true_proton_dR[1]    = kINVALID_FLOAT
        self.true_proton_dR[2]    = kINVALID_FLOAT
        self.true_electron_dR[0]  = kINVALID_FLOAT
        self.true_electron_dR[1]  = kINVALID_FLOAT
        self.true_electron_dR[2]  = kINVALID_FLOAT

        self.true_proton_theta[0]   = kINVALID_FLOAT
        self.true_proton_phi[0]     = kINVALID_FLOAT

        self.true_electron_theta[0] = kINVALID_FLOAT
        self.true_electron_phi[0]   = kINVALID_FLOAT

        self.true_muon_theta[0] = kINVALID_FLOAT
        self.true_muon_phi[0]   = kINVALID_FLOAT

        self.true_opening_angle[0] = kINVALID_FLOAT 
        self.true_proton_ylen[0]   = kINVALID_FLOAT

        # reco

        self.reco_mc_proton_E[0]   = kINVALID_FLOAT
        self.reco_mc_electron_E[0] = kINVALID_FLOAT
        self.reco_mc_total_E[0]    = kINVALID_FLOAT
        
        self.reco_proton_id[0]    = kINVALID_INT
        self.reco_proton_E[0]     = kINVALID_FLOAT
        self.reco_proton_theta[0] = kINVALID_FLOAT
        self.reco_proton_phi[0]   = kINVALID_FLOAT
        self.reco_proton_dEdx[0]  = kINVALID_FLOAT
        self.reco_proton_len[0]   = kINVALID_FLOAT

        self.reco_electron_id[0]    = kINVALID_INT
        self.reco_electron_E[0]     = kINVALID_FLOAT
        self.reco_electron_theta[0] = kINVALID_FLOAT
        self.reco_electron_phi[0]   = kINVALID_FLOAT
        self.reco_electron_dEdx[0]  = kINVALID_FLOAT
        self.reco_electron_len[0]   = kINVALID_FLOAT

        self.reco_total_E[0] = kINVALID_FLOAT
        self.reco_vertex[0]  = kINVALID_FLOAT
        self.reco_vertex[1]  = kINVALID_FLOAT
        self.reco_vertex[2]  = kINVALID_FLOAT

        #
        # numu related
        #
    
        self.CosmicLL[0]      = kINVALID_FLOAT
        self.NuBkgLL[0]       = kINVALID_FLOAT
        self.PassCuts[0]      = kINVALID_INT
        self.VtxAlgo[0]       = kINVALID_INT
        self.NTracks[0]       = kINVALID_INT
        self.N5cmTracks[0]    = kINVALID_INT
        self.InFiducial[0]    = kINVALID_INT
        self.Good3DReco[0]    = kINVALID_INT
        self.AnythingRecod[0] = kINVALID_INT
        
        self.Muon_id[0]          = kINVALID_INT;
        self.Muon_PhiReco[0]     = kINVALID_FLOAT;
        self.Muon_ThetaReco[0]   = kINVALID_FLOAT;
        self.Muon_TrackLength[0] = kINVALID_FLOAT;
        self.Muon_dQdx[0]        = kINVALID_FLOAT;
        self.Muon_Trunc_dQdx1[0] = kINVALID_FLOAT;
        self.Muon_Trunc_dQdx3[0] = kINVALID_FLOAT;
        self.Muon_IonPerLen[0]   = kINVALID_FLOAT;
        self.Muon_Edep[0]        = kINVALID_FLOAT;        

        self.Proton_id[0]          = kINVALID_INT;
        self.Proton_PhiReco[0]     = kINVALID_FLOAT;
        self.Proton_ThetaReco[0]   = kINVALID_FLOAT;
        self.Proton_TrackLength[0] = kINVALID_FLOAT;
        self.Proton_dQdx[0]        = kINVALID_FLOAT;
        self.Proton_Trunc_dQdx1[0] = kINVALID_FLOAT;
        self.Proton_Trunc_dQdx3[0] = kINVALID_FLOAT;
        self.Proton_IonPerLen[0]   = kINVALID_FLOAT;
        self.Proton_Edep[0]        = kINVALID_FLOAT;
        
        
        #
        # PID related
        #
        self.inferred[0]     = kINVALID_INT;
        self.plane[0]        = kINVALID_INT;
        self.eminus_score[0] = kINVALID_FLOAT;
        self.gamma_score[0]  = kINVALID_FLOAT;
        self.muon_score[0]   = kINVALID_FLOAT;
        self.pion_score[0]   = kINVALID_FLOAT;
        self.proton_score[0] = kINVALID_FLOAT;
        

    def init_nue_tree(self,tree):
        
        tree.Branch("run"   , self.run   , "run/I")
        tree.Branch("subrun", self.subrun, "subrun/I")
        tree.Branch("event" , self.event , "event/I")

        tree.Branch("num_croi"  , self.num_croi,   "num_croi/I")
        tree.Branch("num_vertex", self.num_vertex, "num_vertex/I")
        tree.Branch("vertex_id" , self.vertex_id,  "vertex_id/I")

        # truth 
        tree.Branch("nu_pdg"      , self.nu_pdg      , "nu_pdg/I")
        tree.Branch("selected1L1P", self.selected1L1P, "selected1L1P/I")
        tree.Branch("true_nu_E"   , self.true_nu_E   , "true_nu_E/F")
        tree.Branch("true_vertex" , self.true_vertex , "true_vertex[3]/F")

        tree.Branch("inter_type", self.inter_type, "inter_type/I")
        tree.Branch("inter_mode", self.inter_mode, "inter_mode/I")

        tree.Branch("true_proton_E"  , self.true_proton_E  , "true_proton_E/F")
        tree.Branch("true_electron_E", self.true_electron_E, "true_electron_E/F")
        tree.Branch("true_lepton_E"  , self.true_lepton_E  , "true_lepton_E/F")

        tree.Branch("true_proton_theta"  , self.true_proton_theta  , "true_proton_theta/F")
        tree.Branch("true_electron_theta", self.true_electron_theta, "true_electron_theta/F")

        tree.Branch("true_proton_phi"    , self.true_proton_phi    , "true_proton_phi/F")
        tree.Branch("true_electron_phi"  , self.true_electron_phi  , "true_electron_phi/F")

        tree.Branch("true_opening_angle", self.true_opening_angle, "true_opening_angle/F")
        tree.Branch("true_proton_ylen"  , self.true_proton_ylen  , "true_proton_ylen/F")

        # reco mc
        tree.Branch("reco_mc_proton_E"  , self.reco_mc_proton_E  , "reco_mc_proton_E/F")
        tree.Branch("reco_mc_electron_E", self.reco_mc_electron_E, "reco_mc_electron_E/F")
        tree.Branch("reco_mc_total_E"   , self.reco_mc_total_E   , "reco_mc_total_E/F")

        tree.Branch("reco_selected", self.reco_selected, "reco_selected/I")
        tree.Branch("reco_close"   , self.reco_close   , "reco_close/I")
        tree.Branch("reco_on_nu"   , self.reco_on_nu   , "reco_on_nu/I")
        tree.Branch("scedr"        , self.scedr        , "scedr/F")

        tree.Branch("reco_vertex" , self.reco_vertex , "reco_vertex[3]/F")

        # LL
        tree.Branch("LL_dist", self.LL_dist, "LL_dist/F")
        tree.Branch("LLc_e"  , self.LLc_e  , "LLc_e/F")
        tree.Branch("LLc_p"  , self.LLc_p  , "LLc_p/F")
        tree.Branch("LLe_e"  , self.LLe_e  , "LLe_e/F")
        tree.Branch("LLe_p"  , self.LLe_p  , "LLe_p/F")

        # reco track
        tree.Branch("reco_proton_id"    , self.reco_proton_id   , "reco_proton_id/I")
        tree.Branch("reco_proton_E"     , self.reco_proton_E    , "reco_proton_E/F")
        tree.Branch("reco_proton_theta" , self.reco_proton_theta, "reco_proton_theta/F")
        tree.Branch("reco_proton_phi"   , self.reco_proton_phi  , "reco_proton_phi/F")
        tree.Branch("reco_proton_dEdx"  , self.reco_proton_dEdx , "reco_proton_dEdx/F")
        tree.Branch("reco_proton_len"   , self.reco_proton_len  , "reco_proton_len/F")

        tree.Branch("reco_electron_id"    , self.reco_electron_id   , "reco_electron_id/I")
        tree.Branch("reco_electron_E"     , self.reco_electron_E    , "reco_electron_E/F")
        tree.Branch("reco_electron_theta" , self.reco_electron_theta, "reco_electron_theta/F")
        tree.Branch("reco_electron_phi"   , self.reco_electron_phi  , "reco_electron_phi/F")
        tree.Branch("reco_electron_dEdx"  , self.reco_electron_dEdx , "reco_electron_dEdx/F")
        tree.Branch("reco_electron_len"   , self.reco_electron_len  , "reco_electron_len/F")

        # reco combined
        tree.Branch("reco_total_E", self.reco_total_E, "reco_total_E/F")
        
        # reco PID
        tree.Branch("pid_inferred", self.inferred    , "pid_inferred/I")
        tree.Branch("pid_plane"   , self.plane       , "pid_plane/I") 
        tree.Branch("eminus_score", self.eminus_score, "eminus_score/F")
        tree.Branch("gamma_score" , self.gamma_score , "gamma_score/F")
        tree.Branch("muon_score"  , self.muon_score  , "muon_score/F")
        tree.Branch("pion_score"  , self.pion_score  , "pion_score/F")
        tree.Branch("proton_score", self.proton_score, "proton_score/F")


    def init_numu_tree(self,tree):
        
        tree.Branch("run"   , self.run   , "run/I")
        tree.Branch("subrun", self.subrun, "subrun/I")
        tree.Branch("event" , self.event , "event/I")

        tree.Branch("num_croi"  , self.num_croi,   "num_croi/I")
        tree.Branch("num_vertex", self.num_vertex, "num_vertex/I")
        tree.Branch("vertex_id" , self.vertex_id,  "vertex_id/I")

        tree.Branch("nu_pdg"      , self.nu_pdg      , "nu_pdg/I")
        tree.Branch("selected1L1P", self.selected1L1P, "selected1L1P/I")
        tree.Branch("true_nu_E"   , self.true_nu_E   , "true_nu_E/F")
        tree.Branch("true_vertex" , self.true_vertex , "true_vertex[3]/F")

        tree.Branch("inter_type", self.inter_type, "inter_type/I")
        tree.Branch("inter_mode", self.inter_mode, "inter_mode/I")

        tree.Branch("true_proton_E" , self.true_proton_E , "true_proton_E/F")
        tree.Branch("true_lepton_E" , self.true_lepton_E , "true_lepton_E/F")
        
        tree.Branch("true_proton_theta" , self.true_proton_theta , "true_proton_theta/F")
        tree.Branch("true_muon_theta"   , self.true_muon_theta   , "true_muon_theta/F")

        tree.Branch("true_proton_phi" , self.true_proton_phi , "true_proton_phi/F")
        tree.Branch("true_muon_phi"   , self.true_muon_phi   , "true_muon_phi/F")

        tree.Branch("reco_selected", self.reco_selected, "reco_selected/I")
        tree.Branch("reco_close"   , self.reco_close   , "reco_close/I")
        tree.Branch("reco_on_nu"   , self.reco_on_nu   , "reco_on_nu/I")
        tree.Branch("scedr"        , self.scedr        , "scedr/F")

        tree.Branch("reco_vertex" , self.reco_vertex , "reco_vertex[3]/F")

        tree.Branch("CosmicLL"     , self.CosmicLL     , "CosmicLL/F")
        tree.Branch("NuBkgLL"      , self.NuBkgLL      , "NuBkgLL/F")
        tree.Branch("PassCuts"     , self.PassCuts     , "PassCuts/I")
        tree.Branch("VtxAlgo"      , self.VtxAlgo      , "VtxAlgo/I")
        tree.Branch("NTracks"      , self.NTracks      , "NTracks/I")
        tree.Branch("N5cmTracks"   , self.N5cmTracks   , "N5cmTracks/I")
        tree.Branch("InFiducial"   , self.InFiducial   , "InFiducial/I")
        tree.Branch("Good3DReco"   , self.Good3DReco   , "Good3DReco/I")
        tree.Branch("AnythingRecod", self.AnythingRecod, "AnythingRecod/I")

        tree.Branch("Muon_id"          , self.Muon_id          , "Muon_id/I")
        tree.Branch("Muon_PhiReco"     , self.Muon_PhiReco     , "Muon_PhiReco/F")
        tree.Branch("Muon_ThetaReco"   , self.Muon_ThetaReco   , "Muon_ThetaReco/F")
        tree.Branch("Muon_TrackLength" , self.Muon_TrackLength , "Muon_TrackLength/F")
        tree.Branch("Muon_dQdx"        , self.Muon_dQdx        , "Muon_dQdx/F")
        tree.Branch("Muon_Trunc_dQdx1" , self.Muon_Trunc_dQdx1 , "Muon_Trunc_dQdx1/F")
        tree.Branch("Muon_Trunc_dQdx3" , self.Muon_Trunc_dQdx3 , "Muon_Trunc_dQdx3/F")
        tree.Branch("Muon_IonPerLen"   , self.Muon_IonPerLen   , "Muon_IonPerLen/F")
        tree.Branch("Muon_E"           , self.Muon_Edep        , "Muon_E/F")

        tree.Branch("Proton_id"          , self.Proton_id          , "Proton_id/I")
        tree.Branch("Proton_PhiReco"     , self.Proton_PhiReco     , "Proton_PhiReco/F")
        tree.Branch("Proton_ThetaReco"   , self.Proton_ThetaReco   , "Proton_ThetaReco/F")
        tree.Branch("Proton_TrackLength" , self.Proton_TrackLength , "Proton_TrackLength/F")
        tree.Branch("Proton_dQdx"        , self.Proton_dQdx        , "Proton_dQdx/F")
        tree.Branch("Proton_Trunc_dQdx1" , self.Proton_Trunc_dQdx1 , "Proton_Trunc_dQdx1/F")
        tree.Branch("Proton_Trunc_dQdx3" , self.Proton_Trunc_dQdx3 , "Proton_Trunc_dQdx3/F")
        tree.Branch("Proton_IonPerLen"   , self.Proton_IonPerLen   , "Proton_IonPerLen/F")
        tree.Branch("Proton_E"           , self.Proton_Edep        , "Proton_E/F")

        tree.Branch("pid_inferred", self.inferred    , "pid_inferred/I")
        tree.Branch("pid_plane"   , self.plane       , "pid_plane/I") 
        tree.Branch("eminus_score", self.eminus_score, "eminus_score/F")
        tree.Branch("gamma_score" , self.gamma_score , "gamma_score/F")
        tree.Branch("muon_score"  , self.muon_score  , "muon_score/F")
        tree.Branch("pion_score"  , self.pion_score  , "pion_score/F")
        tree.Branch("proton_score", self.proton_score, "proton_score/F")
