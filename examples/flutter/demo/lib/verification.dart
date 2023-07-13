import 'package:flutter/material.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';


void logError(String code, String message) {
  if (message != null) {
    print('Error: $code\nError Message: $message');
  } else {
    print('Error: $code');
  }
}

typedef void setVerifyScore(MatchResult res);


class VerifyFaces extends StatefulWidget {
  final FacerecService _facerecService;
  final setVerifyScore callback;
  final String nextRoute;
  final Image img1;
  final Image img2;
  final Template templ1;
  final Template templ2;

  VerifyFaces(this._facerecService, this.img1, this.img2, this.templ1, this.templ2, this.nextRoute, this.callback);

  @override
  _VerifyFacesState createState() => new _VerifyFacesState();
}



class _VerifyFacesState extends State<VerifyFaces> {
  bool isDetecting = false;
  late Recognizer _recognizer;
  MatchResult? res;


  @override
  void initState() {
    super.initState();
    _recognizer = widget._facerecService.createRecognizer("method10v30_recognizer.xml");
    setState(() {
      res = _recognizer.verifyMatch(widget.templ1, widget.templ2);
      widget.callback(res!);
    });
  }

  Widget compareScore(context){
    return Column(
        children:[
          Expanded(
              child:ListView(
                children: <Widget>[
                  Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: <Widget>[
                        Expanded(child:  widget.img1),
                        Expanded(child:  widget.img2),
                      ]
                  ),
                  new Container(
                    height: 45.0,
                    margin: const EdgeInsets.fromLTRB(10.0, 8.0, 10.0, 0.0),
                    child: Text(
                      res!.score > 0.8 ?
                        "Faces are the same.\nMatch score: ${res!.score}":
                        "Faces are the same.\nMatch score: ${res!.score}",
                      textAlign: TextAlign.center,
                      overflow: TextOverflow.ellipsis,
                      style: TextStyle(fontSize: 20, decoration: TextDecoration.none, color: Colors.amber),
                    ),
                  ),
                ]
            )
          ),
          FloatingActionButton(
              heroTag: "btn4",
              child: const Icon(Icons.navigate_next),
              // color: Colors.blue,
              onPressed: (){
                Navigator.of(context).pushNamed(widget.nextRoute);
              }
          ),
        ]
    );
  }

  @override
  Widget build(BuildContext context) {
    return res == null ? CircularProgressIndicator(): compareScore(context);
  }

  @override
  void dispose() {
    _recognizer.dispose();
    super.dispose();
  }
}
