import subprocess

REPETITIONS = 30
UPSCALE_RANGE = [ 2 ]
CPU_THREAD_RANGE = [ 1, 2, 4, 8, 16, 32, 64, 128 ]
SMALL_IMAGE = { 'name': '../img/in-small.png', 'width': 640, 'height': 479, 'byte_per_pixel': 4 }
LARGE_IMAGE = { 'name': '../img/in-large.png', 'width': 5472, 'height': 3648, 'byte_per_pixel': 4 }
EXECUTABLE = '../bin/test'

def execute(parameters: list):
    parameters = [str(param) for param in parameters]
    # cmd = f'{EXECUTABLE} {" ".join(parameters)}'
    cmd = [EXECUTABLE] + parameters
    print(cmd)
    subprocess.run(cmd, capture_output=True)


if __name__ == '__main__':
    # CPU Multi Thread Test
    for upscale_factor in UPSCALE_RANGE:
        for n_threads in CPU_THREAD_RANGE:
            # parameters = [ SMALL_IMAGE['name'], upscale_factor, 'results/CPU_multi_thread_small.csv', 'cpu', n_threads, REPETITIONS ]
            # execute(parameters)
            parameters = [ LARGE_IMAGE['name'], upscale_factor, 'results/CPU_multi_thread_intermediate.csv', 'cpu', n_threads, REPETITIONS, 1920, 1080 ]
            execute(parameters)
    
    print('[+] Completed CPU Multi Thread Test')
