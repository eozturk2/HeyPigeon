//
//  ViewController.swift
//  Drone Drama
//
//  Created by Dylan Rochon-Terry on 2021-01-30.
//

import UIKit
import MapKit
import CoreLocation



class ViewController: UIViewController {

    @IBOutlet weak var userTextField: UITextField!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var signInButton: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        userTextField.layer.cornerRadius = 15
        passwordTextField.layer.cornerRadius = 15
        signInButton.layer.cornerRadius = 15

    }
    
    func initializeData(){
        
    }


}
