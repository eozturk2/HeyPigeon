//
//  BookingViewController.swift
//  Drone Drama
//
//  Created by Dylan Rochon-Terry on 2021-01-30.
//

import UIKit
import CoreLocation

class BookingViewController: UIViewController, UITextFieldDelegate{

    @IBOutlet weak var phoneNum: UITextField!
    @IBOutlet weak var address: UITextField!
    @IBOutlet weak var myDate: UIDatePicker!
    
    var stationName: String = ""
    var lat: Double = 0
    var lon: Double = 0
    var strDate: String = ""
    var strAddress: String = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setUpDate(myDate: myDate)
        removeDrone(stationName: stationName)
    }
    
    func removeDrone(stationName: String){
        
        for station in Montreal.stationsList{
            if station.name == stationName{
                let num: Int = station.dronesAvailable
                station.dronesAvailable = num-1
            }
        }
        
    }
    func setUpDate(myDate: UIDatePicker){
        myDate.minimumDate = Date()
        
        let calendar = Calendar(identifier: .gregorian)
        let currentDate = Date()
        var components = DateComponents()
        components.calendar = calendar
        components.day = 10
        let maxDate = calendar.date(byAdding: components, to: currentDate)!
        myDate.maximumDate = maxDate
        
    }
    @IBAction func dateChange(_ sender: UIDatePicker) {
        let timeFormatter = DateFormatter()
            timeFormatter.timeStyle = DateFormatter.Style.short
        timeFormatter.dateStyle = DateFormatter.Style.short
        

        strDate = timeFormatter.string(from: sender.date)
        
    }

    @IBAction func done(_ sender: UITextField) {
        sender.resignFirstResponder()
    }

    @IBAction func doneAlso(_ sender: UITextField) {
        sender.resignFirstResponder()
    }
    
//    @IBAction func completed(_ sender: Any) {
    @IBAction func completed(_ sender: Any) {
            
        self.phoneNum.delegate = self
        self.address.delegate = self
        strAddress = address.text!
        
//        let yourAddress: String = address.text!
//        let geoCoder = CLGeocoder()
//
//        geoCoder.geocodeAddressString(yourAddress) { (placemarks, error) in
//            guard
//                let placemarks = placemarks,
//                let location = placemarks.first?.location
//            else {
//                // handle no location found
//                return
//            }// Use your location
//
//        }
        
        performSegue(withIdentifier: "SegueToComplete", sender: self)

            
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {

        // Creating a variable for our destination VC.
        let destVC: SubmitViewController = segue.destination as! SubmitViewController
        // We can directly access that VC as a class, the variable stationInfo is an attribute.
        destVC.lat = self.lat
        destVC.lon = self.lon
        destVC.strDate = self.strDate
        destVC.strAddress = self.strAddress

  }
}


