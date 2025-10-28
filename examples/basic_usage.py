from pymavsdk import Mavsdk

def main():
    mavsdk = Mavsdk()
    mavsdk.add_any_connection("udpin://0.0.0.0:14540")

if __name__ == "__main__":
    main()
