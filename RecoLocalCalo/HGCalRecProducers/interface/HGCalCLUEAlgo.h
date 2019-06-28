#ifndef RecoLocalCalo_HGCalRecProducers_HGCalCLUEAlgo_h
#define RecoLocalCalo_HGCalRecProducers_HGCalCLUEAlgo_h

#include "RecoLocalCalo/HGCalRecProducers/interface/HGCalClusteringAlgoBase.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "Geometry/CaloTopology/interface/HGCalTopology.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "RecoLocalCalo/HGCalRecProducers/interface/HGCalLayerTiles.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "RecoLocalCalo/HGCalRecProducers/interface/HGCalCLUEAlgoGPURunner.cuh"

// C/C++ headers
#include <set>
#include <string>
#include <vector>



using Density=hgcal_clustering::Density;

class HGCalCLUEAlgo : public HGCalClusteringAlgoBase {
 public:

 HGCalCLUEAlgo(const edm::ParameterSet& ps)
  : HGCalClusteringAlgoBase(
      (HGCalClusteringAlgoBase::VerbosityLevel)ps.getUntrackedParameter<unsigned int>("verbosity",3),
      reco::CaloCluster::undefined),
     thresholdW0_(ps.getParameter<std::vector<double> >("thresholdW0")),
     vecDeltas_(ps.getParameter<std::vector<double> >("deltac")),
     kappa_(ps.getParameter<double>("kappa")),
     ecut_(ps.getParameter<double>("ecut")),
     dependSensor_(ps.getParameter<bool>("dependSensor")),
     dEdXweights_(ps.getParameter<std::vector<double> >("dEdXweights")),
     thicknessCorrection_(ps.getParameter<std::vector<double> >("thicknessCorrection")),
     fcPerMip_(ps.getParameter<std::vector<double> >("fcPerMip")),
     fcPerEle_(ps.getParameter<double>("fcPerEle")),
     nonAgedNoises_(ps.getParameter<edm::ParameterSet>("noises").getParameter<std::vector<double> >("values")),
     noiseMip_(ps.getParameter<edm::ParameterSet>("noiseMip").getParameter<double>("noise_MIP")),
     initialized_(false),
     layerTiles_(2*(maxlayer+1)),
     cells_(2*(maxlayer+1)),
     numberOfClustersPerLayer_(2*(maxlayer+1),0)
     {}

  ~HGCalCLUEAlgo() override {}

  void populate(const HGCRecHitCollection &hits) override;

  // this is the method that will start the clusterisation (it is possible to invoke this method
  // more than once - but make sure it is with different hit collections (or else use reset)

  void makeClusters() override;

  // this is the method to get the cluster collection out
  std::vector<reco::BasicCluster> getClusters(bool) override;

  void reset() override {
    clusters_v_.clear();
    for(auto& cl: numberOfClustersPerLayer_)
    {
      cl = 0;
    }

    for(auto& cells : cells_)
      cells.clear();
    
    for(auto& tiles: layerTiles_)
      tiles.clear();

  }

  Density getDensity() override;

  void computeThreshold();

  static void fillPSetDescription(edm::ParameterSetDescription& iDesc) {
    iDesc.add<std::vector<double>>("thresholdW0", {
        2.9,
        2.9,
        2.9
        });
    iDesc.add<std::vector<double>>("deltac", {
        1.3,
        1.3,
        5.0,
        });
    iDesc.add<bool>("dependSensor", true);
    iDesc.add<double>("ecut", 3.0);
    iDesc.add<double>("kappa", 9.0);
    iDesc.addUntracked<unsigned int>("verbosity", 3);
    iDesc.add<std::vector<double>>("dEdXweights",{});
    iDesc.add<std::vector<double>>("thicknessCorrection",{});
    iDesc.add<std::vector<double>>("fcPerMip",{});
    iDesc.add<double>("fcPerEle",0.0);
    edm::ParameterSetDescription descNestedNoises;
    descNestedNoises.add<std::vector<double> >("values", {});
    iDesc.add<edm::ParameterSetDescription>("noises", descNestedNoises);
    edm::ParameterSetDescription descNestedNoiseMIP;
    descNestedNoiseMIP.add<bool>("scaleByDose", false );
    iDesc.add<edm::ParameterSetDescription>("scaleByDose", descNestedNoiseMIP);
    descNestedNoiseMIP.add<std::string>("doseMap", "" );
    iDesc.add<edm::ParameterSetDescription>("doseMap", descNestedNoiseMIP);
    descNestedNoiseMIP.add<double>("noise_MIP", 1./100. );
    iDesc.add<edm::ParameterSetDescription>("noiseMip", descNestedNoiseMIP);
  }

  /// point in the space
  typedef math::XYZPoint Point;

 private:
  // To compute the cluster position
  std::vector<double> thresholdW0_;

  // The two parameters used to identify clusters
  std::vector<double> vecDeltas_;
  double kappa_;

  // The hit energy cutoff
  double ecut_;

  // For keeping the density per hit
  Density density_;

  // various parameters used for calculating the noise levels for a given sensor (and whether to use
  // them)
  bool dependSensor_;
  std::vector<double> dEdXweights_;
  std::vector<double> thicknessCorrection_;
  std::vector<double> fcPerMip_;
  double fcPerEle_;
  std::vector<double> nonAgedNoises_;
  double noiseMip_;
  std::vector<std::vector<double> > thresholds_;
  std::vector<std::vector<double> > v_sigmaNoise_;

  ClueGPURunner gpuRunner;

  // initialization bool
  bool initialized_;

  double outlierDeltaFactor_ = 2;

  
  //this are the tiles for the electromagnetic part
  std::vector<HGCalLayerTiles> layerTiles_;
  
  std::vector<CellsOnLayer> cells_;
  

  std::vector<int> numberOfClustersPerLayer_;

  inline float distance2(int cell1, int cell2, int layerId) const {  // distance squared
    const float dx = cells_[layerId].x[cell1] - cells_[layerId].x[cell2];
    const float dy = cells_[layerId].y[cell1] - cells_[layerId].y[cell2];
    return (dx * dx + dy * dy);
  }  

  inline float distance(int cell1,
                         int cell2, int layerId) const {  // 2-d distance on the layer (x-y)
    return std::sqrt(distance2(cell1, cell2, layerId));
  }
  
  void prepareAlgorithmVariables(const unsigned int layerId);
  void calculateLocalDensity(const unsigned int layerId, float delta_c);  // return max density
  void calculateDistanceToHigher(const unsigned int layerId, float delta_c);
  int findAndAssignClusters(const unsigned int layerId, float delta_c);
  math::XYZPoint calculatePosition(const std::vector<int> &v, const unsigned int layerId) const;
  void setDensity(const unsigned int layerId);

};

#endif