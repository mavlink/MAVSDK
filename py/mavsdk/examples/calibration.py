import time
from mavsdk import Mavsdk, Configuration, ComponentType
from mavsdk.plugins.calibration import Calibration, CalibrationResult

# Global variables for the callback state
calibration_completed = False

def calibration_callback(result, progress_data, user_data=None):
    """Callback for calibration progress and results"""
    global calibration_completed
    
    if result == CalibrationResult.SUCCESS:
        print("--- Calibration succeeded!")
        calibration_completed = True
        
    elif result == CalibrationResult.NEXT:
        # Progress update
        if progress_data.has_progress:
            print(f"    Progress: {int(progress_data.progress * 100)}%")
        if progress_data.has_status_text:
            status_text = progress_data.status_text
            print(f"    Instruction: {status_text}")
            
    elif result == CalibrationResult.FAILED:
        print("--- Calibration failed!")
        calibration_completed = True
        
    elif result == CalibrationResult.NO_SYSTEM:
        print("--- Error: No system connected")
        calibration_completed = True
        
    elif result == CalibrationResult.CONNECTION_ERROR:
        print("--- Error: Connection error")
        calibration_completed = True
        
    elif result == CalibrationResult.BUSY:
        print("--- Error: Vehicle is busy")
        calibration_completed = True
        
    elif result == CalibrationResult.COMMAND_DENIED:
        print("--- Error: Command denied")
        calibration_completed = True
        
    elif result == CalibrationResult.TIMEOUT:
        print("--- Error: Command timed out")
        calibration_completed = True
        
    elif result == CalibrationResult.CANCELLED:
        print("--- Calibration cancelled")
        calibration_completed = True
        
    elif result == CalibrationResult.FAILED_ARMED:
        print("--- Error: Cannot calibrate while armed")
        calibration_completed = True
        
    elif result == CalibrationResult.UNSUPPORTED:
        print("--- Error: Calibration not supported")
        calibration_completed = True
        
    else:
        print("--- Unknown calibration result")
        calibration_completed = True

def calibrate_accelerometer(calibration):
    """Calibrate accelerometer"""
    global calibration_completed
    
    print("Calibrating accelerometer...")
    calibration_completed = False
    
    calibration.calibrate_accelerometer_async(calibration_callback)

    # Wait for calibration to complete
    while not calibration_completed:
        time.sleep(1)

def calibrate_gyro(calibration):
    """Calibrate gyro"""
    global calibration_completed
    
    print("Calibrating gyro...")
    calibration_completed = False
    
    calibration.calibrate_gyro_async(calibration_callback)
    
    # Wait for calibration to complete
    while not calibration_completed:
        time.sleep(1)

def calibrate_magnetometer(calibration):
    """Calibrate magnetometer"""
    global calibration_completed
    
    print("Calibrating magnetometer...")
    calibration_completed = False
    
    calibration.calibrate_magnetometer_async(calibration_callback)
    
    # Wait for calibration to complete
    while not calibration_completed:
        time.sleep(1)

def main():
    # Create MAVSDK instance
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    
    # Add connection
    mavsdk.add_any_connection("udpin://0.0.0.0:14540")
    
    # Wait for autopilot
    print("Waiting for autopilot...")
    
    system = None
    while system is None:
        systems = mavsdk.get_systems()
        for s in systems:
            if s.has_autopilot() and s.is_connected():
                system = s
                break
        if system is None:
            time.sleep(1)
    
    print("Found autopilot system!")
    
    # Create calibration plugin
    calibration = Calibration(system)
    
    # Run calibrations
    calibrate_accelerometer(calibration)
    calibrate_gyro(calibration) 
    calibrate_magnetometer(calibration)
    
    print("All calibrations completed!")
    

if __name__ == "__main__":
    main()
