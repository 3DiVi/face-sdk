# Face SDK Licensing

We offer the following licenses:
* By activation type: 
  * [Online](#online-licenses) – requires Internet connection for activation and periodic validation, portable between devices
  * [Offline](#offline-licenses) – bound to hardware or USB token, non-portable between devices
* By locking type:
  * [Application ID](#application-id) – bound to your iOS or Android app
  * Hardware ID – bound to hardware signature
  * [USB token](#usb-token) – a device that is used instead of a standard license file

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

* Unpack the archive with the distribution in any directory on your device (see [Getting Started](../README.md#getting-started)).
* Run the *get_license* utility from the *bin* folder on a target PC.
* Send the generated `3divi_face_sdk.lic` file to the software provider (3DiVi Company): send an email to your personal manager or contact us using a feedback form at the [3DiVi website](https://face.3divi.com/contact_us).
* The software provider will send you a signed license file.
* Put the license file to the directory *license* and rename the file to `3divi_face_sdk.lic` if needed.

## License Locking

### Application ID

You can bind the Face SDK license to a mobile app developed by you. In this case the users of your app won't need to buy or activate the license. To bind the license to your app, you have to specify your application id (`android_app_id` for Android or `ios_app_id` for iOS). You should get the app id **only after your app is signed for release**, otherwise, the app id will change and the license will no longer be valid.

To get the app id, call the function `FacerecService.getLicenseState` in an app and print the `android_app_id` or `ios_app_id` field of the result in a logcat. Purchase the license file for your application, specifying the app id. Use this license file in the application. Your app will now work on any Android/iOS device (depending on your target platform). One license cannot be used for several applications (they have different app ids, so one license should be bound to one app).

### USB token

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
