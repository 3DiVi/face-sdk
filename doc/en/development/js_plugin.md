# Face SDK VideoEngine JS plugin

The **Face SDK VideoEngine JS** plugin implements functionality of [face detection and tracking](/doc/en/development/face_capturing.md) and active liveness estimation. This section provides information on installing, initializing, and running the **Face SDK VideoEngine JS** plugin, available startup options, and public methods.

## Installing the plugin

Run one of the following commands in the console, depending on the platform you are using (Node or Yarn):
* `npm install @3divi/face-sdk-video-engine-js`
* `yarn add @3divi/face-sdk-video-engine-js`

## Initializing the plugin

1. Import the `VideoEngine` library from the Face SDK:  
`import { VideoEngine } from '@3divi/face-sdk-video-engine-js';`
2. Create an instance of the `VideoEngine` class (see the description of optional startup parameters in the section [Startup options of the plugin](js_plugin.md#startup-options-of-the-plugin)):  
`const videoEngine = new VideoEngine(?options);`

## Running the plugin

1. Call the `load` method in order to load the module: `videoEngine.load();`
2. To run the plugin, call the method `videoEngine.start(input, callback);`, where:
* `input` — an HTML element `HTMLVideoElement` that can be obtained from the `<video></video>` tag and passed as input to the method. The video stream must be specified in the tag (see the section [Initializing the camera](js_plugin.md#initializing-the-camera)).
* `callback` — a callback function that can be used to get the prediction object (see the description of this object in the section [Public methods](js_plugin.md#public-methods)) and process data in real time during the video stream processing (in this mode, a video stream from the camera is being processed: a face is detected and Liveness is estimated), for example, to display a bounding box, get data, and display it on the screen.

_**Note**: loading the module is an asynchronous process._

Example of plugin initialization:
```
const initVideoEngine = async () => {
 try {
  // Do something
  const videoEngine = new VideoEngine();
  await videoEngine.load();
  // Do something
 } catch (error) {
   console.log(error.message);
 }
};
```

### Initializing the camera

Follow the steps below to initialize the camera:
1. Add the `<video></video>` tag to HTML.
2. Get a video tag:  
`const video = documnet.querySelector('video');`
3. Initialize the stream. See the full example: [demo/src/utils.js](/examples/javascript/demo/src/utils.js).
```
try {
  const stream = await navigator.mediaDevices.getUserMedia({
   audio: false,
   video: {
    facingMode: 'user',
    width: VIDEO_SIZE,
    height: VIDEO_SIZE,
   },
  });
  video.srcObject = stream;
  return new Promise((resolve) => {
   video.onloadedmetadata = () => {
    resolve(video);
   };
  });
 } catch (error) {
  throw new Error(error);
}
```

### Startup options of the plugin

The `new VideoEngine()` accepts the options object that contains the following fields:
* `backend` — hardware used for processing. Available values: `webgl` — GPU, `cpu` — CPU. Default value: `webgl`.
* `pose` — an object that stores the rotation of the head in degrees. Used to check Active Liveness and limit the maximum allowed head rotation (see [Camera Positioning and Shooting](/doc/en/guidelines_for_cameras.md#camera-positioning-and-shooting)). Has the `maxRotation` field. Type: *Number* (degree of rotation). Default value: `15`.
* `eyes` — an object that stores the information about the position and state of the eyes. Accepts the following fields:
    * `minDistance` — minimum distance between pupils in pixels. Type: *Number*. Default value: `60`. You can also import the following constants:
        * `REGISTRATION_EYES_MIN_DISTANCE` stores the value of `60`. A person is added to the database if this minimum value is achieved.
        * `IDENTIFICATION_EYES_MIN_DISTANCE` stores the value of `40`. A person can be identified if this minimum value is achieved.
    * `closeLowThreshold` — the minimum threshold value for the state "eyes opened" (from 0 to 1). If the value is less than `closeLowThreshold`, then the result is the "eyes closed" status. Type: *Number* (from 0 to 1). Default value: `0.25`.
    * `closeHighThreshold` — the maximum threshold value for the state "eyes open". If the eyes are closed, the value of `closeHighThreshold` is used. If the value is >= `closeHighThreshold`, the result is the status "eyes opened", otherwise — "eyes closed". Type: *Number* (from 0 to 1). Default value: `0.3`. If the `closeHighThreshold` value is not specified, it's calculated automatically based on `closeLowThreshold`.
    * `maxDurationClose` — the value used to define the "blink" status. If the period between the "eyes opened" and "eyes closed" states is less than `maxDurationClose`, then the result is the status "there was a blink". Type: *Number* (time in ms). Default value: `500`.

## Public methods

* `load` — asynchronous library initialization. Can accept the `backend` field (see [Startup options of the plugin](js_plugin.md#startup-options-of-the-plugin)). Returns the `Promise` object. After the callback is completed, the `Status` object is returned:
```
{
	type: 'success',
	message: 'ok',
};
```
* `reset` — removes the information about the last processing.
* `start` — starts the video stream processing. Returns the `Promise` object that contains the `Status` object (see above). When the `start` method is executed for the first time, the `backend` object and model are initialized. Accepts the following parameters:
    * `input` — an HTML element `HTMLVideoElement` (see [Running the plugin](js_plugin.md#running-the-plugin)).
    * `callback` — a callback function that can be used to process the received data in real time. The argument of this function is the prediction object.
        * `prediction`:
            * `pose` — an object that stores the information about the current position of the head:
                * `axes` — an object of the *roll/pitch/yaw* coordinate system. Type of fields: *Number*.
                * `poseInRequiredRange` — a variable that means that a face position is in the range of the coordinate system mentioned above (angles do not exceed the `maxRotation` value). Type: *Boolean*.
            * `headWasTurned` — whether the head was turned during processing. Type: *Boolean*.
            * `imageBase64` — an image of a current prediction object. Type: *String*. Data format: *Base64*.
            * `face` — an object that stores the information about the eyes and points of a face:
                * `boundingBox` — a bounding box:
                    * `topLeft` — an array of 2D coordinates [x:number, y:number]; (upper left corner).
                    * `bottomRight` — an array of 2D coordinates [x:number, y:number]; (lower right corner).
                * `faceInViewConfidence` — likelihood of a face being present in the frame. Type: *Number* (from 0 to 1).
                * `mesh` — an array with nested arrays of 3D coordinates of face points [...[x:number, y:number, z:number]].
                * `scaledMesh` — a normalized array (the coordinates of the points are adjusted in accordance with the size of a face in the video stream) with nested arrays of 3D coordinates of face points.
                * `annotations` — semantic groupings of the `scaledMesh` coordinates.
                * `eyes` — an object that stores information about the eyes:
                    * `isClosed` — whether the eyes are closed at the current time. Type: *Boolean*.
                    * `isBlinked` — whether there was a blink at the current time. Type: *Boolean*.
                    * `wasBlinked` — whether there was a blink during the entire processing period. Type: *Boolean*.
                    * `eyesDistanceInRequiredRange` — whether the user is at the correct distance from the camera. The calculation takes into account the value of the `minDistance` parameter (the minimum distance between the pupils). Type: *Boolean*.
                    * `levelOpeningEyes` — an object that stores the current degree of opening of each eye:
                        * `left` — the left eye. Type: *Number*.
                        * `right` — the right eye. Type: *Number*.
* Since the first initialization after calling the `start` method can take a long time, you can supplement the code so that a message is displayed during the initialization process (for example, “Initialization in progress”).
* `stop` — stops the processing but saves the current values. If the start method is called, processing will continue. Returns the best prediction object.

An example of an interface returned in the `prediction` object:

```
interface OutputData {
  pose: {
    axes: Axes;
    poseInRequiredRange: boolean;
  };
  face: {
    boundingBox: BoundingBox;
    mesh: Coord3D[];
    scaledMesh: Coord3D[];
    annotations?: { [key: string]: Coord3D };
    faceInViewConfidence: number;
    eyes: {
        isClosed: boolean;
        isBlinked: boolean;
        wasBlinked: boolean;
        eyesDistanceInRequiredRange: boolean;
        levelOpeningEyes: {
          left: number;
          right: number;
        };
  };
};
  headWasTurned: boolean;
  imageBase64?: string;
}
```

## Public fields

* `backend` — hardware used for processing (see [Startup options of the plugin](js_plugin.md#startup-options-of-the-plugin)). Returns data of the *String* type.
* `inProgress` — whether processing was started. Returns a Boolean value.
* `isInitialized` — whether the model was initialized. The model is initialized during the first execution of the `start` method. Returns a Boolean value.
* `isLoaded` — whether the model was loaded. Returns a Boolean value.
* `bestPrediction` — returns the best `prediction` object.
* `bestShot` — returns the best image of the best `prediction` object.