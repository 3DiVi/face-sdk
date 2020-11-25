# GPU Usage

Since face recognition requires a lot of processing power, the GPU acceleration for the Face SDK modules is now available for running deep learning algorithms.

You can use GPU acceleration on:
* Windows x86 64-bit 
* Linux x86 64-bit
* Android

In this section, you'll find the information about the GPU acceleration for available Face SDK modules, how to enable this function, as well as information about timing characteristics for the Face SDK modules with CPU and GPU usage, possible errors when using GPU, and solutions. 

## Desktop [beta]

Currently, the GPU acceleration is available for the following modules (single GPU mode only):

* recognizers (9v30, 9v300, 9v1000, 9v30mask, 9v300mask, 9v1000mask) (see [Face Identification](face_identification.md))

* Software requirements:
   * Nvidia GPU Driver >= 410.48
   * [CUDA Toolkit 10.1](https://developer.nvidia.com/cuda-toolkit-archive)
   * [cuDNN 7](https://developer.nvidia.com/rdp/cudnn-archive#a-collapse765-90)
   * [For Windows] [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/)
* Hardware requirements: 
   * CUDA compatible GPU (NVIDIA GTX 1050 Ti or better)

You can also use pre-built docker containers with CUDA support, such as *nvidia/cuda:10.0-cudnn7-devel-ubuntu16.04* (please note that some licenses can be unavailable in this case).

To run models on GPU, you need to edit the configuration file of one of the supported recognizers: set `use_cuda` to `1`. The GPU acceleration is performed on one of the available GPUs. The GPU index can be set as follows:
* via the `gpu_index` parameter in the configuration file 
* via the `CUDA_VISIBLE_DEVICES` environment variable

### Timing characteristics

The table below shows the speed measurements for the Face SDK modules using CPU and GPU:

| Method | GPU    | CPU    |
| ------ | ------ | ------ |
| 9v1000 | 13ms   | 730ms  |
| 9v300  | 7ms    | 260ms  |
| 9v30   | 3ms    | 30ms  |


_**Note**: the NVIDIA GeForce GTX 1080 Ti and Intel Core i7 were used for the speed test._

### Troubleshooting

| Error | Solution |
| ----- | -------- |
| `libtensorflow.so.2: cannot open shared object file: No such file or directory`  | You need to set the environment variable `LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/path/to/lib`, where `path/to/lib` is the path to the *lib* directory from the `face_sdk` build, or move this file to `../lib` relative to the application you're running (since the run-time search path is set in `libfacerec.so`). |
| `libusb-0.1.so.4: cannot open shared object file: No such file or directory` | Install the `libusb-0.1-4` package on the system by running the command: `apt-get install libusb-0.1-4` |
| Slow initialization | Increasing the default JIT cache size: `export CUDA_CACHE_MAXSIZE=2147483647` (see [JIT Caching](https://developer.nvidia.com/blog/cuda-pro-tip-understand-fat-binaries-jit-caching/)) |

## Android [beta]

Currently, the GPU acceleration is available for the following modules:

* recognizers (9v30, 9v300, 9v1000, 9v30mask, 9v300mask, 9v1000mask) (see [Face Identification](face_identification.md))
* the *blf* detector (see [Face Capturing](face_capturing.md))

The GPU usage can be enabled/disabled via the `use_mobile_gpu` flag in the configuration files of the `Capturer`,` Recognizer`, `VideoWorker` objects (in the configuration file of the `VideoWorker` object, GPU is enabled for detectors). By default, mobile GPU support **is enabled** (the value is `1`). To disable the GPU usage, change the `use_mobile_gpu` flag to `0`.

### Timing characteristics

The table below shows the speed measurements for the Face SDK modules using CPU and GPU:

| Method | CPU | GPU |
| ------ | --- | --- |
| 9v1000 | 3660ms | 610ms |
| 9v300 | 1960ms | 280ms |
| 9v30 | 170ms | 70ms |

_**Note**: Google Pixel 3 was used for the speed test._
