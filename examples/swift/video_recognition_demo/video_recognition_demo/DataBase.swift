import Foundation
import UIKit


public class DataBase{
    var vm_elements: [DatabaseElement] = []

    var data_image: [String] = []
    
    var k = UInt64(0)
    
    let database_dir: String
    let database_image_dir: String
    
    let recognizer: Recognizer
    let capturer: Capturer
    let distance: Float
  
    init(recognizer: Recognizer, capturer: Capturer, distance_threshold: CGFloat) {
        self.recognizer = recognizer
        self.capturer = capturer
        self.distance = Float(distance_threshold)
        
        let fileManager = FileManager.default
        
        let dir : NSString = (NSSearchPathForDirectoriesInDomains(FileManager.SearchPathDirectory.documentDirectory, FileManager.SearchPathDomainMask.userDomainMask, true).first as NSString?)!
        
        self.database_dir = "\(dir)/Database/"
        self.database_image_dir = "\(dir)/DatabaseImage/"
        
        if !fileManager.fileExists(atPath: self.database_dir){
            do {
                try FileManager.default.createDirectory(atPath: self.database_dir, withIntermediateDirectories: false, attributes: nil)
                
                try FileManager.default.createDirectory(atPath: self.database_image_dir, withIntermediateDirectories: false, attributes: nil)
                
                let path = Bundle.main.resourcePath! + "/base/"
                self.loadDatabaseUI(path: path)
                
            } catch let error as NSError {
                print(error.localizedDescription)
            }
        }else{
            self.loadDatabaseNS(path: self.database_dir)
        }
    }
    
    private func loadDatabaseUI(path: String){
        let enumerator: FileManager.DirectoryEnumerator = FileManager.default.enumerator(atPath: "\(path)")!
        
        while let element = enumerator.nextObject() as? String{
            let image = UIImage(contentsOfFile: "\(path)\(element)")!
            
            let data = self.deleteAlfa(image: image)
    
            if (data.count >= Int(image.size.width * image.size.height * 3)){
                let raw_image = RawImage(Int32(image.size.width), Int32(image.size.height), Format.FORMAT_BGR.rawValue, data)
                let samples = self.capturer.capture(raw_image) as! [RawSample]
                if samples.count > 0 {
                    self.saveElement(sample: samples[0], image: image, file_name: element)
                }
            }
        }
    }
    private func loadDatabaseNS(path: String){
        let enumerator: FileManager.DirectoryEnumerator = FileManager.default.enumerator(atPath: "\(path)")!
        
        while let element = enumerator.nextObject() as? String{
            let temp: Template = self.recognizer.loadTemplate("/private\(path)\(element)")
            let baseElement = DatabaseElement(self.k, self.k, temp, self.distance)
            self.data_image.append(element)
            self.k += 1
            self.vm_elements.append(baseElement)
        }
    }
    public func saveElement(sample: RawSample, image: UIImage, file_name: String){
        let temp: Template = self.recognizer.processing(sample)
        let baseElement = DatabaseElement(self.k, self.k, temp, self.distance)
        self.vm_elements.append(baseElement)
        self.data_image.append(file_name)
        self.k += 1
        temp.save("/private" + self.database_dir + file_name)
        self.saveUIImage(sample: sample, image: image, file_name: file_name)
    }
    private func saveUIImage(sample: RawSample, image: UIImage, file_name: String){
        let points = sample.getFaceCutRectangle(FaceCutType.FACE_CUT_FULL_FRONTAL) as! [Point2]
        let cgIm = image.cgImage!.cropping(to: CGRect(x: CGFloat(points[0].x), y: CGFloat(points[0].y), width: CGFloat(points[2].x - points[0].x), height: CGFloat(points[1].y - points[0].y)))!
        
        let img2 = UIImage(cgImage: cgIm)
        let data: NSData = img2.jpegData(compressionQuality: 1.0)! as NSData
        data.write(toFile: self.database_image_dir + file_name, atomically: true)
    }
    
    private func deleteAlfa(image: UIImage) -> [UInt8]{
        let providerData = (image.cgImage!.dataProvider)!.data
        let data = CFDataGetBytePtr(providerData)!
        var result: [UInt8] = []
        
        var j = 0;
        var i = 0;
        while (i < Int(image.size.width * image.size.height * 3)){
            if (j % 4 != 0){
                result.append(data[j])
                i += 1
            }
            j += 1
        }
        
        return result
    }
}
