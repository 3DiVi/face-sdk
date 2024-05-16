import 'package:flutter/material.dart';


class BndBox extends StatelessWidget {
  final List<dynamic> results;
  final double previewOffsetX;
  final double previewOffsetY;
  final double screenH = 0;
  final double screenW = 0;

  BndBox(this.results, this.previewOffsetX, this.previewOffsetY);

  @override
  Widget build(BuildContext context) {
    List<Widget> _renderBoxes() {
      return results.map((re) {
        var _x = re["rect"]["x"];
        var _w = re["rect"]["w"];
        var _y = re["rect"]["y"];
        var _h = re["rect"]["h"];
        var widgetH = re["widget"]["h"];
        var widgetW = re["widget"]["w"];
        var picH = re["picture"]["h"];
        var picW = re["picture"]["w"];
        var x, y, w, h;

        x = _x / picW * widgetW;
        y = _y / picH * widgetH;
        w = _w / picW * widgetW;
        h = _h / picH * widgetH;

        return Positioned(
          left: x,
          top: y,
          width: w.toDouble(),
          height: h.toDouble(),
          child: Container(
            padding: EdgeInsets.only(top: 5.0, left: 5.0),
            decoration: BoxDecoration(
              border: Border.all(
                color: Color.fromRGBO(37, 213, 253, 1.0),
                width: 3.0,
              ),
            ),
          ),
        );
      }).toList();
    }

    return Stack(children: _renderBoxes());
  }
}
