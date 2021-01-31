//
//  StationInfoViewController.swift
//  Drone Drama
//
//  Created by Dylan Rochon-Terry on 2021-01-30.
//

import UIKit

class StationInfoViewController: UIViewController {

    @IBOutlet weak var label: UILabel!
    @IBOutlet weak var dronesAvail: UILabel!
    
    var stationInfo: String = ""
    var stations = Montreal.stationsList
    var avail: String = ""

    override func viewDidLoad() {
        super.viewDidLoad()
        
        for station in stations{
            if station.name == stationInfo{
                label.text = station.name
                avail = String(station.dronesAvailable)
                break
            }
        }
        
        label.text = stationInfo
        dronesAvail.text = avail
        

    

        // Do any additional setup after loading the view.
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {

        // Creating a variable for our destination VC.
        let destVC: BookingViewController = segue.destination as! BookingViewController
        // We can directly access that VC as a class, the variable stationInfo is an attribute.
        destVC.stationName = self.stationInfo
        

  }

}
