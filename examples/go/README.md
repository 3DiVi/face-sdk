# Face SDK Go demos
There are two Go demos for Face SDK. At first, execute this command in termnal to copy FaceSDK Go API and setup Go workspace

```bash
cd SDK_dir/examples/go
./init_workspace.sh
```
Now, you can run both _processing_block_ or _microservice_ sample by executing **run.sh** script from coresponding sample's directory. This script is just 
* add some environmental valiables we need for execution
* add libfacerec.so to **LD_LIBRARY_PATH** variable  
* pass to the demos default path to Face SDK 
and run demo.

## go_processing_block_demo
processing_block is console application that shows basic Processing Block functional.
To run processing_block demo, execute this command:
```bash
./run.sh -input_image <path to image> -unit_type <some_unit_type>
```
Arguments:
* **-input_image** is path to image you want to proceed
* **-unit_type** is Processing Block you're interesting in. Avaiable unit types are face, fitter, emotions, age, gender, mask, glasses, eye_openness, liveness|quality
* **-modification** and **-version"** are additional arguments that you can use to explicitly specify Processing Block modification and version. 

## go_microservice_demo
microservice is example of simple verification(1:1) and identification(1:n) server app.
To run server, execute following command:
```bash
run.sh -images_path <path to directory with images>
```
* **-images_path** is path to folder with images that you want to us as face database during identification

### go_microservice_demo requests
In this section we briefly describe microservice entrypoints you can test.
* GET http://127.0.0.1:8080 to see current state of service  
```json
{
    "templateIndexCapacity": 1000,
    "templateIndexSize": 1000
}
```
* POST http://127.0.0.1:8080/matching to check if 2 images match (are of the same person)
```python
import base64
import requests # pip install requests


def read_image(image_path: str):
    with open(image_path, "rb") as file:
        return file.read()


image_data = base64.b64encode(
    read_image(
        "<image path>")).decode(
    "utf-8")
data = {
    "image": image_data
}

print(requests.post("http://127.0.0.1:8080/matching", json=data).content)
```

```json
{
    "results": [
        {
            "distance": 0, 
            "far": 0,
            "frr": 0.9825583696365356, 
            "index": 311,
            "score": 1,
            "uuid": "32ac624e-eff2-459a-bd17-4b62767d9c72.jpeg"
        }
    ]
}
```
* POST http://127.0.0.1:8080/verification to check if person from request image in microservice database or no
```python
import base64
import requests # pip install requests


def read_image(image_path: str):
    with open(image_path, "rb") as file:
        return file.read()


first_image_data = base64.b64encode(
    read_image(
        "<first image path>")).decode(
    "utf-8")
second_image_data = base64.b64encode(
    read_image(
        "<second image path>")).decode(
    "utf-8")
data = {
    "image1": first_image_data,
    "image2": second_image_data
}

print(requests.post("http://127.0.0.1:8080/verification", json=data).content)
```

```json
{
    {
        "distance": 9495,
        "far": 0.09273551404476166,
        "frr": 0.002809464931488037,
        "score": 0.11070597171783447
    }
}
```
