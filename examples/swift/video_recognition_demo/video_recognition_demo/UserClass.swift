import Foundation
import UIKit


final public class UserClass: CallbackHandler{
    let vc: ViewController
    var kof_x = CGFloat(1.067)
    var kof_y = CGFloat(0.8)
    
    var one_person_id: Int = -2
    let queue = DispatchQueue.global(qos: .userInteractive)
    
    
    var drawings: [CAShapeLayer] = []
    var person_image: [UIImageView] = []
    var text_drawings: [CATextLayer] = []
    
    var personID: [Int: Int] = [:]
    
    
    var search_results: [SearchResult] = []
    var perons_ID: [Int] = []
    
    override init(userdata: UnsafeRawPointer){
        self.vc = bridgeTransfer(ptr: userdata)
        
        super.init(userdata: userdata)
    }
    
    func clearDrawings() {
        self.drawings.forEach({ drawing in drawing.removeFromSuperlayer() })
        self.text_drawings.forEach({ drawing in drawing.removeFromSuperlayer() })
    }
    
    private func drawText(text: String, point: CGPoint, radius: CGFloat, flag: BooleanLiteralType, sdwig: Int, angle: CGFloat){
        let fontSize = 7  + 2 * radius * 0.03
        let x = CGFloat(radius * cos(angle * CGFloat.pi / 180)) + point.x
        let y = CGFloat(radius * sin(angle * CGFloat.pi / 180)) + CGFloat(sdwig) * (fontSize + 5) + point.y
        
        
        let label = CATextLayer()
        label.frame = CGRect(x: x, y: y, width: point.x + radius, height: fontSize + 5)
        label.font = CFBridgingRetain(UIFont(name: "Helvetica Neue", size: fontSize))
        label.fontSize = fontSize
        label.backgroundColor = UIColor.clear.cgColor
        
        label.foregroundColor = flag ? UIColor.green.cgColor : UIColor.red.cgColor

        label.string = text
        
        self.vc.view.layer.addSublayer(label)
        self.text_drawings.append(label)
    }
    
    private func drawOval(point:CGPoint, radius: CGFloat, flag: BooleanLiteralType) {
            
        let path = UIBezierPath(arcCenter: point, radius: radius, startAngle: 0, endAngle: .pi * 2, clockwise: false)
            
        let shapeLayer = CAShapeLayer()
        shapeLayer.path = path.cgPath
        
        shapeLayer.strokeColor = flag ? UIColor.green.cgColor : UIColor.red.cgColor
        
        shapeLayer.fillColor = UIColor.clear.cgColor
        shapeLayer.lineWidth = 1
        
        self.vc.view.layer.addSublayer(shapeLayer)
        self.drawings.append(shapeLayer)
    }
    
    func resizeImage(image: UIImage, targetSize: CGSize) -> UIImage {
        let size = image.size

        let widthRatio  = targetSize.width  / size.width
        let heightRatio = targetSize.height / size.height

        var newSize: CGSize
        if(widthRatio > heightRatio) {
            newSize = CGSize(width: size.width * heightRatio, height: size.height * heightRatio)
        } else {
            newSize = CGSize(width: size.width * widthRatio, height: size.height * widthRatio)
        }

        let rect = CGRect(x: 0, y: 0, width: newSize.width, height: newSize.height)

        UIGraphicsBeginImageContextWithOptions(newSize, false, 1.0)
        image.draw(in: rect)
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()

        return newImage!
    }
    
    public override func trakingCallback(data: TrackingCallbackData) {
        self.queue.async{
            DispatchQueue.main.async {
                
                self.clearDrawings()
                self.kof_x = CGFloat(self.vc.view.frame.width) / CGFloat(480)
                self.kof_y = CGFloat(self.vc.view.frame.height) / CGFloat(640)
                
                let samples = data.samples as! [RawSample]
                
                if samples.count > 0{
                    var all_id: [Int] = []
                    let age_gender = data.samples_track_age_gender as! [AgeGender]
                    let emotions = data.samples_track_emotions
                    let liveness = data.samples_active_liveness_status as! [ActiveLivenessStatus]
                    for j in 0 ... samples.count - 1{
                        var color_flag = false
                        var texts: [String] = []
                        var person_name: String = ""
                        let id = Int(samples[j].getID())
                        all_id.append(id)
                        
                        if samples.count == 1 && self.personID[id] == nil{
                            self.vc.SaveButton[0].isHidden = false
                            self.vc.textFieldName.isHidden = false
                            if self.vc.editText{
                                self.vc.labelText.text = "Possible to add a person to the database"
                            }
                        }else{
                            self.vc.SaveButton[0].isHidden = true
                            self.vc.textFieldName.isHidden = true
                            if self.vc.editText{
                                self.vc.labelText.text = self.personID[id] == nil ? "Need one person in the frame" : "The person has already been identified"
                            }
                        }
                        
                        if (self.personID[id] != nil && self.vc.isCutImage){
                            let name = self.vc.database.data_image[self.personID[id]!]
                            let img = UIImage(contentsOfFile: self.vc.database.database_image_dir + name)!

                            let view_im = UIImageView(image: self.resizeImage(image: img, targetSize: CGSize(width: self.vc.cutImage.frame.width, height: self.vc.cutImage.frame.height)))
                            self.vc.isCutImage = false
                            self.one_person_id = id
                            
                            self.person_image.forEach({ drawing in drawing.layer.removeFromSuperlayer() })
                            self.person_image.removeAll()
                            
                            self.person_image.append(view_im)
                            
                            self.vc.cutImage.addSubview(view_im)
                            self.vc.cutImage.isHidden = false
                        }
                        if self.personID[id] != nil{
                            let name = self.vc.database.data_image[self.personID[id]!]
                            person_name = name.components(separatedBy: ".")[0]
                            color_flag = true
                        }
                                                
                        if self.vc.isAgeGender{
                            texts.append(self.getGender(gender: age_gender[j].gender))
                            texts.append("Year: " + String(format: "%.0f", age_gender[j].age_year))
                        
                        }
                        if self.vc.isEmotions{
                            let emotion = emotions[j] as! [EmotionsConfidence]
                            if emotion.count > 0{
                                texts.append(self.getEmotion(emotion: emotion[0].emotion))
                            }
                        }

                        let lanmarks = samples[j].getLandmarks()

                        let cirkle = self.getCirkle(lanmarks: lanmarks as! [Point2])
                        
                        let queue = DispatchQueue.global(qos: .userInteractive)
                        queue.async{
                            DispatchQueue.main.async {
                                
                                self.drawOval(point: cirkle.center, radius: cirkle.radius, flag: color_flag)
                                
                                if person_name != ""{
                                    self.drawText(text: person_name, point: cirkle.center, radius: cirkle.radius, flag: color_flag, sdwig: -1, angle: -95)
                                }
                                var i = 0
                                for text in texts{
                                    self.drawText(text: text, point: cirkle.center, radius: cirkle.radius, flag: color_flag, sdwig: i, angle: 225)
                                    i += 1
                                }
                            }
                        }
                    }
                    if all_id.firstIndex(of: self.one_person_id) == nil{
                        self.vc.isCutImage = true
                        self.vc.cutImage.isHidden = true
                    }
                }else{
                    if !self.vc.isCutImage{
                        self.vc.isCutImage = true
                        self.vc.cutImage.isHidden = true
                    }
                    self.vc.SaveButton[0].isHidden = true
                    self.vc.textFieldName.isHidden = true
                    self.vc.labelText.text = "Need one person in the frame"
                }
            }
        }
    }
    
    public override func matchFoundCallback(data: MatchFoundCallbackData) {
        let id = Int(data.sample.getID())
        
        self.search_results = data.search_results as! [SearchResult]
        self.personID[id] = Int(search_results[0].person_id)
        
        self.vc.frame_id_match_found = Int(data.frame_id)
    }
    private func getEmotion(emotion: Emotion) -> String {
        switch emotion {
        case .EMOTION_ANGRY:
            return "Angry"
        case .EMOTION_HAPPY:
            return "Happy"
        case .EMOTION_NEUTRAL:
            return "Neutral"
        default:
            return "Surprise"
        }
    }

    private func getGender(gender: Gender) -> String{
        switch gender {
        case .GENDER_FEMALE:
            return "Female"
        default:
            return "Male"
        }
    }

    private func getCirkle(lanmarks: [Point2]) -> (center: CGPoint, radius: CGFloat){
        let count_points = CGFloat(lanmarks.count)
        
        var points:[CGPoint] = []
        var center =  CGPoint(x: CGFloat(0), y: CGFloat(0))
        
        for i in 0 ... lanmarks.count - 1{
            let point = lanmarks[i]
            
            let p = CGPoint(x: CGFloat(point.x) * self.kof_x, y: CGFloat(point.y) * self.kof_y)
            center = CGPoint(x: center.x + p.x, y: center.y + p.y)
            points.append(p)
        }
        center = CGPoint(x: center.x / count_points, y: center.y / count_points)
        
        var radius = CGFloat(0)
        for point in points{
            let x = point.x - center.x
            let y = point.y - center.y
            
            radius += sqrt(x * x + y * y)
        }
        
        radius /= count_points
        radius *= 2.1
        
        return (center, radius)
    }
}
