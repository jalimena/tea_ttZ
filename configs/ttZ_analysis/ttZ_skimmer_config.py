nEvents = -1
printEveryNevents = 1000

inputFilePath = "../samples/background_dy.root"
treeOutputFilePath = "../samples/skimmed/background_dy.root"

weightsBranchName = "genWeight"

#triggerSelection = (
#    "HLT_IsoMu24",
#)

#extraEventCollections = {
#    "GoodLeptons": {
#        "inputCollections": ("Muon", "Electron"),
#        "pt": (30., 9999999.),
#        "eta": (-2.4, 2.4),
#    },
#}

eventSelections = {
    #"MET_pt": (30, 9999999),
    "nMuon": (1, 9999999),
    #"nGoodLeptons": (1, 9999999),
}
