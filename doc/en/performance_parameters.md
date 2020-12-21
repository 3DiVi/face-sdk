# Performance Parameters

Contents: 

* [Identification Performance](#identification-performance)
    * [Timing Characteristics for Core i7 4.5 GHz](#timing-characteristics-for-core-i7-45-ghz)
    * [Memory Characteristics](#memory-characteristics)
    * [ROC on Extended LFW Test](#roc-on-extended-lfw-test)
* [Performance Test](#performance-test)

List of used abbreviations:

* FAR (false accept rate) – the probability that the system incorrectly accepts a non-authorized person.
* TAR (true accept rate) – the probability that the system correctly accepts an authorized person.
* FRR (false reject rate) – the probability that the system incorrectly rejects an authorized person.
* IR (identification rate) – identification rate. 

## Identification Performance

### Timing Characteristics for Core i7 4.5 GHz*

<table cellpadding ="5" border="1" style="border-collapse:collapse;center">
<tr>
  <th rowspan=2>Recognition method</th>
  <th rowspan=2>Template generation (ms)</th>
  <th colspan=3>Accelerated Matching 1:N (ms)</th>
  <th rowspan=2>Matching 1:1 (ms)</th>
</tr>
<tr>
  <th>N = 10<sup>4</sup> </th>
  <th>N = 10<sup>6</sup> </th>
  <th>N = 10<sup>7</sup> </th>
</tr>
<tr align="center"> <th align="center"> 6.7    </th>    <td>  40 (45**)  </td>  <td> 0.25</td> <td> 12.1 </td> <td> 126 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 7.7    </th>    <td> 170 (180**) </td>  <td> 0.25</td> <td> 12.1 </td> <td> 126 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 8.7    </th>    <td>  20 (20**)  </td>  <td> 0.25</td> <td> 12.1 </td> <td> 126 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.30   </th>    <td>  30       </td>   <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.300  </th>    <td> 260 (125***)       </td>  <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.1000 </th>    <td> 730 (305***)        </td>  <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.30mask   </th>    <td>  20       </td>  <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.300mask  </th>    <td> 160 (79***)       </td>  <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
<tr align="center"> <th align="center"> 9.1000mask </th>    <td> 290 (144***)        </td>  <td> 0.18</td> <td> 12.0 </td> <td> 117 </td>  <td>0.04   </td>  </tr>
</table>

\* – characteristics specified in this table are given for a single-core CPU.   
\** – template creation time when `processing_less_memory_consumption` was set to `true` in the `FacerecService.createRecognizer` call for recognizer creation.     
\*** – template creation time with the use of the AVX2 instruction set (see [Face identification](development/face_identification.md)).   

**Note:**
* Accelerated search time is given for `k=1`. As for larger values of `k`, the time will increase up to the search time without acceleration.
* Accelerated search is implemented only for the recognition methods 6.5, 6.6, 6.7, 7.3, 7.6, 7.7, 8.6, 8.7, 9.30, 9.300, 9.1000.
* To achieve this speed, the templates in the index must be located in the order of creation (by using the `Recognizer.processing` or `Recognizer.loadTemplate` method).
* To achieve higher speed, you can use GPU (see [GPU Usage](development/gpu_usage.md)). 

### Memory Characteristics

<table cellpadding ="5" border="1" style="border-collapse:collapse;center">
<tr>
  <th>Recognition method</th>
  <th>Serialized template size (Bytes)</th>
  <th>Template size in RAM (Bytes)</th>
  <th>Memory consumption* (MB)</th>
</tr>
<tr align="center"> <th align="center"> 6.7   </th>   <td>  536 </td>  <td>  636 </td>  <td>  105 (85**)</td> </tr>
<tr align="center"> <th align="center"> 7.7   </th>   <td>  536 </td>  <td>  636 </td>  <td>  195 (163**)</td> </tr>
<tr align="center"> <th align="center"> 8.7   </th>   <td>  536 </td>  <td>  636 </td>  <td>   52 (40**)</td> </tr>
<tr align="center"> <th align="center"> 9.30  </th>   <td>  280 </td>  <td>  380 </td>  <td>  155       </td> </tr>
<tr align="center"> <th align="center"> 9.300 </th>   <td>  280 </td>  <td>  380 </td>  <td>  210       </td> </tr>
<tr align="center"> <th align="center"> 9.1000</th>   <td>  280 </td>  <td>  380 </td>  <td>  290       </td> </tr>
</table>

\* – the amount of memory consumed does not depend on the number of the `Recognizer` objects created by this method  
\** – memory consumption when `processing_less_memory_consumption` was set to `true` in the `FacerecService.createRecognizer` call for recognizer creation  

### ROC on Extended LFW Test

In this test, the mismatch pairs set was increased and LFW errors were fixed in order to get accurate measurements at low FAR.

<p align="center">
<img width="850" src="../img/log_lfw_full_ext_w_all_methods.png"><br>
</p>

<table cellpadding ="3" border="1" style="border-collapse:collapse;center">
<tr>
  <th>FAR</th>
  <th>8.7 TAR (%)</th>
  <th>6.7 TAR (%)</th>
  <th>7.7 TAR (%)</th>
  <th>9.30 TAR (%)</th>
  <th>9.300 TAR (%)</th>
  <th>9.1000 TAR (%)</th>
</tr>
<tr align="center"> <th> 1e-4 </th> <td> 97.3 </td>  <td> 99.0 </td>  <td> 99.3 </td> <td> 97.3 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 1e-5 </th> <td> 93.8 </td>  <td> 97.5 </td>  <td> 98.5 </td> <td> 94.7 </td> <td> 99.0 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 1e-6 </th> <td> 88.9 </td>  <td> 94.8 </td>  <td> 96.9 </td> <td> 90.8 </td> <td> 98.0 </td> <td> 98.5 </td> </tr>
</table>

<details>
  <summary>Click to expand the table with all FAR and TAR values for different identification methods</summary>

<table cellpadding ="3" border="1" style="border-collapse:collapse;center">
<tr>
  <th>FAR</th>
  <th>8.7 TAR (%)</th>
  <th>6.7 TAR (%)</th>
  <th>7.7 TAR (%)</th>
  <th>9.30 TAR (%)</th>
  <th>9.300 TAR (%)</th>
  <th>9.1000 TAR (%)</th>
</tr>
<tr align="center"> <th> 1e-2 </th> <td> 99.6 </td>  <td> 99.7 </td>  <td> 99.8 </td> <td> 99.5 </td> <td> 99.7 </td> <td> 99.8 </td> </tr>
<tr align="center"> <th> 9e-3 </th> <td> 99.6 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.5 </td> <td> 99.7 </td> <td> 99.8 </td> </tr>
<tr align="center"> <th> 8e-3 </th> <td> 99.6 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.4 </td> <td> 99.7 </td> <td> 99.8 </td> </tr>
<tr align="center"> <th> 7e-3 </th> <td> 99.5 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.4 </td> <td> 99.7 </td> <td> 99.8 </td> </tr>
<tr align="center"> <th> 6e-3 </th> <td> 99.5 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.4 </td> <td> 99.7 </td> <td> 99.8 </td> </tr>
<tr align="center"> <th> 5e-3 </th> <td> 99.5 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.3 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 4e-3 </th> <td> 99.4 </td>  <td> 99.7 </td>  <td> 99.7 </td> <td> 99.3 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 3e-3 </th> <td> 99.3 </td>  <td> 99.6 </td>  <td> 99.7 </td> <td> 99.2 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 2e-3 </th> <td> 99.2 </td>  <td> 99.6 </td>  <td> 99.6 </td> <td> 99.1 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 1e-3 </th> <td> 98.8 </td>  <td> 99.5 </td>  <td> 99.6 </td> <td> 98.7 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 9e-4 </th> <td> 98.7 </td>  <td> 99.4 </td>  <td> 99.6 </td> <td> 98.6 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 8e-4 </th> <td> 98.7 </td>  <td> 99.4 </td>  <td> 99.5 </td> <td> 98.6 </td> <td> 99.7 </td> <td> 99.7 </td> </tr>
<tr align="center"> <th> 7e-4 </th> <td> 98.7 </td>  <td> 99.4 </td>  <td> 99.5 </td> <td> 98.5 </td> <td> 99.7 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 6e-4 </th> <td> 98.6 </td>  <td> 99.4 </td>  <td> 99.5 </td> <td> 98.5 </td> <td> 99.7 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 5e-4 </th> <td> 98.5 </td>  <td> 99.4 </td>  <td> 99.5 </td> <td> 98.5 </td> <td> 99.7 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 4e-4 </th> <td> 98.4 </td>  <td> 99.4 </td>  <td> 99.5 </td> <td> 98.3 </td> <td> 99.7 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 3e-4 </th> <td> 98.2 </td>  <td> 99.2 </td>  <td> 99.5 </td> <td> 98.1 </td> <td> 99.6 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 2e-4 </th> <td> 98.0 </td>  <td> 99.2 </td>  <td> 99.4 </td> <td> 97.7 </td> <td> 99.6 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 1e-4 </th> <td> 97.3 </td>  <td> 99.0 </td>  <td> 99.3 </td> <td> 97.3 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 9e-5 </th> <td> 97.3 </td>  <td> 99.0 </td>  <td> 99.2 </td> <td> 97.3 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 8e-5 </th> <td> 97.2 </td>  <td> 99.0 </td>  <td> 99.2 </td> <td> 97.2 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 7e-5 </th> <td> 97.1 </td>  <td> 98.9 </td>  <td> 99.2 </td> <td> 97.1 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 6e-5 </th> <td> 96.9 </td>  <td> 98.8 </td>  <td> 99.1 </td> <td> 96.9 </td> <td> 99.5 </td> <td> 99.6 </td> </tr>
<tr align="center"> <th> 5e-5 </th> <td> 96.6 </td>  <td> 98.7 </td>  <td> 99.0 </td> <td> 96.6 </td> <td> 99.4 </td> <td> 99.5 </td> </tr>
<tr align="center"> <th> 4e-5 </th> <td> 96.3 </td>  <td> 98.5 </td>  <td> 99.0 </td> <td> 96.5 </td> <td> 99.3 </td> <td> 99.5 </td> </tr>
<tr align="center"> <th> 3e-5 </th> <td> 95.8 </td>  <td> 98.5 </td>  <td> 98.9 </td> <td> 96.3 </td> <td> 99.3 </td> <td> 99.3 </td> </tr>
<tr align="center"> <th> 2e-5 </th> <td> 95.0 </td>  <td> 98.1 </td>  <td> 98.8 </td> <td> 95.8 </td> <td> 99.2 </td> <td> 99.2 </td> </tr>
<tr align="center"> <th> 1e-5 </th> <td> 93.8 </td>  <td> 97.5 </td>  <td> 98.5 </td> <td> 94.7 </td> <td> 99.0 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 9e-6 </th> <td> 93.7 </td>  <td> 97.4 </td>  <td> 98.5 </td> <td> 94.6 </td> <td> 99.0 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 8e-6 </th> <td> 93.4 </td>  <td> 97.3 </td>  <td> 98.4 </td> <td> 94.4 </td> <td> 98.9 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 7e-6 </th> <td> 93.2 </td>  <td> 97.2 </td>  <td> 98.4 </td> <td> 94.2 </td> <td> 98.9 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 6e-6 </th> <td> 93.0 </td>  <td> 97.0 </td>  <td> 98.2 </td> <td> 93.9 </td> <td> 98.9 </td> <td> 99.0 </td> </tr>
<tr align="center"> <th> 5e-6 </th> <td> 92.7 </td>  <td> 96.8 </td>  <td> 98.2 </td> <td> 93.7 </td> <td> 98.9 </td> <td> 98.9 </td> </tr>
<tr align="center"> <th> 4e-6 </th> <td> 91.9 </td>  <td> 96.7 </td>  <td> 98.0 </td> <td> 93.1 </td> <td> 98.9 </td> <td> 98.9 </td> </tr>
<tr align="center"> <th> 3e-6 </th> <td> 91.1 </td>  <td> 96.4 </td>  <td> 97.8 </td> <td> 92.8 </td> <td> 98.7 </td> <td> 98.8 </td> </tr>
<tr align="center"> <th> 2e-6 </th> <td> 90.0 </td>  <td> 96.0 </td>  <td> 97.3 </td> <td> 92.1 </td> <td> 98.4 </td> <td> 98.8 </td> </tr>
<tr align="center"> <th> 1e-6 </th> <td> 88.9 </td>  <td> 94.8 </td>  <td> 96.9 </td> <td> 90.8 </td> <td> 98.0 </td> <td> 98.5 </td> </tr>
<tr align="center"> <th> 9e-7 </th> <td> 88.6 </td>  <td> 94.3 </td>  <td> 96.7 </td> <td> 90.6 </td> <td> 98.0 </td> <td> 98.5 </td> </tr>
<tr align="center"> <th> 8e-7 </th> <td> 87.7 </td>  <td> 94.1 </td>  <td> 96.3 </td> <td> 90.4 </td> <td> 97.9 </td> <td> 98.4 </td> </tr>
<tr align="center"> <th> 7e-7 </th> <td> 87.6 </td>  <td> 93.8 </td>  <td> 96.1 </td> <td> 89.7 </td> <td> 97.8 </td> <td> 98.3 </td> </tr>
<tr align="center"> <th> 6e-7 </th> <td> 87.1 </td>  <td> 93.7 </td>  <td> 96.0 </td> <td> 89.3 </td> <td> 97.6 </td> <td> 98.2 </td> </tr>
<tr align="center"> <th> 5e-7 </th> <td> 86.4 </td>  <td> 93.4 </td>  <td> 95.7 </td> <td> 89.1 </td> <td> 97.4 </td> <td> 98.1 </td> </tr>
<tr align="center"> <th> 4e-7 </th> <td> 86.0 </td>  <td> 93.1 </td>  <td> 95.6 </td> <td> 88.1 </td> <td> 97.3 </td> <td> 98.0 </td> </tr>
<tr align="center"> <th> 3e-7 </th> <td> 84.7 </td>  <td> 92.3 </td>  <td> 95.5 </td> <td> 87.8 </td> <td> 95.6 </td> <td> 97.9 </td> </tr>
<tr align="center"> <th> 2e-7 </th> <td> 83.2 </td>  <td> 91.4 </td>  <td> 92.4 </td> <td> 86.9 </td> <td> 94.7 </td> <td> 97.7 </td> </tr>
<tr align="center"> <th> 1e-7 </th> <td> 81.5 </td>  <td> 88.9 </td>  <td> 90.6 </td> <td> 85.6 </td> <td> 89.6 </td> <td> 97.3 </td> </tr>
</table>
</details>

## Performance Test

Modes:
* [Detection](#detection)
* [Processing](#processing)
* [1:1 Recognition Test](#11-recognition-test)
* [1:N Recognition Test](#1n-recognition-test)
* [Search Speed Test](#search-speed-test)
* [Convert a Configuration File](#convert-a-configuration-file)
* [Create a Configuration File](#create-a-configuration-file)

### Detection

In this mode, the program detects faces in images with the id in the range of `[begin_image_id, end_image_id)`. Each image should contain only one person. The images containing more than one person detected will be discarded.

Launch parameters:

* `mode` – program mode (`detection`)
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `sdk_config_dir` – path to the *conf/facerec* directory
* `dataset_config` – config file of the image dataset (see [Convert a Configuration File](#convert-a-configuration-file))
* `capturer_config` – capturer config file name
* `dataset_root_dir` – path to the dataset directory
* `detection_result_file` – file for storing the detection results
* `[begin_image_id]` – index of the image from which detection begins (the default setting is 0)
* `[end_image_id]` – index of the image to which detection is performed (by default, the processing is performed to the end of the file)
* `[use_cpu_cores_count]` – number of cores used for detection (the default setting is 1)

Example of launch from the *bin* directory:
```
./test_sdk \
    --mode detection \
    --dll_path ../lib/libfacerec.so \
    --sdk_config_dir ../conf/facerec \
    --capturer_config common_capturer4_lbf.xml \
    --dataset_config dataset_config.txt \
    --dataset_root_dir /path/to/data \
    --detection_result_file detection_result.txt \
```
The detection result is a text file with the lines in the following format: `<image_id> <points_count> <points>`

### Processing

In this mode, the program creates templates from the faces detected in the images with the id in the range of `[begin_image_id, end_image_id)`.

Launch parameters:

* `mode` – program mode (`processing`)
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `sdk_config_dir` – path to the *conf/facerec* directory
* `dataset_config` – config file of the image dataset (see [Convert a Configuration File](#convert-a-configuration-file))
* `dataset_root_dir` – path to the dataset directory
* `recognizer_config` – recognizer config file name
* `processing_result_file` – file for storing the resulting templates
* `[begin_image_id]` – index of the image from which processing begins (the default setting is 0)
* `[end_image_id]` – index of the image to which processing is performed (by default, the processing is performed to the end of the file)
* `[use_cpu_cores_count]` – number of cores used for processing (the default setting is 1)
* `FILES` – file(s) storing the detection results

Example of launch from the *bin* directory:
```
./test_sdk \
    --mode processing \
    --dll_path ../lib/libfacerec.so \
    --sdk_config_dir ../conf/facerec \
    --dataset_config dataset_config.txt \
    --dataset_root_dir /path/to/data \
    --recognizer_config method6v7_recognizer.xml \
    --processing_result_file ./templates_6v7.bin \
    detection_result.txt
```

The processing result is a binary file containing one record per each generated template. Each record is a 64-bit unsigned integer (`image_id`) followed by a serialized template.

### 1:1 Recognition Test

In this mode, the program performs the 1:1 recognition test using the templates generated from the images with the `id` in the range of `[begin_image_id, end_image_id)`.

Launch parameters:

* `mode` – program mode (`recognition_test_11`)
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `sdk_config_dir` – path to the *conf/facerec* directory
* `dataset_config` – config file of the image dataset (see [Convert a Configuration File](#convert-a-configuration-file))
* `recognizer_config` – recognizer config file name
* `result_roc_file` – file to save the ROC curve
* `result_closest_mismatches_file` – result file with the closest mismatches
* `[begin_image_id]` – index of the first image used in the test (the default setting is 0)
* `[end_image_id]` – index of the first image after `begin_image_id` not used in the test (by default use all from `begin_image_id` to the end of the file)
* `[use_cpu_cores_count]` – number of cores used for matching (the default setting is 1)
* `FILES` – file(s) storing the processing results

Example of launch from the bin directory:
```
./test_sdk \
    --mode recognition_test_11 \
    --dll_path ../lib/libfacerec.so \
    --sdk_config_dir ../conf/facerec \
    --dataset_config dataset_config.txt \
    --recognizer_config method6v7_recognizer.xml \
    --result_roc_file ./roc11_6v7.txt \
    --result_closest_mismatches_file ./closest_mismatches_file.txt \
    templates_6v7.bin
```
1:1 Recognition test results:

* A text file containing a ROC curve; the file line describes the curve point in the following format: `<far> <tar> <distance>`.
* A text file with the lines in the following format: `<distance> <image_id1> <image_id2> <path_to_image1> <path_to_image2>`. This file contains pairs of images marked as belonging to different people, but having a minimum distance between the templates. We recommend you to generate this file using the best method and view the images from the top of it to check the errors of the dataset annotation.

### 1:N Recognition Test

In this mode, the program performs the 1:N recognition test using the templates generated from the images with the `id` in the range of `[begin_image_id, end_image_id)`.

Launch parameters:

* `mode` – program mode (`recognition_test_1N`)
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `sdk_config_dir` – path to the *conf/facerec* directory
* `dataset_config` – config file of the image dataset (see [Convert a Configuration File](#convert-a-configuration-file))
* `recognizer_config` – recognizer config file name
* `result_roc_file` – file to save the ROC curve
* `[begin_image_id]` – index of the first image used in the test (the default setting is 0)
* `[end_image_id]` – index of the first image after `begin_image_id` not used in the test (by default use all from `begin_image_id` to the end of the file)
* `[use_cpu_cores_count]` – number of cores used for searches (the default setting is 1)
* `[acceleration]` – search acceleration type (the default setting is 0)
* `0` – search with `pbio::Recognizer::search` with the `pbio::Recognizer::SearchAccelerationType::NO_SEARCH_ACCELERATION` acceleration
* `1` – search with `pbio::Recognizer::search` with the `pbio::Recognizer::SearchAccelerationType::SEARCH_ACCELERATION_1` acceleration `-1` – search with `pbio::Recognizer::verifyMatch` in a single thread (doesn’t depend on `search_threads_count`)
* `FILES` – file(s) storing the processing results

Example of launch from the bin directory:
```
./test_sdk \
    --mode recognition_test_1N \
    --dll_path ../lib/libfacerec.so \
    --sdk_config_dir ../conf/facerec \
    --dataset_config dataset_config.txt \
    --recognizer_config method6v7_recognizer.xml \
    --result_roc_file ./roc1N_6v7.txt \
    --acceleration 1 \
    templates_6v7.bin
```

The result of the 1:N recognition test is a text file containing a ROC curve. The file line describes the curve point in the following format: `<far> <tar> <distance>`.
  
### Search Speed Test

In this mode, the program performs the search speed test using the templates generated in processing mode or using the utility (see [Template Generator](#template-generator)).

Launch parameters:

* `mode` – program mode (`search_speed_test`)
* `dll_path` – path to the `libfacerec.so` or `facerec.dll` library file
* `sdk_config_dir` – path to the *conf/facerec* directory
* `recognizer_config` – recognizer config file name
* `[templates_count]` – number of templates used (by default, processing is performed until the end of the file)
* `[queries_count]` – number of queries (the default setting is 1)
* `[query_k_nearest]` – number of the nearest templates for search (the default setting is 1)
* `[search_threads_count]` – number of threads used for search (the default setting is 1)
* `[acceleration]` – search acceleration type (the default setting is 0)
* `0` – search with `pbio::Recognizer::search` with the `pbio::Recognizer::SearchAccelerationType::NO_SEARCH_ACCELERATION` acceleration
* `1` – search with `pbio::Recognizer::search` with the `pbio::Recognizer::SearchAccelerationType::SEARCH_ACCELERATION_1` acceleration
* `-1` – search with `pbio::Recognizer::verifyMatch` in a single thread (doesn’t depend on `search_threads_count`)
* `FILES` – file(s) obtained from the processing mode or by using the utility

Example of launch from the *bin* directory:
```
./test_sdk \
    --mode search_speed_test \
    --dll_path ../lib/libfacerec.so \
    --sdk_config_dir ../conf/facerec \
    --recognizer_config method6v7_recognizer.xml \
    templates.bin
```

The result of the search speed test is a message with test results.

### Convert a Configuration File

In this mode, the program converts the config file from `format1` to `format2` (used in other modes):

* `format1` – a text file with the lines in the following format: `<path/to/person/dir/image_file>`. Images with the same `path/to/person/dir` belong to the same person. Images for one person must be in a row.

```
Example:
person1_dir/image1
person1_dir/image2
person2_dir/image1
...
```
    
* `format2` – a text file with 3 lines per image (`<person_id> <image_id> <path_to_image>`).
```
Example:
person0_id
image0_id
path_to_image0
person0_id
image1_id
path_to_image1
...
```

Launch parameters:

* `mode` – program mode (convert_config_format)
* `result_dataset_config` – config file of the image dataset
* `FILE` – one file of format1

Example of launch:
```
./test_sdk \
    --mode convert_config_format \
    --result_dataset_config dataset_config.txt \
    lfw_simple_format.txt
```

The conversion result is a config file of the image dataset.

### Create a Configuration File

Example of creating a config file of `format1`:
```
find -type f | sort > ../lfw_simple_format.txt
```

The paths in the configuration files must be relative to the directory passed through the `dataset_root_dir` parameter.

### Utilities

#### Template Generator

A utility for creating random templates.

Launch parameters:

* `recognizer_version` – recognizer version (select one of [9v30, 9v300, 9v1000, 8v7, 8v6, 7v7, 7v6, 7v3, 7v2, 7, 6v7, 6v6, 6v5, 6v4, 6v3, 6v2, 6])
* `templates_count` – number of generated templates
* `result_file` – a result binary file containing the generated random templates

Example of launch:
```
./template_generator \
    6v7 \
    100000 \
    random_templates.bin
```

The template generator result is a binary file containing the generated random templates of the same format as in the processing mode.

**Source code:** [examples/cpp/test_sdk](../../examples/cpp/test_sdk)
