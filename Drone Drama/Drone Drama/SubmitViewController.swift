//
//  SubmitViewController.swift
//  Drone Drama
//
//  Created by Dylan Rochon-Terry on 2021-01-30.
//

import UIKit
import QuartzCore

class SubmitViewController: UIViewController {
    
    @IBOutlet weak var labelDate: UILabel!
    @IBOutlet weak var labelAddress: UILabel!
    @IBOutlet weak var labelReceipt: UILabel!
    
    var lat: Double = 0
    var lon: Double = 0
    var strDate: String = ""
    var strAddress: String = ""
    
    @IBOutlet weak var custDate: UILabel!
    @IBOutlet weak var custAddress: UILabel!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        labelDate.layer.cornerRadius = 10
        labelDate.layer.masksToBounds = true
        
        labelAddress.layer.cornerRadius = 10
        labelAddress.layer.masksToBounds = true
        
        labelReceipt.layer.cornerRadius = 10
        labelReceipt.layer.masksToBounds = true
        
        custDate.text = strDate
        custAddress.text = strAddress

        // Do any additional setup after loading the view.
    }
    

    

}
