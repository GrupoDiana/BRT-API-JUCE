
/**
* \class CBasicConfiguration
*
* \brief Declaration of CBasicConfiguration class
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
* \b Description: Implements a basic acoustic configuration for BRT-based
* audio plugins. It initializes the listener, HRTF and reverberation models,
* configures the acoustic environment, manages sound source connections
* and loads the required acoustic resources from SOFA files.
*
* \b Licence: This program is free software, you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/


#include <BRTLibrary.h>
#include "BRT_API_Juce.h"

#include "OSC/OSCLogger.hpp"

#ifndef _BASIC_CONFIGURATION_HPP_
#define _BASIC_CONFIGURATION_HPP_

#define SOFA1_FILEPATH "3DTI_HRTF_SADIE_II_D2_256s_48000Hz.sofa"
#define SOFA2_FILEPATH "NearFieldCompensation_ILD_1.2m_48Khz.sofa"
#define SOFA3_FILEPATH "BRIR_Iglesia_9m.sofa"

namespace BRTJUCE {
    class CBasicConfiguration : public CConfiguratorInterface
    {
    public:
        CBasicConfiguration() = default;

        void Setup(OSCLogger& logger) override {

            brtManager.BeginSetup();

            listener = brtManager.CreateListener<BRTBase::CListener>(LISTENER_ID);

            listenerHRTFModel = brtManager.CreateListenerModel<BRTListenerModel::CListenerDirectHRTFConvolutionModel>(LISTENER_HRTF_MODEL_ID);
            listenerReverbModel = brtManager.CreateListenerModel<BRTListenerModel::CListenerAmbisonicReverberantVirtualLoudspeakersModel>(LISTENER_REVERB_MODEL_ID);
            environmentModel = brtManager.CreateEnvironment<BRTEnvironmentModel::CFreeFieldEnvironmentModel>(ENVIRONMENT_FREEFIELD_MODEL_ID);

            bool control = listener->ConnectListenerModel(LISTENER_HRTF_MODEL_ID);
            if (!control) {
                lastErrorMessage = "Listener HRTF connection failed";
            }

            control = listenerHRTFModel->ConnectEnvironmentModel(ENVIRONMENT_FREEFIELD_MODEL_ID);
            if (!control) {
                lastErrorMessage = "Listener environment model connection failed";
            }

            control = listener->ConnectListenerModel(LISTENER_REVERB_MODEL_ID);
            if (!control) {
                lastErrorMessage = "Listener Reverb connection failed";
            }

            environmentModel->EnableDistanceAttenuation();
            environmentModel->EnablePropagationDelay();

            brtManager.EndSetup();
        }

        std::shared_ptr<BRTSourceModel::CSourceOmnidirectionalModel>CreateAndConnectSoundSource(const std::string& _soundSourceID)
        {
            auto soundSource = CConfiguratorInterface::CreateSoundSource(_soundSourceID);

            if (soundSource == nullptr)
                return nullptr;

            if (!ConnectSoundSource(_soundSourceID))
                return nullptr;

            return soundSource;
        }

        //LoadResources
        bool LoadResources(OSCLogger& logger) {

            auto resourcesPath = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getParentDirectory().getChildFile("Resources");

            bool hrtfLoaded = LoadHRTF<BRTListenerModel::CListenerDirectHRTFConvolutionModel>(listenerHRTFModel, resourcesPath.getChildFile(SOFA1_FILEPATH).getFullPathName().toStdString(), logger);
            bool nearfieldLoaded = LoadNearfield<BRTListenerModel::CListenerDirectHRTFConvolutionModel>(listenerHRTFModel, resourcesPath.getChildFile(SOFA2_FILEPATH).getFullPathName().toStdString(), logger);
            bool brirLoaded = LoadBRIR<BRTListenerModel::CListenerAmbisonicReverberantVirtualLoudspeakersModel>(listenerReverbModel, resourcesPath.getChildFile(SOFA3_FILEPATH).getFullPathName().toStdString(), 0.0f, 0.0f, 0.0f, 0.0f, logger);

            return hrtfLoaded && nearfieldLoaded && brirLoaded;
        };

        bool ConnectSoundSource(const std::string& _soundSourceID) override {

            if (environmentModel != nullptr && listenerReverbModel != nullptr) {

                brtManager.BeginSetup();

                environmentModel->ConnectSoundSource(_soundSourceID);
                listenerReverbModel->ConnectSoundSource(_soundSourceID);

                brtManager.EndSetup();
				return true;
            }
			return false;
        }

        //Getter modelos
        std::shared_ptr<BRTListenerModel::CListenerDirectHRTFConvolutionModel> GetListenerHRTFModel() {
            return listenerHRTFModel;
        }

        std::shared_ptr<BRTListenerModel::CListenerAmbisonicReverberantVirtualLoudspeakersModel> GetListenerReverbModel() {
            return listenerReverbModel;
        }

        std::shared_ptr<BRTEnvironmentModel::CFreeFieldEnvironmentModel> GetEnvironmentModel() {
            return environmentModel;
        }

    private:
        std::shared_ptr<BRTListenerModel::CListenerDirectHRTFConvolutionModel> listenerHRTFModel;
        std::shared_ptr<BRTListenerModel::CListenerAmbisonicReverberantVirtualLoudspeakersModel> listenerReverbModel;
        std::shared_ptr<BRTEnvironmentModel::CFreeFieldEnvironmentModel> environmentModel;
    };
}
#endif