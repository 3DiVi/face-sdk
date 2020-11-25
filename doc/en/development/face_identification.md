# Face Identification

Face SDK includes several identification methods that differ in recgnition quality and time characteristics.  
More time-consuming methods provide better recognition quality (see [Identification Performance](../performance_parameters.md#identification-performance) for detailed characteristics of the methods).  
Depending on recognition quality and time all methods can be divided into 3 groups in connection with use cases:

* **9.30** – for real-time video identification on mobile platforms (arm)
* **6.7** – for real-time video identification on desktop platforms (x86)
* **9.300**, **9.1000** – for expert systems, in which a person is searched by a photo

**Deprecated numbering system of methods used in Face SDK up to version 3.3**

The first number (6.x/7.x/8.x) indicates the quality/speed of the method:
* **7.x** - these methods provide the best quality but they're also the slowest ones. Use case: face identification in server systems on large databases (more than 1 million faces).
* **6.x** - these methods are faster compared to the 7.x methods. Use case: real-time face identification in a video stream on desktop/server platforms (x86). These methods can also be used to identify faces in a video stream on modern mobile platforms (arm) with at least 4 cores and a core frequency of at least 1.6GHz.
* **8.x** - the fastest methods. Use case: face identification in a video stream on mobile platforms (arm).

The second number indicates the method version. Older versions were released later and, as a result, they provide better quality. You can switch to them when updating the SDK from the corresponding lower versions. For example, if the 6.6 recognizer was used, it's recommended to use its new version 6.7 when upgrading the SDK. The template size and template creation speed remain the same, and the identification quality becomes better.

**Current numbering system of methods used in Face SDK since version 3.3**

The first number (starting from 9) indicates the method version. The higher the version, the better the quality.  
The second number indicates the approximate template creation time in milliseconds on a modern x86 processor with a frequency of ~3GHz. The slower the method, the better its quality.  

* **x.1000 and x.300** - these methods provide the best quality but they're also the slowest ones.* Use case: face identification in server systems on large databases (more than 1 million faces).
* **x.100** - these methods are faster compared to the x.1000 and x.300 methods. Use case: real-time face identification in a video stream on desktop/server platforms (x86). These methods can also be used to identify faces in a video stream on modern mobile platforms (arm) with at least 4 cores and a core frequency of at least 1.6GHz.
* **x.30** - the fastest methods. Use case: face identification in a video stream on mobile platforms (arm).

In some cases, the latest SDK version may contain updates not for all groups of identification methods. In this case you can either use the old identification method or compare its quality with the new methods from a faster group and switch to the new faster methods if you observe quality improvement. In addition, we provide recommendations on using the specific identification methods when upgrading to a newer version of the SDK.

**Identification methods by use cases depending on Face SDK version**

|  Use case/SDK version    |    SDK-2.0   |   SDK-2.1   |   SDK-2.2   |   SDK-2.3   |   SDK-2.5  |   SDK-3.0   |   SDK-3.1   |   SDK-3.3        |  SDK-3.4        |
| -------------------------- | -------- | ------- | ------- | ------- | ------ | ------- | ------- | ------------ | ------------ |
| Expert systems         | method7      | method7     |   method7v2   |   method7v3   | method7v6    |  method7v6    |    method7v7  |method9v300<br>method9v1000|method9v300<br>method9v1000<br>method9v300mask<br>method9v1000mask|
| Identification in a video stream | method6v2      | method6v3     |   method6v4   |   method6v5   | method6v6    | method6v6     |    method6v7  | method6v7          | method6v7             |
| Mobile platforms        |          |         |         |         |        | method8v6     |    method8v7  | method9v30         | method9v30<br>method9v30mask |

\* - You can speed up the methods using the AVX2 instruction set (available only on Linux x86 64-bit). If you want to use the AVX2 instruction set, move the contents of the `lib/tensorflow_avx2` directory to the `lib` directory.

To identify faces, create the `Recognizer` object by calling the `FacerecService.createRecognizer` method with the specified configuration file.

To create [Encoder](../components.md#encoder) for one stream, specify the parameters `processing_threads_count=true` and `matching_threads_count=false`.  
To create [MatcherDB](../components.md#matcherdb) for one stream, specify the parameters `processing_threads_count=false` and `matching_threads_count=true`.

Here is the list of all configuration files and recognition methods available at the moment:

* `method6_recognizer.xml` – method 6
* `method6v2_recognizer.xml` – method 6.2
* `method6v3_recognizer.xml` – method 6.3
* `method6v4_recognizer.xml` – method 6.4
* `method6v5_recognizer.xml` – method 6.5
* `method6v6_recognizer.xml` – method 6.6
* `method6v7_recognizer.xml` – method 6.7
* `method7_recognizer.xml` – method 7
* `method7v2_recognizer.xml` – method 7.2
* `method7v3_recognizer.xml` – method 7.3
* `method7v6_recognizer.xml` – method 7.6
* `method7v7_recognizer.xml` – method 7.7
* `method8v6_recognizer.xml` – method 8.6
* `method8v7_recognizer.xml` – method 8.7
* `method9v30_recognizer.xml` – method 9.30
* `method9v300_recognizer.xml` – method 9.300
* `method9v1000_recognizer.xml` – method 9.1000

The following configuration files can be used for recognition of masked faces: 

* `method9v30mask_recognizer.xml` 
* `method9v300mask_recognizer.xml`
* `method9v1000mask_recognizer.xml`

These methods provide better identification quality of masked faces.
For example, the standard 9v1000 method provides TAR=0.72 for masked faces, and the optimized 9v1000mask method provides TAR=0.85 at FAR=1E-6

_**Note**: Learn how to detect and recognize masked faces in our [tutorial](../tutorials/detection_and_recognition_of_masked_faces.md)._

Using the `Recognizer` object you can:

* get a method name (`Recognizer.getMethodName`)
* create a template of one captured face (`Recognizer.processing`) – you will get the `Template` object
* match two templates (`Recognizer.verifyMatch`) – you can compare only the templates created by the same method (i.e. with the same configuration file)
* load (deserialize) a template from a binary stream (`Recognizer.loadTemplate`) – you can load only the templates created by the same method (i.e. with the same configuration file)
* create the `TemplatesIndex` object (`Recognizer.createIndex`), which is an index for quick search in large databases
* search the `Template` in the `TemplatesIndex` (`Recognizer.search`)

With the `Template` object you can:

* get a method name (`Template.getMethodName`)
* save (serialize) a template in a binary stream (`Template.save`)

With the `TemplatesIndex` object you can:

* get a method name (`TemplatesIndex.getMethodName`)
* get a number of templates in the index (`TemplatesIndex.size`)
* get a specified template from the index by its number (`TemplatesIndex.at`)
* reserve memory for search (`TemplatesIndex.reserveSearchMemory`)

_**Note:** The methods `Recognizer.createIndex` and `TemplatesIndex.at` do not copy the template data but only smart pointers to the data._
