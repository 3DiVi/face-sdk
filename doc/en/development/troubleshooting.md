# Troubleshooting  

The table below shows error codes and error messages that may occur when using Face SDK, as well as possible solutions:

| Error code | Error description | Possible solution |
|------------|-------------------|-------------------|
| 0x2fd1cbdd | License file not found. | Make sure that the license file is in the license directory. |
| 0x9a7b099f | The processing license is not valid for this method. | Make sure that you created a `Recognizer` object with the `processing` flag and there is a free `processor<method_name>` component in your license file. |
| 0xc2fb31ca | The matching license is not valid for this method. | Make sure that you created a `Recognizer` object with the `matching` flag and there is a free `matcher<method_name>` component in your license file. |
| 0x752187cb | Valid video frame timestamp is required for Depth Liveness estimation. | Make sure that you are passing valid timestamps to the `addVideoFrame` method. |
| 0x23d47b00 | Valid video frame timestamp is required for IR Liveness estimation. | Make sure that you are passing valid timestamps to the `addVideoFrame` method. |
| 0x6b4b4115 | Error while loading template: template labels do not match. | Make sure that the current `Recognizer` method is the same as the one used when saving the template. |
| 0x54faacfe | GPU device is not available but the `use_cuda` parameter is turned on. | Make sure that the required CUDA version is installed or disable GPU usage by setting the `use_cuda` parameter to `0` in the configuration file. |
| 0xb1274980 | The `gpu_index` is negative. | Set a non-negative number for the `gpu_index` parameter in the configuration file. |
| 0x78121b36 | This Face SDK build does not support running the model on GPU. | Disable the `with_cuda` parameter in the configuration file or use the Face SDK build with GPU support. |
| 0x0616033c, 0xbe61f5ed | Error while reading file | Make sure that you downloaded the distribution correctly: try downloading again and/or run `git lfs pull` if you are cloning the repository from GitHub. |