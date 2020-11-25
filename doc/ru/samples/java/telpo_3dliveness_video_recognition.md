# Telpo Liveness Video Recognition 

## Общая информация

Данный проект демонстрирует идентицикацию лиц и определение принадлежности лица живому человеку (Liveness) на терминалах **Telpo Face Recognition TPS980** с 3D- и RGB+IR камерами.

## Запуск предсобранного приложения 

Установите и запустите предсобранное приложение `apk/telpo_3dliveness_video_recognition.apk` для тестирования. Приложение содержит в себе лицензию, которая привязана к [Application ID](/doc/ru/licenses.md#привязка-к-application-id) и ОС Android.

## Сборка демонстрационного приложения из исходного кода 

1. [Запросите триальную лицензию](https://face.3divi.com/products/face_sdk/face_sdk_trial).
2. Поместите триальную лицензию `3divi_face_sdk.lic` в папку  `examples/android/telpo_3dliveness_video_recognition/src/main/assets/license`.
3. Откройте папку `telpo-android` в Android Studio в качестве существующего проекта.  
4. Соберите проект и установите полученное приложение на терминал Telpo.

## Идентификация лиц в масках

Мы предоставляем новый алгоритм идентификации лиц, оптимизированный для детекции лиц в масках и обеспечивающий высокое качество идентификации лиц в масках. Для того, чтобы использовать алгоритм идентификации лиц в масках, выполните следующие шаги: 
1. Откройте файл `AuthAcrivity.java`. 
2. Замените строку `private final String recognizer_config = "method9v30_recognizer.xml";` на `private final String recognizer_config = "method9v30mask_recognizer.xml";`. 
