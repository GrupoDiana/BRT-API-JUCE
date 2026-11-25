
/**
* \class CConfiguratorInterface
*
* \brief Declaration of CConfiguratorInterface class
* \date November 2026
*
* \authors 3DI-DIANA Research Group (University of Malaga) 
* \b Contact: areyes@uma.es
*
* \b Copyright: University of Malaga
*
* \b Contributions: (additional authors/contributors can be added here)
*
* \b Project: BRT API JUCE
*
* \b Description: Abstract base class defining the common acoustic
* configuration structure for BRT-based audio plugins. It manages two
* listener models: CListenerDirectHRTFConvolutionModel for direct sound
* binaural rendering and CListenerAmbisonicReverberantVirtualLoudspeakersModel
* for reverberation processing. It also manages a CFreeFieldEnvironmentModel
* to represent the acoustic environment. Additionally, it provides common
* functionality for listener positioning, sound source management and
* loading acoustic resources, including HRTF, BRIR and near-field
* compensation filters. Specific configurations implement the setup and
* sound source connection procedures.
*
* \b Licence: This program is free software, you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/



#include <BRTLibrary.h>
#include "AppUtils.hpp"

#ifndef _CONFIGURATOR_INTERFACE_HPP_
#define _CONFIGURATOR_INTERFACE_HPP_

#define LISTENER_HRTF_MODEL_ID "listenerHRTF1"
#define LISTENER_REVERB_MODEL_ID "listenerREVERB1"
#define ENVIRONMENT_FREEFIELD_MODEL_ID "FreeField"
#define LISTENER_ID "defaultListener"

namespace BRTJUCE {
class CConfiguratorInterface {

public:

    CConfiguratorInterface() : lastErrorMessage{""}, listener{ nullptr }{};

    virtual ~CConfiguratorInterface() = default;

    virtual void Setup() = 0;

    virtual bool ConnectSoundSource(const std::string& _soundSourceID) = 0;

	std::string GetLastErrorMessage() const {
		return lastErrorMessage;
	}

    void SetListenerPosition(float x, float y, float z) {

        if (listener == nullptr) {
            return;
        }

        Common::CTransform listenerPosition = listener->GetListenerTransform();
        listenerPosition.SetPosition(Common::CVector3(x, y, z));
        listener->SetListenerTransform(listenerPosition);
    }

    BRTBase::CBRTManager& GetBRTManager() {
        return brtManager;
    }

    std::shared_ptr<BRTSourceModel::CSourceOmnidirectionalModel>CreateSoundSource(const std::string& _soundSourceID)
    {
        brtManager.BeginSetup();

        auto source = brtManager.CreateSoundSource<BRTSourceModel::CSourceOmnidirectionalModel>(_soundSourceID);

        brtManager.EndSetup();

        return source;
    }

    template<typename T>
    bool LoadHRTF(std::shared_ptr<T> listenerModel, const std::string& filePath) {

        std::shared_ptr<BRTServices::CSphericalInterpolatedFIRTable> hrtf = std::make_shared<BRTServices::CSphericalInterpolatedFIRTable>();

        if (listenerModel == nullptr) {
            lastErrorMessage = "Listener model not found";
            return false;
        }

        bool hrtfSofaLoaded = AppUtils::LoadHRTFSofaFile(filePath, hrtf);

        if (hrtfSofaLoaded) {
            listenerModel->SetHRTF(hrtf);
            return true;
        }
        else {
            lastErrorMessage = "Failed to load HRTF";
        }
        return false;
    }

    template<typename T>
    bool LoadBRIR(std::shared_ptr<T> listenerModel, const std::string& filePath, float _fadeIn, float _riseTime, float _fadeOut, float _fallTime)
    {
        auto brir = std::make_shared<BRTServices::CSphericalFIRTable>();

        if (listenerModel == nullptr) {
            lastErrorMessage = "Listener model not found";
            return false;
        }

        bool brirLoaded = AppUtils::LoadBRIRSofaFile(filePath, brir, _fadeIn, _riseTime, _fadeOut, _fallTime);

        if (brirLoaded)
        {
            listenerModel->RemoveHRBRIR();
            listenerModel->SetHRBRIR(brir);
            return true;
        }
        else
        {
            lastErrorMessage = "Failed to load BRIR";
        }
        return false;
    }

    template<typename T>
    bool LoadNearfield(std::shared_ptr<T> listenerModel, const std::string& filePath)
    {
        std::shared_ptr<BRTServices::CSphericalSOSTable> sosFilter = std::make_shared<BRTServices::CSphericalSOSTable>();

        if (listenerModel == nullptr) {
            lastErrorMessage = "Listener model not found";
            return false;
        }

        bool nearFieldFilterLoaded = AppUtils::LoadNearFieldSOSFilter(filePath, sosFilter);

        if (nearFieldFilterLoaded) {
            listenerModel->SetNearFieldCompensationFilters(sosFilter);
            return true;
        }
        else {
            lastErrorMessage = "Failed to load near-field ILD filter";
        }
        return false;
    }

    std::shared_ptr<BRTBase::CListener> GetListener() {
        return listener;
    }

    std::shared_ptr<BRTSourceModel::CSourceOmnidirectionalModel> GetSoundSource(const std::string& _soundSourceID)
    {
        return std::dynamic_pointer_cast<BRTSourceModel::CSourceOmnidirectionalModel>(brtManager.GetSoundSource(_soundSourceID));
    }

private:
    


protected:

    ///  Attribues    
    BRTBase::CBRTManager brtManager;
    std::shared_ptr<BRTBase::CListener> listener;
	std::string lastErrorMessage;
};
}
#endif