import subprocess

REPETITIONS = 30
UPSCALE_RANGE = [ 2 ]
GPU_THREAD_RANGE = [ 32, 64, 128, 256 ]
PIXELS_HANDLED_BY_THREAD_SMALL = [ 2**i for i in range(0, 21) ] + [ 1226240 ] 
PIXELS_HANDLED_BY_THREAD_LARGE = [ 2**i for i in range(0, 27) ] + [ 79847424 ]
MAX_THREADS_PER_BLOCK = 1024
SMALL_IMAGE = { 'name': '../img/in-small.png', 'width': 640, 'height': 479, 'byte_per_pixel': 4 }
LARGE_IMAGE = { 'name': '../img/in-large.png', 'width': 5472, 'height': 3648, 'byte_per_pixel': 4 }
EXECUTABLE = '../bin/test'

def execute(parameters: list):
    parameters = [str(param) for param in parameters]
    cmd = [ EXECUTABLE ] + parameters
    subprocess.run(cmd, capture_output=True)

if __name__ == '__main__':
    # GPU with UpscaleWithTextureObject
    for upscale_factor in UPSCALE_RANGE:
        for threads in GPU_THREAD_RANGE:
            # for pixels_handled in PIXELS_HANDLED_BY_THREAD_SMALL:
            #     if threads <= MAX_THREADS_PER_BLOCK:
            #         parameters = [ SMALL_IMAGE['name'], upscale_factor, 'results/GPU_thread_per_pixel_withTextureObject_small.csv', 'gpu', 3, threads, pixels_handled, REPETITIONS ]
            #         execute(parameters)
            for pixels_handled in PIXELS_HANDLED_BY_THREAD_LARGE:
                if threads <= MAX_THREADS_PER_BLOCK:
                    parameters = [ LARGE_IMAGE['name'], upscale_factor, 'results/GPU_thread_per_pixel_withTextureObject_large.csv', 'gpu', 3, threads, pixels_handled, REPETITIONS ]
                    execute(parameters)

                    print(f'    --> Completed {pixels_handled} pixel hadled')

            print(f'--> Completed {threads} threads')
    
    print('[+] Completed GPU with UpscaleWithTextureObject')
