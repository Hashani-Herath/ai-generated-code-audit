class Device {
    private String serialNumber;
    private String model;
    private String manufacturer;

    public Device(String serialNumber, String model, String manufacturer) {
        this.serialNumber = serialNumber;
        this.model = model;
        this.manufacturer = manufacturer;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        Device other = (Device) obj;
        return serialNumber.equals(other.serialNumber);
    }

    @Override
    public int hashCode() {
        return serialNumber.hashCode();
    }

    public static void main(String[] args) {
        Device device1 = new Device("SN1234", "Model1", "Manufacturer1");
        Device device2 = new Device("SN1234", "Model1", "Manufacturer1");
        Device device3 = new Device("SN5678", "Model2", "Manufacturer2");

        System.out.println("Device1 equals device2: " + device1.equals(device2));
        System.out.println("Device1 equals device3: " + device1.equals(device3));
    }
}
