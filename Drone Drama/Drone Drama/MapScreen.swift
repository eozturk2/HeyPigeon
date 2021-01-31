//
//  MapScreen.swift
//  Drone Drama
//
//  Created by Dylan Rochon-Terry on 2021-01-30.
//

import UIKit
import MapKit
import CoreLocation

struct Drone {
    var id: Int
    var battery: Int
}

class Station {
    
    var ID: Int
    var name: String
    var lattitude: CLLocationDegrees
    var longtitude: CLLocationDegrees
    var dronesAvailable: Int
    var dronesList: [Drone]
    
    init(ID: Int, name: String, lattitude: CLLocationDegrees, longtitude: CLLocationDegrees, dronesAvailable: Int) {
        self.ID = ID
        self.name = name
        self.lattitude = lattitude
        self.longtitude = longtitude
        self.dronesAvailable = dronesAvailable
        self.dronesList = []
    }

        

}

struct Montreal {
    static var stationsList: [Station] = []
}

class MapScreen: UIViewController, MKMapViewDelegate {

    @IBOutlet weak var mapView: MKMapView!
    
    let locationManager = CLLocationManager()
    let regionInMeters: Double = 5000
    var chosenStation: String = ""
    var stations = [Station]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        checkLocationServices()
        // Do any additional setup after loading the view.
        initializeData()
        stations = Montreal.stationsList
        pushStationsToMap(stations)
    }
    
    func initializeData(){
        
        Montreal.stationsList = [

            Station(ID: 1, name: "McCord Museum", lattitude: 45.504396844079515,  longtitude: -73.5740062904157, dronesAvailable: 18),
            Station(ID: 2, name: "The Neuro", lattitude: 45.5089280377117, longtitude: -73.58158782584583, dronesAvailable: 4),
            Station(ID: 3, name: "La Fontaine", lattitude: 45.52516930459586, longtitude: -73.57508992006896, dronesAvailable: 7),
            Station(ID: 4, name: "Old Port", lattitude: 45.50689870742285, longtitude: -73.55529133694237, dronesAvailable: 3),
            Station(ID: 5, name: "Petro", lattitude: 45.51697090421779, longtitude: -73.58969685886136, dronesAvailable: 1),
            Station(ID: 6, name: "Sir Wilfred", lattitude: 45.5307111007364, longtitude: -73.58411028804598, dronesAvailable: 4),
            Station(ID: 7, name: "University of Montreal", lattitude: 45.506285788073946, longtitude: -73.61528596466813, dronesAvailable: 6),
            Station(ID: 8, name: "Westmount", lattitude: 45.484662960810134, longtitude: -73.60087164622007, dronesAvailable: 5),
            Station(ID: 9, name: "Cote Des Neiges", lattitude: 45.476313424328474, longtitude: -73.61362464515311, dronesAvailable: 9),
            Station(ID: 10, name: "Verdun Rona", lattitude: 45.46303880899491, longtitude: -73.5682472535278, dronesAvailable: 10),
            Station(ID: 11, name: "Jewish Hospital", lattitude: 45.49677312378055, longtitude: -73.63195774491716, dronesAvailable: 13),
        ]
        
        var j: Int = 0
        for station in Montreal.stationsList {
            var i: Int = 0
            while i < station.dronesAvailable{
                let randomInt = Int.random(in: 20..<100)
                station.dronesList.append(Drone(id: j, battery: randomInt))
                i+=1
                j+=1
            }
        }
    }
    
    // Device wide settings...
    func checkLocationServices() {
        if CLLocationManager.locationServicesEnabled(){
            setUpLocationManager()
            checkLocationAuthorization()
        } else {
            //Show alert telling them location services aren't enabled.
        }
    }
    
    func pushStationsToMap(_ stations: [Station]) {
        
        for station in stations {
            let annotation = MKPointAnnotation()
            annotation.title = station.name
            annotation.subtitle = "Drones Available: " + String (station.dronesAvailable)
            annotation.coordinate = CLLocationCoordinate2D(latitude: station.lattitude, longitude: station.longtitude)
            mapView.addAnnotation(annotation)
        }
    }
    
    // This here lets us create the 'i' icon for more info, where we will then lead to new view. 
    func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
        
        if annotation is MKUserLocation {
            return nil
        }
        
        let reuseId = "pin"
        
        var pinView = mapView.dequeueReusableAnnotationView(withIdentifier: reuseId)
        
        if pinView == nil {
            pinView = MKPinAnnotationView(annotation: annotation, reuseIdentifier: reuseId)
            pinView!.canShowCallout = true
            pinView!.rightCalloutAccessoryView = UIButton(type: .detailDisclosure) as UIButton
        } else {
            pinView!.annotation = annotation
        }
        
        return pinView
    }
    
    func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, calloutAccessoryControlTapped control: UIControl) {
        
        if control == view.rightCalloutAccessoryView {
            chosenStation = view.annotation?.title!! ?? "FAILURE"
            performSegue(withIdentifier: "SegueToInfo", sender: self)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        
        // Creating a variable for our destination VC.
        let destVC: StationInfoViewController = segue.destination as! StationInfoViewController
        // We can directly access that VC as a class, the variable stationInfo is an attribute.
        destVC.stationInfo = chosenStation
        destVC.stations = stations
        
    }
    
    func setUpLocationManager() {
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        
    }
    
    func centerViewOnUSerLocation() {
        if let location = locationManager.location?.coordinate {
            
            let region = MKCoordinateRegion.init(center: location, latitudinalMeters: regionInMeters, longitudinalMeters: regionInMeters)
            mapView.setRegion(region, animated: true)
        }
    }
    
    func checkLocationAuthorization() {
            
        switch CLLocationManager.authorizationStatus() {
        case .authorizedWhenInUse:
            //Do Map stuff
            mapView.showsUserLocation = true
            centerViewOnUSerLocation()
            
            //Maybe we don't need this functionality.
            //locationManager.startUpdatingLocation()
            
            break
        case .denied:
            // Show alert, tell them what to do.
            break
        case .notDetermined:
            locationManager.requestWhenInUseAuthorization()
        case .restricted:
            // Show alert, tell em what to do.
            break
        case .authorizedAlways:
            break
        @unknown default:
            break
        }
            
            
        
    }
    

}

extension MapScreen: CLLocationManagerDelegate {
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        // We come back soon
        guard let location = locations.last else { return }
        let center = CLLocationCoordinate2D(latitude: location.coordinate.latitude, longitude: location.coordinate.longitude)
        let region = MKCoordinateRegion.init(center: center, latitudinalMeters: regionInMeters, longitudinalMeters: regionInMeters)
        mapView.setRegion(region, animated: true)
    }
    
    func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        // We come back soon
    }
}
