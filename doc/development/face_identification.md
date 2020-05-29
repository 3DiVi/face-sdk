# Face Identification

Face SDK includes several identification methods that differ in recgnition quality and time characteristics. More time-consuming methods provide better recognition quality (see [Identification Performance](../performance_parameters.md#identification-performance) for detailed characteristics of the methods). Depending on recognition quality and time all methods can be divided into 3 groups in connection with use cases:

* **9v30** – for real-time video identification on mobile platforms
* **6v7** – for real-time video identification on Windows and Linux
* **9v300**, **9v1000** – for expert systems, in which a person is searched by a photo

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
