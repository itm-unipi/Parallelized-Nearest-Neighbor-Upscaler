#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#include "Headers.h"

using namespace std;

int main(int argc, char* argv[])
{
    // read test parameters
    if (argc >= 4) {
        // get image info
        string inputImageName = argv[1];
        uint8_t upscaleFactor = atoi(argv[2]);
        ofstream file;
        file.open(argv[3], std::ios_base::app);

        // open the image
        uint32_t width, height, bytePerPixel, channel;
        uint8_t* data = stbi_load(inputImageName.c_str(), (int*)&width, (int*)&height, (int*)&bytePerPixel, channel);
        
        vector<float> elapsedTimes;
        string result;

        // iterate the required upscaler
        if (!strcmp(argv[4], "cpu")) {
            // check if it has been required a customized image size
            if (argc == 9) {
                width = atoi(argv[7]);
                height = atoi(argv[8]);
            }

            // get upscale settings from parameters
            uint32_t numThreads = atoi(argv[5]);
            uint32_t numRepetitions = atoi(argv[6]);
            result = to_string(upscaleFactor) + ";" + to_string(width) + ";" + to_string(height) + ";" + to_string(numThreads);

            // repeate the tests
            for (uint32_t i = 0; i < numRepetitions; i++) {
                float elapsedTime = cpuUpscaler(numThreads, upscaleFactor, data, width, height, bytePerPixel);
                elapsedTimes.push_back(elapsedTime);
            }
        } else if (!strcmp(argv[4], "gpu")) {
            // check if it has been required a customized image size
            if (argc == 11) {
                width = atoi(argv[9]);
                height = atoi(argv[10]);
            }

            // get upscale settings from parameters
            Settings settings; 
            UpscalerType upscalerType = static_cast<UpscalerType>(atoi(argv[5]));
            uint32_t numRepetitions = atoi(argv[8]);

            settings = Settings(atoi(argv[6]), upscalerType, width, height, upscaleFactor, atoi(argv[7]));
            result = to_string(upscaleFactor) + ";" + to_string(width) + ";" + to_string(height) + ";" + settings.toString();

            // compute the new upscaled image size
            size_t originalSize = height * width * bytePerPixel * sizeof(uint8_t);
            size_t upscaledSize = height * upscaleFactor * width * upscaleFactor * bytePerPixel * sizeof(uint8_t);

            // repeat the tests
            for (uint32_t i = 0; i < numRepetitions; i++) {
                float elapsedTime = gpuUpscaler(originalSize, upscaledSize, upscaleFactor, settings, data, width, height, bytePerPixel);
                elapsedTimes.push_back(elapsedTime);
            }
        } else {
            cerr << "[-] Wrong parameters" << endl;
            exit(-1);
        }

        // compute the arithmetic mean of the elapsed times, the 95% confidence interval and the worst time
        float meanElapsedTime = std::accumulate(elapsedTimes.begin(), elapsedTimes.end(), 0.0) / elapsedTimes.size();
        float confidenceInterval = computeCI(elapsedTimes);
        auto it = std::max_element(elapsedTimes.begin(), elapsedTimes.end());
        float worstElapsedTime = *it;

        // convert and save the values in the CSV
        result = result + ";" + to_string(meanElapsedTime) + ";" + to_string(confidenceInterval) + ";" + to_string(worstElapsedTime) + "\n";
        std::replace(result.begin(), result.end(), '.', ',');
        file.write(result.c_str(), result.size());
    } else {
        cerr << "[-] Not enough parameters" << endl;
        exit(-1);
    }
}