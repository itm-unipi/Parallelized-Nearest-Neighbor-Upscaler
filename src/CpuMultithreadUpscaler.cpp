#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <cmath>

#include "../lib/stb_image.h"
#include "../lib/stb_image_write.h"

#include "Timer.h"
#include "Headers.h"

using namespace std;

void worker(const uint8_t* originalImage, uint8_t* upscaledImage, uint32_t start, uint32_t stop, uint8_t upscaleFactor, size_t width, size_t height, uint32_t bytePerPixel)
{
    uint32_t upscaledWidth = width * upscaleFactor;
    uint32_t* p = (uint32_t*)upscaledImage;
    uint32_t* q = (uint32_t*)originalImage;

    // iterate the pixels of the original image assigned to this thread
    for (size_t oldIndex = start; oldIndex < stop; oldIndex++) {
        // convert the position in a matrix notation
        uint32_t i = oldIndex / (width);
        uint32_t j = oldIndex - (i * width);

        // compute the position of the first pixel to duplicate in upscaled image
        uint32_t newi = i * upscaleFactor;
        uint32_t newj = j * upscaleFactor;

        // iterate the pixel to duplicate in upscaled image
        for (int m = newi; m < newi + upscaleFactor; m++) {
            for (int n = newj; n < newj + upscaleFactor; n++) {
                // compute the pixel position in the upscaled image vector
                uint32_t newIndex = m * upscaledWidth + n;

                // copy all the channels
                p[newIndex] = q[oldIndex];
            }
        }
    }
}

float cpuMultithreadUpscaler(uint32_t numThread, uint8_t upscaleFactor, uint8_t* originalImage, size_t width, size_t height, uint32_t bytePerPixel, string imageName)
{
    uint8_t* upscaledImage = new uint8_t[(width * upscaleFactor * bytePerPixel) * (height * upscaleFactor)];

    size_t sizeOriginalImage = width * bytePerPixel * height;
    size_t sizeRowUpscaledImage = width * upscaleFactor;

    Timer timer;
    
    // partition pixels to copy among the different threads
    vector<thread> threads;
    uint32_t pixelToManage = ceil((double)(sizeOriginalImage / bytePerPixel / numThread));

    for (int i = 0; i < numThread; ++i) {
        uint32_t start = i * pixelToManage;
        uint32_t stop = (start + pixelToManage) <= (sizeOriginalImage / bytePerPixel) ? (start + pixelToManage) : (sizeOriginalImage / bytePerPixel);
        // cout << start << ":" << stop << endl;
        threads.emplace_back(worker, originalImage, upscaledImage, start, stop, upscaleFactor, width, height, bytePerPixel);
    }

    // execute and wait the threads
    for (auto& thread : threads)
        thread.join();
    
    // print the upscale duration
    float time = timer.getElapsedMilliseconds();
    cout << "[+] (CPU " << numThread << " thread) Time needed: " << time << "ms" << endl;

    // save image as PNG
    if (imageName != "") {
        cout << "[+] Saving image..." << endl;
        if (stbi_write_png(imageName.c_str(), width * upscaleFactor, height * upscaleFactor, bytePerPixel, upscaledImage, width * upscaleFactor * bytePerPixel))
            cout << "[+] Image saved successfully" << endl;
        else
            cout << "[-] Failed to save image" << endl;
    }

    delete[] upscaledImage;
    return time;
}
