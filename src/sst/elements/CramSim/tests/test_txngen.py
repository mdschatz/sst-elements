import sst
import sys
import time
from util import *

#######################################################################################################

# Command line arguments
g_config_file = ""
g_override_list = ""

# Setup global parameters
[g_config_file, g_overrided_list] = read_arguments()
g_params = setup_config_params(g_config_file, g_overrided_list)

numChannels = int(g_params["numChannels"])
maxOutstandingReqs = numChannels*64
numTxnPerCycle = numChannels
maxTxns = 100000 * numChannels


# Define SST core options
sst.setProgramOption("timebase", g_params["clockCycle"])
sst.setProgramOption("stopAtCycle", g_params["stopAtCycle"])
sst.setStatisticLoadLevel(7)
sst.setStatisticOutput("sst.statOutputConsole")


#########################################################################################################

## Configure transaction generator
comp_txnGen = sst.Component("TxnGen", "CramSim.c_TxnGen")
comp_txnGen.addParams(g_params)
comp_txnGen.addParams({
	"maxTxns" : maxTxns,
	"numTxnPerCycle" : numTxnPerCycle,
	"maxOutstandingReqs" : maxOutstandingReqs,
	"readWriteRatio" : 0.5
	})
comp_txnGen.enableAllStatistics()


# controller
comp_controller = sst.Component("MemController"+"0", "CramSim.c_Controller")
comp_controller.addParams(g_params)
comp_controller.addParams({
		"TxnScheduler" : "CramSim.c_TxnScheduler",
		"TxnConverter" : "CramSim.c_TxnConverter",
		"AddrHasher" : "CramSim.c_AddressHasher",
		"CmdScheduler" : "CramSim.c_CmdScheduler" ,
		"DeviceDriver" : "CramSim.c_DeviceDriver"
		})

# device
comp_dimm = sst.Component("Dimm"+"0", "CramSim.c_Dimm")
comp_dimm.addParams(g_params)

# TXNGEN / Controller LINKS
# TxnGen -> Controller (Req)(Txn)
txnReqLink_0 = sst.Link("txnReqLink_0_"+"0")
txnReqLink_0.connect((comp_txnGen, "memLink", g_params["clockCycle"]), (comp_controller, "txngenLink", g_params["clockCycle"]) )

# Controller -> Dimm (Req)
cmdReqLink_1 = sst.Link("cmdReqLink_1_"+"0")
cmdReqLink_1.connect( (comp_controller, "memLink", g_params["clockCycle"]), (comp_dimm, "ctrlLink", g_params["clockCycle"]) )


# enable all statistics
comp_controller.enableAllStatistics()
#comp_txnUnit0.enableAllStatistics({ "type":"sst.AccumulatorStatistic",
#                                    "rate":"1 us"})
#comp_dimm.enableAllStatistics()
