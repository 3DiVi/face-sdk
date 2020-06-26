# Идентификация лиц

Face SDK содержит несколько методов идентификации, отличающихся по характеристикам качества распознавания и времени работы. Менее быстрые методы имеют лучшие показатели по качеству распознавания (характеристики методов представлены в пункте [Характеристики идентификации](../performance_parameters.md#характеристики-идентификации)). В зависимости от качества распознавания и времени работы методы можно разделить на 3 группы в привязке к вариантам использования:

* **9.30** – для real-time идентификации по видео на мобильных платформах (arm)
* **6.7** – для real-time идентификации по видео на настольных платформах (x86)
* **9.300, 9.1000** – для экспертных систем поиска лица по фото

Для идентификации лиц необходимо создать объект `Recognizer`, вызвав метод `FacerecService.createRecognizer` с указанием файла конфигурации.  

Для создания компонента [Encoder](../components.md#encoder) для одного потока необходимо указать параметры `processing_threads_count=true`, `matching_threads_count=false`.  
Для создания компонента [MatcherDB](../components.md#matcherdb) для одного потока необходимо указать параметры `processing_threads_count=false`, `matching_threads_count=true`.  

На данный момент существуют следующие файлы конфигурации и методы распознавания:

* `method6_recognizer.xml` – метод 6
* `method6v2_recognizer.xml` – метод 6.2
* `method6v3_recognizer.xml` – метод 6.3
* `method6v4_recognizer.xml` – метод 6.4
* `method6v5_recognizer.xml` – метод 6.5
* `method6v6_recognizer.xml` – метод 6.6
* `method6v7_recognizer.xml` – метод 6.7
* `method7_recognizer.xml` – метод 7
* `method7v2_recognizer.xml` – метод 7.2
* `method7v3_recognizer.xml` – метод 7.3
* `method7v6_recognizer.xml` – метод 7.6
* `method7v7_recognizer.xml` – метод 7.7
* `method8v6_recognizer.xml` – метод 8.6
* `method8v7_recognizer.xml` – метод 8.7
* `method9v30_recognizer.xml` – метод 9.30
* `method9v300_recognizer.xml` – метод 9.300
* `method9v1000_recognizer.xml` – метод 9.1000

При помощи объекта `Recognizer` вы можете:

* получить имя метода (`Recognizer.getMethodName`)
* создать шаблон лица (`Recognizer.processing`) – вы получите объект `Template`
* сравнить два шаблона (`Recognizer.verifyMatch`) – сравнивать можно только шаблоны, созданные этим же методом (т.е. с таким же конфигурационным файлом)
* загрузить (десериализовать) шаблон (`Recognizer.loadTemplate`) – загружать можно только шаблоны, созданные этим же методом (т.е. с таким же конфигурационным файлом)
* создать индекс `TemplatesIndex` для быстрого поиска в больших базах (`Recognizer.createIndex`)
* выполнить поиск шаблонов в индексе (`Recognizer.search`)

При помощи объекта `Template` вы можете:

* получить имя метода (`Template.getMethodName`)
* сохранить (сериализовать) шаблон (`Template.save`)

При помощи объекта `TemplatesIndex` вы можете:

* получить имя метода (`TemplatesIndex.getMethodName`)
* получить количество шаблонов в индексе (`TemplatesIndex.size`)
* получить шаблон из индекса по номеру (`TemplatesIndex.at`)
* зарезервировать память для поиска (`TemplatesIndex.reserveSearchMemory`)

_**Примечание:** Методы `Recognizer.createIndex` и `TemplatesIndex` не копируют данные шаблона, они копируют только умные указатели на данные._
