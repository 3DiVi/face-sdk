# Face SDK Licensing

We offer the following licenses:
* By [activation type](#license-activation): 
  * [Online](#online-licenses) – requires Internet connection for activation and periodic validation, portable between devices
  * [Offline](#offline-licenses) – bound to hardware or USB token, non-portable between devices
* By [locking type](#license-locking):
  * [Application ID](#locking-to-an-application-id) – bound to your iOS or Android app
  * [Hardware ID](#locking-to-a-hardware-id) – bound to hardware signature
  * [USB token](#locking-to-a-usb-token) – a device that is used instead of a standard license file

Contact us at face@3divi.com if you have any questions.

## License Activation

### Online Licenses

* The *license* directory is a default online license working directory. You can prepare another working directory – all applications that use 3DiVi Face SDK should have read/write access to it. Copy the `log_params.xml` and `net_params.xml` files in that directory from the *license* directory.
* Get the license file from the online license server.
* Copy the license file in the online license working directory and rename the file to `3divi_face_sdk.lic`.
* When you call `FacerecService.createService`, provide the path to the online license working directory in the `license_dir` parameter (or keep it empty if the default online license directory is used).

While any application(s) using 3DiVi Face SDK with an online license are running, the license file will be regularly updated (in the background). The license update includes the license renewal and, if necessary, the hardware signature change.

The network settings used for license updates are stored in the `net_params.xml` file:

* `timeout` – timeout in seconds
* `use_system_proxy` – `yes` or `no`, set `yes` to use system proxy settings
* `no_proxy_for_server` – `yes` or `no`, set `yes` to not use any proxy
* `proxy` – set `url:port` to use that proxy if both parameters `use_system_proxy` and `no_proxy_for_server` are set to `no`

### Offline Licenses

To activate the license on the Face SDK Licensing Portal, please follow the steps below:

* Unpack the archive with the distribution package in any directory on your device (see [Getting Started](/README.md#getting-started)).
* Generate/get the `3divi_face_sdk.lic` file that will contain the information about your hardware required for licensing: 
    * [Windows/Linux] Run the *get_license* utility from the *bin* folder on a target PC/device. As a result, the `3divi_face_sdk.lic` file will be created in the *bin* folder. 
	* [Android] Call the `FacerecService.getLicenseState` function and get the `LicenseState.hardware_reg` field (see an example of getting a *Hardware ID* in [Android Demo](/examples/android/demo/src/main/java/com/vdt/face_recognition/demo/MainActivity.java)). Send your *Hardware ID* to your personal manager. Your manager will send you the `3divi_face_sdk.lic` file.
* Log in to [the Face SDK Licensing Portal](https://cognitive.3divi.com/app/face/login/). If you don't remember your password, you can reset it [here](https://cognitive.3divi.com/app/face/forgot-password/).
* Click *Face SDK* in the menu on the left.
* Select the license file generated in the licensing portal by 3DiVi (see the image below). 

<p align="center">
<img width="700" src="/doc/img/off_lic_fsdk.png"><br>
<b>Selecting the license file from the list of created licenses</b><br>
</p>

* Click on the license name. A new window with detailed information about the license file will appear. 
* Click the *Upload file* button.

<p align="center">
<img width="600" src="/doc/img/off_lic_file.png"><br>
<b>Uploading the license file to the Face SDK Licensing Portal</b><br>
</p>

* After that, you'll see the window "Activate FaceSDK license". Click the *Upload* button to upload the `3divi_face_sdk.lic` file that was created in the *bin* folder.

<p align="center">
<img width="300" src="/doc/img/off_lic_upload.png"><br>
<b>Activating the license file</b><br>
</p>

* Click the *Generate* button to generate the final license file locked to your hardware. 
* Copy the license file to the *license* directory and rename it to `3divi_face_sdk.lic` if needed.

## License Locking

Depending on the locking type, some licenses allow you to use Face SDK only on specific devices. Currently, the following locking types of the Face SDK licenses are available:

* *No locking* – the Face SDK license can be used on any device. 
* *Locking to a platform* – the license can be used only on devices with a specific platform (Windows/Linux/Android/iOS).
 * *Locking to an application ID* – the license can be used only as a part of a specific application (available on Android and iOS).
 * *Locking to a hardware ID* – the license can be used only on a specific device (available on Windows, Linux and Android).
 * *Locking to a USB token* – the license can be used only on a device with a plugged-in USB token (available on Windows and Linux). 
 
### Locking to a Hardware ID

Locking to a *Hardware ID* allows you to use the *Face SDK* only on a specific device. The license is locked to a device during activation. Online licenses are automatically locked when the library is launched for the first time. If you're using an offline license, you have to get a *Hardware ID* on a target device before activation. See how to lock your offline license to a Hardware ID in [Offline Lisenses](#offline-licenses).

You can transfer the online licenses locked to a *Hardware ID* to other devices, however, the license can only be used on one device at a time. To transfer the license, download the license file `3divi_face_sdk.lic` from your [personal Face SDK account](https://cognitive.3divi.com/) once again and move it to the *license* folder on the new device. **Do not copy the license file from the previous device** because in this case an error will occur during re-activation of the license.

### Locking to an Application ID

You can bind the Face SDK license to a mobile app developed by you. In this case the users of your app won't need to buy or activate the license. To bind the license to your app, you have to specify your application id (`android_app_id` for Android or `ios_app_id` for iOS). You should get the app id **only after your app is signed for release**, otherwise, the app id will change and the license will no longer be valid.

To get the app id, call the function `FacerecService.getLicenseState` in an app and print the `android_app_id` or `ios_app_id` field of the result in a logcat. Purchase the license file for your application, specifying the app id. Use this license file in the application. Your app will now work on any Android/iOS device (depending on your target platform). One license cannot be used for several applications (they have different app ids, so one license should be bound to one app).

### Locking to a USB token

A USB token is a device that can be used instead of a standard license file. Just like a license file, a USB token stores the information about purchased Face SDK components. However, a USB token is not linked to hardware signature. All you need to do is to plug your USB token into your device. A USB token is copy-protected. One USB token can be used by one process, therefore, you have to purchase one more USB token if you want to run one more process.

To license Face SDK with a USB token instead of a license file, select one of the options below:

* specify `0x436e9514-hardware-key-serial-<key>` in the `license_dir` parameter of the function `FacerecService.createService`
* in `3divi_face_sdk.lic`, replace the standard content of the license with the line `hardware_key_serial <key>`

where `key` is a USB token serial number. Example: `senselock1234560000000f7e`

If a USB token with the specified serial number isn't found, you'll see the list of all available USB tokens with their serial numbers in the text of an exception thrown by `FacerecService.createService`.

Support for USB tokens was added in the following distribution packages:

* Linux x86_32
* Linux x86_64
* Windows x86_32
* Windows x86_64
