#include "L1Trigger/L1THGCal/interface/be_algorithms/HGCalShowerShape.h"
#include "TMath.h"
#include <cmath>
#include "DataFormats/L1THGCal/interface/HGCalMulticluster.h"
#include "DataFormats/L1THGCal/interface/HGCalCluster.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>



const float HGCalShowerShape::sigmaEtaEta(std::vector<float> energy, std::vector<float> eta){

	int ntc=energy.size();
	//compute weighted eta mean
	float eta_sum=0;
	float w_sum=0; //here weight is energy
	for(int i=0;i<ntc;i++){
		eta_sum+=energy.at(i)*eta.at(i);
		w_sum+=energy.at(i);
	}
	float eta_mean=eta_sum/w_sum;
	//compute weighted eta RMS
	float deltaeta2_sum=0;
	for(int i=0;i<ntc;i++) deltaeta2_sum+=energy.at(i)*pow((eta.at(i)-eta_mean),2);
    float eta_RMS=0;
	if (w_sum!=0) eta_RMS=deltaeta2_sum/w_sum;
	float See=sqrt(eta_RMS);
	return See;
	
}


const float HGCalShowerShape::sigmaPhiPhi(std::vector<float> energy, std::vector<float> phi){

	int ntc=energy.size();
	//compute weighted phi mean
	float phi_sum=0;
	float w_sum=0; //here weight is energy
	for(int i=0;i<ntc;i++){
		if(phi.at(i)>0) phi_sum+=energy.at(i)*phi.at(i);
		else phi_sum+=energy.at(i)*(phi.at(i)+2*TMath::Pi());
		w_sum+=energy.at(i);
	}
	float phi_mean=phi_sum/w_sum;
	if(phi_mean>TMath::Pi()) phi_mean-=2*TMath::Pi();
	//compute weighted eta RMS
	float deltaphi2_sum=0;
	for(int i=0;i<ntc;i++){
		float deltaPhi=std::abs(phi.at(i)-phi_mean);
		if (deltaPhi>TMath::Pi()) deltaPhi=2*TMath::Pi()-deltaPhi;
		deltaphi2_sum+=energy.at(i)*pow(deltaPhi,2);
	}		
    float phi_RMS=0;
	if (w_sum!=0) phi_RMS=deltaphi2_sum/w_sum;
	float Spp=sqrt(phi_RMS);
	return Spp;
	
}



const float HGCalShowerShape::sigmaZZ(std::vector<float> energy, std::vector<float> z){

	int ntc=energy.size();
	//compute weighted eta mean
	float z_sum=0;
	float w_sum=0; //here weight is energy
	for(int i=0;i<ntc;i++){
		z_sum+=energy.at(i)*z.at(i);
		w_sum+=energy.at(i);
	}
	float z_mean=z_sum/w_sum;
	//compute weighted eta RMS
	float deltaz2_sum=0;
	for(int i=0;i<ntc;i++) deltaz2_sum+=energy.at(i)*pow((z.at(i)-z_mean),2);
    float z_RMS=0;
	if (w_sum!=0) z_RMS=deltaz2_sum/w_sum;
	float Szz=sqrt(z_RMS);
	return Szz;
	
}


const int HGCalShowerShape::nLayers(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    int firstLayer=999;
    int lastLayer=-999;
    int layer=999;

    int nLayersEE=28;
   
    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        if((*clu)->subdetId()==HGCEE) layer=(*clu)->layer();//EE
        if((*clu)->subdetId()==HGCHEF) layer=(*clu)->layer()+nLayersEE;//FH

        if(layer<firstLayer) firstLayer=layer;
        if(layer>lastLayer) lastLayer=layer;

    }

	int nLayers=lastLayer-firstLayer+1;

    return nLayers;

}


const int HGCalShowerShape::firstLayer(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    int firstLayer=999;
    int layer=999;

    int nLayersEE=28;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        if((*clu)->subdetId()==HGCEE) layer=(*clu)->layer();//EE
        if((*clu)->subdetId()==HGCHEF) layer=(*clu)->layer()+nLayersEE;//FH

        if(layer<firstLayer) firstLayer=layer;

    }

    return firstLayer;

}




const float HGCalShowerShape::sigmaEtaEtaTot(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<float> tc_energy ; 
    std::vector<float> tc_eta ;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

	    const edm::PtrVector<l1t::HGCalTriggerCell> triggerCells = (*clu)->constituents();
    	unsigned int ncells = triggerCells.size();

		for(unsigned int itc=0; itc<ncells;itc++){

    		l1t::HGCalTriggerCell thistc = *triggerCells[itc];

        	tc_energy.emplace_back(thistc.energy());
        	tc_eta.emplace_back(thistc.eta());
        		
        }

    }

    float SeeTot = sigmaEtaEta(tc_energy,tc_eta);

    tc_energy.clear();
    tc_eta.clear();

    return SeeTot;

}




const float HGCalShowerShape::sigmaPhiPhiTot(const l1t::HGCalMulticluster& c3d){


    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<float> tc_energy ; 
    std::vector<float> tc_phi ;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

	    const edm::PtrVector<l1t::HGCalTriggerCell> triggerCells = (*clu)->constituents();
    	unsigned int ncells = triggerCells.size();

		for(unsigned int itc=0; itc<ncells;itc++){

    		l1t::HGCalTriggerCell thistc = *triggerCells[itc];

        	tc_energy.emplace_back(thistc.energy());
        	tc_phi.emplace_back(thistc.phi());
        		
	    }
    }

    float SppTot = sigmaPhiPhi(tc_energy,tc_phi);

    tc_energy.clear();
    tc_phi.clear();

    return SppTot;

}



const float HGCalShowerShape::sigmaEtaEtaMax(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<int> layer ; // Size : ncl2D
    std::vector<int> subdetID ;
    std::vector<int> nTC ;
    std::vector<float> tc_energy ; 
    std::vector<float> tc_eta ;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        layer.emplace_back((*clu)->layer());
        subdetID.emplace_back((*clu)->subdetId());

		const edm::PtrVector<l1t::HGCalTriggerCell> triggerCells = (*clu)->constituents();
    	unsigned int ncells = triggerCells.size();
		nTC.emplace_back(ncells);

		for(unsigned int itc=0; itc<ncells;itc++){

    		l1t::HGCalTriggerCell thistc = *triggerCells[itc];

        	tc_energy.emplace_back(thistc.energy());
        	tc_eta.emplace_back(thistc.eta());
        		
	    }

    }

    int ncl2D=layer.size();

    float SigmaEtaEtaMax=0;

    std::vector<float> energy_layer ;
    std::vector<float> eta_layer ;

    int nLayersMax=40;//EE+FH
    int nLayersEE=28;

	for(int ilayer=0;ilayer<nLayersMax;ilayer++){   //Loop on HGCal layers

        int Layer_found=0;
		float Layer_See=0;

		int tc_index=0; // trigger cell index inside cl2D vector

		for(int i2d=0;i2d<ncl2D;i2d++){   // Loop on cl2D inside 3DC
	
			int cl2D_layer=-999;

			if(subdetID.at(i2d)==HGCEE)cl2D_layer=layer.at(i2d);
			if(subdetID.at(i2d)==HGCHEF)cl2D_layer=layer.at(i2d)+nLayersEE;

			if (cl2D_layer==ilayer){

		    	Layer_found=1; //+=1 il want to count cl2D per layer

		    	int ntc=nTC.at(i2d);

		    	for(int itc=0; itc<ntc ; itc++) {   //Loop on TC inside cl2D

		    	    energy_layer.emplace_back(tc_energy.at(tc_index));
		    	    eta_layer.emplace_back(tc_eta.at(tc_index));
		    	    tc_index++;

                }

		    }

			else{

				if (Layer_found==1) break; //Go to next ilayer
				tc_index+=nTC.at(i2d);
		
			}

		}


    	if(Layer_found==1) Layer_See=sigmaEtaEta(energy_layer,eta_layer);	
	
		if(Layer_See>SigmaEtaEtaMax) SigmaEtaEtaMax=Layer_See;

	    energy_layer.clear();
		eta_layer.clear();
      
    }    

			
    return SigmaEtaEtaMax;


}   



const float HGCalShowerShape::sigmaPhiPhiMax(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<int> layer ; // Size : ncl2D
    std::vector<int> subdetID ;
    std::vector<int> nTC ;
    std::vector<float> tc_energy ; 
    std::vector<float> tc_phi ;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        layer.emplace_back((*clu)->layer());
        subdetID.emplace_back((*clu)->subdetId());

		const edm::PtrVector<l1t::HGCalTriggerCell> triggerCells = (*clu)->constituents();
    	unsigned int ncells = triggerCells.size();
		nTC.emplace_back(ncells);

		for(unsigned int itc=0; itc<ncells;itc++){

    		l1t::HGCalTriggerCell thistc = *triggerCells[itc];

        	tc_energy.emplace_back(thistc.energy());
        	tc_phi.emplace_back(thistc.phi());
        		
	    }

    }

    int ncl2D=layer.size();

    float SigmaPhiPhiMax=0;

    std::vector<float> energy_layer ;
    std::vector<float> phi_layer ;

    int nLayersMax=40;//EE+FH
    int nLayersEE=28;

	for(int ilayer=0;ilayer<nLayersMax;ilayer++){   //Loop on HGCal layers

        int Layer_found=0;
		float Layer_Spp=0;

		int tc_index=0; // trigger cell index inside cl2D vector

		for(int i2d=0;i2d<ncl2D;i2d++){   // Loop on cl2D inside 3DC
	
			int cl2D_layer=-999;

			if(subdetID.at(i2d)==HGCEE)cl2D_layer=layer.at(i2d);
			if(subdetID.at(i2d)==HGCHEF)cl2D_layer=layer.at(i2d)+nLayersEE;

			if (cl2D_layer==ilayer){

		    	Layer_found=1; //+=1 il want to count cl2D per layer

		    	int ntc=nTC.at(i2d);

		    	for(int itc=0; itc<ntc ; itc++) {   //Loop on TC inside cl2D

		    	    energy_layer.emplace_back(tc_energy.at(tc_index));
		    	    phi_layer.emplace_back(tc_phi.at(tc_index));
		    	    tc_index++;

                }

		    }

			else{

				if (Layer_found==1) break; //Go to next ilayer
				tc_index+=nTC.at(i2d);
		
			}

		}


    	if(Layer_found==1) Layer_Spp=sigmaPhiPhi(energy_layer,phi_layer);	
	
		if(Layer_Spp>SigmaPhiPhiMax) SigmaPhiPhiMax=Layer_Spp;

	    energy_layer.clear();
		phi_layer.clear();
      
    }    

			
    return SigmaPhiPhiMax;


}   



const float HGCalShowerShape::eMax(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<int> layer ; // Size : ncl2D
    std::vector<int> subdetID ;
    std::vector<float> energy ; 

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        layer.emplace_back((*clu)->layer());
        subdetID.emplace_back((*clu)->subdetId());
        energy.emplace_back((*clu)->energy());

    }

    int ncl2D=layer.size();

    float EMax=0;

    int nLayersMax=40;//EE+FH
    int nLayersEE=28;

	for(int ilayer=0;ilayer<nLayersMax;ilayer++){   //Loop on HGCal layers

		int Layer_found=0;
		float Layer_Energy=0;

		for(int i2d=0;i2d<ncl2D;i2d++){   // Loop on cl2D inside 3DC
	
			int cl2D_layer=-999;

			if(subdetID.at(i2d)==HGCEE)cl2D_layer=layer.at(i2d);
			if(subdetID.at(i2d)==HGCHEF)cl2D_layer=layer.at(i2d)+nLayersEE;

			if (cl2D_layer==ilayer){

				Layer_found=1; //+=1 il want to count cl2D per layer

				Layer_Energy+=energy.at(i2d);

			}

			else{

				if (Layer_found==1) break; //Go to next ilayer
		
			}

		}	
	
		if(Layer_Energy>EMax) EMax=Layer_Energy;

   }    

   return EMax;

}



const float HGCalShowerShape::sigmaZZ(const l1t::HGCalMulticluster& c3d){

    const edm::PtrVector<l1t::HGCalCluster> clustersPtrs = c3d.constituents();

    std::vector<float> tc_energy ; 
    std::vector<float> tc_z ;

    for(edm::PtrVector<l1t::HGCalCluster>::const_iterator clu = clustersPtrs.begin(); clu != clustersPtrs.end(); ++clu){

        const edm::PtrVector<l1t::HGCalTriggerCell> triggerCells = (*clu)->constituents();
    	unsigned int ncells = triggerCells.size();

		for(unsigned int itc=0; itc<ncells;itc++){

    		l1t::HGCalTriggerCell thistc = *triggerCells[itc];

        	tc_energy.emplace_back(thistc.energy());
        	tc_z.emplace_back(thistc.position().z());
        		
	    }
    
    }

    float Szz = sigmaZZ(tc_energy,tc_z);

    tc_energy.clear();
    tc_z.clear();

    return Szz;

}



const float HGCalShowerShape::sigmaEtaEtaTot(const l1t::HGCalCluster& c2d){

    const edm::PtrVector<l1t::HGCalTriggerCell> cellsPtrs = c2d.constituents();

    std::vector<float> tc_energy ; 
    std::vector<float> tc_eta ;

    for(edm::PtrVector<l1t::HGCalTriggerCell>::const_iterator cell = cellsPtrs.begin(); cell != cellsPtrs.end(); ++cell){

        tc_energy.emplace_back((*cell)->energy());
        tc_eta.emplace_back((*cell)->eta());
        		
    }

    float See = sigmaEtaEta(tc_energy,tc_eta);

    tc_energy.clear();
    tc_eta.clear();

    return See;

}       



const float HGCalShowerShape::sigmaPhiPhiTot(const l1t::HGCalCluster& c2d){

    const edm::PtrVector<l1t::HGCalTriggerCell> cellsPtrs = c2d.constituents();

    std::vector<float> tc_energy ; 
    std::vector<float> tc_phi ;

    for(edm::PtrVector<l1t::HGCalTriggerCell>::const_iterator cell = cellsPtrs.begin(); cell != cellsPtrs.end(); ++cell){

        	tc_energy.emplace_back((*cell)->energy());
        	tc_phi.emplace_back((*cell)->phi());
        		
    }

    float Spp = sigmaPhiPhi(tc_energy,tc_phi);

    tc_energy.clear();
    tc_phi.clear();

    return Spp;

}  


// -----------------------------------
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
