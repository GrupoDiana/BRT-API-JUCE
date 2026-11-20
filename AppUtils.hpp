/**
* \class AppUtils
*
* \brief Declaration of AppUtils class
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
* \b Description: Provides utility functions for loading acoustic resources
* from SOFA files, including HRTF, BRIR and near-field compensation filters.
* It also provides error reporting through a common error message interface.
*
* \b Licence: This program is free software, you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

#include <string>
#include <BRTLibrary.h>

#ifndef _APP_UTILS_HPP_
#define _APP_UTILS_HPP_

#define HRTFRESAMPLINGSTEP 15

static class AppUtils
{
public:

    static bool LoadHRTFSofaFile(
        const std::string& _filePath,
        std::shared_ptr<BRTServices::CSphericalInterpolatedFIRTable> hrtf)
    {
        BRTReaders::CSOFAReader sofaReader;
        Common::CGlobalParameters globalParameters;
        globalParameters.SetSampleRate(48000);

        int sampleRateInSOFAFile =
            sofaReader.GetSampleRateFromSofa(_filePath);

        if (sampleRateInSOFAFile == -1)
        {
            error = "Error loading HRTF Sofa file";
            return false;
        }

        if (globalParameters.GetSampleRate() != sampleRateInSOFAFile)
        {
            error = "The sample rate in HRTF SOFA file doesn't match the configuration.";
            return false;
        }


        bool result = sofaReader.ReadHRTFFromSofa(_filePath, "", hrtf, HRTFRESAMPLINGSTEP, BRTServices::TEXTRAPOLATION_METHOD::nearest_point);

        if (result)
        {
            //std::cout << "HRTF Sofa file loaded successfully." << std::endl;
            return true;
        }
        else
        {
            error = "Error loading HRTF";
            return false;
        }
    }

    static bool LoadBRIRSofaFile(const std::string& _filePath, std::shared_ptr<BRTServices::CSphericalFIRTable> brir, float _fadeIn, float _riseTime, float _fadeOut, float _fallTime)
    {
        BRTReaders::CSOFAReader sofaReader;
        Common::CGlobalParameters globalParameters;
        globalParameters.SetSampleRate(48000);

        int sampleRateInSOFAFile = sofaReader.GetSampleRateFromSofa(_filePath);

        if (sampleRateInSOFAFile == -1)
        {
            error = "Error loading BRIR Sofa file";
            return false;
        }

        if (globalParameters.GetSampleRate() != sampleRateInSOFAFile)
        {
            error = "The sample rate in BRIR SOFA file doesn't match the configuration.";
            return false;
        }

        bool result = sofaReader.ReadBRIRFromSofa(_filePath, "", brir, _fadeIn, _riseTime, _fadeOut, _fallTime);

        if (result)
        {
            //std::cout << "BRIR Sofa file loaded successfully." << std::endl;
            return true;
        }
        else
        {
            error = "Error loading BRIR";
            return false;
        }
    }

    static bool LoadNearFieldSOSFilter(std::string _ildFilePath, std::shared_ptr<BRTServices::CSphericalSOSTable> _sosFilter) {

        BRTReaders::CSOFAReader sofaReader;
        Common::CGlobalParameters globalParameters;

        int sampleRateInSOFAFile = sofaReader.GetSampleRateFromSofa(_ildFilePath);
        if (sampleRateInSOFAFile == -1) {
            error = "Error loading ILD Sofa file";
            return false;
        }
        if (globalParameters.GetSampleRate() != sampleRateInSOFAFile)
        {
            return false;
        }

        bool result = sofaReader.ReadSOSFiltersFromSofa(_ildFilePath, _sosFilter);
        if (result) {
            return true;
        }
        else {
            error = "Error loading HRTF";
            return false;
        }
    }

    static std::string GetLastErrorMessage()
    {
        return error;
    }

private:

    inline static std::string error;
};

#endif