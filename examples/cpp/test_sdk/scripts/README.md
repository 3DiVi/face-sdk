# Utils
Use python3 for python scripts

## viz_roc.py
python3 viz_roc.py [-s image_file] FILES

**OPTIONS:**

* [-s image_file] - save result in image_file without visualization
* FILES - one or more files with ROC

Run example with visualization:
```bash
python3 viz_roc.py roc.txt
```
Run example with save image and without visualization:
```bash
python3 viz_roc.py -s roc_image.jpg roc.txt
```

## runtime_test_sdk
Set parameters in script

Run example for linux:
```bash
./runtime_test_sdk.sh
```

Run example for windows:
```bash
runtime_test_sdk.bat
```